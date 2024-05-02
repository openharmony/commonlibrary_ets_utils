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

#ifndef JS_CONCURRENT_MODULE_UTILS_LOCKS_ASYNC_LOCK_H
#define JS_CONCURRENT_MODULE_UTILS_LOCKS_ASYNC_LOCK_H

#include <list>
#include <string>
#include "common.h"
#include "lock_request.h"
#include "helper/error_helper.h"
#include "helper/napi_helper.h"
#include "helper/object_helper.h"

namespace Commonlibrary::Concurrent::LocksModule {

struct RequestCreationInfo {
    tid_t tid;
    std::string creationStacktrace;
};

class AsyncLock {
public:
    explicit AsyncLock(const std::string &lockName);
    explicit AsyncLock(uint32_t lockId);
    ~AsyncLock() = default;

    napi_value LockAsync(napi_env env, napi_ref cb, LockMode mode, const LockOptions &options);
    void CleanUpLockRequestOnCompletion(LockRequest* lockRequest);
    bool CleanUpLockRequestOnTimeout(LockRequest* lockRequest);
    napi_status FillLockState(napi_env env, napi_value held, napi_value pending);

    uint32_t IncRefCount();
    uint32_t DecRefCount();

    std::vector<RequestCreationInfo> GetSatisfiedRequestInfos();
    std::vector<RequestCreationInfo> GetPendingRequestInfos();

private:
    void ProcessPendingLockRequest();
    bool CanAcquireLock(LockRequest *lockRequest);
    napi_value CreateLockInfo(napi_env env, const LockRequest *rq);
    static void Destructor(napi_env env, void *data, [[maybe_unused]] void *hint);

    std::list<LockRequest *> pendingList_ {};
    std::list<LockRequest *> heldList_ {};
    LockMode lockStatus_ = LOCK_MODE_UNLOCK;
    std::string lockName_ = "";    // "" for anonymous lock
    uint32_t anonymousLockId_ {};  // 0 for Non-anonymous lock
    uv_work_t work_ = {};
    std::shared_mutex asyncLockMutex_;
    uint32_t refCount_ = 1;
};

}  // namespace Commonlibrary::Concurrent::LocksModule
#endif  // JS_CONCURRENT_MODULE_UTILS_LOCKS_ASYNC_LOCK_H
