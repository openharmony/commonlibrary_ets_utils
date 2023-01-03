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

#ifndef JS_TASKPOOL_MODULE_TASKPOOL_WORKER_H_
#define JS_TASKPOOL_MODULE_TASKPOOL_WORKER_H_

#include <list>
#include <memory>

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "native_engine/native_engine.h"
#include "error_helper.h"
#include "object_helper.h"
#include "task_manager.h"
#include "task_runner.h"
#include "task_queue.h"

namespace Commonlibrary::TaskPoolModule {
using WorkerEnv = napi_env;
class Worker {
public:
    Worker(napi_env env);
    ~Worker() = default;

    void StartExecuteInThread(napi_env env);
    bool PrepareForWorkerInstance();
    void ReleaseWorkerThreadContent();
    void TerminateWorker();
    static bool NeedExpandWorker();
    static void WorkerDestructor();
    static void EnqueueTask(std::unique_ptr<Task> task);
    static void ExecuteInThread(const void* data);
    static void HandleTaskResult(const uv_async_t* req);
    static void PerformTask(const uv_async_t* req);
    static napi_value WorkerConstructor(napi_env env);

    uv_loop_t* GetWorkerLoop() const
    {
        uv_loop_t *loop = nullptr;
        if (workerEnv_ != nullptr) {
            napi_get_uv_event_loop(workerEnv_, &loop);
        }
        return loop;
    }

    void Loop() const
    {
        uv_loop_t* loop = GetWorkerLoop();
        if (loop != nullptr) {
            uv_run(loop, UV_RUN_DEFAULT);
        }
    }

private:
    napi_env hostEnv_ {nullptr};
    napi_env workerEnv_ {nullptr};
    uv_async_t* performTaskSignal_ {nullptr};
    std::unique_ptr<TaskRunner> runner_ {};
    std::recursive_mutex liveEnvLock_ {};
};
} // namespace Commonlibrary::TaskPoolModule
#endif // JS_TASKPOOL_MODULE_TASKPOOL_WORKER_H_