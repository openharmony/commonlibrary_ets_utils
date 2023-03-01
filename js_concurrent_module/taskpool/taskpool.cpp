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
    const char className[] = "Task";
    napi_property_descriptor properties[] = {};
    napi_value taskClass = nullptr;
    napi_define_class(env, className, sizeof(className), Task::TaskConstructor, nullptr,
        sizeof(properties) / sizeof(properties[0]), properties, &taskClass);
    napi_set_named_property(env, exports, "Task", taskClass);

    napi_value executeFunc;
    napi_create_function(env, "execute", NAPI_AUTO_LENGTH, Execute, NULL, &executeFunc);
    napi_set_named_property(env, exports, "execute", executeFunc);

    napi_value cancelFunc;
    napi_create_function(env, "cancel", NAPI_AUTO_LENGTH, Cancel, NULL, &cancelFunc);
    napi_set_named_property(env, exports, "cancel", cancelFunc);

    napi_value priorityObj = nullptr;
    napi_create_object(env, &priorityObj);
    napi_value highPriority = nullptr;
    napi_value mediumPriority = nullptr;
    napi_value lowPriority = nullptr;
    napi_create_int32(env, Priority::HIGH, &highPriority);
    napi_set_named_property(env, priorityObj, "HIGH", highPriority);
    napi_create_int32(env, Priority::MEDIUM, &mediumPriority);
    napi_set_named_property(env, priorityObj, "MEDIUM", mediumPriority);
    napi_create_int32(env, Priority::LOW, &lowPriority);
    napi_set_named_property(env, priorityObj, "LOW", lowPriority);

    napi_property_descriptor exportPriority[] = {
        DECLARE_NAPI_PROPERTY("Priority", priorityObj),
    };
    napi_define_properties(env, exports, sizeof(exportPriority) / sizeof(exportPriority[0]), exportPriority);

    // Add a reserved thread for taskpool
    TaskManager::GetInstance().InitTaskRunner(env);
    return exports;
}

napi_value TaskPool::Execute(napi_env env, napi_callback_info cbinfo)
{
    HITRACE_METER_NAME(HITRACE_TAG_COMMONLIBRARY, __PRETTY_FUNCTION__);
    StartTrace(HITRACE_TAG_COMMONLIBRARY, "ExecuteToWorkerEnd");
    // get the taskpool instance
    TaskManager::GetInstance().InitTaskRunner(env);
    FinishTrace(HITRACE_TAG_COMMONLIBRARY);
    // check the argc
    size_t argc = 0;
    napi_get_cb_info(env, cbinfo, &argc, nullptr, nullptr, nullptr);
    if (argc < 1) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: the number of params must be at least one");
        return nullptr;
    }

    // check the first param is object or func
    napi_value* args = new napi_value[argc];
    ObjectScope<napi_value> scope(args, true);
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, cbinfo, &argc, args, &thisVar, nullptr);
    napi_valuetype type;
    NAPI_CALL(env, napi_typeof(env, args[0], &type));

    if (type == napi_object) {
        Task* task = nullptr;
        NAPI_CALL(env, napi_unwrap(env, args[0], reinterpret_cast<void**>(&task)));
        return ExecuteTask(env, task);
    } else if (type == napi_function) {
        napi_value argsArray;
        napi_create_array_with_length(env, argc - 1, &argsArray);
        for (size_t i = 0; i < argc - 1; i++) {
            napi_set_element(env, argsArray, i, args[i + 1]);
        }
        napi_value object;
        napi_create_object(env, &object);
        napi_set_named_property(env, object, "func", args[0]);
        napi_set_named_property(env, object, "args", argsArray);
        return ExecuteFunction(env, object);
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
    napi_value taskData = nullptr;
    napi_status status = napi_deserialize(taskInfo->env, taskInfo->result, &taskData);
    if (status != napi_ok || taskData == nullptr || !taskInfo->success) {
        napi_reject_deferred(taskInfo->env, taskInfo->deferred, taskData);
    } else {
        napi_resolve_deferred(taskInfo->env, taskInfo->deferred, taskData);
    }
    TaskManager::GetInstance().ReleaseTaskContent(taskInfo);
}

napi_value TaskPool::ExecuteTask(napi_env env, Task* task)
{
    HITRACE_METER_NAME(HITRACE_TAG_COMMONLIBRARY, __PRETTY_FUNCTION__);
    napi_value obj = nullptr;
    napi_get_reference_value(env, task->objRef_, &obj);
    task->executeId_ = TaskManager::GetInstance().GenerateExecuteId();
    TaskInfo* taskInfo = TaskManager::GetInstance().GenerateTaskInfo(env, obj, task->taskId_, task->executeId_);
    if (taskInfo == nullptr) {
        return nullptr;
    }
    TaskManager::GetInstance().StoreStateInfo(taskInfo->executeId, TaskState::WAITING);
    TaskManager::GetInstance().StoreRunningInfo(taskInfo->taskId, taskInfo->executeId);
    napi_create_promise(env, &taskInfo->deferred, &taskInfo->promise);
    Task* currentTask = new Task();
    *currentTask = *task;
    std::unique_ptr<Task> pointer(currentTask);
    TaskManager::GetInstance().EnqueueTask(std::move(pointer));
    if (taskInfo->promise == nullptr) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: create promise error");
        return nullptr;
    }
    return taskInfo->promise;
}

napi_value TaskPool::ExecuteFunction(napi_env env, napi_value object)
{
    HITRACE_METER_NAME(HITRACE_TAG_COMMONLIBRARY, __PRETTY_FUNCTION__);
    std::unique_ptr<Task> task = std::make_unique<Task>();
    task->executeId_ = TaskManager::GetInstance().GenerateExecuteId();
    TaskInfo* taskInfo = TaskManager::GetInstance().GenerateTaskInfo(env, object, 0, task->executeId_);
    if (taskInfo == nullptr) {
        return nullptr;
    }
    napi_create_promise(env, &taskInfo->deferred, &taskInfo->promise);
    TaskManager::GetInstance().EnqueueTask(std::move(task));
    if (taskInfo->promise == nullptr) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: create promise error");
        return nullptr;
    }
    return taskInfo->promise;
}

napi_value TaskPool::Cancel(napi_env env, napi_callback_info cbinfo)
{
    HITRACE_METER_NAME(HITRACE_TAG_COMMONLIBRARY, __PRETTY_FUNCTION__);
    size_t argc = 0;
    napi_get_cb_info(env, cbinfo, &argc, nullptr, nullptr, nullptr);
    if (argc != 1) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: the number of the params must be one");
        return nullptr;
    }

    napi_value* args = new napi_value[argc];
    ObjectScope<napi_value> scope(args, true);
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, cbinfo, &argc, args, &thisVar, nullptr);
    napi_valuetype type;
    NAPI_CALL(env, napi_typeof(env, args[0], &type));
    if (type != napi_object) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: the type of the params must be object");
        return nullptr;
    }
    Task* task = nullptr;
    NAPI_CALL(env, napi_unwrap(env, args[0], reinterpret_cast<void**>(&task)));
    TaskManager::GetInstance().CancelTask(env, task->taskId_);
    return nullptr;
}
} // namespace Commonlibrary::Concurrent::TaskPoolModule