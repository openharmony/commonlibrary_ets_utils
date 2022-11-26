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
const static int MAX_THREADPOOL_SIZE = 4;
static std::list<WorkerEnv> liveEnvs;
static std::list<WorkerEnv> idleEnvs;
static std::mutex g_workersMutex;

Worker::Worker(napi_env env) : hostEnv_(env) {}

bool Worker::NeedInitWorker()
{
    if (liveEnvs.size() > 0) {
        return false;
    }
    return true;
}

napi_value Worker::WorkerConstructor(napi_env env)
{
    Worker *worker = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_workersMutex);
        if (liveEnvs.size() >= MAX_THREADPOOL_SIZE) {
            napi_throw_error(env, nullptr, "Too Many worker thread");
            return nullptr;
        }
        worker = new Worker(env);
        if (worker == nullptr) {
            napi_throw_error(env, nullptr, "create worker error");
            return nullptr;
        }
    }
    worker->StartExecuteInThread(env);
    return nullptr;
}

void Worker::StartExecuteInThread(napi_env env)
{
    if (!runner_) {
        runner_ = std::make_unique<TaskRunner>(TaskStartCallback(ExecuteInThread, this));
    }
    if (runner_) {
        runner_->Execute(); // start a new thread
    } else {
        HILOG_ERROR("runner_ is nullptr");
    }
}

void Worker::ExecuteInThread(const void *data)
{
    auto worker = reinterpret_cast<Worker *>(const_cast<void *>(data));
    napi_env workerEnv = nullptr;
    {
        std::lock_guard<std::recursive_mutex> lock(worker->liveEnvLock_);
        napi_env env = worker->hostEnv_;
        napi_create_runtime(env, &workerEnv);
        if (workerEnv == nullptr) {
            napi_throw_error(env, nullptr, "Worker create runtime error");
            return;
        }
        liveEnvs.push_back(workerEnv);
        idleEnvs.push_back(workerEnv);
        reinterpret_cast<NativeEngine*>(workerEnv)->MarkSubThread();
        worker->workerEnv_ = workerEnv;
    }

    uv_loop_t* loop = worker->GetWorkerLoop();
    if (loop == nullptr) {
        HILOG_ERROR("worker:: Worker loop is nullptr");
        return;
    }

    if (worker->PrepareForWorkerInstance()) {
// #if !defined(WINDOWS_PLATFORM) && !defined(MAC_PLATFORM)
//         uv_async_init(loop, &worker->debuggerOnPostTaskSignal_, reinterpret_cast<uv_async_cb>(
//             Worker::HandleDebuggerTask));
// #endif
        worker->Loop();
    } else {
        HILOG_ERROR("worker:: worker PrepareForWorkerInstance failure");
    }
}

bool Worker::PrepareForWorkerInstance()
{
    std::lock_guard<std::recursive_mutex> lock(liveEnvLock_);

    auto workerEngine = reinterpret_cast<NativeEngine*>(workerEnv_);
    auto hostEngine = reinterpret_cast<NativeEngine*>(hostEnv_);
    hostEngine->CallWorkerAsyncWorkFunc(workerEngine);
// #if !defined(WINDOWS_PLATFORM) && !defined(MAC_PLATFORM)
//     workerEngine->SetDebuggerPostTaskFunc(
//         std::bind(&Worker::DebuggerOnPostTask, this, std::placeholders::_1));
// #endif
    if (!hostEngine->CallInitWorkerFunc(workerEngine)) {
        HILOG_ERROR("Worker init func failure");
        return false;
    }
    return true;
}

} // namespace Commonlibrary::TaskPoolModule