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

#ifndef UTIL_JS_TEXTDECODER_H
#define UTIL_JS_TEXTDECODER_H

#include <memory.h>
#include <string>
#include <vector>
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "unicode/ucnv.h"

using TransformToolPointer = std::unique_ptr<UConverter, void(*)(UConverter*)>;
namespace OHOS::Util {
    struct DecodeArr {
        DecodeArr(UChar *tarPos, size_t tarStaPos, size_t limLen)
        {
            this->target = tarPos;
            this->tarStartPos = tarStaPos;
            this->limitLen = limLen;
        }
        UChar *target = 0;
        size_t tarStartPos = 0;
        size_t limitLen = 0;
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
        /**
         * Constructor of textdecoder
         *
         * @param buff Encoding format.
         * @param optionVec There are two attributes of code related option parameters: fatal and ignorebom.
         */
        TextDecoder(const std::string &buff, int32_t flags);

        /**
         * Destructor of textencoder.
         */
        virtual ~TextDecoder() {}

        /**
         * Destructor of textencoder.
         *
         * @param env NAPI environment parameters.
         * @param src An array that matches the format and needs to be decoded.
         * @param iflag Decoding related option parameters.
         */
        napi_value Decode(napi_env env, napi_value src, bool iflag);

        napi_value DecodeToString(napi_env env, napi_value src, bool iflag);

        /**
         * Gets the size of minimum byte.
         */
        size_t GetMinByteSize() const;

        /**
         * Reset function.
         */
        void Reset() const;

        /**
         * Gets the pointer to the converter.
         */
        UConverter *GetConverterPtr() const
        {
            return tranTool_.get();
        }

        /**
         * Determine whether it is the flag of BOM.
         */
        bool IsBomFlag() const
        {
            int32_t temp = label_ & static_cast<int32_t>(ConverterFlags::BOM_SEEN_FLG);
            return temp == static_cast<int32_t>(ConverterFlags::BOM_SEEN_FLG);
        }

        /**
         * Determine whether it is Unicode.
         */
        bool IsUnicode() const
        {
            int32_t temp = label_ & static_cast<int32_t>(ConverterFlags::UNICODE_FLG);
            return temp == static_cast<int32_t>(ConverterFlags::UNICODE_FLG);
        }

        /**
         * Determine whether it is an ignored BOM.
         */
        bool IsIgnoreBom() const
        {
            int32_t temp = label_ & static_cast<int32_t>(ConverterFlags::IGNORE_BOM_FLG);
            return temp == static_cast<int32_t>(ConverterFlags::IGNORE_BOM_FLG);
        }

        /**
         * Close the pointer of converter.
         */
        static void ConverterClose(UConverter *pointer)
        {
            ucnv_close(pointer);
        }

    private:
        static constexpr uint32_t TEMP_CHAR_LENGTH = 128;
        static bool IsASCIICharacter(uint16_t data)
        {
            return data > 0 && data <= 0x7F;
        }
        static bool CanBeCompressed(const uint16_t *utf16Data, uint32_t utf16Len);
        std::pair<char *, bool> ConvertToChar(UChar* uchar, size_t length, char* tempCharArray);
        napi_value GetResultStr(napi_env env, UChar *arrDat, size_t length);
        void SetBomFlag(const UChar *arr, const UErrorCode codeFlag, const DecodeArr decArr,
                        size_t& rstLen, bool& bomFlag);
        void SetIgnoreBOM(const UChar *arr, size_t resultLen, bool& bomFlag);
        void FreedMemory(UChar *&pData);
        const char* ReplaceNull(void *data, size_t length) const;
        napi_value ThrowError(napi_env env, const char* errMessage);
        int32_t label_ {};
        std::string encStr_ {};
        TransformToolPointer tranTool_;
    };
}
#endif // UTIL_JS_TEXTDECODER_H
