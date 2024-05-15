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

#include "worker.h"

#if defined(ENABLE_TASKPOOL_FFRT)
#include "c/executor_task.h"
#include "ffrt_inner.h"
#endif
#include "commonlibrary/ets_utils/js_sys_module/timer/timer.h"
#include "helper/hitrace_helper.h"
#include "process_helper.h"
#include "task_group.h"
#include "task_manager.h"
#include "tools/log.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
using namespace OHOS::JsSysModule;
using namespace Commonlibrary::Platform;

Worker::PriorityScope::PriorityScope(Worker* worker, Priority taskPriority) : worker_(worker)
{
    if (taskPriority != worker->priority_) {
        HILOG_DEBUG("taskpool:: reset worker priority to match task priority");
        if (TaskManager::GetInstance().IsSystemApp()) {
#if defined(ENABLE_TASKPOOL_FFRT)
            if (ffrt::this_task::update_qos(WORKERPRIORITY_FFRTQOS_MAP.at(taskPriority)) != 0) {
                SetWorkerPriority(taskPriority);
            }
#endif
        } else {
            SetWorkerPriority(taskPriority);
        }
        worker->priority_ = taskPriority;
    }
}

Worker::RunningScope::~RunningScope()
{
    if (scope_ != nullptr) {
        napi_close_handle_scope(worker_->workerEnv_, scope_);
    }
    worker_->NotifyIdle();
    worker_->idleState_ = true;
}

Worker* Worker::WorkerConstructor(napi_env env)
{
    HITRACE_HELPER_METER_NAME("TaskWorkerConstructor: [Add Thread]");
    Worker* worker = new Worker(env);
    worker->StartExecuteInThread();
    return worker;
}

void Worker::ReleaseWorkerHandles(const uv_async_t* req)
{
    auto worker = static_cast<Worker*>(req->data);
    HILOG_DEBUG("taskpool:: enter the worker loop and try to release thread: %{public}d", worker->tid_);
    if (!worker->CheckFreeConditions()) {
        return;
    }

    TaskManager::GetInstance().RemoveWorker(worker);
    HITRACE_HELPER_METER_NAME("ReleaseWorkerHandles: [Release Thread]");
    HILOG_INFO("taskpool:: the thread is idle and will be released, and the total num is %{public}u now",
        TaskManager::GetInstance().GetThreadNum());
    // when there is no active handle, worker loop will stop automatically.
    ConcurrentHelper::UvHandleClose(worker->performTaskSignal_);
#if !defined(WINDOWS_PLATFORM) && !defined(MAC_PLATFORM)
    ConcurrentHelper::UvHandleClose(worker->debuggerOnPostTaskSignal_);
#endif
    ConcurrentHelper::UvHandleClose(worker->clearWorkerSignal_);

    uv_loop_t* loop = worker->GetWorkerLoop();
    if (loop != nullptr) {
        uv_stop(loop);
    }
}

bool Worker::CheckFreeConditions()
{
    auto workerEngine = reinterpret_cast<NativeEngine*>(workerEnv_);
    // only when all conditions are met can the worker be freed
    if (workerEngine->HasListeningCounter()) {
        HILOG_DEBUG("taskpool:: listening operation exists, the worker thread will not exit");
    } else if (Timer::HasTimer(workerEnv_)) {
        HILOG_DEBUG("taskpool:: timer exists, the worker thread will not exit");
    } else if (workerEngine->HasWaitingRequest()) {
        HILOG_DEBUG("taskpool:: waiting request exists, the worker thread will not exit");
    } else if (workerEngine->HasSubEnv()) {
        HILOG_DEBUG("taskpool:: sub env exists, the worker thread will not exit");
    } else if (workerEngine->HasPendingJob()) {
        HILOG_DEBUG("taskpool:: pending job exists, the worker thread will not exit");
    } else if (workerEngine->IsProfiling()) {
        HILOG_DEBUG("taskpool:: the worker thread will not exit during profiling");
    } else {
        return true;
    }
    HILOG_DEBUG("taskpool:: the worker %{public}d can't be released due to not meeting the conditions", tid_);
    TaskManager& taskManager = TaskManager::GetInstance();
    taskManager.RestoreWorker(this);
    taskManager.CountTraceForWorker();
    return false;
}

void Worker::StartExecuteInThread()
{
    if (!runner_) {
        runner_ = std::make_unique<TaskRunner>(TaskStartCallback(ExecuteInThread, this));
    }
    if (runner_) {
        runner_->Execute(); // start a new thread
    } else {
        HILOG_ERROR("taskpool:: runner_ is nullptr");
    }
}

#if !defined(WINDOWS_PLATFORM) && !defined(MAC_PLATFORM)
void Worker::HandleDebuggerTask(const uv_async_t* req)
{
    Worker* worker = reinterpret_cast<Worker*>(req->data);
    if (worker == nullptr) {
        HILOG_ERROR("taskpool:: worker is null");
        return;
    }
    worker->debuggerMutex_.lock();
    auto task = std::move(worker->debuggerQueue_.front());
    worker->debuggerQueue_.pop();
    worker->debuggerMutex_.unlock();
    task();
}

void Worker::DebuggerOnPostTask(std::function<void()>&& task)
{
    if (uv_is_active(reinterpret_cast<uv_handle_t*>(debuggerOnPostTaskSignal_))) {
        std::lock_guard<std::mutex> lock(debuggerMutex_);
        debuggerQueue_.push(std::move(task));
        uv_async_send(debuggerOnPostTaskSignal_);
    }
}
#endif

#if defined(ENABLE_TASKPOOL_FFRT)
void Worker::InitFfrtInfo()
{
    if (TaskManager::GetInstance().IsSystemApp()) {
        static const std::map<int, Priority> FFRTQOS_WORKERPRIORITY_MAP = {
            {ffrt::qos_utility, Priority::LOW},
            {ffrt::qos_default, Priority::DEFAULT},
            {ffrt::qos_user_initiated, Priority::HIGH},
        };
        ffrt_qos_t qos = ffrt_this_task_get_qos();
        priority_ = FFRTQOS_WORKERPRIORITY_MAP.at(qos);
        ffrtTaskHandle_ = ffrt_get_cur_task();
    }
}

void Worker::InitLoopHandleNum()
{
    if (ffrtTaskHandle_ == nullptr) {
        return;
    }

    uv_loop_t* loop = GetWorkerLoop();
    if (loop != nullptr) {
        initActiveHandleNum_ = loop->active_handles;
    } else {
        HILOG_ERROR("taskpool:: worker loop is nullptr when init loop handle num.");
    }
}

bool Worker::IsLoopActive()
{
    uv_loop_t* loop = GetWorkerLoop();
    if (loop != nullptr) {
        return uv_loop_alive_taskpool(loop, initActiveHandleNum_);
    } else {
        HILOG_ERROR("taskpool:: worker loop is nullptr when judge loop alive.");
        return false;
    }
}

uint64_t Worker::GetWaitTime()
{
    return ffrt_epoll_get_wait_time(ffrtTaskHandle_);
}
#endif

void Worker::ExecuteInThread(const void* data)
{
    HITRACE_HELPER_START_TRACE(__PRETTY_FUNCTION__);
    auto worker = reinterpret_cast<Worker*>(const_cast<void*>(data));
    {
        napi_create_runtime(worker->hostEnv_, &worker->workerEnv_);
        if (worker->workerEnv_ == nullptr) {
            HILOG_ERROR("taskpool:: worker create runtime failed");
            return;
        }
        auto workerEngine = reinterpret_cast<NativeEngine*>(worker->workerEnv_);
        // mark worker env is taskpoolThread
        workerEngine->MarkTaskPoolThread();
        workerEngine->InitTaskPoolThread(worker->workerEnv_, Worker::TaskResultCallback);
    }
    uv_loop_t* loop = worker->GetWorkerLoop();
    if (loop == nullptr) {
        HILOG_ERROR("taskpool:: loop is nullptr");
        return;
    }
    // save the worker tid
    worker->tid_ = GetThreadId();

    // Init worker task execute signal
    ConcurrentHelper::UvHandleInit(loop, worker->performTaskSignal_, Worker::PerformTask, worker);
    ConcurrentHelper::UvHandleInit(loop, worker->clearWorkerSignal_, Worker::ReleaseWorkerHandles, worker);

    HITRACE_HELPER_FINISH_TRACE;
#if !defined(WINDOWS_PLATFORM) && !defined(MAC_PLATFORM)
    // Init debugger task post signal
    ConcurrentHelper::UvHandleInit(loop, worker->debuggerOnPostTaskSignal_, Worker::HandleDebuggerTask, worker);
#endif
    if (worker->PrepareForWorkerInstance()) {
        // Call after uv_async_init
        worker->NotifyWorkerCreated();
#if defined(ENABLE_TASKPOOL_FFRT)
        worker->InitFfrtInfo();
        worker->InitLoopHandleNum();
#endif
        worker->RunLoop();
    } else {
        HILOG_ERROR("taskpool:: Worker PrepareForWorkerInstance fail");
    }
    TaskManager::GetInstance().RemoveWorker(worker);
    TaskManager::GetInstance().CountTraceForWorker();
    worker->ReleaseWorkerThreadContent();
    delete worker;
    worker = nullptr;
}

bool Worker::PrepareForWorkerInstance()
{
    HITRACE_HELPER_METER_NAME(__PRETTY_FUNCTION__);
    auto workerEngine = reinterpret_cast<NativeEngine*>(workerEnv_);
#if !defined(WINDOWS_PLATFORM) && !defined(MAC_PLATFORM)
    workerEngine->SetDebuggerPostTaskFunc(
        std::bind(&Worker::DebuggerOnPostTask, this, std::placeholders::_1));
#endif
    if (!workerEngine->CallInitWorkerFunc(workerEngine)) {
        HILOG_ERROR("taskpool:: Worker CallInitWorkerFunc fail");
        return false;
    }
    // register timer interface
    Timer::RegisterTime(workerEnv_);

    // Check exception after worker construction
    if (NapiHelper::IsExceptionPending(workerEnv_)) {
        HILOG_ERROR("taskpool:: Worker construction occur exception");
        return false;
    }
    return true;
}

void Worker::ReleaseWorkerThreadContent()
{
    auto workerEngine = reinterpret_cast<NativeEngine*>(workerEnv_);
    auto hostEngine = reinterpret_cast<NativeEngine*>(hostEnv_);
    if (workerEngine == nullptr) {
        HILOG_ERROR("taskpool:: workerEngine is nullptr");
        return;
    }
    if (hostEngine != nullptr) {
        if (!hostEngine->DeleteWorker(workerEngine)) {
            HILOG_ERROR("taskpool:: DeleteWorker fail");
        }
    }
    if (state_ == WorkerState::BLOCKED) {
        HITRACE_HELPER_METER_NAME("Thread Timeout Exit");
    } else {
        HITRACE_HELPER_METER_NAME("Thread Exit");
    }

    Timer::ClearEnvironmentTimer(workerEnv_);
    // 2. delete NativeEngine created in worker thread
    if (!workerEngine->CallOffWorkerFunc(workerEngine)) {
        HILOG_ERROR("worker:: CallOffWorkerFunc error");
    }
    delete workerEngine;
    workerEnv_ = nullptr;
}

void Worker::NotifyExecuteTask()
{
    if (LIKELY(uv_is_active(reinterpret_cast<uv_handle_t*>(performTaskSignal_)))) {
        uv_async_send(performTaskSignal_);
    }
}

void Worker::NotifyIdle()
{
    TaskManager::GetInstance().NotifyWorkerIdle(this);
}

void Worker::NotifyWorkerCreated()
{
    TaskManager::GetInstance().NotifyWorkerCreated(this);
}

void Worker::NotifyTaskFinished()
{
    auto workerEngine = reinterpret_cast<NativeEngine*>(workerEnv_);
    if (--runningCount_ != 0 || workerEngine->HasPendingJob()) {
        // the worker state is still RUNNING and the start time will be updated
        startTime_ = ConcurrentHelper::GetMilliseconds();
    } else {
        UpdateWorkerState(WorkerState::RUNNING, WorkerState::IDLE);
    }
    idlePoint_ = ConcurrentHelper::GetMilliseconds();
}

bool Worker::UpdateWorkerState(WorkerState expect, WorkerState desired)
{
    return state_.compare_exchange_strong(expect, desired);
}

void Worker::PerformTask(const uv_async_t* req)
{
    uint64_t startTime = ConcurrentHelper::GetMilliseconds();
    auto worker = static_cast<Worker*>(req->data);
    napi_env env = worker->workerEnv_;
    TaskManager::GetInstance().NotifyWorkerRunning(worker);
    auto taskInfo = TaskManager::GetInstance().DequeueTaskId();
    if (taskInfo.first == 0) {
        worker->NotifyIdle();
        return;
    }
    RunningScope runningScope(worker);
    PriorityScope priorityScope(worker, taskInfo.second);
    Task* task = TaskManager::GetInstance().GetTask(taskInfo.first);
    if (task == nullptr) {
        HILOG_DEBUG("taskpool:: task has been released");
        return;
    }
    if (!task->UpdateTask(startTime, worker)) {
        return;
    }
    if (task->IsGroupTask() && (!TaskGroupManager::GetInstance().UpdateGroupState(task->groupId_))) {
        return;
    }
    if (task->IsLongTask()) {
        worker->UpdateLongTaskInfo(task);
    }
    worker->StoreTaskId(task->taskId_);
    // tag for trace parse: Task Perform
    std::string strTrace = "Task Perform: name : "  + task->name_ + ", taskId : " + std::to_string(task->taskId_)
                            + ", priority : " + std::to_string(taskInfo.second);
    HITRACE_HELPER_METER_NAME(strTrace);
    napi_value func = task->DeserializeValue(env, true, false);
    if (func == nullptr) {
        return;
    }
    napi_value args = task->DeserializeValue(env, false, true);
    if (args == nullptr) {
        return;
    }
    if (!worker->InitTaskPoolFunc(env, func, task)) {
        return;
    }
    uint32_t argsNum = NapiHelper::GetArrayLength(env, args);
    napi_value argsArray[argsNum];
    for (size_t i = 0; i < argsNum; i++) {
        argsArray[i] = NapiHelper::GetElement(env, args, i);
    }
    if (task->onStartExecutionSignal_ != nullptr) {
        uv_async_send(task->onStartExecutionSignal_);
    }
    napi_call_function(env, NapiHelper::GetGlobalObject(env), func, argsNum, argsArray, nullptr);
    auto workerEngine = reinterpret_cast<NativeEngine*>(env);
    workerEngine->ClearCurrentTaskInfo();
    task->DecreaseRefCount();
    task->StoreTaskDuration();
    worker->UpdateExecutedInfo();
    HandleFunctionException(env, task);
}

void Worker::NotifyTaskResult(napi_env env, Task* task, napi_value result)
{
    HITRACE_HELPER_METER_NAME(__PRETTY_FUNCTION__);
    void* resultData = nullptr;
    napi_value undefined = NapiHelper::GetUndefinedValue(env);
    bool defaultTransfer = true;
    bool defaultCloneSendable = false;
    napi_status status = napi_serialize_inner(env, result, undefined, undefined,
                                              defaultTransfer, defaultCloneSendable, &resultData);
    if ((status != napi_ok || resultData == nullptr) && task->success_) {
        task->success_ = false;
        std::string errMessage = "taskpool: failed to serialize result.";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        napi_value err = ErrorHelper::NewError(env, ErrorHelper::ERR_WORKER_SERIALIZATION, errMessage.c_str());
        NotifyTaskResult(env, task, err);
        return;
    }
    task->result_ = resultData;
    NotifyHandleTaskResult(task);
}

void Worker::NotifyHandleTaskResult(Task* task)
{
    if (!task->IsReadyToHandle()) {
        return;
    }
    Worker* worker = reinterpret_cast<Worker*>(task->worker_);
    if (worker != nullptr) {
        std::lock_guard<std::mutex> lock(worker->currentTaskIdMutex_);
        auto iter = std::find(worker->currentTaskId_.begin(), worker->currentTaskId_.end(), task->taskId_);
        if (iter != worker->currentTaskId_.end()) {
            worker->currentTaskId_.erase(iter);
        }
    }
    uv_async_send(task->onResultSignal_);
    worker->NotifyTaskFinished();
}

void Worker::TaskResultCallback(napi_env env, napi_value result, bool success, void* data)
{
    HITRACE_HELPER_METER_NAME(__PRETTY_FUNCTION__);
    if (env == nullptr) {
        HILOG_FATAL("taskpool:: TaskResultCallback engine is null");
        return;
    }
    if (data == nullptr) {
        HILOG_FATAL("taskpool:: task is nullptr");
        return;
    }
    Task* task = static_cast<Task*>(data);
    auto worker = static_cast<Worker*>(task->worker_);
    worker->isExecutingLongTask_ = task->IsLongTask();
    task->DecreaseRefCount();
    task->ioTime_ = ConcurrentHelper::GetMilliseconds();
    if (task->cpuTime_ != 0) {
        uint64_t ioDuration = task->ioTime_ - task->startTime_;
        uint64_t cpuDuration = task->cpuTime_ - task->startTime_;
        TaskManager::GetInstance().StoreTaskDuration(task->taskId_, std::max(ioDuration, cpuDuration), cpuDuration);
    }
    task->success_ = success;
    NotifyTaskResult(env, task, result);
}

// reset qos_user_initiated after perform task
void Worker::ResetWorkerPriority()
{
    if (priority_ != Priority::HIGH) {
        if (TaskManager::GetInstance().IsSystemApp()) {
#if defined(ENABLE_TASKPOOL_FFRT)
            if (ffrt::this_task::update_qos(WORKERPRIORITY_FFRTQOS_MAP.at(Priority::HIGH)) != 0) {
                SetWorkerPriority(Priority::HIGH);
            }
#endif
        } else {
            SetWorkerPriority(Priority::HIGH);
        }
        priority_ = Priority::HIGH;
    }
}

void Worker::StoreTaskId(uint64_t taskId)
{
    std::lock_guard<std::mutex> lock(currentTaskIdMutex_);
    currentTaskId_.emplace_back(taskId);
}

bool Worker::InitTaskPoolFunc(napi_env env, napi_value func, Task* task)
{
    auto workerEngine = reinterpret_cast<NativeEngine*>(env);
    bool success = workerEngine->InitTaskPoolFunc(env, func, task);
    napi_value exception;
    napi_get_and_clear_last_exception(env, &exception);
    if (exception != nullptr) {
        HILOG_ERROR("taskpool:: InitTaskPoolFunc occur exception");
        task->success_ = false;
        napi_value errorEvent = ErrorHelper::TranslateErrorEvent(env, exception);
        NotifyTaskResult(env, task, errorEvent);
        return false;
    }
    if (!success) {
        HILOG_ERROR("taskpool:: InitTaskPoolFunc fail");
        napi_value err = ErrorHelper::NewError(env, ErrorHelper::TYPE_ERROR,
                                               "taskpool:: function may not be concurrent.");
        task->success_ = false;
        NotifyTaskResult(env, task, err);
        return false;
    }
    return true;
}

void Worker::UpdateExecutedInfo()
{
    // if the worker is blocked, just skip
    if (LIKELY(state_ != WorkerState::BLOCKED)) {
        uint64_t duration = ConcurrentHelper::GetMilliseconds() - startTime_;
        TaskManager::GetInstance().UpdateExecutedInfo(duration);
    }
}

// Only when the worker has no longTask can it be released.
void Worker::TerminateTask(uint64_t taskId)
{
    std::lock_guard<std::mutex> lock(longMutex_);
    longTasksSet_.erase(taskId);
    if (longTasksSet_.empty()) {
        hasLongTask_ = false;
    }
}

// to store longTasks' state
void Worker::UpdateLongTaskInfo(Task* task)
{
    TaskManager::GetInstance().StoreLongTaskInfo(task->taskId_, this);
    std::lock_guard<std::mutex> lock(longMutex_);
    hasLongTask_ = true;
    isExecutingLongTask_ = true;
    longTasksSet_.emplace(task->taskId_);
}

bool Worker::IsExecutingLongTask()
{
    return isExecutingLongTask_;
}

bool Worker::HasLongTask()
{
    return hasLongTask_;
}

void Worker::HandleFunctionException(napi_env env, Task* task)
{
    napi_value exception;
    napi_get_and_clear_last_exception(env, &exception);
    if (exception != nullptr) {
        HILOG_ERROR("taskpool::PerformTask occur exception");
        task->DecreaseRefCount();
        task->success_ = false;
        napi_value errorEvent = ErrorHelper::TranslateErrorEvent(env, exception);
        NotifyTaskResult(env, task, errorEvent);
        return;
    }
    NotifyHandleTaskResult(task);
}
} // namespace Commonlibrary::Concurrent::TaskPoolModule