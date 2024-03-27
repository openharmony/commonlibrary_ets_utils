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
#include "async_lock.h"
#include "async_lock_manager.h"
#include "helper/error_helper.h"
#include "helper/napi_helper.h"
#include "helper/object_helper.h"
#include "tools/log.h"

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
    LockRequest *lockRequest =
        new LockRequest(this, AsyncLockManager::GetCurrentTid(), env, cb, mode, options, deferred);
    std::unique_lock<std::shared_mutex> lock(asyncLockMutex_);
    if (!CanAcquireLock(lockRequest) && options.isAvailable) {
        napi_value err;
        NAPI_CALL(env, napi_create_string_utf8(env, "The lock is acquired", NAPI_AUTO_LENGTH, &err));
        napi_reject_deferred(env, deferred, err);
    } else {
        lockRequest->OnQueued(options.timeoutMillis);
        pendingList_.push_back(lockRequest);
        ProcessPendingLockRequest();
    }
    return promise;
}

void AsyncLock::CleanUpLockRequestOnCompletion(LockRequest* lockRequest)
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

bool AsyncLock::CleanUpLockRequestOnTimeout(LockRequest* lockRequest)
{
    std::unique_lock<std::shared_mutex> lock(asyncLockMutex_);
    auto it = std::find(pendingList_.begin(), pendingList_.end(), lockRequest);
    if (it == pendingList_.end()) {
        // the lock got held while we were waiting on the mutex, no-op
        return false;
    }
    // we won the race, need to remove the request from the queue and handle the time out event
    pendingList_.erase(it);
    return true;
}

void AsyncLock::ProcessPendingLockRequest()
{
    if (pendingList_.empty()) {
        return;
    }
    LockRequest *lockRequest = pendingList_.front();
    if (!CanAcquireLock(lockRequest)) {
        return;
    }
    if (lockRequest->GetTid() == AsyncLockManager::GetCurrentTid() && lockRequest->GetMode() == LOCK_MODE_SHARED) {
        lockStatus_ = LOCK_MODE_SHARED;
        while (lockRequest->GetTid() == AsyncLockManager::GetCurrentTid() &&
               lockRequest->GetMode() == LOCK_MODE_SHARED) {
            lockRequest->OnSatisfied();
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
        lockRequest->OnSatisfied();
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
    napi_value name;
    NAPI_CALL(env, napi_create_string_utf8(env, lockName_.c_str(), NAPI_AUTO_LENGTH, &name));
    napi_value mode;
    NAPI_CALL(env, napi_create_int32(env, rq->GetMode(), &mode));
    napi_value tid;
    NAPI_CALL(env, napi_create_int32(env, rq->GetTid(), &tid));

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_PROPERTY("name", name),
        DECLARE_NAPI_PROPERTY("mode", mode),
        DECLARE_NAPI_PROPERTY("contextId", tid),
    };
    NAPI_CALL(env, napi_define_properties(env, info, sizeof(properties) / sizeof(properties[0]), properties));
    return info;
}

uint32_t AsyncLock::IncRefCount()
{
    std::unique_lock<std::shared_mutex> lock(asyncLockMutex_);
    return ++refCount_;
}

uint32_t AsyncLock::DecRefCount()
{
    std::unique_lock<std::shared_mutex> lock(asyncLockMutex_);
    return --refCount_;
}

std::vector<RequestCreationInfo> AsyncLock::GetSatisfiedRequestInfos()
{
    std::vector<RequestCreationInfo> result;
    std::unique_lock<std::shared_mutex> lock(asyncLockMutex_);
    for (auto *request : heldList_) {
        result.push_back(RequestCreationInfo { request->GetTid(), request->GetCreationStacktrace() });
    }
    return result;
}

std::vector<RequestCreationInfo> AsyncLock::GetPendingRequestInfos()
{
    std::vector<RequestCreationInfo> result;
    std::unique_lock<std::shared_mutex> lock(asyncLockMutex_);
    for (auto *request : pendingList_) {
        result.push_back(RequestCreationInfo { request->GetTid(), request->GetCreationStacktrace() });
    }
    return result;
}

} // namespace Commonlibrary::Concurrent::LocksModule
