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

#include "async_lock_manager.h"
#include "helper/hitrace_helper.h"
#include "js_concurrent_module/utils/locks/lock_request.h"
#include "js_concurrent_module/utils/locks/weak_wrap.h"
#include "tools/log.h"

namespace Commonlibrary::Concurrent::LocksModule {
using namespace Commonlibrary::Concurrent::Common::Helper;

LockRequest::LockRequest(AsyncLock *lock, tid_t tid, napi_env env, napi_ref cb, LockMode mode,
                         const LockOptions &options, napi_deferred deferred)
    : lock_(lock),
      tid_(tid),
      engine_(reinterpret_cast<NativeEngine *>(env)),
      env_(env),
      callback_(cb),
      mode_(mode),
      options_(options),
      deferred_(deferred),
      work_(nullptr),
      engineId_(engine_->GetId())
{
    // saving the creation point (file, function and line) for future use
    NativeEngine *engine = reinterpret_cast<NativeEngine *>(env);
    engine->BuildJsStackTrace(creationStacktrace_);

    AddEnvCleanupHook();
    InitTimer();

    napi_value resourceName;
    napi_create_string_utf8(env, "AsyncLock::AsyncCallback", NAPI_AUTO_LENGTH, &resourceName);
    napi_status status = napi_create_async_work(env_, nullptr, resourceName, AsyncLockManager::EmptyExecuteCallback,
                                                AsyncAfterWorkCallback, this, &work_);
    if (status != napi_ok) {
        HILOG_FATAL("Internal error: cannot create async work");
    }
}

void LockRequest::DeallocateTimeoutTimerCallback(uv_handle_t* handle)
{
    delete handle;
}

void LockRequest::AsyncAfterWorkCallback(napi_env env, [[maybe_unused]] napi_status status, void *data)
{
    LockRequest* lockRequest = reinterpret_cast<LockRequest *>(data);
    if (lockRequest->envIsInvalid_) {
        HILOG_ERROR("AsyncCallback is called after env cleaned up");
        lockRequest->Release();
        lockRequest->lock_->CleanUpLockRequestOnCompletion(lockRequest);
        return;
    }
    lockRequest->CallCallback();
}

napi_value LockRequest::FinallyCallback(napi_env env, napi_callback_info info)
{
    LockRequest *lockRequest = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, nullptr, reinterpret_cast<void **>(&lockRequest)));
    HITRACE_HELPER_METER_NAME("AsyncLock FinallyCallback, " + lockRequest->GetLockInfo());
    lockRequest->Release();
    lockRequest->lock_->CleanUpLockRequestOnCompletion(lockRequest);

    napi_value undefined;
    napi_get_undefined(env, &undefined);
    return undefined;
}

void LockRequest::CallCallbackAsync()
{
    if (!NativeEngine::IsAlive(engine_) || engineId_ != engine_->GetId()) {
        HILOG_ERROR("Callback is called after env cleaned up");
        lock_->CleanUpLockRequestOnCompletion(this);
        return;
    }
    napi_status status = napi_queue_async_work(env_, work_);
    if (status != napi_ok) {
        HILOG_FATAL("Internal error: cannot queue async work");
    }
}

void LockRequest::CallCallback()
{
    HITRACE_HELPER_METER_NAME("AsyncLock Callback, " + GetLockInfo());
    RemoveEnvCleanupHook();
    if (AbortIfNeeded()) {
        Release();
        lock_->CleanUpLockRequestOnCompletion(this);
        return;
    }
    napi_value cb = nullptr;
    NAPI_CALL_RETURN_VOID(env_, napi_get_reference_value(env_, callback_, &cb));
    napi_value result;
    napi_status status = napi_call_function(env_, nullptr, cb, 0, nullptr, &result);
    if (status == napi_ok) {
        napi_resolve_deferred(env_, deferred_, result);

        bool isPromise = false;
        napi_is_promise(env_, result, &isPromise);
        if (isPromise) {
            // Save lock_ and env_ into locals. If the callback returns fulfilled promise,
            // the lock request will be destroyed during napi_call_function(finallyFn).
            napi_env env = env_;
            // Increament reference counter for the lock. Do it to prevent lock destruction.
            napi_value finallyFn;
            napi_get_named_property(env, result, "finally", &finallyFn);
            napi_value finallyCallback;
            napi_create_function(env, nullptr, 0, FinallyCallback, this, &finallyCallback);
            napi_value finallyPromise;
            napi_call_function(env, result, finallyFn, 1, &finallyCallback, &finallyPromise);
            return;
        }
    } else {
        napi_value err;
        napi_get_and_clear_last_exception(env_, &err);
        napi_reject_deferred(env_, deferred_, err);
    }
    Release();
    lock_->CleanUpLockRequestOnCompletion(this);
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

void LockRequest::TimeoutCallback(uv_timer_t *handle)
{
    LockRequest *lockRequest = static_cast<LockRequest *>(handle->data);
    if (!lockRequest->lock_->CleanUpLockRequest(lockRequest)) {
        return;
    }
    lockRequest->RemoveEnvCleanupHook();

    // Check deadlocks and form the rejector value with or w/o the warning. It is required to be done
    // first in order to obtain the actual data.
    std::string error;
    AsyncLockManager::DumpLocksInfoForThread(AsyncLockManager::GetCurrentTid(lockRequest->env_), error);

    // NOTE: both AsyncLock and LockRequest might be deleted here, but at this point we imply that
    // AsyncLock exists, later on we we will handle the case when it does not

    // NOTE:
    // We might have the race with the lock acquirer function here and the request will be
    // already deleted if the race is won by the acquirer. So we should firstly make sure that
    // the race is won by us and then call the request's methods
    lockRequest->HandleRequestTimeout(std::move(error));

    AsyncLock *lock = lockRequest->lock_;
    napi_env env = lockRequest->env_;
    lockRequest->Release();
    delete lockRequest;
    lock->ProcessPendingLockRequest(env);
}

void LockRequest::HandleRequestTimeout(std::string &&errorMessage)
{
    HILOG_ERROR("AsyncLock lockAsync() timed out! Information: %s", errorMessage.c_str());
    // here we imply that there are no races already and the timeout function should do its job
    // by rejecting the associated promise with an BusinessError instance.

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

std::string LockRequest::GetLockInfo() const
{
    std::string strTrace;
    if (lock_->GetLockId() == 0) {
        strTrace += "lockName: " + lock_->GetLockName();
    } else {
        strTrace += "lockId: " + std::to_string(lock_->GetLockId());
    }
    return strTrace;
}

void LockRequest::EnvCleanup(void *arg)
{
    LockRequest *lockRequest = static_cast<LockRequest *>(arg);
    lockRequest->envIsInvalid_ = true;
    if (!lockRequest->lock_->CleanUpLockRequest(lockRequest)) {
        return;
    }

    AsyncLock *lock = lockRequest->lock_;
    napi_env env = lockRequest->env_;
    lockRequest->Release();
    delete lockRequest;
    lock->ProcessPendingLockRequest(env);
}

void LockRequest::InitTimer()
{
    if (options_.timeoutMillis <= 0) {
        return;
    }
    timeoutTimer_ = new uv_timer_t();
    uv_loop_t *loop = NapiHelper::GetLibUV(env_);

    int status = uv_timer_init(loop, timeoutTimer_);
    if (status != 0) {
        HILOG_FATAL("Internal error: unable to initialize the AsyncLock timeout timer %{public}d", status);
        return;
    }

    uv_update_time(loop);
    timeoutTimer_->data = this;
    status = uv_timer_start(timeoutTimer_, TimeoutCallback, options_.timeoutMillis, 0);
    if (status != 0) {
        HILOG_FATAL("Internal error: unable to start the AsyncLock timeout timer %{public}d", status);
    }
}

void LockRequest::StopTimer()
{
    if (timeoutTimer_ == nullptr) {
        return;
    }
    int status = uv_timer_stop(static_cast<uv_timer_t *>(timeoutTimer_));
    if (status != 0) {
        HILOG_FATAL("Internal error: unable to stop the AsyncLock timeout timer %{public}d", status);
    }
}

void LockRequest::CloseTimer()
{
    if (timeoutTimer_ == nullptr) {
        return;
    }
    uv_close(reinterpret_cast<uv_handle_t *>(timeoutTimer_), [](uv_handle_t *handle) { delete handle; });
    timeoutTimer_ = nullptr;
}

void LockRequest::AddEnvCleanupHook()
{
    NAPI_CALL_RETURN_VOID(env_, napi_add_env_cleanup_hook(env_, EnvCleanup, this));
}

void LockRequest::RemoveEnvCleanupHook()
{
    NAPI_CALL_RETURN_VOID(env_, napi_remove_env_cleanup_hook(env_, EnvCleanup, this));
}

void LockRequest::Release()
{
    CloseTimer();
    NAPI_CALL_RETURN_VOID(env_, napi_delete_reference(env_, callback_));
    callback_ = nullptr;
    NAPI_CALL_RETURN_VOID(env_, napi_delete_async_work(env_, work_));
    work_ = nullptr;
}

} // namespace Commonlibrary::Concurrent::LocksModule
