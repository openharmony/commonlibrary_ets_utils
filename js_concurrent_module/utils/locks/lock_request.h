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

#ifndef JS_CONCURRENT_MODULE_UTILS_LOCKS_LOCK_REQUEST_H
#define JS_CONCURRENT_MODULE_UTILS_LOCKS_LOCK_REQUEST_H

#include <queue>
#include <string>
#include "common.h"
#include "helper/error_helper.h"
#include "helper/napi_helper.h"
#include "helper/object_helper.h"

namespace Commonlibrary::Concurrent::LocksModule {
enum LockMode {
    LOCK_MODE_UNLOCK,
    LOCK_MODE_SHARED,
    LOCK_MODE_EXCLUSIVE,
    LOCK_MODE_MAX
};

struct LockOptions {
    bool isAvailable = false;
    napi_ref signal = nullptr;
    uint32_t timeoutMillis = 0;
};

class AsyncLock;

class LockRequest {
public:
    LockRequest(AsyncLock* lock, tid_t tid, napi_env env, napi_ref cb, LockMode mode, const LockOptions &options,
        napi_deferred deferred);
    ~LockRequest();

    tid_t GetTid() const
    {
        return tid_;
    }

    std::string GetCreationStacktrace()
    {
        return creationStacktrace_;
    }

    LockMode GetMode() const
    {
        return mode_;
    }

    const LockOptions &GetOptions() const
    {
        return options_;
    }

    void CallCallbackAsync();
    void CallCallback();

    void OnQueued(uint32_t timeoutMillis);
    void OnSatisfied();
private:
    bool AbortIfNeeded();
    void ArmTimeoutTimer(uint32_t timeoutMillis);
    void DisarmTimeoutTimer();
    void HandleRequestTimeout(std::string &&errorMessage);
    static void AsyncAfterWorkCallback(napi_env env, napi_status status, void *data);
    static napi_value FinallyCallback(napi_env env, napi_callback_info info);
    static void TimeoutCallback(uv_timer_t *handle);
    static void DeallocateTimeoutTimerCallback(uv_handle_t* handle);

    AsyncLock* lock_;
    tid_t tid_;
    std::string creationStacktrace_;
    napi_env env_;
    napi_ref callback_;
    LockMode mode_;
    LockOptions options_;
    napi_deferred deferred_;
    napi_async_work work_;
    uv_timer_t *timeoutTimer_;
    bool timeoutActive_;
};

struct RequestTimeoutData {
    RequestTimeoutData(AsyncLock* l, LockRequest* r): lock(l), request(r) {}
    AsyncLock* lock;
    LockRequest* request;
};

}  // namespace Commonlibrary::Concurrent::LocksModule
#endif  // JS_CONCURRENT_MODULE_UTILS_LOCKS_LOCK_REQUEST_H
