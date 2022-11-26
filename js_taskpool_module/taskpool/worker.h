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
#include "task_runner.h"
namespace Commonlibrary::TaskPoolModule {
using WorkerEnv = napi_env;
class Worker {
public:
    Worker(napi_env env);
    ~Worker();

    static napi_value WorkerConstructor(napi_env env);
    void StartExecuteInThread(napi_env env);
    static void ExecuteInThread(const void* data);
    bool PrepareForWorkerInstance();
    static bool NeedInitWorker();

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
        } else {
            napi_throw_error(workerEnv_, nullptr, "Worker loop is nullptr");
            return;
        }
    }

    napi_env hostEnv_ {nullptr};
    napi_env workerEnv_ {nullptr};
    std::unique_ptr<TaskRunner> runner_ {};

private:
    std::recursive_mutex liveEnvLock_ {};
};
} // namespace Commonlibrary::TaskPoolModule
#endif // JS_TASKPOOL_MODULE_TASKPOOL_WORKER_H_