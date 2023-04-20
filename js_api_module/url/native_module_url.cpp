 /*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "js_url.h"
#include "securec.h"
#include "utils/log.h"

extern const char _binary_js_url_js_start[];
extern const char _binary_js_url_js_end[];
extern const char _binary_url_abc_start[];
extern const char _binary_url_abc_end[];
namespace OHOS::Url {
    static void UrlStructor(napi_env &env, napi_callback_info &info, URL *&object)
    {
        napi_value thisVar = nullptr;
        size_t argc = 2; // 2:The number of parameters is 2
        napi_value argv[2] = { 0 }; // 2:The number of parameters is 2
        void *data = nullptr;
        napi_get_cb_info(env, info, &argc, nullptr, &thisVar, &data);
        napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
        napi_valuetype valuetype1 = napi_null;
        napi_valuetype valuetype2 = napi_null;
        napi_typeof(env, argv[0], &valuetype1);
        if (valuetype1 == napi_string) {
            std::string temp, tempType = "";
            size_t tempSize, tempTypeSize = 0;
            if (napi_get_value_string_utf8(env, argv[0], nullptr, 0, &tempSize) != napi_ok) {
                HILOG_ERROR("can not get argv[0] size");
                return;
            }
            temp.reserve(tempSize);
            temp.resize(tempSize);
            if (napi_get_value_string_utf8(env, argv[0], temp.data(), tempSize + 1, &tempSize) != napi_ok) {
                HILOG_ERROR("can not get argv[0] value");
                return;
            }
            std::string input = temp;
            napi_typeof(env, argv[1], &valuetype2);
            if (valuetype2 == napi_string) {
                if (napi_get_value_string_utf8(env, argv[1], nullptr, 0, &tempTypeSize) != napi_ok) {
                    HILOG_ERROR("can not get argv[1] size");
                    return;
                }
                tempType.reserve(tempTypeSize);
                tempType.resize(tempTypeSize);
                if (napi_get_value_string_utf8(env, argv[1], tempType.data(),
                                               tempTypeSize + 1, &tempTypeSize) != napi_ok) {
                    HILOG_ERROR("can not get argv[1] value");
                    return;
                }
                std::string base = tempType;
                object = new URL(input, base);
            } else if (valuetype2 == napi_object) {
                URL *tempUrl = nullptr;
                napi_unwrap(env, argv[1], reinterpret_cast<void**>(&tempUrl));
                object = new URL(input, *tempUrl);
            } else {
                HILOG_INFO("secondParameter error");
            }
        } else {
            HILOG_INFO("firstParameter error");
        }
        return;
    }

    static napi_value UrlConstructor(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        void *data = nullptr;
        size_t argc = 0;
        napi_value argv[2] = { 0 }; // 2:The number of parameters is 2
        URL *object = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, nullptr, &thisVar, &data));
        if (argc == 1) {
            NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
            napi_valuetype valuetype = napi_null;
            NAPI_CALL(env, napi_typeof(env, argv[0], &valuetype));
            if (valuetype == napi_string) {
                std::string type = "";
                size_t typeSize = 0;
                if (napi_get_value_string_utf8(env, argv[0], nullptr, 0, &typeSize) != napi_ok) {
                    HILOG_ERROR("can not get argv[0] size");
                    return nullptr;
                }
                type.reserve(typeSize);
                type.resize(typeSize);
                if (napi_get_value_string_utf8(env, argv[0], type.data(), typeSize + 1, &typeSize) != napi_ok) {
                    HILOG_ERROR("can not get argv[0] value");
                    return nullptr;
                }
                std::string input = type;
                object = new URL(input);
            } else {
                HILOG_INFO("Parameter error");
            }
        } else if (argc == 2) { // 2:When the input parameter is set to 2
            UrlStructor(env, info, object);
        }
        napi_wrap(
            env, thisVar, object,
            [](napi_env environment, void *data, void *hint) {
                auto obj = reinterpret_cast<URL*>(data);
                if (obj != nullptr) {
                    delete obj;
                }
            },
            nullptr, nullptr);
        return thisVar;
    }

    static napi_value GetHostname(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        URL *murl = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&murl)));
        napi_value retVal = murl->GetHostname(env);
        return retVal;
    }

    static napi_value GetSearch(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        URL *murl = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&murl)));
        napi_value retVal = murl->GetSearch(env);
        return retVal;
    }

    static napi_value GetUsername(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        URL *murl = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&murl)));
        napi_value retVal = murl->GetUsername(env);
        return retVal;
    }

    static napi_value GetPassword(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        URL *murl = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&murl)));
        napi_value retVal = murl->GetPassword(env);
        return retVal;
    }

    static napi_value GetUrlFragment(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        URL *murl = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&murl)));
        napi_value retVal = murl->GetFragment(env);
        return retVal;
    }

    static napi_value GetUrlScheme(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        URL *murl = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&murl)));
        napi_value retVal = murl->GetScheme(env);
        return retVal;
    }

    static napi_value GetUrlPort(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        URL *murl = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&murl)));
        napi_value retVal = murl->GetPort(env);
        return retVal;
    }

    static napi_value GetUrlHost(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        URL *murl = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&murl)));
        napi_value retVal = murl->GetHost(env);
        return retVal;
    }

    static napi_value GetUrlPath(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        URL *murl = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&murl)));
        napi_value retVal = murl->GetPath(env);
        return retVal;
    }

    static napi_value GetOnOrOff(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        URL *murl = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&murl)));
        napi_value retVal = murl->GetOnOrOff(env);
        return retVal;
    }

    static napi_value GetIsIpv6(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        URL *murl = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&murl)));
        napi_value retVal = murl->GetIsIpv6(env);
        return retVal;
    }

    static napi_value SetHref(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value argv[1] = {0};
        size_t argc = 1;
        std::string input = "";
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
        size_t typelen = 0;
        if (napi_get_value_string_utf8(env, argv[0], nullptr, 0, &typelen) != napi_ok) {
            HILOG_ERROR("can not get argv[0] size");
            return nullptr;
        }
        input.resize(typelen);
        if (napi_get_value_string_utf8(env, argv[0], input.data(), typelen + 1, &typelen) != napi_ok) {
            HILOG_ERROR("can not get argv[0] value");
            return nullptr;
        }
        URL *murl = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&murl)));
        murl->SetHref(input);
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &result));
        return result;
    }

    static napi_value SetHostname(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value argv[1] = {0};
        size_t argc = 1;
        std::string input = "";
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
        size_t typelen = 0;
        if (napi_get_value_string_utf8(env, argv[0], nullptr, 0, &typelen) != napi_ok) {
            HILOG_ERROR("can not get argv[0] size");
            return nullptr;
        }
        input.resize(typelen);
        if (napi_get_value_string_utf8(env, argv[0], input.data(), typelen + 1, &typelen) != napi_ok) {
            HILOG_ERROR("can not get argv[0] value");
            return nullptr;
        }
        URL *murl = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&murl)));
        murl->SetHostname(input);
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &result));
        return result;
    }

    static napi_value SetUrlPort(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value argv[1] = {0};
        size_t argc = 1;
        std::string input = "";
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
        size_t typelen = 0;
        if (napi_get_value_string_utf8(env, argv[0], nullptr, 0, &typelen) != napi_ok) {
            HILOG_ERROR("can not get argv[0] size");
            return nullptr;
        }
        input.resize(typelen);
        if (napi_get_value_string_utf8(env, argv[0], input.data(), typelen + 1, &typelen) != napi_ok) {
            HILOG_ERROR("can not get argv[0] value");
            return nullptr;
        }
        URL *murl = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&murl)));
        murl->SetPort(input);
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &result));
        return result;
    }

    static napi_value SetUrlHost(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value argv[1] = {0};
        size_t argc = 1;
        std::string input = "";
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
        size_t typelen = 0;
        if (napi_get_value_string_utf8(env, argv[0], nullptr, 0, &typelen) != napi_ok) {
            HILOG_ERROR("can not get argv[0] size");
            return nullptr;
        }
        input.resize(typelen);
        if (napi_get_value_string_utf8(env, argv[0], input.data(), typelen + 1, &typelen) != napi_ok) {
            HILOG_ERROR("can not get argv[0] value");
            return nullptr;
        }
        URL *murl = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&murl)));
        murl->SetHost(input);
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &result));
        return result;
    }

    static napi_value SetSearch(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value argv[1] = {0};
        size_t argc = 1;
        std::string input = "";
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
        size_t typelen = 0;
        if (napi_get_value_string_utf8(env, argv[0], nullptr, 0, &typelen) != napi_ok) {
            HILOG_ERROR("can not get argv[0] size");
            return nullptr;
        }
        input.resize(typelen);
        if (napi_get_value_string_utf8(env, argv[0], input.data(), typelen + 1, &typelen) != napi_ok) {
            HILOG_ERROR("can not get argv[0] value");
            return nullptr;
        }
        URL *murl = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&murl)));
        murl->SetSearch(input);
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &result));
        return result;
    }

    static napi_value SetUrlScheme(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value argv[1] = {0};
        size_t argc = 1;
        std::string input = "";
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
        size_t typelen = 0;
        if (napi_get_value_string_utf8(env, argv[0], nullptr, 0, &typelen) != napi_ok) {
            HILOG_ERROR("can not get argv[0] size");
            return nullptr;
        }
        input.resize(typelen);
        if (napi_get_value_string_utf8(env, argv[0], input.data(), typelen + 1, &typelen) != napi_ok) {
            HILOG_ERROR("can not get argv[0] value");
            return nullptr;
        }
        URL *murl = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&murl)));
        murl->SetScheme(input);
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &result));
        return result;
    }

    static napi_value SetUrlFragment(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value argv[1] = {0};
        size_t argc = 1;
        std::string input = "";
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
        size_t typelen = 0;
        if (napi_get_value_string_utf8(env, argv[0], nullptr, 0, &typelen) != napi_ok) {
            HILOG_ERROR("can not get argv[0] size");
            return nullptr;
        }
        input.resize(typelen);
        if (napi_get_value_string_utf8(env, argv[0], input.data(), typelen + 1, &typelen) != napi_ok) {
            HILOG_ERROR("can not get argv[0] value");
            return nullptr;
        }
        URL *murl = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&murl)));
        murl->SetFragment(input);
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &result));
        return result;
    }

    static napi_value SetUsername(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value argv[1] = {0};
        size_t argc = 1;
        std::string input = "";
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
        size_t typelen = 0;
        if (napi_get_value_string_utf8(env, argv[0], nullptr, 0, &typelen) != napi_ok) {
            HILOG_ERROR("can not get argv[0] size");
            return nullptr;
        }
        input.resize(typelen);
        if (napi_get_value_string_utf8(env, argv[0], input.data(), typelen + 1, &typelen) != napi_ok) {
            HILOG_ERROR("can not get argv[0] value");
            return nullptr;
        }
        URL *murl = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&murl)));
        murl->SetUsername(input);
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &result));
        return result;
    }

    static napi_value SetUrlPath(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value argv[1] = {0};
        size_t argc = 1;
        std::string input = "";
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
        size_t typelen = 0;
        if (napi_get_value_string_utf8(env, argv[0], nullptr, 0, &typelen) != napi_ok) {
            HILOG_ERROR("can not get argv[0] size");
            return nullptr;
        }
        input.resize(typelen);
        if (napi_get_value_string_utf8(env, argv[0], input.data(), typelen + 1, &typelen) != napi_ok) {
            HILOG_ERROR("can not get argv[0] value");
            return nullptr;
        }
        URL *murl = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&murl)));
        murl->SetPath(input);
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &result));
        return result;
    }

    static napi_value SetPassword(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value argv[1] = {0};
        size_t argc = 1;
        std::string input = "";
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
        size_t typelen = 0;
        if (napi_get_value_string_utf8(env, argv[0], nullptr, 0, &typelen) != napi_ok) {
            HILOG_ERROR("can not get argv[0] size");
            return nullptr;
        }
        input.resize(typelen);
        if (napi_get_value_string_utf8(env, argv[0], input.data(), typelen + 1, &typelen) != napi_ok) {
            HILOG_ERROR("can not get argv[0] value");
            return nullptr;
        }
        URL *murl = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&murl)));
        murl->SetPassword(input);
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &result));
        return result;
    }

    static napi_value SeachParamsConstructor(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        void *data = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, &data));
        auto object = new URLSearchParams();
        napi_wrap(
            env, thisVar, object,
            [](napi_env environment, void *data, void *hint) {
                auto obj = reinterpret_cast<URLSearchParams*>(data);
                if (obj != nullptr) {
                    delete obj;
                }
            },
            nullptr, nullptr);
        return thisVar;
    }

    static napi_value SetArray(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value argv[1] = {0};
        size_t argc = 1;
        uint32_t length = 0;
        napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
        napi_get_array_length(env, argv[0], &length);
        std::vector<std::string> vec;
        size_t arraySize = 0;
        napi_value napiStr = nullptr;
        for (size_t i = 0; i < length; i++) {
            napi_get_element(env, argv[0], i, &napiStr);
            if (napi_get_value_string_utf8(env, napiStr, nullptr, 0, &arraySize) != napi_ok) {
                HILOG_ERROR("can not get napiStr size");
                return nullptr;
            }
            if (arraySize > 0) {
                std::string cstr = "";
                cstr.resize(arraySize);
                if (napi_get_value_string_utf8(env, napiStr, cstr.data(), arraySize + 1, &arraySize) != napi_ok) {
                    HILOG_ERROR("can not get name value");
                    return nullptr;
                }
                vec.push_back(cstr);
            } else {
                vec.push_back("");
            }
        }
        URLSearchParams *murl = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&murl)));
        murl->SetArray(env, vec);
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &result));
        return result;
    }

    static napi_value GetArray(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        URLSearchParams *murl = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&murl)));
        napi_value retVal = murl->GetArray(env);
        return retVal;
    }

    static napi_value Get(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 1;
        napi_value args = nullptr;
        napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr);
        if (argc != 1) {
            HILOG_INFO("One arg needs to be specified");
            return nullptr;
        }
        URLSearchParams *object = nullptr;
        napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object));
        napi_value result = object->Get(env, args);
        return result;
    }

    static napi_value GetAll(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 1;
        napi_value args = nullptr;
        napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr);
        if (argc != 1) {
            HILOG_INFO("One arg needs to be specified");
            return nullptr;
        }
        URLSearchParams *object = nullptr;
        napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object));
        napi_value result = object->GetAll(env, args);
        return result;
    }

    static napi_value Append(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 2; // 2:The number of parameters is 2
        napi_value args[2] = { 0 }; // 2:The number of parameters is 2
        void *data = nullptr;
        napi_get_cb_info(env, info, &argc, args, &thisVar, &data);
        if (argc != 2) { // 2:If the input parameter is not set to 2,
            HILOG_INFO("Two args needs to be specified");
            return nullptr;
        }
        URLSearchParams *object = nullptr;
        napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object));
        object->Append(env, args[0], args[1]);
        return nullptr;
    }

    static napi_value Delete(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 1;
        napi_value args = nullptr;
        napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr);
        if (argc != 1) {
            HILOG_INFO("One arg needs to be specified");
            return nullptr;
        }
        URLSearchParams *object = nullptr;
        napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object));
        object->Delete(env, args);
        return nullptr;
    }

    static napi_value Entries(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 0;
        napi_value args = nullptr;
        napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr);
        URLSearchParams *object = nullptr;
        napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object));
        napi_value result = object->Entries(env);
        return result;
    }

    static napi_value IsHas(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        URLSearchParams *object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object)));
        napi_value result = object->IsHas(env, args);
        return result;
    }

    static napi_value Set(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 2; // 2:The number of parameters is 2
        napi_value args[2] = { 0 }; // 2:The number of parameters is 2
        napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr);
        URLSearchParams *object = nullptr;
        napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object));
        object->Set(env, args[0], args[1]);
        return nullptr;
    }

    static napi_value Sort(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 0;
        napi_value args = nullptr;
        napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr);
        URLSearchParams *object = nullptr;
        napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object));
        object->Sort();
        return nullptr;
    }

    static napi_value IterByKeys(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 0;
        napi_value args = nullptr;
        napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr);
        URLSearchParams *object = nullptr;
        napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object));
        napi_value result = object->IterByKeys(env);
        return result;
    }

    static napi_value IterByValues(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 0;
        napi_value args = nullptr;
        napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr);
        URLSearchParams *object = nullptr;
        napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object));
        napi_value result = object->IterByValues(env);
        return result;
    }

    static void IsPlusSign(size_t &strLastPos, const size_t &iteaor, std::string &buf, std::string &stringParm)
    {
        if (strLastPos < iteaor) {
            buf += stringParm.substr(strLastPos, iteaor - strLastPos + 1);
        } else {
            buf += "";
        }
        strLastPos = iteaor + 1;
        return;
    }

    static void IsEqualSign(size_t &strLastPos, const size_t &iteaor,
        std::string &buf, std::string &stringParm, std::vector<std::string> &seachParasVec)
    {
        if (strLastPos < iteaor) {
            buf += stringParm.substr(strLastPos, iteaor - strLastPos);
        }
        seachParasVec.push_back(buf);
        buf = "";
        strLastPos = iteaor + 1;
        return;
    }

    static void IsAddressSign(const size_t &strLastPos, const size_t &iteaor, std::string &buf,
        std::string &stringParm, std::vector<std::string> &seachParasVec)
    {
        if (strLastPos < iteaor) {
            buf += stringParm.substr(strLastPos, iteaor - strLastPos);
        }
        seachParasVec.push_back(buf);
        return;
    }
    static void DealParmsString(const size_t &strLastPos, const size_t &iteaor, std::string &buf,
        std::string &stringParm, std::vector<std::string> &seachParasVec)
    {
        if (strLastPos < iteaor) {
            buf += stringParm.substr(strLastPos, iteaor - strLastPos);
        }
        seachParasVec.push_back(buf);
    }
    static void IsEqualCode(size_t &strStartPos, const size_t &iteaor, size_t &strLastPos)
    {
        if (strStartPos == iteaor) {
            strLastPos = iteaor + 1;
            strStartPos = iteaor + 1;
        }
        return;
    }
    static std::vector<std::string> StringParsing(std::string stringParm)
    {
        std::vector<std::string> seachParasVec;
        size_t strStartPos = 0;
        size_t strLastPos = 0;
        bool isHasSpace = false;
        std::string buf = "";
        size_t iteaor = 0;
        for (iteaor = 0; iteaor < stringParm.length(); iteaor++) {
            char code = stringParm[iteaor];
            switch (code) {
                case '&':
                    {
                        IsEqualCode(strStartPos, iteaor, strLastPos);
                        IsAddressSign(strLastPos, iteaor, buf, stringParm, seachParasVec);
                        if (!isHasSpace) {
                            seachParasVec.push_back("");
                        }
                        isHasSpace = false;
                        buf = "";
                        strLastPos = iteaor + 1;
                        strStartPos = iteaor + 1;
                        break;
                    }
                case '=':
                    {
                        if (isHasSpace) {
                            break;
                        }
                        IsEqualSign(strLastPos, iteaor, buf, stringParm, seachParasVec);
                        isHasSpace = true;
                        break;
                    }
                case '+':
                    IsPlusSign(strLastPos, iteaor, buf, stringParm);
                    break;
                default:break;
            }
        }
        if (strStartPos == iteaor) {
            return seachParasVec;
        }
        DealParmsString(strLastPos, iteaor, buf, stringParm, seachParasVec);
        if (!isHasSpace) {
            seachParasVec.push_back("");
        }
        return seachParasVec;
    }

    static napi_value StringParmas(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value argv[1] = {0};
        size_t argc = 1;
        std::string input = "";
        napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
        size_t typelen = 0;
        if (napi_get_value_string_utf8(env, argv[0], nullptr, 0, &typelen) != napi_ok) {
            HILOG_ERROR("can not get argv[0] size");
            return nullptr;
        }
        input.resize(typelen);
        if (napi_get_value_string_utf8(env, argv[0], input.data(), typelen + 1, &typelen) != napi_ok) {
            HILOG_ERROR("can not get argv[0] value");
            return nullptr;
        }
        std::vector<std::string> seachParasmsString;
        seachParasmsString = StringParsing(input);
        napi_value arr = nullptr;
        napi_create_array(env, &arr);
        for (size_t i = 0; i < seachParasmsString.size(); i++) {
            napi_value result = nullptr;
            napi_create_string_utf8(env, seachParasmsString[i].c_str(), seachParasmsString[i].size(), &result);
            napi_set_element(env, arr, i, result);
        }
        return arr;
    }

    static napi_value SeachParamsInit(napi_env env, napi_value exports)
    {
        const char *seachParamsClassName = "URLSearchParams";
        napi_value seachParamsInitClass = nullptr;
        napi_property_descriptor UrlDesc[] = {
            DECLARE_NAPI_FUNCTION("has", IsHas),
            DECLARE_NAPI_FUNCTION("set", Set),
            DECLARE_NAPI_FUNCTION("sort", Sort),
            DECLARE_NAPI_FUNCTION("keys", IterByKeys),
            DECLARE_NAPI_FUNCTION("values", IterByValues),
            DECLARE_NAPI_FUNCTION("get", Get),
            DECLARE_NAPI_FUNCTION("getAll", GetAll),
            DECLARE_NAPI_FUNCTION("append", Append),
            DECLARE_NAPI_FUNCTION("delete", Delete),
            DECLARE_NAPI_FUNCTION("entries", Entries),
            DECLARE_NAPI_GETTER_SETTER("array", GetArray, SetArray),
        };
        NAPI_CALL(env, napi_define_class(env, seachParamsClassName, strlen(seachParamsClassName),
            SeachParamsConstructor, nullptr, sizeof(UrlDesc) / sizeof(UrlDesc[0]),
            UrlDesc, &seachParamsInitClass));
        napi_property_descriptor desc[] = {
            DECLARE_NAPI_PROPERTY("URLSearchParams1", seachParamsInitClass)
        };
        napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
        return exports;
    };

    static napi_value ParamsInit(napi_env env, napi_value exports)
    {
        const char *paramsClassName = "URLSearchParams";
        napi_value ParamsInitClass = nullptr;
        napi_property_descriptor UrlDesc[] = {
            DECLARE_NAPI_FUNCTION("has", IsHas),
            DECLARE_NAPI_FUNCTION("set", Set),
            DECLARE_NAPI_FUNCTION("sort", Sort),
            DECLARE_NAPI_FUNCTION("keys", IterByKeys),
            DECLARE_NAPI_FUNCTION("values", IterByValues),
            DECLARE_NAPI_FUNCTION("get", Get),
            DECLARE_NAPI_FUNCTION("getAll", GetAll),
            DECLARE_NAPI_FUNCTION("append", Append),
            DECLARE_NAPI_FUNCTION("delete", Delete),
            DECLARE_NAPI_FUNCTION("entries", Entries),
            DECLARE_NAPI_GETTER_SETTER("array", GetArray, SetArray),
        };
        NAPI_CALL(env, napi_define_class(env, paramsClassName, strlen(paramsClassName),
            SeachParamsConstructor, nullptr, sizeof(UrlDesc) / sizeof(UrlDesc[0]),
            UrlDesc, &ParamsInitClass));
        napi_property_descriptor desc[] = {
            DECLARE_NAPI_PROPERTY("URLParams1", ParamsInitClass)
        };
        napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
        return exports;
    };

    static napi_value UrlInit(napi_env env, napi_value exports)
    {
        const char *urlClassName = "Url";
        napi_value urlClass = nullptr;
        napi_property_descriptor UrlDesc[] = {
            DECLARE_NAPI_GETTER_SETTER("hostname", GetHostname, SetHostname),
            DECLARE_NAPI_FUNCTION("href", SetHref),
            DECLARE_NAPI_GETTER_SETTER("search", GetSearch, SetSearch),
            DECLARE_NAPI_GETTER_SETTER("username", GetUsername, SetUsername),
            DECLARE_NAPI_GETTER_SETTER("password", GetPassword, SetPassword),
            DECLARE_NAPI_GETTER_SETTER("host", GetUrlHost, SetUrlHost),
            DECLARE_NAPI_GETTER_SETTER("hash", GetUrlFragment, SetUrlFragment),
            DECLARE_NAPI_GETTER_SETTER("protocol", GetUrlScheme, SetUrlScheme),
            DECLARE_NAPI_GETTER_SETTER("pathname", GetUrlPath, SetUrlPath),
            DECLARE_NAPI_GETTER_SETTER("port", GetUrlPort, SetUrlPort),
            DECLARE_NAPI_GETTER("onOrOff", GetOnOrOff),
            DECLARE_NAPI_GETTER("GetIsIpv6", GetIsIpv6),
        };
        NAPI_CALL(env, napi_define_class(env, urlClassName, strlen(urlClassName), UrlConstructor,
                                         nullptr, sizeof(UrlDesc) / sizeof(UrlDesc[0]), UrlDesc, &urlClass));
        napi_property_descriptor desc[] = {
            DECLARE_NAPI_PROPERTY("Url", urlClass)
        };
        napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
        return exports;
    }

    static napi_value Init(napi_env env, napi_value exports)
    {
        napi_property_descriptor desc[] = {
            DECLARE_NAPI_FUNCTION("stringParmas", StringParmas),
        };
        NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
        SeachParamsInit(env, exports);
        ParamsInit(env, exports);
        UrlInit(env, exports);
        return exports;
    }


    static napi_module UrlModule = {
        .nm_version = 1,
        .nm_flags = 0,
        .nm_filename = nullptr,
        .nm_register_func = Init,
        .nm_modname = "url",
        .nm_priv = reinterpret_cast<void*>(0),
        .reserved = {0},
    };
    extern "C" __attribute__((constructor)) void RegisterModule()
    {
        napi_module_register(&UrlModule);
    }
    extern "C"
    __attribute__((visibility("default"))) void NAPI_url_GetJSCode(const char **buf, int *bufLen)
    {
        if (buf != nullptr) {
            *buf = _binary_js_url_js_start;
        }
        if (bufLen != nullptr) {
            *bufLen = _binary_js_url_js_end - _binary_js_url_js_start;
        }
    }
    extern "C"
    __attribute__((visibility("default"))) void NAPI_url_GetABCCode(const char** buf, int* buflen)
    {
        if (buf != nullptr) {
            *buf = _binary_url_abc_start;
        }
        if (buflen != nullptr) {
            *buflen = _binary_url_abc_end - _binary_url_abc_start;
        }
    }
} // namespace
