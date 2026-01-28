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
#include "test_console.h"

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
        Console::InitConsoleModule(reinterpret_cast<napi_env>(engine_));
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

napi_value GetGlobalProperty(napi_env env, const char *name)
{
    napi_value global;
    napi_get_global(env, &global);
    napi_value console = nullptr;
    napi_get_named_property(env, global, "console", &console);
    napi_value value = nullptr;
    napi_get_named_property(env, console, name, &value);
    return value;
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

void ConsoleTest::SetGroupIndent(const std::string& newGroupIndent)
{
    Console::groupIndent = newGroupIndent;
}

std::string ConsoleTest::GetGroupIndent()
{
    return Console::groupIndent;
}

std::string ConsoleTest::ParseLogContent(const std::vector<std::string>& params)
{
    return Console::ParseLogContent(params);
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
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::GroupEnd, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res2);
    ASSERT_CHECK_VALUE_TYPE(env, res2, napi_undefined);

    funcName = "GroupEnd";
    cb = nullptr;
    napi_value res3 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::GroupEnd, nullptr, &cb);
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

    size_t argc3 = 1;
    napi_value nativeMessage2 = nullptr;
    napi_value argv3[] = {nativeMessage2};
    cb = nullptr;
    funcName = "TimeEnd";
    napi_value res7 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::TimeEnd, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc3, argv3, &res7);
    ASSERT_CHECK_VALUE_TYPE(env, res7, napi_undefined);
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

/* @tc.name: GroupEnd
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest023, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 0;
    napi_value res0 = nullptr;
    std::string funcName = "GroupEnd";
    napi_value argv[] = {nullptr};
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::GroupEnd, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res0);
    ASSERT_CHECK_VALUE_TYPE(env, res0, napi_undefined);

    // Test case 1: Normal case, groupIndent size is greater than GROUPINDETATIONWIDTH
    constexpr size_t GROUPINDETATIONWIDTH = 2;
    ConsoleTest::SetGroupIndent(std::string(10 + GROUPINDETATIONWIDTH, ' '));
    napi_value argv1[] = {nullptr};
    napi_value res1 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::GroupEnd, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv1, &res1);
    ASSERT_CHECK_VALUE_TYPE(env, res1, napi_undefined);
    ASSERT_EQ(ConsoleTest::GetGroupIndent().size(), 10); // Check if groupIndent is correctly reduced
}

/* @tc.name: ConsoleLog
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest024, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 3;
    std::string message = "";
    napi_value nativeMessage0 = nullptr;
    napi_create_string_utf8(env, message.c_str(), message.length(), &nativeMessage0);
    napi_value nativeMessage1 = nullptr;
    std::string message2 = "test console";
    napi_value nativeMessage3 = nullptr;
    napi_create_string_utf8(env, message2.c_str(), message2.length(), &nativeMessage3);
    napi_value argv[] = {nativeMessage0, nativeMessage1, nativeMessage3};

    std::string funcName = "ConsoleLog";
    napi_value cb = nullptr;
    napi_value res0 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(),
                         ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res0);
    ASSERT_CHECK_VALUE_TYPE(env, res0, napi_undefined);
}

/* @tc.name: ParseLogContent
 * @tc.desc: Test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest025, testing::ext::TestSize.Level0)
{
    std::string res;
    std::vector<std::string> params;
    res = ConsoleTest::ParseLogContent(params);
    ASSERT_TRUE(res == "");
}

/* @tc.name: log
 * @tc.desc: Test log can be write.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest026, testing::ext::TestSize.Level0)
{
    NativeEngineProxy env;
    napi_value func = nullptr;
    napi_status status = napi_create_function(env, "log", NAPI_AUTO_LENGTH,
                                              ConsoleTest::ConsoleLog<LogLevel::INFO>, nullptr, &func);
    ASSERT_TRUE(status == napi_ok);
    napi_value globalObj = Helper::NapiHelper::GetGlobalObject(env);
    napi_value globalConsole = nullptr;
    napi_get_named_property(env, globalObj, "console", &globalConsole);
    ASSERT_TRUE(napi_set_named_property(env, globalConsole, "log", func) == napi_ok);
}

/* @tc.name: debug
 * @tc.desc: Test debug can be write.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest027, testing::ext::TestSize.Level0)
{
    NativeEngineProxy env;
    napi_value func = nullptr;
    napi_status status = napi_create_function(env, "debug", NAPI_AUTO_LENGTH,
                                              ConsoleTest::ConsoleLog<LogLevel::DEBUG>, nullptr, &func);
    ASSERT_TRUE(status == napi_ok);
    napi_value globalObj = Helper::NapiHelper::GetGlobalObject(env);
    napi_value globalConsole = nullptr;
    napi_get_named_property(env, globalObj, "console", &globalConsole);
    ASSERT_TRUE(napi_set_named_property(env, globalConsole, "debug", func) == napi_ok);
}

/* @tc.name: info
 * @tc.desc: Test info can be write.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest028, testing::ext::TestSize.Level0)
{
    NativeEngineProxy env;
    napi_value func = nullptr;
    napi_status status = napi_create_function(env, "info", NAPI_AUTO_LENGTH,
                                              ConsoleTest::ConsoleLog<LogLevel::INFO>, nullptr, &func);
    ASSERT_TRUE(status == napi_ok);
    napi_value globalObj = Helper::NapiHelper::GetGlobalObject(env);
    napi_value globalConsole = nullptr;
    napi_get_named_property(env, globalObj, "console", &globalConsole);
    ASSERT_TRUE(napi_set_named_property(env, globalConsole, "info", func) == napi_ok);
}

/* @tc.name: warn
 * @tc.desc: Test warn can be write.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest029, testing::ext::TestSize.Level0)
{
    NativeEngineProxy env;
    napi_value func = nullptr;
    napi_status status = napi_create_function(env, "warn", NAPI_AUTO_LENGTH,
                                              ConsoleTest::ConsoleLog<LogLevel::WARN>, nullptr, &func);
    ASSERT_TRUE(status == napi_ok);
    napi_value globalObj = Helper::NapiHelper::GetGlobalObject(env);
    napi_value globalConsole = nullptr;
    napi_get_named_property(env, globalObj, "console", &globalConsole);
    ASSERT_TRUE(napi_set_named_property(env, globalConsole, "warn", func) == napi_ok);
}

/* @tc.name: error
 * @tc.desc: Test error can be write.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest030, testing::ext::TestSize.Level0)
{
    NativeEngineProxy env;
    napi_value func = nullptr;
    napi_status status = napi_create_function(env, "error", NAPI_AUTO_LENGTH,
                                              ConsoleTest::ConsoleLog<LogLevel::ERROR>, nullptr, &func);
    ASSERT_TRUE(status == napi_ok);
    napi_value globalObj = Helper::NapiHelper::GetGlobalObject(env);
    napi_value globalConsole = nullptr;
    napi_get_named_property(env, globalObj, "console", &globalConsole);
    ASSERT_TRUE(napi_set_named_property(env, globalConsole, "error", func) == napi_ok);
}

/* @tc.name: fatal
 * @tc.desc: Test fatal can be write.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest031, testing::ext::TestSize.Level0)
{
    NativeEngineProxy env;
    napi_value func = nullptr;
    napi_status status = napi_create_function(env, "fatal", NAPI_AUTO_LENGTH,
                                              ConsoleTest::ConsoleLog<LogLevel::FATAL>, nullptr, &func);
    ASSERT_TRUE(status == napi_ok);
    napi_value globalObj = Helper::NapiHelper::GetGlobalObject(env);
    napi_value globalConsole = nullptr;
    napi_get_named_property(env, globalObj, "console", &globalConsole);
    ASSERT_TRUE(napi_set_named_property(env, globalConsole, "fatal", func) == napi_ok);
}

/* @tc.name: group
 * @tc.desc: Test group can be write.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest032, testing::ext::TestSize.Level0)
{
    NativeEngineProxy env;
    napi_value func = nullptr;
    napi_status status = napi_create_function(env, "group", NAPI_AUTO_LENGTH,
                                              ConsoleTest::Group, nullptr, &func);
    ASSERT_TRUE(status == napi_ok);
    napi_value globalObj = Helper::NapiHelper::GetGlobalObject(env);
    napi_value globalConsole = nullptr;
    napi_get_named_property(env, globalObj, "console", &globalConsole);
    ASSERT_TRUE(napi_set_named_property(env, globalConsole, "group", func) == napi_ok);
}

/* @tc.name: groupCollapsed
 * @tc.desc: Test groupCollapsed can be write.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest033, testing::ext::TestSize.Level0)
{
    NativeEngineProxy env;
    napi_value func = nullptr;
    napi_status status = napi_create_function(env, "groupCollapsed", NAPI_AUTO_LENGTH,
                                              ConsoleTest::Group, nullptr, &func);
    ASSERT_TRUE(status == napi_ok);
    napi_value globalObj = Helper::NapiHelper::GetGlobalObject(env);
    napi_value globalConsole = nullptr;
    napi_get_named_property(env, globalObj, "console", &globalConsole);
    ASSERT_TRUE(napi_set_named_property(env, globalConsole, "groupCollapsed", func) == napi_ok);
}

/* @tc.name: groupEnd
 * @tc.desc: Test groupEnd can be write.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest034, testing::ext::TestSize.Level0)
{
    NativeEngineProxy env;
    napi_value func = nullptr;
    napi_status status = napi_create_function(env, "groupEnd", NAPI_AUTO_LENGTH,
                                              ConsoleTest::GroupEnd, nullptr, &func);
    ASSERT_TRUE(status == napi_ok);
    napi_value globalObj = Helper::NapiHelper::GetGlobalObject(env);
    napi_value globalConsole = nullptr;
    napi_get_named_property(env, globalObj, "console", &globalConsole);
    ASSERT_TRUE(napi_set_named_property(env, globalConsole, "groupEnd", func) == napi_ok);
}

/* @tc.name: table
 * @tc.desc: Test table can be write.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest035, testing::ext::TestSize.Level0)
{
    NativeEngineProxy env;
    napi_value func = nullptr;
    napi_status status = napi_create_function(env, "table", NAPI_AUTO_LENGTH,
                                              ConsoleTest::Table, nullptr, &func);
    ASSERT_TRUE(status == napi_ok);
    napi_value globalObj = Helper::NapiHelper::GetGlobalObject(env);
    napi_value globalConsole = nullptr;
    napi_get_named_property(env, globalObj, "console", &globalConsole);
    ASSERT_TRUE(napi_set_named_property(env, globalConsole, "table", func) == napi_ok);
}

/* @tc.name: time
 * @tc.desc: Test time can be write.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest036, testing::ext::TestSize.Level0)
{
    NativeEngineProxy env;
    napi_value func = nullptr;
    napi_status status = napi_create_function(env, "time", NAPI_AUTO_LENGTH,
                                              ConsoleTest::Time, nullptr, &func);
    ASSERT_TRUE(status == napi_ok);
    napi_value globalObj = Helper::NapiHelper::GetGlobalObject(env);
    napi_value globalConsole = nullptr;
    napi_get_named_property(env, globalObj, "console", &globalConsole);
    ASSERT_TRUE(napi_set_named_property(env, globalConsole, "time", func) == napi_ok);
}

/* @tc.name: timeLog
 * @tc.desc: Test timeLog can be write.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest037, testing::ext::TestSize.Level0)
{
    NativeEngineProxy env;
    napi_value func = nullptr;
    napi_status status = napi_create_function(env, "timeLog", NAPI_AUTO_LENGTH,
                                              ConsoleTest::TimeLog, nullptr, &func);
    ASSERT_TRUE(status == napi_ok);
    napi_value globalObj = Helper::NapiHelper::GetGlobalObject(env);
    napi_value globalConsole = nullptr;
    napi_get_named_property(env, globalObj, "console", &globalConsole);
    ASSERT_TRUE(napi_set_named_property(env, globalConsole, "timeLog", func) == napi_ok);
}

/* @tc.name: timeEnd
 * @tc.desc: Test timeEnd can be write.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest038, testing::ext::TestSize.Level0)
{
    NativeEngineProxy env;
    napi_value func = nullptr;
    napi_status status = napi_create_function(env, "timeEnd", NAPI_AUTO_LENGTH,
                                              ConsoleTest::TimeEnd, nullptr, &func);
    ASSERT_TRUE(status == napi_ok);
    napi_value globalObj = Helper::NapiHelper::GetGlobalObject(env);
    napi_value globalConsole = nullptr;
    napi_get_named_property(env, globalObj, "console", &globalConsole);
    ASSERT_TRUE(napi_set_named_property(env, globalConsole, "timeEnd", func) == napi_ok);
}

/* @tc.name: trace
 * @tc.desc: Test trace can be write.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest039, testing::ext::TestSize.Level0)
{
    NativeEngineProxy env;
    napi_value func = nullptr;
    napi_status status = napi_create_function(env, "trace", NAPI_AUTO_LENGTH,
                                              ConsoleTest::Trace, nullptr, &func);
    ASSERT_TRUE(status == napi_ok);
    napi_value globalObj = Helper::NapiHelper::GetGlobalObject(env);
    napi_value globalConsole = nullptr;
    napi_get_named_property(env, globalObj, "console", &globalConsole);
    ASSERT_TRUE(napi_set_named_property(env, globalConsole, "trace", func) == napi_ok);
}

/* @tc.name: traceHybridStack
 * @tc.desc: Test traceHybridStack can be write.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest040, testing::ext::TestSize.Level0)
{
    NativeEngineProxy env;
    napi_value func = nullptr;
    napi_status status = napi_create_function(env, "traceHybridStack", NAPI_AUTO_LENGTH,
                                              ConsoleTest::TraceHybridStack, nullptr, &func);
    ASSERT_TRUE(status == napi_ok);
    napi_value globalObj = Helper::NapiHelper::GetGlobalObject(env);
    napi_value globalConsole = nullptr;
    napi_get_named_property(env, globalObj, "console", &globalConsole);
    ASSERT_TRUE(napi_set_named_property(env, globalConsole, "traceHybridStack", func) == napi_ok);
}

/* @tc.name: assert
 * @tc.desc: Test assert can be write.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest041, testing::ext::TestSize.Level0)
{
    NativeEngineProxy env;
    napi_value func = nullptr;
    napi_status status = napi_create_function(env, "assert", NAPI_AUTO_LENGTH,
                                              ConsoleTest::Assert, nullptr, &func);
    ASSERT_TRUE(status == napi_ok);
    napi_value globalObj = Helper::NapiHelper::GetGlobalObject(env);
    napi_value globalConsole = nullptr;
    napi_get_named_property(env, globalObj, "console", &globalConsole);
    ASSERT_TRUE(napi_set_named_property(env, globalConsole, "assert", func) == napi_ok);
}

/* @tc.name: count
 * @tc.desc: Test count can be write.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest042, testing::ext::TestSize.Level0)
{
    NativeEngineProxy env;
    napi_value func = nullptr;
    napi_status status = napi_create_function(env, "count", NAPI_AUTO_LENGTH,
                                              ConsoleTest::Count, nullptr, &func);
    ASSERT_TRUE(status == napi_ok);
    napi_value globalObj = Helper::NapiHelper::GetGlobalObject(env);
    napi_value globalConsole = nullptr;
    napi_get_named_property(env, globalObj, "console", &globalConsole);
    ASSERT_TRUE(napi_set_named_property(env, globalConsole, "count", func) == napi_ok);
}

/* @tc.name: countReset
 * @tc.desc: Test countReset can be write.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest043, testing::ext::TestSize.Level0)
{
    NativeEngineProxy env;
    napi_value func = nullptr;
    napi_status status = napi_create_function(env, "countReset", NAPI_AUTO_LENGTH,
                                              ConsoleTest::CountReset, nullptr, &func);
    ASSERT_TRUE(status == napi_ok);
    napi_value globalObj = Helper::NapiHelper::GetGlobalObject(env);
    ASSERT_TRUE(napi_set_named_property(env, globalObj, "countReset", func) == napi_ok);
}

/* @tc.name: dir
 * @tc.desc: Test dir can be write.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest044, testing::ext::TestSize.Level0)
{
    NativeEngineProxy env;
    napi_value func = nullptr;
    napi_status status = napi_create_function(env, "dir", NAPI_AUTO_LENGTH,
                                              ConsoleTest::Dir, nullptr, &func);
    ASSERT_TRUE(status == napi_ok);
    napi_value globalObj = Helper::NapiHelper::GetGlobalObject(env);
    ASSERT_TRUE(napi_set_named_property(env, globalObj, "dir", func) == napi_ok);
}

/* @tc.name: dirxml
 * @tc.desc: Test dirxml can be write.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest045, testing::ext::TestSize.Level0)
{
    NativeEngineProxy env;
    napi_value func = nullptr;
    napi_status status = napi_create_function(env, "dirxml", NAPI_AUTO_LENGTH,
                                              ConsoleTest::ConsoleLog<LogLevel::INFO>, nullptr, &func);
    ASSERT_TRUE(status == napi_ok);
    napi_value globalObj = Helper::NapiHelper::GetGlobalObject(env);
    ASSERT_TRUE(napi_set_named_property(env, globalObj, "dirxml", func) == napi_ok);
}

/* @tc.name: log
 * @tc.desc: Test log debug info warn property not change.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest046, testing::ext::TestSize.Level0)
{
    NativeEngineProxy env;
    napi_value func1 = nullptr;
    napi_status status = napi_create_function(env, "log", NAPI_AUTO_LENGTH,
                                              ConsoleTest::ConsoleLog<LogLevel::INFO>, nullptr, &func1);
    napi_value func2 = nullptr;
    status = napi_create_function(env, "debug", NAPI_AUTO_LENGTH,
                                  ConsoleTest::ConsoleLog<LogLevel::DEBUG>, nullptr, &func2);
    napi_value func3 = nullptr;
    status = napi_create_function(env, "info", NAPI_AUTO_LENGTH,
                                  ConsoleTest::ConsoleLog<LogLevel::INFO>, nullptr, &func3);
    napi_value func4 = nullptr;
    status = napi_create_function(env, "warn", NAPI_AUTO_LENGTH,
                                  ConsoleTest::ConsoleLog<LogLevel::WARN>, nullptr, &func4);
    ASSERT_TRUE(status == napi_ok);
    napi_property_descriptor properties[] = {
        // napi_default_jsproperty = napi_writable | napi_enumerable | napi_configurable
        {"log", nullptr, nullptr, nullptr, nullptr, func1, napi_default_jsproperty, nullptr},
        {"debug", nullptr, nullptr, nullptr, nullptr, func2, napi_default_jsproperty, nullptr},
        {"info", nullptr, nullptr, nullptr, nullptr, func3, napi_default_jsproperty, nullptr},
        {"warn", nullptr, nullptr, nullptr, nullptr, func4, napi_default_jsproperty, nullptr}
    };
    napi_value globalObj = Helper::NapiHelper::GetGlobalObject(env);
    napi_value console = nullptr;
    status = napi_get_named_property(env, globalObj, "console", &console);
    ASSERT_TRUE(status == napi_ok);
    status = napi_define_properties(env, console, sizeof(properties) / sizeof(properties[0]), properties);
    ASSERT_TRUE(status == napi_ok);
    napi_value logCB = GetGlobalProperty(env, "log");
    napi_value debugCB = GetGlobalProperty(env, "debug");
    napi_value infoCB = GetGlobalProperty(env, "info");
    napi_value warnCB = GetGlobalProperty(env, "warn");
    bool isEqual = false;
    napi_strict_equals(env, logCB, func1, &isEqual);
    ASSERT_TRUE(isEqual);
    napi_strict_equals(env, debugCB, func2, &isEqual);
    ASSERT_TRUE(isEqual);
    napi_strict_equals(env, infoCB, func3, &isEqual);
    ASSERT_TRUE(isEqual);
    napi_strict_equals(env, warnCB, func4, &isEqual);
    ASSERT_TRUE(isEqual);
}

/* @tc.name: error
 * @tc.desc: Test error fatal group property not change.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest047, testing::ext::TestSize.Level0)
{
    NativeEngineProxy env;
    napi_value func1 = nullptr;
    napi_status status = napi_create_function(env, "error", NAPI_AUTO_LENGTH,
                                              ConsoleTest::ConsoleLog<LogLevel::ERROR>, nullptr, &func1);
    ASSERT_TRUE(status == napi_ok);
    napi_value func2 = nullptr;
    napi_create_function(env, "fatal", NAPI_AUTO_LENGTH, ConsoleTest::ConsoleLog<LogLevel::FATAL>, nullptr, &func2);
    napi_value func3 = nullptr;
    napi_create_function(env, "group", NAPI_AUTO_LENGTH, ConsoleTest::Group, nullptr, &func3);
    napi_value func4 = nullptr;
    napi_create_function(env, "groupCollapsed", NAPI_AUTO_LENGTH, ConsoleTest::Group, nullptr, &func4);
    napi_value func5 = nullptr;
    napi_create_function(env, "groupEnd", NAPI_AUTO_LENGTH, ConsoleTest::GroupEnd, nullptr, &func5);
    napi_property_descriptor properties[] = {
        // napi_default_jsproperty = napi_writable | napi_enumerable | napi_configurable
        {"error", nullptr, nullptr, nullptr, nullptr, func1, napi_default_jsproperty, nullptr},
        {"fatal", nullptr, nullptr, nullptr, nullptr, func2, napi_default_jsproperty, nullptr},
        {"group", nullptr, nullptr, nullptr, nullptr, func3, napi_default_jsproperty, nullptr},
        {"groupCollapsed", nullptr, nullptr, nullptr, nullptr, func4, napi_default_jsproperty, nullptr},
        {"groupEnd", nullptr, nullptr, nullptr, nullptr, func5, napi_default_jsproperty, nullptr}
    };
    napi_value globalObj = Helper::NapiHelper::GetGlobalObject(env);
    napi_value console = nullptr;
    napi_get_named_property(env, globalObj, "console", &console);
    status = napi_define_properties(env, console, sizeof(properties) / sizeof(properties[0]), properties);
    ASSERT_TRUE(status == napi_ok);
    napi_value errorCB = GetGlobalProperty(env, "error");
    napi_value fatalCB = GetGlobalProperty(env, "fatal");
    napi_value groupCB = GetGlobalProperty(env, "group");
    napi_value groupCollapsedCB = GetGlobalProperty(env, "groupCollapsed");
    napi_value groupEndCB = GetGlobalProperty(env, "groupEnd");
    bool isEqual = false;
    napi_strict_equals(env, errorCB, func1, &isEqual);
    ASSERT_TRUE(isEqual);
    napi_strict_equals(env, fatalCB, func2, &isEqual);
    ASSERT_TRUE(isEqual);
    napi_strict_equals(env, groupCB, func3, &isEqual);
    ASSERT_TRUE(isEqual);
    napi_strict_equals(env, groupCollapsedCB, func4, &isEqual);
    ASSERT_TRUE(isEqual);
    napi_strict_equals(env, groupEndCB, func5, &isEqual);
    ASSERT_TRUE(isEqual);
}

/* @tc.name: table
 * @tc.desc: Test table time trace property not change.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest048, testing::ext::TestSize.Level0)
{
    NativeEngineProxy env;
    napi_value func1 = nullptr;
    napi_create_function(env, "table", NAPI_AUTO_LENGTH, ConsoleTest::Table, nullptr, &func1);
    napi_value func2 = nullptr;
    napi_create_function(env, "time", NAPI_AUTO_LENGTH, ConsoleTest::Time, nullptr, &func2);
    napi_value func3 = nullptr;
    napi_create_function(env, "timeLog", NAPI_AUTO_LENGTH, ConsoleTest::TimeLog, nullptr, &func3);
    napi_value func4 = nullptr;
    napi_create_function(env, "timeEnd", NAPI_AUTO_LENGTH, ConsoleTest::TimeEnd, nullptr, &func4);
    napi_value func5 = nullptr;
    napi_create_function(env, "trace", NAPI_AUTO_LENGTH, ConsoleTest::Trace, nullptr, &func5);
    napi_value func6 = nullptr;
    napi_create_function(env, "traceHybridStack", NAPI_AUTO_LENGTH, ConsoleTest::TraceHybridStack, nullptr, &func6);
    napi_property_descriptor properties[] = {
        // napi_default_jsproperty = napi_writable | napi_enumerable | napi_configurable
        {"table", nullptr, nullptr, nullptr, nullptr, func1, napi_default_jsproperty, nullptr},
        {"time", nullptr, nullptr, nullptr, nullptr, func2, napi_default_jsproperty, nullptr},
        {"timeLog", nullptr, nullptr, nullptr, nullptr, func3, napi_default_jsproperty, nullptr},
        {"timeEnd", nullptr, nullptr, nullptr, nullptr, func4, napi_default_jsproperty, nullptr},
        {"trace", nullptr, nullptr, nullptr, nullptr, func5, napi_default_jsproperty, nullptr},
        {"traceHybridStack", nullptr, nullptr, nullptr, nullptr, func6, napi_default_jsproperty, nullptr}
    };
    napi_value globalObj = Helper::NapiHelper::GetGlobalObject(env);
    napi_value console = nullptr;
    napi_get_named_property(env, globalObj, "console", &console);
    napi_define_properties(env, console, sizeof(properties) / sizeof(properties[0]), properties);
    napi_value tableCB = GetGlobalProperty(env, "table");
    napi_value timeCB = GetGlobalProperty(env, "time");
    napi_value timeLogCB = GetGlobalProperty(env, "timeLog");
    napi_value timeEndCB = GetGlobalProperty(env, "timeEnd");
    napi_value traceCB = GetGlobalProperty(env, "trace");
    napi_value traceHybridStackCB = GetGlobalProperty(env, "traceHybridStack");
    bool isEqual = false;
    napi_strict_equals(env, tableCB, func1, &isEqual);
    ASSERT_TRUE(isEqual);
    napi_strict_equals(env, timeCB, func2, &isEqual);
    ASSERT_TRUE(isEqual);
    napi_strict_equals(env, timeLogCB, func3, &isEqual);
    ASSERT_TRUE(isEqual);
    napi_strict_equals(env, timeEndCB, func4, &isEqual);
    ASSERT_TRUE(isEqual);
    napi_strict_equals(env, traceCB, func5, &isEqual);
    ASSERT_TRUE(isEqual);
    napi_strict_equals(env, traceHybridStackCB, func6, &isEqual);
    ASSERT_TRUE(isEqual);
}

/* @tc.name: assert
 * @tc.desc: Test assert count dir property not change.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest049, testing::ext::TestSize.Level0)
{
    NativeEngineProxy env;
    napi_value func1 = nullptr;
    napi_create_function(env, "assert", NAPI_AUTO_LENGTH, ConsoleTest::Assert, nullptr, &func1);
    napi_value func2 = nullptr;
    napi_create_function(env, "count", NAPI_AUTO_LENGTH, ConsoleTest::Count, nullptr, &func2);
    napi_value func3 = nullptr;
    napi_create_function(env, "countReset", NAPI_AUTO_LENGTH, ConsoleTest::CountReset, nullptr, &func3);
    napi_value func4 = nullptr;
    napi_create_function(env, "dir", NAPI_AUTO_LENGTH, ConsoleTest::Dir, nullptr, &func4);
    napi_value func5 = nullptr;
    napi_create_function(env, "dirxml", NAPI_AUTO_LENGTH, ConsoleTest::ConsoleLog<LogLevel::INFO>, nullptr, &func5);
    napi_property_descriptor properties[] = {
        // napi_default_jsproperty = napi_writable | napi_enumerable | napi_configurable
        {"assert", nullptr, nullptr, nullptr, nullptr, func1, napi_default_jsproperty, nullptr},
        {"count", nullptr, nullptr, nullptr, nullptr, func2, napi_default_jsproperty, nullptr},
        {"countReset", nullptr, nullptr, nullptr, nullptr, func3, napi_default_jsproperty, nullptr},
        {"dir", nullptr, nullptr, nullptr, nullptr, func4, napi_default_jsproperty, nullptr},
        {"dirxml", nullptr, nullptr, nullptr, nullptr, func5, napi_default_jsproperty, nullptr}
    };
    napi_value globalObj = Helper::NapiHelper::GetGlobalObject(env);
    napi_value console = nullptr;
    napi_get_named_property(env, globalObj, "console", &console);
    napi_status status = napi_define_properties(env, console, sizeof(properties) / sizeof(properties[0]), properties);
    ASSERT_TRUE(status == napi_ok);
    napi_value assertCB = GetGlobalProperty(env, "assert");
    napi_value countCB = GetGlobalProperty(env, "count");
    napi_value countResetCB = GetGlobalProperty(env, "countReset");
    napi_value dirCB = GetGlobalProperty(env, "dir");
    napi_value dirxmlCB = GetGlobalProperty(env, "dirxml");
    bool isEqual = false;
    napi_strict_equals(env, assertCB, func1, &isEqual);
    ASSERT_TRUE(isEqual);
    napi_strict_equals(env, countCB, func2, &isEqual);
    ASSERT_TRUE(isEqual);
    napi_strict_equals(env, countResetCB, func3, &isEqual);
    ASSERT_TRUE(isEqual);
    napi_strict_equals(env, dirCB, func4, &isEqual);
    ASSERT_TRUE(isEqual);
    napi_strict_equals(env, dirxmlCB, func5, &isEqual);
    ASSERT_TRUE(isEqual);
}

/* @tc.name: GetTimerOrCounterName error paths
 * @tc.desc: Test error handling in GetTimerOrCounterName.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest050, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;

    // Test case 1: Empty string should throw error
    size_t argc = 1;
    napi_value emptyString = nullptr;
    napi_create_string_utf8(env, "", NAPI_AUTO_LENGTH, &emptyString);
    napi_value argv[] = {emptyString};

    napi_value cb = nullptr;
    napi_create_function(env, "Count", NAPI_AUTO_LENGTH, ConsoleTest::Count, nullptr, &cb);
    napi_value res = nullptr;
    napi_call_function(env, nullptr, cb, argc, argv, &res);

    bool hasException = false;
    napi_is_exception_pending(env, &hasException);
    ASSERT_TRUE(hasException);

    napi_value exception;
    napi_get_and_clear_last_exception(env, &exception);
}

/* @tc.name: Dir with null value
 * @tc.desc: Test Dir handling of null/undefined values.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest051, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;

    // Test case 1: Dir with null
    size_t argc = 1;
    napi_value nullValue = nullptr;
    napi_get_null(env, &nullValue);
    napi_value argv[] = {nullValue};

    napi_value cb = nullptr;
    napi_create_function(env, "Dir", NAPI_AUTO_LENGTH, ConsoleTest::Dir, nullptr, &cb);
    napi_value res = nullptr;
    napi_call_function(env, nullptr, cb, argc, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);

    // Test case 2: Dir with undefined
    napi_value undefinedValue = nullptr;
    napi_get_undefined(env, &undefinedValue);
    napi_value argv2[] = {undefinedValue};

    cb = nullptr;
    napi_create_function(env, "Dir", NAPI_AUTO_LENGTH, ConsoleTest::Dir, nullptr, &cb);
    napi_value res2 = nullptr;
    napi_call_function(env, nullptr, cb, argc, argv2, &res2);
    ASSERT_CHECK_VALUE_TYPE(env, res2, napi_undefined);
}

/* @tc.name: ParseLogContent edge cases
 * @tc.desc: Test ParseLogContent with edge cases.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest052, testing::ext::TestSize.Level0)
{
    // Test case 1: Empty params
    std::vector<std::string> params1;
    std::string result1 = ConsoleTest::ParseLogContent(params1);
    ASSERT_TRUE(result1 == "");

    // Test case 2: Format with %% escape
    std::vector<std::string> params2 = {"Progress: 50%% complete"};
    std::string result2 = ConsoleTest::ParseLogContent(params2);
    ASSERT_TRUE(result2 == "Progress: 50%% complete");

    // Test case 3: More params than format specifiers
    std::vector<std::string> params3 = {"Hello %s", "world", "extra"};
    std::string result3 = ConsoleTest::ParseLogContent(params3);
    ASSERT_TRUE(result3 == "Hello world extra");

    // Test case 4: Invalid format specifier
    std::vector<std::string> params4 = {"Test %x value"};
    std::string result4 = ConsoleTest::ParseLogContent(params4);
    ASSERT_TRUE(result4 == "Test %x value");
}

/* @tc.name: ConsoleLog with object parameter
 * @tc.desc: Test ConsoleLog handles object coercion.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest053, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;

    size_t argc = 1;
    napi_value obj = nullptr;
    napi_create_object(env, &obj);

    napi_value key = nullptr;
    napi_create_string_utf8(env, "name", NAPI_AUTO_LENGTH, &key);
    napi_value value = nullptr;
    napi_create_string_utf8(env, "test", NAPI_AUTO_LENGTH, &value);
    napi_set_property(env, obj, key, value);

    napi_value argv[] = {obj};

    napi_value cb = nullptr;
    napi_create_function(env, "ConsoleLog", NAPI_AUTO_LENGTH,
                         ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_value res = nullptr;
    napi_call_function(env, nullptr, cb, argc, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: Console.Dir with function
 * @tc.desc: Test Dir with function type object.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest054, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 1;
    std::string funcName = "Dir";
    napi_value cb = nullptr;

    napi_value testFunc = nullptr;
    napi_create_function(env, "testFunc", NAPI_AUTO_LENGTH,
                         ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &testFunc);
    napi_value argv[] = {testFunc};

    napi_value res = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Dir, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: Console.Dir with object
 * @tc.desc: Test Dir with object that has constructor name.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest055, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 1;
    std::string funcName = "Dir";
    napi_value cb = nullptr;

    napi_value obj = nullptr;
    napi_create_object(env, &obj);
    napi_value strVal = StrToNapiValue(env, "test value");
    napi_set_named_property(env, obj, "key", strVal);
    napi_value argv[] = {obj};

    napi_value res = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Dir, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: Console.Table with non-object
 * @tc.desc: Test Table with non-object input.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest056, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 1;
    std::string funcName = "Table";
    napi_value cb = nullptr;

    napi_value strVal = StrToNapiValue(env, "not an object");
    napi_value argv[] = {strVal};

    napi_value res = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Table, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: Console.Table with primitive values
 * @tc.desc: Test Table with object containing primitive values.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest057, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 1;
    std::string funcName = "Table";
    napi_value cb = nullptr;

    napi_value tabularData = nullptr;
    napi_create_object(env, &tabularData);

    napi_value num1 = nullptr;
    napi_create_int32(env, 100, &num1);
    napi_set_named_property(env, tabularData, "a", num1);

    napi_value num2 = nullptr;
    napi_create_int32(env, 200, &num2);
    napi_set_named_property(env, tabularData, "b", num2);

    napi_value argv[] = {tabularData};

    napi_value res = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Table, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: Console.Table with nested objects
 * @tc.desc: Test Table with nested object data.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest058, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 1;
    std::string funcName = "Table";
    napi_value cb = nullptr;

    napi_value tabularData = nullptr;
    napi_create_object(env, &tabularData);

    napi_value innerObj1 = nullptr;
    napi_create_object(env, &innerObj1);
    napi_value val1 = nullptr;
    napi_create_int32(env, 10, &val1);
    napi_set_named_property(env, innerObj1, "x", val1);
    napi_set_named_property(env, tabularData, "row1", innerObj1);

    napi_value innerObj2 = nullptr;
    napi_create_object(env, &innerObj2);
    napi_value val2 = nullptr;
    napi_create_int32(env, 20, &val2);
    napi_set_named_property(env, innerObj2, "x", val2);
    napi_set_named_property(env, tabularData, "row2", innerObj2);

    napi_value argv[] = {tabularData};

    napi_value res = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Table, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: Console.Count with number argument
 * @tc.desc: Test Count with number type argument that needs conversion.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest059, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 1;
    std::string funcName = "Count";
    napi_value cb = nullptr;

    napi_value numVal = nullptr;
    napi_create_int32(env, 123, &numVal);
    napi_value argv[] = {numVal};

    napi_value res = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Count, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: Console.Time with number argument
 * @tc.desc: Test Time with number type argument that needs conversion.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest060, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 1;
    std::string funcName = "Time";
    napi_value cb = nullptr;

    napi_value numVal = nullptr;
    napi_create_int32(env, 456, &numVal);
    napi_value argv[] = {numVal};

    napi_value res = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Time, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: Console.Group with multiple arguments
 * @tc.desc: Test Group with multiple arguments.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest061, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ConsoleTest::SetGroupIndent("");

    size_t argc = 2;
    std::string funcName = "Group";
    napi_value cb = nullptr;

    napi_value msg1 = StrToNapiValue(env, "Group");
    napi_value msg2 = StrToNapiValue(env, "Label");
    napi_value argv[] = {msg1, msg2};

    napi_value res = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Group, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
    ASSERT_EQ(ConsoleTest::GetGroupIndent().size(), 2);

    ConsoleTest::SetGroupIndent("");
}

/* @tc.name: Console.GroupEnd when groupIndent is empty
 * @tc.desc: Test GroupEnd when groupIndent size is less than GROUPINDETATIONWIDTH.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest062, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ConsoleTest::SetGroupIndent(" ");

    size_t argc = 0;
    std::string funcName = "GroupEnd";
    napi_value cb = nullptr;
    napi_value argv[] = {nullptr};

    napi_value res = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::GroupEnd, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
    ASSERT_EQ(ConsoleTest::GetGroupIndent().size(), 1);

    ConsoleTest::SetGroupIndent("");
}

/* @tc.name: Console.Dir with string
 * @tc.desc: Test Dir with string type.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest063, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 1;
    std::string funcName = "Dir";
    napi_value cb = nullptr;

    napi_value strVal = StrToNapiValue(env, "test string");
    napi_value argv[] = {strVal};

    napi_value res = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Dir, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: Console.Assert with multiple messages
 * @tc.desc: Test Assert with false condition and multiple message arguments.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest064, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 3;
    std::string funcName = "Assert";
    napi_value cb = nullptr;

    napi_value condition = nullptr;
    napi_get_boolean(env, false, &condition);
    napi_value msg1 = StrToNapiValue(env, "Error:");
    napi_value msg2 = StrToNapiValue(env, "something went wrong");
    napi_value argv[] = {condition, msg1, msg2};

    napi_value res = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Assert, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: Console.Table with mixed data
 * @tc.desc: Test Table with mixed primitive and object data.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest065, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 1;
    std::string funcName = "Table";
    napi_value cb = nullptr;

    napi_value tabularData = nullptr;
    napi_create_object(env, &tabularData);

    napi_value num = nullptr;
    napi_create_int32(env, 42, &num);
    napi_set_named_property(env, tabularData, "primitive", num);

    napi_value innerObj = nullptr;
    napi_create_object(env, &innerObj);
    napi_value val = nullptr;
    napi_create_int32(env, 100, &val);
    napi_set_named_property(env, innerObj, "nested", val);
    napi_set_named_property(env, tabularData, "object", innerObj);

    napi_value argv[] = {tabularData};

    napi_value res = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Table, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: Console.Trace with multiple arguments
 * @tc.desc: Test Trace with multiple arguments.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest066, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 3;
    std::string funcName = "Trace";
    napi_value cb = nullptr;

    napi_value msg1 = StrToNapiValue(env, "trace");
    napi_value msg2 = StrToNapiValue(env, "message");
    napi_value num = nullptr;
    napi_create_int32(env, 123, &num);
    napi_value argv[] = {msg1, msg2, num};

    napi_value res = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Trace, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: Console.TimeLog with multiple arguments
 * @tc.desc: Test TimeLog with timer name and additional log messages.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest067, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string funcName = "Time";
    napi_value cb = nullptr;

    std::string timerName = "testTimer067";
    napi_value nameVal = StrToNapiValue(env, timerName);
    napi_value argv1[] = {nameVal};

    napi_value res1 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Time, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv1, &res1);
    ASSERT_CHECK_VALUE_TYPE(env, res1, napi_undefined);

    funcName = "TimeLog";
    cb = nullptr;
    napi_value msg1 = StrToNapiValue(env, "extra");
    napi_value msg2 = StrToNapiValue(env, "info");
    napi_value argv2[] = {nameVal, msg1, msg2};

    napi_value res2 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::TimeLog, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 3, argv2, &res2);
    ASSERT_CHECK_VALUE_TYPE(env, res2, napi_undefined);

    funcName = "TimeEnd";
    cb = nullptr;
    napi_value res3 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::TimeEnd, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv1, &res3);
    ASSERT_CHECK_VALUE_TYPE(env, res3, napi_undefined);
}

/* @tc.name: Console.Dir with array
 * @tc.desc: Test Dir with array type.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest068, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 1;
    std::string funcName = "Dir";
    napi_value cb = nullptr;

    napi_value arr = nullptr;
    napi_create_array_with_length(env, 3, &arr);
    napi_value val1 = nullptr;
    napi_create_int32(env, 1, &val1);
    napi_value val2 = nullptr;
    napi_create_int32(env, 2, &val2);
    napi_value val3 = nullptr;
    napi_create_int32(env, 3, &val3);
    napi_set_element(env, arr, 0, val1);
    napi_set_element(env, arr, 1, val2);
    napi_set_element(env, arr, 2, val3);
    napi_value argv[] = {arr};

    napi_value res = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Dir, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: Console.Table with empty object
 * @tc.desc: Test Table with empty object.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest069, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 1;
    std::string funcName = "Table";
    napi_value cb = nullptr;

    napi_value emptyObj = nullptr;
    napi_create_object(env, &emptyObj);
    napi_value argv[] = {emptyObj};

    napi_value res = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Table, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: Console.Count multiple times
 * @tc.desc: Test Count called multiple times with same label.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest070, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string funcName = "Count";
    napi_value cb = nullptr;

    std::string label = "multiCount";
    napi_value labelVal = StrToNapiValue(env, label);
    napi_value argv[] = {labelVal};

    for (int i = 0; i < 3; i++) {
        cb = nullptr;
        napi_value res = nullptr;
        napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Count, nullptr, &cb);
        napi_call_function(env, nullptr, cb, 1, argv, &res);
        ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
    }

    funcName = "CountReset";
    cb = nullptr;
    napi_value res = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::CountReset, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: ParseLogContent
 * @tc.desc: Test ParseLogContent with format string ending with %.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest071, testing::ext::TestSize.Level0)
{
    std::vector<std::string> params;
    params.push_back("test message %");
    params.push_back("extra");
    std::string res = ConsoleTest::ParseLogContent(params);
    ASSERT_TRUE(res == "test message % extra");
}

/* @tc.name: ParseLogContent
 * @tc.desc: Test ParseLogContent with unknown format specifier.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest072, testing::ext::TestSize.Level0)
{
    std::vector<std::string> params;
    params.push_back("test %x value");
    params.push_back("123");
    std::string res = ConsoleTest::ParseLogContent(params);
    ASSERT_TRUE(res == "test %x value 123");
}

/* @tc.name: ParseLogContent
 * @tc.desc: Test ParseLogContent with multiple format specifiers.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest073, testing::ext::TestSize.Level0)
{
    std::vector<std::string> params;
    params.push_back("%s %d %i %f");
    params.push_back("str");
    params.push_back("100");
    params.push_back("200");
    params.push_back("3.14");
    std::string res = ConsoleTest::ParseLogContent(params);
    ASSERT_TRUE(res == "str 100 200 3.14");
}

/* @tc.name: ParseLogContent
 * @tc.desc: Test ParseLogContent with more format specifiers than params.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest074, testing::ext::TestSize.Level0)
{
    std::vector<std::string> params;
    params.push_back("%s %s %s");
    params.push_back("only one");
    std::string res = ConsoleTest::ParseLogContent(params);
    ASSERT_TRUE(res == "only one %s %s");
}

/* @tc.name: Console.Dir with function
 * @tc.desc: Test Dir with function type object.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest075, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 1;
    std::string funcName = "Dir";
    napi_value cb = nullptr;

    napi_value testFunc = nullptr;
    napi_create_function(env, "testFunc", NAPI_AUTO_LENGTH,
                         ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &testFunc);
    napi_value argv[] = {testFunc};

    napi_value res = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Dir, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
