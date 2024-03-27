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

#include "js_textencoder.h"

#include "native_engine.h"
#include "securec.h"
#include "util_helper.h"
#include "tools/log.h"

namespace OHOS::Util {
    using namespace Commonlibrary::Platform;
    napi_value TextEncoder::GetEncoding(napi_env env) const
    {
        napi_value result = nullptr;
        NAPI_CALL(env, napi_create_string_utf8(env, encoding_.c_str(), encoding_.length(), &result));

        return result;
    }

    napi_value TextEncoder::Encode(napi_env env, napi_value src) const
    {
        std::string buffer = "";
        size_t  outLen = 0;
        size_t outLens = 0;
        void *data = nullptr;
        napi_value arrayBuffer = nullptr;
        if (encoding_ == "utf-8" || encoding_ == "UTF-8") {
            size_t bufferSize = 0;
            if (napi_get_value_string_utf8(env, src, nullptr, 0, &bufferSize) != napi_ok) {
                HILOG_ERROR("textencoder::can not get src size");
                return nullptr;
            }
            buffer.resize(bufferSize);
            if (napi_get_value_string_utf8(env, src, buffer.data(), bufferSize + 1, &bufferSize) != napi_ok) {
                HILOG_ERROR("textencoder::can not get src value");
                return nullptr;
            }
            outLen = buffer.length();
            outLens = outLen;
            napi_create_arraybuffer(env, outLen, &data, &arrayBuffer);
            if (memcpy_s(data, outLen, reinterpret_cast<void*>(buffer.data()), outLen) != EOK) {
                HILOG_FATAL("textencoder::copy buffer to arraybuffer error");
                return nullptr;
            }
        } else {
            size_t inputSize = 0;
            napi_get_value_string_utf16(env, src, nullptr, 0, &inputSize);
            char16_t originalBuffer[inputSize + 1];
            napi_get_value_string_utf16(env, src, originalBuffer, inputSize + 1, &inputSize);

            int maxByteSize = GetMaxByteSize(encoding_);
            outLen = maxByteSize * inputSize;
            napi_create_arraybuffer(env, outLen, &data, &arrayBuffer);
            char *writeResult = static_cast<char*>(data);
            EncodeConversion(originalBuffer, writeResult, inputSize, outLens);
        }

        napi_value result = nullptr;
        napi_create_typedarray(env, napi_uint8_array, outLens, arrayBuffer, 0, &result);
        return result;
    }

    napi_value TextEncoder::EncodeInto(napi_env env, napi_value src, napi_value dest) const
    {
        napi_typedarray_type type;
        size_t byteOffset = 0;
        size_t length = 0;
        void *resultData = nullptr;
        napi_value resultBuffer = nullptr;
        NAPI_CALL(env, napi_get_typedarray_info(env, dest, &type, &length, &resultData, &resultBuffer, &byteOffset));

        char *writeResult = static_cast<char*>(resultData);

        int32_t nchars = 0;
        int32_t written = 0;
        size_t inputSize = 0;
        napi_get_value_string_utf16(env, src, nullptr, 0, &inputSize);
        char16_t originalBuffer[inputSize + 1];
        napi_get_value_string_utf16(env, src, originalBuffer, inputSize + 1, &inputSize);

        char16_t targetBuffer[inputSize + 1];
        size_t writableSize = length;
        std::string bufferResult = "";
        size_t i = 0;
        for (; i < inputSize; i++) {
            targetBuffer[i] = originalBuffer[i];
            std::string buffer = UnicodeConversion(encoding_, &targetBuffer[i], inputSize);
            size_t bufferLength = buffer.length();
            if (bufferLength > writableSize) {
                break;
            }
            bufferResult += buffer;
            writableSize -= bufferLength;
        }

        size_t writeLength = bufferResult.length();
        for (size_t j = 0; j < writeLength; j++) {
            *writeResult = bufferResult[j];
            writeResult++;
        }
        nchars = static_cast<int32_t>(i);
        written = static_cast<int32_t>(writeLength);

        napi_value result = nullptr;
        NAPI_CALL(env, napi_create_object(env, &result));

        napi_value read = nullptr;
        NAPI_CALL(env, napi_create_int32(env, nchars, &read));

        NAPI_CALL(env, napi_set_named_property(env, result, "read", read));

        napi_value resWritten = nullptr;
        NAPI_CALL(env, napi_create_int32(env, written, &resWritten));

        NAPI_CALL(env, napi_set_named_property(env, result, "written", resWritten));

        return result;
    }

    void TextEncoder::EncodeConversion(char16_t* originalBuffer, char* writeResult,
                                       size_t inputSize, size_t &outLens) const
    {
        std::string buffer = "";
        std::u16string originalStr(originalBuffer, inputSize);
        int shifting = 0;
        int resultShifting = 0;
        int findIndex = originalStr.find('\0');
        if (findIndex == -1) {
            buffer = UnicodeConversion(encoding_, originalBuffer, inputSize);
            outLens = buffer.length();
            if (memcpy_s(writeResult, outLens, reinterpret_cast<char*>(buffer.data()), outLens) != EOK) {
                HILOG_FATAL("textencoder::copy buffer to arraybuffer error");
                return;
            }
        } else {
            while (findIndex != -1) {
                buffer = UnicodeConversion(encoding_, originalBuffer + shifting, inputSize);
                if (memcpy_s(writeResult + resultShifting, buffer.length(),
                             reinterpret_cast<char*>(buffer.data()), buffer.length()) != EOK) {
                    HILOG_FATAL("textencoder::copy buffer to arraybuffer error");
                    return;
                }
                resultShifting +=  buffer.length();
                *(writeResult + resultShifting) = '\0';
                resultShifting += 1;
                outLens += buffer.length() + 1;
                shifting += findIndex + 1;
                originalStr = originalStr.substr(findIndex + 1, inputSize);
                findIndex = originalStr.find('\0');
            }
            buffer = UnicodeConversion(encoding_, originalBuffer + shifting, inputSize);
            outLens += buffer.length();
            if (memcpy_s(writeResult + resultShifting, buffer.length(),
                         reinterpret_cast<char*>(buffer.data()), buffer.length()) != EOK) {
                HILOG_FATAL("textencoder::copy buffer to arraybuffer error");
                return;
            }
        }
    }
}
