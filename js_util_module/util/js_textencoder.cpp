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
#include "utils/log.h"
#include "util_helper.h"

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
        size_t inputSize = 0;
        napi_get_value_string_utf16(env, src, nullptr, 0, &inputSize);
        char16_t originalBuffer[inputSize + 1];
        napi_get_value_string_utf16(env, src, originalBuffer, inputSize + 1, &inputSize);

        std::string buffer = UnicodeConversion(originalBuffer, inputSize);
        if (buffer == "") {
            HILOG_ERROR("buffer is null.");
            return nullptr;
        }
        size_t outLen = buffer.length();
        void *data = nullptr;
        napi_value arrayBuffer = nullptr;
        napi_create_arraybuffer(env, outLen, &data, &arrayBuffer);
        if (memcpy_s(data, outLen, reinterpret_cast<void*>(buffer.data()), outLen) != EOK) {
            HILOG_ERROR("copy buffer to arraybuffer error");
            return nullptr;
        }
        napi_value result = nullptr;
        napi_create_typedarray(env, napi_uint8_array, outLen, arrayBuffer, 0, &result);
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

        char16_t targetBuffer;
        size_t writableSize = length;
        std::string bufferResult = "";
        size_t i = 0;
        for (; i < length; i++) {
            targetBuffer = originalBuffer[i];
            std::string buffer = UnicodeConversion(&targetBuffer, inputSize);
            size_t bufferLength = buffer.length() - 1;
            if (bufferLength > writableSize) {
                break;
            }
            bufferResult += buffer.substr(0, bufferLength);
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

    std::string TextEncoder::UnicodeConversion(char16_t* originalBuffer, size_t inputSize) const
    {
#if !defined(ANDROID_PLATFORM) && !defined(IOS_PLATFORM)
        std::string buffer = "";
        UErrorCode codeflag = U_ZERO_ERROR;
        UConverter* converter = ucnv_open(encoding_.c_str(), &codeflag);
        if (U_FAILURE(codeflag)) {
            HILOG_ERROR("ucnv_open failed !");
            return "";
        }

        size_t maxByteSize = static_cast<size_t>(ucnv_getMaxCharSize(converter));
        const UChar *source = static_cast<UChar*>(originalBuffer);

        size_t limit = maxByteSize * inputSize;
        size_t len = limit * sizeof(char);
        char *targetArray = nullptr;
        if (limit > 0) {
            targetArray = new char[limit + 1];
            if (memset_s(targetArray, len + sizeof(char), 0, len + sizeof(char)) != EOK) {
                HILOG_ERROR("encode targetArray memset_s failed");
                ucnv_close(converter);
                FreedMemory(targetArray);
                return "";
            }
        } else {
            HILOG_ERROR("limit is error");
            ucnv_close(converter);
            return "";
        }

        char *target = targetArray;
        const char *targetLimit = targetArray + limit;
        const UChar *sourceLimit = source + u_strlen(source);

        ucnv_fromUnicode(converter, &target, targetLimit, &source, sourceLimit, nullptr, true, &codeflag);
        if (U_FAILURE(codeflag)) {
            HILOG_ERROR("ucnv_fromUnicode conversion failed.");
            ucnv_close(converter);
            FreedMemory(targetArray);
            return "";
        }
        buffer = targetArray;
        ucnv_close(converter);
        FreedMemory(targetArray);
        return buffer;
#else
        return "";
#endif
    }

    void TextEncoder::FreedMemory(char *data) const
    {
        if (data != nullptr) {
            delete[] data;
            data = nullptr;
        }
    }
}
