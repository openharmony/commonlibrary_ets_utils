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

#include "timer.h"

#include "utils/log.h"

namespace Commonlibrary::Concurrent::Common::Plugin {
uint32_t Timer::timeCallbackId = 0;
std::map<uint32_t, TimerCallbackInfo*> Timer::timerTable;
std::mutex Timer::timeLock;

TimerCallbackInfo::~TimerCallbackInfo()
{
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

bool Timer::RegisterTime(napi_env env)
{
    if (env == nullptr) {
        return false;
    }
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("setTimeout", SetTimeout),
        DECLARE_NAPI_FUNCTION("setInterval", SetInterval),
        DECLARE_NAPI_FUNCTION("clearTimeout", ClearTimer),
        DECLARE_NAPI_FUNCTION("clearInterval", ClearTimer)
    };
    napi_value globalObj = Helper::NapiHelper::GetGlobalObject(env);
    napi_status status = napi_define_properties(env, globalObj, sizeof(properties) / sizeof(properties[0]), properties);
    return status == napi_ok;
}

napi_value Timer::SetTimeout(napi_env env, napi_callback_info cbinfo)
{
    return Timer::SetTimeoutInner(env, cbinfo, false);
}

napi_value Timer::SetInterval(napi_env env, napi_callback_info cbinfo)
{
    return Timer::SetTimeoutInner(env, cbinfo, true);
}

napi_value Timer::ClearTimer(napi_env env, napi_callback_info cbinfo)
{
    // 1. check args
    size_t argc = Helper::NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    if (argc < 1) {
        HILOG_WARN("first arg should be number");
        return nullptr;
    }
    napi_value* argv = new napi_value[argc];
    Helper::ObjectScope<napi_value> scope(argv, true);
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, cbinfo, &argc, argv, &thisVar, nullptr);

    uint32_t tId;
    napi_status status = napi_get_value_uint32(env, argv[0], &tId);
    if (status != napi_ok) {
        HILOG_WARN("handler should be number");
        return nullptr;
    }
    TimerCallbackInfo* callbackInfo = nullptr;
    {
        std::lock_guard<std::mutex> lock(timeLock);
        auto iter = timerTable.find(tId);
        if (iter == timerTable.end()) {
            HILOG_INFO("handler not in table");
            return nullptr;
        }
        callbackInfo = iter->second;
        timerTable.erase(tId);
    }
    Helper::CloseHelp::DeletePointer(callbackInfo, false);
    return Helper::NapiHelper::GetUndefinedValue(env);
}

void Timer::TimerCallback(uv_timer_t* handle)
{
    TimerCallbackInfo* callbackInfo = static_cast<TimerCallbackInfo*>(handle->data);
    if (callbackInfo == nullptr) {
        return;
    }
    napi_value callback = Helper::NapiHelper::GetReferenceValue(callbackInfo->env_, callbackInfo->callback_);
    napi_value undefinedValue = Helper::NapiHelper::GetUndefinedValue(callbackInfo->env_);
    napi_value callbackResult = nullptr;
    napi_value* callbackArgv = new napi_value[callbackInfo->argc_];
    Helper::ObjectScope<napi_value> scope(callbackArgv, true);
    for (size_t idx = 0; idx < callbackInfo->argc_; idx++) {
        callbackArgv[idx] = Helper::NapiHelper::GetReferenceValue(callbackInfo->env_, callbackInfo->argv_[idx]);
    }
    napi_call_function(callbackInfo->env_, undefinedValue, callback,
                       callbackInfo->argc_, callbackArgv, &callbackResult);
    if (callbackResult == nullptr) {
        HILOG_WARN("call callback error");
        return;
    }
    if (!callbackInfo->repeat_) {
        {
            std::lock_guard<std::mutex> lock(timeLock);
            if (timerTable.find(callbackInfo->tId_) == timerTable.end()) {
                HILOG_ERROR("erase inexistent timerCallbackInfo");
            } else {
                timerTable.erase(callbackInfo->tId_);
            }
        }
        Helper::CloseHelp::DeletePointer(callbackInfo, false);
    } else {
        uv_timer_again(handle);
    }
}

napi_value Timer::SetTimeoutInner(napi_env env, napi_callback_info cbinfo, bool repeat)
{
    // 1. check args
    size_t argc = Helper::NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    if (argc < 1) {
        napi_throw_error(env, nullptr, "callback must be a function. received undefined");
        return nullptr;
    }
    napi_value* argv = new napi_value[argc];
    Helper::ObjectScope<napi_value> scope(argv, true);
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, cbinfo, &argc, argv, &thisVar, nullptr);
    if (!Helper::NapiHelper::IsCallable(env, argv[0])) {
        napi_throw_error(env, nullptr, "callback must be a function.");
        return nullptr;
    }
    int32_t timeout = 0;
    if (argc > 1) {
        napi_status status = napi_get_value_int32(env, argv[1], &timeout);
        if (status != napi_ok) {
            HILOG_WARN("timeout should be number");
            timeout = 0;
        }
    }
    if (timeout < 0) {
        HILOG_WARN("worker:: timeout < 0 is unreasonable");
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
        if (timerTable.find(tId) != timerTable.end()) {
            HILOG_ERROR("timerTable occurs error");
        } else {
            timerTable[tId] = callbackInfo;
        }
    }

    // 6. start timer
    uv_timer_start(callbackInfo->timeReq_, TimerCallback, timeout >= 0 ? timeout : 1, timeout > 0 ? timeout : 1);
    return Helper::NapiHelper::CreateUint32(env, tId);
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
} // namespace Commonlibrary::Concurrent::Common::Plugin
