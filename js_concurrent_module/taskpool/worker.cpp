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
#include "helper/concurrent_helper.h"

#if defined(ENABLE_TASKPOOL_FFRT)
#include "c/executor_task.h"
#include "ffrt_inner.h"
#endif
#include "sys_timer.h"
#include "helper/hitrace_helper.h"
#include "process_helper.h"
#include "sequence_runner_manager.h"
#include "taskpool.h"
#include "task_group_manager.h"
#include "native_engine.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
using namespace OHOS::JsSysModule;
using namespace Commonlibrary::Platform;
static constexpr uint32_t TASKPOOL_TYPE = 2;
static constexpr uint32_t WORKER_ALIVE_TIME = 1800000; // 1800000: 30min
static constexpr int32_t MAX_REPORT_TIMES = 3;

Worker::PriorityScope::PriorityScope(Worker* worker, Priority taskPriority) : worker_(worker)
{
    if (taskPriority != worker->priority_) {
        HILOG_DEBUG("taskpool:: reset worker priority to match task priority");
        if (TaskManager::GetInstance().EnableFfrt()) {
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
    HILOG_DEBUG("taskpool:: RunningScope destruction");
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

void Worker::CloseHandles()
{
    // set all handles to nullptr so that they can not be used even when the loop is re-running
    ConcurrentHelper::UvHandleClose(performTaskSignal_);
#if !defined(WINDOWS_PLATFORM) && !defined(MAC_PLATFORM)
    ConcurrentHelper::UvHandleClose(debuggerOnPostTaskSignal_);
#endif
    ConcurrentHelper::UvHandleClose(clearWorkerSignal_);
    ConcurrentHelper::UvHandleClose(triggerGCCheckSignal_);
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
    HILOG_INFO("taskpool:shrink,now %{public}u", TaskManager::GetInstance().GetThreadNum());
    // when there is no active handle, worker loop will stop automatically.
    worker->CloseHandles();

    uv_loop_t* loop = worker->GetWorkerLoop();
    if (loop != nullptr) {
        uv_stop(loop);
    }
}

bool Worker::CheckFreeConditions()
{
    auto workerEngine = reinterpret_cast<NativeEngine*>(workerEnv_);
    // only when all conditions are met can the worker be freed
    if (HasRunningTasks()) {
        HILOG_DEBUG("taskpool:: async callbacks may exist, the worker thread will not exit");
    } else if (workerEngine->HasListeningCounter()) {
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
    if (ConcurrentHelper::IsUvActive(debuggerOnPostTaskSignal_)) {
        std::lock_guard<std::mutex> lock(debuggerMutex_);
        debuggerQueue_.push(std::move(task));
        uv_async_send(debuggerOnPostTaskSignal_);
    }
}
#endif

#if defined(ENABLE_TASKPOOL_FFRT)
void Worker::InitFfrtInfo()
{
    if (TaskManager::GetInstance().EnableFfrt()) {
        static const std::map<int, Priority> FFRTQOS_WORKERPRIORITY_MAP = {
            {ffrt::qos_background, Priority::IDLE},
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
    ConcurrentHelper::UvHandleInit(loop, worker->triggerGCCheckSignal_, Worker::TriggerGCCheck, worker);

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
    workerEngine->SetDebuggerPostTaskFunc([this](std::function<void()>&& task) {
        this->DebuggerOnPostTask(std::move(task));
    });
#endif
    reinterpret_cast<NativeEngine*>(workerEnv_)->RegisterNapiUncaughtExceptionHandler(
        [workerEngine] (napi_value exception) -> void {
        if (!NativeEngine::IsAlive(workerEngine)) {
            HILOG_WARN("napi_env has been destoryed!");
            return;
        }
        std::string name = "";
        void* data = workerEngine->GetCurrentTaskInfo();
        if (data != nullptr) {
            Task* task = static_cast<Task*>(data);
            name = task->name_;
        }
        NapiErrorManager::GetInstance()->NotifyUncaughtException(reinterpret_cast<napi_env>(workerEngine),
            exception, name, TASKPOOL_TYPE);
    });
    reinterpret_cast<NativeEngine*>(workerEnv_)->RegisterAllPromiseCallback(
        [workerEngine] (napi_value* args) -> void {
        if (!NativeEngine::IsAlive(workerEngine)) {
            HILOG_WARN("napi_env has been destoryed!");
            return;
        }
        std::string name = "";
        void* data = workerEngine->GetCurrentTaskInfo();
        if (data != nullptr) {
            Task* task = static_cast<Task*>(data);
            name = task->name_;
        }
        NapiErrorManager::GetInstance()->NotifyUnhandledRejection(reinterpret_cast<napi_env>(workerEngine),
            args, name, TASKPOOL_TYPE);
    });

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
    if (LIKELY(performTaskSignal_ != nullptr && !uv_is_closing(reinterpret_cast<uv_handle_t*>(performTaskSignal_)))) {
        int ret = uv_async_send(performTaskSignal_);
        if (ret != 0) {
            HILOG_ERROR("taskpool:: worker NotifyExecuteTask uv send failed");
            TaskManager::GetInstance().UvReportHisysEvent(this, "NotifyExecuteTask", "uv_async_send",
                "uv send performTaskSignal_ failed", ret);
        }
    } else {
        HILOG_ERROR("taskpool:: performTaskSignal_ is invalid");
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

void Worker::NotifyTaskBegin()
{
    auto workerEngine = reinterpret_cast<NativeEngine*>(workerEnv_);
    workerEngine->NotifyTaskBegin();
}

void Worker::TriggerGCCheck(const uv_async_t* req)
{
    if (req == nullptr || req->data == nullptr) {
        HILOG_ERROR("taskpool:: req handle is invalid");
        return;
    }
    auto worker = reinterpret_cast<Worker*>(req->data);
    auto workerEngine = reinterpret_cast<NativeEngine*>(worker->workerEnv_);
    workerEngine->NotifyTaskFinished();
}

void Worker::NotifyTaskFinished()
{
    // trigger gc check by uv and return immediately if the handle is invalid
    if (UNLIKELY(!ConcurrentHelper::IsUvActive(triggerGCCheckSignal_))) {
        HILOG_ERROR("taskpool:: triggerGCCheckSignal_ is nullptr or closed");
        return;
    } else {
        uv_async_send(triggerGCCheckSignal_);
    }

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
    auto worker = static_cast<Worker*>(req->data);
    auto taskInfo = TaskManager::GetInstance().DequeueTaskId();
    if (taskInfo.first == 0) {
        if (TaskManager::GetInstance().GetTotalTaskNum() != 0) {
            worker->NotifyExecuteTask();
        }
        return;
    }
    uint64_t startTime = ConcurrentHelper::GetMilliseconds();
    worker->UpdateWorkerWakeUpTime();
    napi_env env = worker->workerEnv_;
    TaskManager::GetInstance().NotifyWorkerRunning(worker);
    RunningScope runningScope(worker);
    WorkerRunningScope workerRunningScope(env);
    PriorityScope priorityScope(worker, taskInfo.second);
    Task* task = TaskManager::GetInstance().GetTaskForPerform(taskInfo.first);
    if (task == nullptr) {
        HILOG_DEBUG("taskpool:: task has been released");
        return;
    } else if (!task->IsValid() && task->ShouldDeleteTask(false)) {
        HILOG_WARN("taskpool:: task is invalid");
        delete task;
        return;
    }
    // try to record the memory data for gc
    worker->NotifyTaskBegin();

    if (!task->UpdateTask(startTime, worker)) {
        worker->NotifyTaskFinished();
        return;
    }
    if (task->IsGroupTask()) {
        TaskGroupManager::GetInstance().UpdateGroupState(task->groupId_);
    }
    if (task->IsLongTask()) {
        worker->UpdateLongTaskInfo(task);
    }
    worker->StoreTaskId(task->taskId_);
    // tag for trace parse: Task Perform
    auto loop = worker->GetWorkerLoop();
    uint64_t loopAddress = reinterpret_cast<uint64_t>(loop);
    std::string strTrace = "Task Perform: name : "  + task->name_ + ", taskId : " + std::to_string(task->taskId_)
                            + ", priority : " + std::to_string(taskInfo.second);
    std::string taskLog = "run id:" + std::to_string(task->taskId_) + ",name:" + task->name_ + ","
                          "loop:" + std::to_string(loopAddress);
    HITRACE_HELPER_METER_NAME(strTrace);
    HILOG_TASK_INFO("taskpool:%{public}s", taskLog.c_str());

    napi_value func = nullptr;
    napi_value args = nullptr;
    napi_value errorInfo = task->DeserializeValue(env, &func, &args);
    if (UNLIKELY(func == nullptr || args == nullptr)) {
        if (errorInfo != nullptr) {
            worker->NotifyTaskResult(env, task, errorInfo);
        }
        return;
    }
    auto workerEngine = reinterpret_cast<NativeEngine*>(env);
    if (!worker->InitTaskPoolFunc(env, func, task)) {
        workerEngine->ClearCurrentTaskInfo();
        return;
    }
    worker->hasExecuted_ = true;
    uint32_t argsNum = NapiHelper::GetArrayLength(env, args);
    napi_value argsArray[argsNum];
    for (size_t i = 0; i < argsNum; i++) {
        argsArray[i] = NapiHelper::GetElement(env, args, i);
    }

    if (!task->CheckStartExecution(taskInfo.second)) { // LOCV_EXCL_BR_LINE
        if (task->ShouldDeleteTask()) {
            delete task;
        }
        workerEngine->ClearCurrentTaskInfo();
        return;
    }
    napi_call_function(env, NapiHelper::GetGlobalObject(env), func, argsNum, argsArray, nullptr);
    workerEngine->ClearCurrentTaskInfo();
    task->DecreaseRefCount();
    task->StoreTaskDuration();
    worker->UpdateExecutedInfo();
    HandleFunctionResult(env, task);
}

void Worker::NotifyTaskResult(napi_env env, Task* task, napi_value result)
{
    HITRACE_HELPER_METER_NAME(__PRETTY_FUNCTION__);
    HILOG_DEBUG("taskpool:: NotifyTaskResult task:%{public}s", std::to_string(task->taskId_).c_str());
    void* resultData = nullptr;
    napi_value undefined = NapiHelper::GetUndefinedValue(env);
    bool defaultTransfer = true;
    bool defaultCloneSendable = false;
    std::string errString = "";
    napi_status status = napi_serialize_inner_with_error(env, result, undefined, undefined, defaultTransfer,
                                                         defaultCloneSendable, &resultData, errString);
    if ((status != napi_ok || resultData == nullptr) && task->success_) {
        task->success_ = false;
        std::string errMessage = "taskpool: failed to serialize result.\nSerialize error: " + errString;
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
    Worker* worker = reinterpret_cast<Worker*>(task->GetWorker());
    if (worker == nullptr) {
        TaskManager::GetInstance().UvReportHisysEvent(nullptr, "NotifyHandleTaskResult", "", "worker is nullptr", -1);
        HILOG_FATAL("taskpool:: worker is nullptr");
        return;
    }
    worker->EraseRunningTaskId(task->GetTaskId());
    auto priority = worker->GetPriority();
    if (!Task::VerifyAndPostResult(task, priority)) {
        if (task->ShouldDeleteTask()) {
            delete task;
        } else if (task->IsSeqRunnerTask()) {
            SequenceRunnerManager::GetInstance().TriggerSeqRunner(task->GetEnv(), task);
        }
    }
    worker->NotifyTaskFinished();
}

void Worker::TaskResultCallback(napi_env env, napi_value result, bool success, void* data)
{
    HITRACE_HELPER_METER_NAME(__PRETTY_FUNCTION__);
    if (env == nullptr) { // LCOV_EXCL_BR_LINE
        std::string error = "TaskResultCallback engine is null";
        TaskManager::GetInstance().UvReportHisysEvent(nullptr, "TaskResultCallback", "", error, -1);
        HILOG_FATAL("taskpool:: %{public}s", error.c_str());
        return;
    }
    if (data == nullptr) { // LCOV_EXCL_BR_LINE
        std::string error = "data is nullptr";
        TaskManager::GetInstance().UvReportHisysEvent(nullptr, "TaskResultCallback", "", error, -1);
        HILOG_FATAL("taskpool:: %{public}s", error.c_str());
        return;
    }
    Task* task = static_cast<Task*>(data);
    if (TaskManager::GetInstance().GetTask(task->taskId_) == nullptr) {
        std::string error = "task is nullptr, taskId: " + std::to_string(task->taskId_);
        TaskManager::GetInstance().UvReportHisysEvent(nullptr, "TaskResultCallback", "", error, -1);
        HILOG_FATAL("taskpool:: task is nullptr");
        return;
    }
    auto worker = static_cast<Worker*>(task->worker_);
    worker->isExecutingLongTask_ = task->IsLongTask();
    task->DecreaseRefCount();
    task->ioTime_ = ConcurrentHelper::GetMilliseconds();
    if (task->cpuTime_ != 0) {
        uint64_t ioDuration = task->ioTime_ - task->startTime_;
        uint64_t cpuDuration = task->cpuTime_ - task->startTime_;
        TaskManager::GetInstance().StoreTaskDuration(task->taskId_, std::max(ioDuration, cpuDuration), cpuDuration);
    }
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    if (exception != nullptr) {
        HILOG_ERROR("taskpool::TaskResultCallback occur exception");
        reinterpret_cast<NativeEngine*>(env)->HandleTaskpoolException(exception);
        task->success_ = false;
        napi_value errorEvent = ErrorHelper::TranslateErrorEvent(env, exception);
        NotifyTaskResult(env, task, errorEvent);
        return;
    }

    task->success_ = success;
    NotifyTaskResult(env, task, result);
}

// reset qos_user_initiated after perform task
void Worker::ResetWorkerPriority()
{
    if (priority_ != Priority::HIGH) {
        if (TaskManager::GetInstance().EnableFfrt()) {
#if defined(ENABLE_TASKPOOL_FFRT)
            if (ffrt::this_task::update_qos(WORKERPRIORITY_FFRTQOS_MAP.at(Priority::HIGH)) != 0) {
                SetWorkerPriority(Priority::HIGH);
            }
            priority_ = Priority::HIGH;
#endif
        }
    }
}

void Worker::StoreTaskId(uint32_t taskId)
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
void Worker::TerminateTask(uint32_t taskId)
{
    HILOG_DEBUG("taskpool:: TerminateTask task:%{public}s", std::to_string(taskId).c_str());
    std::lock_guard<std::mutex> lock(longMutex_);
    longTasksSet_.erase(taskId);
    if (longTasksSet_.empty()) {
        hasLongTask_ = false;
    }
}

// to store longTasks' state
void Worker::UpdateLongTaskInfo(Task* task)
{
    HILOG_DEBUG("taskpool:: UpdateLongTaskInfo task:%{public}s", std::to_string(task->taskId_).c_str());
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

void Worker::HandleFunctionResult(napi_env env, Task* task)
{
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    if (exception != nullptr) {
        HILOG_ERROR("taskpool::PerformTask occur exception");
        reinterpret_cast<NativeEngine*>(env)->HandleTaskpoolException(exception);
        task->DecreaseRefCount();
        task->success_ = false;
        napi_value errorEvent = ErrorHelper::TranslateErrorEvent(env, exception);
        NotifyTaskResult(env, task, errorEvent);
        return;
    }
    NotifyHandleTaskResult(task);
}

void Worker::PostReleaseSignal()
{
    if (UNLIKELY(!ConcurrentHelper::IsUvActive(clearWorkerSignal_))) {
        HILOG_ERROR("taskpool:: clearWorkerSignal_ is nullptr or closed");
        return;
    }
    uv_async_send(clearWorkerSignal_);
}

bool Worker::IsRunnable(uint64_t currTime) const
{
    bool res = true;
    if (currTime > wakeUpTime_) {
        res = (currTime - wakeUpTime_ < 15); // 15: ms
    }
    return res;
}

void Worker::UpdateWorkerWakeUpTime()
{
    wakeUpTime_ = ConcurrentHelper::GetMilliseconds();
}

Priority Worker::GetPriority() const
{
    return priority_;
}

void Worker::EraseRunningTaskId(uint32_t taskId)
{
    std::lock_guard<std::mutex> lock(currentTaskIdMutex_);
    auto iter = std::find(currentTaskId_.begin(), currentTaskId_.end(), taskId);
    if (iter != currentTaskId_.end()) {
        currentTaskId_.erase(iter);
    }
}

#if defined(ENABLE_TASKPOOL_HISYSEVENT)
bool Worker::IsNeedReport(uint64_t intervalTime)
{
    if (reportCount_ >= MAX_REPORT_TIMES) {
        return false;
    }
    if (intervalTime < static_cast<uint64_t>(reportCount_.load() + 1) * WORKER_ALIVE_TIME) {
        return false;
    }
    return true;
}

void Worker::IncreaseReportCount()
{
    reportCount_++;
}
#endif

void Worker::ResetPerformIdleState()
{
    if (priority_ == Priority::IDLE) {
        TaskManager::GetInstance().SetIsPerformIdle(false);
    }
}
} // namespace Commonlibrary::Concurrent::TaskPoolModule
