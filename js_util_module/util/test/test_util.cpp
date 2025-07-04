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
#include "commonlibrary/ets_utils/js_util_module/util/js_base64.h"
#include "commonlibrary/ets_utils/js_util_module/util/js_uuid.h"
#include "commonlibrary/ets_utils/js_util_module/util/js_stringdecoder.h"
#include "commonlibrary/ets_utils/js_util_module/util/js_textencoder.h"
#include "commonlibrary/ets_utils/js_util_module/util/js_textdecoder.h"
#include "commonlibrary/ets_utils/js_util_module/util/js_types.h"
#include "ohos/init_data.h"
#include "tools/log.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "securec.h"


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

/* @tc.name: DoParseUUIDTest004
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest004, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    ASSERT_EQ(arr, nullptr);
}

/* @tc.name: HexToCharUUIDTest001
 * @tc.desc: Hex to char with g convert to x.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, HexToCharUUIDTest001, testing::ext::TestSize.Level0)
{
    unsigned char input = 'g';
    unsigned char res = OHOS::Util::HexToChar(input);
    ASSERT_EQ(res, 'x');
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
    textEncoder.SetOrgEncoding("GBK");
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
    textEncoder.SetOrgEncoding("gb18030");
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
    textEncoder.SetOrgEncoding("GB18030");
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
 * @tc.name: textEncodeTest004
 * @tc.desc: Test encode src.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textEncodeTest004, testing::ext::TestSize.Level0)
{
    SetHwIcuDirectory();
    napi_env env = (napi_env)engine_;
    OHOS::Util::TextEncoder textEncoder("gbk");

    std::string input = "abc123";
    napi_value src = nullptr;
    napi_create_string_utf8(env, input.c_str(), input.size(), &src);
    napi_value result = textEncoder.Encode(env, src);

    char excepted[7] = {0x61, 0x62, 0x63, 0x31, 0x32, 0x33, 0}; // 7:nums of args

    napi_typedarray_type type;
    size_t srcLength = 0;
    void *srcData = nullptr;
    napi_value srcBuffer = nullptr;
    size_t byteOffset = 0;

    napi_get_typedarray_info(
        env, result, &type, &srcLength, &srcData, &srcBuffer, &byteOffset);

    ASSERT_EQ(srcLength, 6); // 6:string length
    char *res = reinterpret_cast<char*>(srcData);

    res[srcLength] = 0;
    ASSERT_STREQ(res, excepted);
}

/**
 * @tc.name: textEncodeTest005
 * @tc.desc: Test encode src.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textEncodeTest005, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::Util::TextEncoder textEncoder("utf-8");
    napi_value src = nullptr;
    napi_value result = textEncoder.Encode(env, src);
    ASSERT_TRUE(result == nullptr);
}

/**
 * @tc.name: textEncodeTest006
 * @tc.desc: Test encode src.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textEncodeTest006, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textEncodeTest006 start");
    SetHwIcuDirectory();
    napi_env env = (napi_env)engine_;
    OHOS::Util::TextEncoder textEncoder("big5");

    std::string input = "abc哈熠";
    napi_value src = nullptr;
    napi_create_string_utf8(env, input.c_str(), input.size(), &src);
    napi_value result = textEncoder.Encode(env, src);

    char excepted[8] = {0x61, 0x62, 0x63, 0xAB, 0xA2, 0xE6, 0x66, 0}; // 8:nums of args

    napi_typedarray_type type;
    size_t srcLength = 0;
    void *srcData = nullptr;
    napi_value srcBuffer = nullptr;
    size_t byteOffset = 0;

    napi_get_typedarray_info(
        env, result, &type, &srcLength, &srcData, &srcBuffer, &byteOffset);

    ASSERT_EQ(srcLength, 7); // 7:string length
    char *res = reinterpret_cast<char*>(srcData);

    res[srcLength] = 0;
    ASSERT_STREQ(res, excepted);
}

/**
 * @tc.name: textEncodeTest007
 * @tc.desc: Test encode src.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textEncodeTest007, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textEncodeTest007 start");
    SetHwIcuDirectory();
    napi_env env = (napi_env)engine_;
    OHOS::Util::TextEncoder textEncoder("shift_jis");

    std::string input = "abc哈熠";
    napi_value src = nullptr;
    napi_create_string_utf8(env, input.c_str(), input.size(), &src);
    napi_value result = textEncoder.Encode(env, src);

    char excepted[8] = {0x61, 0x62, 0x63, 0x99, 0xFB, 0xFC, 0xFC, 0}; // 8:nums of args

    napi_typedarray_type type;
    size_t srcLength = 0;
    void *srcData = nullptr;
    napi_value srcBuffer = nullptr;
    size_t byteOffset = 0;

    napi_get_typedarray_info(
        env, result, &type, &srcLength, &srcData, &srcBuffer, &byteOffset);

    ASSERT_EQ(srcLength, 7); // 7:string length
    char *res = reinterpret_cast<char*>(srcData);

    res[srcLength] = 0;
    ASSERT_STREQ(res, excepted);
}

/**
 * @tc.name: textEncodeTest008
 * @tc.desc: Test encode src.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textEncodeTest008, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textEncodeTest008 start");
    SetHwIcuDirectory();
    napi_env env = (napi_env)engine_;
    OHOS::Util::TextEncoder textEncoder("iso-2022-jp");

    std::string input = "abc哈熠";
    napi_value src = nullptr;
    napi_create_string_utf8(env, input.c_str(), input.size(), &src);
    napi_value result = textEncoder.Encode(env, src);

    char excepted[13] = {0x61, 0x62, 0x63, 0x1B, 0x24, 0x42, 0x52, 0x7D, 0x1B, 0x28, 0x42, 0x1A, 0}; // 13:nums of args

    napi_typedarray_type type;
    size_t srcLength = 0;
    void *srcData = nullptr;
    napi_value srcBuffer = nullptr;
    size_t byteOffset = 0;

    napi_get_typedarray_info(
        env, result, &type, &srcLength, &srcData, &srcBuffer, &byteOffset);

    ASSERT_EQ(srcLength, 12); // 12:string length
    char *res = reinterpret_cast<char*>(srcData);

    res[srcLength] = 0;
    ASSERT_STREQ(res, excepted);
}

/**
 * @tc.name: textEncodeTest009
 * @tc.desc: Test encode src.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textEncodeTest009, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textEncodeTest009 start");
    SetHwIcuDirectory();
    napi_env env = (napi_env)engine_;
    OHOS::Util::TextEncoder textEncoder("ibm866");

    std::string input = "abc哈熠";
    napi_value src = nullptr;
    napi_create_string_utf8(env, input.c_str(), input.size(), &src);
    napi_value result = textEncoder.Encode(env, src);

    char excepted[6] = {0x61, 0x62, 0x63, 0x7F, 0x7F, 0}; // 6:nums of args

    napi_typedarray_type type;
    size_t srcLength = 0;
    void *srcData = nullptr;
    napi_value srcBuffer = nullptr;
    size_t byteOffset = 0;

    napi_get_typedarray_info(
        env, result, &type, &srcLength, &srcData, &srcBuffer, &byteOffset);

    ASSERT_EQ(srcLength, 5); // 5:string length
    char *res = reinterpret_cast<char*>(srcData);

    res[srcLength] = 0;
    ASSERT_STREQ(res, excepted);
}

/**
 * @tc.name: textEncodeTest010
 * @tc.desc: Test encode src.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textEncodeTest010, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textEncodeTest010 start");
    SetHwIcuDirectory();
    napi_env env = (napi_env)engine_;
    OHOS::Util::TextEncoder textEncoder("macintosh");

    std::string input = "abc哈熠";
    napi_value src = nullptr;
    napi_create_string_utf8(env, input.c_str(), input.size(), &src);
    napi_value result = textEncoder.Encode(env, src);

    char excepted[6] = {0x61, 0x62, 0x63, 0x3F, 0x3F, 0}; // 6:nums of args

    napi_typedarray_type type;
    size_t srcLength = 0;
    void *srcData = nullptr;
    napi_value srcBuffer = nullptr;
    size_t byteOffset = 0;

    napi_get_typedarray_info(
        env, result, &type, &srcLength, &srcData, &srcBuffer, &byteOffset);

    ASSERT_EQ(srcLength, 5); // 5:string length
    char *res = reinterpret_cast<char*>(srcData);

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

    ASSERT_EQ(resRead, static_cast<uint32_t>(6));
    ASSERT_EQ(resWritten, static_cast<uint32_t>(6));
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

    ASSERT_EQ(resRead, static_cast<uint32_t>(4));
    ASSERT_EQ(resWritten, static_cast<uint32_t>(4));
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

    ASSERT_EQ(resRead, static_cast<uint32_t>(5));
    ASSERT_EQ(resWritten, static_cast<uint32_t>(5));
}

/**
 * @tc.name: textEncodeIntoTest004
 * @tc.desc: Test returns a dictionary object indicating the progress of the encoding
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textEncodeIntoTest004, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textEncodeIntoTest004 start");
    SetHwIcuDirectory();
    napi_env env = (napi_env)engine_;
    OHOS::Util::TextEncoder textEncoder("big5");

    std::string input = "abc123";
    napi_value src = nullptr;
    napi_create_string_utf8(env, input.c_str(), input.size(), &src);

    napi_value arrayBuffer = nullptr;
    void* arrayBufferPtr = nullptr;
    size_t arrayBufferSize = 20;
    napi_create_arraybuffer(env, arrayBufferSize, &arrayBufferPtr, &arrayBuffer);

    napi_value dest = nullptr;
        napi_create_typedarray(env, napi_uint8_array, arrayBufferSize, arrayBuffer, 0, &dest);

    napi_value result = textEncoder.EncodeInto(env, src, dest);

    napi_value read = nullptr;
    napi_get_named_property(env, result, "read", &read);

    uint32_t resRead = 0;

    napi_get_value_uint32(env, read, &resRead);

    napi_value written = nullptr;
    napi_get_named_property(env, result, "written", &written);

    uint32_t resWritten = 0;
    napi_get_value_uint32(env, read, &resWritten);

    ASSERT_EQ(resRead, static_cast<uint32_t>(6));
    ASSERT_EQ(resWritten, static_cast<uint32_t>(6));
}

/**
 * @tc.name: textEncodeIntoTest005
 * @tc.desc: Test returns a dictionary object indicating the progress of the encoding
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textEncodeIntoTest005, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textEncodeIntoTest005 start");
    SetHwIcuDirectory();
    napi_env env = (napi_env)engine_;
    OHOS::Util::TextEncoder textEncoder("shift_jis");

    std::string input = "abc123哈";
    napi_value src = nullptr;
    napi_create_string_utf8(env, input.c_str(), input.size(), &src);

    napi_value arrayBuffer = nullptr;
    void* arrayBufferPtr = nullptr;
    size_t arrayBufferSize = 20;
    napi_create_arraybuffer(env, arrayBufferSize, &arrayBufferPtr, &arrayBuffer);

    napi_value dest = nullptr;
        napi_create_typedarray(env, napi_uint8_array, arrayBufferSize, arrayBuffer, 0, &dest);

    napi_value result = textEncoder.EncodeInto(env, src, dest);

    napi_value read = nullptr;
    napi_get_named_property(env, result, "read", &read);

    uint32_t resRead = 0;

    napi_get_value_uint32(env, read, &resRead);

    napi_value written = nullptr;
    napi_get_named_property(env, result, "written", &written);

    uint32_t resWritten = 0;
    napi_get_value_uint32(env, read, &resWritten);

    ASSERT_EQ(resRead, static_cast<uint32_t>(7));
    ASSERT_EQ(resWritten, static_cast<uint32_t>(7));
}

/**
 * @tc.name: textEncodeIntoTest006
 * @tc.desc: Test returns a dictionary object indicating the progress of the encoding
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textEncodeIntoTest006, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textEncodeIntoTest006 start");
    SetHwIcuDirectory();
    napi_env env = (napi_env)engine_;
    OHOS::Util::TextEncoder textEncoder("iso-2022-jp");

    std::string input = "abc123哈";
    napi_value src = nullptr;
    napi_create_string_utf8(env, input.c_str(), input.size(), &src);

    napi_value arrayBuffer = nullptr;
    void* arrayBufferPtr = nullptr;
    size_t arrayBufferSize = 20;
    napi_create_arraybuffer(env, arrayBufferSize, &arrayBufferPtr, &arrayBuffer);

    napi_value dest = nullptr;
        napi_create_typedarray(env, napi_uint8_array, arrayBufferSize, arrayBuffer, 0, &dest);

    napi_value result = textEncoder.EncodeInto(env, src, dest);

    napi_value read = nullptr;
    napi_get_named_property(env, result, "read", &read);

    uint32_t resRead = 0;

    napi_get_value_uint32(env, read, &resRead);

    napi_value written = nullptr;
    napi_get_named_property(env, result, "written", &written);

    uint32_t resWritten = 0;
    napi_get_value_uint32(env, read, &resWritten);

    ASSERT_EQ(resRead, static_cast<uint32_t>(7));
    ASSERT_EQ(resWritten, static_cast<uint32_t>(7));
}

/**
 * @tc.name: textEncodeIntoTest007
 * @tc.desc: Test returns a dictionary object indicating the progress of the encoding
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textEncodeIntoTest007, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textEncodeIntoTest007 start");
    SetHwIcuDirectory();
    napi_env env = (napi_env)engine_;
    OHOS::Util::TextEncoder textEncoder("ibm866");

    std::string input = "abc123哈";
    napi_value src = nullptr;
    napi_create_string_utf8(env, input.c_str(), input.size(), &src);

    napi_value arrayBuffer = nullptr;
    void* arrayBufferPtr = nullptr;
    size_t arrayBufferSize = 20;
    napi_create_arraybuffer(env, arrayBufferSize, &arrayBufferPtr, &arrayBuffer);

    napi_value dest = nullptr;
        napi_create_typedarray(env, napi_uint8_array, arrayBufferSize, arrayBuffer, 0, &dest);

    napi_value result = textEncoder.EncodeInto(env, src, dest);

    napi_value read = nullptr;
    napi_get_named_property(env, result, "read", &read);

    uint32_t resRead = 0;

    napi_get_value_uint32(env, read, &resRead);

    napi_value written = nullptr;
    napi_get_named_property(env, result, "written", &written);

    uint32_t resWritten = 0;
    napi_get_value_uint32(env, read, &resWritten);

    ASSERT_EQ(resRead, static_cast<uint32_t>(7));
    ASSERT_EQ(resWritten, static_cast<uint32_t>(7));
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
    int32_t flags = static_cast<int>(OHOS::Util::TextDecoder::ConverterFlags::FATAL_FLG) |
        static_cast<int>(OHOS::Util::TextDecoder::ConverterFlags::IGNORE_BOM_FLG);
    std::string str = "utf-8";
    OHOS::Util::TextDecoder textDecoder(str, flags);
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
    int32_t flags = static_cast<int>(OHOS::Util::TextDecoder::ConverterFlags::FATAL_FLG);
    std::string str = "utf-8";
    OHOS::Util::TextDecoder textDecoder(str, flags);
    bool iflag = true;
    size_t byteLength = 5;
    void* data = nullptr;
    napi_value resultBuff = nullptr;
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    unsigned char arr[5] = {0x61, '\0', 0x62, 0x63, '\0'};
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
    ASSERT_STREQ("a bc ", ch);
    if (ch != nullptr) {
        delete []ch;
        ch = nullptr;
    }
}

/**
 * @tc.name: decoderUtf8003 utf-8
 * @tc.desc: Test date type.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decoderUtf8003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    int32_t flags = static_cast<int>(OHOS::Util::TextDecoder::ConverterFlags::FATAL_FLG) |
        static_cast<int>(OHOS::Util::TextDecoder::ConverterFlags::IGNORE_BOM_FLG);
    std::string str = "utf-8";
    OHOS::Util::TextDecoder textDecoder(str, flags);
    bool iflag = false;
    size_t byteLength = 0;
    void *data = nullptr;
    napi_value resultBuff = nullptr;
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    napi_value result2 = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &result2);
    napi_value testString = textDecoder.Decode(env, result2, iflag);
    ASSERT_TRUE(testString == nullptr);
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
    int32_t flags = 0;
    std::string str = "utf-16le";
    OHOS::Util::TextDecoder textDecoder(str, flags);
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
    int32_t flags = static_cast<int>(OHOS::Util::TextDecoder::ConverterFlags::IGNORE_BOM_FLG);
    std::string str = "utf-16le";
    OHOS::Util::TextDecoder textDecoder(str, flags);
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
    int32_t flags = 0;
    std::string str = "utf-16le";
    OHOS::Util::TextDecoder textDecoder(str, flags);
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
    ASSERT_EQ(0xFEFF, static_cast<int>(tempU16str02[0]));
    ASSERT_EQ(0x61, static_cast<int>(tempU16str02[1]));
    ASSERT_EQ(0x62, static_cast<int>(tempU16str02[2]));
    ASSERT_EQ(0x63, static_cast<int>(tempU16str02[3]));
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
    int32_t flags = static_cast<int>(OHOS::Util::TextDecoder::ConverterFlags::FATAL_FLG) |
        static_cast<int>(OHOS::Util::TextDecoder::ConverterFlags::IGNORE_BOM_FLG);
    std::string str = "utf-16le";
    OHOS::Util::TextDecoder textDecoder(str, flags);
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
    ASSERT_EQ(0xFEFF, static_cast<int>(tempU16str02[0]));
    ASSERT_EQ(0x61, static_cast<int>(tempU16str02[1]));
    ASSERT_EQ(0x62, static_cast<int>(tempU16str02[2]));
    ASSERT_EQ(0x63, static_cast<int>(tempU16str02[3]));
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
    int32_t flags = 0;
    std::string str = "utf-16be";
    OHOS::Util::TextDecoder textDecoder(str, flags);
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
    int32_t flags = 0;
    std::string str = "utf-16be";
    OHOS::Util::TextDecoder textDecoder(str, flags);
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
    ASSERT_EQ(0xFEFF, static_cast<int>(tempU16str02[0]));
    ASSERT_EQ(0x61, static_cast<int>(tempU16str02[1]));
    ASSERT_EQ(0x62, static_cast<int>(tempU16str02[2]));
    ASSERT_EQ(0x63, static_cast<int>(tempU16str02[3]));
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
    int32_t flags = static_cast<int>(OHOS::Util::TextDecoder::ConverterFlags::IGNORE_BOM_FLG);
    std::string str = "utf-16be";
    OHOS::Util::TextDecoder textDecoder(str, flags);
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
    ASSERT_EQ(0xFEFF, static_cast<int>(tempU16str02[0]));
    ASSERT_EQ(0x61, static_cast<int>(tempU16str02[1]));
    ASSERT_EQ(0x62, static_cast<int>(tempU16str02[2]));
    ASSERT_EQ(0x63, static_cast<int>(tempU16str02[3]));
    if (ch != nullptr) {
        delete []ch;
        ch = nullptr;
    }
}

/**
 * @tc.name: decoderUtf8-BOM001
 * @tc.desc: Testing the decoding result of UTF-8 data with BOM.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decoderUtf8BOM001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decoderUtf8BOM001 start");
    napi_env env = (napi_env)engine_;
    int32_t flags = static_cast<int>(OHOS::Util::TextDecoder::ConverterFlags::IGNORE_BOM_FLG);
    std::string encoding = "utf-8";
    OHOS::Util::TextDecoder textDecoder(encoding, flags);
    bool iflag = true;
    size_t byteLength = 6;
    void* data = nullptr;
    napi_value resultBuff = nullptr;
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    unsigned char arr[8] = {0xEF, 0xBB, 0xBF, 0x41, 0x42, 0x43};
    int ret = memcpy_s(data, sizeof(arr), reinterpret_cast<void*>(arr), sizeof(arr));
    ASSERT_EQ(0, ret);
    napi_value result = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &result);
    napi_value testString = textDecoder.DecodeToString(env, result, iflag);
    size_t bufferSize = 0;
    napi_get_value_string_utf16(env, testString, nullptr, 0, &bufferSize);
    size_t length = 0;
    char16_t* ch = nullptr;
    if (bufferSize > 0) {
        ch = new char16_t[bufferSize + 1]();
        napi_get_value_string_utf16(env, testString, ch, bufferSize + 1, &length);
    }
    std::string str =
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.to_bytes(ch);
    ASSERT_EQ(str, "ABC");
    if (ch != nullptr) {
        delete []ch;
        ch = nullptr;
    }
}

/**
 * @tc.name: decoderUtf8-BOM002
 * @tc.desc: Testing the decoding result of UTF-8 data with BOM.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decoderUtf8BOM002, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decoderUtf8BOM002 start");
    napi_env env = (napi_env)engine_;
    int32_t flags = 0;
    std::string encoding = "utf-8";
    OHOS::Util::TextDecoder textDecoder(encoding, flags);
    bool iflag = true;
    size_t byteLength = 6;
    void* data = nullptr;
    napi_value resultBuff = nullptr;
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    unsigned char arr[8] = {0xEF, 0xBB, 0xBF, 0x41, 0x42, 0x43};
    int ret = memcpy_s(data, sizeof(arr), reinterpret_cast<void*>(arr), sizeof(arr));
    ASSERT_EQ(0, ret);
    napi_value result = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &result);
    napi_value testString = textDecoder.DecodeToString(env, result, iflag);
    size_t bufferSize = 0;
    napi_get_value_string_utf16(env, testString, nullptr, 0, &bufferSize);
    size_t length = 0;
    char16_t* ch = nullptr;
    if (bufferSize > 0) {
        ch = new char16_t[bufferSize + 1]();
        napi_get_value_string_utf16(env, testString, ch, bufferSize + 1, &length);
    }
    std::string str =
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.to_bytes(ch);
    ASSERT_EQ(str, "\uFEFFABC");
    if (ch != nullptr) {
        delete []ch;
        ch = nullptr;
    }
}

/**
 * @tc.name: decoderUtf8-BOM003
 * @tc.desc: Decoder utf8 BOM with limit err.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decoderUtf8BOM003, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decoderUtf8BOM003 start");
    napi_env env = (napi_env)engine_;
    int32_t flags = static_cast<int>(OHOS::Util::TextDecoder::ConverterFlags::FATAL_FLG) |
        static_cast<int>(OHOS::Util::TextDecoder::ConverterFlags::IGNORE_BOM_FLG);
    std::string str = "utf-8";
    OHOS::Util::TextDecoder textDecoder(str, flags);
    bool iflag = false;
    size_t byteLength = 0;
    void *data = nullptr;
    napi_value resultBuff = nullptr;
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    napi_value result2 = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &result2);
    napi_value testString = textDecoder.DecodeToString(env, result2, iflag);
    ASSERT_TRUE(testString == nullptr);
}

/**
 * @tc.name: decoderUtf8-BOM004
 * @tc.desc: Testing the decoding result of UTF-8 data with BOM.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decoderUtf8BOM004, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decoderUtf8BOM004 start");
    napi_env env = (napi_env)engine_;
    int32_t flags = static_cast<int>(OHOS::Util::TextDecoder::ConverterFlags::IGNORE_BOM_FLG);
    std::string encoding = "utf-8";
    OHOS::Util::TextDecoder textDecoder(encoding, flags);
    bool iflag = false;
    size_t byteLength = 6;
    void* data = nullptr;
    napi_value resultBuff = nullptr;
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    unsigned char arr[8] = {0xEF, 0xBB, 0xBF, 0x41, 0x42, 0x43};
    int ret = memcpy_s(data, sizeof(arr), reinterpret_cast<void*>(arr), sizeof(arr));
    ASSERT_EQ(0, ret);
    napi_value result = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &result);
    napi_value testString = textDecoder.DecodeToString(env, result, iflag);
    size_t bufferSize = 0;
    napi_get_value_string_utf16(env, testString, nullptr, 0, &bufferSize);
    size_t length = 0;
    char16_t* ch = nullptr;
    if (bufferSize > 0) {
        ch = new char16_t[bufferSize + 1]();
        napi_get_value_string_utf16(env, testString, ch, bufferSize + 1, &length);
    }
    std::string str =
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.to_bytes(ch);
    ASSERT_EQ(str, "ABC");
    if (ch != nullptr) {
        delete []ch;
        ch = nullptr;
    }
}

/**
 * @tc.name: decoderUtf8-BOM005
 * @tc.desc: Testing the decoding result of UTF-8 data with BOM.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decoderUtf8BOM005, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decoderUtf8BOM005 start");
    napi_env env = (napi_env)engine_;
    int32_t flags = 0;
    std::string encoding = "utf-8";
    OHOS::Util::TextDecoder textDecoder(encoding, flags);
    bool iflag = true;
    size_t byteLength = 11;
    void* data = nullptr;
    napi_value resultBuff = nullptr;
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    unsigned char arr[11] = {0xEF, 0xBB, 0xBF, 0x41, 0x42, 0x43, 0x31, 0x32, 0x33, 0x34, 0x35};
    int ret = memcpy_s(data, sizeof(arr), reinterpret_cast<void*>(arr), sizeof(arr));
    ASSERT_EQ(0, ret);
    napi_value result = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &result);
    napi_value testString = textDecoder.DecodeToString(env, result, iflag);
    size_t bufferSize = 0;
    napi_get_value_string_utf16(env, testString, nullptr, 0, &bufferSize);
    size_t length = 0;
    char16_t* ch = nullptr;
    if (bufferSize > 0) {
        ch = new char16_t[bufferSize + 1]();
        napi_get_value_string_utf16(env, testString, ch, bufferSize + 1, &length);
    }
    std::string str =
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.to_bytes(ch);
    ASSERT_EQ(str, "\uFEFFABC12345");
    if (ch != nullptr) {
        delete[] ch;
        ch = nullptr;
    }
}

/**
 * @tc.name: decoderUtf8-BOM006
 * @tc.desc: Testing the decoding result of UTF-8 data with BOM.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decoderUtf8BOM006, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decoderUtf8BOM006 start");
    napi_env env = (napi_env)engine_;
    int32_t flags = 0;
    std::string encoding = "utf-8";
    OHOS::Util::TextDecoder textDecoder(encoding, flags);
    bool iflag = true;
    size_t byteLength = 12;
    void* data = nullptr;
    napi_value resultBuff = nullptr;
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    unsigned char arr[12] = {0xEF, 0xBB, 0xBF, 0x41, 0x42, 0x43, 0x61, 0x62, 0x63, 0x31, 0x32, 0x33};
    int ret = memcpy_s(data, sizeof(arr), reinterpret_cast<void*>(arr), sizeof(arr));
    ASSERT_EQ(0, ret);
    napi_value result = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &result);
    napi_value testString = textDecoder.DecodeToString(env, result, iflag);
    size_t bufferSize = 0;
    napi_get_value_string_utf16(env, testString, nullptr, 0, &bufferSize);
    size_t length = 0;
    char16_t* ch = nullptr;
    if (bufferSize > 0) {
        ch = new char16_t[bufferSize + 1]();
        napi_get_value_string_utf16(env, testString, ch, bufferSize + 1, &length);
    }
    std::string str =
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.to_bytes(ch);
    ASSERT_EQ(str, "\uFEFFABCabc123");
    if (ch != nullptr) {
        delete[] ch;
        ch = nullptr;
    }
}

/**
 * @tc.name: decoderUtf8-BOM007
 * @tc.desc: Testing the decoding result of UTF-8 data with BOM.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decoderUtf8BOM007, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decoderUtf8BOM007 start");
    napi_env env = (napi_env)engine_;
    int32_t flags = 0;
    std::string encoding = "utf-8";
    OHOS::Util::TextDecoder textDecoder(encoding, flags);
    bool iflag = true;
    size_t byteLength = 13;
    void* data = nullptr;
    napi_value resultBuff = nullptr;
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    unsigned char arr[13] = {0xEF, 0xBB, 0xBF, 0x41, 0x42, 0x43, 0x61, 0x62, 0x63, 0x31, 0x32, 0x33, 0x33};
    int ret = memcpy_s(data, sizeof(arr), reinterpret_cast<void*>(arr), sizeof(arr));
    ASSERT_EQ(0, ret);
    napi_value result = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &result);
    napi_value testString = textDecoder.DecodeToString(env, result, iflag);
    size_t bufferSize = 0;
    napi_get_value_string_utf16(env, testString, nullptr, 0, &bufferSize);
    size_t length = 0;
    char16_t* ch = nullptr;
    if (bufferSize > 0) {
        ch = new char16_t[bufferSize + 1]();
        napi_get_value_string_utf16(env, testString, ch, bufferSize + 1, &length);
    }
    std::string str =
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.to_bytes(ch);
    ASSERT_EQ(str, "\uFEFFABCabc1233");
    if (ch != nullptr) {
        delete[] ch;
        ch = nullptr;
    }
}

/**
 * @tc.name: decoderUtf8-BOM008
 * @tc.desc: Testing the decoding result of UTF-8 data with BOM.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decoderUtf8BOM008, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decoderUtf8BOM008 start");
    napi_env env = (napi_env)engine_;
    int32_t flags = static_cast<int>(OHOS::Util::TextDecoder::ConverterFlags::IGNORE_BOM_FLG);
    std::string encoding = "utf-8";
    OHOS::Util::TextDecoder textDecoder(encoding, flags);
    bool iflag = false;
    size_t byteLength = 200;
    void* data = nullptr;
    napi_value resultBuff = nullptr;
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    unsigned char arr[200] = {0xEF, 0xBB, 0xBF};
    for (size_t i = 3; i < 200; ++i) {
        arr[i] = static_cast<char>('A' + (i - 3) % 26);
    }
    int ret = memcpy_s(data, sizeof(arr), reinterpret_cast<void*>(arr), sizeof(arr));
    ASSERT_EQ(0, ret);
    napi_value result = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &result);
    napi_value testString = textDecoder.DecodeToString(env, result, iflag);
    size_t bufferSize = 0;
    napi_get_value_string_utf16(env, testString, nullptr, 0, &bufferSize);
    size_t length = 0;
    char16_t* ch = nullptr;
    if (bufferSize > 0) {
        ch = new char16_t[bufferSize + 1]();
        napi_get_value_string_utf16(env, testString, ch, bufferSize + 1, &length);
    }
    std::string str =
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.to_bytes(ch);
    std::string expectedStr;
    for (size_t i = 3; i < 200; ++i) {
        expectedStr += static_cast<char>('A' + (i - 3) % 26);
    }
    ASSERT_EQ(str, expectedStr);
    if (ch != nullptr) {
        delete []ch;
        ch = nullptr;
    }
}

/**
 * @tc.name: decoderUtf8-BOM009
 * @tc.desc: Testing the decoding result of UTF-8 data with BOM.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decoderUtf8BOM009, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decoderUtf8BOM009 start");
    napi_env env = (napi_env)engine_;
    int32_t flags = static_cast<int>(OHOS::Util::TextDecoder::ConverterFlags::IGNORE_BOM_FLG);
    std::string encoding = "utf-16";
    OHOS::Util::TextDecoder textDecoder(encoding, flags);
    bool iflag = true;
    size_t byteLength = 6;
    void* data = nullptr;
    napi_value resultBuff = nullptr;
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    unsigned char arr[8] = {0xEF, 0xBB, 0xBF, 0x41, 0x42, 0x43};
    int ret = memcpy_s(data, sizeof(arr), reinterpret_cast<void*>(arr), sizeof(arr));
    ASSERT_EQ(0, ret);
    napi_value result = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &result);
    napi_value testString = textDecoder.DecodeToString(env, result, iflag);
    size_t bufferSize = 0;
    napi_get_value_string_utf16(env, testString, nullptr, 0, &bufferSize);
    size_t length = 0;
    char16_t* ch = nullptr;
    if (bufferSize > 0) {
        ch = new char16_t[bufferSize + 1]();
        napi_get_value_string_utf16(env, testString, ch, bufferSize + 1, &length);
    }
    std::string str =
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.to_bytes(ch);
    ASSERT_TRUE(str != "");
    if (ch != nullptr) {
        delete []ch;
        ch = nullptr;
    }
}

/**
 * @tc.name:  decoderUtf8-BOM010
 * @tc.desc: Testing the decoding result of UTF-8 data with BOM.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decoderUtf8BOM010, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decoderUtf8-BOM010 start");
    napi_env env = (napi_env)engine_;
    int32_t flags = 0;
    std::string encoding = "utf-8";
    OHOS::Util::TextDecoder textDecoder(encoding, flags);
    bool iflag = true;
    size_t byteLength = 4;
    void* data = nullptr;
    bool flag = false;
    size_t strLength = 2;
    void* strData = nullptr;
    napi_value resultBuff = nullptr;
    napi_value strBuffer = nullptr;
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    unsigned char arr[4] = {0xE4, 0xBD, 0xA0, 0xE5};
    int ret = memcpy_s(data, sizeof(arr), reinterpret_cast<void*>(arr), sizeof(arr));
    ASSERT_EQ(0, ret);
    napi_value result = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &result);
    napi_value testString = textDecoder.DecodeToString(env, result, iflag);
    size_t bufferSize = 0;
    napi_get_value_string_utf16(env, testString, nullptr, 0, &bufferSize);
    size_t length = 0;
    char16_t* ch = nullptr;
    if (bufferSize > 0) {
        ch = new char16_t[bufferSize + 1]();
        napi_get_value_string_utf16(env, testString, ch, bufferSize + 1, &length);
    }
    std::string str =
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.to_bytes(ch);
    ASSERT_EQ(str, "你");
    if (ch != nullptr) {
        delete []ch;
        ch = nullptr;
    }
    napi_create_arraybuffer(env, strLength, &strData, &strBuffer);
    unsigned char arrayBuffer[2] = {0xA5, 0xBD};
    int strRet = memcpy_s(strData, sizeof(arrayBuffer), reinterpret_cast<void*>(arrayBuffer), sizeof(arrayBuffer));
    ASSERT_EQ(0, strRet);
    napi_value strResult = nullptr;
    napi_create_typedarray(env, napi_int8_array, strLength, strBuffer, 0, &strResult);
    napi_value strString = textDecoder.DecodeToString(env, strResult, flag);
    size_t strSize = 0;
    napi_get_value_string_utf16(env, strString, nullptr, 0, &strSize);
    size_t resultLength = 0;
    char16_t* temp = nullptr;
    if (strSize > 0) {
        temp = new char16_t[strSize + 1]();
        napi_get_value_string_utf16(env, strString, temp, strSize + 1, &resultLength);
    }
    std::string stringResult =
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.to_bytes(temp);
    ASSERT_EQ(stringResult, "好");
    if (temp != nullptr) {
        delete []temp;
        temp = nullptr;
    }
}

/**
 * @tc.name: getMinByteSizeTest001 utf-8
 * @tc.desc: get minbyte size with tranTool nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, getMinByteSizeTest001, testing::ext::TestSize.Level0)
{
    int32_t flags = static_cast<int>(OHOS::Util::TextDecoder::ConverterFlags::FATAL_FLG) |
        static_cast<int>(OHOS::Util::TextDecoder::ConverterFlags::IGNORE_BOM_FLG);
    std::string str = "XYZ123";
    OHOS::Util::TextDecoder textDecoder(str, flags);
    textDecoder.Reset();
    size_t rel1 = textDecoder.GetMinByteSize();
    ASSERT_EQ(rel1, 0);
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

    napi_value result = base64.EncodeSync(env, src, OHOS::Util::Type::BASIC);
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

    napi_value result = base64.EncodeSync(env, src, OHOS::Util::Type::BASIC);
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

    napi_value result = base64.EncodeSync(env, src, OHOS::Util::Type::BASIC);
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

    napi_value result = base64.EncodeSync(env, src, OHOS::Util::Type::BASIC);
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

    napi_value result = base64.EncodeSync(env, src, OHOS::Util::Type::BASIC);
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

/* @tc.name: encodeTest006
 * @tc.desc: Encode sync with napi_uint16_array.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, encodeTest006, testing::ext::TestSize.Level0)
{
    HILOG_INFO("encodeTest006 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Base64 base64;
    unsigned char input[6] = {66, 97, 115, 101, 54, 51};
    napi_value arrayBuffer = nullptr;
    void* data = nullptr;
    size_t arrayBufferSize = 6;
    napi_create_arraybuffer(env, arrayBufferSize, &data, &arrayBuffer);
    int ret = memcpy_s(data, sizeof(input), reinterpret_cast<void*>(input), sizeof(input));
    ASSERT_EQ(0, ret);
    napi_value src = nullptr;
    napi_create_typedarray(env, napi_uint16_array, arrayBufferSize, arrayBuffer, 0, &src);

    napi_value result = base64.EncodeSync(env, src, OHOS::Util::Type::BASIC);
    ASSERT_EQ(nullptr, result);
    napi_value result1 = base64.EncodeToStringSync(env, src, OHOS::Util::Type::BASIC);
    ASSERT_EQ(nullptr, result1);
    napi_value exception;
    napi_get_and_clear_last_exception(env, &exception);
}

/* @tc.name: encodeTest007
 * @tc.desc: Encodes all bytes in the specified u8 array with type BASIC_URL_SAFE.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, encodeTest007, testing::ext::TestSize.Level0)
{
    HILOG_INFO("encodeTest007 start");
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

    napi_value result = base64.EncodeSync(env, src, OHOS::Util::Type::BASIC_URL_SAFE);
    char excepted[7] = {113, 75, 54, 98, 95, 119};
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
    napi_value result = base64.EncodeToStringSync(env, src, OHOS::Util::Type::BASIC);
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
    napi_value result = base64.EncodeToStringSync(env, src, OHOS::Util::Type::BASIC);
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
    napi_value result = base64.EncodeToStringSync(env, src, OHOS::Util::Type::BASIC);
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
    napi_value result = base64.EncodeToStringSync(env, src, OHOS::Util::Type::BASIC);
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
    napi_value result = base64.EncodeToStringSync(env, src, OHOS::Util::Type::BASIC);
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
    napi_value result = base64.DecodeSync(env, src, OHOS::Util::Type::BASIC);
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
    napi_value result = base64.DecodeSync(env, src, OHOS::Util::Type::BASIC);
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
    napi_value result = base64.DecodeSync(env, src, OHOS::Util::Type::BASIC);
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
    napi_value result = base64.DecodeSync(env, src, OHOS::Util::Type::BASIC);
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
    napi_value result = base64.DecodeSync(env, src, OHOS::Util::Type::BASIC);
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

/* @tc.name: decodeTest006
 * @tc.desc: Decodes the Base64-encoded string or input unit32 array with return null.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decodeTest006, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decodeTest006 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Base64 base64;

    std::string input1 = "";
    napi_value src1 = nullptr;
    napi_create_string_utf8(env, input1.c_str(), input1.size(), &src1);
    base64.DecodeSync(env, src1, OHOS::Util::Type::BASIC_URL_SAFE);
    napi_value result1 = base64.DecodeSync(env, src1, OHOS::Util::Type::BASIC);
    ASSERT_EQ(result1, nullptr);
    napi_value exception;
    napi_get_and_clear_last_exception(env, &exception);

    unsigned char input[20] = {81, 109, 70, 122, 90, 84, 89, 48, 73, 69, 53, 118, 90, 71, 85, 117, 97, 110, 77, 61};
    napi_value arrayBuffer = nullptr;
    size_t arrayBufferSize = 20;
    void* data = nullptr;
    napi_create_arraybuffer(env, arrayBufferSize, &data, &arrayBuffer);
    int ret = memcpy_s(data, sizeof(input), reinterpret_cast<void*>(input), sizeof(input));
    ASSERT_EQ(0, ret);
    napi_value src = nullptr;
    napi_create_typedarray(env, napi_uint32_array, arrayBufferSize, arrayBuffer, 0, &src);
    napi_value result = base64.DecodeSync(env, src, OHOS::Util::Type::BASIC);
    ASSERT_EQ(result, nullptr);
    
    napi_get_and_clear_last_exception(env, &exception);
}

/* @tc.name: decodeTest007
 * @tc.desc: Decodes the Base64-encoded string with type BASIC_URL_SAFE.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decodeTest007, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decodeTest007 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Base64 base64;
    std::string input = "qK6b/w==";
    napi_value src = nullptr;
    napi_create_string_utf8(env, input.c_str(), input.size(), &src);
    napi_value result = base64.DecodeSync(env, src, OHOS::Util::Type::BASIC_URL_SAFE);
    ASSERT_EQ(nullptr, result);

    std::string input1 = "qK6b/w";
    napi_value src1 = nullptr;
    napi_create_string_utf8(env, input1.c_str(), input1.size(), &src1);
    napi_value result1 = base64.DecodeSync(env, src1, OHOS::Util::Type::BASIC_URL_SAFE);
    ASSERT_EQ(nullptr, result1);

    std::string input2 = "qK6b/w=";
    napi_value src2 = nullptr;
    napi_create_string_utf8(env, input2.c_str(), input2.size(), &src2);
    napi_value result2 = base64.DecodeSync(env, src2, OHOS::Util::Type::BASIC_URL_SAFE);
    ASSERT_EQ(nullptr, result2);

    napi_value exception;
    napi_get_and_clear_last_exception(env, &exception);
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

    napi_value result = base64.Encode(env, src, OHOS::Util::Type::BASIC);
    bool res = false;
    napi_is_promise(env, result, &res);
    ASSERT_TRUE(res);

    napi_value src1 = nullptr;
    napi_create_typedarray(env, napi_uint16_array, arrayBufferSize, arrayBuffer, 0, &src1);
    napi_value result1 = base64.Encode(env, src1, OHOS::Util::Type::BASIC);
    ASSERT_EQ(result1, nullptr);

    napi_value exception;
    napi_get_and_clear_last_exception(env, &exception);
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

    napi_value result = base64.Encode(env, src, OHOS::Util::Type::BASIC);
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
                               99, 111, 100, 105, 110, 103, 32, 105, 110, 32, 78, 111, 100, 101, 46, 106, 115};
    napi_value arrayBuffer = nullptr;
    void* data = nullptr;
    size_t arrayBufferSize = 26;
    napi_create_arraybuffer(env, arrayBufferSize, &data, &arrayBuffer);
    int ret = memcpy_s(data, sizeof(input), reinterpret_cast<void*>(input), sizeof(input));
    ASSERT_EQ(0, ret);
    napi_value src = nullptr;
    napi_create_typedarray(env, napi_uint8_array, arrayBufferSize, arrayBuffer, 0, &src);

    napi_value result = base64.Encode(env, src, OHOS::Util::Type::BASIC);
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

    napi_value result = base64.Encode(env, src, OHOS::Util::Type::BASIC);
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

    napi_value result = base64.Encode(env, src, OHOS::Util::Type::BASIC);
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
    napi_value result = base64.EncodeToString(env, src, OHOS::Util::Type::BASIC);
    bool res = false;
    napi_is_promise(env, result, &res);
    ASSERT_TRUE(res);

    napi_value src1 = nullptr;
    napi_create_typedarray(env, napi_uint16_array, arrayBufferSize, arrayBuffer, 0, &src1);
    napi_value result1 = base64.EncodeToString(env, src1, OHOS::Util::Type::BASIC);
    ASSERT_EQ(result1, nullptr);

    napi_value exception;
    napi_get_and_clear_last_exception(env, &exception);
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

    napi_value result = base64.EncodeToString(env, src, OHOS::Util::Type::BASIC);
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

    napi_value result = base64.EncodeToString(env, src, OHOS::Util::Type::BASIC);
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

    napi_value result = base64.EncodeToString(env, src, OHOS::Util::Type::BASIC);
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

    napi_value result = base64.EncodeToString(env, src, OHOS::Util::Type::BASIC);
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
    napi_value result = base64.Decode(env, src, OHOS::Util::Type::BASIC);
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
    napi_value result = base64.Decode(env, src, OHOS::Util::Type::BASIC);
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
    napi_value result = base64.Decode(env, src, OHOS::Util::Type::BASIC);
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
    napi_value result = base64.Decode(env, src, OHOS::Util::Type::BASIC);
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
    napi_value result = base64.Decode(env, src, OHOS::Util::Type::BASIC);
    bool res = false;
    napi_is_promise(env, result, &res);
    ASSERT_TRUE(res);
}

/* @tc.name: decodeAsyncTest006
 * @tc.desc: Use the Base64 encoding scheme to asynchronously decode a
             Base64-encoded string or input u8 array into a newly allocated u8 array.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decodeAsyncTest006, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decodeAsyncTest006 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Base64 base64;

    napi_value src = nullptr;
    napi_value result = base64.Decode(env, src, OHOS::Util::Type::BASIC);
    ASSERT_EQ(nullptr, result);

    std::string input1 = "";
    napi_value src1 = nullptr;
    napi_create_string_utf8(env, input1.c_str(), input1.size(), &src1);
    napi_value result1 = base64.Decode(env, src1, OHOS::Util::Type::BASIC);
    ASSERT_EQ(nullptr, result1);
    napi_value exception;
    napi_get_and_clear_last_exception(env, &exception);

    napi_value arrayBuffer = nullptr;
    size_t arrayBufferSize = 0;
    void* data = nullptr;
    napi_create_arraybuffer(env, arrayBufferSize, &data, &arrayBuffer);
    napi_value src2 = nullptr;
    napi_create_typedarray(env, napi_uint8_array, arrayBufferSize, arrayBuffer, 0, &src2);
    napi_value result2 = base64.Decode(env, src2, OHOS::Util::Type::BASIC);
    ASSERT_EQ(nullptr, result2);

    napi_get_and_clear_last_exception(env, &exception);
}

/**
 * @tc.name: stringDecoderWrite001
 * @tc.desc: Test the write function with complete data.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, stringDecoderWrite001, testing::ext::TestSize.Level0)
{
    OHOS::Util::StringDecoder stringDecoder("utf-8");
    napi_env env = (napi_env)engine_;
    const int arrCount = 6;
    size_t byteLength = arrCount;
    void* data = nullptr;
    napi_value resultBuff = nullptr;
    unsigned char arr[arrCount] = {0xE4, 0xBD, 0xA0, 0xE5, 0xA5, 0xBD};
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    int ret = memcpy_s(data, sizeof(arr), reinterpret_cast<void*>(arr), sizeof(arr));
    ASSERT_EQ(0, ret);
    napi_value result = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &result);
    napi_value testRes = stringDecoder.Write(env, result);
    size_t bufferSize = 0;
    if (napi_get_value_string_utf8(env, testRes, nullptr, 0, &bufferSize) != napi_ok) {
        HILOG_ERROR("can not get arg size");
    }
    std::string buffer = "";
    buffer.reserve(bufferSize);
    buffer.resize(bufferSize);
    if (napi_get_value_string_utf8(env, testRes, buffer.data(), bufferSize + 1, &bufferSize) != napi_ok) {
        HILOG_ERROR("can not get arg value");
    }
    ASSERT_STREQ("你好", buffer.c_str());

    napi_value testResEnd = stringDecoder.End(env);
    size_t bufferEndSizeEnd = 0;
    if (napi_get_value_string_utf8(env, testResEnd, nullptr, 0, &bufferEndSizeEnd) != napi_ok) {
        HILOG_ERROR("can not get arg size");
    }
    std::string bufferEnd = "";
    bufferEnd.reserve(bufferEndSizeEnd);
    bufferEnd.resize(bufferEndSizeEnd);
    if (napi_get_value_string_utf8(
        env, testResEnd, bufferEnd.data(), bufferEndSizeEnd + 1, &bufferEndSizeEnd) != napi_ok) {
        HILOG_ERROR("can not get arg value");
    }
    ASSERT_STREQ("", bufferEnd.c_str());
}

/**
 * @tc.name: stringDecoderWrite002
 * @tc.desc: Test the write function by splitting the complete data into two parts.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, stringDecoderWrite002, testing::ext::TestSize.Level0)
{
    OHOS::Util::StringDecoder stringDecoder("utf-8");
    napi_env env = (napi_env)engine_;
    const int arrCount = 2;
    size_t byteLength = arrCount;
    void* data = nullptr;
    napi_value resultBuff = nullptr;
    unsigned char arr[arrCount] = {0xE4, 0xBD};
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    int ret = memcpy_s(data, sizeof(arr), reinterpret_cast<void*>(arr), sizeof(arr));
    ASSERT_EQ(0, ret);
    napi_value result = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &result);
    napi_value testRes = stringDecoder.Write(env, result);
    size_t bufferSize = 0;
    if (napi_get_value_string_utf8(env, testRes, nullptr, 0, &bufferSize) != napi_ok) {
        HILOG_ERROR("can not get arg size");
    }
    std::string buffer = "";
    buffer.reserve(bufferSize);
    buffer.resize(bufferSize);
    if (napi_get_value_string_utf8(env, testRes, buffer.data(), bufferSize + 1, &bufferSize) != napi_ok) {
        HILOG_ERROR("can not get arg value");
    }
    ASSERT_STREQ("", buffer.c_str());

    const int count = 4;
    byteLength = count;
    data = nullptr;
    resultBuff = nullptr;
    unsigned char uint8[count] = {0xA0, 0xE5, 0xA5, 0xBD};
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    ret = memcpy_s(data, sizeof(uint8), reinterpret_cast<void*>(uint8), sizeof(uint8));
    ASSERT_EQ(0, ret);
    result = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &result);
    testRes = stringDecoder.Write(env, result);
    bufferSize = 0;
    if (napi_get_value_string_utf8(env, testRes, nullptr, 0, &bufferSize) != napi_ok) {
        HILOG_ERROR("can not get arg size");
    }
    buffer = "";
    buffer.reserve(bufferSize);
    buffer.resize(bufferSize);
    if (napi_get_value_string_utf8(env, testRes, buffer.data(), bufferSize + 1, &bufferSize) != napi_ok) {
        HILOG_ERROR("can not get arg value");
    }
    ASSERT_STREQ("你好", buffer.c_str());
}

/**
 * @tc.name: stringDecoderWrite003
 * @tc.desc: Test the write function with not typedarray.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, stringDecoderWrite003, testing::ext::TestSize.Level0)
{
    OHOS::Util::StringDecoder stringDecoder("utf-8");
    napi_env env = (napi_env)engine_;
    napi_value testRes = stringDecoder.Write(env, nullptr);
    ASSERT_EQ(testRes, nullptr);

    napi_value exception;
    napi_get_and_clear_last_exception(env, &exception);
}

/**
 * @tc.name: stringDecoderEnd001
 * @tc.desc: Test the end function by splitting the complete data into two parts.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, stringDecoderEnd001, testing::ext::TestSize.Level0)
{
    OHOS::Util::StringDecoder stringDecoder("utf-8");
    napi_env env = (napi_env)engine_;
    const int arrCount = 2;
    size_t byteLength = arrCount;
    void* data = nullptr;
    napi_value resultBuff = nullptr;
    unsigned char arr[arrCount] = {0xE4, 0xBD};
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    int ret = memcpy_s(data, sizeof(arr), reinterpret_cast<void*>(arr), sizeof(arr));
    ASSERT_EQ(0, ret);
    napi_value result = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &result);
    napi_value testRes = stringDecoder.Write(env, result);
    size_t bufferSize = 0;
    if (napi_get_value_string_utf8(env, testRes, nullptr, 0, &bufferSize) != napi_ok) {
        HILOG_ERROR("can not get arg size");
    }
    std::string buffer = "";
    buffer.reserve(bufferSize);
    buffer.resize(bufferSize);
    if (napi_get_value_string_utf8(env, testRes, buffer.data(), bufferSize + 1, &bufferSize) != napi_ok) {
        HILOG_ERROR("can not get arg value");
    }
    ASSERT_STREQ("", buffer.c_str());

    const int count = 4;
    byteLength = count;
    data = nullptr;
    resultBuff = nullptr;
    unsigned char uint8[count] = {0xA0, 0xE5, 0xA5, 0xBD};
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    ret = memcpy_s(data, sizeof(uint8), reinterpret_cast<void*>(uint8), sizeof(uint8));
    ASSERT_EQ(0, ret);
    result = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &result);
    testRes = stringDecoder.End(env, result);
    bufferSize = 0;
    if (napi_get_value_string_utf8(env, testRes, nullptr, 0, &bufferSize) != napi_ok) {
        HILOG_ERROR("can not get arg size");
    }
    buffer = "";
    buffer.reserve(bufferSize);
    buffer.resize(bufferSize);
    if (napi_get_value_string_utf8(env, testRes, buffer.data(), bufferSize + 1, &bufferSize) != napi_ok) {
        HILOG_ERROR("can not get arg value");
    }
    ASSERT_STREQ("你好", buffer.c_str());
}

/**
 * @tc.name: stringDecoderEnd002
 * @tc.desc: Test the end function by splitting the complete data into two parts.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, stringDecoderEnd002, testing::ext::TestSize.Level0)
{
    OHOS::Util::StringDecoder stringDecoder("utf-8");
    napi_env env = (napi_env)engine_;
    const int arrCount = 3;
    size_t byteLength = arrCount;
    void* data = nullptr;
    napi_value resultBuff = nullptr;
    unsigned char arr[arrCount] = {0xE4, 0xBD, 0xA0};
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    int ret = memcpy_s(data, sizeof(arr), reinterpret_cast<void*>(arr), sizeof(arr));
    ASSERT_EQ(0, ret);
    napi_value result = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &result);
    napi_value testRes = stringDecoder.Write(env, result);
    size_t bufferSize = 0;
    if (napi_get_value_string_utf8(env, testRes, nullptr, 0, &bufferSize) != napi_ok) {
        HILOG_ERROR("can not get arg size");
    }
    std::string buffer = "";
    buffer.reserve(bufferSize);
    buffer.resize(bufferSize);
    if (napi_get_value_string_utf8(env, testRes, buffer.data(), bufferSize + 1, &bufferSize) != napi_ok) {
        HILOG_ERROR("can not get arg value");
    }
    ASSERT_STREQ("你", buffer.c_str());
    testRes = stringDecoder.End(env);
    bufferSize = 0;
    if (napi_get_value_string_utf8(env, testRes, nullptr, 0, &bufferSize) != napi_ok) {
        HILOG_ERROR("can not get arg size");
    }
    buffer = "";
    buffer.reserve(bufferSize);
    buffer.resize(bufferSize);
    if (napi_get_value_string_utf8(env, testRes, buffer.data(), bufferSize + 1, &bufferSize) != napi_ok) {
        HILOG_ERROR("can not get arg value");
    }
    ASSERT_STREQ("", buffer.c_str());
}

/**
 * @tc.name: stringDecoderEnd003
 * @tc.desc: string decoder end with pending len 0.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, stringDecoderEnd003, testing::ext::TestSize.Level0)
{
    OHOS::Util::StringDecoder stringDecoder("utf-8");
    napi_env env = (napi_env)engine_;
    const int arrCount = 6;
    size_t byteLength = arrCount;
    void* data = nullptr;
    napi_value resultBuff = nullptr;
    unsigned char arr[arrCount] = {0xE4, 0xBD, 0xA0, 0xE5, 0xA5, 0xBD};
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    memcpy_s(data, sizeof(arr), reinterpret_cast<void*>(arr), sizeof(arr));
    napi_value result = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &result);
    stringDecoder.Write(env, result, false);

    napi_value testResEnd = stringDecoder.End(env);
    size_t bufferEndSizeEnd = 0;
    if (napi_get_value_string_utf8(env, testResEnd, nullptr, 0, &bufferEndSizeEnd) != napi_ok) {
        HILOG_ERROR("can not get arg size");
    }
    std::string bufferEnd = "";
    bufferEnd.reserve(bufferEndSizeEnd);
    bufferEnd.resize(bufferEndSizeEnd);
    if (napi_get_value_string_utf8(
        env, testResEnd, bufferEnd.data(), bufferEndSizeEnd + 1, &bufferEndSizeEnd) != napi_ok) {
        HILOG_ERROR("can not get arg value");
    }
    ASSERT_STREQ("", bufferEnd.c_str());
}

/**
 * @tc.name: stringDecoderEnd004
 * @tc.desc: Test the end function by splitting the complete data into two parts.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, stringDecoderEnd004, testing::ext::TestSize.Level0)
{
    OHOS::Util::StringDecoder stringDecoder("utf-8");
    napi_env env = (napi_env)engine_;
    const int arrCount = 2; // 2:nums of args
    size_t byteLength = arrCount;
    void* data = nullptr;
    napi_value resultBuff = nullptr;
    unsigned char arr[arrCount] = {0xE4, 0xBD};
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    int ret = memcpy_s(data, sizeof(arr), reinterpret_cast<void*>(arr), sizeof(arr));
    ASSERT_EQ(0, ret);
    napi_value res = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &res);
    napi_value testRes = stringDecoder.Write(env, res);
    size_t bufferSize = 0;
    if (napi_get_value_string_utf8(env, testRes, nullptr, 0, &bufferSize) != napi_ok) {
        HILOG_ERROR("can not get arg size");
    }
    std::string buffer = "";
    buffer.reserve(bufferSize);
    buffer.resize(bufferSize);
    if (napi_get_value_string_utf8(env, testRes, buffer.data(), bufferSize + 1, &bufferSize) != napi_ok) {
        HILOG_ERROR("can not get arg value");
    }
    ASSERT_STREQ("", buffer.c_str());
    napi_value result = stringDecoder.End(env);
    ASSERT_TRUE(result == nullptr);
}

/**
 * @tc.name: charEncodeAchieves001
 * @tc.desc: char encode achieves with throw error.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, charEncodeAchieves001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::Util::EncodeInfo* stdEncodeInfo = nullptr;
    stdEncodeInfo = new OHOS::Util::EncodeInfo();
    stdEncodeInfo->slength = 0;
    unsigned char* res = OHOS::Util::EncodeAchieves(env, stdEncodeInfo);
    ASSERT_EQ(nullptr, res);

    OHOS::Util::EncodeInfo* stdEncodeInfo1 = nullptr;
    unsigned char arr[] = {0xE4, 0xBD, 0xA0, 0xE5, 0xA5, 0xBD};
    stdEncodeInfo1 = new OHOS::Util::EncodeInfo();
    stdEncodeInfo1->sinputEncode = arr;
    stdEncodeInfo1->slength = 1;
    stdEncodeInfo1->valueType = OHOS::Util::Type::BASIC_URL_SAFE;
    unsigned char* res1 = OHOS::Util::EncodeAchieves(env, stdEncodeInfo1);
    ASSERT_EQ(0x35, static_cast<unsigned char>(*res1));

    napi_value exception;
    napi_get_and_clear_last_exception(env, &exception);
}

/* @tc.name: testDecode001
 * @tc.desc: Test for abnormal situations in the decode function
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, testDecode001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("testDecode001 start");
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
    napi_create_typedarray(env, napi_int8_array, arrayBufferSize, arrayBuffer, 0, &src);
    napi_value result = base64.Decode(env, src, OHOS::Util::Type::BASIC);
    ASSERT_TRUE(result == nullptr);
}

/* @tc.name: testDecode002
 * @tc.desc: Test for abnormal situations in the decode function
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, testDecode002, testing::ext::TestSize.Level0)
{
    HILOG_INFO("testDecode002 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Base64 base64;
    napi_value src = nullptr;
    napi_create_int32(env, 9, &src);
    napi_value result = base64.Decode(env, src, OHOS::Util::Type::BASIC);
    ASSERT_TRUE(result == nullptr);
}

/* @tc.name: testDecodeSync001
 * @tc.desc: Test for abnormal situations in the DecodeSync function
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, testDecodeSync001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("testDecodeSync001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Base64 base64;

    napi_value src = nullptr;
    napi_create_int32(env, 9, &src);
    napi_value result = base64.DecodeSync(env, src, OHOS::Util::Type::BASIC);
    ASSERT_TRUE(result == nullptr);
}

/**
 * @tc.name: charDecodeAchieves001
 * @tc.desc: char dencode achieves with throw error.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, charDencodeAchieves001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::Util::DecodeInfo* stdDecodeInfo2 = nullptr;
    char arr2[] = {0xE4, 0xBD, 0xA0, 0xE5, 0xA5, 0xBD};
    stdDecodeInfo2 = new OHOS::Util::DecodeInfo();
    stdDecodeInfo2->sinputDecode = arr2;
    stdDecodeInfo2->slength = 2;
    stdDecodeInfo2->valueType = OHOS::Util::Type::BASIC_URL_SAFE;
    unsigned char* res2 = OHOS::Util::DecodeAchieves(env, stdDecodeInfo2);
    ASSERT_EQ(0, static_cast<const char>(*res2));

    OHOS::Util::DecodeInfo* stdDecodeInfo3 = nullptr;
    char arr3[] = {0xE4, 0xBD, 0xA0, 0xE5, 0xA5, 0xBD};
    stdDecodeInfo3 = new OHOS::Util::DecodeInfo();
    stdDecodeInfo3->sinputDecode = arr3;
    stdDecodeInfo3->slength = 3;
    stdDecodeInfo3->valueType = OHOS::Util::Type::BASIC_URL_SAFE;
    unsigned char* res3 = OHOS::Util::DecodeAchieves(env, stdDecodeInfo3);
    ASSERT_EQ(0, static_cast<unsigned char>(*res3));

    napi_value exception;
    napi_get_and_clear_last_exception(env, &exception);
}

/**
 * @tc.name: DecodeToStringNoStream001
 * @tc.desc: Testing the ignoreBOM of TextDecoder.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DecodeToStringNoStream001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("DecodeToStringNoStream start");
    napi_env env = (napi_env)engine_;
    int32_t flags = static_cast<int>(OHOS::Util::TextDecoder::ConverterFlags::FATAL_FLG) |
        static_cast<int>(OHOS::Util::TextDecoder::ConverterFlags::IGNORE_BOM_FLG);
    std::string encoding = "utf-8";
    OHOS::Util::TextDecoder textDecoder(encoding, flags);
    bool iflag = false;
    size_t byteLength = 6;
    void* data = nullptr;
    napi_value resultBuff = nullptr;
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    unsigned char arr[8] = {0xEF, 0xBB, 0xBF, 0x41, 0x42, 0x43};
    int ret = memcpy_s(data, sizeof(arr), reinterpret_cast<void*>(arr), sizeof(arr));
    ASSERT_EQ(0, ret);
    napi_value result = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &result);
    textDecoder.DecodeToString(env, result, iflag);
}

/**
 * @tc.name: DecodeToStringWithStream001
 * @tc.desc: Testing the ignoreBOM of TextDecoder.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DecodeToStringWithStream001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("DecodeToStringWithStream start");
    napi_env env = (napi_env)engine_;
    int32_t flags = static_cast<int>(OHOS::Util::TextDecoder::ConverterFlags::FATAL_FLG) |
        static_cast<int>(OHOS::Util::TextDecoder::ConverterFlags::IGNORE_BOM_FLG);
    std::string encoding = "utf-8";
    OHOS::Util::TextDecoder textDecoder(encoding, flags);
    bool iflag = true;
    size_t byteLength = 6;
    void* data = nullptr;
    napi_value resultBuff = nullptr;
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    unsigned char arr[8] = {0xEF, 0xBB, 0xBF, 0x41, 0x42, 0x43};
    int ret = memcpy_s(data, sizeof(arr), reinterpret_cast<void*>(arr), sizeof(arr));
    ASSERT_EQ(0, ret);
    napi_value result = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &result);
    textDecoder.DecodeToString(env, result, iflag);
}

/**
 * @tc.name: decoderGBK
 * @tc.desc: Testing the decoding result of GBK data with BOM.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decoderGBK001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decoderGBK start");
    napi_env env = (napi_env)engine_;
    int32_t flags = 0;
    std::string encoding = "GBK";
    OHOS::Util::TextDecoder textDecoder(encoding, flags);
    bool iflag = true;
    size_t byteLength = 4;
    void* data = nullptr;
    napi_value resultBuff = nullptr;
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    unsigned char arr[4] = {196, 227, 186, 195};
    int ret = memcpy_s(data, sizeof(arr), reinterpret_cast<void*>(arr), sizeof(arr));
    ASSERT_EQ(0, ret);
    napi_value result = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &result);
    napi_value testString = textDecoder.DecodeToString(env, result, iflag);
    size_t bufferSize = 0;
    napi_get_value_string_utf16(env, testString, nullptr, 0, &bufferSize);
    size_t length = 0;
    char16_t* ch = nullptr;
    if (bufferSize > 0) {
        ch = new char16_t[bufferSize + 1]();
        napi_get_value_string_utf16(env, testString, ch, bufferSize + 1, &length);
    }
    std::string str =
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.to_bytes(ch);
    ASSERT_EQ(str, "你好");
    if (ch != nullptr) {
        delete []ch;
        ch = nullptr;
    }
}

/**
 * @tc.name: decoderbig5
 * @tc.desc: Testing the decoding result of big5 data with BOM.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decoderbig5001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decoderbig5 start");
    napi_env env = (napi_env)engine_;
    int32_t flags = 0;
    std::string encoding = "big5";
    OHOS::Util::TextDecoder textDecoder(encoding, flags);
    bool iflag = true;
    size_t byteLength = 6;
    void* data = nullptr;
    napi_value resultBuff = nullptr;
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    unsigned char arr[6] = {166, 173, 164, 87, 166, 110};
    int ret = memcpy_s(data, sizeof(arr), reinterpret_cast<void*>(arr), sizeof(arr));
    ASSERT_EQ(0, ret);
    napi_value result = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &result);
    napi_value testString = textDecoder.DecodeToString(env, result, iflag);
    size_t bufferSize = 0;
    napi_get_value_string_utf16(env, testString, nullptr, 0, &bufferSize);
    size_t length = 0;
    char16_t* ch = nullptr;
    if (bufferSize > 0) {
        ch = new char16_t[bufferSize + 1]();
        napi_get_value_string_utf16(env, testString, ch, bufferSize + 1, &length);
    }
    std::string str =
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.to_bytes(ch);
    ASSERT_EQ(str, "早上好");
    if (ch != nullptr) {
        delete []ch;
        ch = nullptr;
    }
}

/**
 * @tc.name: decodergb18030
 * @tc.desc: Testing the decoding result of gb18030 data with BOM.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decodergb18030, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decodergb18030 start");
    napi_env env = (napi_env)engine_;
    int32_t flags = 0;
    std::string encoding = "gb18030";
    OHOS::Util::TextDecoder textDecoder(encoding, flags);
    bool iflag = true;
    size_t byteLength = 4;
    void* data = nullptr;
    napi_value resultBuff = nullptr;
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    unsigned char arr[4] = {196, 227, 186, 195};
    int ret = memcpy_s(data, sizeof(arr), reinterpret_cast<void*>(arr), sizeof(arr));
    ASSERT_EQ(0, ret);
    napi_value result = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &result);
    napi_value testString = textDecoder.DecodeToString(env, result, iflag);
    size_t bufferSize = 0;
    napi_get_value_string_utf16(env, testString, nullptr, 0, &bufferSize);
    size_t length = 0;
    char16_t* ch = nullptr;
    if (bufferSize > 0) {
        ch = new char16_t[bufferSize + 1]();
        napi_get_value_string_utf16(env, testString, ch, bufferSize + 1, &length);
    }
    std::string str =
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.to_bytes(ch);
    ASSERT_EQ(str, "你好");
    if (ch != nullptr) {
        delete []ch;
        ch = nullptr;
    }
}

/**
 * @tc.name: decodergbk
 * @tc.desc: Testing the decoding result of gbk data with BOM.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decodergbk, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decodergbk start");
    napi_env env = (napi_env)engine_;
    int32_t flags = 0;
    std::string encoding = "gbk";
    OHOS::Util::TextDecoder textDecoder(encoding, flags);
    bool iflag = true;
    size_t byteLength = 4;
    void* data = nullptr;
    napi_value resultBuff = nullptr;
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    unsigned char arr[4] = {196, 227, 186, 195};
    int ret = memcpy_s(data, sizeof(arr), reinterpret_cast<void*>(arr), sizeof(arr));
    ASSERT_EQ(0, ret);
    napi_value result = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &result);
    napi_value testString = textDecoder.DecodeToString(env, result, iflag);
    size_t bufferSize = 0;
    napi_get_value_string_utf16(env, testString, nullptr, 0, &bufferSize);
    size_t length = 0;
    char16_t* ch = nullptr;
    if (bufferSize > 0) {
        ch = new char16_t[bufferSize + 1]();
        napi_get_value_string_utf16(env, testString, ch, bufferSize + 1, &length);
    }
    std::string str =
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.to_bytes(ch);
    ASSERT_EQ(str, "你好");
    if (ch != nullptr) {
        delete []ch;
        ch = nullptr;
    }
}

/**
 * @tc.name: decoderwindows1257
 * @tc.desc: Testing the decoding result of  windows-1257 data with BOM.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decoderwindows1257, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decoderwindows1257 start");
    napi_env env = (napi_env)engine_;
    int32_t flags = 0;
    std::string str = "windows-1257";
    OHOS::Util::TextDecoder textDecoder(str, flags);
    bool iflag = false;
    size_t byteLength = 4;
    void* data = nullptr;
    napi_value resultBuff = nullptr;
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    unsigned char arr[4] = {84, 101, 114, 101};
    int ret = memcpy_s(data, sizeof(arr), reinterpret_cast<void*>(arr), sizeof(arr));
    ASSERT_EQ(0, ret);
    napi_value result2 = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &result2);
    napi_value testString = textDecoder.DecodeToString(env, result2, iflag);
    size_t bufferSize = 0;
    napi_get_value_string_utf8(env, testString, nullptr, 0, &bufferSize);
    size_t length = 0;
    char* ch = nullptr;
    if (bufferSize > 0) {
        ch = new char[bufferSize + 1]();
        napi_get_value_string_utf8(env, testString, ch, bufferSize + 1, &length);
    }
    ASSERT_STREQ("Tere", ch);
    if (ch != nullptr) {
        delete []ch;
        ch = nullptr;
    }
}

/**
 * @tc.name: decoderwindows874
 * @tc.desc: Testing the decoding result of windows-874 data with BOM.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decoderwindows874, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decoderwindows874 start");
    napi_env env = (napi_env)engine_;
    int32_t flags = 0;
    std::string str = "windows-874";
    OHOS::Util::TextDecoder textDecoder(str, flags);
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
    napi_value testString = textDecoder.DecodeToString(env, result2, iflag);
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
 * @tc.name: decodermacintosh
 * @tc.desc: Testing the decoding result of macintosh data with BOM.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decodermacintosh, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decodermacintosh start");
    napi_env env = (napi_env)engine_;
    int32_t flags = 0;
    std::string str = "macintosh";
    OHOS::Util::TextDecoder textDecoder(str, flags);
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
    napi_value testString = textDecoder.DecodeToString(env, result2, iflag);
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
 * @tc.name: decoderkoi8u
 * @tc.desc: Testing the decoding result of koi8-u data with BOM.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decoderkoi8u, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decoderkoi8u start");
    napi_env env = (napi_env)engine_;
    int32_t flags = 0;
    std::string str = "koi8-u";
    OHOS::Util::TextDecoder textDecoder(str, flags);
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
    napi_value testString = textDecoder.DecodeToString(env, result2, iflag);
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
 * @tc.name: decoderiso885915
 * @tc.desc: Testing the decoding result of iso-8859-15 data with BOM.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decoderiso885915, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decoderiso885915 start");
    napi_env env = (napi_env)engine_;
    int32_t flags = 0;
    std::string str = "iso-8859-15";
    OHOS::Util::TextDecoder textDecoder(str, flags);
    bool iflag = false;
    size_t byteLength = 4;
    void* data = nullptr;
    napi_value resultBuff = nullptr;
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    unsigned char arr[4] = {72, 111, 108, 97};
    int ret = memcpy_s(data, sizeof(arr), reinterpret_cast<void*>(arr), sizeof(arr));
    ASSERT_EQ(0, ret);
    napi_value result2 = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &result2);
    napi_value testString = textDecoder.DecodeToString(env, result2, iflag);
    size_t bufferSize = 0;
    napi_get_value_string_utf8(env, testString, nullptr, 0, &bufferSize);
    size_t length = 0;
    char* ch = nullptr;
    if (bufferSize > 0) {
        ch = new char[bufferSize + 1]();
        napi_get_value_string_utf8(env, testString, ch, bufferSize + 1, &length);
    }
    ASSERT_STREQ("Hola", ch);
    if (ch != nullptr) {
        delete []ch;
        ch = nullptr;
    }
}

/**
 * @tc.name: decoderwindows1256
 * @tc.desc: Testing the decoding result of  windows-1256 data with BOM.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decoderwindows1256, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decoderwindows1256 start");
    napi_env env = (napi_env)engine_;
    int32_t flags = 0;
    std::string str = "windows-1256";
    OHOS::Util::TextDecoder textDecoder(str, flags);
    bool iflag = false;
    size_t byteLength = 7;
    void* data = nullptr;
    napi_value resultBuff = nullptr;
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    unsigned char arr[7] = {77, 97, 114, 104, 97, 98, 97};
    int ret = memcpy_s(data, sizeof(arr), reinterpret_cast<void*>(arr), sizeof(arr));
    ASSERT_EQ(0, ret);
    napi_value result2 = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &result2);
    napi_value testString = textDecoder.DecodeToString(env, result2, iflag);
    size_t bufferSize = 0;
    napi_get_value_string_utf8(env, testString, nullptr, 0, &bufferSize);
    size_t length = 0;
    char* ch = nullptr;
    if (bufferSize > 0) {
        ch = new char[bufferSize + 1]();
        napi_get_value_string_utf8(env, testString, ch, bufferSize + 1, &length);
    }
    ASSERT_STREQ("Marhaba", ch);
    if (ch != nullptr) {
        delete []ch;
        ch = nullptr;
    }
}

/**
 * @tc.name: decoderwindows1255
 * @tc.desc: Testing the decoding result of  windows-1255 data with BOM.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, decoderwindows1255, testing::ext::TestSize.Level0)
{
    HILOG_INFO("decoderwindows1255 start");
    napi_env env = (napi_env)engine_;
    int32_t flags = 0;
    std::string str = "windows-1255";
    OHOS::Util::TextDecoder textDecoder(str, flags);
    bool iflag = false;
    size_t byteLength = 7;
    void* data = nullptr;
    napi_value resultBuff = nullptr;
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    unsigned char arr[7] = {77, 101, 114, 104, 97, 98, 97};
    int ret = memcpy_s(data, sizeof(arr), reinterpret_cast<void*>(arr), sizeof(arr));
    ASSERT_EQ(0, ret);
    napi_value result2 = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &result2);
    napi_value testString = textDecoder.DecodeToString(env, result2, iflag);
    size_t bufferSize = 0;
    napi_get_value_string_utf8(env, testString, nullptr, 0, &bufferSize);
    size_t length = 0;
    char* ch = nullptr;
    if (bufferSize > 0) {
        ch = new char[bufferSize + 1]();
        napi_get_value_string_utf8(env, testString, ch, bufferSize + 1, &length);
    }
    ASSERT_STREQ("Merhaba", ch);
    if (ch != nullptr) {
        delete []ch;
        ch = nullptr;
    }
}

/**
 * @tc.name: textEncodeIntoTest008
 * @tc.desc: Test returns a dictionary object indicating the progress of the encoding
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textEncodeIntoTest008, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textEncodeIntoTest008 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::TextEncoder textEncoder("utf-8");
    std::string input = "😃";
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
    napi_get_value_uint32(env, written, &resWritten);
    ASSERT_EQ(resRead, static_cast<uint32_t>(2));
    ASSERT_EQ(resWritten, static_cast<uint32_t>(4));
}

/**
 * @tc.name: textEncodeIntoTest009
 * @tc.desc: Test returns a dictionary object indicating the progress of the encoding
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textEncodeIntoTest009, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textEncodeIntoTest009 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::TextEncoder textEncoder("utf-8");
    std::string input = "😃a";
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
    napi_get_value_uint32(env, written, &resWritten);
    ASSERT_EQ(resRead, static_cast<uint32_t>(3));
    ASSERT_EQ(resWritten, static_cast<uint32_t>(5));
}

/**
 * @tc.name: textEncodeIntoTest010
 * @tc.desc: Test returns a dictionary object indicating the progress of the encoding
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textEncodeIntoTest010, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textEncodeIntoTest010 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::TextEncoder textEncoder("utf-8");
    std::string input = "😃a🤞";
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
    napi_get_value_uint32(env, written, &resWritten);
    ASSERT_EQ(resRead, static_cast<uint32_t>(5));
    ASSERT_EQ(resWritten, static_cast<uint32_t>(9));
}

/**
 * @tc.name: textEncodeIntoTest011
 * @tc.desc: Test returns a dictionary object indicating the progress of the encoding
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textEncodeIntoTest011, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textEncodeIntoTest011 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::TextEncoder textEncoder("utf-8");
    std::string input = "中a";
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
    napi_get_value_uint32(env, written, &resWritten);
    ASSERT_EQ(resRead, static_cast<uint32_t>(2));
    ASSERT_EQ(resWritten, static_cast<uint32_t>(4));
}

/**
 * @tc.name: textEncodeIntoTest012
 * @tc.desc: Test returns a dictionary object indicating the progress of the encoding
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textEncodeIntoTest012, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textEncodeIntoTest012 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::TextEncoder textEncoder("utf-8");
    std::string input = "中aÿ";
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
    napi_get_value_uint32(env, written, &resWritten);
    ASSERT_EQ(resRead, static_cast<uint32_t>(3));
    ASSERT_EQ(resWritten, static_cast<uint32_t>(6));
}

/**
 * @tc.name: textEncodeIntoTest013
 * @tc.desc: Test returns a dictionary object indicating the progress of the encoding
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textEncodeIntoTest013, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textEncodeIntoTest013 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::TextEncoder textEncoder("utf-8");
    std::string input = "😃a";
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
    napi_get_value_uint32(env, written, &resWritten);
    ASSERT_EQ(resRead, static_cast<uint32_t>(3)); // 3: resRead expected results
    ASSERT_EQ(resWritten, static_cast<uint32_t>(5)); // 5: resWritten expected results
}

/**
 * @tc.name: textEncodeIntoTest014
 * @tc.desc: Test returns a dictionary object indicating the progress of the encoding
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textEncodeIntoTest014, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textEncodeIntoTest014 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::TextEncoder textEncoder("gb2312");
    std::string input = "中文abc";
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
    napi_get_value_uint32(env, written, &resWritten);
    ASSERT_EQ(resRead, static_cast<uint32_t>(5)); // 5: resRead expected results
    ASSERT_EQ(resWritten, static_cast<uint32_t>(7)); // 7: resWritten expected results
}

/**
 * @tc.name: textEncodeIntoTest015
 * @tc.desc: Test returns a dictionary object indicating the progress of the encoding
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textEncodeIntoTest015, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textEncodeIntoTest015 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::TextEncoder textEncoder("iso-8859-1");
    std::string input = "München";
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
    napi_get_value_uint32(env, written, &resWritten);
    ASSERT_EQ(resRead, static_cast<uint32_t>(7)); // 7: resRead expected results
    ASSERT_EQ(resWritten, static_cast<uint32_t>(7)); // 7: resWritten expected results
}

/**
 * @tc.name: textEncodeIntoTest016
 * @tc.desc: Test returns a dictionary object indicating the progress of the encoding
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textEncodeIntoTest016, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textEncodeIntoTest016 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::TextEncoder textEncoder("windows-1251");
    std::string input = "Привет";
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
    napi_get_value_uint32(env, written, &resWritten);
    ASSERT_EQ(resRead, static_cast<uint32_t>(6)); // 6: resRead expected results
    ASSERT_EQ(resWritten, static_cast<uint32_t>(6)); // 6: resWritten expected results
}

/**
 * @tc.name: textEncodeIntoTest017
 * @tc.desc: Test returns a dictionary object indicating the progress of the encoding
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textEncodeIntoTest017, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textEncodeIntoTest017 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::TextEncoder textEncoder("shift_jis");
    std::string input = "日本語";
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
    napi_get_value_uint32(env, written, &resWritten);
    ASSERT_EQ(resRead, static_cast<uint32_t>(3)); // 3: resRead expected results
    ASSERT_EQ(resWritten, static_cast<uint32_t>(6)); // 6: resWritten expected results
}

/**
 * @tc.name: textEncodeIntoTest018
 * @tc.desc: Test returns a dictionary object indicating the progress of the encoding
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textEncodeIntoTest018, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textEncodeIntoTest018 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::TextEncoder textEncoder("windows-874");
    std::string input = "สวัสดี";
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
    napi_get_value_uint32(env, written, &resWritten);
    ASSERT_EQ(resRead, static_cast<uint32_t>(6)); // 6: resRead expected results
    ASSERT_EQ(resWritten, static_cast<uint32_t>(6)); // 6: resWritten expected results
}

/**
 * @tc.name: textEncodeIntoTest019
 * @tc.desc: Test returns a dictionary object indicating the progress of the encoding
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textEncodeIntoTest019, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textEncodeIntoTest019 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::TextEncoder textEncoder("big5");
    std::string input = "電腦";
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
    napi_get_value_uint32(env, written, &resWritten);
    ASSERT_EQ(resRead, static_cast<uint32_t>(2)); // 2: resRead expected results
    ASSERT_EQ(resWritten, static_cast<uint32_t>(4)); // 4: resWritten expected results
}

/**
 * @tc.name: textEncodeIntoTest020
 * @tc.desc: Test returns a dictionary object indicating the progress of the encoding
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textEncodeIntoTest020, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textEncodeIntoTest020 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::TextEncoder textEncoder("koi8-u");
    std::string input = "Україна";
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
    napi_get_value_uint32(env, written, &resWritten);
    ASSERT_EQ(resRead, static_cast<uint32_t>(7)); // 7: resRead expected results
    ASSERT_EQ(resWritten, static_cast<uint32_t>(7)); // 7: resWritten expected results
}

/**
 * @tc.name: textEncodeIntoTest021
 * @tc.desc: Test returns a dictionary object indicating the progress of the encoding
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textEncodeIntoTest021, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textEncodeIntoTest021 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::TextEncoder textEncoder("iso-2022-jp");
    std::string input = "Hello日本";
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
    napi_get_value_uint32(env, written, &resWritten);
    ASSERT_EQ(resRead, static_cast<uint32_t>(6)); // 6: resRead expected results
    ASSERT_EQ(resWritten, static_cast<uint32_t>(13)); // 13: resWritten expected results
}

/**
 * @tc.name: textEncodeIntoTest022
 * @tc.desc: Test returns a dictionary object indicating the progress of the encoding
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textEncodeIntoTest022, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textEncodeIntoTest022 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::TextEncoder textEncoder("ibm866");
    std::string input = "Компьютер";
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
    napi_get_value_uint32(env, written, &resWritten);
    ASSERT_EQ(resRead, static_cast<uint32_t>(9)); // 9: resRead expected results
    ASSERT_EQ(resWritten, static_cast<uint32_t>(9)); // 9: resWritten expected results
}

/**
 * @tc.name: textEncodeIntoTest023
 * @tc.desc: Test returns a dictionary object indicating the progress of the encoding
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textEncodeIntoTest023, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textEncodeIntoTest023 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::TextEncoder textEncoder("gbk");
    std::string input = "㐀𠀀";
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
    napi_get_value_uint32(env, written, &resWritten);
    ASSERT_EQ(resRead, static_cast<uint32_t>(3)); // 3: resRead expected results
    ASSERT_EQ(resWritten, static_cast<uint32_t>(3)); // 3: resWritten expected results
}

/**
 * @tc.name: textEncodeIntoTest024
 * @tc.desc: Test returns a dictionary object indicating the progress of the encoding
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textEncodeIntoTest024, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textEncodeIntoTest024 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::TextEncoder textEncoder("euc-kr");
    std::string input = "한국어";
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
    napi_get_value_uint32(env, written, &resWritten);
    ASSERT_EQ(resRead, static_cast<uint32_t>(3)); // 3: resRead expected results
    ASSERT_EQ(resWritten, static_cast<uint32_t>(6)); // 6: resWritten expected results
}

/**
 * @tc.name: textEncodeIntoTest025
 * @tc.desc: Test returns a dictionary object indicating the progress of the encoding
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textEncodeIntoTest025, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textEncodeIntoTest025 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::TextEncoder textEncoder("iso-8859-15");
    std::string input = "€100";
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
    napi_get_value_uint32(env, written, &resWritten);
    ASSERT_EQ(resRead, static_cast<uint32_t>(4)); // 4: resRead expected results
    ASSERT_EQ(resWritten, static_cast<uint32_t>(4)); // 4: resWritten expected results
}

/**
 * @tc.name: textEncodeIntoTest026
 * @tc.desc: Test returns a dictionary object indicating the progress of the encoding
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textEncodeIntoTest026, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textEncodeIntoTest026 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::TextEncoder textEncoder("macintosh");
    std::string input = "ƒ©±";
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
    napi_get_value_uint32(env, written, &resWritten);
    ASSERT_EQ(resRead, static_cast<uint32_t>(3)); // 3: resRead expected results
    ASSERT_EQ(resWritten, static_cast<uint32_t>(3)); // 3: resWritten expected results
}

/**
 * @tc.name: textEncodeIntoTest027
 * @tc.desc: Test returns a dictionary object indicating the progress of the encoding
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textEncodeIntoTest027, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textEncodeIntoTest027 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::TextEncoder textEncoder("windows-1250");
    std::string input = "Žluťoučký";
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
    napi_get_value_uint32(env, written, &resWritten);
    ASSERT_EQ(resRead, static_cast<uint32_t>(9)); // 9: resRead expected results
    ASSERT_EQ(resWritten, static_cast<uint32_t>(9)); // 9: resWritten expected results
}

/**
 * @tc.name: textEncodeIntoTest028
 * @tc.desc: Test returns a dictionary object indicating the progress of the encoding
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textEncodeIntoTest028, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textEncodeIntoTest028 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::TextEncoder textEncoder("gb18030");
    std::string input = "𠮷字";
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
    napi_get_value_uint32(env, written, &resWritten);
    ASSERT_EQ(resRead, static_cast<uint32_t>(3)); // 3: resRead expected results
    ASSERT_EQ(resWritten, static_cast<uint32_t>(4)); // 4: resWritten expected results
}

/**
 * @tc.name: textIsTypedArray001
 * @tc.desc: Test check whether the entered value is the type of typedarray
 * @tc.type: FUNCs
 */
HWTEST_F(NativeEngineTest, textIsTypedArray001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textIsTypedArray001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types types;
    napi_value obj = nullptr;
    napi_create_object(env, &obj);
    bool res = true;
    napi_value result = types.IsTypedArray(env, obj);
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: textIsTypedArray002
 * @tc.desc: Test check whether the entered value is the type of typedarray
 * @tc.type: FUNCs
 */
HWTEST_F(NativeEngineTest, textIsTypedArray002, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textIsTypedArray002 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types types;
    napi_value arrayBuffer = nullptr;
    napi_value src = nullptr;
    void* arrayBufferPtr = nullptr;
    size_t arrayBufferSize = 20;
    napi_create_arraybuffer(env, arrayBufferSize * sizeof(size_t), &arrayBufferPtr, &arrayBuffer);
    napi_create_typedarray(env, napi_int8_array, arrayBufferSize, arrayBuffer, 0, &src);
    bool res = false;
    napi_value result = types.IsTypedArray(env, src);
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: textIsWeakSet001
 * @tc.desc: Test check whether the entered value is the type of weakset
 * @tc.type: FUNCs
 */
HWTEST_F(NativeEngineTest, textIsWeakSet001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textIsWeakSet001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types types;
    napi_value src = nullptr;
    napi_create_object(env, &src);
    bool res = true;
    napi_value result = types.IsWeakSet(env, src);
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: textIsWeakMap001
 * @tc.desc: Test check whether the entered value is the type of weakmap
 * @tc.type: FUNCs
 */
HWTEST_F(NativeEngineTest, textIsWeakMap001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textIsWeakMap001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types types;
    napi_value src = nullptr;
    std::string input = "str";
    napi_create_string_utf8(env, input.c_str(), input.size(), &src);
    bool res = true;
    napi_value result = types.IsWeakMap(env, src);
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: textIsUint32Array001
 * @tc.desc: Test check whether the entered value is the type of uint32array
 * @tc.type: FUNCs
 */
HWTEST_F(NativeEngineTest, textIsUint32Array001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textIsUint32Array001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types types;
    napi_value src = nullptr;
    std::string input = "str";
    napi_create_string_utf8(env, input.c_str(), input.size(), &src);
    bool res = true;
    napi_value result = types.IsUint32Array(env, src);
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: textIsUint32Array002
 * @tc.desc: Test check whether the entered value is the type of uint32array
 * @tc.type: FUNCs
 */
HWTEST_F(NativeEngineTest, textIsUint32Array002, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textIsUint32Array002 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types types;
    napi_value obj = nullptr;
    napi_create_object(env, &obj);
    bool res = true;
    napi_value result = types.IsUint32Array(env, obj);
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: textIsUint32Array003
 * @tc.desc: Test check whether the entered value is the type of uint32array
 * @tc.type: FUNCs
 */
HWTEST_F(NativeEngineTest, textIsUint32Array003, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textIsUint32Array003 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types types;
    napi_value arrayBuffer = nullptr;
    size_t arrayBufferSize = 20;
    void* data = nullptr;
    napi_create_arraybuffer(env, arrayBufferSize * sizeof(size_t), &data, &arrayBuffer);
    napi_value src = nullptr;
    napi_create_typedarray(env, napi_int8_array, arrayBufferSize, arrayBuffer, 0, &src);
    bool res = true;
    napi_value result = types.IsUint32Array(env, src);
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: textIsUint32Array004
 * @tc.desc: Test check whether the entered value is the type of uint32array
 * @tc.type: FUNCs
 */
HWTEST_F(NativeEngineTest, textIsUint32Array004, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textIsUint32Array004 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types types;
    napi_value arrayBuffer = nullptr;
    size_t arrayBufferSize = 20;
    void* data = nullptr;
    napi_create_arraybuffer(env, arrayBufferSize * sizeof(size_t), &data, &arrayBuffer);
    napi_value src = nullptr;
    napi_create_typedarray(env, napi_uint32_array, arrayBufferSize, arrayBuffer, 0, &src);
    bool res = false;
    napi_value result = types.IsUint32Array(env, src);
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: textIsUint16Array001
 * @tc.desc: Test check whether the entered value is the type of uint16array
 * @tc.type: FUNCs
 */
HWTEST_F(NativeEngineTest, textIsUint16Array001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textIsUint16Array001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types types;
    napi_value obj = nullptr;
    napi_create_object(env, &obj);
    bool res = true;
    napi_value result = types.IsUint16Array(env, obj);
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: textIsUint16Array002
 * @tc.desc: Test check whether the entered value is the type of uint16array
 * @tc.type: FUNCs
 */
HWTEST_F(NativeEngineTest, textIsUint16Array002, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textIsUint16Array002 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types types;
    napi_value arrayBuffer = nullptr;
    size_t arrayBufferSize = 20;
    void* data = nullptr;
    napi_create_arraybuffer(env, arrayBufferSize * sizeof(size_t), &data, &arrayBuffer);
    napi_value src = nullptr;
    napi_create_typedarray(env, napi_uint16_array, arrayBufferSize, arrayBuffer, 0, &src);
    bool res = false;
    napi_value result = types.IsUint16Array(env, src);
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: textIsUint8ClampedArray001
 * @tc.desc: Test check whether the entered value is the type of uint8clampedarray
 * @tc.type: FUNCs
 */
HWTEST_F(NativeEngineTest, textIsUint8ClampedArray001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textIsUint8ClampedArray001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types types;
    napi_value obj = nullptr;
    napi_create_object(env, &obj);
    bool res = true;
    napi_value result = types.IsUint8ClampedArray(env, obj);
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: textIsUint8ClampedArray002
 * @tc.desc: Test check whether the entered value is the type of uint8clampedarray
 * @tc.type: FUNCs
 */
HWTEST_F(NativeEngineTest, textIsUint8ClampedArray002, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textIsUint8ClampedArray002 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types types;
    napi_value arrayBuffer = nullptr;
    size_t arrayBufferSize = 20;
    void* data = nullptr;
    napi_create_arraybuffer(env, arrayBufferSize * sizeof(size_t), &data, &arrayBuffer);
    napi_value src = nullptr;
    napi_create_typedarray(env, napi_uint8_clamped_array, arrayBufferSize, arrayBuffer, 0, &src);
    bool res = false;
    napi_value result = types.IsUint8ClampedArray(env, src);
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: textIsUint8Array001
 * @tc.desc: Test check whether the entered value is the type of uint8array
 * @tc.type: FUNCs
 */
HWTEST_F(NativeEngineTest, textIsUint8Array001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textIsUint8Array001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types types;
    napi_value obj = nullptr;
    napi_create_object(env, &obj);
    bool res = true;
    napi_value result = types.IsUint8Array(env, obj);
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: textIsUint8Array002
 * @tc.desc: Test check whether the entered value is the type of uint8array
 * @tc.type: FUNCs
 */
HWTEST_F(NativeEngineTest, textIsUint8Array002, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textIsUint8Array002 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types types;
    napi_value arrayBuffer = nullptr;
    size_t arrayBufferSize = 20;
    void* data = nullptr;
    napi_create_arraybuffer(env, arrayBufferSize * sizeof(size_t), &data, &arrayBuffer);
    napi_value src = nullptr;
    napi_create_typedarray(env, napi_uint8_array, arrayBufferSize, arrayBuffer, 0, &src);
    bool res = false;
    napi_value result = types.IsUint8Array(env, src);
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: testIsSymbolObject001
 * @tc.desc: Test check whether the entered value is the symbol type of object
 * @tc.type: FUNCs
 */
HWTEST_F(NativeEngineTest, testIsSymbolObject001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("testIsSymbolObject001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types types;
    napi_value obj = nullptr;
    napi_create_object(env, &obj);
    bool res = true;
    napi_value result = types.IsSymbolObject(env, obj);
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: testIsSymbolObject002
 * @tc.desc: Test check whether the entered value is the symbol type of object
 * @tc.type: FUNCs
 */
HWTEST_F(NativeEngineTest, testIsSymbolObject002, testing::ext::TestSize.Level0)
{
    HILOG_INFO("testIsSymbolObject002 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types types;

    std::string input = "testSymbol";
    napi_value testSymbol = nullptr;
    napi_create_string_utf8(env, input.c_str(), input.size(), &testSymbol);
    napi_value symbolVal = nullptr;
    napi_create_symbol(env, testSymbol, &symbolVal);

    bool res = true;
    napi_value result = types.IsSymbolObject(env, symbolVal);
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: testIsStringObject001
 * @tc.desc: Test check whether the entered value is the string type of object
 * @tc.type: FUNCs
 */
HWTEST_F(NativeEngineTest, testIsStringObject001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("testIsStringObject001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types types;
    napi_value obj = nullptr;
    napi_create_object(env, &obj);
    bool res = true;
    napi_value result = types.IsStringObject(env, obj);
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: testIsSharedArrayBuffer001
 * @tc.desc: Test check whether the entered value is the type of sharedarraybuffer
 * @tc.type: FUNCs
 */
HWTEST_F(NativeEngineTest, testIsSharedArrayBuffer001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("testIsSharedArrayBuffer001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types types;
    napi_value obj = nullptr;
    napi_create_object(env, &obj);
    bool res = true;
    napi_value result = types.IsSharedArrayBuffer(env, obj);
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: testIsSharedArrayBuffer002
 * @tc.desc: Test check whether the entered value is the type of sharedarraybuffer
 * @tc.type: FUNCs
 */
HWTEST_F(NativeEngineTest, testIsSharedArrayBuffer002, testing::ext::TestSize.Level0)
{
    HILOG_INFO("testIsSharedArrayBuffer002 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types types;
    napi_value arrayBuffer = nullptr;
    void* arrayBufferPtr = nullptr;
    size_t arrayBufferSize = 1024;
    napi_create_arraybuffer(env, arrayBufferSize * sizeof(size_t), &arrayBufferPtr, &arrayBuffer);
    bool res = true;
    napi_value result = types.IsSharedArrayBuffer(env, arrayBuffer);
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: testIsSetIterator001
 * @tc.desc: Test check whether the entered value is the iterator type of set
 * @tc.type: FUNCs
 */
HWTEST_F(NativeEngineTest, testIsSetIterator001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("testIsSetIterator001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types types;
    napi_value obj = nullptr;
    napi_create_object(env, &obj);
    bool res = true;
    napi_value result = types.IsSetIterator(env, obj);
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: testIsSet001
 * @tc.desc: Test check whether the entered value is the type of set
 * @tc.type: FUNCs
 */
HWTEST_F(NativeEngineTest, testIsSet001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("testIsSet001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types types;
    napi_value obj = nullptr;
    napi_create_object(env, &obj);
    bool res = true;
    napi_value result = types.IsSet(env, obj);
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: testIsRegExp001
 * @tc.desc: Test check whether the entered value is the type of regexp
 * @tc.type: FUNCs
 */
HWTEST_F(NativeEngineTest, testIsRegExp001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("testIsRegExp001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types types;
    napi_value obj = nullptr;
    napi_create_object(env, &obj);
    bool res = true;
    napi_value result = types.IsRegExp(env, obj);
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: testIsProxy001
 * @tc.desc: Test check whether the entered value is the type of proxy
 * @tc.type: FUNCs
 */
HWTEST_F(NativeEngineTest, testIsProxy001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("testIsProxy001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types types;
    napi_value obj = nullptr;
    napi_create_object(env, &obj);
    bool res = true;
    napi_value result = types.IsProxy(env, obj);
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: testIsPromise001
 * @tc.desc: Test check whether the entered value is the type of promise
 * @tc.type: FUNCs
 */
HWTEST_F(NativeEngineTest, testIsPromise001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("testIsPromise001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types types;
    napi_value obj = nullptr;
    napi_create_object(env, &obj);
    bool res = true;
    napi_value result = types.IsPromise(env, obj);
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: testIsPromise002
 * @tc.desc: Test check whether the entered value is the type of promise
 * @tc.type: FUNCs
 */
HWTEST_F(NativeEngineTest, testIsPromise002, testing::ext::TestSize.Level0)
{
    HILOG_INFO("testIsPromise002 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types types;
    napi_deferred deferred = nullptr;
    napi_value promise = nullptr;
    ASSERT_CHECK_CALL(napi_create_promise(env, &deferred, &promise));
    ASSERT_NE(deferred, nullptr);
    ASSERT_NE(promise, nullptr);
    bool res = false;
    napi_value result = types.IsPromise(env, promise);
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: testIsNumberObject001
 * @tc.desc: Test check whether the entered value is the number type of object
 * @tc.type: FUNCs
 */
HWTEST_F(NativeEngineTest, testIsNumberObject001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("testIsNumberObject001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types types;
    napi_value obj = nullptr;
    napi_create_object(env, &obj);
    bool res = true;
    napi_value result = types.IsNumberObject(env, obj);
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: testIsNativeError001
 * @tc.desc: Test check whether the entered value is of type error
 * @tc.type: FUNCs
 */
HWTEST_F(NativeEngineTest, testIsNativeError001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("testIsNativeError001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types types;
    napi_value obj = nullptr;
    napi_create_object(env, &obj);
    bool res = true;
    napi_value result = types.IsNativeError(env, obj);
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: testIsNativeError002
 * @tc.desc: Test check whether the entered value is of type error
 * @tc.type: FUNCs
 */
HWTEST_F(NativeEngineTest, testIsNativeError002, testing::ext::TestSize.Level0)
{
    HILOG_INFO("testIsNativeError002 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types types;

    napi_value code = nullptr;
    napi_value message = nullptr;
    std::string input = "abc123";
    ASSERT_CHECK_CALL(napi_create_string_utf8(env, input.c_str(), NAPI_AUTO_LENGTH, &code));
    ASSERT_CHECK_CALL(napi_create_string_utf8(env, input.c_str(), NAPI_AUTO_LENGTH, &message));

    napi_value error = nullptr;
    ASSERT_CHECK_CALL(napi_create_error(env, code, message, &error));
    ASSERT_TRUE(error != nullptr);

    bool res = false;
    napi_value result = types.IsNativeError(env, error);
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: testIsModuleNamespaceObject001
 * @tc.desc: Test check whether the entered value is the module name space type of object
 * @tc.type: FUNCs
 */
HWTEST_F(NativeEngineTest, testIsModuleNamespaceObject001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("testIsModuleNamespaceObject001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types types;
    napi_value obj = nullptr;
    napi_create_object(env, &obj);
    bool res = true;
    napi_value result = types.IsModuleNamespaceObject(env, obj);
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: testIsMapIterator001
 * @tc.desc: Test check whether the entered value is the iterator type of map
 * @tc.type: FUNCs
 */
HWTEST_F(NativeEngineTest, testIsMapIterator001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("testIsMapIterator001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types types;
    napi_value obj = nullptr;
    napi_create_object(env, &obj);
    bool res = true;
    napi_value result = types.IsMapIterator(env, obj);
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: textIsDateTest001
 * @tc.desc: Check if the input value is of type Date.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textIsDateTest001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textIsDateTest001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types type;
    napi_value createResult = nullptr;
    double time = 202110181203150; // Date and Time
    napi_status status = napi_create_date(env, time, &createResult);
    if (status == napi_ok) {
        HILOG_INFO("Types::napi_create_date success");
    } else {
        HILOG_INFO("Types::napi_create_date fail");
    }
    napi_value result = type.IsDate(env, createResult);
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: textIsExternalTest001
 * @tc.desc: Check if the input value is of type native External.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textIsExternalTest001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textIsExternalTest001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types type;
    napi_value dataVal = nullptr;
    int* raw = new int(1);
    napi_status status = napi_create_external(env, (void*) raw, NULL, NULL, &dataVal);
    if (status == napi_ok) {
        HILOG_INFO("Types::napi_create_external success");
    } else {
        HILOG_INFO("Types::napi_create_external fail");
    }
    napi_value result = type.IsExternal(env, dataVal);
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: textIsMapTest001
 * @tc.desc: Check if the input value is of Map type.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textIsMapTest001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textIsMapTest001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types type;
    napi_value resMap = nullptr;
    napi_create_map(env, &resMap);
    napi_value result = type.IsMap(env, resMap);
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: textIsMapTest002
 * @tc.desc: Check if the input value is of Map type.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textIsMapTest002, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textIsMapTest002 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types type;
    napi_value dest = nullptr;
    napi_create_sendable_map(env, &dest);
    napi_value result = type.IsMap(env, dest);
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: textIsAnyArrayBufferTest001
 * @tc.desc: Check if the input value is of type ArrayBuffer or SharedArrayBuffer.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textIsAnyArrayBufferTest001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textIsAnyArrayBufferTest001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types type;
    napi_value arrayBuffer = nullptr;
    void* arrayBufferPtr = nullptr;
    size_t arrayBufferSize = 1;
    napi_create_arraybuffer(env, arrayBufferSize * sizeof(size_t), &arrayBufferPtr, &arrayBuffer);
    napi_value result = type.IsAnyArrayBuffer(env, arrayBuffer);
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: textIsAnyArrayBufferTest002
 * @tc.desc: Check if the input value is of type ArrayBuffer or SharedArrayBuffer.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textIsAnyArrayBufferTest002, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textIsAnyArrayBufferTest002 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types type;
    napi_value testFlag = nullptr;
    bool test = false;
    napi_get_value_bool(env, testFlag, &test);
    napi_value result = type.IsAnyArrayBuffer(env, testFlag);
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: textIsArrayBufferViewTest001
 * @tc.desc: Check if the input value is of the built-in ArrayBufferView auxiliary type.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textIsArrayBufferViewTest001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textIsArrayBufferViewTest001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types type;
    napi_value testFlag = nullptr;
    bool test = false;
    napi_get_value_bool(env, testFlag, &test);
    napi_value result = type.IsArrayBufferView(env, testFlag);
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: textIsArrayBufferViewTest002
 * @tc.desc: Check if the input value is of the built-in ArrayBufferView auxiliary type.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textIsArrayBufferViewTest002, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textIsArrayBufferViewTest002 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types type;
    napi_value arrayBuffer = nullptr;
    void* arrayBufferPtr = nullptr;
    size_t arrayBufferSize = 1;
    napi_create_arraybuffer(env, arrayBufferSize, &arrayBufferPtr, &arrayBuffer);
    napi_value dataview = nullptr;
    napi_create_dataview(env, arrayBufferSize, arrayBuffer, 0, &dataview);
    napi_value result = type.IsArrayBufferView(env, dataview);
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: textIsDataViewTest001
 * @tc.desc: Check if the input value is of type WebView.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textIsDataViewTest001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textIsDataViewTest001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types type;
    napi_value arrBuffer = nullptr;
    void* arrBufferPtr = nullptr;
    size_t arrBufferSize = 1;
    napi_create_arraybuffer(env, arrBufferSize * sizeof(size_t), &arrBufferPtr, &arrBuffer);
    napi_value dataView = nullptr;
    napi_create_dataview(env, arrBufferSize * sizeof(size_t), arrBuffer, 0, &dataView);
    napi_value result = type.IsDataView(env, dataView);
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: textIsArrayBufferViewTest003
 * @tc.desc: Check if the input value is of the built-in ArrayBufferView auxiliary type.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textIsArrayBufferViewTest003, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textIsArrayBufferViewTest003 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types type;
    size_t byteLength = 1;
    void* data = nullptr;
    napi_value resultBuff = nullptr;
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    unsigned char arr[1] = {0x1};
    int ret = memcpy_s(data, sizeof(arr), reinterpret_cast<void*>(arr), sizeof(arr));
    ASSERT_EQ(0, ret);
    napi_value int8Array = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &int8Array);
    napi_value result = type.IsArrayBufferView(env, int8Array);
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: textIsArrayBufferViewTest004
 * @tc.desc: Check if the input value is of the built-in ArrayBufferView auxiliary type.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textIsArrayBufferViewTest004, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textIsArrayBufferViewTest004 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types type;
    size_t byteLength = 1;
    void* data = nullptr;
    napi_value resultBuff = nullptr;
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    unsigned char arr[1] = {0x1};
    int ret = memcpy_s(data, sizeof(arr), reinterpret_cast<void*>(arr), sizeof(arr));
    ASSERT_EQ(0, ret);
    napi_value uint8Array = nullptr;
    napi_create_typedarray(env, napi_uint8_array, byteLength, resultBuff, 0, &uint8Array);
    napi_value result = type.IsArrayBufferView(env, uint8Array);
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: textIsArrayBufferViewTest005
 * @tc.desc: Check if the input value is of the built-in ArrayBufferView auxiliary type.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textIsArrayBufferViewTest005, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textIsArrayBufferViewTest005 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types type;
    size_t byteLength = 1;
    void* data = nullptr;
    napi_value resultBuff = nullptr;
    napi_create_arraybuffer(env, byteLength * sizeof(int16_t), &data, &resultBuff);
    int16_t arr[1] = {0x1};
    int ret = memcpy_s(data, sizeof(arr), reinterpret_cast<void*>(arr), sizeof(arr));
    ASSERT_EQ(0, ret);
    napi_value int16Array = nullptr;
    napi_create_typedarray(env, napi_int16_array, byteLength, resultBuff, 0, &int16Array);
    napi_value result = type.IsArrayBufferView(env, int16Array);
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: textIsArgumentsObjectTest001
 * @tc.desc: Check if the input value is an arguments object.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textIsArgumentsObjectTest001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textIsArgumentsObjectTest001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types type;
    bool test = false;
    napi_value testVal = nullptr;
    napi_get_value_bool(env, testVal, &test);
    napi_value result = type.IsArgumentsObject(env, testVal);
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: textIsArrayBufferTest001
 * @tc.desc: Check if the input value is of type ArrayBuffer.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textIsArrayBufferTest001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textIsArrayBufferTest001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types type;
    napi_value arrayBuffer = nullptr;
    void* arrayBufferPtr = nullptr;
    size_t arrayBufferSize = 1;
    napi_create_arraybuffer(env, arrayBufferSize, &arrayBufferPtr, &arrayBuffer);
    napi_value result = type.IsArrayBuffer(env, arrayBuffer);
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: textIsAsyncFunctionTest001
 * @tc.desc: Check if the input value is an asynchronous function type.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textIsAsyncFunctionTest001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textIsAsyncFunctionTest001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types type;
    bool testRes = false;
    napi_value testRst = nullptr;
    napi_get_value_bool(env, testRst, &testRes);
    napi_value result = type.IsAsyncFunction(env, testRst);
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: textIsInt32ArrayTest001
 * @tc.desc: Check if the input value is of type Int32Array array.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textIsInt32ArrayTest001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textIsInt32ArrayTest001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types type;
    napi_value arrayBuffer = nullptr;
    void* arrayBufferPtr = nullptr;
    size_t arrayBufferSize = 1;
    napi_create_arraybuffer(env, arrayBufferSize * sizeof(size_t), &arrayBufferPtr, &arrayBuffer);
    napi_value dest = nullptr;
    napi_create_typedarray(env, napi_int32_array, arrayBufferSize, arrayBuffer, 0, &dest);
    napi_value result = type.IsInt32Array(env, dest);
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: textIsBigInt64ArrayTest001
 * @tc.desc: Check if the input value is of type BigInt64Array.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textIsBigInt64ArrayTest001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textIsBigInt64ArrayTest001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types type;
    napi_value arrayBuffer = nullptr;
    void* arrayBufferPtr = nullptr;
    size_t arrayBufferSize = 1;
    napi_create_arraybuffer(env, arrayBufferSize * sizeof(int64_t), &arrayBufferPtr, &arrayBuffer);
    napi_value dest = nullptr;
    napi_create_typedarray(env, napi_bigint64_array, arrayBufferSize, arrayBuffer, 0, &dest);
    napi_value result = type.IsBigInt64Array(env, dest);
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: textIsBigUint64ArrayTest001
 * @tc.desc: Check if the input value is of type BigUint64Array.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textIsBigUint64ArrayTest001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textIsBigUint64ArrayTest001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types type;
    napi_value arrayBuffer = nullptr;
    void* arrayBufferPtr = nullptr;
    size_t arrayBufferSize = 20; // number 20
    napi_create_arraybuffer(env, arrayBufferSize * sizeof(uint64_t), &arrayBufferPtr, &arrayBuffer);
    napi_value dest = nullptr;
    napi_create_typedarray(env, napi_biguint64_array, arrayBufferSize, arrayBuffer, 0, &dest);
    napi_value result = type.IsBigUint64Array(env, dest);
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: textIsGeneratorFunctionTest001
 * @tc.desc: Check if the input value is of the generator function type.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textIsGeneratorFunctionTest001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textIsGeneratorFunctionTest001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types type;
    bool resFlag = false;
    napi_value dest = nullptr;
    napi_get_value_bool(env, dest, &resFlag);
    napi_value result = type.IsGeneratorFunction(env, dest);
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: textIsGeneratorObjectTest001
 * @tc.desc: Check if the input value is of the generator object type.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textIsGeneratorObjectTest001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textIsGeneratorObjectTest001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types type;
    napi_value dest = nullptr;
    napi_create_object(env, &dest);
    napi_value result = type.IsGeneratorObject(env, dest);
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: textIsFloat32ArrayTest001
 * @tc.desc: Check if the input value is of Float32Array array type.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textIsFloat32ArrayTest001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textIsFloat32ArrayTest001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types type;
    napi_value arrayBuffer = nullptr;
    void* arrayBufferPtr = nullptr;
    size_t arrayBufferSize = 1;
    napi_create_arraybuffer(env, arrayBufferSize * sizeof(float), &arrayBufferPtr, &arrayBuffer);
    napi_value dest = nullptr;
    napi_create_typedarray(env, napi_float32_array, arrayBufferSize, arrayBuffer, 0, &dest);
    napi_value result = type.IsFloat32Array(env, dest);
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: textIsBooleanObjectTest001
 * @tc.desc: Check if the input value is a Boolean object type.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textIsBooleanObjectTest001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textIsBooleanObjectTest001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types type;
    napi_value testRst = nullptr;
    napi_get_boolean(env, false, &testRst);
    napi_value result = type.IsBooleanObject(env, testRst);
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: textIsBoxedPrimitiveTest001
 * @tc.desc: Check if the input value is of Boolean, Number, String, or Symbol object type.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textIsBoxedPrimitiveTest001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textIsBoxedPrimitiveTest001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types type;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "abcd", NAPI_AUTO_LENGTH, &src);
    napi_value result = type.IsBoxedPrimitive(env, src);
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: textIsFloat64ArrayTest001
 * @tc.desc: Check if the input value is of Float64Array array type.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textIsFloat64ArrayTest001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textIsFloat64ArrayTest001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types type;
    napi_value arrayBuffer = nullptr;
    void* arrayBufferPtr = nullptr;
    size_t arrayBufferSize = 1;
    napi_create_arraybuffer(env, arrayBufferSize * sizeof(double), &arrayBufferPtr, &arrayBuffer);
    napi_value dest = nullptr;
    napi_create_typedarray(env, napi_float64_array, arrayBufferSize, arrayBuffer, 0, &dest);
    napi_value result = type.IsFloat64Array(env, dest);
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: textIsInt8ArrayTest001
 * @tc.desc: Check if the input value is of type Int8Array array.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textIsInt8ArrayTest001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textIsInt8ArrayTest001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types type;
    napi_value arrayBuffer = nullptr;
    void* arrayBufferPtr = nullptr;
    size_t arrayBufferSize = 1;
    napi_create_arraybuffer(env, arrayBufferSize * sizeof(int8_t), &arrayBufferPtr, &arrayBuffer);
    napi_value dest = nullptr;
    napi_create_typedarray(env, napi_int8_array, arrayBufferSize, arrayBuffer, 0, &dest);
    napi_value result = type.IsInt8Array(env, dest);
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: textIsInt16ArrayTest001
 * @tc.desc: Check if the input value is of type Int16Array array.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textIsInt16ArrayTest001, testing::ext::TestSize.Level0)
{
    HILOG_INFO("textIsInt16ArrayTest001 start");
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types type;
    napi_value arrayBuffer = nullptr;
    void* arrayBufferPtr = nullptr;
    size_t arrayBufferSize = 1;
    napi_create_arraybuffer(env, arrayBufferSize * sizeof(int16_t), &arrayBufferPtr, &arrayBuffer);
    napi_value dest = nullptr;
    napi_create_typedarray(env, napi_int16_array, arrayBufferSize, arrayBuffer, 0, &dest);
    napi_value result = type.IsInt16Array(env, dest);
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: textIsArrayBufferViewTest006
 * @tc.desc: Check if the input value is of the built-in ArrayBufferView auxiliary type.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textIsArrayBufferViewTest006, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types type;
    napi_value arrayBuffer = nullptr;
    void* arrayBufferPtr = nullptr;
    size_t arrayBufferSize = 20; // number 20
    napi_create_arraybuffer(env, arrayBufferSize * sizeof(uint64_t), &arrayBufferPtr, &arrayBuffer);
    napi_value dest = nullptr;
    napi_create_typedarray(env, napi_biguint64_array, arrayBufferSize, arrayBuffer, 0, &dest);
    napi_value result = type.IsArrayBufferView(env, dest);
    bool res = false;
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: textIsArgumentsObjectTest002
 * @tc.desc: Check if the input value is an arguments object.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textIsArgumentsObjectTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types type;
    napi_value dest = nullptr;
    napi_create_object(env, &dest);
    napi_value result = type.IsArgumentsObject(env, dest);
    bool res = true;
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

static napi_value SayHello(napi_env env, napi_callback_info info)
{
    napi_value ret;
    NAPI_CALL(env, napi_create_int32(env, 1, &ret));
    return ret;
}

/**
 * @tc.name: textIsAsyncFunctionTest002
 * @tc.desc: Check if the input value is an asynchronous function type.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textIsAsyncFunctionTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types type;
    napi_value funcValue = nullptr;
    napi_create_function(env, nullptr, NAPI_AUTO_LENGTH, SayHello, nullptr, &funcValue);
    bool res = true;
    napi_value result = type.IsAsyncFunction(env, funcValue);
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: textIsBooleanObjectTest002
 * @tc.desc: Check if the input value is a Boolean object type.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textIsBooleanObjectTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types type;
    napi_value dest = nullptr;
    napi_create_object(env, &dest);
    bool res = true;
    napi_value result = type.IsBooleanObject(env, dest);
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: textIsGeneratorFunctionTest002
 * @tc.desc: Check if the input value is of the generator function type.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, textIsGeneratorFunctionTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::Util::Types type;
    napi_value funcValue = nullptr;
    napi_create_function(env, nullptr, NAPI_AUTO_LENGTH, SayHello, nullptr, &funcValue);
    napi_value result = type.IsGeneratorFunction(env, funcValue);
    bool res = true;
    napi_get_value_bool(env, result, &res);
    ASSERT_FALSE(res);
}