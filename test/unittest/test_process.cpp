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
    OHOS::Js_sys_module::Process::ChildProcess objectInfo(env);

    objectInfo.InitOptionsInfo(options);

    objectInfo.Spawn(command);

    return objectInfo;
}
/**
 * @tc.name: ProcessUptimeTest001
 * @tc.desc: Test process Uptime.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessUptimeTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::Js_sys_module::Process::Process process(env);
    napi_value timeStart = process.Uptime();
    sleep(1);
    napi_value timeEnd = process.Uptime();
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
    OHOS::Js_sys_module::Process::Process process(env);

    std::string command("ls; sleep 1");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::Js_sys_module::Process::ChildProcess childprocess = RunCommand(env, temp, nullptr);

    napi_value pid = childprocess.Getpid();
    napi_value signal = nullptr;
    napi_create_int32(env, 9, &signal);
    napi_value result = process.Kill(pid, signal);
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ProcessKillTest002
 * @tc.desc: Test process kill signal.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ProcessKillTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::Js_sys_module::Process::Process process(env);

    std::string command("ls; sleep 1");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::Js_sys_module::Process::ChildProcess childprocess = RunCommand(env, temp, nullptr);

    napi_value pid = childprocess.Getpid();
    napi_value signal = nullptr;
    napi_create_int32(env, 999, &signal);
    napi_value result = process.Kill(pid, signal);
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
    OHOS::Js_sys_module::Process::Process process(env);

    std::string command("each abc");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::Js_sys_module::Process::ChildProcess childprocess = RunCommand(env, temp, nullptr);

    napi_value output = childprocess.GetOutput();
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
    OHOS::Js_sys_module::Process::Process process(env);

    std::string command("mkdir test.txt");
    napi_value temp = nullptr;
    napi_create_string_utf8(env, command.c_str(), command.length(), &temp);

    OHOS::Js_sys_module::Process::ChildProcess childprocess = RunCommand(env, temp, nullptr);

    napi_value errorOutput = childprocess.GetErrorOutput();
    bool res = false;
    napi_is_promise(env, errorOutput, &res);
    ASSERT_TRUE(res);
}