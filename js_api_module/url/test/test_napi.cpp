/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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
#include "tools/log.h"
#include "js_url.h"
#include <regex>
#include <sstream>
#include "securec.h"
#include "unicode/stringpiece.h"
#include "unicode/unistr.h"
#include "native_module_url.h"
#include "url_helper.h"

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

static bool IsEscapeRange(const char ch)
{
    if ((ch > 0 && ch < '*') || (ch > '*' && ch < '-') || (ch == '/') ||
        (ch > '9' && ch < 'A') || (ch > 'Z' && ch < '_') || (ch == '`') || (ch > 'z')) {
        return true;
    }
    return false;
}

static std::string ReviseStr(std::string &str, std::string *reviseChar)
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

static std::string DecimalToPercentHexString(uint8_t n)
{
    std::string tem = "%";
    return tem + OHOS::Url::HEX_CHAR_MAP[(n >> OHOS::Url::INT_SHIFT_SIZE) & 0xf] + OHOS::Url::HEX_CHAR_MAP[n & 0xf];
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

static std::vector<std::string> GetParamsStrig(napi_env env, const napi_value tempStr)
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
    ASSERT_STREQ(output.c_str(), "/baz");
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

HWTEST_F(NativeEngineTest, testUrlConstructs014, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http1://www.example.com");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetScheme(env), output);
    ASSERT_STREQ(output.c_str(), "http1:");
}

HWTEST_F(NativeEngineTest, testUrlConstructs015, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http2://www.example.com:22/adfee/kk?a=4#saf");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetScheme(env), output);
    ASSERT_STREQ(output.c_str(), "http2:");
}

HWTEST_F(NativeEngineTest, testUrlConstructs016, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http2://username:password@www.example.com:22/adfee/kk?a=4#saf");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetScheme(env), output);
    ASSERT_STREQ(output.c_str(), "http2:");
}

HWTEST_F(NativeEngineTest, testUrlConstructs017, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://");
    napi_env env = (napi_env)engine_;
    napi_value result = url.GetOnOrOff(env);
    bool value = false;
    napi_get_value_bool(env, result, &value);
    ASSERT_FALSE(value);
}

HWTEST_F(NativeEngineTest, testUrlConstructs018, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@/adfee/kk?a=4#saf");
    napi_env env = (napi_env)engine_;
    napi_value result = url.GetOnOrOff(env);
    bool value = false;
    napi_get_value_bool(env, result, &value);
    ASSERT_FALSE(value);
}

HWTEST_F(NativeEngineTest, testUrlConstructs019, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@/adfee/kk?a=4#saf", "");
    napi_env env = (napi_env)engine_;
    napi_value result = url.GetOnOrOff(env);
    bool value = false;
    napi_get_value_bool(env, result, &value);
    ASSERT_FALSE(value);
}

HWTEST_F(NativeEngineTest, testUrlConstructs020, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@sda/adfee/kk?a=4#saf", "http://sads/sad");
    napi_env env = (napi_env)engine_;
    napi_value result = url.GetOnOrOff(env);
    bool value = false;
    napi_get_value_bool(env, result, &value);
    ASSERT_TRUE(value);
}

HWTEST_F(NativeEngineTest, testUrlConstructs021, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("//fdsa", "http://sad");
    napi_env env = (napi_env)engine_;
    napi_value result = url.GetOnOrOff(env);
    bool value = false;
    napi_get_value_bool(env, result, &value);
    ASSERT_TRUE(value);
}

HWTEST_F(NativeEngineTest, testUrlConstructs022, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("-http://username:password@/adfee/kk?a=4#saf");
    napi_env env = (napi_env)engine_;
    napi_value result = url.GetOnOrOff(env);
    bool value = false;
    napi_get_value_bool(env, result, &value);
    ASSERT_FALSE(value);
}

HWTEST_F(NativeEngineTest, testUrlConstructs023, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL base("http://www.example.com");
    OHOS::Url::URL url("//asd", base);
    napi_env env = (napi_env)engine_;
    napi_value result = url.GetOnOrOff(env);
    bool value = false;
    napi_get_value_bool(env, result, &value);
    ASSERT_TRUE(value);
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

HWTEST_F(NativeEngineTest, testUrlSetScheme004, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("https://username:password@host:8080/directory/file?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetScheme(env), output);
    ASSERT_STREQ(output.c_str(), "https:");
    url.SetScheme("file");
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

HWTEST_F(NativeEngineTest, testUrlSetHost003, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:199/directory/file?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetHost(env), output);
    ASSERT_STREQ(output.c_str(), "host:199");
    url.SetHost("/");
    DealNapiStrValue(env, url.GetHost(env), output);
    ASSERT_STREQ(output.c_str(), "host:199");
}

HWTEST_F(NativeEngineTest, testUrlSetHost004, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:199/directory/file?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetHost(env), output);
    ASSERT_STREQ(output.c_str(), "host:199");
    url.SetHost("asdzxc:54/");
    napi_value result = url.GetOnOrOff(env);
    bool value = false;
    napi_get_value_bool(env, result, &value);
    ASSERT_TRUE(value);
}

HWTEST_F(NativeEngineTest, testUrlSetHref001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:199/directory/file?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetHost(env), output);
    ASSERT_STREQ(output.c_str(), "host:199");
    url.SetHref("http:example.com");
    DealNapiStrValue(env, url.GetHost(env), output);
    ASSERT_STREQ(output.c_str(), "example.com");
}

HWTEST_F(NativeEngineTest, testUrlSetHref002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:199/directory/file?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetHost(env), output);
    ASSERT_STREQ(output.c_str(), "host:199");
    url.SetHref("   http:example.com");
    DealNapiStrValue(env, url.GetHost(env), output);
    ASSERT_STREQ(output.c_str(), "example.com");
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

HWTEST_F(NativeEngineTest, testUrlPort004, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:655656/directory/file?query#fragment");
    napi_env env = (napi_env)engine_;
    napi_value result = url.GetOnOrOff(env);
    bool value = false;
    napi_get_value_bool(env, result, &value);
    ASSERT_FALSE(value);
}

HWTEST_F(NativeEngineTest, testUrlPort005, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:6c3/directory/file?query#fragment");
    napi_env env = (napi_env)engine_;
    napi_value result = url.GetOnOrOff(env);
    bool value = false;
    napi_get_value_bool(env, result, &value);
    ASSERT_FALSE(value);
}

HWTEST_F(NativeEngineTest, testUrlSetPort001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:100/directory/file?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetPort(env), output);
    ASSERT_STREQ(output.c_str(), "100");
    url.SetPort("600?");
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
    OHOS::Url::URL url("http2://username:password@host:8080?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetPath(env), output);
    ASSERT_STREQ(output.c_str(), "");
}

HWTEST_F(NativeEngineTest, testUrlPathname004, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:8080?query#fragment");
    napi_env env = (napi_env)engine_;
    napi_value result = url.GetOnOrOff(env);
    bool value = false;
    napi_get_value_bool(env, result, &value);
    ASSERT_TRUE(value);
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
    ASSERT_STREQ(output.c_str(), "/%5CD:");
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
    ASSERT_STREQ(output.c_str(), "/:%5CD:");
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

HWTEST_F(NativeEngineTest, testUrlSetPath004, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:8080?query#fragment");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetPath(env), output);
    ASSERT_STREQ(output.c_str(), "/");
    url.SetPath("");
    DealNapiStrValue(env, url.GetPath(env), output);
    ASSERT_STREQ(output.c_str(), "/");
}

HWTEST_F(NativeEngineTest, testUrlSetPath005, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("file://hostsasf");
    std::string output;
    napi_env env = (napi_env)engine_;
    url.SetPath("/askdj");
    DealNapiStrValue(env, url.GetPath(env), output);
    ASSERT_STREQ(output.c_str(), "/askdj");
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

HWTEST_F(NativeEngineTest, testUrlGetOnOrOff002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://192.168.259.551:80/index.html");
    napi_env env = (napi_env)engine_;
    napi_value result = url.GetOnOrOff(env);
    bool value = false;
    napi_get_value_bool(env, result, &value);
    ASSERT_FALSE(value);
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

HWTEST_F(NativeEngineTest, testUrlSetHostname003, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("file:///www.example.com");
    napi_env env = (napi_env)engine_;
    std::string value = "localhost/";
    url.SetHostname(value);
    napi_value result = url.GetOnOrOff(env);
    bool value1 = false;
    napi_get_value_bool(env, result, &value1);
    ASSERT_TRUE(value1);
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

HWTEST_F(NativeEngineTest, testUrlSearchParamsAppend004, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    napi_value input2 = StrToNapiValue(env, "jk");
    params.Append(env, result, input2);
    DealNapiStrValue(env, params.Get(env, input2), output);
    ASSERT_STREQ(output.c_str(), "");
}

HWTEST_F(NativeEngineTest, testUrlSearchParamsAppend005, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "foo~!@#$%^&*()_+-=");
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    params.Append(env, input1, result);
    DealNapiStrValue(env, params.Get(env, input1), output);
    ASSERT_STREQ(output.c_str(), "");
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

HWTEST_F(NativeEngineTest, testUrlSearchParamsDelete004, testing::ext::TestSize.Level0)
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
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    params.Delete(env, result);
    DealNapiStrValue(env, params.Get(env, input3), output);
    ASSERT_STREQ(output.c_str(), "jk");
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

HWTEST_F(NativeEngineTest, testUrlSearchParamsGet004, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "hello, \U0001F600 world!");
    DealNapiStrValue(env, params.Get(env, input1), output);
    ASSERT_STREQ(output.c_str(), "");
}

HWTEST_F(NativeEngineTest, testUrlSearchParamsGet005, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    DealNapiStrValue(env, params.Get(env, result), output);
    ASSERT_STREQ(output.c_str(), "");
}

HWTEST_F(NativeEngineTest, testUrlSearchParamsGet006, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "foo~!@#$%^&*()_+-=");
    napi_value input2 = StrToNapiValue(env, "jk");
    napi_value input3 = StrToNapiValue(env, "asd");
    params.Append(env, input1, input2);
    DealNapiStrValue(env, params.Get(env, input3), output);
    ASSERT_STREQ(output.c_str(), "");
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

HWTEST_F(NativeEngineTest, testUrlSearchParamsGetAll004, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    DealNapiStrValue(env, params.GetAll(env, result), output);
    ASSERT_STREQ(output.c_str(), "");
}

HWTEST_F(NativeEngineTest, testUrlSearchParamsGetAll005, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "jk");
    DealNapiStrValue(env, params.GetAll(env, input1), output);
    ASSERT_STREQ(output.c_str(), "");
}

HWTEST_F(NativeEngineTest, testUrlSearchParamsGetAll006, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "ma");
    napi_value input2 = StrToNapiValue(env, "jk");
    params.Append(env, input1, input2);
    napi_value input3 = StrToNapiValue(env, "ma");
    DealNapiStrValue(env, params.GetAll(env, input3), output);
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

HWTEST_F(NativeEngineTest, testUrlSearchParamsHas004, testing::ext::TestSize.Level0)
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
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    DealNapiStrValue(env, params.IsHas(env, result), output);
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

HWTEST_F(NativeEngineTest, testUrlSearchParamsSet004, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "ma1");
    napi_value input2 = StrToNapiValue(env, "aa");
    params.Append(env, input1, input2);
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    params.Set(env, result, input2);
    napi_value tempValue = params.GetArray(env);
    std::vector<std::string> paramsString = GetParamsStrig(env, tempValue);
    DealNapiStrValue(env, ToString(env, paramsString), output);
    ASSERT_STREQ(output.c_str(), "ma1=aa");
}

HWTEST_F(NativeEngineTest, testUrlSearchParamsSet005, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    std::string output;
    napi_env env = (napi_env)engine_;
    napi_value input1 = StrToNapiValue(env, "ma1");
    napi_value input2 = StrToNapiValue(env, "aa");
    params.Append(env, input1, input2);
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    params.Set(env, input1, result);
    napi_value tempValue = params.GetArray(env);
    std::vector<std::string> paramsString = GetParamsStrig(env, tempValue);
    DealNapiStrValue(env, ToString(env, paramsString), output);
    ASSERT_STREQ(output.c_str(), "ma1=aa");
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

HWTEST_F(NativeEngineTest, testUrlSearchParamsEntries002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    napi_env env = (napi_env)engine_;
    std::string output;
    params.Entries(env);
    napi_value input1 = StrToNapiValue(env, "ma");
    DealNapiStrValue(env, params.Get(env, input1), output);
    ASSERT_STREQ(output.c_str(), "");
}

HWTEST_F(NativeEngineTest, testUrlutilities001, testing::ext::TestSize.Level0)
{
    std::string inPut = "";
    OHOS::Url::DecodeSpecialChars(inPut);
    inPut = "%1238";
    OHOS::Url::DecodeSpecialChars(inPut);
    inPut = "    ";
    OHOS::Url::DeleteC0OrSpace(inPut);
    inPut = "   123  ";
    OHOS::Url::DeleteC0OrSpace(inPut);
    inPut = "dks	aasdd";
    OHOS::Url::DeleteTabOrNewline(inPut);
    inPut = "1asdf";
    std::string temp = "";
    std::bitset<11> flags; // 11:Each bit of a BIT represents a different parsing state.
    bool isFalse = OHOS::Url::AnalysisScheme(inPut, temp, flags);
    ASSERT_FALSE(isFalse);
    inPut = "@Sdf";
    OHOS::Url::AnalysisUsernameAndPasswd(inPut, temp, temp, flags);
    inPut = "12@3@222@Sdf";
    OHOS::Url::AnalysisUsernameAndPasswd(inPut, temp, temp, flags);
    inPut = "../../";
    std::vector<std::string> temp1;
    OHOS::Url::AnalysisPath(inPut, temp1, flags, true);
    inPut = "asd#sad";
    OHOS::Url::AnalysisOpaqueHost(inPut, temp, flags);
    inPut = "asdsad";
    OHOS::Url::AnalysisOpaqueHost(inPut, temp, flags);
    bool isHexDigit =  OHOS::Url::IsHexDigit('/');
    ASSERT_FALSE(isHexDigit);
}

HWTEST_F(NativeEngineTest, testUrlutilities002, testing::ext::TestSize.Level0)
{
    OHOS::Url::UrlData urlDataInfo;
    std::string inPut = "192.136.54.22";
    std::string temp = "";
    std::bitset<11> flags;
    OHOS::Url::DealIpv4(inPut);
    inPut = "1080:0:0:0:8:800:200C:417A";
    OHOS::Url::FormatIpv6(inPut);
    inPut = "2136504";
    size_t i = 0;
    OHOS::Url::SplitNum(inPut, i);
    inPut = "192.168.1.55";
    OHOS::Url::AnalyseIPv4(inPut, temp, flags);
    inPut = "";
    OHOS::Url::AnalysisHost(inPut, inPut, flags, true);
    inPut = "[asjdhkj";
    OHOS::Url::AnalysisHost(inPut, inPut, flags, true);
    inPut = "f:11";
    bool isTrue = OHOS::Url::ISFileNohost(inPut);
    ASSERT_TRUE(isTrue);
    inPut = "../../../";
    OHOS::Url::AnalysisFilePath(inPut, urlDataInfo, flags);
    OHOS::Url::AnalysisFilescheme(inPut, urlDataInfo, flags);
    OHOS::Url::AnalyInfoPath(flags, urlDataInfo, inPut);
    inPut = "sadwasd";
    OHOS::Url::AnalyHostPath(inPut, flags, urlDataInfo);
    inPut = "sad@wasd";
    OHOS::Url::AnalyStrHost(inPut, urlDataInfo, flags);
    inPut = "";
    OHOS::Url::AnalyStrHost(inPut, urlDataInfo, flags);
    inPut = "//sjalkjd";
    urlDataInfo.scheme = "sa";
    OHOS::Url::AnalysisNoDefaultProtocol(inPut, urlDataInfo, flags);
    inPut = "//sjalkjd";
    urlDataInfo.scheme = "";
    OHOS::Url::AnalysisNoDefaultProtocol(inPut, urlDataInfo, flags);
    inPut = "";
    OHOS::Url::AnalysisOnlyHost(inPut, urlDataInfo, flags, i);
    bool isHexDigit =  OHOS::Url::IsHexDigit('b');
    ASSERT_TRUE(isHexDigit);
}

std::string GetStringUtf8(napi_env env, napi_value str)
{
    std::string buffer = "";
    size_t bufferSize = 0;
    if (napi_get_value_string_utf8(env, str, nullptr, 0, &bufferSize) != napi_ok) {
        HILOG_ERROR("can not get src size");
        return buffer;
    }
    buffer.resize(bufferSize);
    if (napi_get_value_string_utf8(env, str, buffer.data(), bufferSize + 1, &bufferSize) != napi_ok) {
        HILOG_ERROR("can not get src value");
        return buffer;
    }
    return buffer;
}

HWTEST_F(NativeEngineTest, testUrlModule001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value exports = nullptr;
    napi_create_object(env, &exports);
    OHOS::Url::Init(env, exports);
    napi_value urlClass = nullptr;
    napi_value constructorArgs[1] =  { 0 };
    std::string input = "http://username:password@www.baidu.com:99/path/path?query#fagment";
    napi_create_string_utf8(env, input.c_str(), input.size(), &constructorArgs[0]);
    napi_get_named_property(env, exports, "Url", &urlClass);
    napi_value instance = nullptr;
    napi_new_instance(env, urlClass, 1, constructorArgs, &instance);

    std::string input1 = "www.example.com";
    napi_value newValue = nullptr;
    napi_create_string_utf8(env, input1.c_str(), input1.size(), &newValue);
    napi_set_named_property(env, instance, "hostname", newValue);
    napi_value urlProperty = nullptr;
    napi_get_named_property(env, instance, "hostname", &urlProperty);
    std::string res = GetStringUtf8(env, urlProperty);
    ASSERT_STREQ(res.c_str(), "www.example.com");

    napi_value tempFn = nullptr;
    napi_get_named_property(env, instance, "onOrOff", &tempFn);
    bool res1 = false;
    napi_get_value_bool(env, tempFn, &res1);
    ASSERT_TRUE(res1);

    napi_get_named_property(env, instance, "GetIsIpv6", &tempFn);
    res1 = true;
    napi_get_value_bool(env, tempFn, &res1);
    ASSERT_FALSE(res1);

    input1 = "query1";
    newValue = nullptr;
    napi_create_string_utf8(env, input1.c_str(), input1.size(), &newValue);
    napi_set_named_property(env, instance, "search", newValue);
    urlProperty = nullptr;
    napi_get_named_property(env, instance, "search", &urlProperty);
    res = GetStringUtf8(env, urlProperty);
    ASSERT_STREQ(res.c_str(), "?query1");

    input1 = "username1";
    newValue = nullptr;
    napi_create_string_utf8(env, input1.c_str(), input1.size(), &newValue);
    napi_set_named_property(env, instance, "username", newValue);
    urlProperty = nullptr;
    napi_get_named_property(env, instance, "username", &urlProperty);
    res = GetStringUtf8(env, urlProperty);
    ASSERT_STREQ(res.c_str(), "username1");
}

HWTEST_F(NativeEngineTest, testUrlModule003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value exports = nullptr;
    napi_create_object(env, &exports);
    OHOS::Url::Init(env, exports);
    napi_value urlClass = nullptr;
    napi_value constructorArgs[1] =  { 0 };
    std::string input = "http://username:password@www.baidu.com:99/path/path?query#fagment";
    napi_create_string_utf8(env, input.c_str(), input.size(), &constructorArgs[0]);
    napi_get_named_property(env, exports, "Url", &urlClass);
    napi_value instance = nullptr;
    napi_new_instance(env, urlClass, 1, constructorArgs, &instance);

    std::string input1 = "password1";
    napi_value newValue = nullptr;
    napi_create_string_utf8(env, input1.c_str(), input1.size(), &newValue);
    napi_set_named_property(env, instance, "password", newValue);
    napi_value urlProperty = nullptr;
    napi_get_named_property(env, instance, "password", &urlProperty);
    std::string res = GetStringUtf8(env, urlProperty);
    ASSERT_STREQ(res.c_str(), "password1");

    input1 = "www.example.com:11";
    newValue = nullptr;
    napi_create_string_utf8(env, input1.c_str(), input1.size(), &newValue);
    napi_set_named_property(env, instance, "host", newValue);
    urlProperty = nullptr;
    napi_get_named_property(env, instance, "host", &urlProperty);
    res = GetStringUtf8(env, urlProperty);
    ASSERT_STREQ(res.c_str(), "www.example.com:11");

    input1 = "fagment1";
    newValue = nullptr;
    napi_create_string_utf8(env, input1.c_str(), input1.size(), &newValue);
    napi_set_named_property(env, instance, "hash", newValue);
    urlProperty = nullptr;
    napi_get_named_property(env, instance, "hash", &urlProperty);
    res = GetStringUtf8(env, urlProperty);
    ASSERT_STREQ(res.c_str(), "#fagment1");
}

HWTEST_F(NativeEngineTest, testUrlModule004, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value exports = nullptr;
    napi_create_object(env, &exports);
    OHOS::Url::Init(env, exports);
    napi_value urlClass = nullptr;
    napi_value constructorArgs[1] =  { 0 };
    std::string input = "http://username:password@www.baidu.com:99/path/path?query#fagment";
    napi_create_string_utf8(env, input.c_str(), input.size(), &constructorArgs[0]);
    napi_get_named_property(env, exports, "Url", &urlClass);
    napi_value instance = nullptr;
    napi_new_instance(env, urlClass, 1, constructorArgs, &instance);

    std::string input1 = "https:";
    napi_value newValue = nullptr;
    napi_create_string_utf8(env, input1.c_str(), input1.size(), &newValue);
    napi_set_named_property(env, instance, "protocol", newValue);
    napi_value urlProperty = nullptr;
    napi_get_named_property(env, instance, "protocol", &urlProperty);
    std::string res = GetStringUtf8(env, urlProperty);
    ASSERT_STREQ(res.c_str(), "https:");

    input1 = "/path/path1";
    newValue = nullptr;
    napi_create_string_utf8(env, input1.c_str(), input1.size(), &newValue);
    napi_set_named_property(env, instance, "pathname", newValue);
    urlProperty = nullptr;
    napi_get_named_property(env, instance, "pathname", &urlProperty);
    res = GetStringUtf8(env, urlProperty);
    ASSERT_STREQ(res.c_str(), "/path/path1");

    input1 = "55";
    newValue = nullptr;
    napi_create_string_utf8(env, input1.c_str(), input1.size(), &newValue);
    napi_set_named_property(env, instance, "port", newValue);
    urlProperty = nullptr;
    napi_get_named_property(env, instance, "port", &urlProperty);
    res = GetStringUtf8(env, urlProperty);
    ASSERT_STREQ(res.c_str(), "55");

    napi_value constructorArgs1[1] =  { 0 };
    napi_create_string_utf8(env, input.c_str(), input.size(), &constructorArgs1[0]);
    napi_value hrefFn = nullptr;
    napi_get_named_property(env, instance, "href", &hrefFn);
    napi_value result1 = nullptr;
    napi_call_function(env, instance, hrefFn, 1, constructorArgs1, &result1);

    urlProperty = nullptr;
    napi_get_named_property(env, instance, "port", &urlProperty);
    res = GetStringUtf8(env, urlProperty);
    ASSERT_STREQ(res.c_str(), "99");
}

HWTEST_F(NativeEngineTest, testUrlModule002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value exports = nullptr;
    napi_create_object(env, &exports);
    OHOS::Url::Init(env, exports);
    napi_value urlParamsClass = nullptr;
    napi_get_named_property(env, exports, "URLParams1", &urlParamsClass);
    napi_value instance = nullptr;
    napi_new_instance(env, urlParamsClass, 0, nullptr, &instance);

    napi_value paramsFn = nullptr;
    napi_get_named_property(env, instance, "append", &paramsFn);
    napi_value input1 = StrToNapiValue(env, "ma");
    napi_value input2 = StrToNapiValue(env, "jk");
    napi_value args[] = { input1, input2 };
    napi_value result = nullptr;
    napi_call_function(env, instance, paramsFn, 2, args, &result);

    paramsFn = nullptr;
    napi_get_named_property(env, instance, "set", &paramsFn);
    napi_value input3 = StrToNapiValue(env, "aa");
    napi_value args1[] = { input1, input3 };
    napi_call_function(env, instance, paramsFn, 2, args1, &result);

    paramsFn = nullptr;
    napi_get_named_property(env, instance, "array", &paramsFn);
    std::string output = "";
    std::vector<std::string> paramsString = GetParamsStrig(env, paramsFn);
    DealNapiStrValue(env, ToString(env, paramsString), output);
    ASSERT_STREQ(output.c_str(), "ma=aa");
    napi_set_named_property(env, instance, "array", paramsFn);

    paramsFn = nullptr;
    napi_get_named_property(env, instance, "has", &paramsFn);
    napi_value args2[1] = { input1 };
    napi_call_function(env, instance, paramsFn, 1, args2, &result);
    bool res1 = false;
    napi_get_value_bool(env, result, &res1);
    ASSERT_TRUE(res1);
}
HWTEST_F(NativeEngineTest, testUrlModule005, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value exports = nullptr;
    napi_create_object(env, &exports);
    OHOS::Url::Init(env, exports);
    napi_value urlParamsClass = nullptr;
    napi_get_named_property(env, exports, "URLParams1", &urlParamsClass);
    napi_value instance = nullptr;
    napi_new_instance(env, urlParamsClass, 0, nullptr, &instance);
    std::string output = "";
    napi_value paramsFn = nullptr;
    napi_get_named_property(env, instance, "append", &paramsFn);
    napi_value input1 = StrToNapiValue(env, "ma");
    napi_value result = nullptr;

    paramsFn = nullptr;
    napi_get_named_property(env, instance, "set", &paramsFn);
    napi_value input3 = StrToNapiValue(env, "aa");
    napi_value args1[] = { input1, input3 };
    napi_call_function(env, instance, paramsFn, 2, args1, &result);

    paramsFn = nullptr;
    napi_get_named_property(env, instance, "sort", &paramsFn);
    napi_call_function(env, instance, paramsFn, 0, nullptr, &result);

    paramsFn = nullptr;
    napi_get_named_property(env, instance, "keys", &paramsFn);
    napi_call_function(env, instance, paramsFn, 0, nullptr, &result);

    paramsFn = nullptr;
    napi_get_named_property(env, instance, "values", &paramsFn);
    napi_call_function(env, instance, paramsFn, 0, nullptr, &result);

    paramsFn = nullptr;
    napi_get_named_property(env, instance, "get", &paramsFn);

    napi_value args3[1] = { input1 };
    napi_call_function(env, instance, paramsFn, 1, args3, &result);
    DealNapiStrValue(env, result, output);
    ASSERT_STREQ(output.c_str(), "aa");

    paramsFn = nullptr;
    napi_get_named_property(env, instance, "getAll", &paramsFn);

    napi_value args4[1] = { input1 };
    napi_call_function(env, instance, paramsFn, 1, args4, &result);
    DealNapiStrValue(env, result, output);
    ASSERT_STREQ(output.c_str(), "aa");

    paramsFn = nullptr;
    napi_get_named_property(env, instance, "entries", &paramsFn);
    napi_call_function(env, instance, paramsFn, 0, nullptr, &result);

    paramsFn = nullptr;
    napi_get_named_property(env, instance, "delete", &paramsFn);
    napi_value args5[1] = { input1 };
    napi_call_function(env, instance, paramsFn, 1, args5, &result);
    DealNapiStrValue(env, result, output);
    ASSERT_STREQ(output.c_str(), "aa");
}
HWTEST_F(NativeEngineTest, testUrlModule006, testing::ext::TestSize.Level0)
{
    std::string inPut =  "#1asdf";
    std::string temp = "";
    std::bitset<11> flags; // 11:Each bit of a BIT represents a different parsing state.
    bool isFalse = OHOS::Url::AnalysisScheme(inPut, temp, flags);
    ASSERT_FALSE(isFalse);
}

HWTEST_F(NativeEngineTest, testUrlModule007, testing::ext::TestSize.Level0)
{
    std::string inPut =  "";
    std::string temp = "";
    std::bitset<11> flags(1023) ; // 11:Each bit of a BIT represents a different parsing state.
    OHOS::Url::AnalysisScheme(inPut, temp, flags);
    long a = flags.to_ulong();
    ASSERT_EQ(a, 1023);
}

HWTEST_F(NativeEngineTest, testUrlModule008, testing::ext::TestSize.Level0)
{
    std::string inPut =  "66666";
    std::string temp = "";
    std::bitset<11> flags(1023) ; // 11:Each bit of a BIT represents a different parsing state.
    OHOS::Url::AnalysisScheme(inPut, temp, flags);
    long a = flags.to_ulong();
    ASSERT_EQ(a, 1023);
}

HWTEST_F(NativeEngineTest, testUrlutilities009, testing::ext::TestSize.Level0)
{
    std::string inPut = "99::1080:0:0:8:800:200C:417A";
    OHOS::Url::FormatIpv6(inPut);
    ASSERT_STREQ(inPut.c_str(), "99:1080:0:0:8:800:200C:417A");
    inPut = "::99:1080:0:0:8:800:200C:417A";
    OHOS::Url::FormatIpv6(inPut);
    ASSERT_STREQ(inPut.c_str(), "0:99:1080:0:0:8:800:200C:417A");
}

HWTEST_F(NativeEngineTest, testUrlutilities010, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://0377.0xff.255.1:80/index.html");
    std::string str = "0x.23.56";
    url.SetHostname(str);
    ASSERT_STREQ(str.c_str(), "0x.23.56");
}

HWTEST_F(NativeEngineTest, testUrlutilities011, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://0377.0xff.255.1:80/index.html");
    std::string str = "0.23.56";
    url.SetHostname(str);
    ASSERT_STREQ(str.c_str(), "0.23.56");
}

HWTEST_F(NativeEngineTest, testUrlutilities012, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://0377.0xff.255.1:80/index.html");
    std::string str = "33."".";
    url.SetHostname(str);
    ASSERT_STREQ(str.c_str(), "33..");
}

HWTEST_F(NativeEngineTest, testUrlutilities013, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:199/directory/file?query#fragment");
    std::string str = "111111111.1001000.111.10101";
    url.SetHost(str);
    ASSERT_STREQ(str.c_str(), "111111111.1001000.111.10101");
}

HWTEST_F(NativeEngineTest, testUrlutilities014, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://username:password@host:199/directory/file?query#fragment");
    std::string str = "10.11.11111111";
    url.SetHost(str);
    ASSERT_STREQ(str.c_str(), "10.11.11111111");
}

HWTEST_F(NativeEngineTest, testUrlutilities015, testing::ext::TestSize.Level0)
{
    std::string str = "hello world!";
    size_t number = 0;
    OHOS::Url::SplitNum(str, number);
    ASSERT_STREQ(str.c_str(), "hello world!");
}

HWTEST_F(NativeEngineTest, testUrlutilities016, testing::ext::TestSize.Level0)
{
    std::string input = "H+#LLOWORLD";
    std::string scheme = "";
    std::bitset<static_cast<size_t>(OHOS::Url::BitsetStatusFlag::BIT_STATUS_11)> flags;
    bool rst = OHOS::Url::AnalysisScheme(input, scheme, flags);
    ASSERT_FALSE(rst);
}

HWTEST_F(NativeEngineTest, testUrlutilities017, testing::ext::TestSize.Level0)
{
    std::string input = "H+#LLOWORLD:";
    OHOS::Url::UrlData urlinfo;
    std::bitset<static_cast<size_t>(OHOS::Url::BitsetStatusFlag::BIT_STATUS_11)> flags;
    size_t pos = 0;
    OHOS::Url::AnalysisOnlyHost(input, urlinfo, flags, pos);
    ASSERT_STREQ(input.c_str(), "H+#LLOWORLD:");
}

HWTEST_F(NativeEngineTest, testUrlutilities018, testing::ext::TestSize.Level0)
{
    std::string input = "H+#LLOWORLD:65536";
    OHOS::Url::UrlData urlinfo;
    std::bitset<static_cast<size_t>(OHOS::Url::BitsetStatusFlag::BIT_STATUS_11)> flags;
    size_t pos = 0;
    OHOS::Url::AnalysisOnlyHost(input, urlinfo, flags, pos);
    ASSERT_STREQ(input.c_str(), "H+#LLOWORLD:65536");
}

HWTEST_F(NativeEngineTest, testUrlutilities019, testing::ext::TestSize.Level0)
{
    std::string input01 = "256";
    std::vector<std::string> nums;
    nums.push_back(input01);
    std::string host;
    std::bitset<static_cast<size_t>(OHOS::Url::BitsetStatusFlag::BIT_STATUS_11)> flags;
    OHOS::Url::FormatIpv4(nums, host, flags);
    ASSERT_STREQ(host.c_str(), "0.0.1.0");
}

HWTEST_F(NativeEngineTest, testUrlutilities020, testing::ext::TestSize.Level0)
{
    std::string input = "abcdefghi";
    std::string input01 = "1";
    std::string input02 = "2";
    std::vector<std::string> nums;
    nums.push_back(input);
    nums.push_back(input01);
    nums.push_back(input02);
    std::string host;
    std::bitset<static_cast<size_t>(OHOS::Url::BitsetStatusFlag::BIT_STATUS_11)> flags;
    OHOS::Url::FormatIpv4(nums, host, flags);
    ASSERT_EQ(nums.size(), 3);
}

HWTEST_F(NativeEngineTest, testUrlutilities021, testing::ext::TestSize.Level0)
{
    std::string input = "155.155.256.111111111";
    std::string host;
    std::bitset<static_cast<size_t>(OHOS::Url::BitsetStatusFlag::BIT_STATUS_11)> flags;
    OHOS::Url::AnalyseIPv4(input, host, flags);
    ASSERT_STREQ(input.c_str(), "155.155.256.111111111");
}

HWTEST_F(NativeEngineTest, testUrlutilities022, testing::ext::TestSize.Level0)
{
    std::string input = "http://example.com/path/.%2E/to%2eresource";
    OHOS::Url::UrlData urlinfo;
    std::bitset<static_cast<size_t>(OHOS::Url::BitsetStatusFlag::BIT_STATUS_11)> flags;
    OHOS::Url::AnalysisFilePath(input, urlinfo, flags);
    ASSERT_STREQ(input.c_str(), "to%2eresource");
}

HWTEST_F(NativeEngineTest, testUrlutilities023, testing::ext::TestSize.Level0)
{
    std::string input = "http://example.com/path/to%2eresource/%2E";
    OHOS::Url::UrlData urlinfo;
    std::bitset<static_cast<size_t>(OHOS::Url::BitsetStatusFlag::BIT_STATUS_11)> flags;
    OHOS::Url::AnalysisFilePath(input, urlinfo, flags);
    ASSERT_STREQ(input.c_str(), "%2E");
}

HWTEST_F(NativeEngineTest, testUrlutilities024, testing::ext::TestSize.Level0)
{
    std::string input = ".%2E";
    OHOS::Url::UrlData urlinfo;
    std::string str = "h:";
    urlinfo.path.push_back(str);
    std::bitset<static_cast<size_t>(OHOS::Url::BitsetStatusFlag::BIT_STATUS_11)> flags;
    OHOS::Url::AnalysisFilePath(input, urlinfo, flags);
    ASSERT_STREQ(input.c_str(), ".%2E");
}

HWTEST_F(NativeEngineTest, testUrlutilities025, testing::ext::TestSize.Level0)
{
    std::string input = ".%2E";
    OHOS::Url::UrlData urlinfo;
    std::string str = "w:";
    urlinfo.path.push_back(str);
    std::bitset<static_cast<size_t>(OHOS::Url::BitsetStatusFlag::BIT_STATUS_11)> flags;
    OHOS::Url::AnalysisFilePath(input, urlinfo, flags);
    ASSERT_STREQ(input.c_str(), ".%2E");
}

HWTEST_F(NativeEngineTest, testUrlutilities026, testing::ext::TestSize.Level0)
{
    std::string input = ".%2E";
    OHOS::Url::UrlData urlinfo;
    std::string str = "z:";
    urlinfo.path.push_back(str);
    std::bitset<static_cast<size_t>(OHOS::Url::BitsetStatusFlag::BIT_STATUS_11)> flags;
    OHOS::Url::AnalysisFilePath(input, urlinfo, flags);
    ASSERT_STREQ(input.c_str(), ".%2E");
}

HWTEST_F(NativeEngineTest, testUrlutilities027, testing::ext::TestSize.Level0)
{
    std::string temp = "w:51";
    size_t pos = 2;
    OHOS::Url::UrlData urlinfo;
    std::bitset<static_cast<size_t>(OHOS::Url::BitsetStatusFlag::BIT_STATUS_11)> flags;
    OHOS::Url::AnalysisSpecialFile(temp, pos, urlinfo, flags);
    ASSERT_STREQ(temp.c_str(), "w:51");
}

HWTEST_F(NativeEngineTest, testUrlutilities028, testing::ext::TestSize.Level0)
{
    std::string input = "/g";;
    OHOS::Url::UrlData urlinfo;
    std::bitset<static_cast<size_t>(OHOS::Url::BitsetStatusFlag::BIT_STATUS_11)> flags;
    OHOS::Url::AnalysisFile(input, urlinfo, flags);
    ASSERT_STREQ(input.c_str(), "g");
}

HWTEST_F(NativeEngineTest, testUrlutilities029, testing::ext::TestSize.Level0)
{
    std::bitset<static_cast<size_t>(OHOS::Url::BitsetStatusFlag::BIT_STATUS_11)> flags;
    flags.set(static_cast<size_t>(OHOS::Url::BitsetStatusFlag::BIT0));
    std::string input = "//abc";
    OHOS::Url::UrlData urlinfo;
    OHOS::Url::AnalysisFile(input, urlinfo, flags);
    ASSERT_STREQ(input.c_str(), "//abc");
}

HWTEST_F(NativeEngineTest, testUrlutilities030, testing::ext::TestSize.Level0)
{
    std::bitset<static_cast<size_t>(OHOS::Url::BitsetStatusFlag::BIT_STATUS_11)> flags;
    flags.set(static_cast<size_t>(OHOS::Url::BitsetStatusFlag::BIT0));
    std::string input = "/abc";
    OHOS::Url::UrlData urlinfo;
    OHOS::Url::AnalyInfoPath(flags, urlinfo, input);
    ASSERT_STREQ(input.c_str(), "/abc");
}

HWTEST_F(NativeEngineTest, testUrlutilities031, testing::ext::TestSize.Level0)
{
    std::bitset<static_cast<size_t>(OHOS::Url::BitsetStatusFlag::BIT_STATUS_11)> flags;
    flags.set(static_cast<size_t>(OHOS::Url::BitsetStatusFlag::BIT0));
    std::string input = ":h";
    OHOS::Url::UrlData urlinfo;
    OHOS::Url::AnalyHostPath(input, flags, urlinfo);
    ASSERT_TRUE(flags[0]);
}

HWTEST_F(NativeEngineTest, testUrlutilities032, testing::ext::TestSize.Level0)
{
    std::bitset<static_cast<size_t>(OHOS::Url::BitsetStatusFlag::BIT_STATUS_11)> flags;
    flags.set(static_cast<size_t>(OHOS::Url::BitsetStatusFlag::BIT0));
    std::string input = "///";
    OHOS::Url::UrlData urlinfo;
    OHOS::Url::AnalysisNoDefaultProtocol(input, urlinfo, flags);
    ASSERT_STREQ(input.c_str(), "///");
}

HWTEST_F(NativeEngineTest, testUrlutilities033, testing::ext::TestSize.Level0)
{
    std::bitset<static_cast<size_t>(OHOS::Url::BitsetStatusFlag::BIT_STATUS_11)> flags;
    flags.set(static_cast<size_t>(OHOS::Url::BitsetStatusFlag::BIT0));
    std::string input = "//";
    OHOS::Url::UrlData urlinfo;
    OHOS::Url::AnalysisNoDefaultProtocol(input, urlinfo, flags);
    ASSERT_STREQ(input.c_str(), "//");
}

HWTEST_F(NativeEngineTest, testUrlutilities034, testing::ext::TestSize.Level0)
{
    std::bitset<static_cast<size_t>(OHOS::Url::BitsetStatusFlag::BIT_STATUS_11)> flags;
    flags.set(static_cast<size_t>(OHOS::Url::BitsetStatusFlag::BIT0));
    std::string input = "//::/";
    OHOS::Url::UrlData urlinfo;
    OHOS::Url::AnalysisNoDefaultProtocol(input, urlinfo, flags);
    ASSERT_STREQ(input.c_str(), "//::/");
}

HWTEST_F(NativeEngineTest, testUrlutilities035, testing::ext::TestSize.Level0)
{
    std::string input = "abc:d:e";
    OHOS::Url::UrlData urlinfo;
    std::bitset<static_cast<size_t>(OHOS::Url::BitsetStatusFlag::BIT_STATUS_11)> flags;
    flags.set(static_cast<size_t>(OHOS::Url::BitsetStatusFlag::BIT0));
    size_t pos = 0;
    OHOS::Url::AnalysisOnlyHost(input, urlinfo, flags, pos);
    ASSERT_STREQ(input.c_str(), "abc:d:e");
}

HWTEST_F(NativeEngineTest, testUrlutilities036, testing::ext::TestSize.Level0)
{
    std::string input = "abc:d:e/fg";
    OHOS::Url::UrlData urlinfo;
    std::bitset<static_cast<size_t>(OHOS::Url::BitsetStatusFlag::BIT_STATUS_11)> flags;
    flags.set(static_cast<size_t>(OHOS::Url::BitsetStatusFlag::BIT0));
    size_t pos = 6; // 6:string position
    OHOS::Url::ParsingHostAndPath(input, urlinfo, pos, flags);
    ASSERT_STREQ(input.c_str(), "abc:d:e/fg");
}

HWTEST_F(NativeEngineTest, testUrlutilities037, testing::ext::TestSize.Level0)
{
    OHOS::Url::UrlData urlData;
    OHOS::Url::UrlData baseData;
    baseData.path.push_back("");
    urlData.path.push_back("");
    bool isFile = false;
    OHOS::Url::ShorteningPath(urlData, baseData, isFile);
    ASSERT_FALSE(isFile);
}

HWTEST_F(NativeEngineTest, testUrlutilities038, testing::ext::TestSize.Level0)
{
    OHOS::Url::UrlData urlData;
    OHOS::Url::UrlData baseData;
    baseData.path.push_back("a:");
    urlData.path.push_back("a");
    bool isFile = true;
    OHOS::Url::ShorteningPath(urlData, baseData, isFile);
    ASSERT_TRUE(isFile);
}

HWTEST_F(NativeEngineTest, testUrlutilities040, testing::ext::TestSize.Level0)
{
    std::string input = ".%2E";
    std::vector<std::string> path;
    std::bitset<static_cast<size_t>(OHOS::Url::BitsetStatusFlag::BIT_STATUS_11)> flags;
    bool isSpecial = true;
    OHOS::Url::AnalysisPath(input, path, flags, isSpecial);
    ASSERT_STREQ(input.c_str(), ".%2E");
}

HWTEST_F(NativeEngineTest, testUrlutilities041, testing::ext::TestSize.Level0)
{
    std::string input = "%2E";
    std::vector<std::string> path;
    std::bitset<static_cast<size_t>(OHOS::Url::BitsetStatusFlag::BIT_STATUS_11)> flags;
    bool isSpecial = true;
    OHOS::Url::AnalysisPath(input, path, flags, isSpecial);
    ASSERT_STREQ(input.c_str(), "%2E");
}

HWTEST_F(NativeEngineTest, testUrlutilities042, testing::ext::TestSize.Level0)
{
    std::string input = "99::1080:800:200C:417A";
    OHOS::Url::FormatIpv6(input);
    ASSERT_STREQ(input.c_str(), "99:0:0:0:1080:800:200C:417A");
}

HWTEST_F(NativeEngineTest, testUrlutilities043, testing::ext::TestSize.Level0)
{
    std::string input = "000:1080:800:200C:417A";
    std::vector<std::string> ipv6;
    ipv6.push_back(input);
    OHOS::Url::RemoveLeadingZeros(ipv6);
    ASSERT_STREQ(input.c_str(), "000:1080:800:200C:417A");
}

HWTEST_F(NativeEngineTest, testUrlutilities044, testing::ext::TestSize.Level0)
{
    std::string input = "::ffff:192.168.1.1";
    std::string host = "";
    std::bitset<static_cast<size_t>(OHOS::Url::BitsetStatusFlag::BIT_STATUS_11)> flags;
    OHOS::Url::IPv6Host(input, host, flags);
    ASSERT_STREQ(input.c_str(), "::ffff:c0a8:101");
}

HWTEST_F(NativeEngineTest, testUrlutilities045, testing::ext::TestSize.Level0)
{
    std::string input = "w";
    OHOS::Url::UrlData urlinfo;
    std::bitset<static_cast<size_t>(OHOS::Url::BitsetStatusFlag::BIT_STATUS_11)> flags;
    OHOS::Url::AnalysisNoDefaultProtocol(input, urlinfo, flags);
    ASSERT_STREQ(input.c_str(), "w");
}

HWTEST_F(NativeEngineTest, testUrlutilities046, testing::ext::TestSize.Level0)
{
    std::string input = "ww:w]wa:b]c/www";
    OHOS::Url::UrlData urlinfo;
    size_t pos;
    std::bitset<static_cast<size_t>(OHOS::Url::BitsetStatusFlag::BIT_STATUS_11)> flags;
    flags.set(static_cast<size_t>(OHOS::Url::BitsetStatusFlag::BIT0));
    OHOS::Url::ParsingHostAndPath(input, urlinfo, pos, flags);
    ASSERT_STREQ(input.c_str(), "ww:w]wa:b]c/www");
}

HWTEST_F(NativeEngineTest, testUrlutilities047, testing::ext::TestSize.Level0)
{
    OHOS::Url::UrlData urlData;
    std::string s1 = "www";
    std::string s2 = "aaa";
    std::string s3 = "bbb";
    std::string s4 = "ccc";
    urlData.path.push_back(s1);
    urlData.path.push_back(s2);
    urlData.path.push_back(s3);
    urlData.path.push_back(s4);
    std::string rst = OHOS::Url::BasePathToStr(urlData);
    ASSERT_STREQ(rst.c_str(), "www/aaa/bbb/ccc");
}

HWTEST_F(NativeEngineTest, testUrlutilities048, testing::ext::TestSize.Level0)
{
    std::vector<std::string> ipv6;
    std::string str = "00ccccc";
    ipv6.push_back(str);
    OHOS::Url::RemoveLeadingZeros(ipv6);
    ASSERT_STREQ(str.c_str(), "00ccccc");
}

HWTEST_F(NativeEngineTest, testUrlutilities049, testing::ext::TestSize.Level0)
{
    std::string input = "//@/aabab";
    OHOS::Url::UrlData urlinfo;
    std::bitset<static_cast<size_t>(OHOS::Url::BitsetStatusFlag::BIT_STATUS_11)> flags;
    OHOS::Url::AnalysisNoDefaultProtocol(input, urlinfo, flags);
    ASSERT_STREQ(input.c_str(), "//@/aabab");
}

HWTEST_F(NativeEngineTest, testUrlutilities050, testing::ext::TestSize.Level0)
{
    std::string input = "99::1080:8:800:200C:417A";
    OHOS::Url::FormatIpv6(input);
    ASSERT_STREQ(input.c_str(), "99:0:0:1080:8:800:200C:417A");
}

HWTEST_F(NativeEngineTest, testUrlHelper001, testing::ext::TestSize.Level0)
{
    std::string input = "abc~!@#$|[]{}`^%()_+-=";
    std::string urlEncode = OHOS::Url::EncodePercentEncoding(input, OHOS::Url::URL_ENCODED_PERCENT_SIGN_CHARS);
    ASSERT_STREQ(urlEncode.c_str(), "abc~!@#$%7C%5B%5D%7B%7D%60%5E%25()_+-=");

    std::string userinfoEncode = OHOS::Url::EncodePercentEncoding(input, OHOS::Url::USERINFO_PERCENT_SIGN_CHARS);
    ASSERT_STREQ(userinfoEncode.c_str(), "abc~!@#$%7C%5B%5D%7B%7D%60%5E%()_+-=");

    std::string pathEncode = OHOS::Url::EncodePercentEncoding(input, OHOS::Url::PATH_PERCENT_SIGN_CHARS);
    ASSERT_STREQ(pathEncode.c_str(), "abc~!@#$|[]%7B%7D%60%5E%()_+-=");

    std::string fragmentEncode = OHOS::Url::EncodePercentEncoding(input, OHOS::Url::FRAGMENT_PERCENT_SIGN_CHARS);
    ASSERT_STREQ(fragmentEncode.c_str(), "abc~!@#$|[]{}`%5E%()_+-=");

    std::string queryEncode = OHOS::Url::EncodePercentEncoding(input, OHOS::Url::QUERY_PERCENT_SIGN_CHARS);
    ASSERT_STREQ(queryEncode.c_str(), "abc~!@#$|[]{}`^%()_+-=");

}

HWTEST_F(NativeEngineTest, testUrlHelper002, testing::ext::TestSize.Level0)
{
    std::string input = "";
    for (uint8_t i = 0; i <= 0x7F; i++) {
        input += static_cast<char>(i);
    }
    std::string out =
        "%00%01%02%03%04%05%06%07%08%09%0A%0B%0C%0D%0E%0F%10%11%12%13%14%15%16%17%18%19%1A%1B%1C%1D"
        "%1E%1F%20!%22#$%25&'()*+,-./0123456789:;%3C=%3E?@ABCDEFGHIJKLMNOPQRSTUVWXYZ%5B%5C%5D%5E_%60abcdefghijklm"
        "nopqrstuvwxyz%7B%7C%7D~%7F";
    // username, password {'%25':'%'}
    std::string outUserInfo =
        "%00%01%02%03%04%05%06%07%08%09%0A%0B%0C%0D%0E%0F%10%11%12%13%14%15%16%17%18%19%1A%1B%1C%1D"
        "%1E%1F%20!%22#$%&'()*+,-./0123456789:;%3C=%3E?@ABCDEFGHIJKLMNOPQRSTUVWXYZ%5B%5C%5D%5E_%60abcdefghijklm"
        "nopqrstuvwxyz%7B%7C%7D~%7F";
    // {'%7C': '|', '%5B': '[', '%5D': ']', '%25': '%'}
    std::string outPath =
        "%00%01%02%03%04%05%06%07%08%09%0A%0B%0C%0D%0E%0F%10%11%12%13%14%15%16%17%18%19%1A%1B%1C%1D"
        "%1E%1F%20!%22#$%&'()*+,-./0123456789:;%3C=%3E?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[%5C]%5E_%60abcdefghijklm"
        "nopqrstuvwxyz%7B|%7D~%7F";
    // hash, fragment {'%7C': '|', '%5B': '[', '%5D': ']', '%7B': '{', '%7D': '}', '%60': '`', '%25': '%'}
    std::string outFragment =
        "%00%01%02%03%04%05%06%07%08%09%0A%0B%0C%0D%0E%0F%10%11%12%13%14%15%16%17%18%19%1A%1B%1C%1D"
        "%1E%1F%20!%22#$%&'()*+,-./0123456789:;%3C=%3E?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[%5C]%5E_`abcdefghijklm"
        "nopqrstuvwxyz{|}~%7F";
    // query {'%7C': '|', '%5B': '[', '%5D': ']', '%7B': '{', '%7D': '}', '%60': '`', '%5E': '^', '%25': '%'}
    std::string outQuery =
        "%00%01%02%03%04%05%06%07%08%09%0A%0B%0C%0D%0E%0F%10%11%12%13%14%15%16%17%18%19%1A%1B%1C%1D"
        "%1E%1F%20!%22#$%&'()*+,-./0123456789:;%3C=%3E?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[%5C]^_`abcdefghijklm"
        "nopqrstuvwxyz{|}~%7F";

    std::string urlEncode = OHOS::Url::EncodePercentEncoding(input, OHOS::Url::URL_ENCODED_PERCENT_SIGN_CHARS);
    ASSERT_STREQ(urlEncode.c_str(), out.c_str());
    ASSERT_STREQ(
        OHOS::Url::EncodePercentEncoding(input, OHOS::Url::USERINFO_PERCENT_SIGN_CHARS).c_str(), outUserInfo.c_str());
    ASSERT_STREQ(OHOS::Url::EncodePercentEncoding(input, OHOS::Url::PATH_PERCENT_SIGN_CHARS).c_str(), outPath.c_str());
    ASSERT_STREQ(
        OHOS::Url::EncodePercentEncoding(input, OHOS::Url::FRAGMENT_PERCENT_SIGN_CHARS).c_str(), outFragment.c_str());
    ASSERT_STREQ(
        OHOS::Url::EncodePercentEncoding(input, OHOS::Url::QUERY_PERCENT_SIGN_CHARS).c_str(), outQuery.c_str());
}

HWTEST_F(NativeEngineTest, testUrlHelper003, testing::ext::TestSize.Level0)
{
    std::string input = "";
    char chars[] = { 0x1, 0x7, 0x8, 0xF, 0x25, 0x2F, 0x5B, 0x5D, 0x5E, 0x60, 0x6C, 0x7B, 0x7C, 0x7D, 0x7F, 0x8F, 0xC1,
        0xF1, 0xFF };
    for (uint8_t i = 0; i < sizeof(chars) / sizeof(char); i++) {
        input += chars[i];
    }
    std::string out = "%01%07%08%0F%25/%5B%5D%5E%60l%7B%7C%7D%7F%8F%C1%F1%FF";
    // username, password {'%25':'%'}
    std::string outUserInfo = "%01%07%08%0F%/%5B%5D%5E%60l%7B%7C%7D%7F%8F%C1%F1%FF";
    // {'%7C': '|', '%5B': '[', '%5D': ']', '%25': '%'}
    std::string outPath = "%01%07%08%0F%/[]%5E%60l%7B|%7D%7F%8F%C1%F1%FF";
    // hash, fragment {'%7C': '|', '%5B': '[', '%5D': ']', '%7B': '{', '%7D': '}', '%60': '`', '%25': '%'}
    std::string outFragment = "%01%07%08%0F%/[]%5E`l{|}%7F%8F%C1%F1%FF";
    // query {'%7C': '|', '%5B': '[', '%5D': ']', '%7B': '{', '%7D': '}', '%60': '`', '%5E': '^', '%25': '%'}
    std::string outQuery = "%01%07%08%0F%/[]^`l{|}%7F%8F%C1%F1%FF";

    std::string urlEncode = OHOS::Url::EncodePercentEncoding(input, OHOS::Url::URL_ENCODED_PERCENT_SIGN_CHARS);
    ASSERT_STREQ(urlEncode.c_str(), out.c_str());
    ASSERT_STREQ(
        OHOS::Url::EncodePercentEncoding(input, OHOS::Url::USERINFO_PERCENT_SIGN_CHARS).c_str(), outUserInfo.c_str());
    ASSERT_STREQ(OHOS::Url::EncodePercentEncoding(input, OHOS::Url::PATH_PERCENT_SIGN_CHARS).c_str(), outPath.c_str());
    ASSERT_STREQ(
        OHOS::Url::EncodePercentEncoding(input, OHOS::Url::FRAGMENT_PERCENT_SIGN_CHARS).c_str(), outFragment.c_str());
    ASSERT_STREQ(
        OHOS::Url::EncodePercentEncoding(input, OHOS::Url::QUERY_PERCENT_SIGN_CHARS).c_str(), outQuery.c_str());
}

HWTEST_F(NativeEngineTest, testUrlHelper004, testing::ext::TestSize.Level0)
{
    std::string input = "";
    for (uint8_t i = 0x80; i != 0x00; i++) {
        input += static_cast<char>(i);
    }
    std::string out =
        "%80%81%82%83%84%85%86%87%88%89%8A%8B%8C%8D%8E%8F%90%91%92%93%94%95%96%97%98%99%9A%9B%9C%9D%9E%9F"
        "%A0%A1%A2%A3%A4%A5%A6%A7%A8%A9%AA%AB%AC%AD%AE%AF%B0%B1%B2%B3%B4%B5%B6%B7%B8%B9%BA%BB%BC%BD%BE%BF"
        "%C0%C1%C2%C3%C4%C5%C6%C7%C8%C9%CA%CB%CC%CD%CE%CF%D0%D1%D2%D3%D4%D5%D6%D7%D8%D9%DA%DB%DC%DD%DE%DF"
        "%E0%E1%E2%E3%E4%E5%E6%E7%E8%E9%EA%EB%EC%ED%EE%EF%F0%F1%F2%F3%F4%F5%F6%F7%F8%F9%FA%FB%FC%FD%FE%FF";

    std::string urlEncode = OHOS::Url::EncodePercentEncoding(input, OHOS::Url::URL_ENCODED_PERCENT_SIGN_CHARS);
    ASSERT_STREQ(urlEncode.c_str(), out.c_str());
    ASSERT_STREQ(OHOS::Url::EncodePercentEncoding(input, OHOS::Url::USERINFO_PERCENT_SIGN_CHARS).c_str(), out.c_str());
    ASSERT_STREQ(OHOS::Url::EncodePercentEncoding(input, OHOS::Url::PATH_PERCENT_SIGN_CHARS).c_str(), out.c_str());
    ASSERT_STREQ(OHOS::Url::EncodePercentEncoding(input, OHOS::Url::FRAGMENT_PERCENT_SIGN_CHARS).c_str(), out.c_str());
    ASSERT_STREQ(OHOS::Url::EncodePercentEncoding(input, OHOS::Url::QUERY_PERCENT_SIGN_CHARS).c_str(), out.c_str());
}

HWTEST_F(NativeEngineTest, testUrlHelper005, testing::ext::TestSize.Level0)
{
    std::string input = "abc~!@#$%7C%5B%5D%7B%7D%60%5E%()_+-=%7c";
    std::string decodeResult = OHOS::Url::DecodePercentEncoding(input);
    ASSERT_STREQ(decodeResult.c_str(), "abc~!@#$|[]{}`^%()_+-=|");
    std::string inputTwo = "abc~!@#$|[]{}`^()_+-=";
    std::string decodeResultTwo = OHOS::Url::DecodePercentEncoding(inputTwo);
    ASSERT_STREQ(decodeResultTwo.c_str(), "abc~!@#$|[]{}`^()_+-=");
}

HWTEST_F(NativeEngineTest, testUrlHelper006, testing::ext::TestSize.Level0)
{
    std::string input = "?foo=1&index=0&url=https%3A%2F%2Fssse?iz=67594&from=article.detail%26x-expires%3D17&ff=&cc";
    std::vector<OHOS::Url::KeyValue> params {};
    OHOS::Url::StringAnalyzing(input, params);
    ASSERT_EQ(params.size(), 6);
    ASSERT_STREQ(params[0].first.c_str(), "foo");
    ASSERT_STREQ(params[0].second.c_str(), "1");
    ASSERT_STREQ(params[1].first.c_str(), "index");
    ASSERT_STREQ(params[1].second.c_str(), "0");
    ASSERT_STREQ(params[2].first.c_str(), "url");
    ASSERT_STREQ(params[2].second.c_str(), "https://ssse?iz=67594");
    ASSERT_STREQ(params[3].first.c_str(), "from");
    ASSERT_STREQ(params[3].second.c_str(), "article.detail&x-expires=17");
    ASSERT_STREQ(params[4].first.c_str(), "ff");
    ASSERT_STREQ(params[4].second.c_str(), "");
    ASSERT_STREQ(params[5].first.c_str(), "cc");
    ASSERT_STREQ(params[5].second.c_str(), "");
}

HWTEST_F(NativeEngineTest, testUrlHelper007, testing::ext::TestSize.Level0)
{
    std::string input = "?bar=2&&&&&foo~!@$%^&*()_+=test+@$%";
    std::vector<OHOS::Url::KeyValue> params {};
    OHOS::Url::StringAnalyzing(input, params);
    ASSERT_EQ(params.size(), 7);
    ASSERT_STREQ(params[0].first.c_str(), "bar");
    ASSERT_STREQ(params[0].second.c_str(), "2");
    ASSERT_STREQ(params[1].first.c_str(), "");
    ASSERT_STREQ(params[1].second.c_str(), "");
    ASSERT_STREQ(params[2].first.c_str(), "");
    ASSERT_STREQ(params[2].second.c_str(), "");
    ASSERT_STREQ(params[3].first.c_str(), "");
    ASSERT_STREQ(params[3].second.c_str(), "");
    ASSERT_STREQ(params[4].first.c_str(), "");
    ASSERT_STREQ(params[4].second.c_str(), "");
    ASSERT_STREQ(params[5].first.c_str(), "foo~!@$%^");
    ASSERT_STREQ(params[5].second.c_str(), "");
    ASSERT_STREQ(params[6].first.c_str(), "*()_ ");
    ASSERT_STREQ(params[6].second.c_str(), "test @$%");
}

// 0xxx xxxx 0x00 -0x7F decode, 0x80-0xFF not decode
HWTEST_F(NativeEngineTest, testUrlHelper008, testing::ext::TestSize.Level0)
{
    for (int i = 0x00; i <= 0xFF; i++) {
        auto tem = DecimalToPercentHexString(i);
        //%xx,  char 0xxx xxxx
        if (i <= 0x7F) {
            ASSERT_STRNE(OHOS::Url::DecodePercentEncoding(tem).c_str(), tem.c_str());
        } else {
            ASSERT_STREQ(OHOS::Url::DecodePercentEncoding(tem).c_str(), tem.c_str());
        }
    }
}

// 10xx xxxx xxxx xxxx , 0x8000-0xFFFF ,
// 0xc080-0xc0BF, 0xDF80-0xDFBF decode
HWTEST_F(NativeEngineTest, testUrlHelper009, testing::ext::TestSize.Level1)
{
    for (int i = 0x80; i <= 0xFF; i++) {
        auto tem = DecimalToPercentHexString(i);
        for (int j = 0x00; j <= 0xFF; j++) {
            auto tem1 = tem + DecimalToPercentHexString(j);
            //%xx%xx, first char 110X XXXX - 1101 1111;//second char 10XX XXXX - 1011 1111
            if ((i >= 0xc0 && i <= 0xDF) && (j >= 0x80 && j <= 0xBF)) {
                // mean can be decode, 2 length
                ASSERT_EQ(OHOS::Url::DecodePercentEncoding(tem1).length(), 2);
            } else {
                // only part decode or not, 2 length
                ASSERT_NE(OHOS::Url::DecodePercentEncoding(tem1).length(), 2);
            }
        }
    }
}

// 111x xxxx xxxx xxxx xxxx xxxx, 0xE00000-0xFFFF00
HWTEST_F(NativeEngineTest, testUrlHelper010, testing::ext::TestSize.Level1)
{
    for (int i = 0xE00000; i <= 0xFF0000; i++) {
        // 16 two byte, 8 one byte,get char from all byte
        auto tem3 = DecimalToPercentHexString((i >> 16)) + DecimalToPercentHexString((i >> 8) & 0xFF) +
                    DecimalToPercentHexString(i & 0xFF);
        // std::cout<<"beigin"<<tem3.c_str()<<std::endl;
        //%xx%xx%xx,first char 1110 XXXX - 1110 1111; 0xE0-0xEF
        // second char 10XX XXXX - 1011 1111, third char 10XX XXXX - 1011 1111;0x80-0xBF
        if (((i & 0xFF0000) >= 0xE00000 && (i & 0xFF0000) <= 0xEF0000) &&
            ((i & 0x00FF00) >= 0x008000 && (i & 0x00FF00) <= 0x00BF00) &&
            ((i & 0x0000FF) >= 0x000080 && (i & 0x0000FF) <= 0x0000BF)) {
            // 3 legnth
            ASSERT_EQ(OHOS::Url::DecodePercentEncoding(tem3).length(), 3);

        } else {
            // 3 legnth
            ASSERT_NE(OHOS::Url::DecodePercentEncoding(tem3).length(), 3);
        }
    }
}

// 1111 0XXX xxxx xxxx xxxx xxxx xxxx xxxx
//  0xF0-0xF7,
HWTEST_F(NativeEngineTest, testUrlHelper011, testing::ext::TestSize.Level1)
{
    // pick up part,for 0x00-0xFF to large, will run to long time
    int last[] = { 0x00, 0x1F, 0x20, 0x2F, 0x7F, 0x80, 0x81, 0xBF, 0xFF };
    for (int i = 0xF00000; i <= 0xFF0000; i++) {
        auto tem = DecimalToPercentHexString((i >> 16)) + DecimalToPercentHexString((i >> 8) & 0xFF) +
                   DecimalToPercentHexString(i & 0xFF);
        for (int k1 = 0; k1 <= sizeof(last) / sizeof(int); k1++) {
            auto tem4 = tem + DecimalToPercentHexString(last[k1]);
            //%xx%xx%xx%xx,first char 1111 0XXX - 1111 0111;0xF0-0xF7
            // second char 10XX XXXX - 1011 1111;0x80-0xBF
            // third char 10XX XXXX - 1011 1111;0x80-0xBF
            // fourth char 10XX XXXX - 1011 1111;0x80-0xBF
            if (((i & 0xFF0000) >= 0xF00000 && (i & 0xFF0000) <= 0xF70000) &&
                ((i & 0x00FF00) >= 0x008000 && (i & 0x00FF00) <= 0x00BF00) &&
                ((i & 0x0000FF) >= 0x000080 && (i & 0x0000FF) <= 0x0000BF) && (last[k1] >= 0x80 && last[k1] <= 0xBF)) {
                ASSERT_EQ(OHOS::Url::DecodePercentEncoding(tem4).length(), 4);
            } else {
                ASSERT_NE(OHOS::Url::DecodePercentEncoding(tem4).length(), 4);
            }
        }
    }
}

HWTEST_F(NativeEngineTest, testUrlHostname004, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://example.com/path");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetHostname(env), output);
    ASSERT_STREQ(output.c_str(), "example.com");
    url.SetHostname("newhost.org");
    DealNapiStrValue(env, url.GetHostname(env), output);
    ASSERT_STREQ(output.c_str(), "newhost.org");
}

HWTEST_F(NativeEngineTest, testUrlHostname005, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://[2001:db8::1]/file");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetHostname(env), output);
    ASSERT_STREQ(output.c_str(), "[2001:db8::1]");
    url.SetHostname("new.ipv6.host");
    DealNapiStrValue(env, url.GetHostname(env), output);
    ASSERT_STREQ(output.c_str(), "new.ipv6.host");
}

HWTEST_F(NativeEngineTest, testUrlSearch004, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://example.com?q=test");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetSearch(env), output);
    ASSERT_STREQ(output.c_str(), "?q=test");
    url.SetSearch("filter=active");
    DealNapiStrValue(env, url.GetSearch(env), output);
    ASSERT_STREQ(output.c_str(), "?filter=active");
}

HWTEST_F(NativeEngineTest, testUrlSearch005, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("https://example.com/#section");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetSearch(env), output);
    ASSERT_STREQ(output.c_str(), "");
    url.SetSearch("debug=1&cache=false");
    DealNapiStrValue(env, url.GetSearch(env), output);
    ASSERT_STREQ(output.c_str(), "?debug=1&cache=false");
}

HWTEST_F(NativeEngineTest, testUrlUsername004, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("ftp://john@example.com/file");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetUsername(env), output);
    ASSERT_STREQ(output.c_str(), "john");
    url.SetUsername("admin_user");
    DealNapiStrValue(env, url.GetUsername(env), output);
    ASSERT_STREQ(output.c_str(), "admin_user");
}

HWTEST_F(NativeEngineTest, testUrlUsername005, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("https://example.com/resource");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetUsername(env), output);
    ASSERT_STREQ(output.c_str(), "");
    url.SetUsername("special#user@name");
    DealNapiStrValue(env, url.GetUsername(env), output);
    ASSERT_STREQ(output.c_str(), "special%23user%40name");
}

HWTEST_F(NativeEngineTest, testUrlPassword004, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("https://user:pass123@example.com");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetPassword(env), output);
    ASSERT_STREQ(output.c_str(), "pass123");
    url.SetPassword("new@secure!pwd");
    DealNapiStrValue(env, url.GetPassword(env), output);
    ASSERT_STREQ(output.c_str(), "new%40secure!pwd");
}

HWTEST_F(NativeEngineTest, testUrlPassword005, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://example.com");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetPassword(env), output);
    ASSERT_STREQ(output.c_str(), "");
    url.SetPassword("pwd:with:colons");
    DealNapiStrValue(env, url.GetPassword(env), output);
    ASSERT_STREQ(output.c_str(), "pwd%3Awith%3Acolons");
}

HWTEST_F(NativeEngineTest, testUrlHost004, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://localhost:8080");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetHost(env), output);
    ASSERT_STREQ(output.c_str(), "localhost:8080");
    url.SetHost("newdomain.com:9090");
    DealNapiStrValue(env, url.GetHost(env), output);
    ASSERT_STREQ(output.c_str(), "newdomain.com:9090");
}

HWTEST_F(NativeEngineTest, testUrlHost005, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("https://[2001:db8:1::2]");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetHost(env), output);
    ASSERT_STREQ(output.c_str(), "[2001:db8:1::2]");
    url.SetHost("xn--fiqs8s:443");
    DealNapiStrValue(env, url.GetHost(env), output);
    ASSERT_STREQ(output.c_str(), "xn--fiqs8s:443");
}

HWTEST_F(NativeEngineTest, testUrlFragment001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://example.com#section1");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetFragment(env), output);
    ASSERT_STREQ(output.c_str(), "#section1");
    url.SetFragment("new_section");
    DealNapiStrValue(env, url.GetFragment(env), output);
    ASSERT_STREQ(output.c_str(), "#new_section");
}

HWTEST_F(NativeEngineTest, testUrlFragment002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("https://example.com");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetFragment(env), output);
    ASSERT_STREQ(output.c_str(), "");
    url.SetFragment("中文锚点");
    DealNapiStrValue(env, url.GetFragment(env), output);
    ASSERT_TRUE(output.find("%E4%B8%AD%E6%96%87%E9%94%9A%E7%82%B9") != std::string::npos);
}

HWTEST_F(NativeEngineTest, testUrlScheme001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("https://secure.site");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetScheme(env), output);
    ASSERT_STREQ(output.c_str(), "https:");
    url.SetScheme("ftp");
    DealNapiStrValue(env, url.GetScheme(env), output);
    ASSERT_STREQ(output.c_str(), "https:");
}

HWTEST_F(NativeEngineTest, testUrlScheme002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("custom-scheme://resource");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetScheme(env), output);
    ASSERT_STREQ(output.c_str(), "custom-scheme:");
    url.SetScheme("new-protocol");
    DealNapiStrValue(env, url.GetScheme(env), output);
    ASSERT_STREQ(output.c_str(), "new-protocol");
}

HWTEST_F(NativeEngineTest, testUrlPath001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://example.com/old/path");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetPath(env), output);
    ASSERT_STREQ(output.c_str(), "/old/path");
    url.SetPath("/new/resource");
    DealNapiStrValue(env, url.GetPath(env), output);
    ASSERT_STREQ(output.c_str(), "/new/resource");
}

HWTEST_F(NativeEngineTest, testUrlPath002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("file:///dir/file.txt");
    std::string output;
    napi_env env = (napi_env)engine_;
    DealNapiStrValue(env, url.GetPath(env), output);
    ASSERT_STREQ(output.c_str(), "/dir/file.txt");
    url.SetPath("/new/dir/../data.json");
    DealNapiStrValue(env, url.GetPath(env), output);
    ASSERT_STREQ(output.c_str(), "/new/data.json");
}

HWTEST_F(NativeEngineTest, testUrlEmptyInput001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("");
    std::string output;
    napi_env env = (napi_env)engine_;
    url.SetScheme("data");
    DealNapiStrValue(env, url.GetScheme(env), output);
    ASSERT_STREQ(output.c_str(), "data");
    DealNapiStrValue(env, url.GetHostname(env), output);
    ASSERT_STREQ(output.c_str(), "");
}

HWTEST_F(NativeEngineTest, testUrlSpecialChars001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://user:pwd@host.com");
    std::string output;
    napi_env env = (napi_env)engine_;
    
    url.SetUsername("user:name@with");
    DealNapiStrValue(env, url.GetUsername(env), output);
    ASSERT_STREQ(output.c_str(), "user%3Aname%40with");
    
    url.SetPassword("p@ss:w0rd#123");
    DealNapiStrValue(env, url.GetPassword(env), output);
    ASSERT_STREQ(output.c_str(), "p%40ss%3Aw0rd%23123");
}

HWTEST_F(NativeEngineTest, testUrlFullUpdate001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URL url("http://old.com/oldpath?q=1#old");
    std::string output;
    napi_env env = (napi_env)engine_;
    
    url.SetHostname("newhost.org");
    DealNapiStrValue(env, url.GetHostname(env), output);
    ASSERT_STREQ(output.c_str(), "newhost.org");
    
    url.SetPath("/new/path/resource");
    DealNapiStrValue(env, url.GetPath(env), output);
    ASSERT_STREQ(output.c_str(), "/new/path/resource");
    
    url.SetSearch("debug=true");
    DealNapiStrValue(env, url.GetSearch(env), output);
    ASSERT_STREQ(output.c_str(), "?debug=true");
    
    url.SetFragment("new-section");
    DealNapiStrValue(env, url.GetFragment(env), output);
    ASSERT_STREQ(output.c_str(), "#new-section");
}

HWTEST_F(NativeEngineTest, testUrlParamsAppend001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    napi_env env = (napi_env)engine_;
    napi_value key = StrToNapiValue(env, "username");
    napi_value val = StrToNapiValue(env, "john_doe");
    params.Append(env, key, val);
    
    napi_value query = StrToNapiValue(env, "username");
    std::string output;
    DealNapiStrValue(env, params.Get(env, query), output);
    ASSERT_STREQ(output.c_str(), "john_doe");
}

HWTEST_F(NativeEngineTest, testUrlParamsDelete001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    napi_env env = (napi_env)engine_;
    napi_value key = StrToNapiValue(env, "token");
    napi_value val = StrToNapiValue(env, "abc123");
    params.Append(env, key, val);
    bool value = false;
    napi_get_value_bool(env, params.IsHas(env, key), &value);
    ASSERT_TRUE(value);
    params.Delete(env, key);
    napi_get_value_bool(env, params.IsHas(env, key), &value);
    ASSERT_FALSE(value);
}

HWTEST_F(NativeEngineTest, testUrlParamsGetAll001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    napi_env env = (napi_env)engine_;
    napi_value key = StrToNapiValue(env, "color");
    
    params.Append(env, key, StrToNapiValue(env, "red"));
    params.Append(env, key, StrToNapiValue(env, "green"));
    params.Append(env, key, StrToNapiValue(env, "blue"));
    
    std::string output;
    DealNapiStrValue(env, params.Get(env, key), output);
    ASSERT_STREQ(output.c_str(), "red");
}

HWTEST_F(NativeEngineTest, testUrlParamsSet001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    napi_env env = (napi_env)engine_;
    napi_value key = StrToNapiValue(env, "page");
    
    params.Append(env, key, StrToNapiValue(env, "1"));
    params.Set(env, key, StrToNapiValue(env, "2"));
    
    std::string output;
    DealNapiStrValue(env, params.Get(env, key), output);
    ASSERT_STREQ(output.c_str(), "2");
}

HWTEST_F(NativeEngineTest, testUrlParamsSort001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    napi_env env = (napi_env)engine_;
    
    params.Append(env, StrToNapiValue(env, "b"), StrToNapiValue(env, "2"));
    params.Append(env, StrToNapiValue(env, "a"), StrToNapiValue(env, "1"));
    params.Append(env, StrToNapiValue(env, "c"), StrToNapiValue(env, "3"));
    params.Sort();
    
    std::string output;
    DealNapiStrValue(env, params.IterByKeys(env), output);
    ASSERT_STREQ(output.c_str(), "");
}

HWTEST_F(NativeEngineTest, testUrlParamsIterByKeys001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    napi_env env = (napi_env)engine_;
    
    params.Append(env, StrToNapiValue(env, "first"), StrToNapiValue(env, "1"));
    params.Append(env, StrToNapiValue(env, "second"), StrToNapiValue(env, "2"));
    
    std::string output;
    DealNapiStrValue(env, params.IterByKeys(env), output);
    ASSERT_STREQ(output.c_str(), "");
}

HWTEST_F(NativeEngineTest, testUrlParamsIterByValues001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    napi_env env = (napi_env)engine_;
    
    params.Append(env, StrToNapiValue(env, "x"), StrToNapiValue(env, "10"));
    params.Append(env, StrToNapiValue(env, "y"), StrToNapiValue(env, "20"));
    
    std::string output;
    DealNapiStrValue(env, params.IterByValues(env), output);
    ASSERT_STREQ(output.c_str(), "");
}

HWTEST_F(NativeEngineTest, testUrlParamsEmpty001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    napi_env env = (napi_env)engine_;
    napi_value key = StrToNapiValue(env, "empty");
    
    bool value = false;
    napi_get_value_bool(env, params.IsHas(env, key), &value);
    ASSERT_FALSE(value);
    
    std::string output;
    DealNapiStrValue(env, params.Get(env, key), output);
    ASSERT_STREQ(output.c_str(), "");
    
    DealNapiStrValue(env, params.GetAll(env, key), output);
    ASSERT_STREQ(output.c_str(), "");
}

HWTEST_F(NativeEngineTest, testUrlParamsSpecialChars001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    napi_env env = (napi_env)engine_;
    napi_value key = StrToNapiValue(env, "query");
    napi_value value = StrToNapiValue(env, "a=b&c=d");
    
    params.Append(env, key, value);
    
    std::string output;
    DealNapiStrValue(env, params.Get(env, key), output);
    ASSERT_STREQ(output.c_str(), "a=b&c=d");
    
    DealNapiStrValue(env, params.IterByValues(env), output);
    ASSERT_STREQ(output.c_str(), "a=b&c=d");
}

HWTEST_F(NativeEngineTest, testUrlParamsParse001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    napi_env env = (napi_env)engine_;
    std::string input1 = "key1";
    std::string input2 = "value1";
    std::string input3 = "key2";
    std::string input4 = "value2";
    std::vector<std::string> vec;
    vec.push_back(input1);
    vec.push_back(input2);
    vec.push_back(input3);
    vec.push_back(input4);
    params.SetArray(env, vec);
    std::string output;
    DealNapiStrValue(env, params.Get(env, StrToNapiValue(env, "key1")), output);
    ASSERT_STREQ(output.c_str(), "value1");
    
    DealNapiStrValue(env, params.Get(env, StrToNapiValue(env, "key2")), output);
    ASSERT_STREQ(output.c_str(), "value2");
    
    bool value = false;
    napi_get_value_bool(env, params.IsHas(env, StrToNapiValue(env, "key1")), &value);
    ASSERT_TRUE(value);
}

HWTEST_F(NativeEngineTest, testUrlParamsDelete002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    napi_env env = (napi_env)engine_;
    napi_value key = StrToNapiValue(env, "nonexistent");
    
    params.Delete(env, key);
    bool value = false;
    napi_get_value_bool(env, params.IsHas(env, key), &value);
    ASSERT_FALSE(value);
}

HWTEST_F(NativeEngineTest, testUrlParamsGetAll002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    napi_env env = (napi_env)engine_;
    napi_value key = StrToNapiValue(env, "lang");
    
    params.Append(env, key, StrToNapiValue(env, "en"));
    params.Append(env, key, StrToNapiValue(env, "zh"));
    params.Append(env, key, StrToNapiValue(env, "fr"));
    
    std::string output;
    DealNapiStrValue(env, params.GetAll(env, key), output);
    ASSERT_STREQ(output.c_str(), "");
    
    params.Set(env, key, StrToNapiValue(env, "jp"));
    DealNapiStrValue(env, params.GetAll(env, key), output);
    ASSERT_STREQ(output.c_str(), "");
}

HWTEST_F(NativeEngineTest, testUrlParamsEmptyStrings001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    napi_env env = (napi_env)engine_;
    napi_value emptyKey = StrToNapiValue(env, "");
    napi_value emptyVal = StrToNapiValue(env, "");
    
    params.Append(env, emptyKey, emptyVal);
    bool value = false;
    napi_get_value_bool(env, params.IsHas(env, emptyKey), &value);
    ASSERT_TRUE(value);
    
    std::string output;
    DealNapiStrValue(env, params.Get(env, emptyKey), output);
    ASSERT_STREQ(output.c_str(), "");
    
    DealNapiStrValue(env, params.IterByKeys(env), output);
    ASSERT_STREQ(output.c_str(), "");
}

HWTEST_F(NativeEngineTest, testUrlParamsOverride001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    napi_env env = (napi_env)engine_;
    napi_value key = StrToNapiValue(env, "version");
    
    params.Append(env, key, StrToNapiValue(env, "1.0"));
    params.Append(env, key, StrToNapiValue(env, "2.0"));
    params.Set(env, key, StrToNapiValue(env, "3.0"));
    
    std::string output;
    DealNapiStrValue(env, params.Get(env, key), output);
    ASSERT_STREQ(output.c_str(), "3.0");
    
    DealNapiStrValue(env, params.GetAll(env, key), output);
    ASSERT_STREQ(output.c_str(), "3.0");
}

HWTEST_F(NativeEngineTest, testUrlParamsSort002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    napi_env env = (napi_env)engine_;
    
    params.Append(env, StrToNapiValue(env, "zebra"), StrToNapiValue(env, "animal"));
    params.Append(env, StrToNapiValue(env, "apple"), StrToNapiValue(env, "fruit"));
    params.Append(env, StrToNapiValue(env, "book"), StrToNapiValue(env, "object"));
    
    params.Sort();
    
    std::string output;
    DealNapiStrValue(env, params.IterByKeys(env), output);
    ASSERT_STREQ(output.c_str(), "");
    
    DealNapiStrValue(env, params.IterByValues(env), output);
    ASSERT_STREQ(output.c_str(), "");
}

HWTEST_F(NativeEngineTest, testUrlParamsUnicode001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    napi_env env = (napi_env)engine_;
    napi_value key = StrToNapiValue(env, "城市");
    napi_value value = StrToNapiValue(env, "北京市");
    
    params.Append(env, key, value);
    
    std::string output;
    bool value1 = false;
    napi_get_value_bool(env, params.IsHas(env, key), &value1);
    ASSERT_TRUE(value1);
    
    DealNapiStrValue(env, params.Get(env, key), output);
    ASSERT_STREQ(output.c_str(), "北京市");
    
    DealNapiStrValue(env, params.IterByKeys(env), output);
    ASSERT_STREQ(output.c_str(), "北京市");
}

HWTEST_F(NativeEngineTest, testUrlParamsDelete003, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    napi_env env = (napi_env)engine_;
    std::string input1 = "product";
    std::string input2 = "phone";
    std::string input3 = "product";
    std::string input4 = "laptop";
    std::string input5 = "product";
    std::string input6 = "tablet";
    std::vector<std::string> vec;
    vec.push_back(input1);
    vec.push_back(input2);
    vec.push_back(input3);
    vec.push_back(input4);
    vec.push_back(input5);
    vec.push_back(input6);
    params.SetArray(env, vec);
    napi_value key = StrToNapiValue(env, "product");
    
    params.Delete(env, key);
    
    std::string output;
    DealNapiStrValue(env, params.GetAll(env, key), output);
    ASSERT_STREQ(output.c_str(), "");
}

HWTEST_F(NativeEngineTest, testUrlParamsParse002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    napi_env env = (napi_env)engine_;
    std::string input1 = "key1";
    std::string input2 = "1";
    std::string input3 = "two";
    std::string input4 = "laptop";
    std::string input5 = "key1";
    std::string input6 = "2";
    std::vector<std::string> vec;
    vec.push_back(input1);
    vec.push_back(input2);
    vec.push_back(input3);
    vec.push_back(input4);
    vec.push_back(input5);
    vec.push_back(input6);
    params.SetArray(env, vec);
    napi_value key1 = StrToNapiValue(env, "key1");
    
    std::string output;
    DealNapiStrValue(env, params.Get(env, key1), output);
    ASSERT_STREQ(output.c_str(), "1");
    
    DealNapiStrValue(env, params.GetAll(env, key1), output);
    ASSERT_STREQ(output.c_str(), "1");
    
    DealNapiStrValue(env, params.IterByKeys(env), output);
    ASSERT_STREQ(output.c_str(), "1");
}

HWTEST_F(NativeEngineTest, testUrlParamsSet002, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    napi_env env = (napi_env)engine_;
    napi_value key = StrToNapiValue(env, "new_key");
    
    bool value = false;
    napi_get_value_bool(env, params.IsHas(env, key), &value);
    ASSERT_FALSE(value);
    params.Set(env, key, StrToNapiValue(env, "new_value"));
    napi_get_value_bool(env, params.IsHas(env, key), &value);
    ASSERT_TRUE(value);
    
    std::string output;
    DealNapiStrValue(env, params.Get(env, key), output);
    ASSERT_STREQ(output.c_str(), "new_value");
}

HWTEST_F(NativeEngineTest, testUrlParamsComprehensive001, testing::ext::TestSize.Level0)
{
    OHOS::Url::URLSearchParams params = OHOS::Url::URLSearchParams();
    napi_env env = (napi_env)engine_;
    
    napi_value keyA = StrToNapiValue(env, "a");
    params.Append(env, keyA, StrToNapiValue(env, "1"));
    params.Append(env, keyA, StrToNapiValue(env, "2"));
    
    napi_value keyB = StrToNapiValue(env, "b");
    params.Set(env, keyB, StrToNapiValue(env, "3"));
    
    bool value = false;
    napi_get_value_bool(env, params.IsHas(env, keyA), &value);
    ASSERT_TRUE(value);
    napi_get_value_bool(env, params.IsHas(env, keyB), &value);
    ASSERT_TRUE(value);
    
    std::string output;
    DealNapiStrValue(env, params.GetAll(env, keyA), output);
    ASSERT_STREQ(output.c_str(), "");
    
    DealNapiStrValue(env, params.IterByKeys(env), output);
    ASSERT_STREQ(output.c_str(), "");
    
    DealNapiStrValue(env, params.IterByValues(env), output);
    ASSERT_STREQ(output.c_str(), "");
    
    params.Delete(env, keyA);
    napi_get_value_bool(env, params.IsHas(env, keyA), &value);
    ASSERT_FALSE(value);
    napi_get_value_bool(env, params.IsHas(env, keyB), &value);
    ASSERT_TRUE(value);
    
    params.Append(env, StrToNapiValue(env, "c"), StrToNapiValue(env, "4"));
    params.Append(env, StrToNapiValue(env, "a"), StrToNapiValue(env, "5"));
    params.Sort();
    
    DealNapiStrValue(env, params.IterByKeys(env), output);
    ASSERT_STREQ(output.c_str(), "");
}