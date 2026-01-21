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

#include <securec.h>
#include <thread>

#include "async_runner_manager.h"
#include "tools/log.h"
#if defined(ENABLE_TASKPOOL_FFRT)
#include "bundle_info.h"
#include "bundle_mgr_interface.h"
#include "bundle_mgr_proxy.h"
#include "iservice_registry.h"
#include "parameters.h"
#include "status_receiver_interface.h"
#include "system_ability_definition.h"
#include "c/executor_task.h"
#include "ffrt_inner.h"
#endif
#include "log_manager.h"
#include "sys_timer.h"
#include "helper/hitrace_helper.h"
#include "taskpool.h"
#include "task_group_manager.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
using namespace OHOS::JsSysModule;
template void TaskManager::TryExpandWithCheckIdle<true>();
template void TaskManager::TryExpandWithCheckIdle<false>();

static constexpr int8_t HIGH_PRIORITY_TASK_COUNT = 5;
static constexpr int8_t MEDIUM_PRIORITY_TASK_COUNT = 5;
static constexpr int32_t MAX_TASK_DURATION = 100; // 100: 100ms
static constexpr uint32_t STEP_SIZE = 2;
static constexpr uint32_t DEFAULT_THREADS = 3;
static constexpr uint32_t DEFAULT_MIN_THREADS = 1; // 1: minimum thread num when idle
static constexpr uint32_t MIN_TIMEOUT_TIME = 180000; // 180000: 3min
static constexpr uint32_t MAX_TIMEOUT_TIME = 600000; // 600000: 10min
static constexpr int32_t MAX_IDLE_TIME = 30000; // 30000: 30s
static constexpr uint32_t TRIGGER_INTERVAL = 30000; // 30000: 30s
static constexpr uint32_t SHRINK_STEP = 4; // 4: try to release 4 threads every time
static constexpr uint32_t MAX_UINT32_T = 0xFFFFFFFF; // 0xFFFFFFFF: max uint32_t
static constexpr uint32_t TRIGGER_EXPAND_INTERVAL = 10; // 10: ms, trigger recheck expansion interval
[[maybe_unused]] static constexpr uint32_t IDLE_THRESHOLD = 2; // 2: 2 intervals later will release the thread
static constexpr char ON_CALLBACK_STR[] = "TaskPoolOnCallbackTask";
static constexpr char ON_ENQUEUE_STR[] = "TaskPoolOnEnqueueTask";
static constexpr char ON_START_STR[] = "TaskPoolOnStartTask";
static constexpr uint32_t UNEXECUTE_TASK_TIME = 60000; // 60000: 1min
static constexpr uint32_t LOG_INTERVAL = 100; // 100: 100ms
static constexpr uint32_t WAITING_INTERVAL = 1000; // 1000: 1s

#if defined(ENABLE_TASKPOOL_EVENTHANDLER)
static const std::map<Priority, OHOS::AppExecFwk::EventQueue::Priority> TASK_EVENTHANDLER_PRIORITY_MAP = {
    {Priority::IDLE, OHOS::AppExecFwk::EventQueue::Priority::IDLE},
    {Priority::LOW, OHOS::AppExecFwk::EventQueue::Priority::LOW},
    {Priority::MEDIUM, OHOS::AppExecFwk::EventQueue::Priority::HIGH},
    {Priority::HIGH, OHOS::AppExecFwk::EventQueue::Priority::IMMEDIATE},
};
#endif

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
    HILOG_INFO("taskpool:: ~TaskManager");
    TimerStop(balanceTimer_, "taskpool:: balanceTimer_ is nullptr");
    TimerStop(expandTimer_, "taskpool:: expandTimer_ is nullptr");
    TimerStop(logTimer_, "taskpool:: logTimer_ is nullptr");
    TimerStop(waitingTimer_, "taskpool:: waitingTimer_ is nullptr");

    ConcurrentHelper::UvHandleClose(dispatchHandle_);

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
        std::lock_guard<std::mutex> lock(callbackMutex_);
        for (auto& [_, callbackPtr] : callbackTable_) {
            if (callbackPtr == nullptr) {
                continue;
            }
            callbackPtr.reset();
        }
        callbackTable_.clear();
    }

    {
        std::lock_guard<std::recursive_mutex> lock(tasksMutex_);
        for (auto& [_, task] : tasks_) {
            delete task;
            task = nullptr;
        }
        tasks_.clear();
        runningTasks_.clear();
    }
    CountTraceForWorker();
}

void TaskManager::TimerInit(uv_timer_t*& timer, bool startFlag, uv_timer_cb cb, uint64_t repeat)
{
    timer = new uv_timer_t;
    int err = uv_timer_init(loop_, timer);
    if (err < 0) {
        delete timer;
        timer = nullptr;
        HILOG_ERROR("taskpool:: timer init failed");
        return;
    }
    if (startFlag) {
        uv_timer_start(timer, cb, 0, repeat);
    }
}

void TaskManager::TimerStop(uv_timer_t*& timer, const char* errMessage)
{
    if (timer == nullptr) {
        HILOG_ERROR("%{public}s", errMessage);
    } else {
        uv_timer_stop(timer);
        ConcurrentHelper::UvHandleClose(timer);
    }
}

void TaskManager::CountTraceForWorker(bool needLog)
{
    std::lock_guard<std::recursive_mutex> lock(workersMutex_);
    CountTraceForWorkerWithoutLock(needLog);
}

inline void TaskManager::CountTraceForWorkerWithoutLock(bool needLog)
{
    int64_t threadNum = static_cast<int64_t>(workers_.size());
    int64_t idleWorkers = static_cast<int64_t>(idleWorkers_.size());
    int64_t timeoutWorkers = static_cast<int64_t>(timeoutWorkers_.size());
    HITRACE_HELPER_COUNT_TRACE("timeoutThreadNum", timeoutWorkers);
    HITRACE_HELPER_COUNT_TRACE("threadNum", threadNum);
    HITRACE_HELPER_COUNT_TRACE("runningThreadNum", threadNum - idleWorkers);
    HITRACE_HELPER_COUNT_TRACE("idleThreadNum", idleWorkers);
    AddCountTraceForWorkerLog(needLog, threadNum, idleWorkers, timeoutWorkers);
}

napi_value TaskManager::GetThreadInfos(napi_env env)
{
    napi_value threadInfos = nullptr;
    napi_create_array(env, &threadInfos);
    std::unordered_set<std::unique_ptr<ThreadInfo>> threadInfoSet {};
    {
        std::lock_guard<std::recursive_mutex> lock(workersMutex_);
        for (auto& worker : workers_) {
            if (worker->workerEnv_ == nullptr) {
                continue;
            }
            auto threadInfo = std::make_unique<ThreadInfo>();
            threadInfo->tid = worker->tid_;
            threadInfo->priority = worker->priority_;
            for (auto& id : worker->currentTaskId_) {
                threadInfo->currentTaskId.emplace_back(id);
            }
            threadInfoSet.emplace(std::move(threadInfo));
        }
    }
    int32_t i = 0;
    for (auto& info : threadInfoSet) {
        napi_value tid = NapiHelper::CreateUint32(env, static_cast<uint32_t>(info->tid));
        napi_value priority = NapiHelper::CreateUint32(env, static_cast<uint32_t>(info->priority));

        napi_value taskId = nullptr;
        napi_create_array(env, &taskId);
        int32_t j = 0;
        for (auto& currentId : info->currentTaskId) {
            napi_value id = NapiHelper::CreateUint32(env, currentId);
            napi_set_element(env, taskId, j, id);
            j++;
        }
        napi_value threadInfo = nullptr;
        napi_create_object(env, &threadInfo);
        napi_set_named_property(env, threadInfo, "tid", tid);
        napi_set_named_property(env, threadInfo, "priority", priority);
        napi_set_named_property(env, threadInfo, "taskIds", taskId);
        napi_set_element(env, threadInfos, i, threadInfo);
        i++;
    }
    return threadInfos;
}

napi_value TaskManager::GetTaskInfos(napi_env env)
{
    napi_value taskInfos = nullptr;
    napi_create_array(env, &taskInfos);
    std::unordered_set<std::unique_ptr<TaskCurrentInfo>> taskCurrentInfoSet {};
    {
        std::lock_guard<std::recursive_mutex> lock(tasksMutex_);
        for (const auto& [_, task] : tasks_) {
            if (task->taskState_ == ExecuteState::NOT_FOUND || task->taskState_ == ExecuteState::DELAYED ||
                task->taskState_ == ExecuteState::FINISHED) {
                continue;
            }
            auto taskCurrentInfo = std::make_unique<TaskCurrentInfo>();
            taskCurrentInfo->taskId = task->taskId_;
            taskCurrentInfo->name = task->name_;
            taskCurrentInfo->taskState = task->taskState_;
            taskCurrentInfo->startTime = task->startTime_;
            taskCurrentInfoSet.emplace(std::move(taskCurrentInfo));
        }
    }
    int32_t index = 0;
    for (auto& info : taskCurrentInfoSet) {
        napi_value taskInfoValue = NapiHelper::CreateObject(env);
        napi_value taskId = NapiHelper::CreateUint32(env, info->taskId);
        napi_value name = nullptr;
        napi_create_string_utf8(env, info->name.c_str(), info->name.size(), &name);
        napi_set_named_property(env, taskInfoValue, "name", name);
        ExecuteState state = info->taskState;
        uint64_t duration = 0;
        if (state == ExecuteState::RUNNING || state == ExecuteState::ENDING) {
            duration = ConcurrentHelper::GetMilliseconds() - info->startTime;
        }
        napi_value stateValue = NapiHelper::CreateUint32(env, static_cast<uint32_t>(state));
        napi_set_named_property(env, taskInfoValue, "taskId", taskId);
        napi_set_named_property(env, taskInfoValue, "state", stateValue);
        napi_value durationValue = NapiHelper::CreateUint32(env, duration);
        napi_set_named_property(env, taskInfoValue, "duration", durationValue);
        napi_set_element(env, taskInfos, index, taskInfoValue);
        index++;
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
    uint32_t targetNum = ComputeSuitableIdleNum() + GetRunningWorkers();
    return targetNum;
}

uint32_t TaskManager::ComputeSuitableIdleNum()
{
    uint32_t targetNum = 0;
    if (GetNonIdleTaskNum() != 0 && totalExecCount_ == 0) {
        // this branch is used for avoiding time-consuming tasks that may block the taskpool
        targetNum = std::min(STEP_SIZE, GetNonIdleTaskNum());
    } else if (totalExecCount_ != 0) {
        auto durationPerTask = static_cast<double>(totalExecTime_) / totalExecCount_;
        uint32_t result = std::ceil(durationPerTask * GetNonIdleTaskNum() / MAX_TASK_DURATION);
        targetNum = std::min(result, GetNonIdleTaskNum());
    }
    return targetNum | 1;
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
        // if the worker is executing the longTask, we will not do the check
        if ((worker->state_ == WorkerState::IDLE) || (worker->IsExecutingLongTask()) ||
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
        // If the current worker has a longTask and is not executing, we will only interrupt it.
        if (worker->HasLongTask()) {
            continue;
        }
        needChecking = true;
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
    if (UNLIKELY(!isHandleInited_)) {
        NotifyExecuteTask();
        needChecking_ = true;
        HILOG_WARN("taskpool:: the dispatchHandle_ is nullptr");
        return;
    }
    uv_async_send(dispatchHandle_);
}

#if defined(OHOS_PLATFORM)
// read /proc/[pid]/task/[tid]/stat to get the number of idle threads.
bool TaskManager::ReadThreadInfo(pid_t tid, char* buf, uint32_t size)
{
    char path[128]; // 128: buffer for path
    pid_t pid = getpid();
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
    std::unordered_set<pid_t> tids {};
    {
        std::lock_guard<std::recursive_mutex> lock(workersMutex_);
        for (auto& worker : idleWorkers_) {
#if defined(ENABLE_TASKPOOL_FFRT)
            if (worker->ffrtTaskHandle_ != nullptr) {
                if (worker->GetWaitTime() > 0) {
                    idleCount++;
                }
                continue;
            }
#endif
            tids.emplace(worker->tid_);
        }
    }
    // The ffrt thread does not read thread info
    for (auto tid : tids) {
        if (!ReadThreadInfo(tid, buf, sizeof(buf))) {
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
#if defined(ENABLE_TASKPOOL_FFRT)
        if (worker->ffrtTaskHandle_ != nullptr) {
            uint64_t workerWaitTime = worker->GetWaitTime();
            bool isWorkerLoopActive = worker->IsLoopActive();
            if (workerWaitTime == 0) {
                continue;
            }
            uint64_t currTime = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::steady_clock::now().time_since_epoch()).count());
            if (!isWorkerLoopActive) {
                freeList_.emplace_back(worker);
            } else {
                // worker uv alive, and will be free in 2 intervals if not wake
                WorkerAliveAndReport(worker);
            }
            continue;
        }
#endif
        if (!ReadThreadInfo(worker->tid_, buf, sizeof(buf))) {
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
        if (worker->state_ != WorkerState::IDLE || worker->HasLongTask()) {
            continue;
        }
        auto idleTime = ConcurrentHelper::GetMilliseconds() - worker->idlePoint_;
        if (idleTime < MAX_IDLE_TIME || worker->HasRunningTasks()) {
            continue;
        }
        idleWorkers_.erase(worker);
        HILOG_DEBUG("taskpool:: try to release idle thread: %{public}d", worker->tid_);
        worker->PostReleaseSignal();
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
        auto iter = std::find_if(idleWorkers_.begin(), idleWorkers_.end(), [](Worker* worker) {
            auto idleTime = ConcurrentHelper::GetMilliseconds() - worker->idlePoint_;
            return idleTime > MAX_IDLE_TIME && !worker->HasRunningTasks() && !worker->HasLongTask();
        });
        // remove it from all sets
        if (iter != idleWorkers_.end()) {
            auto worker = *iter;
            idleWorkers_.erase(worker);
            HILOG_DEBUG("taskpool:: try to release idle thread: %{public}d", worker->tid_);
            worker->PostReleaseSignal();
        }
    }
}
#endif

void TaskManager::NotifyShrink(uint32_t targetNum)
{
    std::lock_guard<std::recursive_mutex> lock(workersMutex_);
    uint32_t workerCount = workers_.size();
    uint32_t minThread = ConcurrentHelper::IsLowMemory() ? 0 : DEFAULT_MIN_THREADS;
    // update the maxThreads_ periodically
    maxThreads_ = ConcurrentHelper::GetMaxThreads();
    if (minThread == 0) { // LCOV_EXCL_BR_LINE
        HILOG_DEBUG("taskpool::low mem");
    }
    if (workerCount > minThread && workerCount > targetNum) {
        targetNum = std::max(minThread, targetNum);
        uint32_t step = std::min(workerCount - targetNum, SHRINK_STEP);
        TriggerShrink(step);
    }
    // remove all timeout workers
    for (auto iter = timeoutWorkers_.begin(); iter != timeoutWorkers_.end();) {
        auto worker = *iter;
        if (workers_.find(worker) == workers_.end()) {
            HILOG_WARN("taskpool:: current worker maybe release");
            iter = timeoutWorkers_.erase(iter);
        } else if (!worker->HasRunningTasks()) {
            HILOG_DEBUG("taskpool:: try to release timeout thread: %{public}d", worker->tid_);
            worker->PostReleaseSignal();
            timeoutWorkers_.erase(iter++);
            return;
        } else {
            iter++;
        }
    }
    uint32_t idleNum = idleWorkers_.size();
    // System memory state is moderate and the worker has exeuted tasks, we will try to release it
    if (ConcurrentHelper::IsModerateMemory() && workerCount == idleNum && workerCount == DEFAULT_MIN_THREADS) {
        auto worker = *(idleWorkers_.begin());
        // worker that has longTask should not be released
        if (worker == nullptr || worker->HasLongTask()) {
            return;
        }
        if (worker->hasExecuted_) { // worker that hasn't execute any tasks should not be released
            TriggerShrink(DEFAULT_MIN_THREADS);
            return;
        }
    }

    // Create a worker for performance
    if (!ConcurrentHelper::IsLowMemory() && workers_.empty()) {
        CreateWorkers(hostEnv_);
    }
    // stop the timer
    if ((workerCount == idleNum && workerCount <= minThread) && timeoutWorkers_.empty()) {
        suspend_ = true;
        uv_timer_stop(balanceTimer_);
        HILOG_DEBUG("taskpool:: timer will be suspended");
    }
}

void TaskManager::TriggerLoadBalance([[maybe_unused]] const uv_timer_t* req)
{
    TaskManager& taskManager = TaskManager::GetInstance();
    taskManager.CheckForBlockedWorkers();
    uint32_t targetNum = taskManager.ComputeSuitableThreadNum();
    taskManager.NotifyShrink(targetNum);
    taskManager.CountTraceForWorker(true);
}

void TaskManager::PrintLog([[maybe_unused]] const uv_timer_t* req)
{
    TaskManager& taskManager = TaskManager::GetInstance();
    taskManager.logManager_.PrintLog();
}

void TaskManager::PrintWaitingTime([[maybe_unused]] const uv_timer_t* req)
{
    TaskManager& taskManager = TaskManager::GetInstance();
    std::string currentTimeStamp = ConcurrentHelper::GetCurrentTimeStampWithMS();
    uint32_t highTaskId = 0;
    uint32_t middleTaskId = 0;
    uint32_t lowTaskId = 0;
    {
        std::lock_guard<std::mutex> lock(taskManager.taskQueuesMutex_);
        auto& highTaskQueue = taskManager.taskQueues_[Priority::HIGH];
        if (!highTaskQueue->IsEmpty()) {
            highTaskId = highTaskQueue->GetHead();
        }

        auto& mediumTaskQueue = taskManager.taskQueues_[Priority::MEDIUM];
        if (!mediumTaskQueue->IsEmpty()) {
            middleTaskId = mediumTaskQueue->GetHead();
        }

        auto& lowTaskQueue = taskManager.taskQueues_[Priority::LOW];
        if (!lowTaskQueue->IsEmpty()) {
            lowTaskId = lowTaskQueue->GetHead();
        }
    }

    std::string highTime = taskManager.GetTaskEnqueueTime(highTaskId);
    std::string middleTime = taskManager.GetTaskEnqueueTime(middleTaskId);
    std::string lowTime = taskManager.GetTaskEnqueueTime(lowTaskId);

    std::ostringstream oss;
    oss << "currentT " << currentTimeStamp;

    bool hasEntries = false;
    if (!highTime.empty()) {
        oss << ", highEnqueueT " << highTime;
        hasEntries = true;
    }
    if (!middleTime.empty()) {
        oss << ", middleEnqueueT " << middleTime;
        hasEntries = true;
    }
    if (!lowTime.empty()) {
        oss << ", lowEnqueueT " << lowTime;
        hasEntries = true;
    }

    if (hasEntries) {
        HILOG_INFO("taskpool::%{public}s", oss.str().c_str());
    } else {
        HILOG_DEBUG("taskpool::no wating task now");
    }
}

void TaskManager::DispatchAndTryExpand([[maybe_unused]] const uv_async_t* req)
{
    TaskManager& taskManager = TaskManager::GetInstance();
    taskManager.DispatchAndTryExpandInner();
}

void TaskManager::DispatchAndTryExpandInner()
{
    // dispatch task in the TaskPoolManager thread
    NotifyExecuteTask();
    // do not check the worker idleTime first
    TryExpandWithCheckIdle<false>();
    if (!timerTriggered_ && GetNonIdleTaskNum() != 0) {
        timerTriggered_ = true;
        // use timer to check worker idle time after dispatch task
        // if worker has been blocked or fd is broken, taskpool can expand automatically
        uv_timer_start(expandTimer_, reinterpret_cast<uv_timer_cb>(TryExpand), TRIGGER_EXPAND_INTERVAL, 0);
    }
}

void TaskManager::TryExpand([[maybe_unused]] const uv_timer_t* req)
{
    TaskManager& taskManager = TaskManager::GetInstance();
    taskManager.TryExpandWithCheckIdle<true>();
}

template <bool needCheckIdle>
void TaskManager::TryExpandWithCheckIdle()
{
    if (GetNonIdleTaskNum() == 0) {
        if constexpr (!needCheckIdle) {
            HILOG_DEBUG("taskpool:: no need expand");
        }
        return;
    }

    needChecking_ = false;
    timerTriggered_ = false;
    uint32_t targetNum = ComputeSuitableIdleNum();
    uint32_t workerCount = 0;
    uint32_t idleCount = 0;
    uint32_t timeoutWorkers = 0;
    {
        std::lock_guard<std::recursive_mutex> lock(workersMutex_);
        workerCount = workers_.size();
        timeoutWorkers = timeoutWorkers_.size();
        if constexpr (needCheckIdle) {
            uint64_t currTime = ConcurrentHelper::GetMilliseconds();
            idleCount = static_cast<uint32_t>(std::count_if(idleWorkers_.begin(), idleWorkers_.end(),
                [currTime](const auto& worker) {
                    return worker->IsRunnable(currTime);
                }));
        } else {
            idleCount = idleWorkers_.size();
        }
    }
    uint32_t maxThreads = std::max(maxThreads_, DEFAULT_THREADS);
    maxThreads = (timeoutWorkers == 0) ? maxThreads : maxThreads + 2; // 2: extra threads
    if (workerCount < maxThreads && idleCount < targetNum) {
        // Prevent the total number of workers from exceeding maxThreads
        uint32_t step = std::min(std::min(maxThreads, targetNum) - idleCount, maxThreads - workerCount);
        step = step >= expandingCount_ ? step - expandingCount_ : 0;
        if (step == 0) {
            return;
        }
        CreateWorkers(hostEnv_, step);
        // print taskpool workers info
        HILOG_INFO("taskpool:: max:%{public}u, create:%{public}u, total:%{public}u", maxThreads, step, GetThreadNum());
    }
    if (UNLIKELY(suspend_)) {
        suspend_ = false;
        uv_timer_again(balanceTimer_);
    }
}

void TaskManager::RunTaskManager()
{
    loop_ = uv_loop_new();
    if (loop_ == nullptr) { // LCOV_EXCL_BR_LINE
        HILOG_FATAL("taskpool:: new loop failed.");
        return;
    }
    ConcurrentHelper::UvHandleInit(loop_, dispatchHandle_, TaskManager::DispatchAndTryExpand);

    TimerInit(balanceTimer_, true, reinterpret_cast<uv_timer_cb>(TaskManager::TriggerLoadBalance), TRIGGER_INTERVAL);
    TimerInit(expandTimer_, false, reinterpret_cast<uv_timer_cb>(TaskManager::TriggerLoadBalance), TRIGGER_INTERVAL);
    TimerInit(logTimer_, true, reinterpret_cast<uv_timer_cb>(TaskManager::PrintLog), LOG_INTERVAL);
    TimerInit(waitingTimer_, true, reinterpret_cast<uv_timer_cb>(TaskManager::PrintWaitingTime), WAITING_INTERVAL);

    isHandleInited_ = true;
#if defined IOS_PLATFORM || defined MAC_PLATFORM
    pthread_setname_np("OS_TaskManager");
#else
    pthread_setname_np(pthread_self(), "OS_TaskManager");
#endif
    if (UNLIKELY(needChecking_)) {
        needChecking_ = false;
        uv_async_send(dispatchHandle_);
    }
    uv_run(loop_, UV_RUN_DEFAULT);
    if (loop_ != nullptr) {
        uv_loop_delete(loop_);
    }
}

void TaskManager::CancelTask(napi_env env, uint32_t taskId)
{
    // 1. Cannot find taskInfo by executeId, throw error
    // 2. Find executing taskInfo, skip it
    // 3. Find waiting taskInfo, cancel it
    // 4. Find canceled taskInfo, skip it
    std::string strTrace = "Cancel tId " + std::to_string(taskId);
    HILOG_INFO("taskpool::%{public}s", strTrace.c_str());
    HITRACE_HELPER_METER_NAME(strTrace);
    Task* task = GetTask(taskId);
    if (task == nullptr) {
        std::string errMsg = "taskpool:: the task may not exist";
        HILOG_ERROR("%{public}s", errMsg.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_CANCEL_NONEXIST_TASK, errMsg.c_str());
        return;
    }
    RemoveTaskEnqueueTime(taskId);
    if (task->taskState_ == ExecuteState::CANCELED) {
        HILOG_DEBUG("taskpool:: task has been canceled");
        return;
    }
    if (task->IsGroupCommonTask()) {
        // when task is a group common task, still check the state
        if (task->currentTaskInfo_ == nullptr || task->taskState_ == ExecuteState::NOT_FOUND ||
            task->taskState_ == ExecuteState::FINISHED || task->taskState_ == ExecuteState::ENDING) {
            std::string errMsg = "taskpool:: task is not executed or has been executed";
            HILOG_DEBUG("%{public}s", errMsg.c_str());
            ErrorHelper::ThrowError(env, ErrorHelper::ERR_CANCEL_NONEXIST_TASK, errMsg.c_str());
            return;
        }
        TaskGroup* taskGroup = TaskGroupManager::GetInstance().GetTaskGroup(task->groupId_);
        if (taskGroup == nullptr) {
            return;
        }
        return taskGroup->CancelGroupTask(env, task->taskId_);
    }
    if (task->IsPeriodicTask()) {
        task->UpdateTaskStateToCanceled();
        return;
    }
    if (task->IsSeqRunnerTask()) {
        CancelSeqRunnerTask(env, task);
        return;
    }
    if (task->IsAsyncRunnerTask()) {
        AsyncRunnerManager::GetInstance().CancelAsyncRunnerTask(env, task);
        return;
    }
    ExecuteState state = ExecuteState::NOT_FOUND;
    {
        std::lock_guard<std::recursive_mutex> lock(task->taskMutex_);
        if (task->taskState_ == ExecuteState::CANCELED) {
            HILOG_DEBUG("taskpool:: task has been canceled");
            return;
        }
        if ((task->currentTaskInfo_ == nullptr && task->taskState_ != ExecuteState::DELAYED) ||
            task->taskState_ == ExecuteState::NOT_FOUND || task->taskState_ == ExecuteState::FINISHED ||
            task->taskState_ == ExecuteState::ENDING) {
            std::string errMsg = "taskpool:: task is not executed or has been executed";
            HILOG_DEBUG("%{public}s", errMsg.c_str());
            ErrorHelper::ThrowError(env, ErrorHelper::ERR_CANCEL_NONEXIST_TASK, errMsg.c_str());
            return;
        }
        state = task->taskState_.exchange(ExecuteState::CANCELED);
    }
    if (task->IsSameEnv(env)) {
        task->CancelInner(state);
        return;
    }
    CancelTaskMessage* message = new CancelTaskMessage(state, task->taskId_);
    task->TriggerCancel(message);
}

void TaskManager::CancelSeqRunnerTask(napi_env env, Task* task)
{
    {
        std::lock_guard<std::recursive_mutex> lock(task->taskMutex_);
        if (task->taskState_ != ExecuteState::FINISHED) {
            task->taskState_ = ExecuteState::CANCELED;
            return;
        }
    }
    std::string errMsg = "taskpool:: sequenceRunner task has been executed";
    HILOG_ERROR("%{public}s", errMsg.c_str());
    ErrorHelper::ThrowError(env, ErrorHelper::ERR_CANCEL_NONEXIST_TASK, errMsg.c_str());
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
    CountTraceForWorkerWithoutLock();
}

uint32_t TaskManager::GetRunningWorkers()
{
    std::lock_guard<std::recursive_mutex> lock(workersMutex_);
    return std::count_if(workers_.begin(), workers_.end(), [](const auto& worker) {
        return worker->HasRunningTasks();
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
    uint32_t sum = 0;
    for (const auto& elements : taskQueues_) {
        sum += elements->GetTaskNum();
    }
    return sum;
}

uint32_t TaskManager::GetNonIdleTaskNum()
{
    return nonIdleTaskNum_;
}

void TaskManager::IncreaseTaskNum(Priority priority)
{
    totalTaskNum_.fetch_add(1);
    if (priority != Priority::IDLE) {
        nonIdleTaskNum_.fetch_add(1);
    }
}

void TaskManager::DecreaseTaskNum(Priority priority)
{
    totalTaskNum_.fetch_sub(1);
    if (priority != Priority::IDLE) {
        nonIdleTaskNum_.fetch_sub(1);
    }
}

uint32_t TaskManager::GetThreadNum()
{
    std::lock_guard<std::recursive_mutex> lock(workersMutex_);
    return workers_.size();
}

void TaskManager::EnqueueTaskId(uint32_t taskId, Priority priority)
{
    {
        std::lock_guard<std::mutex> lock(taskQueuesMutex_);
        IncreaseTaskNum(priority);
        taskQueues_[priority]->EnqueueTaskId(taskId);
    }
    TryTriggerExpand();
    Task* task = GetTask(taskId);
    if (task == nullptr) {
        HILOG_FATAL("taskpool:: task is nullptr");
        return;
    }
    task->IncreaseTaskLifecycleCount();
    if (task->IsSeqRunnerTask() || task->IsAsyncRunnerTask()) {
        auto onEnqueueTask = [taskId]([[maybe_unused]] void *data) {
            Task *task = TaskManager::GetInstance().GetTask(taskId);
            if (task == nullptr) {
                HILOG_WARN("taskpool:: task is null");
                return;
            }
            task->TriggerEnqueueCallback();
        };
        auto napiPrio = g_napiPriorityMap.at(priority);
        uint64_t handleId = 0;
        napi_send_cancelable_event(task->GetEnv(), onEnqueueTask, nullptr, napiPrio, &handleId, ON_ENQUEUE_STR);
    } else {
        task->TriggerEnqueueCallback();
    }
}

bool TaskManager::EraseWaitingTaskId(uint32_t taskId, Priority priority)
{
    std::lock_guard<std::mutex> lock(taskQueuesMutex_);
    if (!taskQueues_[priority]->EraseWaitingTaskId(taskId)) {
        HILOG_WARN("taskpool:: taskId is not in executeQueue when cancel");
        return false;
    }
    DecreaseTaskNum(priority);
    return true;
}

std::pair<uint32_t, Priority> TaskManager::DequeueTaskId()
{
    bool isChoose = IsChooseIdle();
    {
        std::lock_guard<std::mutex> lock(taskQueuesMutex_);
        auto& highTaskQueue = taskQueues_[Priority::HIGH];
        if (!highTaskQueue->IsEmpty() && highPrioExecuteCount_ < HIGH_PRIORITY_TASK_COUNT) {
            highPrioExecuteCount_++;
            return GetTaskByPriority(highTaskQueue, Priority::HIGH);
        }
        highPrioExecuteCount_ = 0;

        auto& mediumTaskQueue = taskQueues_[Priority::MEDIUM];
        if (!mediumTaskQueue->IsEmpty() && mediumPrioExecuteCount_ < MEDIUM_PRIORITY_TASK_COUNT) {
            mediumPrioExecuteCount_++;
            return GetTaskByPriority(mediumTaskQueue, Priority::MEDIUM);
        }
        mediumPrioExecuteCount_ = 0;

        auto& lowTaskQueue = taskQueues_[Priority::LOW];
        if (!lowTaskQueue->IsEmpty()) {
            return GetTaskByPriority(lowTaskQueue, Priority::LOW);
        }

        auto& idleTaskQueue = taskQueues_[Priority::IDLE];
        if (!IsPerformIdle() && highTaskQueue->IsEmpty() && mediumTaskQueue->IsEmpty() && !idleTaskQueue->IsEmpty() &&
            isChoose) {
            return GetTaskByPriority(idleTaskQueue, Priority::IDLE);
        }
    }
    return std::make_pair(0, Priority::LOW);
}

bool TaskManager::IsChooseIdle()
{
    std::lock_guard<std::recursive_mutex> lock(workersMutex_);
    if (workers_.size() != idleWorkers_.size()) {
        return false;
    }
    return true;
}

std::pair<uint32_t, Priority> TaskManager::GetTaskByPriority(const std::unique_ptr<ExecuteQueue>& taskQueue,
    Priority priority)
{
    uint32_t taskId = taskQueue->DequeueTaskId();
    DecreaseTaskNum(priority);
    if (IsDependendByTaskId(taskId)) {
        EnqueuePendingTaskInfo(taskId, priority);
        return std::make_pair(0, priority);
    }
    preDequeneTime_ = ConcurrentHelper::GetMilliseconds();
    if (priority == Priority::IDLE && taskId != 0) {
        SetIsPerformIdle(true);
    }
    return std::make_pair(taskId, priority);
}

void TaskManager::NotifyExecuteTask()
{
    std::lock_guard<std::recursive_mutex> lock(workersMutex_);
    if (GetNonIdleTaskNum() == 0 && workers_.size() != idleWorkers_.size()) {
        // When there are only idle tasks and workers executing them, it is not triggered
        HILOG_INFO("taskpool::not notify");
        return;
    }
    if (idleWorkers_.size() == 0) {
        HILOG_INFO("taskpool::0 idle");
        return;
    }

    for (auto& worker : idleWorkers_) {
        worker->NotifyExecuteTask();
    }
}

void TaskManager::InitTaskManager(napi_env env)
{
    HITRACE_HELPER_METER_NAME("InitTaskManager");
    if (!isInitialized_.exchange(true, std::memory_order_relaxed)) {
#if defined(ENABLE_TASKPOOL_FFRT)
        globalEnableFfrtFlag_ = OHOS::system::GetIntParameter<int>("persist.commonlibrary.taskpoolglobalenableffrt", 0);
        if (!globalEnableFfrtFlag_) {
            UpdateSystemAppFlag();
            if (IsSystemApp()) {
                disableFfrtFlag_ = OHOS::system::GetIntParameter<int>("persist.commonlibrary.taskpooldisableffrt", 0);
            }
        }
        if (EnableFfrt()) {
            HILOG_DEBUG("taskpool:: apps use ffrt");
        } else {
            HILOG_DEBUG("taskpool:: apps do not use ffrt");
        }
#endif
#if defined(ENABLE_TASKPOOL_EVENTHANDLER)
        mainThreadHandler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(
            OHOS::AppExecFwk::EventRunner::GetMainEventRunner());
#endif
    #if defined(ENABLE_CONCURRENCY_INTEROP)
        if (reinterpret_cast<NativeEngine*>(env)->IsMainThread() && ANIHelper::GetAniVm() == nullptr) {
            HILOG_INFO("taskpool:: get aniVm is null in main thread.");
        }
#endif
        auto mainThreadEngine = NativeEngine::GetMainThreadEngine();
        if (mainThreadEngine == nullptr) {
            HILOG_FATAL("taskpool:: mainThreadEngine is nullptr");
            return;
        }
        hostEnv_ = reinterpret_cast<napi_env>(mainThreadEngine);

        // Add a reserved thread for taskpool
        CreateWorkers(hostEnv_);
        // Create a timer to manage worker threads
        std::thread workerManager([this] {this->RunTaskManager();});
        workerManager.detach();
    }
}

void TaskManager::CreateWorkers(napi_env env, uint32_t num)
{
    HILOG_DEBUG("taskpool:: CreateWorkers, num:%{public}u", num);
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
    timeoutWorkers_.erase(worker);
    workers_.erase(worker);
}

void TaskManager::RestoreWorker(Worker* worker)
{
    std::lock_guard<std::recursive_mutex> lock(workersMutex_);
    if (UNLIKELY(suspend_)) {
        suspend_ = false;
        uv_timer_again(balanceTimer_);
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

// ---------------------------------- SendData ---------------------------------------
void TaskManager::RegisterCallback(napi_env env, uint32_t taskId, std::shared_ptr<CallbackInfo> callbackInfo,
                                   const std::string& type)
{
    std::lock_guard<std::mutex> lock(callbackMutex_);
    if (callbackInfo != nullptr) {
        callbackInfo->type = type;
        auto iter = callbackTable_.find(taskId);
        if (iter != callbackTable_.end() && iter->second != nullptr) {
            callbackInfo->refCount = iter->second->refCount;
        }
    } else { // LOCV_EXCL_BR_LINE
        HILOG_WARN("taskpool:: callbackInfo is null.");
    }
    callbackTable_[taskId] = callbackInfo;
}

void TaskManager::IncreaseSendDataRefCount(uint32_t taskId)
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

void TaskManager::DecreaseSendDataRefCount(napi_env env, uint32_t taskId, Task* task)
{
    if (taskId == 0) { // do not support func
        return;
    }
    std::lock_guard<std::mutex> lock(callbackMutex_);
    auto iter = callbackTable_.find(taskId);
    if (iter == callbackTable_.end() || iter->second == nullptr) {
        return;
    }

    if (task != nullptr && !task->IsValid()) {
        callbackTable_.erase(iter);
        return;
    }

    if (--iter->second->refCount == 0) {
        callbackTable_.erase(iter);
    }
}

void TaskManager::ExecuteSendData(napi_env env, TaskResultInfo* resultInfo, uint32_t taskId)
{
    auto [hostEnv, priority] = GetTaskEnvAndPriority(taskId);
    if (hostEnv == nullptr) {
        delete resultInfo;
        return;
    }
    std::lock_guard<std::mutex> lock(callbackMutex_);
    auto iter = callbackTable_.find(taskId);
    if (iter == callbackTable_.end() || iter->second == nullptr) {
        HILOG_ERROR("taskpool:: the callback in SendData is not registered on the host side");
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_NOT_REGISTERED);
        delete resultInfo;
        return;
    }
    auto callbackInfo = iter->second;
    ++callbackInfo->refCount;
    auto workerEngine = reinterpret_cast<NativeEngine*>(env);
    workerEngine->IncreaseListeningCounter();
    auto onCallbackTask = [resultInfo]([[maybe_unused]] void* data) {
        TaskPool::ExecuteOnReceiveDataCallback(resultInfo);
    };
    uint64_t handleId = 0;
    napi_status status = napi_send_cancelable_event(hostEnv, onCallbackTask, nullptr, priority,
                                                    &handleId, ON_CALLBACK_STR);
    if (status != napi_ok) {
        HILOG_ERROR("taskpool:: failed to send event to the host side");
        --callbackInfo->refCount;
        workerEngine->DecreaseListeningCounter();
        delete resultInfo;
    }
}
// ---------------------------------- SendData ---------------------------------------

void TaskManager::NotifyDependencyTaskInfo(uint32_t taskId)
{
    HILOG_DEBUG("taskpool:: task:%{public}s NotifyDependencyTaskInfo", std::to_string(taskId).c_str());
    HITRACE_HELPER_METER_NAME(__PRETTY_FUNCTION__);
    std::unique_lock<std::shared_mutex> lock(dependentTaskInfosMutex_);
    auto iter = dependentTaskInfos_.find(taskId);
    if (iter == dependentTaskInfos_.end() || iter->second.empty()) {
        HILOG_DEBUG("taskpool:: dependentTaskInfo empty");
        return;
    }
    for (auto taskIdIter = iter->second.begin(); taskIdIter != iter->second.end();) {
        auto taskInfo = DequeuePendingTaskInfo(*taskIdIter);
        RemoveDependencyById(taskId, *taskIdIter);
        taskIdIter = iter->second.erase(taskIdIter);
        if (taskInfo.first != 0) {
            EnqueueTaskId(taskInfo.first, taskInfo.second);
        }
    }
}

void TaskManager::RemoveDependencyById(uint32_t dependentTaskId, uint32_t taskId)
{
    HILOG_DEBUG("taskpool::task:%{public}s RemoveDependencyById", std::to_string(taskId).c_str());
    // remove dependency after task execute
    std::unique_lock<std::shared_mutex> lock(dependTaskInfosMutex_);
    auto dependTaskIter = dependTaskInfos_.find(taskId);
    if (dependTaskIter != dependTaskInfos_.end()) {
        auto dependTaskInnerIter = dependTaskIter->second.find(dependentTaskId);
        if (dependTaskInnerIter != dependTaskIter->second.end()) {
            dependTaskIter->second.erase(dependTaskInnerIter);
        }
    }
}

bool TaskManager::IsDependendByTaskId(uint32_t taskId)
{
    std::shared_lock<std::shared_mutex> lock(dependTaskInfosMutex_);
    auto iter = dependTaskInfos_.find(taskId);
    if (iter == dependTaskInfos_.end() || iter->second.empty()) {
        return false;
    }
    return true;
}

bool TaskManager::IsDependentByTaskId(uint32_t dependentTaskId)
{
    std::shared_lock<std::shared_mutex> lock(dependentTaskInfosMutex_);
    auto iter = dependentTaskInfos_.find(dependentTaskId);
    if (iter == dependentTaskInfos_.end() || iter->second.empty()) {
        return false;
    }
    return true;
}

bool TaskManager::StoreTaskDependency(uint32_t taskId, std::set<uint32_t> taskIdSet)
{
    HILOG_DEBUG("taskpool:: task:%{public}s StoreTaskDependency", std::to_string(taskId).c_str());
    StoreDependentTaskInfo(taskIdSet, taskId);
    std::unique_lock<std::shared_mutex> lock(dependTaskInfosMutex_);
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

bool TaskManager::CheckCircularDependency(std::set<uint32_t> dependentIdSet, std::set<uint32_t> idSet, uint32_t taskId)
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

bool TaskManager::RemoveTaskDependency(uint32_t taskId, uint32_t dependentId)
{
    HILOG_DEBUG("taskpool:: task:%{public}s RemoveTaskDependency", std::to_string(taskId).c_str());
    RemoveDependentTaskInfo(dependentId, taskId);
    std::unique_lock<std::shared_mutex> lock(dependTaskInfosMutex_);
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

void TaskManager::EnqueuePendingTaskInfo(uint32_t taskId, Priority priority)
{
    if (taskId == 0) {
        return;
    }
    std::unique_lock<std::shared_mutex> lock(pendingTaskInfosMutex_);
    pendingTaskInfos_.emplace(taskId, priority);
}

std::pair<uint32_t, Priority> TaskManager::DequeuePendingTaskInfo(uint32_t taskId)
{
    std::unique_lock<std::shared_mutex> lock(pendingTaskInfosMutex_);
    if (pendingTaskInfos_.empty()) {
        return std::make_pair(0, Priority::DEFAULT);
    }
    std::pair<uint32_t, Priority> result;
    for (auto it = pendingTaskInfos_.begin(); it != pendingTaskInfos_.end(); ++it) {
        if (it->first == taskId) {
            result = std::make_pair(it->first, it->second);
            it = pendingTaskInfos_.erase(it);
            break;
        }
    }
    return result;
}

void TaskManager::RemovePendingTaskInfo(uint32_t taskId)
{
    HILOG_DEBUG("taskpool:: task:%{public}s RemovePendingTaskInfo", std::to_string(taskId).c_str());
    std::unique_lock<std::shared_mutex> lock(pendingTaskInfosMutex_);
    pendingTaskInfos_.erase(taskId);
}

void TaskManager::StoreDependentTaskInfo(std::set<uint32_t> dependentTaskIdSet, uint32_t taskId)
{
    HILOG_DEBUG("taskpool:: task:%{public}s StoreDependentTaskInfo", std::to_string(taskId).c_str());
    std::unique_lock<std::shared_mutex> lock(dependentTaskInfosMutex_);
    for (const auto& id : dependentTaskIdSet) {
        auto iter = dependentTaskInfos_.find(id);
        if (iter == dependentTaskInfos_.end()) {
            std::set<uint32_t> set{taskId};
            dependentTaskInfos_.emplace(id, std::move(set));
        } else {
            iter->second.emplace(taskId);
        }
    }
}

void TaskManager::RemoveDependentTaskInfo(uint32_t dependentTaskId, uint32_t taskId)
{
    HILOG_DEBUG("taskpool:: task:%{public}s RemoveDependentTaskInfo", std::to_string(taskId).c_str());
    std::unique_lock<std::shared_mutex> lock(dependentTaskInfosMutex_);
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

std::string TaskManager::GetTaskDependInfoToString(uint32_t taskId)
{
    std::shared_lock<std::shared_mutex> lock(dependTaskInfosMutex_);
    std::string str = "TaskInfos: taskId: " + std::to_string(taskId) + ", dependTaskId:";
    auto iter = dependTaskInfos_.find(taskId);
    if (iter != dependTaskInfos_.end()) {
        for (const auto& id : iter->second) {
            str += " " + std::to_string(id);
        }
    }
    return str;
}

void TaskManager::StoreTaskDuration(uint32_t taskId, uint64_t totalDuration, uint64_t cpuDuration)
{
    HILOG_DEBUG("taskpool:: task:%{public}s StoreTaskDuration", std::to_string(taskId).c_str());
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

uint64_t TaskManager::GetTaskDuration(uint32_t taskId, std::string durationType)
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

void TaskManager::RemoveTaskDuration(uint32_t taskId)
{
    HILOG_DEBUG("taskpool:: task:%{public}s RemoveTaskDuration", std::to_string(taskId).c_str());
    std::unique_lock<std::shared_mutex> lock(taskDurationInfosMutex_);
    auto iter = taskDurationInfos_.find(taskId);
    if (iter != taskDurationInfos_.end()) {
        taskDurationInfos_.erase(iter);
    }
}

void TaskManager::StoreLongTaskInfo(uint32_t taskId, Worker* worker)
{
    std::unique_lock<std::shared_mutex> lock(longTasksMutex_);
    longTasksMap_.emplace(taskId, worker);
}

void TaskManager::RemoveLongTaskInfo(uint32_t taskId)
{
    std::unique_lock<std::shared_mutex> lock(longTasksMutex_);
    longTasksMap_.erase(taskId);
}

Worker* TaskManager::GetLongTaskInfo(uint32_t taskId)
{
    std::shared_lock<std::shared_mutex> lock(longTasksMutex_);
    auto iter = longTasksMap_.find(taskId);
    return iter != longTasksMap_.end() ? iter->second : nullptr;
}

void TaskManager::StoreTaskEnqueueTime(uint32_t taskId, std::string enqueueTimeStamp)
{
    std::lock_guard<std::mutex> lock(taskEnqueueTimeMutex_);
    taskEnqueueTimeMap_.emplace(taskId, enqueueTimeStamp);
}

void TaskManager::RemoveTaskEnqueueTime(uint32_t taskId)
{
    std::lock_guard<std::mutex> lock(taskEnqueueTimeMutex_);
    taskEnqueueTimeMap_.erase(taskId);
}

std::string TaskManager::GetTaskEnqueueTime(uint32_t taskId)
{
    std::lock_guard<std::mutex> lock(taskEnqueueTimeMutex_);
    auto iter = taskEnqueueTimeMap_.find(taskId);
    return iter != taskEnqueueTimeMap_.end() ? iter->second : "";
}

void TaskManager::TerminateTask(uint32_t taskId)
{
    HILOG_DEBUG("taskpool:: task:%{public}s TerminateTask", std::to_string(taskId).c_str());
    auto worker = GetLongTaskInfo(taskId);
    if (UNLIKELY(worker == nullptr)) {
        return;
    }
    worker->TerminateTask(taskId);
    RemoveLongTaskInfo(taskId);
}

void TaskManager::ReleaseTaskData(napi_env env, Task* task, bool shouldDeleteTask)
{
    task->ReleaseData();
    task->CancelPendingTask(env);

    task->ClearDelayedTimers();

    if (task->IsFunctionTask() || task->IsGroupFunctionTask()) {
        return;
    }
    if (!task->IsMainThreadTask()) {
        task->SetValid(false);
    }
    uint32_t taskId = task->taskId_;
    DecreaseSendDataRefCount(env, taskId, task);
    RemoveTaskDuration(taskId);
    RemovePendingTaskInfo(taskId);
    ReleaseCallBackInfo(task);
    {
        std::unique_lock<std::shared_mutex> lock(dependentTaskInfosMutex_);
        for (auto dependentTaskIter = dependentTaskInfos_.begin(); dependentTaskIter != dependentTaskInfos_.end();) {
            if (dependentTaskIter->second.find(taskId) != dependentTaskIter->second.end()) {
                dependentTaskIter = dependentTaskInfos_.erase(dependentTaskIter);
            } else {
                ++dependentTaskIter;
            }
        }
    }
    std::unique_lock<std::shared_mutex> lock(dependTaskInfosMutex_);
    auto dependTaskIter = dependTaskInfos_.find(taskId);
    if (dependTaskIter != dependTaskInfos_.end()) {
        dependTaskInfos_.erase(dependTaskIter);
    }
}

void TaskManager::ReleaseCallBackInfo(Task* task)
{
    HILOG_DEBUG("taskpool:: ReleaseCallBackInfo task:%{public}s", std::to_string(task->taskId_).c_str());
    std::lock_guard<std::recursive_mutex> lock(task->taskMutex_);
    if (task->onEnqueuedCallBackInfo_ != nullptr) {
        delete task->onEnqueuedCallBackInfo_;
        task->onEnqueuedCallBackInfo_ = nullptr;
    }

    if (task->onStartExecutionCallBackInfo_ != nullptr) {
        delete task->onStartExecutionCallBackInfo_;
        task->onStartExecutionCallBackInfo_ = nullptr;
    }

    if (task->onExecutionFailedCallBackInfo_ != nullptr) {
        delete task->onExecutionFailedCallBackInfo_;
        task->onExecutionFailedCallBackInfo_ = nullptr;
    }

    if (task->onExecutionSucceededCallBackInfo_ != nullptr) {
        delete task->onExecutionSucceededCallBackInfo_;
        task->onExecutionSucceededCallBackInfo_ = nullptr;
    }
}

void TaskManager::StoreTask(Task* task)
{
    uint64_t id = reinterpret_cast<uint64_t>(task);
    std::lock_guard<std::recursive_mutex> lock(tasksMutex_);
    uint32_t taskId = CalculateTaskId(id);
    while (tasks_.find(taskId) != tasks_.end()) {
        id++;
        taskId = CalculateTaskId(id);
    }
    task->SetTaskId(taskId);
    tasks_.emplace(taskId, task);
}

bool TaskManager::RemoveTask(uint32_t taskId)
{
    std::lock_guard<std::recursive_mutex> lock(tasksMutex_);
    bool res = true;
    auto runningIter = runningTasks_.find(taskId);
    if (runningIter != runningTasks_.end()) {
        res = false;
    }
    runningTasks_.erase(taskId);
    tasks_.erase(taskId);
    return res;
}

void TaskManager::RemoveRunningTask(uint32_t taskId)
{
    std::lock_guard<std::recursive_mutex> lock(tasksMutex_);
    runningTasks_.erase(taskId);
}

Task* TaskManager::GetTask(uint32_t taskId)
{
    std::lock_guard<std::recursive_mutex> lock(tasksMutex_);
    auto iter = tasks_.find(taskId);
    if (iter == tasks_.end()) {
        return nullptr;
    }
    return iter->second;
}

Task* TaskManager::GetTaskForPerform(uint32_t taskId)
{
    std::lock_guard<std::recursive_mutex> lock(tasksMutex_);
    auto iter = tasks_.find(taskId);
    if (iter == tasks_.end()) {
        return nullptr;
    }
    runningTasks_.emplace(taskId, iter->second);
    return iter->second;
}

#if defined(ENABLE_TASKPOOL_FFRT)
void TaskManager::UpdateSystemAppFlag()
{
    auto abilityManager = OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (abilityManager == nullptr) {
        HILOG_ERROR("taskpool:: fail to GetSystemAbility abilityManager is nullptr.");
        return;
    }
    auto bundleObj = abilityManager->GetSystemAbility(OHOS::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (bundleObj == nullptr) {
        HILOG_ERROR("taskpool:: fail to get bundle manager service.");
        return;
    }
    auto bundleMgr = OHOS::iface_cast<OHOS::AppExecFwk::IBundleMgr>(bundleObj);
    if (bundleMgr == nullptr) {
        HILOG_ERROR("taskpool:: Bundle manager is nullptr.");
        return;
    }
    OHOS::AppExecFwk::BundleInfo bundleInfo;
    if (bundleMgr->GetBundleInfoForSelf(
        static_cast<int32_t>(OHOS::AppExecFwk::GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_APPLICATION), bundleInfo)
        != OHOS::ERR_OK) {
        HILOG_ERROR("taskpool:: fail to GetBundleInfoForSelf");
        return;
    }
    isSystemApp_ = bundleInfo.applicationInfo.isSystemApp;
}
#endif

#if defined(ENABLE_TASKPOOL_EVENTHANDLER)
bool TaskManager::PostTask(std::function<void()> task, const char* taskName, Priority priority)
{
    return mainThreadHandler_->PostTask(task, taskName, 0, TASK_EVENTHANDLER_PRIORITY_MAP.at(priority));
}
#endif

void TaskManager::BatchRejectDeferred(napi_env env, std::list<napi_deferred> deferreds, std::string error)
{
    if (deferreds.empty()) {
        return;
    }
    napi_value message = CancelError(env, 0, error.c_str());
    for (auto deferred : deferreds) {
        napi_reject_deferred(env, deferred, message);
    }
}

uint32_t TaskManager::CalculateTaskId(uint64_t id)
{
    size_t hash = std::hash<uint64_t>{}(id);
    uint64_t taskId = static_cast<uint64_t>((hash + taskIdSalt_) & MAX_UINT32_T);
    IncreaseTaskIdSalt();
    while (taskId == 0) {
        ++taskId;
        hash = std::hash<uint64_t>{}(taskId);
        taskId = static_cast<uint64_t>((hash + taskIdSalt_) & MAX_UINT32_T);
        IncreaseTaskIdSalt();
    }
    return static_cast<uint32_t>(taskId);
}

void TaskManager::ClearDependentTask(uint32_t taskId)
{
    HILOG_DEBUG("taskpool:: task:%{public}s ClearDependentTask", std::to_string(taskId).c_str());
    RemoveDependTaskByTaskId(taskId);
    DequeuePendingTaskInfo(taskId);
    std::unique_lock<std::shared_mutex> lock(dependentTaskInfosMutex_);
    RemoveDependentTaskByTaskId(taskId);
}

void TaskManager::RemoveDependTaskByTaskId(uint32_t taskId)
{
    std::set<uint32_t> dependTaskIds;
    {
        std::unique_lock<std::shared_mutex> lock(dependTaskInfosMutex_);
        auto iter = dependTaskInfos_.find(taskId);
        if (iter == dependTaskInfos_.end()) {
            return;
        }
        dependTaskIds.insert(iter->second.begin(), iter->second.end());
        dependTaskInfos_.erase(iter);
    }
    for (auto dependTaskId : dependTaskIds) {
        RemoveDependentTaskInfo(dependTaskId, taskId);
    }
}

void TaskManager::RemoveDependentTaskByTaskId(uint32_t taskId)
{
    auto iter = dependentTaskInfos_.find(taskId);
    if (iter == dependentTaskInfos_.end() || iter->second.empty()) {
        HILOG_DEBUG("taskpool:: dependentTaskInfo empty");
        return;
    }
    for (auto taskIdIter = iter->second.begin(); taskIdIter != iter->second.end();) {
        DequeuePendingTaskInfo(*taskIdIter);
        RemoveDependencyById(taskId, *taskIdIter);
        auto id = *taskIdIter;
        taskIdIter = iter->second.erase(taskIdIter);
        auto task = GetTask(id);
        if (task == nullptr) {
            continue;
        }
        // 1. The task is in taskQueues_, need remove.
        // 2. The task has not been executed yet, need remove.
        if (task->currentTaskInfo_ != nullptr && EraseWaitingTaskId(task->taskId_, task->currentTaskInfo_->priority)) {
            delete task->currentTaskInfo_;
            task->currentTaskInfo_ = nullptr;
            task->DecreaseTaskLifecycleCount();
        }
        if (task->currentTaskInfo_ == nullptr) {
            reinterpret_cast<NativeEngine*>(task->env_)->DecreaseSubEnvCounter();
            DecreaseSendDataRefCount(task->env_, task->taskId_, task);
            napi_reference_unref(task->env_, task->taskRef_, nullptr);
        }
        RemoveDependentTaskByTaskId(task->taskId_);
    }
}

void TaskManager::WriteHisysForFfrtAndUv(Worker* worker, HisyseventParams* hisyseventParams)
{
#if defined(ENABLE_TASKPOOL_HISYSEVENT)
    if (!EnableFfrt() || hisyseventParams == nullptr) {
        return;
    }
    int32_t tid = 0;
    if (worker != nullptr) {
        auto loop = worker->GetWorkerLoop();
        uint64_t loopAddress = reinterpret_cast<uint64_t>(loop);
        hisyseventParams->message += "; current runningLoop: " + std::to_string(loopAddress);
        tid = worker->tid_;
    }
    hisyseventParams->tid = tid;
    hisyseventParams->pid = getpid();
    hisyseventParams->message += "; idleWorkers num: " + std::to_string(idleWorkers_.size());
    hisyseventParams->message += "; workers num: " + std::to_string(workers_.size());
    int32_t ret = DfxHisysEvent::WriteFfrtAndUv(hisyseventParams);
    if (ret < 0) {
        HILOG_WARN("taskpool:: HisyseventWrite failed, ret: %{public}s", std::to_string(ret).c_str());
    }
#endif
}

void TaskManager::CheckTasksAndReportHisysEvent()
{
#if defined(ENABLE_TASKPOOL_HISYSEVENT)
    if (!EnableFfrt()) {
        return;
    }
    uint64_t currTime = ConcurrentHelper::GetMilliseconds();
    auto taskNum = GetTaskNum();
    if (taskNum == 0 || currTime - preDequeneTime_ < UNEXECUTE_TASK_TIME || preDequeneTime_ < reportTime_) {
        return;
    }
    std::string message = "Task scheduling timeout, total task num:" + std::to_string(taskNum);
    HisyseventParams* hisyseventParams = new HisyseventParams();
    hisyseventParams->methodName = "CheckTasksAndReportHisysEvent";
    hisyseventParams->funName = "";
    hisyseventParams->logType = "ffrt";
    hisyseventParams->code = DfxHisysEvent::TASK_TIMEOUT;
    hisyseventParams->message = message;
    WriteHisysForFfrtAndUv(nullptr, hisyseventParams);
    reportTime_ = currTime;
#endif
}

void TaskManager::WorkerAliveAndReport(Worker* worker)
{
#if defined(ENABLE_TASKPOOL_HISYSEVENT)
    uint64_t workerWaitTime = worker->GetWaitTime();
    uint64_t currTime = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count());
    uint64_t intervalTime = currTime - workerWaitTime;
    if (!worker->IsNeedReport(intervalTime)) {
        return;
    }
    std::string message = "worker uv alive, may have handle leak";
    HisyseventParams* hisyseventParams = new HisyseventParams();
    hisyseventParams->methodName = "WorkerAliveAndReport";
    hisyseventParams->funName = "";
    hisyseventParams->logType = "ffrt";
    hisyseventParams->code = DfxHisysEvent::WORKER_ALIVE;
    hisyseventParams->message = message;
    hisyseventParams->waitTime = intervalTime;
    WriteHisysForFfrtAndUv(worker, hisyseventParams);
    worker->IncreaseReportCount();
#endif
}

void TaskManager::UvReportHisysEvent(Worker* worker, std::string methodName, std::string funName, std::string message,
                                     int32_t code)
{
#if defined(ENABLE_TASKPOOL_HISYSEVENT)
    HisyseventParams* hisyseventParams = new HisyseventParams();
    hisyseventParams->methodName = methodName;
    hisyseventParams->funName = funName;
    hisyseventParams->logType = "uv";
    hisyseventParams->code = code;
    hisyseventParams->message = message;
    WriteHisysForFfrtAndUv(worker, hisyseventParams);
#endif
}

napi_value TaskManager::CancelError(napi_env env, int32_t errCode, const char* errMessage, napi_value result,
                                    bool success)
{
    std::string errTitle = "";
    napi_value concurrentError = nullptr;

    napi_value code = nullptr;
    napi_create_uint32(env, errCode, &code);

    napi_value name = nullptr;
    std::string errName = "BusinessError";

    if (errCode == ErrorHelper::ERR_ASYNCRUNNER_TASK_CANCELED) {
        errTitle = "The asyncRunner task has been canceled.";
    }

    if (errCode == 0 && errMessage == nullptr) {
        errTitle = "taskpool:: task has been canceled";
    }
    
    napi_create_string_utf8(env, errName.c_str(), NAPI_AUTO_LENGTH, &name);
    napi_value msg = nullptr;
    if (errMessage == nullptr) {
        napi_create_string_utf8(env, errTitle.c_str(), NAPI_AUTO_LENGTH, &msg);
    } else {
        napi_create_string_utf8(env, (errTitle + std::string(errMessage)).c_str(), NAPI_AUTO_LENGTH, &msg);
    }

    napi_create_error(env, nullptr, msg, &concurrentError);
    napi_set_named_property(env, concurrentError, "code", code);
    napi_set_named_property(env, concurrentError, "name", name);
    napi_value data = nullptr;
    napi_create_object(env, &data);
    napi_value resultValue = nullptr;
    napi_value errorValue = msg;
    napi_get_undefined(env, &resultValue);
    if (result != nullptr) {
        bool isError = false;
        napi_is_error(env, result, &isError);
        napi_value error = NapiHelper::GetNameProperty(env, result, "error");
        if (NapiHelper::IsNotUndefined(env, error)) {
            errorValue = error;
        } else if (isError && !success) {
            napi_value errMsg = NapiHelper::GetNameProperty(env, result, "message");
            errorValue = errMsg;
        } else {
            resultValue = result;
        }
    }
    napi_set_named_property(env, data, "result", resultValue);
    napi_set_named_property(env, data, "error", errorValue);
    napi_set_named_property(env, concurrentError, "data", data);
    return concurrentError;
}

void TaskManager::SetIsPerformIdle(bool performIdle)
{
    isPerformIdle_ = performIdle;
}

bool TaskManager::IsPerformIdle() const
{
    return isPerformIdle_;
}

#if defined(ENABLE_CONCURRENCY_INTEROP)
    void TaskManager::AttachWorkerToAniVm(Worker* worker)
    {
        if (!ANIHelper::IsConcurrencySupportInterop()) {
            return;
        }
        // attach worker env to 1.2vm
        std::string interop = "--interop=enable";
        ani_option interopEnabled {interop.data(), (void *)worker->workerEnv_};
        ani_options aniArgs {1, &interopEnabled};
        auto* aniVm = ANIHelper::GetAniVm();
        if (aniVm == nullptr) {
            HILOG_ERROR("taskpool:: aviVm is null.");
            return;
        }
        ani_status status = aniVm->AttachCurrentThread(&aniArgs, ANI_VERSION_1, &worker->aniEnv_);
        if (status != ANI_OK || worker->aniEnv_ == nullptr) {
            HILOG_ERROR("taskpool:: AttachCurrentThread failed.");
        }
    }
    
    void TaskManager::DetachWorkerFromAniVm(Worker* worker)
    {
        if (!ANIHelper::IsConcurrencySupportInterop()) {
            return;
        }
        // detach worker from 1.2vm
        auto* aniVm = ANIHelper::GetAniVm();
        if (aniVm == nullptr) {
            HILOG_ERROR("taskpool:: DetachWorkerFromAniVm aviVm is null.");
            return;
        }
        ani_status status = aniVm->DetachCurrentThread();
        if (status != ANI_OK) {
            HILOG_ERROR("taskpool:: DetachCurrentThread failed.");
        }
    }
#endif
 	 
uint32_t TaskManager::GetTotalTaskNum() const
{
    return totalTaskNum_;
}

void TaskManager::AddCountTraceForWorkerLog(bool needLog, int64_t threadNum, int64_t idleThreadNum,
                                            int64_t timeoutThreadNum)
{
    if (needLog) {
        // A: all workers num, R: running workers num, I: idle workers num, O: timeout workers num
        HILOG_INFO("taskpool:: A:%{public}s, R:%{public}s, I:%{public}s, O:%{public}s",
            std::to_string(threadNum).c_str(), std::to_string(threadNum - idleThreadNum).c_str(),
            std::to_string(idleThreadNum).c_str(), std::to_string(timeoutThreadNum).c_str());
    }
}

bool TaskManager::ExecuteTaskStartExecution(uint32_t taskId, Priority priority)
{
    Task* task = GetTask(taskId);
    if (task == nullptr) {
        HILOG_ERROR("taskpool:: ExecuteTaskStartExecution task nullptr, id:%{public}s",
            std::to_string(taskId).c_str());
        return false;
    }
    std::lock_guard<std::recursive_mutex> lock(task->taskMutex_);
    if (!task->IsValid()) {
        HILOG_ERROR("taskpool:: ExecuteTaskStartExecution task invalid, id:%{public}s",
            std::to_string(taskId).c_str());
        return false;
    }
    if (task->onStartExecutionCallBackInfo_ == nullptr) {
        return true;
    }
    auto hostEnv = task->GetEnv();
    auto workerEngine = reinterpret_cast<NativeEngine*>(hostEnv);
    workerEngine->IncreaseListeningCounter();
    auto onStartTask = [taskId]([[maybe_unused]] void* data) {
        Task* task = TaskManager::GetInstance().GetTask(taskId);
        if (task == nullptr || task->onStartExecutionCallBackInfo_ == nullptr) {
            return;
        }
        Task::StartExecutionTask(task->onStartExecutionCallBackInfo_);
    };
    auto napiPrio = g_napiPriorityMap.at(priority);
    uint64_t handleId = 0;
    napi_status status = napi_send_cancelable_event(hostEnv, onStartTask, nullptr, napiPrio,
                                                    &handleId, ON_START_STR);
    if (status != napi_ok) { // LOCV_EXCL_BR_LINE
        HILOG_ERROR("taskpool:: failed to send event to the host side");
        workerEngine->DecreaseListeningCounter();
    }
    return true;
}

std::tuple<napi_env, napi_event_priority> TaskManager::GetTaskEnvAndPriority(uint32_t taskId)
{
    auto task = GetTask(taskId);
    if (task == nullptr || !task->IsValid()) {
        HILOG_ERROR("taskpool:: GetTaskEnvAndPriority task is nullptr");
        return {nullptr, napi_eprio_high};
    }
    std::lock_guard<std::recursive_mutex> lock(task->taskMutex_);
    auto worker = task->GetWorker();
    if (worker == nullptr) {
        HILOG_ERROR("taskpool:: GetTaskEnvAndPriority worker is nullptr");
        return {nullptr, napi_eprio_high};
    }
    napi_env hostEnv = task->GetEnv();
    napi_event_priority priority = g_napiPriorityMap.at(worker->GetPriority());
    return {hostEnv, priority};
}

CallbackInfo* TaskManager::GetSenddataCallback(uint32_t taskId)
{
    std::lock_guard<std::mutex> lock(callbackMutex_);
    auto iter = callbackTable_.find(taskId);
    if (iter == callbackTable_.end() || iter->second == nullptr) {
        return nullptr;
    }
    return iter->second.get();
}

std::string TaskManager::GetFuncNameFromData(void* data)
{
    std::string name = "Taskpool Thread";
    if (data == nullptr) {
        return name;
    }
    Task* task = static_cast<Task*>(data);
    if (!IsValidTask(task)) {
        return name;
    }
    return name + " " + task->name_;
}

bool TaskManager::IsValidTask(Task* task)
{
    if (task == nullptr) {
        return false;
    }
    bool flag = false;
    {
        std::lock_guard<std::recursive_mutex> lock(tasksMutex_);
        for (auto& [_, rTask] : tasks_) {
            if (rTask == task) {
                flag = true;
                break;
            }
        }
    }
    if (flag && task->IsValid()) {
        return true;
    }
    return false;
}

void TaskManager::IncreaseTaskIdSalt()
{
    if (taskIdSalt_ >= MAX_UINT32_T) {
        taskIdSalt_ = 1;
    }
    taskIdSalt_.fetch_add(1);
}
} // namespace Commonlibrary::Concurrent::TaskPoolModule