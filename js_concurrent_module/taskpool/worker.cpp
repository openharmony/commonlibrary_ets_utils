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

#include "hitrace_meter.h"
#include "plugin/timer.h"
#include "task_manager.h"
#include "utils/log.h"

namespace Commonlibrary::ConcurrentModule {
Worker::Worker(napi_env env) : hostEnv_(env) {}

Worker* Worker::WorkerConstructor(napi_env env)
{
    HITRACE_METER_NAME(HITRACE_TAG_COMMONLIBRARY, __PRETTY_FUNCTION__);
    Worker* worker = new Worker(env);
    worker->StartExecuteInThread();
    return worker;
}

Worker::~Worker()
{
    // when there is no active handle, worker loop will stop automatic.
    uv_close(reinterpret_cast<uv_handle_t*>(performTaskSignal_), [](uv_handle_t* handle) {
        if (handle != nullptr) {
            delete reinterpret_cast<uv_async_t*>(handle);
            handle = nullptr;
        }
    });
    uv_loop_t* loop = GetWorkerLoop();
    if (loop != nullptr) {
        uv_stop(loop);
    }
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

void Worker::ExecuteInThread(const void* data)
{
    StartTrace(HITRACE_TAG_COMMONLIBRARY, __PRETTY_FUNCTION__);
    auto worker = reinterpret_cast<Worker*>(const_cast<void*>(data));
    {
        napi_create_runtime(worker->hostEnv_, &worker->workerEnv_);
        if (worker->workerEnv_ == nullptr) {
            HILOG_ERROR("taskpool:: workerEnv is nullptr");
            return;
        }
        auto workerEngine = reinterpret_cast<NativeEngine*>(worker->workerEnv_);
        workerEngine->MarkSubThread();
        workerEngine->InitTaskPoolThread(workerEngine, Worker::TaskResultCallback);
    }
    uv_loop_t* loop = worker->GetWorkerLoop();
    if (loop == nullptr) {
        HILOG_ERROR("taskpool:: loop is nullptr");
        return;
    }

    // Init worker task execute signal
    worker->performTaskSignal_ = new uv_async_t;
    worker->performTaskSignal_->data = worker;
    uv_async_init(loop, worker->performTaskSignal_, reinterpret_cast<uv_async_cb>(Worker::PerformTask));
    FinishTrace(HITRACE_TAG_COMMONLIBRARY);
    if (worker->PrepareForWorkerInstance()) {
        // Call after uv_async_init
        worker->NotifyIdle();
        worker->RunLoop();
    } else {
        HILOG_ERROR("taskpool:: Worker PrepareForWorkerInstance failure");
    }
    worker->ReleaseWorkerThreadContent();
    delete worker;
    worker = nullptr;
}

bool Worker::PrepareForWorkerInstance()
{
    HITRACE_METER_NAME(HITRACE_TAG_COMMONLIBRARY, __PRETTY_FUNCTION__);
    auto workerEngine = reinterpret_cast<NativeEngine*>(workerEnv_);
    auto hostEngine = reinterpret_cast<NativeEngine*>(hostEnv_);
    if (!hostEngine->CallInitWorkerFunc(workerEngine)) {
        HILOG_ERROR("taskpool:: Worker CallInitWorkerFunc failure");
        return false;
    }
    return true;
}

void Worker::ReleaseWorkerThreadContent()
{
    HITRACE_METER_NAME(HITRACE_TAG_COMMONLIBRARY, __PRETTY_FUNCTION__);
    auto workerEngine = reinterpret_cast<NativeEngine*>(workerEnv_);
    if (workerEngine == nullptr) {
        HILOG_ERROR("taskpool:: workerEngine is nullptr");
        return;
    }

    // 2. delete NativeEngine created in worker thread
    workerEngine->DeleteEngine();
    delete workerEngine;
    workerEnv_ = nullptr;
}

void Worker::NotifyExecuteTask()
{
    if (uv_is_active(reinterpret_cast<uv_handle_t*>(performTaskSignal_))) {
        uv_async_send(performTaskSignal_);
    }
}

void Worker::NotifyIdle()
{
    taskInfo_ = nullptr;
    TaskManager::GetInstance().NotifyWorkerIdle(this);
}

void Worker::PerformTask(const uv_async_t* req)
{
    HITRACE_METER_NAME(HITRACE_TAG_COMMONLIBRARY, __PRETTY_FUNCTION__);
    auto worker = static_cast<Worker*>(req->data);
    napi_env env = worker->workerEnv_;
    std::unique_ptr<Task> task = TaskManager::GetInstance().DequeueTask();
    if (task == nullptr) {
        worker->NotifyIdle();
        return;
    }

    TaskInfo* taskInfo = TaskManager::GetInstance().PopTaskInfo(task->executeId_);
    if (taskInfo == nullptr) { // task may have been canceled
        worker->NotifyIdle();
        HILOG_ERROR("taskpool::PerformTask taskInfo is null");
        return;
    }
    worker->taskInfo_ = taskInfo;
    TaskManager::GetInstance().UpdateState(taskInfo->executeId, TaskState::RUNNING);
    napi_value undefined;
    napi_get_undefined(env, &undefined);
    napi_value taskData;
    napi_status status = napi_deserialize(env, taskInfo->serializationData, &taskData);
    if (status != napi_ok || taskData == nullptr) {
        HILOG_ERROR("taskpool::PerformTask napi_deserialize fail");
        napi_value err = Helper::ErrorHelper::NewError(env, Helper::ErrorHelper::WORKERSERIALIZATION_ERROR,
            "taskpool: failed to deserialize message.");
        taskInfo->success = false;
        NotifyTaskResult(env, worker, taskInfo, err);
        return;
    }
    napi_value func;
    napi_get_named_property(env, taskData, "func", &func);
    auto funcVal = reinterpret_cast<NativeValue*>(func);
    auto workerEngine = reinterpret_cast<NativeEngine*>(env);
    workerEngine->InitTaskPoolFunc(workerEngine, funcVal);
    napi_value args;
    napi_get_named_property(env, taskData, "args", &args);
    uint32_t argsNum = 0;
    napi_get_array_length(env, args, &argsNum);
    napi_value argsArray[argsNum];
    napi_value val;
    for (size_t i = 0; i < argsNum; i++) {
        napi_get_element(env, args, i, &val);
        argsArray[i] = val;
    }
    // Store taskinfo in last argument
    napi_value data;
    napi_create_external(env, worker, nullptr, nullptr, &data);

    napi_value result;
    napi_call_function(env, data, func, argsNum, argsArray, &result);

    napi_value exception;
    napi_get_and_clear_last_exception(env, &exception);
    if (exception != nullptr) {
        HILOG_ERROR("taskpool::PerformTask occur exception");
        taskInfo->success = false;
        NotifyTaskResult(env, worker, taskInfo, exception);
    }
}

void Worker::NotifyTaskResult(napi_env env, Worker* worker, TaskInfo *taskInfo, napi_value result)
{
    HITRACE_METER_NAME(HITRACE_TAG_COMMONLIBRARY, __PRETTY_FUNCTION__);
    napi_value undefined;
    napi_get_undefined(env, &undefined);

    napi_value resultData;
    napi_status status = napi_serialize(env, result, undefined, &resultData);
    if ((status != napi_ok || resultData == nullptr) && taskInfo->success) {
        taskInfo->success = false;
        napi_value err = Helper::ErrorHelper::NewError(env, Helper::ErrorHelper::WORKERSERIALIZATION_ERROR,
            "taskpool: failed to serialize result.");
        NotifyTaskResult(env, worker, taskInfo, err);
        return;
    }
    taskInfo->result = resultData;

    TaskManager::GetInstance().UpdateState(taskInfo->executeId, TaskState::TERMINATED);
    TaskManager::GetInstance().PopRunningInfo(taskInfo->taskId, taskInfo->executeId);
    uv_async_send(taskInfo->onResultSignal);
    worker->NotifyIdle();
}

void Worker::TaskResultCallback(NativeEngine* engine, NativeValue* value, NativeValue* data)
{
    HITRACE_METER_NAME(HITRACE_TAG_COMMONLIBRARY, __PRETTY_FUNCTION__);
    if (engine == nullptr) {
        HILOG_FATAL("taskpool::TaskResultCallback engine is null");
        return;
    }

    auto env = reinterpret_cast<napi_env>(engine);
    auto result = reinterpret_cast<napi_value>(value);

    napi_valuetype type;
    napi_typeof(env, reinterpret_cast<napi_value>(data), &type);
    if (type != napi_external) {
        HILOG_INFO("taskpool::TaskResultCallback Concurrent func not called by taskpool.execute");
        return;
    }

    Worker* worker = nullptr;
    napi_get_value_external(env, reinterpret_cast<napi_value>(data), reinterpret_cast<void**>(&worker));
    if (worker == nullptr) {
        HILOG_FATAL("taskpool::TaskResultCallback worker is null");
        return;
    }
    TaskInfo* taskInfo = worker->taskInfo_;
    if (taskInfo == nullptr) {
        HILOG_FATAL("taskpool::TaskResultCallback taskInfo is null");
        return;
    }

    NotifyTaskResult(env, worker, taskInfo, result);
}
} // namespace Commonlibrary::ConcurrentModule