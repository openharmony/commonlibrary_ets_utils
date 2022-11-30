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

#include <unistd.h>
#include <unordered_map>

#include "utils/log.h"

namespace Commonlibrary::TaskPoolModule {
const static int MAX_THREADPOOL_SIZE = 4;
static std::unordered_map<int32_t, WorkerEnv> g_idleEnvs;
static std::queue<std::unique_ptr<Task>> g_hostQueue;
static std::unordered_map<int32_t, WorkerEnv> g_liveEnvs;
static std::unordered_map<WorkerEnv, Worker*> g_workerHostEnvMap;
static TaskQueue g_taskQueue;
static std::mutex g_workersMutex;

Worker::Worker(napi_env env) : hostEnv_(env) {}

void Worker::EnqueueTask(std::unique_ptr<Task> task)
{
    g_taskQueue.EnqueueTask(std::move(task));
}

bool Worker::NeedInitWorker()
{
    std::unique_lock<std::mutex> lock(g_workersMutex);
    if (g_liveEnvs.size() > 0) {
        return false;
    }
    return true;
}

bool Worker::HasIdleEnv()
{
    std::unique_lock<std::mutex> lock(g_workersMutex);
    if (g_idleEnvs.size() > 0) {
        return true;
    }
    return false;
}

bool Worker::NeedExpandWorker()
{
    std::unique_lock<std::mutex> lock(g_workersMutex);
    if (g_liveEnvs.size() >= MAX_THREADPOOL_SIZE) {
        HILOG_ERROR("taskpool:: work Thread Num reaches the maximum");
        return false;
    }
    return true;
}

napi_value Worker::WorkerConstructor(napi_env env)
{
    Worker *worker = nullptr;
    {
        std::unique_lock<std::mutex> lock(g_workersMutex);
        if (g_liveEnvs.size() >= MAX_THREADPOOL_SIZE) {
            napi_throw_error(env, nullptr, "taskpool:: Too Many worker thread");
            return nullptr;
        }
        worker = new Worker(env);
        g_workerHostEnvMap.emplace(env, worker);
        if (worker == nullptr) {
            napi_throw_error(env, nullptr, "taskpool:: create worker error");
            return nullptr;
        }
    }
    worker->StartExecuteInThread(env);
    return nullptr;
}

void Worker::HostOnMessage(const uv_async_t* req)
{
    Worker* worker = static_cast<Worker*>(req->data);
    while (!g_hostQueue.empty()) {
        std::unique_ptr<Task> task = std::move(g_hostQueue.front());
        g_hostQueue.pop();
        napi_value taskData;
        napi_deserialize(worker->hostEnv_, task->resultData_, &taskData);
        if (taskData == nullptr) {
            napi_reject_deferred(worker->hostEnv_, task->deferred_, nullptr);
        } else {
            napi_resolve_deferred(worker->hostEnv_, task->deferred_, taskData);
        }
    }
}

void Worker::StartExecuteInThread(napi_env env)
{
    hostOnMessageSignal_ = new uv_async_t;
    uv_loop_t* loop = GetHostLoop();
    uv_async_init(loop, hostOnMessageSignal_, reinterpret_cast<uv_async_cb>(Worker::HostOnMessage));
    hostOnMessageSignal_->data = this;
    if (!runner_) {
        runner_ = std::make_unique<TaskRunner>(TaskStartCallback(ExecuteInThread, this));
    }
    if (runner_) {
        runner_->Execute(); // start a new thread
    } else {
        HILOG_ERROR("taskpool:: runner_ is nullptr");
    }
}

void Worker::ExecuteInThread(const void *data)
{
    auto worker = reinterpret_cast<Worker *>(const_cast<void *>(data));
    napi_env workerEnv = nullptr;
    {
        std::unique_lock<std::recursive_mutex> lock(worker->liveEnvLock_);
        napi_env env = worker->hostEnv_;
        napi_create_runtime(env, &workerEnv);
        if (workerEnv == nullptr) {
            napi_throw_error(env, nullptr, "taskpool:: Worker create runtime error");
            return;
        }
        int32_t tid = gettid();
        g_liveEnvs[tid] = workerEnv;
        g_idleEnvs[tid] = workerEnv;
        reinterpret_cast<NativeEngine*>(workerEnv)->MarkSubThread();
        worker->workerEnv_ = workerEnv;
    }

    uv_loop_t* loop = worker->GetWorkerLoop();
    if (loop == nullptr) {
        HILOG_ERROR("taskpool:: Worker loop is nullptr");
        return;
    }

    if (worker->PrepareForWorkerInstance()) {
        worker->performTaskSignal_ = new uv_async_t;
        worker->performTaskSignal_->data = worker;
        uv_async_init(loop, worker->performTaskSignal_, reinterpret_cast<uv_async_cb>(Worker::PerformTask));
        uv_async_send(worker->performTaskSignal_);
// #if !defined(WINDOWS_PLATFORM) && !defined(MAC_PLATFORM)
//         uv_async_init(loop, &worker->debuggerOnPostTaskSignal_, reinterpret_cast<uv_async_cb>(
//             Worker::HandleDebuggerTask));
// #endif
        worker->Loop();
    } else {
        HILOG_ERROR("taskpool:: worker PrepareForWorkerInstance failure");
    }
}

bool Worker::PrepareForWorkerInstance()
{
    std::unique_lock<std::recursive_mutex> lock(liveEnvLock_);

    auto workerEngine = reinterpret_cast<NativeEngine*>(workerEnv_);
    auto hostEngine = reinterpret_cast<NativeEngine*>(hostEnv_);
    hostEngine->CallWorkerAsyncWorkFunc(workerEngine);
// #if !defined(WINDOWS_PLATFORM) && !defined(MAC_PLATFORM)
//     workerEngine->SetDebuggerPostTaskFunc(
//         std::bind(&Worker::DebuggerOnPostTask, this, std::placeholders::_1));
// #endif
    if (!hostEngine->CallInitWorkerFunc(workerEngine)) {
        HILOG_ERROR("taskpool:: worker init func failure");
        return false;
    }
    return true;
}

void Worker::PerformTask(const uv_async_t* req)
{
    while (std::unique_ptr<Task> task = g_taskQueue.DequeueTask()) {
        napi_env taskHostEnv = task->hostEnv_;
        if (HasIdleEnv()) {
            napi_env env = nullptr;
            int32_t tid = gettid();
            env = g_idleEnvs[tid];
            napi_value taskData;
            napi_deserialize(env, task->taskData_, &taskData);
            napi_value func;
            napi_value args;
            napi_get_named_property(env, taskData, "func", &func);
            napi_get_named_property(env, taskData, "args", &args);
            napi_valuetype type;
            napi_typeof(env, func, &type);
            napi_value undefined;
            napi_get_undefined(env, &undefined);
            napi_value result;
            napi_call_function(env, undefined, func, 1, &args, &result);
            napi_value resultData;
            napi_serialize(env, result, undefined, &resultData);
            task->resultData_ = resultData;
            g_workersMutex.lock();
            g_hostQueue.push(std::move(task));
            g_workersMutex.unlock();
            auto worker = g_workerHostEnvMap[taskHostEnv];
            uv_async_send(worker->hostOnMessageSignal_);
        }
    }
}
} // namespace Commonlibrary::TaskPoolModule