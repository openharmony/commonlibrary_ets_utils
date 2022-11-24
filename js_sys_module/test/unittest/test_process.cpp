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
#include "utils/log.h"
#include "js_childprocess.h"
#include "js_process.h"

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
static OHOS::Js_sys_module::Process::ChildProcess RunCommand(napi_env env, napi_value command, napi_value options)
{
    OHOS::Js_sys_module::Process::ChildProcess objectInfo;

    objectInfo.InitOptionsInfo(env, options);

    objectInfo.Spawn(env, command);

    return objectInfo;
}
std::string testStr = "";
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
    OHOS::Js_sys_module::Process::Process process;
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
 * @tc.name: ProcessKillTest001
 * @tc.desc: Test process kill signal.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessKillTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::Js_sys_module::Process::Process process;

    std::string command("ls; sleep 1");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::Js_sys_module::Process::ChildProcess childprocess = RunCommand(env, temp, nullptr);

    napi_value pid = childprocess.Getpid(env);
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
    OHOS::Js_sys_module::Process::Process process;

    std::string command("ls; sleep 1");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::Js_sys_module::Process::ChildProcess childprocess = RunCommand(env, temp, nullptr);

    napi_value pid = childprocess.Getpid(env);
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
    OHOS::Js_sys_module::Process::Process process;

    std::string command("each abc");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::Js_sys_module::Process::ChildProcess childprocess = RunCommand(env, temp, nullptr);

    napi_value output = childprocess.GetOutput(env);
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
    OHOS::Js_sys_module::Process::Process process;

    std::string command("mkdir test.txt");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::Js_sys_module::Process::ChildProcess childprocess = RunCommand(env, temp, nullptr);

    napi_value errorOutput = childprocess.GetErrorOutput(env);
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
    OHOS::Js_sys_module::Process::Process process;
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
    OHOS::Js_sys_module::Process::Process process;
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
    OHOS::Js_sys_module::Process::Process process;
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
    OHOS::Js_sys_module::Process::Process process;
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
    OHOS::Js_sys_module::Process::Process process;
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
    OHOS::Js_sys_module::Process::Process process;
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
    OHOS::Js_sys_module::Process::Process process;

    std::string command("ls; sleep 1s;");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::Js_sys_module::Process::ChildProcess childprocess = RunCommand(env, temp, nullptr);
    napi_value result = childprocess.Getppid(env);
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
    OHOS::Js_sys_module::Process::Process process;

    std::string command("ls; sleep 1s;");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::Js_sys_module::Process::ChildProcess childprocess = RunCommand(env, temp, nullptr);
    napi_value result = childprocess.Getpid(env);
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
    OHOS::Js_sys_module::Process::Process process;
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
    OHOS::Js_sys_module::Process::Process process;
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
    OHOS::Js_sys_module::Process::Process process;
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
    OHOS::Js_sys_module::Process::Process process;
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
    OHOS::Js_sys_module::Process::Process process;
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
    OHOS::Js_sys_module::Process::Process process;
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
    OHOS::Js_sys_module::Process::Process process;
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
    OHOS::Js_sys_module::Process::Process process;
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
    OHOS::Js_sys_module::Process::Process process;
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
    OHOS::Js_sys_module::Process::Process process;
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
    OHOS::Js_sys_module::Process::Process process;
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
    OHOS::Js_sys_module::Process::Process process;
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
    OHOS::Js_sys_module::Process::Process process;
    napi_value result = process.GetStartRealtime(env);
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
 * @tc.name: ProcesGetPastCputime001
 * @tc.desc: test Get the CPU time from the process startup to the current time.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcesGetPastCputime001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::Js_sys_module::Process::Process process;
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
    OHOS::Js_sys_module::Process::Process process;
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
    OHOS::Js_sys_module::Process::Process process;

    std::string command("ls; sleep 1");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::Js_sys_module::Process::ChildProcess childprocess = RunCommand(env, temp, nullptr);
    childprocess.Wait(env);
    childprocess.Close();
    napi_value exitCode = childprocess.GetExitCode(env);
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
    OHOS::Js_sys_module::Process::Process process;

    std::string command("ls; sleep 1s;");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::Js_sys_module::Process::ChildProcess childprocess = RunCommand(env, temp, nullptr);
    childprocess.Wait(env);
    napi_value signo = nullptr;
    napi_create_int32(env, 9, &signo);
    childprocess.Kill(env, signo);
    napi_value result = childprocess.GetKilled(env);
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}