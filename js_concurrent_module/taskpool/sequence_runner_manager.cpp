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

SequenceRunner* SequenceRunnerManager::GetRunner(uint64_t runnerId)
{
    auto runner = BaseRunnerManager::GetRunner(runnerId);
    if (runner != nullptr) {
        return static_cast<SequenceRunner*>(runner);
    }
    return nullptr;
}

SequenceRunner* SequenceRunnerManager::CreateOrGetGlobalRunner(napi_env env, napi_value thisVar, size_t argc,
                                                               const std::string& name, uint32_t priority)
{
    std::unique_lock<std::mutex> lock(runnersMutex_);
    SequenceRunnerConfig sequenceRunnerConfig(argc, priority);
    return static_cast<SequenceRunner*>(
        BaseRunnerManager::CreateOrGetGlobalRunner(env, thisVar, name, &sequenceRunnerConfig)
    );
}

BaseRunner* SequenceRunnerManager::CreateGlobalRunner(const std::string& name, void* config)
{
    SequenceRunnerConfig* sequenceRunnerConfig = static_cast<SequenceRunnerConfig*>(config);
    Priority priorityVal = Priority::DEFAULT;
    if (sequenceRunnerConfig->argc_ == 2) { // 2: The number of parameters is 2.
        priorityVal = static_cast<Priority>(sequenceRunnerConfig->priority_);
    }
    auto seqRunner = new SequenceRunner(priorityVal, name, true);
    return static_cast<BaseRunner *>(seqRunner);
}

bool SequenceRunnerManager::CheckGlobalRunnerParams(napi_env env, BaseRunner* runner, void* config)
{
    SequenceRunnerConfig* sequenceRunnerConfig = static_cast<SequenceRunnerConfig*>(config);
    SequenceRunner* seqRunner = static_cast<SequenceRunner*>(runner);
    if (sequenceRunnerConfig->priority_ != seqRunner->priority_) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "seqRunner:: priority can not changed.");
        return false;
    }
    return true;
}

void SequenceRunnerManager::AddTaskToSeqRunner(uint64_t seqRunnerId, Task* task)
{
    std::unique_lock<std::mutex> lock(runnersMutex_);
    auto iter = runners_.find(seqRunnerId);
    if (iter == runners_.end()) {
        HILOG_ERROR("seqRunner:: seqRunner not found.");
        return;
    } else {
        SequenceRunner* runner = static_cast<SequenceRunner*>(iter->second);
        runner->AddTask(task);
    }
}

bool SequenceRunnerManager::TriggerSeqRunner(napi_env env, Task* lastTask)
{
    uint64_t seqRunnerId = lastTask->runnerId_;
    SequenceRunner* seqRunner = GetRunner(seqRunnerId);
    BaseRunner* baseRunner = static_cast<BaseRunner*>(seqRunner);
    if (seqRunner == nullptr) {
        HILOG_ERROR("taskpool:: trigger seqRunner not exist.");
        return false;
    }
    if (UnrefAndDestroyRunner(baseRunner)) {
        HILOG_WARN("taskpool:: trigger seqRunner is removed.");
        return false;
    }
    if (seqRunner->currentTaskId_ != lastTask->taskId_) {
        HILOG_ERROR("taskpool:: only front task can trigger seqRunner.");
        return false;
    }
    seqRunner->TriggerTask(env);
    return true;
}

void SequenceRunnerManager::LogRunnerNotExist()
{
    HILOG_ERROR("taskpool:: seqRunner not exist.");
}
} // namespace Commonlibrary::Concurrent::TaskPoolModule