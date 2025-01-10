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

#include "js_textencoder.h"
#include "util_helper.h"

namespace OHOS::Util {
    using namespace Commonlibrary::Platform;
    napi_value TextEncoder::GetEncoding(napi_env env) const
    {
        napi_value result = nullptr;
        NAPI_CALL(env, napi_create_string_utf8(env, orgEncoding_.c_str(), orgEncoding_.length(), &result));

        return result;
    }

    napi_value TextEncoder::Encode(napi_env env, napi_value src) const
    {
        napi_value result = nullptr;
        if (encoding_ == "utf-8") {
            // optimized, fastpath for utf-8 encode
            napi_encode(env, src, &result);
        } else {
            size_t outLens = 0;
            napi_value arrayBuffer = nullptr;
            EncodeConversion(env, src, &arrayBuffer, outLens, encoding_);
            napi_create_typedarray(env, napi_uint8_array, outLens, arrayBuffer, 0, &result);
        }

        return result;
    }

    napi_value TextEncoder::EncodeInto(napi_env env, napi_value src, napi_value dest) const
    {
        napi_typedarray_type type;
        size_t byteOffset = 0;
        size_t length = 0;
        void *resultData = nullptr;
        napi_value resultBuffer = nullptr;
        NAPI_CALL(env, napi_get_typedarray_info(env, dest, &type, &length, &resultData, &resultBuffer, &byteOffset));

        char *writeResult = static_cast<char*>(resultData);

        int32_t nchars = 0;
        uint32_t written = 0;
        TextEcodeInfo encodeInfo(env, src, encoding_);
        EncodeToUtf8(encodeInfo, writeResult, &written, length, &nchars);

        napi_value result = nullptr;
        NAPI_CALL(env, napi_create_object(env, &result));

        napi_value read = nullptr;
        NAPI_CALL(env, napi_create_int32(env, nchars, &read));

        NAPI_CALL(env, napi_set_named_property(env, result, "read", read));

        napi_value resWritten = nullptr;
        NAPI_CALL(env, napi_create_uint32(env, written, &resWritten));

        NAPI_CALL(env, napi_set_named_property(env, result, "written", resWritten));

        return result;
    }
}
