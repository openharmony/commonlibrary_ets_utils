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

napi_value TaskPool::Execute(napi_env env, napi_callback_info cbinfo)
{
    TaskPool::GetCurrentTaskpool()->InitTaskRunner(env);

    // generate the taskInfo
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
    g_mutex.lock();
    task->taskId_ = g_taskId;
    g_taskId += 1;
    g_mutex.unlock();

    TaskInfo *taskInfo = new TaskInfo();
    Worker::StoreTaskInfo(task->taskId_, taskInfo);
    taskInfo->env = env;
    taskInfo->serializationData = taskData;
    taskInfo->taskSignal = new uv_async_t;
    taskInfo->taskId = task->taskId_;
    uv_loop_t *loop = nullptr;
    napi_get_uv_event_loop(env, &loop);
    uv_async_init(loop, taskInfo->taskSignal, reinterpret_cast<uv_async_cb>(Worker::HandleTaskResult));
    taskInfo->taskSignal->data = taskInfo;

    // generate the promise and enqueue the task
    napi_create_promise(env, &taskInfo->deferred, &taskInfo->promise);
    Worker::EnqueueTask(std::move(task));
    return taskInfo->promise;
}

napi_value TaskPool::Cancel(napi_env env, napi_callback_info cbinfo)
{
    return nullptr;
}
} // namespace Commonlibrary::TaskPoolModule