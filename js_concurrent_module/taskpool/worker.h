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

#if defined(ENABLE_TASKPOOL_FFRT)
#include "cpp/task.h"
#endif
#include "helper/concurrent_helper.h"
#include "helper/error_helper.h"
#include "helper/napi_helper.h"
#include "helper/object_helper.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "native_engine/native_engine.h"
#include "qos_helper.h"
#include "task.h"
#include "task_runner.h"
#include "tools/log.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
using namespace Commonlibrary::Concurrent::Common;
using namespace Commonlibrary::Concurrent::Common::Helper;
using namespace Commonlibrary::Platform;

enum class WorkerState { IDLE, RUNNING, BLOCKED };

#if defined(ENABLE_TASKPOOL_FFRT)
static const std::map<Priority, int> WORKERPRIORITY_FFRTQOS_MAP = {
    {Priority::IDLE, ffrt::qos_background},
    {Priority::LOW, ffrt::qos_utility},
    {Priority::MEDIUM, ffrt::qos_default},
    {Priority::HIGH, ffrt::qos_user_initiated},
};
#endif

struct ThreadInfo {
    Priority priority {Priority::DEFAULT};
    pid_t tid = 0;
    std::vector<uint32_t> currentTaskId {};
};

class Worker {
public:
    using DebuggerPostTask = std::function<void()>;

    static Worker* WorkerConstructor(napi_env env);

    void NotifyExecuteTask();

    void NotifyTaskBegin();
    // the function will only be called when the task is finished or
    // exits abnormally, so we can not put it in the scope directly
    void NotifyTaskFinished();

    Priority GetPriority() const;

    static void NotifyTaskResult(napi_env env, Task* task, napi_value result);
    static void NotifyHandleTaskResult(Task* task);

#if defined(ENABLE_TASKPOOL_FFRT)
    bool IsLoopActive();
    uint64_t GetWaitTime();
#endif
#if defined(ENABLE_TASKPOOL_HISYSEVENT)
    bool IsNeedReport(uint64_t intervalTime);
    void IncreaseReportCount();
#endif

private:
    explicit Worker(napi_env env) : hostEnv_(env) {};

    ~Worker() = default;

    Worker(const Worker &) = delete;
    Worker& operator=(const Worker &) = delete;
    Worker(Worker &&) = delete;
    Worker& operator=(Worker &&) = delete;

    void NotifyIdle();
    void NotifyWorkerCreated();
    void NotifyTaskRunning()
    {
        state_ = WorkerState::RUNNING;
        startTime_ = ConcurrentHelper::GetMilliseconds();
        runningCount_++;
    }

    bool HasRunningTasks() const
    {
        return runningCount_ != 0;
    }

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
        }
    }

    // we will use the scope to manage resources automatically,
    // including the HandleScope and NotifyRunning/NotifyIdle
    class RunningScope {
    public:
        explicit RunningScope(Worker* worker) : worker_(worker)
        {
            napi_open_handle_scope(worker_->workerEnv_, &scope_);
            worker_->idleState_ = false;
            worker->isExecutingLongTask_ = false;
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
        PriorityScope(Worker* worker, Priority taskPriority);
        ~PriorityScope()
        {
            worker_->ResetWorkerPriority();
        }

    private:
        Worker* worker_ = nullptr;
    };

    void StartExecuteInThread();
    static void ExecuteInThread(const void* data);
    bool PrepareForWorkerInstance();
    void ReleaseWorkerThreadContent();
    void ResetWorkerPriority();
    bool CheckFreeConditions();
    bool UpdateWorkerState(WorkerState expect, WorkerState desired);
    void StoreTaskId(uint32_t taskId);
    bool InitTaskPoolFunc(napi_env env, napi_value func, Task* task);
    void UpdateExecutedInfo();
    void UpdateLongTaskInfo(Task* task);
    bool IsExecutingLongTask();
    bool HasLongTask();
    void TerminateTask(uint32_t taskId);
    void CloseHandles();
    void PostReleaseSignal();
    bool IsRunnable(uint64_t currTime) const;
    void UpdateWorkerWakeUpTime();
    void EraseRunningTaskId(uint32_t taskId);

    static void HandleFunctionResult(napi_env env, Task* task);
    static void PerformTask(const uv_async_t* req);
    static void TaskResultCallback(napi_env env, napi_value result, bool success, void* data);
    static void ReleaseWorkerHandles(const uv_async_t* req);
    static void TriggerGCCheck(const uv_async_t* req);

#if defined(ENABLE_TASKPOOL_FFRT)
    void InitFfrtInfo();
    void InitLoopHandleNum();
#endif

    napi_env hostEnv_ {nullptr};
    napi_env workerEnv_ {nullptr};
    uv_async_t* performTaskSignal_ {nullptr};
    uv_async_t* clearWorkerSignal_ {nullptr};
    uv_async_t* triggerGCCheckSignal_ {nullptr};
#if !defined(WINDOWS_PLATFORM) && !defined(MAC_PLATFORM)
    uv_async_t* debuggerOnPostTaskSignal_ {nullptr};
    std::mutex debuggerMutex_;
    std::queue<DebuggerPostTask> debuggerQueue_ {};
#endif
    std::unique_ptr<TaskRunner> runner_ {nullptr};

    std::atomic<int32_t> runningCount_ = 0;
    std::atomic<bool> idleState_ = true; // true means the worker is idle
    std::atomic<uint64_t> idlePoint_ = ConcurrentHelper::GetMilliseconds();
    std::atomic<uint64_t> startTime_ = ConcurrentHelper::GetMilliseconds();
    std::atomic<uint64_t> wakeUpTime_ = ConcurrentHelper::GetMilliseconds();
    std::atomic<WorkerState> state_ {WorkerState::IDLE};
    std::atomic<bool> hasExecuted_ = false; // false means this worker hasn't execute any tasks
    Priority priority_ {Priority::DEFAULT};
    pid_t tid_ = 0;
    std::vector<uint32_t> currentTaskId_ {};
    std::mutex currentTaskIdMutex_;
    uint64_t lastCpuTime_ = 0;
    uint32_t idleCount_ = 0;
    std::atomic<bool> hasLongTask_ = false;
    std::atomic<bool> isExecutingLongTask_ = false;
    std::mutex longMutex_;
    std::unordered_set<uint32_t> longTasksSet_ {};
    friend class TaskManager;
    friend class NativeEngineTest;

#if defined(ENABLE_TASKPOOL_FFRT)
    void* ffrtTaskHandle_ = nullptr;
    uint32_t initActiveHandleNum_ = 0;
#endif
#if defined(ENABLE_TASKPOOL_HISYSEVENT)
    std::atomic<int32_t> reportCount_ = 0;
#endif
};
} // namespace Commonlibrary::Concurrent::TaskPoolModule
#endif // JS_CONCURRENT_MODULE_TASKPOOL_WORKER_H