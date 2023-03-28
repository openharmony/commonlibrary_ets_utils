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

#include "helper/error_helper.h"
#include "taskpool.h"
#include "utils/log.h"
#include "worker.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
static constexpr int8_t MAX_THREADPOOL_SIZE = 4;
static constexpr int8_t HIGH_PRIORITY_TASK_COUNT = 5;
static constexpr int8_t MEDIUM_PRIORITY_TASK_COUNT = 5;

using namespace Commonlibrary::Concurrent::Common::Helper;

TaskManager &TaskManager::GetInstance()
{
    static TaskManager manager;
    return manager;
}

TaskManager::TaskManager()
{
    for (size_t i = 0; i < taskQueues_.size(); i++) {
        std::unique_ptr<TaskQueue> taskQueue = std::make_unique<TaskQueue>();
        taskQueues_[i] = std::move(taskQueue);
    }
}

TaskManager::~TaskManager()
{
    {
        std::lock_guard<std::mutex> lock(workersMutex_);
        for (auto &worker : workers_) {
            delete worker;
        }
        workers_.clear();
    }
    {
        std::unique_lock<std::shared_mutex> lock(taskInfosMutex_);
        for (auto &[_, taskInfo] : taskInfos_) {
            delete taskInfo;
        }
        taskInfos_.clear();
    }
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

void TaskManager::StoreStateInfo(uint32_t executeId, TaskState state)
{
    std::unique_lock<std::shared_mutex> lock(taskStatesMutex_);
    taskStates_.emplace(executeId, state);
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
    // remove the the taskInfo when executed
    taskInfos_.erase(iter);
    return taskInfo;
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

TaskState TaskManager::QueryState(uint32_t executeId)
{
    std::shared_lock<std::shared_mutex> lock(taskStatesMutex_);
    auto iter = taskStates_.find(executeId);
    if (iter == taskStates_.end()) {
        HILOG_ERROR("taskpool:: failed to find the target task");
        return TaskState::NOT_FOUND;
    }
    return iter->second;
}

bool TaskManager::UpdateState(uint32_t executeId, TaskState state)
{
    std::unique_lock<std::shared_mutex> lock(taskStatesMutex_);
    auto iter = taskStates_.find(executeId);
    if (iter == taskStates_.end()) {
        return false;
    }
    if (state == TaskState::RUNNING) {
        iter->second = state;
    } else {
        taskStates_.erase(iter);
    }
    return true;
}

void TaskManager::CancelTask(napi_env env, uint32_t taskId)
{
    std::unique_lock<std::shared_mutex> lock(runningInfosMutex_);
    auto iter = runningInfos_.find(taskId);
    if (iter == runningInfos_.end() || iter->second.empty()) {
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_CANCAL_NONEXIST_TASK,
            "taskpool:: can not find the task");
        return;
    }
    int32_t result = 0;
    for (auto executeId : iter->second) {
        TaskState state = QueryState(executeId);
        if (state == TaskState::NOT_FOUND) {
            result = ErrorHelper::ERR_CANCAL_NONEXIST_TASK;
            break;
        }
        UpdateState(executeId, TaskState::CANCELED);
        if (state == TaskState::WAITING) {
            TaskInfo* taskInfo = PopTaskInfo(executeId);
            napi_value undefined = nullptr;
            napi_get_undefined(taskInfo->env, &undefined);
            napi_reject_deferred(taskInfo->env, taskInfo->deferred, undefined);
            ReleaseTaskContent(taskInfo);
        } else {
            result = ErrorHelper::ERR_CANCAL_RUNNING_TASK;
        }
    }

    if (result == ErrorHelper::ERR_CANCAL_NONEXIST_TASK) {
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_CANCAL_NONEXIST_TASK,
            "taskpool:: can not find the task");
    } else if (result == ErrorHelper::ERR_CANCAL_RUNNING_TASK) {
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_CANCAL_RUNNING_TASK,
            "taskpool:: can not cancel the running task");
    } else {
        runningInfos_.erase(iter);
    }
}

TaskInfo* TaskManager::GenerateTaskInfo(napi_env env, napi_value object, uint32_t taskId, uint32_t executeId)
{
    napi_value undefined;
    napi_get_undefined(env, &undefined);
    napi_value taskData;
    napi_status serializeStatus = napi_ok;
    serializeStatus = napi_serialize(env, object, undefined, &taskData);
    if (serializeStatus != napi_ok || taskData == nullptr) {
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_WORKER_SERIALIZATION,
            "taskpool: failed to serialize message.");
        return nullptr;
    }
    TaskInfo* taskInfo = new (std::nothrow) TaskInfo();
    taskInfo->env = env;
    taskInfo->executeId = executeId;
    taskInfo->serializationData = taskData;
    taskInfo->taskId = taskId;
    taskInfo->onResultSignal = new uv_async_t;
    uv_loop_t* loop = NapiHelper::GetLibUV(env);
    uv_async_init(loop, taskInfo->onResultSignal, reinterpret_cast<uv_async_cb>(TaskPool::HandleTaskResult));
    taskInfo->onResultSignal->data = taskInfo;

    StoreTaskInfo(executeId, taskInfo);
    return taskInfo;
}

void TaskManager::ReleaseTaskContent(TaskInfo* taskInfo)
{
    if (taskInfo == nullptr) {
        return;
    }
    if (taskInfo->onResultSignal != nullptr &&
        !uv_is_closing(reinterpret_cast<uv_handle_t*>(taskInfo->onResultSignal))) {
        uv_close(reinterpret_cast<uv_handle_t*>(taskInfo->onResultSignal), [](uv_handle_t* handle) {
            if (handle != nullptr) {
                delete reinterpret_cast<uv_async_t*>(handle);
                handle = nullptr;
            }
        });
    }
    delete taskInfo;
    taskInfo = nullptr;
}

bool TaskManager::NeedExpandWorker()
{
    std::unique_lock<std::mutex> lock(workersMutex_);
    return workers_.size() < MAX_THREADPOOL_SIZE;
}

void TaskManager::NotifyWorkerIdle(Worker *worker)
{
    {
        std::unique_lock<std::mutex> lock(workersMutex_);
        idleWorkers_.insert(worker);
    }
    NotifyExecuteTask();
}

void TaskManager::NotifyWorkerAdded(Worker *worker)
{
    std::unique_lock<std::mutex> lock(workersMutex_);
    workers_.insert(worker);
}

void TaskManager::EnqueueTask(std::unique_ptr<Task> task, Priority priority)
{
    {
        std::unique_lock<std::mutex> lock(taskQueuesMutex_);
        taskQueues_[priority]->EnqueueTask(std::move(task));
    }
    NotifyExecuteTask();
}

std::unique_ptr<Task> TaskManager::DequeueTask()
{
    std::unique_lock<std::mutex> lock(taskQueuesMutex_);
    if (highPriorityTask_ < HIGH_PRIORITY_TASK_COUNT) {
        auto &highTaskQueue = taskQueues_[Priority::HIGH];
        auto task = highTaskQueue->DequeueTask();
        if (task != nullptr) {
            highPriorityTask_++;
            return task;
        }
    }
    highPriorityTask_ = 0;

    if (mediumPriorityTask_ < MEDIUM_PRIORITY_TASK_COUNT) {
        auto &mediumTaskQueue = taskQueues_[Priority::MEDIUM];
        auto task = mediumTaskQueue->DequeueTask();
        if (task != nullptr) {
            mediumPriorityTask_++;
            return task;
        }
    }
    mediumPriorityTask_ = 0;

    auto &lowTaskQueue = taskQueues_[Priority::LOW];
    return lowTaskQueue->DequeueTask();
}

void TaskManager::NotifyExecuteTask()
{
    std::unique_lock<std::mutex> lock(workersMutex_);
    if (idleWorkers_.empty()) {
        return;
    }
    auto candidator = idleWorkers_.begin();
    Worker *worker = *candidator;
    idleWorkers_.erase(candidator);
    worker->NotifyExecuteTask();
}

void TaskManager::InitTaskRunner(napi_env env)
{
    if (NeedExpandWorker()) {
        auto worker = Worker::WorkerConstructor(env);
        NotifyWorkerAdded(worker);
    }
}
} // namespace Commonlibrary::Concurrent::TaskPoolModule
