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

#include "utils/log.h"

namespace Commonlibrary::TaskPoolModule {
const static int MAX_THREADPOOL_SIZE = 2;
static std::list<Worker*> g_workers;
static TaskQueue g_taskQueue;
static std::mutex g_workersMutex;

Worker::Worker(napi_env env) : hostEnv_(env) {}

bool Worker::NeedExpandWorker()
{
    std::unique_lock<std::mutex> lock(g_workersMutex);
    if (g_workers.size() >= MAX_THREADPOOL_SIZE) {
        HILOG_DEBUG("taskpool:: Worker thread num reaches the maximum");
        return false;
    }
    return true;
}

void Worker::WorkerConstructor(napi_env env)
{
    Worker* worker = nullptr;
    {
        std::unique_lock<std::mutex> lock(g_workersMutex);
        worker = new (std::nothrow) Worker(env);
        g_workers.push_back(worker);
    }
    worker->StartExecuteInThread(env);
}

void Worker::WorkerDestructor()
{
    g_taskQueue.Terminate();
    TaskManager::ClearTaskInfo();
    std::lock_guard<std::mutex> lock(g_workersMutex);
    for (auto &item : g_workers) {
        item->TerminateWorker();
    }
}

void Worker::StartExecuteInThread(napi_env env)
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
    napi_env workerEnv = nullptr;
    {
        std::unique_lock<std::recursive_mutex> lock(worker->liveEnvLock_);
        napi_env env = worker->hostEnv_;
        napi_create_runtime(env, &workerEnv);
        if (workerEnv == nullptr) {
            HILOG_ERROR("taskpool:: workerEnv is nullptr");
            return;
        }
        reinterpret_cast<NativeEngine*>(workerEnv)->MarkSubThread();
        worker->workerEnv_ = workerEnv;
    }
    uv_loop_t* loop = worker->GetWorkerLoop();
    if (loop == nullptr) {
        HILOG_ERROR("taskpool:: loop is nullptr");
        return;
    }

    if (worker->PrepareForWorkerInstance()) {
        worker->performTaskSignal_ = new uv_async_t;
        worker->performTaskSignal_->data = worker;
        uv_async_init(loop, worker->performTaskSignal_, reinterpret_cast<uv_async_cb>(Worker::PerformTask));
        uv_async_send(worker->performTaskSignal_);
        worker->Loop();
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
    hostEngine->CallWorkerAsyncWorkFunc(workerEngine);
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

    // 1. remove worker instance count
    {
        std::lock_guard<std::mutex> lock(g_workersMutex);
        std::list<Worker*>::iterator it = std::find(g_workers.begin(), g_workers.end(), this);
        if (it != g_workers.end()) {
            g_workers.erase(it);
        }
    }

    // 2. delete NativeEngine created in worker thread
    workerEngine->CloseAsyncWork();
    workerEngine->DeleteEngine();
    delete workerEngine;
    workerEnv_ = nullptr;
}

void Worker::TerminateWorker()
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

void Worker::EnqueueTask(std::unique_ptr<Task> task)
{
    g_taskQueue.EnqueueTask(std::move(task));
}

void Worker::HandleTaskResult(const uv_async_t* req)
{
    TaskInfo* taskInfo = static_cast<TaskInfo*>(req->data);
    napi_value taskData;
    napi_status status = napi_deserialize(taskInfo->env, taskInfo->result, &taskData);
    if (status != napi_ok || taskData == nullptr) {
        napi_reject_deferred(taskInfo->env, taskInfo->deferred, nullptr);
    } else {
        napi_resolve_deferred(taskInfo->env, taskInfo->deferred, taskData);
    }
    TaskManager::ReleaseTaskContent(taskInfo);
}

void Worker::PerformTask(const uv_async_t* req)
{
    Worker* worker = static_cast<Worker*>(req->data);
    while (std::unique_ptr<Task> task = g_taskQueue.DequeueTask()) {
        napi_env env = worker->workerEnv_;
        TaskInfo* taskInfo = TaskManager::PopTaskInfo(task->executeId_);
        if (taskInfo == nullptr) { // task may have been canceled
            continue;
        }
        TaskManager::UpdateState(taskInfo->executeId, TaskState::RUNNING);
        napi_value undefined;
        napi_get_undefined(env, &undefined);
        napi_value taskData;
        napi_deserialize(env, taskInfo->serializationData, &taskData);
        napi_value func;
        napi_get_named_property(env, taskData, "func", &func);
        auto funcVal = reinterpret_cast<NativeValue*>(func);
        auto workerEngine = reinterpret_cast<NativeEngine*>(env);
        workerEngine->CallInitTaskFunc(workerEngine, funcVal);
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
        napi_value result;
        napi_call_function(env, undefined, func, argsNum, argsArray, &result);
        napi_value resultData;
        napi_serialize(env, result, undefined, &resultData);
        taskInfo->result = resultData;
        TaskManager::UpdateState(taskInfo->executeId, TaskState::TERMINATED);
        TaskManager::PopRunningInfo(task->taskId_, task->executeId_);
        uv_async_send(taskInfo->taskSignal);
        g_taskQueue.NotifyWorkerThread();
    }
}
} // namespace Commonlibrary::TaskPoolModule