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
#include <codecvt>
#include "commonlibrary/ets_utils/js_util_module/util/js_uuid.h"
#include "commonlibrary/ets_utils/js_util_module/util/js_textencoder.h"
#include "commonlibrary/ets_utils/js_util_module/util/js_textdecoder.h"
#include "commonlibrary/ets_utils/js_util_module/util/js_base64.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "securec.h"
#include "utils/log.h"

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

/* @tc.name: GetStringUUIDTest001
 * @tc.desc: Test Generate a random RFC 4122 version 4 UUID.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, GetStringUUIDTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string uuid = OHOS::Util::GetStringUUID(env, true);
    ASSERT_EQ(uuid.length(), 36);
}

/* @tc.name: GetStringUUIDTest002
 * @tc.desc: Test Generate a random RFC 4122 version 4 UUID.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, GetStringUUIDTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string uuid = OHOS::Util::GetStringUUID(env, false);
    ASSERT_EQ(uuid.length(), 36);
}

/* @tc.name: GetBinaryUUIDTest001
 * @tc.desc: Test Generate a random RFC 4122 version 4 UUID.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, GetBinaryUUIDTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arr = OHOS::Util::GetBinaryUUID(env, true);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: GetBinaryUUIDTest002
 * @tc.desc: Test Generate a random RFC 4122 version 4 UUID.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, GetBinaryUUIDTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arr = OHOS::Util::GetBinaryUUID(env, false);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest001
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100f9c", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest002
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    std::string input = "abc123";
    napi_create_string_utf8(env, input.c_str(), NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest003
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    std::string input = "abc123abc";
    napi_create_string_utf8(env, input.c_str(), NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: getEncodingTest001
 * @tc.desc: Test acquire encoding mode.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, getEncodingTest001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("getEncodingTest001 start");
    napi_env env = (napi_env)engine_;

    OHOS::Util::TextEncoder textEncoder("GBK");
    napi_value result = textEncoder.GetEncoding(env);

    char *buffer = nullptr;
    size_t bufferSize = 0;
    napi_get_value_string_utf8(env, result, buffer, -1, &bufferSize);
    if (bufferSize > 0) {
        buffer = new char[bufferSize + 1];
        napi_get_value_string_utf8(env, result, buffer, bufferSize + 1, &bufferSize);
    }

    ASSERT_STREQ(buffer, "GBK");
    if (buffer != nullptr) {
        delete []buffer;
        buffer = nullptr;
    }
}

/* @tc.name: getEncodingTest002
 * @tc.desc: Test acquire encoding mode.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, getEncodingTest002, testing::ext::TestSize.Level0)
{
    HILOG_INFO("getEncodingTest002 start");
    napi_env env = (napi_env)engine_;

    OHOS::Util::TextEncoder textEncoder("gb18030");
    napi_value result = textEncoder.GetEncoding(env);

    char *buffer = nullptr;
    size_t bufferSize = 0;
    napi_get_value_string_utf8(env, result, buffer, -1, &bufferSize);
    if (bufferSize > 0) {
        buffer = new char[bufferSize + 1];
        napi_get_value_string_utf8(env, result, buffer, bufferSize + 1, &bufferSize);
    }

    ASSERT_STREQ(buffer, "gb18030");
    if (buffer != nullptr) {
        delete []buffer;
        buffer = nullptr;
    }
}

/* @tc.name: getEncodingTest003
 * @tc.desc: Test acquire encoding mode.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, getEncodingTest003, testing::ext::TestSize.Level0)
{
    HILOG_INFO("getEncodingTest003 start");
    napi_env env = (napi_env)engine_;

    OHOS::Util::TextEncoder textEncoder("GB18030");
    napi_value result = textEncoder.GetEncoding(env);

    char *buffer = nullptr;
    size_t bufferSize = 0;
    napi_get_value_string_utf8(env, result, buffer, -1, &bufferSize);
    if (bufferSize > 0) {
        buffer = new char[bufferSize + 1];
        napi_get_value_string_utf8(env, result, buffer, bufferSize + 1, &bufferSize);
    }

    ASSERT_STREQ(buffer, "GB18030");
    if (buffer != nullptr) {
        delete []buffer;
        buffer = nullptr;
    }
}

/**
 * @tc.name: textEncodeTest001
 * @tc.desc: Test encode src.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textEncodeTest001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("getEncodingTest001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::TextEncoder textEncoder("utf-8");

    std::string input = "abc123";
    napi_value src = nullptr;
    napi_create_string_utf8(env, input.c_str(), input.size(), &src);
    napi_value result = textEncoder.Encode(env, src);

    char excepted[7] = {0x61, 0x62, 0x63, 0x31, 0x32, 0x33, 0};

    napi_typedarray_type type;
    size_t srcLength = 0;
    void* srcData = nullptr;
    napi_value srcBuffer = nullptr;
    size_t byteOffset = 0;

    napi_get_typedarray_info(
        env, result, &type, &srcLength, &srcData, &srcBuffer, &byteOffset);

    ASSERT_EQ(srcLength, 6);
    char* res = reinterpret_cast<char*>(srcData);

    res[srcLength] = 0;
    ASSERT_STREQ(res, excepted);
}

/**
 * @tc.name: textEncodeTest002
 * @tc.desc: Test encode src.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textEncodeTest002, testing::ext::TestSize.Level0)
{
    HILOG_INFO("getEncodingTest002 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::TextEncoder textEncoder("utf-8");

    std::string input = "";
    napi_value src = nullptr;
    napi_create_string_utf8(env, input.c_str(), input.size(), &src);
    napi_value result = textEncoder.Encode(env, src);

    napi_typedarray_type type;
    size_t srcLength = 0;
    void* srcData = nullptr;
    napi_value srcBuffer = nullptr;
    size_t byteOffset = 0;

    napi_get_typedarray_info(
        env, result, &type, &srcLength, &srcData, &srcBuffer, &byteOffset);

    ASSERT_STREQ((char*)srcData, nullptr);
}

/**
 * @tc.name: textEncodeTest003
 * @tc.desc: Test encode src.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textEncodeTest003, testing::ext::TestSize.Level0)
{
    HILOG_INFO("getEncodingTest003 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::TextEncoder textEncoder("utf-8");

    std::string input = "text";
    napi_value src = nullptr;
    napi_create_string_utf8(env, input.c_str(), input.size(), &src);
    napi_value result = textEncoder.Encode(env, src);

    char excepted[7] = {0x74, 0x65, 0x78, 0x74, 0};

    napi_typedarray_type type;
    size_t srcLength = 0;
    void* srcData = nullptr;
    napi_value srcBuffer = nullptr;
    size_t byteOffset = 0;

    napi_get_typedarray_info(
        env, result, &type, &srcLength, &srcData, &srcBuffer, &byteOffset);

    ASSERT_EQ(srcLength, 4);
    char* res = reinterpret_cast<char*>(srcData);

    res[srcLength] = 0;
    ASSERT_STREQ(res, excepted);
}

/**
 * @tc.name: textEncodeIntoTest001
 * @tc.desc: Test returns a dictionary object indicating the progress of the encoding
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textEncodeIntoTest001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textEncodeIntoTest001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::TextEncoder textEncoder("utf-8");

    std::string input = "abc123";
    napi_value src = nullptr;
    napi_create_string_utf8(env, input.c_str(), input.size(), &src);

    napi_value arrayBuffer = nullptr;
    void* arrayBufferPtr = nullptr;
    size_t arrayBufferSize = 20;
    napi_create_arraybuffer(env, arrayBufferSize, &arrayBufferPtr, &arrayBuffer);

    napi_value dest = nullptr;
        napi_create_typedarray(env, napi_int8_array, arrayBufferSize, arrayBuffer, 0, &dest);

    napi_value result = textEncoder.EncodeInto(env, src, dest);

    napi_value read = nullptr;
    napi_get_named_property(env, result, "read", &read);

    uint32_t resRead = 0;

    napi_get_value_uint32(env, read, &resRead);

    napi_value written = nullptr;
    napi_get_named_property(env, result, "written", &written);

    uint32_t resWritten = 0;
    napi_get_value_uint32(env, read, &resWritten);

    ASSERT_EQ(resRead, (uint32_t)6);
    ASSERT_EQ(resWritten, (uint32_t)6);
}

/**
 * @tc.name: textEncodeIntoTest002
 * @tc.desc: Test returns a dictionary object indicating the progress of the encoding
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textEncodeIntoTest002, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textEncodeIntoTest002 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::TextEncoder textEncoder("utf-8");

    std::string input = "text";
    napi_value src = nullptr;
    napi_create_string_utf8(env, input.c_str(), input.size(), &src);

    napi_value arrayBuffer = nullptr;
    void* arrayBufferPtr = nullptr;
    size_t arrayBufferSize = 20;
    napi_create_arraybuffer(env, arrayBufferSize, &arrayBufferPtr, &arrayBuffer);

    napi_value dest = nullptr;
        napi_create_typedarray(env, napi_int8_array, arrayBufferSize, arrayBuffer, 0, &dest);

    napi_value result = textEncoder.EncodeInto(env, src, dest);

    napi_value read = nullptr;
    napi_get_named_property(env, result, "read", &read);

    uint32_t resRead = 0;

    napi_get_value_uint32(env, read, &resRead);

    napi_value written = nullptr;
    napi_get_named_property(env, result, "written", &written);

    uint32_t resWritten = 0;
    napi_get_value_uint32(env, read, &resWritten);

    ASSERT_EQ(resRead, (uint32_t)4);
    ASSERT_EQ(resWritten, (uint32_t)4);
}

/**
 * @tc.name: textEncodeIntoTest003
 * @tc.desc: Test returns a dictionary object indicating the progress of the encoding
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textEncodeIntoTest003, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textEncodeIntoTest003 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::TextEncoder textEncoder("utf-8");

    std::string input = "12345";
    napi_value src = nullptr;
    napi_create_string_utf8(env, input.c_str(), input.size(), &src);

    napi_value arrayBuffer = nullptr;
    void* arrayBufferPtr = nullptr;
    size_t arrayBufferSize = 20;
    napi_create_arraybuffer(env, arrayBufferSize, &arrayBufferPtr, &arrayBuffer);

    napi_value dest = nullptr;
        napi_create_typedarray(env, napi_int8_array, arrayBufferSize, arrayBuffer, 0, &dest);

    napi_value result = textEncoder.EncodeInto(env, src, dest);

    napi_value read = nullptr;
    napi_get_named_property(env, result, "read", &read);

    uint32_t resRead = 0;

    napi_get_value_uint32(env, read, &resRead);

    napi_value written = nullptr;
    napi_get_named_property(env, result, "written", &written);

    uint32_t resWritten = 0;
    napi_get_value_uint32(env, read, &resWritten);

    ASSERT_EQ(resRead, (uint32_t)5);
    ASSERT_EQ(resWritten, (uint32_t)5);
}

/**
 * @tc.name: GetEncoding001
 * @tc.desc: Test date type.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, GetEncoding001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("TextDecoder::getEncodingTest001 start");
    napi_env env = (napi_env)engine_;
    std::vector<int> inputVec;
    int fatal = -1;
    int ignoreBOM = -1;
    inputVec.push_back(fatal);
    inputVec.push_back(ignoreBOM);
    std::string str = "utf-8";
    OHOS::Util::TextDecoder textDecoder(str, inputVec);
    napi_value testString = textDecoder.GetEncoding(env);
    size_t bufferSize = 0;
    napi_get_value_string_utf8(env, testString, nullptr, 0, &bufferSize);
    std::string tmpTestStr = "utf-8";
    size_t strLength = 0;
    char* buffer = nullptr;
    if (bufferSize > 0) {
        buffer = new char[bufferSize + 1]();
        napi_get_value_string_utf8(env, testString, buffer, bufferSize + 1, &strLength);
    }
    const char *result = tmpTestStr.c_str();
    size_t resultLength = tmpTestStr.length();
    ASSERT_STREQ(result, buffer);
    ASSERT_EQ(resultLength, strLength);
    if (buffer != nullptr) {
        delete []buffer;
        buffer = nullptr;
    }
}

/**
 * @tc.name: GetEncoding002
 * @tc.desc: Test date type.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, GetEncoding002, testing::ext::TestSize.Level0)
{
    HILOG_INFO("TextDecoder::getEncodingTest002 start");
    napi_env env = (napi_env)engine_;
    std::vector<int> inputVec;
    int fatal = -1;
    int ignoreBOM = -1;
    inputVec.push_back(fatal);
    inputVec.push_back(ignoreBOM);
    std::string str = "GB18030";
    OHOS::Util::TextDecoder textDecoder(str, inputVec);
    napi_value testString = textDecoder.GetEncoding(env);
    size_t bufferSize = 0;
    napi_get_value_string_utf8(env, testString, nullptr, 0, &bufferSize);
    std::string tmpTestStr = "GB18030";
    size_t strLength = 0;
    char* buffer = nullptr;
    if (bufferSize > 0) {
        buffer = new char[bufferSize + 1]();
        napi_get_value_string_utf8(env, testString, buffer, bufferSize + 1, &strLength);
    }
    const char *result = tmpTestStr.c_str();
    size_t resultLength = tmpTestStr.length();
    ASSERT_STREQ(result, buffer);
    ASSERT_EQ(resultLength, strLength);
    if (buffer != nullptr) {
        delete []buffer;
        buffer = nullptr;
    }
}

/**
 * @tc.name: GetEncoding003
 * @tc.desc: Test date type.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, GetEncoding003, testing::ext::TestSize.Level0)
{
    HILOG_INFO("TextDecoder::getEncodingTest003 start");
    napi_env env = (napi_env)engine_;
    std::vector<int> inputVec;
    int fatal = -1;
    int ignoreBOM = -1;
    inputVec.push_back(fatal);
    inputVec.push_back(ignoreBOM);
    std::string str = "gb18030";
    OHOS::Util::TextDecoder textDecoder(str, inputVec);
    napi_value testString = textDecoder.GetEncoding(env);
    size_t bufferSize = 0;
    napi_get_value_string_utf8(env, testString, nullptr, 0, &bufferSize);
    std::string tmpTestStr = "gb18030";
    size_t strLength = 0;
    char* buffer = nullptr;
    if (bufferSize > 0) {
        buffer = new char[bufferSize + 1]();
        napi_get_value_string_utf8(env, testString, buffer, bufferSize + 1, &strLength);
    }
    const char *result = tmpTestStr.c_str();
    size_t resultLength = tmpTestStr.length();
    ASSERT_STREQ(result, buffer);
    ASSERT_EQ(resultLength, strLength);
    if (buffer != nullptr) {
        delete []buffer;
        buffer = nullptr;
    }
}

/**
 * @tc.name: GetFatal001
 * @tc.desc: Test date type.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, GetFatal001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("TextDecoder::GetFatal001 start");
    napi_env env = (napi_env)engine_;
    std::vector<int> inputVec;
    int fatal = 1;
    int ignoreBOM = 0;
    inputVec.push_back(fatal);
    inputVec.push_back(ignoreBOM);
    std::string str = "utf-8";
    OHOS::Util::TextDecoder textDecoder(str, inputVec);
    napi_value naVal = textDecoder.GetFatal(env);
    bool result = false;
    napi_get_value_bool(env, naVal, &result);
    ASSERT_TRUE(result);
}

/**
 * @tc.name: GetFatal002
 * @tc.desc: Test date type.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, GetFatal002, testing::ext::TestSize.Level0)
{
    HILOG_INFO("TextDecoder::GetFatal002 start");
    napi_env env = (napi_env)engine_;
    std::vector<int> inputVec;
    int fatal = -1;
    int ignoreBOM = 1;
    inputVec.push_back(fatal);
    inputVec.push_back(ignoreBOM);
    std::string str = "utf-8";
    OHOS::Util::TextDecoder textDecoder(str, inputVec);
    napi_value naVal = textDecoder.GetFatal(env);
    bool result = false;
    napi_get_value_bool(env, naVal, &result);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: GetFatal003
 * @tc.desc: Test date type.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, GetFatal003, testing::ext::TestSize.Level0)
{
    HILOG_INFO("TextDecoder::GetFatal003 start");
    napi_env env = (napi_env)engine_;
    std::vector<int> inputVec;
    int fatal = 0;
    int ignoreBOM = 1;
    inputVec.push_back(fatal);
    inputVec.push_back(ignoreBOM);
    std::string str = "utf-8";
    OHOS::Util::TextDecoder textDecoder(str, inputVec);
    napi_value naVal = textDecoder.GetFatal(env);
    bool result = false;
    napi_get_value_bool(env, naVal, &result);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: GetIgnoreBOM001
 * @tc.desc: Test date type.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, GetIgnoreBOM001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("TextDecoder::GetIgnoreBOM001 start");
    napi_env env = (napi_env)engine_;
    std::vector<int> inputVec;
    int fatal = -1;
    int ignoreBOM = 1;
    inputVec.push_back(fatal);
    inputVec.push_back(ignoreBOM);
    std::string str = "utf-8";
    OHOS::Util::TextDecoder textDecoder(str, inputVec);
    napi_value naVal = textDecoder.GetIgnoreBOM(env);
    bool result = false;
    napi_get_value_bool(env, naVal, &result);
    ASSERT_TRUE(result);
}

/**
 * @tc.name: GetIgnoreBOM002
 * @tc.desc: Test date type.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, GetIgnoreBOM002, testing::ext::TestSize.Level0)
{
    HILOG_INFO("TextDecoder::GetIgnoreBOM002 start");
    napi_env env = (napi_env)engine_;
    std::vector<int> inputVec;
    int fatal = 0;
    int ignoreBOM = 1;
    inputVec.push_back(fatal);
    inputVec.push_back(ignoreBOM);
    std::string str = "utf-8";
    OHOS::Util::TextDecoder textDecoder(str, inputVec);
    napi_value naVal = textDecoder.GetIgnoreBOM(env);
    bool result = false;
    napi_get_value_bool(env, naVal, &result);
    ASSERT_TRUE(result);
}

/**
 * @tc.name: GetIgnoreBOM003
 * @tc.desc: Test date type.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, GetIgnoreBOM003, testing::ext::TestSize.Level0)
{
    HILOG_INFO("TextDecoder::GetIgnoreBOM003 start");
    napi_env env = (napi_env)engine_;
    std::vector<int> inputVec;
    int fatal = 1;
    int ignoreBOM = 1;
    inputVec.push_back(fatal);
    inputVec.push_back(ignoreBOM);
    std::string str = "utf-8";
    OHOS::Util::TextDecoder textDecoder(str, inputVec);
    napi_value naVal = textDecoder.GetIgnoreBOM(env);
    bool result = false;
    napi_get_value_bool(env, naVal, &result);
    ASSERT_TRUE(result);
}

/**
 * @tc.name: decoderUtf8001 utf-8
 * @tc.desc: Test date type.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decoderUtf8001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decoderUtf8001 start");
    napi_env env = (napi_env)engine_;
    std::vector<int> inputVec;
    int fatal = -1;
    int ignoreBOM = -1;
    inputVec.push_back(fatal);
    inputVec.push_back(ignoreBOM);
    std::string str = "utf-8";
    OHOS::Util::TextDecoder textDecoder(str, inputVec);
    bool iflag = false;
    size_t byteLength = 3;
    void* data = nullptr;
    napi_value resultBuff = nullptr;
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    unsigned char arr[3] = {0x61, 0x62, 0x63};
    int ret = memcpy_s(data, sizeof(arr), reinterpret_cast<void*>(arr), sizeof(arr));
    ASSERT_EQ(0, ret);
    napi_value result2 = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &result2);
    napi_value testString = textDecoder.Decode(env, result2, iflag);
    size_t bufferSize = 0;
    napi_get_value_string_utf8(env, testString, nullptr, 0, &bufferSize);
    size_t length = 0;
    char* ch = nullptr;
    if (bufferSize > 0) {
        ch = new char[bufferSize + 1]();
        napi_get_value_string_utf8(env, testString, ch, bufferSize + 1, &length);
    }
    ASSERT_STREQ("abc", ch);
    if (ch != nullptr) {
        delete []ch;
        ch = nullptr;
    }
}

/**
 * @tc.name: decoderUtf8002 utf-8
 * @tc.desc: Test date type.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decoderUtf8002, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decoderUtf8002 start");
    napi_env env = (napi_env)engine_;
    std::vector<int> inputVec;
    int fatal = -1;
    int ignoreBOM = 0;
    inputVec.push_back(fatal);
    inputVec.push_back(ignoreBOM);
    std::string str = "utf-8";
    OHOS::Util::TextDecoder textDecoder(str, inputVec);
    bool iflag = true;
    size_t byteLength = 3;
    void* data = nullptr;
    napi_value resultBuff = nullptr;
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    unsigned char arr[3] = {0x61, 0x62, 0x63};
    int ret = memcpy_s(data, sizeof(arr), reinterpret_cast<void*>(arr), sizeof(arr));
    ASSERT_EQ(0, ret);
    napi_value result2 = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &result2);
    napi_value testString = textDecoder.Decode(env, result2, iflag);
    size_t bufferSize = 0;
    size_t length = 0;
    napi_get_value_string_utf8(env, testString, nullptr, 0, &bufferSize);
    char* ch = nullptr;
    if (bufferSize > 0) {
        ch = new char[bufferSize + 1]();
        napi_get_value_string_utf8(env, testString, ch, bufferSize + 1, &length);
    }
    ASSERT_STREQ("abc", ch);
    if (ch != nullptr) {
        delete []ch;
        ch = nullptr;
    }
}

/**
 * @tc.name: decoderUtf16le001 utf-16le
 * @tc.desc: Test date type.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decoderUtf16le001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decoderUtf16le001 start");
    napi_env env = (napi_env)engine_;
    std::vector<int> inputVec;
    int fatal = 0;
    int ignoreBOM = 0;
    inputVec.push_back(fatal);
    inputVec.push_back(ignoreBOM);
    std::string str = "utf-16le";
    OHOS::Util::TextDecoder textDecoder(str, inputVec);
    bool iflag = false;
    size_t byteLength = 6;
    void* data = nullptr;
    napi_value resultBuff = nullptr;
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    unsigned char arr[6] = {0x61, 0x00, 0x62, 0x00, 0x63, 0x00};
    int ret = memcpy_s(data, sizeof(arr), reinterpret_cast<void*>(arr), sizeof(arr));
    ASSERT_EQ(0, ret);
    napi_value result2 = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &result2);
    napi_value testString = textDecoder.Decode(env, result2, iflag);
    size_t bufferSize = 0;
    size_t length = 0;
    napi_get_value_string_utf8(env, testString, nullptr, 0, &bufferSize);
    char* ch = nullptr;
    if (bufferSize > 0) {
        ch = new char[bufferSize + 1]();
        napi_get_value_string_utf8(env, testString, ch, bufferSize + 1, &length);
    }
    ASSERT_STREQ("abc", ch);
    if (ch != nullptr) {
        delete []ch;
        ch = nullptr;
    }
}

/**
 * @tc.name: decoderUtf16le002 utf-16le
 * @tc.desc: Test date type.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decoderUtf16le002, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decoderUtf16le002 start");
    napi_env env = (napi_env)engine_;
    std::vector<int>  inputVec;
    int fatal = 0;
    int ignoreBOM = 1;
    inputVec.push_back(fatal);
    inputVec.push_back(ignoreBOM);
    std::string str = "utf-16le";
    OHOS::Util::TextDecoder textDecoder(str, inputVec);
    bool iflag = true;
    size_t byteLength = 6;
    void* data = nullptr;
    napi_value resultBuff = nullptr;
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    unsigned char arr[6] = {0x61, 0x00, 0x62, 0x00, 0x63, 0x00};
    int ret = memcpy_s(data, sizeof(arr), reinterpret_cast<void*>(arr), sizeof(arr));
    ASSERT_EQ(0, ret);
    napi_value result2 = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &result2);
    napi_value testString = textDecoder.Decode(env, result2, iflag);
    size_t bufferSize = 0;
    napi_get_value_string_utf8(env, testString, nullptr, 0, &bufferSize);
    char* ch = nullptr;
    size_t length = 0;
    if (bufferSize > 0) {
        ch = new char[bufferSize + 1]();
        napi_get_value_string_utf8(env, testString, ch, bufferSize + 1, &length);
    }
    ASSERT_STREQ("abc", ch);
    if (ch != nullptr) {
        delete []ch;
        ch = nullptr;
    }
}

/**
 * @tc.name: decoderUtf16le003 utf-16le
 * @tc.desc: Test date type.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decoderUtf16le003, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decoderUtf16le003 start");
    napi_env env = (napi_env)engine_;
    std::vector<int>  inputVec;
    int fatal = 0;
    int ignoreBOM = 0;
    inputVec.push_back(fatal);
    inputVec.push_back(ignoreBOM);
    std::string str = "utf-16le";
    OHOS::Util::TextDecoder textDecoder(str, inputVec);
    bool iflag = true;
    size_t byteLength = 8;
    void* data = nullptr;
    napi_value resultBuff = nullptr;
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    unsigned char arr[8] = {0xFF, 0xFE, 0x61, 0x00, 0x62, 0x00, 0x63, 0x00};
    int ret = memcpy_s(data, sizeof(arr), reinterpret_cast<void*>(arr), sizeof(arr));
    ASSERT_EQ(0, ret);
    napi_value result2 = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &result2);
    napi_value testString = textDecoder.Decode(env, result2, iflag);
    size_t bufferSize = 0;
    napi_get_value_string_utf8(env, testString, nullptr, 0, &bufferSize);
    char* ch = nullptr;
    size_t length = 0;
    std::string tempStr01 = "";
    if (bufferSize > 0) {
        ch = new char[bufferSize + 1]();
        napi_get_value_string_utf8(env, testString, ch, bufferSize + 1, &length);
        tempStr01 = ch;
    }
    std::u16string tempU16str02 =
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.from_bytes(tempStr01);
    ASSERT_EQ(0xFEFF, (int)tempU16str02[0]);
    ASSERT_EQ(0x61, (int)tempU16str02[1]);
    ASSERT_EQ(0x62, (int)tempU16str02[2]);
    ASSERT_EQ(0x63, (int)tempU16str02[3]);
    if (ch != nullptr) {
        delete []ch;
        ch = nullptr;
    }
}

/**
 * @tc.name: decoderUtf16le004 utf-16le
 * @tc.desc: Test date type.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decoderUtf16le004, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decoderUtf16le004 start");
    napi_env env = (napi_env)engine_;
    std::vector<int>  inputVec;
    int fatal = -1;
    int ignoreBOM = -1;
    inputVec.push_back(fatal);
    inputVec.push_back(ignoreBOM);
    std::string str = "utf-16le";
    OHOS::Util::TextDecoder textDecoder(str, inputVec);
    bool iflag = false;
    size_t byteLength = 8;
    void* data = nullptr;
    napi_value resultBuff = nullptr;
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    unsigned char arr[8] = {0xFF, 0xFE, 0x61, 0x00, 0x62, 0x00, 0x63, 0x00};
    int ret = memcpy_s(data, sizeof(arr), reinterpret_cast<void*>(arr), sizeof(arr));
    ASSERT_EQ(0, ret);
    napi_value result2 = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &result2);
    napi_value testString = textDecoder.Decode(env, result2, iflag);
    size_t bufferSize = 0;
    napi_get_value_string_utf8(env, testString, nullptr, 0, &bufferSize);
    char* ch = nullptr;
    size_t length = 0;
    std::string tempStr01 = "";
    if (bufferSize > 0) {
        ch = new char[bufferSize + 1]();
        napi_get_value_string_utf8(env, testString, ch, bufferSize + 1, &length);
        tempStr01 = ch;
    }
    std::u16string tempU16str02 =
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.from_bytes(tempStr01);
    ASSERT_EQ(0xFEFF, (int)tempU16str02[0]);
    ASSERT_EQ(0x61, (int)tempU16str02[1]);
    ASSERT_EQ(0x62, (int)tempU16str02[2]);
    ASSERT_EQ(0x63, (int)tempU16str02[3]);
    if (ch != nullptr) {
        delete []ch;
        ch = nullptr;
    }
}

/**
 * @tc.name: decoderUtf16be001 utf-16be
 * @tc.desc: Test date type.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decoderUtf16be001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decoderUtf16be001 start");
    napi_env env = (napi_env)engine_;
    std::vector<int>  inputVec;
    int fatal = 0;
    int ignoreBOM = 0;
    inputVec.push_back(fatal);
    inputVec.push_back(ignoreBOM);
    std::string str = "utf-16be";
    OHOS::Util::TextDecoder textDecoder(str, inputVec);
    bool iflag = false;
    size_t byteLength = 6;
    void* data = nullptr;
    napi_value resultBuff = nullptr;
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    unsigned char arr[6] = {0x00, 0x61, 0x00, 0x62, 0x00, 0x63};
    int ret = memcpy_s(data, sizeof(arr), reinterpret_cast<void*>(arr), sizeof(arr));
    ASSERT_EQ(0, ret);
    napi_value result2 = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &result2);
    napi_value testString = textDecoder.Decode(env, result2, iflag);
    size_t bufferSize = 0;
    napi_get_value_string_utf8(env, testString, nullptr, 0, &bufferSize);
    size_t length = 0;
    char* ch = nullptr;
    if (bufferSize > 0) {
        ch = new char[bufferSize + 1]();
        napi_get_value_string_utf8(env, testString, ch, bufferSize + 1, &length);
    }
    ASSERT_STREQ("abc", ch);
    if (ch != nullptr) {
        delete []ch;
        ch = nullptr;
    }
}

/**
 * @tc.name: decoderUtf16be002 utf-16be
 * @tc.desc: Test date type.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decoderUtf16be002, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decoderUtf16be002 start");
    napi_env env = (napi_env)engine_;
    std::vector<int>  inputVec;
    int fatal = 0;
    int ignoreBOM = 0;
    inputVec.push_back(fatal);
    inputVec.push_back(ignoreBOM);
    std::string str = "utf-16be";
    OHOS::Util::TextDecoder textDecoder(str, inputVec);
    bool iflag = false;
    size_t byteLength = 8;
    void* data = nullptr;
    napi_value resultBuff = nullptr;
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    unsigned char arr[8] = {0xFE, 0xFF, 0x00, 0x61, 0x00, 0x62, 0x00, 0x63};
    int ret = memcpy_s(data, sizeof(arr), reinterpret_cast<void*>(arr), sizeof(arr));
    ASSERT_EQ(0, ret);
    napi_value result2 = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &result2);
    napi_value testString = textDecoder.Decode(env, result2, iflag);
    size_t bufferSize = 0;
    napi_get_value_string_utf8(env, testString, nullptr, 0, &bufferSize);
    size_t length = 0;
    char* ch = nullptr;
    std::string tempStr01 = "";
    if (bufferSize > 0) {
        ch = new char[bufferSize + 1]();
        napi_get_value_string_utf8(env, testString, ch, bufferSize + 1, &length);
        tempStr01 = ch;
    }
    std::u16string tempU16str02 =
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.from_bytes(tempStr01);
    ASSERT_EQ(0xFEFF, (int)tempU16str02[0]);
    ASSERT_EQ(0x61, (int)tempU16str02[1]);
    ASSERT_EQ(0x62, (int)tempU16str02[2]);
    ASSERT_EQ(0x63, (int)tempU16str02[3]);
    if (ch != nullptr) {
        delete []ch;
        ch = nullptr;
    }
}

/**
 * @tc.name: decoderUtf16be003 utf-16be
 * @tc.desc: Test date type.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decoderUtf16be003, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decoderUtf16be003 start");
    napi_env env = (napi_env)engine_;
    std::vector<int>  inputVec;
    int fatal = 0;
    int ignoreBOM = 1;
    inputVec.push_back(fatal);
    inputVec.push_back(ignoreBOM);
    std::string str = "utf-16be";
    OHOS::Util::TextDecoder textDecoder(str, inputVec);
    bool iflag = true;
    size_t byteLength = 8;
    void* data = nullptr;
    napi_value resultBuff = nullptr;
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    unsigned char arr[8] = {0xFE, 0xFF, 0x00, 0x61, 0x00, 0x62, 0x00, 0x63};
    int ret = memcpy_s(data, sizeof(arr), reinterpret_cast<void*>(arr), sizeof(arr));
    ASSERT_EQ(0, ret);
    napi_value result2 = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &result2);
    napi_value testString = textDecoder.Decode(env, result2, iflag);
    size_t bufferSize = 0;
    napi_get_value_string_utf8(env, testString, nullptr, 0, &bufferSize);
    size_t length = 0;
    char* ch = nullptr;
    std::string tempStr01 = "";
    if (bufferSize > 0) {
        ch = new char[bufferSize + 1]();
        napi_get_value_string_utf8(env, testString, ch, bufferSize + 1, &length);
        tempStr01 = ch;
    }
    std::u16string tempU16str02 =
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.from_bytes(tempStr01);
    ASSERT_EQ(0xFEFF, (int)tempU16str02[0]);
    ASSERT_EQ(0x61, (int)tempU16str02[1]);
    ASSERT_EQ(0x62, (int)tempU16str02[2]);
    ASSERT_EQ(0x63, (int)tempU16str02[3]);
    if (ch != nullptr) {
        delete []ch;
        ch = nullptr;
    }
}

/* @tc.name: encodeTest001
 * @tc.desc: Encodes all bytes in the specified u8 array into
             the newly allocated u8 array using the Base64 encoding scheme.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, encodeTest001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("encodeTest001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Base64 base64;
    unsigned char input[3] = {0x73, 0x31, 0x33};
    napi_value arrayBuffer = nullptr;
    void* data = nullptr;
    size_t arrayBufferSize = 3;
    napi_create_arraybuffer(env, arrayBufferSize, &data, &arrayBuffer);
    int ret = memcpy_s(data, sizeof(input), reinterpret_cast<void*>(input), sizeof(input));
    ASSERT_EQ(0, ret);
    napi_value src = nullptr;
    napi_create_typedarray(env, napi_uint8_array, arrayBufferSize, arrayBuffer, 0, &src);

    napi_value result = base64.EncodeSync(env, src);
    char excepted[4] = {0x63, 0x7A, 0x45, 0x7A};
    napi_typedarray_type type;
    size_t srcLength = 0;
    void* srcData = nullptr;
    napi_value srcBuffer = nullptr;
    size_t byteOffset = 0;
    napi_get_typedarray_info(env, result, &type, &srcLength, &srcData, &srcBuffer, &byteOffset);
    char* res = (char*)srcData;
    ASSERT_EQ(res[0], excepted[0]);
    ASSERT_EQ(res[1], excepted[1]);
    ASSERT_EQ(res[2], excepted[2]);
    ASSERT_EQ(res[3], excepted[3]);
}

/* @tc.name: encodeTest002
 * @tc.desc: Encodes all bytes in the specified u8 array
             into the newly allocated u8 array using the Base64 encoding scheme.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, encodeTest002, testing::ext::TestSize.Level0)
{
    HILOG_INFO("encodeTest002 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Base64 base64;
    unsigned char input[14] = {66, 97, 115, 101, 54, 52, 32, 78, 111, 100, 101, 46, 106, 115};
    napi_value arrayBuffer = nullptr;
    void* data = nullptr;
    size_t arrayBufferSize = 14;
    napi_create_arraybuffer(env, arrayBufferSize, &data, &arrayBuffer);
    int ret = memcpy_s(data, sizeof(input), reinterpret_cast<void*>(input), sizeof(input));
    ASSERT_EQ(0, ret);
    napi_value src = nullptr;
    napi_create_typedarray(env, napi_uint8_array, arrayBufferSize, arrayBuffer, 0, &src);

    napi_value result = base64.EncodeSync(env, src);
    char excepted[20] = {81, 109, 70, 122, 90, 84, 89, 48, 73, 69, 53, 118, 90, 71, 85, 117, 97, 110, 77, 61};
    napi_typedarray_type type;
    size_t srcLength = 0;
    void* srcData = nullptr;
    napi_value srcBuffer = nullptr;
    size_t byteOffset = 0;
    napi_get_typedarray_info(env, result, &type, &srcLength, &srcData, &srcBuffer, &byteOffset);
    char* res = (char*)srcData;
    for (size_t i = 0; i < 20; i++) {
        ASSERT_EQ(res[i], excepted[i]);
    }
}

/* @tc.name: encodeTest003
 * @tc.desc: Encodes all bytes in the specified u8 array
             into the newly allocated u8 array using the Base64 encoding scheme.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, encodeTest003, testing::ext::TestSize.Level0)
{
    HILOG_INFO("encodeTest003 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Base64 base64;
    unsigned char input[26] = {66, 97, 115, 101, 54, 52, 32, 69, 110,
                              99, 111, 100, 105, 110, 103, 32, 105, 110, 32, 78, 111, 100, 101, 46, 106, 115};
    napi_value arrayBuffer = nullptr;
    void* data = nullptr;
    size_t arrayBufferSize = 26;
    napi_create_arraybuffer(env, arrayBufferSize, &data, &arrayBuffer);
    int ret = memcpy_s(data, sizeof(input), reinterpret_cast<void*>(input), sizeof(input));
    ASSERT_EQ(0, ret);
    napi_value src = nullptr;
    napi_create_typedarray(env, napi_uint8_array, arrayBufferSize, arrayBuffer, 0, &src);

    napi_value result = base64.EncodeSync(env, src);
    char excepted[36] = {81, 109, 70, 122, 90, 84, 89, 48, 73, 69, 86, 117, 89, 50, 57, 107, 97, 87, 53,
                        110, 73, 71, 108, 117, 73, 69, 53, 118, 90, 71, 85, 117, 97, 110, 77, 61};
    napi_typedarray_type type;
    size_t srcLength = 0;
    void* srcData = nullptr;
    napi_value srcBuffer = nullptr;
    size_t byteOffset = 0;
    napi_get_typedarray_info(env, result, &type, &srcLength, &srcData, &srcBuffer, &byteOffset);
    char* res = (char*)srcData;
    for (size_t i = 0; i < 36; i++) {
        ASSERT_EQ(res[i], excepted[i]);
    }
}

/* @tc.name: encodeTest004
 * @tc.desc: Encodes all bytes in the specified u8 array into the
             newly allocated u8 array using the Base64 encoding scheme.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, encodeTest004, testing::ext::TestSize.Level0)
{
    HILOG_INFO("encodeTest004 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Base64 base64;
    unsigned char input[4] = {168, 174, 155, 255};
    napi_value arrayBuffer = nullptr;
    void* data = nullptr;
    size_t arrayBufferSize = 4;
    napi_create_arraybuffer(env, arrayBufferSize, &data, &arrayBuffer);
    int ret = memcpy_s(data, sizeof(input), reinterpret_cast<void*>(input), sizeof(input));
    ASSERT_EQ(0, ret);
    napi_value src = nullptr;
    napi_create_typedarray(env, napi_uint8_array, arrayBufferSize, arrayBuffer, 0, &src);

    napi_value result = base64.EncodeSync(env, src);
    char excepted[8] = {113, 75, 54, 98, 47, 119, 61, 61};
    napi_typedarray_type type;
    size_t srcLength = 0;
    void* srcData = nullptr;
    napi_value srcBuffer = nullptr;
    size_t byteOffset = 0;
    napi_get_typedarray_info(env, result, &type, &srcLength, &srcData, &srcBuffer, &byteOffset);
    char* res = (char*)srcData;
    for (size_t i = 0; i < 8; i++) {
        ASSERT_EQ(res[i], excepted[i]);
    }
}

/* @tc.name: encodeTest005
 * @tc.desc: Encodes all bytes in the specified u8 array
             into the newly allocated u8 array using the Base64 encoding scheme.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, encodeTest005, testing::ext::TestSize.Level0)
{
    HILOG_INFO("encodeTest005 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Base64 base64;
    unsigned char input[6] = {66, 97, 115, 101, 54, 52};
    napi_value arrayBuffer = nullptr;
    void* data = nullptr;
    size_t arrayBufferSize = 6;
    napi_create_arraybuffer(env, arrayBufferSize, &data, &arrayBuffer);
    int ret = memcpy_s(data, sizeof(input), reinterpret_cast<void*>(input), sizeof(input));
    ASSERT_EQ(0, ret);
    napi_value src = nullptr;
    napi_create_typedarray(env, napi_uint8_array, arrayBufferSize, arrayBuffer, 0, &src);

    napi_value result = base64.EncodeSync(env, src);
    char excepted[8] = {81, 109, 70, 122, 90, 84, 89, 48};
    napi_typedarray_type type;
    size_t srcLength = 0;
    void* srcData = nullptr;
    napi_value srcBuffer = nullptr;
    size_t byteOffset = 0;
    napi_get_typedarray_info(env, result, &type, &srcLength, &srcData, &srcBuffer, &byteOffset);
    char* res = (char*)srcData;
    for (size_t i = 0; i < 8; i++) {
        ASSERT_EQ(res[i], excepted[i]);
    }
}

/* @tc.name: encodeToStringTest001
 * @tc.desc: Encodes the specified byte array as a String using the Base64 encoding scheme.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, encodeToStringTest001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("encodeToStringTest001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Base64 base64;

    unsigned char input[3] = {115, 49, 51};
    napi_value arrayBuffer = nullptr;
    size_t arrayBufferSize = 3;
    void* data = nullptr;
    napi_create_arraybuffer(env, arrayBufferSize, &data, &arrayBuffer);
    int ret = memcpy_s(data, sizeof(input), reinterpret_cast<void*>(input), sizeof(input));
    ASSERT_EQ(0, ret);
    napi_value src = nullptr;
    napi_create_typedarray(env, napi_uint8_array, arrayBufferSize, arrayBuffer, 0, &src);
    napi_value result = base64.EncodeToStringSync(env, src);
    size_t prolen = 0;
    char* inputString = nullptr;
    napi_get_value_string_utf8(env, result, nullptr, 0, &prolen);
    if (prolen > 0) {
        inputString = new char[prolen + 1];
        if (memset_s(inputString, prolen + 1, '\0', prolen + 1) != 0) {
            napi_throw_error(env, "-1", "decode inputString memset_s failed");
        }
    } else {
        napi_throw_error(env, "-2", "prolen is error !");
    }
    napi_get_value_string_utf8(env, result, inputString, prolen + 1, &prolen);
    ASSERT_STREQ("czEz", inputString);
    if (inputString != nullptr) {
        delete []inputString;
        inputString = nullptr;
    }
}

/* @tc.name: encodeToStringTest002
 * @tc.desc: Encodes the specified byte array as a String using the Base64 encoding scheme.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, encodeToStringTest002, testing::ext::TestSize.Level0)
{
    HILOG_INFO("encodeToStringTest002 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Base64 base64;

    unsigned char input[14] = {66, 97, 115, 101, 54, 52, 32, 78, 111, 100, 101, 46, 106, 115};
    napi_value arrayBuffer = nullptr;
    size_t arrayBufferSize = 14;
    void* data = nullptr;
    napi_create_arraybuffer(env, arrayBufferSize, &data, &arrayBuffer);
    int ret = memcpy_s(data, sizeof(input), reinterpret_cast<void*>(input), sizeof(input));
    ASSERT_EQ(0, ret);
    napi_value src = nullptr;
    napi_create_typedarray(env, napi_uint8_array, arrayBufferSize, arrayBuffer, 0, &src);
    napi_value result = base64.EncodeToStringSync(env, src);
    size_t prolen = 0;
    char* inputString = nullptr;
    napi_get_value_string_utf8(env, result, nullptr, 0, &prolen);
    if (prolen > 0) {
        inputString = new char[prolen + 1];
        if (memset_s(inputString, prolen + 1, '\0', prolen + 1) != 0) {
            napi_throw_error(env, "-1", "decode inputString memset_s failed");
        }
    } else {
        napi_throw_error(env, "-2", "prolen is error !");
    }
    napi_get_value_string_utf8(env, result, inputString, prolen + 1, &prolen);
    ASSERT_STREQ("QmFzZTY0IE5vZGUuanM=", inputString);
    if (inputString != nullptr) {
        delete []inputString;
        inputString = nullptr;
    }
}

/* @tc.name: encodeToStringTest003
 * @tc.desc: Encodes the specified byte array as a String using the Base64 encoding scheme.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, encodeToStringTest003, testing::ext::TestSize.Level0)
{
    HILOG_INFO("encodeToStringTest003 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Base64 base64;

    unsigned char input[26] = {66, 97, 115, 101, 54, 52, 32, 69, 110,
                              99, 111, 100, 105, 110, 103, 32, 105, 110, 32, 78, 111, 100, 101, 46, 106, 115};
    napi_value arrayBuffer = nullptr;
    size_t arrayBufferSize = 26;
    void* data = nullptr;
    napi_create_arraybuffer(env, arrayBufferSize, &data, &arrayBuffer);
    int ret = memcpy_s(data, sizeof(input), reinterpret_cast<void*>(input), sizeof(input));
    ASSERT_EQ(0, ret);
    napi_value src = nullptr;
    napi_create_typedarray(env, napi_uint8_array, arrayBufferSize, arrayBuffer, 0, &src);
    napi_value result = base64.EncodeToStringSync(env, src);
    size_t prolen = 0;
    char* inputString = nullptr;
    napi_get_value_string_utf8(env, result, nullptr, 0, &prolen);
    if (prolen > 0) {
        inputString = new char[prolen + 1];
        if (memset_s(inputString, prolen + 1, '\0', prolen + 1) != 0) {
            napi_throw_error(env, "-1", "decode inputString memset_s failed");
        }
    } else {
        napi_throw_error(env, "-2", "prolen is error !");
    }
    napi_get_value_string_utf8(env, result, inputString, prolen + 1, &prolen);
    ASSERT_STREQ("QmFzZTY0IEVuY29kaW5nIGluIE5vZGUuanM=", inputString);
    if (inputString != nullptr) {
        delete []inputString;
        inputString = nullptr;
    }
}

/* @tc.name: encodeToStringTest004
 * @tc.desc: Encodes the specified byte array as a String using the Base64 encoding scheme.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, encodeToStringTest004, testing::ext::TestSize.Level0)
{
    HILOG_INFO("encodeToStringTest004 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Base64 base64;

    unsigned char input[4] = {168, 174, 155, 255};
    napi_value arrayBuffer = nullptr;
    size_t arrayBufferSize = 4;
    void* data = nullptr;
    napi_create_arraybuffer(env, arrayBufferSize, &data, &arrayBuffer);
    int ret = memcpy_s(data, sizeof(input), reinterpret_cast<void*>(input), sizeof(input));
    ASSERT_EQ(0, ret);
    napi_value src = nullptr;
    napi_create_typedarray(env, napi_uint8_array, arrayBufferSize, arrayBuffer, 0, &src);
    napi_value result = base64.EncodeToStringSync(env, src);
    size_t prolen = 0;
    char* inputString = nullptr;
    napi_get_value_string_utf8(env, result, nullptr, 0, &prolen);
    if (prolen > 0) {
        inputString = new char[prolen + 1];
        if (memset_s(inputString, prolen + 1, '\0', prolen + 1) != 0) {
            napi_throw_error(env, "-1", "decode inputString memset_s failed");
        }
    } else {
        napi_throw_error(env, "-2", "prolen is error !");
    }
    napi_get_value_string_utf8(env, result, inputString, prolen + 1, &prolen);
    ASSERT_STREQ("qK6b/w==", inputString);
    if (inputString != nullptr) {
        delete []inputString;
        inputString = nullptr;
    }
}

/* @tc.name: encodeToStringTest005
 * @tc.desc: Encodes the specified byte array as a String using the Base64 encoding scheme.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, encodeToStringTest005, testing::ext::TestSize.Level0)
{
    HILOG_INFO("encodeToStringTest005 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Base64 base64;

    unsigned char input[6] = {66, 97, 115, 101, 54, 52};
    napi_value arrayBuffer = nullptr;
    size_t arrayBufferSize = 6;
    void* data = nullptr;
    napi_create_arraybuffer(env, arrayBufferSize, &data, &arrayBuffer);
    int ret = memcpy_s(data, sizeof(input), reinterpret_cast<void*>(input), sizeof(input));
    ASSERT_EQ(0, ret);
    napi_value src = nullptr;
    napi_create_typedarray(env, napi_uint8_array, arrayBufferSize, arrayBuffer, 0, &src);
    napi_value result = base64.EncodeToStringSync(env, src);
    size_t prolen = 0;
    char* inputString = nullptr;
    napi_get_value_string_utf8(env, result, nullptr, 0, &prolen);
    if (prolen > 0) {
        inputString = new char[prolen + 1];
        if (memset_s(inputString, prolen + 1, '\0', prolen + 1) != 0) {
            napi_throw_error(env, "-1", "decode inputString memset_s failed");
        }
    } else {
        napi_throw_error(env, "-2", "prolen is error !");
    }
    napi_get_value_string_utf8(env, result, inputString, prolen + 1, &prolen);
    ASSERT_STREQ("QmFzZTY0", inputString);
    if (inputString != nullptr) {
        delete []inputString;
        inputString = nullptr;
    }
}

/* @tc.name: decodeTest001
 * @tc.desc: Decodes the Base64-encoded string or input u8 array
             into the newly allocated u8 array using the Base64 encoding scheme.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decodeTest001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decodeTest001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Base64 base64;

    unsigned char input[4] = {99, 122, 69, 122};
    napi_value arrayBuffer = nullptr;
    size_t arrayBufferSize = 4;
    void* data = nullptr;
    napi_create_arraybuffer(env, arrayBufferSize, &data, &arrayBuffer);
    int ret = memcpy_s(data, sizeof(input), reinterpret_cast<void*>(input), sizeof(input));
    ASSERT_EQ(0, ret);
    napi_value src = nullptr;
    napi_create_typedarray(env, napi_uint8_array, arrayBufferSize, arrayBuffer, 0, &src);
    napi_value result = base64.DecodeSync(env, src);
    char excepted[3] = {115, 49, 51};
    napi_typedarray_type type;
    size_t srcLength = 0;
    void* srcData = nullptr;
    napi_value srcBuffer = nullptr;
    size_t byteOffset = 0;
    napi_get_typedarray_info(env, result, &type, &srcLength, &srcData, &srcBuffer, &byteOffset);
    char* res = (char*)srcData;

    ASSERT_EQ(res[0], excepted[0]);
    ASSERT_EQ(res[1], excepted[1]);
    ASSERT_EQ(res[2], excepted[2]);
}

/* @tc.name: decodeTest002
 * @tc.desc: Decodes the Base64-encoded string or input u8 array
             into the newly allocated u8 array using the Base64 encoding scheme.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decodeTest002, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decodeTest002 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Base64 base64;

    unsigned char input[20] = {81, 109, 70, 122, 90, 84, 89, 48, 73, 69, 53, 118, 90, 71, 85, 117, 97, 110, 77, 61};
    napi_value arrayBuffer = nullptr;
    size_t arrayBufferSize = 20;
    void* data = nullptr;
    napi_create_arraybuffer(env, arrayBufferSize, &data, &arrayBuffer);
    int ret = memcpy_s(data, sizeof(input), reinterpret_cast<void*>(input), sizeof(input));
    ASSERT_EQ(0, ret);
    napi_value src = nullptr;
    napi_create_typedarray(env, napi_uint8_array, arrayBufferSize, arrayBuffer, 0, &src);
    napi_value result = base64.DecodeSync(env, src);
    char excepted[14] = {66, 97, 115, 101, 54, 52, 32, 78, 111, 100, 101, 46, 106, 115};
    napi_typedarray_type type;
    size_t srcLength = 0;
    void* srcData = nullptr;
    napi_value srcBuffer = nullptr;
    size_t byteOffset = 0;
    napi_get_typedarray_info(env, result, &type, &srcLength, &srcData, &srcBuffer, &byteOffset);
    char* res = (char*)srcData;

    for (size_t i = 0; i < 14; i++) {
        ASSERT_EQ(res[i], excepted[i]);
    }
}

/* @tc.name: decodeTest003
 * @tc.desc: Decodes the Base64-encoded string or input u8 array
             into the newly allocated u8 array using the Base64 encoding scheme.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decodeTest003, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decodeTest003 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Base64 base64;

    std::string input = "czEz";
    napi_value src = nullptr;
    napi_create_string_utf8(env, input.c_str(), input.size(), &src);
    napi_value result = base64.DecodeSync(env, src);
    char excepted[3] = {115, 49, 51};
    napi_typedarray_type type;
    size_t srcLength = 0;
    void* srcData = nullptr;
    napi_value srcBuffer = nullptr;
    size_t byteOffset = 0;
    napi_get_typedarray_info(env, result, &type, &srcLength, &srcData, &srcBuffer, &byteOffset);
    char* res = (char*)srcData;

    ASSERT_EQ(res[0], excepted[0]);
    ASSERT_EQ(res[1], excepted[1]);
    ASSERT_EQ(res[2], excepted[2]);
}

/* @tc.name: decodeTest004
 * @tc.desc: Decodes the Base64-encoded string or input u8 array
             into the newly allocated u8 array using the Base64 encoding scheme.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decodeTest004, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decodeTest004 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Base64 base64;

    std::string input = "qK6b/w==";
    napi_value src = nullptr;
    napi_create_string_utf8(env, input.c_str(), input.size(), &src);
    napi_value result = base64.DecodeSync(env, src);
    char excepted[4] = {168, 174, 155, 255};
    napi_typedarray_type type;
    size_t srcLength = 0;
    void* srcData = nullptr;
    napi_value srcBuffer = nullptr;
    size_t byteOffset = 0;
    napi_get_typedarray_info(env, result, &type, &srcLength, &srcData, &srcBuffer, &byteOffset);
    char* res = (char*)srcData;
    for (size_t i = 0; i < 4; i++) {
        ASSERT_EQ(res[i], excepted[i]);
    }
}

/* @tc.name: decodeTest005
 * @tc.desc: Decodes the Base64-encoded string or input u8 array
             into the newly allocated u8 array using the Base64 encoding scheme.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decodeTest005, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decodeTest005 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Base64 base64;

    std::string input = "QmFzZTY0";
    napi_value src = nullptr;
    napi_create_string_utf8(env, input.c_str(), input.size(), &src);
    napi_value result = base64.DecodeSync(env, src);
    char excepted[6] = {66, 97, 115, 101, 54, 52};
    napi_typedarray_type type;
    size_t srcLength = 0;
    void* srcData = nullptr;
    napi_value srcBuffer = nullptr;
    size_t byteOffset = 0;
    napi_get_typedarray_info(env, result, &type, &srcLength, &srcData, &srcBuffer, &byteOffset);
    char* res = (char*)srcData;
    for (size_t i = 0; i < 6; i++) {
        ASSERT_EQ(res[i], excepted[i]);
    }
}

/* @tc.name: encodeAsyncTest001
 * @tc.desc: Asynchronously encodes all bytes in the specified u8 array
             into the newly allocated u8 array using the Base64 encoding scheme.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, encodeAsyncTest001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("encodeAsyncTest001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Base64 base64;
    unsigned char input[3] = {0x73, 0x31, 0x33};
    napi_value arrayBuffer = nullptr;
    void* data = nullptr;
    size_t arrayBufferSize = 3;
    napi_create_arraybuffer(env, arrayBufferSize, &data, &arrayBuffer);
    int ret = memcpy_s(data, sizeof(input), reinterpret_cast<void*>(input), sizeof(input));
    ASSERT_EQ(0, ret);
    napi_value src = nullptr;
    napi_create_typedarray(env, napi_uint8_array, arrayBufferSize, arrayBuffer, 0, &src);

    napi_value result = base64.Encode(env, src);
    bool res = false;
    napi_is_promise(env, result, &res);
    ASSERT_TRUE(res);
}

/* @tc.name: encodeAsyncTest002
 * @tc.desc: Asynchronously encodes all bytes in the specified u8 array
             into the newly allocated u8 array using the Base64 encoding scheme.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, encodeAsyncTest002, testing::ext::TestSize.Level0)
{
    HILOG_INFO("encodeAsyncTest002 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Base64 base64;
    unsigned char input[14] = {66, 97, 115, 101, 54, 52, 32, 78, 111, 100, 101, 46, 106, 115};
    napi_value arrayBuffer = nullptr;
    void* data = nullptr;
    size_t arrayBufferSize = 14;
    napi_create_arraybuffer(env, arrayBufferSize, &data, &arrayBuffer);
    int ret = memcpy_s(data, sizeof(input), reinterpret_cast<void*>(input), sizeof(input));
    ASSERT_EQ(0, ret);
    napi_value src = nullptr;
    napi_create_typedarray(env, napi_uint8_array, arrayBufferSize, arrayBuffer, 0, &src);

    napi_value result = base64.Encode(env, src);
    bool res = false;
    napi_is_promise(env, result, &res);
    ASSERT_TRUE(res);
}

/* @tc.name: encodeAsyncTest003
 * @tc.desc: Asynchronously encodes all bytes in the specified u8 array
             into the newly allocated u8 array using the Base64 encoding scheme.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, encodeAsyncTest003, testing::ext::TestSize.Level0)
{
    HILOG_INFO("encodeAsyncTest003 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Base64 base64;
    unsigned char input[26] = {66, 97, 115, 101, 54, 52, 32, 69, 110,
                              99, 111, 100, 105, 110, 103, 32, 105, 110, 32, 78, 111, 100, 101, 46, 106, 115};;
    napi_value arrayBuffer = nullptr;
    void* data = nullptr;
    size_t arrayBufferSize = 26;
    napi_create_arraybuffer(env, arrayBufferSize, &data, &arrayBuffer);
    int ret = memcpy_s(data, sizeof(input), reinterpret_cast<void*>(input), sizeof(input));
    ASSERT_EQ(0, ret);
    napi_value src = nullptr;
    napi_create_typedarray(env, napi_uint8_array, arrayBufferSize, arrayBuffer, 0, &src);

    napi_value result = base64.Encode(env, src);
    bool res = false;
    napi_is_promise(env, result, &res);
    ASSERT_TRUE(res);
}

/* @tc.name: encodeAsyncTest004
 * @tc.desc: Asynchronously encodes all bytes in the specified u8 array
             into the newly allocated u8 array using the Base64 encoding scheme.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, encodeAsyncTest004, testing::ext::TestSize.Level0)
{
    HILOG_INFO("encodeAsyncTest004 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Base64 base64;
    unsigned char input[4] = {168, 174, 155, 255};
    napi_value arrayBuffer = nullptr;
    void* data = nullptr;
    size_t arrayBufferSize = 4;
    napi_create_arraybuffer(env, arrayBufferSize, &data, &arrayBuffer);
    int ret = memcpy_s(data, sizeof(input), reinterpret_cast<void*>(input), sizeof(input));
    ASSERT_EQ(0, ret);
    napi_value src = nullptr;
    napi_create_typedarray(env, napi_uint8_array, arrayBufferSize, arrayBuffer, 0, &src);

    napi_value result = base64.Encode(env, src);
    bool res = false;
    napi_is_promise(env, result, &res);
    ASSERT_TRUE(res);
}

/* @tc.name: encodeAsyncTest005
 * @tc.desc: Asynchronously encodes all bytes in the specified u8 array
             into the newly allocated u8 array using the Base64 encoding scheme.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, encodeAsyncTest005, testing::ext::TestSize.Level0)
{
    HILOG_INFO("encodeAsyncTest005 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Base64 base64;
    unsigned char input[6] = {66, 97, 115, 101, 54, 52};
    napi_value arrayBuffer = nullptr;
    void* data = nullptr;
    size_t arrayBufferSize = 6;
    napi_create_arraybuffer(env, arrayBufferSize, &data, &arrayBuffer);
    int ret = memcpy_s(data, sizeof(input), reinterpret_cast<void*>(input), sizeof(input));
    ASSERT_EQ(0, ret);
    napi_value src = nullptr;
    napi_create_typedarray(env, napi_uint8_array, arrayBufferSize, arrayBuffer, 0, &src);

    napi_value result = base64.Encode(env, src);
    bool res = false;
    napi_is_promise(env, result, &res);
    ASSERT_TRUE(res);
}

/* @tc.name: encodeToStringAsyncTest001
 * @tc.desc: Asynchronously encodes the specified byte array into a String using the Base64 encoding scheme.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, encodeToStringAsyncTest001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("encodeToStringAsyncTest001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Base64 base64;

    unsigned char input[3] = {115, 49, 51};
    napi_value arrayBuffer = nullptr;
    size_t arrayBufferSize = 3;
    void* data = nullptr;
    napi_create_arraybuffer(env, arrayBufferSize, &data, &arrayBuffer);
    int ret = memcpy_s(data, sizeof(input), reinterpret_cast<void*>(input), sizeof(input));
    ASSERT_EQ(0, ret);
    napi_value src = nullptr;
    napi_create_typedarray(env, napi_uint8_array, arrayBufferSize, arrayBuffer, 0, &src);
    napi_value result = base64.EncodeToString(env, src);
    bool res = false;
    napi_is_promise(env, result, &res);
    ASSERT_TRUE(res);
}

/* @tc.name: encodeToStringAsyncTest002
 * @tc.desc: Asynchronously encodes the specified byte array into a String using the Base64 encoding scheme.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, encodeToStringAsyncTest002, testing::ext::TestSize.Level0)
{
    HILOG_INFO("encodeToStringAsyncTest002 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Base64 base64;
    unsigned char input[14] = {66, 97, 115, 101, 54, 52, 32, 78, 111, 100, 101, 46, 106, 115};
    napi_value arrayBuffer = nullptr;
    void* data = nullptr;
    size_t arrayBufferSize = 14;
    napi_create_arraybuffer(env, arrayBufferSize, &data, &arrayBuffer);
    int ret = memcpy_s(data, sizeof(input), reinterpret_cast<void*>(input), sizeof(input));
    ASSERT_EQ(0, ret);
    napi_value src = nullptr;
    napi_create_typedarray(env, napi_uint8_array, arrayBufferSize, arrayBuffer, 0, &src);

    napi_value result = base64.EncodeToString(env, src);
    bool res = false;
    napi_is_promise(env, result, &res);
    ASSERT_TRUE(res);
}

/* @tc.name: encodeToStringAsyncTest003
 * @tc.desc: Asynchronously encodes the specified byte array into a String using the Base64 encoding scheme.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, encodeToStringAsyncTest003, testing::ext::TestSize.Level0)
{
    HILOG_INFO("encodeToStringAsyncTest003 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Base64 base64;
    unsigned char input[26] = {66, 97, 115, 101, 54, 52, 32, 69, 110,
                                99, 111, 100, 105, 110, 103, 32, 105, 110, 32, 78, 111, 100, 101, 46, 106, 115};
    napi_value arrayBuffer = nullptr;
    void* data = nullptr;
    size_t arrayBufferSize = 26;
    napi_create_arraybuffer(env, arrayBufferSize, &data, &arrayBuffer);
    int ret = memcpy_s(data, sizeof(input), reinterpret_cast<void*>(input), sizeof(input));
    ASSERT_EQ(0, ret);
    napi_value src = nullptr;
    napi_create_typedarray(env, napi_uint8_array, arrayBufferSize, arrayBuffer, 0, &src);

    napi_value result = base64.EncodeToString(env, src);
    bool res = false;
    napi_is_promise(env, result, &res);
    ASSERT_TRUE(res);
}

/* @tc.name: encodeToStringAsyncTest004
 * @tc.desc: Asynchronously encodes the specified byte array into a String using the Base64 encoding scheme.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, encodeToStringAsyncTest004, testing::ext::TestSize.Level0)
{
    HILOG_INFO("encodeToStringAsyncTest004 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Base64 base64;
    unsigned char input[4] = {168, 174, 155, 255};
    napi_value arrayBuffer = nullptr;
    void* data = nullptr;
    size_t arrayBufferSize = 4;
    napi_create_arraybuffer(env, arrayBufferSize, &data, &arrayBuffer);
    int ret = memcpy_s(data, sizeof(input), reinterpret_cast<void*>(input), sizeof(input));
    ASSERT_EQ(0, ret);
    napi_value src = nullptr;
    napi_create_typedarray(env, napi_uint8_array, arrayBufferSize, arrayBuffer, 0, &src);

    napi_value result = base64.EncodeToString(env, src);
    bool res = false;
    napi_is_promise(env, result, &res);
    ASSERT_TRUE(res);
}

/* @tc.name: encodeToStringAsyncTest005
 * @tc.desc: Asynchronously encodes the specified byte array into a String using the Base64 encoding scheme.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, encodeToStringAsyncTest005, testing::ext::TestSize.Level0)
{
    HILOG_INFO("encodeToStringAsyncTest005 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Base64 base64;
    unsigned char input[6] = {66, 97, 115, 101, 54, 52};
    napi_value arrayBuffer = nullptr;
    void* data = nullptr;
    size_t arrayBufferSize = 6;
    napi_create_arraybuffer(env, arrayBufferSize, &data, &arrayBuffer);
    int ret = memcpy_s(data, sizeof(input), reinterpret_cast<void*>(input), sizeof(input));
    ASSERT_EQ(0, ret);
    napi_value src = nullptr;
    napi_create_typedarray(env, napi_uint8_array, arrayBufferSize, arrayBuffer, 0, &src);

    napi_value result = base64.EncodeToString(env, src);
    bool res = false;
    napi_is_promise(env, result, &res);
    ASSERT_TRUE(res);
}

/* @tc.name: decodeAsyncTest001
 * @tc.desc: Use the Base64 encoding scheme to asynchronously decode a
             Base64-encoded string or input u8 array into a newly allocated u8 array.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decodeAsyncTest001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decodeAsyncTest001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Base64 base64;

    unsigned char input[4] = {99, 122, 69, 122};
    napi_value arrayBuffer = nullptr;
    size_t arrayBufferSize = 4;
    void* data = nullptr;
    napi_create_arraybuffer(env, arrayBufferSize, &data, &arrayBuffer);
    int ret = memcpy_s(data, sizeof(input), reinterpret_cast<void*>(input), sizeof(input));
    ASSERT_EQ(0, ret);
    napi_value src = nullptr;
    napi_create_typedarray(env, napi_uint8_array, arrayBufferSize, arrayBuffer, 0, &src);
    napi_value result = base64.Decode(env, src);
    bool res = false;
    napi_is_promise(env, result, &res);
    ASSERT_TRUE(res);
}

/* @tc.name: decodeAsyncTest002
 * @tc.desc: Use the Base64 encoding scheme to asynchronously decode a
             Base64-encoded string or input u8 array into a newly allocated u8 array.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decodeAsyncTest002, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decodeAsyncTest002 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Base64 base64;

    unsigned char input[8] = {113, 75, 54, 98, 47, 119, 61, 61};
    napi_value arrayBuffer = nullptr;
    size_t arrayBufferSize = 8;
    void* data = nullptr;
    napi_create_arraybuffer(env, arrayBufferSize, &data, &arrayBuffer);
    int ret = memcpy_s(data, sizeof(input), reinterpret_cast<void*>(input), sizeof(input));
    ASSERT_EQ(0, ret);
    napi_value src = nullptr;
    napi_create_typedarray(env, napi_uint8_array, arrayBufferSize, arrayBuffer, 0, &src);
    napi_value result = base64.Decode(env, src);
    bool res = false;
    napi_is_promise(env, result, &res);
    ASSERT_TRUE(res);
}

/* @tc.name: decodeAsyncTest003
 * @tc.desc: Use the Base64 encoding scheme to asynchronously decode a
             Base64-encoded string or input u8 array into a newly allocated u8 array.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decodeAsyncTest003, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decodeAsyncTest003 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Base64 base64;

    std::string input = "czEz";
    napi_value src = nullptr;
    napi_create_string_utf8(env, input.c_str(), input.size(), &src);
    napi_value result = base64.Decode(env, src);
    bool res = false;
    napi_is_promise(env, result, &res);
    ASSERT_TRUE(res);
}

/* @tc.name: decodeAsyncTest004
 * @tc.desc: Use the Base64 encoding scheme to asynchronously decode a
             Base64-encoded string or input u8 array into a newly allocated u8 array.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decodeAsyncTest004, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decodeAsyncTest004 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Base64 base64;

    std::string input = "QmFzZTY0IEVuY29kaW5nIGluIE5vZGUuanM=";
    napi_value src = nullptr;
    napi_create_string_utf8(env, input.c_str(), input.size(), &src);
    napi_value result = base64.Decode(env, src);
    bool res = false;
    napi_is_promise(env, result, &res);
    ASSERT_TRUE(res);
}

/* @tc.name: decodeAsyncTest005
 * @tc.desc: Use the Base64 encoding scheme to asynchronously decode a
             Base64-encoded string or input u8 array into a newly allocated u8 array.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decodeAsyncTest005, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decodeAsyncTest005 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Base64 base64;

    std::string input = "qK6b/w==";
    napi_value src = nullptr;
    napi_create_string_utf8(env, input.c_str(), input.size(), &src);
    napi_value result = base64.Decode(env, src);
    bool res = false;
    napi_is_promise(env, result, &res);
    ASSERT_TRUE(res);
}