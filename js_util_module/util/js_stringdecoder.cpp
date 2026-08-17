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
    bool result = false;
    napi_is_typedarray(env, src, &result);
    if (!result) {
        napi_throw_error(env, "401",
            "Parameter error. The type of Parameter must be Uint8Array or string.");
        return nullptr;
    }
    napi_get_typedarray_info(env, src, &type, &length, &data, &arrayBuffer, &byteOffset);
    const char *source = static_cast<char*>(data);
    size_t limit = static_cast<size_t>(ucnv_getMinCharSize(conv_)) * length;
    size_t outputLimit = limit + 1;
    UChar *arr = nullptr;
    if (limit > 0) {
        arr = new UChar[outputLimit] { 0 };
    } else {
        napi_throw_error(env, ERROR_CODE, "Error obtaining minimum number of input bytes");
        return nullptr;
    }
    UChar *target = arr;
    UErrorCode codeFlag = U_ZERO_ERROR;
    ucnv_toUnicode(conv_, &target, target + outputLimit, &source, source + length, nullptr, flush, &codeFlag);
    if (U_FAILURE(codeFlag)) {
        ucnv_resetToUnicode(conv_);
        FreedMemory(arr);
        std::string err = "decoder error, ";
        err += u_errorName(codeFlag);
        napi_throw_error(env, ERROR_CODE, err.c_str());
        return nullptr;
    }

    napi_value resultStr = nullptr;
    size_t resultLen = target - arr;
    if (napi_create_string_utf16(env, reinterpret_cast<char16_t *>(arr), resultLen, &resultStr) != napi_ok) {
        HILOG_ERROR("StringDecoder:: create string error!");
        FreedMemory(arr);
        return nullptr;
    }
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
    // Max pending output: one supplementary char (2 UChars) + potential overflow (1 UChar)
    static constexpr int maxOutputLen = 8;
    UChar outputBuffer[maxOutputLen];
    UChar *target = outputBuffer;
    const char *emptySrc = "";
    const char *emptySrcEnd = emptySrc;
    UErrorCode errorCode = U_ZERO_ERROR;
    ucnv_toUnicode(conv_, &target, outputBuffer + maxOutputLen,
                   &emptySrc, emptySrcEnd, nullptr, true, &errorCode);
    if (U_FAILURE(errorCode)) {
        ucnv_resetToUnicode(conv_);
        std::string err = "decoder error, ";
        err += u_errorName(errorCode);
        napi_throw_error(env, ERROR_CODE, err.c_str());
        return nullptr;
    }
    size_t resultLen = target - outputBuffer;
    if (resultLen == 0) {
        NAPI_CALL(env, napi_create_string_utf8(env, "", NAPI_AUTO_LENGTH, &resultStr));
        return resultStr;
    }
    if (napi_create_string_utf16(env, reinterpret_cast<char16_t *>(outputBuffer),
                                 resultLen, &resultStr) != napi_ok) {
        HILOG_ERROR("StringDecoder:: end create string error!");
        return nullptr;
    }
    return resultStr;
}

void StringDecoder::FreedMemory(UChar *&pData)
{
    if (pData != nullptr) {
        delete[] pData;
        pData = nullptr;
    }
}
}
