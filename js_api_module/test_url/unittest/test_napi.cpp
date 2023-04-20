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
#include <regex>
#include <sstream>
#include "securec.h"
#include "unicode/stringpiece.h"
#include "unicode/unistr.h"

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

napi_value StrToNapiValue(napi_env env, const std::string &result)
{
    napi_value output = nullptr;
    napi_create_string_utf8(env, result.c_str(), result.size(), &output);
    return output;
}

bool IsEscapeRange(const char ch)
{
    if ((ch > 0 && ch < '*') || (ch > '*' && ch < '-') || (ch == '/') ||
        (ch > '9' && ch < 'A') || (ch > 'Z' && ch < '_') || (ch == '`') || (ch > 'z')) {
        return true;
    }
    return false;
}

std::string ReviseStr(std::string &str, std::string *reviseChar)
{
    icu::StringPiece sp(str.c_str());
    icu::UnicodeString wstr = icu::UnicodeString::fromUTF8(sp);
    const size_t lenStr = static_cast<size_t>(wstr.length());
    if (lenStr == 0) {
        return "";
    }
    std::string output = "";
    size_t numOfAscii = 128; // 128:Number of ASCII characters
    size_t i = 0;
    for (; i < lenStr; i++) {
        auto charaEncode = static_cast<size_t>(wstr[i]);
        if (charaEncode < numOfAscii) {
            // 2:Defines the escape range of ASCII characters
            if (IsEscapeRange(charaEncode)) {
                output += reviseChar[charaEncode];
            } else {
                output += str.substr(i, 1);
            }
        } else if (charaEncode <= 0x000007FF) { // Convert the Unicode code into two bytes
            std::string output1 = reviseChar[0x000000C0 | (charaEncode / 64)]; // 64:the first byte
            std::string output2 = reviseChar[numOfAscii | (charaEncode & 0x0000003F)];
            output += output1 + output2;
        } else if ((charaEncode >= 0x0000E000) || (charaEncode <= 0x0000D7FF)) {
            std::string output1 = reviseChar[0x000000E0 | (charaEncode / 4096)]; // 4096:Acquisition method
            std::string output2 = reviseChar[numOfAscii | ((charaEncode / 64) & 0x0000003F)]; // 64:second byte
            std::string output3 = reviseChar[numOfAscii | (charaEncode & 0x0000003F)];
            output += output1 + output2 + output3;
        } else {
            const size_t charaEncode1 = static_cast<size_t>(str[++i]) & 1023; // 1023:Convert codes
            charaEncode = 65536 + (((charaEncode & 1023) << 10) | charaEncode1); // 65536:Specific transcoding
            std::string output1 = reviseChar[0x000000F0 | (charaEncode / 262144)]; // 262144:the first byte
            std::string output2 = reviseChar[numOfAscii | ((charaEncode / 4096) & 0x0000003F)]; // 4096:second byte
            std::string output3 = reviseChar[numOfAscii | ((charaEncode / 64) & 0x0000003F)]; // 64:third byte
            std::string output4 = reviseChar[numOfAscii | (charaEncode & 0x0000003F)];
            output += output1 + output2 + output3 + output4;
        }
    }
    return output;
}

napi_value ToString(napi_env env, std::vector<std::string> &searchParams)
{
    std::string output = "";
    std::string reviseChar[256] = {""}; // 256:Array length
    for (size_t i = 0; i < 256; ++i) { // 256:Array length
        size_t j = i;
        std::stringstream ioss;
        std::string str1 = "";
        ioss << std::hex << j;
        ioss >> str1;
        transform(str1.begin(), str1.end(), str1.begin(), ::toupper);
        if (i < 16) { // 16:Total number of 0-F
            reviseChar[i] = '%' + ("0" + str1);
        } else {
            reviseChar[i] = '%' + str1;
        }
    }
    reviseChar[0x20] = "+"; // 0x20:ASCII value of spaces
    const size_t lenStr = searchParams.size();
    napi_value result = nullptr;
    if (lenStr == 0) {
        napi_create_string_utf8(env, output.c_str(), output.size(), &result);
        return result;
    }
    std::string firstStrKey = ReviseStr(searchParams[0], reviseChar);
    std::string firstStrValue = ReviseStr(searchParams[1], reviseChar);
    output = firstStrKey + "=" + firstStrValue;
    if (lenStr % 2 == 0) { // 2:Divisible by 2
        size_t pos = 2; // 2:Initial Position
        for (; pos < lenStr; pos += 2) { // 2:Searching for the number and number of keys and values
            std::string strKey = ReviseStr(searchParams[pos], reviseChar);
            std::string strValue = ReviseStr(searchParams[pos + 1], reviseChar);
            output += +"&" + strKey + "=" + strValue;
        }
    }
    napi_create_string_utf8(env, output.c_str(), output.size(), &result);
    return result;
}

std::vector<std::string> GetParamsStrig(napi_env env, const napi_value tempStr)
{
    std::vector<std::string> vec;
    size_t arraySize = 0;
    uint32_t length = 0;
    napi_get_array_length(env, tempStr, &length);
    napi_value napiStr = nullptr;
    for (size_t i = 0; i < length; i++) {
        napi_get_element(env, tempStr, i, &napiStr);
        if (napi_get_value_string_utf8(env, napiStr, nullptr, 0, &arraySize) != napi_ok) {
            HILOG_ERROR("can not get napiStr size");
            return vec;
        }
        if (arraySize > 0) {
            std::string cstr = "";
            cstr.resize(arraySize);
            if (napi_get_value_string_utf8(env, napiStr, cstr.data(), arraySize + 1, &arraySize) != napi_ok) {
                HILOG_ERROR("can not get napiStr size");
                return vec;
            }
            vec.push_back(cstr);
        } else {
            vec.push_back("");
        }
    }
    return vec;
}

HWTEST_F(NativeEngineTest, testUrlConstructs001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("https://example.org:81/a/b/c?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetPath(env), output);
    ASSERT_STREQ(output.c_str(), "/a/b/c");
}

HWTEST_F(NativeEngineTest, testUrlConstructs002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:8080/directory/file?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetPath(env), output);
    ASSERT_STREQ(output.c_str(), "/directory/file");
}

HWTEST_F(NativeEngineTest, testUrlConstructs003, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("../baz", "http://example.org/foo/bar");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetPath(env), output);
    ASSERT_STREQ(output.c_str(), "/foo/baz");
}

HWTEST_F(NativeEngineTest, testUrlConstructs004, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("/../sca/./path/path/../scasa/jjjjj", "http://www.example.com");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetPath(env), output);
    ASSERT_STREQ(output.c_str(), "/sca/path/scasa/jjjjj");
}

HWTEST_F(NativeEngineTest, testUrlConstructs005, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:8080/directory/file?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetScheme(env), output);
    ASSERT_STREQ(output.c_str(), "http:");
}

HWTEST_F(NativeEngineTest, testUrlConstructs006, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL base("http://www.example.com");
    OHOS::Url::URL url("//sca/./path/./scasa#", base);
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetPath(env), output);
    ASSERT_STREQ(output.c_str(), "/path/scasa");
}

HWTEST_F(NativeEngineTest, testUrlConstructs007, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL base("http://www.example.com");
    OHOS::Url::URL url("/dire/query", base);
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetPath(env), output);
    ASSERT_STREQ(output.c_str(), "/dire/query");
}

HWTEST_F(NativeEngineTest, testUrlConstructs008, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL base("http://www.example.com");
    OHOS::Url::URL url("foo/bar//fragment", base);
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetPath(env), output);
    ASSERT_STREQ(output.c_str(), "/foo/bar//fragment");
}

HWTEST_F(NativeEngineTest, testUrlConstructs009, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL base("http://www.example.com");
    OHOS::Url::URL url("/../sca/./path/./s#casa", base);
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetPath(env), output);
    ASSERT_STREQ(output.c_str(), "/sca/path/s");
}

HWTEST_F(NativeEngineTest, testUrlConstructs010, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL base("http://www.example.com");
    OHOS::Url::URL url("/../sca/./path/./sca?sa", base);
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetPath(env), output);
    ASSERT_STREQ(output.c_str(), "/sca/path/sca");
}

HWTEST_F(NativeEngineTest, testUrlConstructs011, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL base("http://www.example.com");
    OHOS::Url::URL url("", base);
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetPath(env), output);
    ASSERT_STREQ(output.c_str(), "/");
}

HWTEST_F(NativeEngineTest, testUrlConstructs012, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("file://\\/www.example.com");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetScheme(env), output);
    ASSERT_STREQ(output.c_str(), "file:");
}

HWTEST_F(NativeEngineTest, testUrlConstructs013, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("file:///www.example.com");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetScheme(env), output);
    ASSERT_STREQ(output.c_str(), "file:");
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

HWTEST_F(NativeEngineTest, testUrlSetScheme001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("https://username:password@host:8080/directory/file?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetScheme(env), output);
    ASSERT_STREQ(output.c_str(), "https:");
    url.SetScheme("ftp:");
    DealNapiStrValue(env, url.GetScheme(env), output);
    ASSERT_STREQ(output.c_str(), "ftp:");
}

HWTEST_F(NativeEngineTest, testUrlSetScheme002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("https://username:password@host:8080/directory/file?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetScheme(env), output);
    ASSERT_STREQ(output.c_str(), "https:");
    url.SetScheme("666");
    DealNapiStrValue(env, url.GetScheme(env), output);
    ASSERT_STREQ(output.c_str(), "https:");
}

HWTEST_F(NativeEngineTest, testUrlSetScheme003, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("https://username:password@host:8080/directory/file?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetScheme(env), output);
    ASSERT_STREQ(output.c_str(), "https:");
    url.SetScheme("-/+");
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

HWTEST_F(NativeEngineTest, testUrlSetUsername001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:8080/directory/file?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetUsername(env), output);
    ASSERT_STREQ(output.c_str(), "username");
    url.SetUsername("666");
    DealNapiStrValue(env, url.GetUsername(env), output);
    ASSERT_STREQ(output.c_str(), "666");
}

HWTEST_F(NativeEngineTest, testUrlSetUsername002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:8080/directory/file?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetUsername(env), output);
    ASSERT_STREQ(output.c_str(), "username");
    url.SetUsername("");
    DealNapiStrValue(env, url.GetUsername(env), output);
    ASSERT_STREQ(output.c_str(), "");
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

HWTEST_F(NativeEngineTest, testUrlSetPassword001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:8080/directory/file?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetPassword(env), output);
    ASSERT_STREQ(output.c_str(), "password");
    url.SetPassword("666");
    DealNapiStrValue(env, url.GetPassword(env), output);
    ASSERT_STREQ(output.c_str(), "666");
}

HWTEST_F(NativeEngineTest, testUrlSetPassword002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:8080/directory/file?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetPassword(env), output);
    ASSERT_STREQ(output.c_str(), "password");
    url.SetPassword("");
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

HWTEST_F(NativeEngineTest, testUrlSetHost001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:199/directory/file?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetHost(env), output);
    ASSERT_STREQ(output.c_str(), "host:199");
    url.SetHost("0.0.0.0.0:199");
    DealNapiStrValue(env, url.GetHost(env), output);
    ASSERT_STREQ(output.c_str(), "0.0.0.0.0:199");
}

HWTEST_F(NativeEngineTest, testUrlSetHost002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:199/directory/file?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetHost(env), output);
    ASSERT_STREQ(output.c_str(), "host:199");
    url.SetHost("");
    DealNapiStrValue(env, url.GetHost(env), output);
    ASSERT_STREQ(output.c_str(), "host:199");
}

HWTEST_F(NativeEngineTest, testUrlSetHref001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:199/directory/file?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetHost(env), output);
    ASSERT_STREQ(output.c_str(), "host:199");
    url.SetHref("http:huawei.com");
    DealNapiStrValue(env, url.GetHost(env), output);
    ASSERT_STREQ(output.c_str(), "huawei.com");
}

HWTEST_F(NativeEngineTest, testUrlSetHref002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:199/directory/file?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetHost(env), output);
    ASSERT_STREQ(output.c_str(), "host:199");
    url.SetHref("   http:huawei.com");
    DealNapiStrValue(env, url.GetHost(env), output);
    ASSERT_STREQ(output.c_str(), "huawei.com");
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

HWTEST_F(NativeEngineTest, testUrlSetPort001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:100/directory/file?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetPort(env), output);
    ASSERT_STREQ(output.c_str(), "100");
    url.SetPort("600");
    DealNapiStrValue(env, url.GetPort(env), output);
    ASSERT_STREQ(output.c_str(), "600");
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

HWTEST_F(NativeEngineTest, testUrlSetPath001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:8080?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetPath(env), output);
    ASSERT_STREQ(output.c_str(), "/");
    url.SetPath("\\D:");
    DealNapiStrValue(env, url.GetPath(env), output);
    ASSERT_STREQ(output.c_str(), "/D:");
}

HWTEST_F(NativeEngineTest, testUrlSetPath002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:8080?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetPath(env), output);
    ASSERT_STREQ(output.c_str(), "/");
    url.SetPath(":\\D:");
    DealNapiStrValue(env, url.GetPath(env), output);
    ASSERT_STREQ(output.c_str(), "/:/D:");
}

HWTEST_F(NativeEngineTest, testUrlSetPath003, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:8080?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetPath(env), output);
    ASSERT_STREQ(output.c_str(), "/");
    url.SetPath("::");
    DealNapiStrValue(env, url.GetPath(env), output);
    ASSERT_STREQ(output.c_str(), "/::");
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

HWTEST_F(NativeEngineTest, testUrlSetSearch001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:8080/directory/file?ppp9875=77#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetSearch(env), output);
    ASSERT_STREQ(output.c_str(), "?ppp9875=77");
    url.SetSearch("");
    DealNapiStrValue(env, url.GetSearch(env), output);
    ASSERT_STREQ(output.c_str(), "");
}

HWTEST_F(NativeEngineTest, testUrlSetSearch002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:8080/directory/file?ppp9875=77#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetSearch(env), output);
    ASSERT_STREQ(output.c_str(), "?ppp9875=77");
    url.SetSearch("?444=555");
    DealNapiStrValue(env, url.GetSearch(env), output);
    ASSERT_STREQ(output.c_str(), "?444=555");
}

HWTEST_F(NativeEngineTest, testUrlSetSearch003, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:8080/directory/file?ppp9875=77#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetSearch(env), output);
    ASSERT_STREQ(output.c_str(), "?ppp9875=77");
    url.SetSearch("444=555");
    DealNapiStrValue(env, url.GetSearch(env), output);
    ASSERT_STREQ(output.c_str(), "?444=555");
}

HWTEST_F(NativeEngineTest, testUrlSetSearch004, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:8080/directory/file?ppp9875=77#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetSearch(env), output);
    ASSERT_STREQ(output.c_str(), "?ppp9875=77");
    url.SetSearch("?##44=55");
    DealNapiStrValue(env, url.GetSearch(env), output);
    ASSERT_STREQ(output.c_str(), "?%23%2344=55");
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

HWTEST_F(NativeEngineTest, testUrlSetFragment001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:8080/directory/file?query=pppppp#123456");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetFragment(env), output);
    ASSERT_STREQ(output.c_str(), "#123456");
    url.SetFragment("");
    DealNapiStrValue(env, url.GetFragment(env), output);
    ASSERT_STREQ(output.c_str(), "");
}

HWTEST_F(NativeEngineTest, testUrlSetFragment002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:8080/directory/file?query=pppppp#123456");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetFragment(env), output);
    ASSERT_STREQ(output.c_str(), "#123456");
    url.SetFragment("#445566");
    DealNapiStrValue(env, url.GetFragment(env), output);
    ASSERT_STREQ(output.c_str(), "#445566");
}

HWTEST_F(NativeEngineTest, testUrlSetFragment003, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:8080/directory/file?query=pppppp#123456");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetFragment(env), output);
    ASSERT_STREQ(output.c_str(), "#123456");
    url.SetFragment("445566");
    DealNapiStrValue(env, url.GetFragment(env), output);
    ASSERT_STREQ(output.c_str(), "#445566");
}

HWTEST_F(NativeEngineTest, testUrlIPV6001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://[1080:0:0:0:8:800:200C:417A]/index.html");
    napi_env env = (napi_env)engine_;
    napi_value result = url.GetIsIpv6(env);
    bool value = false;
    napi_get_value_bool(env, result, &value);
    ASSERT_TRUE(value);
}

HWTEST_F(NativeEngineTest, testUrlIPV6002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://0377.0xff.255.1:80/index.html");
    napi_env env = (napi_env)engine_;
    napi_value result = url.GetIsIpv6(env);
    bool value = false;
    napi_get_value_bool(env, result, &value);
    ASSERT_FALSE(value);
}

HWTEST_F(NativeEngineTest, testUrlIPV6003, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://[1080:0:0:0:8:800:200C.417A]/index.html");
    napi_env env = (napi_env)engine_;
    napi_value result = url.GetIsIpv6(env);
    bool value = false;
    napi_get_value_bool(env, result, &value);
    ASSERT_FALSE(value);
}

HWTEST_F(NativeEngineTest, testUrlGetOnOrOff001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://0377.0xff.255.1:80/index.html");
    napi_env env = (napi_env)engine_;
    napi_value result = url.GetOnOrOff(env);
    bool value = false;
    napi_get_value_bool(env, result, &value);
    ASSERT_TRUE(value);
}

HWTEST_F(NativeEngineTest, testUrlSetHostname001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://0377.0xff.255.1:80/index.html");
    napi_env env = (napi_env)engine_;
    std::string output;
    std::string value = "host";
    url.SetHostname(value);
    DealNapiStrValue(env, url.GetHostname(env), output);
    ASSERT_STREQ(output.c_str(), "host");
}

HWTEST_F(NativeEngineTest, testUrlSetHostname002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://0377.0xff.255.1:80/index.html");
    napi_env env = (napi_env)engine_;
    std::string output;
    std::string value = "";
    url.SetHostname(value);
    DealNapiStrValue(env, url.GetHostname(env), output);
    ASSERT_STREQ(output.c_str(), "255.255.255.1");
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

HWTEST_F(NativeEngineTest, testUrlSearchParamsAppend001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "ma");
    napi_value input2 = StrToNapiValue(env, "jk");
    params.Append(env, input1, input2);
    napi_value tempValue = params.GetArray(env);
    std::vector<std::string> paramsString = GetParamsStrig(env, tempValue);
    DealNapiStrValue(env, ToString(env, paramsString), output);
    ASSERT_STREQ(output.c_str(), "ma=jk");
}

HWTEST_F(NativeEngineTest, testUrlSearchParamsAppend002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "ma 大");
    napi_value input2 = StrToNapiValue(env, "jk￥");
    params.Append(env, input1, input2);
    napi_value tempValue = params.GetArray(env);
    std::vector<std::string> paramsString = GetParamsStrig(env, tempValue);
    DealNapiStrValue(env, ToString(env, paramsString), output);
    ASSERT_STREQ(output.c_str(), "ma+%E5%A4%A7=jk%EF%BF%A5");
}

HWTEST_F(NativeEngineTest, testUrlSearchParamsAppend003, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "foo~!@#$%^&*()_+-=");
    napi_value input2 = StrToNapiValue(env, "jk");
    params.Append(env, input1, input2);
    napi_value tempValue = params.GetArray(env);
    std::vector<std::string> paramsString = GetParamsStrig(env, tempValue);
    DealNapiStrValue(env, ToString(env, paramsString), output);
    ASSERT_STREQ(output.c_str(), "foo%7E%21%40%23%24%25%5E%26*%28%29_%2B-%3D=jk");
}

HWTEST_F(NativeEngineTest, testUrlSearchParamsDelete001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "foo~!@#$%^&*()_+-=");
    napi_value input2 = StrToNapiValue(env, "jk");
    params.Append(env, input1, input2);
    params.Delete(env, input1);
    napi_value tempValue = params.GetArray(env);
    std::vector<std::string> paramsString = GetParamsStrig(env, tempValue);
    DealNapiStrValue(env, ToString(env, paramsString), output);
    ASSERT_STREQ(output.c_str(), "");
}

HWTEST_F(NativeEngineTest, testUrlSearchParamsDelete002, testing::ext::TestSize.Level0)
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
    napi_value tempValue = params.GetArray(env);
    std::vector<std::string> paramsString = GetParamsStrig(env, tempValue);
    DealNapiStrValue(env, ToString(env, paramsString), output);
    ASSERT_STREQ(output.c_str(), "ma=jk");
}

HWTEST_F(NativeEngineTest, testUrlSearchParamsDelete003, testing::ext::TestSize.Level0)
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
    napi_value tempValue = params.GetArray(env);
    std::vector<std::string> paramsString = GetParamsStrig(env, tempValue);
    DealNapiStrValue(env, ToString(env, paramsString), output);
    ASSERT_STREQ(output.c_str(), "");
}

HWTEST_F(NativeEngineTest, testUrlSearchParamsGet001, testing::ext::TestSize.Level0)
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

HWTEST_F(NativeEngineTest, testUrlSearchParamsGet002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "jk");
    DealNapiStrValue(env, params.Get(env, input1), output);
    ASSERT_STREQ(output.c_str(), "");
}

HWTEST_F(NativeEngineTest, testUrlSearchParamsGet003, testing::ext::TestSize.Level0)
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

HWTEST_F(NativeEngineTest, testUrlSearchParamsGetAll001, testing::ext::TestSize.Level0)
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

HWTEST_F(NativeEngineTest, testUrlSearchParamsGetAll002, testing::ext::TestSize.Level0)
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

HWTEST_F(NativeEngineTest, testUrlSearchParamsGetAll003, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "#[%@]");
    napi_value input2 = StrToNapiValue(env, "jk");
    params.Append(env, input1, input2);
    napi_value input3 = StrToNapiValue(env, "ma1");
    napi_value input4 = StrToNapiValue(env, "jk1");
    params.Append(env, input3, input4);
    napi_value input5 = StrToNapiValue(env, "jk2");
    DealNapiStrValue(env, params.GetAll(env, input5), output);
    ASSERT_STREQ(output.c_str(), "");
}

HWTEST_F(NativeEngineTest, testUrlSearchParamsToUSVString001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "你好");
    napi_value input2 = StrToNapiValue(env, "안녕하세요");
    params.Append(env, input1, input2);
    napi_value input3 = StrToNapiValue(env, "[saa]");
    napi_value input4 = StrToNapiValue(env, "{aas}");
    params.Append(env, input3, input4);
    napi_value input5 = StrToNapiValue(env, "你好");
    DealNapiStrValue(env, params.Get(env, input5), output);
    ASSERT_STREQ(output.c_str(), "안녕하세요");
}

HWTEST_F(NativeEngineTest, testUrlSearchParamsHas001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "ma");
    DealNapiStrValue(env, params.IsHas(env, input1), output);
    ASSERT_STREQ(output.c_str(), "");
}

HWTEST_F(NativeEngineTest, testUrlSearchParamsHas002, testing::ext::TestSize.Level0)
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

HWTEST_F(NativeEngineTest, testUrlSearchParamsHas003, testing::ext::TestSize.Level0)
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

HWTEST_F(NativeEngineTest, testUrlSearchParamsSet001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "ma");
    napi_value input2 = StrToNapiValue(env, "jk");
    params.Append(env, input1, input2);
    napi_value input3 = StrToNapiValue(env, "aa");
    params.Set(env, input1, input3);
    napi_value tempValue = params.GetArray(env);
    std::vector<std::string> paramsString = GetParamsStrig(env, tempValue);
    DealNapiStrValue(env, ToString(env, paramsString), output);
    ASSERT_STREQ(output.c_str(), "ma=aa");
}

HWTEST_F(NativeEngineTest, testUrlSearchParamsSet002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "ma1");
    napi_value input2 = StrToNapiValue(env, "aa");
    params.Append(env, input1, input2);
    params.Set(env, input1, input2);
    napi_value tempValue = params.GetArray(env);
    std::vector<std::string> paramsString = GetParamsStrig(env, tempValue);
    DealNapiStrValue(env, ToString(env, paramsString), output);
    ASSERT_STREQ(output.c_str(), "ma1=aa");
}

HWTEST_F(NativeEngineTest, testUrlSearchParamsSet003, testing::ext::TestSize.Level0)
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
    napi_value tempValue = params.GetArray(env);
    std::vector<std::string> paramsString = GetParamsStrig(env, tempValue);
    DealNapiStrValue(env, ToString(env, paramsString), output);
    ASSERT_STREQ(output.c_str(), "ma=jk&ma1=aa");
}

HWTEST_F(NativeEngineTest, testUrlSearchParamsSort001, testing::ext::TestSize.Level0)
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
    napi_value tempValue = params.GetArray(env);
    std::vector<std::string> paramsString = GetParamsStrig(env, tempValue);
    DealNapiStrValue(env, ToString(env, paramsString), output);
    ASSERT_STREQ(output.c_str(), "a=jk1&ma=jk");
}

HWTEST_F(NativeEngineTest, testUrlSearchParamsSort002, testing::ext::TestSize.Level0)
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
    napi_value tempValue = params.GetArray(env);
    std::vector<std::string> paramsString = GetParamsStrig(env, tempValue);
    DealNapiStrValue(env, ToString(env, paramsString), output);
    ASSERT_STREQ(output.c_str(), "foo%7E%21%40%23%24%25%5E%26*%28%29_%2B-%3D=jk1&ma=jk");
}

HWTEST_F(NativeEngineTest, testUrlSearchParamsSort003, testing::ext::TestSize.Level0)
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
    napi_value tempValue = params.GetArray(env);
    std::vector<std::string> paramsString = GetParamsStrig(env, tempValue);
    DealNapiStrValue(env, ToString(env, paramsString), output);
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
    napi_value tempValue = params.GetArray(env);
    std::vector<std::string> paramsString = GetParamsStrig(env, tempValue);
    DealNapiStrValue(env, ToString(env, paramsString), output);
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
    napi_value tempValue = params.GetArray(env);
    std::vector<std::string> paramsString = GetParamsStrig(env, tempValue);
    DealNapiStrValue(env, ToString(env, paramsString), output);
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
    napi_value tempValue = params.GetArray(env);
    std::vector<std::string> paramsString = GetParamsStrig(env, tempValue);
    DealNapiStrValue(env, ToString(env, paramsString), output);
    ASSERT_STREQ(output.c_str(), "foo%7E%21%40%23%24%25%5E%26*%28%29_%2B-%3D=jk");
}

HWTEST_F(NativeEngineTest, testUrlSearchParams, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "ma");
    DealNapiStrValue(env, params.IsHas(env, input1), output);
    ASSERT_STREQ(output.c_str(), "");
}

HWTEST_F(NativeEngineTest, testUrlSearchParamsSetArray, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string input1 = "jk";
    std::string input2 = "ma";
    napi_env env = (napi_env)engine_;
    std::vector<std::string> vec;
    vec.push_back(input1);
    vec.push_back(input2);
    params.SetArray(env, vec);
    napi_value result = params.GetArray(env);
    uint32_t length = 0;
    napi_value napiStr = nullptr;
    size_t arraySize = 0;
    napi_get_array_length(env, result, &length);
    std::string cstr1 = "";
    for (size_t i = 0; i < length; i++) {
        napi_get_element(env, result, i, &napiStr);
        napi_get_value_string_utf8(env, napiStr, nullptr, 0, &arraySize);
        if (arraySize > 0) {
            napi_get_value_string_utf8(env, napiStr, cstr1.data(), arraySize + 1, &arraySize);
        }
        if (i == 0) {
            ASSERT_STREQ("jk", cstr1.data());
        } else {
            ASSERT_STREQ(cstr1.data(), "ma");
        }
    }
}

HWTEST_F(NativeEngineTest, testUrlSearchParamsIterByKeys, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string input1 = "jk";
    std::string input2 = "ma";
    napi_env env = (napi_env)engine_;
    std::vector<std::string> vec;
    vec.push_back(input1);
    vec.push_back(input2);
    params.SetArray(env, vec);
    napi_value result = params.IterByKeys(env);
    uint32_t length = 0;
    napi_value napiStr = nullptr;
    size_t arraySize = 0;
    napi_get_array_length(env, result, &length);
    std::string cstr = "";
    for (size_t i = 0; i < length; i += 2) {
        napi_get_element(env, result, i, &napiStr);
        napi_get_value_string_utf8(env, napiStr, nullptr, 0, &arraySize);
        if (arraySize > 0) {
            napi_get_value_string_utf8(env, napiStr, cstr.data(), arraySize + 1, &arraySize);
        }
    }
    ASSERT_STREQ(cstr.data(), "jk");
}

HWTEST_F(NativeEngineTest, testUrlSearchParamsIterByValues, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string input1 = "jk";
    std::string input2 = "ma";
    napi_env env = (napi_env)engine_;
    std::vector<std::string> vec;
    vec.push_back(input1);
    vec.push_back(input2);
    params.SetArray(env, vec);
    napi_value result = params.IterByValues(env);
    uint32_t length = 0;
    napi_value napiStr = nullptr;
    size_t arraySize = 0;
    napi_get_array_length(env, result, &length);
    std::string cstr = "";
    for (size_t i = 0; i < length; i++) {
        napi_get_element(env, result, i, &napiStr);
        napi_get_value_string_utf8(env, napiStr, nullptr, 0, &arraySize);
        if (arraySize > 0) {
            napi_get_value_string_utf8(env, napiStr, cstr.data(), arraySize + 1, &arraySize);
        }
    }
    ASSERT_STREQ(cstr.data(), "ma");
}

HWTEST_F(NativeEngineTest, testUrlSearchParamsEntries001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "ma");
    napi_value input2 = StrToNapiValue(env, "jk");
    params.Append(env, input1, input2);
    napi_value input3 = StrToNapiValue(env, "你好");
    napi_value input4 = StrToNapiValue(env, "jk1");
    params.Append(env, input3, input4);
    napi_value input5 = StrToNapiValue(env, "a");
    napi_value input6 = StrToNapiValue(env, "jk2");
    params.Append(env, input5, input6);
    napi_value result = params.Entries(env);
    uint32_t length = 0;
    napi_value napiStr = nullptr;
    size_t arraySize = 0;
    napi_get_array_length(env, result, &length);
    std::string cstr = "";
    for (size_t i = 0; i < length; i++) {
        napi_get_element(env, result, i, &napiStr);
        napi_get_value_string_utf8(env, napiStr, nullptr, 0, &arraySize);
        if (arraySize > 0) {
            napi_get_value_string_utf8(env, napiStr, cstr.data(), arraySize + 1, &arraySize);
        }
    }
    ASSERT_EQ(length, 3);
}