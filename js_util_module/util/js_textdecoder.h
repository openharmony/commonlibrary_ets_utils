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

#ifndef UTIL_JS_TEXTDECODER_H_
#define UTIL_JS_TEXTDECODER_H_

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
        TextDecoder(const std::string &buff, std::vector<int> optionVec);

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

        /**
         * Get encoding format.
         *
         * @param env NAPI environment parameters.
         */
        napi_value GetEncoding(napi_env env) const;

        /**
         * Gets the setting of the exception thrown.
         *
         * @param env NAPI environment parameters.
         */
        napi_value GetFatal(napi_env env) const;

        /**
         * Gets whether to ignore the setting of BOM flag.
         *
         * @param env NAPI environment parameters.
         */
        napi_value GetIgnoreBOM(napi_env env) const;

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
            uint32_t temp = label_ & static_cast<uint32_t>(ConverterFlags::BOM_SEEN_FLG);
            if (temp == static_cast<uint32_t>(ConverterFlags::BOM_SEEN_FLG)) {
                return true;
            } else {
                return false;
            }
        }

        /**
         * Determine whether it is Unicode.
         */
        bool IsUnicode() const
        {
            uint32_t temp = label_ & static_cast<uint32_t>(ConverterFlags::UNICODE_FLG);
            if (temp == static_cast<uint32_t>(ConverterFlags::UNICODE_FLG)) {
                return true;
            } else {
                return false;
            }
        }

        /**
         * Determine whether it is an ignored BOM.
         */
        bool IsIgnoreBom() const
        {
            uint32_t temp = label_ & static_cast<uint32_t>(ConverterFlags::IGNORE_BOM_FLG);
            if (temp == static_cast<uint32_t>(ConverterFlags::IGNORE_BOM_FLG)) {
                return true;
            } else {
                return false;
            }
        }

        /**
         * Close the pointer of converter.
         */
        static void ConverterClose(UConverter *pointer)
        {
            ucnv_close(pointer);
        }

    private:
        void SetBomFlag(const UChar *arr, const UErrorCode codeFlag, const DecodeArr decArr,
                        size_t& rstLen, bool& bomFlag);
        void FreedMemory(UChar *pData);
        uint32_t label_;
        std::string encStr_;
        TransformToolPointer tranTool_;
    };
}
#endif // UTIL_JS_TEXTDECODER_H_
