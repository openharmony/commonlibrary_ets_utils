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

#include "base_runner.h"
#include "base_runner_manager.h"
#include "task_manager.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
using namespace Commonlibrary::Concurrent::Common::Helper;
BaseRunner::~BaseRunner() = default;

uint64_t BaseRunner::DecreaseCount()
{
    if (refCount_ > 0) {
        refCount_--;
    }
    return refCount_;
}

void BaseRunner::IncreaseCount()
{
    refCount_++;
}

void BaseRunner::RunnerDestructor(napi_env env, void* data, [[maybe_unused]] void* hint)
{
    BaseRunner* baseRunner = static_cast<BaseRunner*>(data);
    if (env == nullptr || baseRunner == nullptr) {
        return;
    }
    auto runner = baseRunner->GetManager().GetRunner(baseRunner->runnerId_);
    if (runner == nullptr) {
        return;
    }
    baseRunner->GetManager().UnrefAndDestroyRunner(runner);
}

bool BaseRunner::RemoveWaitingTask(Task* task)
{
    std::unique_lock<std::shared_mutex> lock(taskMutex_);
    if (tasks_.empty()) {
        return false;
    }
    auto iter = std::find(tasks_.begin(), tasks_.end(), task);
    if (iter != tasks_.end()) {
        tasks_.erase(iter);
        return true;
    }
    return false;
}

bool BaseRunner::RunnerConstructorInner(napi_env env, napi_value& thisVar, BaseRunner* baseRunner,
                                        napi_property_descriptor* properties, size_t propertyCount)
{
    uint64_t runnerId = reinterpret_cast<uint64_t>(baseRunner);
    baseRunner->runnerId_ = runnerId;
    napi_value napiRunnerId = NapiHelper::CreateUint64(env, runnerId);
    baseRunner->GetManager().StoreRunner(runnerId, baseRunner);
    napi_define_properties(env, thisVar, propertyCount, properties);
    baseRunner->LogRunnerConstructor(baseRunner->name_, runnerId);
    napi_status status = napi_wrap(env, thisVar, baseRunner, RunnerDestructor, nullptr, nullptr);
    if (status != napi_ok) {
        baseRunner->LogRunnerConstructorInnerReturn(status);
        RunnerDestructor(env, baseRunner, nullptr);
        return false;
    }
    return true;
}
} // namespace Commonlibrary::Concurrent::TaskPoolModule