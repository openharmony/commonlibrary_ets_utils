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

#include <cinttypes>
#include <securec.h>
#include <thread>

#include "commonlibrary/ets_utils/js_sys_module/timer/timer.h"
#include "helper/concurrent_helper.h"
#include "helper/error_helper.h"
#include "helper/hitrace_helper.h"
#include "taskpool.h"
#include "utils/log.h"
#include "worker.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
using namespace OHOS::JsSysModule;

static constexpr int8_t HIGH_PRIORITY_TASK_COUNT = 5;
static constexpr int8_t MEDIUM_PRIORITY_TASK_COUNT = 5;
static constexpr int32_t MAX_TASK_DURATION = 100; // 100: 100ms
static constexpr uint32_t STEP_SIZE = 2;
static constexpr uint32_t DEFAULT_THREADS = 3;
static constexpr uint32_t MIN_THREADS = 1; // 1: minimum thread num when idle
static constexpr uint32_t MIN_TIMEOUT_TIME = 180000; // 180000: 3min
static constexpr uint32_t MAX_TIMEOUT_TIME = 600000; // 600000: 10min
static constexpr int32_t MAX_IDLE_TIME = 50000; // 50000: 50s
[[maybe_unused]] static constexpr uint32_t IDLE_THRESHOLD = 2; // 2: 2min later will release the thread

// ----------------------------------- TaskManager ----------------------------------------
TaskManager& TaskManager::GetInstance()
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
    if (timer_ == nullptr) {
        HILOG_ERROR("taskpool:: timer_ is nullptr");
    } else {
        uv_timer_stop(timer_);
        ConcurrentHelper::UvHandleClose(timer_);
        ConcurrentHelper::UvHandleClose(expandHandle_);
    }

    if (loop_ != nullptr) {
        uv_stop(loop_);
    }

    {
        std::lock_guard<std::recursive_mutex> lock(workersMutex_);
        for (auto& worker : workers_) {
            delete worker;
        }
        workers_.clear();
    }

    {
        std::unique_lock<std::shared_mutex> lock(tasksMutex_);
        for (auto& [_, task] : tasks_) {
            delete task;
            task = nullptr;
        }
        tasks_.clear();
    }
    CountTraceForWorker();
}

void TaskManager::CountTraceForWorker()
{
    std::lock_guard<std::recursive_mutex> lock(workersMutex_);
    int64_t threadNum = static_cast<int64_t>(workers_.size());
    int64_t idleWorkers = static_cast<int64_t>(idleWorkers_.size());
    int64_t timeoutWorkers = static_cast<int64_t>(timeoutWorkers_.size());
    HITRACE_HELPER_COUNT_TRACE("timeoutThreadNum", timeoutWorkers);
    HITRACE_HELPER_COUNT_TRACE("threadNum", threadNum);
    HITRACE_HELPER_COUNT_TRACE("runningThreadNum", threadNum - idleWorkers);
    HITRACE_HELPER_COUNT_TRACE("idleThreadNum", idleWorkers);
}

napi_value TaskManager::GetThreadInfos(napi_env env)
{
    napi_value threadInfos = nullptr;
    napi_create_array(env, &threadInfos);
    {
        std::lock_guard<std::recursive_mutex> lock(workersMutex_);
        int32_t i = 0;
        for (auto& worker : workers_) {
            if (worker->workerEnv_ == nullptr) {
                continue;
            }
            napi_value tid = nullptr;
            napi_value priority = nullptr;
            napi_create_int32(env, static_cast<int32_t>(worker->tid_), &tid);
            napi_create_int32(env, static_cast<int32_t>(worker->priority_), &priority);

            napi_value taskId = nullptr;
            napi_create_array(env, &taskId);
            int32_t j = 0;
            {
                std::lock_guard<std::mutex> lock(worker->currentTaskIdMutex_);
                for (auto& currentId : worker->currentTaskId_) {
                    napi_value id = NapiHelper::CreateUint64(env, currentId);
                    napi_set_element(env, taskId, j, id);
                    j++;
                }
            }
            napi_value threadInfo = nullptr;
            napi_create_object(env, &threadInfo);
            napi_set_named_property(env, threadInfo, "tid", tid);
            napi_set_named_property(env, threadInfo, "priority", priority);
            napi_set_named_property(env, threadInfo, "taskIds", taskId);
            napi_set_element(env, threadInfos, i, threadInfo);
            i++;
        }
    }
    return threadInfos;
}

napi_value TaskManager::GetTaskInfos(napi_env env)
{
    napi_value taskInfos = nullptr;
    napi_create_array(env, &taskInfos);
    {
        std::unique_lock<std::shared_mutex> lock(tasksMutex_);
        int32_t i = 0;
        for (const auto& [_, task] : tasks_) {
            if (task->taskState_ == ExecuteState::NOT_FOUND) {
                continue;
            }
            napi_value taskInfoValue = nullptr;
            napi_create_object(env, &taskInfoValue);
            std::unique_lock<std::shared_mutex> lock(task->taskMutex_);
            napi_value taskId = NapiHelper::CreateUint64(env, task->taskId_);
            napi_value name = nullptr;
            napi_create_string_utf8(env, task->name_.c_str(), task->name_.size(), &name);
            napi_set_named_property(env, taskInfoValue, "name", name);
            ExecuteState state;
            uint64_t duration = 0;
            if (task->taskState_ == ExecuteState::WAITING) {
                state = ExecuteState::WAITING;
            } else {
                duration = ConcurrentHelper::GetMilliseconds() - task->startTime_;
                state = ExecuteState::RUNNING;
            }
            napi_value stateValue = nullptr;
            napi_create_int32(env, state, &stateValue);
            napi_set_named_property(env, taskInfoValue, "taskId", taskId);
            napi_set_named_property(env, taskInfoValue, "state", stateValue);
            napi_value durationValue = NapiHelper::CreateUint32(env, duration);
            napi_set_named_property(env, taskInfoValue, "duration", durationValue);
            napi_set_element(env, taskInfos, i, taskInfoValue);
            i++;
        }
    }
    return taskInfos;
}

void TaskManager::UpdateExecutedInfo(uint64_t duration)
{
    totalExecTime_ += duration;
    totalExecCount_++;
}

uint32_t TaskManager::ComputeSuitableThreadNum()
{
    uint32_t targetNum = 0;
    if (GetTaskNum() != 0 && totalExecCount_ == 0) {
        // this branch is used for avoiding time-consuming works that may block the taskpool
        targetNum = std::min(STEP_SIZE, GetTaskNum());
    }
    uint32_t result = 0;
    if (totalExecCount_ != 0) {
        auto durationPerTask = static_cast<double>(totalExecTime_) / totalExecCount_;
        result = std::ceil(durationPerTask * GetTaskNum() / MAX_TASK_DURATION);
        targetNum += std::min(result, GetTaskNum());
    }
    targetNum += GetRunningWorkers();
    targetNum |= 1;
    return targetNum;
}

void TaskManager::CheckForBlockedWorkers()
{
    // the threshold will be dynamically modified to provide more flexibility in detecting exceptions
    // if the thread num has reached the limit and the idle worker is not available, a short time will be used,
    // else we will choose the longer one
    std::lock_guard<std::recursive_mutex> lock(workersMutex_);
    bool needChecking = false;
    bool state = (GetThreadNum() == ConcurrentHelper::GetMaxThreads()) && (GetIdleWorkers() == 0);
    uint64_t threshold = state ? MIN_TIMEOUT_TIME : MAX_TIMEOUT_TIME;
    for (auto iter = workers_.begin(); iter != workers_.end(); iter++) {
        auto worker = *iter;
        // if the worker thread is idle, just skip it, and only the worker in running state can be marked as timeout
        if ((worker->state_ == WorkerState::IDLE) ||
            (ConcurrentHelper::GetMilliseconds() - worker->startTime_ < threshold) ||
            !worker->UpdateWorkerState(WorkerState::RUNNING, WorkerState::BLOCKED)) {
            continue;
        }
        // When executing the promise task, the worker state may not be updated and will be
        // marked as 'BLOCKED', so we should exclude this situation.
        // Besides, if the worker is not executing sync tasks or micro tasks, it may handle
        // the task like I/O in uv threads, we should also exclude this situation.
        auto workerEngine = reinterpret_cast<NativeEngine*>(worker->workerEnv_);
        if (worker->idleState_ && !workerEngine->IsExecutingPendingJob()) {
            if (!workerEngine->HasWaitingRequest()) {
                worker->UpdateWorkerState(WorkerState::BLOCKED, WorkerState::IDLE);
            } else {
                worker->UpdateWorkerState(WorkerState::BLOCKED, WorkerState::RUNNING);
                worker->startTime_ = ConcurrentHelper::GetMilliseconds();
            }
            continue;
        }

        HILOG_INFO("taskpool:: The worker has been marked as timeout.");
        needChecking = true;
        workerEngine->TerminateExecution();

        idleWorkers_.erase(worker);
        timeoutWorkers_.insert(worker);
    }
    // should trigger the check when we have marked and removed workers
    if (UNLIKELY(needChecking)) {
        TryExpand();
    }
}

void TaskManager::TryTriggerExpand()
{
    // post the signal to notify the monitor thread to expand
    if (UNLIKELY(expandHandle_ == nullptr)) {
        needChecking_ = true;
        HILOG_DEBUG("taskpool:: the expandHandle_ is nullptr");
        return;
    }
    uv_async_send(expandHandle_);
}

#if defined(OHOS_PLATFORM)
// read /proc/[pid]/task/[tid]/stat to get the number of idle threads.
bool TaskManager::ReadThreadInfo(Worker* worker, char* buf, uint32_t size)
{
    char path[128]; // 128: buffer for path
    pid_t pid = getpid();
    pid_t tid = worker->tid_;
    ssize_t bytesLen = -1;
    int ret = snprintf_s(path, sizeof(path), sizeof(path) - 1, "/proc/%d/task/%d/stat", pid, tid);
    if (ret < 0) {
        HILOG_ERROR("snprintf_s failed");
        return false;
    }
    int fd = open(path, O_RDONLY | O_NONBLOCK);
    if (UNLIKELY(fd == -1)) {
        return false;
    }
    bytesLen = read(fd, buf, size - 1);
    close(fd);
    if (bytesLen <= 0) {
        HILOG_ERROR("taskpool:: failed to read %{public}s", path);
        return false;
    }
    buf[bytesLen] = '\0';
    return true;
}

uint32_t TaskManager::GetIdleWorkers()
{
    char buf[4096]; // 4096: buffer for thread info
    uint32_t idleCount = 0;
    std::lock_guard<std::recursive_mutex> lock(workersMutex_);
    for (auto& worker : idleWorkers_) {
        if (!ReadThreadInfo(worker, buf, sizeof(buf))) {
            continue;
        }
        char state;
        if (sscanf_s(buf, "%*d %*s %c", &state, sizeof(state)) != 1) { // 1: state
            HILOG_ERROR("taskpool: sscanf_s of state failed for %{public}c", state);
            return 0;
        }
        if (state == 'S') {
            idleCount++;
        }
    }
    return idleCount;
}

void TaskManager::GetIdleWorkersList(uint32_t step)
{
    char buf[4096]; // 4096: buffer for thread info
    for (auto& worker : idleWorkers_) {
        if (!ReadThreadInfo(worker, buf, sizeof(buf))) {
            continue;
        }
        char state;
        uint64_t utime;
        if (sscanf_s(buf, "%*d %*s %c %*d %*d %*d %*d %*d %*u %*lu %*lu %*lu %*lu %llu",
            &state, sizeof(state), &utime) != 2) { // 2: state and utime
            HILOG_ERROR("taskpool: sscanf_s of state failed for %{public}d", worker->tid_);
            return;
        }
        if (state != 'S' || utime != worker->lastCpuTime_) {
            worker->idleCount_ = 0;
            worker->lastCpuTime_ = utime;
            continue;
        }
        if (++worker->idleCount_ >= IDLE_THRESHOLD) {
            freeList_.emplace_back(worker);
        }
    }
}

void TaskManager::TriggerShrink(uint32_t step)
{
    GetIdleWorkersList(step);
    step = std::min(step, static_cast<uint32_t>(freeList_.size()));
    uint32_t count = 0;
    for (size_t i = 0; i < freeList_.size(); i++) {
        auto worker = freeList_[i];
        if (worker->state_ != WorkerState::IDLE) { // may in I/O
            continue;
        }
        auto idleTime = ConcurrentHelper::GetMilliseconds() - worker->idlePoint_;
        if (idleTime < MAX_IDLE_TIME || worker->runningCount_ != 0) {
            continue;
        }
        idleWorkers_.erase(worker);
        HILOG_DEBUG("taskpool:: try to release idle thread: %{public}d", worker->tid_);
        uv_async_send(worker->clearWorkerSignal_);
        if (++count == step) {
            break;
        }
    }
    freeList_.clear();
}
#else
uint32_t TaskManager::GetIdleWorkers()
{
    std::lock_guard<std::recursive_mutex> lock(workersMutex_);
    return idleWorkers_.size();
}

void TaskManager::TriggerShrink(uint32_t step)
{
    for (uint32_t i = 0; i < step; i++) {
        // try to free the worker that idle time meets the requirement
        auto iter = std::find_if(idleWorkers_.begin(), idleWorkers_.end(), [](Worker *worker) {
            auto idleTime = ConcurrentHelper::GetMilliseconds() - worker->idlePoint_;
            return idleTime > MAX_IDLE_TIME && worker->runningCount_ == 0;
        });
        // remove it from all sets
        if (iter != idleWorkers_.end()) {
            auto worker = *iter;
            idleWorkers_.erase(worker);
            HILOG_DEBUG("taskpool:: try to release idle thread: %{public}d", worker->tid_);
            uv_async_send(worker->clearWorkerSignal_);
        }
    }
}
#endif

void TaskManager::NotifyShrink(uint32_t targetNum)
{
    uint32_t workerCount = GetThreadNum();
    if (workerCount > MIN_THREADS && workerCount > targetNum) {
        std::lock_guard<std::recursive_mutex> lock(workersMutex_);
        targetNum = std::max(MIN_THREADS, targetNum);
        uint32_t step = std::min(workerCount - targetNum, STEP_SIZE);
        TriggerShrink(step);
    }
    // remove all timeout workers
    std::lock_guard<std::recursive_mutex> lock(workersMutex_);
    for (auto iter = timeoutWorkers_.begin(); iter != timeoutWorkers_.end();) {
        HILOG_DEBUG("taskpool:: try to release timeout thread: %{public}d", (*iter)->tid_);
        uv_async_send((*iter)->clearWorkerSignal_);
        timeoutWorkers_.erase(iter++);
    }
    // stop the timer
    if ((workers_.size() == idleWorkers_.size() && workers_.size() == MIN_THREADS) && timeoutWorkers_.empty()) {
        suspend_ = true;
        uv_timer_stop(timer_);
        HILOG_DEBUG("taskpool:: timer will be suspended");
    }
}

void TaskManager::TriggerLoadBalance(const uv_timer_t* req)
{
    TaskManager& taskManager = TaskManager::GetInstance();
    // do not check when try to expand
    if (taskManager.expandingCount_ != 0) {
        return;
    }

    taskManager.CheckForBlockedWorkers();
    uint32_t targetNum = taskManager.ComputeSuitableThreadNum();
    taskManager.NotifyShrink(targetNum);
    taskManager.CountTraceForWorker();
}

void TaskManager::TryExpand()
{
    if (GetIdleWorkers() != 0) {
        return;
    }
    // for accuracy, if worker is being created, we will not trigger expansion,
    // and the expansion will be triggered until all workers are created
    if (expandingCount_ != 0) {
        needChecking_ = true;
        return;
    }
    needChecking_ = false; // do not need to check
    uint32_t targetNum = ComputeSuitableThreadNum();
    targetNum |= 1;
    uint32_t workerCount = GetThreadNum();
    const uint32_t maxThreads = std::max(ConcurrentHelper::GetMaxThreads(), DEFAULT_THREADS);
    if (workerCount < maxThreads && workerCount < targetNum) {
        uint32_t step = std::min(maxThreads, targetNum) - workerCount;
        CreateWorkers(hostEnv_, step);
        HILOG_INFO("taskpool:: maxThreads: %{public}u, created num: %{public}u, total num: %{public}u",
            maxThreads, step, GetThreadNum());
    }
    if (UNLIKELY(suspend_)) {
        suspend_ = false;
        uv_timer_again(timer_);
    }
}

void TaskManager::NotifyExpand(const uv_async_t* req)
{
    TaskManager& taskManager = TaskManager::GetInstance();
    taskManager.TryExpand();
}

void TaskManager::RunTaskManager()
{
    loop_ = uv_default_loop();
    timer_ = new uv_timer_t;
    uv_timer_init(loop_, timer_);
    expandHandle_ = new uv_async_t;
    uv_timer_start(timer_, reinterpret_cast<uv_timer_cb>(TaskManager::TriggerLoadBalance), 0, 60000); // 60000: 1min
    uv_async_init(loop_, expandHandle_, reinterpret_cast<uv_async_cb>(TaskManager::NotifyExpand));
#if defined IOS_PLATFORM || defined MAC_PLATFORM
    pthread_setname_np("OS_TaskManager");
#else
    pthread_setname_np(pthread_self(), "OS_TaskManager");
#endif
    if (UNLIKELY(needChecking_)) {
        needChecking_ = false;
        uv_async_send(expandHandle_);
    }
    uv_run(loop_, UV_RUN_DEFAULT);
    uv_loop_close(loop_);
}

void TaskManager::CancelTask(napi_env env, uint64_t taskId)
{
    // 1. Cannot find taskInfo by executeId, throw error
    // 2. Find executing taskInfo, skip it
    // 3. Find waiting taskInfo, cancel it
    // 4. Find canceled taskInfo, skip it
    Task* task = GetTask(taskId);
    if (task == nullptr) {
        return;
    }
    std::unique_lock<std::shared_mutex> lock(task->taskMutex_);
    if (task->currentTaskInfo_ == nullptr) {
        HILOG_ERROR("taskpool:: cancel non-existent task");
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_CANCEL_NONEXIST_TASK);
        return;
    }
    ExecuteState state = task->taskState_;
    switch (state) {
        case ExecuteState::NOT_FOUND:
            HILOG_ERROR("taskpool:: cancel non-existent task");
            ErrorHelper::ThrowError(env, ErrorHelper::ERR_CANCEL_NONEXIST_TASK);
            return;
        case ExecuteState::RUNNING:
            task->taskState_ = ExecuteState::CANCELED;
            task->CancelPendingTask(env, ExecuteState::RUNNING);
            break;
        case ExecuteState::WAITING:
            task->taskState_ = ExecuteState::CANCELED;
            task->CancelPendingTask(env, ExecuteState::WAITING);
            break;
        default: // Default is CANCELED, means task isCanceled, do not need to mark again.
            break;
    }
}

void TaskManager::NotifyWorkerIdle(Worker* worker)
{
    {
        std::lock_guard<std::recursive_mutex> lock(workersMutex_);
        if (worker->state_ == WorkerState::BLOCKED) {
            return;
        }
        idleWorkers_.insert(worker);
    }
    if (GetTaskNum() != 0) {
        NotifyExecuteTask();
    }
    CountTraceForWorker();
}

void TaskManager::NotifyWorkerCreated(Worker* worker)
{
    NotifyWorkerIdle(worker);
    expandingCount_--;
    if (UNLIKELY(needChecking_ && expandingCount_ == 0 && expandHandle_ != nullptr)) {
        needChecking_ = false;
        uv_async_send(expandHandle_);
    }
}

void TaskManager::NotifyWorkerAdded(Worker* worker)
{
    std::lock_guard<std::recursive_mutex> lock(workersMutex_);
    workers_.insert(worker);
    HILOG_DEBUG("taskpool:: a new worker has been added and the current num is %{public}zu", workers_.size());
}

void TaskManager::NotifyWorkerRunning(Worker* worker)
{
    std::lock_guard<std::recursive_mutex> lock(workersMutex_);
    idleWorkers_.erase(worker);
    CountTraceForWorker();
}

uint32_t TaskManager::GetRunningWorkers()
{
    std::lock_guard<std::recursive_mutex> lock(workersMutex_);
    return std::count_if(workers_.begin(), workers_.end(), [](const auto& worker) {
        return worker->runningCount_ != 0;
    });
}

uint32_t TaskManager::GetTimeoutWorkers()
{
    std::lock_guard<std::recursive_mutex> lock(workersMutex_);
    return timeoutWorkers_.size();
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

void TaskManager::EnqueueTaskId(uint64_t taskId, Priority priority)
{
    {
        std::lock_guard<std::mutex> lock(taskQueuesMutex_);
        taskQueues_[priority]->EnqueueTaskId(taskId);
    }
    NotifyExecuteTask();
}

std::pair<uint64_t, Priority> TaskManager::DequeueTaskId()
{
    std::lock_guard<std::mutex> lock(taskQueuesMutex_);
    auto& highTaskQueue = taskQueues_[Priority::HIGH];
    if (!highTaskQueue->IsEmpty() && highPrioExecuteCount_ < HIGH_PRIORITY_TASK_COUNT) {
        auto& highTaskQueue = taskQueues_[Priority::HIGH];
        highPrioExecuteCount_++;
        uint64_t taskId = highTaskQueue->DequeueTaskId();
        if (IsDependendByTaskId(taskId)) {
            EnqueuePendingTaskInfo(taskId, Priority::HIGH);
            return std::make_pair(0, Priority::HIGH);
        }
        return std::make_pair(taskId, Priority::HIGH);
    }
    highPrioExecuteCount_ = 0;

    auto& mediumTaskQueue = taskQueues_[Priority::MEDIUM];
    if (!mediumTaskQueue->IsEmpty() && mediumPrioExecuteCount_ < MEDIUM_PRIORITY_TASK_COUNT) {
        mediumPrioExecuteCount_++;
        uint64_t taskId = mediumTaskQueue->DequeueTaskId();
        if (IsDependendByTaskId(taskId)) {
            EnqueuePendingTaskInfo(taskId, Priority::MEDIUM);
            return std::make_pair(0, Priority::MEDIUM);
        }
        return std::make_pair(taskId, Priority::MEDIUM);
    }
    mediumPrioExecuteCount_ = 0;

    auto& lowTaskQueue = taskQueues_[Priority::LOW];
    uint64_t taskId = lowTaskQueue->DequeueTaskId();
    if (IsDependendByTaskId(taskId)) {
        EnqueuePendingTaskInfo(taskId, Priority::LOW);
        return std::make_pair(0, Priority::LOW);
    }
    return std::make_pair(taskId, Priority::LOW);
}

void TaskManager::NotifyExecuteTask()
{
    std::lock_guard<std::recursive_mutex> lock(workersMutex_);
    for (auto& worker : idleWorkers_) {
        worker->NotifyExecuteTask();
    }
}

void TaskManager::InitTaskManager(napi_env env)
{
    HITRACE_HELPER_METER_NAME("InitTaskManager");
    auto hostEngine = reinterpret_cast<NativeEngine*>(env);
    while (hostEngine != nullptr && !hostEngine->IsMainThread()) {
        hostEngine = hostEngine->GetHostEngine();
    }
    if (!isInitialized_.exchange(true, std::memory_order_relaxed)) {
        hostEnv_ = reinterpret_cast<napi_env>(hostEngine);
        // Add a reserved thread for taskpool
        CreateWorkers(hostEnv_);
        // Create a timer to manage worker threads
        std::thread workerManager(&TaskManager::RunTaskManager, this);
        workerManager.detach();
    }
}

void TaskManager::CreateWorkers(napi_env env, uint32_t num)
{
    for (uint32_t i = 0; i < num; i++) {
        expandingCount_++;
        auto worker = Worker::WorkerConstructor(env);
        NotifyWorkerAdded(worker);
    }
    CountTraceForWorker();
}

void TaskManager::RemoveWorker(Worker* worker)
{
    std::lock_guard<std::recursive_mutex> lock(workersMutex_);
    idleWorkers_.erase(worker);
    workers_.erase(worker);
}

void TaskManager::RestoreWorker(Worker* worker)
{
    std::lock_guard<std::recursive_mutex> lock(workersMutex_);
    if (UNLIKELY(suspend_)) {
        suspend_ = false;
        uv_timer_again(timer_);
    }
    if (worker->state_ == WorkerState::BLOCKED) {
        // since the worker is blocked, we should add it to the timeout set
        timeoutWorkers_.insert(worker);
        return;
    }
    // Since the worker may be executing some tasks in IO thread, we should add it to the
    // worker sets and call the 'NotifyWorkerIdle', which can still execute some tasks in its own thread.
    HILOG_DEBUG("taskpool:: worker has been restored and the current num is: %{public}zu", workers_.size());
    idleWorkers_.emplace_hint(idleWorkers_.end(), worker);
    if (GetTaskNum() != 0) {
        NotifyExecuteTask();
    }
}

void TaskManager::RegisterCallback(napi_env env, uint64_t taskId, std::shared_ptr<CallbackInfo> callbackInfo)
{
    std::lock_guard<std::mutex> lock(callbackMutex_);
    callbackTable_[taskId] = callbackInfo;
}

std::shared_ptr<CallbackInfo> TaskManager::GetCallbackInfo(uint64_t taskId)
{
    std::lock_guard<std::mutex> lock(callbackMutex_);
    auto iter = callbackTable_.find(taskId);
    if (iter == callbackTable_.end() || iter->second == nullptr) {
        HILOG_ERROR("taskpool:: the callback does not exist");
        return nullptr;
    }
    return iter->second;
}

void TaskManager::IncreaseRefCount(uint64_t taskId)
{
    if (taskId == 0) { // do not support func
        return;
    }
    std::lock_guard<std::mutex> lock(callbackMutex_);
    auto iter = callbackTable_.find(taskId);
    if (iter == callbackTable_.end() || iter->second == nullptr) {
        return;
    }
    iter->second->refCount++;
}

void TaskManager::DecreaseRefCount(napi_env env, uint64_t taskId)
{
    if (taskId == 0) { // do not support func
        return;
    }
    std::lock_guard<std::mutex> lock(callbackMutex_);
    auto iter = callbackTable_.find(taskId);
    if (iter == callbackTable_.end() || iter->second == nullptr) {
        return;
    }
    iter->second->refCount--;
    if (iter->second->refCount == 0) {
        callbackTable_.erase(iter);
    }
}

napi_value TaskManager::NotifyCallbackExecute(napi_env env, TaskResultInfo* resultInfo, Task* task)
{
    std::lock_guard<std::mutex> lock(callbackMutex_);
    auto iter = callbackTable_.find(task->taskId_);
    if (iter == callbackTable_.end() || iter->second == nullptr) {
        HILOG_ERROR("taskpool:: the callback in SendData is not registered on the host side");
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_NOT_REGISTERED);
        delete resultInfo;
        return nullptr;
    }
    Worker* worker = static_cast<Worker*>(task->worker_);
    worker->Enqueue(resultInfo);
    auto callbackInfo = iter->second;
    callbackInfo->refCount++;
    callbackInfo->onCallbackSignal->data = worker;
    uv_async_send(callbackInfo->onCallbackSignal);
    return nullptr;
}

void TaskManager::NotifyDependencyTaskInfo(uint64_t taskId)
{
    std::unique_lock<std::shared_mutex> lock(dependTaskInfosMutex_);
    auto iter = dependentTaskInfos_.find(taskId);
    if (iter == dependentTaskInfos_.end() || iter->second.empty()) {
        return;
    }
    for (auto taskIdIter = iter->second.begin(); taskIdIter != iter->second.end();) {
        {
            std::unique_lock<std::shared_mutex> lock(tasksMutex_);
            auto taskIter = tasks_.find(*taskIdIter);
            if (taskIter == tasks_.end()) {
                taskIdIter = iter->second.erase(taskIdIter);
                continue;
            }
        }
        auto taskInfo = DequeuePendingTaskInfo(*taskIdIter);
        if (taskInfo.first == 0) {
            taskIdIter = iter->second.erase(taskIdIter);
            continue;
        }
        EnqueueTaskId(taskInfo.first, taskInfo.second);
        auto dependTaskIter = dependTaskInfos_.find(*taskIdIter);
        if (dependTaskIter != dependTaskInfos_.end()) {
            auto dependTaskInnerIter = dependTaskIter->second.find(taskId);
            if (dependTaskInnerIter != dependTaskIter->second.end()) {
                dependTaskIter->second.erase(dependTaskInnerIter);
            }
        }
        taskIdIter = iter->second.erase(taskIdIter);
    }
}

bool TaskManager::IsDependendByTaskId(uint64_t taskId)
{
    std::shared_lock<std::shared_mutex> lock(dependTaskInfosMutex_);
    auto iter = dependTaskInfos_.find(taskId);
    if (iter == dependTaskInfos_.end() || iter->second.empty()) {
        return false;
    }
    return true;
}

bool TaskManager::IsDependentByTaskId(uint64_t dependentTaskId)
{
    std::shared_lock<std::shared_mutex> lock(dependTaskInfosMutex_);
    auto iter = dependentTaskInfos_.find(dependentTaskId);
    if (iter == dependentTaskInfos_.end() || iter->second.empty()) {
        return false;
    }
    return true;
}

bool TaskManager::StoreTaskDependency(uint64_t taskId, std::set<uint64_t> taskIdSet)
{
    std::unique_lock<std::shared_mutex> lock(dependTaskInfosMutex_);
    StoreDependentTaskInfo(taskIdSet, taskId);
    auto iter = dependTaskInfos_.find(taskId);
    if (iter == dependTaskInfos_.end()) {
        for (const auto& dependentId : taskIdSet) {
            auto idIter = dependTaskInfos_.find(dependentId);
            if (idIter == dependTaskInfos_.end()) {
                continue;
            }
            if (!CheckCircularDependency(taskIdSet, idIter->second, taskId)) {
                return false;
            }
        }
        dependTaskInfos_.emplace(taskId, std::move(taskIdSet));
        return true;
    }

    for (const auto& dependentId : iter->second) {
        auto idIter = dependTaskInfos_.find(dependentId);
        if (idIter == dependTaskInfos_.end()) {
            continue;
        }
        if (!CheckCircularDependency(iter->second, idIter->second, taskId)) {
            return false;
        }
    }
    iter->second.insert(taskIdSet.begin(), taskIdSet.end());
    return true;
}

bool TaskManager::CheckCircularDependency(std::set<uint64_t> dependentIdSet, std::set<uint64_t> idSet, uint64_t taskId)
{
    for (const auto& id : idSet) {
        if (id == taskId) {
            return false;
        }
        auto iter = dependentIdSet.find(id);
        if (iter != dependentIdSet.end()) {
            continue;
        }
        auto dIter = dependTaskInfos_.find(id);
        if (dIter == dependTaskInfos_.end()) {
            continue;
        }
        if (!CheckCircularDependency(dependentIdSet, dIter->second, taskId)) {
            return false;
        }
    }
    return true;
}

bool TaskManager::RemoveTaskDependency(uint64_t taskId, uint64_t dependentId)
{
    std::unique_lock<std::shared_mutex> lock(dependTaskInfosMutex_);
    RemoveDependentTaskInfo(dependentId, taskId);
    auto iter = dependTaskInfos_.find(taskId);
    if (iter == dependTaskInfos_.end()) {
        return false;
    }
    auto dependIter = iter->second.find(dependentId);
    if (dependIter ==  iter->second.end()) {
        return false;
    }
    iter->second.erase(dependIter);
    return true;
}

void TaskManager::EnqueuePendingTaskInfo(uint64_t taskId, Priority priority)
{
    if (taskId == 0) {
        return;
    }
    std::unique_lock<std::shared_mutex> lock(pendingTaskInfosMutex_);
    pendingTaskInfos_.emplace(taskId, priority);
}

std::pair<uint64_t, Priority> TaskManager::DequeuePendingTaskInfo(uint64_t taskId)
{
    std::unique_lock<std::shared_mutex> lock(pendingTaskInfosMutex_);
    if (pendingTaskInfos_.empty()) {
        return std::make_pair(0, Priority::DEFAULT);
    }
    std::pair<uint64_t, Priority> result;
    for (auto it = pendingTaskInfos_.begin(); it != pendingTaskInfos_.end(); ++it) {
        if (it->first == taskId) {
            result = std::make_pair(it->first, it->second);
            it = pendingTaskInfos_.erase(it);
            break;
        }
    }
    return result;
}

void TaskManager::RemovePendingTaskInfo(uint64_t taskId)
{
    std::unique_lock<std::shared_mutex> lock(pendingTaskInfosMutex_);
    pendingTaskInfos_.erase(taskId);
}

void TaskManager::StoreDependentTaskInfo(std::set<uint64_t> dependentTaskIdSet, uint64_t taskId)
{
    for (const auto& id : dependentTaskIdSet) {
        auto iter = dependentTaskInfos_.find(id);
        if (iter == dependentTaskInfos_.end()) {
            std::set<uint64_t> set{taskId};
            dependentTaskInfos_.emplace(id, std::move(set));
        } else {
            iter->second.emplace(taskId);
        }
    }
}

void TaskManager::RemoveDependentTaskInfo(uint64_t dependentTaskId, uint64_t taskId)
{
    auto iter = dependentTaskInfos_.find(dependentTaskId);
    if (iter == dependentTaskInfos_.end()) {
        return;
    }
    auto taskIter = iter->second.find(taskId);
    if (taskIter == iter->second.end()) {
        return;
    }
    iter->second.erase(taskIter);
}

void TaskManager::StoreTaskDuration(uint64_t taskId, uint64_t totalDuration, uint64_t cpuDuration)
{
    std::unique_lock<std::shared_mutex> lock(taskDurationInfosMutex_);
    auto iter = taskDurationInfos_.find(taskId);
    if (iter == taskDurationInfos_.end()) {
        std::pair<uint64_t, uint64_t> durationData = std::make_pair(totalDuration, cpuDuration);
        taskDurationInfos_.emplace(taskId, std::move(durationData));
    } else {
        if (totalDuration != 0) {
            iter->second.first = totalDuration;
        }
        if (cpuDuration != 0) {
            iter->second.second = cpuDuration;
        }
    }
}

uint64_t TaskManager::GetTaskDuration(uint64_t taskId, std::string durationType)
{
    std::unique_lock<std::shared_mutex> lock(taskDurationInfosMutex_);
    auto iter = taskDurationInfos_.find(taskId);
    if (iter == taskDurationInfos_.end()) {
        return 0;
    }
    if (durationType == TASK_TOTAL_TIME) {
        return iter->second.first;
    } else if (durationType == TASK_CPU_TIME) {
        return iter->second.second;
    } else if (iter->second.first == 0) {
        return 0;
    }
    return iter->second.first - iter->second.second;
}

void TaskManager::RemoveTaskDuration(uint64_t taskId)
{
    std::unique_lock<std::shared_mutex> lock(taskDurationInfosMutex_);
    auto iter = taskDurationInfos_.find(taskId);
    if (iter != taskDurationInfos_.end()) {
        taskDurationInfos_.erase(iter);
    }
}

void TaskManager::ReleaseTaskData(napi_env env, Task* task)
{
    uint64_t taskId = task->taskId_;
    RemoveTask(taskId);
    if (task->IsFunctionTask() || task->IsGroupFunctionTask()) {
        return;
    }
    DecreaseRefCount(env, taskId);
    RemoveTaskDuration(taskId);
    if (!task->IsCommonTask()) {
        return;
    }
    RemovePendingTaskInfo(taskId);
    std::unique_lock<std::shared_mutex> lock(dependTaskInfosMutex_);
    for (auto dependentTaskIter = dependentTaskInfos_.begin(); dependentTaskIter != dependentTaskInfos_.end();) {
        if (dependentTaskIter->second.find(taskId) != dependentTaskIter->second.end()) {
            dependentTaskIter = dependentTaskInfos_.erase(dependentTaskIter);
        } else {
            ++dependentTaskIter;
        }
    }
    auto dependTaskIter = dependTaskInfos_.find(taskId);
    if (dependTaskIter != dependTaskInfos_.end()) {
        dependTaskInfos_.erase(dependTaskIter);
    }
}

void TaskManager::StoreTask(uint64_t taskId, Task* task)
{
    std::unique_lock<std::shared_mutex> lock(tasksMutex_);
    tasks_.emplace(taskId, task);
}

void TaskManager::RemoveTask(uint64_t taskId)
{
    std::unique_lock<std::shared_mutex> lock(tasksMutex_);
    tasks_.erase(taskId);
}

Task* TaskManager::GetTask(uint64_t taskId)
{
    std::unique_lock<std::shared_mutex> lock(tasksMutex_);
    auto iter = tasks_.find(taskId);
    if (iter == tasks_.end()) {
        return nullptr;
    }
    return iter->second;
}

// ----------------------------------- TaskGroupManager ----------------------------------------
TaskGroupManager& TaskGroupManager::GetInstance()
{
    static TaskGroupManager groupManager;
    return groupManager;
}

void TaskGroupManager::AddTask(uint64_t groupId, napi_ref taskRef, uint64_t taskId)
{
    auto groupIter = taskGroups_.find(groupId);
    if (groupIter == taskGroups_.end()) {
        HILOG_DEBUG("taskpool:: taskGroup has been released");
        return;
    }
    auto taskGroup = reinterpret_cast<TaskGroup*>(groupIter->second);
    if (taskGroup == nullptr) {
        HILOG_ERROR("taskpool:: taskGroup is null");
        return;
    }
    taskGroup->taskRefs_.push_back(taskRef);
    taskGroup->taskNum_++;
    taskGroup->taskIds_.push_back(taskId);
}

void TaskGroupManager::ReleaseTaskGroupData(napi_env env, TaskGroup* group)
{
    TaskGroupManager::GetInstance().RemoveTaskGroup(group->groupId_);
    for (uint64_t taskId : group->taskIds_) {
        Task* task = TaskManager::GetInstance().GetTask(taskId);
        if (task == nullptr) {
            continue;
        }
        napi_reference_unref(task->env_, task->taskRef_, nullptr);
    }
}

void TaskGroupManager::CancelGroup(napi_env env, uint64_t groupId)
{
    TaskGroup* taskGroup = GetTaskGroup(groupId);
    if (taskGroup == nullptr) {
        HILOG_ERROR("taskpool:: CancelGroup group is nullptr");
        return;
    }
    {
        std::unique_lock<std::shared_mutex> lock(taskGroup->taskGroupMutex_);
        if (taskGroup->currentGroupInfo_ == nullptr) {
            HILOG_ERROR("taskpool:: cancel non-existent task group");
            ErrorHelper::ThrowError(env, ErrorHelper::ERR_CANCEL_NONEXIST_TASK_GROUP);
            return;
        }
        taskGroup->CancelPendingGroup(env);
    }
    if (taskGroup->groupState_ == ExecuteState::NOT_FOUND) {
        HILOG_ERROR("taskpool:: cancel non-existent task group");
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_CANCEL_NONEXIST_TASK_GROUP);
        return;
    }
    if (taskGroup->groupState_ == ExecuteState::CANCELED) {
        return;
    }
    if (taskGroup->currentGroupInfo_->finishedTask != taskGroup->taskNum_) {
        for (uint64_t taskId : taskGroup->taskIds_) {
            CancelGroupTask(env, taskId, taskGroup);
        }
    }
    taskGroup->groupState_ = ExecuteState::CANCELED;
}

void TaskGroupManager::CancelGroupTask(napi_env env, uint64_t taskId, TaskGroup* group)
{
    auto task = TaskManager::GetInstance().GetTask(taskId);
    if (task == nullptr) {
        HILOG_INFO("taskpool:: CancelGroupTask task is nullptr");
        return;
    }
    std::unique_lock<std::shared_mutex> lock(task->taskMutex_);
    ExecuteState state = task->taskState_;
    switch (state) {
        case ExecuteState::NOT_FOUND:
            return;
        case ExecuteState::RUNNING:
            task->taskState_ = ExecuteState::CANCELED;
            break;
        case ExecuteState::WAITING:
            task->taskState_ = ExecuteState::CANCELED;
            if (group->groupState_ == ExecuteState::WAITING) {
                group->groupState_ = ExecuteState::CANCELED;
                break;
            }
            if (task->currentTaskInfo_ != nullptr) {
                delete task->currentTaskInfo_;
                task->currentTaskInfo_ = nullptr;
            }
            break;
        default:
            break;
    }
}

void TaskGroupManager::StoreSequenceRunner(uint64_t seqRunnerId, SequenceRunner* seqRunner)
{
    std::unique_lock<std::mutex> lock(seqRunnersMutex_);
    seqRunners_.emplace(seqRunnerId, seqRunner);
}

void TaskGroupManager::RemoveSequenceRunner(uint64_t seqRunnerId)
{
    std::unique_lock<std::mutex> lock(seqRunnersMutex_);
    seqRunners_.erase(seqRunnerId);
}

SequenceRunner* TaskGroupManager::GetSeqRunner(uint64_t seqRunnerId)
{
    std::unique_lock<std::mutex> lock(seqRunnersMutex_);
    auto iter = seqRunners_.find(seqRunnerId);
    if (iter != seqRunners_.end()) {
        return iter->second;
    }
    HILOG_DEBUG("taskpool:: sequenceRunner has been released.");
    return nullptr;
}

void TaskGroupManager::AddTaskToSeqRunner(uint64_t seqRunnerId, Task* task)
{
    std::unique_lock<std::mutex> lock(seqRunnersMutex_);
    auto iter = seqRunners_.find(seqRunnerId);
    if (iter == seqRunners_.end()) {
        HILOG_ERROR("seqRunner:: seqRunner not found.");
        return;
    } else {
        std::unique_lock<std::shared_mutex> seqRunnerLock(iter->second->seqRunnerMutex_);
        iter->second->seqRunnerTasks_.push(task);
    }
}

bool TaskGroupManager::TriggerSeqRunner(napi_env env, Task* lastTask)
{
    uint64_t seqRunnerId = lastTask->seqRunnerId_;
    SequenceRunner* seqRunner = GetSeqRunner(seqRunnerId);
    if (seqRunner == nullptr) {
        HILOG_ERROR("seqRunner:: trigger seqRunner not exist.");
        return false;
    }
    napi_reference_unref(env, seqRunner->seqRunnerRef_, nullptr);
    if (seqRunner->currentTaskId_ != lastTask->taskId_) {
        HILOG_ERROR("seqRunner:: only front task can trigger seqRunner.");
        return false;
    }
    {
        std::unique_lock<std::shared_mutex> lock(seqRunner->seqRunnerMutex_);
        if (seqRunner->seqRunnerTasks_.empty()) {
            HILOG_DEBUG("seqRunner:: seqRunner %" PRIu64 " empty.", seqRunnerId);
            seqRunner->currentTaskId_ = 0;
            return true;
        }
        Task* task = seqRunner->seqRunnerTasks_.front();
        seqRunner->seqRunnerTasks_.pop();
        while (task->taskState_ == ExecuteState::CANCELED) {
            if (seqRunner->seqRunnerTasks_.empty()) {
                HILOG_DEBUG("seqRunner:: seqRunner %" PRIu64 " empty in cancel loop.", seqRunnerId);
                seqRunner->currentTaskId_ = 0;
                return true;
            }
            task = seqRunner->seqRunnerTasks_.front();
            seqRunner->seqRunnerTasks_.pop();
        }
        seqRunner->currentTaskId_ = task->taskId_;
        task->IncreaseRefCount();
        HILOG_DEBUG("seqRunner:: Trig task %" PRIu64 " in seqRunner %" PRIu64 ".", task->taskId_, seqRunnerId);
        TaskManager::GetInstance().EnqueueTaskId(task->taskId_, seqRunner->priority_);
        TaskManager::GetInstance().TryTriggerExpand();
    }
    return true;
}

void TaskGroupManager::StoreTaskGroup(uint64_t groupId, TaskGroup* taskGroup)
{
    std::lock_guard<std::mutex> lock(taskGroupsMutex_);
    taskGroups_.emplace(groupId, taskGroup);
}

void TaskGroupManager::RemoveTaskGroup(uint64_t groupId)
{
    std::lock_guard<std::mutex> lock(taskGroupsMutex_);
    taskGroups_.erase(groupId);
}

TaskGroup* TaskGroupManager::GetTaskGroup(uint64_t groupId)
{
    std::lock_guard<std::mutex> lock(taskGroupsMutex_);
    auto groupIter = taskGroups_.find(groupId);
    if (groupIter == taskGroups_.end()) {
        return nullptr;
    }
    return reinterpret_cast<TaskGroup*>(groupIter->second);
}

void TaskGroupManager::UpdateGroupState(uint64_t groupId)
{
    TaskGroup* group = GetTaskGroup(groupId);
    if (group == nullptr) {
        HILOG_DEBUG("taskpool:: UpdateGroupState taskGroup has been released");
        return;
    }
    group->groupState_ = ExecuteState::RUNNING;
}
} // namespace Commonlibrary::Concurrent::TaskPoolModule
