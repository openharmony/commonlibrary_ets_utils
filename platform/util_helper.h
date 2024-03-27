/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef PLATFORM_UTIL_HELPER_H
#define PLATFORM_UTIL_HELPER_H

#include <codecvt>
#include <locale>
#include <string>

#include "unicode/unistr.h"
#include "unicode/ucnv.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "tools/log.h"

namespace Commonlibrary::Platform {
    UConverter* CreateConverter(const std::string& encStr_, UErrorCode& codeflag);
    std::string ConvertToString(UChar* uchar, size_t length);
    void EncodeIntoChinese(napi_env env, napi_value src, std::string encoding, std::string& buffer);
    std::string UnicodeConversion(std::string encoding, char16_t* originalBuffer, size_t inputSize);
    void FreedMemory(char *data);
    int GetMaxByteSize(std::string encoding);
} // namespace Commonlibrary::Platform
#endif // PLATFORM_UTIL_HELPER_H