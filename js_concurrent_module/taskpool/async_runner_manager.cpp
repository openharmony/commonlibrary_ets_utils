/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "async_runner_manager.h"

#include <cinttypes>

#include "helper/error_helper.h"
#include "task_manager.h"
#include "tools/log.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {

AsyncRunnerManager& AsyncRunnerManager::GetInstance()
{
    static AsyncRunnerManager asyncRunnerManager;
    return asyncRunnerManager;
}

AsyncRunner* AsyncRunnerManager::CreateOrGetGlobalRunner(napi_env env, napi_value thisVar, const std::string& name,
                                                         uint32_t runningCapacity, uint32_t waitingCapacity)
{
    AsyncRunner *asyncRunner = nullptr;
    uint64_t asyncRunnerId = 0;
    {
        std::unique_lock<std::mutex> lock(globalAsyncRunnerMutex_);
        auto iter = globalAsyncRunner_.find(name);
        if (iter == globalAsyncRunner_.end()) {
            asyncRunner = AsyncRunner::CreateGlobalRunner(name, runningCapacity, waitingCapacity);
            globalAsyncRunner_.emplace(name, asyncRunner);
            return asyncRunner;
        } else {
            asyncRunner = iter->second;
            bool res = asyncRunner->CheckGlobalRunnerParams(env, runningCapacity, waitingCapacity);
            if (!res) {
                return nullptr;
            }
            asyncRunnerId = asyncRunner->asyncRunnerId_;
        }
    }
    if (!FindRunnerAndRef(asyncRunnerId)) {
        return nullptr;
    }

    return asyncRunner;
}

void AsyncRunnerManager::StoreAsyncRunner(uint64_t asyncRunnerId, AsyncRunner* asyncRunner)
{
    std::unique_lock<std::mutex> lock(asyncRunnersMutex_);
    asyncRunners_.emplace(asyncRunnerId, asyncRunner);
}

void AsyncRunnerManager::RemoveAsyncRunner(uint64_t asyncRunnerId)
{
    asyncRunners_.erase(asyncRunnerId);
}

AsyncRunner* AsyncRunnerManager::GetAsyncRunner(uint64_t asyncRunnerId)
{
    std::unique_lock<std::mutex> lock(asyncRunnersMutex_);
    auto iter = asyncRunners_.find(asyncRunnerId);
    if (iter != asyncRunners_.end()) {
        return iter->second;
    }
    return nullptr;
}

bool AsyncRunnerManager::TriggerAsyncRunner(napi_env env, Task* lastTask)
{
    uint64_t asyncRunnerId = lastTask->asyncRunnerId_;
    AsyncRunner* asyncRunner = GetAsyncRunner(asyncRunnerId);
    if (asyncRunner == nullptr) {
        HILOG_ERROR("taskpool:: trigger asyncRunner not exist.");
        return false;
    }
    if (UnrefAndDestroyRunner(asyncRunner)) {
        HILOG_ERROR("taskpool:: trigger asyncRunner is remove.");
        return false;
    }
    asyncRunner->TriggerWaitingTask();
    return true;
}

void AsyncRunnerManager::RemoveGlobalAsyncRunner(const std::string& name)
{
    std::unique_lock<std::mutex> lock(globalAsyncRunnerMutex_);
    auto iter = globalAsyncRunner_.find(name);
    if (iter != globalAsyncRunner_.end()) {
        globalAsyncRunner_.erase(iter);
    }
}

void AsyncRunnerManager::CancelAsyncRunnerTask(napi_env env, Task* task)
{
    std::string errMsg = "";
    ExecuteState state = ExecuteState::NOT_FOUND;
    {
        std::lock_guard<std::recursive_mutex> lock(task->taskMutex_);
        if (task->taskState_ == ExecuteState::FINISHED || task->taskState_ == ExecuteState::ENDING) {
            errMsg = "AsyncRunner task has been executed.";
            HILOG_ERROR("taskpool:: %{public}s", errMsg.c_str());
            ErrorHelper::ThrowError(env, ErrorHelper::ERR_CANCEL_NONEXIST_TASK, errMsg.c_str());
            return;
        }

        state = task->taskState_.exchange(ExecuteState::CANCELED);
    }
    task->CancelPendingTask(env);
    auto asyncRunner = GetAsyncRunner(task->asyncRunnerId_);
    if (state == ExecuteState::WAITING && task->currentTaskInfo_ != nullptr &&
        TaskManager::GetInstance().EraseWaitingTaskId(task->taskId_, task->currentTaskInfo_->priority)) {
        task->DecreaseTaskLifecycleCount();
        TaskManager::GetInstance().DecreaseSendDataRefCount(task->env_, task->taskId_);
        if (asyncRunner != nullptr) {
            asyncRunner->TriggerRejectErrorTimer(task, ErrorHelper::ERR_ASYNCRUNNER_TASK_CANCELED, true);
        }
        TriggerAsyncRunner(env, task);
    }
    
    if (asyncRunner != nullptr) {
        asyncRunner->RemoveWaitingTask(task);
    }
}

void AsyncRunnerManager::RemoveWaitingTask(Task* task)
{
    auto asyncRunner = GetAsyncRunner(task->asyncRunnerId_);
    if (asyncRunner != nullptr) {
        asyncRunner->RemoveWaitingTask(task, false);
    }
}

bool AsyncRunnerManager::FindRunnerAndRef(uint64_t asyncRunnerId)
{
    std::unique_lock<std::mutex> lock(asyncRunnersMutex_);
    auto iter = asyncRunners_.find(asyncRunnerId);
    if (iter == asyncRunners_.end()) {
        HILOG_ERROR("taskpool:: asyncRunner not exist.");
        return false;
    }
    iter->second->IncreaseAsyncCount();
    return true;
}

bool AsyncRunnerManager::UnrefAndDestroyRunner(AsyncRunner* asyncRunner)
{
    {
        std::unique_lock<std::mutex> lock(asyncRunnersMutex_);
        if (asyncRunner->DecreaseAsyncCount() != 0) {
            return false;
        }
        RemoveAsyncRunner(asyncRunner->asyncRunnerId_);
    }
    if (asyncRunner->isGlobalRunner_) {
        RemoveGlobalAsyncRunner(asyncRunner->name_);
    }
    delete asyncRunner;
    asyncRunner = nullptr;
    return true;
}

void AsyncRunnerManager::DecreaseRunningCount(uint64_t asyncRunnerId)
{
    std::unique_lock<std::mutex> lock(asyncRunnersMutex_);
    auto iter = asyncRunners_.find(asyncRunnerId);
    if (iter == asyncRunners_.end()) {
        return;
    }
    iter->second->DecreaseRunningCount();
}
} // namespace Commonlibrary::Concurrent::TaskPoolModule