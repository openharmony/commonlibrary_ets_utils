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
#include "helper/object_helper.h"
#include "hitrace_meter.h"
#include "task_manager.h"
#include "utils/log.h"
#include "worker.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
using namespace Commonlibrary::Concurrent::Common::Helper;
napi_value TaskPool::InitTaskPool(napi_env env, napi_value exports)
{
    HITRACE_METER_NAME(HITRACE_TAG_COMMONLIBRARY, __PRETTY_FUNCTION__);
    napi_value taskClass = nullptr;
    napi_define_class(env, "Task", NAPI_AUTO_LENGTH, Task::TaskConstructor, nullptr, 0, nullptr, &taskClass);

    // define priority
    napi_value priorityObj = NapiHelper::CreateObject(env);
    napi_value highPriority = nullptr;
    napi_value mediumPriority = nullptr;
    napi_value lowPriority = nullptr;
    napi_create_int32(env, Priority::HIGH, &highPriority);
    napi_create_int32(env, Priority::MEDIUM, &mediumPriority);
    napi_create_int32(env, Priority::LOW, &lowPriority);
    napi_property_descriptor exportPriority[] = {
        DECLARE_NAPI_PROPERTY("HIGH", highPriority),
        DECLARE_NAPI_PROPERTY("MEDIUM", mediumPriority),
        DECLARE_NAPI_PROPERTY("LOW", lowPriority),
    };
    napi_define_properties(env, priorityObj, sizeof(exportPriority) / sizeof(exportPriority[0]), exportPriority);

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_PROPERTY("Task", taskClass),
        DECLARE_NAPI_PROPERTY("Priority", priorityObj),
        DECLARE_NAPI_FUNCTION("execute", Execute),
        DECLARE_NAPI_FUNCTION("cancel", Cancel),
    };
    napi_define_properties(env, exports, sizeof(properties) / sizeof(properties[0]), properties);

    StartTrace(HITRACE_TAG_COMMONLIBRARY, "InitTaskManager");
    TaskManager::GetInstance().InitTaskManager(env);
    FinishTrace(HITRACE_TAG_COMMONLIBRARY);
    return exports;
}

napi_value TaskPool::Execute(napi_env env, napi_callback_info cbinfo)
{
    HITRACE_METER_NAME(HITRACE_TAG_COMMONLIBRARY, __PRETTY_FUNCTION__);
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

    if (type == napi_object) {
        napi_value function = nullptr;
        napi_value arguments = nullptr;
        napi_value taskId = nullptr;
        napi_get_named_property(env, args[0], FUNCTION_STR, &function);
        napi_get_named_property(env, args[0], ARGUMENTS_STR, &arguments);
        napi_get_named_property(env, args[0], TASKID_STR, &taskId);
        if (function == nullptr || arguments == nullptr || taskId == nullptr) {
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: task value is error");
            return nullptr;
        }
        uint32_t id = 0;
        napi_get_value_uint32(env, taskId, &id);

        uint32_t priority = Priority::DEFAULT; // DEFAULT priority is MEDIUM
        if (argc > 1) {
            napi_get_value_uint32(env, args[1], &priority);
            if (priority >= Priority::NUMBER) {
                ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: priority value is error");
                return nullptr;
            }
        }
        return ExecuteFunction(env, function, arguments, id, Priority(priority));
    } else if (type == napi_function) {
        napi_value argsArray;
        napi_create_array_with_length(env, argc - 1, &argsArray);
        for (size_t i = 0; i < argc - 1; i++) {
            napi_set_element(env, argsArray, i, args[i + 1]);
        }
        return ExecuteFunction(env, args[0], argsArray, 0); // 0 : taskpool.execute(function) taskId is 0
    }
    ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: first param must be object or function");
    return nullptr;
}

void TaskPool::HandleTaskResult(const uv_async_t* req)
{
    HITRACE_METER_NAME(HITRACE_TAG_COMMONLIBRARY, __PRETTY_FUNCTION__);
    auto taskInfo = static_cast<TaskInfo*>(req->data);
    if (taskInfo == nullptr) {
        HILOG_FATAL("taskpool::HandleTaskResult taskInfo is null");
        return;
    }
    napi_handle_scope scope = nullptr;
    NAPI_CALL_RETURN_VOID(taskInfo->env, napi_open_handle_scope(taskInfo->env, &scope));
    napi_value taskData = nullptr;
    napi_status status = napi_deserialize(taskInfo->env, taskInfo->result, &taskData);
    if (status != napi_ok || taskData == nullptr || !taskInfo->success) {
        napi_reject_deferred(taskInfo->env, taskInfo->deferred, taskData);
    } else {
        napi_resolve_deferred(taskInfo->env, taskInfo->deferred, taskData);
    }
    NAPI_CALL_RETURN_VOID(taskInfo->env, napi_close_handle_scope(taskInfo->env, scope));
    TaskManager::GetInstance().ReleaseTaskContent(taskInfo);
}

napi_value TaskPool::ExecuteFunction(napi_env env,
                                    napi_value function, napi_value arguments, uint32_t taskId, Priority priority)
{
    std::string strTrace = "ExecuteFunction: taskId is " + std::to_string(taskId);
    StartTrace(HITRACE_TAG_COMMONLIBRARY, strTrace);
    uint32_t executeId = TaskManager::GetInstance().GenerateExecuteId();
    TaskInfo* taskInfo = TaskManager::GetInstance().GenerateTaskInfo(env, function, arguments, taskId, executeId);
    if (taskInfo == nullptr) {
        return nullptr;
    }
    TaskManager::GetInstance().StoreStateInfo(executeId, TaskState::WAITING);
    TaskManager::GetInstance().StoreRunningInfo(taskId, executeId);
    napi_value promise = nullptr;
    napi_create_promise(env, &taskInfo->deferred, &promise);
    TaskManager::GetInstance().EnqueueExecuteId(executeId, priority);
    if (promise == nullptr) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: create promise error");
        return nullptr;
    }
    FinishTrace(HITRACE_TAG_COMMONLIBRARY);
    return promise;
}

napi_value TaskPool::Cancel(napi_env env, napi_callback_info cbinfo)
{
    HITRACE_METER_NAME(HITRACE_TAG_COMMONLIBRARY, __PRETTY_FUNCTION__);
    size_t argc = 1;
    napi_value args[1];
    napi_get_cb_info(env, cbinfo, &argc, args, nullptr, nullptr);
    if (argc < 1) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: the number of the params must be one");
        return nullptr;
    }

    napi_valuetype type;
    napi_typeof(env, args[0], &type);
    if (type != napi_object) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: the type of the params must be object");
        return nullptr;
    }
    napi_value taskId = nullptr;
    napi_get_named_property(env, args[0], TASKID_STR, &taskId);
    if (taskId == nullptr) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: the type of the params must be task");
        return nullptr;
    }
    uint32_t id = 0;
    napi_get_value_uint32(env, taskId, &id);
    TaskManager::GetInstance().CancelTask(env, id);
    return nullptr;
}
} // namespace Commonlibrary::Concurrent::TaskPoolModule