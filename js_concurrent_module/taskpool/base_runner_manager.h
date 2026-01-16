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

#ifndef JS_CONCURRENT_MODULE_TASKPOOL_BASE_RUNNER_MANAGER_H
#define JS_CONCURRENT_MODULE_TASKPOOL_BASE_RUNNER_MANAGER_H

#include "base_runner.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
class BaseRunnerManager {
public:
    BaseRunnerManager() = default;
    virtual ~BaseRunnerManager();
    BaseRunner* GetRunner(uint64_t runnerId);
    bool FindRunnerAndRef(uint64_t runnerId);
    void RemoveWaitingTask(Task* task);
    void StoreRunner(uint64_t runnerId, BaseRunner* runner);
    bool UnrefAndDestroyRunner(BaseRunner* runner);
    BaseRunner* CreateOrGetGlobalRunner(napi_env env, napi_value thisVar, const std::string& name,
                                        void* config);
    void RemoveGlobalRunner(const std::string& name);
    void RemoveRunner(uint64_t runnerId);

    // <<name1, runner>, <name2, runner>, ...>
    std::unordered_map<std::string, BaseRunner*> globalRunner_ {};
    std::mutex globalRunnerMutex_;
    // <runnerId, runner>
    std::unordered_map<uint64_t, BaseRunner*> runners_ {};
    std::mutex runnersMutex_;

protected:
    virtual bool CheckGlobalRunnerParams(napi_env env, BaseRunner* runner, void* config) = 0;
    virtual BaseRunner* CreateGlobalRunner(const std::string& name, void* config) = 0;
    virtual void LogRunnerNotExist() = 0;

private:
    BaseRunnerManager(const BaseRunnerManager &) = delete;
    BaseRunnerManager& operator=(const BaseRunnerManager &) = delete;
    BaseRunnerManager(BaseRunnerManager &&) = delete;
    BaseRunnerManager& operator=(BaseRunnerManager &&) = delete;
};
} // namespace Commonlibrary::Concurrent::TaskPoolModule

#endif // JS_CONCURRENT_MODULE_TASKPOOL_BASE_RUNNER_MANAGER_H