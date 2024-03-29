/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef UTIL_JS_STRINGDECODER_H
#define UTIL_JS_STRINGDECODER_H

#include <string>
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "unicode/ucnv.h"

namespace OHOS::Util {
class StringDecoder {
public:
    explicit StringDecoder(const std::string &encoding);
    ~StringDecoder() { ucnv_close(conv_); }
    napi_value Write(napi_env env, napi_value src, UBool flush = false);
    napi_value End(napi_env env, napi_value src);
    napi_value End(napi_env env);
private:
    void FreedMemory(UChar *pData);
    const char* pend_ {};
    int pendingLen_ {};
    UConverter* conv_ {};
    };
}
#endif // UTIL_JS_STRINGDECODER_H
