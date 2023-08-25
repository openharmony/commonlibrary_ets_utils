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
#include "helper/hitrace_helper.h"
#include "taskpool.h"
#include "utils/log.h"
#include "worker.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
using namespace OHOS::JsSysModule;

static constexpr int8_t HIGH_PRIORITY_TASK_COUNT = 5;
static constexpr int8_t MEDIUM_PRIORITY_TASK_COUNT = 5;
static constexpr int32_t MAX_TASK_DURATION = 100; // 100: 100ms
static constexpr int32_t MAX_IDLE_TIME = 50000; // 50000: 50s
static constexpr uint32_t STEP_SIZE = 2;
static constexpr uint32_t DEFAULT_THREADS = 3;
static constexpr uint32_t MIN_THREADS = 1; // 1: minimum thread num when idle
static constexpr uint32_t MIN_TIMEOUT_TIME = 180000; // 180000: 3min
static constexpr uint32_t MAX_TIMEOUT_TIME = 600000; // 6000000: 10min

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
        ConcurrentHelper::UvHandleClose(notifyRestartTimer_);
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
        std::unique_lock<std::shared_mutex> lock(taskInfosMutex_);
        for (auto& [_, taskInfo] : taskInfos_) {
            delete taskInfo;
        }
        taskInfos_.clear();
    }
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
                    napi_value id = nullptr;
                    napi_create_uint32(env, currentId, &id);
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
        std::unique_lock<std::shared_mutex> lock(taskInfosMutex_);
        int32_t i = 0;
        for (auto& [_, taskInfo] : taskInfos_) {
            napi_value taskInfoValue = nullptr;
            napi_create_object(env, &taskInfoValue);
            napi_value taskId = nullptr;
            napi_create_uint32(env, taskInfo->taskId, &taskId);
            napi_value stateValue = nullptr;
            ExecuteState state;
            uint64_t duration = 0;
            if (taskInfo->isCanceled) {
                state = ExecuteState::CANCELED;
            } else if (taskInfo->worker != nullptr) {
                Worker* worker = reinterpret_cast<Worker*>(taskInfo->worker);
                duration = ConcurrentHelper::GetMilliseconds() - worker->startTime_;
                state = ExecuteState::RUNNING;
            } else {
                state = ExecuteState::WAITING;
            }
            napi_create_int32(env, state, &stateValue);
            napi_set_named_property(env, taskInfoValue, "taskId", taskId);
            napi_set_named_property(env, taskInfoValue, "state", stateValue);
            napi_value durationValue = nullptr;
            napi_create_uint32(env, static_cast<uint32_t>(duration), &durationValue);
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
        targetNum = STEP_SIZE;
    } else if (totalExecCount_ == 0) {
        targetNum = 0; // no task since created
    }

    uint32_t result = 0;
    if (totalExecCount_ != 0) {
        auto durationPerTask = static_cast<double>(totalExecTime_) / totalExecCount_;
        result = std::ceil(durationPerTask * GetTaskNum() / MAX_TASK_DURATION);
    }

    targetNum += std::min(result, GetTaskNum());
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
    bool state = (GetThreadNum() == ConcurrentHelper::GetMaxThreads()) && (GetIdleWorkers() == 0);
    uint64_t threshold = state ? MIN_TIMEOUT_TIME : MAX_TIMEOUT_TIME;
    for (auto iter = workers_.begin(); iter != workers_.end();) {
        auto worker = *iter;
        // if the worker thread is idle, just skip it, and only the worker in running state can be marked as timeout
        if ((worker->state_ == WorkerState::IDLE) ||
            (ConcurrentHelper::GetMilliseconds() - worker->startTime_ < threshold) ||
            !worker->UpdateWorkerState(WorkerState::RUNNING, WorkerState::BLOCKED)) {
            iter++;
            continue;
        }

        HILOG_DEBUG("taskpool:: The worker has been marked as timeout.");
        timeoutWorkers_.insert(worker);
        idleWorkers_.erase(worker);
        workers_.erase(iter++);
    }
    if (UNLIKELY(!timeoutWorkers_.empty())) {
        uv_async_send(expandHandle_);
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

void TaskManager::NotifyShrink(uint32_t targetNum)
{
    uint32_t workerCount = GetThreadNum();
    if (workerCount > MIN_THREADS && workerCount > targetNum) {
        std::lock_guard<std::recursive_mutex> lock(workersMutex_);
        uint32_t maxNum = std::max(MIN_THREADS, targetNum);
        uint32_t step = std::min(workerCount - maxNum, STEP_SIZE);
        for (uint32_t i = 0; i < step; i++) {
            // try to free the worker that idle time meets the requirement
            auto iter = std::find_if(idleWorkers_.begin(), idleWorkers_.end(), [](Worker *worker) {
                auto idleTime = ConcurrentHelper::GetMilliseconds() - worker->idlePoint_;
                return idleTime > MAX_IDLE_TIME;
            });
            // remove it from all sets
            if (iter != idleWorkers_.end()) {
                auto worker = *iter;
                workers_.erase(worker);
                idleWorkers_.erase(worker);
                uv_async_send(worker->clearWorkerSignal_);
            }
        }
    }

    // remove all timeout workers
    std::lock_guard<std::recursive_mutex> lock(workersMutex_);
    for (auto iter = timeoutWorkers_.begin(); iter != timeoutWorkers_.end();) {
        uv_async_send((*iter)->clearWorkerSignal_);
        timeoutWorkers_.erase(iter++);
    }

    // stop the timer
    if ((workers_.size() == MIN_THREADS) && (idleWorkers_.size() == MIN_THREADS) && timeoutWorkers_.empty()) {
        suspend_ = true;
        uv_timer_stop(timer_);
    }
}

void TaskManager::TriggerLoadBalance(const uv_timer_t* req)
{
    TaskManager& taskManager = TaskManager::GetInstance();
    // do not check when try to expand
    if (taskManager.expandingCount_ != 0) {
        return;
    }
    HITRACE_HELPER_COUNT_TRACE("threadNum", static_cast<int64_t>(taskManager.GetThreadNum()));
    HITRACE_HELPER_COUNT_TRACE("runningThreadNum", static_cast<int64_t>(taskManager.GetRunningWorkers()));
    HITRACE_HELPER_COUNT_TRACE("idleThreadNum", static_cast<int64_t>(taskManager.GetIdleWorkers()));
    HITRACE_HELPER_COUNT_TRACE("timeoutThreadNum", static_cast<int64_t>(taskManager.GetTimeoutWorkers()));

    taskManager.CheckForBlockedWorkers();
    uint32_t targetNum = taskManager.ComputeSuitableThreadNum();
    taskManager.NotifyShrink(targetNum);
}

void TaskManager::RestartTimer(const uv_async_t* req)
{
    TaskManager& taskManager = TaskManager::GetInstance();
    uv_timer_again(taskManager.timer_);
}

void TaskManager::TryExpand()
{
    {
        std::lock_guard<std::recursive_mutex> lock(workersMutex_);
        // only try to create workers when there is no idle worker
        if (!idleWorkers_.empty()) {
            return;
        }
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
    }
    if (UNLIKELY(suspend_)) {
        suspend_ = false;
        uv_async_send(notifyRestartTimer_);
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
    notifyRestartTimer_ = new uv_async_t;
    expandHandle_ = new uv_async_t;
    uv_timer_start(timer_, reinterpret_cast<uv_timer_cb>(TaskManager::TriggerLoadBalance), 0, 60000); // 60000: 1min
    uv_async_init(loop_, notifyRestartTimer_, reinterpret_cast<uv_async_cb>(TaskManager::RestartTimer));
    uv_async_init(loop_, expandHandle_, reinterpret_cast<uv_async_cb>(TaskManager::NotifyExpand));
#if defined IOS_PLATFORM || defined MAC_PLATFORM
    pthread_setname_np("TaskMgrThread");
#else
    pthread_setname_np(pthread_self(), "TaskMgrThread");
#endif
    if (UNLIKELY(needChecking_)) {
        needChecking_ = false;
        uv_async_send(expandHandle_);
    }
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

void TaskManager::StoreTaskInfo(uint32_t executeId, TaskInfo* taskInfo)
{
    std::unique_lock<std::shared_mutex> lock(taskInfosMutex_);
    taskInfos_.emplace(executeId, taskInfo);
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
    // remove the the taskInfo after call function
    taskInfos_.erase(iter);
    return taskInfo;
}

TaskInfo* TaskManager::GetTaskInfo(uint32_t executeId)
{
    std::unique_lock<std::shared_mutex> lock(taskInfosMutex_);
    auto iter = taskInfos_.find(executeId);
    if (iter == taskInfos_.end() || iter->second == nullptr) {
        return nullptr;
    }
    return iter->second;
}

bool TaskManager::MarkCanceledState(uint32_t executeId)
{
    std::unique_lock<std::shared_mutex> lock(taskInfosMutex_);
    auto iter = taskInfos_.find(executeId);
    if (iter == taskInfos_.end() || iter->second == nullptr) {
        return false;
    }
    if (!UpdateExecuteState(executeId, ExecuteState::CANCELED)) {
        return false;
    }
    iter->second->isCanceled = true;
    return true;
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

void TaskManager::AddExecuteState(uint32_t executeId)
{
    std::unique_lock<std::shared_mutex> lock(executeStatesMutex_);
    executeStates_.emplace(executeId, ExecuteState::WAITING);
}

bool TaskManager::UpdateExecuteState(uint32_t executeId, ExecuteState state)
{
    std::unique_lock<std::shared_mutex> lock(executeStatesMutex_);
    auto iter = executeStates_.find(executeId);
    if (iter == executeStates_.end()) {
        return false;
    }
    std::string traceInfo = "UpdateExecuteState: executeId : " + std::to_string(executeId) +
                            ", executeState : " + std::to_string(state);
    HITRACE_HELPER_METER_NAME(traceInfo);
    iter->second = state;
    return true;
}

void TaskManager::RemoveExecuteState(uint32_t executeId)
{
    std::unique_lock<std::shared_mutex> lock(executeStatesMutex_);
    executeStates_.erase(executeId);
}

ExecuteState TaskManager::QueryExecuteState(uint32_t executeId)
{
    std::shared_lock<std::shared_mutex> lock(executeStatesMutex_);
    auto iter = executeStates_.find(executeId);
    if (iter == executeStates_.end()) {
        HILOG_DEBUG("taskpool:: Can not find the target task");
        return ExecuteState::NOT_FOUND;
    }
    return iter->second;
}

void TaskManager::CancelTask(napi_env env, uint32_t taskId)
{
    // Cannot find task by taskId, throw error
    std::unique_lock<std::shared_mutex> lock(runningInfosMutex_);
    auto iter = runningInfos_.find(taskId);
    if (iter == runningInfos_.end() || iter->second.empty()) {
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_CANCEL_NONEXIST_TASK);
        HILOG_ERROR("taskpool:: query non-existent task");
        return;
    }
    for (uint32_t executeId : iter->second) {
        CancelExecution(env, executeId);
    }
}

void TaskManager::CancelExecution(napi_env env, uint32_t executeId)
{
    // 1. Cannot find taskInfo by executeId, throw error
    // 2. Find executing taskInfo, skip it
    // 3. Find waiting taskInfo, cancel it
    // 4. Find canceled taskInfo, skip it
    ExecuteState state = QueryExecuteState(executeId);
    TaskInfo* taskInfo = nullptr;
    switch (state) {
        case ExecuteState::NOT_FOUND:
            ErrorHelper::ThrowError(env, ErrorHelper::ERR_CANCEL_NONEXIST_TASK);
            HILOG_ERROR("taskpool:: cancel non-existent task");
            return;
        case ExecuteState::RUNNING:
            if (!MarkCanceledState(executeId)) {
                ErrorHelper::ThrowError(env, ErrorHelper::ERR_CANCEL_NONEXIST_TASK,
                                        "taskpool:: fail to mark cancel state");
                return;
            }
            break;
        case ExecuteState::WAITING:
            HILOG_DEBUG("taskpool:: Cancel waiting task");
            taskInfo = PopTaskInfo(executeId);
            if (taskInfo != nullptr) {
                napi_value undefined = NapiHelper::GetUndefinedValue(taskInfo->env);
                napi_reject_deferred(taskInfo->env, taskInfo->deferred, undefined);
                ReleaseTaskContent(taskInfo);
            }
            RemoveExecuteState(executeId);
            break;
        default: // Default is CANCELED, means task isCanceled, do not need to mark again.
            break;
    }
}

TaskInfo* TaskManager::GenerateTaskInfo(napi_env env, napi_value func, napi_value args,
                                        uint32_t taskId, uint32_t executeId, napi_value transferList)
{
    napi_value undefined = NapiHelper::GetUndefinedValue(env);
    napi_value serializationFunction;
    napi_status status = napi_serialize(env, func, undefined, &serializationFunction);
    if (status != napi_ok || serializationFunction == nullptr) {
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_WORKER_SERIALIZATION, "taskpool: failed to serialize function.");
        return nullptr;
    }
    napi_value serializationArguments;
    if (transferList == nullptr) {
        HILOG_DEBUG("taskpool:: task params default transfer");
        status = napi_serialize(env, args, args, &serializationArguments);
    } else {
        HILOG_DEBUG("taskpool:: call setTransferList to set task params transfer or not transfer");
        status = napi_serialize(env, args, transferList, &serializationArguments);
    }
    if (status != napi_ok || serializationArguments == nullptr) {
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_WORKER_SERIALIZATION, "taskpool: failed to serialize arguments.");
        return nullptr;
    }
    TaskInfo* taskInfo = new TaskInfo();
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
    reinterpret_cast<NativeEngine*>(env)->IncreaseSubEnvCounter();
    return taskInfo;
}

TaskInfo* TaskManager::GenerateTaskInfoFromTask(napi_env env, napi_value task, uint32_t executeId)
{
    napi_value function = NapiHelper::GetNameProperty(env, task, FUNCTION_STR);
    napi_value arguments = NapiHelper::GetNameProperty(env, task, ARGUMENTS_STR);
    napi_value taskId = NapiHelper::GetNameProperty(env, task, TASKID_STR);
    if (function == nullptr || arguments == nullptr || taskId == nullptr) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: task value is error");
        return nullptr;
    }
    napi_value transferList = nullptr;
    if (NapiHelper::HasNameProperty(env, task, TRANSFERLIST_STR)) {
        transferList = NapiHelper::GetNameProperty(env, task, TRANSFERLIST_STR);
    }
    uint32_t id = NapiHelper::GetUint32Value(env, taskId);
    TaskInfo* taskInfo = GenerateTaskInfo(env, function, arguments, id, executeId, transferList);
    return taskInfo;
}

void TaskManager::ReleaseTaskContent(TaskInfo* taskInfo)
{
    reinterpret_cast<NativeEngine*>(taskInfo->env)->DecreaseSubEnvCounter();
    if (taskInfo->onResultSignal != nullptr &&
        !uv_is_closing(reinterpret_cast<uv_handle_t*>(taskInfo->onResultSignal))) {
        ConcurrentHelper::UvHandleClose(taskInfo->onResultSignal);
    }
    delete taskInfo;
    taskInfo = nullptr;
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
}

uint32_t TaskManager::GetIdleWorkers()
{
    std::lock_guard<std::recursive_mutex> lock(workersMutex_);
    return idleWorkers_.size();
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

void TaskManager::EnqueueExecuteId(uint32_t executeId, Priority priority)
{
    {
        std::lock_guard<std::mutex> lock(taskQueuesMutex_);
        taskQueues_[priority]->EnqueueExecuteId(executeId);
    }
    NotifyExecuteTask();
}

std::pair<uint32_t, Priority> TaskManager::DequeueExecuteId()
{
    std::lock_guard<std::mutex> lock(taskQueuesMutex_);
    auto& highTaskQueue = taskQueues_[Priority::HIGH];
    if (!highTaskQueue->IsEmpty() && highPrioExecuteCount_ < HIGH_PRIORITY_TASK_COUNT) {
        auto& highTaskQueue = taskQueues_[Priority::HIGH];
        highPrioExecuteCount_++;
        return std::make_pair(highTaskQueue->DequeueExecuteId(), Priority::HIGH);
    }
    highPrioExecuteCount_ = 0;

    auto& mediumTaskQueue = taskQueues_[Priority::MEDIUM];
    if (!mediumTaskQueue->IsEmpty() && mediumPrioExecuteCount_ < MEDIUM_PRIORITY_TASK_COUNT) {
        mediumPrioExecuteCount_++;
        return std::make_pair(mediumTaskQueue->DequeueExecuteId(), Priority::MEDIUM);
    }
    mediumPrioExecuteCount_ = 0;

    auto& lowTaskQueue = taskQueues_[Priority::LOW];
    return std::make_pair(lowTaskQueue->DequeueExecuteId(), Priority::LOW);
}

void TaskManager::NotifyExecuteTask()
{
    std::lock_guard<std::recursive_mutex> lock(workersMutex_);
    if (idleWorkers_.empty()) {
        return;
    }
    auto candidator = idleWorkers_.begin();
    Worker* worker = *candidator;
    worker->executeInfo_ = TaskManager::GetInstance().DequeueExecuteId();
    worker->NotifyExecuteTask();
    idleWorkers_.erase(candidator);
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
        uv_async_send(notifyRestartTimer_);
    }

    if (worker->state_ == WorkerState::BLOCKED) {
        // since the worker is blocked, we should add it to the timeout set
        timeoutWorkers_.insert(worker);
        return;
    }
    // Since the worker may be executing some tasks in IO thread, we should add it to the
    // worker sets and call the 'NotifyWorkerIdle', which can still execute some tasks in its own thread.
    workers_.emplace(worker);
    idleWorkers_.emplace_hint(idleWorkers_.end(), worker);
    if (GetTaskNum() != 0) {
        NotifyExecuteTask();
    }
}

napi_value TaskManager::IsCanceled(napi_env env, napi_callback_info cbinfo)
{
    bool isCanceled = false;
    auto engine = reinterpret_cast<NativeEngine*>(env);
    if (!engine->IsTaskPoolThread()) {
        HILOG_ERROR("taskpool:: call isCanceled not in taskpool thread");
        return NapiHelper::CreateBooleanValue(env, isCanceled);
    }
    // Get taskInfo and query task cancel state
    void* data = engine->GetCurrentTaskInfo();
    if (data == nullptr) {
        HILOG_ERROR("taskpool:: call isCanceled not in Concurrent function");
    } else {
        TaskInfo* taskInfo = static_cast<TaskInfo*>(data);
        isCanceled = taskInfo->isCanceled;
    }
    return NapiHelper::CreateBooleanValue(env, isCanceled);
}

// ----------------------------------- TaskGroupManager ----------------------------------------
TaskGroupManager &TaskGroupManager::GetInstance()
{
    static TaskGroupManager groupManager;
    return groupManager;
}

uint32_t TaskGroupManager::GenerateGroupId()
{
    return groupId_++;
}

GroupInfo* TaskGroupManager::GenerateGroupInfo(napi_env env, uint32_t taskNum, uint32_t groupId,
                                               uint32_t groupExecuteId)
{
    GroupInfo* groupInfo = new GroupInfo();
    groupInfo->taskNum = taskNum;
    groupInfo->groupId = groupId;
    napi_value resArr;
    napi_create_array_with_length(env, taskNum, &resArr);
    napi_ref arrRef = NapiHelper::CreateReference(env, resArr, 1);
    groupInfo->resArr = arrRef;
    StoreExecuteId(groupId, groupExecuteId);
    StoreRunningExecuteId(groupExecuteId);
    AddGroupInfoById(groupExecuteId, groupInfo);
    return groupInfo;
}

void TaskGroupManager::ClearGroupInfo(napi_env env, uint32_t groupExecuteId, GroupInfo* groupInfo)
{
    RemoveRunningExecuteId(groupExecuteId);
    RemoveGroupInfoById(groupExecuteId);
    napi_delete_reference(env, groupInfo->resArr);
    delete groupInfo;
    groupInfo = nullptr;
}

void TaskGroupManager::AddTask(uint32_t groupId, napi_ref task)
{
    std::unique_lock<std::shared_mutex> lock(tasksMutex_);
    auto iter = tasks_.find(groupId);
    if (iter == tasks_.end()) {
        std::list<napi_ref> list {task};
        tasks_.emplace(groupId, list);
    } else {
        iter->second.push_back(task);
    }
}

const std::list<napi_ref>& TaskGroupManager::GetTasksByGroup(uint32_t groupId)
{
    std::shared_lock<std::shared_mutex> lock(tasksMutex_);
    auto iter = tasks_.find(groupId);
    if (iter == tasks_.end()) {
        static const std::list<napi_ref> EMPTY_TASK_LIST {};
        return EMPTY_TASK_LIST;
    }
    return iter->second;
}

void TaskGroupManager::ClearTasks(napi_env env, uint32_t groupId)
{
    std::unique_lock<std::shared_mutex> lock(tasksMutex_);
    auto iter = tasks_.find(groupId);
    if (iter == tasks_.end()) {
        return;
    }
    for (napi_ref task : iter->second) {
        napi_delete_reference(env, task);
    }
    tasks_.erase(iter);
}

void TaskGroupManager::StoreExecuteId(uint32_t groupId, uint32_t groupExecuteId)
{
    std::lock_guard<std::mutex> lock(groupExecuteIdsMutex_);
    auto iter = groupExecuteIds_.find(groupId);
    if (iter == groupExecuteIds_.end()) {
        std::list<uint32_t> list {groupExecuteId};
        groupExecuteIds_.emplace(groupId, std::move(list));
    } else {
        iter->second.push_back(groupExecuteId);
    }
}

void TaskGroupManager::RemoveExecuteId(uint32_t groupId, uint32_t groupExecuteId)
{
    std::lock_guard<std::mutex> lock(groupExecuteIdsMutex_);
    auto iter = groupExecuteIds_.find(groupId);
    if (iter != groupExecuteIds_.end()) {
        iter->second.remove(groupExecuteId);
    }
    if (iter->second.empty()) {
        groupExecuteIds_.erase(iter);
    }
}

void TaskGroupManager::ClearExecuteId(uint32_t groupId)
{
    std::lock_guard<std::mutex> lock(groupExecuteIdsMutex_);
    groupExecuteIds_.erase(groupId);
}

void TaskGroupManager::CancelGroup(napi_env env, uint32_t groupId)
{
    std::lock_guard<std::mutex> lock(groupExecuteIdsMutex_);
    auto iter = groupExecuteIds_.find(groupId);
    if (iter == groupExecuteIds_.end() || iter->second.empty()) {
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_CANCEL_NONEXIST_TASK_GROUP);
        HILOG_ERROR("taskpool:: cancel non-existent task group");
        return;
    }
    for (uint32_t groupExecuteId : iter->second) {
        bool isRunning = IsRunning(groupExecuteId);
        if (!isRunning) {
            continue;
        }
        GroupInfo* info = GetGroupInfoByExecutionId(groupExecuteId);
        if (info == nullptr) {
            continue;
        }
        const std::list<uint32_t>& executeList = info->executeIds;
        for (uint32_t executeId : executeList) {
            CancelGroupExecution(executeId);
        }
        napi_value undefined = NapiHelper::GetUndefinedValue(env);
        napi_reject_deferred(env, info->deferred, undefined);
        TaskGroupManager::GetInstance().ClearGroupInfo(env, groupExecuteId, info);
    }
}

uint32_t TaskGroupManager::GenerateGroupExecuteId()
{
    return groupExecuteId_++;
}

void TaskGroupManager::StoreRunningExecuteId(uint32_t groupExecuteId)
{
    std::unique_lock<std::shared_mutex> lock(groupExecutionsMutex_);
    runningGroupExecutions_.insert(groupExecuteId);
}

void TaskGroupManager::RemoveRunningExecuteId(uint32_t groupExecuteId)
{
    std::unique_lock<std::shared_mutex> lock(groupExecutionsMutex_);
    runningGroupExecutions_.erase(groupExecuteId);
}

bool TaskGroupManager::IsRunning(uint32_t groupExecuteId)
{
    std::shared_lock<std::shared_mutex> lock(groupExecutionsMutex_);
    bool isRunning = runningGroupExecutions_.find(groupExecuteId) != runningGroupExecutions_.end();
    return isRunning;
}

void TaskGroupManager::AddGroupInfoById(uint32_t groupExecuteId, GroupInfo* info)
{
    std::unique_lock<std::shared_mutex> lock(groupInfoMapMutex_);
    groupInfoMap_.emplace(groupExecuteId, info);
}

void TaskGroupManager::RemoveGroupInfoById(uint32_t groupExecuteId)
{
    std::unique_lock<std::shared_mutex> lock(groupInfoMapMutex_);
    groupInfoMap_.erase(groupExecuteId);
}

GroupInfo* TaskGroupManager::GetGroupInfoByExecutionId(uint32_t groupExecuteId)
{
    std::shared_lock<std::shared_mutex> lock(groupInfoMapMutex_);
    auto iter = groupInfoMap_.find(groupExecuteId);
    if (iter != groupInfoMap_.end()) {
        return iter->second;
    }
    return nullptr;
}

void TaskGroupManager::CancelGroupExecution(uint32_t executeId)
{
    ExecuteState state = TaskManager::GetInstance().QueryExecuteState(executeId);
    TaskInfo* taskInfo = nullptr;
    switch (state) {
        case ExecuteState::NOT_FOUND:
            break;
        case ExecuteState::RUNNING:
            TaskManager::GetInstance().MarkCanceledState(executeId);
            break;
        case ExecuteState::WAITING:
            HILOG_DEBUG("taskpool:: Cancel waiting task in group");
            taskInfo = TaskManager::GetInstance().PopTaskInfo(executeId);
            if (taskInfo == nullptr) {
                HILOG_ERROR("taskpool:: taskInfo is nullptr when cancel waiting execution");
                return;
            }
            TaskManager::GetInstance().RemoveExecuteState(executeId);
            TaskManager::GetInstance().ReleaseTaskContent(taskInfo);
            break;
        default: // Default is CANCELED, means task isCanceled, do not need to mark again.
            break;
    }
}
} // namespace Commonlibrary::Concurrent::TaskPoolModule
