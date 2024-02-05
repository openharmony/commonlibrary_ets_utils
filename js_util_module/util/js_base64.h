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

#include <cstring>

#ifdef IOS_PLATFORM
#include <sys/sysctl.h>
#else
#include <sys/sysinfo.h>
#endif

#include <unistd.h>
#include "napi/native_api.h"
#include "napi/native_node_api.h"

#ifndef UTIL_JS_BASE64_H
#define UTIL_JS_BASE64_H

namespace OHOS::Util {

    enum Type {
        BASIC,
        MIME,
        BASIC_URL_SAFE,
        MIME_URL_SAFE
    };

    struct EncodeInfo {
        napi_async_work worker = nullptr;
        napi_deferred deferred = nullptr;
        napi_value promise = nullptr;
        unsigned char *sinputEncode = nullptr;
        unsigned char *sinputEncoding = nullptr;
        size_t slength = 0;
        size_t soutputLen = 0;
        napi_env env;
        Type valueType = BASIC;
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
        Type valueType = BASIC;
    };

    enum ConverterFlags {
        BIT_FLG = 0x40,
        SIXTEEN_FLG = 0x3F,
        XFF_FLG = 0xFF,
    };

    void FreeMemory(unsigned char *address);
    void FreeMemory(char *address);
    unsigned char *EncodeAchieves(EncodeInfo *encodeInfo);
    unsigned char *DecodeAchieves(DecodeInfo *decodeInfo);

    class Base64 {
    public:
        /**
         * Constructor of Base64.
         */
        explicit Base64() {}

        /**
         * Destructor of Base64.
         */
        virtual ~Base64() {}

        /**
         * Output the corresponding text after encoding the input parameters.
         *
         * @param env NAPI environment parameters.
         * @param src Encode the input uint8 array.
         */
        napi_value EncodeSync(napi_env env, napi_value src, Type valueType);

        /**
         * Output the corresponding text after encoding the input parameters.
         *
         * @param env NAPI environment parameters.
         * @param src Encode the input uint8 array.
         */
        napi_value EncodeToStringSync(napi_env env, napi_value src, Type valueType);

        /**
         * Output the corresponding text after encoding the input parameters.
         *
         * @param env NAPI environment parameters.
         * @param src Decode the input uint8 array or string.
         */
        napi_value DecodeSync(napi_env env, napi_value src, Type valueType);

        /**
         * Output the corresponding text after asynchronously encoding the input parameters.
         *
         * @param env NAPI environment parameters.
         * @param src Asynchronously encoded input uint8 array.
         */
        napi_value Encode(napi_env env, napi_value src, Type valueType);

        /**
         * Output the corresponding text after asynchronously encoding the input parameters.
         *
         * @param env NAPI environment parameters.
         * @param src Asynchronously encoded input uint8 array.
         */
        napi_value EncodeToString(napi_env env, napi_value src, Type valueType);

        /**
         * Output the corresponding text after asynchronously encoding the input parameters.
         *
         * @param env NAPI environment parameters.
         * @param src Asynchronously decode the input uint8 array or string.
         */
        napi_value Decode(napi_env env, napi_value src, Type valueType);

    private:
        unsigned char *DecodeAchieve(napi_env env, const char *input, size_t inputLen, Type valueType);
        unsigned char *EncodeAchieve(const unsigned char *input, size_t inputLen, Type valueType);
        int Finds(napi_env env, char ch, Type valueType);
        size_t DecodeOut(size_t equalCount, size_t retLen);
        size_t retLen = 0;
        size_t decodeOutLen = 0;
        size_t outputLen = 0;
        unsigned char *pret = nullptr;
        const unsigned char *inputEncode_ = nullptr;
        const char *inputDecode_ = nullptr;
        unsigned char *retDecode = nullptr;
        void CreateEncodePromise(napi_env env, unsigned char *inputDecode, size_t length, Type valueType);
        void CreateEncodeToStringPromise(napi_env env, unsigned char *inputDecode, size_t length, Type valueType);
        void CreateDecodePromise(napi_env env, char *inputDecode, size_t length, Type valueType);
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
#endif // UTIL_JS_BASE64_H
