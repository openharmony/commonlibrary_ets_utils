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

#ifndef JS_CONCURRENT_MODULE_TASKPOOL_WORKER_H_
#define JS_CONCURRENT_MODULE_TASKPOOL_WORKER_H_

#include <list>
#include <memory>

#include "helper/error_helper.h"
#include "helper/napi_helper.h"
#include "helper/object_helper.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "native_engine/native_engine.h"
#include "task.h"
#include "task_runner.h"
#include "utils/log.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
using namespace Commonlibrary::Concurrent::Common::Helper;

class Worker {
public:
    ~Worker();

    static Worker* WorkerConstructor(napi_env env);

    void NotifyExecuteTask();

    void NotifyIdle();

private:
    explicit Worker(napi_env env);

#if !defined(WINDOWS_PLATFORM) && !defined(MAC_PLATFORM)
    static void HandleDebuggerTask(const uv_async_t* req);
    void DebuggerOnPostTask(std::function<void()>&& task);
#endif

    uv_loop_t* GetWorkerLoop() const
    {
        if (workerEnv_ != nullptr) {
            return NapiHelper::GetLibUV(workerEnv_);
        }
        return nullptr;
    }

    void RunLoop() const
    {
        uv_loop_t* loop = GetWorkerLoop();
        if (loop != nullptr) {
            uv_run(loop, UV_RUN_DEFAULT);
        } else {
            HILOG_ERROR("taskpool:: Worker loop is nullptr when start worker loop");
            return;
        }
    }

    void StartExecuteInThread();
    static void ExecuteInThread(const void* data);
    bool PrepareForWorkerInstance();
    void ReleaseWorkerThreadContent();
    static void PerformTask(const uv_async_t* req);
    static void TaskResultCallback(NativeEngine* engine, NativeValue* value, NativeValue* data);
    static void NotifyTaskResult(napi_env env, TaskInfo* taskInfo, napi_value result);

    napi_env hostEnv_ {nullptr};
    napi_env workerEnv_ {nullptr};
    uv_async_t *performTaskSignal_ {nullptr};
#if !defined(WINDOWS_PLATFORM) && !defined(MAC_PLATFORM)
    uv_async_t *debuggerOnPostTaskSignal_ {nullptr};
    std::function<void()> debuggerTask_;
#endif
    std::unique_ptr<TaskRunner> runner_ {};
};
} // namespace Commonlibrary::Concurrent::TaskPoolModule
#endif // JS_CONCURRENT_MODULE_TASKPOOL_WORKER_H_
