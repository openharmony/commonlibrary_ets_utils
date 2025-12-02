/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "base_runner_manager.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
BaseRunnerManager::~BaseRunnerManager() = default;

BaseRunner* BaseRunnerManager::CreateOrGetGlobalRunner(napi_env env, napi_value thisVar, const std::string& name,
                                                       void* config)
{
    BaseRunner *baseRunner = nullptr;
    auto iter = globalRunner_.find(name);
    if (iter == globalRunner_.end()) {
        baseRunner = CreateGlobalRunner(name, config);
        globalRunner_.emplace(name, baseRunner);
        return baseRunner;
    }
    auto runnerIter = runners_.find(iter->second->runnerId_);
    if (runnerIter == runners_.end()) {
        globalRunner_.erase(iter);
        baseRunner = CreateGlobalRunner(name, config);
        globalRunner_.emplace(name, baseRunner);
        return baseRunner;
    }
    baseRunner = iter->second;
    bool res = CheckGlobalRunnerParams(env, baseRunner, config);
    if (!res) {
        return nullptr;
    }
    baseRunner->IncreaseCount();
    return baseRunner;
}

void BaseRunnerManager::StoreRunner(uint64_t runnerId, BaseRunner* runner)
{
    std::unique_lock<std::mutex> lock(runnersMutex_);
    runners_.emplace(runnerId, runner);
}

BaseRunner* BaseRunnerManager::GetRunner(uint64_t runnerId)
{
    std::unique_lock<std::mutex> lock(runnersMutex_);
    auto iter = runners_.find(runnerId);
    if (iter != runners_.end()) {
        return iter->second;
    }
    HILOG_DEBUG("taskpool:: get runner, id %{public}s.", std::to_string(runnerId).c_str());
    return nullptr;
}

void BaseRunnerManager::RemoveRunner(uint64_t runnerId)
{
    runners_.erase(runnerId);
}

void BaseRunnerManager::RemoveGlobalRunner(const std::string& name)
{
    auto iter = globalRunner_.find(name);
    if (iter != globalRunner_.end()) {
        globalRunner_.erase(iter);
    }
}

void BaseRunnerManager::RemoveWaitingTask(Task* task)
{
    auto runner = GetRunner(task->runnerId_);
    if (runner == nullptr) {
        return ;
    }
    if (runner->RemoveWaitingTask(task)) {
        UnrefAndDestroyRunner(runner);
    }
}

bool BaseRunnerManager::FindRunnerAndRef(uint64_t runnerId)
{
    std::unique_lock<std::mutex> lock(runnersMutex_);
    auto iter = runners_.find(runnerId);
    if (iter == runners_.end()) {
        LogRunnerNotExist();
        return false;
    }
    iter->second->IncreaseCount();
    return true;
}

bool BaseRunnerManager::UnrefAndDestroyRunner(BaseRunner* runner)
{
    std::unique_lock<std::mutex> lock(runnersMutex_);
    if (runner->DecreaseCount() != 0) {
        return false;
    }
    RemoveRunner(runner->runnerId_);
    if (runner->isGlobalRunner_) {
        RemoveGlobalRunner(runner->name_);
    }
    delete runner;
    runner = nullptr;
    return true;
}
}