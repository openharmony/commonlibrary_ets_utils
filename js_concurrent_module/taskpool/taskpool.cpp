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

#include "commonlibrary/ets_utils/js_concurrent_module/common/helper/error_helper.h"
#include "commonlibrary/ets_utils/js_concurrent_module/common/helper/object_helper.h"
#include "utils/log.h"
#include "worker.h"

namespace Commonlibrary::ConcurrentModule {
using namespace Commonlibrary::ConcurrentModule::Helper;
napi_value TaskPool::InitTaskPool(napi_env env, napi_value exports)
{
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
    return exports;
}

TaskPool& TaskPool::GetInstance()
{
    static TaskPool taskpool;
    return taskpool;
}

void TaskPool::InitTaskRunner(napi_env env)
{
    std::unique_lock<std::mutex> lock(mtx_);
    if (!isInitialized_ || Worker::NeedExpandWorker()) {
        Worker::WorkerConstructor(env);
        isInitialized_ = true;
    }
}

TaskPool::~TaskPool()
{
    Worker::WorkerDestructor();
}

napi_value TaskPool::Execute(napi_env env, napi_callback_info cbinfo)
{
    // get the taskpool instance
    TaskPool::GetInstance().InitTaskRunner(env);

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

    if (argc == 1 && type != napi_object) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: first param must be object when argc is one");
        return nullptr;
    } else if (type == napi_object) {
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

TaskInfo* TaskPool::GenerateTaskInfo(napi_env env, napi_value object, uint32_t taskId, uint32_t executeId)
{
    napi_value undefined;
    napi_get_undefined(env, &undefined);
    napi_value taskData;
    napi_status serializeStatus = napi_ok;
    serializeStatus = napi_serialize(env, object, undefined, &taskData);
    if (serializeStatus != napi_ok || taskData == nullptr) {
        return nullptr;
    }
    TaskInfo* taskInfo = new (std::nothrow) TaskInfo();
    TaskManager::StoreTaskInfo(executeId, taskInfo);
    taskInfo->env = env;
    taskInfo->executeId = executeId;
    taskInfo->serializationData = taskData;
    taskInfo->taskId = taskId;
    taskInfo->taskSignal = new uv_async_t;
    uv_loop_t* loop = nullptr;
    napi_get_uv_event_loop(env, &loop);
    uv_async_init(loop, taskInfo->taskSignal, reinterpret_cast<uv_async_cb>(Worker::HandleTaskResult));
    taskInfo->taskSignal->data = taskInfo;
    return taskInfo;
}

napi_value TaskPool::ExecuteTask(napi_env env, Task* task)
{
    napi_value obj = nullptr;
    napi_get_reference_value(env, task->objRef_, &obj);
    task->executeId_ = TaskManager::GenerateExecuteId();
    TaskInfo* taskInfo = GenerateTaskInfo(env, obj, task->taskId_, task->executeId_);
    TaskManager::StoreStateInfo(task->executeId_, TaskState::WAITING);
    TaskManager::StoreRunningInfo(task->taskId_, task->executeId_);
    napi_create_promise(env, &taskInfo->deferred, &taskInfo->promise);
    if (taskInfo->promise == nullptr || taskInfo->deferred == nullptr) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: create promise error");
        return nullptr;
    }
    Task* currentTask = new Task();
    *currentTask = *task;
    std::unique_ptr<Task> pointer(currentTask);
    Worker::EnqueueTask(std::move(pointer));
    return taskInfo->promise;
}

napi_value TaskPool::ExecuteFunction(napi_env env, napi_value object)
{
    std::unique_ptr<Task> task = std::make_unique<Task>();
    task->executeId_ = TaskManager::GenerateExecuteId();
    TaskInfo* taskInfo = GenerateTaskInfo(env, object, 0, task->executeId_); // 0: 0 for function specially
    napi_create_promise(env, &taskInfo->deferred, &taskInfo->promise);
    if (taskInfo->promise == nullptr || taskInfo->deferred == nullptr) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: create promise error");
        return nullptr;
    }
    Worker::EnqueueTask(std::move(task));
    return taskInfo->promise;
}

napi_value TaskPool::Cancel(napi_env env, napi_callback_info cbinfo)
{
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
    TaskManager::CancelTask(env, task->taskId_);
    return nullptr;
}
} // namespace Commonlibrary::ConcurrentModule