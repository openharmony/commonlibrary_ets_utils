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
#ifndef TEST_CONSOLE_H
#define TEST_CONSOLE_H

#include "../console.h"

namespace OHOS::JsSysModule {

class ConsoleTest {
public:
    ConsoleTest() = default;
    ~ConsoleTest() = default;
    template<LogLevel LEVEL>
    static napi_value ConsoleLog(napi_env env, napi_callback_info info);
    static napi_value Count(napi_env env, napi_callback_info info);
    static napi_value CountReset(napi_env env, napi_callback_info info);
    static napi_value Dir(napi_env env, napi_callback_info info);
    static napi_value Group(napi_env env, napi_callback_info info);
    static napi_value GroupEnd(napi_env env, napi_callback_info info);
    static napi_value Table(napi_env env, napi_callback_info info);
    static napi_value Time(napi_env env, napi_callback_info info);
    static napi_value TimeLog(napi_env env, napi_callback_info info);
    static napi_value TimeEnd(napi_env env, napi_callback_info info);
    static napi_value Trace(napi_env env, napi_callback_info info);
    static napi_value TraceHybridStack(napi_env env, napi_callback_info info);
    static napi_value Assert(napi_env env, napi_callback_info info);
    static void PrintTime(std::string timerName, double time, std::string& log);
};
}
#endif // TEST_CONSOLE_H