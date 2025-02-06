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
    std::unique_lock<std::mutex> lock(globalSeqRunnerMutex_);
    SequenceRunner* seqRunner = nullptr;
    auto iter = globalSeqRunner_.find(name);
    if (iter == globalSeqRunner_.end()) {
        seqRunner = new SequenceRunner();
        // refresh priority default values on first creation
        if (argc == 2) { // 2: The number of parameters is 2.
            seqRunner->priority_ = static_cast<Priority>(priority);
        }
        seqRunner->isGlobalRunner_ = true;
        seqRunner->seqName_ = name;
        globalSeqRunner_.emplace(name, seqRunner);
    } else {
        seqRunner = iter->second;
        if (priority != seqRunner->priority_) {
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "seqRunner:: priority can not changed.");
            return nullptr;
        }
    }
    seqRunner->count_++;
    auto tmpIter = seqRunner->globalSeqRunnerRef_.find(env);
    if (tmpIter == seqRunner->globalSeqRunnerRef_.end()) {
        napi_ref gloableSeqRunnerRef = nullptr;
        napi_create_reference(env, thisVar, 0, &gloableSeqRunnerRef);
        seqRunner->globalSeqRunnerRef_.emplace(env, gloableSeqRunnerRef);
    }

    return seqRunner;
}

bool SequenceRunnerManager::TriggerGlobalSeqRunner(napi_env env, SequenceRunner* seqRunner)
{
    std::unique_lock<std::mutex> lock(globalSeqRunnerMutex_);
    if (seqRunner->isGlobalRunner_) {
        auto iter = seqRunner->globalSeqRunnerRef_.find(env);
        if (iter == seqRunner->globalSeqRunnerRef_.end()) {
            return false;
        }
        napi_reference_unref(env, iter->second, nullptr);
    } else {
        napi_reference_unref(env, seqRunner->seqRunnerRef_, nullptr);
    }
    return true;
}

uint64_t SequenceRunnerManager::DecreaseSeqCount(SequenceRunner* seqRunner)
{
    std::unique_lock<std::mutex> lock(globalSeqRunnerMutex_);
    return --(seqRunner->count_);
}

void SequenceRunnerManager::RemoveGlobalSeqRunnerRef(napi_env env, SequenceRunner* seqRunner)
{
    std::lock_guard<std::mutex> lock(globalSeqRunnerMutex_);
    auto iter = seqRunner->globalSeqRunnerRef_.find(env);
    if (iter != seqRunner->globalSeqRunnerRef_.end()) {
        napi_delete_reference(env, iter->second);
        seqRunner->globalSeqRunnerRef_.erase(iter);
    }
}

void SequenceRunnerManager::RemoveSequenceRunner(const std::string& name)
{
    std::unique_lock<std::mutex> lock(globalSeqRunnerMutex_);
    auto iter = globalSeqRunner_.find(name.c_str());
    if (iter != globalSeqRunner_.end()) {
        globalSeqRunner_.erase(iter->first);
    }
}

void SequenceRunnerManager::GlobalSequenceRunnerDestructor(napi_env env, SequenceRunner* seqRunner)
{
    RemoveGlobalSeqRunnerRef(env, seqRunner);
    if (DecreaseSeqCount(seqRunner) == 0) {
        RemoveSequenceRunner(seqRunner->seqName_);
        SequenceRunnerManager::GetInstance().RemoveSequenceRunner(seqRunner->seqRunnerId_);
        delete seqRunner;
    }
}

bool SequenceRunnerManager::IncreaseGlobalSeqRunner(napi_env env, SequenceRunner* seqRunner)
{
    std::unique_lock<std::mutex> lock(globalSeqRunnerMutex_);
    if (seqRunner->isGlobalRunner_) {
        auto iter = seqRunner->globalSeqRunnerRef_.find(env);
        if (iter == seqRunner->globalSeqRunnerRef_.end()) {
            return false;
        }
        napi_reference_ref(env, iter->second, nullptr);
    } else {
        napi_reference_ref(env, seqRunner->seqRunnerRef_, nullptr);
    }
    return true;
}

void SequenceRunnerManager::StoreSequenceRunner(uint64_t seqRunnerId, SequenceRunner* seqRunner)
{
    std::unique_lock<std::mutex> lock(seqRunnersMutex_);
    seqRunners_.emplace(seqRunnerId, seqRunner);
}

void SequenceRunnerManager::RemoveSequenceRunner(uint64_t seqRunnerId)
{
    std::unique_lock<std::mutex> lock(seqRunnersMutex_);
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
        std::unique_lock<std::shared_mutex> seqRunnerLock(iter->second->seqRunnerMutex_);
        iter->second->seqRunnerTasks_.push_back(task);
    }
}

bool SequenceRunnerManager::TriggerSeqRunner(napi_env env, Task* lastTask)
{
    uint64_t seqRunnerId = lastTask->seqRunnerId_;
    SequenceRunner* seqRunner = GetSeqRunner(seqRunnerId);
    if (seqRunner == nullptr) {
        HILOG_ERROR("seqRunner:: trigger seqRunner not exist.");
        return false;
    }
    if (!SequenceRunnerManager::GetInstance().TriggerGlobalSeqRunner(env, seqRunner)) {
        HILOG_ERROR("seqRunner:: trigger globalSeqRunner not exist.");
        return false;
    }
    if (seqRunner->currentTaskId_ != lastTask->taskId_) {
        HILOG_ERROR("seqRunner:: only front task can trigger seqRunner.");
        return false;
    }
    std::list<napi_deferred> deferreds {};
    {
        std::unique_lock<std::shared_mutex> lock(seqRunner->seqRunnerMutex_);
        if (seqRunner->seqRunnerTasks_.empty()) {
            seqRunner->currentTaskId_ = 0;
            return true;
        }
        Task* task = seqRunner->seqRunnerTasks_.front();
        seqRunner->seqRunnerTasks_.pop_front();
        bool isEmpty = false;
        while (task->taskState_ == ExecuteState::CANCELED) {
            deferreds.push_back(task->currentTaskInfo_->deferred);
            task->DisposeCanceledTask();
            if (seqRunner->seqRunnerTasks_.empty()) {
                HILOG_DEBUG("seqRunner:: seqRunner %{public}s empty in cancel loop.",
                            std::to_string(seqRunnerId).c_str());
                seqRunner->currentTaskId_ = 0;
                isEmpty = true;
                break;
            }
            task = seqRunner->seqRunnerTasks_.front();
            seqRunner->seqRunnerTasks_.pop_front();
        }
        if (!isEmpty) {
            seqRunner->currentTaskId_ = task->taskId_;
            task->IncreaseRefCount();
            task->taskState_ = ExecuteState::WAITING;
            HILOG_DEBUG("seqRunner:: Trigger task %{public}s in seqRunner %{public}s.",
                        std::to_string(task->taskId_).c_str(), std::to_string(seqRunnerId).c_str());
            TaskManager::GetInstance().EnqueueTaskId(task->taskId_, seqRunner->priority_);
        }
    }
    TaskManager::GetInstance().BatchRejectDeferred(env, deferreds, "taskpool:: sequenceRunner task has been canceled");
    return true;
}

void SequenceRunnerManager::RemoveWaitingTask(Task* task)
{
    auto seqRunner = GetSeqRunner(task->seqRunnerId_);
    if (seqRunner != nullptr) {
        seqRunner->RemoveWaitingTask(task);
    }
}
} // namespace Commonlibrary::Concurrent::TaskPoolModule