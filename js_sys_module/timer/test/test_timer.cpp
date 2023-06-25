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
#include "utils/log.h"

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

NativeValue *TimerCallback(NativeEngine *engine, NativeCallbackInfo *info)
{
    if (info == nullptr || info->functionInfo == nullptr || info->functionInfo->data == nullptr) {
        HILOG_ERROR("TimerCallback, Invalid input info.");
        return nullptr;
    }
    return nullptr;
}

napi_callback_info napi_create_cbinfo(napi_env env,
                                      size_t* argc,
                                      NativeValue** argv)
{
    NativeCallbackInfo* info = new NativeCallbackInfo;

    if (argv != nullptr && argc != nullptr) {
        info->argc = *argc;
        info->argv = argv;
        return reinterpret_cast<napi_callback_info>(info);
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
    NativeValue* argv0[] = {nullptr};
    napi_callback_info cbinfo0 = napi_create_cbinfo(env, &argc, argv0); // no args has exception
    napi_value tId0 = TimerTest::SetInterval(env, cbinfo0);
    ASSERT_TRUE(tId0 == nullptr);
    bool res0 = 0;
    napi_is_exception_pending(env, &res0);
    ASSERT_TRUE(res0);
    engine_->GetAndClearLastException();
}

/* @tc.name: settimeout
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TimerTest003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 2;
    NativeValue* nativeMessage0 = engine_->CreateNumber(5); // Random number
    NativeValue* nativeMessage1 = engine_->CreateNumber(50); // Random number
    NativeValue* argv[] = {nativeMessage0, nativeMessage1};
    napi_callback_info cbinfo = napi_create_cbinfo(env, &argc, argv); // uncallable
    napi_value tId = TimerTest::SetTimeout(env, cbinfo);
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
    NativeValue* nativeMessage0 = engine_->CreateNumber(50); // Random number
    NativeValue* nativeMessage1 = engine_->CreateFunction("callback", strlen("callback"),
                                                          TimerCallback, nullptr);
    NativeValue* argv[] = {nativeMessage1, nativeMessage0};
    napi_callback_info cbinfo = napi_create_cbinfo(env, &argc, argv);
    napi_value tId = TimerTest::SetTimeout(env, cbinfo);
    ASSERT_CHECK_VALUE_TYPE(env, tId, napi_number);

    NativeValue* inputId = reinterpret_cast<NativeValue*>(tId);
    NativeValue* argv1[] = {inputId};
    napi_callback_info cbinfo1 = napi_create_cbinfo(env, &argc, argv1);
    napi_value res = TimerTest::ClearTimer(env, cbinfo1);
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
    NativeValue* nativeMessage0 = engine_->CreateFunction("callback", strlen("callback"),
                                                          TimerCallback, nullptr);
    int32_t number = -50.0; // Random number
    NativeValue* nativeMessage1 = engine_->CreateNumber(number);
    NativeValue* argv[] = {nativeMessage0, nativeMessage1};
    napi_callback_info cbinfo = napi_create_cbinfo(env, &argc, argv); // timeout < 0
    napi_value tId = TimerTest::SetTimeout(env, cbinfo);
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
    NativeValue* nativeMessage0 = engine_->CreateFunction("callback", strlen("callback"),
                                                          TimerCallback, nullptr);
    std::string message = "50";
    NativeValue* nativeMessage1 = engine_->CreateString(message.c_str(), message.length());
    NativeValue* argv[] = {nativeMessage0, nativeMessage1};
    napi_callback_info cbinfo = napi_create_cbinfo(env, &argc, argv); // timeout is string
    napi_value tId = TimerTest::SetTimeout(env, cbinfo);
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
    NativeValue* argv[] = {nullptr};
    napi_callback_info cbinfo = napi_create_cbinfo(env, &argc, argv); // no args
    napi_value res = TimerTest::ClearTimer(env, cbinfo);
    ASSERT_TRUE(res == nullptr);
}

/* @tc.name: ClearTimer
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TimerTest008, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 1;
    std::string message = "50"; // Random number
    NativeValue* nativeMessage = engine_->CreateString(message.c_str(), message.length());
    NativeValue* argv[] = {nativeMessage};
    napi_callback_info cbinfo = napi_create_cbinfo(env, &argc, argv); // first param should be number
    napi_value res = TimerTest::ClearTimer(env, cbinfo);
    ASSERT_TRUE(res == nullptr);
}

/* @tc.name: ClearTimer
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TimerTest009, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 1;
    NativeValue* inputId = engine_->CreateNumber(50); // Random number
    NativeValue* argv[] = {inputId};
    napi_callback_info cbinfo = napi_create_cbinfo(env, &argc, argv); // timerId is inexistent
    napi_value res = TimerTest::ClearTimer(env, cbinfo);
    ASSERT_TRUE(res == nullptr);
}

/* @tc.name: setinteval
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TimerTest010, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 2;
    NativeValue* nativeMessage0 = engine_->CreateNumber(5); // Random number
    NativeValue* nativeMessage1 = engine_->CreateNumber(50); // Random number
    NativeValue* argv1[] = {nativeMessage0, nativeMessage1};
    napi_callback_info cbinfo1 = napi_create_cbinfo(env, &argc, argv1); // uncallable
    napi_value tId1 = TimerTest::SetInterval(env, cbinfo1);
    ASSERT_CHECK_VALUE_TYPE(env, tId1, napi_undefined);

    NativeValue* nativeMessage2 = engine_->CreateFunction("callback", strlen("callback"),
                                                          TimerCallback, nullptr);
    NativeValue* argv2[] = {nativeMessage2, nativeMessage1};
    napi_callback_info cbinfo2 = napi_create_cbinfo(env, &argc, argv2);
    napi_value tId2 = TimerTest::SetInterval(env, cbinfo2);
    ASSERT_CHECK_VALUE_TYPE(env, tId2, napi_number);

    int32_t number = -50.0; // Random number
    NativeValue* nativeMessage3 = engine_->CreateNumber(number);
    NativeValue* argv3[] = {nativeMessage2, nativeMessage3};
    napi_callback_info cbinfo3 = napi_create_cbinfo(env, &argc, argv3); // timeout < 0
    napi_value tId3 = TimerTest::SetTimeout(env, cbinfo3);
    ASSERT_CHECK_VALUE_TYPE(env, tId3, napi_number);

    std::string message = "50"; // Random number
    NativeValue* nativeMessage4 =  engine_->CreateString(message.c_str(), message.length());
    NativeValue* argv4[] = {nativeMessage2, nativeMessage4};
    napi_callback_info cbinfo4 = napi_create_cbinfo(env, &argc, argv4); // timeout is string
    napi_value tId4 = TimerTest::SetInterval(env, cbinfo4);
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
    NativeValue* nativeMessage1 = engine_->CreateFunction("callback", strlen("callback"),
                                                          TimerCallback, nullptr);
    NativeValue* argv[] = {nativeMessage1};
    napi_callback_info cbinfo = napi_create_cbinfo(env, &argc, argv);
    napi_value tId = TimerTest::SetTimeout(env, cbinfo);
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
    NativeValue* nativeMessage0 = engine_->CreateNumber(50); // Random number
    NativeValue* nativeMessage1 = engine_->CreateFunction("callback", strlen("callback"),
                                                          TimerCallback, nullptr);
    NativeValue* nativeMessage2 = engine_->CreateNumber(50); // Random number
    NativeValue* argv[] = {nativeMessage1, nativeMessage0, nativeMessage2};
    napi_callback_info cbinfo = napi_create_cbinfo(env, &argc, argv);
    napi_value tId = TimerTest::SetTimeout(env, cbinfo);
    ASSERT_CHECK_VALUE_TYPE(env, tId, napi_number);
}

