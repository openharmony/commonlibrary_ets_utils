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
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, errMessage.c_str());
        return nullptr;
    }

    Task* task = GenerateTask(env, thisVar, func, name, args, argc);
    TaskManager::GetInstance().StoreTask(task->taskId_, task);
    napi_wrap(env, thisVar, task, TaskDestructor, nullptr, nullptr);
    napi_create_reference(env, thisVar, 0, &task->taskRef_);
    return thisVar;
}

napi_value Task::LongTaskConstructor(napi_env env, napi_callback_info cbinfo)
{
    auto thisVar = TaskConstructor(env, cbinfo);
    Task* task;
    napi_unwrap(env, thisVar, reinterpret_cast<void**>(&task));
    task->isLongTask_ = true;
    return thisVar;
}

void Task::TaskDestructor(napi_env env, void* data, [[maybe_unused]] void* hint)
{
    Task* task = static_cast<Task*>(data);
    TaskManager::GetInstance().ReleaseTaskData(env, task);
    delete task;
}

Task* Task::GenerateTask(napi_env env, napi_value napiTask, napi_value func,
                         napi_value name, napi_value* args, size_t argc)
{
    napi_value argsArray;
    napi_create_array_with_length(env, argc, &argsArray);
    for (size_t i = 0; i < argc; i++) {
        napi_set_element(env, argsArray, i, args[i]);
    }
    if (name == nullptr) {
        name = NapiHelper::GetNameProperty(env, func, NAME);
    }
    char* nameStr = NapiHelper::GetString(env, name);
    Task* task = new Task(env, TaskType::TASK, nameStr);
    delete[] nameStr;
    task->taskId_ = reinterpret_cast<uint64_t>(task);
    uv_loop_t* loop = NapiHelper::GetLibUV(env);
    ConcurrentHelper::UvHandleInit(loop, task->onResultSignal_, TaskPool::HandleTaskResult, task);
    napi_value taskId = NapiHelper::CreateUint64(env, task->taskId_);
    napi_value napiTrue = NapiHelper::CreateBooleanValue(env, true);
    napi_value napiFalse = NapiHelper::CreateBooleanValue(env, false);
    // add task name to task
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
    napi_value argsArray;
    napi_create_array_with_length(env, argc, &argsArray);
    for (size_t i = 0; i < argc; i++) {
        napi_set_element(env, argsArray, i, args[i]);
    }
    napi_value undefined = NapiHelper::GetUndefinedValue(env);
    TaskInfo* taskInfo = GenerateTaskInfo(env, func, argsArray, undefined, undefined, Priority::DEFAULT);
    if (taskInfo == nullptr) {
        return nullptr;
    }
    napi_value napiFuncName = NapiHelper::GetNameProperty(env, func, NAME);
    char* nameStr = NapiHelper::GetString(env, napiFuncName);
    Task* task = new Task(env, type, nameStr);
    delete[] nameStr;
    task->taskId_ = reinterpret_cast<uint64_t>(task);
    task->currentTaskInfo_ = taskInfo;
    uv_loop_t* loop = NapiHelper::GetLibUV(env);
    ConcurrentHelper::UvHandleInit(loop, task->onResultSignal_, TaskPool::HandleTaskResult, task);
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
        std::lock_guard<std::recursive_mutex> lock(taskMutex_);
        if (currentTaskInfo_ == nullptr) {
            currentTaskInfo_ = pendingInfo;
        } else {
            pendingTaskInfos_.push_back(pendingInfo);
        }
    }
    char* name = NapiHelper::GetString(env, taskName);
    if (strlen(name) == 0) {
        napi_value funcName = NapiHelper::GetNameProperty(env, func, NAME);
        name = NapiHelper::GetString(env, funcName);
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
                                "taskpool:: the number of setTransferList parma must be less than 2");
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
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: setTransferList first param must be array");
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
                                    "taskpool:: the element in array must be arraybuffer");
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
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: the number of setCloneList parma must be 1");
        return nullptr;
    }
    if (!NapiHelper::IsArray(env, args[0])) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: setCloneList first param must be array");
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
        if (!NapiHelper::IsArrayBuffer(env, cloneVal) && !NapiHelper::IsSendable(env, cloneVal)) {
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
                "taskpool:: setCloneList elements in array must be ArrayBuffer or Sendable Class instance");
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
            "taskpool:: the number of OnReceiveData parma must be less than 2");
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
            "taskpool:: OnReceiveData's parameter should be function");
        return nullptr;
    }
    // store callbackInfo
    napi_value napiTaskId = NapiHelper::GetNameProperty(env, thisVar, "taskId");
    uint64_t taskId = NapiHelper::GetUint64Value(env, napiTaskId);
    napi_ref callbackRef = Helper::NapiHelper::CreateReference(env, args[0], 1);
    std::shared_ptr<CallbackInfo> callbackInfo = std::make_shared<CallbackInfo>(env, 1, callbackRef);
    auto loop = NapiHelper::GetLibUV(env);
    ConcurrentHelper::UvHandleInit(loop, callbackInfo->onCallbackSignal, TaskPool::ExecuteCallback);
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
    HITRACE_HELPER_METER_NAME(__PRETTY_FUNCTION__);
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    if (argc == 0) {
        std::string errMessage = "taskpool:: addDependency has no params";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, errMessage.c_str());
        return nullptr;
    }

    napi_status status = napi_ok;
    HandleScope scope(env, status);
    napi_value args[argc];
    napi_value napiTask;
    napi_get_cb_info(env, cbinfo, &argc, args, &napiTask, nullptr);
    Task* task = nullptr;
    napi_unwrap(env, napiTask, reinterpret_cast<void**>(&task));
    std::string errMessage = "";
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
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, errMessage.c_str());
            return nullptr;
        } else {
            Task* dependentTask = nullptr;
            napi_unwrap(env, args[i], reinterpret_cast<void**>(&dependentTask));
            if (dependentTask->taskId_ == task->taskId_) {
                HILOG_ERROR("taskpool:: there is a circular dependency");
                ErrorHelper::ThrowError(env, ErrorHelper::ERR_CIRCULAR_DEPENDENCY);
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
    HITRACE_HELPER_METER_NAME(TaskManager::GetInstance().GetTaskDependInfoToString(task->taskId_));
    return nullptr;
}

napi_value Task::RemoveDependency(napi_env env, napi_callback_info cbinfo)
{
    HITRACE_HELPER_METER_NAME(__PRETTY_FUNCTION__);
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    if (argc == 0) {
        std::string errMessage = "taskpool:: removeDependency has no params";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, errMessage.c_str());
        return nullptr;
    }
    napi_status status = napi_ok;
    HandleScope scope(env, status);
    napi_value args[argc];
    napi_value napiTask;
    napi_get_cb_info(env, cbinfo, &argc, args, &napiTask, nullptr);
    Task* task = nullptr;
    napi_unwrap(env, napiTask, reinterpret_cast<void**>(&task));
    if (!task->HasDependency()) {
        ThrowNoDependencyError(env);
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
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, errMessage.c_str());
            return nullptr;
        }
        Task* dependentTask = nullptr;
        napi_unwrap(env, args[i], reinterpret_cast<void**>(&dependentTask));
        if (!dependentTask->HasDependency()) {
            ThrowNoDependencyError(env);
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
    HITRACE_HELPER_METER_NAME(TaskManager::GetInstance().GetTaskDependInfoToString(task->taskId_));
    return nullptr;
}

void Task::ExecuteListenerCallback(const uv_async_t* req)
{
    auto listenerCallBackInfo = static_cast<ListenerCallBackInfo*>(req->data);
    if (listenerCallBackInfo == nullptr) {
        HILOG_FATAL("taskpool:: listenerCallBackInfo is null");
        return;
    }

    auto env = listenerCallBackInfo->env_;
    auto func = NapiHelper::GetReferenceValue(env, listenerCallBackInfo->callbackRef_);
    if (func == nullptr) {
        HILOG_INFO("taskpool:: ExecuteListenerCallback func is null");
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
    if (listenerCallBackInfo == nullptr) {
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
            "taskpool:: OnEnqueued's parameter should be function");
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
    task->onEnqueuedCallBackInfo = new ListenerCallBackInfo(env, callbackRef, nullptr);
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
            "taskpool:: OnStartExecution's parameter should be function");
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
    task->onStartExecutionCallBackInfo = new ListenerCallBackInfo(env, callbackRef, nullptr);
    auto loop = NapiHelper::GetLibUV(env);
    ConcurrentHelper::UvHandleInit(loop, task->onStartExecutionSignal_,
        Task::ExecuteListenerCallback, task->onStartExecutionCallBackInfo);
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
            "taskpool:: OnExecutionFailed's parameter should be function");
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
    task->onExecutionFailedCallBackInfo = new ListenerCallBackInfo(env, callbackRef, nullptr);
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
            "taskpool:: OnExecutionSucceeded's parameter should be function");
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
    task->onExecutionSucceededCallBackInfo = new ListenerCallBackInfo(env, callbackRef, nullptr);
    return nullptr;
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

// The uninitialized state is Task, and then taskType_ will be updated based on the task type.
bool Task::IsInitialized() const
{
    return taskType_ != TaskType::TASK;
}

TaskInfo* Task::GenerateTaskInfo(napi_env env, napi_value func, napi_value args,
                                 napi_value transferList, napi_value cloneList, Priority priority,
                                 bool defaultTransfer, bool defaultCloneSendable)
{
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
    TaskManager::GetInstance().NotifyDependencyTaskInfo(taskId_);
    std::lock_guard<std::recursive_mutex> lock(taskMutex_);
    napi_reference_unref(env_, taskRef_, nullptr);
    delete currentTaskInfo_;
    if (pendingTaskInfos_.empty()) {
        currentTaskInfo_ = nullptr;
        return;
    }
    currentTaskInfo_ = pendingTaskInfos_.front();
    pendingTaskInfos_.pop_front();
    taskState_ = ExecuteState::WAITING;
    TaskManager::GetInstance().EnqueueTaskId(taskId_, currentTaskInfo_->priority);
}

void Task::CancelPendingTask(napi_env env)
{
    if (pendingTaskInfos_.empty()) {
        return;
    }
    napi_value error = ErrorHelper::NewError(env, 0, "taskpool:: task has been canceled");
    auto engine = reinterpret_cast<NativeEngine*>(env);
    for (const auto& info : pendingTaskInfos_) {
        engine->DecreaseSubEnvCounter();
        napi_reject_deferred(env, info->deferred, error);
        napi_reference_unref(env, taskRef_, nullptr);
        delete info;
    }
    pendingTaskInfos_.clear();
}

bool Task::UpdateTask(uint64_t startTime, void* worker)
{
    if (taskState_ == ExecuteState::CANCELED) { // task may have been canceled
        static_cast<Worker*>(worker)->NotifyTaskFinished();
        HILOG_DEBUG("taskpool:: task has been canceled");
        return false;
    }
    taskState_ = ExecuteState::RUNNING;
    startTime_ = startTime;
    worker_ = worker;
    return true;
}

napi_value Task::DeserializeValue(napi_env env, bool isFunc, bool isArgs)
{
    napi_value result = nullptr;
    napi_status status = napi_ok;
    std::string errMessage = "";
    std::lock_guard<std::recursive_mutex> lock(taskMutex_);
    if (UNLIKELY(currentTaskInfo_ == nullptr)) {
        HILOG_ERROR("taskpool:: the currentTaskInfo is nullptr, the task may have been cancelled");
        return nullptr;
    }
    if (isFunc) {
        status = napi_deserialize(env, currentTaskInfo_->serializationFunction, &result);
        if (!IsGroupFunctionTask()) {
            napi_delete_serialization_data(env, currentTaskInfo_->serializationFunction);
        }
        if (status != napi_ok || result == nullptr) {
            errMessage = "taskpool:: failed to deserialize function.";
            HILOG_ERROR("%{public}s", errMessage.c_str());
            napi_value err = ErrorHelper::NewError(env, ErrorHelper::ERR_WORKER_SERIALIZATION, errMessage.c_str());
            success_ = false;
            static_cast<Worker*>(worker_)->NotifyTaskResult(env, this, err);
            return nullptr;
        }
        return result;
    } else if (isArgs) {
        status = napi_deserialize(env, currentTaskInfo_->serializationArguments, &result);
        if (!IsGroupFunctionTask()) {
            napi_delete_serialization_data(env, currentTaskInfo_->serializationArguments);
        }
        if (status != napi_ok || result == nullptr) {
            errMessage = "taskpool:: failed to deserialize arguments.";
            HILOG_ERROR("%{public}s", errMessage.c_str());
            napi_value err = ErrorHelper::NewError(env, ErrorHelper::ERR_WORKER_SERIALIZATION, errMessage.c_str());
            success_ = false;
            static_cast<Worker*>(worker_)->NotifyTaskResult(env, this, err);
            return nullptr;
        }
        return result;
    }
    return nullptr;
}

void Task::StoreTaskDuration()
{
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
    if (IsInitialized() && IsLongTask()) {
        errMessage = "taskpool:: The long task can only be executed once";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, errMessage.c_str());
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
    if (IsInitialized() && IsLongTask()) {
        errMessage = "taskpool:: Multiple executions of longTask are not supported in the executeDelayed";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, errMessage.c_str());
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
    if (taskType_ != TaskType::TASK) {
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
} // namespace Commonlibrary::Concurrent::TaskPoolModule