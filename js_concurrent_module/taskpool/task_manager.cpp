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

#include "task_manager.h"

#include <thread>

#include "commonlibrary/ets_utils/js_sys_module/timer/timer.h"
#include "helper/concurrent_helper.h"
#include "helper/error_helper.h"
#include "taskpool.h"
#include "utils/log.h"
#include "worker.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
using namespace OHOS::JsSysModule;

static constexpr int8_t HIGH_PRIORITY_TASK_COUNT = 5;
static constexpr int8_t MEDIUM_PRIORITY_TASK_COUNT = 5;
static constexpr int32_t STEP_SIZE = 2;
static constexpr int32_t MAX_TASK_DURATION = 300; // 300: 300ms
static constexpr int32_t MAX_IDLE_TIME = 30000; // 30000: 30s
static constexpr int32_t MAX_RETRY_COUNT = 40; // 40: counter for stopping timer
static constexpr int32_t MIN_THREADS = 1; // 1: min thread num when idle
static constexpr int32_t DEFAULT_THREADS = 3;

TaskManager &TaskManager::GetInstance()
{
    static TaskManager manager;
    return manager;
}

TaskManager::TaskManager()
{
    for (size_t i = 0; i < taskQueues_.size(); i++) {
        std::unique_ptr<ExecuteQueue> taskQueue = std::make_unique<ExecuteQueue>();
        taskQueues_[i] = std::move(taskQueue);
    }
}

TaskManager::~TaskManager()
{
    uv_timer_stop(timer_);
    uv_close(reinterpret_cast<uv_handle_t*>(timer_), [](uv_handle_t* handle) {
        if (handle != nullptr) {
            delete reinterpret_cast<uv_timer_t*>(handle);
            handle = nullptr;
        }
    });

    uv_close(reinterpret_cast<uv_handle_t*>(notifyRestartTimer_), [](uv_handle_t* handle) {
        if (handle != nullptr) {
            delete reinterpret_cast<uv_async_t*>(handle);
            handle = nullptr;
        }
    });

    if (loop_ != nullptr) {
        uv_stop(loop_);
    }

    {
        std::lock_guard<std::recursive_mutex> lock(workersMutex_);
        for (auto &worker : workers_) {
            delete worker;
        }
        workers_.clear();
    }
    {
        std::unique_lock<std::shared_mutex> lock(taskInfosMutex_);
        for (auto &[_, taskInfo] : taskInfos_) {
            delete taskInfo;
        }
        taskInfos_.clear();
    }
}

void TaskManager::UpdateExecutedInfo(uint64_t duration)
{
    totalExecTime_ += duration;
    totalExecCount_++;
}

int32_t TaskManager::ComputeSuitableThreadNum()
{
    if (GetTaskNum() != 0 && totalExecCount_ == 0) {
        return GetThreadNum() + STEP_SIZE;
    } else if (totalExecCount_ == 0) {
        return 0; // no task since created
    }

    auto durationPerTask = static_cast<double>(totalExecTime_) / totalExecCount_;
    return std::ceil(durationPerTask * GetTaskNum() / MAX_TASK_DURATION);
}

void TaskManager::CreateOrDeleteWorkers(int32_t targetNum)
{
    // uv_timer_start should not run on the background frequently when there is no task
    if (targetNum == 0 && retryCount_ >= MAX_RETRY_COUNT) {
        uv_timer_stop(timer_);
        suspend_ = true;
        return;
    } else if (targetNum == 0) {
        retryCount_++;
    }

    int32_t workerCount = GetThreadNum();
    const int32_t maxThreads = std::max(ConcurrentHelper::GetProcessNum() - 1, DEFAULT_THREADS);
    targetNum = targetNum | 1;
    if (workerCount < maxThreads && workerCount < targetNum) {
        int32_t step = std::min(maxThreads, targetNum) - workerCount;
        for (int32_t i = 0; i < step; i++) {
            CreateWorker(hostEnv_);
        }
    } else if (workerCount > MIN_THREADS && workerCount > targetNum) {
        std::lock_guard<std::recursive_mutex> lock(workersMutex_);
        int32_t maxNum = std::max(MIN_THREADS, targetNum);
        int32_t step = std::min(workerCount - maxNum, STEP_SIZE);
        for (int32_t i = 0; i < step; i++) {
            auto iter = std::find_if(idleWorkers_.begin(), idleWorkers_.end(), [this](Worker *worker) {
                auto idleTime = ConcurrentHelper::GetMilliseconds() - worker->idlePoint_;
                return idleTime > MAX_IDLE_TIME && worker->runningCount_ == 0 &&
                    !Timer::HasTimer(worker->workerEnv_) && !HasTaskEnvInfo(worker->workerEnv_);
            });
            if (iter != idleWorkers_.end()) {
                auto item = workers_.find(*iter);
                workers_.erase(item);
                uv_async_send((*iter)->clearWorkerSignal_);
                idleWorkers_.erase(iter);
            }
        }
    }
}

void TaskManager::TriggerLoadBalance(const uv_timer_t *req)
{
    TaskManager& taskManager = TaskManager::GetInstance();
    int32_t targetNum = taskManager.ComputeSuitableThreadNum();
    taskManager.CreateOrDeleteWorkers(targetNum);
}

void TaskManager::RestartTimer(const uv_async_t* req)
{
    TaskManager& taskManager = TaskManager::GetInstance();
    uv_timer_again(taskManager.timer_);
}

void TaskManager::RunTaskManager()
{
    loop_ = uv_default_loop();
    timer_ = new uv_timer_t;
    uv_timer_init(loop_, timer_);
    notifyRestartTimer_ = new uv_async_t;
    uv_timer_start(timer_, reinterpret_cast<uv_timer_cb>(
        TaskManager::TriggerLoadBalance), 0, 1000); // 1000: 1s
    uv_async_init(loop_, notifyRestartTimer_, reinterpret_cast<uv_async_cb>(TaskManager::RestartTimer));
    pthread_setname_np(pthread_self(), "TaskMgrThread");
    uv_run(loop_, UV_RUN_DEFAULT);
    uv_loop_close(loop_);
}

uint32_t TaskManager::GenerateTaskId()
{
    return currentTaskId_++;
}

uint32_t TaskManager::GenerateExecuteId()
{
    return currentExecuteId_++;
}

void TaskManager::StoreTaskEnvInfo(napi_env env)
{
    std::unique_lock<std::shared_mutex> lock(taskEnvInfoMutex_);
    auto iter = taskEnvInfo_.find(env);
    if (iter == taskEnvInfo_.end()) {
        taskEnvInfo_.emplace(env, 1); // 1: default value
    } else {
        iter->second++;
    }
}

void TaskManager::PopTaskEnvInfo(napi_env env)
{
    std::unique_lock<std::shared_mutex> lock(taskEnvInfoMutex_);
    auto iter = taskEnvInfo_.find(env);
    if (iter == taskEnvInfo_.end()) {
        return;
    } else if (--iter->second == 0) {
        taskEnvInfo_.erase(iter);
    }
}

bool TaskManager::HasTaskEnvInfo(napi_env env)
{
    std::shared_lock<std::shared_mutex> lock(taskEnvInfoMutex_);
    return taskEnvInfo_.find(env) != taskEnvInfo_.end();
}

void TaskManager::StoreTaskInfo(uint32_t executeId, TaskInfo* taskInfo)
{
    std::unique_lock<std::shared_mutex> lock(taskInfosMutex_);
    taskInfos_.emplace(executeId, taskInfo);
}

void TaskManager::StoreStateInfo(uint32_t executeId, TaskState state)
{
    std::unique_lock<std::shared_mutex> lock(taskStatesMutex_);
    taskStates_.emplace(executeId, state);
}

void TaskManager::StoreRunningInfo(uint32_t taskId, uint32_t executeId)
{
    std::unique_lock<std::shared_mutex> lock(runningInfosMutex_);
    auto iter = runningInfos_.find(taskId);
    if (iter == runningInfos_.end()) {
        std::list<uint32_t> list {executeId};
        runningInfos_.emplace(taskId, list);
    } else {
        iter->second.push_front(executeId);
    }
}

TaskInfo* TaskManager::PopTaskInfo(uint32_t executeId)
{
    std::unique_lock<std::shared_mutex> lock(taskInfosMutex_);
    auto iter = taskInfos_.find(executeId);
    if (iter == taskInfos_.end() || iter->second == nullptr) {
        return nullptr;
    }

    TaskInfo* taskInfo = iter->second;
    // remove the the taskInfo when executed
    taskInfos_.erase(iter);
    return taskInfo;
}

void TaskManager::PopRunningInfo(uint32_t taskId, uint32_t executeId)
{
    std::unique_lock<std::shared_mutex> lock(runningInfosMutex_);
    auto iter = runningInfos_.find(taskId);
    if (iter == runningInfos_.end()) {
        return;
    }
    iter->second.remove(executeId);
}

TaskState TaskManager::QueryState(uint32_t executeId)
{
    std::shared_lock<std::shared_mutex> lock(taskStatesMutex_);
    auto iter = taskStates_.find(executeId);
    if (iter == taskStates_.end()) {
        HILOG_ERROR("taskpool:: failed to find the target task");
        return TaskState::NOT_FOUND;
    }
    return iter->second;
}

bool TaskManager::UpdateState(uint32_t executeId, TaskState state)
{
    std::unique_lock<std::shared_mutex> lock(taskStatesMutex_);
    auto iter = taskStates_.find(executeId);
    if (iter == taskStates_.end()) {
        return false;
    }
    if (state == TaskState::RUNNING) {
        iter->second = state;
    } else {
        taskStates_.erase(iter);
    }
    return true;
}

void TaskManager::CancelTask(napi_env env, uint32_t taskId)
{
    std::unique_lock<std::shared_mutex> lock(runningInfosMutex_);
    auto iter = runningInfos_.find(taskId);
    if (iter == runningInfos_.end() || iter->second.empty()) {
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_CANCEL_NONEXIST_TASK, "taskpool:: can not find the task");
        return;
    }
    for (auto executeId : iter->second) {
        TaskState state = QueryState(executeId);
        if (state == TaskState::NOT_FOUND) {
            ErrorHelper::ThrowError(env, ErrorHelper::ERR_CANCEL_NONEXIST_TASK, "taskpool:: can not find the task");
            return;
        }
        UpdateState(executeId, TaskState::CANCELED);
        if (state == TaskState::WAITING) {
            TaskInfo* taskInfo = PopTaskInfo(executeId);
            if (taskInfo != nullptr) {
                napi_value undefined = nullptr;
                napi_get_undefined(taskInfo->env, &undefined);
                napi_reject_deferred(taskInfo->env, taskInfo->deferred, undefined);
                PopTaskEnvInfo(taskInfo->env);
                ReleaseTaskContent(taskInfo);
            }
        } else {
            ErrorHelper::ThrowError(env, ErrorHelper::ERR_CANCEL_RUNNING_TASK,
                                "taskpool:: can not cancel the running task");
            return;
        }
    }
    runningInfos_.erase(iter);
}

TaskInfo* TaskManager::GenerateTaskInfo(napi_env env, napi_value func,
                                        napi_value args, uint32_t taskId, uint32_t executeId)
{
    napi_value undefined;
    napi_get_undefined(env, &undefined);
    napi_value serializationFunction;
    napi_status status = napi_serialize(env, func, undefined, &serializationFunction);
    if (status != napi_ok || serializationFunction == nullptr) {
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_WORKER_SERIALIZATION, "taskpool: failed to serialize function.");
        return nullptr;
    }
    napi_value serializationArguments;
    status = napi_serialize(env, args, undefined, &serializationArguments);
    if (status != napi_ok || serializationArguments == nullptr) {
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_WORKER_SERIALIZATION, "taskpool: failed to serialize arguments.");
        return nullptr;
    }
    TaskInfo* taskInfo = new (std::nothrow) TaskInfo();
    taskInfo->env = env;
    taskInfo->executeId = executeId;
    taskInfo->serializationFunction = serializationFunction;
    taskInfo->serializationArguments = serializationArguments;
    taskInfo->taskId = taskId;
    taskInfo->onResultSignal = new uv_async_t;
    uv_loop_t* loop = NapiHelper::GetLibUV(env);
    uv_async_init(loop, taskInfo->onResultSignal, reinterpret_cast<uv_async_cb>(TaskPool::HandleTaskResult));
    taskInfo->onResultSignal->data = taskInfo;

    StoreTaskInfo(executeId, taskInfo);
    StoreTaskEnvInfo(env);
    return taskInfo;
}

void TaskManager::ReleaseTaskContent(TaskInfo* taskInfo)
{
    if (taskInfo->onResultSignal != nullptr &&
        !uv_is_closing(reinterpret_cast<uv_handle_t*>(taskInfo->onResultSignal))) {
        uv_close(reinterpret_cast<uv_handle_t*>(taskInfo->onResultSignal), [](uv_handle_t* handle) {
            if (handle != nullptr) {
                delete reinterpret_cast<uv_async_t*>(handle);
                handle = nullptr;
            }
        });
    }
    delete taskInfo;
    taskInfo = nullptr;
}

void TaskManager::NotifyWorkerIdle(Worker *worker)
{
    {
        std::lock_guard<std::recursive_mutex> lock(workersMutex_);
        idleWorkers_.insert(worker);
    }
    if (GetTaskNum() != 0) {
        NotifyExecuteTask();
    }
}

void TaskManager::NotifyWorkerAdded(Worker *worker)
{
    std::lock_guard<std::recursive_mutex> lock(workersMutex_);
    workers_.insert(worker);
}

uint32_t TaskManager::GetTaskNum()
{
    std::lock_guard<std::mutex> lock(taskQueuesMutex_);
    return taskQueues_[Priority::HIGH]->GetTaskNum() + taskQueues_[Priority::MEDIUM]->GetTaskNum() +
        taskQueues_[Priority::LOW]->GetTaskNum();
}

uint32_t TaskManager::GetThreadNum()
{
    std::lock_guard<std::recursive_mutex> lock(workersMutex_);
    return workers_.size();
}

void TaskManager::EnqueueExecuteId(uint32_t executeId, Priority priority)
{
    // once enqueued, reset the counter to make threads released at given time
    // if timer is stopped and then new tasks enqueue, restart it
    retryCount_ = 0;
    if (suspend_) {
        suspend_ = false;
        uv_async_send(notifyRestartTimer_);
    }

    {
        std::lock_guard<std::mutex> lock(taskQueuesMutex_);
        taskQueues_[priority]->EnqueueExecuteId(executeId);
    }
    NotifyExecuteTask();
}

uint32_t TaskManager::DequeueExecuteId()
{
    std::lock_guard<std::mutex> lock(taskQueuesMutex_);
    if (highPrioExecuteCount_ < HIGH_PRIORITY_TASK_COUNT) {
        auto &highTaskQueue = taskQueues_[Priority::HIGH];
        highPrioExecuteCount_++;
        return highTaskQueue->DequeueExecuteId();
    }
    highPrioExecuteCount_ = 0;

    if (mediumPrioExecuteCount_ < MEDIUM_PRIORITY_TASK_COUNT) {
        auto &mediumTaskQueue = taskQueues_[Priority::MEDIUM];
        mediumPrioExecuteCount_++;
        return mediumTaskQueue->DequeueExecuteId();
    }
    mediumPrioExecuteCount_ = 0;

    auto &lowTaskQueue = taskQueues_[Priority::LOW];
    return lowTaskQueue->DequeueExecuteId();
}

void TaskManager::NotifyExecuteTask()
{
    std::lock_guard<std::recursive_mutex> lock(workersMutex_);
    if (idleWorkers_.empty()) {
        return;
    }
    auto candidator = idleWorkers_.begin();
    Worker *worker = *candidator;
    idleWorkers_.erase(candidator);
    worker->NotifyExecuteTask();
}

void TaskManager::InitTaskManager(napi_env env)
{
    auto hostEngine = reinterpret_cast<NativeEngine*>(env);
    while (!hostEngine->IsMainThread()) {
        hostEngine = hostEngine->GetHostEngine();
    }
    std::lock_guard<std::mutex> lock(initMutex_);
    if (!isInitialized_) {
        isInitialized_ = true;
        hostEnv_ = env;
        // Add a reserved thread for taskpool
        CreateWorker(env);
        // Create a timer to manage worker threads
        std::thread workerManager(&TaskManager::RunTaskManager, this);
        workerManager.detach();
    }
}

void TaskManager::CreateWorker(napi_env env)
{
    auto worker = Worker::WorkerConstructor(env);
    NotifyWorkerAdded(worker);
}
} // namespace Commonlibrary::Concurrent::TaskPoolModule
