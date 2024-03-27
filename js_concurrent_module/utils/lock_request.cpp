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

LockRequest::LockRequest(AsyncLock *lock, tid_t tid, napi_env env, napi_ref cb, LockMode mode,
                         const LockOptions &options, napi_deferred deferred)
    : lock_(lock),
      tid_(tid),
      env_(env),
      callback_(cb),
      mode_(mode),
      options_(options),
      deferred_(deferred),
      work_(nullptr),
      timeoutActive_(false)
{
    // timeout timer initialization: just fill the data fields, do not arm it
    timeoutTimer_ = new uv_timer_t();
    uv_timer_init(NapiHelper::GetLibUV(env), timeoutTimer_);
    RequestTimeoutData *data = new RequestTimeoutData(lock, this);
    timeoutTimer_->data = data;

    // saving the creation point (file, function and line) for future use
    NativeEngine *engine = reinterpret_cast<NativeEngine *>(env);
    engine->BuildJsStackTrace(creationStacktrace_);

    napi_value resourceName;
    napi_create_string_utf8(env, "AsyncLock::AsyncCallback", NAPI_AUTO_LENGTH, &resourceName);
    napi_status status = napi_create_async_work(
        env_, nullptr, resourceName, [](napi_env, void *) {}, AsyncAfterWorkCallback, this, &work_);
    if (status != napi_ok) {
        HILOG_FATAL("Internal error: cannot create async work");
    }
}

void LockRequest::DeallocateTimeoutTimerCallback(uv_handle_t* handle)
{
    delete handle;
}

LockRequest::~LockRequest()
{
    RequestTimeoutData *data = static_cast<RequestTimeoutData*>(timeoutTimer_->data);
    delete data;
    uv_close(reinterpret_cast<uv_handle_t*>(timeoutTimer_), DeallocateTimeoutTimerCallback);
}

void LockRequest::AsyncAfterWorkCallback(napi_env env, [[maybe_unused]] napi_status status, void *data)
{
    LockRequest* lockRequest = reinterpret_cast<LockRequest *>(data);
    napi_delete_async_work(env, lockRequest->work_);
    lockRequest->CallCallback();
}

napi_value LockRequest::FinallyCallback(napi_env env, napi_callback_info info)
{
    LockRequest *lockRequest = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, nullptr, reinterpret_cast<void **>(&lockRequest)));
    lockRequest->lock_->CleanUpLockRequestOnCompletion(lockRequest);

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
        lock_->CleanUpLockRequestOnCompletion(this);
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
    lock_->CleanUpLockRequestOnCompletion(this);
}

void LockRequest::OnSatisfied()
{
    if (timeoutActive_) {
        DisarmTimeoutTimer();
    }
}

void LockRequest::OnQueued(uint32_t timeoutMillis)
{
    if (timeoutMillis > 0) {
        ArmTimeoutTimer(timeoutMillis);
    }
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

void LockRequest::ArmTimeoutTimer(uint32_t timeoutMillis)
{
    timeoutActive_ = true;
    uv_timer_start(timeoutTimer_, TimeoutCallback, timeoutMillis, 0);
    // NOTE: handle the abortsignal functionality in the future
    // NOTE: need to check call results for 0
}

void LockRequest::DisarmTimeoutTimer()
{
    uv_timer_stop(timeoutTimer_);
    timeoutActive_ = false;
}

void LockRequest::TimeoutCallback(uv_timer_t *handle)
{
    RequestTimeoutData *data = static_cast<RequestTimeoutData*>(handle->data);
    uv_timer_stop(handle);
    if (data == nullptr) {
        // fail! something terribly bad happened!
        HILOG_FATAL("Internal error: unable to handle the AsyncLock timeout");
    }
    // Check deadlocks and form the rejector value with or w/o the warning. It is required to be done
    // first in order to obtain the actual data.
    std::string error;
    AsyncLockManager::DumpLocksInfoForThread(AsyncLockManager::GetCurrentTid(), error);

    // NOTE: both AsyncLock and LockRequest might be deleted here, but at this point we imply that
    // AsyncLock exists, later on we we will handle the case when it does not

    // NOTE:
    // We might have the race with the lock acquirer function here and the request will be
    // already deleted if the race is won by the acquirer. So we should firstly make sure that
    // the race is won by us and then call the request's methods

    bool success = data->lock->CleanUpLockRequestOnTimeout(data->request);
    if (!success) {
        return;
    }
    data->request->HandleRequestTimeout(std::move(error));
    // will delete 'data' too
    delete data->request;
}

void LockRequest::HandleRequestTimeout(std::string &&errorMessage)
{
    HILOG_ERROR("AsyncLock lockAsync() timed out! Information: %s", errorMessage.c_str());
    // here we imply that there are no races already and the timeout function should do its job
    // by rejecting the associated promise with an BusinessError instance.
    napi_delete_reference(env_, callback_);

    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env_, &scope);
    if (scope == nullptr) {
        return;
    }

    HILOG_ERROR("AsyncLock lockAsync() timed out! Rejecting the promise.");
    napi_value error = ErrorHelper::NewError(env_, ErrorHelper::ERR_ASYNCLOCK_TIMEOUT, errorMessage.c_str());
    napi_reject_deferred(env_, deferred_, error);

    napi_close_handle_scope(env_, scope);
}

} // namespace Commonlibrary::Concurrent::LocksModule
