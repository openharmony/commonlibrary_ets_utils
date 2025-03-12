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

#ifndef UTIL_JS_TEXTENCODER_H
#define UTIL_JS_TEXTENCODER_H
 
#include <ani.h>
#include <string>
#include <string_view>
#include <unicode/ucnv.h>
#include "tools/log.h"
 
namespace ani_helper {
std::u16string Utf8ToUtf16LE(std::string_view u8Str, bool *ok = nullptr);
std::u16string Utf8ToUtf16LE(std::string_view u8Str, size_t resultLengthLimit,
                             size_t *nInputCharsConsumed = nullptr, bool *ok = nullptr);
std::u16string Utf16LEToBE(std::u16string_view wstr);
std::string_view Utf8GetPrefix(std::string_view u8Str, size_t resultSizeBytesLimit,
                               size_t *nInputCharsConsumed = nullptr, bool *ok = nullptr);
}

namespace OHOS::ETSUtil {
struct UConverterWrapper {
    const char *encoding;
    UConverter *converter;

    explicit UConverterWrapper(const char *encoding): encoding(encoding)
    {
        UErrorCode codeflag = U_ZERO_ERROR;
        converter = ucnv_open(encoding, &codeflag);
        if (U_FAILURE(codeflag)) {
            HILOG_ERROR("ncnv_open failed with encoding '%s' and error '%s'.", encoding, u_errorName(codeflag));
            // converter is nullptr on failure
        }
    }

    UConverterWrapper(const UConverterWrapper&) = delete;
    UConverterWrapper& operator=(const UConverterWrapper&) = delete;

    ~UConverterWrapper()
    {
        ucnv_close(converter);
    }

    bool IsValid() const
    {
        return converter != nullptr;
    }
};

struct WriteEncodedDataResult {
    size_t nInputCharsConsumed;
    size_t resultSizeBytes;
};

struct ArrayBufferInfos {
    void* data;
    size_t length;
};

struct Uint8ArrayWithBufferInfo {
    ani_object arrayObject;
    ani_ref bufferObject;
    void* bufferData;
    size_t bufferLength;
};

ani_object DoEncodeInto(ani_env *env, [[maybe_unused]] ani_object object, ani_string stringObj, ani_string aniEncoding);
ani_object DoEncodeIntoUint8Array(ani_env *env, [[maybe_unused]] ani_object object, ani_string inputStringObj,
    ani_string encodingObj, ani_object destObj);
} // namespace ani_helper
#endif // UTIL_JS_TEXTENCODER_H
