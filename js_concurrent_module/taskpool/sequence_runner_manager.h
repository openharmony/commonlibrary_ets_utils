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

#include "base_runner_manager.h"
#include "sequence_runner.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
class SequenceRunnerConfig {
public:
    SequenceRunnerConfig(uint32_t argc, uint32_t priority)
        : argc_(argc), priority_(priority) {}
    uint32_t argc_ {};
    uint32_t priority_ {};
};

class SequenceRunnerManager : public BaseRunnerManager {
public:
    static SequenceRunnerManager& GetInstance();
    SequenceRunner* GetRunner(uint64_t runnerId);
    SequenceRunner* CreateOrGetGlobalRunner(napi_env env, napi_value thisVar, size_t argc,
                                            const std::string& name, uint32_t priority);
    void AddTaskToSeqRunner(uint64_t seqRunnerId, Task* task);
    bool TriggerSeqRunner(napi_env env, Task* lastTask);

protected:
    bool CheckGlobalRunnerParams(napi_env env, BaseRunner* runner, void* config) override;
    BaseRunner* CreateGlobalRunner(const std::string& name, void* config) override;
    void LogRunnerNotExist() override;

private:
    SequenceRunnerManager() = default;
    ~SequenceRunnerManager() override = default;
    SequenceRunnerManager(const SequenceRunnerManager &) = delete;
    SequenceRunnerManager& operator=(const SequenceRunnerManager &) = delete;
    SequenceRunnerManager(SequenceRunnerManager &&) = delete;
    SequenceRunnerManager& operator=(SequenceRunnerManager &&) = delete;
    friend class NativeEngineTest;
};
} // namespace Commonlibrary::Concurrent::TaskPoolModule
#endif // JS_CONCURRENT_MODULE_TASKPOOL_SEQ_RUNNER_MANAGER_H