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
#include "task_manager.h"
#include "utils/log.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
using namespace Commonlibrary::Concurrent::Common::Helper;
napi_value TaskPool::InitTaskPool(napi_env env, napi_value exports)
{
    HILOG_INFO("taskpool:: Import taskpool");
    HITRACE_HELPER_METER_NAME(__PRETTY_FUNCTION__);
    napi_value taskClass = nullptr;
    napi_define_class(env, "Task", NAPI_AUTO_LENGTH, Task::TaskConstructor, nullptr, 0, nullptr, &taskClass);
    napi_value taskGroupClass = nullptr;
    napi_define_class(env, "TaskGroup", NAPI_AUTO_LENGTH, TaskGroup::TaskGroupConstructor, nullptr, 0, nullptr,
                      &taskGroupClass);
    napi_value isCanceledFunc;
    napi_create_function(env, "isCanceled", NAPI_AUTO_LENGTH, TaskManager::IsCanceled, NULL, &isCanceledFunc);
    napi_set_named_property(env, taskClass, "isCanceled", isCanceledFunc);

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
        DECLARE_NAPI_PROPERTY("Priority", priorityObj),
        DECLARE_NAPI_FUNCTION("execute", Execute),
        DECLARE_NAPI_FUNCTION("cancel", Cancel),
    };
    napi_define_properties(env, exports, sizeof(properties) / sizeof(properties[0]), properties);

    HITRACE_HELPER_START_TRACE("InitTaskManager");
    TaskManager::GetInstance().InitTaskManager(env);
    HITRACE_HELPER_FINISH_TRACE;
    return exports;
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
            priority = NapiHelper::GetUint32Value(env, args[1]);
            if (priority >= Priority::NUMBER) {
                ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: priority value is error");
                return nullptr;
            }
        }
        bool isGroup = false;
        napi_has_named_property(env, args[0], GROUP_ID_STR, &isGroup);
        if (isGroup) {
            return ExecuteGroup(env, args[0], Priority(priority));
        } else {
            uint32_t executeId = TaskManager::GetInstance().GenerateExecuteId();
            TaskInfo* taskInfo = TaskManager::GetInstance().GenerateTaskInfoFromTask(env, args[0], executeId);
            napi_value promise = NapiHelper::CreatePromise(env, &taskInfo->deferred);
            TaskManager::GetInstance().StoreRunningInfo(taskInfo->taskId, executeId);
            ExecuteFunction(env, taskInfo, Priority(priority));
            return promise;
        }
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
    TaskInfo* taskInfo = TaskManager::GetInstance().GenerateTaskInfo(env, args[0], argsArray, 0, executeId);
    napi_value promise = NapiHelper::CreatePromise(env, &taskInfo->deferred);
    TaskManager::GetInstance().StoreRunningInfo(0, executeId);
    ExecuteFunction(env, taskInfo);
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

    // trace : Task PerformTask end after deserialize
    std::string strTrace = "Task PerformTask End: taskId : " + std::to_string(taskInfo->taskId);
    strTrace += ", executeId : " + std::to_string(taskInfo->executeId);
    if (taskInfo->isCanceled) {
        strTrace += " [IsCanceled]";
    } else if (status != napi_ok) {
        strTrace += " [DeserializeFailed]";
    } else if (taskInfo->success) {
        strTrace += " [Successful]";
    } else {
        strTrace += " [Unsuccessful]";
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
    TaskManager::GetInstance().ReleaseTaskContent(taskInfo);
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
    std::string strTrace = "Task Allocation: taskId : " + std::to_string(taskInfo->taskId);
    strTrace += ", executeId : " + std::to_string(executeId);
    strTrace += ", priority : " + std::to_string(priority);
    strTrace += ", executeState : " + std::to_string(ExecuteState::WAITING);
    HITRACE_HELPER_METER_NAME(strTrace);
    TaskManager::GetInstance().TryTriggerLoadBalance();
    TaskManager::GetInstance().AddExecuteState(executeId);
    TaskManager::GetInstance().EnqueueExecuteId(executeId, priority);
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

    if (!NapiHelper::IsObject(args[0])) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: the type of the params must be object");
        return nullptr;
    }

    bool isGroup = false;
    napi_has_named_property(env, args[0], GROUP_ID_STR, &isGroup);
    if (!isGroup) {
        napi_value taskId = NapiHelper::GetNameProperty(env, args[0], TASKID_STR);
        if (taskId == nullptr) {
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: the type of the params must be task");
            return nullptr;
        }
        uint32_t id = NapiHelper::GetUint32Value(env, taskId);
        const std::list<uint32_t>& executeList = TaskManager::GetInstance().QueryRunningTask(env, id);
        for (uint32_t executeId : executeList) {
            TaskManager::GetInstance().CancelExecution(env, executeId);
        }
    } else {
        napi_value groupIdVal = NapiHelper::GetNameProperty(env, args[0], GROUP_ID_STR);
        if (groupIdVal == nullptr) {
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
                                    "taskpool:: the type of the params must be taskGroup");
            return nullptr;
        }
        uint32_t groupId = NapiHelper::GetUint32Value(env, groupIdVal);
        const std::list<uint32_t>& ids = TaskGroupManager::GetInstance().GetExecuteIdList(groupId);
        if (ids.empty()) {
            ErrorHelper::ThrowError(env, ErrorHelper::ERR_CANCEL_NONEXIST_TASK_GROUP);
            HILOG_ERROR("taskpool:: cancel nonexist task group");
            return nullptr;
        }
        TaskGroupManager::GetInstance().CancelGroup(env, ids);
        TaskGroupManager::GetInstance().ClearExecuteId(groupId);
    }
    return nullptr;
}
} // namespace Commonlibrary::Concurrent::TaskPoolModule