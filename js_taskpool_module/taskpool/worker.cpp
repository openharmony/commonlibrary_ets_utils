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

#include <shared_mutex>
#include <unistd.h>
#include <unordered_map>

#include "utils/log.h"

namespace Commonlibrary::TaskPoolModule {
const static int MAX_THREADPOOL_SIZE = 4;
static std::list<WorkerEnv> g_liveEnvs;
static std::unordered_map<int32_t, TaskInfo*> g_taskInfoMap;
static TaskQueue g_taskQueue;
static std::mutex g_workersMutex;
static std::shared_mutex g_taskMutex;

Worker::Worker(napi_env env) : hostEnv_(env) {}

void Worker::StoreTaskInfo(int32_t taskId, TaskInfo *taskInfo)
{
    std::unique_lock<std::shared_mutex> lock(g_taskMutex);
    g_taskInfoMap.emplace(taskId, taskInfo);
}

void Worker::EnqueueTask(std::unique_ptr<Task> task)
{
    g_taskQueue.EnqueueTask(std::move(task));
}

void Worker::CancelTask(int32_t taskId)
{
    std::unique_lock<std::shared_mutex> lock(g_taskMutex);
    auto iter = g_taskInfoMap.find(taskId);
    if (iter == g_taskInfoMap.end() || iter->second == nullptr) {
        HILOG_ERROR("taskpool:: Failed to find the task");
        return;
    }
    iter->second->canceled = true;
}

bool Worker::NeedInitWorker()
{
    std::unique_lock<std::mutex> lock(g_workersMutex);
    if (g_liveEnvs.size() > 0) {
        return false;
    }
    return true;
}

bool Worker::NeedExpandWorker()
{
    std::unique_lock<std::mutex> lock(g_workersMutex);
    if (g_liveEnvs.size() >= MAX_THREADPOOL_SIZE) {
        HILOG_DEBUG("taskpool:: Worker thread num reaches the maximum");
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
            return nullptr;
        }
        worker = new Worker(env);
        if (worker == nullptr) {
            ThrowError(env, Worker::NOTRUNNING_ERROR, "taskpool:: worker is null when InitWorker");
            return nullptr;
        }
    }
    worker->StartExecuteInThread(env);
    return nullptr;
}

void ReleaseTaskContent(TaskInfo* taskInfo)
{
    if (taskInfo != nullptr && taskInfo->taskSignal != nullptr &&
        !uv_is_closing(reinterpret_cast<uv_handle_t*>(taskInfo->taskSignal))) {
        uv_close(reinterpret_cast<uv_handle_t*>(taskInfo->taskSignal), [](uv_handle_t* handle) {
            if (handle != nullptr) {
                delete reinterpret_cast<uv_async_t*>(handle);
                handle = nullptr;
            }
        });
    }

    std::unique_lock<std::shared_mutex> lock(g_taskMutex);
    auto iter = g_taskInfoMap.find(taskInfo->taskId);
    delete iter->second;
    iter->second = nullptr;
    g_taskInfoMap.erase(iter);
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
    ReleaseTaskContent(taskInfo);
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

void Worker::ExecuteInThread(const void *data)
{
    auto worker = reinterpret_cast<Worker *>(const_cast<void *>(data));
    napi_env workerEnv = nullptr;
    {
        std::unique_lock<std::recursive_mutex> lock(worker->liveEnvLock_);
        napi_env env = worker->hostEnv_;
        napi_create_runtime(env, &workerEnv);
        if (workerEnv == nullptr) {
            ThrowError(env, Worker::NOTRUNNING_ERROR, "taskpool:: Worker create runtime error");
            return;
        }
        g_liveEnvs.push_back(workerEnv);
        reinterpret_cast<NativeEngine*>(workerEnv)->MarkSubThread();
        worker->workerEnv_ = workerEnv;
    }
    uv_loop_t* loop = worker->GetWorkerLoop();
    if (loop == nullptr) {
        ThrowError(workerEnv, Worker::NOTRUNNING_ERROR, "taskpool:: Worker loop is nullptr");
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

void Worker::ThrowError(napi_env env, int32_t errCode, const char* errMessage)
{
    std::string errTitle = "";
    napi_value workerError = nullptr;

    napi_value code = nullptr;
    napi_create_uint32(env, errCode, &code);

    napi_value name = nullptr;
    std::string errName = "BusinessError";
    napi_create_string_utf8(env, errName.c_str(), NAPI_AUTO_LENGTH, &name);
    napi_value msg = nullptr;
    napi_create_string_utf8(env, (errTitle + std::string(errMessage)).c_str(), NAPI_AUTO_LENGTH, &msg);

    napi_create_error(env, nullptr, msg, &workerError);
    napi_set_named_property(env, workerError, "code", code);
    napi_set_named_property(env, workerError, "name", name);
    napi_throw(env, workerError);
}

void Worker::PerformTask(const uv_async_t* req)
{
    Worker *worker = static_cast<Worker*>(req->data);
    while (std::unique_ptr<Task> task = g_taskQueue.DequeueTask()) {
        napi_env env = worker->workerEnv_;
        TaskInfo *taskInfo = nullptr;
        {
            std::shared_lock<std::shared_mutex> lock(g_taskMutex);
            auto iter = g_taskInfoMap.find(task->taskId_);
            if (iter == g_taskInfoMap.end() || iter->second == nullptr) {
                HILOG_ERROR("taskpool:: taskInfo is imcomplete");
                return;
            }
            taskInfo = iter->second;
        }
        if (taskInfo->canceled) {
            ReleaseTaskContent(taskInfo);
            continue;
        }

        napi_value taskData;
        napi_status status = napi_deserialize(env, taskInfo->serializationData, &taskData);
        if (status != napi_ok || taskData == nullptr) {
            continue;
        }
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
        status = napi_serialize(env, result, undefined, &resultData);
        if (status != napi_ok || resultData == nullptr) {
            continue;
        }
        taskInfo->result = resultData;
        uv_async_send(taskInfo->taskSignal);
        g_taskQueue.NotifyWorkerThread();
    }
}
} // namespace Commonlibrary::TaskPoolModule