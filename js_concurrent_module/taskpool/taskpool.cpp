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

#include "helper/error_helper.h"
#include "helper/hitrace_helper.h"
#include "helper/napi_helper.h"
#include "helper/object_helper.h"
#include "message_queue.h"
#include "task_manager.h"
#include "utils/log.h"
#include "worker.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
using namespace Commonlibrary::Concurrent::Common::Helper;

napi_value TaskPool::InitTaskPool(napi_env env, napi_value exports)
{
    HILOG_INFO("taskpool:: Import taskpool");
    HITRACE_HELPER_METER_NAME(__PRETTY_FUNCTION__);
    napi_value taskClass = nullptr;
    napi_define_class(env, "Task", NAPI_AUTO_LENGTH, Task::TaskConstructor, nullptr, 0, nullptr, &taskClass);
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

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_PROPERTY("Task", taskClass),
        DECLARE_NAPI_PROPERTY("TaskGroup", taskGroupClass),
        DECLARE_NAPI_PROPERTY("SequenceRunner", seqRunnerClass),
        DECLARE_NAPI_PROPERTY("Priority", priorityObj),
        DECLARE_NAPI_FUNCTION("execute", Execute),
        DECLARE_NAPI_FUNCTION("executeDelayed", ExecuteDelayed),
        DECLARE_NAPI_FUNCTION("cancel", Cancel),
        DECLARE_NAPI_FUNCTION("getTaskPoolInfo", GetTaskPoolInfo),
    };
    napi_define_properties(env, exports, sizeof(properties) / sizeof(properties[0]), properties);

    TaskManager::GetInstance().InitTaskManager(env);
    return exports;
}

void TaskPool::ExecuteCallback(uv_async_t* req)
{
    auto worker = static_cast<Worker*>(req->data);
    while (!worker->IsQueueEmpty()) {
        auto resultInfo = worker->Dequeue();
        ObjectScope<TaskResultInfo> resultInfoScope(resultInfo, false);
        napi_status status = napi_ok;
        CallbackScope callbackScope(resultInfo->hostEnv, resultInfo->taskId, status);
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
            HILOG_ERROR("taskpool:: failed to serialize function in SendData");
            std::string errMessage = "taskpool:: failed to serialize function";
            ErrorHelper::ThrowError(env, ErrorHelper::ERR_WORKER_SERIALIZATION, errMessage.c_str());
            continue;
        }
        uint32_t argsNum = NapiHelper::GetArrayLength(env, args);
        napi_value argsArray[argsNum];
        napi_value val;
        for (size_t i = 0; i < argsNum; i++) {
            napi_get_element(env, args, i, &val);
            argsArray[i] = val;
        }
        napi_call_function(env, NapiHelper::GetGlobalObject(env), func, argsNum, argsArray, &result);
        if (NapiHelper::IsExceptionPending(env)) {
            napi_value exception = nullptr;
            napi_get_and_clear_last_exception(env, &exception);
            HILOG_ERROR("taskpool:: an exception has occurred napi_call_function");
        }
    }
}

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

napi_value TaskPool::Execute(napi_env env, napi_callback_info cbinfo)
{
    HITRACE_HELPER_METER_NAME(__PRETTY_FUNCTION__);
    // check the argc
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    if (argc < 1) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: the number of params must be at least one");
        return nullptr;
    }

    // check the first param is object or func
    napi_value* args = new napi_value[argc];
    ObjectScope<napi_value> scope(args, true);
    napi_get_cb_info(env, cbinfo, &argc, args, nullptr, nullptr);
    napi_valuetype type;
    napi_typeof(env, args[0], &type);

    uint32_t priority = Priority::DEFAULT; // DEFAULT priority is MEDIUM
    if (type == napi_object) {
        // Get execution priority
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
            return ExecuteGroup(env, args[0], Priority(priority));
        }
        napi_value napiTaskId = NapiHelper::GetNameProperty(env, args[0], TASKID_STR);
        uint32_t taskId = NapiHelper::GetUint32Value(env, napiTaskId);
        std::string errMessage = "";
        if (TaskManager::GetInstance().IsGroupTask(taskId)) {
            errMessage = "taskpool:: groupTask cannot execute outside";
            HILOG_ERROR("%{public}s", errMessage.c_str());
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, errMessage.c_str());
            return nullptr;
        }
        if (TaskManager::GetInstance().IsSeqRunnerTask(taskId)) {
            errMessage = "taskpool:: seqRunnerTask cannot execute outside";
            HILOG_ERROR("%{public}s", errMessage.c_str());
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, errMessage.c_str());
            return nullptr;
        }
        uint32_t executeId = TaskManager::GetInstance().GenerateExecuteId();
        TaskInfo* taskInfo = TaskManager::GetInstance().GenerateTaskInfoFromTask(env, args[0], executeId);
        if (taskInfo == nullptr) {
            HILOG_ERROR("taskpool::ExecuteTask taskInfo is nullptr");
            return nullptr;
        }
        napi_value promise = NapiHelper::CreatePromise(env, &taskInfo->deferred);
        TaskManager::GetInstance().StoreRunningInfo(taskInfo->taskId, executeId);
        ExecuteFunction(env, taskInfo, Priority(priority));
        return promise;
    }
    if (type != napi_function) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: first param must be object or function");
        return nullptr;
    }
    // Type is napi_function, execute from func directly
    napi_value argsArray;
    napi_create_array_with_length(env, argc - 1, &argsArray);
    for (size_t i = 0; i < argc - 1; i++) {
        napi_set_element(env, argsArray, i, args[i + 1]);
    }
    uint32_t executeId = TaskManager::GetInstance().GenerateExecuteId();
    // Set task id to 0 when execute from func directly
    napi_value taskName = NapiHelper::CreateEmptyString(env);
    napi_value undefined = NapiHelper::GetUndefinedValue(env);
    TaskInfo* taskInfo = TaskManager::GetInstance().GenerateTaskInfo(env, args[0], argsArray, taskName, 0, executeId,
                                                                     undefined, undefined);
    if (taskInfo == nullptr) {
        HILOG_ERROR("taskpool::ExecuteFunction taskInfo is nullptr");
        return nullptr;
    }
    napi_value promise = NapiHelper::CreatePromise(env, &taskInfo->deferred);
    TaskManager::GetInstance().StoreRunningInfo(0, executeId);
    ExecuteFunction(env, taskInfo);
    return promise;
}

void TaskPool::DelayTask(uv_timer_t* handle)
{
    TaskMessage *taskMessage = reinterpret_cast<TaskMessage *>(handle->data);
    if (!TaskManager::GetInstance().IsCanceledByExecuteId(taskMessage->executeId)) {
        TaskManager::GetInstance().IncreaseRefCount(taskMessage->taskId);
        TaskManager::GetInstance().EnqueueExecuteId(taskMessage->executeId, Priority(taskMessage->priority));
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
    size_t argc = 4;
    napi_value args[4];
    napi_get_cb_info(env, cbinfo, &argc, args, nullptr, nullptr);
    if (argc < 2 || argc > 3) { // 2: delayTime and task 3: delayTime、 task and priority
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: the number of params must be two or three");
        return nullptr;
    }

    // check the first param is number
    if (!NapiHelper::IsNumber(env, args[0])) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: delayTime type is error");
        return nullptr;
    }

    int32_t delayTime = NapiHelper::GetInt32Value(env, args[0]);
    if (delayTime < 0) {
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_DELAY_TIME_ERROR);
        return nullptr;
    }

    // check the second param is object
    napi_valuetype type;
    napi_typeof(env, args[1], &type);
    if (type != napi_object) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: task type is error");
        return nullptr;
    }

    // get execution priority
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

    uint32_t executeId = TaskManager::GetInstance().GenerateExecuteId();
    TaskInfo* taskInfo = TaskManager::GetInstance().GenerateTaskInfoFromTask(env, args[1], executeId);
    if (taskInfo == nullptr) {
        HILOG_ERROR("taskpool::ExecuteTask taskInfo is nullptr");
        return nullptr;
    }
    TaskManager::GetInstance().StoreRunningInfo(taskInfo->taskId, executeId);
    TaskManager::GetInstance().AddExecuteState(executeId);
    uv_loop_t* loop = uv_default_loop();
    uv_update_time(loop);
    uv_timer_t* timer = new uv_timer_t;
    uv_timer_init(loop, timer);
    TaskMessage *taskMessage = new TaskMessage();
    taskMessage->executeId = executeId;
    taskMessage->priority = priority;
    taskMessage->taskId = taskInfo->taskId;
    timer->data = taskMessage;
    uv_timer_start(timer, reinterpret_cast<uv_timer_cb>(DelayTask), delayTime, 0);
    uv_work_t *work = new uv_work_t;
    uv_queue_work_with_qos(loop, work, [](uv_work_t *) {},
                           [](uv_work_t *work, int32_t) {delete work; }, uv_qos_user_initiated);
    napi_value promise = NapiHelper::CreatePromise(env, &taskInfo->deferred);
    return promise;
}

napi_value TaskPool::ExecuteGroup(napi_env env, napi_value taskGroup, Priority priority)
{
    napi_value groupIdVal = NapiHelper::GetNameProperty(env, taskGroup, GROUP_ID_STR);
    uint32_t groupId = NapiHelper::GetUint32Value(env, groupIdVal);
    TaskGroupManager& groupManager = TaskGroupManager::GetInstance();
    const std::list<napi_ref>& taskRefs = groupManager.GetTasksByGroup(groupId);
    uint32_t groupExecuteId = groupManager.GenerateGroupExecuteId();
    GroupInfo* groupInfo = groupManager.GenerateGroupInfo(env, taskRefs.size(), groupId, groupExecuteId);
    napi_value promise = NapiHelper::CreatePromise(env, &groupInfo->deferred);
    for (auto iter = taskRefs.begin(); iter != taskRefs.end(); iter++) {
        uint32_t executeId = TaskManager::GetInstance().GenerateExecuteId();
        groupInfo->executeIds.push_back(executeId);
        napi_value task = NapiHelper::GetReferenceValue(env, *iter);
        TaskInfo* taskInfo = TaskManager::GetInstance().GenerateTaskInfoFromTask(env, task, executeId);
        if (taskInfo == nullptr) {
            HILOG_ERROR("taskpool::ExecuteGroup taskInfo is nullptr");
            return nullptr;
        }
        taskInfo->groupExecuteId = groupExecuteId;
        ExecuteFunction(env, taskInfo, Priority(priority));
    }
    return promise;
}

void TaskPool::HandleTaskResult(const uv_async_t* req)
{
    HITRACE_HELPER_METER_NAME(__PRETTY_FUNCTION__);
    auto taskInfo = static_cast<TaskInfo*>(req->data);
    if (taskInfo == nullptr) {
        HILOG_FATAL("taskpool::HandleTaskResult taskInfo is null");
        return;
    }
    napi_handle_scope scope = nullptr;
    NAPI_CALL_RETURN_VOID(taskInfo->env, napi_open_handle_scope(taskInfo->env, &scope));
    napi_value taskData = nullptr;
    napi_status status = napi_deserialize(taskInfo->env, taskInfo->result, &taskData);
    napi_delete_serialization_data(taskInfo->env, taskInfo->result);

    // tag for trace parse: Task PerformTask End
    std::string strTrace = "Task PerformTask End: taskId : " + std::to_string(taskInfo->taskId);
    strTrace += ", executeId : " + std::to_string(taskInfo->executeId);
    if (taskInfo->isCanceled) {
        strTrace += ", performResult : IsCanceled";
    } else if (status != napi_ok) {
        HILOG_ERROR("taskpool: failed to deserialize result");
        strTrace += ", performResult : DeserializeFailed";
    } else if (taskInfo->success) {
        strTrace += ", performResult : Successful";
    } else {
        strTrace += ", performResult : Unsuccessful";
    }
    HITRACE_HELPER_METER_NAME(strTrace);

    bool success = status == napi_ok && !taskInfo->isCanceled && taskInfo->success;
    if (taskData == nullptr) {
        napi_get_undefined(taskInfo->env, &taskData);
    }
    if (taskInfo->groupExecuteId == 0) {
        if (success) {
            napi_resolve_deferred(taskInfo->env, taskInfo->deferred, taskData);
        } else {
            napi_reject_deferred(taskInfo->env, taskInfo->deferred, taskData);
        }
    } else {
        UpdateGroupInfoByResult(taskInfo->env, taskInfo, taskData, success);
    }
    NAPI_CALL_RETURN_VOID(taskInfo->env, napi_close_handle_scope(taskInfo->env, scope));
    // seqRunnerTask will trigger the next
    if (taskInfo->seqRunnerId) {
        if (!TaskGroupManager::GetInstance().TriggerSeqRunner(taskInfo->env, taskInfo)) {
            HILOG_ERROR("seqRunner:: task %{public}d trigger in seqRunner %{public}d failed",
                        taskInfo->groupExecuteId, taskInfo->seqRunnerId);
        }
    }
    TaskManager::GetInstance().DecreaseRefCount(taskInfo->env, taskInfo->taskId);
    if (TaskManager::GetInstance().CanReleaseTaskContent(taskInfo->executeId)) {
        TaskManager::GetInstance().ReleaseTaskContent(taskInfo);
    }
}

void TaskPool::UpdateGroupInfoByResult(napi_env env, TaskInfo* taskInfo, napi_value res, bool success)
{
    uint32_t groupExecuteId = taskInfo->groupExecuteId;
    bool isRunning = TaskGroupManager::GetInstance().IsRunning(groupExecuteId);
    if (!isRunning) {
        return;
    }
    GroupInfo* groupInfo = TaskGroupManager::GetInstance().GetGroupInfoByExecutionId(groupExecuteId);
    if (groupInfo == nullptr) {
        return;
    }
    uint32_t headId = *groupInfo->executeIds.begin();
    uint32_t index = taskInfo->executeId - headId;
    if (success) {
        // Update res at resArr
        napi_ref arrRef = groupInfo->resArr;
        napi_value resArr = NapiHelper::GetReferenceValue(env, arrRef);
        napi_set_element(env, resArr, index, res);

        groupInfo->finishedTask++;
        if (groupInfo->finishedTask < groupInfo->taskNum) {
            return;
        }
        napi_resolve_deferred(env, groupInfo->deferred, resArr);
    } else {
        napi_value undefined = nullptr;
        napi_get_undefined(env, &undefined);
        napi_reject_deferred(env, groupInfo->deferred, undefined);
    }
    TaskGroupManager::GetInstance().RemoveExecuteId(groupInfo->groupId, groupExecuteId);
    TaskGroupManager::GetInstance().ClearGroupInfo(env, groupExecuteId, groupInfo);
}

void TaskPool::ExecuteFunction(napi_env env, TaskInfo* taskInfo, Priority priority)
{
    uint32_t executeId = taskInfo->executeId;
    taskInfo->priority = priority;
    // tag for trace parse: Task Allocation
    std::string strTrace = "Task Allocation: taskId : " + std::to_string(taskInfo->taskId)
        + ", executeId : " + std::to_string(executeId)
        + ", priority : " + std::to_string(priority)
        + ", executeState : " + std::to_string(ExecuteState::WAITING);
    HITRACE_HELPER_METER_NAME(strTrace);
    TaskManager::GetInstance().IncreaseRefCount(taskInfo->taskId);
    TaskManager::GetInstance().AddExecuteState(executeId);
    TaskManager::GetInstance().EnqueueExecuteId(executeId, priority);
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
        napi_value taskId = NapiHelper::GetNameProperty(env, args[0], TASKID_STR);
        if (taskId == nullptr) {
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: the type of the params must be task");
            return nullptr;
        }
        uint32_t id = NapiHelper::GetUint32Value(env, taskId);
        TaskManager::GetInstance().CancelTask(env, id);
    } else {
        napi_value groupIdVal = NapiHelper::GetNameProperty(env, args[0], GROUP_ID_STR);
        if (groupIdVal == nullptr) {
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
                                    "taskpool:: the type of the params must be taskGroup");
            return nullptr;
        }
        uint32_t groupId = NapiHelper::GetUint32Value(env, groupIdVal);
        TaskGroupManager::GetInstance().CancelGroup(env, groupId);
        TaskGroupManager::GetInstance().ClearExecuteId(groupId);
    }
    return nullptr;
}
} // namespace Commonlibrary::Concurrent::TaskPoolModule