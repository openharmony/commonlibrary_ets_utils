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

#include "plugin/timer.h"
#include "task_manager.h"
#include "utils/log.h"

namespace Commonlibrary::ConcurrentModule {
Worker::Worker(napi_env env) : hostEnv_(env) {}

Worker* Worker::WorkerConstructor(napi_env env)
{
    Worker* worker = new Worker(env);
    worker->StartExecuteInThread();
    return worker;
}

Worker::~Worker()
{
    // when there is no active handle, worker loop will stop automatic.
    uv_close(reinterpret_cast<uv_handle_t*>(&performTaskSignal_), [](uv_handle_t* handle) {
        if (handle != nullptr) {
            delete reinterpret_cast<uv_async_t*>(handle);
            handle = nullptr;
        }
    });
    uv_close(reinterpret_cast<uv_handle_t*>(&notifyResultSignal_), [](uv_handle_t* handle) {
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
    auto worker = reinterpret_cast<Worker*>(const_cast<void*>(data));
    {
        std::unique_lock<std::recursive_mutex> lock(worker->liveEnvLock_);
        napi_env env = worker->hostEnv_;
        napi_create_runtime(env, &worker->workerEnv_);
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

    if (worker->PrepareForWorkerInstance()) {
        // Init host task callback signal
        uv_loop_t* hostLoop = worker->GetHostLoop();
        uv_async_init(hostLoop, &worker->notifyResultSignal_, reinterpret_cast<uv_async_cb>(Worker::HandleTaskResult));

        // Init worker task execute signal
        uv_async_init(loop, &worker->performTaskSignal_, reinterpret_cast<uv_async_cb>(Worker::PerformTask));
        TaskManager::GetInstance().NotifyWorkerIdle(worker);  // Call after uv_async_init
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
    std::unique_lock<std::recursive_mutex> lock(liveEnvLock_);
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
    uv_async_send(&performTaskSignal_);
}

void Worker::HandleTaskResult(const uv_async_t* req)
{
    Worker* worker = Helper::DereferenceHelp::DereferenceOf(&Worker::notifyResultSignal_, req);
    TaskInfo* taskInfo = worker->taskInfo_;
    napi_value taskData;
    napi_status status = napi_deserialize(worker->hostEnv_, taskInfo->result, &taskData);
    if (status != napi_ok || taskData == nullptr) {
        napi_reject_deferred(worker->hostEnv_, taskInfo->deferred, nullptr);
    } else {
        napi_resolve_deferred(worker->hostEnv_, taskInfo->deferred, taskData);
    }
    worker->taskInfo_ = nullptr;
    TaskManager::GetInstance().ReleaseTaskContent(taskInfo);
}

void Worker::PerformTask(const uv_async_t* req)
{
    Worker* worker = Helper::DereferenceHelp::DereferenceOf(&Worker::performTaskSignal_, req);
    if (worker == nullptr) {
        HILOG_FATAL("taskpool::worker is null");
        return;
    }
    napi_env env = worker->workerEnv_;
    std::unique_ptr<Task> task = TaskManager::GetInstance().DequeueTask();
    if (task == nullptr) {
        TaskManager::GetInstance().NotifyWorkerIdle(worker);
        return;
    }

    TaskInfo* taskInfo = TaskManager::GetInstance().PopTaskInfo(task->executeId_);
    if (taskInfo == nullptr) { // task may have been canceled
        TaskManager::GetInstance().NotifyWorkerIdle(worker);
        HILOG_ERROR("taskpool::taskInfo is null");
        return;
    }
    worker->taskInfo_ = taskInfo;
    TaskManager::GetInstance().UpdateState(taskInfo->executeId, TaskState::RUNNING);
    napi_value undefined;
    napi_get_undefined(env, &undefined);
    napi_value taskData;
    napi_deserialize(env, taskInfo->serializationData, &taskData);
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
}

void Worker::TaskResultCallback(NativeEngine* engine, NativeValue* value, NativeValue* data)
{
    if (engine == nullptr) {
        HILOG_FATAL("taskpool:: engine is null");
        return;
    }

    auto env = reinterpret_cast<napi_env>(engine);
    auto result = reinterpret_cast<napi_value>(value);

    napi_valuetype type;
    napi_typeof(env, reinterpret_cast<napi_value>(data), &type);
    if (type != napi_external) {
        HILOG_INFO("taskpool:: Concurrent func not called by taskpool.execute");
        return;
    }

    Worker* worker = nullptr;
    napi_get_value_external(env, reinterpret_cast<napi_value>(data), reinterpret_cast<void**>(&worker));
    if (worker == nullptr) {
        HILOG_FATAL("taskpool:: worker is null");
        return;
    }
    TaskInfo* taskInfo = worker->taskInfo_;

    napi_value undefined;
    napi_get_undefined(env, &undefined);

    napi_value resultData;
    napi_status status = napi_serialize(env, result, undefined, &resultData);
    if (status != napi_ok || resultData == nullptr) {
        Helper::ErrorHelper::ThrowError(env, Helper::ErrorHelper::WORKERSERIALIZATION_ERROR,
            "taskpool: failed to serialize message.");
        return;
    }
    taskInfo->result = resultData;
    TaskManager::GetInstance().UpdateState(taskInfo->executeId, TaskState::TERMINATED);
    TaskManager::GetInstance().PopRunningInfo(taskInfo->taskId, taskInfo->executeId);
    uv_async_send(&worker->notifyResultSignal_);
    TaskManager::GetInstance().NotifyWorkerIdle(worker);
}
} // namespace Commonlibrary::ConcurrentModule