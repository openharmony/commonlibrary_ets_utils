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

#include "ark_native_engine.h"
#include "native_engine/native_value.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "test.h"
#include "test_timer.h"
#include "../sys_timer.h"
#include "tools/log.h"

using namespace Commonlibrary::Concurrent::Common;
using namespace OHOS::JsSysModule;
using panda::RuntimeOption;

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

class NativeEngineProxy {
public:
    NativeEngineProxy()
    {
        //Setup
        RuntimeOption option;
        option.SetGcType(RuntimeOption::GC_TYPE::GEN_GC);
        const int64_t poolSize = 0x1000000;
        option.SetGcPoolSize(poolSize);
        option.SetLogLevel(RuntimeOption::LOG_LEVEL::ERROR);
        option.SetDebuggerLibraryPath("");
        vm_ = panda::JSNApi::CreateJSVM(option);
        if (vm_ == nullptr) {
            return;
        }

        engine_ = new ArkNativeEngine(vm_, nullptr);
        Timer::RegisterTime(reinterpret_cast<napi_env>(engine_));
    }

    ~NativeEngineProxy()
    {
        delete engine_;
        panda::JSNApi::DestroyJSVM(vm_);
    }

    inline ArkNativeEngine* operator-() const
    {
        return engine_;
    }

    inline operator napi_env() const
    {
        return reinterpret_cast<napi_env>(engine_);
    }

private:
    EcmaVM* vm_ {nullptr};
    ArkNativeEngine* engine_ {nullptr};
};

struct TestData {
    napi_env env_;
};

void AssertFalse()
{
    ASSERT_TRUE(false);
}

napi_value TimerAbort(napi_env env, napi_callback_info info)
{
    AssertFalse();
    return nullptr;
}

napi_value GetGlobalProperty(napi_env env, const char *name)
{
    napi_value value = nullptr;
    napi_value global;
    napi_get_global(env, &global);
    napi_get_named_property(env, global, name, &value);
    return value;
}

napi_value ThrowingCallback(napi_env env, napi_callback_info info)
{
    napi_value errorMessage;
    napi_value error;
    napi_create_string_utf8(env, "An error occurred!", NAPI_AUTO_LENGTH, &errorMessage);
    napi_create_error(env, nullptr, errorMessage, &error);
    napi_throw(env, error);
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
    napi_value cb = GetGlobalProperty(env, "setInterval");
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
    napi_value cb = GetGlobalProperty(env, "setTimeout");
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
    napi_value setTimeoutCB = GetGlobalProperty(env, "setTimeout");
    napi_value tId = nullptr;
    napi_call_function(env, nullptr, setTimeoutCB, argc, argv, &tId);
    ASSERT_CHECK_VALUE_TYPE(env, tId, napi_number);
    napi_value argv1[] = {tId};
    napi_value clearTimerCB = GetGlobalProperty(env, "clearTimeout");
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
    napi_value cb = GetGlobalProperty(env, "setTimeout");
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
    napi_value cb = GetGlobalProperty(env, "setTimeout");
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
    napi_value cb = GetGlobalProperty(env, "clearTimeout");
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
    napi_value cb = GetGlobalProperty(env, "clearTimeout");
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
    napi_value cb = GetGlobalProperty(env, "clearTimeout");
    napi_value inputId = nullptr;
    napi_create_uint32(env, 50, &inputId); // Random number
    napi_value argv[] = { inputId }; // timerId is inexistent
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
    napi_value cb1 = GetGlobalProperty(env, "setInterval");
    napi_value tId1 = nullptr;
    napi_call_function(env, nullptr, cb1, argc, argv1, &tId1);
    ASSERT_CHECK_VALUE_TYPE(env, tId1, napi_undefined);
    napi_value nativeMessage2 = nullptr;
    napi_create_function(env, "callback", NAPI_AUTO_LENGTH, TimerCallback, nullptr, &nativeMessage2);
    napi_value argv2[] = {nativeMessage2, nativeMessage1};
    napi_value tId2 = nullptr;
    napi_value cb2 = GetGlobalProperty(env, "setInterval");
    napi_call_function(env, nullptr, cb2, argc, argv2, &tId2);
    ASSERT_CHECK_VALUE_TYPE(env, tId2, napi_number);
    int32_t number = -50.0; // Random number
    napi_value nativeMessage3 = nullptr;
    napi_create_int32(env, number, &nativeMessage3);
    napi_value argv3[] = {nativeMessage2, nativeMessage3};
    napi_value tId3 = nullptr;
    napi_value cb3 = GetGlobalProperty(env, "setTimeout");
    napi_call_function(env, nullptr, cb3, argc, argv3, &tId3);
    ASSERT_CHECK_VALUE_TYPE(env, tId3, napi_number);
    std::string message = "50"; // Random number
    napi_value nativeMessage4 = nullptr;
    napi_create_string_utf8(env, message.c_str(), message.length(), &nativeMessage4);
    napi_value argv4[] = {nativeMessage2, nativeMessage4};
    napi_value tId4 = nullptr;
    napi_value cb4 = GetGlobalProperty(env, "setInterval");
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
    napi_value cb = GetGlobalProperty(env, "setTimeout");
    napi_value nativeMessage1 = nullptr;
    napi_create_function(env, "callback", NAPI_AUTO_LENGTH, TimerCallback, nullptr, &nativeMessage1);
    napi_value argv[] = {nativeMessage1};
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
    napi_value cb = GetGlobalProperty(env, "setTimeout");
    napi_value nativeMessage0 = nullptr;
    napi_create_uint32(env, 50, &nativeMessage0); // Random number
    napi_value nativeMessage2 = nullptr;
    napi_create_uint32(env, 50, &nativeMessage2); // Random number
    napi_value nativeMessage1 = nullptr;
    napi_create_function(env, "callback", NAPI_AUTO_LENGTH, TimerCallback, nullptr, &nativeMessage1);
    napi_value argv[] = {nativeMessage1, nativeMessage0, nativeMessage2};
    napi_call_function(env, nullptr, cb, argc, argv, &tId);
    ASSERT_CHECK_VALUE_TYPE(env, tId, napi_number);
}

/* @tc.name: DeleteTimer
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TimerTest013, testing::ext::TestSize.Level0)
{
    std::map<uint32_t, TimerCallbackInfo*>& table = TimerTest::create_timerTable();
    napi_env env = (napi_env)engine_;
    uint32_t tId = 1;
    int32_t timeout = 1000;
    napi_ref callback = nullptr;
    bool repeat = false;
    size_t argc = 0;
    napi_ref* argv = nullptr;
    TimerCallbackInfo* callbackInfo = new TimerCallbackInfo(env, tId, timeout, callback, repeat, argc, argv);
    table[tId] = callbackInfo;
    TimerTest::DeleteTimer(tId, callbackInfo);
    ASSERT_TRUE(table.find(tId) == table.end());
}

/* @tc.name: Settimeout
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TimerTest014, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value nativeMessage0 = nullptr;
    napi_create_uint32(env, 50, &nativeMessage0);
    napi_value nativeMessage1 = nullptr;
    napi_create_function(env, "callback", NAPI_AUTO_LENGTH, TimerCallback, nullptr, &nativeMessage1);
    napi_value argv[] = {nativeMessage1, nativeMessage0};
    napi_value setTimeoutCB = GetGlobalProperty(env, "setTimeout");
    napi_value tId = nullptr;
    size_t argc = 2;
    napi_call_function(env, nullptr, setTimeoutCB, argc, argv, &tId);
    ASSERT_CHECK_VALUE_TYPE(env, tId, napi_number);
    napi_env* env2 = new napi_env;
    napi_create_runtime(env, env2);
    napi_value argv1[] = {tId};
    napi_value clearTimerCB = GetGlobalProperty(env, "clearTimeout");
    napi_value res = nullptr;
    napi_call_function(*env2, nullptr, clearTimerCB, 1, argv1, &res);
    ASSERT_CHECK_VALUE_TYPE(*env2, res, napi_undefined);
}

/* @tc.name: ClearEnvironmentTimer
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TimerTest015, testing::ext::TestSize.Level0)
{
    uv_timer_t* handle = new uv_timer_t;
    handle->data = nullptr;
    TimerTest::TimerCallback(handle);

    napi_env env = (napi_env)engine_;
    size_t argc = 2;
    napi_value nativeMessage0 = nullptr;
    napi_create_uint32(env, 50, &nativeMessage0); // Random number
    napi_value nativeMessage1 = nullptr;
    napi_create_function(env, "callback", NAPI_AUTO_LENGTH, TimerCallback, nullptr, &nativeMessage1);
    napi_value argv[] = {nativeMessage1, nativeMessage0};
    napi_value setTimeoutCB = GetGlobalProperty(env, "setTimeout");
    napi_value tId = nullptr;
    napi_call_function(env, nullptr, setTimeoutCB, argc, argv, &tId);
    ASSERT_CHECK_VALUE_TYPE(env, tId, napi_number);
    napi_env* env2 = new napi_env;
    napi_create_runtime(env, env2);
    TimerTest::ClearEnvironmentTimer(*env2);
    std::map<uint32_t, TimerCallbackInfo*>& table = TimerTest::create_timerTable();
    ASSERT_TRUE(table.size() != 0);
}

/* @tc.name: settimeout
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TimerTest016, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_env* env2 = new napi_env;
    napi_create_runtime(env, env2);
    size_t argc = 2;
    napi_value nativeMessage0 = nullptr;
    napi_create_uint32(*env2, 50, &nativeMessage0); // Random number
    napi_value nativeMessage1 = nullptr;
    napi_create_function(*env2, "callback", NAPI_AUTO_LENGTH, TimerCallback, nullptr, &nativeMessage1);
    napi_value argv[] = {nativeMessage1, nativeMessage0};
    napi_value setTimeoutCB = GetGlobalProperty(env, "setTimeout");
    napi_value tId = nullptr;
    napi_call_function(*env2, nullptr, setTimeoutCB, argc, argv, &tId);
    ASSERT_CHECK_VALUE_TYPE(*env2, tId, napi_number);
}

/* @tc.name: settimeout
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TimerTest017, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_env env2 = nullptr;
    uv_timer_t* handle = new uv_timer_t;
    uint32_t tId2 = 1;
    int32_t timeout = 1000;
    napi_ref callback = nullptr;
    bool repeat = false;
    size_t argc2 = 0;
    napi_ref* argv2 = nullptr;
    handle->data = new TimerCallbackInfo(env, tId2, timeout, callback, repeat, argc2, argv2);
    TimerCallbackInfo* callbackInfo = static_cast<TimerCallbackInfo*>(handle->data);
    callbackInfo->env_ = env2;
    TimerTest::TimerCallback(handle);

    size_t argc = 2;
    napi_value nativeMessage0 = nullptr;
    napi_create_uint32(env, 50, &nativeMessage0); // Random number
    napi_value nativeMessage1 = nullptr;
    napi_create_function(env, "callback", NAPI_AUTO_LENGTH, ThrowingCallback, nullptr, &nativeMessage1);
    napi_value argv[] = {nativeMessage1, nativeMessage0};
    napi_value setTimeoutCB = GetGlobalProperty(env, "setTimeout");
    napi_value tId = nullptr;
    napi_call_function(env, nullptr, setTimeoutCB, argc, argv, &tId);
    ASSERT_CHECK_VALUE_TYPE(env, tId, napi_number);
}

/* @tc.name: settimeout
 * @tc.desc: Test settimeout should not be change.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TimerTest018, testing::ext::TestSize.Level0)
{
    NativeEngineProxy env;
    thread_local auto cleanUpData = new TestData();
    cleanUpData->env_ = env;
    napi_add_env_cleanup_hook(env, [](void * data) {
        auto that = reinterpret_cast<TestData*>(cleanUpData);
        size_t argc = 2;
        napi_value nativeMessage0 = nullptr;
        napi_value nativeMessage1 = nullptr;
        napi_create_function(that->env_, "callback", NAPI_AUTO_LENGTH, TimerAbort, nullptr, &nativeMessage1);
        napi_create_uint32(that->env_, 0, &nativeMessage0);
        napi_value argv[] = {nativeMessage1, nativeMessage0};
        napi_value setTimeoutCB = GetGlobalProperty(that->env_, "setTimeout");
        napi_value tId = nullptr;
        napi_call_function(that->env_, nullptr, setTimeoutCB, argc, argv, &tId);
        ASSERT_CHECK_VALUE_TYPE(that->env_, tId, napi_number);
    }, cleanUpData);
}

/* @tc.name: setInterval
 * @tc.desc: Test setInterval should not be change.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TimerTest019, testing::ext::TestSize.Level0)
{
    NativeEngineProxy env;
    thread_local auto cleanData = new TestData();
    cleanData->env_ = env;
    napi_add_env_cleanup_hook(env, [](void * data) {
        napi_value nativeMessage1 = nullptr;
        auto that = reinterpret_cast<TestData*>(cleanData);
        size_t argc = 2;
        napi_create_function(that->env_, "callback", NAPI_AUTO_LENGTH, TimerAbort, nullptr, &nativeMessage1);
        napi_value nativeMessage0 = nullptr;
        napi_create_uint32(that->env_, 0, &nativeMessage0);
        napi_value argv[] = {nativeMessage1, nativeMessage0};
        napi_value setIntervalCB = GetGlobalProperty(that->env_, "setInterval");
        napi_value tId = nullptr;
        napi_call_function(that->env_, nullptr, setIntervalCB, argc, argv, &tId);
        ASSERT_CHECK_VALUE_TYPE(that->env_, tId, napi_number);
    }, cleanData);
}

/* @tc.name: timer
 * @tc.desc: Test timer can be write.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TimerTest020, testing::ext::TestSize.Level0)
{
    NativeEngineProxy env;
    napi_value func1 = nullptr;
    napi_status status = napi_create_function(env, "setTimeout", NAPI_AUTO_LENGTH,
                                              TimerTest::SetTimeout, nullptr, &func1);
    ASSERT_TRUE(status == napi_ok);
    napi_value func2 = nullptr;
    status = napi_create_function(env, "setInterval", NAPI_AUTO_LENGTH, TimerTest::SetInterval, nullptr, &func2);
    ASSERT_TRUE(status == napi_ok);
    napi_value func3 = nullptr;
    status = napi_create_function(env, "clearTimeout", NAPI_AUTO_LENGTH, TimerTest::ClearTimer, nullptr, &func3);
    ASSERT_TRUE(status == napi_ok);
    napi_value func4 = nullptr;
    status = napi_create_function(env, "clearInterval", NAPI_AUTO_LENGTH, TimerTest::ClearTimer, nullptr, &func4);
    ASSERT_TRUE(status == napi_ok);
    napi_property_descriptor properties[] = {
        {"setTimeout", nullptr, nullptr, nullptr, nullptr, func1, napi_default_jsproperty, nullptr},
        {"setInterval", nullptr, nullptr, nullptr, nullptr, func2, napi_default_jsproperty, nullptr},
        {"clearTimeout", nullptr, nullptr, nullptr, nullptr, func3, napi_default_jsproperty, nullptr},
        {"clearInterval", nullptr, nullptr, nullptr, nullptr, func4, napi_default_jsproperty, nullptr}
    };
    napi_value globalObj = Helper::NapiHelper::GetGlobalObject(env);
    status = napi_define_properties(env, globalObj, sizeof(properties) / sizeof(properties[0]), properties);
    ASSERT_TRUE(status == napi_ok);
    napi_value setTimeoutCB = GetGlobalProperty(env, "setTimeout");
    napi_value setIntervalCB = GetGlobalProperty(env, "setInterval");
    napi_value clearTimeoutCB = GetGlobalProperty(env, "clearTimeout");
    napi_value clearIntervalCB = GetGlobalProperty(env, "clearInterval");
    bool isEqual = false;
    napi_strict_equals(env, setTimeoutCB, func1, &isEqual);
    ASSERT_TRUE(isEqual);
    napi_strict_equals(env, setIntervalCB, func2, &isEqual);
    ASSERT_TRUE(isEqual);
    napi_strict_equals(env, clearTimeoutCB, func3, &isEqual);
    ASSERT_TRUE(isEqual);
    napi_strict_equals(env, clearIntervalCB, func4, &isEqual);
    ASSERT_TRUE(isEqual);
}

/* @tc.name: settimeout
 * @tc.desc: Test settimeout can be write.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TimerTest021, testing::ext::TestSize.Level0)
{
    NativeEngineProxy env;
    napi_value func = nullptr;
    napi_status status = napi_create_function(env, "setTimeout", NAPI_AUTO_LENGTH,
                                              TimerTest::SetTimeout, nullptr, &func);
    ASSERT_TRUE(status == napi_ok);
    napi_value globalObj = Helper::NapiHelper::GetGlobalObject(env);
    ASSERT_TRUE(napi_set_named_property(env, globalObj, "setTimeout", func) == napi_ok);
}

/* @tc.name: setInterval
 * @tc.desc: Test setInterval can be write.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TimerTest022, testing::ext::TestSize.Level0)
{
    NativeEngineProxy env;
    napi_value func = nullptr;
    napi_status status = napi_create_function(env, "setInterval", NAPI_AUTO_LENGTH,
                                              TimerTest::SetInterval, nullptr, &func);
    ASSERT_TRUE(status == napi_ok);
    napi_value globalObj = Helper::NapiHelper::GetGlobalObject(env);
    ASSERT_TRUE(napi_set_named_property(env, globalObj, "setInterval", func) == napi_ok);
}

/* @tc.name: clearTimeout
 * @tc.desc: Test clearTimeout can be write.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TimerTest023, testing::ext::TestSize.Level0)
{
    NativeEngineProxy env;
    napi_value func = nullptr;
    napi_status status = napi_create_function(env, "clearTimeout", NAPI_AUTO_LENGTH,
                                              TimerTest::ClearTimer, nullptr, &func);
    ASSERT_TRUE(status == napi_ok);
    napi_value globalObj = Helper::NapiHelper::GetGlobalObject(env);
    ASSERT_TRUE(napi_set_named_property(env, globalObj, "clearTimeout", func) == napi_ok);
}

/* @tc.name: clearInterval
 * @tc.desc: Test clearInterval can be write.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TimerTest024, testing::ext::TestSize.Level0)
{
    NativeEngineProxy env;
    napi_value func = nullptr;
    napi_status status = napi_create_function(env, "clearInterval", NAPI_AUTO_LENGTH,
                                              TimerTest::ClearTimer, nullptr, &func);
    ASSERT_TRUE(status == napi_ok);
    napi_value globalObj = Helper::NapiHelper::GetGlobalObject(env);
    ASSERT_TRUE(napi_set_named_property(env, globalObj, "clearInterval", func) == napi_ok);
}