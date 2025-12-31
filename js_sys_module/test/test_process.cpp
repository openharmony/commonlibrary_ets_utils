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

#include <ctime>
#include "test.h"

#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include "securec.h"
#include "js_childprocess.h"
#include "js_process.h"
#include "tools/log.h"

#define ASSERT_CHECK_CALL(call)   \
    {                             \
        ASSERT_EQ(call, napi_ok); \
    }

#define ASSERT_CHECK_VALUE_TYPE(env, value, type)               \
    {                                                           \
        napi_valuetype valueType = napi_undefined;              \
        ASSERT_TRUE(value != nullptr);                          \
        ASSERT_CHECK_CALL(napi_typeof(env, value, &valueType)); \
        ASSERT_EQ(valueType, type);                             \
    }
OHOS::JsSysModule::Process::ChildProcess* RunCommand(napi_env env, napi_value command, napi_value options)
{
    OHOS::JsSysModule::Process::ChildProcess* objectInfo = nullptr;
    objectInfo = new OHOS::JsSysModule::Process::ChildProcess();
    objectInfo->InitOptionsInfo(env, options);

    objectInfo->Spawn(env, command);
    return objectInfo;
}
static std::string testStr = "";
napi_value Method(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    size_t argc = 0;
    napi_value args[6] = { 0 }; // 6:six args
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, nullptr, &thisVar, nullptr));
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));

    napi_value name = args[0];
    napi_value value = args[1];

    std::string buffer1 = "";
    size_t bufferSize1 = 0;
    napi_get_value_string_utf8(env, name, nullptr, 0, &bufferSize1);
    buffer1.reserve(bufferSize1 + 1);
    buffer1.resize(bufferSize1);
    napi_get_value_string_utf8(env, name, buffer1.data(), bufferSize1 + 1, &bufferSize1);

    std::string buffer2 = "";
    size_t bufferSize2 = 0;
    napi_get_value_string_utf8(env, value, nullptr, 0, &bufferSize2);
    buffer2.reserve(bufferSize2 + 1);
    buffer2.resize(bufferSize2);
    napi_get_value_string_utf8(env, value, buffer2.data(), bufferSize2 + 1, &bufferSize2);
    testStr += buffer1 + buffer2;
    napi_value result = nullptr;
    napi_get_boolean(env, true, &result);
    return result;
}
/**
 * @tc.name: ProcessUptimeTest001
 * @tc.desc: Test process Uptime.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessUptimeTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;
    napi_value timeStart = process.Uptime(env);
    sleep(1);
    napi_value timeEnd = process.Uptime(env);
    double start = 0;
    double end = 0;
    napi_get_value_double(env, timeStart, &start);
    napi_get_value_double(env, timeEnd, &end);
    ASSERT_EQ(end - start, 1);
}

/**
 * @tc.name: ProcessUptimeTest002
 * @tc.desc: Test process Uptime.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessUptimeTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;
    napi_value timeStart = process.Uptime(env);
    sleep(2);
    napi_value timeEnd = process.Uptime(env);
    double start = 0;
    double end = 0;
    napi_get_value_double(env, timeStart, &start);
    napi_get_value_double(env, timeEnd, &end);
    ASSERT_EQ(end - start, 2);
}

/**
 * @tc.name: ProcessUptimeTest003
 * @tc.desc: Test process Uptime.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessUptimeTest003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;
    napi_value timeStart = process.Uptime(env);
    sleep(3);
    napi_value timeEnd = process.Uptime(env);
    double start = 0;
    double end = 0;
    napi_get_value_double(env, timeStart, &start);
    napi_get_value_double(env, timeEnd, &end);
    ASSERT_EQ(end - start, 3);
}

/**
 * @tc.name: ProcessUptimeTest004
 * @tc.desc: Test process Uptime.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessUptimeTest004, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;
    napi_value timeStart = process.Uptime(env);
    sleep(4);
    napi_value timeEnd = process.Uptime(env);
    double start = 0;
    double end = 0;
    napi_get_value_double(env, timeStart, &start);
    napi_get_value_double(env, timeEnd, &end);
    ASSERT_EQ(end - start, 4);
}

/**
 * @tc.name: ProcessUptimeTest005
 * @tc.desc: Test process Uptime.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessUptimeTest005, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;
    napi_value timeStart = process.Uptime(env);
    sleep(5);
    napi_value timeEnd = process.Uptime(env);
    double start = 0;
    double end = 0;
    napi_get_value_double(env, timeStart, &start);
    napi_get_value_double(env, timeEnd, &end);
    ASSERT_EQ(end - start, 5);
}

/**
 * @tc.name: ProcessUptimeTest006
 * @tc.desc: Test process Uptime.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessUptimeTest006, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;
    napi_value timeStart = process.Uptime(env);
    sleep(6);
    napi_value timeEnd = process.Uptime(env);
    double start = 0;
    double end = 0;
    napi_get_value_double(env, timeStart, &start);
    napi_get_value_double(env, timeEnd, &end);
    ASSERT_EQ(end - start, 6);
}

/**
 * @tc.name: ProcessUptimeTest007
 * @tc.desc: Test process Uptime.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessUptimeTest007, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;
    napi_value timeStart = process.Uptime(env);
    sleep(7);
    napi_value timeEnd = process.Uptime(env);
    double start = 0;
    double end = 0;
    napi_get_value_double(env, timeStart, &start);
    napi_get_value_double(env, timeEnd, &end);
    ASSERT_EQ(end - start, 7);
}

/**
 * @tc.name: ProcessUptimeTest008
 * @tc.desc: Test process Uptime.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessUptimeTest008, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;
    napi_value timeStart = process.Uptime(env);
    sleep(8);
    napi_value timeEnd = process.Uptime(env);
    double start = 0;
    double end = 0;
    napi_get_value_double(env, timeStart, &start);
    napi_get_value_double(env, timeEnd, &end);
    ASSERT_EQ(end - start, 8);
}

/**
 * @tc.name: ProcessUptimeTest009
 * @tc.desc: Test process Uptime.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessUptimeTest009, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;
    napi_value timeStart = process.Uptime(env);
    sleep(9);
    napi_value timeEnd = process.Uptime(env);
    double start = 0;
    double end = 0;
    napi_get_value_double(env, timeStart, &start);
    napi_get_value_double(env, timeEnd, &end);
    ASSERT_EQ(end - start, 9);
}

/**
 * @tc.name: ProcessUptimeTest010
 * @tc.desc: Test process Uptime.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessUptimeTest010, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;
    napi_value timeStart = process.Uptime(env);
    sleep(10);
    napi_value timeEnd = process.Uptime(env);
    double start = 0;
    double end = 0;
    napi_get_value_double(env, timeStart, &start);
    napi_get_value_double(env, timeEnd, &end);
    ASSERT_EQ(end - start, 10);
}

/**
 * @tc.name: ProcessUptimeTest011
 * @tc.desc: Test process Uptime.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessUptimeTest011, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;
    napi_value timeStart = process.Uptime(env);
    sleep(11);
    napi_value timeEnd = process.Uptime(env);
    double start = 0;
    double end = 0;
    napi_get_value_double(env, timeStart, &start);
    napi_get_value_double(env, timeEnd, &end);
    ASSERT_EQ(end - start, 11);
}

/**
 * @tc.name: ProcessKillTest001
 * @tc.desc: Test process kill signal.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessKillTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 1");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);

    napi_value pid = childprocess->Getpid(env);
    childprocess->Close();
    napi_value signal = nullptr;
    napi_create_int32(env, 9, &signal);
    napi_value result = process.Kill(env, pid, signal);
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: ProcessKillTest002
 * @tc.desc: Test process kill signal.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessKillTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 1");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);

    napi_value pid = childprocess->Getpid(env);
    childprocess->Close();
    napi_value signal = nullptr;
    napi_create_int32(env, 999, &signal);
    napi_value result = process.Kill(env, pid, signal);
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: ProcessRunCmdTest001
 * @tc.desc: Test process RunCmd fork process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessRunCmdTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("each abc");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);

    napi_value output = childprocess->GetOutput(env);
    childprocess->Close();
    bool res = false;
    napi_is_promise(env, output, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessRunCmdTest002
 * @tc.desc: Test process RunCmd fork process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessRunCmdTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("mkdir test.txt");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);

    napi_value errorOutput = childprocess->GetErrorOutput(env);
    childprocess->Close();
    bool res = false;
    napi_is_promise(env, errorOutput, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessRunCmdTest003
 * @tc.desc: Test process RunCmd fork process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessRunCmdTest003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("mkdir 3.txt");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);

    napi_value errorOutput = childprocess->GetErrorOutput(env);
    childprocess->Close();
    bool res = false;
    napi_is_promise(env, errorOutput, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessRunCmdTest004
 * @tc.desc: Test process RunCmd fork process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessRunCmdTest004, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("mkdir 4.txt");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);

    napi_value errorOutput = childprocess->GetErrorOutput(env);
    childprocess->Close();
    bool res = false;
    napi_is_promise(env, errorOutput, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessRunCmdTest005
 * @tc.desc: Test process RunCmd fork process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessRunCmdTest005, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("mkdir 5.txt");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);

    napi_value errorOutput = childprocess->GetErrorOutput(env);
    childprocess->Close();
    bool res = false;
    napi_is_promise(env, errorOutput, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessRunCmdTest006
 * @tc.desc: Test process RunCmd fork process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessRunCmdTest006, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("mkdir 6.txt");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);

    napi_value errorOutput = childprocess->GetErrorOutput(env);
    childprocess->Close();
    bool res = false;
    napi_is_promise(env, errorOutput, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessRunCmdTest007
 * @tc.desc: Test process RunCmd fork process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessRunCmdTest007, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("mkdir 7.txt");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);

    napi_value errorOutput = childprocess->GetErrorOutput(env);
    childprocess->Close();
    bool res = false;
    napi_is_promise(env, errorOutput, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessRunCmdTest008
 * @tc.desc: Test process RunCmd fork process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessRunCmdTest008, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("mkdir 8.txt");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);

    napi_value errorOutput = childprocess->GetErrorOutput(env);
    childprocess->Close();
    bool res = false;
    napi_is_promise(env, errorOutput, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessRunCmdTest009
 * @tc.desc: Test process RunCmd fork process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessRunCmdTest009, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("mkdir 9.txt");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);

    napi_value errorOutput = childprocess->GetErrorOutput(env);
    childprocess->Close();
    bool res = false;
    napi_is_promise(env, errorOutput, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessRunCmdTest010
 * @tc.desc: Test process RunCmd fork process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessRunCmdTest010, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("mkdir 10.txt");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);

    napi_value errorOutput = childprocess->GetErrorOutput(env);
    childprocess->Close();
    bool res = false;
    napi_is_promise(env, errorOutput, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessRunCmdTest011
 * @tc.desc: Test process RunCmd fork process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessRunCmdTest011, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("mkdir 11.txt");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);

    napi_value errorOutput = childprocess->GetErrorOutput(env);
    childprocess->Close();
    bool res = false;
    napi_is_promise(env, errorOutput, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessRunCmdTest012
 * @tc.desc: Test process RunCmd fork process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessRunCmdTest012, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("mkdir 12.txt");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);

    napi_value errorOutput = childprocess->GetErrorOutput(env);
    childprocess->Close();
    bool res = false;
    napi_is_promise(env, errorOutput, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessRunCmdTest013
 * @tc.desc: Test process RunCmd fork process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessRunCmdTest013, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("mkdir 13.txt");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);

    napi_value errorOutput = childprocess->GetErrorOutput(env);
    childprocess->Close();
    bool res = false;
    napi_is_promise(env, errorOutput, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessRunCmdTest014
 * @tc.desc: Test process RunCmd fork process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessRunCmdTest014, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("mkdir 14.txt");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);

    napi_value errorOutput = childprocess->GetErrorOutput(env);
    childprocess->Close();
    bool res = false;
    napi_is_promise(env, errorOutput, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessGetUidTest001
 * @tc.desc: Test process uid.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessGetUidTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;
    napi_value napiUid = process.GetUid(env);
    int32_t uid = 0;
    napi_get_value_int32(env, napiUid, &uid);
    bool res = false;
    if (uid >= 0) {
        res = true;
    }
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessGetGidTest001
 * @tc.desc: Test process gid.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessGetGidTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;
    napi_value napiGid = process.GetGid(env);
    int32_t gid = 0;
    napi_get_value_int32(env, napiGid, &gid);
    bool res = false;
    if (gid >= 0) {
        res = true;
    }
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessGetEUidTest001
 * @tc.desc: Test process euid.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessGetEUidTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;
    napi_value napiEuid = process.GetEUid(env);
    int32_t euid = 0;
    napi_get_value_int32(env, napiEuid, &euid);
    bool res = false;
    if (euid >= 0) {
        res = true;
    }
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessGetEGidTest001
 * @tc.desc: Test process egid.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessGetEGidTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;
    napi_value napiEgid = process.GetEGid(env);
    int32_t egid = 0;
    napi_get_value_int32(env, napiEgid, &egid);
    bool res = false;
    if (egid >= 0) {
        res = true;
    }
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessGetPidTest001
 * @tc.desc: Test process pid.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessGetPidTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;
    napi_value napiPid = process.GetPid(env);
    int32_t pid = 0;
    napi_get_value_int32(env, napiPid, &pid);
    bool res = false;
    if (pid > 0) {
        res = true;
    }
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessGetPidTest001
 * @tc.desc: Test process ppid.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessGetPpidTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;
    napi_value napiPpid = process.GetPpid(env);
    int32_t ppid = 0;
    napi_get_value_int32(env, napiPpid, &ppid);
    bool res = false;
    if (ppid > 0) {
        res = true;
    }
    ASSERT_TRUE(res);
}

/**
 * @tc.name:childProcessPpidTest001
 * @tc.desc: test get the parent process ID.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, childProcessPpidTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 1s;");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    napi_value result = childprocess->Getppid(env);
    childprocess->Close();
    int32_t ppid = 0;
    napi_get_value_int32(env, result, &ppid);
    bool res = false;
    if (ppid >= 0) {
        res = true;
    }
    ASSERT_TRUE(res);
}

/**
 * @tc.name:childProcesspidTest001
 * @tc.desc: test get the specific pid value.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, childProcesspidTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 1s;");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    napi_value result = childprocess->Getpid(env);
    childprocess->Close();
    int32_t pid = 0;
    napi_get_value_int32(env, result, &pid);
    bool res = false;
    if (pid >= 0) {
        res = true;
    }
    ASSERT_TRUE(res);
}

/**
 * @tc.name:childProcesspidTest002
 * @tc.desc: test get the specific pid value.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, childProcesspidTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 2s;");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    napi_value result = childprocess->Getpid(env);
    childprocess->Close();
    int32_t pid = 0;
    napi_get_value_int32(env, result, &pid);
    bool res = false;
    if (pid >= 0) {
        res = true;
    }
    ASSERT_TRUE(res);
}

/**
 * @tc.name:childProcesspidTest003
 * @tc.desc: test get the specific pid value.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, childProcesspidTest003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 3s;");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    napi_value result = childprocess->Getpid(env);
    childprocess->Close();
    int32_t pid = 0;
    napi_get_value_int32(env, result, &pid);
    bool res = false;
    if (pid >= 0) {
        res = true;
    }
    ASSERT_TRUE(res);
}

/**
 * @tc.name:childProcesspidTest004
 * @tc.desc: test get the specific pid value.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, childProcesspidTest004, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 4s;");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    napi_value result = childprocess->Getpid(env);
    childprocess->Close();
    int32_t pid = 0;
    napi_get_value_int32(env, result, &pid);
    bool res = false;
    if (pid >= 0) {
        res = true;
    }
    ASSERT_TRUE(res);
}

/**
 * @tc.name:childProcesspidTest005
 * @tc.desc: test get the specific pid value.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, childProcesspidTest005, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 5s;");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    napi_value result = childprocess->Getpid(env);
    childprocess->Close();
    int32_t pid = 0;
    napi_get_value_int32(env, result, &pid);
    bool res = false;
    if (pid >= 0) {
        res = true;
    }
    ASSERT_TRUE(res);
}

/**
 * @tc.name:childProcesspidTest006
 * @tc.desc: test get the specific pid value.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, childProcesspidTest006, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 6s;");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    napi_value result = childprocess->Getpid(env);
    childprocess->Close();
    int32_t pid = 0;
    napi_get_value_int32(env, result, &pid);
    bool res = false;
    if (pid >= 0) {
        res = true;
    }
    ASSERT_TRUE(res);
}

/**
 * @tc.name:childProcesspidTest007
 * @tc.desc: test get the specific pid value.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, childProcesspidTest007, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 7s;");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    napi_value result = childprocess->Getpid(env);
    childprocess->Close();
    int32_t pid = 0;
    napi_get_value_int32(env, result, &pid);
    bool res = false;
    if (pid >= 0) {
        res = true;
    }
    ASSERT_TRUE(res);
}

/**
 * @tc.name:childProcesspidTest008
 * @tc.desc: test get the specific pid value.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, childProcesspidTest008, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 8s;");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    napi_value result = childprocess->Getpid(env);
    childprocess->Close();
    int32_t pid = 0;
    napi_get_value_int32(env, result, &pid);
    bool res = false;
    if (pid >= 0) {
        res = true;
    }
    ASSERT_TRUE(res);
}

/**
 * @tc.name:childProcesspidTest009
 * @tc.desc: test get the specific pid value.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, childProcesspidTest009, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 9s;");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    napi_value result = childprocess->Getpid(env);
    childprocess->Close();
    int32_t pid = 0;
    napi_get_value_int32(env, result, &pid);
    bool res = false;
    if (pid >= 0) {
        res = true;
    }
    ASSERT_TRUE(res);
}

/**
 * @tc.name:childProcesspidTest010
 * @tc.desc: test get the specific pid value.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, childProcesspidTest010, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 10s;");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    napi_value result = childprocess->Getpid(env);
    childprocess->Close();
    int32_t pid = 0;
    napi_get_value_int32(env, result, &pid);
    bool res = false;
    if (pid >= 0) {
        res = true;
    }
    ASSERT_TRUE(res);
}

/**
 * @tc.name:childProcesspidTest011
 * @tc.desc: test get the specific pid value.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, childProcesspidTest011, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 11s;");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    napi_value result = childprocess->Getpid(env);
    childprocess->Close();
    int32_t pid = 0;
    napi_get_value_int32(env, result, &pid);
    bool res = false;
    if (pid >= 0) {
        res = true;
    }
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessGetGroupsTest001
 * @tc.desc: Test process groups.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessGetGroupsTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;
    napi_value element = nullptr;
    napi_value groups = process.GetGroups(env);
    napi_get_element(env, groups, 1, &element);
    int32_t indexOne = 0;
    napi_get_value_int32(env, element, &indexOne);
    bool res = false;
    if (indexOne >= 0) {
        res = true;
    }
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessChdirTest001
 * @tc.desc: Test process gid.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessChdirTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;
    std::string catalogue = "/system/lib";
    napi_value temp = nullptr;
    napi_create_string_utf8(env, catalogue.c_str(), catalogue.length(), &temp);
    process.Chdir(env, temp);
    napi_value cwd = process.Cwd(env);
    size_t bufferSize = 0;
    if (napi_get_value_string_utf8(env, cwd, nullptr, 0, &bufferSize) != napi_ok) {
        HILOG_ERROR("can not get str size");
    }
    std::string result = "";
    result.reserve(bufferSize + 1);
    result.resize(bufferSize);
    if (napi_get_value_string_utf8(env, cwd, result.data(), bufferSize + 1, &bufferSize) != napi_ok) {
        HILOG_ERROR("can not get str value");
    }
    std::string tag = "";
    tag = result;
    bool res = false;
    if (tag == catalogue) {
        res = true;
    }
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessOn001
 * @tc.desc: Test process gid.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessOn001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;
    napi_value temp = nullptr;
    std::string cbNameEvent = "add";
    napi_create_string_utf8(env, cbNameEvent.c_str(), cbNameEvent.length(), &temp);
    std::string cbName = "cbMethod";
    napi_value cb = nullptr;
    napi_create_function(env, cbName.c_str(), cbName.size(), Method, nullptr, &cb);
    process.On(env, temp, cb);
    napi_value convertResult = nullptr;
    convertResult = process.Off(env, temp);
    bool res = false;
    napi_get_value_bool(env, convertResult, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: ProcessOn002
 * @tc.desc: Test process gid.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessOn002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;
    napi_value temp = nullptr;
    std::string cbNameEvent = "UnHandleRejection";
    napi_create_string_utf8(env, cbNameEvent.c_str(), cbNameEvent.length(), &temp);
    std::string cbName = "cbMethod";
    napi_value cb = nullptr;
    napi_create_function(env, cbName.c_str(), cbName.size(), Method, nullptr, &cb);
    process.On(env, temp, cb);
    napi_value convertResult = nullptr;
    convertResult = process.Off(env, temp);
    bool res = false;
    napi_get_value_bool(env, convertResult, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessGetTid001
 * @tc.desc: Test process gid.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessGetTid001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;
    napi_value napiTid = process.GetTid(env);
    int32_t tid = 0;
    napi_get_value_int32(env, napiTid, &tid);
    bool res = false;
    if (tid != 0) {
        res = true;
    }
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessIsIsolatedProcess001
 * @tc.desc: test whether the process is isolated.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessIsolatedProcess001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;
    napi_value result = process.IsIsolatedProcess(env);
    bool res = false;
    napi_get_value_bool(env, result, &res);
    if (res) {
        ASSERT_TRUE(res);
    } else {
        ASSERT_FALSE(res);
    }
}

/**
 * @tc.name: ProcessIsAppUid001
 * @tc.desc: test whether the process is AppUid.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessIsAppUid001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;
    napi_value uid = nullptr;
    napi_create_int32(env, 9, &uid);
    napi_value result = process.IsAppUid(env, uid);
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: ProcessIs64Bit001
 * @tc.desc: test the operating environment is 64-bit.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessIs64Bit001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;
    napi_value result = process.Is64Bit(env);
    bool res = false;
    napi_get_value_bool(env, result, &res);
    if (res) {
        ASSERT_TRUE(res);
    } else {
        ASSERT_FALSE(res);
    }
}

/**
 * @tc.name: ProcessGetEnvironmentVar001
 * @tc.desc: test get the value corresponding to the environment variable.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessGetEnvironmentVar001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;
    napi_value temp = nullptr;
    std::string envVar = "PATH";
    napi_create_string_utf8(env, envVar.c_str(), envVar.length(), &temp);
    napi_value result = process.GetEnvironmentVar(env, temp);
    napi_valuetype valuetype;
    napi_typeof(env, result, &valuetype);
    bool res = false;
    if (valuetype == napi_string) {
        res = true;
    }
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcesGetUidForName001
 * @tc.desc: test Get process uid by process name.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcesGetUidForName001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;
    napi_value temp = nullptr;
    std::string user = "root";
    napi_create_string_utf8(env, user.c_str(), user.length(), &temp);
    napi_value result = process.GetUidForName(env, temp);
    int32_t num = 0;
    napi_get_value_int32(env, result, &num);
    bool res = false;
    if (num >= 0) {
        res = true;
    }
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcesGetUidForName002
 * @tc.desc: test Get process uid by process name.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcesGetUidForName002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;
    napi_value temp = nullptr;
    std::string user = "1234";
    napi_create_string_utf8(env, user.c_str(), user.length(), &temp);
    napi_value result = process.GetUidForName(env, temp);
    int32_t num = 0;
    napi_get_value_int32(env, result, &num);
    bool res = false;
    if (num == -1) {
        res = true;
    }
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcesGetThreadPriority001
 * @tc.desc: test Get thread priority based on specified tid.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcesGetThreadPriority001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;
    napi_value napiTid = process.GetTid(env);
    napi_value result = process.GetThreadPriority(env, napiTid);
    napi_valuetype valuetype;
    napi_typeof(env, result, &valuetype);
    bool res = false;
    if (valuetype == napi_number) {
        res = true;
    }
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcesGetGetStartRealtime001
 * @tc.desc: test Get the real-time elapsed time from system startup to process startup.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcesGetGetStartRealtime001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;
    napi_value result = process.GetStartRealtime(env);
    double num = 0;
    napi_get_value_double(env, result, &num);
    bool res = false;
    if (num == 0) {
        res = true;
    }
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcesGetPastCputime001
 * @tc.desc: test Get the CPU time from the process startup to the current time.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcesGetPastCputime001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;
    napi_value result = process.GetPastCputime(env);
    int32_t num = 0;
    napi_get_value_int32(env, result, &num);
    bool res = false;
    if (num != 0) {
        res = true;
    }
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcesGetSystemConfig001
 * @tc.desc: test Get system configuration information.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcesGetSystemConfig001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;
    napi_value config = nullptr;
    napi_create_int32(env, _SC_NPROCESSORS_CONF, &config);
    napi_value result = process.GetSystemConfig(env, config);
    int32_t num = 0;
    napi_get_value_int32(env, result, &num);
    bool res = false;
    if (num != 0) {
        res = true;
    }
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessCloseTest001
 * @tc.desc: Close the target process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessCloseTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 1");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    childprocess->Close();
    napi_value exitCode = childprocess->GetExitCode(env);
    int32_t num = 0;
    napi_value result = nullptr;
    napi_get_value_int32(env, exitCode, &num);
    bool res = false;
    if (num == 0) {
        res = true;
    }
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessCloseTest002
 * @tc.desc: Close the target process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessCloseTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 2");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    childprocess->Close();
    napi_value exitCode = childprocess->GetExitCode(env);
    int32_t num = 0;
    napi_value result = nullptr;
    napi_get_value_int32(env, exitCode, &num);
    bool res = false;
    if (num == 0) {
        res = true;
    }
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessCloseTest003
 * @tc.desc: Close the target process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessCloseTest003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 3");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    childprocess->Close();
    napi_value exitCode = childprocess->GetExitCode(env);
    int32_t num = 0;
    napi_value result = nullptr;
    napi_get_value_int32(env, exitCode, &num);
    bool res = false;
    if (num == 0) {
        res = true;
    }
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessCloseTest004
 * @tc.desc: Close the target process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessCloseTest004, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 4");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    childprocess->Close();
    napi_value exitCode = childprocess->GetExitCode(env);
    int32_t num = 0;
    napi_value result = nullptr;
    napi_get_value_int32(env, exitCode, &num);
    bool res = false;
    if (num == 0) {
        res = true;
    }
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessCloseTest005
 * @tc.desc: Close the target process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessCloseTest005, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 5");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    childprocess->Close();
    napi_value exitCode = childprocess->GetExitCode(env);
    int32_t num = 0;
    napi_value result = nullptr;
    napi_get_value_int32(env, exitCode, &num);
    bool res = false;
    if (num == 0) {
        res = true;
    }
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessCloseTest006
 * @tc.desc: Close the target process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessCloseTest006, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 6");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    childprocess->Close();
    napi_value exitCode = childprocess->GetExitCode(env);
    int32_t num = 0;
    napi_value result = nullptr;
    napi_get_value_int32(env, exitCode, &num);
    bool res = false;
    if (num == 0) {
        res = true;
    }
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessCloseTest007
 * @tc.desc: Close the target process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessCloseTest007, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 7");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    childprocess->Close();
    napi_value exitCode = childprocess->GetExitCode(env);
    int32_t num = 0;
    napi_value result = nullptr;
    napi_get_value_int32(env, exitCode, &num);
    bool res = false;
    if (num == 0) {
        res = true;
    }
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessCloseTest008
 * @tc.desc: Close the target process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessCloseTest008, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 8");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    childprocess->Close();
    napi_value exitCode = childprocess->GetExitCode(env);
    int32_t num = 0;
    napi_value result = nullptr;
    napi_get_value_int32(env, exitCode, &num);
    bool res = false;
    if (num == 0) {
        res = true;
    }
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessCloseTest009
 * @tc.desc: Close the target process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessCloseTest009, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 9");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    childprocess->Close();
    napi_value exitCode = childprocess->GetExitCode(env);
    int32_t num = 0;
    napi_value result = nullptr;
    napi_get_value_int32(env, exitCode, &num);
    bool res = false;
    if (num == 0) {
        res = true;
    }
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessCloseTest010
 * @tc.desc: Close the target process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessCloseTest010, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 10");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    childprocess->Close();
    napi_value exitCode = childprocess->GetExitCode(env);
    int32_t num = 0;
    napi_value result = nullptr;
    napi_get_value_int32(env, exitCode, &num);
    bool res = false;
    if (num == 0) {
        res = true;
    }
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessCloseTest011
 * @tc.desc: Close the target process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessCloseTest011, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 11");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    childprocess->Close();
    napi_value exitCode = childprocess->GetExitCode(env);
    int32_t num = 0;
    napi_value result = nullptr;
    napi_get_value_int32(env, exitCode, &num);
    bool res = false;
    if (num == 0) {
        res = true;
    }
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessCloseTest012
 * @tc.desc: Close the target process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessCloseTest012, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 12");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    childprocess->Close();
    napi_value exitCode = childprocess->GetExitCode(env);
    int32_t num = 0;
    napi_value result = nullptr;
    napi_get_value_int32(env, exitCode, &num);
    bool res = false;
    if (num == 0) {
        res = true;
    }
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessCloseTest013
 * @tc.desc: Close the target process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessCloseTest013, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 13");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    childprocess->Close();
    napi_value exitCode = childprocess->GetExitCode(env);
    int32_t num = 0;
    napi_value result = nullptr;
    napi_get_value_int32(env, exitCode, &num);
    bool res = false;
    if (num == 0) {
        res = true;
    }
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessCloseTest014
 * @tc.desc: Close the target process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessCloseTest014, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 14");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    childprocess->Close();
    napi_value exitCode = childprocess->GetExitCode(env);
    int32_t num = 0;
    napi_value result = nullptr;
    napi_get_value_int32(env, exitCode, &num);
    bool res = false;
    if (num == 0) {
        res = true;
    }
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessCloseTest015
 * @tc.desc: Close the target process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessCloseTest015, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 15");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    childprocess->Close();
    napi_value exitCode = childprocess->GetExitCode(env);
    int32_t num = 0;
    napi_value result = nullptr;
    napi_get_value_int32(env, exitCode, &num);
    bool res = false;
    if (num == 0) {
        res = true;
    }
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessCloseTest016
 * @tc.desc: Close the target process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessCloseTest016, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 16");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    childprocess->Close();
    napi_value exitCode = childprocess->GetExitCode(env);
    int32_t num = 0;
    napi_value result = nullptr;
    napi_get_value_int32(env, exitCode, &num);
    bool res = false;
    if (num == 0) {
        res = true;
    }
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessCloseTest017
 * @tc.desc: Close the target process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessCloseTest017, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 17");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    childprocess->Close();
    napi_value exitCode = childprocess->GetExitCode(env);
    int32_t num = 0;
    napi_value result = nullptr;
    napi_get_value_int32(env, exitCode, &num);
    bool res = false;
    if (num == 0) {
        res = true;
    }
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessCloseTest018
 * @tc.desc: Close the target process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessCloseTest018, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 18");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    childprocess->Close();
    napi_value exitCode = childprocess->GetExitCode(env);
    int32_t num = 0;
    napi_value result = nullptr;
    napi_get_value_int32(env, exitCode, &num);
    bool res = false;
    if (num == 0) {
        res = true;
    }
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessCloseTest019
 * @tc.desc: Close the target process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessCloseTest019, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 19");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    childprocess->Close();
    napi_value exitCode = childprocess->GetExitCode(env);
    int32_t num = 0;
    napi_value result = nullptr;
    napi_get_value_int32(env, exitCode, &num);
    bool res = false;
    if (num == 0) {
        res = true;
    }
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessCloseTest020
 * @tc.desc: Close the target process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessCloseTest020, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 20");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    childprocess->Close();
    napi_value exitCode = childprocess->GetExitCode(env);
    int32_t num = 0;
    napi_value result = nullptr;
    napi_get_value_int32(env, exitCode, &num);
    bool res = false;
    if (num == 0) {
        res = true;
    }
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessCloseTest021
 * @tc.desc: Close the target process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessCloseTest021, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 21");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    childprocess->Close();
    napi_value exitCode = childprocess->GetExitCode(env);
    int32_t num = 0;
    napi_value result = nullptr;
    napi_get_value_int32(env, exitCode, &num);
    bool res = false;
    if (num == 0) {
        res = true;
    }
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessCloseTest022
 * @tc.desc: Close the target process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessCloseTest022, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 22");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    childprocess->Close();
    napi_value exitCode = childprocess->GetExitCode(env);
    int32_t num = 0;
    napi_value result = nullptr;
    napi_get_value_int32(env, exitCode, &num);
    bool res = false;
    if (num == 0) {
        res = true;
    }
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessCloseTest023
 * @tc.desc: Close the target process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessCloseTest023, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 23");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    childprocess->Close();
    napi_value exitCode = childprocess->GetExitCode(env);
    int32_t num = 0;
    napi_value result = nullptr;
    napi_get_value_int32(env, exitCode, &num);
    bool res = false;
    if (num == 0) {
        res = true;
    }
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessCloseTest024
 * @tc.desc: Close the target process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessCloseTest024, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 24");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    childprocess->Close();
    napi_value exitCode = childprocess->GetExitCode(env);
    int32_t num = 0;
    napi_value result = nullptr;
    napi_get_value_int32(env, exitCode, &num);
    bool res = false;
    if (num == 0) {
        res = true;
    }
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessCloseTest025
 * @tc.desc: Close the target process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessCloseTest025, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 25");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    childprocess->Close();
    napi_value exitCode = childprocess->GetExitCode(env);
    int32_t num = 0;
    napi_value result = nullptr;
    napi_get_value_int32(env, exitCode, &num);
    bool res = false;
    if (num == 0) {
        res = true;
    }
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessCloseTest026
 * @tc.desc: Close the target process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessCloseTest026, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 26");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    childprocess->Close();
    napi_value exitCode = childprocess->GetExitCode(env);
    int32_t num = 0;
    napi_value result = nullptr;
    napi_get_value_int32(env, exitCode, &num);
    bool res = false;
    if (num == 0) {
        res = true;
    }
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessCloseTest027
 * @tc.desc: Close the target process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessCloseTest027, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 27");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    childprocess->Close();
    napi_value exitCode = childprocess->GetExitCode(env);
    int32_t num = 0;
    napi_value result = nullptr;
    napi_get_value_int32(env, exitCode, &num);
    bool res = false;
    if (num == 0) {
        res = true;
    }
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessCloseTest028
 * @tc.desc: Close the target process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessCloseTest028, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 28");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    childprocess->Close();
    napi_value exitCode = childprocess->GetExitCode(env);
    int32_t num = 0;
    napi_value result = nullptr;
    napi_get_value_int32(env, exitCode, &num);
    bool res = false;
    if (num == 0) {
        res = true;
    }
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name:childProcessKillTest001
 * @tc.desc: Send a signal to process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, childProcessKillTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 1s;");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    napi_value signo = nullptr;
    napi_create_int32(env, 9, &signo);
    childprocess->Kill(env, signo);
    napi_value result = childprocess->GetKilled(env);
    childprocess->Close();
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name:childProcessKillTest002
 * @tc.desc: Send a signal to process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, childProcessKillTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 2s;");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    napi_value signo = nullptr;
    napi_create_int32(env, 9, &signo);
    childprocess->Kill(env, signo);
    napi_value result = childprocess->GetKilled(env);
    childprocess->Close();
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name:childProcessKillTest003
 * @tc.desc: Send a signal to process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, childProcessKillTest003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 3s;");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    napi_value signo = nullptr;
    napi_create_int32(env, 9, &signo);
    childprocess->Kill(env, signo);
    napi_value result = childprocess->GetKilled(env);
    childprocess->Close();
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name:childProcessKillTest004
 * @tc.desc: Send a signal to process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, childProcessKillTest004, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 4s;");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    napi_value signo = nullptr;
    napi_create_int32(env, 9, &signo);
    childprocess->Kill(env, signo);
    napi_value result = childprocess->GetKilled(env);
    childprocess->Close();
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name:childProcessKillTest005
 * @tc.desc: Send a signal to process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, childProcessKillTest005, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 5s;");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    napi_value signo = nullptr;
    napi_create_int32(env, 9, &signo);
    childprocess->Kill(env, signo);
    napi_value result = childprocess->GetKilled(env);
    childprocess->Close();
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name:childProcessKillTest006
 * @tc.desc: Send a signal to process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, childProcessKillTest006, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 6s;");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    napi_value signo = nullptr;
    napi_create_int32(env, 9, &signo);
    childprocess->Kill(env, signo);
    napi_value result = childprocess->GetKilled(env);
    childprocess->Close();
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name:childProcessKillTest007
 * @tc.desc: Send a signal to process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, childProcessKillTest007, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 7s;");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    napi_value signo = nullptr;
    napi_create_int32(env, 9, &signo);
    childprocess->Kill(env, signo);
    napi_value result = childprocess->GetKilled(env);
    childprocess->Close();
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name:childProcessKillTest008
 * @tc.desc: Send a signal to process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, childProcessKillTest008, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 8s;");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    napi_value signo = nullptr;
    napi_create_int32(env, 9, &signo);
    childprocess->Kill(env, signo);
    napi_value result = childprocess->GetKilled(env);
    childprocess->Close();
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name:childProcessKillTest009
 * @tc.desc: Send a signal to process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, childProcessKillTest009, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 9s;");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    napi_value signo = nullptr;
    napi_create_int32(env, 9, &signo);
    childprocess->Kill(env, signo);
    napi_value result = childprocess->GetKilled(env);
    childprocess->Close();
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name:childProcessKillTest010
 * @tc.desc: Send a signal to process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, childProcessKillTest010, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 10s;");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    napi_value signo = nullptr;
    napi_create_int32(env, 9, &signo);
    childprocess->Kill(env, signo);
    napi_value result = childprocess->GetKilled(env);
    childprocess->Close();
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name:childProcessKillTest011
 * @tc.desc: Send a signal to process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, childProcessKillTest011, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 11s;");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    napi_value signo = nullptr;
    napi_create_int32(env, 9, &signo);
    childprocess->Kill(env, signo);
    napi_value result = childprocess->GetKilled(env);
    childprocess->Close();
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name:childProcessKillTest012
 * @tc.desc: Send a signal to process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, childProcessKillTest012, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 12s;");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    napi_value signo = nullptr;
    napi_create_int32(env, 9, &signo);
    childprocess->Kill(env, signo);
    napi_value result = childprocess->GetKilled(env);
    childprocess->Close();
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name:childProcessKillTest013
 * @tc.desc: Send a signal to process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, childProcessKillTest013, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 13s;");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    napi_value signo = nullptr;
    napi_create_int32(env, 9, &signo);
    childprocess->Kill(env, signo);
    napi_value result = childprocess->GetKilled(env);
    childprocess->Close();
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name:childProcessKillTest014
 * @tc.desc: Send a signal to process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, childProcessKillTest014, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 14s;");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    napi_value signo = nullptr;
    napi_create_int32(env, 9, &signo);
    childprocess->Kill(env, signo);
    napi_value result = childprocess->GetKilled(env);
    childprocess->Close();
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name:childProcessKillTest015
 * @tc.desc: Send a signal to process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, childProcessKillTest015, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 15s;");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    napi_value signo = nullptr;
    napi_create_int32(env, 9, &signo);
    childprocess->Kill(env, signo);
    napi_value result = childprocess->GetKilled(env);
    childprocess->Close();
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name:childProcessKillTest016
 * @tc.desc: Send a signal to process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, childProcessKillTest016, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 16s;");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    napi_value signo = nullptr;
    napi_create_int32(env, 9, &signo);
    childprocess->Kill(env, signo);
    napi_value result = childprocess->GetKilled(env);
    childprocess->Close();
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name:childProcessKillTest017
 * @tc.desc: Send a signal to process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, childProcessKillTest017, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 17s;");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    napi_value signo = nullptr;
    napi_create_int32(env, 9, &signo);
    childprocess->Kill(env, signo);
    napi_value result = childprocess->GetKilled(env);
    childprocess->Close();
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name:childProcessKillTest018
 * @tc.desc: Send a signal to process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, childProcessKillTest018, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 18s;");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    napi_value signo = nullptr;
    napi_create_int32(env, 9, &signo);
    childprocess->Kill(env, signo);
    napi_value result = childprocess->GetKilled(env);
    childprocess->Close();
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name:childProcessKillTest019
 * @tc.desc: Send a signal to process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, childProcessKillTest019, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 19s;");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    napi_value signo = nullptr;
    napi_create_int32(env, 9, &signo);
    childprocess->Kill(env, signo);
    napi_value result = childprocess->GetKilled(env);
    childprocess->Close();
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name:childProcessKillTest020
 * @tc.desc: Send a signal to process.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, childProcessKillTest020, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::JsSysModule::Process::Process process;

    std::string command("ls; sleep 20s;");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::JsSysModule::Process::ChildProcess* childprocess = RunCommand(env, temp, nullptr);
    childprocess->Wait(env);
    napi_value signo = nullptr;
    napi_create_int32(env, 9, &signo);
    childprocess->Kill(env, signo);
    napi_value result = childprocess->GetKilled(env);
    childprocess->Close();
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}
