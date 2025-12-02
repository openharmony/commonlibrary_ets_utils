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

#ifndef JS_CONCURRENT_MODULE_TASKPOOL_ASYNC_RUNNER_MANAGER_H
#define JS_CONCURRENT_MODULE_TASKPOOL_ASYNC_RUNNER_MANAGER_H

#include <mutex>
#include <unordered_map>

#include "async_runner.h"
#include "base_runner_manager.h"
#include "napi/native_api.h"
#include "task.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
using namespace Commonlibrary::Concurrent::Common::Helper;

class AsyncRunnerConfig {
public:
    AsyncRunnerConfig(uint32_t runningCapacity, uint32_t waitingCapacity)
        : runningCapacity_(runningCapacity), waitingCapacity_(waitingCapacity) {}
    uint32_t runningCapacity_ {};
    uint32_t waitingCapacity_ {};
};

class AsyncRunnerManager : public BaseRunnerManager {
public:
    static AsyncRunnerManager& GetInstance();
    AsyncRunner* GetRunner(uint64_t runnerId);
    AsyncRunner* CreateOrGetGlobalRunner(napi_env env, napi_value thisVar, const std::string& name,
                                         uint32_t runningCapacity, uint32_t waitingCapacity);
    bool TriggerAsyncRunner(napi_env env, Task* lastTask);
    void CancelAsyncRunnerTask(napi_env env, Task* task);
    void DecreaseRunningCount(uint64_t asyncRunnerId);

protected:
    bool CheckGlobalRunnerParams(napi_env env, BaseRunner *runner, void* config) override;
    BaseRunner* CreateGlobalRunner(const std::string& name, void* config) override;
    void LogRunnerNotExist() override;

private:
    AsyncRunnerManager() = default;
    ~AsyncRunnerManager() override = default;
    AsyncRunnerManager(const AsyncRunnerManager &) = delete;
    AsyncRunnerManager& operator=(const AsyncRunnerManager &) = delete;
    AsyncRunnerManager(AsyncRunnerManager &&) = delete;
    AsyncRunnerManager& operator=(AsyncRunnerManager &&) = delete;
};
} // namespace Commonlibrary::Concurrent::TaskPoolModule
#endif // JS_CONCURRENT_MODULE_TASKPOOL_ASYNC_RUNNER_MANAGER_H