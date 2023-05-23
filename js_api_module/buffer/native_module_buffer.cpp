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

#include <algorithm>
#include <iostream>
#include <vector>

#include "commonlibrary/ets_utils/js_api_module/buffer/converter.h"
#include "commonlibrary/ets_utils/js_api_module/buffer/js_blob.h"
#include "commonlibrary/ets_utils/js_api_module/buffer/js_buffer.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

using namespace std;

extern const char _binary_js_buffer_js_start[];
extern const char _binary_js_buffer_js_end[];
extern const char _binary_buffer_abc_start[];
extern const char _binary_buffer_abc_end[];

namespace OHOS::buffer {
enum class ParaType:int32_t {
    NUMBER = 0,
    BUFFER,
    UINT8ARRAY,
    ARRAYBUFFER,
    NUMBERS,
    STRING
};
void FinalizeBufferCallback(napi_env env, void *finalizeData, void *finalizeHint)
{
    if (finalizeData != nullptr) {
        auto obj = reinterpret_cast<Buffer *>(finalizeData);
        delete obj;
    }
}

void FinalizeBlobCallback(napi_env env, void *finalizeData, void *finalizeHint)
{
    if (finalizeData != nullptr) {
        auto obj = reinterpret_cast<Blob *>(finalizeData);
        delete obj;
    }
}

static string GetStringUtf8(napi_env env, napi_value strValue)
{
    string str = "";
    size_t strSize = 0;
    NAPI_CALL(env, napi_get_value_string_utf8(env, strValue, nullptr, 0, &strSize));
    str.reserve(strSize + 1);
    str.resize(strSize);
    NAPI_CALL(env, napi_get_value_string_utf8(env, strValue, const_cast<char *>(str.data()), strSize + 1, &strSize));
    int pos = count(str.begin(), str.end(), '\0');
    if (pos != 0) {
        str.resize(strSize);
    }
    return str;
}

static string GetStringASCII(napi_env env, napi_value strValue)
{
    string str = "";
    size_t strSize = 0;
    NAPI_CALL(env, napi_get_value_string_latin1(env, strValue, nullptr, 0, &strSize));
    str.reserve(strSize + 1);
    str.resize(strSize);
    NAPI_CALL(env, napi_get_value_string_latin1(env, strValue, const_cast<char *>(str.data()), strSize + 1, &strSize));
    return str;
}

static string GetString(napi_env env, EncodingType encodingType, napi_value strValue)
{
    if (encodingType == BASE64 || encodingType == BASE64URL) {
        return GetStringASCII(env, strValue);
    } else {
        return GetStringUtf8(env, strValue);
    }
}

static napi_value FromStringUtf8(napi_env env, napi_value thisVar, napi_value str)
{
    string utf8Str = GetStringUtf8(env, str);
    Buffer *buffer = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void **>(&buffer)));
    buffer->WriteString(utf8Str, utf8Str.length());

    return thisVar;
}

static napi_value FromStringASCII(napi_env env, napi_value thisVar, napi_value str, uint32_t size)
{
    string asciiStr = GetStringASCII(env, str);
    Buffer *buffer = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void **>(&buffer)));

    buffer->WriteString(asciiStr, size);
    return thisVar;
}

static std::u16string GetStringUtf16LE(napi_env env, napi_value strValue)
{
    string utf8Str = GetStringUtf8(env, strValue);
    u16string u16Str = Utf8ToUtf16BE(utf8Str);
    return Utf16BEToLE(u16Str);
}

static napi_value FromStringUtf16LE(napi_env env, napi_value thisVar, napi_value str)
{
    string utf8Str = GetStringUtf8(env, str);
    Buffer *buffer = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void **>(&buffer)));
    u16string u16Str = Utf8ToUtf16BE(utf8Str);
    // 2 : the size of string is 2 times of u16str's length
    buffer->WriteString(u16Str, 0, u16Str.size() * 2);

    return thisVar;
}

static std::string GetStringBase64(napi_env env, napi_value str)
{
    string base64Str = GetStringASCII(env, str);
    string strDecoded = Base64Decode(base64Str);
    return strDecoded;
}

static napi_value FromStringBase64(napi_env env, napi_value thisVar, napi_value str, uint32_t size)
{
    string strDecoded = GetStringBase64(env, str);
    Buffer *buffer = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void **>(&buffer)));

    buffer->WriteString(strDecoded, size);
    return thisVar;
}

static std::string GetStringHex(napi_env env, napi_value str)
{
    string hexStr = GetStringASCII(env, str);
    string strDecoded = HexDecode(hexStr);
    return strDecoded;
}

static napi_value FromStringHex(napi_env env, napi_value thisVar, napi_value str)
{
    string hexStr = GetStringASCII(env, str);
    Buffer *buffer = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&buffer)));

    string strDecoded = HexDecode(hexStr);
    buffer->WriteString(strDecoded, strDecoded.length());
    buffer->SetLength(strDecoded.length());

    return thisVar;
}

static napi_value FromString(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    size_t argc = 0;
    napi_value args[3] = { 0 };
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, nullptr, &thisVar, nullptr));
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));
    // 2 : the third argument
    NAPI_ASSERT(env, args[0] != nullptr && args[1] != nullptr && args[2] != nullptr, "Parameter is empty.");
    
    uint32_t size = 0;
    // 2 : the third argument
    NAPI_CALL(env, napi_get_value_uint32(env, args[2], &size));

    string type = GetStringASCII(env, args[1]);
    EncodingType eType = Buffer::GetEncodingType(type);
    switch (eType) {
        case ASCII:
        case LATIN1:
        case BINARY:
            return FromStringASCII(env, thisVar, args[0], size);
        case UTF8:
            return FromStringUtf8(env, thisVar, args[0]);
        case UTF16LE:
            return FromStringUtf16LE(env, thisVar, args[0]);
        case BASE64:
        case BASE64URL:
            return FromStringBase64(env, thisVar, args[0], size);
        case HEX:
            return FromStringHex(env, thisVar, args[0]);
        default:
            break;
    }

    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

static vector<uint8_t> GetArray(napi_env env, napi_value arr)
{
    uint32_t length = 0;
    napi_get_array_length(env, arr, &length);
    napi_value napiNumber = nullptr;
    vector<uint8_t> vec;
    for (size_t i = 0; i < length; i++) {
        napi_get_element(env, arr, i, &napiNumber);
        uint32_t num = 0;
        napi_get_value_uint32(env, napiNumber, &num);
        // 255 : the max number of one byte unsigned value
        num = num > 255 ? 0 : num;
        vec.push_back(num);
    }
    return vec;
}

static napi_value BlobConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    Blob *blob = new Blob();
    if (blob == nullptr) {
        return nullptr;
    }
    size_t argc = 0;
    napi_value argv[3] = { 0 };
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, nullptr, &thisVar, nullptr));
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));

    if (argc == 1) { // Array
        vector<uint8_t> arr = GetArray(env, argv[0]);
        blob->Init(arr.data(), arr.size());
    } else { // Blob
        Blob *blobIn = nullptr;
        NAPI_CALL(env, napi_unwrap(env, argv[0], reinterpret_cast<void **>(&blobIn)));
        int32_t start = -1;
        NAPI_CALL(env, napi_get_value_int32(env, argv[1], &start));
        // 2 : the argument's count is 2
        if (argc == 2) {
            blob->Init(blobIn, start);
        } else if (argc == 3) { // 3 : the argument's count is 3
            int32_t end = -1;
            // 2 : the third argument
            NAPI_CALL(env, napi_get_value_int32(env, argv[2], &end));
            blob->Init(blobIn, start, end);
        } else {
            return nullptr;
        }
    }
    napi_status status = napi_wrap(env, thisVar, blob,  FinalizeBlobCallback, nullptr, nullptr);
    if (status != napi_ok) {
        if (blob != nullptr) {
            delete blob;
        }
        HILOG_ERROR("can not create blob");
        return nullptr;
    }

    return thisVar;
}

static napi_value GetBufferWrapValue(napi_env env, napi_value thisVar, Buffer *buffer)
{
    napi_status status = napi_wrap(env, thisVar, buffer, FinalizeBufferCallback, nullptr, nullptr);
    if (status != napi_ok) {
        if (buffer != nullptr) {
            delete buffer;
        }
        HILOG_ERROR("can not wrap buffer");
        return nullptr;
    }
    return thisVar;
}

static napi_value BufferConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    Buffer *buffer = new Buffer();
    if (buffer == nullptr) {
        return nullptr;
    }
    size_t argc = 0;
    napi_value argv[4] = { 0 };
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, nullptr, &thisVar, nullptr));
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));

    int32_t pType = -1;
    NAPI_CALL(env, napi_get_value_int32(env, argv[0], &pType));
    ParaType paraType = static_cast<ParaType>(pType);
    if (paraType == ParaType::NUMBER) {
        uint32_t length = 0;
        NAPI_CALL(env, napi_get_value_uint32(env, argv[1], &length));
        buffer->Init(length);
    } else if (paraType == ParaType::NUMBERS) {
        vector<uint8_t> arr = GetArray(env, argv[1]);
        buffer->Init(arr.size());
        buffer->SetArray(arr);
    } else if (paraType == ParaType::BUFFER) {
        if (argc == 2) { // the count of argument is 2
            Buffer *buf = nullptr;
            NAPI_CALL(env, napi_unwrap(env, argv[1], reinterpret_cast<void **>(&buf)));
            buffer->Init(buf);
        } else if (argc == 4) { // the count of argument is 4
            Buffer *pool = nullptr;
            NAPI_CALL(env, napi_unwrap(env, argv[1], reinterpret_cast<void **>(&pool)));
            uint32_t poolOffset = 0;
            // 2 : the third argument
            NAPI_CALL(env, napi_get_value_uint32(env, argv[2], &poolOffset));
            uint32_t length = 0;
            // 3 : the forth argument
            NAPI_CALL(env, napi_get_value_uint32(env, argv[3], &length));
            buffer->Init(pool, poolOffset, length);
        } else {
            return nullptr;
        }
    } else if (paraType == ParaType::STRING) {
        return nullptr;
    } else if (paraType == ParaType::UINT8ARRAY) {
        napi_typedarray_type type = napi_int8_array;
        size_t offset = 0;
        size_t aryLen = 0;
        void *resultData = nullptr;
        napi_value resultBuffer = nullptr;
        NAPI_CALL(env, napi_get_typedarray_info(env, argv[1], &type, &aryLen,
                                                &resultData, &resultBuffer, &offset));
        buffer->Init(reinterpret_cast<uint8_t *>(resultData) - offset, offset, aryLen);
    } else if (paraType == ParaType::ARRAYBUFFER) {
        void *data = nullptr;
        size_t bufferSize = 0;
        // 1 : the second argument
        NAPI_CALL(env, napi_get_arraybuffer_info(env, argv[1], &data, &bufferSize));
        uint32_t byteOffset = 0;
        // 2 : the third argument
        NAPI_CALL(env, napi_get_value_uint32(env, argv[2], &byteOffset));
        uint32_t length = 0;
        // 3 : the forth argument
        NAPI_CALL(env, napi_get_value_uint32(env, argv[3], &length));
        buffer->Init(reinterpret_cast<uint8_t*>(data), byteOffset, length);
    } else {
        napi_throw_error(env, nullptr, "parameter type is error");
        return nullptr;
    }
    
    return GetBufferWrapValue(env, thisVar, buffer);
}

Buffer *GetValueOffsetAndBuf(napi_env env, napi_callback_info info, int32_t *pValue, uint32_t *pOffset)
{
    napi_value thisVar = nullptr;
    size_t argc = 0;
    napi_value args[2] = { 0 };
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, nullptr, &thisVar, nullptr));
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));
    NAPI_ASSERT(env, args[0] != nullptr && args[1] != nullptr, "Parameter is empty.");

    Buffer *buf = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void **>(&buf)));
    NAPI_CALL(env, napi_get_value_int32(env, args[0], pValue));
    NAPI_CALL(env, napi_get_value_uint32(env, args[1], pOffset));
    return buf;
}

Buffer *GetOffsetAndBuf(napi_env env, napi_callback_info info, uint32_t *pOffset)
{
    napi_value thisVar = nullptr;
    size_t argc = 1;
    napi_value args[1] = { 0 };
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, nullptr, &thisVar, nullptr));
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));
    NAPI_ASSERT(env, args[0] != nullptr, "Parameter is empty.");
    
    Buffer *buf = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void **>(&buf)));
    NAPI_CALL(env, napi_get_value_uint32(env, args[0], pOffset));
    return buf;
}

static napi_value WriteInt32BE(napi_env env, napi_callback_info info)
{
    int32_t value = 0;
    uint32_t offset = 0;
    Buffer *buf = GetValueOffsetAndBuf(env, info, &value, &offset);
    if (buf != nullptr) {
        buf->WriteInt32BE(value, offset);
    }
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

static napi_value ReadInt32BE(napi_env env, napi_callback_info info)
{
    uint32_t offset = 0;
    Buffer *buf = GetOffsetAndBuf(env, info, &offset);
    int32_t res = 0;
    if (buf != nullptr) {
        res = buf->ReadInt32BE(offset);
    }
    napi_value result = nullptr;
    napi_create_int32(env, res, &result);
    return result;
}

static napi_value SetArray(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    size_t argc = 1;
    napi_value args[1] = { 0 };
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, nullptr, &thisVar, nullptr));
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));
    NAPI_ASSERT(env, args[0] != nullptr, "Parameter is empty.");
    
    bool isArray = false;
    NAPI_CALL(env, napi_is_array(env, args[0], &isArray));
    if (isArray) {
        Buffer *buf = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void **>(&buf)));
        vector<uint8_t> arr = GetArray(env, args[0]);
        buf->SetArray(arr);
    }
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

static napi_value GetLength(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
    Buffer *buf = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void **>(&buf)));
    uint32_t res = buf->GetLength();
    napi_value result = nullptr;
    napi_create_uint32(env, res, &result);
    return result;
}

static napi_value GetByteOffset(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
    Buffer *buf = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void **>(&buf)));
    uint32_t res = buf->GetByteOffset();
    napi_value result = nullptr;
    napi_create_uint32(env, res, &result);
    return result;
}

static napi_value WriteString(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    size_t argc = 0;
    napi_value args[4] = { 0 };
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, nullptr, &thisVar, nullptr));
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));
    // 4 : 4 arguments is right
    NAPI_ASSERT(env, argc == 4, "Wrong number of arguments");
    NAPI_ASSERT(env, args[0] != nullptr && args[1] != nullptr &&
                // 2 : 3 : the third argument and the forth argument
                args[2] != nullptr && args[3] != nullptr, "Parameter is empty.");

    // 3 : the forth argument
    string encoding = GetStringASCII(env, args[3]);
    EncodingType encodingType = Buffer::GetEncodingType(encoding);
    string value = GetString(env, encodingType, args[0]);

    uint32_t offset = 0;
    uint32_t length = 0;
    NAPI_CALL(env, napi_get_value_uint32(env, args[1], &offset));
    // 2 : the third argument
    NAPI_CALL(env, napi_get_value_uint32(env, args[2], &length));

    Buffer *buf = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void **>(&buf)));
    length = (value.length() < length) ? value.length() : length;
    unsigned int lengthWrote = buf->WriteString(value, offset, length, encoding);

    napi_value result = nullptr;
    napi_create_uint32(env, lengthWrote, &result);
    return result;
}

static napi_value FillString(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    size_t argc = 0;
    napi_value args[4] = { 0 };
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, nullptr, &thisVar, nullptr));
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));
    NAPI_ASSERT(env, args[0] != nullptr && args[1] != nullptr &&
                // 2 : 3 : the third argument and the forth argument
                args[2] != nullptr && args[3] != nullptr, "Parameter is empty.");

    string encoding = GetStringASCII(env, args[3]);
    EncodingType encodingType = Buffer::GetEncodingType(encoding);
    string value = GetString(env, encodingType, args[0]);

    uint32_t offset = 0;
    uint32_t end = 0;
    NAPI_CALL(env, napi_get_value_uint32(env, args[1], &offset));
    // 2 : the third argument
    NAPI_CALL(env, napi_get_value_uint32(env, args[2], &end));

    Buffer *buf = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void **>(&buf)));
    buf->FillString(value, offset, end, encoding);

    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

static napi_value FillNumbers(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    size_t argc = 0;
    napi_value args[3] = { 0 };
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, nullptr, &thisVar, nullptr));
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));
    // 2 : the third argument
    NAPI_ASSERT(env, args[0] != nullptr && args[1] != nullptr && args[2] != nullptr, "Parameter is empty.");

    uint32_t offset = 0;
    uint32_t end = 0;
    NAPI_CALL(env, napi_get_value_uint32(env, args[1], &offset));
    // 2 : the third argument
    NAPI_CALL(env, napi_get_value_uint32(env, args[2], &end));

    Buffer *buf = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void **>(&buf)));
    vector<uint8_t> arr = GetArray(env, args[0]);
    buf->FillNumber(arr, offset, end);

    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

static napi_value FillBuffer(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    size_t argc = 0;
    napi_value args[3] = { 0 };
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, nullptr, &thisVar, nullptr));
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));
    // 2 : the third argument
    NAPI_ASSERT(env, args[0] != nullptr && args[1] != nullptr && args[2] != nullptr, "Parameter is empty.");

    uint32_t offset = 0;
    uint32_t end = 0;
    NAPI_CALL(env, napi_get_value_uint32(env, args[1], &offset));
    // 2 : the third argument
    NAPI_CALL(env, napi_get_value_uint32(env, args[2], &end));

    Buffer *buffer = nullptr;
    NAPI_CALL(env, napi_unwrap(env, args[0], reinterpret_cast<void **>(&buffer)));
    Buffer *ego = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void **>(&ego)));
    ego->FillBuffer(buffer, offset, end);

    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

static napi_value Utf8ByteLength(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    size_t argc = 1;
    napi_value args[1] = { 0 };
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, nullptr, &thisVar, nullptr));
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));
    NAPI_ASSERT(env, args[0] != nullptr, "Parameter is empty.");
    size_t byteLen = 0;
    NAPI_CALL(env, napi_get_value_string_utf8(env, args[0], nullptr, 0, &byteLen));
    napi_value result = nullptr;
    napi_create_uint32(env, byteLen, &result);
    return result;
}

static napi_value GetBufferData(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
    Buffer *buf = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void **>(&buf)));
    uint32_t length = buf->GetLength();
    uint8_t data[length];
    buf->ReadBytes(data, 0, length);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_array(env, &result));
    size_t key = 0;
    napi_value value = nullptr;
    for (int i = 0, len = sizeof(data); i < len; i++) {
        napi_create_uint32(env, data[i], &value);
        napi_set_element(env, result, key, value);
        key++;
    }
    return result;
}

static napi_value Get(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    size_t argc = 1;
    napi_value args[1] = { 0 };
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, nullptr, &thisVar, nullptr));
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));
    NAPI_ASSERT(env, args[0] != nullptr, "Parameter is empty.");
    uint32_t index = 0;
    NAPI_CALL(env, napi_get_value_uint32(env, args[0], &index));
    Buffer *buf = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void **>(&buf)));
    int32_t value = buf->Get(index);
    napi_value result = nullptr;
    napi_create_int32(env, value, &result);
    return result;
}

static napi_value Set(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    size_t argc = 0;
    napi_value args[2] = { 0 };
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, nullptr, &thisVar, nullptr));
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));
    NAPI_ASSERT(env, args[0] != nullptr && args[1] != nullptr, "Parameter is empty.");
    
    Buffer *buf = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void **>(&buf)));
    
    uint32_t index = 0;
    int32_t value = 0;
    NAPI_CALL(env, napi_get_value_uint32(env, args[0], &index));
    NAPI_CALL(env, napi_get_value_int32(env, args[1], &value));
    buf->Set(index, value);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

static napi_value WriteInt32LE(napi_env env, napi_callback_info info)
{
    int32_t value = 0;
    uint32_t offset = 0;
    Buffer *buf = GetValueOffsetAndBuf(env, info, &value, &offset);
    if (buf != nullptr) {
        buf->WriteInt32LE(value, offset);
    }
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

static napi_value ReadInt32LE(napi_env env, napi_callback_info info)
{
    uint32_t offset = 0;
    Buffer *buf = GetOffsetAndBuf(env, info, &offset);
    int32_t res = 0;
    if (buf != nullptr) {
        res = buf->ReadInt32LE(offset);
    }
    napi_value result = nullptr;
    napi_create_int32(env, res, &result);
    return result;
}

static napi_value WriteUInt32BE(napi_env env, napi_callback_info info)
{
    int32_t value = 0;
    uint32_t offset = 0;
    Buffer *buf = GetValueOffsetAndBuf(env, info, &value, &offset);
    if (buf != nullptr) {
        buf->WriteUInt32BE(value, offset);
    }
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

static napi_value ReadUInt32BE(napi_env env, napi_callback_info info)
{
    uint32_t offset = 0;
    Buffer *buf = GetOffsetAndBuf(env, info, &offset);
    uint32_t res = 0;
    if (buf != nullptr) {
        res = buf->ReadUInt32BE(offset);
    }
    napi_value result = nullptr;
    napi_create_uint32(env, res, &result);
    return result;
}

static napi_value WriteUInt32LE(napi_env env, napi_callback_info info)
{
    int32_t value = 0;
    uint32_t offset = 0;
    Buffer *buf = GetValueOffsetAndBuf(env, info, &value, &offset);
    if (buf != nullptr) {
        buf->WriteUInt32LE(value, offset);
    }
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

static napi_value ReadUInt32LE(napi_env env, napi_callback_info info)
{
    uint32_t offset = 0;
    Buffer *buf = GetOffsetAndBuf(env, info, &offset);
    uint32_t res = 0;
    if (buf != nullptr) {
        res = buf->ReadUInt32LE(offset);
    }
    napi_value result = nullptr;
    napi_create_uint32(env, res, &result);
    return result;
}

static napi_value SubBuffer(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    size_t argc = 0;
    napi_value args[2] = { 0 };
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, nullptr, &thisVar, nullptr));
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));
    NAPI_ASSERT(env, args[0] != nullptr && args[1] != nullptr, "Parameter is empty.");
    Buffer *buf = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void **>(&buf)));
    
    uint32_t start = 0;
    uint32_t end = 0;
    NAPI_CALL(env, napi_get_value_uint32(env, args[0], &start));
    NAPI_CALL(env, napi_get_value_uint32(env, args[1], &end));
    Buffer *resBuf = buf->SubBuffer(start, end);
    
    return GetBufferWrapValue(env, thisVar, resBuf);
}

static napi_value Copy(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    size_t argc = 0;
    napi_value args[4] = { 0 };
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, nullptr, &thisVar, nullptr));
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));
    // 4 : 4 arguments is right
    NAPI_ASSERT(env, argc == 4, "Wrong number of arguments");
    
    uint32_t targetStart = 0;
    uint32_t sourceStart = 0;
    uint32_t sourceEnd = 0;
    NAPI_CALL(env, napi_get_value_uint32(env, args[1], &targetStart));
    // 2 : the third argument
    NAPI_CALL(env, napi_get_value_uint32(env, args[2], &sourceStart));
    // 3 : the forth argument
    NAPI_CALL(env, napi_get_value_uint32(env, args[3], &sourceEnd));
    Buffer *targetBuf = nullptr;
    NAPI_CALL(env, napi_unwrap(env, args[0], reinterpret_cast<void **>(&targetBuf)));
    Buffer *sBuf = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void **>(&sBuf)));
    uint32_t cLength = sBuf->Copy(targetBuf, targetStart, sourceStart, sourceEnd);
    napi_value result = nullptr;
    napi_create_int32(env, cLength, &result);
    return result;
}

static napi_value Compare(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    size_t argc = 0;
    napi_value args[4] = { 0 };
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, nullptr, &thisVar, nullptr));
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));
    uint32_t targetStart = 0;
    uint32_t sourceStart = 0;
    uint32_t length = 0;
    NAPI_CALL(env, napi_get_value_uint32(env, args[1], &targetStart));
    // 2 : the third argument
    NAPI_CALL(env, napi_get_value_uint32(env, args[2], &sourceStart));
    // 3 : the forth argument
    NAPI_CALL(env, napi_get_value_uint32(env, args[3], &length));
    Buffer *targetBuf = nullptr;
    NAPI_CALL(env, napi_unwrap(env, args[0], reinterpret_cast<void **>(&targetBuf)));
    Buffer *sBuf = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void **>(&sBuf)));
    int res = sBuf->Compare(targetBuf, targetStart, sourceStart, length);
    napi_value result = nullptr;
    napi_create_int32(env, res, &result);
    return result;
}

static napi_value ToUtf8(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    size_t argc = 0;
    napi_value args[2] = { 0 };
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, nullptr, &thisVar, nullptr));
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));
    uint32_t start = 0;
    uint32_t end = 0;
    NAPI_CALL(env, napi_get_value_uint32(env, args[0], &start));
    NAPI_CALL(env, napi_get_value_uint32(env, args[1], &end));
    Buffer *buf = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&buf)));
    uint32_t length = end - start;
    uint8_t data[length];
    buf->ReadBytes(data, start, length);
    napi_value result = nullptr;
    napi_create_string_utf8(env, reinterpret_cast<char *>(data), length, &result);
    return result;
}

static napi_value ToBase64(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    size_t argc = 0;
    napi_value args[2] = { 0 };
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, nullptr, &thisVar, nullptr));
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));
    uint32_t start = 0;
    uint32_t end = 0;
    NAPI_CALL(env, napi_get_value_uint32(env, args[0], &start));
    NAPI_CALL(env, napi_get_value_uint32(env, args[1], &end));
    Buffer *buf = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&buf)));
    uint32_t length = end - start;
    std::string str = buf->ToBase64(start, length);
    napi_value result = nullptr;
    napi_create_string_latin1(env, str.c_str(), str.length(), &result);
    return result;
}

static napi_value IndexOf(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    size_t argc = 0;
    napi_value args[4] = { 0 };
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, nullptr, &thisVar, nullptr));
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));
    uint32_t offset = 0;
    NAPI_CALL(env, napi_get_value_uint32(env, args[1], &offset));

    // 2 : the third argument
    string type = GetStringASCII(env, args[2]);
    EncodingType eType = Buffer::GetEncodingType(type);
    std::string str;
    std::u16string u16Str;
    uint32_t len = 0;
    const char *data = nullptr;
    switch (eType) {
        case ASCII:
        case LATIN1:
        case BINARY:
            str = GetStringASCII(env, args[0]);
            data = str.c_str();
            break;
        case UTF8:
            str = GetStringUtf8(env, args[0]);
            data = str.c_str();
            break;
        case UTF16LE: {
            u16Str = GetStringUtf16LE(env, args[0]);
            data = reinterpret_cast<char *>(const_cast<char16_t *>(u16Str.c_str()));
            len = u16Str.length() * 2; // 2 : 2 means the length of wide char String is 2 times of char String
            break;
        }
        case BASE64:
        case BASE64URL:
            str = GetStringBase64(env, args[0]);
            data = str.c_str();
            break;
        case HEX:
            str = GetStringHex(env, args[0]);
            data = str.c_str();
            break;
        default:
            break;
    }

    Buffer *buf = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void **>(&buf)));
    bool isReverse = false;
    // 3 : the forth argument
    NAPI_CALL(env, napi_get_value_bool(env, args[3], &isReverse));
    int index = -1;
    if (isReverse) {
        len = (eType == UTF16LE) ? len : str.length();
        index = buf->LastIndexOf(data, offset, len);
    } else {
        len = (eType == UTF16LE) ? len : str.length();
        index = buf->IndexOf(data, offset, len);
    }
    napi_value result = nullptr;
    napi_create_int32(env, index, &result);
    return result;
}

static napi_value Utf8StringToNumbers(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    size_t argc = 0;
    napi_value args[1] = { 0 };
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, nullptr, &thisVar, nullptr));
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));

    std::string str = GetStringUtf8(env, args[0]);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_array(env, &result));
    size_t key = 0;
    napi_value value = nullptr;
    for (uint32_t i = 0; i < str.length(); i++) {
        napi_create_uint32(env, uint32_t(str.at(i) & 0xFF), &value);
        napi_set_element(env, result, key, value);
        key++;
    }
    return result;
}

struct PromiseInfo {
    napi_env env = nullptr;
    napi_async_work worker = nullptr;
    napi_deferred deferred = nullptr;
    napi_value promise = nullptr;
    Blob* jsBlob = nullptr;
    napi_value arrayBuffer = nullptr;
    napi_value string = nullptr;
};

static void CopiedBlobToArrayBuffer(napi_env env, napi_status status, void *data)
{
    auto promiseInfo = reinterpret_cast<PromiseInfo *>(data);
    Blob *blob = promiseInfo->jsBlob;
    void *bufdata = nullptr;
    size_t bufferSize = blob->GetLength();
    napi_create_arraybuffer(env, bufferSize, &bufdata, &promiseInfo->arrayBuffer);
    blob->ReadBytes(reinterpret_cast<uint8_t *>(bufdata), bufferSize);
    napi_resolve_deferred(env, promiseInfo->deferred, promiseInfo->arrayBuffer);
    napi_delete_async_work(env, promiseInfo->worker);
    delete promiseInfo;
}

static void CopiedBlobToString(napi_env env, napi_status status, void *data)
{
    auto promiseInfo = reinterpret_cast<PromiseInfo *>(data);
    Blob *blob = promiseInfo->jsBlob;
    napi_create_string_utf8(env, reinterpret_cast<char *>(blob->GetRaw()), blob->GetLength(), &promiseInfo->string);
    napi_resolve_deferred(env, promiseInfo->deferred, promiseInfo->string);
    napi_delete_async_work(env, promiseInfo->worker);
    delete promiseInfo;
}

static napi_value ArrayBufferAsync(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    PromiseInfo *promiseInfo = new PromiseInfo();
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
    Blob *blob = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void **>(&blob)));
    promiseInfo->jsBlob = blob;
    napi_create_promise(env, &promiseInfo->deferred, &promiseInfo->promise);
    napi_create_string_utf8(env, "CopyBlobToArrayBuffer", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env, nullptr, resourceName, nullptr, CopiedBlobToArrayBuffer,
                           reinterpret_cast<void *>(promiseInfo), &promiseInfo->worker);
    napi_queue_async_work(env, promiseInfo->worker);
    return promiseInfo->promise;
}

static napi_value TextAsync(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    PromiseInfo *promiseInfo = new PromiseInfo();
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
    Blob *blob = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void **>(&blob)));
    promiseInfo->jsBlob = blob;

    napi_create_promise(env, &promiseInfo->deferred, &promiseInfo->promise);
    napi_create_string_utf8(env, "GetPromiseOfString", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env, nullptr, resourceName, nullptr, CopiedBlobToString,
                           reinterpret_cast<void *>(promiseInfo), &promiseInfo->worker);
    napi_queue_async_work(env, promiseInfo->worker);

    return promiseInfo->promise;
}

static napi_value GetBytes(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
    Blob *blob = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void **>(&blob)));
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_array(env, &result));
    size_t key = 0;
    napi_value value = nullptr;
    for (unsigned int i = 0; i < blob->GetLength(); i++) {
        napi_create_uint32(env, uint32_t(blob->GetByte(i) & 0xFF), &value);
        napi_set_element(env, result, key, value);
        key++;
    }
    return result;
}

static napi_value BufferInit(napi_env env, napi_value exports)
{
    string className = "Buffer";
    napi_value bufferClass = nullptr;
    napi_property_descriptor bufferDesc[] = {
        DECLARE_NAPI_FUNCTION("writeInt32BE", WriteInt32BE),
        DECLARE_NAPI_FUNCTION("readInt32BE", ReadInt32BE),
        DECLARE_NAPI_FUNCTION("writeInt32LE", WriteInt32LE),
        DECLARE_NAPI_FUNCTION("readInt32LE", ReadInt32LE),
        DECLARE_NAPI_FUNCTION("writeUInt32BE", WriteUInt32BE),
        DECLARE_NAPI_FUNCTION("readUInt32BE", ReadUInt32BE),
        DECLARE_NAPI_FUNCTION("writeUInt32LE", WriteUInt32LE),
        DECLARE_NAPI_FUNCTION("readUInt32LE", ReadUInt32LE),
        DECLARE_NAPI_FUNCTION("setArray", SetArray),
        DECLARE_NAPI_FUNCTION("getLength", GetLength),
        DECLARE_NAPI_FUNCTION("getByteOffset", GetByteOffset),
        DECLARE_NAPI_FUNCTION("writeString", WriteString),
        DECLARE_NAPI_FUNCTION("fromString", FromString),
        DECLARE_NAPI_FUNCTION("fillString", FillString),
        DECLARE_NAPI_FUNCTION("fillNumbers", FillNumbers),
        DECLARE_NAPI_FUNCTION("fillBuffer", FillBuffer),
        DECLARE_NAPI_FUNCTION("getBufferData", GetBufferData),
        DECLARE_NAPI_FUNCTION("get", Get),
        DECLARE_NAPI_FUNCTION("set", Set),
        DECLARE_NAPI_FUNCTION("subBuffer", SubBuffer),
        DECLARE_NAPI_FUNCTION("copy", Copy),
        DECLARE_NAPI_FUNCTION("compare", Compare),
        DECLARE_NAPI_FUNCTION("toUtf8", ToUtf8),
        DECLARE_NAPI_FUNCTION("toBase64", ToBase64),
        DECLARE_NAPI_FUNCTION("indexOf", IndexOf),
    };
    NAPI_CALL(env, napi_define_class(env, className.c_str(), className.length(), BufferConstructor,
                                     nullptr, sizeof(bufferDesc) / sizeof(bufferDesc[0]), bufferDesc, &bufferClass));
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_PROPERTY("Buffer", bufferClass),
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    return exports;
}

static napi_value BlobInit(napi_env env, napi_value exports)
{
    string className = "Blob";
    napi_value blobClass = nullptr;
    napi_property_descriptor blobDesc[] = {
        DECLARE_NAPI_FUNCTION("arraybuffer", ArrayBufferAsync),
        DECLARE_NAPI_FUNCTION("text", TextAsync),
        DECLARE_NAPI_FUNCTION("getBytes", GetBytes),
    };
    NAPI_CALL(env, napi_define_class(env, className.c_str(), className.length(), BlobConstructor,
                                     nullptr, sizeof(blobDesc) / sizeof(blobDesc[0]), blobDesc, &blobClass));
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_PROPERTY("Blob", blobClass),
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    return exports;
}

static napi_value Init(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("utf8ByteLength", Utf8ByteLength),
        DECLARE_NAPI_FUNCTION("utf8StringToNumbers", Utf8StringToNumbers),
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    BufferInit(env, exports);
    BlobInit(env, exports);
    return exports;
}

extern "C"
__attribute__((visibility("default"))) void NAPI_buffer_GetJSCode(const char **buf, int *bufLen)
{
    if (buf != nullptr) {
        *buf = _binary_js_buffer_js_start;
    }

    if (bufLen != nullptr) {
        *bufLen = _binary_js_buffer_js_end - _binary_js_buffer_js_start;
    }
}

extern "C"
__attribute__((visibility("default"))) void NAPI_buffer_GetABCCode(const char** buf, int* buflen)
{
    if (buf != nullptr) {
        *buf = _binary_buffer_abc_start;
    }
    if (buflen != nullptr) {
        *buflen = _binary_buffer_abc_end - _binary_buffer_abc_start;
    }
}

static napi_module bufferModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "buffer",
    .nm_priv = reinterpret_cast<void *>(0),
    .reserved = {0},
};

extern "C" __attribute__((constructor)) void RegisterModule()
{
    napi_module_register(&bufferModule);
}
} // namespace OHOS::Buffer
