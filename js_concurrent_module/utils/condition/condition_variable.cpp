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

#include <algorithm>
#include <cstdint>
#include <mutex>
#include <string>
#include "condition_variable.h"
#include "helper/napi_helper.h"

namespace Commonlibrary::Concurrent::Condition {

std::unordered_map<std::string, ConditionVariable *> ConditionVariable::condMap_;
std::mutex ConditionVariable::mapMtx_;

ConditionVariable::ConditionVariable()
{
    IncreaseRefCount();
}

ConditionVariable::ConditionVariable(const std::string &name) : name_(name)
{
    IncreaseRefCount();
}

ConditionVariable *ConditionVariable::FindOrCreateCondition(const std::string &condName)
{
    std::lock_guard<std::mutex> mapLock(mapMtx_);
    ConditionVariable *cond {nullptr};
    if (condMap_.find(condName) == condMap_.end()) {
        cond = new ConditionVariable(condName);
        condMap_.emplace(condName, cond);
    } else {
        cond = condMap_[condName];
        cond->IncreaseRefCount();
    }
    return cond;
}

void ConditionVariable::TryRemoveCondition()
{
    {
        std::lock_guard<std::mutex> lock(mtx_);
        --refCount_;
        if (refCount_ != 0) {
            return;
        }
        if (name_.empty()) {
            delete this;
            return;
        }
    }
    std::lock_guard<std::mutex> mapLock(mapMtx_);
    std::lock_guard<std::mutex> lock(mtx_);
    if (refCount_ != 0 || condMap_.find(name_) == condMap_.end()) {
        return;
    }
    condMap_.erase(name_);
    delete this;
}

void ConditionVariable::AddTask(napi_env env, napi_deferred deferred, uint64_t timeout)
{
    ConditionTask *task = new ConditionTask(this, env, deferred, timeout);
    std::lock_guard<std::mutex> lock(mtx_);
    tasks_.emplace_back(task);
    ++refCount_;
}

void ConditionVariable::FinishTask(bool finishAll)
{
    std::lock_guard<std::mutex> lock(mtx_);
    while (!tasks_.empty()) {
        ConditionTask *task = tasks_.front();
        tasks_.pop_front();
        napi_send_event(
            task->GetEnv(),
            [cond = this, task]() {
                {
                    std::lock_guard<std::mutex> lock(cond->mtx_);
                    task->Finish(ConditionTaskFinishReason::NOTIFY);
                }
                cond->TryRemoveCondition();
            },
            napi_eprio_immediate);
        if (!finishAll) {
            break;
        }
    }
}

void ConditionVariable::FindAndFinishTask(ConditionTask *task, ConditionTaskFinishReason reason)
{
    {
        std::lock_guard<std::mutex> lock(mtx_);
        auto it = std::find(tasks_.begin(), tasks_.end(), task);
        if (it == tasks_.end()) {
            return;
        }
        tasks_.erase(it);
        task->Finish(reason);
    }
    TryRemoveCondition();
}

uint32_t ConditionVariable::GetRefCount()
{
    std::lock_guard<std::mutex> lock(mtx_);
    return refCount_;
}

void ConditionVariable::IncreaseRefCount()
{
    std::lock_guard<std::mutex> lock(mtx_);
    ++refCount_;
}

void ConditionVariable::DecreaseRefCount()
{
    std::lock_guard<std::mutex> lock(mtx_);
    --refCount_;
}

}  // namespace Commonlibrary::Concurrent::Condition
