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
            HILOG_ERROR("TextEncoder:: ucnv_open failed !");
            return "";
        }

        size_t maxByteSize = static_cast<size_t>(ucnv_getMaxCharSize(converter));
        const UChar *source = originalBuffer;
        size_t limit = maxByteSize * inputSize;
        size_t len = limit * sizeof(char);
        char *targetArray = nullptr;
        if (limit > 0) {
            targetArray = new (std::nothrow) char[limit + 1];
            if (targetArray == nullptr) {
                HILOG_ERROR("TextEncoder:: UnicodeConversion memory allocation failed, targetArray is nullptr");
                ucnv_close(converter);
                return "";
            }
            if (memset_s(targetArray, len + sizeof(char), 0, len + sizeof(char)) != EOK) {
                HILOG_ERROR("TextEncoder:: encode targetArray memset_s failed");
                ucnv_close(converter);
                FreedMemory(targetArray);
                return "";
            }
        } else {
            HILOG_ERROR("TextEncoder:: limit is error");
            ucnv_close(converter);
            return "";
        }

        char *target = targetArray;
        const char *targetLimit = targetArray + limit;
        const UChar *sourceLimit = source + u_strlen(source);
        if (sourceLimit == nullptr) {
            HILOG_ERROR("TextEncoder:: sourceLimit is nullptr");
            return "";
        }
        ucnv_fromUnicode(converter, &target, targetLimit, &source, sourceLimit, nullptr, true, &codeflag);
        if (U_FAILURE(codeflag)) {
            HILOG_ERROR("TextEncoder:: ucnv_fromUnicode conversion failed.");
            ucnv_close(converter);
            FreedMemory(targetArray);
            return "";
        }

        buffer = targetArray;
        ucnv_close(converter);
        FreedMemory(targetArray);
        return buffer;
    }

    void EncodeToUtf8(TextEcodeInfo encodeInfo, char* writeResult, uint32_t* written, size_t length, int32_t* nchars)
    {
        if (encodeInfo.encoding == "utf-16be" || encodeInfo.encoding == "utf-16le") {
            EncodeTo16BE(encodeInfo, writeResult, written, length, nchars);
        } else {
            OtherEncodeUtf8(encodeInfo, writeResult, written, length, nchars);
        }
    }

    void EncodeConversion(napi_env env, napi_value src, napi_value* arrayBuffer, size_t &outLens, std::string encoding)
    {
        if (encoding == "utf-16le") {
            size_t  outLen = 0;
            void *data = nullptr;
            std::u16string u16Str = EncodeUtf16BE(env, src);
            outLen = u16Str.length() * 2; // 2:multiple
            outLens = outLen;
            napi_create_arraybuffer(env, outLen, &data, arrayBuffer);
            if (memcpy_s(data, outLen, reinterpret_cast<void*>(u16Str.data()), outLen) != EOK) {
                HILOG_FATAL("TextEncoder:: copy buffer to arraybuffer error");
                return;
            }
        } else if (encoding == "utf-16be") {
            size_t  outLen = 0;
            void *data = nullptr;
            std::u16string u16BEStr = EncodeUtf16BE(env, src);
            std::u16string u16LEStr = Utf16BEToLE(u16BEStr);
            outLen = u16LEStr.length() * 2; // 2:multiple
            outLens = outLen;
            napi_create_arraybuffer(env, outLen, &data, arrayBuffer);
            if (memcpy_s(data, outLen, reinterpret_cast<void*>(u16LEStr.data()), outLen) != EOK) {
                HILOG_FATAL("TextEncoder:: copy buffer to arraybuffer error");
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
            HILOG_ERROR("TextEncoder:: ucnv_open failed !");
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
        char16_t *originalBuffer = ApplyMemory(inputSize);
        if (originalBuffer == nullptr) {
            HILOG_ERROR("TextEncoder:: originalBuffer is nullptr");
            return;
        }
        napi_get_value_string_utf16(env, src, originalBuffer, inputSize + 1, &inputSize);
        outLen = static_cast<size_t>(GetMaxByteSize(encoding)) * inputSize;
        napi_create_arraybuffer(env, outLen, &data, arrayBuffer);
        char *writeResult = static_cast<char*>(data);
        if (writeResult == nullptr) {
            FreedMemory(originalBuffer);
            HILOG_ERROR("TextEncoder:: writeResult is nullptr");
            return;
        }
        std::string buffer = "";
        std::u16string originalStr(originalBuffer, inputSize);
        size_t shifting = 0;
        size_t resultShifting = 0;
        size_t findIndex = originalStr.find('\0');
        if (findIndex == std::string::npos) {
            buffer = UnicodeConversion(encoding, originalBuffer, inputSize);
            outLens = buffer.length();
            if (memcpy_s(writeResult, outLens, reinterpret_cast<char*>(buffer.data()), outLens) != EOK) {
                FreedMemory(originalBuffer);
                HILOG_FATAL("TextEncoder:: copy buffer to arraybuffer error");
                return;
            }
        } else {
            while (findIndex != std::string::npos) {
                buffer = UnicodeConversion(encoding, originalBuffer + shifting, inputSize);
                if (memcpy_s(writeResult + resultShifting, buffer.length(),
                             reinterpret_cast<char*>(buffer.data()), buffer.length()) != EOK) {
                    FreedMemory(originalBuffer);
                    HILOG_FATAL("TextEncoder:: copy buffer to arraybuffer error");
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
                FreedMemory(originalBuffer);
                HILOG_FATAL("TextEncoder:: copy buffer to arraybuffer error");
                return;
            }
        }
        FreedMemory(originalBuffer);
    }

    std::u16string EncodeUtf16BE(napi_env env, napi_value src)
    {
        std::string buffer = "";
        size_t bufferSize = 0;

        if (napi_get_value_string_utf8(env, src, nullptr, 0, &bufferSize) != napi_ok) {
            HILOG_ERROR("TextEncoder:: can not get src size");
            return u"";
        }
        buffer.reserve(bufferSize + 1);
        buffer.resize(bufferSize);
        if (napi_get_value_string_utf8(env, src, buffer.data(), bufferSize + 1, &bufferSize) != napi_ok) {
            HILOG_ERROR("TextEncoder:: can not get src value");
            return u"";
        }
        std::u16string u16Str = Utf8ToUtf16BE(buffer);
        return u16Str;
    }

    bool IsValidLowSurrogate(char16_t high)
    {
        // 0xD800: minimum value of low proxy term. 0xDBFF: Maximum value of low proxy term.
        return (high >= 0xD800 && high <= 0xDBFF);
    }

    bool IsValidHighSurrogate(char16_t low)
    {
        // 0xDC00: minimum value of high proxy item. 0xDFFF: maximum value of high proxy item.
        return (low >= 0xDC00 && low <= 0xDFFF);
    }

    uint32_t OtherEncodeUtf8Inner(char16_t *originalBuffer, InputBufferInfo inputInfo, size_t &index,
        OutBufferInfo &outInfo)
    {
        if (IsValidLowSurrogate(originalBuffer[index]) && inputInfo.encoding == "utf-8") {
            size_t tempIndex = index + 1;
            if (tempIndex < inputInfo.inputSize && IsValidHighSurrogate(originalBuffer[index + 1])) {
                // 2: move the pointer forward to the position of two elements.
                std::u16string utf16String(&originalBuffer[index], &originalBuffer[index] + 2);
                std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
                outInfo.rstBuffer = converter.to_bytes(utf16String);
                outInfo.rstBufferLength = outInfo.rstBuffer.length();
                if (outInfo.rstBufferLength > outInfo.writedSize) {
                    return STATE_BREAK_ZERO;
                }
                index++;
                outInfo.cntSize += 2; // 2: two bytes
                outInfo.bufferResult += outInfo.rstBuffer;
                outInfo.writedSize -= outInfo.rstBufferLength;
                return STATE_CONTINUE_ONE;
            }
        }
        return STATE_OTHER_TWO;
    }

    void OtherEncodeUtf8(TextEcodeInfo encodeInfo, char* writeResult, uint32_t* written, size_t length, int32_t* nchar)
    {
        size_t inputSize = 0;
        napi_get_value_string_utf16(encodeInfo.env, encodeInfo.src, nullptr, 0, &inputSize);
        char16_t *originalBuffer = ApplyMemory(inputSize);
        if (originalBuffer == nullptr) {
            HILOG_ERROR("TextEncoder:: originalBuffer is nullptr");
            return;
        }
        napi_get_value_string_utf16(encodeInfo.env, encodeInfo.src, originalBuffer, inputSize + 1, &inputSize);
        std::vector<char16_t> targetBuffer(inputSize + 1, u'\0');
        InputBufferInfo inputInfo(encodeInfo.encoding, inputSize);
        OutBufferInfo outInfo(length, "", 0, 0, "");
        for (size_t i = 0; i < inputSize; i++) {
            targetBuffer[i] = originalBuffer[i];
            uint32_t rstState = OtherEncodeUtf8Inner(originalBuffer, inputInfo, i, outInfo);
            if (rstState == STATE_BREAK_ZERO) {
                break;
            } else if (rstState == STATE_CONTINUE_ONE) {
                continue;
            }
            outInfo.rstBuffer = UnicodeConversion(encodeInfo.encoding, &targetBuffer[i], inputSize);
            outInfo.rstBufferLength = outInfo.rstBuffer.length();
            if (outInfo.rstBufferLength > outInfo.writedSize) {
                break;
            }
            outInfo.cntSize++;
            outInfo.bufferResult += outInfo.rstBuffer;
            outInfo.writedSize -= outInfo.rstBufferLength;
        }
        size_t writeLength = outInfo.bufferResult.length();
        for (size_t j = 0; j < writeLength; j++) {
            *writeResult = outInfo.bufferResult[j];
            writeResult++;
        }
        *nchar = static_cast<int32_t>(outInfo.cntSize);
        *written = static_cast<uint32_t>(writeLength);
        FreedMemory(originalBuffer);
    }

    void EncodeTo16BE(TextEcodeInfo encodeInfo, char* writeResult, uint32_t* written, size_t length, int32_t* nchars)
    {
        size_t inputSize = 0;
        napi_get_value_string_utf16(encodeInfo.env, encodeInfo.src, nullptr, 0, &inputSize);
        char16_t *originalBuffer = ApplyMemory(inputSize);
        if (originalBuffer == nullptr) {
            HILOG_ERROR("TextEncoder:: originalBuffer is nullptr");
            return;
        }
        napi_get_value_string_utf16(encodeInfo.env, encodeInfo.src, originalBuffer, inputSize + 1, &inputSize);

        size_t writableSize = length;
        std::u16string bufferResult = u"";
        size_t i = 0;
        for (; i < inputSize; i++) {
            std::string strBuff = "";
            std::u16string buffer = u"";
            strBuff = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.to_bytes(originalBuffer[i]);
            if (encodeInfo.encoding == "utf-16le") {
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
            FreedMemory(originalBuffer);
            HILOG_FATAL("TextEncoder:: copy buffer to arraybuffer error");
            return;
        }
        *nchars = static_cast<int32_t>(i);
        *written = static_cast<uint32_t>(writeLength);
        FreedMemory(originalBuffer);
    }

    char16_t *ApplyMemory(const size_t &inputSize)
    {
        char16_t *originalBuffer = nullptr;
        if (inputSize > 0) {
            originalBuffer = new (std::nothrow) char16_t[inputSize + 1];
            if (originalBuffer == nullptr) {
                HILOG_ERROR("TextEncoder:: originalBuffer memory allocation failed, originalBuffer is nullptr");
                return nullptr;
            }
            if (memset_s(originalBuffer, inputSize + 1, u'\0', inputSize + 1) != EOK) {
                HILOG_ERROR("encode originalBuffer memset_s failed");
                FreedMemory(originalBuffer);
                return nullptr;
            }
        } else {
            HILOG_ERROR("inputSize is error");
            return nullptr;
        }
        return originalBuffer;
    }

    void FreedMemory(char16_t *&data)
    {
        if (data != nullptr) {
            delete[] data;
            data = nullptr;
        }
    }
} // namespace Commonlibrary::Platform