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

#include "commonlibrary/ets_utils/js_sys_module/timer/timer.h"
#include "helper/hitrace_helper.h"
#include "process_helper.h"
#include "task_group.h"
#include "task_manager.h"
#include "utils/log.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
using namespace OHOS::JsSysModule;
using namespace Commonlibrary::Platform;

Worker::RunningScope::~RunningScope()
{
    if (scope_ != nullptr) {
        napi_close_handle_scope(worker_->workerEnv_, scope_);
    }
    worker_->NotifyIdle();
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
    if (!worker->CheckFreeConditions()) {
        return;
    }

    HITRACE_HELPER_METER_NAME("ReleaseWorkerHandles: [Release Thread]");
    HILOG_INFO("taskpool:: the thread is idle and will be released");
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
    bool res = !Timer::HasTimer(workerEnv_) && !workerEngine->HasWaitingRequest() &&
        !workerEngine->HasSubEnv() && !workerEngine->HasPendingJob() && !workerEngine->IsProfiling();
    // if conditions are not met, we will try to free the worker during the next check
    if (res) {
        return true;
    }
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
    worker->debuggerTask_();
}

void Worker::DebuggerOnPostTask(std::function<void()>&& task)
{
    if (uv_is_active(reinterpret_cast<uv_handle_t*>(debuggerOnPostTaskSignal_))) {
        debuggerTask_ = std::move(task);
        uv_async_send(debuggerOnPostTaskSignal_);
    }
}
#endif

void Worker::ExecuteInThread(const void* data)
{
    HITRACE_HELPER_START_TRACE(__PRETTY_FUNCTION__);
    auto worker = reinterpret_cast<Worker*>(const_cast<void*>(data));
    {
        napi_create_runtime(worker->hostEnv_, &worker->workerEnv_);
        if (worker->workerEnv_ == nullptr) {
            HILOG_ERROR("taskpool:: workerEnv is nullptr");
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
    worker->performTaskSignal_ = new uv_async_t;
    worker->performTaskSignal_->data = worker;
    uv_async_init(loop, worker->performTaskSignal_, reinterpret_cast<uv_async_cb>(Worker::PerformTask));

    worker->clearWorkerSignal_ = new uv_async_t;
    worker->clearWorkerSignal_->data = worker;
    uv_async_init(loop, worker->clearWorkerSignal_, reinterpret_cast<uv_async_cb>(Worker::ReleaseWorkerHandles));

    HITRACE_HELPER_FINISH_TRACE;
#if !defined(WINDOWS_PLATFORM) && !defined(MAC_PLATFORM)
    // Init debugger task post signal
    worker->debuggerOnPostTaskSignal_ = new uv_async_t;
    worker->debuggerOnPostTaskSignal_->data = worker;
    uv_async_init(loop, worker->debuggerOnPostTaskSignal_, reinterpret_cast<uv_async_cb>(Worker::HandleDebuggerTask));
#endif
    if (worker->PrepareForWorkerInstance()) {
        // Call after uv_async_init
        worker->NotifyWorkerCreated();
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
    } else {
        TaskManager::GetInstance().EnqueueExecuteId(executeInfo_.first, executeInfo_.second);
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
    if (--runningCount_ != 0) {
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
    napi_env env = worker->workerEnv_;
    napi_status status = napi_ok;
    RunningScope runningScope(worker, status);
    auto executeInfo = worker->executeInfo_;
    if (UNLIKELY(status != napi_ok)) {
        GET_AND_THROW_LAST_ERROR((env));
        TaskManager::GetInstance().EnqueueExecuteId(executeInfo.first, executeInfo.second);
        return;
    }

    PriorityScope priorityScope(worker, executeInfo.second);
    TaskInfo* taskInfo = TaskManager::GetInstance().GetTaskInfo(executeInfo.first);
    if (taskInfo == nullptr) { // task may have been canceled
        worker->NotifyTaskFinished();
        HILOG_DEBUG("taskpool::PerformTask taskInfo is null");
        return;
    }
    {
        std::lock_guard<std::mutex> lock(worker->currentTaskIdMutex_);
        worker->currentTaskId_.emplace_back(taskInfo->taskId);
    }
    // tag for trace parse: Task Perform
    std::string strTrace = "Task Perform: functionName : "  + taskInfo->funcName + ", taskId : " +
                           std::to_string(taskInfo->taskId) + ", executeId : " + std::to_string(taskInfo->executeId);
    HITRACE_HELPER_METER_NAME(strTrace);

    taskInfo->worker = worker;
    TaskManager::GetInstance().UpdateExecuteState(taskInfo->executeId, ExecuteState::RUNNING);
    napi_value func;
    status = napi_deserialize(env, taskInfo->serializationFunction, &func);
    if (status != napi_ok || func == nullptr) {
        HILOG_ERROR("taskpool:: PerformTask deserialize function fail");
        napi_value err = ErrorHelper::NewError(env, ErrorHelper::ERR_WORKER_SERIALIZATION,
                                               "taskpool: failed to deserialize function.");
        taskInfo->success = false;
        NotifyTaskResult(env, taskInfo, err);
        return;
    }
    napi_value args;
    status = napi_deserialize(env, taskInfo->serializationArguments, &args);
    if (status != napi_ok || args == nullptr) {
        HILOG_ERROR("taskpool:: PerformTask deserialize arguments fail");
        napi_value err = ErrorHelper::NewError(env, ErrorHelper::ERR_WORKER_SERIALIZATION,
                                               "taskpool: failed to deserialize arguments.");
        taskInfo->success = false;
        NotifyTaskResult(env, taskInfo, err);
        return;
    }

    auto workerEngine = reinterpret_cast<NativeEngine*>(env);
    // Store taskinfo in function
    bool success = workerEngine->InitTaskPoolFunc(env, func, taskInfo);
    napi_value exception;
    napi_get_and_clear_last_exception(env, &exception);
    if (exception != nullptr) {
        HILOG_ERROR("taskpool:: InitTaskPoolFunc occur exception");
        taskInfo->success = false;
        napi_value errorEvent = ErrorHelper::TranslateErrorEvent(env, exception);
        NotifyTaskResult(env, taskInfo, errorEvent);
        return;
    }
    if (!success) {
        HILOG_ERROR("taskpool:: InitTaskPoolFunc fail");
        napi_value err = ErrorHelper::NewError(env, ErrorHelper::TYPE_ERROR,
                                               "taskpool: function may not be concurrent.");
        taskInfo->success = false;
        NotifyTaskResult(env, taskInfo, err);
        return;
    }

    uint32_t argsNum = NapiHelper::GetArrayLength(env, args);
    napi_value argsArray[argsNum];
    napi_value val;
    for (size_t i = 0; i < argsNum; i++) {
        napi_get_element(env, args, i, &val);
        argsArray[i] = val;
    }

    napi_value result;
    napi_call_function(env, NapiHelper::GetGlobalObject(env), func, argsNum, argsArray, &result);
    // if the worker is blocked, just skip
    if (LIKELY(worker->state_ != WorkerState::BLOCKED)) {
        uint64_t duration = ConcurrentHelper::GetMilliseconds() - worker->startTime_;
        TaskManager::GetInstance().UpdateExecutedInfo(duration);
    }
    napi_get_and_clear_last_exception(env, &exception);
    if (exception != nullptr) {
        HILOG_ERROR("taskpool::PerformTask occur exception");
        taskInfo->success = false;
        napi_value errorEvent = ErrorHelper::TranslateErrorEvent(env, exception);
        NotifyTaskResult(env, taskInfo, errorEvent);
    }
}

void Worker::NotifyTaskResult(napi_env env, TaskInfo* taskInfo, napi_value result)
{
    HITRACE_HELPER_METER_NAME(__PRETTY_FUNCTION__);
    napi_value resultData;
    napi_status status = napi_serialize(env, result, result, &resultData);
    if ((status != napi_ok || resultData == nullptr) && taskInfo->success) {
        taskInfo->success = false;
        std::string errMessage = "taskpool: failed to serialize result.";
        napi_value err = ErrorHelper::NewError(env, ErrorHelper::ERR_WORKER_SERIALIZATION, errMessage.c_str());
        HILOG_ERROR("%{public}s", errMessage.c_str());
        NotifyTaskResult(env, taskInfo, err);
        return;
    }
    taskInfo->result = resultData;

    TaskManager::GetInstance().RemoveExecuteState(taskInfo->executeId);
    if (taskInfo->groupExecuteId == 0) {
        TaskManager::GetInstance().PopRunningInfo(taskInfo->taskId, taskInfo->executeId);
    }
    TaskManager::GetInstance().PopTaskInfo(taskInfo->executeId);
    Worker* worker = reinterpret_cast<Worker*>(taskInfo->worker);
    {
        std::lock_guard<std::mutex> lock(worker->currentTaskIdMutex_);
        worker->currentTaskId_.erase(std::find(worker->currentTaskId_.begin(),
                                     worker->currentTaskId_.end(), taskInfo->taskId));
    }
    uv_async_send(taskInfo->onResultSignal);
    worker->NotifyTaskFinished();
}

void Worker::TaskResultCallback(napi_env env, napi_value result, bool success, void* data)
{
    HITRACE_HELPER_METER_NAME(__PRETTY_FUNCTION__);
    if (env == nullptr) {
        HILOG_FATAL("taskpool::TaskResultCallback engine is null");
        return;
    }
    if (data == nullptr) {
        HILOG_FATAL("taskpool:: taskInfo is nullptr");
        return;
    }
    TaskInfo* taskInfo = static_cast<TaskInfo*>(data);
    taskInfo->success = success;
    NotifyTaskResult(env, taskInfo, result);
}

// reset qos_user_initiated after perform task
void Worker::ResetWorkerPriority()
{
    if (priority_ != Priority::HIGH) {
        SetWorkerPriority(Priority::HIGH);
        priority_ = Priority::HIGH;
    }
}
} // namespace Commonlibrary::Concurrent::TaskPoolModule
