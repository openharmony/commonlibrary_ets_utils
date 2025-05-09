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

#include "js_base64.h"
#include "securec.h"
#include "tools/log.h"
#include "tools/ets_error.h"

namespace OHOS::Util {
    namespace {
        static const size_t TRAGET_TWO = 2;
        static const size_t TRAGET_THREE = 3;
        static const size_t TRAGET_FOUR = 4;
        static const size_t TRAGET_SIX = 6;
        static const size_t TRAGET_EIGHT = 8;
        static const size_t TRAGET_SIXTYFIVE = 65;
        const char BASE[] = {
            65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82,
            83, 84, 85, 86, 87, 88, 89, 90, 97, 98, 99, 100, 101, 102, 103, 104, 105,
            106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120,
            121, 122, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 43, 47, 61
        };

        const char BASEURL[] = {
            65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82,
            83, 84, 85, 86, 87, 88, 89, 90, 97, 98, 99, 100, 101, 102, 103, 104, 105,
            106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120,
            121, 122, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 45, 95
        };
    }

    /* base64 encode */
    napi_value Base64::EncodeSync(napi_env env, napi_value src, Type valueType)
    {
        napi_typedarray_type type;
        size_t length = 0;
        void *resultData = nullptr;
        napi_get_typedarray_info(env, src, &type, &length, &resultData, nullptr, nullptr);
        if (type != napi_uint8_array || length == 0) {
            napi_throw_error(env, "401",
                "Parameter error. The type of Parameter must be Uint8Array and the length greater than zero.");
            return nullptr;
        }
        inputEncode_ = static_cast<const unsigned char*>(resultData);
        unsigned char *rets = EncodeAchieve(inputEncode_, length, valueType);
        if (rets == nullptr) {
            napi_throw_error(env, "-1", "encode input is null");
            return nullptr;
        }
        void *data = nullptr;
        napi_value arrayBuffer = nullptr;
        size_t bufferSize = outputLen;
        napi_create_arraybuffer(env, bufferSize, &data, &arrayBuffer);
        if (memcpy_s(data, bufferSize, reinterpret_cast<const void*>(rets), bufferSize) != EOK) {
            FreeMemory(rets);
            HILOG_ERROR("Base64:: copy ret to arraybuffer error");
            return nullptr;
        }
        napi_value result = nullptr;
        napi_create_typedarray(env, napi_uint8_array, bufferSize, arrayBuffer, 0, &result);
        FreeMemory(rets);
        return result;
    }

    /* base64 encodeToString */
    napi_value Base64::EncodeToStringSync(napi_env env, napi_value src, Type valueType)
    {
        napi_typedarray_type type;
        size_t length = 0;
        void *resultData = nullptr;
        napi_get_typedarray_info(env, src, &type, &length, &resultData, nullptr, nullptr);
        if (type != napi_uint8_array || length == 0) {
            napi_throw_error(env, "401",
                "Parameter error. The type of Parameter must be Uint8Array and the length greater than zero.");
            return nullptr;
        }
        inputEncode_ = static_cast<const unsigned char*>(resultData);
        unsigned char *ret = EncodeAchieve(inputEncode_, length, valueType);
        if (ret == nullptr) {
            FreeMemory(ret);
            napi_throw_error(env, "-1", "encodeToString input is null");
            return nullptr;
        }
        const char *encString = reinterpret_cast<const char*>(ret);
        napi_value resultStr = nullptr;
        napi_create_string_utf8(env, encString, strlen(encString), &resultStr);
        FreeMemory(ret);
        return resultStr;
    }

    unsigned char *Base64::EncodeAchieve(const unsigned char *input, size_t inputLen, Type valueType)
    {
        unsigned char *ret = nullptr;
        outputLen = (inputLen / TRAGET_THREE) * TRAGET_FOUR;
        if ((inputLen % TRAGET_THREE) > 0) {
            outputLen += TRAGET_FOUR;
        }
        if (outputLen > 0) {
            ret = new (std::nothrow) unsigned char[outputLen + 1];
            if (ret == nullptr) {
                HILOG_ERROR("Base64:: memory allocation failed, ret is nullptr");
                return nullptr;
            }
            if (memset_s(ret, outputLen + 1, '\0', outputLen + 1) != EOK) {
                HILOG_ERROR("Base64:: encode ret memset_s failed");
                FreeMemory(ret);
                return nullptr;
            }
        } else {
            HILOG_ERROR("Base64:: outputLen is error");
            return nullptr;
        }
        if (ret == nullptr) {
            return ret;
        }

        bool flag = false;
        if (valueType == Type::BASIC_URL_SAFE || valueType == Type::MIME_URL_SAFE) {
            flag = true;
        }
        const char *searchArray = flag ? BASEURL : BASE;
        unsigned char *result = EncodeAchieveInner(input, ret, searchArray, inputLen, valueType);
        return result;
    }

    unsigned char *Base64::EncodeAchieveInner(const unsigned char *input, unsigned char *ret,
                                              const char *searchArray, size_t inputLen, Type valueType)
    {
        size_t inp = 0;
        size_t temp = 0;
        size_t bitWise = 0;
        size_t index = 0;
        while (inp < inputLen) {
            temp = 0;
            bitWise = 0;
            while (temp < TRAGET_THREE) {
                if (inp >= inputLen) {
                    break;
                }
                bitWise = ((bitWise << TRAGET_EIGHT) | (input[inp] & XFF_FLG));
                inp++;
                temp++;
            }
            bitWise = (bitWise << ((TRAGET_THREE - temp) * TRAGET_EIGHT));
            for (size_t i = 0; i < TRAGET_FOUR; i++) {
                if (temp < i && (valueType == Type::BASIC_URL_SAFE || valueType == Type::MIME_URL_SAFE)) {
                    outputLen -= (temp == 1) ? TRAGET_TWO : 1;
                    break;
                } else if (temp < i && valueType != Type::BASIC_URL_SAFE && valueType != Type::MIME_URL_SAFE) {
                    ret[index++] = searchArray[BIT_FLG];
                } else {
                    ret[index++] = searchArray[(bitWise >> ((TRAGET_THREE - i) * TRAGET_SIX)) & SIXTEEN_FLG];
                }
            }
        }
        ret[index] = 0;
        return ret;
    }

    /* base64 decode */
    napi_value Base64::DecodeSync(napi_env env, napi_value src, Type valueType)
    {
        bool resDecode = DecodeSyncInner(env, src, valueType);
        if (!resDecode || pret == nullptr) {
            return nullptr;
        }
        void *data = nullptr;
        napi_value arrayBuffer = nullptr;
        size_t bufferSize = decodeOutLen;
        napi_create_arraybuffer(env, bufferSize, &data, &arrayBuffer);
        if (memcpy_s(data, bufferSize, reinterpret_cast<const void*>(pret), bufferSize) != EOK) {
            FreeMemory(pret);
            HILOG_ERROR("Base64:: copy retDecode to arraybuffer error");
            return nullptr;
        }
        napi_value result = nullptr;
        napi_create_typedarray(env, napi_uint8_array, bufferSize, arrayBuffer, 0, &result);
        FreeMemory(pret);
        return result;
    }

    bool Base64::DecodeSyncInner(napi_env env, napi_value src, Type valueType)
    {
        napi_valuetype valuetype = napi_undefined;
        napi_typeof(env, src, &valuetype);
        napi_typedarray_type type;
        size_t length = 0;
        void *resultData = nullptr;
        char *inputString = nullptr;
        if (valuetype != napi_valuetype::napi_string) {
            napi_get_typedarray_info(env, src, &type, &length, &resultData, nullptr, nullptr);
        }
        if (valuetype == napi_valuetype::napi_string) {
            size_t prolen = 0;
            napi_get_value_string_utf8(env, src, nullptr, 0, &prolen);
            if (prolen > 0) {
                inputString = new (std::nothrow) char[prolen + 1];
                if (inputString == nullptr) {
                    HILOG_ERROR("Base64:: memory allocation failed, inputString is nullptr");
                    return false;
                }
                if (memset_s(inputString, prolen + 1, '\0', prolen + 1) != EOK) {
                    FreeMemory(inputString);
                    napi_throw_error(env, "-1", "decode inputString memset_s failed");
                    return false;
                }
            } else {
                napi_throw_error(env, "-2", "prolen is error !");
                return false;
            }
            if (inputString != nullptr) {
                napi_get_value_string_utf8(env, src, inputString, prolen + 1, &prolen);
                pret = DecodeAchieve(env, inputString, prolen, valueType);
            }
        } else if (type == napi_typedarray_type::napi_uint8_array && length > 0) {
            inputDecode_ = static_cast<const char*>(resultData);
            pret = DecodeAchieve(env, inputDecode_, length, valueType);
        } else {
            std::string errMsg =
                "Parameter error. The type of Parameter must be Uint8Array or string and the length greater than 0.";
            napi_throw_error(env, "401", errMsg.c_str());
            return false;
        }
        FreeMemory(inputString);
        return true;
    }

    unsigned char *Base64::DecodeAchieve(napi_env env, const char *input, size_t inputLen, Type valueType)
    {
        retLen = (inputLen / TRAGET_FOUR) * TRAGET_THREE;
        decodeOutLen = retLen;
        size_t equalCount = 0;

        if (*(input + inputLen - 1) == '=') {
            equalCount++;
        }
        if (*(input + inputLen - TRAGET_TWO) == '=') {
            equalCount++;
        }
        retLen = DecodeOut(equalCount, retLen);
        if (retLen > 0) {
            retDecode = new (std::nothrow) unsigned char[retLen + 1];
            if (retDecode == nullptr) {
                HILOG_ERROR("Base64:: memory allocation failed, retDecode is nullptr");
                return nullptr;
            }
            if (memset_s(retDecode, retLen + 1, '\0', retLen + 1) != EOK) {
                FreeMemory(retDecode);
                napi_throw_error(env, "-1", "decode retDecode memset_s failed");
                return nullptr;
            }
        } else {
            napi_throw_error(env, "-2", "retLen is error !");
            return nullptr;
        }
        if (retDecode == nullptr) {
            return retDecode;
        }
        if (valueType == Type::BASIC_URL_SAFE || valueType == Type::MIME_URL_SAFE) {
            size_t remainder = inputLen % TRAGET_FOUR;
            if (remainder == TRAGET_TWO) {
                decodeOutLen += 1;
            } else if (remainder == TRAGET_THREE) {
                decodeOutLen += TRAGET_TWO;
            }
        }
        unsigned char *result = nullptr;
        result = DecodeAchieveInner(env, input, inputLen, equalCount, valueType);
        if (result == nullptr) {
            FreeMemory(retDecode);
        }
        return result;
    }

    unsigned char *Base64::DecodeAchieveInner(napi_env env, const char *input,
                                              size_t inputLen, size_t equalCount, Type valueType)
    {
        size_t index = 0;
        size_t inp = 0;
        size_t temp = 0;
        size_t bitWise = 0;
        while (inp < (inputLen - equalCount)) {
            temp = 0;
            bitWise = 0;
            while (temp < TRAGET_FOUR) {
                if (inp >= (inputLen - equalCount)) {
                    break;
                }
                int findsData = Finds(env, input[inp], valueType);
                if (findsData == -1) {
                    return nullptr;
                }
                bitWise = (bitWise << TRAGET_SIX) | static_cast<size_t>(findsData);
                inp++;
                temp++;
            }
            bitWise = bitWise << ((TRAGET_FOUR - temp) * TRAGET_SIX);
            for (size_t i = 0; i < TRAGET_THREE; i++) {
                if (i == temp) {
                    break;
                }
                retDecode[index++] = static_cast<char>((bitWise >> ((TRAGET_TWO - i) * TRAGET_EIGHT)) & XFF_FLG);
            }
        }
        retDecode[index] = 0;
        return retDecode;
    }

    size_t Base64::DecodeOut(size_t equalCount, size_t retLen)
    {
        size_t temp = retLen;
        switch (equalCount) {
            case 0:
                temp += TRAGET_FOUR;
                break;
            case 1:
                temp += TRAGET_FOUR;
                decodeOutLen -= 1;
                break;
            case TRAGET_TWO:
                temp += TRAGET_THREE;
                decodeOutLen -= TRAGET_TWO;
                break;
            default:
                temp += TRAGET_TWO;
                break;
        }
        return temp;
    }

    /* Decoding lookup function */
    int Base64::Finds(napi_env env, char ch, Type valueType)
    {
        bool flag = false;
        if (valueType == Type::BASIC_URL_SAFE || valueType == Type::MIME_URL_SAFE) {
            flag = true;
        }
        int tableLen = flag ? TRAGET_SIXTYFIVE - 1 : TRAGET_SIXTYFIVE;
        const char *searchArray = flag ? BASEURL : BASE;
        for (int i = 0; i < tableLen; i++) {
            if (searchArray[i] == ch) {
                return i;
            }
        }
        napi_throw_error(env, "-1", "The input string contains unsupported characters");
        return -1;
    }

    napi_value Base64::Encode(napi_env env, napi_value src, Type valueType)
    {
        napi_typedarray_type type;
        size_t length = 0;
        void *resultData = nullptr;
        napi_get_typedarray_info(env, src, &type, &length, &resultData, nullptr, nullptr);
        if (type != napi_uint8_array || length == 0) {
            napi_throw_error(env, "401",
                "Parameter error. The type of Parameter must be Uint8Array and the length greater than zero.");
            return nullptr;
        }
        unsigned char *inputEncode = nullptr;
        inputEncode = static_cast<unsigned char*>(resultData);
        CreateEncodePromise(env, inputEncode, length, valueType);
        if (stdEncodeInfo_ == nullptr) {
            HILOG_ERROR("Base64:: Encode return promise failed, stdEncodeInfo_ is nullptr");
            return nullptr;
        }
        return stdEncodeInfo_->promise;
    }
    
    napi_value Base64::EncodeToString(napi_env env, napi_value src, Type valueType)
    {
        napi_typedarray_type type;
        size_t length = 0;
        void *resultData = nullptr;
        napi_get_typedarray_info(env, src, &type, &length, &resultData, nullptr, nullptr);
        if (type != napi_uint8_array || length == 0) {
            napi_throw_error(env, "401",
                "Parameter error. The type of Parameter must be Uint8Array and the length greater than zero.");
            return nullptr;
        }
        unsigned char *inputEncode = nullptr;
        inputEncode = static_cast<unsigned char*>(resultData);
        CreateEncodeToStringPromise(env, inputEncode, length, valueType);
        if (stdEncodeInfo_ == nullptr) {
            HILOG_ERROR("Base64:: EncodeToString return promise failed, stdEncodeInfo_ is nullptr");
            return nullptr;
        }
        return stdEncodeInfo_->promise;
    }

    void Base64::CreateEncodePromise(napi_env env, unsigned char *inputDecode, size_t length, Type valueType)
    {
        napi_value resourceName = nullptr;
        stdEncodeInfo_ = new (std::nothrow) EncodeInfo();
        if (stdEncodeInfo_ == nullptr) {
            HILOG_ERROR("Base64:: memory allocation failed, stdEncodeInfo_ is nullptr");
            return;
        }
        stdEncodeInfo_->sinputEncode = inputDecode;
        stdEncodeInfo_->slength = length;
        stdEncodeInfo_->env = env;
        stdEncodeInfo_->valueType = valueType;
        napi_create_promise(env, &stdEncodeInfo_->deferred, &stdEncodeInfo_->promise);
        napi_create_string_utf8(env, "ReadStdEncode", NAPI_AUTO_LENGTH, &resourceName);
        napi_create_async_work(env, nullptr, resourceName, ReadStdEncode, EndStdEncode,
                               reinterpret_cast<void*>(stdEncodeInfo_), &stdEncodeInfo_->worker);
        napi_queue_async_work_with_qos(env, stdEncodeInfo_->worker, napi_qos_user_initiated);
    }

    void Base64::CreateEncodeToStringPromise(napi_env env, unsigned char *inputDecode, size_t length, Type valueType)
    {
        napi_value resourceName = nullptr;
        stdEncodeInfo_ = new (std::nothrow) EncodeInfo();
        if (stdEncodeInfo_ == nullptr) {
            HILOG_ERROR("Base64:: memory allocation failed, stdEncodeInfo_ is nullptr");
            return;
        }
        stdEncodeInfo_->sinputEncode = inputDecode;
        stdEncodeInfo_->slength = length;
        stdEncodeInfo_->valueType = valueType;
        napi_create_promise(env, &stdEncodeInfo_->deferred, &stdEncodeInfo_->promise);
        napi_create_string_utf8(env, "ReadStdEncodeToString", NAPI_AUTO_LENGTH, &resourceName);
        napi_create_async_work(env, nullptr, resourceName, ReadStdEncodeToString, EndStdEncodeToString,
                               reinterpret_cast<void*>(stdEncodeInfo_), &stdEncodeInfo_->worker);
        napi_queue_async_work_with_qos(env, stdEncodeInfo_->worker, napi_qos_user_initiated);
    }

    unsigned char *EncodeAchieves(napi_env env, EncodeInfo *encodeInfo)
    {
        const unsigned char *input = encodeInfo->sinputEncode;
        size_t inputLen = encodeInfo->slength;
        unsigned char *ret = nullptr;

        size_t outputLen = 0;
        outputLen = (inputLen / TRAGET_THREE) * TRAGET_FOUR;
        if ((inputLen % TRAGET_THREE) > 0) {
            outputLen += TRAGET_FOUR;
        }
        encodeInfo->soutputLen = outputLen;
        if (outputLen > 0) {
            ret = new unsigned char[outputLen + 1];
            if (memset_s(ret, outputLen + 1, '\0', outputLen + 1) != EOK) {
                FreeMemory(ret);
                napi_throw_error(encodeInfo->env, "-1", "ret path memset_s failed");
                return nullptr;
            }
        } else {
            napi_throw_error(encodeInfo->env, "-2", "outputLen is error !");
            return nullptr;
        }
        if (ret == nullptr) {
            return ret;
        }

        bool flag = false;
        if (encodeInfo->valueType == Type::BASIC_URL_SAFE || encodeInfo->valueType == Type::MIME_URL_SAFE) {
            flag = true;
        }
        const char *searchArray = flag ? BASEURL : BASE;
        unsigned char *result = nullptr;
        result = EncodeAchievesInner(ret, encodeInfo, searchArray, inputLen, input);
        return result;
    }

    unsigned char *EncodeAchievesInner(unsigned char *ret, EncodeInfo *encodeInfo,
                                       const char *searchArray, size_t inputLen, const unsigned char *input)
    {
        size_t inp = 0;
        size_t temp = 0;
        size_t bitWise = 0;
        size_t index = 0;
        while (inp < inputLen) {
            temp = 0;
            bitWise = 0;
            while (temp < TRAGET_THREE) {
                if (inp >= inputLen) {
                    break;
                }
                bitWise = ((bitWise << TRAGET_EIGHT) | (input[inp] & XFF_FLG));
                inp++;
                temp++;
            }
            bitWise = (bitWise << ((TRAGET_THREE - temp) * TRAGET_EIGHT));
            for (size_t i = 0; i < TRAGET_FOUR; i++) {
                if (temp < i &&
                    (encodeInfo->valueType == Type::BASIC_URL_SAFE || encodeInfo->valueType == Type::MIME_URL_SAFE)) {
                    encodeInfo->soutputLen -= (temp == 1) ? TRAGET_TWO : 1;
                    break;
                } else if (temp < i &&
                    (encodeInfo->valueType != Type::BASIC_URL_SAFE && encodeInfo->valueType != Type::MIME_URL_SAFE)) {
                    ret[index++] = searchArray[BIT_FLG];
                } else {
                    ret[index++] = searchArray[(bitWise >> ((TRAGET_THREE - i) * TRAGET_SIX)) & SIXTEEN_FLG];
                }
            }
        }
        ret[index] = 0;
        return ret;
    }

    void Base64::ReadStdEncode(napi_env env, void *data)
    {
        auto stdEncodeInfo = reinterpret_cast<EncodeInfo*>(data);
        unsigned char *rets = EncodeAchieves(env, stdEncodeInfo);
        stdEncodeInfo->sinputEncoding = rets;
    }

    void Base64::EndStdEncode(napi_env env, napi_status status, void *buffer)
    {
        auto stdEncodeInfo = reinterpret_cast<EncodeInfo*>(buffer);
        void *data = nullptr;
        napi_handle_scope scope = nullptr;
        napi_open_handle_scope(env, &scope);
        if (scope == nullptr) {
            return;
        }
        napi_value arrayBuffer = nullptr;
        size_t bufferSize = stdEncodeInfo->soutputLen;
        napi_create_arraybuffer(env, bufferSize, &data, &arrayBuffer);
        if (memcpy_s(data, bufferSize,
            reinterpret_cast<const void*>(stdEncodeInfo->sinputEncoding), bufferSize) != EOK) {
            HILOG_ERROR("Base64:: copy ret to arraybuffer error");
            napi_delete_async_work(env, stdEncodeInfo->worker);
            napi_close_handle_scope(env, scope);
            return;
        }
        napi_value result = nullptr;
        napi_create_typedarray(env, napi_uint8_array, bufferSize, arrayBuffer, 0, &result);
        napi_resolve_deferred(env, stdEncodeInfo->deferred, result);
        napi_delete_async_work(env, stdEncodeInfo->worker);
        napi_close_handle_scope(env, scope);
        delete[] stdEncodeInfo->sinputEncoding;
        delete stdEncodeInfo;
    }

    void Base64::ReadStdEncodeToString(napi_env env, void *data)
    {
        auto stdEncodeInfo = reinterpret_cast<EncodeInfo*>(data);
        unsigned char *rets = EncodeAchieves(env, stdEncodeInfo);
        stdEncodeInfo->sinputEncoding = rets;
    }

    void Base64::EndStdEncodeToString(napi_env env, napi_status status, void *buffer)
    {
        auto stdEncodeInfo = reinterpret_cast<EncodeInfo*>(buffer);
        napi_handle_scope scope = nullptr;
        napi_open_handle_scope(env, &scope);
        if (scope == nullptr) {
            return;
        }
        const char *encString = reinterpret_cast<const char*>(stdEncodeInfo->sinputEncoding);
        napi_value resultStr = nullptr;
        napi_create_string_utf8(env, encString, strlen(encString), &resultStr);
        napi_resolve_deferred(env, stdEncodeInfo->deferred, resultStr);
        napi_delete_async_work(env, stdEncodeInfo->worker);
        napi_close_handle_scope(env, scope);
        delete[] stdEncodeInfo->sinputEncoding;
        delete stdEncodeInfo;
    }

    napi_value Base64::Decode(napi_env env, napi_value src, Type valueType)
    {
        napi_valuetype valuetype = napi_undefined;
        napi_typeof(env, src, &valuetype);
        napi_typedarray_type type;
        size_t length = 0;
        void *resultData = nullptr;
        char *inputString = nullptr;
        char *inputDecode = nullptr;
        if (valuetype != napi_valuetype::napi_string) {
            if (napi_get_typedarray_info(env, src, &type, &length, &resultData, nullptr, nullptr) != napi_ok) {
                std::string errMsg =
                    "Parameter error. The type of Parameter must be Uint8Array or string.";
                napi_throw_error(env, "401", errMsg.c_str());
                return nullptr;
            }
        }
        if (valuetype == napi_valuetype::napi_string) {
            size_t prolen = 0;
            napi_get_value_string_utf8(env, src, nullptr, 0, &prolen);
            if (prolen > 0) {
                inputString = new char[prolen + 1];
                if (memset_s(inputString, prolen + 1, '\0', prolen + 1) != EOK) {
                    delete[] inputString;
                    napi_throw_error(env, "-1", "decode inputString memset_s failed");
                    return nullptr;
                }
            } else {
                napi_throw_error(env, "-2", "prolen is error !");
                return nullptr;
            }
            napi_get_value_string_utf8(env, src, inputString, prolen + 1, &prolen);
            CreateDecodePromise(env, inputString, prolen, valueType);
        } else if (type == napi_typedarray_type::napi_uint8_array && length > 0) {
            inputDecode = static_cast<char*>(resultData);
            CreateDecodePromise(env, inputDecode, length, valueType);
        } else {
            std::string errMsg =
                "Parameter error. The type of Parameter must be Uint8Array or string and the length greater than 0.";
            napi_throw_error(env, "401", errMsg.c_str());
            FreeMemory(inputString);
            return nullptr;
        }
        if (stdDecodeInfo_ == nullptr) {
            HILOG_ERROR("Base64:: Decode return promise failed, stdDecodeInfo_ is nullptr");
            return nullptr;
        }
        return stdDecodeInfo_->promise;
    }

    void Base64::CreateDecodePromise(napi_env env, char *inputDecode, size_t length, Type valueType)
    {
        napi_value resourceName = nullptr;
        stdDecodeInfo_ = new (std::nothrow) DecodeInfo();
        if (stdDecodeInfo_ == nullptr) {
            HILOG_ERROR("Base64:: memory allocation failed, stdDecodeInfo_ is nullptr");
            return;
        }
        stdDecodeInfo_->sinputDecode = inputDecode;
        stdDecodeInfo_->slength = length;
        stdDecodeInfo_->env = env;
        stdDecodeInfo_->valueType = valueType;
        napi_create_promise(env, &stdDecodeInfo_->deferred, &stdDecodeInfo_->promise);
        napi_create_string_utf8(env, "ReadStdDecode", NAPI_AUTO_LENGTH, &resourceName);
        napi_create_async_work(env, nullptr, resourceName, ReadStdDecode, EndStdDecode,
                               reinterpret_cast<void*>(stdDecodeInfo_), &stdDecodeInfo_->worker);
        napi_queue_async_work_with_qos(env, stdDecodeInfo_->worker, napi_qos_user_initiated);
    }

    int Finds(char ch, Type valueType)
    {
        bool flag = false;
        if (valueType == Type::BASIC_URL_SAFE || valueType == Type::MIME_URL_SAFE) {
            flag = true;
        }
        int tableLen = flag ? TRAGET_SIXTYFIVE - 1 : TRAGET_SIXTYFIVE;
        const char *searchArray = flag ? BASEURL : BASE;
        int couts = 0;
        for (int i = 0; i < tableLen; i++) {
            if (searchArray[i] == ch) {
                couts = i;
            }
        }
        return couts;
    }

    size_t DecodeOut(size_t equalCount, size_t retLen, DecodeInfo *decodeInfo)
    {
        switch (equalCount) {
            case 0:
                retLen += TRAGET_FOUR;
                break;
            case 1:
                retLen += TRAGET_FOUR;
                decodeInfo->decodeOutLen -= 1;
                break;
            case TRAGET_TWO:
                retLen += TRAGET_THREE;
                decodeInfo->decodeOutLen -= TRAGET_TWO;
                break;
            default:
                retLen += TRAGET_TWO;
                break;
        }
        return retLen;
    }

    unsigned char *DecodeAchieves(napi_env env, DecodeInfo *decodeInfo)
    {
        const char *input = decodeInfo->sinputDecode;
        size_t inputLen = decodeInfo->slength;
        size_t retLen = 0;
        retLen = (inputLen / TRAGET_FOUR) * TRAGET_THREE;
        decodeInfo->decodeOutLen = retLen;
        size_t equalCount = 0;
        unsigned char *retDecode = nullptr;
        if (*(input + inputLen - 1) == '=') {
            equalCount++;
        }
        if (*(input + inputLen - TRAGET_TWO) == '=') {
            equalCount++;
        }
        retLen = DecodeOut(equalCount, retLen, decodeInfo);
        if (retLen > 0) {
            retDecode = new unsigned char[retLen + 1];
            if (memset_s(retDecode, retLen + 1, '\0', retLen + 1) != EOK) {
                FreeMemory(retDecode);
                napi_throw_error(decodeInfo->env, "-1", "decode retDecode memset_s failed");
                return nullptr;
            }
        } else {
            napi_throw_error(decodeInfo->env, "-2", "retLen is error !");
            return nullptr;
        }
        if (decodeInfo->valueType == Type::BASIC_URL_SAFE || decodeInfo->valueType == Type::MIME_URL_SAFE) {
            size_t remainder = inputLen % TRAGET_FOUR;
            if (remainder == TRAGET_TWO) {
                decodeInfo->decodeOutLen += 1;
            } else if (remainder == TRAGET_THREE) {
                decodeInfo->decodeOutLen += TRAGET_TWO;
            }
        }

        unsigned char *result = nullptr;
        result = DecodeAchievesInner(inputLen, equalCount, input, decodeInfo, retDecode);
        if (result == nullptr) {
            FreeMemory(retDecode);
        }
        return result;
    }

    unsigned char *DecodeAchievesInner(size_t inputLen, size_t equalCount,
                                       const char *input, DecodeInfo *decodeInfo, unsigned char *retDecode)
    {
        size_t inp = 0;
        size_t temp = 0;
        size_t bitWise = 0;
        size_t index = 0;
        while (inp < (inputLen - equalCount)) {
            temp = 0;
            bitWise = 0;
            while (temp < TRAGET_FOUR) {
                if (inp >= (inputLen - equalCount)) {
                    break;
                }
                int findData = Finds(input[inp], decodeInfo->valueType);
                if (findData == -1) {
                    return nullptr;
                }
                bitWise = (bitWise << TRAGET_SIX) | static_cast<size_t>(findData);
                inp++;
                temp++;
            }
            bitWise = bitWise << ((TRAGET_FOUR - temp) * TRAGET_SIX);
            for (size_t i = 0; i < TRAGET_THREE; i++) {
                if (i == temp) {
                    break;
                }
                retDecode[index++] = static_cast<char>((bitWise >> ((TRAGET_TWO - i) * TRAGET_EIGHT)) & XFF_FLG);
            }
        }
        retDecode[index] = 0;
        return retDecode;
    }

    void Base64::ReadStdDecode(napi_env env, void *data)
    {
        auto stdDecodeInfo = reinterpret_cast<DecodeInfo*>(data);
        unsigned char *rets = DecodeAchieves(env, stdDecodeInfo);
        stdDecodeInfo->sinputDecoding = rets;
    }
    void Base64::EndStdDecode(napi_env env, napi_status status, void *buffer)
    {
        auto stdDecodeInfo = reinterpret_cast<DecodeInfo*>(buffer);
        void *data = nullptr;
        napi_handle_scope scope = nullptr;
        napi_open_handle_scope(env, &scope);
        if (scope == nullptr) {
            return;
        }
        napi_value arrayBuffer = nullptr;
        size_t bufferSize = stdDecodeInfo->decodeOutLen;
        napi_create_arraybuffer(env, bufferSize, &data, &arrayBuffer);
        if (memcpy_s(data, bufferSize,
            reinterpret_cast<const void*>(stdDecodeInfo->sinputDecoding), bufferSize) != EOK) {
            HILOG_ERROR("Base64:: copy ret to arraybuffer error");
            int32_t errCode = 401; // 401：errCode
            const char* errMessage =
                "Parameter error. The type of the parameter must be a string and must be valid and legal";
            napi_value error = Tools::ErrorHelper::CreateError(env, errCode, errMessage);
            napi_reject_deferred(env, stdDecodeInfo->deferred, error);
            napi_delete_async_work(env, stdDecodeInfo->worker);
            napi_close_handle_scope(env, scope);
            return;
        }
        napi_value result = nullptr;
        napi_create_typedarray(env, napi_uint8_array, bufferSize, arrayBuffer, 0, &result);
        napi_resolve_deferred(env, stdDecodeInfo->deferred, result);
        napi_delete_async_work(env, stdDecodeInfo->worker);
        napi_close_handle_scope(env, scope);
        delete[] stdDecodeInfo->sinputDecoding;
        delete stdDecodeInfo;
    }

    /* Memory cleanup function */
    void FreeMemory(char *&address)
    {
        if (address != nullptr) {
            delete[] address;
            address = nullptr;
        }
    }
    void FreeMemory(unsigned char *&address)
    {
        if (address != nullptr) {
            delete[] address;
            address = nullptr;
        }
    }
}
