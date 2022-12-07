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

#include <unistd.h>

#include "object_helper.h"
#include "utils/log.h"
#include "worker.h"

namespace Commonlibrary::TaskPoolModule {
using namespace CompilerRuntime::WorkerModule::Helper;
static int32_t g_taskId = 0;
static std::mutex g_mutex;

napi_value TaskPool::InitTaskPool(napi_env env, napi_value exports)
{
    const char className[] = "Task";
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("cancel", Cancel),
    };
    napi_value workerClazz = nullptr;
    napi_define_class(env, className, sizeof(className), Task::TaskConstructor, nullptr,
        sizeof(properties) / sizeof(properties[0]), properties, &workerClazz);
    napi_set_named_property(env, exports, "Task", workerClazz);

    napi_value initFunc;
    napi_create_function(env, "execute", NAPI_AUTO_LENGTH, Execute, NULL, &initFunc);
    napi_set_named_property(env, exports, "execute", initFunc);

    napi_value cancelFunc;
    napi_create_function(env, "cancel", NAPI_AUTO_LENGTH, Cancel, NULL, &cancelFunc);
    napi_set_named_property(env, exports, "cancel", cancelFunc);
    return exports;
}

TaskPool *TaskPool::GetCurrentTaskpool()
{
    static TaskPool taskpool;
    return &taskpool;
}

void TaskPool::InitTaskRunner(napi_env env)
{
    std::unique_lock<std::mutex> lock(mtx_);
    if (!isInitialized_ && Worker::NeedInitWorker()) {
        Worker::WorkerConstructor(env);
        isInitialized_ = true;
        return;
    }
    if (Worker::NeedExpandWorker()) {
        Worker::WorkerConstructor(env);
        return;
    }
}

void TaskPool::GenerateTaskId(Task* task)
{
    std::unique_lock<std::mutex> lock(g_mutex);
    task->taskId_ = g_taskId++;
}

napi_value TaskPool::Execute(napi_env env, napi_callback_info cbinfo)
{
    TaskPool::GetCurrentTaskpool()->InitTaskRunner(env);

    // generate the taskInfo
    size_t argc = 0;
    napi_get_cb_info(env, cbinfo, &argc, nullptr, nullptr, nullptr);
    if (argc < 1) {
        Worker::ThrowError(env, Worker::TYPE_ERROR, "taskpool:: the number of the params should at least be one");
        return nullptr;
    }
    napi_value* args = new napi_value[argc];
    ObjectScope<napi_value> scope(args, true);
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, cbinfo, &argc, args, &thisVar, nullptr);
    napi_valuetype type;
    NAPI_CALL(env, napi_typeof(env, args[0], &type));
    if (type == napi_object) {
        return ExecuteTask(env, cbinfo);
    } else if (type == napi_function) {
        return ExecuteFunction(env, cbinfo);
    }

    Worker::ThrowError(env, Worker::TYPE_ERROR, "taskpool:: the type of the first param must be object or function");
    return nullptr;
}

napi_value TaskPool::ExecuteTask(napi_env env, napi_callback_info cbinfo)
{
    // generate the taskInfo
    size_t argc = 0;
    napi_get_cb_info(env, cbinfo, &argc, nullptr, nullptr, nullptr);

    napi_value* args = new napi_value[argc];
    ObjectScope<napi_value> scope(args, true);
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, cbinfo, &argc, args, &thisVar, nullptr);

    napi_value taskArgs;
    napi_get_named_property(env, args[0], "args", &taskArgs);
    bool isArray = false;
    napi_is_array(env, taskArgs, &isArray);
    if (!isArray) {
        Worker::ThrowError(env, Worker::TYPE_ERROR, "taskpool:: the type of the args must be array");
        return nullptr;
    }
    uint32_t argsNum = 0;
    napi_get_array_length(env, taskArgs, &argsNum);

    std::unique_ptr<Task> task = std::make_unique<Task>();
    napi_value undefined;
    napi_get_undefined(env, &undefined);
    napi_value taskData;
    napi_status serializeStatus = napi_ok;
    serializeStatus = napi_serialize(env, args[0], undefined, &taskData);
    if (serializeStatus != napi_ok || taskData == nullptr) {
        Worker::ThrowError(env, Worker::SERIALIZATION_ERROR, "taskpool:: Failed to serialize the message");
        return nullptr;
    }

    GenerateTaskId(task.get());
    TaskInfo *taskInfo = new TaskInfo();
    Worker::StoreTaskInfo(task->taskId_, taskInfo);
    taskInfo->env = env;
    taskInfo->serializationData = taskData;
    taskInfo->taskSignal = new uv_async_t;
    taskInfo->taskId = task->taskId_;
    taskInfo->argsNum = argsNum;
    uv_loop_t *loop = nullptr;
    napi_get_uv_event_loop(env, &loop);
    uv_async_init(loop, taskInfo->taskSignal, reinterpret_cast<uv_async_cb>(Worker::HandleTaskResult));
    taskInfo->taskSignal->data = taskInfo;

    // generate the promise and enqueue the task
    napi_create_promise(env, &taskInfo->deferred, &taskInfo->promise);
    Worker::EnqueueTask(std::move(task));
    return taskInfo->promise;
}

napi_value TaskPool::ExecuteFunction(napi_env env, napi_callback_info cbinfo)
{
    return nullptr;
}

napi_value TaskPool::Cancel(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = 0;
    napi_get_cb_info(env, cbinfo, &argc, nullptr, nullptr, nullptr);
    if (argc != 1) {
        Worker::ThrowError(env, Worker::TYPE_ERROR, "the number of the params must be one");
        return nullptr;
    }

    napi_value* args = new napi_value[argc];
    ObjectScope<napi_value> scope(args, true);
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, cbinfo, &argc, args, &thisVar, nullptr);
    napi_valuetype type;
    NAPI_CALL(env, napi_typeof(env, args[0], &type));
    if (type != napi_object) {
        Worker::ThrowError(env, Worker::TYPE_ERROR, "the type of the params must be object");
        return nullptr;
    }
    Task *task = nullptr;
    NAPI_CALL(env, napi_unwrap(env, args[0], reinterpret_cast<void **>(&task)));
    Worker::CancelTask(task->taskId_);
    return nullptr;
}
} // namespace Commonlibrary::TaskPoolModule