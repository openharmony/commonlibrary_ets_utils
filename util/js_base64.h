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

#include <cstring>
#include <sys/sysinfo.h>
#include <unistd.h>
#include "napi/native_api.h"
#include "napi/native_node_api.h"

#ifndef BASE_COMPILERUNTIME_JS_UTIL_MODULE_BASE64_CLASS_H
#define BASE_COMPILERUNTIME_JS_UTIL_MODULE_BASE64_CLASS_H

namespace OHOS::Util {
    struct EncodeInfo {
        napi_async_work worker = nullptr;
        napi_deferred deferred = nullptr;
        napi_value promise = nullptr;
        unsigned char *sinputEncode = nullptr;
        unsigned char *sinputEncoding = nullptr;
        size_t slength = 0;
        size_t soutputLen = 0;
        napi_env env;
    };

    struct DecodeInfo {
        napi_async_work worker = nullptr;
        napi_deferred deferred = nullptr;
        napi_value promise = nullptr;
        char *sinputDecode = nullptr;
        unsigned char *sinputDecoding = nullptr;
        size_t slength = 0;
        size_t decodeOutLen = 0;
        size_t retLen = 0;
        napi_env env;
    };

    enum ConverterFlags {
        BIT_FLG = 0x40,
        SIXTEEN_FLG = 0x3F,
        XFF_FLG = 0xFF,
    };
    void FreeMemory(unsigned char *address);
    unsigned char *EncodeAchieves(EncodeInfo *encodeInfo);
    unsigned char *DecodeAchieves(DecodeInfo *decodeInfo);
    class Base64 {
    public:
        explicit Base64(napi_env env);
        virtual ~Base64() {}
        napi_value EncodeSync(napi_value src);
        napi_value EncodeToStringSync(napi_value src);
        napi_value DecodeSync(napi_value src);
        napi_value Encode(napi_value src);
        napi_value EncodeToString(napi_value src);
        napi_value Decode(napi_value src);
    private:
        napi_env env;
        unsigned char *DecodeAchieve(const char *input, size_t inputLen);
        unsigned char *EncodeAchieve(const unsigned char *input, size_t inputLen);
        size_t Finds(char ch);
        size_t DecodeOut(size_t equalCount, size_t retLen);
        void FreeMemory(unsigned char *address);
        void FreeMemory(char *address);
        size_t retLen = 0;
        size_t decodeOutLen = 0;
        size_t outputLen = 0;
        unsigned char *pret = nullptr;
        const unsigned char *inputEncode_ = nullptr;
        const char *inputDecode_ = nullptr;
        unsigned char *retDecode = nullptr;
        void CreateEncodePromise(unsigned char *inputDecode, size_t length);
        void CreateEncodeToStringPromise(unsigned char *inputDecode, size_t length);
        void CreateDecodePromise(char *inputDecode, size_t length);
        EncodeInfo *stdEncodeInfo_ = nullptr;
        DecodeInfo *stdDecodeInfo_ = nullptr;
        static void ReadStdEncode(napi_env env, void *data);
        static void EndStdEncode(napi_env env, napi_status status, void *buffer);
        static void ReadStdEncodeToString(napi_env env, void *data);
        static void EndStdEncodeToString(napi_env env, napi_status status, void *buffer);
        static void ReadStdDecode(napi_env env, void *data);
        static void EndStdDecode(napi_env env, napi_status status, void *buffer);
    };
}
#endif