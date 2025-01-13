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

#include "js_textdecoder.h"

#include "ohos/init_data.h"
#include "securec.h"
#include "util_helper.h"

namespace OHOS::Util {
    using namespace Commonlibrary::Platform;

    TextDecoder::TextDecoder(const std::string &buff, int32_t flags)
        : encStr_(buff), tranTool_(nullptr, nullptr)
    {
        label_ |= flags;
#if !defined(__ARKUI_CROSS__)
        SetHwIcuDirectory();
#endif
        bool fatal = (flags & static_cast<int32_t>(ConverterFlags::FATAL_FLG)) ==
             static_cast<int32_t>(ConverterFlags::FATAL_FLG);
        UErrorCode codeflag = U_ZERO_ERROR;
        UConverter *conv = CreateConverter(encStr_, codeflag);
        if (U_FAILURE(codeflag)) {
            HILOG_ERROR("TextDecoder:: ucnv_open failed !");
            return;
        }
        if (fatal) {
            codeflag = U_ZERO_ERROR;
            ucnv_setToUCallBack(conv, UCNV_TO_U_CALLBACK_STOP, nullptr, nullptr, nullptr, &codeflag);
        }
        TransformToolPointer tempTranTool(conv, ConverterClose);
        tranTool_ = std::move(tempTranTool);
    }
    
    //static
    bool TextDecoder::CanBeCompressed(const uint16_t *utf16Data, uint32_t utf16Len)
    {
        uint32_t index = 0;
        for (; index + 4 <= utf16Len; index += 4) { // 4: process the data in chunks of 4 elements to improve speed
            // Check if all four characters in the current block are ASCII characters
            if (!IsASCIICharacter(utf16Data[index]) ||
                !IsASCIICharacter(utf16Data[index + 1]) || // 1: the second element of the block
                !IsASCIICharacter(utf16Data[index + 2]) || // 2: the third element of the block
                !IsASCIICharacter(utf16Data[index + 3])) { // 3: the fourth element of the block
                return false;
            }
        }
        // Check remaining characters if they are ASCII
        for (; index < utf16Len; ++index) {
            if (!IsASCIICharacter(utf16Data[index])) {
                return false;
            }
        }
        return true;
    }

    std::pair<char *, bool> TextDecoder::ConvertToChar(UChar *uchar, size_t length, char *tempCharArray)
    {
        uint16_t *uint16Data = reinterpret_cast<uint16_t *>(uchar);
        if (CanBeCompressed(uint16Data, length)) {
            if (length <= 0) {
                HILOG_ERROR("TextDecoder:: length is error");
                return std::make_pair(nullptr, false);
            }
            char *strUtf8;
            if (length <= TEMP_CHAR_LENGTH) {
                strUtf8 = tempCharArray;
            } else {
                strUtf8 = new (std::nothrow) char[length];
                if (strUtf8 == nullptr) {
                    HILOG_ERROR("TextDecoder:: memory allocation failed, strUtf8 is nullptr");
                    return std::make_pair(nullptr, false);
                }
            }
            for (size_t i = 0; i < length; ++i) {
                strUtf8[i] = static_cast<char>(uchar[i]);
            }
            return std::make_pair(strUtf8, true);
        }
        return std::make_pair(nullptr, false);
    }

    napi_value TextDecoder::Decode(napi_env env, napi_value src, bool iflag)
    {
        uint8_t flags = 0;
        flags |= (iflag ? 0 : static_cast<uint8_t>(ConverterFlags::FLUSH_FLG));
        UBool flush = ((flags & static_cast<uint8_t>(ConverterFlags::FLUSH_FLG))) ==
        static_cast<uint8_t>(ConverterFlags::FLUSH_FLG);
        napi_typedarray_type type;
        size_t length = 0;
        void *data = nullptr;
        size_t byteOffset = 0;
        napi_value arrayBuffer = nullptr;
        NAPI_CALL(env, napi_get_typedarray_info(env, src, &type, &length, &data, &arrayBuffer, &byteOffset));
        const char *source = ReplaceNull(data, length);
        size_t limit = GetMinByteSize() * length;
        size_t len = limit * sizeof(UChar);
        UChar *arr = nullptr;
        if (limit > 0) {
            arr = new (std::nothrow) UChar[limit + 1];
            if (arr == nullptr) {
                HILOG_ERROR("TextDecoder:: memory allocation failed, decode arr is nullptr");
                return nullptr;
            }
            if (memset_s(arr, len + sizeof(UChar), 0, len + sizeof(UChar)) != EOK) {
                HILOG_ERROR("TextDecoder:: decode arr memset_s failed");
                FreedMemory(arr);
                return nullptr;
            }
        } else {
            HILOG_DEBUG("TextDecoder:: limit is error");
            return nullptr;
        }
        UChar *target = arr;
        size_t tarStartPos = reinterpret_cast<uintptr_t>(arr);
        UErrorCode codeFlag = U_ZERO_ERROR;
        ucnv_toUnicode(GetConverterPtr(), &target, target + len, &source, source + length, nullptr, flush, &codeFlag);
        if (codeFlag != U_ZERO_ERROR) {
            return ThrowError(env, "TextDecoder decoding error.");
        }
        size_t resultLength = 0;
        bool omitInitialBom = false;
        DecodeArr decArr(target, tarStartPos, limit);
        SetBomFlag(arr, codeFlag, decArr, resultLength, omitInitialBom);
        UChar *arrDat = arr;
        if (omitInitialBom && resultLength > 0) {
            arrDat = &arr[2]; // 2: Obtains the 2 value of the array.
        }
        std::string tepStr = ConvertToString(arrDat, length);
        napi_value resultStr = nullptr;
        NAPI_CALL(env, napi_create_string_utf8(env, tepStr.c_str(), tepStr.size(), &resultStr));
        FreedMemory(arr);
        if (flush) {
            label_ &= static_cast<int32_t>(ConverterFlags::BOM_SEEN_FLG);
            Reset();
        }
        return resultStr;
    }

    napi_value TextDecoder::GetResultStr(napi_env env, UChar *arrDat,
                                         size_t length)
    {
        napi_value resultStr = nullptr;
        if (length <= TEMP_CHAR_LENGTH) {
            char tempCharArray[TEMP_CHAR_LENGTH];
            std::pair<char *, bool> tempPair = ConvertToChar(arrDat, length, tempCharArray);
            if (tempPair.second == true) {
                char *utf8Str = tempPair.first;
                napi_create_string_utf8(env, utf8Str, length, &resultStr);
            } else {
                napi_create_string_utf16(env, reinterpret_cast<char16_t *>(arrDat), length, &resultStr);
            }
        } else {
            std::pair<char *, bool> tempPair = ConvertToChar(arrDat, length, nullptr);
            if (tempPair.second == true) {
                char *utf8Str = tempPair.first;
                napi_create_string_utf8(env, utf8Str, length, &resultStr);
                NAPI_ASSERT(env, utf8Str != nullptr, "Data allocation failed");
                delete[] utf8Str;
            } else {
                napi_create_string_utf16(env, reinterpret_cast<char16_t *>(arrDat), length, &resultStr);
            }
        }
        return resultStr;
    }

    napi_value TextDecoder::DecodeToString(napi_env env,
                                           napi_value src, bool iflag)
    {
        uint8_t flags = 0;
        flags |= (iflag ? 0 : static_cast<uint8_t>(ConverterFlags::FLUSH_FLG));
        UBool flush = (flags & static_cast<uint8_t>(ConverterFlags::FLUSH_FLG)) ==
            static_cast<uint8_t>(ConverterFlags::FLUSH_FLG);
        napi_typedarray_type type;
        size_t length = 0;
        void *data = nullptr;
        size_t byteOffset = 0;
        napi_value arrayBuffer = nullptr;
        napi_get_typedarray_info(env, src, &type, &length, &data, &arrayBuffer, &byteOffset);
        const char *source = static_cast<char *>(data);
        size_t limit = GetMinByteSize() * length;
        size_t len = limit * sizeof(UChar);
        UChar *arr = nullptr;
        if (limit > 0) {
            arr = new (std::nothrow) UChar[limit + 1]{0};
            if (arr == nullptr) {
                HILOG_ERROR("TextDecoder:: memory allocation failed, arr is nullptr");
                return nullptr;
            }
        } else {
            HILOG_DEBUG("TextDecoder:: limit is error");
            return nullptr;
        }
        UChar *target = arr;
        UErrorCode codeFlag = U_ZERO_ERROR;
        ucnv_toUnicode(GetConverterPtr(), &target, target + len, &source, source + length, nullptr, flush, &codeFlag);
        if (codeFlag != U_ZERO_ERROR) {
            FreedMemory(arr);
            napi_throw_error(env, "401",
                "Parameter error. Please check if the decode data matches the encoding format.");
            return nullptr;
        }
        size_t resultLen = target - arr;
        bool omitInitialBom = false;
        SetIgnoreBOM(arr, resultLen, omitInitialBom);
        UChar *arrDat = arr;
        if (omitInitialBom) {
            arrDat = &arr[1];
            resultLen--;
        }
        napi_value resultStr = GetResultStr(env, arrDat, resultLen);
        FreedMemory(arr);
        if (flush) {
            label_ &= ~static_cast<int32_t>(ConverterFlags::BOM_SEEN_FLG);
            Reset();
        }
        return resultStr;
    }

    size_t TextDecoder::GetMinByteSize() const
    {
        if (tranTool_ == nullptr) {
            return 0;
        }
        size_t res = static_cast<size_t>(ucnv_getMinCharSize(tranTool_.get()));
        return res;
    }

    void TextDecoder::Reset() const
    {
        if (tranTool_ == nullptr) {
            return;
        }
        ucnv_reset(tranTool_.get());
    }

    void TextDecoder::FreedMemory(UChar *&pData)
    {
        if (pData != nullptr) {
            delete[] pData;
            pData = nullptr;
        }
    }

    void TextDecoder::SetBomFlag(const UChar *arr, const UErrorCode codeFlag, const DecodeArr decArr,
                                 size_t &rstLen, bool &bomFlag)
    {
        if (arr == nullptr) {
            return;
        }
        if (U_SUCCESS(codeFlag)) {
            if (decArr.limitLen > 0) {
                rstLen = reinterpret_cast<uintptr_t>(decArr.target) - decArr.tarStartPos;
                if (rstLen > 0 && IsUnicode() && !IsIgnoreBom() && !IsBomFlag()) {
                    bomFlag = (arr[0] == 0xFEFF) ? true : false;
                    label_ |= static_cast<int32_t>(ConverterFlags::BOM_SEEN_FLG);
                }
            }
        }
    }

    void TextDecoder::SetIgnoreBOM(const UChar *arr, size_t resultLen, bool &bomFlag)
    {
        switch (ucnv_getType(GetConverterPtr())) {
            case UCNV_UTF8:
            case UCNV_UTF16_BigEndian:
            case UCNV_UTF16_LittleEndian:
                label_ |= static_cast<int32_t>(ConverterFlags::UNICODE_FLG);
                break;
            default:
                break;
        }
        if (resultLen > 0 && IsUnicode() && IsIgnoreBom()) {
            bomFlag = (arr[0] == 0xFEFF) ? true : false;
        }
        label_ |= static_cast<int32_t>(ConverterFlags::BOM_SEEN_FLG);
    }

    napi_value TextDecoder::ThrowError(napi_env env, const char* errMessage)
    {
        napi_value utilError = nullptr;
        napi_value code = nullptr;
        uint32_t errCode = 10200019;
        napi_create_uint32(env, errCode, &code);
        napi_value name = nullptr;
        std::string errName = "BusinessError";
        napi_value msg = nullptr;
        napi_create_string_utf8(env, errMessage, NAPI_AUTO_LENGTH, &msg);
        napi_create_string_utf8(env, errName.c_str(), NAPI_AUTO_LENGTH, &name);
        napi_create_error(env, nullptr, msg, &utilError);
        napi_set_named_property(env, utilError, "code", code);
        napi_set_named_property(env, utilError, "name", name);
        napi_throw(env, utilError);
        napi_value res = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &res));
        return res;
    }

    const char* TextDecoder::ReplaceNull(void *data, size_t length) const
    {
        char *str = static_cast<char*>(data);
        if (encStr_ == "utf-8") {
            for (size_t i = 0; i < length; ++i) {
                if (str[i] == '\0') {
                    str[i] = ' ';
                }
            }
        }
        return const_cast<const char*>(str);
    }
}
