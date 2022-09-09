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

#include "native_engine.h"
#include "securec.h"
#include "utils/log.h"

namespace OHOS::Util {
    napi_value TextEncoder::GetEncoding(napi_env env) const
    {
        napi_value result = nullptr;
        NAPI_CALL(env, napi_create_string_utf8(env, encoding_.c_str(), encoding_.length(), &result));

        return result;
    }

    napi_value TextEncoder::Encode(napi_env env, napi_value src) const
    {
        std::string buffer = "";
        if (!(encoding_ == "utf-8" || encoding_ == "UTF-8")) {
            NativeEngine *engine = reinterpret_cast<NativeEngine*>(env);
            NativeValue *nativeValue = reinterpret_cast<NativeValue*>(src);
            engine->EncodeToChinese(nativeValue, buffer, encoding_);
        } else {
            size_t bufferSize = 0;
            if (napi_get_value_string_utf8(env, src, nullptr, 0, &bufferSize) != napi_ok) {
                HILOG_ERROR("can not get src size");
                return nullptr;
            }
            buffer.resize(bufferSize);
            if (napi_get_value_string_utf8(env, src, buffer.data(), bufferSize + 1, &bufferSize) != napi_ok) {
                HILOG_ERROR("can not get src value");
                return nullptr;
            }
        }

        size_t outLen = buffer.length();
        void *data = nullptr;
        napi_value arrayBuffer = nullptr;
        napi_create_arraybuffer(env, outLen, &data, &arrayBuffer);
        if (memcpy_s(data, outLen, reinterpret_cast<void*>(buffer.data()), outLen) != EOK) {
            HILOG_ERROR("copy buffer to arraybuffer error");
            return nullptr;
        }
        napi_value result = nullptr;
        napi_create_typedarray(env, napi_uint8_array, outLen, arrayBuffer, 0, &result);
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
        int32_t written = 0;
        NativeEngine *engine = reinterpret_cast<NativeEngine*>(env);
        NativeValue *nativeValue = reinterpret_cast<NativeValue*>(src);
        engine->EncodeToUtf8(nativeValue, writeResult, &written, length, &nchars);

        napi_value result = nullptr;
        NAPI_CALL(env, napi_create_object(env, &result));

        napi_value read = nullptr;
        NAPI_CALL(env, napi_create_int32(env, nchars, &read));

        NAPI_CALL(env, napi_set_named_property(env, result, "read", read));

        napi_value resWritten = nullptr;
        NAPI_CALL(env, napi_create_int32(env, written, &resWritten));

        NAPI_CALL(env, napi_set_named_property(env, result, "written", resWritten));

        return result;
    }
}
