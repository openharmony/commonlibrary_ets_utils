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

#include <uv.h>
#include <unistd.h>
#include <sys/syscall.h>
#include "async_lock.h"
#include "helper/error_helper.h"
#include "helper/napi_helper.h"
#include "helper/object_helper.h"
#include "utils/log.h"

namespace Commonlibrary::Concurrent::LocksModule {
using namespace Commonlibrary::Concurrent::Common::Helper;

AsyncLock::AsyncLock(const std::string &lockName)
{
    lockName_ = lockName;
    anonymousLockId_ = 0;
}

AsyncLock::AsyncLock(uint32_t lockId)
{
    lockName_ = "";
    anonymousLockId_ = lockId;
}

napi_value AsyncLock::LockAsync(napi_env env, napi_ref cb, LockMode mode, const LockOptions &options)
{
    napi_value promise;
    napi_deferred deferred;
    napi_create_promise(env, &deferred, &promise);
    LockRequest* lockRequest = new LockRequest(this, GetCurrentTid(), env, cb, mode, options, deferred);
    std::unique_lock<std::shared_mutex> lock(asyncLockMutex_);
    if (!CanAcquireLock(lockRequest) && options.isAvailable) {
        napi_value err;
        NAPI_CALL(env, napi_create_string_utf8(env, "The lock is acquired", NAPI_AUTO_LENGTH, &err));
        napi_reject_deferred(env, deferred, err);
    } else {
        pendingList_.push_back(lockRequest);
        ProcessPendingLockRequest();
    }
    return promise;
}

void AsyncLock::CleanUpLockRequest(LockRequest* lockRequest)
{
    std::unique_lock<std::shared_mutex> lock(asyncLockMutex_);
    auto it = std::find(heldList_.begin(), heldList_.end(), lockRequest);
    if (it == heldList_.end()) {
        HILOG_FATAL("Lock is not found");
        return;
    }
    heldList_.erase(it);
    lockStatus_ = LOCK_MODE_UNLOCK;
    delete lockRequest;
    ProcessPendingLockRequest();
}

void AsyncLock::ProcessPendingLockRequest()
{
    if (pendingList_.empty()) {
        return;
    }
    LockRequest* lockRequest = pendingList_.front();
    if (!CanAcquireLock(lockRequest)) {
        return;
    }
    if (lockRequest->GetTid() == GetCurrentTid() && lockRequest->GetMode() == LOCK_MODE_SHARED) {
        lockStatus_ = LOCK_MODE_SHARED;
        while (lockRequest->GetTid() == GetCurrentTid() && lockRequest->GetMode() == LOCK_MODE_SHARED) {
            heldList_.push_back(lockRequest);
            pendingList_.pop_front();
            asyncLockMutex_.unlock();
            lockRequest->CallCallback();
            asyncLockMutex_.lock();
            if (pendingList_.empty()) {
                break;
            }
            lockRequest = pendingList_.front();
        }
    } else {
        lockStatus_ = LOCK_MODE_EXCLUSIVE;
        heldList_.push_back(lockRequest);
        pendingList_.pop_front();
        lockRequest->CallCallbackAsync();
    }
}

bool AsyncLock::CanAcquireLock(LockRequest *lockRequest)
{
    if (heldList_.empty()) {
        return true;
    }
    if (lockRequest->GetMode() == LOCK_MODE_SHARED && lockStatus_ == LOCK_MODE_SHARED &&
            heldList_.front()->GetTid() == lockRequest->GetTid()) {
        return true;
    }
    if (lockStatus_ == LOCK_MODE_UNLOCK) {
        return true;
    }
    return false;
}

napi_status AsyncLock::FillLockState(napi_env env, napi_value held, napi_value pending)
{
    std::unique_lock<std::shared_mutex> lock(asyncLockMutex_);
    uint32_t idx = 0;
    for (LockRequest *rq : heldList_) {
        napi_value info = CreateLockInfo(env, rq);
        bool pendingException = false;
        napi_is_exception_pending(env, &pendingException);
        if (pendingException) {
            return napi_pending_exception;
        }
        napi_value index;
        napi_create_int32(env, idx, &index);
        napi_status status = napi_set_property(env, held, index, info);
        if (status != napi_ok) {
            return status;
        }
        ++idx;
    }
    idx = 0;
    for (LockRequest *rq : pendingList_) {
        napi_value info = CreateLockInfo(env, rq);
        bool pendingException = false;
        napi_is_exception_pending(env, &pendingException);
        if (pendingException) {
            return napi_pending_exception;
        }
        napi_value index;
        napi_create_int32(env, idx, &index);
        napi_status status = napi_set_property(env, pending, index, info);
        if (status != napi_ok) {
            return status;
        }
        ++idx;
    }
    return napi_ok;
}

napi_value AsyncLock::CreateLockInfo(napi_env env, const LockRequest *rq)
{
    napi_value info;
    NAPI_CALL(env, napi_create_object(env, &info));
    napi_value id;
    napi_value name;
    if (lockName_.empty()) {
        NAPI_CALL(env, napi_create_int32(env, anonymousLockId_, &id));
        NAPI_CALL(env, napi_get_undefined(env, &name));
    } else {
        NAPI_CALL(env, napi_get_undefined(env, &id));
        NAPI_CALL(env, napi_create_string_utf8(env, lockName_.c_str(), NAPI_AUTO_LENGTH, &name));
    }
    napi_value mode;
    NAPI_CALL(env, napi_create_int32(env, rq->GetMode(), &mode));
    napi_value tid;
    NAPI_CALL(env, napi_create_int32(env, rq->GetTid(), &tid));

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_PROPERTY("id", id),
        DECLARE_NAPI_PROPERTY("name", name),
        DECLARE_NAPI_PROPERTY("mode", mode),
        DECLARE_NAPI_PROPERTY("threadId", tid),
    };
    NAPI_CALL(env, napi_define_properties(env, info, sizeof(properties) / sizeof(properties[0]), properties));
    return info;
}

int AsyncLock::GetCurrentTid()
{
    return static_cast<int>(syscall(SYS_gettid));
}
} // namespace Commonlibrary::Concurrent::LocksModule
