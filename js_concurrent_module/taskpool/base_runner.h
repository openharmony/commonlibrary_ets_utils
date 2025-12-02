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

#ifndef JS_CONCURRENT_MODULE_TASKPOOL_BASE_RUNNER_H
#define JS_CONCURRENT_MODULE_TASKPOOL_BASE_RUNNER_H

#include <mutex>
#include <deque>
#include <unordered_map>

#include "task.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
class BaseRunnerManager;
class BaseRunner {
public:
    explicit BaseRunner(const std::string& name = "", bool isGlobal = false)
        : name_(name), isGlobalRunner_(isGlobal) {}
    virtual ~BaseRunner();
    virtual BaseRunnerManager& GetManager() = 0;
    uint64_t DecreaseCount();
    void IncreaseCount();
    bool RemoveWaitingTask(Task* task);
    static bool RunnerConstructorInner(napi_env env, napi_value& thisVar, BaseRunner* baseRunner,
                                       napi_property_descriptor* properties, size_t propertyCount);
    static void RunnerDestructor(napi_env env, void* data, void* hint);

protected:
    virtual void LogRunnerConstructor(std::string name, uint64_t runnerId) = 0;
    virtual void LogRunnerConstructorInnerReturn(napi_status status) = 0;

private:
    BaseRunner(const BaseRunner &) = delete;
    BaseRunner& operator=(const BaseRunner &) = delete;
    BaseRunner(BaseRunner &&) = delete;
    BaseRunner& operator=(BaseRunner &&) = delete;

    friend class NativeEngineTest;
public:
    uint64_t runnerId_;
    std::string name_ {};
    std::atomic<bool> isGlobalRunner_ {false};
    std::atomic<uint64_t> refCount_{1};
    std::deque<Task*> tasks_ {};
    std::shared_mutex taskMutex_;
};
} // namespace Commonlibrary::Concurrent::TaskPoolModule

#endif // JS_CONCURRENT_MODULE_TASKPOOL_BASE_RUNNER_H