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

#include "async_lock.h"
#include "helper/error_helper.h"
#include "helper/napi_helper.h"
#include "helper/object_helper.h"
#include "utils/log.h"

namespace Commonlibrary::Concurrent::LocksModule {
using namespace Commonlibrary::Concurrent::Common::Helper;

LockRequest::LockRequest(AsyncLock* lock, int tid, napi_env env, napi_ref cb, LockMode mode, const LockOptions &options,
    napi_deferred deferred)
    :lock_(lock), tid_(tid), env_(env), callback_(cb), mode_(mode), options_(options), deferred_(deferred),
    work_(nullptr)
{
    napi_value resourceName;
    napi_create_string_utf8(env, "AsyncLock::AsyncCallback", NAPI_AUTO_LENGTH, &resourceName);
    napi_status status = napi_create_async_work(env_, nullptr, resourceName, [] (napi_env, void *) {},
        AsyncAfterWorkCallback, this, &work_);
    if (status != napi_ok) {
        HILOG_FATAL("Internal error: cannot create async work");
    }
}

void LockRequest::AsyncAfterWorkCallback(napi_env env, [[maybe_unused]] napi_status status, void *data)
{
    LockRequest* lockRequest = reinterpret_cast<LockRequest *>(data);
    lockRequest->CallCallback();
    napi_delete_async_work(env, lockRequest->work_);
}

napi_value LockRequest::FinallyCallback(napi_env env, napi_callback_info info)
{
    LockRequest *lockRequest = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, nullptr, reinterpret_cast<void **>(&lockRequest)));
    lockRequest->lock_->CleanUpLockRequest(lockRequest);

    napi_value undefined;
    napi_get_undefined(env, &undefined);
    return undefined;
}

void LockRequest::CallCallbackAsync()
{
    napi_status status = napi_queue_async_work(env_, work_);
    if (status != napi_ok) {
        HILOG_FATAL("Internal error: cannot queue async work");
    }
}

void LockRequest::CallCallback()
{
    if (AbortIfNeeded()) {
        napi_delete_reference(env_, callback_);
        lock_->CleanUpLockRequest(this);
        return;
    }
    napi_value cb;
    napi_get_reference_value(env_, callback_, &cb);
    napi_value result;
    napi_status status = napi_call_function(env_, nullptr, cb, 0, nullptr, &result);
    napi_delete_reference(env_, callback_);
    if (status == napi_ok) {
        napi_resolve_deferred(env_, deferred_, result);

        bool isPromise = false;
        napi_is_promise(env_, result, &isPromise);
        if (isPromise) {
            napi_value finallyFn;
            napi_get_named_property(env_, result, "finally", &finallyFn);
            napi_value finallyCallback;
            napi_create_function(env_, nullptr, 0, FinallyCallback, this, &finallyCallback);
            napi_value finallyPromise;
            napi_call_function(env_, result, finallyFn, 1, &finallyCallback, &finallyPromise);
            return;
        }
    } else {
        napi_value err;
        napi_get_and_clear_last_exception(env_, &err);
        napi_reject_deferred(env_, deferred_, err);
    }
    lock_->CleanUpLockRequest(this);
}

bool LockRequest::AbortIfNeeded()
{
    if (options_.signal == nullptr) {
        return false;
    }
    napi_value signal;
    napi_get_reference_value(env_, options_.signal, &signal);
    napi_value aborted = NapiHelper::GetNameProperty(env_, signal, "aborted");
    bool isAborted = false;
    napi_get_value_bool(env_, aborted, &isAborted);
    if (!isAborted) {
        return false;
    }
    napi_value reason = NapiHelper::GetNameProperty(env_, signal, "reason");
    napi_reject_deferred(env_, deferred_, reason);
    return true;
}
} // namespace Commonlibrary::Concurrent::LocksModule
