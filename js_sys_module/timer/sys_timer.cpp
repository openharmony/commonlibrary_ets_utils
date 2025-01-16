/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "sys_timer.h"

#include <cinttypes>

#ifdef ENABLE_HITRACE_HELPER_METER
#include "helper/hitrace_helper.h"
#endif
#include "native_engine/native_engine.h"
#include "tools/log.h"

#ifdef ENABLE_CONTAINER_SCOPE
using OHOS::Ace::ContainerScope;
#endif

namespace OHOS::JsSysModule {
using namespace Commonlibrary::Concurrent::Common;

uint32_t Timer::timeCallbackId = 0;
uint32_t deleteTimerCount = 0;
std::map<uint32_t, TimerCallbackInfo*> Timer::timerTable;
std::mutex Timer::timeLock;

TimerCallbackInfo::~TimerCallbackInfo()
{
#ifdef ENABLE_HITRACE_HELPER_METER
    HITRACE_HELPER_METER_NAME("~TimerCallbackInfo address = " + std::to_string(reinterpret_cast<std::uintptr_t>(this)));
#endif
    Helper::NapiHelper::DeleteReference(env_, callback_);
    for (size_t idx = 0; idx < argc_; idx++) {
        Helper::NapiHelper::DeleteReference(env_, argv_[idx]);
    }
    Helper::CloseHelp::DeletePointer(argv_, true);

    uv_timer_stop(timeReq_);
    uv_close(reinterpret_cast<uv_handle_t*>(timeReq_), [](uv_handle_t* handle) {
        if (handle != nullptr) {
            delete (uv_timer_t*)handle;
            handle = nullptr;
        }
    });
}

typedef napi_value (*SetTimeFunction)(napi_env env, napi_callback_info info, bool repeat);

napi_value Timer::SetTimeOutFaker(napi_env env, napi_callback_info cbinfo, bool repeat)
{
    std::lock_guard<std::mutex> lock(timeLock);
    uint32_t tId = timeCallbackId++;
    HILOG_WARN("Timer is deactivated on current JS Thread, timer id = %{public}" PRIu32, tId);
    return Helper::NapiHelper::CreateUint32(env, tId);
}

struct TimeData {
    napi_env env_;
    SetTimeFunction func_;
};

void Timer::CleanUpHook(void* data)
{
    auto that = reinterpret_cast<TimeData*>(data);
    Timer::ClearEnvironmentTimer(that->env_);
    that->func_ = Timer::SetTimeOutFaker;
    that->env_ = nullptr;
}

bool Timer::RegisterTime(napi_env env)
{
    if (env == nullptr) {
        return false;
    }
    thread_local auto data = new TimeData();
    data->env_ = env;
    data->func_ = SetTimeoutInner;
    napi_add_env_cleanup_hook(env, CleanUpHook, data);

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_DEFAULT_PROPERTY_FUNCTION_WITH_DATA("setTimeout", SetTimeout, data),
        DECLARE_NAPI_DEFAULT_PROPERTY_FUNCTION_WITH_DATA("setInterval", SetInterval, data),
        DECLARE_NAPI_DEFAULT_PROPERTY_FUNCTION("clearTimeout", ClearTimer),
        DECLARE_NAPI_DEFAULT_PROPERTY_FUNCTION("clearInterval", ClearTimer)
    };
    napi_value globalObj = Helper::NapiHelper::GetGlobalObject(env);
    napi_status status = napi_define_properties(env, globalObj, sizeof(properties) / sizeof(properties[0]), properties);
    return status == napi_ok;
}

napi_value Timer::SetTimeout(napi_env env, napi_callback_info cbinfo)
{
    void *data = nullptr;
    napi_get_cb_info(env, cbinfo, 0, nullptr, nullptr, &data);
    return reinterpret_cast<TimeData*>(data)->func_(env, cbinfo, false);
}

napi_value Timer::SetInterval(napi_env env, napi_callback_info cbinfo)
{
    void *data = nullptr;
    napi_get_cb_info(env, cbinfo, 0, nullptr, nullptr, &data);
    return reinterpret_cast<TimeData*>(data)->func_(env, cbinfo, true);
}

napi_value Timer::ClearTimer(napi_env env, napi_callback_info cbinfo)
{
    // 1. check args
    size_t argc = 1;
    napi_value argv[1];
    napi_get_cb_info(env, cbinfo, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        HILOG_ERROR("the number of params must be one");
        return nullptr;
    }

    uint32_t tId;
    napi_status status = napi_get_value_uint32(env, argv[0], &tId);
    if (status != napi_ok) {
        HILOG_DEBUG("first param should be number");
        return nullptr;
    }

    {
        std::lock_guard<std::mutex> lock(timeLock);
        auto iter = timerTable.find(tId);
        if (iter == timerTable.end()) {
            // timer already cleared
            return nullptr;
        }
        TimerCallbackInfo* callbackInfo = iter->second;
        if (callbackInfo->env_ != env) {
            HILOG_ERROR("Timer is deleting by another thread, please check js code. TimerID:%{public}u", tId);
        } else {
            timerTable.erase(tId);
            Helper::CloseHelp::DeletePointer(callbackInfo, false);
            HILOG_INFO("DeleteTimer ID: %{public}u, count: %{public}u", tId, ++deleteTimerCount);
#ifdef ENABLE_HITRACE_HELPER_METER
            HITRACE_HELPER_METER_NAME("DeleteTimer ID: " + std::to_string(tId) + ", count: "
                + std::to_string(deleteTimerCount));
#endif
        }
    }
    return Helper::NapiHelper::GetUndefinedValue(env);
}

void Timer::TimerCallback(uv_timer_t* handle)
{
    TimerCallbackInfo* callbackInfo = static_cast<TimerCallbackInfo*>(handle->data);
    if (callbackInfo == nullptr) {
        return;
    }
    // Save the following parameters to ensure that they can still obtained if callback clears the callbackInfo.
    bool repeat = callbackInfo->repeat_;
    uint32_t tId = callbackInfo->tId_;
    napi_env env = callbackInfo->env_;
#ifdef ENABLE_CONTAINER_SCOPE
    ContainerScope containerScope(callbackInfo->containerScopeId_);
#endif
#ifdef ENABLE_HITRACE_HELPER_METER
    HITRACE_HELPER_METER_NAME("TimerCallback callbackInfo address = "
        + std::to_string(reinterpret_cast<std::uintptr_t>(callbackInfo)) + ", tId = " + std::to_string(tId));
#endif
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env, &scope);
    if (scope == nullptr) {
        return;
    }
    napi_value callback = Helper::NapiHelper::GetReferenceValue(env, callbackInfo->callback_);
    napi_value undefinedValue = Helper::NapiHelper::GetUndefinedValue(env);
    napi_value callbackResult = nullptr;
    napi_value* callbackArgv = new napi_value[callbackInfo->argc_];
    for (size_t idx = 0; idx < callbackInfo->argc_; idx++) {
        callbackArgv[idx] = Helper::NapiHelper::GetReferenceValue(env, callbackInfo->argv_[idx]);
    }

    napi_call_function(env, undefinedValue, callback,
                       callbackInfo->argc_, callbackArgv, &callbackResult);
    Helper::CloseHelp::DeletePointer(callbackArgv, true);
    if (Helper::NapiHelper::IsExceptionPending(env)) {
        HILOG_ERROR("Pending exception in TimerCallback. Triggering HandleUncaughtException");
        reinterpret_cast<NativeEngine*>(env)->HandleUncaughtException();
        napi_close_handle_scope(env, scope);
        DeleteTimer(tId, callbackInfo);
        return;
    }

    // callback maybe contain ClearTimer, so we need check to avoid use-after-free and double-free of callbackInfo
    std::lock_guard<std::mutex> lock(timeLock);
    if (timerTable.find(tId) == timerTable.end()) {
        napi_close_handle_scope(env, scope);
        return;
    }
    if (!repeat) {
        timerTable.erase(tId);
        napi_close_handle_scope(env, scope);
        Helper::CloseHelp::DeletePointer(callbackInfo, false);
    } else {
        napi_close_handle_scope(env, scope);
        uv_timer_again(handle);
    }
}

napi_value Timer::SetTimeoutInnerCore(napi_env env, napi_value* argv, size_t argc, bool repeat)
{
    int32_t timeout = 0;
    if (argc > 1) {
        napi_status status = napi_get_value_int32(env, argv[1], &timeout);
        if (status != napi_ok) {
            HILOG_WARN("timeout should be number");
            timeout = 0;
        }
    }
    if (timeout < 0) {
        HILOG_DEBUG("timeout < 0 is unreasonable");
    }
    // 2. get callback args
    size_t callbackArgc = argc >= 2 ? argc - 2 : 0; // 2 include callback and timeout
    napi_ref* callbackArgv = nullptr;
    if (callbackArgc > 0) {
        callbackArgv = new napi_ref[callbackArgc];
        for (size_t idx = 0; idx < callbackArgc; idx++) {
            callbackArgv[idx] =
                Helper::NapiHelper::CreateReference(env, argv[idx + 2], 1); // 2 include callback and timeout
        }
    }
    // 3. generate time callback id
    // 4. generate time callback info
    // 5. push callback info into timerTable
    uint32_t tId = 0;
    TimerCallbackInfo* callbackInfo = nullptr;
    {
        std::lock_guard<std::mutex> lock(timeLock);
        tId = timeCallbackId++;
        napi_ref callbackRef = Helper::NapiHelper::CreateReference(env, argv[0], 1);
        callbackInfo = new TimerCallbackInfo(env, tId, timeout, callbackRef, repeat, callbackArgc, callbackArgv);
#ifdef ENABLE_CONTAINER_SCOPE
        callbackInfo->containerScopeId_ = ContainerScope::CurrentId();
#endif
        if (timerTable.find(tId) != timerTable.end()) {
            HILOG_ERROR("timerTable occurs error");
        } else {
            timerTable[tId] = callbackInfo;
        }
    }

    HILOG_DEBUG("SetTimeoutInnerCore function call before libuv! tId = %{public}u,timeout = %{public}u", tId, timeout);
#ifdef ENABLE_HITRACE_HELPER_METER
    HITRACE_HELPER_METER_NAME("SetTimeoutInnerCore function call before libuv! tId = " + std::to_string(tId)
        + ", timeout = " + std::to_string(timeout));
#endif
    // 6. start timer
    uv_loop_t* loop = Helper::NapiHelper::GetLibUV(env);
    NativeEngine* engine = reinterpret_cast<NativeEngine*>(env);
    uv_update_time(loop);
    uv_timer_start(callbackInfo->timeReq_, TimerCallback, timeout >= 0 ? timeout : 1, timeout > 0 ? timeout : 1);
    if (engine->IsMainThread()) {
        uv_async_send(&loop->wq_async);
    }
    return Helper::NapiHelper::CreateUint32(env, tId);
}

napi_value Timer::SetTimeoutInner(napi_env env, napi_callback_info cbinfo, bool repeat)
{
    size_t argc = Helper::NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    if (argc < 1) {
        napi_throw_error(env, nullptr, "StartTimeoutOrInterval, callback info is nullptr.");
        return nullptr;
    }
    napi_value* argv = new napi_value[argc];
    Helper::ObjectScope<napi_value> scope(argv, true);
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, cbinfo, &argc, argv, &thisVar, nullptr);
    if (!Helper::NapiHelper::IsCallable(env, argv[0])) {
        HILOG_ERROR("Set callback timer failed with invalid parameter.");
        return Helper::NapiHelper::GetUndefinedValue(env);
    }

    return SetTimeoutInnerCore(env, argv, argc, repeat);
}

void Timer::ClearEnvironmentTimer(napi_env env)
{
    std::lock_guard<std::mutex> lock(timeLock);
    auto iter = timerTable.begin();
    while (iter != timerTable.end()) {
        TimerCallbackInfo* callbackInfo = iter->second;
        if (callbackInfo->env_ == env) {
            iter = timerTable.erase(iter);
            Helper::CloseHelp::DeletePointer(callbackInfo, false);
        } else {
            iter++;
        }
    }
}

bool Timer::HasTimer(napi_env env)
{
    std::lock_guard<std::mutex> lock(timeLock);
    auto iter = std::find_if(timerTable.begin(), timerTable.end(), [env](const auto &item) {
        return item.second->env_ == env;
    });
    return iter != timerTable.end();
}

void Timer::DeleteTimer(uint32_t tId, TimerCallbackInfo* callbackInfo)
{
    std::lock_guard<std::mutex> lock(timeLock);
    // callback maybe contain ClearTimer, so we need check to avoid use-after-free and double-free of callbackInfo
    if (timerTable.find(tId) != timerTable.end()) {
        timerTable.erase(tId);
        Helper::CloseHelp::DeletePointer(callbackInfo, false);
    }
}
} // namespace Commonlibrary::JsSysModule
