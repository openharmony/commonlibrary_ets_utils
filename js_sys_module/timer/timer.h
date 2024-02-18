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

#ifndef TIMER_TIMER_H
#define TIMER_TIMER_H

#include <map>
#include <mutex>
#include <uv.h>

#include "commonlibrary/ets_utils/js_concurrent_module/common/helper/napi_helper.h"
#include "commonlibrary/ets_utils/js_concurrent_module/common/helper/object_helper.h"
#ifdef ENABLE_CONTAINER_SCOPE
#include "core/common/container_scope.h"
#endif
#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS::JsSysModule {
struct TimerCallbackInfo {
    napi_env env_ {nullptr};
    uint32_t tId_ {};
    int32_t timeout_ {};
    napi_ref callback_ {};
    bool repeat_ {};
    uv_timer_t* timeReq_ {nullptr};
    size_t argc_ {};
    napi_ref* argv_ {nullptr};
#ifdef ENABLE_CONTAINER_SCOPE
    int32_t containerScopeId_ {-1};
#endif

    TimerCallbackInfo(napi_env env, uint32_t tId, int32_t timeout, napi_ref callback,
                     bool repeat, size_t argc, napi_ref* argv)
        : env_(env), tId_(tId), timeout_(timeout), callback_(callback),
          repeat_(repeat), argc_(argc), argv_(argv)
    {
        uv_loop_t* loop = Commonlibrary::Concurrent::Common::Helper::NapiHelper::GetLibUV(env_);
        timeReq_ = new uv_timer_t;
        uv_timer_init(loop, timeReq_);
        timeReq_->data = this;
    }

    ~TimerCallbackInfo();
};

class Timer {
public:
    Timer() = default;
    ~Timer() = default;
    static bool RegisterTime(napi_env env);
    static void ClearEnvironmentTimer(napi_env env);
    static bool HasTimer(napi_env env);
    friend class TimerTest;

private:
    static napi_value SetTimeout(napi_env env, napi_callback_info cbinfo);
    static napi_value SetInterval(napi_env env, napi_callback_info cbinfo);
    static napi_value ClearTimer(napi_env env, napi_callback_info cbinfo);
    static napi_value SetTimeoutInner(napi_env env, napi_callback_info cbinfo, bool repeat);
    static void TimerCallback(uv_timer_t* handle);
    static void DeleteTimer(uint32_t tId, TimerCallbackInfo* callbackInfo);

    static uint32_t timeCallbackId;
    static std::map<uint32_t, TimerCallbackInfo*> timerTable;
    static std::mutex timeLock;
};
} // namespace Commonlibrary::JsSysModule
#endif // TIMER_TIMER_H
