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
std::unordered_map<std::string, ConditionVariable*> ConditionVariable::condMap_;
std::mutex ConditionVariable::mapMtx_;

void ConditionVariable::EnvCleanupHook(void* arg)
{
    auto* workerData = static_cast<PromiseInfo*>(arg);
    workerData->cond->CleanupHookEnvPromise(workerData, workerData->cond);
}

void ConditionVariable::CleanupHookEnvPromise(PromiseInfo *pi, ConditionVariable *cond)
{
    {
        std::unique_lock<std::mutex> lock(cond->queueMtx_);
        auto it = std::find_if(cond->promiseQueue_.begin(), cond->promiseQueue_.end(),
            [pi](const std::shared_ptr<PromiseInfo>& p) { return p.get() == pi; });
        if (it != cond->promiseQueue_.end()) {
            cond->promiseQueue_.erase(it);
            --(cond->refCount_);
        } else {
            return;
        }
        napi_release_threadsafe_function(pi->tsfn_, napi_tsfn_abort);
    }
    if (pi->timer != nullptr) {
        uv_timer_stop(pi->timer);
        delete static_cast<WorkerData*>(pi->timer->data);
        uv_close(reinterpret_cast<uv_handle_t*>(pi->timer), [](uv_handle_t* handle) {
            delete handle;
        });
        pi->timer = nullptr;
    }
}

void ConditionVariable::AddEnvCleanupHook(std::shared_ptr<PromiseInfo> pi)
{
    napi_add_env_cleanup_hook(pi->env, EnvCleanupHook, static_cast<void*>(pi.get()));
}

void ConditionVariable::RemoveEnvCleanupHook(std::shared_ptr<PromiseInfo> pi)
{
    napi_remove_env_cleanup_hook(pi->env, EnvCleanupHook, static_cast<void*>(pi.get()));
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
        DECLARE_NAPI_INSTANCE_OBJECT_PROPERTY("wait"),
        DECLARE_NAPI_INSTANCE_OBJECT_PROPERTY("waitFor"),
        DECLARE_NAPI_INSTANCE_OBJECT_PROPERTY("notifyAll"),
        DECLARE_NAPI_INSTANCE_OBJECT_PROPERTY("notifyOne"),
        DECLARE_NAPI_STATIC_FUNCTION("request", Request),
    };

    napi_value ConditionVariableClass = nullptr;
    napi_define_sendable_class(env, "ConditionVariable", NAPI_AUTO_LENGTH, Constructor, nullptr,
                               sizeof(props) / sizeof(props[0]), props, nullptr, &ConditionVariableClass);
    napi_create_reference(env, ConditionVariableClass, 1, &conditionClassRef);
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

    auto tsfn = cond->CreateThreadSafeFunction(env);
    {
        std::unique_lock<std::mutex> lock(cond->queueMtx_);
        auto promiseInfo = std::make_shared<PromiseInfo>(cond, env, deferred, SettleBy::NOTIFY, nullptr, tsfn);
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
                                                "Invalid number of arguments");
        return nullptr;
    }

    int32_t milliseconds = 0;
    napi_status status = napi_get_value_int32(env, args, &milliseconds);
    if (status != napi_ok) {
        Common::Helper::ErrorHelper::ThrowError(env, Common::Helper::ErrorHelper::TYPE_ERROR,
                                                "Invalid argument type. Expected number");
        return nullptr;
    }
    if (milliseconds < 0) {
        milliseconds = 0;
    }
    ConditionVariable* cond;
    napi_unwrap_sendable(env, thisVar, reinterpret_cast<void **>(&cond));
    napi_deferred deferred;
    napi_value promise;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
    uv_timer_t* timer = new uv_timer_t;
    uv_timer_init(Common::Helper::NapiHelper::GetLibUV(env), timer);
   
    auto tsfn = cond->CreateThreadSafeFunction(env);
    auto pi = std::make_shared<PromiseInfo>(cond, env, deferred, SettleBy::WAITTIME, timer, tsfn);
    {
        std::unique_lock<std::mutex> lock(cond->queueMtx_);
        cond->promiseQueue_.push_back(pi);
        cond->AddEnvCleanupHook(pi);
        ++(cond->refCount_);
    }
    if (!cond->TimerTask(pi, cond, timer, static_cast<uint64_t>(milliseconds))) {
        HILOG_FATAL("Failed to start timer");
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
        {
            std::unique_lock<std::mutex> lock(cond->queueMtx_);
            auto it = std::find_if(cond->promiseQueue_.begin(), cond->promiseQueue_.end(),
                [pi](const std::shared_ptr<PromiseInfo>& p) { return p.get() == pi.get(); });
            if (it != cond->promiseQueue_.end()) {
                cond->promiseQueue_.erase(it);
                ScheduleAsyncWork(pi, cond);
                cond->RemoveEnvCleanupHook(pi);
            } else
                return;
        }
        uv_timer_stop(handle);
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
    if (!cond->promiseQueue_.empty()) {
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
    if (!cond->promiseQueue_.empty()) {
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
    PromiseInfo* pi = static_cast<PromiseInfo*>(data);

    ResolveDeferred(pi, pi->tsfn_);
    {
        cond->DecreaseRefCount();
        if (cond->GetRefCount() == 0) {
            if (cond->conditionName_.empty()) {
                delete cond;
            } else if (cond->CheckAndRemoveCondition()) {
                delete cond;
            }
        }
    }
}

napi_value ConditionVariable::Constructor(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = Common::Helper::NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    NAPI_ASSERT(env, argc == 0 || argc == 1, "AsyncLock::Constructor: the number of params must be zero or one");

    auto args = std::make_unique<napi_value[]>(argc);
    napi_value thisVar;
    napi_get_cb_info(env, cbinfo, &argc, args.get(), &thisVar, nullptr);
    ConditionVariable *cond = nullptr;

    if (argc == 1) {
        napi_valuetype type;
        NAPI_CALL(env, napi_typeof(env, args[0], &type));
        if (type != napi_string) {
            Common::Helper::ErrorHelper::ThrowError(env, Common::Helper::ErrorHelper::TYPE_ERROR,
                "Request:: param must be string");
            return nullptr;
        }

        std::string condName = Common::Helper::NapiHelper::GetString(env, args[0]);
        cond = FindOrCreateCondition(condName);
    } else {
        cond = new ConditionVariable();
        cond->IncreaseRefCount();
    }

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION_WITH_DATA("wait", Wait, thisVar),
        DECLARE_NAPI_FUNCTION_WITH_DATA("waitFor", WaitFor, thisVar),
        DECLARE_NAPI_FUNCTION_WITH_DATA("notifyAll", Notify, thisVar),
        DECLARE_NAPI_FUNCTION_WITH_DATA("notifyOne", NotifyOne, thisVar),
    };

    NAPI_CALL(env, napi_define_properties(env, thisVar, sizeof(properties) / sizeof(properties[0]), properties));
    napi_wrap_sendable(env, thisVar, reinterpret_cast<void *>(cond), ConditionVariable::Destructor, nullptr);
    return thisVar;
}

napi_value ConditionVariable::Request(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = Common::Helper::NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    NAPI_ASSERT(env, argc == 1, "Request:: the number of params must be one");
    auto args = std::make_unique<napi_value[]>(argc);
    NAPI_CALL(env, napi_get_cb_info(env, cbinfo, &argc, args.get(), nullptr, nullptr));
    napi_value conditionClass;
    NAPI_CALL(env, napi_get_reference_value(env, conditionClassRef, &conditionClass));
    napi_value instance;
    NAPI_CALL(env, napi_new_instance(env, conditionClass, argc, args.get(), &instance));

    return instance;
}

void ConditionVariable::ResolveDeferred(PromiseInfo* pi, napi_threadsafe_function tsfn)
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
    PromiseInfo* workData = new PromiseInfo(cond, promiseInfo->env, promiseInfo->deferred, promiseInfo->resolved,
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
    cond->DecreaseRefCount();
    if (cond->GetRefCount() == 0) {
        if (cond->conditionName_.empty()) {
            delete cond;
        } else if (cond->CheckAndRemoveCondition()) {
            delete cond;
        }
    } else if (cond->conditionName_.empty()) {
        cond->PromiseProcessFlow(cond, SettleBy::CLEANCV, true);
    }
}

void ConditionVariable::PromiseProcessFlow(ConditionVariable *cond, SettleBy settleBy, bool notifyAll)
{
    std::unique_lock<std::mutex> lock(cond->queueMtx_);
    while (!cond->promiseQueue_.empty()) {
        auto pi = cond->promiseQueue_.front();
        pi->resolved = settleBy;
        cond->promiseQueue_.pop_front();
        ScheduleAsyncWork(pi, cond);
        cond->RemoveEnvCleanupHook(pi);

        if (pi->timer != nullptr) {
            uv_timer_stop(pi->timer);
            delete static_cast<WorkerData*>(pi->timer->data);
            uv_close(reinterpret_cast<uv_handle_t*>(pi->timer), [](uv_handle_t* handle) {
                delete handle;
            });
            pi->timer = nullptr;
        }

        if (!notifyAll) {
            break;
        }
    }
}

void ConditionVariable::IncreaseRefCount()
{
    std::unique_lock<std::mutex> lock(queueMtx_);
    ++refCount_;
}

void ConditionVariable::DecreaseRefCount()
{
    std::unique_lock<std::mutex> lock(queueMtx_);
    --refCount_;
}

void ConditionVariable::SetConditionName(const std::string &condName)
{
    conditionName_ = condName;
}

std::string ConditionVariable::GetConditionName()
{
    return conditionName_;
}

ConditionVariable *ConditionVariable::FindOrCreateCondition(const std::string &condName)
{
    std::unique_lock<std::mutex> lock(mapMtx_);
    ConditionVariable *cond = nullptr;
    auto it = condMap_.find(condName);
    if (it == condMap_.end()) {
        cond = new ConditionVariable();
        cond->SetConditionName(condName);
        condMap_.emplace(condName, cond);
    } else {
        cond = it->second;
    }
    cond->IncreaseRefCount();
    return cond;
}

bool ConditionVariable::CheckAndRemoveCondition()
{
    std::unique_lock<std::mutex> lockMap(mapMtx_);
    std::unique_lock<std::mutex> lockQueue(queueMtx_);
    if (refCount_ == 0) {
        auto it = condMap_.find(conditionName_);
        if (it != condMap_.end()) {
            condMap_.erase(it);
            return true;
        }
    }

    return false;
}
}  // namespace Commonlibrary::Concurrent::Condition
