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

#include <cinttypes>

#include "helper/error_helper.h"
#include "helper/hitrace_helper.h"
#include "helper/napi_helper.h"
#include "helper/object_helper.h"
#include "message_queue.h"
#include "task_manager.h"
#include "tools/log.h"
#include "worker.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
using namespace Commonlibrary::Concurrent::Common::Helper;

napi_value TaskPool::InitTaskPool(napi_env env, napi_value exports)
{
    HILOG_INFO("taskpool:: Import taskpool");
    HITRACE_HELPER_METER_NAME(__PRETTY_FUNCTION__);
    napi_value taskClass = nullptr;
    napi_define_class(env, "Task", NAPI_AUTO_LENGTH, Task::TaskConstructor, nullptr, 0, nullptr, &taskClass);
    napi_value longTaskClass = nullptr;
    napi_define_class(env, "LongTask", NAPI_AUTO_LENGTH, Task::LongTaskConstructor,
                      nullptr, 0, nullptr, &longTaskClass);
    napi_value isCanceledFunc = nullptr;
    napi_create_function(env, "isCanceled", NAPI_AUTO_LENGTH, Task::IsCanceled, NULL, &isCanceledFunc);
    napi_set_named_property(env, taskClass, "isCanceled", isCanceledFunc);
    napi_value sendDataFunc = nullptr;
    napi_create_function(env, "sendData", NAPI_AUTO_LENGTH, Task::SendData, NULL, &sendDataFunc);
    napi_set_named_property(env, taskClass, "sendData", sendDataFunc);
    napi_value taskGroupClass = nullptr;
    napi_define_class(env, "TaskGroup", NAPI_AUTO_LENGTH, TaskGroup::TaskGroupConstructor, nullptr, 0, nullptr,
                      &taskGroupClass);
    napi_value seqRunnerClass = nullptr;
    napi_define_class(env, "SequenceRunner", NAPI_AUTO_LENGTH, SequenceRunner::SeqRunnerConstructor,
                      nullptr, 0, nullptr, &seqRunnerClass);

    // define priority
    napi_value priorityObj = NapiHelper::CreateObject(env);
    napi_value highPriority = NapiHelper::CreateUint32(env, Priority::HIGH);
    napi_value mediumPriority = NapiHelper::CreateUint32(env, Priority::MEDIUM);
    napi_value lowPriority = NapiHelper::CreateUint32(env, Priority::LOW);
    napi_property_descriptor exportPriority[] = {
        DECLARE_NAPI_PROPERTY("HIGH", highPriority),
        DECLARE_NAPI_PROPERTY("MEDIUM", mediumPriority),
        DECLARE_NAPI_PROPERTY("LOW", lowPriority),
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
        DECLARE_NAPI_PROPERTY("TaskGroup", taskGroupClass),
        DECLARE_NAPI_PROPERTY("SequenceRunner", seqRunnerClass),
        DECLARE_NAPI_PROPERTY("Priority", priorityObj),
        DECLARE_NAPI_PROPERTY("State", stateObj),
        DECLARE_NAPI_FUNCTION("execute", Execute),
        DECLARE_NAPI_FUNCTION("executeDelayed", ExecuteDelayed),
        DECLARE_NAPI_FUNCTION("cancel", Cancel),
        DECLARE_NAPI_FUNCTION("getTaskPoolInfo", GetTaskPoolInfo),
        DECLARE_NAPI_FUNCTION("terminateTask", TerminateTask),
        DECLARE_NAPI_FUNCTION("isConcurrent", IsConcurrent),
    };
    napi_define_properties(env, exports, sizeof(properties) / sizeof(properties[0]), properties);

    TaskManager::GetInstance().InitTaskManager(env);
    return exports;
}

// ---------------------------------- SendData ---------------------------------------
void TaskPool::ExecuteCallback(const uv_async_t* req)
{
    auto* msgQueue = TaskManager::GetInstance().GetMessageQueue(req);
    if (msgQueue == nullptr) {
        HILOG_ERROR("taskpool:: msgQueue is nullptr");
        return;
    }
    ExecuteCallbackInner(*msgQueue);
}

void TaskPool::ExecuteCallbackInner(MsgQueue& msgQueue)
{
    while (!msgQueue.IsEmpty()) {
        auto resultInfo = msgQueue.DeQueue();
        ObjectScope<TaskResultInfo> resultInfoScope(resultInfo, false);
        napi_status status = napi_ok;
        CallbackScope callbackScope(resultInfo->hostEnv, resultInfo->workerEnv, resultInfo->taskId, status);
        if (status != napi_ok) {
            HILOG_ERROR("napi_open_handle_scope failed");
            return;
        }
        auto env = resultInfo->hostEnv;
        auto callbackInfo = TaskManager::GetInstance().GetCallbackInfo(resultInfo->taskId);
        if (callbackInfo == nullptr) {
            HILOG_ERROR("taskpool:: the callback in SendData is not registered on the host side");
            ErrorHelper::ThrowError(env, ErrorHelper::ERR_NOT_REGISTERED);
            continue;
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
            continue;
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
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: the number of the params must be one");
        return nullptr;
    }
    if (!NapiHelper::IsObject(env, args[0])) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: the type of the params must be object");
        return nullptr;
    }
    napi_value napiTaskId = NapiHelper::GetNameProperty(env, args[0], TASKID_STR);
    if (napiTaskId == nullptr) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: the type of the params must be task");
        return nullptr;
    }
    uint64_t taskId = NapiHelper::GetUint64Value(env, napiTaskId);
    TaskManager::GetInstance().TerminateTask(taskId);
    return nullptr;
}

napi_value TaskPool::Execute(napi_env env, napi_callback_info cbinfo)
{
    HITRACE_HELPER_METER_NAME(__PRETTY_FUNCTION__);
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    if (argc < 1) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: the number of params must be at least one");
        return nullptr;
    }
    napi_value* args = new napi_value[argc];
    ObjectScope<napi_value> scope(args, true);
    napi_get_cb_info(env, cbinfo, &argc, args, nullptr, nullptr);
    napi_valuetype type = napi_undefined;
    napi_typeof(env, args[0], &type);
    if (type == napi_object) {
        uint32_t priority = Priority::DEFAULT; // DEFAULT priority is MEDIUM
        if (argc > 1) {
            if (!NapiHelper::IsNumber(env, args[1])) {
                ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: priority type is error");
                return nullptr;
            }
            priority = NapiHelper::GetUint32Value(env, args[1]);
            if (priority >= Priority::NUMBER) {
                ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: priority value is error");
                return nullptr;
            }
        }
        if (NapiHelper::HasNameProperty(env, args[0], GROUP_ID_STR)) {
            return ExecuteGroup(env, args[0], static_cast<Priority>(priority));
        }
        Task* task = nullptr;
        napi_unwrap(env, args[0], reinterpret_cast<void**>(&task));
        if (task == nullptr) {
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: the first param must be task");
            return nullptr;
        }
        if (!task->CanExecute(env)) {
            return nullptr;
        }
        napi_value promise = task->GetTaskInfoPromise(env, args[0], TaskType::COMMON_TASK,
                                                      static_cast<Priority>(priority));
        if (promise == nullptr) {
            return nullptr;
        }
        ExecuteTask(env, task, static_cast<Priority>(priority));
        return promise;
    }
    if (type != napi_function) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: first param must be object or function");
        return nullptr;
    }
    Task* task = Task::GenerateFunctionTask(env, args[0], args + 1, argc - 1, TaskType::FUNCTION_TASK);
    if (task == nullptr) {
        HILOG_ERROR("taskpool:: GenerateFunctionTask failed");
        return nullptr;
    }
    TaskManager::GetInstance().StoreTask(task->taskId_, task);
    napi_value promise = NapiHelper::CreatePromise(env, &task->currentTaskInfo_->deferred);
    ExecuteTask(env, task);
    return promise;
}

void TaskPool::DelayTask(uv_timer_t* handle)
{
    TaskMessage *taskMessage = static_cast<TaskMessage *>(handle->data);
    auto task = TaskManager::GetInstance().GetTask(taskMessage->taskId);
    if (task == nullptr) {
        HILOG_DEBUG("taskpool:: task is nullptr");
    } else if (task->taskState_ == ExecuteState::CANCELED) {
        HILOG_DEBUG("taskpool:: DelayTask task has been canceled");
        napi_value error = ErrorHelper::NewError(task->env_, 0, "taskpool:: task has been canceled");
        napi_reject_deferred(task->env_, taskMessage->deferred, error);
    } else {
        TaskManager::GetInstance().IncreaseRefCount(taskMessage->taskId);
        task->IncreaseRefCount();
        napi_value napiTask = NapiHelper::GetReferenceValue(task->env_, task->taskRef_);
        TaskInfo* taskInfo = task->GetTaskInfo(task->env_, napiTask, taskMessage->priority);
        taskInfo->deferred = taskMessage->deferred;
        if (task->taskState_ == ExecuteState::DELAYED || task->taskState_ == ExecuteState::FINISHED) {
            task->taskState_ = ExecuteState::WAITING;
            TaskManager::GetInstance().EnqueueTaskId(taskMessage->taskId, Priority(taskMessage->priority));
        }
        TaskManager::GetInstance().TryTriggerExpand();
    }
    uv_timer_stop(handle);
    uv_close(reinterpret_cast<uv_handle_t*>(handle), [](uv_handle_t* handle) {
        if (handle != nullptr) {
            delete reinterpret_cast<uv_timer_t*>(handle);
            handle = nullptr;
        }
    });
    delete taskMessage;
    taskMessage = nullptr;
}

napi_value TaskPool::ExecuteDelayed(napi_env env, napi_callback_info cbinfo)
{
    HITRACE_HELPER_METER_NAME(__PRETTY_FUNCTION__);
    size_t argc = 3; // 3: delayTime, task and priority
    napi_value args[3]; // 3: delayTime, task and priority
    napi_get_cb_info(env, cbinfo, &argc, args, nullptr, nullptr);
    if (argc < 2 || argc > 3) { // 2: delayTime and task 3: delayTime, task and priority
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: the number of params must be two or three");
        return nullptr;
    }
    if (!NapiHelper::IsNumber(env, args[0])) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: delayTime type is error");
        return nullptr;
    }
    int32_t delayTime = NapiHelper::GetInt32Value(env, args[0]);
    if (delayTime < 0) {
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_DELAY_TIME_ERROR);
        return nullptr;
    }
    if (!NapiHelper::IsObject(env, args[1])) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: task type is error");
        return nullptr;
    }
    Task* task = nullptr;
    napi_unwrap(env, args[1], reinterpret_cast<void**>(&task));
    if (task == nullptr) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: the type of second param must be task");
        return nullptr;
    }
    if (!task->CanExecuteDelayed(env)) {
        return nullptr;
    }

    uint32_t priority = Priority::DEFAULT; // DEFAULT priority is MEDIUM
    if (argc > 2) { // 2: the params might have priority
        if (!NapiHelper::IsNumber(env, args[2])) {
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: priority type is error");
            return nullptr;
        }
        priority = NapiHelper::GetUint32Value(env, args[2]); // 2: get task priority
        if (priority >= Priority::NUMBER) {
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: priority value is error");
            return nullptr;
        }
    }
    if (!task->IsInitialized() || task->taskState_ == ExecuteState::CANCELED ||
        task->taskState_ == ExecuteState::FINISHED) {
        task->taskState_ = ExecuteState::DELAYED;
    }
    task->UpdateTaskType(TaskType::COMMON_TASK);
    uv_loop_t* loop = NapiHelper::GetLibUV(env);
    uv_update_time(loop);
    uv_timer_t* timer = new uv_timer_t;
    uv_timer_init(loop, timer);
    TaskMessage *taskMessage = new TaskMessage();
    taskMessage->priority = static_cast<Priority>(priority);
    taskMessage->taskId = task->taskId_;
    napi_value promise = NapiHelper::CreatePromise(env, &taskMessage->deferred);
    timer->data = taskMessage;

    std::string strTrace = "ExecuteDelayed: taskId: " + std::to_string(task->taskId_);
    strTrace += ", priority: " + std::to_string(priority);
    strTrace += ", delayTime " + std::to_string(delayTime);
    HITRACE_HELPER_METER_NAME(strTrace);

    uv_timer_start(timer, reinterpret_cast<uv_timer_cb>(DelayTask), delayTime, 0);
    NativeEngine* engine = reinterpret_cast<NativeEngine*>(env);
    if (engine->IsMainThread()) {
        uv_async_send(&loop->wq_async);
    } else {
        uv_work_t *work = new uv_work_t;
        uv_queue_work_with_qos(loop, work, [](uv_work_t *) {},
                               [](uv_work_t *work, int32_t) {delete work; }, uv_qos_user_initiated);
    }
    return promise;
}

napi_value TaskPool::ExecuteGroup(napi_env env, napi_value napiTaskGroup, Priority priority)
{
    napi_value napiGroupId = NapiHelper::GetNameProperty(env, napiTaskGroup, GROUP_ID_STR);
    uint64_t groupId = NapiHelper::GetUint64Value(env, napiGroupId);
    auto taskGroup = TaskGroupManager::GetInstance().GetTaskGroup(groupId);
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
    {
        std::lock_guard<std::recursive_mutex> lock(taskGroup->taskGroupMutex_);
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

void TaskPool::HandleTaskResult(const uv_async_t* req)
{
    HITRACE_HELPER_METER_NAME(__PRETTY_FUNCTION__);
    auto task = static_cast<Task*>(req->data);
    if (task == nullptr) {
        HILOG_FATAL("taskpool:: HandleTaskResult task is null");
        return;
    }
    napi_handle_scope scope = nullptr;
    NAPI_CALL_RETURN_VOID(task->env_, napi_open_handle_scope(task->env_, &scope));
    napi_value napiTaskResult = nullptr;
    napi_status status = napi_deserialize(task->env_, task->result_, &napiTaskResult);
    napi_delete_serialization_data(task->env_, task->result_);

    // tag for trace parse: Task PerformTask End
    std::string strTrace = "Task PerformTask End: taskId : " + std::to_string(task->taskId_);
    if (task->taskState_ == ExecuteState::CANCELED) {
        strTrace += ", performResult : IsCanceled";
        napiTaskResult = ErrorHelper::NewError(task->env_, 0, "taskpool:: task has been canceled");
    } else if (status != napi_ok) {
        HILOG_ERROR("taskpool: failed to deserialize result");
        strTrace += ", performResult : DeserializeFailed";
    } else if (task->success_) {
        strTrace += ", performResult : Successful";
    } else {
        strTrace += ", performResult : Unsuccessful";
    }
    HITRACE_HELPER_METER_NAME(strTrace);

    if (napiTaskResult == nullptr) {
        napi_get_undefined(task->env_, &napiTaskResult);
    }
    reinterpret_cast<NativeEngine*>(task->env_)->DecreaseSubEnvCounter();
    bool success = ((status == napi_ok) && (task->taskState_ != ExecuteState::CANCELED)) && (task->success_);
    if (!task->IsGroupTask()) {
        if (success) {
            napi_resolve_deferred(task->env_, task->currentTaskInfo_->deferred, napiTaskResult);
            if (task->onExecutionSucceededCallBackInfo != nullptr) {
                task->ExecuteListenerCallback(task->onExecutionSucceededCallBackInfo);
            }
        } else {
            napi_reject_deferred(task->env_, task->currentTaskInfo_->deferred, napiTaskResult);
            if ((task->onExecutionFailedCallBackInfo != nullptr) && (napiTaskResult != nullptr)) {
                task->onExecutionFailedCallBackInfo->taskError_ = napiTaskResult;
                task->ExecuteListenerCallback(task->onExecutionFailedCallBackInfo);
            }
        }
    } else {
        UpdateGroupInfoByResult(task->env_, task, napiTaskResult, success);
    }
    NAPI_CALL_RETURN_VOID(task->env_, napi_close_handle_scope(task->env_, scope));
    TriggerTask(task);
}

void TaskPool::TriggerTask(Task* task)
{
    if (task->IsGroupTask()) {
        return;
    }
    TaskManager::GetInstance().DecreaseRefCount(task->env_, task->taskId_);
    task->taskState_ = ExecuteState::FINISHED;
    // seqRunnerTask will trigger the next
    if (task->IsSeqRunnerTask()) {
        if (!TaskGroupManager::GetInstance().TriggerSeqRunner(task->env_, task)) {
            HILOG_ERROR("seqRunner:: task %{public}" PRIu64 " trigger in seqRunner %{public}" PRIu64 " failed",
                        task->taskId_, task->seqRunnerId_);
        }
    } else if (task->IsCommonTask()) {
        task->NotifyPendingTask();
    }
    if (!task->IsFunctionTask()) {
        napi_reference_unref(task->env_, task->taskRef_, nullptr);
        return;
    }
    TaskManager::GetInstance().RemoveTask(task->taskId_);
    delete task;
}

void TaskPool::UpdateGroupInfoByResult(napi_env env, Task* task, napi_value res, bool success)
{
    TaskManager::GetInstance().DecreaseRefCount(task->env_, task->taskId_);
    task->taskState_ = ExecuteState::FINISHED;
    napi_reference_unref(env, task->taskRef_, nullptr);
    if (task->IsGroupCommonTask()) {
        delete task->currentTaskInfo_;
        task->currentTaskInfo_ = nullptr;
    }
    TaskGroup* taskGroup = TaskGroupManager::GetInstance().GetTaskGroup(task->groupId_);
    if (taskGroup == nullptr) {
        HILOG_DEBUG("taskpool:: taskGroup has been released");
        return;
    }
    if (taskGroup->currentGroupInfo_ == nullptr) {
        HILOG_DEBUG("taskpool:: taskGroup has been canceled");
        return;
    }
    uint32_t index = taskGroup->GetTaskIndex(task->taskId_);
    auto groupInfo = taskGroup->currentGroupInfo_;
    if (success) {
        // Update res at resArr
        napi_ref arrRef = groupInfo->resArr;
        napi_value resArr = NapiHelper::GetReferenceValue(env, arrRef);
        napi_set_element(env, resArr, index, res);

        groupInfo->finishedTask++;
        if (groupInfo->finishedTask < taskGroup->taskNum_) {
            return;
        }
        napi_resolve_deferred(env, groupInfo->deferred, resArr);
    } else {
        napi_value error = ErrorHelper::NewError(env, 0, "taskpool:: taskGroup has exception or has been canceled");
        napi_reject_deferred(env, groupInfo->deferred, error);
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
    task->IncreaseRefCount();
    TaskManager::GetInstance().IncreaseRefCount(task->taskId_);
    if (task->IsFunctionTask() || (task->taskState_ != ExecuteState::WAITING &&
        task->taskState_ != ExecuteState::RUNNING)) {
        task->taskState_ = ExecuteState::WAITING;
        TaskManager::GetInstance().EnqueueTaskId(task->taskId_, priority);
    }
    TaskManager::GetInstance().TryTriggerExpand();
}

napi_value TaskPool::Cancel(napi_env env, napi_callback_info cbinfo)
{
    HITRACE_HELPER_METER_NAME(__PRETTY_FUNCTION__);
    size_t argc = 1;
    napi_value args[1];
    napi_get_cb_info(env, cbinfo, &argc, args, nullptr, nullptr);
    if (argc < 1) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: the number of the params must be one");
        return nullptr;
    }

    if (!NapiHelper::IsObject(env, args[0])) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: the type of the params must be object");
        return nullptr;
    }

    if (!NapiHelper::HasNameProperty(env, args[0], GROUP_ID_STR)) {
        napi_value napiTaskId = NapiHelper::GetNameProperty(env, args[0], TASKID_STR);
        if (napiTaskId == nullptr) {
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: the type of the params must be task");
            return nullptr;
        }
        uint64_t taskId = NapiHelper::GetUint64Value(env, napiTaskId);
        TaskManager::GetInstance().CancelTask(env, taskId);
    } else {
        napi_value napiGroupId = NapiHelper::GetNameProperty(env, args[0], GROUP_ID_STR);
        if (napiGroupId == nullptr) {
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
                                    "taskpool:: the type of the params must be taskGroup");
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
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: the number of the params must be one");
        return nullptr;
    }

    if (!NapiHelper::IsFunction(env, args[0])) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: the type of the params must be function");
        return nullptr;
    }

    bool isConcurrent = false;
    isConcurrent = NapiHelper::IsConcurrentFunction(env, args[0]);
    return NapiHelper::CreateBooleanValue(env, isConcurrent);
}
} // namespace Commonlibrary::Concurrent::TaskPoolModule