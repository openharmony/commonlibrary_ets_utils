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

#include "test.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "utils/log.h"
#include "js_url.h"

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

napi_status DealNapiStrValue(napi_env env, const napi_value napiStr, std::string &result)
{
    std::string buffer = "";
    size_t bufferSize = 0;
    napi_status status = napi_ok;
    status = napi_get_value_string_utf8(env, napiStr, nullptr, -1, &bufferSize);
    buffer.reserve(bufferSize + 1);
    buffer.resize(bufferSize);
    if (status != napi_ok) {
        HILOG_ERROR("can not get buffer size");
        return status;
    }
    if (bufferSize > 0) {
        status = napi_get_value_string_utf8(env, napiStr, buffer.data(), bufferSize + 1, &bufferSize);
        if (status != napi_ok) {
            HILOG_ERROR("can not get buffer value");
            return status;
        }
    }
    if (buffer.data() != nullptr) {
        result = buffer;
    }
    return status;
}

napi_value StrToNapiValue(napi_env env, std::string result)
{
    napi_value output = nullptr;
    napi_create_string_utf8(env, result.c_str(), result.size(), &output);
    return output;
}


HWTEST_F(NativeEngineTest, testUrlProtocol001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:8080/directory/file?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetScheme(env), output);
    ASSERT_STREQ(output.c_str(), "http:");
}

HWTEST_F(NativeEngineTest, testUrlProtocol002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("ftp://username:password@host:8080/directory/file?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetScheme(env), output);
    ASSERT_STREQ(output.c_str(), "ftp:");
}

HWTEST_F(NativeEngineTest, testUrlProtocol003, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("https://username:password@host:8080/directory/file?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetScheme(env), output);
    ASSERT_STREQ(output.c_str(), "https:");
}

HWTEST_F(NativeEngineTest, testUrlUsername001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:8080/directory/file?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetUsername(env), output);
    ASSERT_STREQ(output.c_str(), "username");
}

HWTEST_F(NativeEngineTest, testUrlUsername002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://zhao:password@host:8080/directory/file?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetUsername(env), output);
    ASSERT_STREQ(output.c_str(), "zhao");
}

HWTEST_F(NativeEngineTest, testUrlUsername003, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://skk:password@host:8080/directory/file?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetUsername(env), output);
    ASSERT_STREQ(output.c_str(), "skk");
}

HWTEST_F(NativeEngineTest, testUrlPassword001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:11@host:8080/directory/file?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetPassword(env), output);
    ASSERT_STREQ(output.c_str(), "11");
}

HWTEST_F(NativeEngineTest, testUrlPassword002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:23aa@host:8080/directory/file?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetPassword(env), output);
    ASSERT_STREQ(output.c_str(), "23aa");
}

HWTEST_F(NativeEngineTest, testUrlPassword003, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username@host:8080/directory/file?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetPassword(env), output);
    ASSERT_STREQ(output.c_str(), "");
}

HWTEST_F(NativeEngineTest, testUrlHost001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:8080/directory/file?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetHost(env), output);
    ASSERT_STREQ(output.c_str(), "host:8080");
}

HWTEST_F(NativeEngineTest, testUrlHost002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@hosthost/directory/file?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetHost(env), output);
    ASSERT_STREQ(output.c_str(), "hosthost");
}

HWTEST_F(NativeEngineTest, testUrlHost003, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:199/directory/file?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetHost(env), output);
    ASSERT_STREQ(output.c_str(), "host:199");
}

HWTEST_F(NativeEngineTest, testUrlHostname001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:8080/directory/file?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetHostname(env), output);
    ASSERT_STREQ(output.c_str(), "host");
}

HWTEST_F(NativeEngineTest, testUrlHostname002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host123:8080/directory/file?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetHostname(env), output);
    ASSERT_STREQ(output.c_str(), "host123");
}

HWTEST_F(NativeEngineTest, testUrlHostname003, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@885ssa:8080/directory/file?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetHostname(env), output);
    ASSERT_STREQ(output.c_str(), "885ssa");
}

HWTEST_F(NativeEngineTest, testUrlPort001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:8080/directory/file?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetPort(env), output);
    ASSERT_STREQ(output.c_str(), "8080");
}

HWTEST_F(NativeEngineTest, testUrlPort002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:100/directory/file?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetPort(env), output);
    ASSERT_STREQ(output.c_str(), "100");
}


HWTEST_F(NativeEngineTest, testUrlPort003, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host/directory/file?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetPort(env), output);
    ASSERT_STREQ(output.c_str(), "");
}

HWTEST_F(NativeEngineTest, testUrlPathname001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:8080/directory/file?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetPath(env), output);
    ASSERT_STREQ(output.c_str(), "/directory/file");
}

HWTEST_F(NativeEngineTest, testUrlPathname002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:8080/directory?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetPath(env), output);
    ASSERT_STREQ(output.c_str(), "/directory");
}

HWTEST_F(NativeEngineTest, testUrlPathname003, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:8080?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetPath(env), output);
    ASSERT_STREQ(output.c_str(), "/");
}

HWTEST_F(NativeEngineTest, testUrlSearch001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:8080/directory/file?query=qqqq#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetSearch(env), output);
    ASSERT_STREQ(output.c_str(), "?query=qqqq");
}

HWTEST_F(NativeEngineTest, testUrlSearch002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:8080/directory/file?query=qqqq&ll=pp#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetSearch(env), output);
    ASSERT_STREQ(output.c_str(), "?query=qqqq&ll=pp");
}

HWTEST_F(NativeEngineTest, testUrlSearch003, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:8080/directory/file?ppp9875=77#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetSearch(env), output);
    ASSERT_STREQ(output.c_str(), "?ppp9875=77");
}

HWTEST_F(NativeEngineTest, testUrlGetFragment001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:8080/directory/file?query=pppppp#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetFragment(env), output);
    ASSERT_STREQ(output.c_str(), "#fragment");
}

HWTEST_F(NativeEngineTest, testUrlGetFragment002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:8080/directory/file?query=pppppp#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetFragment(env), output);
    ASSERT_STREQ(output.c_str(), "#fragment");
}

HWTEST_F(NativeEngineTest, testUrlGetFragment003, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:8080/directory/file?query=pppppp#123456");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetFragment(env), output);
    ASSERT_STREQ(output.c_str(), "#123456");
}

HWTEST_F(NativeEngineTest, testUrlSearchParams001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:8080/directory/file?foo=1&bar=2");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetSearch(env), output);
    ASSERT_STREQ(output.c_str(), "?foo=1&bar=2");
}

HWTEST_F(NativeEngineTest, testUrlSearchParams002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("https://example.com?d=value1&c=value2&b=大&4=key4");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetSearch(env), output);
    ASSERT_STREQ(output.c_str(), "?d=value1&c=value2&b=大&4=key4");
}

HWTEST_F(NativeEngineTest, testUrlSearchParams003, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:8080/directory/file?你好=china#qwer=da");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetSearch(env), output);
    ASSERT_STREQ(output.c_str(), "?你好=china");
}

HWTEST_F(NativeEngineTest, testUrlAppend001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "ma");
    napi_value input2 = StrToNapiValue(env, "jk");
    params.Append(env, input1, input2);
    DealNapiStrValue(env, params.ToString(env), output);
    ASSERT_STREQ(output.c_str(), "ma=jk");
}

HWTEST_F(NativeEngineTest, testUrlAppend002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "ma 大");
    napi_value input2 = StrToNapiValue(env, "jk￥");
    params.Append(env, input1, input2);
    DealNapiStrValue(env, params.ToString(env), output);
    ASSERT_STREQ(output.c_str(), "ma+%E5%A4%A7=jk%EF%BF%A5");
}

HWTEST_F(NativeEngineTest, testUrlAppend003, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "foo~!@#$%^&*()_+-=");
    napi_value input2 = StrToNapiValue(env, "jk");
    params.Append(env, input1, input2);
    DealNapiStrValue(env, params.ToString(env), output);
    ASSERT_STREQ(output.c_str(), "foo%7E%21%40%23%24%25%5E%26*%28%29_%2B-%3D=jk");
}

HWTEST_F(NativeEngineTest, testUrlDelete001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "foo~!@#$%^&*()_+-=");
    napi_value input2 = StrToNapiValue(env, "jk");
    params.Append(env, input1, input2);
    params.Delete(env, input1);
    DealNapiStrValue(env, params.ToString(env), output);
    ASSERT_STREQ(output.c_str(), "");
}

HWTEST_F(NativeEngineTest, testUrlDelete002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "foo~!@#$%^&*()_+-=");
    napi_value input2 = StrToNapiValue(env, "jk");
    params.Append(env, input1, input2);
    napi_value input3 = StrToNapiValue(env, "ma");
    napi_value input4 = StrToNapiValue(env, "jk");
    params.Append(env, input3, input4);
    params.Delete(env, input1);
    DealNapiStrValue(env, params.ToString(env), output);
    ASSERT_STREQ(output.c_str(), "ma=jk");
}

HWTEST_F(NativeEngineTest, testUrlDelete003, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "foo~!@#$%^&*()_+-=");
    napi_value input2 = StrToNapiValue(env, "jk");
    params.Append(env, input1, input2);
    napi_value input3 = StrToNapiValue(env, "ma");
    napi_value input4 = StrToNapiValue(env, "jk");
    params.Append(env, input3, input4);
    params.Delete(env, input1);
    params.Delete(env, input3);
    DealNapiStrValue(env, params.ToString(env), output);
    ASSERT_STREQ(output.c_str(), "");
}

HWTEST_F(NativeEngineTest, testUrlGet001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "foo~!@#$%^&*()_+-=");
    napi_value input2 = StrToNapiValue(env, "jk");
    params.Append(env, input1, input2);
    DealNapiStrValue(env, params.Get(env, input1), output);
    ASSERT_STREQ(output.c_str(), "jk");
}

HWTEST_F(NativeEngineTest, testUrlGet002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "jk");
    DealNapiStrValue(env, params.Get(env, input1), output);
    ASSERT_STREQ(output.c_str(), "");
}

HWTEST_F(NativeEngineTest, testUrlGet003, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "foo~!@#$%^&*()_+-=");
    napi_value input2 = StrToNapiValue(env, "jk");
    params.Append(env, input1, input2);
    DealNapiStrValue(env, params.Get(env, input1), output);
    ASSERT_STREQ(output.c_str(), "jk");
}

HWTEST_F(NativeEngineTest, testUrlGetAll001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "foo~!@#$%^&*()_+-=");
    napi_value input2 = StrToNapiValue(env, "jk");
    params.Append(env, input1, input2);
    napi_value input3 = StrToNapiValue(env, "jk");
    DealNapiStrValue(env, params.GetAll(env, input3), output);
    ASSERT_STREQ(output.c_str(), "");
}

HWTEST_F(NativeEngineTest, testUrlGetAll002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "ma");
    napi_value input2 = StrToNapiValue(env, "jk");
    params.Append(env, input1, input2);
    napi_value input3 = StrToNapiValue(env, "jk1");
    DealNapiStrValue(env, params.GetAll(env, input3), output);
    ASSERT_STREQ(output.c_str(), "");
}

HWTEST_F(NativeEngineTest, testUrlGetAll003, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "ma");
    napi_value input2 = StrToNapiValue(env, "jk");
    params.Append(env, input1, input2);
    napi_value input3 = StrToNapiValue(env, "ma1");
    napi_value input4 = StrToNapiValue(env, "jk1");
    params.Append(env, input3, input4);
    napi_value input5 = StrToNapiValue(env, "jk2");
    DealNapiStrValue(env, params.GetAll(env, input5), output);
    ASSERT_STREQ(output.c_str(), "");
}

HWTEST_F(NativeEngineTest, testUrlHas001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "ma");
    DealNapiStrValue(env, params.IsHas(env, input1), output);
    ASSERT_STREQ(output.c_str(), "");
}

HWTEST_F(NativeEngineTest, testUrlHas002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "ma");
    napi_value input2 = StrToNapiValue(env, "jk");
    params.Append(env, input1, input2);
    napi_value input3 = StrToNapiValue(env, "foo~!@#$%^&*()_+-=");
    napi_value input4 = StrToNapiValue(env, "jk1");
    params.Append(env, input3, input4);
    DealNapiStrValue(env, params.IsHas(env, input1), output);
    ASSERT_STREQ(output.c_str(), "");
}

HWTEST_F(NativeEngineTest, testUrlHas003, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "ma");
    napi_value input2 = StrToNapiValue(env, "jk");
    params.Append(env, input1, input2);
    napi_value input3 = StrToNapiValue(env, "foo~!@#$%^&*()_+-=");
    napi_value input4 = StrToNapiValue(env, "jk1");
    params.Append(env, input3, input4);
    DealNapiStrValue(env, params.IsHas(env, input3), output);
    ASSERT_STREQ(output.c_str(), "");
}

HWTEST_F(NativeEngineTest, testUrlSet001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "ma");
    napi_value input2 = StrToNapiValue(env, "jk");
    params.Append(env, input1, input2);
    napi_value input3 = StrToNapiValue(env, "aa");
    params.Set(env, input1, input3);
    DealNapiStrValue(env, params.ToString(env), output);
    ASSERT_STREQ(output.c_str(), "ma=aa");
}

HWTEST_F(NativeEngineTest, testUrlSet002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "ma1");
    napi_value input2 = StrToNapiValue(env, "aa");
    params.Append(env, input1, input2);
    params.Set(env, input1, input2);
    DealNapiStrValue(env, params.ToString(env), output);
    ASSERT_STREQ(output.c_str(), "ma1=aa");
}

HWTEST_F(NativeEngineTest, testUrlSet003, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "ma");
    napi_value input2 = StrToNapiValue(env, "jk");
    params.Append(env, input1, input2);
    napi_value input3 = StrToNapiValue(env, "ma1");
    napi_value input4 = StrToNapiValue(env, "aa");
    params.Set(env, input3, input4);
    DealNapiStrValue(env, params.ToString(env), output);
    ASSERT_STREQ(output.c_str(), "ma=jk&ma1=aa");
}

HWTEST_F(NativeEngineTest, testUrlSort001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "ma");
    napi_value input2 = StrToNapiValue(env, "jk");
    params.Append(env, input1, input2);
    napi_value input3 = StrToNapiValue(env, "a");
    napi_value input4 = StrToNapiValue(env, "jk1");
    params.Append(env, input3, input4);
    params.Sort();
    DealNapiStrValue(env, params.ToString(env), output);
    ASSERT_STREQ(output.c_str(), "a=jk1&ma=jk");
}

HWTEST_F(NativeEngineTest, testUrlSort002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "ma");
    napi_value input2 = StrToNapiValue(env, "jk");
    params.Append(env, input1, input2);
    napi_value input3 = StrToNapiValue(env, "foo~!@#$%^&*()_+-=");
    napi_value input4 = StrToNapiValue(env, "jk1");
    params.Append(env, input3, input4);
    params.Sort();
    DealNapiStrValue(env, params.ToString(env), output);
    ASSERT_STREQ(output.c_str(), "foo%7E%21%40%23%24%25%5E%26*%28%29_%2B-%3D=jk1&ma=jk");
}

HWTEST_F(NativeEngineTest, testUrlSort003, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "ma");
    napi_value input2 = StrToNapiValue(env, "jk");
    params.Append(env, input1, input2);
    napi_value input3 = StrToNapiValue(env, "foo~!@#$%^&*()_+-=");
    napi_value input4 = StrToNapiValue(env, "jk1");
    params.Append(env, input3, input4);
    napi_value input5 = StrToNapiValue(env, "a");
    napi_value input6 = StrToNapiValue(env, "jk2");
    params.Append(env, input5, input6);
    params.Sort();
    DealNapiStrValue(env, params.ToString(env), output);
    ASSERT_STREQ(output.c_str(), "a=jk2&foo%7E%21%40%23%24%25%5E%26*%28%29_%2B-%3D=jk1&ma=jk");
}

HWTEST_F(NativeEngineTest, testUrlSearchParamsToString001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "1 12");
    napi_value input2 = StrToNapiValue(env, "jk");
    params.Append(env, input1, input2);
    DealNapiStrValue(env, params.ToString(env), output);
    ASSERT_STREQ(output.c_str(), "1+12=jk");
}

HWTEST_F(NativeEngineTest, testUrlSearchParamsToString002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "￥=)");
    napi_value input2 = StrToNapiValue(env, "jk");
    params.Append(env, input1, input2);
    DealNapiStrValue(env, params.ToString(env), output);
    ASSERT_STREQ(output.c_str(), "%EF%BF%A5%3D%29=jk");
}

HWTEST_F(NativeEngineTest, testUrlSearchParamsToString003, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "foo~!@#$%^&*()_+-=");
    napi_value input2 = StrToNapiValue(env, "jk");
    params.Append(env, input1, input2);
    DealNapiStrValue(env, params.ToString(env), output);
    ASSERT_STREQ(output.c_str(), "foo%7E%21%40%23%24%25%5E%26*%28%29_%2B-%3D=jk");
}
