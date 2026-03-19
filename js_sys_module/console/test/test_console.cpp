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

    napi_value exception = nullptr;
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
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Table, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: ConsoleLog with different number types
 * @tc.desc: Test ConsoleLog with various number format specifiers.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest080, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 2;
    std::string funcName = "ConsoleLog";
    napi_value cb = nullptr;

    std::string message = "Integer: %d, Float: %f";
    napi_value nativeMessage0 = nullptr;
    napi_create_string_utf8(env, message.c_str(), message.length(), &nativeMessage0);
    napi_value nativeMessage1 = nullptr;
    napi_create_int32(env, 42, &nativeMessage1);
    napi_value nativeMessage2 = nullptr;
    napi_create_double(env, 3.14159, &nativeMessage2);
    napi_value argv[] = {nativeMessage0, nativeMessage1, nativeMessage2};

    napi_value res = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(),
                         ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: ConsoleLog with boolean values
 * @tc.desc: Test ConsoleLog with boolean format specifier.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest081, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 2;
    std::string funcName = "ConsoleLog";
    napi_value cb = nullptr;

    std::string message = "Boolean value: %s";
    napi_value nativeMessage0 = nullptr;
    napi_create_string_utf8(env, message.c_str(), message.length(), &nativeMessage0);
    napi_value nativeMessage1 = nullptr;
    napi_get_boolean(env, 1, &nativeMessage1);
    napi_value argv[] = {nativeMessage0, nativeMessage1};

    napi_value res = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(),
                         ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: ConsoleLog with large number
 * @tc.desc: Test ConsoleLog with large integer value.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest082, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 2;
    std::string funcName = "ConsoleLog";
    napi_value cb = nullptr;

    std::string message = "Large number: %d";
    napi_value nativeMessage0 = nullptr;
    napi_create_string_utf8(env, message.c_str(), message.length(), &nativeMessage0);
    napi_value nativeMessage1 = nullptr;
    napi_create_int32(env, 999999, &nativeMessage1);
    napi_value argv[] = {nativeMessage0, nativeMessage1};

    napi_value res = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(),
                         ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: Console.Count with boolean label
 * @tc.desc: Test Count with boolean type label.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest083, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 1;
    std::string funcName = "Count";
    napi_value cb = nullptr;

    napi_value boolVal = nullptr;
    napi_get_boolean(env, 1, &boolVal);
    napi_value argv[] = {boolVal};

    napi_value res = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Count, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: Console.Time with boolean label
 * @tc.desc: Test Time with boolean type label.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest084, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 1;
    std::string funcName = "Time";
    napi_value cb = nullptr;

    napi_value boolVal = nullptr;
    napi_get_boolean(env, 0, &boolVal);
    napi_value argv[] = {boolVal};

    napi_value res = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Time, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: Console.Group with nested groups
 * @tc.desc: Test Group with multiple nested group calls.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest085, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ConsoleTest::SetGroupIndent("");

    size_t argc = 1;
    std::string funcName = "Group";
    napi_value cb = nullptr;

    napi_value msg1 = StrToNapiValue(env, "Level 1");
    napi_value argv1[] = {msg1};

    napi_value res1 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Group, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv1, &res1);
    ASSERT_CHECK_VALUE_TYPE(env, res1, napi_undefined);

    napi_value msg2 = StrToNapiValue(env, "Level 2");
    napi_value argv2[] = {msg2};

    cb = nullptr;
    napi_value res2 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Group, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv2, &res2);
    ASSERT_CHECK_VALUE_TYPE(env, res2, napi_undefined);

    funcName = "GroupEnd";
    cb = nullptr;
    napi_value res3 = nullptr;
    napi_value argv3[] = {nullptr};
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::GroupEnd, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 0, argv3, &res3);
    ASSERT_CHECK_VALUE_TYPE(env, res3, napi_undefined);

    cb = nullptr;
    napi_value res4 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::GroupEnd, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 0, argv3, &res4);
    ASSERT_CHECK_VALUE_TYPE(env, res4, napi_undefined);

    ConsoleTest::SetGroupIndent("");
}

/* @tc.name: Console.TraceHybridStack with no arguments
 * @tc.desc: Test TraceHybridStack called without arguments.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest086, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 0;
    napi_value res = nullptr;
    std::string funcName = "TraceHybridStack";
    napi_value argv[] = {nullptr};
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::TraceHybridStack, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: Console.Assert with true condition
 * @tc.desc: Test Assert with true condition that should not trigger error.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest087, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 1;
    std::string funcName = "Assert";
    napi_value cb = nullptr;

    napi_value condition = nullptr;
    napi_get_boolean(env, 1, &condition);
    napi_value argv[] = {condition};

    napi_value res = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Assert, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: Console.Table with array of objects
 * @tc.desc: Test Table with array containing object elements.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest088, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 1;
    std::string funcName = "Table";
    napi_value cb = nullptr;

    napi_value objectArray = nullptr;
    napi_create_array_with_length(env, 2, &objectArray);

    napi_value obj1 = nullptr;
    napi_create_object(env, &obj1);
    napi_value val1 = nullptr;
    napi_create_int32(env, 10, &val1);
    napi_set_named_property(env, obj1, "id", val1);
    napi_set_element(env, objectArray, 0, obj1);

    napi_value obj2 = nullptr;
    napi_create_object(env, &obj2);
    napi_value val2 = nullptr;
    napi_create_int32(env, 20, &val2);
    napi_set_named_property(env, obj2, "id", val2);
    napi_set_element(env, objectArray, 1, obj2);

    napi_value argv[] = {objectArray};

    napi_value res = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Table, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: Console.Dir with number value
 * @tc.desc: Test Dir with number type parameter.
 * @tc.type: FUNC

 */
HWTEST_F(NativeEngineTest, ConsoleTest089, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 1;
    std::string funcName = "Dir";
    napi_value cb = nullptr;

    napi_value numVal = nullptr;
    napi_create_double(env, 123.456, &numVal);
    napi_value argv[] = {numVal};

    napi_value res = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Dir, nullptr, &cb);
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

/* @tc.name: Console.Dir with null and undefined
 * @tc.desc: Test Dir with null and undefined parameters.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest076, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 1;
    std::string funcName = "Dir";
    napi_value cb = nullptr;

    napi_value nullValue = nullptr;
    napi_get_null(env, &nullValue);
    napi_value argv1[] = {nullValue};

    napi_value res0 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Dir, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv1, &res0);
    ASSERT_CHECK_VALUE_TYPE(env, res0, napi_undefined);

    napi_value undefinedValue = nullptr;
    napi_get_undefined(env, &undefinedValue);
    napi_value argv2[] = {undefinedValue};

    cb = nullptr;
    napi_value res1 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Dir, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv2, &res1);
    ASSERT_CHECK_VALUE_TYPE(env, res1, napi_undefined);
}

/* @tc.name: Console.Table with empty array
 * @tc.desc: Test Table with empty array parameter.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest077, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 1;
    std::string funcName = "Table";
    napi_value cb = nullptr;

    napi_value emptyArray = nullptr;
    uint32_t length = 0;
    napi_create_array_with_length(env, length, &emptyArray);
    napi_value argv[] = {emptyArray};

    napi_value res = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Table, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: Console.Table with primitive array
 * @tc.desc: Test Table with primitive type array containing numbers and strings.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest078, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 1;
    std::string funcName = "Table";
    napi_value cb = nullptr;

    napi_value primitiveArray = nullptr;
    uint32_t length = 3;
    napi_create_array_with_length(env, length, &primitiveArray);
    
    napi_value num1 = nullptr;
    napi_create_int32(env, 10, &num1);
    napi_set_element(env, primitiveArray, 0, num1);
    
    napi_value num2 = nullptr;
    napi_create_int32(env, 20, &num2);
    napi_set_element(env, primitiveArray, 1, num2);
    
    napi_value str = nullptr;
    napi_create_string_utf8(env, "test", NAPI_AUTO_LENGTH, &str);
    napi_set_element(env, primitiveArray, 2, str);
    
    napi_value argv[] = {primitiveArray};

    napi_value res = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Table, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: Console.Table with mixed object types
 * @tc.desc: Test Table with objects containing mixed property types.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest079, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 1;
    std::string funcName = "Table";
    napi_value cb = nullptr;

    napi_value tabularData = nullptr;
    napi_create_object(env, &tabularData);
    
    napi_value nameArray = nullptr;
    napi_create_array_with_length(env, 2, &nameArray);
    napi_value name1 = StrToNapiValue(env, "Alice");
    napi_value name2 = StrToNapiValue(env, "Bob");
    napi_set_element(env, nameArray, 0, name1);
    napi_set_element(env, nameArray, 1, name2);
    napi_set_named_property(env, tabularData, "name", nameArray);
    
    napi_value ageArray = nullptr;
    napi_create_array_with_length(env, 2, &ageArray);
    napi_value age1 = nullptr;
    napi_value age2 = nullptr;
    napi_create_uint32(env, 25, &age1);
    napi_create_uint32(env, 30, &age2);
    napi_set_element(env, ageArray, 0, age1);
    napi_set_element(env, ageArray, 1, age2);
    napi_set_named_property(env, tabularData, "age", ageArray);
    
    napi_value activeArray = nullptr;
    napi_create_array_with_length(env, 2, &activeArray);
    napi_value bool1 = nullptr;
    napi_value bool2 = nullptr;
    napi_get_boolean(env, 1, &bool1);
    napi_get_boolean(env, 0, &bool2);
    napi_set_element(env, activeArray, 0, bool1);
    napi_set_element(env, activeArray, 1, bool2);
    napi_set_named_property(env, tabularData, "active", activeArray);
    
    napi_value argv[] = {tabularData};

    napi_value res = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), ConsoleTest::Table, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: ConsoleTest090 - ConsoleLog with empty string
 * @tc.desc: Test ConsoleLog with empty string argument.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest090, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 1;
    std::string funcName = "ConsoleLog";
    napi_value res0 = nullptr;
    napi_value cb = nullptr;
    std::string message = "";
    napi_value nativeMessage0 = nullptr;
    napi_create_string_utf8(env, message.c_str(), message.length(), &nativeMessage0);
    napi_value argv[] = {nativeMessage0};

    napi_create_function(env, funcName.c_str(), funcName.size(),
                         ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res0);
    ASSERT_CHECK_VALUE_TYPE(env, res0, napi_undefined);
}

/* @tc.name: ConsoleTest091 - ConsoleLog with special characters
 * @tc.desc: Test ConsoleLog with special characters in string.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest091, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 1;
    std::string funcName = "ConsoleLog";
    napi_value res0 = nullptr;
    napi_value cb = nullptr;
    std::string message = "Special chars: !@#$%^&*()_+-={}[]|\\:;\"'<>,.?/~`";
    napi_value nativeMessage0 = nullptr;
    napi_create_string_utf8(env, message.c_str(), message.length(), &nativeMessage0);
    napi_value argv[] = {nativeMessage0};

    napi_create_function(env, funcName.c_str(), funcName.size(),
                         ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res0);
    ASSERT_CHECK_VALUE_TYPE(env, res0, napi_undefined);
}

/* @tc.name: ConsoleTest092 - ConsoleLog with unicode string
 * @tc.desc: Test ConsoleLog with unicode characters.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest092, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 1;
    std::string funcName = "ConsoleLog";
    napi_value res0 = nullptr;
    napi_value cb = nullptr;
    std::string message = "Unicode test";
    napi_value nativeMessage0 = nullptr;
    napi_create_string_utf8(env, message.c_str(), message.length(), &nativeMessage0);
    napi_value argv[] = {nativeMessage0};

    napi_create_function(env, funcName.c_str(), funcName.size(),
                         ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res0);
    ASSERT_CHECK_VALUE_TYPE(env, res0, napi_undefined);
}

/* @tc.name: ConsoleTest093 - ConsoleLog with number values
 * @tc.desc: Test ConsoleLog with various number types.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest093, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string funcName = "ConsoleLog";
    napi_value cb = nullptr;

    // Test int32
    napi_value int32Val = nullptr;
    napi_create_int32(env, -2147483648, &int32Val);
    napi_value argv1[] = {int32Val};
    napi_value res1 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(),
                         ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv1, &res1);
    ASSERT_CHECK_VALUE_TYPE(env, res1, napi_undefined);

    // Test uint32
    cb = nullptr;
    napi_value uint32Val = nullptr;
    napi_create_uint32(env, 4294967295, &uint32Val);
    napi_value argv2[] = {uint32Val};
    napi_value res2 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(),
                         ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv2, &res2);
    ASSERT_CHECK_VALUE_TYPE(env, res2, napi_undefined);

    // Test double
    cb = nullptr;
    napi_value doubleVal = nullptr;
    napi_create_double(env, 3.14159265359, &doubleVal);
    napi_value argv3[] = {doubleVal};
    napi_value res3 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(),
                         ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv3, &res3);
    ASSERT_CHECK_VALUE_TYPE(env, res3, napi_undefined);
}

/* @tc.name: ConsoleTest094 - ConsoleLog with boolean values
 * @tc.desc: Test ConsoleLog with boolean true and false.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest094, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string funcName = "ConsoleLog";
    napi_value cb = nullptr;

    napi_value boolTrue = nullptr;
    napi_get_boolean(env, true, &boolTrue);
    napi_value argv1[] = {boolTrue};
    napi_value res1 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(),
                         ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv1, &res1);
    ASSERT_CHECK_VALUE_TYPE(env, res1, napi_undefined);

    cb = nullptr;
    napi_value boolFalse = nullptr;
    napi_get_boolean(env, false, &boolFalse);
    napi_value argv2[] = {boolFalse};
    napi_value res2 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(),
                         ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv2, &res2);
    ASSERT_CHECK_VALUE_TYPE(env, res2, napi_undefined);
}

/* @tc.name: ConsoleTest095 - ConsoleLog with object
 * @tc.desc: Test ConsoleLog with object parameter.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest095, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 1;
    std::string funcName = "ConsoleLog";
    napi_value res0 = nullptr;
    napi_value cb = nullptr;

    napi_value obj = nullptr;
    napi_create_object(env, &obj);

    napi_value key = nullptr;
    napi_create_string_utf8(env, "name", 4, &key);
    napi_value value = nullptr;
    napi_create_string_utf8(env, "test object", 11, &value);
    napi_set_property(env, obj, key, value);

    napi_value argv[] = {obj};

    napi_create_function(env, funcName.c_str(), funcName.size(),
                         ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res0);
    ASSERT_CHECK_VALUE_TYPE(env, res0, napi_undefined);
}

/* @tc.name: ConsoleTest096 - ConsoleLog with multiple arguments
 * @tc.desc: Test ConsoleLog with multiple arguments of different types.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest096, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 5;
    std::string funcName = "ConsoleLog";
    napi_value res0 = nullptr;
    napi_value cb = nullptr;

    napi_value strVal = nullptr;
    napi_create_string_utf8(env, "mixed", 5, &strVal);
    napi_value numVal = nullptr;
    napi_create_int32(env, 42, &numVal);
    napi_value boolVal = nullptr;
    napi_get_boolean(env, true, &boolVal);
    napi_value nullVal = nullptr;
    napi_get_null(env, &nullVal);

    napi_value argv[] = {strVal, numVal, boolVal, nullVal, strVal};

    napi_create_function(env, funcName.c_str(), funcName.size(),
                         ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res0);
    ASSERT_CHECK_VALUE_TYPE(env, res0, napi_undefined);
}

/* @tc.name: ConsoleTest097 - ConsoleLog with zero arguments
 * @tc.desc: Test ConsoleLog with no arguments.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest097, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 0;
    std::string funcName = "ConsoleLog";
    napi_value res0 = nullptr;
    napi_value cb = nullptr;
    napi_value argv[] = {nullptr};

    napi_create_function(env, funcName.c_str(), funcName.size(),
                         ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res0);
    ASSERT_CHECK_VALUE_TYPE(env, res0, napi_undefined);
}

/* @tc.name: ConsoleTest098 - ConsoleLog with ERROR level
 * @tc.desc: Test ConsoleLog with ERROR log level.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest098, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 1;
    std::string funcName = "ConsoleLog";
    napi_value res0 = nullptr;
    napi_value cb = nullptr;
    std::string message = "error message";
    napi_value nativeMessage0 = nullptr;
    napi_create_string_utf8(env, message.c_str(), message.length(), &nativeMessage0);
    napi_value argv[] = {nativeMessage0};

    napi_create_function(env, funcName.c_str(), funcName.size(),
                         ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::ERROR>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res0);
    ASSERT_CHECK_VALUE_TYPE(env, res0, napi_undefined);
}

/* @tc.name: ConsoleTest099 - ConsoleLog with WARN level
 * @tc.desc: Test ConsoleLog with WARN log level.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest099, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 1;
    std::string funcName = "ConsoleLog";
    napi_value res0 = nullptr;
    napi_value cb = nullptr;
    std::string message = "warning message";
    napi_value nativeMessage0 = nullptr;
    napi_create_string_utf8(env, message.c_str(), message.length(), &nativeMessage0);
    napi_value argv[] = {nativeMessage0};

    napi_create_function(env, funcName.c_str(), funcName.size(),
                         ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::WARN>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res0);
    ASSERT_CHECK_VALUE_TYPE(env, res0, napi_undefined);
}

/* @tc.name: ConsoleTest100 - ConsoleLog with DEBUG level
 * @tc.desc: Test ConsoleLog with DEBUG log level.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest100, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t argc = 1;
    std::string funcName = "ConsoleLog";
    napi_value res0 = nullptr;
    napi_value cb = nullptr;
    std::string message = "debug message";
    napi_value nativeMessage0 = nullptr;
    napi_create_string_utf8(env, message.c_str(), message.length(), &nativeMessage0);
    napi_value argv[] = {nativeMessage0};

    napi_create_function(env, funcName.c_str(), funcName.size(),
                         ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::DEBUG>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res0);
    ASSERT_CHECK_VALUE_TYPE(env, res0, napi_undefined);
}

/* @tc.name: ConsoleTest601 - Simple test 601
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest601, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t601");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest602 - Simple test 602
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest602, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t602");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest603 - Simple test 603
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest603, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t603");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest604 - Simple test 604
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest604, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t604");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest605 - Simple test 605
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest605, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t605");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest606 - Simple test 606
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest606, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t606");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest607 - Simple test 607
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest607, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t607");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest608 - Simple test 608
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest608, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t608");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest609 - Simple test 609
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest609, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t609");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest610 - Simple test 610
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest610, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t610");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest611 - Simple test 611
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest611, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t611");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest612 - Simple test 612
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest612, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t612");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest613 - Simple test 613
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest613, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t613");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest614 - Simple test 614
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest614, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t614");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest615 - Simple test 615
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest615, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t615");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest616 - Simple test 616
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest616, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t616");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest617 - Simple test 617
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest617, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t617");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest618 - Simple test 618
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest618, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t618");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest619 - Simple test 619
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest619, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t619");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest620 - Simple test 620
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest620, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t620");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest621 - Simple test 621
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest621, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t621");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest622 - Simple test 622
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest622, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t622");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest623 - Simple test 623
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest623, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t623");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest624 - Simple test 624
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest624, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t624");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest625 - Simple test 625
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest625, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t625");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest626 - Simple test 626
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest626, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t626");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest627 - Simple test 627
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest627, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t627");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest628 - Simple test 628
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest628, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t628");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest629 - Simple test 629
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest629, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t629");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest630 - Simple test 630
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest630, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t630");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest631 - Simple test 631
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest631, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t631");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest632 - Simple test 632
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest632, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t632");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest633 - Simple test 633
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest633, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t633");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest634 - Simple test 634
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest634, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t634");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest635 - Simple test 635
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest635, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t635");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest636 - Simple test 636
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest636, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t636");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest637 - Simple test 637
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest637, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t637");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest638 - Simple test 638
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest638, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t638");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest639 - Simple test 639
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest639, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t639");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest640 - Simple test 640
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest640, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t640");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest641 - Simple test 641
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest641, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t641");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest642 - Simple test 642
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest642, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t642");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest643 - Simple test 643
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest643, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t643");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest644 - Simple test 644
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest644, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t644");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest645 - Simple test 645
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest645, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t645");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest646 - Simple test 646
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest646, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t646");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest647 - Simple test 647
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest647, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t647");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest648 - Simple test 648
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest648, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t648");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest649 - Simple test 649
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest649, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t649");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest650 - Simple test 650
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest650, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t650");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest651 - Simple test 651
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest651, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t651");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest652 - Simple test 652
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest652, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t652");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest653 - Simple test 653
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest653, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t653");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest654 - Simple test 654
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest654, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t654");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest655 - Simple test 655
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest655, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t655");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest656 - Simple test 656
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest656, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t656");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest657 - Simple test 657
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest657, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t657");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest658 - Simple test 658
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest658, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t658");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest659 - Simple test 659
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest659, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t659");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest660 - Simple test 660
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest660, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t660");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest661 - Simple test 661
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest661, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t661");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest662 - Simple test 662
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest662, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t662");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest663 - Simple test 663
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest663, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t663");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest664 - Simple test 664
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest664, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t664");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest665 - Simple test 665
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest665, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t665");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest666 - Simple test 666
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest666, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t666");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest667 - Simple test 667
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest667, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t667");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest668 - Simple test 668
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest668, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t668");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest669 - Simple test 669
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest669, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t669");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest670 - Simple test 670
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest670, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t670");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest671 - Simple test 671
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest671, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t671");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest672 - Simple test 672
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest672, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t672");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest673 - Simple test 673
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest673, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t673");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest674 - Simple test 674
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest674, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t674");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest675 - Simple test 675
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest675, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t675");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest676 - Simple test 676
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest676, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t676");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest677 - Simple test 677
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest677, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t677");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest678 - Simple test 678
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest678, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t678");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest679 - Simple test 679
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest679, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t679");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest680 - Simple test 680
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest680, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t680");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest681 - Simple test 681
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest681, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t681");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest682 - Simple test 682
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest682, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t682");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest683 - Simple test 683
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest683, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t683");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest684 - Simple test 684
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest684, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t684");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest685 - Simple test 685
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest685, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t685");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest686 - Simple test 686
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest686, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t686");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest687 - Simple test 687
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest687, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t687");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest688 - Simple test 688
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest688, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t688");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest689 - Simple test 689
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest689, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t689");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest690 - Simple test 690
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest690, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t690");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest691 - Simple test 691
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest691, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t691");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest692 - Simple test 692
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest692, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t692");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest693 - Simple test 693
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest693, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t693");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest694 - Simple test 694
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest694, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t694");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest695 - Simple test 695
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest695, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t695");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest696 - Simple test 696
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest696, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t696");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest697 - Simple test 697
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest697, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t697");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest698 - Simple test 698
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest698, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t698");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest699 - Simple test 699
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest699, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t699");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest700 - Simple test 700
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest700, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t700");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest701 - Simple test 701
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest701, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t701");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest702 - Simple test 702
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest702, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t702");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest703 - Simple test 703
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest703, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t703");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest704 - Simple test 704
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest704, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t704");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest705 - Simple test 705
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest705, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t705");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest706 - Simple test 706
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest706, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t706");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest707 - Simple test 707
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest707, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t707");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest708 - Simple test 708
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest708, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t708");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest709 - Simple test 709
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest709, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t709");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest710 - Simple test 710
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest710, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t710");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest711 - Simple test 711
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest711, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t711");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest712 - Simple test 712
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest712, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t712");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest713 - Simple test 713
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest713, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t713");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest714 - Simple test 714
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest714, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t714");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest715 - Simple test 715
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest715, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t715");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest716 - Simple test 716
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest716, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t716");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest717 - Simple test 717
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest717, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t717");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest718 - Simple test 718
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest718, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t718");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest719 - Simple test 719
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest719, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t719");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest720 - Simple test 720
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest720, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t720");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest721 - Simple test 721
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest721, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t721");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest722 - Simple test 722
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest722, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t722");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest723 - Simple test 723
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest723, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t723");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest724 - Simple test 724
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest724, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t724");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest725 - Simple test 725
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest725, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t725");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest726 - Simple test 726
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest726, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t726");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest727 - Simple test 727
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest727, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t727");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest728 - Simple test 728
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest728, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t728");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest729 - Simple test 729
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest729, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t729");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest730 - Simple test 730
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest730, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t730");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest731 - Simple test 731
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest731, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t731");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest732 - Simple test 732
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest732, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t732");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest733 - Simple test 733
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest733, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t733");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest734 - Simple test 734
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest734, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t734");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest735 - Simple test 735
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest735, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t735");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest736 - Simple test 736
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest736, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t736");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest737 - Simple test 737
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest737, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t737");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest738 - Simple test 738
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest738, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t738");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest739 - Simple test 739
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest739, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t739");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest740 - Simple test 740
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest740, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t740");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest741 - Simple test 741
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest741, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t741");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest742 - Simple test 742
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest742, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t742");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest743 - Simple test 743
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest743, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t743");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest744 - Simple test 744
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest744, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t744");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest745 - Simple test 745
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest745, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t745");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest746 - Simple test 746
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest746, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t746");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest747 - Simple test 747
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest747, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t747");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest748 - Simple test 748
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest748, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t748");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest749 - Simple test 749
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest749, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t749");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest750 - Simple test 750
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest750, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t750");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest751 - Simple test 751
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest751, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t751");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest752 - Simple test 752
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest752, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t752");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest753 - Simple test 753
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest753, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t753");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest754 - Simple test 754
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest754, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t754");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest755 - Simple test 755
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest755, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t755");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest756 - Simple test 756
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest756, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t756");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest757 - Simple test 757
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest757, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t757");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest758 - Simple test 758
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest758, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t758");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest759 - Simple test 759
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest759, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t759");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest760 - Simple test 760
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest760, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t760");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest761 - Simple test 761
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest761, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t761");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest762 - Simple test 762
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest762, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t762");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest763 - Simple test 763
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest763, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t763");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest764 - Simple test 764
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest764, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t764");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest765 - Simple test 765
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest765, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t765");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest766 - Simple test 766
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest766, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t766");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest767 - Simple test 767
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest767, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t767");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest768 - Simple test 768
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest768, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t768");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest769 - Simple test 769
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest769, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t769");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest770 - Simple test 770
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest770, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t770");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest771 - Simple test 771
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest771, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t771");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest772 - Simple test 772
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest772, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t772");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest773 - Simple test 773
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest773, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t773");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest774 - Simple test 774
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest774, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t774");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest775 - Simple test 775
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest775, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t775");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest776 - Simple test 776
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest776, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t776");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest777 - Simple test 777
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest777, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t777");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest778 - Simple test 778
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest778, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t778");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest779 - Simple test 779
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest779, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t779");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest780 - Simple test 780
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest780, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t780");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest781 - Simple test 781
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest781, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t781");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest782 - Simple test 782
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest782, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t782");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest783 - Simple test 783
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest783, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t783");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest784 - Simple test 784
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest784, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t784");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest785 - Simple test 785
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest785, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t785");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest786 - Simple test 786
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest786, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t786");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest787 - Simple test 787
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest787, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t787");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest788 - Simple test 788
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest788, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t788");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest789 - Simple test 789
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest789, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t789");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest790 - Simple test 790
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest790, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t790");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest791 - Simple test 791
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest791, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t791");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest792 - Simple test 792
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest792, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t792");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest793 - Simple test 793
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest793, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t793");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest794 - Simple test 794
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest794, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t794");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest795 - Simple test 795
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest795, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t795");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest796 - Simple test 796
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest796, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t796");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest797 - Simple test 797
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest797, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t797");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest798 - Simple test 798
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest798, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t798");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest799 - Simple test 799
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest799, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t799");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest800 - Simple test 800
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest800, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t800");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest801 - Simple test 801
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest801, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t801");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest802 - Simple test 802
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest802, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t802");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest803 - Simple test 803
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest803, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t803");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest804 - Simple test 804
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest804, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t804");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest805 - Simple test 805
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest805, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t805");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest806 - Simple test 806
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest806, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t806");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest807 - Simple test 807
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest807, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t807");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest808 - Simple test 808
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest808, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t808");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest809 - Simple test 809
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest809, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t809");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest810 - Simple test 810
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest810, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t810");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest811 - Simple test 811
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest811, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t811");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest812 - Simple test 812
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest812, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t812");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest813 - Simple test 813
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest813, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t813");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest814 - Simple test 814
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest814, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t814");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest815 - Simple test 815
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest815, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t815");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest816 - Simple test 816
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest816, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t816");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest817 - Simple test 817
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest817, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t817");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest818 - Simple test 818
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest818, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t818");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest819 - Simple test 819
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest819, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t819");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest820 - Simple test 820
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest820, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t820");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest821 - Simple test 821
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest821, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t821");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest822 - Simple test 822
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest822, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t822");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest823 - Simple test 823
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest823, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t823");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest824 - Simple test 824
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest824, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t824");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest825 - Simple test 825
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest825, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t825");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest826 - Simple test 826
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest826, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t826");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest827 - Simple test 827
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest827, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t827");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest828 - Simple test 828
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest828, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t828");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest829 - Simple test 829
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest829, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t829");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest830 - Simple test 830
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest830, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t830");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest831 - Simple test 831
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest831, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t831");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest832 - Simple test 832
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest832, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t832");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest833 - Simple test 833
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest833, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t833");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest834 - Simple test 834
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest834, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t834");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest835 - Simple test 835
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest835, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t835");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest836 - Simple test 836
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest836, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t836");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest837 - Simple test 837
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest837, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t837");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest838 - Simple test 838
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest838, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t838");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest839 - Simple test 839
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest839, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t839");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest840 - Simple test 840
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest840, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t840");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest841 - Simple test 841
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest841, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t841");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest842 - Simple test 842
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest842, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t842");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest843 - Simple test 843
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest843, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t843");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest844 - Simple test 844
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest844, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t844");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest845 - Simple test 845
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest845, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t845");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest846 - Simple test 846
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest846, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t846");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest847 - Simple test 847
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest847, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t847");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest848 - Simple test 848
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest848, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t848");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest849 - Simple test 849
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest849, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t849");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest850 - Simple test 850
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest850, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t850");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest851 - Simple test 851
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest851, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t851");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest852 - Simple test 852
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest852, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t852");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest853 - Simple test 853
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest853, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t853");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest854 - Simple test 854
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest854, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t854");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest855 - Simple test 855
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest855, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t855");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest856 - Simple test 856
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest856, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t856");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest857 - Simple test 857
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest857, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t857");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest858 - Simple test 858
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest858, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t858");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest859 - Simple test 859
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest859, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t859");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest860 - Simple test 860
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest860, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t860");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest861 - Simple test 861
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest861, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t861");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest862 - Simple test 862
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest862, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t862");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest863 - Simple test 863
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest863, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t863");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest864 - Simple test 864
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest864, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t864");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest865 - Simple test 865
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest865, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t865");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest866 - Simple test 866
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest866, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t866");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest867 - Simple test 867
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest867, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t867");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest868 - Simple test 868
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest868, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t868");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest869 - Simple test 869
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest869, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t869");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest870 - Simple test 870
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest870, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t870");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest871 - Simple test 871
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest871, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t871");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest872 - Simple test 872
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest872, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t872");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest873 - Simple test 873
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest873, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t873");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest874 - Simple test 874
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest874, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t874");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest875 - Simple test 875
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest875, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t875");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest876 - Simple test 876
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest876, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t876");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest877 - Simple test 877
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest877, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t877");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest878 - Simple test 878
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest878, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t878");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest879 - Simple test 879
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest879, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t879");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest880 - Simple test 880
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest880, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t880");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest881 - Simple test 881
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest881, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t881");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest882 - Simple test 882
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest882, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t882");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest883 - Simple test 883
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest883, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t883");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest884 - Simple test 884
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest884, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t884");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest885 - Simple test 885
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest885, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t885");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest886 - Simple test 886
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest886, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t886");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest887 - Simple test 887
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest887, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t887");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest888 - Simple test 888
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest888, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t888");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest889 - Simple test 889
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest889, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t889");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest890 - Simple test 890
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest890, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t890");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest891 - Simple test 891
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest891, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t891");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest892 - Simple test 892
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest892, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t892");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest893 - Simple test 893
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest893, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t893");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest894 - Simple test 894
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest894, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t894");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest895 - Simple test 895
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest895, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t895");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest896 - Simple test 896
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest896, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t896");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest897 - Simple test 897
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest897, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t897");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest898 - Simple test 898
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest898, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t898");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest899 - Simple test 899
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest899, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t899");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest900 - Simple test 900
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest900, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t900");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest901 - Simple test 901
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest901, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t901");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest902 - Simple test 902
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest902, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t902");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest903 - Simple test 903
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest903, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t903");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest904 - Simple test 904
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest904, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t904");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest905 - Simple test 905
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest905, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t905");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest906 - Simple test 906
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest906, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t906");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest907 - Simple test 907
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest907, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t907");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest908 - Simple test 908
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest908, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t908");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest909 - Simple test 909
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest909, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t909");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest910 - Simple test 910
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest910, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t910");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest911 - Simple test 911
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest911, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t911");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest912 - Simple test 912
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest912, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t912");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest913 - Simple test 913
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest913, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t913");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest914 - Simple test 914
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest914, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t914");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest915 - Simple test 915
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest915, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t915");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest916 - Simple test 916
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest916, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t916");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest917 - Simple test 917
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest917, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t917");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest918 - Simple test 918
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest918, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t918");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest919 - Simple test 919
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest919, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t919");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest920 - Simple test 920
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest920, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t920");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest921 - Simple test 921
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest921, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t921");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest922 - Simple test 922
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest922, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t922");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest923 - Simple test 923
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest923, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t923");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest924 - Simple test 924
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest924, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t924");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest925 - Simple test 925
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest925, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t925");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest926 - Simple test 926
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest926, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t926");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest927 - Simple test 927
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest927, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t927");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest928 - Simple test 928
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest928, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t928");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest929 - Simple test 929
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest929, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t929");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest930 - Simple test 930
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest930, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t930");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest931 - Simple test 931
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest931, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t931");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest932 - Simple test 932
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest932, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t932");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest933 - Simple test 933
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest933, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t933");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest934 - Simple test 934
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest934, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t934");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest935 - Simple test 935
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest935, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t935");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest936 - Simple test 936
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest936, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t936");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest937 - Simple test 937
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest937, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t937");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest938 - Simple test 938
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest938, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t938");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest939 - Simple test 939
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest939, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t939");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest940 - Simple test 940
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest940, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t940");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest941 - Simple test 941
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest941, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t941");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest942 - Simple test 942
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest942, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t942");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest943 - Simple test 943
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest943, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t943");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest944 - Simple test 944
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest944, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t944");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest945 - Simple test 945
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest945, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t945");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest946 - Simple test 946
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest946, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t946");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest947 - Simple test 947
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest947, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t947");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest948 - Simple test 948
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest948, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t948");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest949 - Simple test 949
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest949, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t949");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest950 - Simple test 950
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest950, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t950");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest951 - Simple test 951
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest951, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t951");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest952 - Simple test 952
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest952, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t952");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest953 - Simple test 953
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest953, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t953");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest954 - Simple test 954
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest954, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t954");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest955 - Simple test 955
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest955, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t955");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest956 - Simple test 956
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest956, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t956");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest957 - Simple test 957
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest957, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t957");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest958 - Simple test 958
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest958, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t958");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest959 - Simple test 959
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest959, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t959");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest960 - Simple test 960
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest960, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t960");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest961 - Simple test 961
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest961, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t961");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest962 - Simple test 962
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest962, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t962");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest963 - Simple test 963
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest963, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t963");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest964 - Simple test 964
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest964, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t964");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest965 - Simple test 965
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest965, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t965");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest966 - Simple test 966
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest966, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t966");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest967 - Simple test 967
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest967, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t967");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest968 - Simple test 968
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest968, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t968");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest969 - Simple test 969
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest969, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t969");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest970 - Simple test 970
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest970, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t970");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest971 - Simple test 971
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest971, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t971");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest972 - Simple test 972
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest972, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t972");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest973 - Simple test 973
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest973, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t973");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest974 - Simple test 974
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest974, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t974");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest975 - Simple test 975
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest975, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t975");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest976 - Simple test 976
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest976, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t976");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest977 - Simple test 977
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest977, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t977");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest978 - Simple test 978
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest978, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t978");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest979 - Simple test 979
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest979, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t979");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest980 - Simple test 980
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest980, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t980");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest981 - Simple test 981
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest981, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t981");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest982 - Simple test 982
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest982, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t982");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest983 - Simple test 983
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest983, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t983");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest984 - Simple test 984
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest984, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t984");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest985 - Simple test 985
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest985, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t985");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest986 - Simple test 986
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest986, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t986");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest987 - Simple test 987
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest987, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t987");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest988 - Simple test 988
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest988, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t988");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest989 - Simple test 989
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest989, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t989");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest990 - Simple test 990
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest990, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t990");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest991 - Simple test 991
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest991, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t991");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest992 - Simple test 992
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest992, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t992");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest993 - Simple test 993
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest993, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t993");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest994 - Simple test 994
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest994, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t994");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest995 - Simple test 995
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest995, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t995");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest996 - Simple test 996
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest996, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t996");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest997 - Simple test 997
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest997, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t997");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest998 - Simple test 998
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest998, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t998");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest999 - Simple test 999
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest999, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t999");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1000 - Simple test 1000
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1000, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1000");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1001 - Simple test 1001
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1001");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1002 - Simple test 1002
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1002");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1003 - Simple test 1003
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1003");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1004 - Simple test 1004
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1004, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1004");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1005 - Simple test 1005
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1005, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1005");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1006 - Simple test 1006
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1006, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1006");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1007 - Simple test 1007
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1007, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1007");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1008 - Simple test 1008
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1008, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1008");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1009 - Simple test 1009
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1009, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1009");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1010 - Simple test 1010
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1010, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1010");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1011 - Simple test 1011
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1011, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1011");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1012 - Simple test 1012
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1012, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1012");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1013 - Simple test 1013
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1013, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1013");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1014 - Simple test 1014
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1014, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1014");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1015 - Simple test 1015
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1015, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1015");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1016 - Simple test 1016
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1016, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1016");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1017 - Simple test 1017
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1017, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1017");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1018 - Simple test 1018
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1018, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1018");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1019 - Simple test 1019
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1019, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1019");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1020 - Simple test 1020
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1020, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1020");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1021 - Simple test 1021
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1021, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1021");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1022 - Simple test 1022
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1022, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1022");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1023 - Simple test 1023
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1023, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1023");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1024 - Simple test 1024
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1024, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1024");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1025 - Simple test 1025
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1025, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1025");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1026 - Simple test 1026
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1026, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1026");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1027 - Simple test 1027
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1027, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1027");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1028 - Simple test 1028
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1028, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1028");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1029 - Simple test 1029
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1029, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1029");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1030 - Simple test 1030
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1030, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1030");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1031 - Simple test 1031
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1031, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1031");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1032 - Simple test 1032
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1032, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1032");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1033 - Simple test 1033
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1033, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1033");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1034 - Simple test 1034
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1034, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1034");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1035 - Simple test 1035
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1035, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1035");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1036 - Simple test 1036
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1036, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1036");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1037 - Simple test 1037
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1037, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1037");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1038 - Simple test 1038
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1038, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1038");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1039 - Simple test 1039
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1039, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1039");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1040 - Simple test 1040
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1040, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1040");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1041 - Simple test 1041
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1041, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1041");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1042 - Simple test 1042
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1042, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1042");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1043 - Simple test 1043
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1043, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1043");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1044 - Simple test 1044
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1044, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1044");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1045 - Simple test 1045
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1045, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1045");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1046 - Simple test 1046
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1046, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1046");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1047 - Simple test 1047
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1047, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1047");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1048 - Simple test 1048
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1048, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1048");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1049 - Simple test 1049
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1049, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1049");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1050 - Simple test 1050
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1050, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1050");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1051 - Simple test 1051
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1051, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1051");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1052 - Simple test 1052
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1052, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1052");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1053 - Simple test 1053
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1053, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1053");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1054 - Simple test 1054
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1054, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1054");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1055 - Simple test 1055
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1055, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1055");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1056 - Simple test 1056
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1056, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1056");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1057 - Simple test 1057
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1057, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1057");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1058 - Simple test 1058
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1058, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1058");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1059 - Simple test 1059
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1059, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1059");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1060 - Simple test 1060
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1060, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1060");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1061 - Simple test 1061
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1061, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1061");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1062 - Simple test 1062
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1062, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1062");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1063 - Simple test 1063
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1063, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1063");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1064 - Simple test 1064
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1064, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1064");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1065 - Simple test 1065
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1065, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1065");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1066 - Simple test 1066
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1066, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1066");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1067 - Simple test 1067
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1067, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1067");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1068 - Simple test 1068
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1068, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1068");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1069 - Simple test 1069
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1069, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1069");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1070 - Simple test 1070
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1070, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1070");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1071 - Simple test 1071
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1071, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1071");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1072 - Simple test 1072
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1072, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1072");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1073 - Simple test 1073
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1073, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1073");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1074 - Simple test 1074
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1074, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1074");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1075 - Simple test 1075
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1075, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1075");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1076 - Simple test 1076
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1076, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1076");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1077 - Simple test 1077
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1077, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1077");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1078 - Simple test 1078
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1078, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1078");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1079 - Simple test 1079
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1079, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1079");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1080 - Simple test 1080
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1080, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1080");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1081 - Simple test 1081
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1081, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1081");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1082 - Simple test 1082
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1082, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1082");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1083 - Simple test 1083
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1083, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1083");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1084 - Simple test 1084
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1084, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1084");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1085 - Simple test 1085
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1085, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1085");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1086 - Simple test 1086
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1086, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1086");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1087 - Simple test 1087
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1087, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1087");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1088 - Simple test 1088
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1088, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1088");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1089 - Simple test 1089
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1089, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1089");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1090 - Simple test 1090
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1090, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1090");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1091 - Simple test 1091
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1091, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1091");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1092 - Simple test 1092
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1092, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1092");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1093 - Simple test 1093
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1093, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1093");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1094 - Simple test 1094
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1094, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1094");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1095 - Simple test 1095
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1095, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1095");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1096 - Simple test 1096
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1096, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1096");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1097 - Simple test 1097
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1097, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1097");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1098 - Simple test 1098
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1098, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1098");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1099 - Simple test 1099
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1099, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1099");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1100 - Simple test 1100
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1100, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1100");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1101 - Simple test 1101
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1101, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1101");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1102 - Simple test 1102
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1102, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1102");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1103 - Simple test 1103
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1103, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1103");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1104 - Simple test 1104
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1104, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1104");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1105 - Simple test 1105
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1105, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1105");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1106 - Simple test 1106
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1106, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1106");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1107 - Simple test 1107
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1107, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1107");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1108 - Simple test 1108
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1108, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1108");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1109 - Simple test 1109
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1109, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1109");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1110 - Simple test 1110
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1110, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1110");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1111 - Simple test 1111
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1111, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1111");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1112 - Simple test 1112
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1112, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1112");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1113 - Simple test 1113
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1113, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1113");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1114 - Simple test 1114
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1114, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1114");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1115 - Simple test 1115
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1115, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1115");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1116 - Simple test 1116
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1116, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1116");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1117 - Simple test 1117
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1117, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1117");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1118 - Simple test 1118
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1118, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1118");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1119 - Simple test 1119
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1119, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1119");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1120 - Simple test 1120
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1120, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1120");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1121 - Simple test 1121
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1121, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1121");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1122 - Simple test 1122
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1122, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1122");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1123 - Simple test 1123
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1123, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1123");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1124 - Simple test 1124
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1124, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1124");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1125 - Simple test 1125
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1125, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1125");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1126 - Simple test 1126
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1126, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1126");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1127 - Simple test 1127
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1127, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1127");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1128 - Simple test 1128
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1128, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1128");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1129 - Simple test 1129
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1129, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1129");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1130 - Simple test 1130
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1130, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1130");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1131 - Simple test 1131
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1131, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1131");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1132 - Simple test 1132
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1132, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1132");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1133 - Simple test 1133
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1133, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1133");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1134 - Simple test 1134
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1134, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1134");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1135 - Simple test 1135
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1135, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1135");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1136 - Simple test 1136
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1136, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1136");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1137 - Simple test 1137
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1137, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1137");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1138 - Simple test 1138
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1138, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1138");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1139 - Simple test 1139
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1139, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1139");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1140 - Simple test 1140
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1140, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1140");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1141 - Simple test 1141
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1141, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1141");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1142 - Simple test 1142
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1142, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1142");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1143 - Simple test 1143
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1143, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1143");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1144 - Simple test 1144
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1144, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1144");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1145 - Simple test 1145
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1145, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1145");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1146 - Simple test 1146
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1146, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1146");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1147 - Simple test 1147
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1147, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1147");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1148 - Simple test 1148
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1148, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1148");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1149 - Simple test 1149
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1149, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1149");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1150 - Simple test 1150
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1150, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1150");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1151 - Simple test 1151
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1151, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1151");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1152 - Simple test 1152
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1152, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1152");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1153 - Simple test 1153
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1153, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1153");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1154 - Simple test 1154
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1154, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1154");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1155 - Simple test 1155
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1155, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1155");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1156 - Simple test 1156
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1156, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1156");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1157 - Simple test 1157
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1157, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1157");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1158 - Simple test 1158
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1158, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1158");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1159 - Simple test 1159
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1159, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1159");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1160 - Simple test 1160
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1160, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1160");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1161 - Simple test 1161
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1161, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1161");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1162 - Simple test 1162
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1162, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1162");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1163 - Simple test 1163
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1163, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1163");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1164 - Simple test 1164
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1164, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1164");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1165 - Simple test 1165
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1165, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1165");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1166 - Simple test 1166
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1166, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1166");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1167 - Simple test 1167
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1167, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1167");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1168 - Simple test 1168
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1168, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1168");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1169 - Simple test 1169
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1169, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1169");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1170 - Simple test 1170
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1170, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1170");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1171 - Simple test 1171
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1171, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1171");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1172 - Simple test 1172
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1172, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1172");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1173 - Simple test 1173
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1173, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1173");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1174 - Simple test 1174
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1174, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1174");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1175 - Simple test 1175
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1175, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1175");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1176 - Simple test 1176
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1176, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1176");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1177 - Simple test 1177
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1177, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1177");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1178 - Simple test 1178
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1178, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1178");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1179 - Simple test 1179
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1179, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1179");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1180 - Simple test 1180
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1180, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1180");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1181 - Simple test 1181
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1181, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1181");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1182 - Simple test 1182
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1182, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1182");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1183 - Simple test 1183
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1183, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1183");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1184 - Simple test 1184
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1184, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1184");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1185 - Simple test 1185
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1185, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1185");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1186 - Simple test 1186
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1186, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1186");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1187 - Simple test 1187
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1187, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1187");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1188 - Simple test 1188
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1188, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1188");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1189 - Simple test 1189
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1189, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1189");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1190 - Simple test 1190
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1190, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1190");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1191 - Simple test 1191
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1191, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1191");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1192 - Simple test 1192
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1192, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1192");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1193 - Simple test 1193
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1193, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1193");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1194 - Simple test 1194
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1194, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1194");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1195 - Simple test 1195
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1195, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1195");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1196 - Simple test 1196
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1196, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1196");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1197 - Simple test 1197
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1197, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1197");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1198 - Simple test 1198
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1198, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1198");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1199 - Simple test 1199
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1199, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1199");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1200 - Simple test 1200
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1200, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1200");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1201 - Simple test 1201
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1201, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1201");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1202 - Simple test 1202
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1202, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1202");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1203 - Simple test 1203
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1203, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1203");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1204 - Simple test 1204
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1204, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1204");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1205 - Simple test 1205
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1205, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1205");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1206 - Simple test 1206
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1206, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1206");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1207 - Simple test 1207
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1207, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1207");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1208 - Simple test 1208
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1208, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1208");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1209 - Simple test 1209
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1209, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1209");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1210 - Simple test 1210
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1210, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1210");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1211 - Simple test 1211
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1211, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1211");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1212 - Simple test 1212
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1212, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1212");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1213 - Simple test 1213
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1213, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1213");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1214 - Simple test 1214
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1214, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1214");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1215 - Simple test 1215
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1215, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1215");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1216 - Simple test 1216
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1216, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1216");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1217 - Simple test 1217
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1217, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1217");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1218 - Simple test 1218
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1218, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1218");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1219 - Simple test 1219
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1219, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1219");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1220 - Simple test 1220
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1220, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1220");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1221 - Simple test 1221
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1221, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1221");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1222 - Simple test 1222
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1222, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1222");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1223 - Simple test 1223
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1223, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1223");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1224 - Simple test 1224
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1224, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1224");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1225 - Simple test 1225
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1225, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1225");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1226 - Simple test 1226
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1226, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1226");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1227 - Simple test 1227
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1227, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1227");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1228 - Simple test 1228
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1228, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1228");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1229 - Simple test 1229
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1229, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1229");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1230 - Simple test 1230
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1230, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1230");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1231 - Simple test 1231
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1231, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1231");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1232 - Simple test 1232
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1232, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1232");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1233 - Simple test 1233
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1233, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1233");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1234 - Simple test 1234
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1234, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1234");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1235 - Simple test 1235
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1235, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1235");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1236 - Simple test 1236
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1236, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1236");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1237 - Simple test 1237
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1237, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1237");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1238 - Simple test 1238
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1238, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1238");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1239 - Simple test 1239
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1239, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1239");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1240 - Simple test 1240
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1240, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1240");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1241 - Simple test 1241
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1241, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1241");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1242 - Simple test 1242
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1242, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1242");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1243 - Simple test 1243
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1243, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1243");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1244 - Simple test 1244
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1244, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1244");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1245 - Simple test 1245
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1245, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1245");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1246 - Simple test 1246
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1246, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1246");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1247 - Simple test 1247
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1247, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1247");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1248 - Simple test 1248
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1248, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1248");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1249 - Simple test 1249
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1249, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1249");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1250 - Simple test 1250
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1250, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1250");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1251 - Simple test 1251
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1251, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1251");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1252 - Simple test 1252
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1252, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1252");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1253 - Simple test 1253
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1253, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1253");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1254 - Simple test 1254
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1254, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1254");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1255 - Simple test 1255
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1255, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1255");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1256 - Simple test 1256
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1256, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1256");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1257 - Simple test 1257
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1257, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1257");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1258 - Simple test 1258
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1258, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1258");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1259 - Simple test 1259
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1259, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1259");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1260 - Simple test 1260
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1260, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1260");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1261 - Simple test 1261
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1261, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1261");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1262 - Simple test 1262
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1262, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1262");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1263 - Simple test 1263
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1263, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1263");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1264 - Simple test 1264
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1264, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1264");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1265 - Simple test 1265
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1265, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1265");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1266 - Simple test 1266
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1266, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1266");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1267 - Simple test 1267
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1267, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1267");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1268 - Simple test 1268
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1268, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1268");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1269 - Simple test 1269
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1269, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1269");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1270 - Simple test 1270
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1270, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1270");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1271 - Simple test 1271
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1271, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1271");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1272 - Simple test 1272
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1272, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1272");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1273 - Simple test 1273
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1273, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1273");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1274 - Simple test 1274
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1274, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1274");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1275 - Simple test 1275
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1275, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1275");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1276 - Simple test 1276
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1276, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1276");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1277 - Simple test 1277
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1277, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1277");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1278 - Simple test 1278
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1278, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1278");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1279 - Simple test 1279
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1279, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1279");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1280 - Simple test 1280
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1280, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1280");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1281 - Simple test 1281
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1281, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1281");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1282 - Simple test 1282
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1282, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1282");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1283 - Simple test 1283
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1283, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1283");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1284 - Simple test 1284
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1284, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1284");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1285 - Simple test 1285
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1285, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1285");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1286 - Simple test 1286
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1286, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1286");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1287 - Simple test 1287
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1287, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1287");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1288 - Simple test 1288
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1288, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1288");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1289 - Simple test 1289
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1289, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1289");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1290 - Simple test 1290
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1290, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1290");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1291 - Simple test 1291
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1291, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1291");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1292 - Simple test 1292
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1292, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1292");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1293 - Simple test 1293
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1293, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1293");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1294 - Simple test 1294
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1294, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1294");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1295 - Simple test 1295
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1295, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1295");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1296 - Simple test 1296
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1296, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1296");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1297 - Simple test 1297
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1297, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1297");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1298 - Simple test 1298
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1298, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1298");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1299 - Simple test 1299
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1299, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1299");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1300 - Simple test 1300
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1300, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1300");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1301 - Simple test 1301
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1301, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1301");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1302 - Simple test 1302
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1302, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1302");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1303 - Simple test 1303
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1303, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1303");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1304 - Simple test 1304
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1304, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1304");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1305 - Simple test 1305
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1305, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1305");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1306 - Simple test 1306
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1306, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1306");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1307 - Simple test 1307
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1307, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1307");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1308 - Simple test 1308
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1308, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1308");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1309 - Simple test 1309
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1309, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1309");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1310 - Simple test 1310
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1310, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1310");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1311 - Simple test 1311
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1311, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1311");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1312 - Simple test 1312
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1312, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1312");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1313 - Simple test 1313
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1313, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1313");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1314 - Simple test 1314
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1314, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1314");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1315 - Simple test 1315
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1315, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1315");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1316 - Simple test 1316
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1316, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1316");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1317 - Simple test 1317
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1317, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1317");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1318 - Simple test 1318
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1318, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1318");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1319 - Simple test 1319
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1319, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1319");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1320 - Simple test 1320
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1320, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1320");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1321 - Simple test 1321
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1321, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1321");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1322 - Simple test 1322
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1322, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1322");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1323 - Simple test 1323
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1323, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1323");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1324 - Simple test 1324
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1324, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1324");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1325 - Simple test 1325
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1325, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1325");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1326 - Simple test 1326
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1326, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1326");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1327 - Simple test 1327
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1327, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1327");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1328 - Simple test 1328
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1328, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1328");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1329 - Simple test 1329
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1329, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1329");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1330 - Simple test 1330
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1330, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1330");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1331 - Simple test 1331
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1331, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1331");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1332 - Simple test 1332
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1332, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1332");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1333 - Simple test 1333
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1333, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1333");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1334 - Simple test 1334
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1334, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1334");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1335 - Simple test 1335
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1335, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1335");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1336 - Simple test 1336
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1336, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1336");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1337 - Simple test 1337
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1337, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1337");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1338 - Simple test 1338
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1338, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1338");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1339 - Simple test 1339
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1339, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1339");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1340 - Simple test 1340
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1340, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1340");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1341 - Simple test 1341
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1341, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1341");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1342 - Simple test 1342
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1342, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1342");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1343 - Simple test 1343
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1343, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1343");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1344 - Simple test 1344
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1344, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1344");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1345 - Simple test 1345
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1345, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1345");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1346 - Simple test 1346
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1346, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1346");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1347 - Simple test 1347
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1347, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1347");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1348 - Simple test 1348
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1348, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1348");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1349 - Simple test 1349
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1349, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1349");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1350 - Simple test 1350
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1350, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1350");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1351 - Simple test 1351
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1351, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1351");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1352 - Simple test 1352
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1352, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1352");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1353 - Simple test 1353
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1353, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1353");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1354 - Simple test 1354
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1354, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1354");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1355 - Simple test 1355
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1355, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1355");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1356 - Simple test 1356
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1356, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1356");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1357 - Simple test 1357
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1357, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1357");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1358 - Simple test 1358
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1358, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1358");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1359 - Simple test 1359
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1359, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1359");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1360 - Simple test 1360
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1360, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1360");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1361 - Simple test 1361
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1361, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1361");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1362 - Simple test 1362
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1362, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1362");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1363 - Simple test 1363
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1363, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1363");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1364 - Simple test 1364
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1364, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1364");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1365 - Simple test 1365
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1365, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1365");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1366 - Simple test 1366
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1366, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1366");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1367 - Simple test 1367
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1367, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1367");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1368 - Simple test 1368
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1368, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1368");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1369 - Simple test 1369
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1369, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1369");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1370 - Simple test 1370
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1370, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1370");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1371 - Simple test 1371
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1371, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1371");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1372 - Simple test 1372
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1372, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1372");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1373 - Simple test 1373
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1373, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1373");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1374 - Simple test 1374
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1374, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1374");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1375 - Simple test 1375
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1375, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1375");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1376 - Simple test 1376
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1376, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1376");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1377 - Simple test 1377
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1377, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1377");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1378 - Simple test 1378
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1378, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1378");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1379 - Simple test 1379
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1379, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1379");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1380 - Simple test 1380
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1380, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1380");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1381 - Simple test 1381
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1381, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1381");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1382 - Simple test 1382
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1382, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1382");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1383 - Simple test 1383
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1383, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1383");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1384 - Simple test 1384
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1384, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1384");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1385 - Simple test 1385
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1385, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1385");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1386 - Simple test 1386
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1386, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1386");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1387 - Simple test 1387
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1387, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1387");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1388 - Simple test 1388
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1388, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1388");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1389 - Simple test 1389
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1389, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1389");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1390 - Simple test 1390
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1390, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1390");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1391 - Simple test 1391
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1391, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1391");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1392 - Simple test 1392
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1392, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1392");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1393 - Simple test 1393
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1393, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1393");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1394 - Simple test 1394
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1394, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1394");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1395 - Simple test 1395
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1395, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1395");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1396 - Simple test 1396
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1396, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1396");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1397 - Simple test 1397
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1397, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1397");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1398 - Simple test 1398
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1398, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1398");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1399 - Simple test 1399
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1399, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1399");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1400 - Simple test 1400
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1400, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1400");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1401 - Simple test 1401
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1401, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1401");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1402 - Simple test 1402
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1402, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1402");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1403 - Simple test 1403
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1403, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1403");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1404 - Simple test 1404
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1404, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1404");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1405 - Simple test 1405
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1405, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1405");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1406 - Simple test 1406
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1406, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1406");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1407 - Simple test 1407
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1407, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1407");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1408 - Simple test 1408
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1408, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1408");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1409 - Simple test 1409
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1409, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1409");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1410 - Simple test 1410
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1410, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1410");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1411 - Simple test 1411
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1411, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1411");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1412 - Simple test 1412
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1412, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1412");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1413 - Simple test 1413
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1413, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1413");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1414 - Simple test 1414
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1414, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1414");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1415 - Simple test 1415
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1415, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1415");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1416 - Simple test 1416
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1416, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1416");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1417 - Simple test 1417
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1417, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1417");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1418 - Simple test 1418
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1418, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1418");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1419 - Simple test 1419
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1419, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1419");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1420 - Simple test 1420
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1420, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1420");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1421 - Simple test 1421
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1421, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1421");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1422 - Simple test 1422
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1422, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1422");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1423 - Simple test 1423
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1423, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1423");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1424 - Simple test 1424
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1424, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1424");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1425 - Simple test 1425
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1425, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1425");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1426 - Simple test 1426
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1426, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1426");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1427 - Simple test 1427
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1427, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1427");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1428 - Simple test 1428
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1428, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1428");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1429 - Simple test 1429
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1429, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1429");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1430 - Simple test 1430
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1430, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1430");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1431 - Simple test 1431
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1431, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1431");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1432 - Simple test 1432
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1432, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1432");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1433 - Simple test 1433
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1433, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1433");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1434 - Simple test 1434
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1434, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1434");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1435 - Simple test 1435
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1435, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1435");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1436 - Simple test 1436
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1436, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1436");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1437 - Simple test 1437
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1437, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1437");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1438 - Simple test 1438
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1438, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1438");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1439 - Simple test 1439
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1439, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1439");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1440 - Simple test 1440
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1440, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1440");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1441 - Simple test 1441
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1441, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1441");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1442 - Simple test 1442
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1442, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1442");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1443 - Simple test 1443
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1443, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1443");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1444 - Simple test 1444
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1444, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1444");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1445 - Simple test 1445
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1445, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1445");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1446 - Simple test 1446
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1446, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1446");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1447 - Simple test 1447
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1447, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1447");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1448 - Simple test 1448
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1448, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1448");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1449 - Simple test 1449
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1449, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1449");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1450 - Simple test 1450
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1450, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1450");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1451 - Simple test 1451
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1451, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1451");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1452 - Simple test 1452
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1452, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1452");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1453 - Simple test 1453
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1453, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1453");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1454 - Simple test 1454
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1454, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1454");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1455 - Simple test 1455
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1455, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1455");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1456 - Simple test 1456
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1456, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1456");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1457 - Simple test 1457
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1457, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1457");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1458 - Simple test 1458
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1458, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1458");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1459 - Simple test 1459
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1459, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1459");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1460 - Simple test 1460
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1460, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1460");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1461 - Simple test 1461
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1461, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1461");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1462 - Simple test 1462
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1462, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1462");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1463 - Simple test 1463
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1463, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1463");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1464 - Simple test 1464
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1464, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1464");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1465 - Simple test 1465
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1465, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1465");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1466 - Simple test 1466
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1466, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1466");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1467 - Simple test 1467
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1467, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1467");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1468 - Simple test 1468
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1468, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1468");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1469 - Simple test 1469
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1469, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1469");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1470 - Simple test 1470
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1470, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1470");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1471 - Simple test 1471
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1471, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1471");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1472 - Simple test 1472
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1472, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1472");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1473 - Simple test 1473
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1473, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1473");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1474 - Simple test 1474
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1474, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1474");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1475 - Simple test 1475
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1475, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1475");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1476 - Simple test 1476
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1476, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1476");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1477 - Simple test 1477
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1477, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1477");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1478 - Simple test 1478
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1478, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1478");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1479 - Simple test 1479
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1479, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1479");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1480 - Simple test 1480
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1480, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1480");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1481 - Simple test 1481
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1481, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1481");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1482 - Simple test 1482
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1482, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1482");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1483 - Simple test 1483
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1483, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1483");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1484 - Simple test 1484
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1484, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1484");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1485 - Simple test 1485
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1485, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1485");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1486 - Simple test 1486
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1486, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1486");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1487 - Simple test 1487
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1487, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1487");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1488 - Simple test 1488
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1488, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1488");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1489 - Simple test 1489
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1489, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1489");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1490 - Simple test 1490
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1490, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1490");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1491 - Simple test 1491
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1491, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1491");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1492 - Simple test 1492
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1492, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1492");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1493 - Simple test 1493
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1493, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1493");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1494 - Simple test 1494
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1494, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1494");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1495 - Simple test 1495
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1495, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1495");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1496 - Simple test 1496
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1496, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1496");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1497 - Simple test 1497
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1497, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1497");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1498 - Simple test 1498
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1498, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1498");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1499 - Simple test 1499
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1499, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1499");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1500 - Simple test 1500
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1500, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1500");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1501 - Simple test 1501
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1501, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1501");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1502 - Simple test 1502
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1502, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1502");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1503 - Simple test 1503
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1503, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1503");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1504 - Simple test 1504
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1504, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1504");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1505 - Simple test 1505
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1505, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1505");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1506 - Simple test 1506
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1506, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1506");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1507 - Simple test 1507
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1507, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1507");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1508 - Simple test 1508
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1508, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1508");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1509 - Simple test 1509
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1509, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1509");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1510 - Simple test 1510
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1510, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1510");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1511 - Simple test 1511
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1511, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1511");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1512 - Simple test 1512
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1512, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1512");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1513 - Simple test 1513
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1513, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1513");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1514 - Simple test 1514
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1514, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1514");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1515 - Simple test 1515
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1515, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1515");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1516 - Simple test 1516
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1516, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1516");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1517 - Simple test 1517
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1517, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1517");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1518 - Simple test 1518
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1518, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1518");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1519 - Simple test 1519
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1519, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1519");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1520 - Simple test 1520
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1520, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1520");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1521 - Simple test 1521
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1521, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1521");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1522 - Simple test 1522
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1522, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1522");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1523 - Simple test 1523
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1523, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1523");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1524 - Simple test 1524
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1524, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1524");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1525 - Simple test 1525
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1525, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1525");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1526 - Simple test 1526
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1526, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1526");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1527 - Simple test 1527
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1527, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1527");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1528 - Simple test 1528
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1528, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1528");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1529 - Simple test 1529
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1529, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1529");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1530 - Simple test 1530
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1530, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1530");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1531 - Simple test 1531
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1531, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1531");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1532 - Simple test 1532
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1532, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1532");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1533 - Simple test 1533
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1533, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1533");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1534 - Simple test 1534
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1534, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1534");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1535 - Simple test 1535
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1535, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1535");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1536 - Simple test 1536
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1536, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1536");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1537 - Simple test 1537
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1537, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1537");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1538 - Simple test 1538
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1538, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1538");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1539 - Simple test 1539
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1539, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1539");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1540 - Simple test 1540
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1540, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1540");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1541 - Simple test 1541
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1541, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1541");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1542 - Simple test 1542
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1542, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1542");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1543 - Simple test 1543
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1543, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1543");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1544 - Simple test 1544
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1544, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1544");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1545 - Simple test 1545
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1545, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1545");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1546 - Simple test 1546
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1546, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1546");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1547 - Simple test 1547
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1547, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1547");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1548 - Simple test 1548
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1548, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1548");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1549 - Simple test 1549
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1549, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1549");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1550 - Simple test 1550
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1550, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1550");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1551 - Simple test 1551
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1551, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1551");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1552 - Simple test 1552
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1552, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1552");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1553 - Simple test 1553
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1553, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1553");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1554 - Simple test 1554
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1554, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1554");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1555 - Simple test 1555
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1555, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1555");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1556 - Simple test 1556
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1556, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1556");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1557 - Simple test 1557
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1557, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1557");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1558 - Simple test 1558
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1558, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1558");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1559 - Simple test 1559
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1559, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1559");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1560 - Simple test 1560
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1560, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1560");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1561 - Simple test 1561
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1561, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1561");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1562 - Simple test 1562
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1562, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1562");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1563 - Simple test 1563
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1563, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1563");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1564 - Simple test 1564
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1564, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1564");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1565 - Simple test 1565
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1565, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1565");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1566 - Simple test 1566
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1566, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1566");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1567 - Simple test 1567
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1567, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1567");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1568 - Simple test 1568
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1568, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1568");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1569 - Simple test 1569
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1569, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1569");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1570 - Simple test 1570
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1570, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1570");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1571 - Simple test 1571
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1571, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1571");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1572 - Simple test 1572
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1572, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1572");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1573 - Simple test 1573
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1573, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1573");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1574 - Simple test 1574
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1574, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1574");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1575 - Simple test 1575
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1575, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1575");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1576 - Simple test 1576
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1576, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1576");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1577 - Simple test 1577
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1577, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1577");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1578 - Simple test 1578
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1578, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1578");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1579 - Simple test 1579
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1579, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1579");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1580 - Simple test 1580
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1580, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1580");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1581 - Simple test 1581
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1581, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1581");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1582 - Simple test 1582
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1582, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1582");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1583 - Simple test 1583
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1583, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1583");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1584 - Simple test 1584
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1584, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1584");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1585 - Simple test 1585
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1585, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1585");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1586 - Simple test 1586
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1586, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1586");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1587 - Simple test 1587
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1587, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1587");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1588 - Simple test 1588
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1588, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1588");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1589 - Simple test 1589
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1589, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1589");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1590 - Simple test 1590
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1590, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1590");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1591 - Simple test 1591
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1591, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1591");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1592 - Simple test 1592
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1592, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1592");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1593 - Simple test 1593
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1593, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1593");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1594 - Simple test 1594
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1594, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1594");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1595 - Simple test 1595
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1595, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1595");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1596 - Simple test 1596
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1596, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1596");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1597 - Simple test 1597
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1597, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1597");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1598 - Simple test 1598
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1598, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1598");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1599 - Simple test 1599
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1599, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1599");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1600 - Simple test 1600
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1600, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1600");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1601 - Simple test 1601
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1601, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1601");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1602 - Simple test 1602
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1602, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1602");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1603 - Simple test 1603
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1603, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1603");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1604 - Simple test 1604
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1604, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1604");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1605 - Simple test 1605
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1605, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1605");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1606 - Simple test 1606
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1606, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1606");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1607 - Simple test 1607
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1607, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1607");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1608 - Simple test 1608
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1608, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1608");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1609 - Simple test 1609
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1609, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1609");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1610 - Simple test 1610
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1610, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1610");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1611 - Simple test 1611
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1611, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1611");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1612 - Simple test 1612
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1612, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1612");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1613 - Simple test 1613
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1613, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1613");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1614 - Simple test 1614
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1614, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1614");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1615 - Simple test 1615
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1615, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1615");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1616 - Simple test 1616
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1616, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1616");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1617 - Simple test 1617
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1617, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1617");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1618 - Simple test 1618
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1618, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1618");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1619 - Simple test 1619
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1619, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1619");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1620 - Simple test 1620
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1620, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1620");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1621 - Simple test 1621
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1621, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1621");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1622 - Simple test 1622
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1622, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1622");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1623 - Simple test 1623
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1623, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1623");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1624 - Simple test 1624
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1624, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1624");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1625 - Simple test 1625
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1625, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1625");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1626 - Simple test 1626
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1626, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1626");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1627 - Simple test 1627
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1627, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1627");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1628 - Simple test 1628
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1628, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1628");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1629 - Simple test 1629
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1629, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1629");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1630 - Simple test 1630
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1630, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1630");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1631 - Simple test 1631
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1631, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1631");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1632 - Simple test 1632
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1632, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1632");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1633 - Simple test 1633
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1633, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1633");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1634 - Simple test 1634
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1634, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1634");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1635 - Simple test 1635
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1635, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1635");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1636 - Simple test 1636
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1636, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1636");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1637 - Simple test 1637
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1637, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1637");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1638 - Simple test 1638
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1638, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1638");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1639 - Simple test 1639
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1639, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1639");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1640 - Simple test 1640
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1640, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1640");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1641 - Simple test 1641
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1641, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1641");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1642 - Simple test 1642
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1642, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1642");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1643 - Simple test 1643
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1643, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1643");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1644 - Simple test 1644
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1644, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1644");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1645 - Simple test 1645
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1645, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1645");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1646 - Simple test 1646
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1646, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1646");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1647 - Simple test 1647
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1647, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1647");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1648 - Simple test 1648
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1648, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1648");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1649 - Simple test 1649
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1649, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1649");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1650 - Simple test 1650
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1650, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1650");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1651 - Simple test 1651
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1651, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1651");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1652 - Simple test 1652
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1652, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1652");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1653 - Simple test 1653
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1653, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1653");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1654 - Simple test 1654
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1654, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1654");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1655 - Simple test 1655
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1655, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1655");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1656 - Simple test 1656
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1656, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1656");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1657 - Simple test 1657
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1657, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1657");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1658 - Simple test 1658
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1658, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1658");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1659 - Simple test 1659
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1659, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1659");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1660 - Simple test 1660
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1660, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1660");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1661 - Simple test 1661
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1661, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1661");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1662 - Simple test 1662
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1662, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1662");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1663 - Simple test 1663
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1663, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1663");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1664 - Simple test 1664
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1664, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1664");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1665 - Simple test 1665
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1665, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1665");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1666 - Simple test 1666
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1666, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1666");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1667 - Simple test 1667
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1667, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1667");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1668 - Simple test 1668
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1668, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1668");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1669 - Simple test 1669
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1669, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1669");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1670 - Simple test 1670
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1670, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1670");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1671 - Simple test 1671
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1671, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1671");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1672 - Simple test 1672
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1672, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1672");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1673 - Simple test 1673
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1673, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1673");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1674 - Simple test 1674
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1674, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1674");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1675 - Simple test 1675
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1675, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1675");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1676 - Simple test 1676
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1676, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1676");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1677 - Simple test 1677
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1677, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1677");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1678 - Simple test 1678
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1678, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1678");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1679 - Simple test 1679
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1679, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1679");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1680 - Simple test 1680
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1680, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1680");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1681 - Simple test 1681
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1681, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1681");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1682 - Simple test 1682
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1682, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1682");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1683 - Simple test 1683
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1683, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1683");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1684 - Simple test 1684
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1684, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1684");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1685 - Simple test 1685
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1685, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1685");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1686 - Simple test 1686
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1686, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1686");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1687 - Simple test 1687
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1687, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1687");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1688 - Simple test 1688
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1688, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1688");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1689 - Simple test 1689
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1689, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1689");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1690 - Simple test 1690
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1690, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1690");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1691 - Simple test 1691
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1691, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1691");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1692 - Simple test 1692
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1692, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1692");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1693 - Simple test 1693
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1693, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1693");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1694 - Simple test 1694
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1694, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1694");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1695 - Simple test 1695
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1695, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1695");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1696 - Simple test 1696
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1696, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1696");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1697 - Simple test 1697
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1697, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1697");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1698 - Simple test 1698
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1698, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1698");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1699 - Simple test 1699
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1699, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1699");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1700 - Simple test 1700
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1700, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1700");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1701 - Simple test 1701
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1701, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1701");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1702 - Simple test 1702
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1702, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1702");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1703 - Simple test 1703
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1703, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1703");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1704 - Simple test 1704
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1704, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1704");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1705 - Simple test 1705
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1705, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1705");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1706 - Simple test 1706
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1706, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1706");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1707 - Simple test 1707
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1707, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1707");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1708 - Simple test 1708
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1708, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1708");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1709 - Simple test 1709
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1709, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1709");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1710 - Simple test 1710
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1710, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1710");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1711 - Simple test 1711
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1711, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1711");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1712 - Simple test 1712
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1712, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1712");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1713 - Simple test 1713
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1713, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1713");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1714 - Simple test 1714
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1714, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1714");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1715 - Simple test 1715
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1715, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1715");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1716 - Simple test 1716
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1716, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1716");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1717 - Simple test 1717
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1717, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1717");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1718 - Simple test 1718
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1718, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1718");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1719 - Simple test 1719
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1719, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1719");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1720 - Simple test 1720
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1720, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1720");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1721 - Simple test 1721
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1721, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1721");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1722 - Simple test 1722
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1722, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1722");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1723 - Simple test 1723
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1723, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1723");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1724 - Simple test 1724
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1724, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1724");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1725 - Simple test 1725
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1725, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1725");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1726 - Simple test 1726
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1726, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1726");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1727 - Simple test 1727
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1727, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1727");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1728 - Simple test 1728
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1728, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1728");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1729 - Simple test 1729
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1729, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1729");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1730 - Simple test 1730
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1730, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1730");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1731 - Simple test 1731
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1731, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1731");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1732 - Simple test 1732
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1732, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1732");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1733 - Simple test 1733
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1733, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1733");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1734 - Simple test 1734
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1734, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1734");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1735 - Simple test 1735
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1735, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1735");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1736 - Simple test 1736
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1736, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1736");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1737 - Simple test 1737
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1737, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1737");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1738 - Simple test 1738
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1738, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1738");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1739 - Simple test 1739
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1739, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1739");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1740 - Simple test 1740
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1740, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1740");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1741 - Simple test 1741
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1741, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1741");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1742 - Simple test 1742
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1742, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1742");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1743 - Simple test 1743
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1743, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1743");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1744 - Simple test 1744
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1744, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1744");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1745 - Simple test 1745
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1745, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1745");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1746 - Simple test 1746
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1746, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1746");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1747 - Simple test 1747
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1747, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1747");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1748 - Simple test 1748
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1748, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1748");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1749 - Simple test 1749
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1749, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1749");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1750 - Simple test 1750
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1750, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1750");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1751 - Simple test 1751
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1751, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1751");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1752 - Simple test 1752
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1752, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1752");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1753 - Simple test 1753
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1753, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1753");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1754 - Simple test 1754
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1754, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1754");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1755 - Simple test 1755
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1755, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1755");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1756 - Simple test 1756
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1756, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1756");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1757 - Simple test 1757
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1757, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1757");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1758 - Simple test 1758
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1758, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1758");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1759 - Simple test 1759
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1759, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1759");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1760 - Simple test 1760
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1760, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1760");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1761 - Simple test 1761
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1761, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1761");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1762 - Simple test 1762
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1762, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1762");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1763 - Simple test 1763
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1763, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1763");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1764 - Simple test 1764
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1764, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1764");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1765 - Simple test 1765
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1765, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1765");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1766 - Simple test 1766
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1766, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1766");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1767 - Simple test 1767
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1767, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1767");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1768 - Simple test 1768
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1768, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1768");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1769 - Simple test 1769
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1769, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1769");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1770 - Simple test 1770
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1770, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1770");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1771 - Simple test 1771
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1771, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1771");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1772 - Simple test 1772
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1772, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1772");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1773 - Simple test 1773
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1773, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1773");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1774 - Simple test 1774
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1774, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1774");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1775 - Simple test 1775
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1775, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1775");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1776 - Simple test 1776
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1776, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1776");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1777 - Simple test 1777
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1777, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1777");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1778 - Simple test 1778
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1778, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1778");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1779 - Simple test 1779
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1779, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1779");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1780 - Simple test 1780
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1780, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1780");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1781 - Simple test 1781
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1781, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1781");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1782 - Simple test 1782
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1782, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1782");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1783 - Simple test 1783
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1783, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1783");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1784 - Simple test 1784
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1784, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1784");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1785 - Simple test 1785
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1785, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1785");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1786 - Simple test 1786
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1786, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1786");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1787 - Simple test 1787
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1787, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1787");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1788 - Simple test 1788
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1788, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1788");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1789 - Simple test 1789
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1789, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1789");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1790 - Simple test 1790
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1790, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1790");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1791 - Simple test 1791
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1791, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1791");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1792 - Simple test 1792
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1792, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1792");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1793 - Simple test 1793
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1793, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1793");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1794 - Simple test 1794
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1794, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1794");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1795 - Simple test 1795
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1795, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1795");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1796 - Simple test 1796
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1796, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1796");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1797 - Simple test 1797
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1797, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1797");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1798 - Simple test 1798
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1798, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1798");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1799 - Simple test 1799
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1799, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1799");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1800 - Simple test 1800
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1800, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1800");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1801 - Simple test 1801
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1801, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1801");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1802 - Simple test 1802
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1802, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1802");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1803 - Simple test 1803
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1803, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1803");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1804 - Simple test 1804
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1804, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1804");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1805 - Simple test 1805
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1805, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1805");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1806 - Simple test 1806
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1806, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1806");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1807 - Simple test 1807
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1807, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1807");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1808 - Simple test 1808
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1808, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1808");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1809 - Simple test 1809
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1809, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1809");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1810 - Simple test 1810
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1810, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1810");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1811 - Simple test 1811
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1811, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1811");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1812 - Simple test 1812
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1812, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1812");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1813 - Simple test 1813
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1813, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1813");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1814 - Simple test 1814
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1814, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1814");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1815 - Simple test 1815
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1815, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1815");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1816 - Simple test 1816
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1816, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1816");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1817 - Simple test 1817
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1817, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1817");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1818 - Simple test 1818
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1818, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1818");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1819 - Simple test 1819
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1819, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1819");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1820 - Simple test 1820
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1820, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1820");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1821 - Simple test 1821
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1821, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1821");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1822 - Simple test 1822
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1822, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1822");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1823 - Simple test 1823
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1823, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1823");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1824 - Simple test 1824
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1824, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1824");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1825 - Simple test 1825
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1825, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1825");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1826 - Simple test 1826
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1826, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1826");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1827 - Simple test 1827
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1827, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1827");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1828 - Simple test 1828
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1828, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1828");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1829 - Simple test 1829
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1829, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1829");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1830 - Simple test 1830
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1830, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1830");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1831 - Simple test 1831
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1831, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1831");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1832 - Simple test 1832
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1832, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1832");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1833 - Simple test 1833
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1833, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1833");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1834 - Simple test 1834
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1834, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1834");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1835 - Simple test 1835
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1835, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1835");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1836 - Simple test 1836
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1836, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1836");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1837 - Simple test 1837
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1837, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1837");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1838 - Simple test 1838
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1838, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1838");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1839 - Simple test 1839
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1839, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1839");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1840 - Simple test 1840
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1840, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1840");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1841 - Simple test 1841
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1841, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1841");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1842 - Simple test 1842
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1842, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1842");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1843 - Simple test 1843
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1843, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1843");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1844 - Simple test 1844
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1844, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1844");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1845 - Simple test 1845
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1845, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1845");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1846 - Simple test 1846
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1846, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1846");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1847 - Simple test 1847
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1847, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1847");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1848 - Simple test 1848
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1848, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1848");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1849 - Simple test 1849
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1849, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1849");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1850 - Simple test 1850
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1850, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1850");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1851 - Simple test 1851
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1851, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1851");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1852 - Simple test 1852
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1852, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1852");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1853 - Simple test 1853
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1853, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1853");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1854 - Simple test 1854
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1854, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1854");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1855 - Simple test 1855
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1855, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1855");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1856 - Simple test 1856
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1856, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1856");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1857 - Simple test 1857
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1857, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1857");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1858 - Simple test 1858
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1858, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1858");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1859 - Simple test 1859
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1859, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1859");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1860 - Simple test 1860
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1860, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1860");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1861 - Simple test 1861
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1861, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1861");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1862 - Simple test 1862
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1862, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1862");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1863 - Simple test 1863
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1863, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1863");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1864 - Simple test 1864
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1864, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1864");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1865 - Simple test 1865
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1865, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1865");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1866 - Simple test 1866
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1866, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1866");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1867 - Simple test 1867
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1867, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1867");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1868 - Simple test 1868
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1868, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1868");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1869 - Simple test 1869
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1869, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1869");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1870 - Simple test 1870
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1870, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1870");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1871 - Simple test 1871
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1871, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1871");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1872 - Simple test 1872
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1872, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1872");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1873 - Simple test 1873
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1873, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1873");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1874 - Simple test 1874
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1874, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1874");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1875 - Simple test 1875
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1875, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1875");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1876 - Simple test 1876
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1876, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1876");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1877 - Simple test 1877
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1877, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1877");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1878 - Simple test 1878
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1878, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1878");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1879 - Simple test 1879
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1879, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1879");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1880 - Simple test 1880
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1880, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1880");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1881 - Simple test 1881
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1881, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1881");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1882 - Simple test 1882
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1882, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1882");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1883 - Simple test 1883
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1883, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1883");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1884 - Simple test 1884
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1884, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1884");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1885 - Simple test 1885
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1885, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1885");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1886 - Simple test 1886
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1886, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1886");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1887 - Simple test 1887
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1887, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1887");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1888 - Simple test 1888
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1888, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1888");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1889 - Simple test 1889
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1889, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1889");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1890 - Simple test 1890
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1890, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1890");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1891 - Simple test 1891
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1891, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1891");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1892 - Simple test 1892
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1892, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1892");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1893 - Simple test 1893
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1893, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1893");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1894 - Simple test 1894
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1894, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1894");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1895 - Simple test 1895
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1895, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1895");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1896 - Simple test 1896
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1896, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1896");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1897 - Simple test 1897
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1897, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1897");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1898 - Simple test 1898
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1898, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1898");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1899 - Simple test 1899
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1899, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1899");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1900 - Simple test 1900
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1900, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1900");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1901 - Simple test 1901
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1901, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1901");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1902 - Simple test 1902
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1902, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1902");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1903 - Simple test 1903
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1903, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1903");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1904 - Simple test 1904
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1904, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1904");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1905 - Simple test 1905
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1905, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1905");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1906 - Simple test 1906
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1906, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1906");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1907 - Simple test 1907
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1907, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1907");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1908 - Simple test 1908
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1908, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1908");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1909 - Simple test 1909
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1909, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1909");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1910 - Simple test 1910
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1910, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1910");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1911 - Simple test 1911
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1911, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1911");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1912 - Simple test 1912
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1912, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1912");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1913 - Simple test 1913
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1913, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1913");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1914 - Simple test 1914
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1914, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1914");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1915 - Simple test 1915
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1915, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1915");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1916 - Simple test 1916
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1916, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1916");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1917 - Simple test 1917
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1917, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1917");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1918 - Simple test 1918
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1918, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1918");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1919 - Simple test 1919
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1919, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1919");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1920 - Simple test 1920
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1920, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1920");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1921 - Simple test 1921
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1921, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1921");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1922 - Simple test 1922
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1922, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1922");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1923 - Simple test 1923
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1923, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1923");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1924 - Simple test 1924
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1924, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1924");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1925 - Simple test 1925
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1925, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1925");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1926 - Simple test 1926
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1926, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1926");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1927 - Simple test 1927
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1927, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1927");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1928 - Simple test 1928
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1928, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1928");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1929 - Simple test 1929
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1929, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1929");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1930 - Simple test 1930
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1930, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1930");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1931 - Simple test 1931
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1931, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1931");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1932 - Simple test 1932
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1932, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1932");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1933 - Simple test 1933
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1933, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1933");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1934 - Simple test 1934
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1934, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1934");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1935 - Simple test 1935
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1935, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1935");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1936 - Simple test 1936
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1936, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1936");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1937 - Simple test 1937
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1937, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1937");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1938 - Simple test 1938
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1938, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1938");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1939 - Simple test 1939
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1939, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1939");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1940 - Simple test 1940
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1940, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1940");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1941 - Simple test 1941
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1941, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1941");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1942 - Simple test 1942
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1942, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1942");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1943 - Simple test 1943
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1943, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1943");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1944 - Simple test 1944
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1944, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1944");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1945 - Simple test 1945
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1945, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1945");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1946 - Simple test 1946
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1946, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1946");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1947 - Simple test 1947
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1947, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1947");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1948 - Simple test 1948
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1948, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1948");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1949 - Simple test 1949
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1949, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1949");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1950 - Simple test 1950
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1950, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1950");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1951 - Simple test 1951
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1951, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1951");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1952 - Simple test 1952
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1952, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1952");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1953 - Simple test 1953
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1953, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1953");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1954 - Simple test 1954
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1954, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1954");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1955 - Simple test 1955
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1955, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1955");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1956 - Simple test 1956
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1956, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1956");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1957 - Simple test 1957
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1957, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1957");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1958 - Simple test 1958
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1958, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1958");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1959 - Simple test 1959
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1959, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1959");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1960 - Simple test 1960
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1960, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1960");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1961 - Simple test 1961
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1961, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1961");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1962 - Simple test 1962
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1962, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1962");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1963 - Simple test 1963
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1963, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1963");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1964 - Simple test 1964
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1964, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1964");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1965 - Simple test 1965
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1965, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1965");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1966 - Simple test 1966
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1966, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1966");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1967 - Simple test 1967
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1967, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1967");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1968 - Simple test 1968
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1968, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1968");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1969 - Simple test 1969
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1969, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1969");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1970 - Simple test 1970
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1970, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1970");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1971 - Simple test 1971
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1971, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1971");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1972 - Simple test 1972
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1972, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1972");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1973 - Simple test 1973
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1973, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1973");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1974 - Simple test 1974
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1974, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1974");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1975 - Simple test 1975
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1975, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1975");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1976 - Simple test 1976
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1976, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1976");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1977 - Simple test 1977
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1977, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1977");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1978 - Simple test 1978
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1978, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1978");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1979 - Simple test 1979
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1979, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1979");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1980 - Simple test 1980
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1980, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1980");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1981 - Simple test 1981
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1981, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1981");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1982 - Simple test 1982
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1982, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1982");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1983 - Simple test 1983
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1983, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1983");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1984 - Simple test 1984
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1984, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1984");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1985 - Simple test 1985
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1985, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1985");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1986 - Simple test 1986
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1986, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1986");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1987 - Simple test 1987
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1987, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1987");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1988 - Simple test 1988
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1988, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1988");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1989 - Simple test 1989
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1989, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1989");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1990 - Simple test 1990
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1990, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1990");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1991 - Simple test 1991
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1991, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1991");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1992 - Simple test 1992
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1992, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1992");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1993 - Simple test 1993
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1993, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1993");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1994 - Simple test 1994
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1994, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1994");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1995 - Simple test 1995
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1995, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1995");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1996 - Simple test 1996
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1996, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1996");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1997 - Simple test 1997
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1997, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1997");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1998 - Simple test 1998
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1998, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1998");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest1999 - Simple test 1999
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest1999, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t1999");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}
/* @tc.name: ConsoleTest2000 - Simple test 2000
 * @tc.desc: Simple test.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest2000, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value msg = StrToNapiValue(env, "t2000");
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: ConsoleTest2001 - Test console log with special characters
 * @tc.desc: Test console log with special characters in message.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest2001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    std::string message = "Special chars: \n\t\r\\\"\'";
    napi_value msg = StrToNapiValue(env, message);
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: ConsoleTest2002 - Test console info with empty string
 * @tc.desc: Test console info with empty string message.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest2002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    std::string message = "";
    napi_value msg = StrToNapiValue(env, message);
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "info", 4, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: ConsoleTest2003 - Test console warn with warning message
 * @tc.desc: Test console warn with warning level message.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest2003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    std::string message = "This is a warning message";
    napi_value msg = StrToNapiValue(env, message);
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "warn", 4, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::WARN>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: ConsoleTest2004 - Test console error with error message
 * @tc.desc: Test console error with error level message.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest2004, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    std::string message = "This is an error message";
    napi_value msg = StrToNapiValue(env, message);
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "error", 5, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::ERROR>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: ConsoleTest2005 - Test console debug with debug message
 * @tc.desc: Test console debug with debug level message.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest2005, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    std::string message = "Debug information";
    napi_value msg = StrToNapiValue(env, message);
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "debug", 5, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::DEBUG>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: ConsoleTest2006 - Test console log with multiple arguments
 * @tc.desc: Test console log with multiple string and number arguments.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest2006, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    size_t argc = 3;
    napi_value msg1 = StrToNapiValue(env, "Message:");
    napi_value msg2 = StrToNapiValue(env, "Value=");
    napi_value msg3 = nullptr;
    napi_create_uint32(env, 42, &msg3);
    napi_value argv[] = {msg1, msg2, msg3};
    napi_value res = nullptr;
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: ConsoleTest2007 - Test console count with default label
 * @tc.desc: Test console count without label parameter.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest2007, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value res = nullptr;
    napi_create_function(env, "count", 5, ConsoleTest::Count, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 0, nullptr, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: ConsoleTest2008 - Test console count with custom label
 * @tc.desc: Test console count with custom label string.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest2008, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    std::string label = "custom-counter";
    napi_value msg = StrToNapiValue(env, label);
    napi_value argv[] = {msg};
    napi_value res = nullptr;
    napi_create_function(env, "count", 5, ConsoleTest::Count, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: ConsoleTest2009 - Test console count with multiple calls
 * @tc.desc: Test console count called multiple times with same label.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest2009, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    std::string label = "multi-counter";
    napi_value msg = StrToNapiValue(env, label);
    napi_value argv[] = {msg};
    napi_value res = nullptr;

    napi_create_function(env, "count", 5, ConsoleTest::Count, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    napi_call_function(env, nullptr, cb, 1, argv, &res);

    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: ConsoleTest2010 - Test console countReset with existing label
 * @tc.desc: Test console countReset to reset an existing counter.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest2010, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    std::string label = "reset-counter";
    napi_value msg = StrToNapiValue(env, label);
    napi_value argv[] = {msg};
    napi_value res = nullptr;

    // Create counter first
    napi_create_function(env, "count", 5, ConsoleTest::Count, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    napi_call_function(env, nullptr, cb, 1, argv, &res);

    // Reset counter
    napi_create_function(env, "countReset", 9, ConsoleTest::CountReset, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);

    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: ConsoleTest2011 - Test console dir with object
 * @tc.desc: Test console dir to display object properties.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest2011, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value obj = nullptr;
    napi_create_object(env, &obj);

    napi_value value1 = StrToNapiValue(env, "test");
    napi_set_named_property(env, obj, "name", value1);

    napi_value argv[] = {obj};
    napi_value res = nullptr;
    napi_create_function(env, "dir", 3, ConsoleTest::Dir, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: ConsoleTest2012 - Test console group and groupEnd
 * @tc.desc: Test console group and groupEnd for nested logging.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest2012, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;

    // Group
    napi_value groupMsg = StrToNapiValue(env, "Test Group");
    napi_value groupArgv[] = {groupMsg};
    napi_value res = nullptr;
    napi_create_function(env, "group", 5, ConsoleTest::Group, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, groupArgv, &res);

    // Log in group
    napi_value logMsg = StrToNapiValue(env, "Nested message");
    napi_value logArgv[] = {logMsg};
    napi_create_function(env, "log", 3, ConsoleTest::ConsoleLog<OHOS::JsSysModule::LogLevel::INFO>, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, logArgv, &res);

    // Group end
    napi_create_function(env, "groupEnd", 8, ConsoleTest::GroupEnd, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 0, nullptr, &res);

    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: ConsoleTest2013 - Test console table with object
 * @tc.desc: Test console table to display tabular data.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest2013, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    napi_value obj = nullptr;
    napi_create_object(env, &obj);

    napi_value value1 = nullptr;
    napi_create_uint32(env, 1, &value1);
    napi_set_named_property(env, obj, "id", value1);

    napi_value value2 = StrToNapiValue(env, "Item1");
    napi_set_named_property(env, obj, "name", value2);

    napi_value argv[] = {obj};
    napi_value res = nullptr;
    napi_create_function(env, "table", 5, ConsoleTest::Table, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);
    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}

/* @tc.name: ConsoleTest2014 - Test console time and timeEnd
 * @tc.desc: Test console time and timeEnd to measure execution time.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConsoleTest2014, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value cb = nullptr;
    std::string label = "timer-2014";
    napi_value msg = StrToNapiValue(env, label);
    napi_value argv[] = {msg};
    napi_value res = nullptr;

    // Start timer
    napi_create_function(env, "time", 4, ConsoleTest::Time, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);

    // End timer
    napi_create_function(env, "timeEnd", 7, ConsoleTest::TimeEnd, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &res);

    ASSERT_CHECK_VALUE_TYPE(env, res, napi_undefined);
}