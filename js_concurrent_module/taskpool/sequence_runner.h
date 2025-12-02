/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef JS_CONCURRENT_MODULE_TASKPOOL_RUNNER_H
#define JS_CONCURRENT_MODULE_TASKPOOL_RUNNER_H

#include <unordered_map>

#include "base_runner.h"
#include "task.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
class SequenceRunnerManager;
class SequenceRunner : public BaseRunner {
public:
    SequenceRunner() = default;
    ~SequenceRunner() override = default;
    explicit SequenceRunner(Priority priority, const std::string& name = "", bool isGlobal = false)
        : BaseRunner(name, isGlobal), priority_(priority) {}
    static napi_value SeqRunnerConstructor(napi_env env, napi_callback_info cbinfo);
    static napi_value Execute(napi_env env, napi_callback_info cbinfo);
    void AddTask(Task* task);
    void TriggerTask(napi_env env);

protected:
    BaseRunnerManager& GetManager() override;
    void LogRunnerConstructor(std::string name, uint64_t runnerId) override;
    void LogRunnerConstructorInnerReturn(napi_status status) override;

private:
    SequenceRunner(const SequenceRunner &) = delete;
    SequenceRunner& operator=(const SequenceRunner &) = delete;
    SequenceRunner(SequenceRunner &&) = delete;
    SequenceRunner& operator=(SequenceRunner &&) = delete;

    static void ExecuteTaskImmediately(uint32_t taskId, Priority priority);
    static bool SeqRunnerConstructorInner(napi_env env, napi_value& thisVar, SequenceRunner* seqRunner);
    bool UpdateCurrentTaskId(uint32_t taskId);

    friend class NativeEngineTest;
public:
    std::atomic<uint64_t> currentTaskId_ {};
    Priority priority_ {Priority::DEFAULT};
};
} // namespace Commonlibrary::Concurrent::TaskPoolModule
#endif // JS_CONCURRENT_MODULE_TASKPOOL_RUNNER_H