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
#include <algorithm>
#include <codecvt>

#include <locale>
#include <map>
#include <string>
#include <vector>

#include "ohos/init_data.h"
#include "securec.h"
#include "unicode/unistr.h"
#include "util_helper.h"
#include "tools/log.h"

namespace OHOS::Util {
    using namespace Commonlibrary::Platform;

    TextDecoder::TextDecoder(const std::string &buff, std::vector<int> optionVec)
        : label_(0), encStr_(buff), tranTool_(nullptr, nullptr)
    {
        uint32_t i32Flag = 0;
        if (optionVec.size() == 2) { // 2:Meaning of optionVec size 2
            if (optionVec[0] >= 0 && optionVec[1] >= 0) {
                i32Flag |= optionVec[0] ? static_cast<uint32_t>(ConverterFlags::FATAL_FLG) : 0;
                i32Flag |= optionVec[1] ? static_cast<uint32_t>(ConverterFlags::IGNORE_BOM_FLG) : 0;
            } else if (optionVec[0] >= 0 && optionVec[1] < 0) {
                i32Flag |= optionVec[0] ? static_cast<uint32_t>(ConverterFlags::FATAL_FLG) : 0;
            } else if (optionVec[0] < 0 && optionVec[1] >= 0) {
                i32Flag |= optionVec[1] ? static_cast<uint32_t>(ConverterFlags::IGNORE_BOM_FLG) : 0;
            }
        }
        label_ = i32Flag;
#if !defined(__ARKUI_CROSS__)
        SetHwIcuDirectory();
#endif
        bool fatal = (i32Flag & static_cast<uint32_t>(ConverterFlags::FATAL_FLG)) ==
             static_cast<uint32_t>(ConverterFlags::FATAL_FLG);
        UErrorCode codeflag = U_ZERO_ERROR;
        UConverter *conv = CreateConverter(encStr_, codeflag);

        if (U_FAILURE(codeflag)) {
            HILOG_ERROR("ucnv_open failed !");
            return;
        }
        if (fatal) {
            codeflag = U_ZERO_ERROR;
            ucnv_setToUCallBack(conv, UCNV_TO_U_CALLBACK_STOP, nullptr, nullptr, nullptr, &codeflag);
        }
        TransformToolPointer tempTranTool(conv, ConverterClose);
        tranTool_ = std::move(tempTranTool);
    }

    napi_value TextDecoder::Decode(napi_env env, napi_value src, bool iflag)
    {
        uint8_t flags = 0;
        flags |= (iflag ? 0 : static_cast<uint8_t>(ConverterFlags::FLUSH_FLG));
        UBool flush = ((flags & static_cast<uint8_t>(ConverterFlags::FLUSH_FLG))) ==
        static_cast<uint8_t>(ConverterFlags::FLUSH_FLG);
        napi_typedarray_type type;
        size_t length = 0;
        void *data1 = nullptr;
        size_t byteOffset = 0;
        napi_value arrayBuffer = nullptr;
        NAPI_CALL(env, napi_get_typedarray_info(env, src, &type, &length, &data1, &arrayBuffer, &byteOffset));
        const char *source = static_cast<char*>(data1);
        size_t limit = GetMinByteSize() * length;
        size_t len = limit * sizeof(UChar);
        UChar *arr = nullptr;
        if (limit > 0) {
            arr = new UChar[limit + 1];
            if (memset_s(arr, len + sizeof(UChar), 0, len + sizeof(UChar)) != EOK) {
                HILOG_ERROR("decode arr memset_s failed");
                FreedMemory(arr);
                return nullptr;
            }
        } else {
            HILOG_DEBUG("limit is error");
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
            label_ &= static_cast<uint32_t>(ConverterFlags::BOM_SEEN_FLG);
            Reset();
        }
        return resultStr;
    }

    napi_value TextDecoder::GetEncoding(napi_env env) const
    {
        size_t length = strlen(encStr_.c_str());
        napi_value result = nullptr;
        NAPI_CALL(env, napi_create_string_utf8(env, encStr_.c_str(), length, &result));
        return result;
    }

    napi_value TextDecoder::GetFatal(napi_env env) const
    {
        uint32_t temp = label_ & static_cast<uint32_t>(ConverterFlags::FATAL_FLG);
        bool comRst = false;
        if (temp == static_cast<uint32_t>(ConverterFlags::FATAL_FLG)) {
            comRst = true;
        } else {
            comRst = false;
        }
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_boolean(env, comRst, &result));
        return result;
    }

    napi_value TextDecoder::GetIgnoreBOM(napi_env env) const
    {
        uint32_t temp = label_ & static_cast<uint32_t>(ConverterFlags::IGNORE_BOM_FLG);
        bool comRst = false;
        if (temp == static_cast<uint32_t>(ConverterFlags::IGNORE_BOM_FLG)) {
            comRst = true;
        } else {
            comRst = false;
        }
        napi_value result;
        NAPI_CALL(env, napi_get_boolean(env, comRst, &result));
        return result;
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

    void TextDecoder::FreedMemory(UChar *pData)
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
                    label_ |= static_cast<uint32_t>(ConverterFlags::BOM_SEEN_FLG);
                }
            }
        }
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
}
