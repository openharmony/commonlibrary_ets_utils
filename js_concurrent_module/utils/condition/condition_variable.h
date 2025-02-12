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

#ifndef JS_CONCURRENT_MODULE_UTILS_CONDITION_VARIABLE_H
#define JS_CONCURRENT_MODULE_UTILS_CONDITION_VARIABLE_H

#include <cstdint>
#include <deque>
#include <mutex>
#include <string>
#include <unordered_map>
#include "condition_task.h"

namespace Commonlibrary::Concurrent::Condition {

class ConditionVariable {
public:
    ConditionVariable();
    explicit ConditionVariable(const std::string &name);

    static ConditionVariable *FindOrCreateCondition(const std::string &name);
    void TryRemoveCondition();

    void AddTask(napi_env env, napi_deferred deferred, uint64_t timeout = -1);
    void FinishTask(bool finishAll = false);
    void FindAndFinishTask(ConditionTask *task, ConditionTaskFinishReason reason);

private:
    static std::unordered_map<std::string, ConditionVariable *> condMap_;
    static std::mutex mapMtx_;

    uint32_t GetRefCount();
    void IncreaseRefCount();
    void DecreaseRefCount();

    std::string name_ {};
    std::deque<ConditionTask *> tasks_;
    uint32_t refCount_ {0};
    std::mutex mtx_;
};

}  // namespace Commonlibrary::Concurrent::Condition

#endif
