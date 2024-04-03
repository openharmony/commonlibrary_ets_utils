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

#include "js_stringdecoder.h"
#include "util_helper.h"

namespace OHOS::Util {
using namespace Commonlibrary::Platform;
static const char* ERROR_CODE = "401";

StringDecoder::StringDecoder(const std::string &encoding)
{
    UErrorCode codeflag = U_ZERO_ERROR;
    conv_ = CreateConverter(encoding, codeflag);
}

napi_value StringDecoder::Write(napi_env env, napi_value src, UBool flush)
{
    napi_typedarray_type type;
    size_t length = 0;
    void *data = nullptr;
    size_t byteOffset = 0;
    napi_value arrayBuffer = nullptr;
    NAPI_CALL(env, napi_get_typedarray_info(env, src, &type, &length, &data, &arrayBuffer, &byteOffset));
    const char *source = static_cast<char*>(data);
    size_t limit = static_cast<size_t>(ucnv_getMinCharSize(conv_)) * length;
    size_t len = limit * sizeof(UChar);
    UChar *arr = nullptr;
    if (limit > 0) {
        arr = new UChar[limit + 1] { 0 };
    } else {
        napi_throw_error(env, ERROR_CODE, "Error obtaining minimum number of input bytes");
        return nullptr;
    }
    UChar *target = arr;
    UErrorCode codeFlag = U_ZERO_ERROR;
    ucnv_toUnicode(conv_, &target, target + len, &source, source + length, nullptr, flush, &codeFlag);
    if (U_FAILURE(codeFlag)) {
        FreedMemory(arr);
        std::string err = "decoder error, ";
        err += u_errorName(codeFlag);
        napi_throw_error(env, ERROR_CODE, err.c_str());
        return nullptr;
    }
    pendingLen_ = ucnv_toUCountPending(conv_, &codeFlag);
    pend_ = source + length - pendingLen_;
    std::string tepStr = ConvertToString(arr, length);
    napi_value resultStr = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, tepStr.c_str(), tepStr.size(), &resultStr));
    FreedMemory(arr);
    return resultStr;
}

napi_value StringDecoder::End(napi_env env, napi_value src)
{
    return Write(env, src, true);
}

napi_value StringDecoder::End(napi_env env)
{
    napi_value resultStr = nullptr;
    if (pendingLen_ == 0) {
        NAPI_CALL(env, napi_create_string_utf8(env, "", NAPI_AUTO_LENGTH, &resultStr));
        return resultStr;
    }
    UErrorCode errorCode = U_ZERO_ERROR;
    UChar outputBuffer[pendingLen_];
    UChar *targetEnd = outputBuffer + pendingLen_;
    UChar *target = outputBuffer;
    const char *src = pend_;
    const char *sourceEnd = pend_ + pendingLen_;
    UBool flush = true;
    ucnv_toUnicode(conv_, &target, targetEnd, &src, sourceEnd, nullptr, flush, &errorCode);
    if (U_FAILURE(errorCode)) {
        std::string err = "decoder error, ";
        err += u_errorName(errorCode);
        napi_throw_error(env, ERROR_CODE, err.c_str());
        return nullptr;
    }
    std::string tepStr = ConvertToString(target, pendingLen_);
    NAPI_CALL(env, napi_create_string_utf8(env, tepStr.c_str(), tepStr.size(), &resultStr));
    return resultStr;
}

void StringDecoder::FreedMemory(UChar *pData)
{
    if (pData != nullptr) {
        delete[] pData;
        pData = nullptr;
    }
}
}
