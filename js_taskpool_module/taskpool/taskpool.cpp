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

#include "object_helper.h"
#include "utils/log.h"
#include "worker.h"

namespace Commonlibrary::TaskPoolModule {
using namespace CompilerRuntime::WorkerModule::Helper;
napi_value TaskPool::InitTaskPool(napi_env env, napi_value exports)
{
    napi_value func;
    napi_create_function(env, "execute", NAPI_AUTO_LENGTH, Execute, NULL, &func);
    napi_set_named_property(env, exports, "execute", func);
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
    }
    if (Worker::NeedExpandWorker()) {
        Worker::WorkerConstructor(env);
    }
}

napi_value TaskPool::Execute(napi_env env, napi_callback_info cbinfo)
{
    TaskPool::GetCurrentTaskpool()->InitTaskRunner(env);

    // generate the taskInfo
    size_t argc = 0;
    napi_get_cb_info(env, cbinfo, &argc, nullptr, nullptr, nullptr);
    if (argc != 1) {
        napi_throw_error(env, nullptr, "TaskPool Execute param num should be one");
        return nullptr;
    }

    napi_value* args = new napi_value[argc];
    ObjectScope<napi_value> scope(args, true);
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, cbinfo, &argc, args, &thisVar, nullptr);
    napi_valuetype type;
    NAPI_CALL(env, napi_typeof(env, args[0], &type));
    if (type != napi_object) {
        napi_throw_error(env, nullptr, "TaskPool Execute param type should be Object");
        return nullptr;
    }

    std::unique_ptr<Task> task = std::make_unique<Task>();
    napi_value undefined;
    napi_get_undefined(env, &undefined);
    napi_value taskData;
    napi_status serializeStatus = napi_ok;
    serializeStatus = napi_serialize(env, args[0], undefined, &taskData);
    if (serializeStatus != napi_ok || taskData == nullptr) {
        napi_throw_error(env, nullptr, "Failed to serialize message");
        return nullptr;
    }
    task->taskData_ = taskData;

    // generate the promise and enqueue the task
    napi_value promise = nullptr;
    napi_create_promise(env, &task->deferred_, &promise);
    Worker::EnqueueTask(std::move(task));
    return promise;
}
} // namespace Commonlibrary::TaskPoolModule