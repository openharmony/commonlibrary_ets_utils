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

#include "js_textencoder.h"

#include <string>

#include "native_engine.h"
#include "securec.h"
#include "utils/log.h"
namespace OHOS::Util {
    TextEncoder::TextEncoder(napi_env env) : env_(env), encoding_("utf-8")
    {
    }

    napi_value TextEncoder::GetEncoding() const
    {
        napi_value result = nullptr;
        NAPI_CALL(env_, napi_create_string_utf8(env_, encoding_.c_str(), encoding_.length(), &result));

        return result;
    }

    napi_value TextEncoder::Encode(napi_value src) const
    {
        char *buffer = nullptr;
        size_t bufferSize = 0;

        NAPI_CALL(env_, napi_get_value_string_utf8(env_, src, buffer, 0, &bufferSize));
        NAPI_ASSERT(env_, bufferSize > 0, "bufferSize == 0");
        buffer = new char[bufferSize + 1];

        NAPI_CALL(env_, napi_get_value_string_utf8(env_, src, buffer, bufferSize + 1, &bufferSize));

        void *data = nullptr;
        napi_value arrayBuffer = nullptr;
        NAPI_CALL(env_, napi_create_arraybuffer(env_, bufferSize, &data, &arrayBuffer));
        if (memcpy_s(data, bufferSize, reinterpret_cast<void*>(buffer), bufferSize) != 0) {
            HILOG_ERROR("copy buffer to arraybuffer error");
            return nullptr;
        }

        delete []buffer;

        napi_value result = nullptr;
        NAPI_CALL(env_, napi_create_typedarray(env_, napi_uint8_array, bufferSize, arrayBuffer, 0, &result));

        return result;
    }

    napi_value TextEncoder::EncodeInto(napi_value src, napi_value dest) const
    {
        napi_typedarray_type type;
        size_t byteOffset = 0;
        size_t length = 0;
        void *resultData = nullptr;
        napi_value resultBuffer = nullptr;
        NAPI_CALL(env_, napi_get_typedarray_info(env_, dest, &type, &length, &resultData, &resultBuffer, &byteOffset));

        char *writeResult = static_cast<char*>(resultData) + byteOffset;

        int32_t nchars = 0;
        uint32_t written = 0;
        NativeEngine *engine = reinterpret_cast<NativeEngine*>(env_);
        NativeValue *nativeValue = reinterpret_cast<NativeValue*>(src);
        engine->EncodeToUtf8(nativeValue, writeResult, &written, length, &nchars);

        napi_value result = nullptr;
        NAPI_CALL(env_, napi_create_object(env_, &result));

        napi_value read = nullptr;
        NAPI_CALL(env_, napi_create_uint32(env_, nchars, &read));

        NAPI_CALL(env_, napi_set_named_property(env_, result, "read", read));

        napi_value resWritten = nullptr;
        NAPI_CALL(env_, napi_create_uint32(env_, written, &resWritten));

        NAPI_CALL(env_, napi_set_named_property(env_, result, "written", resWritten));

        return result;
    }
}