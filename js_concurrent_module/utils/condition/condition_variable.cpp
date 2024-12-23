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
#include "condition_variable.h"
#include <uv.h>
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "helper/error_helper.h"
#include "tools/log.h"

namespace Commonlibrary::Concurrent::Condition {
static constexpr int ARGCONE = 1;
static thread_local napi_ref conditionClassRef = nullptr;

void ConditionVariable::EnvCleanupHook(void* arg)
{
    auto* workerData = static_cast<WorkerData*>(arg);
    workerData->cond->CleanupHookEnvPromise(workerData->pi, workerData->cond);
    delete workerData;
}

void ConditionVariable::CleanupHookEnvPromise(std::shared_ptr<PromiseInfo> pi, ConditionVariable *cond)
{
    napi_value undefined;
    napi_get_undefined(pi->env, &undefined);
    napi_reject_deferred(pi->env, pi->deferred, undefined);
    {
        std::lock_guard<std::mutex> lock(cond->queueMtx_);
        auto it = std::find_if(cond->promiseQueue_.begin(), cond->promiseQueue_.end(),
            [pi](const std::shared_ptr<PromiseInfo>& p) { return p.get() == pi.get(); });
        if (it != cond->promiseQueue_.end()) {
            cond->promiseQueue_.erase(it);
            --(cond->refCount_);
        }
        napi_release_threadsafe_function(pi->tsfn_, napi_tsfn_abort);
    }
    if (pi->timer != nullptr) {
        uv_timer_stop(pi->timer);
        uv_close(reinterpret_cast<uv_handle_t*>(pi->timer), [](uv_handle_t* handle) {
            delete handle;
        });
        delete static_cast<WorkerData*>(pi->timer->data);
        pi->timer = nullptr;
    }
}

void ConditionVariable::AddEnvCleanupHook(std::shared_ptr<PromiseInfo> pi)
{
    auto* workerData = new WorkerData{pi, this};
    napi_add_env_cleanup_hook(pi->env, EnvCleanupHook, static_cast<void*>(workerData));
}

void ConditionVariable::RemoveEnvCleanupHook(std::shared_ptr<PromiseInfo> pi)
{
    auto* workerData = new WorkerData{pi, this};
    napi_remove_env_cleanup_hook(pi->env, EnvCleanupHook, static_cast<void*>(workerData));
    delete workerData;
}

napi_value ConditionVariable::Init(napi_env env, napi_value exports)
{
    napi_value locks;
    bool hasLocks = false;
    napi_has_named_property(env, exports, "locks", &hasLocks);
    if (hasLocks) {
        napi_get_named_property(env, exports, "locks", &locks);
    } else {
        napi_create_object(env, &locks);
        napi_set_named_property(env, exports, "locks", locks);
    }

    napi_property_descriptor props[] = {
        DECLARE_NAPI_FUNCTION("wait", Wait),
        DECLARE_NAPI_FUNCTION("waitFor", WaitFor),
        DECLARE_NAPI_FUNCTION("notify", Notify),
        DECLARE_NAPI_FUNCTION("notifyOne", NotifyOne),
    };

    napi_value ConditionVariableClass = nullptr;
    napi_define_sendable_class(env, "ConditionVariable", NAPI_AUTO_LENGTH, Constructor, nullptr,
                               sizeof(props) / sizeof(props[0]), props, nullptr, &ConditionVariableClass);
    napi_create_reference(env, ConditionVariableClass, 1, &conditionClassRef);
    napi_set_named_property(env, exports, "ConditionVariable", ConditionVariableClass);

    napi_set_named_property(env, locks, "ConditionVariable", ConditionVariableClass);

    return exports;
}

napi_value ConditionVariable::Wait(napi_env env, napi_callback_info cbinfo)
{
    napi_value thisVar;
    ConditionVariable* cond;
    napi_get_cb_info(env, cbinfo, nullptr, nullptr, &thisVar, nullptr);
    napi_unwrap_sendable(env, thisVar, reinterpret_cast<void **>(&cond));
    napi_deferred deferred;
    napi_value promise;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
    if (promise == nullptr) {
        Common::Helper::ErrorHelper::ThrowError(env, Common::Helper::ErrorHelper::TYPE_ERROR,
            "Failed to create promise in Wait method: ");
        return nullptr;
    }

    auto tsfn = cond->CreateThreadSafeFunction(env);
    {
        std::lock_guard<std::mutex> lock(cond->queueMtx_);
        auto promiseInfo = std::make_shared<PromiseInfo>(env, deferred, SettleBy::NOTIFY, nullptr, tsfn);
        cond->promiseQueue_.emplace_back(promiseInfo);
        cond->AddEnvCleanupHook(promiseInfo);
        ++(cond->refCount_);
    }

    return promise;
}

napi_value ConditionVariable::WaitFor(napi_env env, napi_callback_info cbinfo)
{
    napi_value thisVar;
    size_t argc = ARGCONE;
    napi_value args;
    napi_get_cb_info(env, cbinfo, &argc, &args, &thisVar, nullptr);

    if (argc != ARGCONE) {
        Common::Helper::ErrorHelper::ThrowError(env, Common::Helper::ErrorHelper::TYPE_ERROR,
            "First argument must be a number");
        return nullptr;
    }

    double milliseconds;
    napi_get_value_double(env, args, &milliseconds);
    ConditionVariable* cond;
    napi_unwrap_sendable(env, thisVar, reinterpret_cast<void **>(&cond));
    napi_deferred deferred;
    napi_value promise;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
    if (promise == nullptr) {
        Common::Helper::ErrorHelper::ThrowError(env, Common::Helper::ErrorHelper::TYPE_ERROR,
            "Invalid argument type");
        return nullptr;
    }
    uv_timer_t* timer = new uv_timer_t;
    uv_timer_init(Common::Helper::NapiHelper::GetLibUV(env), timer);
   
    auto tsfn = cond->CreateThreadSafeFunction(env);
    auto pi = std::make_shared<PromiseInfo>(env, deferred, SettleBy::WAITTIME, timer, tsfn);
    {
        std::lock_guard<std::mutex> lock(cond->queueMtx_);
        cond->promiseQueue_.push_back(pi);
        cond->AddEnvCleanupHook(pi);
        ++(cond->refCount_);
    }
    if (!cond->TimerTask(pi, cond, timer, static_cast<uint64_t>(milliseconds))) {
        Common::Helper::ErrorHelper::ThrowError(env, Common::Helper::ErrorHelper::TYPE_ERROR,
            "Failed to start timer");
        if (timer) {
            delete timer;
            timer = nullptr;
        }
        return nullptr;
    }

    return promise;
}

bool ConditionVariable::TimerTask(std::shared_ptr<PromiseInfo> pi, ConditionVariable *cond, uv_timer_t* timer,
    uint64_t milliseconds)
{
    WorkerData* workeData = new WorkerData{pi, cond};
    timer->data = workeData;
    int startStatus = uv_timer_start(timer, [](uv_timer_t* handle) {
        WorkerData* workeData = static_cast<WorkerData*>(handle->data);
        std::shared_ptr<PromiseInfo> pi = workeData->pi;
        pi->resolved = SettleBy::WAITTIME;
        ConditionVariable* cond = workeData->cond;
        uv_timer_stop(handle);
        {
            std::lock_guard<std::mutex> lock(cond->queueMtx_);
            auto it = std::find_if(cond->promiseQueue_.begin(), cond->promiseQueue_.end(),
                [pi](const std::shared_ptr<PromiseInfo>& p) { return p.get() == pi.get(); });
            if (it != cond->promiseQueue_.end()) {
                ScheduleAsyncWork(pi, cond);
                cond->promiseQueue_.erase(it);
                --(cond->refCount_);
            }
        }
        uv_close(reinterpret_cast<uv_handle_t*>(handle), [](uv_handle_t* handle) {
            WorkerData* workeData = static_cast<WorkerData*>(handle->data);
            if (workeData->pi->timer) {
                delete workeData->pi->timer;
                workeData->pi->timer = nullptr;
            }
            if (workeData) {
                delete workeData;
                workeData = nullptr;
            }
        });
    }, milliseconds, 0);
    if (startStatus != 0) {
        delete static_cast<WorkerData*>(timer->data);
        return false;
    }
    return true;
}

napi_value ConditionVariable::Notify(napi_env env, napi_callback_info cbinfo)
{
    napi_value thisVar;
    napi_get_cb_info(env, cbinfo, nullptr, nullptr, &thisVar, nullptr);
    ConditionVariable* cond;
    napi_unwrap_sendable(env, thisVar, reinterpret_cast<void **>(&cond));
    if (cond->promiseQueue_.empty()) {
        Common::Helper::ErrorHelper::ThrowError(env, Common::Helper::ErrorHelper::TYPE_ERROR,
            "There is no notice to wait for");
    } else {
        cond->PromiseProcessFlow(cond, SettleBy::NOTIFY, true);
    }

    return nullptr;
}

napi_value ConditionVariable::NotifyOne(napi_env env, napi_callback_info cbinfo)
{
    napi_value thisVar;
    napi_get_cb_info(env, cbinfo, nullptr, nullptr, &thisVar, nullptr);
    ConditionVariable* cond;
    napi_unwrap_sendable(env, thisVar, reinterpret_cast<void **>(&cond));
    if (cond->promiseQueue_.empty()) {
        Common::Helper::ErrorHelper::ThrowError(env, Common::Helper::ErrorHelper::TYPE_ERROR,
            "There is no notice to wait for");
    } else {
        cond->PromiseProcessFlow(cond, SettleBy::NOTIFY, false);
    }
    return nullptr;
}

napi_threadsafe_function ConditionVariable::CreateThreadSafeFunction(napi_env env)
{
    napi_threadsafe_function tsfn;
    napi_value asyncName;
    napi_create_string_utf8(env, "ConditionSafeFunc", NAPI_AUTO_LENGTH, &asyncName);
    napi_create_threadsafe_function(
        env,
        nullptr,
        nullptr,
        asyncName,
        0,
        1,
        nullptr,
        nullptr,
        this,
        CallJsCallback,
        &tsfn);
    if (tsfn == nullptr) {
        HILOG_FATAL("Create thread safe function failed");
    }
    return tsfn;
}

void ConditionVariable::CallJsCallback(napi_env env, napi_value js_callback, void* context, void* data)
{
    ConditionVariable* cond = reinterpret_cast<ConditionVariable*>(context);
    std::shared_ptr<PromiseInfo> pi(static_cast<PromiseInfo*>(data));

    ResolveDeferred(pi, pi->tsfn_);
    if (cond->GetRefCount() == 0) {
        delete cond;
    }
}

napi_value ConditionVariable::Constructor(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = 0;
    napi_value* argv = nullptr;

    napi_value thisVar;
    napi_get_cb_info(env, cbinfo, &argc, argv, &thisVar, nullptr);
    ConditionVariable *cond = new ConditionVariable();
    {
        std::lock_guard<std::mutex> lock(cond->queueMtx_);
        ++(cond->refCount_);
    }
    napi_wrap_sendable(env, thisVar, reinterpret_cast<void *>(cond), ConditionVariable::Destructor, nullptr);
    return thisVar;
}

void ConditionVariable::ResolveDeferred(std::shared_ptr<PromiseInfo> pi, napi_threadsafe_function tsfn)
{
    napi_handle_scope scope;
    napi_open_handle_scope(pi->env, &scope);
    napi_value result_val;
    switch (pi->resolved) {
        case SettleBy::WAITTIME:
            napi_get_boolean(pi->env, true, &result_val);
            napi_create_int32(pi->env, static_cast<int32_t>(SettleBy::WAITTIME), &result_val);
            napi_resolve_deferred(pi->env, pi->deferred, result_val);
            break;
        case SettleBy::NOTIFY:
            napi_get_boolean(pi->env, true, &result_val);
            napi_create_int32(pi->env, static_cast<int32_t>(SettleBy::NOTIFY), &result_val);
            napi_resolve_deferred(pi->env, pi->deferred, result_val);
            break;
        case SettleBy::CLEANCV:
            std::string restultStr = "ConditionVariable is being deleted, clean all wait by reject";
            napi_create_string_utf8(pi->env, restultStr.c_str(), restultStr.size(), &result_val);
            napi_reject_deferred(pi->env, pi->deferred, result_val);
            break;
    }
    napi_close_handle_scope(pi->env, scope);

    napi_release_threadsafe_function(tsfn, napi_tsfn_release);
}

void ConditionVariable::ScheduleAsyncWork(std::shared_ptr<PromiseInfo> promiseInfo, ConditionVariable* cond)
{
    PromiseInfo* workData = new PromiseInfo(promiseInfo->env, promiseInfo->deferred, promiseInfo->resolved,
        promiseInfo->timer, promiseInfo->tsfn_);

    NAPI_CALL_RETURN_VOID(promiseInfo->env, napi_call_threadsafe_function(workData->tsfn_, workData,
        napi_tsfn_blocking));
}

int32_t ConditionVariable::GetRefCount()
{
    return refCount_;
}

void ConditionVariable::Destructor(napi_env env, void *nativeObject, [[maybe_unused]] void *finalizeHint)
{
    auto cond = reinterpret_cast<ConditionVariable *>(nativeObject);
    {
        std::lock_guard<std::mutex> lock(cond->queueMtx_);
        --(cond->refCount_);
    }
    if (cond->GetRefCount() == 0) {
        delete cond;
    } else {
        cond->PromiseProcessFlow(cond, SettleBy::CLEANCV, true);
    }
}

void ConditionVariable::PromiseProcessFlow(ConditionVariable *cond, SettleBy settleBy, bool notifyAll)
{
    std::lock_guard<std::mutex> lock(cond->queueMtx_);
    while (!cond->promiseQueue_.empty()) {
        auto pi = cond->promiseQueue_.front();
        pi->resolved = settleBy;
        ScheduleAsyncWork(pi, cond);
        cond->promiseQueue_.pop_front();
        cond->RemoveEnvCleanupHook(pi);

        if (pi->timer != nullptr) {
            uv_timer_stop(pi->timer);
            uv_close(reinterpret_cast<uv_handle_t*>(pi->timer), [](uv_handle_t* handle) {
                delete handle;
            });
            delete static_cast<WorkerData*>(pi->timer->data);
            pi->timer = nullptr;
        }
        --(cond->refCount_);

        if (!notifyAll) {
            break;
        }
    }
}
}  // namespace Commonlibrary::Concurrent::Condition