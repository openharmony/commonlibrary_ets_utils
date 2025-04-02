/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <iostream>
#include "ani_textdecoder.h"
#include "securec.h"
#include "ohos/init_data.h"

namespace OHOS::ETSUtil {
UConverter* CreateConverter(const std::string& encStr_, UErrorCode& codeflag)
{
    UConverter *conv = ucnv_open(encStr_.c_str(), &codeflag);
    if (U_FAILURE(codeflag)) {
        HILOG_ERROR("TextDecoder:: Unable to create a UConverter object %{public}s", u_errorName(codeflag));
        return nullptr;
    }
    ucnv_setFromUCallBack(conv, UCNV_FROM_U_CALLBACK_SUBSTITUTE, NULL, NULL, NULL, &codeflag);
    if (U_FAILURE(codeflag)) {
        HILOG_ERROR("TextDecoder:: Unable to set the from Unicode callback function");
        ucnv_close(conv);
        return nullptr;
    }

    ucnv_setToUCallBack(conv, UCNV_TO_U_CALLBACK_SUBSTITUTE, NULL, NULL, NULL, &codeflag);
    if (U_FAILURE(codeflag)) {
        HILOG_ERROR("TextDecoder:: Unable to set the to Unicode callback function");
        ucnv_close(conv);
        return nullptr;
    }
    return conv;
}

TextDecoder::TextDecoder(const std::string &buff, int32_t flags)
    : encStr_(buff), tranTool_(nullptr, nullptr)
{
    SetHwIcuDirectory();
    label_ |= flags;
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

ani_string TextDecoder::GetResultStr(ani_env *env, UChar *arrDat, size_t length)
{
    ani_string resultStr {};
    if (length <= TEMP_CHAR_LENGTH) {
        char tempCharArray[TEMP_CHAR_LENGTH];
        std::pair<char *, bool> tempPair = ConvertToChar(arrDat, length, tempCharArray);
        if (tempPair.second == true) {
            char *utf8Str = tempPair.first;
            env->String_NewUTF8(utf8Str, length, &resultStr);
        } else {
            env->String_NewUTF16(reinterpret_cast<const uint16_t*>(arrDat), length, &resultStr);
        }
    } else {
        std::pair<char *, bool> tempPair = ConvertToChar(arrDat, length, nullptr);
        if (tempPair.second == true) {
            char *utf8Str = tempPair.first;
            env->String_NewUTF8(utf8Str, length, &resultStr);
            if (utf8Str == nullptr) {
                HILOG_ERROR("TextDecoder:: Data allocation failed");
            }
            delete[] utf8Str;
        } else {
            env->String_NewUTF16(reinterpret_cast<const uint16_t*>(arrDat), length, &resultStr);
        }
    }
    return resultStr;
}

ani_object TextDecoder::ThrowError(ani_env *env, std::string message)
{
    ani_string errString;
    env->String_NewUTF8(message.c_str(), message.size(), &errString);
    static const char *className = "L@ohos/util/util/BusinessError;";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR("TextDecoder:: Not found %{public}s", className);
        return nullptr;
    }

    ani_method errorCtor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", "Lstd/core/String;:V", &errorCtor)) {
        HILOG_ERROR("TextDecoder:: Class_FindMethod <ctor> Failed");
        return nullptr;
    }

    ani_object errorObj;
    if (ANI_OK != env->Object_New(cls, errorCtor, &errorObj, errString)) {
        HILOG_ERROR("TextDecoder:: Object_New Array Faild");
    }
    return errorObj;
}

ani_string TextDecoder::DecodeToString(ani_env *env, void *data, int32_t byteOffset, int32_t length, bool iflag)
{
    uint8_t flags = 0;
    flags |= (iflag ? 0 : static_cast<uint8_t>(ConverterFlags::FLUSH_FLG));
    UBool flush = (flags & static_cast<uint8_t>(ConverterFlags::FLUSH_FLG)) ==
        static_cast<uint8_t>(ConverterFlags::FLUSH_FLG);
    const char *source = static_cast<char *>(data) + byteOffset;
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
        HILOG_ERROR("TextDecoder:: limit is error");
        return nullptr;
    }
    UChar *target = arr;
    UErrorCode codeFlag = U_ZERO_ERROR;
    ucnv_toUnicode(GetConverterPtr(), &target, target + len, &source, source + length, nullptr, flush, &codeFlag);
    if (codeFlag != U_ZERO_ERROR) {
        FreedMemory(arr);
        std::string message = "Parameter error. Please check if the decode data matches the encoding format.";
        env->ThrowError(static_cast<ani_error>(ThrowError(env, message)));
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
    ani_string resultStr = GetResultStr(env, arrDat, resultLen);
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
}
