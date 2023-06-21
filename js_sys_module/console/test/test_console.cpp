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
#include "test_console.h"

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

template<LogLevel LEVEL>
napi_value ConsoleTest::ConsoleLog(napi_env env, napi_callback_info info)
{
    return Console::ConsoleLog<LEVEL>(env, info);
}
napi_value ConsoleTest::Count(napi_env env, napi_callback_info info)
{
    return Console::Count(env, info);
}
napi_value ConsoleTest::CountReset(napi_env env, napi_callback_info info)
{
    return Console::CountReset(env, info);
}
napi_value ConsoleTest::Dir(napi_env env, napi_callback_info info)
{
    return Console::Dir(env, info);
}
napi_value ConsoleTest::Group(napi_env env, napi_callback_info info)
{
    return Console::Group(env, info);
}
napi_value ConsoleTest::GroupEnd(napi_env env, napi_callback_info info)
{
    return Console::GroupEnd(env, info);
}
napi_value ConsoleTest::Table(napi_env env, napi_callback_info info)
{
    return Console::Table(env, info);
}
napi_value ConsoleTest::Time(napi_env env, napi_callback_info info)
{
    return Console::Time(env, info);
}
napi_value ConsoleTest::TimeLog(napi_env env, napi_callback_info info)
{
    return Console::TimeLog(env, info);
}
napi_value ConsoleTest::TimeEnd(napi_env env, napi_callback_info info)
{
    return Console::TimeEnd(env, info);
}
napi_value ConsoleTest::Trace(napi_env env, napi_callback_info info)
{
    return Console::Trace(env, info);
}
napi_value ConsoleTest::Assert(napi_env env, napi_callback_info info)
{
    return Console::Assert(env, info);
}
void ConsoleTest::PrintTime(std::string timerName, double time, std::string& log)
{
    Console::PrintTime(timerName, time, log);
}

napi_value StrToNapiValue(napi_env env, const std::string &result)
{
    napi_value output = nullptr;
    napi_create_string_utf8(env, result.c_str(), result.size(), &output);
    return output;
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
/* @tc.name: Console.Log/Console.Info/Console.debug/Console.error/Console.warn Test001
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 2;
    std::string message = "console test %d";
    NativeValue* nativeMessage0 =  engine_->CreateString(message.c_str(), message.length());
    NativeValue* nativeMessage1 = engine_->CreateNumber(5); // Random number
    NativeValue* argv[] = {nativeMessage0, nativeMessage1};

    napi_callback_info cbinfo = napi_create_cbinfo(env, &argc, argv);
    napi_value res0 = ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>(env, cbinfo);
    ASSERT_CHECK_VALUE_TYPE(env, res0, napi_undefined);
    napi_value res1 = ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::DEBUG>(env, cbinfo);
    ASSERT_CHECK_VALUE_TYPE(env, res1, napi_undefined);
    napi_value res2 = ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::ERROR>(env, cbinfo);
    ASSERT_CHECK_VALUE_TYPE(env, res2, napi_undefined);
    napi_value res3 =  ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::WARN>(env, cbinfo);
    ASSERT_CHECK_VALUE_TYPE(env, res3, napi_undefined);
    napi_value res4 = ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::FATAL>(env, cbinfo);
    ASSERT_CHECK_VALUE_TYPE(env, res4, napi_undefined);
}

/* @tc.name: OutputFormat test Test002
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 2;
    std::string message0 = "console test %d";
    NativeValue* nativeMessage0 = engine_->CreateString(message0.c_str(), message0.length());
    NativeValue* nativeMessage1 = engine_->CreateNumber(5); // Random number
    NativeValue* argv0[] = {nativeMessage0, nativeMessage1};
    napi_callback_info cbinfo0 = napi_create_cbinfo(env, &argc, argv0);
    napi_value res0 = ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>(env, cbinfo0);
    ASSERT_CHECK_VALUE_TYPE(env, res0, napi_undefined);
}

/* @tc.name: OutputFormat test Test003
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 2;
    std::string message0 = "console test %s";
    NativeValue* nativeMessage0 = engine_->CreateString(message0.c_str(), message0.length());
    NativeValue* nativeMessage1 = engine_->CreateNumber(5); // Random number
    NativeValue* argv0[] = {nativeMessage0, nativeMessage1};
    napi_callback_info cbinfo0 = napi_create_cbinfo(env, &argc, argv0);
    napi_value res0 = ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>(env, cbinfo0);
    ASSERT_CHECK_VALUE_TYPE(env, res0, napi_undefined);
}

/* @tc.name: OutputFormat test Test004
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest004, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 2;
    std::string message0 = "console test %j";
    NativeValue* nativeMessage0 = engine_->CreateString(message0.c_str(), message0.length());
    NativeValue* nativeMessage1 = engine_->CreateNumber(5); // Random number
    NativeValue* argv0[] = {nativeMessage0, nativeMessage1};
    napi_callback_info cbinfo0 = napi_create_cbinfo(env, &argc, argv0);
    napi_value res0 = ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>(env, cbinfo0);
    ASSERT_CHECK_VALUE_TYPE(env, res0, napi_undefined);
}

/* @tc.name: OutputFormat test Test005
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest005, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 2;
    std::string message0 = "console test %O";
    NativeValue* nativeMessage0 = engine_->CreateString(message0.c_str(), message0.length());
    NativeValue* nativeMessage1 = engine_->CreateNumber(5); // Random number
    NativeValue* argv0[] = {nativeMessage0, nativeMessage1};
    napi_callback_info cbinfo0 = napi_create_cbinfo(env, &argc, argv0);
    napi_value res0 = ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>(env, cbinfo0);
    ASSERT_CHECK_VALUE_TYPE(env, res0, napi_undefined);
}

/* @tc.name: OutputFormat test Test006
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest006, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 2;
    std::string message0 = "console test %o";
    NativeValue* nativeMessage0 = engine_->CreateString(message0.c_str(), message0.length());
    NativeValue* nativeMessage1 = engine_->CreateNumber(5); // Random number
    NativeValue* argv0[] = {nativeMessage0, nativeMessage1};
    napi_callback_info cbinfo0 = napi_create_cbinfo(env, &argc, argv0);
    napi_value res0 = ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>(env, cbinfo0);
    ASSERT_CHECK_VALUE_TYPE(env, res0, napi_undefined);
}

/* @tc.name: OutputFormat test Test007
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest007, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 2;
    std::string message0 = "console test %i";
    NativeValue* nativeMessage0 = engine_->CreateString(message0.c_str(), message0.length());
    NativeValue* nativeMessage1 = engine_->CreateNumber(5); // Random number
    NativeValue* argv0[] = {nativeMessage0, nativeMessage1};
    napi_callback_info cbinfo0 = napi_create_cbinfo(env, &argc, argv0);
    napi_value res0 = ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>(env, cbinfo0);
    ASSERT_CHECK_VALUE_TYPE(env, res0, napi_undefined);
}

/* @tc.name: OutputFormat test Test005
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest008, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 2;
    std::string message0 = "console test %f";
    NativeValue* nativeMessage0 = engine_->CreateString(message0.c_str(), message0.length());
    NativeValue* nativeMessage1 = engine_->CreateNumber(8); // Random number
    NativeValue* argv0[] = {nativeMessage0, nativeMessage1};
    napi_callback_info cbinfo0 = napi_create_cbinfo(env, &argc, argv0);
    napi_value res0 = ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>(env, cbinfo0);
    ASSERT_CHECK_VALUE_TYPE(env, res0, napi_undefined);
}

/* @tc.name: OutputFormat test Test009
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest009, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 2;
    std::string message0 = "console test %c";
    NativeValue* nativeMessage0 = engine_->CreateString(message0.c_str(), message0.length());
    NativeValue* nativeMessage1 = engine_->CreateNumber(8); // Random number
    NativeValue* argv0[] = {nativeMessage0, nativeMessage1};
    napi_callback_info cbinfo0 = napi_create_cbinfo(env, &argc, argv0);
    napi_value res0 = ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>(env, cbinfo0);
    ASSERT_CHECK_VALUE_TYPE(env, res0, napi_undefined);
}

/* @tc.name: OutputFormat test Test010
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest010, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 2;
    std::string message0 = "console test %%";
    NativeValue* nativeMessage0 = engine_->CreateString(message0.c_str(), message0.length());
    NativeValue* nativeMessage1 = engine_->CreateNumber(8); // Random number
    NativeValue* argv0[] = {nativeMessage0, nativeMessage1};
    napi_callback_info cbinfo0 = napi_create_cbinfo(env, &argc, argv0);
    napi_value res0 = ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>(env, cbinfo0);
    ASSERT_CHECK_VALUE_TYPE(env, res0, napi_undefined);
}

/* @tc.name: OutputFormat test Test011
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest011, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 2;
    std::string message0 = "console test %r";
    NativeValue* nativeMessage0 = engine_->CreateString(message0.c_str(), message0.length());
    NativeValue* nativeMessage1 = engine_->CreateNumber(8);
    NativeValue* argv0[] = {nativeMessage0, nativeMessage1};
    napi_callback_info cbinfo0 = napi_create_cbinfo(env, &argc, argv0);
    napi_value res0 = ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>(env, cbinfo0);
    ASSERT_CHECK_VALUE_TYPE(env, res0, napi_undefined);
}

/* @tc.name: Console.count/Console.countReset/
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest012, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc1 = 0;
    NativeValue* argv1[] = {nullptr};
    napi_callback_info cbinfo1 = napi_create_cbinfo(env, &argc1, argv1);
    napi_value res0 = ConsoleTest::Count(env, cbinfo1);  // test default
    ASSERT_CHECK_VALUE_TYPE(env, res0, napi_undefined);

    size_t argc2 = 1;
    std::string counterName = "abc"; // random value
    NativeValue* nativeMessage = engine_->CreateString(counterName.c_str(), counterName.length());
    NativeValue* argv2[] = {nativeMessage};
    napi_callback_info cbinfo2 = napi_create_cbinfo(env, &argc2, argv2);
    napi_value res1 = ConsoleTest::Count(env, cbinfo2);
    ASSERT_CHECK_VALUE_TYPE(env, res1, napi_undefined);
    napi_value res2 = ConsoleTest::CountReset(env, cbinfo2);
    ASSERT_CHECK_VALUE_TYPE(env, res2, napi_undefined);
    napi_value res3 = ConsoleTest::CountReset(env, cbinfo2); // test re-reset
    ASSERT_CHECK_VALUE_TYPE(env, res3, napi_undefined);
}

/* @tc.name: Console.Dir
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest013, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;

    size_t argc = 0;
    NativeValue* argv1[] = {nullptr};
    napi_callback_info cbinfo1 = napi_create_cbinfo(env, &argc, argv1);
    napi_value res0 = ConsoleTest::Dir(env, cbinfo1);
    ASSERT_CHECK_VALUE_TYPE(env, res0, napi_undefined);

    argc = 1;
    NativeValue* nativeMessage = engine_->CreateNumber(5);
    NativeValue* argv2[] = {nativeMessage};
    napi_callback_info cbinfo2 = napi_create_cbinfo(env, &argc, argv2);
    napi_value res1 = ConsoleTest::Dir(env, cbinfo2);
    ASSERT_CHECK_VALUE_TYPE(env, res1, napi_undefined);
}

/* @tc.name: Console.Group/Console.GroupEnd
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest014, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 0;
    NativeValue* argv1[] = {nullptr};
    napi_callback_info cbinfo1 = napi_create_cbinfo(env, &argc, argv1);
    napi_value res1 = ConsoleTest::Group(env, cbinfo1);
    ASSERT_CHECK_VALUE_TYPE(env, res1, napi_undefined);

    argc = 1;
    NativeValue* nativeMessage = engine_->CreateNumber(5); // Random number
    NativeValue* argv2[] = {nativeMessage, nativeMessage};
    napi_callback_info cbinfo2 = napi_create_cbinfo(env, &argc, argv2);
    napi_value res2 = ConsoleTest::Group(env, cbinfo2);
    ASSERT_CHECK_VALUE_TYPE(env, res2, napi_undefined);
    napi_value res3 = ConsoleTest::GroupEnd(env, cbinfo2);
    ASSERT_CHECK_VALUE_TYPE(env, res3, napi_undefined);
    napi_value res4 = ConsoleTest::GroupEnd(env, cbinfo2); // test length < GROUPINDENTATION
    ASSERT_CHECK_VALUE_TYPE(env, res4, napi_undefined);
    napi_value res5 = ConsoleTest::GroupEnd(env, cbinfo2);
    ASSERT_CHECK_VALUE_TYPE(env, res5, napi_undefined);
}

/* @tc.name: Console.Table
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest015, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;

    size_t argc = 0;
    NativeValue* argv1[] = {nullptr};
    napi_callback_info cbinfo1 = napi_create_cbinfo(env, &argc, argv1);
    napi_value res1 = ConsoleTest::Table(env, cbinfo1);
    ASSERT_CHECK_VALUE_TYPE(env, res1, napi_undefined);

    napi_value tabularData = nullptr;
    uint32_t length0 = 2;
    napi_create_array_with_length(env, length0, &tabularData);
    napi_value number = nullptr;
    napi_create_int32(env, 5, &number); // Random number
    napi_value array1 = nullptr;
    uint32_t length1 = 2;
    napi_create_array_with_length(env, length1, &array1);
    napi_set_element(env, array1, 0, number);
    napi_set_element(env, array1, 1, number);
    napi_set_named_property(env, tabularData, "number", array1);

    napi_value array2 = nullptr;
    uint32_t length2 = 2;
    napi_create_array_with_length(env, length2, &array2);
    napi_value strVal = StrToNapiValue(env, "name"); // random value
    napi_set_element(env, array2, 0, strVal);
    napi_set_element(env, array2, 1, strVal);
    napi_set_named_property(env, tabularData, "string", array2);

    argc = 1;
    NativeValue* nativeMessage = reinterpret_cast<NativeValue*>(tabularData);
    NativeValue* argv2[] = {nativeMessage};
    napi_callback_info cbinfo2 = napi_create_cbinfo(env, &argc, argv2);
    napi_value res2 = ConsoleTest::Table(env, cbinfo2);
    ASSERT_CHECK_VALUE_TYPE(env, res2, napi_undefined);
}

/* @tc.name: Console.Time/Timelog/TimeEnd
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest016, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc0 = 0;
    NativeValue* argv0[] = {nullptr};
    napi_callback_info cbinfo0 = napi_create_cbinfo(env, &argc0, argv0);
    napi_value res0 = ConsoleTest::Time(env, cbinfo0); // Test default
    ASSERT_CHECK_VALUE_TYPE(env, res0, napi_undefined);

    size_t argc1 = 1;
    std::string timerName = "abc"; // Random value
    NativeValue* nativeMessage = engine_->CreateString(timerName.c_str(), timerName.length());
    NativeValue* argv1[] = {nativeMessage};
    napi_callback_info cbinfo1 = napi_create_cbinfo(env, &argc1, argv1);
    napi_value res1 = ConsoleTest::Time(env, cbinfo1);
    ASSERT_CHECK_VALUE_TYPE(env, res1, napi_undefined);
    napi_value res2 = ConsoleTest::Time(env, cbinfo1); // Test repeat timerName, warn message check
    ASSERT_CHECK_VALUE_TYPE(env, res2, napi_undefined);

    size_t argc2 = 2;
    std::string log = "timeLog"; // Random value
    NativeValue* nativeMessage1 = engine_->CreateString(log.c_str(), log.length());
    NativeValue* argv2[] = {nativeMessage, nativeMessage1};
    napi_callback_info cbinfo2 = napi_create_cbinfo(env, &argc2, argv2);
    napi_value res3 = ConsoleTest::TimeLog(env, cbinfo2);
    ASSERT_CHECK_VALUE_TYPE(env, res3, napi_undefined);
    napi_value res4 = ConsoleTest::TimeEnd(env, cbinfo1); // Erase timer
    ASSERT_CHECK_VALUE_TYPE(env, res4, napi_undefined);
    napi_value res5 = ConsoleTest::TimeLog(env, cbinfo1); // Timer doesn't exists, warn message check
    ASSERT_CHECK_VALUE_TYPE(env, res5, napi_undefined);
    napi_value res6 = ConsoleTest::TimeEnd(env, cbinfo1); // Timer doesn't exists, warn message check
    ASSERT_CHECK_VALUE_TYPE(env, res6, napi_undefined);
}

/* @tc.name: Console.Trace
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest017, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;

    size_t argc = 0;
    NativeValue* argv1[] = {nullptr};
    napi_callback_info cbinfo1 = napi_create_cbinfo(env, &argc, argv1);
    napi_value res1 = ConsoleTest::Trace(env, cbinfo1);
    ASSERT_CHECK_VALUE_TYPE(env, res1, napi_undefined);

    argc = 1;
    std::string message = "abc"; // Random value
    NativeValue* nativeMessage = engine_->CreateString(message.c_str(), message.length());
    NativeValue* argv2[] = {nativeMessage};
    napi_callback_info cbinfo2 = napi_create_cbinfo(env, &argc, argv2);
    napi_value res2 = ConsoleTest::Trace(env, cbinfo2);
    ASSERT_CHECK_VALUE_TYPE(env, res2, napi_undefined);
}

/* @tc.name: Console.Assert
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest018, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    
    size_t argc0 = 0;
    NativeValue* argv0[] = {nullptr}; // No input
    napi_callback_info cbinfo0 = napi_create_cbinfo(env, &argc0, argv0);
    napi_value res0 = ConsoleTest::Assert(env, cbinfo0);
    ASSERT_CHECK_VALUE_TYPE(env, res0, napi_undefined);

    size_t argc1 = 1;
    NativeValue* nativeMessage0 = engine_->CreateBoolean(1); // True
    NativeValue* argv1[] = {nativeMessage0};
    napi_callback_info cbinfo1 = napi_create_cbinfo(env, &argc1, argv1);
    napi_value res1 = ConsoleTest::Assert(env, cbinfo1);
    ASSERT_CHECK_VALUE_TYPE(env, res1, napi_undefined);

    NativeValue* nativeMessage1 = engine_->CreateBoolean(0); // argc = 1 && False
    NativeValue* argv2[] = {nativeMessage1};
    napi_callback_info cbinfo2 = napi_create_cbinfo(env, &argc1, argv2);
    napi_value res2 = ConsoleTest::Assert(env, cbinfo2);
    ASSERT_CHECK_VALUE_TYPE(env, res2, napi_undefined);

    size_t argc2 = 2;
    NativeValue* nativeMessage3 = engine_->CreateBoolean(0); // False
    std::string message = "log"; // Message to print
    NativeValue* nativeMessage4 = engine_->CreateString(message.c_str(), message.length());
    NativeValue* argv3[] = {nativeMessage3, nativeMessage4};
    napi_callback_info cbinfo3 = napi_create_cbinfo(env, &argc2, argv3);
    napi_value res3 = ConsoleTest::Assert(env, cbinfo3);
    ASSERT_CHECK_VALUE_TYPE(env, res3, napi_undefined);
}

/* @tc.name: Init
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest019, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    Console::InitConsoleModule(env);
    bool res = 0;
    napi_is_exception_pending(env, &res);
    ASSERT_TRUE(!res);
}

/* @tc.name: PrintTime
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest020, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string name = "Timer1";
    std::string log = "log";

    double ms = 50;
    ConsoleTest::PrintTime(name, ms, log);
    bool res0 = 0;
    napi_is_exception_pending(env, &res0);
    ASSERT_TRUE(!res0);

    double seconds = 1 * 1000;
    ConsoleTest::PrintTime(name, seconds, log);
    bool res1 = 0;
    napi_is_exception_pending(env, &res1);
    ASSERT_TRUE(!res1);

    double minutes = 60 * seconds;
    ConsoleTest::PrintTime(name, minutes, log);
    bool res2 = 0;
    napi_is_exception_pending(env, &res2);
    ASSERT_TRUE(!res2);

    double hours = 60 * minutes;
    ConsoleTest::PrintTime(name, hours, log);
    bool res3 = 0;
    napi_is_exception_pending(env, &res3);
    ASSERT_TRUE(!res3);
}

/* @tc.name: Console.Log/Console.Info/Console.debug/Console.error/Console.warn
 * @tc.desc: Test.no input
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest021, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 0;
    NativeValue* argv[] = {nullptr};

    napi_callback_info cbinfo = napi_create_cbinfo(env, &argc, argv);
    napi_value res0 = ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>(env, cbinfo);
    ASSERT_CHECK_VALUE_TYPE(env, res0, napi_undefined);
    napi_value res1 = ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::DEBUG>(env, cbinfo);
    ASSERT_CHECK_VALUE_TYPE(env, res1, napi_undefined);
    napi_value res2 = ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::ERROR>(env, cbinfo);
    ASSERT_CHECK_VALUE_TYPE(env, res2, napi_undefined);
    napi_value res3 =  ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::WARN>(env, cbinfo);
    ASSERT_CHECK_VALUE_TYPE(env, res3, napi_undefined);
    napi_value res4 = ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::FATAL>(env, cbinfo);
    ASSERT_CHECK_VALUE_TYPE(env, res4, napi_undefined);
}