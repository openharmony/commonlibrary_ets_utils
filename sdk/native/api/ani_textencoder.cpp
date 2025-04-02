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

#include <memory>
#include <securec.h>
#include <optional>
#include <vector>
#include "ani_textencoder.h"

#define ANI_RETURN_NULLOPT_ON_FAILURE(retCode, ...) \
    if (ANI_OK != (retCode)) {                      \
        HILOG_ERROR(__VA_ARGS__);                   \
        return std::nullopt;                        \
    }

#define ANI_RETURN_NULLPTR_ON_FAILURE(retCode, ...) \
    if (ANI_OK != (retCode)) {                      \
        HILOG_ERROR(__VA_ARGS__);                   \
        return nullptr;                             \
    }

namespace OHOS::ETSUtil {
std::string ANIUtils_ANIStringToStdString(ani_env *env, ani_string ani_str)
{
    ani_size strSize;
    env->String_GetUTF8Size(ani_str, &strSize);
    std::vector<char> buffer(strSize + 1); // +1 for null terminator
    ani_size bytesWritten = 0;
    env->String_GetUTF8(ani_str, buffer.data(), strSize + 1, &bytesWritten);
    buffer[bytesWritten] = '\0';
    std::string content = std::string(buffer.data());
    return content;
}

std::optional<ArrayBufferInfos> GetBufferInfo(ani_env *env, ani_arraybuffer buffer)
{
    ArrayBufferInfos res;
    ANI_RETURN_NULLOPT_ON_FAILURE(env->ArrayBuffer_GetInfo(buffer, &res.data, &res.length),
        "Internal failure: env->ArrayBuffer_GetInfo()");
    return res;
}

template <class... Args>
ani_object NewUint8Array(ani_env *env, const char* signature, Args... args)
{
    // todo with further optimization: Can FindClass and FindMethod be one-pass?
    ani_class arrayClass;
    ANI_RETURN_NULLPTR_ON_FAILURE(env->FindClass("Lescompat/Uint8Array;", &arrayClass),
        "Internal failure: env->FindClass()");
    ani_method arrayCtor;
    ANI_RETURN_NULLPTR_ON_FAILURE(env->Class_FindMethod(arrayClass, "<ctor>", signature, &arrayCtor),
        "Internal failure: env->Class_FindMethod() with signature %{public}s", signature);
    ani_object res;
    ANI_RETURN_NULLPTR_ON_FAILURE(env->Object_New(arrayClass, arrayCtor, &res, args...),
        "Internal failure: env->Object_New()");
    return res;
}

template <class... Args>
std::optional<Uint8ArrayWithBufferInfo> NewUint8ArrayWithBufferInfo(
    ani_env *env, const char* signature, Args... args)
{
    ani_object res = NewUint8Array(env, signature, args...);
    if (res == nullptr) {
        return std::nullopt;
    }
    ani_ref buffer;
    ANI_RETURN_NULLOPT_ON_FAILURE(env->Object_GetFieldByName_Ref(res, "buffer", &buffer),
        "Internal failure: env->Object_GetFieldByName_Ref() with field \"buffer\".");
    void *bufData;
    size_t bufLength;
    ANI_RETURN_NULLOPT_ON_FAILURE(
        env->ArrayBuffer_GetInfo(static_cast<ani_arraybuffer>(buffer), &bufData, &bufLength),
        "Internal failure: env->ArrayBuffer_GetInfo()");
    return Uint8ArrayWithBufferInfo{res, buffer, bufData, bufLength};
}

std::optional<WriteEncodedDataResult> OtherEncode(
    ani_env *env, std::u16string_view inputString, const UConverterWrapper &cvt, char *dest, size_t destSize)
{
    if (!cvt.IsValid()) {
        return std::nullopt;
    }
    char *destHead = dest;
    char *destTail = destHead + destSize;
    char *destWrittenUntil = destHead;
    size_t nInputCharsConsumed = 0;
    size_t startPos = 0;
    size_t endPos = 0;
    while (startPos < inputString.length()) {
        endPos = inputString.find('\0', startPos);
        if (endPos == std::string_view::npos) {
            endPos = inputString.length();
        }
        const UChar *inputHead = inputString.data() + startPos;
        const UChar *inputTail = inputString.data() + endPos;
        const UChar *inputReadUntil = inputHead;
        UErrorCode codeFlag = U_ZERO_ERROR;
        ucnv_fromUnicode(cvt.converter, &destWrittenUntil, destTail, &inputReadUntil, inputTail,
                         nullptr, true, &codeFlag);
        // Note: U_BUFFER_OVERFLOW_ERROR is expected result when the output buffer is small.
        if (codeFlag != U_ZERO_ERROR && codeFlag != U_BUFFER_OVERFLOW_ERROR) {
            HILOG_ERROR("TextEncoder:: Failure when converting to encoding %{public}s%{public}s",
                cvt.encoding, u_errorName(codeFlag));
            return std::nullopt;
        }
        // todo: nInputCharsConsumes is probably incorrect
        // with "huge characters" which needs 2 char16_t to store in UTF-16.
        nInputCharsConsumed += (inputReadUntil - inputHead);
        // todo: Probably incorrect behavior if '\0' in destination encoding consumes multiple bytes.
        if (endPos < inputString.length()) {
            if (destWrittenUntil < destTail) {
                *destWrittenUntil++ = '\0';
            }
        }
        if (codeFlag == U_BUFFER_OVERFLOW_ERROR || destWrittenUntil == destTail) {
            break;
        }
        startPos = endPos + 1;
    }
    return WriteEncodedDataResult{
        nInputCharsConsumed,
        destWrittenUntil - destHead,
    };
}

std::optional<WriteEncodedDataResult> OtherEncode(
    ani_env *env, ani_string inputStringObj, const UConverterWrapper &cvt, char *dest, size_t destSize)
{
    ani_size inputSize = 0;
    ANI_RETURN_NULLOPT_ON_FAILURE(env->String_GetUTF16Size(inputStringObj, &inputSize),
        "Internal error: env->String_GetUTF16Size() failed.");
    std::unique_ptr<char16_t[]> inputBuffer = std::make_unique<char16_t[]>(inputSize + 1);
    ANI_RETURN_NULLOPT_ON_FAILURE(
        env->String_GetUTF16(inputStringObj, reinterpret_cast<uint16_t*>(inputBuffer.get()),
                             inputSize + 1, &inputSize),
        "Internal error: env->String_GetUTF16() failed.");
    return OtherEncode(env, std::u16string_view{inputBuffer.get(), inputSize}, cvt, dest, destSize);
}

ani_arraybuffer OtherEncodeToBuffer(
    ani_env *env, ani_string inputStringObj, const UConverterWrapper &cvt, WriteEncodedDataResult *writeRes)
{
    ani_size inputSize = 0;
    ANI_RETURN_NULLPTR_ON_FAILURE(env->String_GetUTF16Size(inputStringObj, &inputSize),
        "Internal error: env->String_GetUTF16Size() failed.");
    ani_arraybuffer buffer;
    void *bufferData;
    size_t bufferSize = ucnv_getMaxCharSize(cvt.converter) * inputSize;
    ANI_RETURN_NULLPTR_ON_FAILURE(env->CreateArrayBuffer(bufferSize, &bufferData, &buffer),
        "Internal error: env->CreateArrayBuffer() failed.");

    std::unique_ptr<char16_t[]> inputData = std::make_unique<char16_t[]>(inputSize + 1);
    // todo: Is releasing buffer needed?
    ANI_RETURN_NULLPTR_ON_FAILURE(
        env->String_GetUTF16(inputStringObj, reinterpret_cast<uint16_t*>(inputData.get()),
                             inputSize + 1, &inputSize),
        "Internal error: env->String_GetUTF16() failed.");
    std::optional<WriteEncodedDataResult> writeResOpt = OtherEncode(
        env, std::u16string_view{inputData.get(), inputSize}, cvt, static_cast<char*>(bufferData), bufferSize);
    if (!writeResOpt) {
        return nullptr; // todo: Is releasing buffer needed?
    }
    if (writeRes != nullptr) {
        *writeRes = *writeResOpt;
    }
    return buffer;
}

ani_object OtherEncodeToUint8Array(ani_env *env, ani_string inputStringObj, const UConverterWrapper &cvt)
{
    WriteEncodedDataResult writeRes;
    ani_arraybuffer buffer = OtherEncodeToBuffer(env, inputStringObj, cvt, &writeRes);
    if (buffer == nullptr) {
        return nullptr;
    }
    return NewUint8Array(env, "Lescompat/Buffer;II:V",
                         buffer, ani_int{0}, static_cast<ani_int>(writeRes.resultSizeBytes));
}

std::optional<WriteEncodedDataResult> WriteEncodedData(
    ani_env *env, ani_string inputStringObj, const std::string &encoding, char *dest, size_t destSizeBytes)
{
    if (encoding == "utf-8") {
        std::string utf8InputString = ANIUtils_ANIStringToStdString(env, inputStringObj);
        size_t nInputCharsConsumed;
        bool ok = false;
        std::string_view inputPrefix = ani_helper::Utf8GetPrefix(
            utf8InputString, destSizeBytes, &nInputCharsConsumed, &ok);
        if (!ok) {
            HILOG_ERROR("TextEncoder:: Failure during reading UTF-8 input.");
            return std::nullopt;
        }
        size_t resultSizeBytes = inputPrefix.length();
        if (EOK != memcpy_s(dest, destSizeBytes, inputPrefix.data(), resultSizeBytes)) {
            HILOG_ERROR("TextEncoder:: Failure during memcpy_s.");
            return std::nullopt;
        }
        return WriteEncodedDataResult{nInputCharsConsumed, resultSizeBytes};
    }
    if (encoding == "utf-16le" || encoding == "utf-16be") {
        std::string utf8InputString = ANIUtils_ANIStringToStdString(env, inputStringObj);
        size_t resultLengthLimit = destSizeBytes / 2;
        size_t nInputCharsConsumed;
        bool ok = false;
        std::u16string u16Str = ani_helper::Utf8ToUtf16LE(
            utf8InputString, resultLengthLimit, &nInputCharsConsumed, &ok);
        if (!ok) {
            HILOG_ERROR("TextEncoder:: Failure during conversion from UTF-8 to UTF-16.");
            return std::nullopt;
        }
        if (encoding == "utf-16be") {
            u16Str = ani_helper::Utf16LEToBE(u16Str);
        }
        size_t resultSizeBytes = u16Str.length() * 2; // 2 : 2 bytes per u16 character
        if (EOK != memcpy_s(dest, destSizeBytes, u16Str.data(), resultSizeBytes)) {
            HILOG_ERROR("TextEncoder:: Failure during memcpy_s");
            return std::nullopt;
        }
        return WriteEncodedDataResult{nInputCharsConsumed, resultSizeBytes};
    }
    UConverterWrapper cvt(encoding.c_str());
    if (!cvt.IsValid()) {
        return std::nullopt;
    }
    return OtherEncode(env, inputStringObj, cvt, dest, destSizeBytes);
}

ani_object DoEncodeInto(ani_env *env, [[maybe_unused]] ani_object object, ani_string stringObj, ani_string aniEncoding)
{
    std::string encodingStr = ANIUtils_ANIStringToStdString(env, aniEncoding);
    if (encodingStr == "utf-8") {
        std::string inputString = ANIUtils_ANIStringToStdString(env, stringObj);
        std::optional<Uint8ArrayWithBufferInfo> arrInfo =
            NewUint8ArrayWithBufferInfo(env, "I:V", static_cast<ani_int>(inputString.length()));
        if (!arrInfo) {
            return nullptr; // todo: Exception
        }
        if (EOK != memcpy_s(arrInfo->bufferData, arrInfo->bufferLength, inputString.data(), inputString.length())) {
            HILOG_ERROR("TextEncoder:: Failure during memcpy_s.");
            return nullptr; // todo: Exception
        }
        return arrInfo->arrayObject;
    }
    if (encodingStr == "utf-16le" || encodingStr == "utf-16be") {
        std::string inputString = ANIUtils_ANIStringToStdString(env, stringObj);
        bool ok = false;
        std::u16string utf16Str = ani_helper::Utf8ToUtf16LE(inputString, &ok);
        if (!ok) {
            HILOG_ERROR("TextEncoder:: Failure during conversion from UTF-8 to UTF-16.");
            return nullptr; // todo: Exception handling
        }
        if (encodingStr == "utf-16be") {
            utf16Str = ani_helper::Utf16LEToBE(utf16Str);
        }
        size_t sizeBytes = utf16Str.length() * 2; // 2 : 2 bytes per UTF-16 character
        std::optional<Uint8ArrayWithBufferInfo> arrInfo =
            NewUint8ArrayWithBufferInfo(env, "I:V", static_cast<ani_int>(sizeBytes));
        if (!arrInfo) {
            return nullptr; // todo: Exception
        }
        if (EOK != memcpy_s(arrInfo->bufferData, arrInfo->bufferLength, utf16Str.data(), sizeBytes)) {
            HILOG_ERROR("TextEncoder:: Failure during memcpy_s.");
            return nullptr; // todo: Exception
        }
        return arrInfo->arrayObject;
    }
    UConverterWrapper cvt(encodingStr.c_str());
    if (!cvt.IsValid()) {
        return nullptr; // todo: Exception
    }
    ani_object res = OtherEncodeToUint8Array(env, stringObj, cvt);
    return res; // todo: Exception when res == nullptr
}

ani_object DoEncodeIntoUint8Array(
    ani_env *env, [[maybe_unused]] ani_object object,
    ani_string inputStringObj, ani_string encodingObj, ani_object destObj)
{
    std::string encoding = ANIUtils_ANIStringToStdString(env, encodingObj);
    ani_int byteLength;
    ani_int byteOffset;
    ani_ref buffer;
    if (ANI_OK != env->Object_GetFieldByName_Int(destObj, "byteLengthInt", &byteLength) ||
        ANI_OK != env->Object_GetFieldByName_Int(destObj, "byteOffsetInt", &byteOffset) ||
        ANI_OK != env->Object_GetFieldByName_Ref(destObj, "buffer", &buffer)) {
        HILOG_ERROR("TextEncoder:: Failed to get byteLength, byteOffset, buffer from Uint8Array.");
        return nullptr; // todo: Exception handling
    }
    std::optional<ArrayBufferInfos> bufInfo = GetBufferInfo(env, static_cast<ani_arraybuffer>(buffer));
    if (!bufInfo) {
        return nullptr; // todo: Exception handling
    }
    std::optional<WriteEncodedDataResult> writeRes = WriteEncodedData(
        env, inputStringObj, encoding, static_cast<char*>(bufInfo->data) + byteOffset, byteLength);
    if (!writeRes) {
        return nullptr; // todo: Exception handling
    }

    ani_class resultClass;
    const char *resultClassName = "L@ohos/util/util/EncodeIntoUint8ArrayInfoInner;";
    if (ANI_OK != env->FindClass(resultClassName, &resultClass)) {
        HILOG_ERROR("TextEncoder:: Failed to get class %{public}s", resultClassName);
        return nullptr; // todo: Exception handling
    }
    ani_method resultCtor;
    if (ANI_OK != env->Class_FindMethod(resultClass, "<ctor>", "II:V", &resultCtor)) {
        HILOG_ERROR("TextEncoder:: Failed to get constructor of class %{public}s", resultClassName);
        return nullptr; // todo: Exception handling
    }
    ani_object res;
    if (ANI_OK != env->Object_New(resultClass, resultCtor, &res,
                                  static_cast<ani_int>(writeRes->nInputCharsConsumed),
                                  static_cast<ani_int>(writeRes->resultSizeBytes))) {
        HILOG_ERROR("TextEncoder:: Failed to construct object of class %{public}s", resultClassName);
        return nullptr; // todo: Exception handling
    }
    return res;
}
} // namespace OHOS::ETSUtil