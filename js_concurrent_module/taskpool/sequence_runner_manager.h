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

#ifndef JS_CONCURRENT_MODULE_TASKPOOL_SEQ_RUNNER_MANAGER_H
#define JS_CONCURRENT_MODULE_TASKPOOL_SEQ_RUNNER_MANAGER_H

#include "sequence_runner.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
class SequenceRunnerManager {
public:
    static SequenceRunnerManager& GetInstance();
    SequenceRunner* GetSeqRunner(uint64_t seqRunnerId);

    SequenceRunner* CreateOrGetGlobalRunner(napi_env env, napi_value thisVar, size_t argc,
                                            const std::string& name, uint32_t priority);
    void SequenceRunnerDestructor(SequenceRunner* seqRunner);
    void AddTaskToSeqRunner(uint64_t seqRunnerId, Task* task);
    bool TriggerSeqRunner(napi_env env, Task* lastTask);
    void StoreSequenceRunner(uint64_t seqRunnerId, SequenceRunner* seqRunner);
    void RemoveWaitingTask(Task* task);
    bool FindRunnerAndRef(uint64_t seqRunnerId);

private:
    SequenceRunnerManager() = default;
    ~SequenceRunnerManager() = default;
    SequenceRunnerManager(const SequenceRunnerManager &) = delete;
    SequenceRunnerManager& operator=(const SequenceRunnerManager &) = delete;
    SequenceRunnerManager(SequenceRunnerManager &&) = delete;
    SequenceRunnerManager& operator=(SequenceRunnerManager &&) = delete;
    void RemoveSequenceRunnerByName(const std::string& name);
    void RemoveSequenceRunner(uint64_t seqRunnerId);
    bool UnrefAndDestroyRunner(SequenceRunner* seqRunner);

    // <<name1, seqRunner>, <name2, seqRunner>, ...>
    std::unordered_map<std::string, SequenceRunner*> globalSeqRunner_ {};
    // <seqRunnerId, SequenceRunner>
    std::unordered_map<uint64_t, SequenceRunner*> seqRunners_ {};
    std::mutex seqRunnersMutex_;

    friend class NativeEngineTest;
};
} // namespace Commonlibrary::Concurrent::TaskPoolModule
#endif // JS_CONCURRENT_MODULE_TASKPOOL_SEQ_RUNNER_MANAGER_H