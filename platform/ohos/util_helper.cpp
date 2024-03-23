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