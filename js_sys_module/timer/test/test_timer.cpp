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

#include "native_engine/native_value.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "test.h"
#include "test_timer.h"
#include "../timer.h"
#include "tools/log.h"

using namespace Commonlibrary::Concurrent::Common;
using namespace OHOS::JsSysModule;

#define ASSERT_CHECK_CALL(call)   \
    {                             \
        ASSERT_EQ(call, napi_ok); \
    }

#define ASSERT_CHECK_VALUE_TYPE(env, value, type)               \
    {                                                           \
        napi_valuetype valueType = napi_undefined;              \
        ASSERT_TRUE((value) != nullptr);                        \
        ASSERT_CHECK_CALL(napi_typeof(env, value, &valueType)); \
        ASSERT_EQ(valueType, type);                             \
    }

napi_value TimerCallback(napi_env env, napi_callback_info info)
{
    if (info == nullptr) {
        HILOG_ERROR("TimerCallback, Invalid input info.");
    }
    return nullptr;
}

/* @tc.name: Init
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TimerTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    bool res0 = Timer::RegisterTime(env);
    ASSERT_TRUE(res0);
    bool res1 = Timer::RegisterTime(nullptr);
    ASSERT_TRUE(!res1);
}

/* @tc.name: settimeout
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TimerTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 0;
    napi_value argv0[] = {nullptr}; // no args has exception
    napi_value cb = nullptr;
    napi_create_function(env, "setInterval", NAPI_AUTO_LENGTH, TimerTest::SetInterval, nullptr, &cb);
    napi_value res = nullptr;
    napi_call_function(env, nullptr, cb, argc, argv0, &res);
    ASSERT_TRUE(res == nullptr);
    bool res0 = 0;
    napi_is_exception_pending(env, &res0);
    ASSERT_TRUE(res0);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
}

/* @tc.name: settimeout
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TimerTest003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 2;
    napi_value nativeMessage0 = nullptr;
    napi_create_uint32(env, 5, &nativeMessage0); // Random number
    napi_value nativeMessage1 = nullptr;
    napi_create_uint32(env, 50, &nativeMessage1); // Random number
    napi_value argv[] = {nativeMessage0, nativeMessage1};
    napi_value cb = nullptr;
    napi_create_function(env, "setTimeout", NAPI_AUTO_LENGTH, TimerTest::SetTimeout, nullptr, &cb); // uncallable
    napi_value tId = nullptr;
    napi_call_function(env, nullptr, cb, argc, argv, &tId);
    ASSERT_CHECK_VALUE_TYPE(env, tId, napi_undefined);
}

/* @tc.name: settimeout/ClearTimer
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TimerTest004, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 2;
    napi_value nativeMessage0 = nullptr;
    napi_create_uint32(env, 50, &nativeMessage0); // Random number
    napi_value nativeMessage1 = nullptr;
    napi_create_function(env, "callback", NAPI_AUTO_LENGTH, TimerCallback, nullptr, &nativeMessage1);
    napi_value argv[] = {nativeMessage1, nativeMessage0};
    napi_value setTimeoutCB = nullptr;
    napi_create_function(env, "setTimeout", NAPI_AUTO_LENGTH, TimerTest::SetTimeout, nullptr, &setTimeoutCB);
    napi_value tId = nullptr;
    napi_call_function(env, nullptr, setTimeoutCB, argc, argv, &tId);
    ASSERT_CHECK_VALUE_TYPE(env, tId, napi_number);
    napi_value argv1[] = {tId};
    napi_value clearTimerCB = nullptr;
    napi_create_function(env, "clearTimer", NAPI_AUTO_LENGTH, TimerTest::ClearTimer, nullptr, &clearTimerCB);
    napi_value res = nullptr;
    napi_call_function(env, nullptr, clearTimerCB, 1, argv1, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: settimeout
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TimerTest005, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 2;
    int32_t number = -50.0; // Random number
    napi_value nativeMessage0 = nullptr;
    napi_create_int32(env, number, &nativeMessage0);
    napi_value nativeMessage1 = nullptr;
    napi_create_function(env, "callback", NAPI_AUTO_LENGTH, TimerCallback, nullptr, &nativeMessage1);
    napi_value argv[] = {nativeMessage1, nativeMessage0};
    napi_value cb = nullptr;
    napi_create_function(env, "setTimerout", NAPI_AUTO_LENGTH, TimerTest::SetTimeout, nullptr, &cb);
    napi_value tId = nullptr;
    napi_call_function(env, nullptr, cb, argc, argv, &tId);
    ASSERT_CHECK_VALUE_TYPE(env, tId, napi_number);
}

/* @tc.name: settimeout
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TimerTest006, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 2;
    napi_value nativeMessage0 = nullptr;
    napi_create_function(env, "callback", NAPI_AUTO_LENGTH, TimerCallback, nullptr, &nativeMessage0);
    std::string message = "50";
    napi_value nativeMessage1 = nullptr;
    napi_create_string_utf8(env, message.c_str(), message.length(), &nativeMessage1); // timeout is string
    napi_value argv[] = {nativeMessage0, nativeMessage1};
    napi_value cb = nullptr;
    napi_create_function(env, "setTimerout", NAPI_AUTO_LENGTH, TimerTest::SetTimeout, nullptr, &cb);
    napi_value tId = nullptr;
    napi_call_function(env, nullptr, cb, argc, argv, &tId);
    ASSERT_CHECK_VALUE_TYPE(env, tId, napi_number);
}

/* @tc.name: ClearTimer
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TimerTest007, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 0;
    napi_value argv[] = {nullptr}; // no args
    napi_value cb = nullptr;
    napi_create_function(env, "clearTimer", NAPI_AUTO_LENGTH, TimerTest::ClearTimer, nullptr, &cb);
    napi_value res = nullptr;
    napi_call_function(env, nullptr, cb, argc, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: ClearTimer
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TimerTest008, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 1;
    napi_value nativeMessage = nullptr;
    std::string message = "50"; // Random number
    napi_create_string_utf8(env, message.c_str(), message.length(), &nativeMessage);
    napi_value argv[] = {nativeMessage};
    napi_value cb = nullptr;
    napi_create_function(env, "clearTimer", NAPI_AUTO_LENGTH, TimerTest::ClearTimer, nullptr, &cb);
    napi_value res = nullptr;
    napi_call_function(env, nullptr, cb, argc, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: ClearTimer
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TimerTest009, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 1;
    napi_value cb = nullptr;
    napi_value inputId = nullptr;
    napi_create_uint32(env, 50, &inputId); // Random number
    napi_value argv[] = { inputId }; // timerId is inexistent
    napi_create_function(env, "clearTimer", NAPI_AUTO_LENGTH, TimerTest::ClearTimer, nullptr, &cb);
    napi_value res = nullptr;
    napi_call_function(env, nullptr, cb, argc, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: setinteval
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TimerTest010, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 2;
    napi_value nativeMessage0 = nullptr;
    napi_create_uint32(env, 5, &nativeMessage0); // Random number
    napi_value nativeMessage1 = nullptr;
    napi_create_uint32(env, 50, &nativeMessage1); // Random number
    napi_value argv1[] = {nativeMessage0, nativeMessage1};
    napi_value cb1 = nullptr;
    napi_create_function(env, "setInterval", NAPI_AUTO_LENGTH, TimerTest::SetInterval, nullptr, &cb1); // uncallable
    napi_value tId1 = nullptr;
    napi_call_function(env, nullptr, cb1, argc, argv1, &tId1);
    ASSERT_CHECK_VALUE_TYPE(env, tId1, napi_undefined);
    napi_value nativeMessage2 = nullptr;
    napi_create_function(env, "callback", NAPI_AUTO_LENGTH, TimerCallback, nullptr, &nativeMessage2);
    napi_value argv2[] = {nativeMessage2, nativeMessage1};
    napi_value tId2 = nullptr;
    napi_value cb2 = nullptr;
    napi_create_function(env, "setInterval", NAPI_AUTO_LENGTH, TimerTest::SetInterval, nullptr, &cb2);
    napi_call_function(env, nullptr, cb2, argc, argv2, &tId2);
    ASSERT_CHECK_VALUE_TYPE(env, tId2, napi_number);
    int32_t number = -50.0; // Random number
    napi_value nativeMessage3 = nullptr;
    napi_create_int32(env, number, &nativeMessage3);
    napi_value argv3[] = {nativeMessage2, nativeMessage3};
    napi_value tId3 = nullptr;
    napi_value cb3 = nullptr;
    napi_create_function(env, "setTimeout", NAPI_AUTO_LENGTH, TimerTest::SetTimeout, nullptr, &cb3);
    napi_call_function(env, nullptr, cb3, argc, argv3, &tId3);
    ASSERT_CHECK_VALUE_TYPE(env, tId3, napi_number);
    std::string message = "50"; // Random number
    napi_value nativeMessage4 = nullptr;
    napi_create_string_utf8(env, message.c_str(), message.length(), &nativeMessage4);
    napi_value argv4[] = {nativeMessage2, nativeMessage4};
    napi_value tId4 = nullptr;
    napi_value cb4 = nullptr;
    napi_create_function(env, "setInterval", NAPI_AUTO_LENGTH, TimerTest::SetInterval, nullptr, &cb4);
    napi_call_function(env, nullptr, cb4, argc, argv4, &tId4);
    ASSERT_CHECK_VALUE_TYPE(env, tId4, napi_number);

    bool res0 = Timer::HasTimer(env);
    ASSERT_TRUE(res0);
    Timer::ClearEnvironmentTimer(env);
    bool res1 = Timer::HasTimer(env);
    ASSERT_TRUE(!res1);
}

/* @tc.name: settimeout
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TimerTest011, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 1;  // argc = 1, timeout = 0

    napi_value tId = nullptr;
    napi_value cb = nullptr;
    napi_value nativeMessage1 = nullptr;
    napi_create_function(env, "callback", NAPI_AUTO_LENGTH, TimerCallback, nullptr, &nativeMessage1);
    napi_value argv[] = {nativeMessage1};
    napi_create_function(env, "setTimeout", NAPI_AUTO_LENGTH, TimerTest::SetTimeout, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &tId);
    ASSERT_CHECK_VALUE_TYPE(env, tId, napi_number);
}

/* @tc.name: settimeout
 * @tc.desc: Test: callbackArgc > 0
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TimerTest012, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 3;
    napi_value tId = nullptr;
    napi_value cb = nullptr;
    napi_value nativeMessage0 = nullptr;
    napi_create_uint32(env, 50, &nativeMessage0); // Random number
    napi_value nativeMessage2 = nullptr;
    napi_create_uint32(env, 50, &nativeMessage2); // Random number
    napi_value nativeMessage1 = nullptr;
    napi_create_function(env, "callback", NAPI_AUTO_LENGTH, TimerCallback, nullptr, &nativeMessage1);
    napi_value argv[] = {nativeMessage1, nativeMessage0, nativeMessage2};
    napi_create_function(env, "setTimeout", NAPI_AUTO_LENGTH, TimerTest::SetTimeout, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &tId);
    ASSERT_CHECK_VALUE_TYPE(env, tId, napi_number);
}
