/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "taskpool.h"

#include "async_runner_manager.h"
#include "helper/hitrace_helper.h"
#include "sequence_runner_manager.h"
#include "task_group_manager.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
using namespace Commonlibrary::Concurrent::Common::Helper;

napi_value TaskPool::InitTaskPool(napi_env env, napi_value exports)
{
    HITRACE_HELPER_METER_NAME(__PRETTY_FUNCTION__);
    napi_value taskClass = nullptr;
    NAPI_CALL(env, napi_define_class(env, "Task", NAPI_AUTO_LENGTH, Task::TaskConstructor,
              nullptr, 0, nullptr, &taskClass));
    napi_value longTaskClass = nullptr;
    NAPI_CALL(env, napi_define_class(env, "LongTask", NAPI_AUTO_LENGTH, Task::LongTaskConstructor,
              nullptr, 0, nullptr, &longTaskClass));
    napi_value genericsTaskClass = nullptr;
    NAPI_CALL(env, napi_define_class(env, "GenericsTask", NAPI_AUTO_LENGTH, Task::TaskConstructor,
              nullptr, 0, nullptr, &genericsTaskClass));
    napi_value isCanceledFunc = nullptr;
    NAPI_CALL(env, napi_create_function(env, "isCanceled", NAPI_AUTO_LENGTH, Task::IsCanceled, NULL, &isCanceledFunc));
    napi_set_named_property(env, taskClass, "isCanceled", isCanceledFunc);
    napi_set_named_property(env, longTaskClass, "isCanceled", isCanceledFunc);
    napi_value sendDataFunc = nullptr;
    NAPI_CALL(env, napi_create_function(env, "sendData", NAPI_AUTO_LENGTH, Task::SendData, NULL, &sendDataFunc));
    napi_set_named_property(env, taskClass, "sendData", sendDataFunc);
    napi_set_named_property(env, longTaskClass, "sendData", sendDataFunc);
    napi_value taskGroupClass = nullptr;
    NAPI_CALL(env, napi_define_class(env, "TaskGroup", NAPI_AUTO_LENGTH, TaskGroup::TaskGroupConstructor,
              nullptr, 0, nullptr, &taskGroupClass));
    napi_value seqRunnerClass = nullptr;
    NAPI_CALL(env, napi_define_class(env, "SequenceRunner", NAPI_AUTO_LENGTH, SequenceRunner::SeqRunnerConstructor,
              nullptr, 0, nullptr, &seqRunnerClass));
    napi_value asyncRunnerClass = nullptr;
    NAPI_CALL(env, napi_define_class(env, "AsyncRunner", NAPI_AUTO_LENGTH, AsyncRunner::AsyncRunnerConstructor,
              nullptr, 0, nullptr, &asyncRunnerClass));

    // define priority
    napi_value priorityObj = NapiHelper::CreateObject(env);
    napi_value highPriority = NapiHelper::CreateUint32(env, Priority::HIGH);
    napi_value mediumPriority = NapiHelper::CreateUint32(env, Priority::MEDIUM);
    napi_value lowPriority = NapiHelper::CreateUint32(env, Priority::LOW);
    napi_value idlePriority = NapiHelper::CreateUint32(env, Priority::IDLE);
    napi_property_descriptor exportPriority[] = {
        DECLARE_NAPI_PROPERTY("HIGH", highPriority),
        DECLARE_NAPI_PROPERTY("MEDIUM", mediumPriority),
        DECLARE_NAPI_PROPERTY("LOW", lowPriority),
        DECLARE_NAPI_PROPERTY("IDLE", idlePriority),
    };
    napi_define_properties(env, priorityObj, sizeof(exportPriority) / sizeof(exportPriority[0]), exportPriority);

    // define State
    napi_value stateObj = NapiHelper::CreateObject(env);
    napi_value waitingState = NapiHelper::CreateUint32(env, ExecuteState::WAITING);
    napi_value runningState = NapiHelper::CreateUint32(env, ExecuteState::RUNNING);
    napi_value canceledState = NapiHelper::CreateUint32(env, ExecuteState::CANCELED);
    napi_property_descriptor exportState[] = {
        DECLARE_NAPI_PROPERTY("WAITING", waitingState),
        DECLARE_NAPI_PROPERTY("RUNNING", runningState),
        DECLARE_NAPI_PROPERTY("CANCELED", canceledState),
    };
    napi_define_properties(env, stateObj, sizeof(exportState) / sizeof(exportState[0]), exportState);

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_PROPERTY("Task", taskClass),
        DECLARE_NAPI_PROPERTY("LongTask", longTaskClass),
        DECLARE_NAPI_PROPERTY("GenericsTask", genericsTaskClass),
        DECLARE_NAPI_PROPERTY("TaskGroup", taskGroupClass),
        DECLARE_NAPI_PROPERTY("SequenceRunner", seqRunnerClass),
        DECLARE_NAPI_PROPERTY("AsyncRunner", asyncRunnerClass),
        DECLARE_NAPI_PROPERTY("Priority", priorityObj),
        DECLARE_NAPI_PROPERTY("State", stateObj),
        DECLARE_NAPI_FUNCTION("execute", Execute),
        DECLARE_NAPI_FUNCTION("executeDelayed", ExecuteDelayed),
        DECLARE_NAPI_FUNCTION("cancel", Cancel),
        DECLARE_NAPI_FUNCTION("getTaskPoolInfo", GetTaskPoolInfo),
        DECLARE_NAPI_FUNCTION("terminateTask", TerminateTask),
        DECLARE_NAPI_FUNCTION("isConcurrent", IsConcurrent),
        DECLARE_NAPI_FUNCTION("executePeriodically", ExecutePeriodically),
        DECLARE_NAPI_FUNCTION("getTask", GetTask),
    };
    napi_define_properties(env, exports, sizeof(properties) / sizeof(properties[0]), properties);

    TaskManager::GetInstance().InitTaskManager(env);
    return exports;
}

// ---------------------------------- SendData ---------------------------------------
void TaskPool::ExecuteOnReceiveDataCallback(TaskResultInfo* resultInfo)
{
    CallbackInfo* callbackInfo = TaskManager::GetInstance().GetSenddataCallback(resultInfo->taskId);
    if (callbackInfo == nullptr) {
        HILOG_ERROR("taskpool:: ExecuteOnReceiveDataCallback callbackInfo is nullptr");
        return;
    }
    ObjectScope<TaskResultInfo> resultInfoScope(resultInfo, false);
    napi_status status = napi_ok;
    std::string traceLabel = "ExecuteOnReceiveDataCallback type: " + callbackInfo->type
        + ", taskId: " + std::to_string(resultInfo->taskId);
    HITRACE_HELPER_METER_NAME(traceLabel);
    auto env = callbackInfo->hostEnv;
    CallbackScope callbackScope(env, resultInfo, status);
    if (status != napi_ok) {
        HILOG_ERROR("napi_open_handle_scope failed");
        return;
    }
    auto func = NapiHelper::GetReferenceValue(env, callbackInfo->callbackRef);
    napi_value args;
    napi_value result;
    status = napi_deserialize(env, resultInfo->serializationArgs, &args);
    napi_delete_serialization_data(env, resultInfo->serializationArgs);
    if (status != napi_ok || args == nullptr) {
        std::string errMessage = "taskpool:: failed to serialize function";
        HILOG_ERROR("%{public}s in SendData", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_WORKER_SERIALIZATION, errMessage.c_str());
        return;
    }
    uint32_t argsNum = NapiHelper::GetArrayLength(env, args);
    napi_value argsArray[argsNum];
    for (size_t i = 0; i < argsNum; i++) {
        argsArray[i] = NapiHelper::GetElement(env, args, i);
    }
    napi_call_function(env, NapiHelper::GetGlobalObject(env), func, argsNum, argsArray, &result);
    if (NapiHelper::IsExceptionPending(env)) {
        napi_value exception = nullptr;
        napi_get_and_clear_last_exception(env, &exception);
        HILOG_ERROR("taskpool:: an exception has occurred in napi_call_function");
    }
}
// ---------------------------------- SendData ---------------------------------------

napi_value TaskPool::GetTaskPoolInfo(napi_env env, [[maybe_unused]] napi_callback_info cbinfo)
{
    napi_value result = nullptr;
    napi_create_object(env, &result);
    napi_value threadInfos = TaskManager::GetInstance().GetThreadInfos(env);
    napi_value taskInfos = TaskManager::GetInstance().GetTaskInfos(env);
    napi_set_named_property(env, result, "threadInfos", threadInfos);
    napi_set_named_property(env, result, "taskInfos", taskInfos);
    return result;
}

napi_value TaskPool::TerminateTask(napi_env env, napi_callback_info cbinfo)
{
    HITRACE_HELPER_METER_NAME(__PRETTY_FUNCTION__);
    size_t argc = 1; // 1: long task
    napi_value args[1];
    napi_get_cb_info(env, cbinfo, &argc, args, nullptr, nullptr);
    if (argc < 1) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "the number of terminateTask's params must be one.");
        return nullptr;
    }
    if (!NapiHelper::IsObject(env, args[0])) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "the type of terminateTask's params must be object.");
        return nullptr;
    }
    napi_value napiTaskId = NapiHelper::GetNameProperty(env, args[0], TASKID_STR);
    uint32_t taskId = NapiHelper::GetUint32Value(env, napiTaskId);
    auto task = TaskManager::GetInstance().GetTask(taskId);
    if (task == nullptr || !task->IsLongTask()) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
            "the type of terminateTask's params must be long task.");
        return nullptr;
    }
    TaskManager::GetInstance().TerminateTask(taskId);
    return nullptr;
}

napi_value TaskPool::Execute(napi_env env, napi_callback_info cbinfo)
{
    HITRACE_HELPER_METER_NAME(__PRETTY_FUNCTION__);
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    if (argc < 1) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "the number of execute's params must be at least one.");
        return nullptr;
    }
    napi_value* args = new napi_value[argc];
    ObjectScope<napi_value> scope(args, true);
    napi_get_cb_info(env, cbinfo, &argc, args, nullptr, nullptr);
    napi_valuetype type = napi_undefined;
    napi_typeof(env, args[0], &type);
    if (type == napi_object) {
        uint32_t priority = Priority::DEFAULT; // DEFAULT priority is MEDIUM
        uint32_t timeout = 0;
        if (argc > 1) {
            auto result = GetExecuteParams(env, args[1]);
            if (result.first >= Priority::NUMBER) {
                return nullptr;
            }
            priority = result.first;
            timeout = result.second;
        }
        if (NapiHelper::HasNameProperty(env, args[0], GROUP_ID_STR)) {
            return ExecuteGroup(env, args[0], static_cast<Priority>(priority));
        }
        Task* task = nullptr;
        napi_unwrap(env, args[0], reinterpret_cast<void**>(&task));
        if (task == nullptr) {
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "the type of execute's first param must be task.");
            return nullptr;
        }
        if (!task->CanExecute(env)) {
            return nullptr;
        }
        if (timeout > 0 && !task->IsNotFoundState()) {
            ErrorHelper::ThrowError(env, ErrorHelper::ERR_TASK_CANNOT_EXECUTED, "the task cannot be set timeout.");
            return nullptr;
        }
        napi_value promise = task->GetTaskInfoPromise(env, args[0], TaskType::COMMON_TASK,
                                                      static_cast<Priority>(priority));
        if (promise == nullptr) {
            return nullptr;
        }
        task->timeout_ = timeout;
        ExecuteTask(env, task, static_cast<Priority>(priority));
        return promise;
    }
    if (type != napi_function) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
            "the type of execute's first param must be object or function.");
        return nullptr;
    }
    Task* task = Task::GenerateFunctionTask(env, args[0], args + 1, argc - 1, TaskType::FUNCTION_TASK);
    if (task == nullptr) {
        HILOG_ERROR("taskpool:: GenerateFunctionTask failed");
        return nullptr;
    }
    napi_value promise = NapiHelper::CreatePromise(env, &task->currentTaskInfo_->deferred);
    if (promise == nullptr) { // LOCV_EXCL_BR_LINE
        task->ReleaseData();
        task->SetValid(false);
        if (TaskManager::GetInstance().RemoveTask(task->GetTaskId())) {
            delete task;
            task = nullptr;
        }
        std::string err = "execute create promise failed, maybe has exception.";
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, err.c_str());
        HILOG_ERROR("taskpool:: %{public}s", err.c_str());
        return nullptr;
    }
    ExecuteTask(env, task);
    return promise;
}

void TaskPool::DelayTask(uv_timer_t* handle)
{
    TaskMessage* taskMessage = static_cast<TaskMessage*>(handle->data);
    auto task = TaskManager::GetInstance().GetTask(taskMessage->taskId);
    napi_status status = napi_ok;
    if (task == nullptr) {
        HILOG_DEBUG("taskpool:: task is nullptr");
    } else if (task->taskState_ == ExecuteState::CANCELED) {
        HILOG_DEBUG("taskpool:: DelayTask task has been canceled");
        HandleScope scope(task->env_, status);
        if (status != napi_ok) {
            HILOG_ERROR("taskpool:: napi_open_handle_scope failed");
            return;
        }
        napi_value error = TaskManager::GetInstance().CancelError(task->env_, 0, "taskpool:: task has been canceled");
        napi_reject_deferred(task->env_, taskMessage->deferred, error);
    } else {
        HILOG_INFO("taskpool::delay tId %{public}s", std::to_string(taskMessage->taskId).c_str());
        HandleScope scope(task->env_, status);
        if (status != napi_ok) {
            HILOG_ERROR("taskpool:: napi_open_handle_scope failed");
            return;
        }
        TaskManager::GetInstance().IncreaseSendDataRefCount(taskMessage->taskId);
        task->IncreaseRefCount();
        napi_value napiTask = NapiHelper::GetReferenceValue(task->env_, task->taskRef_);
        TaskInfo* taskInfo = task->GetTaskInfo(task->env_, napiTask, taskMessage->priority);
        if (taskInfo != nullptr) {
            taskInfo->deferred = taskMessage->deferred;
            if (task->taskState_ == ExecuteState::DELAYED || task->taskState_ == ExecuteState::FINISHED) {
                task->taskState_ = ExecuteState::WAITING;
                TaskManager::GetInstance().EnqueueTaskId(taskMessage->taskId, Priority(taskMessage->priority));
            }
        } else {
            napi_value execption = nullptr;
            napi_get_and_clear_last_exception(task->env_, &execption);
            if (execption != nullptr) {
                napi_reject_deferred(task->env_, taskMessage->deferred, execption);
            }
        }
    }
    if (task != nullptr) {
        std::lock_guard<std::recursive_mutex> lock(task->taskMutex_);
        task->delayedTimers_.erase(handle);
    }
    uv_timer_stop(handle);
    ConcurrentHelper::UvHandleClose(handle);
    delete taskMessage;
    taskMessage = nullptr;
}

napi_value TaskPool::ExecuteDelayed(napi_env env, napi_callback_info cbinfo)
{
    HITRACE_HELPER_METER_NAME(__PRETTY_FUNCTION__);
    uint32_t priority = Priority::DEFAULT; // DEFAULT priority is MEDIUM
    int32_t delayTime = 0;
    Task* task = nullptr;
    if (!CheckDelayedParams(env, cbinfo, priority, delayTime, task)) {
        return nullptr;
    }

    task->UpdateTaskStateToDelayed();
    task->UpdateTaskType(TaskType::COMMON_TASK);

    TaskMessage* taskMessage = new TaskMessage();
    taskMessage->priority = static_cast<Priority>(priority);
    taskMessage->taskId = task->taskId_;
    napi_value promise = NapiHelper::CreatePromise(env, &taskMessage->deferred);
    if (promise == nullptr) { // LOCV_EXCL_BR_LINE
        delete taskMessage;
        taskMessage = nullptr;
        std::string err = "executeDelayed create promise failed, maybe has exception.";
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, err.c_str());
        HILOG_ERROR("taskpool:: %{public}s", err.c_str());
        return nullptr;
    }
    uv_loop_t* loop = NapiHelper::GetLibUV(env);
    uv_update_time(loop);
    uv_timer_t* timer = new uv_timer_t;
    uv_timer_init(loop, timer);
    timer->data = taskMessage;

    std::string strTrace = "ExecuteDelayed: taskId: " + std::to_string(task->taskId_);
    strTrace += ", priority: " + std::to_string(priority);
    strTrace += ", delayTime " + std::to_string(delayTime);
    HITRACE_HELPER_METER_NAME(strTrace);
    HILOG_INFO("taskpool::%{public}s", strTrace.c_str());

    uv_timer_start(timer, reinterpret_cast<uv_timer_cb>(DelayTask), delayTime, 0);
    {
        std::lock_guard<std::recursive_mutex> lock(task->taskMutex_);
        task->delayedTimers_.insert(timer);
    }
    NativeEngine* engine = reinterpret_cast<NativeEngine*>(env);
    if (engine->IsMainThread()) {
        uv_async_send(&loop->wq_async);
    }
    return promise;
}

napi_value TaskPool::ExecuteGroup(napi_env env, napi_value napiTaskGroup, Priority priority)
{
    napi_value napiGroupId = NapiHelper::GetNameProperty(env, napiTaskGroup, GROUP_ID_STR);
    uint64_t groupId = NapiHelper::GetUint64Value(env, napiGroupId);
    HILOG_INFO("taskpool::ExecuteGroup groupId %{public}s", std::to_string(groupId).c_str());
    auto taskGroup = TaskGroupManager::GetInstance().GetTaskGroup(groupId);
    if (taskGroup == nullptr) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskGroup is nullptr.");
        return nullptr;
    }
    napi_reference_ref(env, taskGroup->groupRef_, nullptr);
    if (taskGroup->groupState_ == ExecuteState::NOT_FOUND || taskGroup->groupState_ == ExecuteState::FINISHED ||
        taskGroup->groupState_ == ExecuteState::CANCELED) {
        taskGroup->groupState_ = ExecuteState::WAITING;
    }
    GroupInfo* groupInfo = new GroupInfo();
    groupInfo->priority = priority;
    napi_value resArr;
    napi_create_array_with_length(env, taskGroup->taskIds_.size(), &resArr);
    napi_ref arrRef = NapiHelper::CreateReference(env, resArr, 1);
    groupInfo->resArr = arrRef;
    napi_value promise = NapiHelper::CreatePromise(env, &groupInfo->deferred);
    if (promise == nullptr) { // LOCV_EXCL_BR_LINE
        napi_reference_unref(env, taskGroup->groupRef_, nullptr);
        napi_delete_reference(env, groupInfo->resArr);
        delete groupInfo;
        groupInfo = nullptr;
        std::string err = "executeGroup create promise failed, maybe has exception.";
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, err.c_str());
        HILOG_ERROR("taskpool:: %{public}s", err.c_str());
        return nullptr;
    }
    {
        std::lock_guard<std::recursive_mutex> lock(taskGroup->taskGroupMutex_);
        if (taskGroup->taskNum_ == 0) {
            napi_resolve_deferred(env, groupInfo->deferred, resArr);
            taskGroup->groupState_ = ExecuteState::FINISHED;
            napi_delete_reference(env, groupInfo->resArr);
            napi_reference_unref(env, taskGroup->groupRef_, nullptr);
            delete groupInfo;
            taskGroup->currentGroupInfo_ = nullptr;
            return promise;
        }
        if (taskGroup->currentGroupInfo_ == nullptr) {
            taskGroup->currentGroupInfo_ = groupInfo;
            for (auto iter = taskGroup->taskRefs_.begin(); iter != taskGroup->taskRefs_.end(); iter++) {
                napi_value napiTask = NapiHelper::GetReferenceValue(env, *iter);
                Task* task = nullptr;
                napi_unwrap(env, napiTask, reinterpret_cast<void**>(&task));
                if (task == nullptr) {
                    HILOG_ERROR("taskpool::ExecuteGroup task is nullptr");
                    return nullptr;
                }
                napi_reference_ref(env, task->taskRef_, nullptr);
                if (task->IsGroupCommonTask()) {
                    task->GetTaskInfo(env, napiTask, static_cast<Priority>(priority));
                }
                ExecuteTask(env, task, static_cast<Priority>(priority));
            }
        } else {
            taskGroup->pendingGroupInfos_.push_back(groupInfo);
        }
    }
    return promise;
}

void TaskPool::HandleTaskResult(Task* task)
{
    HILOG_DEBUG("taskpool:: HandleTaskResult task");
    HITRACE_HELPER_METER_NAME(__PRETTY_FUNCTION__);
    TaskManager::GetInstance().RemoveRunningTask(task->taskId_); // update task execution info
    if (!task->IsMainThreadTask()) {
        if (task->ShouldDeleteTask(false)) {
            delete task;
            return;
        }
        if (task->IsFunctionTask()) {
            napi_remove_env_cleanup_hook(task->env_, Task::CleanupHookFunc, task);
        }
    }
    task->DecreaseTaskLifecycleCount();

    HandleTaskResultInner(task);
}

void TaskPool::HandleTaskResultInner(Task* task)
{
    napi_handle_scope scope = nullptr;
    NAPI_CALL_RETURN_VOID(task->env_, napi_open_handle_scope(task->env_, &scope));
    napi_value napiTaskResult = nullptr;
    napi_status status = napi_deserialize(task->env_, task->result_, &napiTaskResult);
    napi_delete_serialization_data(task->env_, task->result_);
    if (task->IsTimeoutState()) {
        task->success_ = false;
    }
    bool isCancel = false;
    RecordTaskResultLog(task, status, napiTaskResult, isCancel);

    if (napiTaskResult == nullptr) {
        napi_get_undefined(task->env_, &napiTaskResult);
    }
    reinterpret_cast<NativeEngine*>(task->env_)->DecreaseSubEnvCounter();
    bool success = ((status == napi_ok) && !task->IsTimeoutState() && !task->IsCanceledState()) && (task->success_);
    task->UpdateTaskStateToEnding();
    task->isCancelToFinish_ = false;
    if (task->IsGroupTask()) {
        UpdateGroupInfoByResult(task->env_, task, napiTaskResult, success);
    } else if (!task->IsPeriodicTask() && !task->IsTimeoutState()) {
        if (success) {
            napi_resolve_deferred(task->env_, task->currentTaskInfo_->deferred, napiTaskResult);
            if (task->onExecutionSucceededCallBackInfo_ != nullptr) {
                task->ExecuteListenerCallback(task->onExecutionSucceededCallBackInfo_, task->taskId_);
            }
        } else {
            napi_reject_deferred(task->env_, task->currentTaskInfo_->deferred, napiTaskResult);
            if (task->onExecutionFailedCallBackInfo_ != nullptr) {
                task->onExecutionFailedCallBackInfo_->taskError_ = napiTaskResult;
                task->ExecuteListenerCallback(task->onExecutionFailedCallBackInfo_, task->taskId_);
            }
        }
    }
    NAPI_CALL_RETURN_VOID(task->env_, napi_close_handle_scope(task->env_, scope));
    TriggerTask(task, isCancel);
}

void TaskPool::TriggerTask(Task* task, bool isCancel)
{
    HILOG_DEBUG("taskpool:: task:%{public}s TriggerTask", std::to_string(task->taskId_).c_str());
    if (task->IsGroupTask()) {
        return;
    }
    TaskManager::GetInstance().DecreaseSendDataRefCount(task->env_, task->taskId_);
    task->UpdateTaskStateToFinished();
    // seqRunnerTask will trigger the next
    if (task->IsSeqRunnerTask()) {
        if (!SequenceRunnerManager::GetInstance().TriggerSeqRunner(task->env_, task)) {
            HILOG_WARN("taskpool:: task %{public}s trigger in seqRunner %{public}s failed",
                std::to_string(task->taskId_).c_str(), std::to_string(task->runnerId_).c_str());
        }
    } else if (task->IsCommonTask()) {
        if (!isCancel) {
            TaskManager::GetInstance().NotifyDependencyTaskInfo(task->taskId_);
        }
        task->NotifyPendingTask();
    } else if (task->IsAsyncRunnerTask()) {
        if (!AsyncRunnerManager::GetInstance().TriggerAsyncRunner(task->env_, task)) {
            HILOG_ERROR("taskpool:: task %{public}s trigger in asyncRunner %{public}s failed",
                        std::to_string(task->taskId_).c_str(), std::to_string(task->runnerId_).c_str());
        }
    }
    if (task->IsPeriodicTask()) {
        return;
    }
    if (!task->IsFunctionTask()) {
        napi_reference_unref(task->env_, task->taskRef_, nullptr);
        return;
    }
    // function task need release data
    task->ReleaseData();
    TaskManager::GetInstance().RemoveTask(task->taskId_);
    delete task;
}

void TaskPool::UpdateGroupInfoByResult(napi_env env, Task* task, napi_value res, bool success)
{
    HILOG_DEBUG("taskpool:: task:%{public}s UpdateGroupInfoByResult", std::to_string(task->taskId_).c_str());
    TaskManager::GetInstance().DecreaseSendDataRefCount(task->env_, task->taskId_);
    task->UpdateTaskStateToFinished();
    napi_reference_unref(env, task->taskRef_, nullptr);
    if (task->IsGroupCommonTask()) {
        delete task->currentTaskInfo_;
        task->currentTaskInfo_ = nullptr;
    }
    TaskGroup* taskGroup = TaskGroupManager::GetInstance().GetTaskGroup(task->groupId_);
    if (taskGroup == nullptr || taskGroup->currentGroupInfo_ == nullptr) {
        HILOG_DEBUG("taskpool:: taskGroup may have been released or canceled");
        return;
    }
    // store the result
    uint32_t index = taskGroup->GetTaskIndex(task->taskId_);
    auto groupInfo = taskGroup->currentGroupInfo_;
    napi_ref arrRef = groupInfo->resArr;
    napi_value resArr = NapiHelper::GetReferenceValue(env, arrRef);
    napi_set_element(env, resArr, index, res);
    groupInfo->finishedTaskNum++;
    // store the index when the first exception occurs
    if (!success && !groupInfo->HasException()) {
        groupInfo->SetFailedIndex(index);
    }
    // we will not handle the result until all tasks are finished
    if (groupInfo->finishedTaskNum < taskGroup->taskNum_) {
        return;
    }
    // if there is no exception, just resolve
    if (!groupInfo->HasException()) {
        HILOG_INFO("taskpool::taskGroup perform end, taskGroupId %{public}s", std::to_string(task->groupId_).c_str());
        napi_resolve_deferred(env, groupInfo->deferred, resArr);
        for (uint32_t taskId : taskGroup->taskIds_) {
            auto task = TaskManager::GetInstance().GetTask(taskId);
            if (task != nullptr && task->onExecutionSucceededCallBackInfo_ != nullptr) {
                task->ExecuteListenerCallback(task->onExecutionSucceededCallBackInfo_, task->taskId_);
            }
        }
    } else { // LOCV_EXCL_BR_LINE
        napi_value res = nullptr;
        napi_get_element(env, resArr, groupInfo->GetFailedIndex(), &res);
        napi_reject_deferred(env, groupInfo->deferred, res);
        auto iter = taskGroup->taskIds_.begin();
        std::advance(iter, groupInfo->GetFailedIndex());
        auto task = iter != taskGroup->taskIds_.end() ? TaskManager::GetInstance().GetTask(*iter) : nullptr;
        if (task != nullptr && task->onExecutionFailedCallBackInfo_ != nullptr) {
            task->onExecutionFailedCallBackInfo_->taskError_ = res;
            task->ExecuteListenerCallback(task->onExecutionFailedCallBackInfo_, task->taskId_);
        }
    }
    taskGroup->groupState_ = ExecuteState::FINISHED;
    napi_delete_reference(env, groupInfo->resArr);
    napi_reference_unref(env, taskGroup->groupRef_, nullptr);
    delete groupInfo;
    taskGroup->currentGroupInfo_ = nullptr;
    taskGroup->NotifyGroupTask(env);
}

void TaskPool::ExecuteTask(napi_env env, Task* task, Priority priority)
{
    // tag for trace parse: Task Allocation
    std::string strTrace = "Task Allocation: taskId : " + std::to_string(task->taskId_)
        + ", priority : " + std::to_string(priority)
        + ", executeState : " + std::to_string(ExecuteState::WAITING);
    HITRACE_HELPER_METER_NAME(strTrace);
    std::string taskLog = "Task Allocation: " + std::to_string(task->taskId_) + ", " + std::to_string(priority);
    task->IncreaseRefCount();
    TaskManager::GetInstance().IncreaseSendDataRefCount(task->taskId_);
    if (task->UpdateTaskStateToWaiting()) {
        HILOG_TASK_INFO("taskpool:: %{public}s", taskLog.c_str());
        task->isCancelToFinish_ = false;
        TaskManager::GetInstance().EnqueueTaskId(task->taskId_, priority);
        TriggerTimeoutTimer(env, task);
    } else {
        HILOG_WARN("taskpool:: %{public}s, not enqueue", taskLog.c_str());
    }
}

napi_value TaskPool::Cancel(napi_env env, napi_callback_info cbinfo)
{
    HITRACE_HELPER_METER_NAME(__PRETTY_FUNCTION__);
    size_t argc = 1;
    napi_value args[1];
    napi_get_cb_info(env, cbinfo, &argc, args, nullptr, nullptr);
    if (argc < 1) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "the number of cancel's params must be 1.");
        return nullptr;
    }

    if (NapiHelper::IsNumber(env, args[0])) {
        uint32_t taskId = NapiHelper::GetUint32Value(env, args[0]);
        TaskManager::GetInstance().CancelTask(env, taskId);
        return nullptr;
    }

    if (!NapiHelper::IsObject(env, args[0])) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "the type of cancel's params must be object.");
        return nullptr;
    }

    if (!NapiHelper::HasNameProperty(env, args[0], GROUP_ID_STR)) {
        napi_value napiTaskId = NapiHelper::GetNameProperty(env, args[0], TASKID_STR);
        if (napiTaskId == nullptr) {
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "the type of cancel's params must be task.");
            return nullptr;
        }
        uint32_t taskId = NapiHelper::GetUint32Value(env, napiTaskId);
        TaskManager::GetInstance().CancelTask(env, taskId);
    } else {
        napi_value napiGroupId = NapiHelper::GetNameProperty(env, args[0], GROUP_ID_STR);
        if (napiGroupId == nullptr) {
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "the type of cancel's params must be taskGroup.");
            return nullptr;
        }
        uint64_t groupId = NapiHelper::GetUint64Value(env, napiGroupId);
        TaskGroupManager::GetInstance().CancelGroup(env, groupId);
    }
    return nullptr;
}

napi_value TaskPool::IsConcurrent(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = 1;
    napi_value args[1];
    napi_get_cb_info(env, cbinfo, &argc, args, nullptr, nullptr);
    if (argc != 1) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "the number of isConcurrent's params must be 1.");
        return nullptr;
    }

    if (!NapiHelper::IsFunction(env, args[0])) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
            "the type of isConcurrent's first param must be function.");
        return nullptr;
    }

    bool isConcurrent = NapiHelper::IsConcurrentFunction(env, args[0]);
    return NapiHelper::CreateBooleanValue(env, isConcurrent);
}

void TaskPool::PeriodicTaskCallback(uv_timer_t* handle)
{
    Task* task = reinterpret_cast<Task*>(handle->data);
    if (task == nullptr) {
        HILOG_DEBUG("taskpool:: the task is nullptr");
        return;
    } else if (!task->IsPeriodicTask()) {
        HILOG_DEBUG("taskpool:: the current task is not a periodic task");
        return;
    } else if (task->taskState_ == ExecuteState::CANCELED) {
        if (task->currentTaskInfo_ == nullptr) {
            HILOG_DEBUG("taskpool:: the periodic task has been canceled");
            napi_reference_unref(task->env_, task->taskRef_, nullptr);
            task->CancelPendingTask(task->env_);
            uv_timer_stop(task->timer_);
            ConcurrentHelper::UvHandleClose(task->timer_);
        }
        return;
    }
    TaskManager::GetInstance().IncreaseSendDataRefCount(task->taskId_);

    napi_status status = napi_ok;
    HandleScope scope(task->env_, status);
    if (status != napi_ok) {
        HILOG_ERROR("taskpool:: napi_open_handle_scope failed");
        return;
    }
    napi_value napiTask = NapiHelper::GetReferenceValue(task->env_, task->taskRef_);
    TaskInfo* taskInfo = task->GetTaskInfo(task->env_, napiTask, task->periodicTaskPriority_);
    if (taskInfo == nullptr) {
        HILOG_DEBUG("taskpool:: the periodic task taskInfo is nullptr");
        return;
    }

    task->IncreaseRefCount();
    HILOG_INFO("taskpool::PeriodicTaskCallback tId %{public}s", std::to_string(task->taskId_).c_str());
    if (task->UpdateTaskStateToWaiting()) {
        TaskManager::GetInstance().EnqueueTaskId(task->taskId_, task->periodicTaskPriority_);
    }
}

napi_value TaskPool::ExecutePeriodically(napi_env env, napi_callback_info cbinfo)
{
    int32_t period = 0;
    uint32_t priority = Priority::DEFAULT;
    Task* periodicTask = nullptr;
    if (!CheckPeriodicallyParams(env, cbinfo, period, priority, periodicTask)) {
        return nullptr;
    }

    if (!periodicTask->CanExecutePeriodically(env)) {
        return nullptr;
    }
    periodicTask->UpdatePeriodicTask();

    periodicTask->periodicTaskPriority_ = static_cast<Priority>(priority);
    napi_value napiTask = NapiHelper::GetReferenceValue(env, periodicTask->taskRef_);
    auto [func, args, transferList, cloneList] = Task::GetSerializeParams(env, napiTask);
    if (func == nullptr || args == nullptr) {
        return nullptr;
    }
    std::tuple<napi_value, napi_value, bool, bool> params = {
        transferList, cloneList, periodicTask->defaultTransfer_, periodicTask->defaultCloneSendable_
    };
    auto [serFunction, serArguments] = Task::GetSerializeResult(env, func, args, params);
    if (serFunction == nullptr || serArguments == nullptr) { // LOCV_EXCL_BR_LINE
        return nullptr;
    }

    TriggerTimer(env, periodicTask, period);
    return nullptr;
}

void TaskPool::TriggerTimer(napi_env env, Task* task, int32_t period)
{
    HILOG_INFO("taskpool::TriggerTimer tId %{public}s", std::to_string(task->taskId_).c_str());
    uv_loop_t* loop = NapiHelper::GetLibUV(env);
    task->timer_ = new uv_timer_t;
    uv_timer_init(loop, task->timer_);
    task->timer_->data = task;
    uv_update_time(loop);
    uv_timer_start(task->timer_, PeriodicTaskCallback, period, period);
    NativeEngine* engine = reinterpret_cast<NativeEngine*>(env);
    if (engine->IsMainThread()) {
        uv_async_send(&loop->wq_async);
    }
}

bool TaskPool::CheckDelayedParams(napi_env env, napi_callback_info cbinfo, uint32_t &priority, int32_t &delayTime,
                                  Task* &task)
{
    size_t argc = 3; // 3: delayTime, task and priority
    napi_value args[3]; // 3: delayTime, task and priority
    napi_get_cb_info(env, cbinfo, &argc, args, nullptr, nullptr);
    if (argc < 2 || argc > 3) { // 2: delayTime and task 3: delayTime, task and priority
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
            "the number of executeDelayed's params must be two or three.");
        return false;
    }

    if (!NapiHelper::IsNumber(env, args[0])) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
            "the type of executeDelayed's first param must be number.");
        return false;
    }

    if (!NapiHelper::IsObject(env, args[1])) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
            "the type of the second param must be object.");
        return false;
    }

    delayTime = NapiHelper::GetInt32Value(env, args[0]);
    if (delayTime < 0) {
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_DELAY_TIME_ERROR, "The delayTime is less than zero");
        return false;
    }

    if (argc > 2) { // 2: the params might have priority
        if (!NapiHelper::IsNumber(env, args[2])) {
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
                "the type of executeDelayed's third param must be number.");
            return false;
        }
        priority = NapiHelper::GetUint32Value(env, args[2]); // 2: get task priority
        if (priority >= Priority::NUMBER) {
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "executeDelayed's priority value is error.");
            return false;
        }
    }

    napi_unwrap(env, args[1], reinterpret_cast<void**>(&task));
    if (task == nullptr) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "the type of executeDelayed's second param must be task");
        return false;
    }
    if (!task->CanExecuteDelayed(env)) {
        return false;
    }
    return true;
}

bool TaskPool::CheckPeriodicallyParams(napi_env env, napi_callback_info cbinfo, int32_t &period,
                                       uint32_t &priority, Task* &periodicTask)
{
    size_t argc = 3; // 3 : period, task, priority
    napi_value args[3]; // 3 : period, task, priority
    napi_get_cb_info(env, cbinfo, &argc, args, nullptr, nullptr);
    if (argc < 2 || argc > 3) { // 2 : period, task and 3 : period, task, priority
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
            "the number of executePeriodically's params must be two or three.");
        return false;
    }
    if (!NapiHelper::IsNumber(env, args[0])) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
            "the type of executePeriodically's first param must be number.");
        return false;
    }
    period = NapiHelper::GetInt32Value(env, args[0]);
    if (period < 0) {
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_DELAY_TIME_ERROR, "The period value is less than zero.");
        return false;
    }
    if (!NapiHelper::IsObject(env, args[1])) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
            "the type of executePeriodically's second param must be task.");
        return false;
    }

    if (argc >= 3) { // 3 : third param maybe priority
        if (!NapiHelper::IsNumber(env, args[2])) { // 2 : priority
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
                "the executePeriodically's third param must be priority.");
            return false;
        }
        priority = NapiHelper::GetUint32Value(env, args[2]); // 2 : priority
        if (priority >= Priority::NUMBER) {
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
                "the value of executePeriodically's priority is invalid.");
            return false;
        }
    }

    napi_unwrap(env, args[1], reinterpret_cast<void**>(&periodicTask));
    if (periodicTask == nullptr) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
            "the type of executePeriodically's second param must be task.");
        return false;
    }

    return true;
}

void TaskPool::RecordTaskResultLog(Task* task, napi_status status, napi_value& napiTaskResult, bool& isCancel)
{
    // tag for trace parse: Task PerformTask End
    std::string strTrace = "Task PerformTask End: taskId : " + std::to_string(task->taskId_);
    std::string taskLog = "Task PerformTask End: " + std::to_string(task->taskId_)
                        + ", " + ConcurrentHelper::GetCurrentTimeStampWithMS();
    if (task->taskState_ == ExecuteState::CANCELED) {
        strTrace += ", performResult : IsCanceled";
        napiTaskResult = task->IsAsyncRunnerTask() ? TaskManager::GetInstance().CancelError(task->env_,
            ErrorHelper::ERR_ASYNCRUNNER_TASK_CANCELED, nullptr, napiTaskResult, task->success_) :
            TaskManager::GetInstance().CancelError(task->env_, 0, nullptr, napiTaskResult, task->success_);
        isCancel = true;
        taskLog += ", IsCanceled";
    } else if (status != napi_ok) {
        strTrace += ", performResult : DeserializeFailed";
        taskLog += ", DeserializeFailed";
    } else if (task->IsTimeoutState()) {
        strTrace += ", performResult : Timeout";
        taskLog += ", Timeout";
    } else if (task->success_) {
        strTrace += ", performResult : Successful";
    } else { // LCOV_EXCL_BR_LINE
        strTrace += ", performResult : Unsuccessful";
        taskLog += ", Unsuccessful";
    }
    HITRACE_HELPER_METER_NAME(strTrace);
    TaskManager::GetInstance().PushLog(taskLog);
}

napi_value TaskPool::GetTask(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    if (argc < 1 || argc > 2) { // 1: taskId, 2: taskId and name
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "the number of getTask's params must be one or two.");
        return nullptr;
    }
    napi_value* args = new napi_value[argc];
    ObjectScope<napi_value> scope(args, true);
    napi_get_cb_info(env, cbinfo, &argc, args, nullptr, nullptr);
    if (!NapiHelper::IsNumber(env, args[0])) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "the type of getTask's first param must be number.");
        return nullptr;
    }
    napi_value undefined = NapiHelper::GetUndefinedValue(env);
    std::string name = "";
    bool checkName = false;
    // If the second parameter passed in is undefined, no validation is required.
    if (argc > 1 && NapiHelper::IsNotUndefined(env, args[1])) {
        if (!NapiHelper::IsString(env, args[1])) {
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
                "the type of getTask's second param must be string.");
            return nullptr;
        }
        name = NapiHelper::GetString(env, args[1]);
        checkName = true;
    }
    uint32_t taskId = NapiHelper::GetUint32Value(env, args[0]);
    Task* task = TaskManager::GetInstance().GetTask(taskId);
    if (task == nullptr || env != task->GetEnv() || !task->IsValid()) {
        // if task is nullptr, return undefined;
        // if task isn't valid, return undefined;
        // If current env does not match the env when the task was created, return undefined.
        return undefined;
    }
    if (checkName && name != task->name_) {
        // If need check name, but name and taskName are not equal, return undefined.
        return undefined;
    }
    napi_value result = nullptr;
    napi_get_reference_value(env, task->taskRef_, &result);
    return result;
}

void Taskpool::TriggerTimeoutTimer(napi_env env, Task* task)
{
    if (!task->IsTimeoutTask()) {
        return;
    }
    TaskMessage* message = new TaskMessage();
    message->taskId = task->taskId_;
    uv_loop_t* loop = NapiHelper::GetLibUV(env);
    uv_update_time(loop);
    task->timer_ = new uv_timer_t;
    uv_timer_init(loop, task->timer_);
    task->timer_->data = message;
    uv_timer_start(task->timer_, reinterpret_cast<uv_timer_cb>(TimeoutCallback), task->timeout_, 0);
    NativeEngine* engine = reinterpret_cast<NativeEngine*>(env);
    if (engine->IsMainThread()) {
        uv_async_send(&loop->wq_async);
    }
}

void Taskpool::TimeoutCallback(uv_timer_t* handle)
{
    TaskMessage* message = reinterpret_cast<TaskMessage*>(handle->data);
    auto task = TaskManager::GetInstance().GetTask(message->taskId);
    delete message;
    message = nullptr;
    if (task == nullptr || !task->IsValid()) {
        HILOG_ERROR("taskpool:: the task is nullptr or invalid");
        return;
    }
    ExecuteState state = task->taskState_;
    if (!task->UpdateTaskStateToTimeout()) {
        HILOG_ERROR("taskpool:: the task update state to timeout fail");
        return;
    }

    napi_status status = napi_ok;
    HandleScope scope(task->GetEnv(), status);
    if (status != napi_ok) {
        HILOG_ERROR("taskpool:: napi_open_handle_scope failed");
        return;
    }
    napi_value error = ErrorHelper::NewError(task->GetEnv(), ErrorHelper::ERR_TASK_TIMEOUT);
    if (state == ExecuteState::RUNNING) {
        napi_reject_deferred(task->GetEnv(), task->currentTaskInfo_->deferred, error);
        return;
    }
    napi_deferred deferred = nullptr;
    {
        std::lock_guard<std::recursive_mutex> lock(task->taskMutex_);
        if (task->currentTaskInfo_ != nullptr &&
            TaskManager::GetInstance().EraseWaitingTaskId(task->GetTaskId(), task->currentTaskInfo_->priority)) {
            reinterpret_cast<NativeEngine*>(task->GetEnv())->DecreaseSubEnvCounter();
            task->DecreaseTaskLifecycleCount();
            TaskManager::GetInstance().DecreaseSendDataRefCount(task->GetEnv(), task->GetTaskId());
            deferred = task->currentTaskInfo_->deferred;
            napi_reference_unref(task->GetEnv(), task->taskRef_, nullptr);
            delete task->currentTaskInfo_;
            task->currentTaskInfo_ = nullptr;
        }
    }
    if (deferred != nullptr) {
        napi_reject_deferred(task->GetEnv(), deferred, error);
    }
}

std::pair<uint32_t, uint32_t> Taskpool::GetExecuteParams(napi_env env, napi_value arg)
{
    napi_value priorityValue = nullptr;
    uint32_t timeout = 0;
    uint32_t priority = Priority::DEFAULT;
    if (NapiHelper::IsObject(env, arg)) {
        priorityValue = NapiHelper::GetNameProperty(env, arg, "priority");
        napi_value timeoutValue = NapiHelper::GetNameProperty(env, arg, "timeout");
        if (NapiHelper::IsNotUndefined(env, timeoutValue)) {
            if (!NapiHelper::IsNumber(env, timeoutValue)) {
                ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "the type of timeout must be number.");
                return std::make_pair(0, Priority::NUMBER);
            }
            timeout = NapiHelper::GetUint32Value(env, timeoutValue);
        }
    } else {
        priorityValue = arg;
    }
    if (NapiHelper::IsNotUndefined(env, priorityValue)) {
        if (!NapiHelper::IsNumber(env, priorityValue)) {
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "the type of priority must be number.");
            return std::make_pair(0, Priority::NUMBER);
        }
        priority = NapiHelper::GetUint32Value(env, priorityValue);
        if (priority >= Priority::NUMBER) {
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "priority value is error");
            return std::make_pair(0, Priority::NUMBER);
        }
    }
    return std::make_pair(priority, timeout);
}
} // namespace Commonlibrary::Concurrent::TaskPoolModule
