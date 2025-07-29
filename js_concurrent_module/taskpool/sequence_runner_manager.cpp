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


#include "sequence_runner_manager.h"

#include "helper/error_helper.h"
#include "helper/hitrace_helper.h"
#include "sequence_runner.h"
#include "task_manager.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
using namespace Commonlibrary::Concurrent::Common::Helper;

SequenceRunnerManager& SequenceRunnerManager::GetInstance()
{
    static SequenceRunnerManager sequenceRunnerManager;
    return sequenceRunnerManager;
}

SequenceRunner* SequenceRunnerManager::CreateOrGetGlobalRunner(napi_env env, napi_value thisVar, size_t argc,
                                                               const std::string& name, uint32_t priority)
{
    SequenceRunner* seqRunner = nullptr;
    std::unique_lock<std::mutex> lock(seqRunnersMutex_);
    auto iter = globalSeqRunner_.find(name);
    if (iter == globalSeqRunner_.end()) {
        Priority priorityVal = Priority::DEFAULT;
        if (argc == 2) { // 2: The number of parameters is 2.
            priorityVal = static_cast<Priority>(priority);
        }
        seqRunner = new SequenceRunner(priorityVal, name, true);
        globalSeqRunner_.emplace(name, seqRunner);
        return seqRunner;
    }
    
    seqRunner = iter->second;
    if (priority != seqRunner->priority_) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "seqRunner:: priority can not changed.");
        return nullptr;
    }
    seqRunner->IncreaseSeqCount();

    return seqRunner;
}

void SequenceRunnerManager::RemoveSequenceRunnerByName(const std::string& name)
{
    auto iter = globalSeqRunner_.find(name.c_str());
    if (iter != globalSeqRunner_.end()) {
        globalSeqRunner_.erase(iter->first);
    }
}

void SequenceRunnerManager::SequenceRunnerDestructor(SequenceRunner* seqRunner)
{
    auto runner = GetSeqRunner(seqRunner->seqRunnerId_);
    if (runner == nullptr) {
        return;
    }
    UnrefAndDestroyRunner(seqRunner);
}

void SequenceRunnerManager::StoreSequenceRunner(uint64_t seqRunnerId, SequenceRunner* seqRunner)
{
    std::unique_lock<std::mutex> lock(seqRunnersMutex_);
    seqRunners_.emplace(seqRunnerId, seqRunner);
}

void SequenceRunnerManager::RemoveSequenceRunner(uint64_t seqRunnerId)
{
    seqRunners_.erase(seqRunnerId);
}

SequenceRunner* SequenceRunnerManager::GetSeqRunner(uint64_t seqRunnerId)
{
    std::unique_lock<std::mutex> lock(seqRunnersMutex_);
    auto iter = seqRunners_.find(seqRunnerId);
    if (iter != seqRunners_.end()) {
        return iter->second;
    }
    HILOG_DEBUG("taskpool:: sequenceRunner has been released.");
    return nullptr;
}

void SequenceRunnerManager::AddTaskToSeqRunner(uint64_t seqRunnerId, Task* task)
{
    std::unique_lock<std::mutex> lock(seqRunnersMutex_);
    auto iter = seqRunners_.find(seqRunnerId);
    if (iter == seqRunners_.end()) {
        HILOG_ERROR("seqRunner:: seqRunner not found.");
        return;
    } else {
        iter->second->AddTask(task);
    }
}

bool SequenceRunnerManager::TriggerSeqRunner(napi_env env, Task* lastTask)
{
    uint64_t seqRunnerId = lastTask->seqRunnerId_;
    SequenceRunner* seqRunner = GetSeqRunner(seqRunnerId);
    if (seqRunner == nullptr) {
        HILOG_ERROR("taskpool:: trigger seqRunner not exist.");
        return false;
    }
    if (UnrefAndDestroyRunner(seqRunner)) {
        HILOG_ERROR("taskpool:: trigger seqRunner is removed.");
        return false;
    }
    if (seqRunner->currentTaskId_ != lastTask->taskId_) {
        HILOG_ERROR("taskpool:: only front task can trigger seqRunner.");
        return false;
    }
    seqRunner->TriggerTask(env);
    return true;
}

void SequenceRunnerManager::RemoveWaitingTask(Task* task)
{
    auto seqRunner = GetSeqRunner(task->seqRunnerId_);
    if (seqRunner == nullptr) {
        return;
    }
    if (seqRunner->RemoveWaitingTask(task)) {
        UnrefAndDestroyRunner(seqRunner);
    }
}

bool SequenceRunnerManager::FindRunnerAndRef(uint64_t seqRunnerId)
{
    std::unique_lock<std::mutex> lock(seqRunnersMutex_);
    auto iter = seqRunners_.find(seqRunnerId);
    if (iter == seqRunners_.end()) {
        HILOG_ERROR("taskpool:: seqRunner not exist.");
        return false;
    }
    iter->second->IncreaseSeqCount();
    return true;
}

bool SequenceRunnerManager::UnrefAndDestroyRunner(SequenceRunner* seqRunner)
{
    std::unique_lock<std::mutex> lock(seqRunnersMutex_);
    if (seqRunner->DecreaseSeqCount() != 0) {
        return false;
    }
    RemoveSequenceRunner(seqRunner->seqRunnerId_);

    if (seqRunner->isGlobalRunner_) {
        RemoveSequenceRunnerByName(seqRunner->seqName_);
    }
    delete seqRunner;
    return true;
}
} // namespace Commonlibrary::Concurrent::TaskPoolModule