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

#ifndef UTIL_JS_TEXTENCODER_H
#define UTIL_JS_TEXTENCODER_H

#include <string>

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "native_engine.h"
#include "unicode/ucnv.h"

namespace OHOS::Util {
    constexpr const int32_t API_VERSION_MOD = 100; // 100: api version mod
    constexpr const int32_t API13 = 13; // 13: api version
    class TextEncoder {
    public:
        /**
         * Constructor of textdecoder.
         *
         * @param encoding Encoding format
         */
        explicit TextEncoder(const std::string &encoding) : encoding_(encoding) {}

        /**
         * Destructor of textencoder.
         */
        virtual ~TextEncoder() {}

        /**
         * Get encoding format.
         *
         * @param env NAPI environment parameters.
         */
        napi_value GetEncoding(napi_env env) const;

        /**
         * Output the corresponding text after encoding the input parameters.
         *
         * @param env NAPI environment parameters.
         * @param src A string that needs to be encoded.
         */
        napi_value Encode(napi_env env, napi_value src) const;

        /**
         * Place the generated UTF-8 encoded text.
         *
         * @param env NAPI environment parameters.
         * @param src A string that needs to be encoded.
         * @param dest Uint8array object instance, which is used to put the generated UTF-8 encoded text into it.
         */
        napi_value EncodeInto(napi_env env, napi_value src, napi_value dest) const;

        void SetOrgEncoding(std::string orgEncoding)
        {
            orgEncoding_ = orgEncoding;
        }

        void SetApiIsolated(napi_env env)
        {
            if (encoding_ != "utf-16be" && encoding_ != "utf-16le") {
                return;
            }
            NativeEngine* engine = reinterpret_cast<NativeEngine*>(env);
            if (engine != nullptr) {
                int32_t currentApi = engine->GetApiVersion() % API_VERSION_MOD;
                if (currentApi >= API13) {
                    encoding_ = encoding_ == "utf-16be" ? "utf-16le" : "utf-16be";
                }
            }
        }
    private:
        std::string encoding_ {};
        std::string orgEncoding_ {};
    };
}
#endif // UTIL_JS_TEXTENCODER_H
