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

#ifndef JS_CONCURRENT_MODULE_TASKPOOL_WORKER_H
#define JS_CONCURRENT_MODULE_TASKPOOL_WORKER_H

#include <mutex>

#include "helper/concurrent_helper.h"
#include "helper/error_helper.h"
#include "helper/napi_helper.h"
#include "helper/object_helper.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "native_engine/native_engine.h"
#include "platform/qos_helper.h"
#include "task.h"
#include "task_runner.h"
#include "utils/log.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
using namespace Commonlibrary::Concurrent::Common;
using namespace Commonlibrary::Concurrent::Common::Helper;
using namespace Commonlibrary::Concurrent::Common::Platform;

enum class WorkerState { IDLE, RUNNING, BLOCKED };

class Worker {
public:
    static Worker* WorkerConstructor(napi_env env);

    void NotifyExecuteTask();

private:
    explicit Worker(napi_env env) : hostEnv_(env) {};

    ~Worker() = default;

    void NotifyIdle();
    void NotifyWorkerCreated();
    void NotifyTaskRunning()
    {
        state_ = WorkerState::RUNNING;
        startTime_ = ConcurrentHelper::GetMilliseconds();
        runningCount_++;
    }

    // the function will only be called when the task is finished or
    // exits abnormally, so we can not put it in the scope directly
    void NotifyTaskFinished();

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

    // we will use the scope to manage resources automatically,
    // including the HandleScope and NotifyRunning/NotifyIdle
    class RunningScope {
    public:
        RunningScope(Worker* worker, napi_status& status) : worker_(worker)
        {
            status = napi_open_handle_scope(worker_->workerEnv_, &scope_);
            worker_->NotifyTaskRunning();
        }

        ~RunningScope();

    private:
        Worker* worker_ = nullptr;
        napi_handle_scope scope_ = nullptr;
    };

    // use PriorityScope to manage the priority setting of workers
    // reset qos_user_initiated when exit PriorityScope
    class PriorityScope {
    public:
        PriorityScope(Worker* worker, Priority taskPriority) : worker_(worker)
        {
            if (taskPriority != worker->priority_) {
                HILOG_DEBUG("taskpool:: reset worker priority to match task priority");
                SetWorkerPriority(taskPriority);
                worker->priority_ = taskPriority;
            }
        }
        ~PriorityScope()
        {
            if (worker_ != nullptr) {
                worker_->ResetWorkerPriority();
            }
        }

    private:
        Worker* worker_ = nullptr;
    };

    void StartExecuteInThread();
    static void ExecuteInThread(const void* data);
    bool IsExceptionPending(napi_env env) const;
    bool PrepareForWorkerInstance();
    void ReleaseWorkerThreadContent();
    void ResetWorkerPriority();
    static void PerformTask(const uv_async_t* req);
    static void TaskResultCallback(NativeEngine* engine, NativeValue* result, bool success, void* data);
    static void NotifyTaskResult(napi_env env, TaskInfo* taskInfo, napi_value result);
    static void ReleaseWorkerHandles(const uv_async_t* req);

    napi_env hostEnv_ {nullptr};
    napi_env workerEnv_ {nullptr};
    uv_async_t* performTaskSignal_ {nullptr};
    uv_async_t* clearWorkerSignal_ {nullptr};
#if !defined(WINDOWS_PLATFORM) && !defined(MAC_PLATFORM)
    uv_async_t* debuggerOnPostTaskSignal_ {nullptr};
    std::function<void()> debuggerTask_;
#endif
    std::unique_ptr<TaskRunner> runner_ {nullptr};

    std::atomic<uint64_t> startTime_ = 0;
    std::atomic<int32_t> runningCount_ = 0;
    std::atomic<uint64_t> idlePoint_ = ConcurrentHelper::GetMilliseconds();
    WorkerState state_ {WorkerState::IDLE};
    std::mutex stateMutex_;
    Priority priority_ {Priority::DEFAULT};

    friend class TaskManager;
};
} // namespace Commonlibrary::Concurrent::TaskPoolModule
#endif // JS_CONCURRENT_MODULE_TASKPOOL_WORKER_H
