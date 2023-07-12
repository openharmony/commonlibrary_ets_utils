/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef TEST_TIMER_H
#define TEST_TIMER_H

#include "commonlibrary/ets_utils/js_concurrent_module/common/helper/napi_helper.h"
#include "../timer.h"

namespace OHOS::JsSysModule {
class TimerTest {
public:
    TimerTest() = default;
    ~TimerTest() = default;
    static napi_value SetTimeout(napi_env env, napi_callback_info cbinfo);
    static napi_value SetInterval(napi_env env, napi_callback_info cbinfo);
    static napi_value ClearTimer(napi_env env, napi_callback_info cbinfo);
};

napi_value TimerTest::SetTimeout(napi_env env, napi_callback_info cbinfo)
{
    return Timer::SetTimeout(env, cbinfo);
};

napi_value TimerTest::SetInterval(napi_env env, napi_callback_info cbinfo)
{
    return Timer::SetInterval(env, cbinfo);
};

napi_value TimerTest::ClearTimer(napi_env env, napi_callback_info cbinfo)
{
    return Timer::ClearTimer(env, cbinfo);
};
}
#endif // TEST_TIMER_H
