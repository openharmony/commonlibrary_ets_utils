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

AsyncRunner* AsyncRunnerManager::GetRunner(uint64_t runnerId)
{
    auto runner = BaseRunnerManager::GetRunner(runnerId);
    if (runner != nullptr) {
        return static_cast<AsyncRunner*>(runner);
    }
    return nullptr;
}

AsyncRunner* AsyncRunnerManager::CreateOrGetGlobalRunner(napi_env env, napi_value thisVar, const std::string& name,
                                                         uint32_t runningCapacity, uint32_t waitingCapacity)
{
    std::unique_lock<std::mutex> lock(runnersMutex_);
    AsyncRunnerConfig asyncRunnerconfig(runningCapacity, waitingCapacity);
    return static_cast<AsyncRunner*>(
        BaseRunnerManager::CreateOrGetGlobalRunner(env, thisVar, name, &asyncRunnerconfig));
}

BaseRunner* AsyncRunnerManager::CreateGlobalRunner(const std::string& name, void* config)
{
    AsyncRunnerConfig* asyncRunnerConfig = static_cast<AsyncRunnerConfig*>(config);
    return AsyncRunner::CreateGlobalRunner(name, asyncRunnerConfig->runningCapacity_,
                                           asyncRunnerConfig->waitingCapacity_);
}

bool AsyncRunnerManager::CheckGlobalRunnerParams(napi_env env, BaseRunner *runner, void* config)
{
    AsyncRunnerConfig* asyncRunnerConfig = static_cast<AsyncRunnerConfig*>(config);
    AsyncRunner* asyncRunner = static_cast<AsyncRunner*>(runner);
    if (asyncRunnerConfig->runningCapacity_ != asyncRunner->runningCapacity_) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "AsyncRunner runningCapacity can not changed.");
        return false;
    }
    if (asyncRunnerConfig->waitingCapacity_ != asyncRunner->waitingCapacity_) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "AsyncRunner waitingCapacity can not changed.");
        return false;
    }
    return true;
}

bool AsyncRunnerManager::TriggerAsyncRunner(napi_env env, Task* lastTask)
{
    uint64_t asyncRunnerId = lastTask->runnerId_;
    AsyncRunner* asyncRunner = GetRunner(asyncRunnerId);
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
    auto asyncRunner = GetRunner(task->runnerId_);
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
        if (asyncRunner->RemoveWaitingTask(task)) {
            asyncRunner->TriggerRejectErrorTimer(task, ErrorHelper::ERR_ASYNCRUNNER_TASK_CANCELED);
        }
    }
}

void AsyncRunnerManager::LogRunnerNotExist()
{
    HILOG_ERROR("taskpool:: asyncRunner not exist.");
}

void AsyncRunnerManager::DecreaseRunningCount(uint64_t asyncRunnerId)
{
    std::unique_lock<std::mutex> lock(runnersMutex_);
    auto iter = runners_.find(asyncRunnerId);
    if (iter == runners_.end()) {
        return ;
    }
    AsyncRunner* asyncRunner = static_cast<AsyncRunner*>(iter->second);
    asyncRunner->DecreaseRunningCount();
}
} // namespace Commonlibrary::Concurrent::TaskPoolModule