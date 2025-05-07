/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "task.h"

#include "helper/error_helper.h"
#include "helper/napi_helper.h"
#include "helper/object_helper.h"
#include "helper/hitrace_helper.h"
#include "task_manager.h"
#include "taskpool.h"
#include "tools/log.h"
#include "worker.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
static constexpr char ONRECEIVEDATA_STR[] = "onReceiveData";
static constexpr char SETTRANSFERLIST_STR[] = "setTransferList";
static constexpr char SET_CLONE_LIST_STR[] = "setCloneList";
static constexpr char ONENQUEUED_STR[] = "onEnqueued";
static constexpr char ONSTARTEXECUTION_STR[] = "onStartExecution";
static constexpr char ONEXECUTIONFAILED_STR[] = "onExecutionFailed";
static constexpr char ONEXECUTIONSUCCEEDED_STR[] = "onExecutionSucceeded";
static constexpr char ISDONE_STR[] = "isDone";

using namespace Commonlibrary::Concurrent::Common::Helper;

Task::Task(napi_env env, TaskType taskType, std::string name) : env_(env), taskType_(taskType), name_(name) {}

napi_value Task::TaskConstructor(napi_env env, napi_callback_info cbinfo)
{
    // check argv count
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    std::string errMessage = "";
    if (argc < 1) {
        errMessage = "taskpool:: create task need more than one param";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, errMessage.c_str());
        return nullptr;
    }
    napi_value* args = new napi_value[argc];
    ObjectScope<napi_value> scope(args, true);
    napi_value thisVar = nullptr;
    napi_value func = nullptr;
    napi_value name = nullptr;
    napi_get_cb_info(env, cbinfo, &argc, args, &thisVar, nullptr);
    // if the first is task name, the second might be func
    if (argc > 1 && NapiHelper::IsString(env, args[0])) {
        name = args[0];
        func = args[1];
        args += 2; // 2: name and func
        argc -= 2; // 2: name and func
    } else {
        func = args[0];
        args += 1; // 1: func
        argc -= 1; // 1: func
    }
    if (!NapiHelper::IsFunction(env, func)) {
        errMessage = "taskpool:: the first or second param of task must be function";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
            "the type of the first or second param of task must be function.");
        return nullptr;
    }

    Task* task = GenerateTask(env, thisVar, func, name, args, argc);
    napi_status status = napi_wrap(env, thisVar, task, TaskDestructor, nullptr, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR("taskpool::TaskConstructor napi_wrap return value is %{public}d", status);
        delete task;
        task = nullptr;
        return nullptr;
    }
    TaskManager::GetInstance().StoreTask(task->taskId_, task);
    napi_create_reference(env, thisVar, 0, &task->taskRef_);
    if (!task->IsMainThreadTask()) {
        napi_add_env_cleanup_hook(env, Task::CleanupHookFunc, task);
    }
    return thisVar;
}

napi_value Task::LongTaskConstructor(napi_env env, napi_callback_info cbinfo)
{
    auto thisVar = TaskConstructor(env, cbinfo);
    if (thisVar == nullptr) {
        return nullptr;
    }
    Task* task;
    napi_unwrap(env, thisVar, reinterpret_cast<void**>(&task));
    task->isLongTask_ = true;
    return thisVar;
}

void Task::TaskDestructor(napi_env env, void* data, [[maybe_unused]] void* hint)
{
    Task* task = static_cast<Task*>(data);
    HILOG_INFO("taskpool:: taskId:%{public}s TaskDestructor", std::to_string(task->taskId_).c_str());
    if (!task->IsMainThreadTask()) {
        napi_remove_env_cleanup_hook(env, Task::CleanupHookFunc, task);
    }
    // for performance, do not lock first
    if (task->IsMainThreadTask() || task->refCount_ == 0) {
        TaskManager::GetInstance().ReleaseTaskData(env, task);
        napi_delete_reference(env, task->taskRef_);
        delete task;
        return;
    }
    bool shouldDelete = false;
    {
        std::lock_guard<RECURSIVE_MUTEX> lock(task->taskMutex_);
        task->SetValid(false);
        if (task->refCount_ == 0) {
            shouldDelete = true;
        }
        TaskManager::GetInstance().ReleaseTaskData(env, task, shouldDelete);
        napi_delete_reference(env, task->taskRef_);
    }
    if (shouldDelete) {
        delete task;
    }
}

void Task::CleanupHookFunc(void* arg)
{
    if (arg == nullptr) {
        HILOG_ERROR("taskpool:: cleanupHook arg is nullptr");
        return;
    }
    Task* task = static_cast<Task*>(arg);
    if (task->IsSeqRunnerTask()) {
        SequenceRunnerManager::GetInstance().RemoveWaitingTask(task);
    }
    std::lock_guard<RECURSIVE_MUTEX> lock(task->taskMutex_);
    if (task->onResultSignal_ != nullptr) {
        uv_close(reinterpret_cast<uv_handle_t*>(task->onResultSignal_), nullptr);
    }
    if (task->onStartExecutionSignal_ != nullptr) {
        uv_close(reinterpret_cast<uv_handle_t*>(task->onStartExecutionSignal_), nullptr);
    }
    if (task->IsFunctionTask()) {
        task->SetValid(false);
    }
}

Task* Task::GenerateTask(napi_env env, napi_value napiTask, napi_value func,
                         napi_value name, napi_value* args, size_t argc)
{
    HILOG_DEBUG("taskpool:: task GenerateTask");
    napi_value argsArray = NapiHelper::CreateArrayWithLength(env, argc);
    for (size_t i = 0; i < argc; i++) {
        napi_set_element(env, argsArray, i, args[i]);
    }
    if (name == nullptr) {
        name = NapiHelper::GetNameProperty(env, func, NAME);
    }
    char* nameStr = NapiHelper::GetChars(env, name);
    Task* task = new Task(env, TaskType::TASK, nameStr);
    delete[] nameStr;
    task->taskId_ = reinterpret_cast<uint64_t>(task);
    task->InitHandle(env);

    napi_value taskId = NapiHelper::CreateUint64(env, task->taskId_);
    napi_value napiTrue = NapiHelper::CreateBooleanValue(env, true);
    napi_value napiFalse = NapiHelper::CreateBooleanValue(env, false);
    napi_set_named_property(env, napiTask, FUNCTION_STR, func);
    napi_set_named_property(env, napiTask, TASKID_STR, taskId);
    napi_set_named_property(env, napiTask, ARGUMENTS_STR, argsArray);
    napi_set_named_property(env, napiTask, DEFAULT_TRANSFER_STR, napiTrue);
    napi_set_named_property(env, napiTask, DEFAULT_CLONE_SENDABLE_STR, napiFalse);
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION(SETTRANSFERLIST_STR, SetTransferList),
        DECLARE_NAPI_FUNCTION(SET_CLONE_LIST_STR, SetCloneList),
        DECLARE_NAPI_FUNCTION(ONRECEIVEDATA_STR, OnReceiveData),
        DECLARE_NAPI_FUNCTION(ADD_DEPENDENCY_STR, AddDependency),
        DECLARE_NAPI_FUNCTION(REMOVE_DEPENDENCY_STR, RemoveDependency),
        DECLARE_NAPI_FUNCTION(ONENQUEUED_STR, OnEnqueued),
        DECLARE_NAPI_FUNCTION(ONSTARTEXECUTION_STR, OnStartExecution),
        DECLARE_NAPI_FUNCTION(ONEXECUTIONFAILED_STR, OnExecutionFailed),
        DECLARE_NAPI_FUNCTION(ONEXECUTIONSUCCEEDED_STR, OnExecutionSucceeded),
        DECLARE_NAPI_FUNCTION(ISDONE_STR, IsDone),
        DECLARE_NAPI_GETTER(TASK_TOTAL_TIME, GetTotalDuration),
        DECLARE_NAPI_GETTER(TASK_CPU_TIME, GetCPUDuration),
        DECLARE_NAPI_GETTER(TASK_IO_TIME, GetIODuration),
        DECLARE_NAPI_GETTER(NAME, GetName)
    };
    napi_define_properties(env, napiTask, sizeof(properties) / sizeof(properties[0]), properties);
    return task;
}

Task* Task::GenerateFunctionTask(napi_env env, napi_value func, napi_value* args, size_t argc, TaskType type)
{
    HILOG_DEBUG("taskpool:: task GenerateFunctionTask");
    napi_value argsArray;
    napi_create_array_with_length(env, argc, &argsArray);
    for (size_t i = 0; i < argc; i++) {
        napi_set_element(env, argsArray, i, args[i]);
    }
    napi_value undefined = NapiHelper::GetUndefinedValue(env);
    TaskInfo* taskInfo = GenerateTaskInfo(env, func, argsArray, undefined, undefined, Priority::DEFAULT);
    if (taskInfo == nullptr) {
        HILOG_ERROR("taskpool:: task GenerateFunctionTask end, taskInfo is nullptr");
        return nullptr;
    }
    napi_value napiFuncName = NapiHelper::GetNameProperty(env, func, NAME);
    char* nameStr = NapiHelper::GetChars(env, napiFuncName);
    Task* task = new Task(env, type, nameStr);
    delete[] nameStr;
    task->taskId_ = reinterpret_cast<uint64_t>(task);
    task->currentTaskInfo_ = taskInfo;
    task->InitHandle(env);
    if (!task->IsMainThreadTask()) {
        napi_add_env_cleanup_hook(env, CleanupHookFunc, task);
    }
    return task;
}

napi_value Task::GetTaskInfoPromise(napi_env env, napi_value task, TaskType taskType, Priority priority)
{
    TaskInfo* taskInfo = GetTaskInfo(env, task, priority);
    if (taskInfo == nullptr) {
        return nullptr;
    }
    UpdateTaskType(taskType);
    return NapiHelper::CreatePromise(env, &taskInfo->deferred);
}

TaskInfo* Task::GetTaskInfo(napi_env env, napi_value task, Priority priority)
{
    napi_value func = NapiHelper::GetNameProperty(env, task, FUNCTION_STR);
    napi_value args = NapiHelper::GetNameProperty(env, task, ARGUMENTS_STR);
    napi_value taskName = NapiHelper::GetNameProperty(env, task, NAME);
    napi_value napiDefaultTransfer = NapiHelper::GetNameProperty(env, task, DEFAULT_TRANSFER_STR);
    napi_value napiDefaultClone = NapiHelper::GetNameProperty(env, task, DEFAULT_CLONE_SENDABLE_STR);
    if (func == nullptr || args == nullptr || napiDefaultTransfer == nullptr || napiDefaultClone == nullptr) {
        std::string errMessage = "taskpool:: task value is error";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, errMessage.c_str());
        return nullptr;
    }
    napi_value transferList = NapiHelper::GetUndefinedValue(env);
    if (NapiHelper::HasNameProperty(env, task, TRANSFERLIST_STR)) {
        transferList = NapiHelper::GetNameProperty(env, task, TRANSFERLIST_STR);
    }
    napi_value cloneList = NapiHelper::GetUndefinedValue(env);
    if (NapiHelper::HasNameProperty(env, task, CLONE_LIST_STR)) {
        cloneList = NapiHelper::GetNameProperty(env, task, CLONE_LIST_STR);
    }
    bool defaultTransfer = NapiHelper::GetBooleanValue(env, napiDefaultTransfer);
    bool defaultCloneSendable = NapiHelper::GetBooleanValue(env, napiDefaultClone);
    TaskInfo* pendingInfo = GenerateTaskInfo(env, func, args, transferList, cloneList, priority,
                                             defaultTransfer, defaultCloneSendable);
    if (pendingInfo == nullptr) {
        return nullptr;
    }
    {
        std::lock_guard<RECURSIVE_MUTEX> lock(taskMutex_);
        if (currentTaskInfo_ == nullptr) {
            currentTaskInfo_ = pendingInfo;
        } else {
            pendingTaskInfos_.push_back(pendingInfo);
        }
    }
    char* name = NapiHelper::GetChars(env, taskName);
    if (strlen(name) == 0) {
        napi_value funcName = NapiHelper::GetNameProperty(env, func, NAME);
        name = NapiHelper::GetChars(env, funcName);
    }
    name_ = std::string(name);
    delete[] name;
    return pendingInfo;
}

napi_value Task::SetTransferList(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = 1;
    napi_value args[1];
    napi_value thisVar;
    napi_get_cb_info(env, cbinfo, &argc, args, &thisVar, nullptr);
    // Check whether clone list has been set
    if (NapiHelper::HasNameProperty(env, thisVar, CLONE_LIST_STR)) {
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_IN_BOTH_CLONE_AND_TRANSFER);
        return nullptr;
    }
    if (argc > 1) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
            "the number of setTransferList parma must be less than 2.");
        return nullptr;
    }
    Task* task = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void**>(&task));
    if (task == nullptr) {
        HILOG_ERROR("taskpool:: task is nullptr");
        return nullptr;
    }
    napi_value undefined = NapiHelper::GetUndefinedValue(env);
    napi_value falseVal = NapiHelper::CreateBooleanValue(env, false);
    if (argc == 0) {
        HILOG_DEBUG("taskpool:: set task params not transfer");
        napi_set_named_property(env, thisVar, TRANSFERLIST_STR, undefined);
        // set task.defaultTransfer false
        napi_set_named_property(env, thisVar, DEFAULT_TRANSFER_STR, falseVal);
        return nullptr;
    }
    if (!NapiHelper::IsArray(env, args[0])) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
            "the type of setTransferList first param must be array.");
        return nullptr;
    }
    // set task.defaultTransfer false
    napi_set_named_property(env, thisVar, DEFAULT_TRANSFER_STR, falseVal);
    uint32_t arrayLength = NapiHelper::GetArrayLength(env, args[0]);
    if (arrayLength == 0) {
        HILOG_DEBUG("taskpool:: set task params not transfer");
        napi_set_named_property(env, thisVar, TRANSFERLIST_STR, undefined);
        return nullptr;
    }
    for (size_t i = 0; i < arrayLength; i++) {
        napi_value transferVal = NapiHelper::GetElement(env, args[0], i);
        if (!NapiHelper::IsArrayBuffer(env, transferVal)) {
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
                "the type of the element in array must be arraybuffer.");
            return nullptr;
        }
    }
    HILOG_DEBUG("taskpool:: check setTransferList param success");
    napi_set_named_property(env, thisVar, TRANSFERLIST_STR, args[0]);
    return nullptr;
}

napi_value Task::SetCloneList(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = 1;
    napi_value args[1];
    napi_value thisVar;
    napi_get_cb_info(env, cbinfo, &argc, args, &thisVar, nullptr);
    // Check whether transfer list has been set
    if (NapiHelper::HasNameProperty(env, thisVar, TRANSFERLIST_STR)) {
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_IN_BOTH_CLONE_AND_TRANSFER);
        return nullptr;
    }
    if (argc != 1) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
            "the number of setCloneList parma must be 1.");
        return nullptr;
    }
    if (!NapiHelper::IsArray(env, args[0])) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
            "the type of setCloneList first param must be array.");
        return nullptr;
    }
    Task* task = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void**>(&task));
    if (task == nullptr) {
        HILOG_ERROR("taskpool:: task is nullptr");
        return nullptr;
    }
    napi_value undefined = NapiHelper::GetUndefinedValue(env);
    uint32_t arrayLength = NapiHelper::GetArrayLength(env, args[0]);
    if (arrayLength == 0) {
        HILOG_DEBUG("taskpool:: clone list is empty");
        napi_set_named_property(env, thisVar, CLONE_LIST_STR, undefined);
        return nullptr;
    }
    for (size_t i = 0; i < arrayLength; i++) {
        napi_value cloneVal = NapiHelper::GetElement(env, args[0], i);
        if (NapiHelper::IsBitVector(env, cloneVal)) {
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "BitVector not support setCloneList.");
            return nullptr;
        }
        if (!NapiHelper::IsArrayBuffer(env, cloneVal) && !NapiHelper::IsSendable(env, cloneVal)) {
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
                "the type of setCloneList elements in array must be arraybuffer or sendable.");
            return nullptr;
        }
    }
    napi_set_named_property(env, thisVar, CLONE_LIST_STR, args[0]);
    return nullptr;
}

napi_value Task::IsCanceled(napi_env env, napi_callback_info cbinfo)
{
    bool isCanceled = false;
    auto engine = reinterpret_cast<NativeEngine*>(env);
    if (!engine->IsTaskPoolThread()) {
        HILOG_ERROR("taskpool:: call isCanceled not in taskpool thread");
        return NapiHelper::CreateBooleanValue(env, isCanceled);
    }
    // Get task and query task cancel state
    void* data = engine->GetCurrentTaskInfo();
    if (data == nullptr) {
        HILOG_ERROR("taskpool:: call isCanceled not in Concurrent function");
    } else {
        Task* task = static_cast<Task*>(data);
        isCanceled = task->taskState_ == ExecuteState::CANCELED ? true : false;
    }
    return NapiHelper::CreateBooleanValue(env, isCanceled);
}

napi_value Task::OnReceiveData(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    if (argc >= 2) { // 2: the number of parmas
        HILOG_ERROR("taskpool:: the number of OnReceiveData parma must be less than 2");
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
            "the number of OnReceiveData parma must be less than 2.");
        return nullptr;
    }

    napi_value thisVar;
    if (argc == 0) {
        HILOG_INFO("taskpool:: Set taskpool.Task.onReceiveData to undefined");
        napi_get_cb_info(env, cbinfo, &argc, nullptr, &thisVar, nullptr);
        napi_value id = NapiHelper::GetNameProperty(env, thisVar, "taskId");
        uint64_t taskId = NapiHelper::GetUint64Value(env, id);
        TaskManager::GetInstance().RegisterCallback(env, taskId, nullptr);
        return nullptr;
    }

    napi_value args[1];
    napi_get_cb_info(env, cbinfo, &argc, args, &thisVar, nullptr);
    napi_valuetype type;
    NAPI_CALL(env, napi_typeof(env, args[0], &type));
    if (type != napi_function) {
        HILOG_ERROR("taskpool:: OnReceiveData's parameter should be function");
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
            "the type of onReceiveData's parameter must be function.");
        return nullptr;
    }
    // store callbackInfo
    napi_value napiTaskId = NapiHelper::GetNameProperty(env, thisVar, "taskId");
    uint64_t taskId = NapiHelper::GetUint64Value(env, napiTaskId);
    auto task = TaskManager::GetInstance().GetTask(taskId);
    if (task == nullptr) {
        HILOG_ERROR("taskpool:: OnReceiveData's task is nullptr");
        return nullptr;
    }
    napi_ref callbackRef = Helper::NapiHelper::CreateReference(env, args[0], 1);
    std::shared_ptr<CallbackInfo> callbackInfo = std::make_shared<CallbackInfo>(env, 1, callbackRef, task);
#if defined(ENABLE_TASKPOOL_EVENTHANDLER)
    if (!task->IsMainThreadTask()) {
        auto loop = NapiHelper::GetLibUV(env);
        ConcurrentHelper::UvHandleInit(loop, callbackInfo->onCallbackSignal, TaskPool::ExecuteCallback);
    }
#else
    auto loop = NapiHelper::GetLibUV(env);
    ConcurrentHelper::UvHandleInit(loop, callbackInfo->onCallbackSignal, TaskPool::ExecuteCallback);
#endif
    TaskManager::GetInstance().RegisterCallback(env, taskId, callbackInfo);
    return nullptr;
}

napi_value Task::SendData(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    napi_value args[argc];
    napi_get_cb_info(env, cbinfo, &argc, args, nullptr, nullptr);

    napi_value argsArray;
    napi_create_array_with_length(env, argc, &argsArray);
    for (size_t i = 0; i < argc; i++) {
        napi_set_element(env, argsArray, i, args[i]);
    }

    auto engine = reinterpret_cast<NativeEngine*>(env);
    if (!engine->IsTaskPoolThread()) {
        HILOG_ERROR("taskpool:: SendData is not called in the taskpool thread");
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_NOT_IN_TASKPOOL_THREAD);
        return nullptr;
    }
    Task* task = nullptr;
    void* data = engine->GetCurrentTaskInfo();
    if (data == nullptr) {
        HILOG_ERROR("taskpool:: SendData is not called in the concurrent function");
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_NOT_IN_CONCURRENT_FUNCTION);
        return nullptr;
    } else {
        task = static_cast<Task*>(data);
    }

    napi_value undefined = NapiHelper::GetUndefinedValue(env);
    void* serializationArgs = nullptr;
    bool defaultClone = false;
    bool defaultTransfer = true;
    napi_status status = napi_serialize_inner(env, argsArray, undefined, undefined,
                                              defaultTransfer, defaultClone, &serializationArgs);
    if (status != napi_ok || serializationArgs == nullptr) {
        std::string errMessage = "taskpool:: failed to serialize function";
        HILOG_ERROR("%{public}s in SendData", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_WORKER_SERIALIZATION, errMessage.c_str());
        return nullptr;
    }

    TaskResultInfo* resultInfo = new TaskResultInfo(task->env_, env, task->taskId_, serializationArgs);
    return TaskManager::GetInstance().NotifyCallbackExecute(env, resultInfo, task);
}

napi_value Task::AddDependency(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    if (argc == 0) {
        std::string errMessage = "taskpool:: addDependency has no params";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "addDependency has no params.");
        return nullptr;
    }

    napi_status status = napi_ok;
    HandleScope scope(env, status);
    napi_value args[argc];
    napi_value napiTask;
    napi_get_cb_info(env, cbinfo, &argc, args, &napiTask, nullptr);
    Task* task = nullptr;
    napi_unwrap(env, napiTask, reinterpret_cast<void**>(&task));
    if (task == nullptr) {
        HILOG_ERROR("taskpool:: task is nullptr");
        return nullptr;
    }
    std::string errMessage = "";
    if (task->IsPeriodicTask()) {
        HILOG_ERROR("taskpool:: the periodic task cannot have a dependency");
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_TASK_HAVE_DEPENDENCY);
        return nullptr;
    }
    if (task->IsCommonTask() || task->IsSeqRunnerTask()) {
        errMessage = "taskpool:: seqRunnerTask or executedTask cannot addDependency";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, errMessage.c_str());
        return nullptr;
    }
    if (task->IsGroupCommonTask()) {
        errMessage = "taskpool:: groupTask cannot addDependency";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, errMessage.c_str());
        return nullptr;
    }
    task->SetHasDependency(true);
    std::set<uint64_t> idSet;
    for (size_t i = 0; i < argc; i++) {
        if (!NapiHelper::HasNameProperty(env, args[i], TASKID_STR)) {
            errMessage = "taskpool:: addDependency param is not task";
            HILOG_ERROR("%{public}s", errMessage.c_str());
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
                "the type of the addDependency param must be task.");
            return nullptr;
        } else {
            Task* dependentTask = nullptr;
            napi_unwrap(env, args[i], reinterpret_cast<void**>(&dependentTask));
            if (dependentTask == nullptr) {
                HILOG_ERROR("taskpool:: dependentTask is nullptr");
                return nullptr;
            }
            if (dependentTask->taskId_ == task->taskId_) {
                HILOG_ERROR("taskpool:: there is a circular dependency");
                ErrorHelper::ThrowError(env, ErrorHelper::ERR_CIRCULAR_DEPENDENCY);
                return nullptr;
            }
            if (dependentTask->IsPeriodicTask()) {
                HILOG_ERROR("taskpool:: the periodic task cannot have a dependency");
                ErrorHelper::ThrowError(env, ErrorHelper::ERR_TASK_HAVE_DEPENDENCY);
                return nullptr;
            }
            if (dependentTask->IsCommonTask() || dependentTask->IsSeqRunnerTask()) {
                errMessage = "taskpool:: seqRunnerTask or executedTask cannot be relied on";
                HILOG_ERROR("%{public}s", errMessage.c_str());
                ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, errMessage.c_str());
                return nullptr;
            }
            if (dependentTask->IsGroupCommonTask()) {
                errMessage = "taskpool:: groupTask cannot be relied on";
                HILOG_ERROR("%{public}s", errMessage.c_str());
                ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, errMessage.c_str());
                return nullptr;
            }
            idSet.emplace(dependentTask->taskId_);
            dependentTask->SetHasDependency(true);
        }
    }
    if (!TaskManager::GetInstance().StoreTaskDependency(task->taskId_, idSet)) {
        HILOG_ERROR("taskpool:: there is a circular dependency");
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_CIRCULAR_DEPENDENCY);
    }
    std::string strTrace = "Task::AddDependency: ";
    HITRACE_HELPER_METER_NAME(strTrace + TaskManager::GetInstance().GetTaskDependInfoToString(task->taskId_));
    return nullptr;
}

napi_value Task::RemoveDependency(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    if (argc == 0) {
        std::string errMessage = "taskpool:: removeDependency has no params";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "removeDependency has no params.");
        return nullptr;
    }
    napi_status status = napi_ok;
    HandleScope scope(env, status);
    napi_value args[argc];
    napi_value napiTask;
    napi_get_cb_info(env, cbinfo, &argc, args, &napiTask, nullptr);
    Task* task = nullptr;
    napi_unwrap(env, napiTask, reinterpret_cast<void**>(&task));
    if (task == nullptr) {
        HILOG_ERROR("taskpool:: the task is nullptr");
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "the task is nullptr");
        return nullptr;
    }
    if (!task->HasDependency()) {
        ThrowNoDependencyError(env);
        return nullptr;
    }
    if (task->IsPeriodicTask()) {
        HILOG_ERROR("taskpool:: the periodic task cannot call removeDependency");
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_TASK_HAVE_DEPENDENCY);
        return nullptr;
    }
    if (task->IsCommonTask()) {
        std::string errMessage = "taskpool:: executedTask cannot removeDependency";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_INEXISTENT_DEPENDENCY, errMessage.c_str());
        return nullptr;
    }
    for (size_t i = 0; i < argc; i++) {
        if (!NapiHelper::HasNameProperty(env, args[i], TASKID_STR)) {
            std::string errMessage = "taskpool:: removeDependency param is not task";
            HILOG_ERROR("%{public}s", errMessage.c_str());
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
                "the type of removeDependency param must be task.");
            return nullptr;
        }
        Task* dependentTask = nullptr;
        napi_unwrap(env, args[i], reinterpret_cast<void**>(&dependentTask));
        if (dependentTask == nullptr) {
            HILOG_ERROR("taskpool:: the dependent task is nullptr");
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "the dependent task is nullptr");
            return nullptr;
        }
        if (!dependentTask->HasDependency()) {
            ThrowNoDependencyError(env);
            return nullptr;
        }
        if (dependentTask->IsPeriodicTask()) {
            HILOG_ERROR("taskpool:: the periodic task cannot call removeDependency");
            ErrorHelper::ThrowError(env, ErrorHelper::ERR_TASK_HAVE_DEPENDENCY);
            return nullptr;
        }
        if (dependentTask->IsCommonTask()) {
            std::string errMessage = "taskpool:: cannot removeDependency on a dependent and executed task";
            HILOG_ERROR("%{public}s", errMessage.c_str());
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, errMessage.c_str());
            return nullptr;
        }
        if (!TaskManager::GetInstance().RemoveTaskDependency(task->taskId_, dependentTask->taskId_)) {
            HILOG_ERROR("taskpool:: the dependency does not exist");
            ErrorHelper::ThrowError(env, ErrorHelper::ERR_INEXISTENT_DEPENDENCY);
            return nullptr;
        }
        dependentTask->TryClearHasDependency();
    }
    task->TryClearHasDependency();
    std::string strTrace = "Task::RemoveDependency: ";
    HITRACE_HELPER_METER_NAME(strTrace + TaskManager::GetInstance().GetTaskDependInfoToString(task->taskId_));
    return nullptr;
}

void Task::StartExecutionCallback(const uv_async_t* req)
{
    HILOG_DEBUG("taskpool:: task StartExecutionCallback");
    auto listenerCallBackInfo = static_cast<ListenerCallBackInfo*>(req->data);
    if (listenerCallBackInfo == nullptr) { // LCOV_EXCL_BR_LINE
        HILOG_FATAL("taskpool:: StartExecutionCallBackInfo is null");
        return;
    }
    StartExecutionTask(listenerCallBackInfo);
}

void Task::StartExecutionTask(ListenerCallBackInfo* listenerCallBackInfo)
{
    auto env = listenerCallBackInfo->env_;
    napi_status status = napi_ok;
    HandleScope scope(env, status);
    if (status != napi_ok) {
        HILOG_ERROR("taskpool:: napi_open_handle_scope failed");
        return;
    }
    auto func = NapiHelper::GetReferenceValue(env, listenerCallBackInfo->callbackRef_);
    if (func == nullptr) {
        HILOG_INFO("taskpool:: StartExecutionCallback func is null");
        return;
    }

    napi_value result;
    napi_call_function(env, NapiHelper::GetGlobalObject(env), func, 0, nullptr, &result);
    if (NapiHelper::IsExceptionPending(env)) {
        napi_value exception = nullptr;
        napi_get_and_clear_last_exception(env, &exception);
        std::string funcStr = NapiHelper::GetPrintString(env, func);
        HILOG_ERROR("taskpool:: an exception has occurred napi_call_function, func is %{public}s", funcStr.c_str());
    }
}

void Task::ExecuteListenerCallback(ListenerCallBackInfo* listenerCallBackInfo)
{
    HILOG_DEBUG("taskpool:: task ExecuteListenerCallback");
    if (listenerCallBackInfo == nullptr) { // LCOV_EXCL_BR_LINE
        HILOG_FATAL("taskpool:: listenerCallBackInfo is null");
        return;
    }

    napi_env env = listenerCallBackInfo->env_;
    napi_value func = NapiHelper::GetReferenceValue(env, listenerCallBackInfo->callbackRef_);
    if (func == nullptr) {
        HILOG_INFO("taskpool:: ExecuteListenerCallback func is null");
        return;
    }

    napi_value result;
    napi_value args = listenerCallBackInfo->taskError_;
    if (args != nullptr) {
        napi_call_function(env, NapiHelper::GetGlobalObject(env), func, 1, &args, &result);
    } else {
        napi_call_function(env, NapiHelper::GetGlobalObject(env), func, 0, nullptr, &result);
    }

    if (NapiHelper::IsExceptionPending(env)) {
        napi_value exception = nullptr;
        napi_get_and_clear_last_exception(env, &exception);
        std::string funcStr = NapiHelper::GetPrintString(env, func);
        HILOG_ERROR("taskpool:: an exception has occurred napi_call_function, func is %{public}s", funcStr.c_str());
    }
}

napi_value Task::OnEnqueued(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    napi_value thisVar;
    if (argc == 0) {
        HILOG_INFO("taskpool:: the number of the params must be one");
        return nullptr;
    }

    napi_value args[1];
    napi_get_cb_info(env, cbinfo, &argc, args, &thisVar, nullptr);
    napi_valuetype type;
    NAPI_CALL(env, napi_typeof(env, args[0], &type));
    if (type != napi_function) {
        HILOG_ERROR("taskpool:: OnEnqueued's parameter should be function");
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
            "the type of OnEnqueued's parameter must be function.");
        return nullptr;
    }

    Task* task = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void**>(&task));
    if (task == nullptr) {
        HILOG_ERROR("taskpool:: task is nullptr");
        return nullptr;
    }

    if (task->taskState_ != ExecuteState::NOT_FOUND) {
        HILOG_ERROR("taskpool:: The executed task does not support the registration of listeners.");
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_REGISTRATION_OF_LISTENERS);
        return nullptr;
    }

    napi_ref callbackRef = Helper::NapiHelper::CreateReference(env, args[0], 1);
    task->onEnqueuedCallBackInfo_ = new ListenerCallBackInfo(env, callbackRef, nullptr);
    return nullptr;
}

napi_value Task::OnStartExecution(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    napi_value thisVar;
    if (argc == 0) {
        HILOG_INFO("taskpool:: the number of the params must be one");
        return nullptr;
    }

    napi_value args[1];
    napi_get_cb_info(env, cbinfo, &argc, args, &thisVar, nullptr);
    napi_valuetype type;
    NAPI_CALL(env, napi_typeof(env, args[0], &type));
    if (type != napi_function) {
        HILOG_ERROR("taskpool:: OnStartExecution's parameter should be function");
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
            "the type of OnStartExecution's parameter must be function.");
        return nullptr;
    }

    Task* task = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void**>(&task));
    if (task == nullptr) {
        HILOG_ERROR("taskpool:: task is nullptr");
        return nullptr;
    }

    if (task->taskState_ != ExecuteState::NOT_FOUND) {
        HILOG_ERROR("taskpool:: The executed task does not support the registration of listeners.");
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_REGISTRATION_OF_LISTENERS);
        return nullptr;
    }

    napi_ref callbackRef = Helper::NapiHelper::CreateReference(env, args[0], 1);
    task->onStartExecutionCallBackInfo_ = new ListenerCallBackInfo(env, callbackRef, nullptr);
#if defined(ENABLE_TASKPOOL_EVENTHANDLER)
    if (!task->IsMainThreadTask()) {
        auto loop = NapiHelper::GetLibUV(env);
        ConcurrentHelper::UvHandleInit(loop, task->onStartExecutionSignal_,
            Task::StartExecutionCallback, task->onStartExecutionCallBackInfo_);
    }
#else
    auto loop = NapiHelper::GetLibUV(env);
    ConcurrentHelper::UvHandleInit(loop, task->onStartExecutionSignal_,
        Task::StartExecutionCallback, task->onStartExecutionCallBackInfo_);
#endif

    return nullptr;
}

napi_value Task::OnExecutionFailed(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    napi_value thisVar;
    if (argc == 0) {
        HILOG_INFO("taskpool:: the number of the params must be one");
        return nullptr;
    }

    napi_value args[1];
    napi_get_cb_info(env, cbinfo, &argc, args, &thisVar, nullptr);
    napi_valuetype type;
    NAPI_CALL(env, napi_typeof(env, args[0], &type));
    if (type != napi_function) {
        HILOG_ERROR("taskpool:: OnExecutionFailed's parameter should be function");
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
            "the type of OnExecutionFailed's parameter must be function.");
        return nullptr;
    }

    Task* task = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void**>(&task));
    if (task == nullptr) {
        HILOG_ERROR("taskpool:: task is nullptr");
        return nullptr;
    }

    if (task->taskState_ != ExecuteState::NOT_FOUND) {
        HILOG_ERROR("taskpool:: The executed task does not support the registration of listeners.");
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_REGISTRATION_OF_LISTENERS);
        return nullptr;
    }

    napi_ref callbackRef = Helper::NapiHelper::CreateReference(env, args[0], 1);
    task->onExecutionFailedCallBackInfo_ = new ListenerCallBackInfo(env, callbackRef, nullptr);
    return nullptr;
}

napi_value Task::OnExecutionSucceeded(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    napi_value thisVar;
    if (argc == 0) {
        HILOG_INFO("taskpool:: the number of the params must be one");
        return nullptr;
    }

    napi_value args[1];
    napi_get_cb_info(env, cbinfo, &argc, args, &thisVar, nullptr);
    napi_valuetype type;
    NAPI_CALL(env, napi_typeof(env, args[0], &type));
    if (type != napi_function) {
        HILOG_ERROR("taskpool:: OnExecutionSucceeded's parameter should be function");
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
            "the type of OnExecutionSucceeded's parameter must be function");
        return nullptr;
    }

    Task* task = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void**>(&task));
    if (task == nullptr) {
        HILOG_ERROR("taskpool:: task is nullptr");
        return nullptr;
    }

    if (task->taskState_ != ExecuteState::NOT_FOUND) {
        HILOG_ERROR("taskpool:: The executed task does not support the registration of listeners.");
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_REGISTRATION_OF_LISTENERS);
        return nullptr;
    }

    napi_ref callbackRef = Helper::NapiHelper::CreateReference(env, args[0], 1);
    task->onExecutionSucceededCallBackInfo_ = new ListenerCallBackInfo(env, callbackRef, nullptr);
    return nullptr;
}

napi_value Task::IsDone(napi_env env, napi_callback_info cbinfo)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, cbinfo, nullptr, nullptr, &thisVar, nullptr);
    Task* task = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void**>(&task));
    if (task == nullptr) {
        HILOG_ERROR("taskpool:: task is nullptr");
        return NapiHelper::CreateBooleanValue(env, false);
    }

    if (task->taskState_ == ExecuteState::FINISHED || task->taskState_ == ExecuteState::ENDING) {
        return NapiHelper::CreateBooleanValue(env, true);
    }
    return NapiHelper::CreateBooleanValue(env, false);
}

napi_value Task::GetTaskDuration(napi_env env, napi_callback_info& cbinfo, std::string durationType)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, cbinfo, nullptr, nullptr, &thisVar, nullptr);
    napi_value napiTaskId = NapiHelper::GetNameProperty(env, thisVar, TASKID_STR);
    uint64_t taskId = NapiHelper::GetUint64Value(env, napiTaskId);
    uint64_t totalDuration = TaskManager::GetInstance().GetTaskDuration(taskId, durationType);
    return NapiHelper::CreateUint32(env, totalDuration);
}

napi_value Task::GetTotalDuration(napi_env env, napi_callback_info cbinfo)
{
    return GetTaskDuration(env, cbinfo, TASK_TOTAL_TIME);
}

napi_value Task::GetCPUDuration(napi_env env, napi_callback_info cbinfo)
{
    return GetTaskDuration(env, cbinfo, TASK_CPU_TIME);
}

napi_value Task::GetIODuration(napi_env env, napi_callback_info cbinfo)
{
    return GetTaskDuration(env, cbinfo, TASK_IO_TIME);
}

napi_value Task::GetName(napi_env env, [[maybe_unused]] napi_callback_info cbinfo)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, cbinfo, nullptr, nullptr, &thisVar, nullptr);
    napi_value napiTaskId = NapiHelper::GetNameProperty(env, thisVar, TASKID_STR);
    uint64_t taskId = NapiHelper::GetUint64Value(env, napiTaskId);
    napi_value name = nullptr;
    std::string taskName = TaskManager::GetInstance().GetTaskName(taskId);
    napi_create_string_utf8(env, taskName.c_str(), NAPI_AUTO_LENGTH, &name);
    return name;
}

void Task::UpdateTaskType(TaskType taskType)
{
    taskType_ = taskType;
    napi_reference_ref(env_, taskRef_, nullptr);
}

bool Task::IsRepeatableTask() const
{
    return IsCommonTask() || IsGroupCommonTask() || IsGroupFunctionTask();
}

bool Task::IsGroupTask() const
{
    return IsGroupCommonTask() || IsGroupFunctionTask();
}

bool Task::IsGroupCommonTask() const
{
    return taskType_ == TaskType::GROUP_COMMON_TASK;
}

bool Task::IsGroupFunctionTask() const
{
    return taskType_ == TaskType::GROUP_FUNCTION_TASK;
}

bool Task::IsCommonTask() const
{
    return taskType_ == TaskType::COMMON_TASK;
}

bool Task::IsSeqRunnerTask() const
{
    return taskType_ == TaskType::SEQRUNNER_TASK;
}

bool Task::IsFunctionTask() const
{
    return taskType_ == TaskType::FUNCTION_TASK;
}

bool Task::IsLongTask() const
{
    return isLongTask_;
}

bool Task::IsPeriodicTask() const
{
    return isPeriodicTask_;
}

bool Task::IsMainThreadTask() const
{
    return isMainThreadTask_;
}

// The uninitialized state is Task, and then taskType_ will be updated based on the task type.
bool Task::IsExecuted() const
{
    return taskType_ != TaskType::TASK;
}

TaskInfo* Task::GenerateTaskInfo(napi_env env, napi_value func, napi_value args,
                                 napi_value transferList, napi_value cloneList, Priority priority,
                                 bool defaultTransfer, bool defaultCloneSendable)
{
    HILOG_DEBUG("taskpool:: task GenerateTaskInfo");
    napi_value undefined = NapiHelper::GetUndefinedValue(env);
    void* serializationFunction = nullptr;
    napi_status status = napi_serialize_inner(env, func, undefined, undefined,
                                              defaultTransfer, defaultCloneSendable, &serializationFunction);
    std::string errMessage = "";
    if (status != napi_ok || serializationFunction == nullptr) {
        errMessage = "taskpool: failed to serialize function.";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_NOT_CONCURRENT_FUNCTION, errMessage.c_str());
        return nullptr;
    }
    void* serializationArguments = nullptr;
    status = napi_serialize_inner(env, args, transferList, cloneList,
                                  defaultTransfer, defaultCloneSendable, &serializationArguments);
    if (status != napi_ok || serializationArguments == nullptr) {
        errMessage = "taskpool: failed to serialize arguments.";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_WORKER_SERIALIZATION, errMessage.c_str());
        return nullptr;
    }

    TaskInfo* taskInfo = new TaskInfo();
    taskInfo->serializationFunction = serializationFunction;
    taskInfo->serializationArguments = serializationArguments;
    taskInfo->priority = priority;
    reinterpret_cast<NativeEngine*>(env)->IncreaseSubEnvCounter();
    return taskInfo;
}

void Task::IncreaseRefCount()
{
    taskRefCount_.fetch_add(2); // 2 : for PerformTask and TaskResultCallback
}

void Task::DecreaseRefCount()
{
    taskRefCount_.fetch_sub(1);
}

bool Task::IsReadyToHandle() const
{
    return (taskRefCount_ & 1) == 0;
}

void Task::NotifyPendingTask()
{
    HILOG_DEBUG("taskpool:: task:%{public}s NotifyPendingTask", std::to_string(taskId_).c_str());
    TaskManager::GetInstance().NotifyDependencyTaskInfo(taskId_);
    std::lock_guard<RECURSIVE_MUTEX> lock(taskMutex_);
    delete currentTaskInfo_;
    if (pendingTaskInfos_.empty()) {
        currentTaskInfo_ = nullptr;
        HILOG_DEBUG("taskpool:: task:%{public}s NotifyPendingTask end, currentTaskInfo_ nullptr",
                    std::to_string(taskId_).c_str());
        return;
    }
    currentTaskInfo_ = pendingTaskInfos_.front();
    pendingTaskInfos_.pop_front();
    taskState_ = ExecuteState::WAITING;
    TaskManager::GetInstance().EnqueueTaskId(taskId_, currentTaskInfo_->priority);
}

void Task::CancelPendingTask(napi_env env)
{
    HILOG_DEBUG("taskpool:: task:%{public}s CancelPendingTask", std::to_string(taskId_).c_str());
    if (pendingTaskInfos_.empty()) {
        HILOG_DEBUG("taskpool:: task CancelPendingTask end, pendingTaskInfos_ nullptr");
        return;
    }
    napi_value error = nullptr;
    if (!IsPeriodicTask()) {
        error = ErrorHelper::NewError(env, 0, "taskpool:: task has been canceled");
    }
    auto engine = reinterpret_cast<NativeEngine*>(env);
    for (const auto& info : pendingTaskInfos_) {
        engine->DecreaseSubEnvCounter();
        if (!IsPeriodicTask()) {
            napi_reject_deferred(env, info->deferred, error);
        }
        napi_reference_unref(env, taskRef_, nullptr);
        delete info;
    }
    pendingTaskInfos_.clear();
}

bool Task::UpdateTask(uint64_t startTime, void* worker)
{
    HILOG_DEBUG("taskpool:: task:%{public}s UpdateTask", std::to_string(taskId_).c_str());
    if (taskState_ != ExecuteState::CANCELED) { // task may have been canceled
        taskState_ = ExecuteState::RUNNING;
    }
    startTime_ = startTime;
    worker_ = worker;
    return true;
}

napi_value Task::DeserializeValue(napi_env env, napi_value* func, napi_value* args)
{
    std::lock_guard<RECURSIVE_MUTEX> lock(taskMutex_);
    if (UNLIKELY(currentTaskInfo_ == nullptr)) {
        HILOG_ERROR("taskpool:: the currentTaskInfo is nullptr, the task may have been cancelled");
        return nullptr;
    }

    napi_status status = napi_ok;
    std::string errMessage = "";
    status = napi_deserialize(env, currentTaskInfo_->serializationFunction, func);
    if (!IsGroupFunctionTask()) {
        napi_delete_serialization_data(env, currentTaskInfo_->serializationFunction);
    }
    if (status != napi_ok || func == nullptr) {
        errMessage = "taskpool:: failed to deserialize function.";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        napi_value err = ErrorHelper::NewError(env, ErrorHelper::ERR_WORKER_SERIALIZATION, errMessage.c_str());
        success_ = false;
        return err;
    }

    status = napi_deserialize(env, currentTaskInfo_->serializationArguments, args);
    if (!IsGroupFunctionTask()) {
        napi_delete_serialization_data(env, currentTaskInfo_->serializationArguments);
    }
    if (status != napi_ok || args == nullptr) {
        errMessage = "taskpool:: failed to deserialize function.";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        napi_value err = ErrorHelper::NewError(env, ErrorHelper::ERR_WORKER_SERIALIZATION, errMessage.c_str());
        success_ = false;
        return err;
    }
    return nullptr;
}

void Task::StoreTaskDuration()
{
    HILOG_DEBUG("taskpool:: task:%{public}s StoreTaskDuration", std::to_string(taskId_).c_str());
    cpuTime_ = ConcurrentHelper::GetMilliseconds();
    uint64_t cpuDuration = cpuTime_ - startTime_;
    if (ioTime_ != 0) {
        uint64_t ioDuration = ioTime_ - startTime_;
        TaskManager::GetInstance().StoreTaskDuration(taskId_, std::max(cpuDuration, ioDuration), cpuDuration);
    } else {
        TaskManager::GetInstance().StoreTaskDuration(taskId_, 0, cpuDuration);
    }
}

bool Task::CanForSequenceRunner(napi_env env)
{
    std::string errMessage = "";
    // task with dependence is not allowed
    if (HasDependency()) {
        errMessage = "seqRunner:: dependent task not allowed.";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_ADD_DEPENDENT_TASK_TO_SEQRUNNER, errMessage.c_str());
        return false;
    }
    if (IsPeriodicTask()) {
        errMessage = "taskpool:: SequenceRunner cannot execute the periodicTask";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_TASK_EXECUTE_AGAIN, errMessage.c_str());
        return false;
    }
    if (IsCommonTask() || IsSeqRunnerTask()) {
        errMessage = "taskpool:: SequenceRunner cannot execute seqRunnerTask or executedTask";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, errMessage.c_str());
        return false;
    }
    if (IsGroupCommonTask()) {
        errMessage = "taskpool:: SequenceRunner cannot execute groupTask";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, errMessage.c_str());
        return false;
    }
    return true;
}

bool Task::CanForTaskGroup(napi_env env)
{
    std::string errMessage = "";
    if (HasDependency()) {
        errMessage = "taskpool:: dependent task not allowed.";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, errMessage.c_str());
        return false;
    }
    if (IsPeriodicTask()) {
        errMessage = "taskpool:: The interface does not support the periodicTask";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_TASK_EXECUTE_AGAIN, errMessage.c_str());
        return false;
    }
    if (IsCommonTask() || IsSeqRunnerTask()) {
        errMessage = "taskpool:: taskGroup cannot add seqRunnerTask or executedTask";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, errMessage.c_str());
        return false;
    }
    if (IsGroupCommonTask()) {
        errMessage = "taskpool:: taskGroup cannot add groupTask";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, errMessage.c_str());
        return false;
    }
    if (IsLongTask()) {
        errMessage = "taskpool:: The interface does not support the long task";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, errMessage.c_str());
        return false;
    }
    taskType_ = TaskType::GROUP_COMMON_TASK;
    return true;
}

bool Task::CanExecute(napi_env env)
{
    std::string errMessage = "";
    if (IsGroupCommonTask()) {
        errMessage = "taskpool:: groupTask cannot execute outside";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, errMessage.c_str());
        return false;
    }
    if (IsSeqRunnerTask()) {
        errMessage = "taskpool:: seqRunnerTask cannot execute outside";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, errMessage.c_str());
        return false;
    }
    if (IsCommonTask() && HasDependency()) {
        errMessage = "taskpool:: executedTask with dependency cannot execute again";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, errMessage.c_str());
        return false;
    }
    if (IsExecuted() && IsLongTask()) {
        errMessage = "taskpool:: The long task can only be executed once";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, errMessage.c_str());
        return false;
    }
    if (IsPeriodicTask()) {
        errMessage = "taskpool:: the periodicTask cannot execute again";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_TASK_EXECUTE_AGAIN, errMessage.c_str());
        return false;
    }
    return true;
}

bool Task::CanExecuteDelayed(napi_env env)
{
    std::string errMessage = "";
    if (IsGroupCommonTask()) {
        errMessage = "taskpool:: groupTask cannot executeDelayed outside";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, errMessage.c_str());
        return false;
    }
    if (IsSeqRunnerTask()) {
        errMessage = "taskpool:: seqRunnerTask cannot executeDelayed outside";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, errMessage.c_str());
        return false;
    }
    if (IsCommonTask() && HasDependency()) {
        errMessage = "taskpool:: executedTask with dependency cannot executeDelayed again";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, errMessage.c_str());
        return false;
    }
    if (IsExecuted() && IsLongTask()) {
        errMessage = "taskpool:: Multiple executions of longTask are not supported in the executeDelayed";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, errMessage.c_str());
        return false;
    }
    if (IsPeriodicTask()) {
        errMessage = "taskpool:: the periodicTask cannot executeDelayed";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_TASK_EXECUTE_AGAIN, errMessage.c_str());
        return false;
    }
    return true;
}

bool Task::CanExecutePeriodically(napi_env env)
{
    if (IsExecuted() || IsPeriodicTask()) {
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_TASK_EXECUTE_PERIODICALLY);
        return false;
    }
    if (HasDependency()) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
            "taskpool:: the task with dependency cannot executePeriodically");
        return false;
    }
    return true;
}

void Task::SetHasDependency(bool hasDependency)
{
    hasDependency_ = hasDependency;
}

bool Task::HasDependency() const
{
    return hasDependency_;
}

void Task::TryClearHasDependency()
{
    HILOG_DEBUG("taskpool:: task:%{public}s TryClearHasDependency", std::to_string(taskId_).c_str());
    if (IsExecuted()) {
        HILOG_DEBUG("taskpool:: task TryClearHasDependency end, task is executed");
        return;
    }
    if ((!TaskManager::GetInstance().IsDependentByTaskId(taskId_)) &&
        (!TaskManager::GetInstance().IsDependendByTaskId(taskId_))) {
        SetHasDependency(false);
    }
}

void Task::ThrowNoDependencyError(napi_env env)
{
    std::string errMessage = "taskpool:: task has no dependency";
    HILOG_ERROR("%{public}s", errMessage.c_str());
    ErrorHelper::ThrowError(env, ErrorHelper::ERR_INEXISTENT_DEPENDENCY, errMessage.c_str());
}

void Task::UpdatePeriodicTask()
{
    taskType_ = TaskType::COMMON_TASK;
    napi_reference_ref(env_, taskRef_, nullptr);
    isPeriodicTask_ = true;
}

void Task::InitHandle(napi_env env)
{
#if defined(ENABLE_TASKPOOL_EVENTHANDLER)
    if (!OHOS::AppExecFwk::EventRunner::IsAppMainThread()) {
        uv_loop_t* loop = NapiHelper::GetLibUV(env);
        ConcurrentHelper::UvHandleInit(loop, onResultSignal_, TaskPool::HandleTaskResult, this);
    } else {
        isMainThreadTask_ = true;
        HILOG_DEBUG("taskpool:: eventrunner should be nullptr if the current thread is not the main thread");
    }
#else
    uv_loop_t* loop = NapiHelper::GetLibUV(env);
    ConcurrentHelper::UvHandleInit(loop, onResultSignal_, TaskPool::HandleTaskResult, this);
    auto engine = reinterpret_cast<NativeEngine*>(env);
    isMainThreadTask_ = engine->IsMainThread();
#endif
}

void Task::ClearDelayedTimers()
{
    HILOG_DEBUG("taskpool:: task ClearDelayedTimers");
    std::list<napi_deferred> deferreds {};
    {
        std::lock_guard<RECURSIVE_MUTEX> lock(taskMutex_);
        TaskMessage *taskMessage = nullptr;
        for (auto t: delayedTimers_) {
            if (t == nullptr) {
                continue;
            }
            taskMessage = static_cast<TaskMessage *>(t->data);
            deferreds.push_back(taskMessage->deferred);
            uv_timer_stop(t);
            uv_close(reinterpret_cast<uv_handle_t*>(t), [](uv_handle_t* handle) {
                delete (uv_timer_t*)handle;
                handle = nullptr;
            });
            delete taskMessage;
            taskMessage = nullptr;
        }
        delayedTimers_.clear();
    }
    std::string error = "taskpool:: task has been canceled";
    TaskManager::GetInstance().BatchRejectDeferred(env_, deferreds, error);
}

bool Task::VerifyAndPostResult(Priority priority)
{
#if defined(ENABLE_TASKPOOL_EVENTHANDLER)
    if (IsMainThreadTask()) {
        HITRACE_HELPER_METER_NAME("VerifyAndPostResult: PostTask");
        uint64_t taskId = taskId_;
        auto onResultTask = [taskId]() {
            Task* task = TaskManager::GetInstance().GetTask(taskId);
            if (task == nullptr) {
                return;
            }
            TaskPool::HandleTaskResultCallback(task);
        };
        TaskManager::GetInstance().PostTask(onResultTask, "TaskPoolOnResultTask", priority);
        return true;
    } else {
        std::lock_guard<RECURSIVE_MUTEX> lock(taskMutex_);
        if (!IsValid() || onResultSignal_ == nullptr || uv_is_closing((uv_handle_t*)onResultSignal_)) {
            return false;
        }
        uv_async_send(onResultSignal_);
        return true;
    }
#else
    std::lock_guard<RECURSIVE_MUTEX> lock(taskMutex_);
    if (!IsValid() || onResultSignal_ == nullptr || uv_is_closing((uv_handle_t*)onResultSignal_)) {
        return false;
    }
    uv_async_send(onResultSignal_);
    return true;
#endif
}

void Task::IncreaseTaskRefCount()
{
    refCount_++; // when tasks are created or executed, refCount_ will increment
}

void Task::DecreaseTaskRefCount()
{
    refCount_--; // when tasks finished, refCount_ will decrement
}

bool Task::ShouldDeleteTask(bool needUnref)
{
    std::lock_guard<RECURSIVE_MUTEX> lock(taskMutex_);
    if (!IsValid()) {
        HILOG_WARN("taskpool:: task is invalid");
        TaskManager::GetInstance().RemoveTask(taskId_);
        return true;
    }
    if (needUnref) {
        DecreaseTaskRefCount();
    }
    return false;
}

bool Task::CheckStartExecution(Priority priority)
{
#if defined(ENABLE_TASKPOOL_EVENTHANDLER)
    if (IsMainThreadTask()) {
        if (onStartExecutionCallBackInfo_ == nullptr) {
            return true;
        }
        HITRACE_HELPER_METER_NAME("PerformTask: PostTask");
        uint64_t taskId = taskId_;
        auto onStartExecutionTask = [taskId]() {
            Task* task = TaskManager::GetInstance().GetTask(taskId);
            if (task == nullptr || task->onStartExecutionCallBackInfo_ == nullptr) {
                return;
            }
            Task::StartExecutionTask(task->onStartExecutionCallBackInfo_);
        };
        TaskManager::GetInstance().PostTask(onStartExecutionTask, "TaskPoolOnStartExecutionTask", priority);
    } else {
        if (onStartExecutionSignal_ == nullptr) {
            return true;
        }
        std::lock_guard<RECURSIVE_MUTEX> lock(taskMutex_);
        if (!IsValid()) {
            return false;
        }
        if (onStartExecutionSignal_ != nullptr && !uv_is_closing((uv_handle_t*)onStartExecutionSignal_)) {
            uv_async_send(onStartExecutionSignal_);
        }
    }
    return true;
#else
    if (onStartExecutionSignal_ == nullptr) {
        return true;
    }
    std::lock_guard<RECURSIVE_MUTEX> lock(taskMutex_);
    if (!IsValid()) {
        return false;
    }
    if (onStartExecutionSignal_ != nullptr && !uv_is_closing((uv_handle_t*)onStartExecutionSignal_)) {
        uv_async_send(onStartExecutionSignal_);
    }
    return true;
#endif
}

void Task::SetValid(bool isValid)
{
    isValid_.store(isValid);
}

bool Task::IsValid()
{
    return isValid_.load();
}
} // namespace Commonlibrary::Concurrent::TaskPoolModule