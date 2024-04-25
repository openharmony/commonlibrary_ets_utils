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
        if (encodeInfo.encoding == "utf-16be" || encodeInfo.encoding == "utf-16le") {
            EncodeTo16BE(encodeInfo, writeResult, written, length, nchars);
        } else {
            OtherEncodeUtf8(encodeInfo, writeResult, written, length, nchars);
        }
    }

    void EncodeConversion(napi_env env, napi_value src, napi_value* arrayBuffer, size_t &outLens, std::string encoding)
    {
        if (encoding == "utf-16be") {
            size_t  outLen = 0;
            void *data = nullptr;
            std::u16string u16Str = EncodeUtf16BE(env, src);
            outLen = u16Str.length() * 2; // 2:multiple
            outLens = outLen;
            napi_create_arraybuffer(env, outLen, &data, arrayBuffer);
            if (memcpy_s(data, outLen, reinterpret_cast<void*>(u16Str.data()), outLen) != EOK) {
                HILOG_FATAL("textencoder::copy buffer to arraybuffer error");
                return;
            }
        } else if (encoding == "utf-16le") {
            size_t  outLen = 0;
            void *data = nullptr;
            std::u16string u16BEStr = EncodeUtf16BE(env, src);
            std::u16string u16LEStr = Utf16BEToLE(u16BEStr);
            outLen = u16LEStr.length() * 2; // 2:multiple
            outLens = outLen;
            napi_create_arraybuffer(env, outLen, &data, arrayBuffer);
            if (memcpy_s(data, outLen, reinterpret_cast<void*>(u16LEStr.data()), outLen) != EOK) {
                HILOG_FATAL("textencoder::copy buffer to arraybuffer error");
                return;
            }
        } else {
            OtherEncode(env, src, arrayBuffer, outLens, encoding);
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

    bool IsOneByte(uint8_t u8Char)
    {
        return (u8Char & 0x80) == 0;
    }

    std::u16string Utf8ToUtf16BE(const std::string &u8Str, bool *ok)
    {
        std::u16string u16Str = u"";
        u16Str.reserve(u8Str.size());
        std::string::size_type len = u8Str.length();
        const unsigned char *data = reinterpret_cast<const unsigned char *>(u8Str.data());
        bool isOk = true;
        for (std::string::size_type i = 0; i < len; ++i) {
            uint8_t c1 = data[i];
            if (IsOneByte(c1)) {
                u16Str.push_back(static_cast<char16_t>(c1));
                continue;
            }
            switch (c1 & HIGER_4_BITS_MASK) {
                case FOUR_BYTES_STYLE: {
                    uint8_t c2 = data[++i];
                    uint8_t c3 = data[++i];
                    uint8_t c4 = data[++i];
                    uint32_t codePoint = ((c1 & LOWER_3_BITS_MASK) << (3 * UTF8_VALID_BITS)) | // 3:multiple
                        ((c2 & LOWER_6_BITS_MASK) << (2 * UTF8_VALID_BITS)) | // 2:multiple
                        ((c3 & LOWER_6_BITS_MASK) << UTF8_VALID_BITS) | (c4 & LOWER_6_BITS_MASK);
                    if (codePoint >= UTF16_SPECIAL_VALUE) {
                        codePoint -= UTF16_SPECIAL_VALUE;
                        u16Str.push_back(static_cast<char16_t>((codePoint >> 10) | HIGH_AGENT_MASK)); // 10:offset value
                        u16Str.push_back(static_cast<char16_t>((codePoint & LOWER_10_BITS_MASK) | LOW_AGENT_MASK));
                    } else {
                        u16Str.push_back(static_cast<char16_t>(codePoint));
                    }
                    break;
                }
                case THREE_BYTES_STYLE: {
                    uint8_t c2 = data[++i];
                    uint8_t c3 = data[++i];
                    uint32_t codePoint = ((c1 & LOWER_4_BITS_MASK) << (2 * UTF8_VALID_BITS)) | // 2:multiple
                        ((c2 & LOWER_6_BITS_MASK) << UTF8_VALID_BITS) | (c3 & LOWER_6_BITS_MASK);
                    u16Str.push_back(static_cast<char16_t>(codePoint));
                    break;
                }
                case TWO_BYTES_STYLE1:
                case TWO_BYTES_STYLE2: {
                    uint8_t c2 = data[++i];
                    uint32_t codePoint = ((c1 & LOWER_5_BITS_MASK) << UTF8_VALID_BITS) | (c2 & LOWER_6_BITS_MASK);
                    u16Str.push_back(static_cast<char16_t>(codePoint));
                    break;
                }
                default: {
                    isOk = false;
                    break;
                }
            }
        }
        if (ok != nullptr) {
            *ok = isOk;
        }
        return u16Str;
    }

    std::u16string Utf16BEToLE(const std::u16string &wstr)
    {
        std::u16string str16 = u"";
        const char16_t *data = wstr.data();
        for (unsigned int i = 0; i < wstr.length(); i++) {
            char16_t wc = data[i];
            char16_t high = (wc >> 8) & 0x00FF; // 8:offset value
            char16_t low = wc & 0x00FF;
            char16_t c16 = (low << 8) | high; // 8:offset value
            str16.push_back(c16);
        }
        return str16;
    }

    void OtherEncode(napi_env env, napi_value src, napi_value* arrayBuffer, size_t &outLens, std::string encoding)
    {
        size_t  outLen = 0;
        void *data = nullptr;
        size_t inputSize = 0;
        napi_get_value_string_utf16(env, src, nullptr, 0, &inputSize);
        char16_t originalBuffer[inputSize + 1];
        napi_get_value_string_utf16(env, src, originalBuffer, inputSize + 1, &inputSize);

        int maxByteSize = GetMaxByteSize(encoding);
        outLen = static_cast<size_t>(maxByteSize) * inputSize;
        napi_create_arraybuffer(env, outLen, &data, arrayBuffer);
        char *writeResult = static_cast<char*>(data);

        std::string buffer = "";
        std::u16string originalStr(originalBuffer, inputSize);
        int shifting = 0;
        size_t resultShifting = 0;
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

    std::u16string EncodeUtf16BE(napi_env env, napi_value src)
    {
        std::string buffer = "";
        size_t bufferSize = 0;

        if (napi_get_value_string_utf8(env, src, nullptr, 0, &bufferSize) != napi_ok) {
            HILOG_ERROR("textencoder::can not get src size");
            return u"";
        }
        buffer.resize(bufferSize);
        if (napi_get_value_string_utf8(env, src, buffer.data(), bufferSize + 1, &bufferSize) != napi_ok) {
            HILOG_ERROR("textencoder::can not get src value");
            return u"";
        }
        std::u16string u16Str = Utf8ToUtf16BE(buffer);
        return u16Str;
    }

    void OtherEncodeUtf8(TextEcodeInfo encodeInfo, char* writeResult, int32_t* written, size_t length, int32_t* nchar)
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
        *nchar = static_cast<int32_t>(i);
        *written = static_cast<int32_t>(writeLength);
    }

    void EncodeTo16BE(TextEcodeInfo encodeInfo, char* writeResult, int32_t* written, size_t length, int32_t* nchars)
    {
        size_t inputSize = 0;
        napi_get_value_string_utf16(encodeInfo.env, encodeInfo.src, nullptr, 0, &inputSize);
        char16_t originalBuffer[inputSize + 1];
        napi_get_value_string_utf16(encodeInfo.env, encodeInfo.src, originalBuffer, inputSize + 1, &inputSize);

        size_t writableSize = length;
        std::u16string bufferResult = u"";
        size_t i = 0;
        for (; i < inputSize; i++) {
            std::string strBuff = "";
            std::u16string buffer = u"";
            strBuff = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.to_bytes(originalBuffer[i]);
            if (encodeInfo.encoding == "utf-16be") {
                buffer = Utf8ToUtf16BE(strBuff);
            } else {
                std::u16string u16Str = Utf8ToUtf16BE(strBuff);
                buffer = Utf16BEToLE(u16Str);
            }
            size_t bufferLength = buffer.length() * 2; // 2:multiple
            if (bufferLength > writableSize) {
                break;
            }
            bufferResult += buffer;
            writableSize -= bufferLength;
        }

        size_t writeLength = bufferResult.length() * 2; // 2:multiple
        if (memcpy_s(writeResult, writeLength, reinterpret_cast<char*>(bufferResult.data()), writeLength) != EOK) {
            HILOG_FATAL("textencoder::copy buffer to arraybuffer error");
            return;
        }
        *nchars = static_cast<int32_t>(i);
        *written = static_cast<int32_t>(writeLength);
    }
} // namespace Commonlibrary::Platform