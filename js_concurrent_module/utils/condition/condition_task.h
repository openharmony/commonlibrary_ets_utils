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

#ifndef JS_CONCURRENT_MODULE_UTILS_CONDITION_TASK_H
#define JS_CONCURRENT_MODULE_UTILS_CONDITION_TASK_H

#include <cstdint>
#include "node_api.h"
#include "uv.h"

namespace Commonlibrary::Concurrent::Condition {

class ConditionVariable;

enum class ConditionTaskFinishReason { NOTIFY, TIMEOUT, ENV_CLEANUP };

class ConditionTask {
public:
    ConditionTask(ConditionVariable *cond, napi_env env, napi_deferred deferred, uint64_t timeout);
    ~ConditionTask();

    napi_env GetEnv();
    void Finish(ConditionTaskFinishReason reason);

private:
    static void EnvCleanup(void *arg);

    void ResolvePromise();
    void RejectPromise();

    void InitTimer(uint64_t timeout);
    void StopTimer();
    void CloseTimer();

    void AddEnvCleanupHook();
    void RemoveEnvCleanupHook();

    ConditionVariable *cond_;
    napi_env env_;
    napi_deferred deferred_;
    uv_timer_t *timer_ {nullptr};
};

}  // namespace Commonlibrary::Concurrent::Condition

#endif
