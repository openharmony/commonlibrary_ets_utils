/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "util_helper.h"

#include "native_engine.h"
#include "securec.h"

namespace Commonlibrary::Platform {
    UConverter* CreateConverter(const std::string& encStr_, UErrorCode& codeflag)
    {
        UConverter *conv = ucnv_open(encStr_.c_str(), &codeflag);
        if (U_FAILURE(codeflag)) {
            HILOG_ERROR("Unable to create a UConverter object: %s\n", u_errorName(codeflag));
            return NULL;
        }
        ucnv_setFromUCallBack(conv, UCNV_FROM_U_CALLBACK_SUBSTITUTE, NULL, NULL, NULL, &codeflag);
        if (U_FAILURE(codeflag)) {
            HILOG_ERROR("Unable to set the from Unicode callback function");
            ucnv_close(conv);
            return NULL;
        }

        ucnv_setToUCallBack(conv, UCNV_TO_U_CALLBACK_SUBSTITUTE, NULL, NULL, NULL, &codeflag);
        if (U_FAILURE(codeflag)) {
            HILOG_ERROR("Unable to set the to Unicode callback function");
            ucnv_close(conv);
            return NULL;
        }
        return conv;
    }

    std::string ConvertToString(UChar * uchar, size_t length)
    {
        std::u16string tempStr16(uchar);
        std::string tepStr = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.to_bytes(tempStr16);
        return tepStr;
    }

    void EncodeIntoChinese(napi_env env, napi_value src, std::string encoding, std::string& buffer)
    {
        NativeEngine *engine = reinterpret_cast<NativeEngine*>(env);
        engine->EncodeToChinese(src, buffer, encoding);
    }

    std::string UnicodeConversion(std::string encoding, char16_t* originalBuffer, size_t inputSize)
    {
        std::string buffer = "";
        UErrorCode codeflag = U_ZERO_ERROR;
        UConverter* converter = ucnv_open(encoding.c_str(), &codeflag);
        if (U_FAILURE(codeflag)) {
            HILOG_ERROR("textencoder::ucnv_open failed !");
            return "";
        }

        size_t maxByteSize = static_cast<size_t>(ucnv_getMaxCharSize(converter));
        const UChar *source = originalBuffer;
        size_t limit = maxByteSize * inputSize;
        size_t len = limit * sizeof(char);
        char *targetArray = nullptr;
        if (limit > 0) {
            targetArray = new char[limit + 1];
            if (memset_s(targetArray, len + sizeof(char), 0, len + sizeof(char)) != EOK) {
                HILOG_ERROR("textencoder::encode targetArray memset_s failed");
                ucnv_close(converter);
                FreedMemory(targetArray);
                return "";
            }
        } else {
            HILOG_ERROR("textencoder::limit is error");
            ucnv_close(converter);
            return "";
        }

        char *target = targetArray;
        const char *targetLimit = targetArray + limit;
        const UChar *sourceLimit = source + u_strlen(source);

        ucnv_fromUnicode(converter, &target, targetLimit, &source, sourceLimit, nullptr, true, &codeflag);
        if (U_FAILURE(codeflag)) {
            HILOG_ERROR("textencoder::ucnv_fromUnicode conversion failed.");
            ucnv_close(converter);
            FreedMemory(targetArray);
            return "";
        }

        buffer = targetArray;
        ucnv_close(converter);
        FreedMemory(targetArray);
        return buffer;
    }

    void EncodeToUtf8(TextEcodeInfo encodeInfo, char* writeResult, int32_t* written, size_t length, int32_t* nchars)
    {
        size_t inputSize = 0;
        napi_get_value_string_utf16(encodeInfo.env, encodeInfo.src, nullptr, 0, &inputSize);
        char16_t originalBuffer[inputSize + 1];
        napi_get_value_string_utf16(encodeInfo.env, encodeInfo.src, originalBuffer, inputSize + 1, &inputSize);

        char16_t targetBuffer[inputSize + 1];
        size_t writableSize = length;
        std::string bufferResult = "";
        size_t i = 0;
        for (; i < inputSize; i++) {
            targetBuffer[i] = originalBuffer[i];
            std::string buffer = UnicodeConversion(encodeInfo.encoding, &targetBuffer[i], inputSize);
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
        *nchars = static_cast<int32_t>(i);
        *written = static_cast<int32_t>(writeLength);
    }

    void EncodeConversion(napi_env env, napi_value src, napi_value* arrayBuffer, size_t &outLens, std::string encoding)
    {
        size_t  outLen = 0;
        void *data = nullptr;
        size_t inputSize = 0;
        napi_get_value_string_utf16(env, src, nullptr, 0, &inputSize);
        char16_t originalBuffer[inputSize + 1];
        napi_get_value_string_utf16(env, src, originalBuffer, inputSize + 1, &inputSize);

        int maxByteSize = GetMaxByteSize(encoding);
        outLen = maxByteSize * inputSize;
        napi_create_arraybuffer(env, outLen, &data, arrayBuffer);
        char *writeResult = static_cast<char*>(data);

        std::string buffer = "";
        std::u16string originalStr(originalBuffer, inputSize);
        int shifting = 0;
        int resultShifting = 0;
        int findIndex = originalStr.find('\0');
        if (findIndex == -1) {
            buffer = UnicodeConversion(encoding, originalBuffer, inputSize);
            outLens = buffer.length();
            if (memcpy_s(writeResult, outLens, reinterpret_cast<char*>(buffer.data()), outLens) != EOK) {
                HILOG_FATAL("textencoder::copy buffer to arraybuffer error");
                return;
            }
        } else {
            while (findIndex != -1) {
                buffer = UnicodeConversion(encoding, originalBuffer + shifting, inputSize);
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
            buffer = UnicodeConversion(encoding, originalBuffer + shifting, inputSize);
            outLens += buffer.length();
            if (memcpy_s(writeResult + resultShifting, buffer.length(),
                         reinterpret_cast<char*>(buffer.data()), buffer.length()) != EOK) {
                HILOG_FATAL("textencoder::copy buffer to arraybuffer error");
                return;
            }
        }
    }

    int GetMaxByteSize(std::string encoding)
    {
        UErrorCode codeflag = U_ZERO_ERROR;
        UConverter* converter = ucnv_open(encoding.c_str(), &codeflag);
        if (U_FAILURE(codeflag)) {
            HILOG_ERROR("textencoder::ucnv_open failed !");
            return 0;
        }

        int maxByteSize = static_cast<int>(ucnv_getMaxCharSize(converter));
        ucnv_close(converter);
        return maxByteSize;
    }

    void FreedMemory(char *data)
    {
        if (data != nullptr) {
            delete[] data;
            data = nullptr;
        }
    }
} // namespace Commonlibrary::Platform