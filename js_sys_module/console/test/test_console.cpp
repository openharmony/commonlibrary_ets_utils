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
napi_value ConsoleTest::TraceHybridStack(napi_env env, napi_callback_info info)
{
    return Console::TraceHybridStack(env, info);
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

/* @tc.name: Console.Log/Console.Info/Console.debug/Console.error/Console.warn Test001
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 2;
    std::string message = "console test %d";
    napi_value nativeMessage0 = nullptr;
    napi_create_string_utf8(env, message.c_str(), message.length(), &nativeMessage0);
    napi_value nativeMessage1 = nullptr;
    napi_create_uint32(env, 5, &nativeMessage1);
    napi_value argv[] = {nativeMessage0, nativeMessage1};

    std::string funcName = "ConsoleLog";
    napi_value cb = nullptr;
    napi_value res0 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(),
                         ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res0);
    ASSERT_CHECK_VALUE_TYPE(env, res0, napi_undefined);

    cb = nullptr;
    napi_value res1 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(),
                         ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::DEBUG>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res1);
    ASSERT_CHECK_VALUE_TYPE(env, res1, napi_undefined);

    cb = nullptr;
    napi_value res2 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(),
                         ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::ERROR>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res2);
    ASSERT_CHECK_VALUE_TYPE(env, res2, napi_undefined);

    cb = nullptr;
    napi_value res3 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(),
                         ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::WARN>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res3);
    ASSERT_CHECK_VALUE_TYPE(env, res3, napi_undefined);

    cb = nullptr;
    napi_value res4 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(),
                         ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::FATAL>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res4);
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
    napi_value res0 = nullptr;
    std::string funcName = "ConsoleLog";
    napi_value cb = nullptr;
    std::string message = "console test %d";
    napi_value nativeMessage0 = nullptr;
    napi_create_string_utf8(env, message.c_str(), message.length(), &nativeMessage0);
    napi_value nativeMessage1 = nullptr;
    napi_create_uint32(env, 5, &nativeMessage1);
    napi_value argv[] = {nativeMessage0, nativeMessage1};

    napi_create_function(env, funcName.c_str(), funcName.size(),
                         ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res0);
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
    napi_value res0 = nullptr;
    std::string funcName = "ConsoleLog";
    napi_value cb = nullptr;
    std::string message = "console test %s";
    napi_value nativeMessage0 = nullptr;
    napi_create_string_utf8(env, message.c_str(), message.length(), &nativeMessage0);
    napi_value nativeMessage1 = nullptr;
    napi_create_uint32(env, 5, &nativeMessage1);
    napi_value argv[] = {nativeMessage0, nativeMessage1};

    napi_create_function(env, funcName.c_str(), funcName.size(),
                         ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res0);
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
    napi_value res0 = nullptr;
    std::string funcName = "ConsoleLog";
    napi_value cb = nullptr;
    std::string message = "console test %j";
    napi_value nativeMessage0 = nullptr;
    napi_create_string_utf8(env, message.c_str(), message.length(), &nativeMessage0);
    napi_value nativeMessage1 = nullptr;
    napi_create_uint32(env, 5, &nativeMessage1);
    napi_value argv[] = {nativeMessage0, nativeMessage1};

    napi_create_function(env, funcName.c_str(), funcName.size(),
                         ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res0);
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
    napi_value res0 = nullptr;
    std::string funcName = "ConsoleLog";
    napi_value cb = nullptr;
    std::string message = "console test %O";
    napi_value nativeMessage0 = nullptr;
    napi_create_string_utf8(env, message.c_str(), message.length(), &nativeMessage0);
    napi_value nativeMessage1 = nullptr;
    napi_create_uint32(env, 5, &nativeMessage1);
    napi_value argv[] = {nativeMessage0, nativeMessage1};
    napi_create_function(env, funcName.c_str(), funcName.size(),
                         ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res0);
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
    napi_value res0 = nullptr;
    std::string funcName = "ConsoleLog";
    napi_value cb = nullptr;
    std::string message = "console test %o";
    napi_value nativeMessage0 = nullptr;
    napi_create_string_utf8(env, message.c_str(), message.length(), &nativeMessage0);
    napi_value nativeMessage1 = nullptr;
    napi_create_uint32(env, 5, &nativeMessage1);
    napi_value argv[] = {nativeMessage0, nativeMessage1};

    napi_create_function(env, funcName.c_str(), funcName.size(),
                         ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res0);
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
    napi_value res0 = nullptr;
    std::string funcName = "ConsoleLog";
    napi_value cb = nullptr;
    std::string message = "console test %i";
    napi_value nativeMessage0 = nullptr;
    napi_create_string_utf8(env, message.c_str(), message.length(), &nativeMessage0);
    napi_value nativeMessage1 = nullptr;
    napi_create_uint32(env, 5, &nativeMessage1);
    napi_value argv[] = {nativeMessage0, nativeMessage1};

    napi_create_function(env, funcName.c_str(), funcName.size(),
                         ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res0);
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
    napi_value res0 = nullptr;
    std::string funcName = "ConsoleLog";
    napi_value cb = nullptr;
    std::string message = "console test %f";
    napi_value nativeMessage0 = nullptr;
    napi_create_string_utf8(env, message.c_str(), message.length(), &nativeMessage0);
    napi_value nativeMessage1 = nullptr;
    napi_create_uint32(env, 8, &nativeMessage1);
    napi_value argv[] = {nativeMessage0, nativeMessage1};

    napi_create_function(env, funcName.c_str(), funcName.size(),
                         ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res0);
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
    napi_value res0 = nullptr;
    std::string funcName = "ConsoleLog";
    napi_value cb = nullptr;
    std::string message = "console test %c";
    napi_value nativeMessage0 = nullptr;
    napi_create_string_utf8(env, message.c_str(), message.length(), &nativeMessage0);
    napi_value nativeMessage1 = nullptr;
    napi_create_uint32(env, 5, &nativeMessage1);
    napi_value argv[] = {nativeMessage0, nativeMessage1};

    napi_create_function(env, funcName.c_str(), funcName.size(),
                         ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res0);
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
    napi_value res0 = nullptr;
    std::string funcName = "ConsoleLog";
    napi_value cb = nullptr;
    std::string message = "console test %%";
    napi_value nativeMessage0 = nullptr;
    napi_create_string_utf8(env, message.c_str(), message.length(), &nativeMessage0);
    napi_value nativeMessage1 = nullptr;
    napi_create_uint32(env, 5, &nativeMessage1);
    napi_value argv[] = {nativeMessage0, nativeMessage1};

    napi_create_function(env, funcName.c_str(), funcName.size(),
                         ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res0);
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
    napi_value res0 = nullptr;
    std::string funcName = "ConsoleLog";
    napi_value cb = nullptr;
    std::string message = "console test %r";
    napi_value nativeMessage0 = nullptr;
    napi_create_string_utf8(env, message.c_str(), message.length(), &nativeMessage0);
    napi_value nativeMessage1 = nullptr;
    napi_create_uint32(env, 8, &nativeMessage1);
    napi_value argv[] = {nativeMessage0, nativeMessage1};

    napi_create_function(env, funcName.c_str(), funcName.size(),
                         ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res0);
    ASSERT_CHECK_VALUE_TYPE(env, res0, napi_undefined);
}

/* @tc.name: Console.count/Console.countReset/
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest012, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 0;
    napi_value res = nullptr;
    std::string funcName = "Count";
    napi_value argv[] = {nullptr};
    napi_value cb = nullptr;

    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Count, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);

    size_t argc2 = 1;
    std::string message = "abc"; // random value
    napi_value nativeMessage0 = nullptr;
    napi_create_string_utf8(env, message.c_str(), message.length(), &nativeMessage0);
    napi_value argv2[] = {nativeMessage0};
    cb = nullptr;
    napi_value res0 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Count, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc2, argv2, &res0);
    ASSERT_CHECK_VALUE_TYPE(env, res0, napi_undefined);

    cb = nullptr;
    funcName = "CountReset";
    napi_value res1 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::CountReset, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc2, argv2, &res1);
    ASSERT_CHECK_VALUE_TYPE(env, res1, napi_undefined);

    cb = nullptr;
    funcName = "CountReset";
    napi_value res2 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::CountReset, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc2, argv2, &res2);
    ASSERT_CHECK_VALUE_TYPE(env, res2, napi_undefined);
}

/* @tc.name: Console.Dir
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest013, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 0;
    napi_value res0 = nullptr;
    std::string funcName = "Dir";
    napi_value argv[] = {nullptr};
    napi_value cb = nullptr;

    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Dir, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res0);
    ASSERT_CHECK_VALUE_TYPE(env, res0, napi_undefined);

    argc = 1;
    napi_value res1 = nullptr;
    napi_value nativeMessage = nullptr;
    napi_create_uint32(env, 5, &nativeMessage);
    napi_value argv2[] = {nativeMessage};
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Dir, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv2, &res1);
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
    napi_value res0 = nullptr;
    std::string funcName = "Group";
    napi_value argv1[] = {nullptr};
    napi_value cb = nullptr;
    
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Group, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv1, &res0);
    ASSERT_CHECK_VALUE_TYPE(env, res0, napi_undefined);

    argc = 1;
    napi_value nativeMessage = nullptr;
    napi_create_uint32(env, 5, &nativeMessage);
    napi_value argv[] = {nativeMessage, nativeMessage};

    funcName = "Group";
    cb = nullptr;
    napi_value res1 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Group, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res1);
    ASSERT_CHECK_VALUE_TYPE(env, res1, napi_undefined);

    funcName = "GroupEnd";
    cb = nullptr;
    napi_value res2 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Group, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res2);
    ASSERT_CHECK_VALUE_TYPE(env, res2, napi_undefined);

    funcName = "GroupEnd";
    cb = nullptr;
    napi_value res3 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Group, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res3);
    ASSERT_CHECK_VALUE_TYPE(env, res3, napi_undefined);
}

/* @tc.name: Console.Table
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest015, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value res0 = nullptr;
    size_t argc = 0;
    std::string funcName = "Table";
    napi_value argv[] = {nullptr};
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Table, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res0);
    ASSERT_CHECK_VALUE_TYPE(env, res0, napi_undefined);

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
    napi_value argv2[] = {tabularData};
    napi_value res3 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Table, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv2, &res3);
    ASSERT_CHECK_VALUE_TYPE(env, res3, napi_undefined);
}

/* @tc.name: Console.Time/Timelog/TimeEnd
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest016, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 0;
    napi_value res0 = nullptr;
    std::string funcName = "Time";
    napi_value argv[] = {nullptr};
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Time, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res0);
    ASSERT_CHECK_VALUE_TYPE(env, res0, napi_undefined);

    size_t argc1 = 1;
    std::string timerName = "abc"; // Random value
    napi_value nativeMessage0 = nullptr;
    napi_create_string_utf8(env, timerName.c_str(), timerName.length(), &nativeMessage0);
    napi_value argv1[] = {nativeMessage0};

    cb = nullptr;
    napi_value res2 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Time, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc1, argv1, &res2);
    ASSERT_CHECK_VALUE_TYPE(env, res2, napi_undefined);

    cb = nullptr;
    napi_value res3 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Time, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc1, argv1, &res3);
    ASSERT_CHECK_VALUE_TYPE(env, res3, napi_undefined);

    size_t argc2 = 2;
    std::string log = "timeLog"; // Random value
    napi_value nativeMessage1 = nullptr;
    napi_create_string_utf8(env, log.c_str(), log.length(), &nativeMessage1);
    napi_value argv2[] = {nativeMessage0, nativeMessage1};

    cb = nullptr;
    funcName = "TimeLog";
    napi_value res = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::TimeLog, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc2, argv2, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);

    cb = nullptr;
    funcName = "TimeEnd";
    napi_value res4 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::TimeEnd, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc2, argv2, &res4);
    ASSERT_CHECK_VALUE_TYPE(env, res4, napi_undefined);

    cb = nullptr;
    funcName = "TimeLog";
    napi_value res5 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::TimeLog, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc2, argv2, &res5);
    ASSERT_CHECK_VALUE_TYPE(env, res5, napi_undefined);

    cb = nullptr;
    funcName = "TimeEnd";
    napi_value res6 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::TimeEnd, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc2, argv2, &res6);
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
    napi_value res0 = nullptr;
    std::string funcName = "Trace";
    napi_value argv[] = {nullptr};
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Trace, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res0);
    ASSERT_CHECK_VALUE_TYPE(env, res0, napi_undefined);

    argc = 1;
    std::string message = "abc"; // Random value
    napi_value nativeMessage1 = nullptr;
    napi_create_string_utf8(env, message.c_str(), message.length(), &nativeMessage1);
    napi_value argv2[] = {nativeMessage1};

    cb = nullptr;
    napi_value res1 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Trace, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv2, &res1);
    ASSERT_CHECK_VALUE_TYPE(env, res1, napi_undefined);
}

/* @tc.name: Console.Assert
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest018, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 0;
    napi_value res0 = nullptr;
    std::string funcName = "Assert";
    napi_value argv[] = {nullptr};
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Assert, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res0);
    ASSERT_CHECK_VALUE_TYPE(env, res0, napi_undefined);

    napi_value nativeMessage0 = nullptr;
    size_t argc1 = 1;
    napi_get_boolean(env, 1, &nativeMessage0);
    napi_value argv1[] = {nativeMessage0};

    cb = nullptr;
    napi_value res1 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Assert, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc1, argv1, &res1);
    ASSERT_CHECK_VALUE_TYPE(env, res1, napi_undefined);

    cb = nullptr;
    napi_value res2 = nullptr;
    napi_value nativeMessage1 = nullptr;
    napi_get_boolean(env, 0, &nativeMessage1); // argc = 1 && False
    napi_value argv2[] = {nativeMessage1};
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Assert, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc1, argv2, &res2);
    ASSERT_CHECK_VALUE_TYPE(env, res2, napi_undefined);

    size_t argc2 = 2;
    cb = nullptr;
    napi_value res3 = nullptr;
    napi_value nativeMessage3 = nullptr;
    napi_get_boolean(env, 0, &nativeMessage3); // && False
    std::string message = "log"; // Message to print
    napi_value nativeMessage4 = nullptr;
    napi_create_string_utf8(env, message.c_str(), message.length(), &nativeMessage4);
    napi_value argv3[] = {nativeMessage3, nativeMessage4};
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Assert, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc2, argv3, &res3);
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
    napi_value argv[] = {nullptr};

    std::string funcName = "ConsoleLog";
    napi_value cb = nullptr;
    napi_value res0 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(),
                         ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res0);
    ASSERT_CHECK_VALUE_TYPE(env, res0, napi_undefined);

    cb = nullptr;
    napi_value res1 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(),
                         ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::DEBUG>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res1);
    ASSERT_CHECK_VALUE_TYPE(env, res1, napi_undefined);

    cb = nullptr;
    napi_value res2 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(),
                         ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::ERROR>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res2);
    ASSERT_CHECK_VALUE_TYPE(env, res2, napi_undefined);

    cb = nullptr;
    napi_value res3 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(),
                         ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::WARN>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res3);
    ASSERT_CHECK_VALUE_TYPE(env, res3, napi_undefined);

    cb = nullptr;
    napi_value res4 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(),
                         ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::FATAL>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res4);
    ASSERT_CHECK_VALUE_TYPE(env, res4, napi_undefined);
}

/* @tc.name: Console.TraceHybridStack
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest022, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 0;
    napi_value res0 = nullptr;
    std::string funcName = "TraceHybridStack";
    napi_value argv[] = {nullptr};
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::TraceHybridStack, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res0);
    ASSERT_CHECK_VALUE_TYPE(env, res0, napi_undefined);

    argc = 1;
    std::string message = "abc"; // Random value
    napi_value nativeMessage1 = nullptr;
    napi_create_string_utf8(env, message.c_str(), message.length(), &nativeMessage1);
    napi_value argv2[] = {nativeMessage1};

    cb = nullptr;
    napi_value res1 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::TraceHybridStack, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv2, &res1);
    ASSERT_CHECK_VALUE_TYPE(env, res1, napi_undefined);
}