/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_CCRUNTIME_TEXTCODER_JS_TEXTDECODER_H
#define FOUNDATION_CCRUNTIME_TEXTCODER_JS_TEXTDECODER_H

#include <memory.h>
#include <string>
#include <vector>
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "unicode/ucnv.h"

using TransformToolPointer = std::unique_ptr<UConverter, void(*)(UConverter*)>;
namespace OHOS::Util {
    struct DecodeArr {
        DecodeArr(UChar *tarPos, size_t tarStaPos, int32_t limLen)
        {
            this->target = tarPos;
            this->tarStartPos = tarStaPos;
            this->limitLen = limLen;
        }
        UChar *target = 0;
        size_t tarStartPos = 0;
        int32_t limitLen = 0;
    };

    class TextDecoder {
    public:
        enum class ConverterFlags {
            FLUSH_FLG = 0x1,
            FATAL_FLG = 0x2,
            IGNORE_BOM_FLG = 0x4,
            UNICODE_FLG = 0x8,
            BOM_SEEN_FLG = 0x10,
        };
    public:
        TextDecoder(napi_env env, std::string buff, std::vector<int> optionVec);
        virtual ~TextDecoder() {}
        napi_value Decode(napi_value src, bool iflag);
        napi_value GetEncoding() const;
        napi_value GetFatal() const;
        napi_value GetIgnoreBOM() const;
        int32_t GetMinByteSize() const;
        void Reset() const;
        UConverter *GetConverterPtr() const
        {
            return tranTool_.get();
        }
        bool IsBomFlag() const
        {
            uint32_t temp = label_ & static_cast<uint32_t>(ConverterFlags::BOM_SEEN_FLG);
            if (temp == static_cast<uint32_t>(ConverterFlags::BOM_SEEN_FLG)) {
                return true;
            } else {
                return false;
            }
        }
        bool IsUnicode() const
        {
            uint32_t temp = label_ & static_cast<uint32_t>(ConverterFlags::UNICODE_FLG);
            if (temp == static_cast<uint32_t>(ConverterFlags::UNICODE_FLG)) {
                return true;
            } else {
                return false;
            }
        }
        bool IsIgnoreBom() const
        {
            uint32_t temp = label_ & static_cast<uint32_t>(ConverterFlags::IGNORE_BOM_FLG);
            if (temp == static_cast<uint32_t>(ConverterFlags::IGNORE_BOM_FLG)) {
                return true;
            } else {
                return false;
            }
        }
        static void ConverterClose(UConverter *pointer)
        {
            ucnv_close(pointer);
        }
    private:
        void SetBomFlag(const UChar *arr, const UErrorCode codeFlag, const DecodeArr decArr,
                        size_t& rstLen, bool& bomFlag);
        void FreedMemory(UChar *pData);
        napi_env env_;
        uint32_t label_;
        std::string encStr_;
        TransformToolPointer tranTool_;
    };
}
#endif /* FOUNDATION_CCRUNTIME_TEXTCODER_JS_TEXTDECODER_H */
