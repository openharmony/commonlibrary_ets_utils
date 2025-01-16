 /*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "native_module_url.h"
#include "tools/log.h"
#include "url_helper.h"

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
            std::string temp = "";
            std::string tempType = "";
            size_t tempSize = 0;
            size_t tempTypeSize = 0;
            if (napi_get_value_string_utf8(env, argv[0], nullptr, 0, &tempSize) != napi_ok) {
                HILOG_ERROR("UrlStructor:: can not get argv[0] size");
                return;
            }
            temp.reserve(tempSize);
            temp.resize(tempSize);
            if (napi_get_value_string_utf8(env, argv[0], temp.data(), tempSize + 1, &tempSize) != napi_ok) {
                HILOG_ERROR("UrlStructor:: can not get argv[0] value");
                return;
            }
            std::string input = temp;
            napi_typeof(env, argv[1], &valuetype2);
            if (valuetype2 == napi_string) {
                if (napi_get_value_string_utf8(env, argv[1], nullptr, 0, &tempTypeSize) != napi_ok) {
                    HILOG_ERROR("UrlStructor:: can not get argv[1] size");
                    return;
                }
                tempType.reserve(tempTypeSize);
                tempType.resize(tempTypeSize);
                if (napi_get_value_string_utf8(env, argv[1], tempType.data(),
                                               tempTypeSize + 1, &tempTypeSize) != napi_ok) {
                    HILOG_ERROR("UrlStructor:: can not get argv[1] value");
                    return;
                }
                std::string base = tempType;
                object = new (std::nothrow) URL(input, base);
                if (object == nullptr) {
                    HILOG_ERROR("UrlStructor:: memory allocation failed, object is nullptr");
                    return;
                }
            } else if (valuetype2 == napi_object) {
                URL *tempUrl = nullptr;
                napi_unwrap(env, argv[1], reinterpret_cast<void**>(&tempUrl));
                if (tempUrl == nullptr) {
                    HILOG_ERROR("UrlStructor:: tempUrl is nullptr");
                    return;
                }
                object = new (std::nothrow) URL(input, *tempUrl);
                if (object == nullptr) {
                    HILOG_ERROR("UrlStructor:: memory allocation failed, object is nullptr");
                    return;
                }
            } else {
                HILOG_INFO("UrlStructor:: secondParameter error");
            }
        } else {
            HILOG_INFO("UrlStructor:: firstParameter error");
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
                    HILOG_ERROR("UrlStructor:: can not get argv[0] size");
                    return nullptr;
                }
                type.reserve(typeSize);
                type.resize(typeSize);
                if (napi_get_value_string_utf8(env, argv[0], type.data(), typeSize + 1, &typeSize) != napi_ok) {
                    HILOG_ERROR("UrlStructor:: can not get argv[0] value");
                    return nullptr;
                }
                std::string input = type;
                object = new (std::nothrow) URL(input);
                if (object == nullptr) {
                    HILOG_ERROR("UrlStructor:: memory allocation failed, object is nullptr");
                    return nullptr;
                }
            } else {
                HILOG_INFO("UrlStructor:: Parameter error");
            }
        } else if (argc == 2) { // 2:When the input parameter is set to 2
            UrlStructor(env, info, object);
        }
        napi_status status = napi_wrap(env, thisVar, object,
            [](napi_env environment, void *data, void *hint) {
                auto obj = reinterpret_cast<URL*>(data);
                if (obj != nullptr) {
                    delete obj;
                    obj = nullptr;
                }
            }, nullptr, nullptr);
        if (status != napi_ok && object != nullptr) {
            delete object;
            object = nullptr;
        }
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

    static napi_value GetEncodeSearch(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        if (napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr) != napi_ok) {
            HILOG_ERROR("URL:: can not get thisVar");
            return nullptr;
        }
        URL *murl = nullptr;
        if (napi_unwrap(env, thisVar, reinterpret_cast<void**>(&murl)) != napi_ok) {
            HILOG_ERROR("URL:: can not get murl");
            return nullptr;
        }
        napi_value retVal = murl->GetEncodeSearch(env);
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
            HILOG_ERROR("URL:: can not get argv[0] size");
            return nullptr;
        }
        input.resize(typelen);
        if (napi_get_value_string_utf8(env, argv[0], input.data(), typelen + 1, &typelen) != napi_ok) {
            HILOG_ERROR("URL:: can not get argv[0] value");
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
            HILOG_ERROR("URL:: can not get argv[0] size");
            return nullptr;
        }
        input.resize(typelen);
        if (napi_get_value_string_utf8(env, argv[0], input.data(), typelen + 1, &typelen) != napi_ok) {
            HILOG_ERROR("URL:: can not get argv[0] value");
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
            HILOG_ERROR("URL:: can not get argv[0] size");
            return nullptr;
        }
        input.resize(typelen);
        if (napi_get_value_string_utf8(env, argv[0], input.data(), typelen + 1, &typelen) != napi_ok) {
            HILOG_ERROR("URL:: can not get argv[0] value");
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
            HILOG_ERROR("URL:: can not get argv[0] size");
            return nullptr;
        }
        input.resize(typelen);
        if (napi_get_value_string_utf8(env, argv[0], input.data(), typelen + 1, &typelen) != napi_ok) {
            HILOG_ERROR("URL:: can not get argv[0] value");
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
            HILOG_ERROR("URL:: can not get argv[0] size");
            return nullptr;
        }
        input.resize(typelen);
        if (napi_get_value_string_utf8(env, argv[0], input.data(), typelen + 1, &typelen) != napi_ok) {
            HILOG_ERROR("URL:: can not get argv[0] value");
            return nullptr;
        }
        URL *murl = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&murl)));
        murl->SetSearch(input);
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &result));
        return result;
    }

    static napi_value SetEncodeSearch(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value argv[1] = {0};
        size_t argc = 1;
        std::string input = "";
        if (napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr) != napi_ok) {
            HILOG_ERROR("URL:: can not get thisVar");
            return nullptr;
        }
        size_t typelen = 0;
        if (napi_get_value_string_utf8(env, argv[0], nullptr, 0, &typelen) != napi_ok) {
            HILOG_ERROR("URL:: can not get argv[0] size");
            return nullptr;
        }
        input.resize(typelen);
        if (napi_get_value_string_utf8(env, argv[0], input.data(), typelen + 1, &typelen) != napi_ok) {
            HILOG_ERROR("URL:: can not get argv[0] value");
            return nullptr;
        }
        URL *murl = nullptr;
        if (napi_unwrap(env, thisVar, reinterpret_cast<void**>(&murl)) != napi_ok) {
            HILOG_ERROR("URL:: can not get url");
            return nullptr;
        }
        murl->SetEncodeSearch(input);
        napi_value result = nullptr;
        if (napi_get_undefined(env, &result) != napi_ok) {
            HILOG_ERROR("URL:: can not get result");
            return nullptr;
        }
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
            HILOG_ERROR("URL:: can not get argv[0] size");
            return nullptr;
        }
        input.resize(typelen);
        if (napi_get_value_string_utf8(env, argv[0], input.data(), typelen + 1, &typelen) != napi_ok) {
            HILOG_ERROR("URL:: can not get argv[0] value");
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
            HILOG_ERROR("URL:: can not get argv[0] size");
            return nullptr;
        }
        input.resize(typelen);
        if (napi_get_value_string_utf8(env, argv[0], input.data(), typelen + 1, &typelen) != napi_ok) {
            HILOG_ERROR("URL:: can not get argv[0] value");
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
            HILOG_ERROR("URL:: can not get argv[0] size");
            return nullptr;
        }
        input.resize(typelen);
        if (napi_get_value_string_utf8(env, argv[0], input.data(), typelen + 1, &typelen) != napi_ok) {
            HILOG_ERROR("URL:: can not get argv[0] value");
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
            HILOG_ERROR("URL:: can not get argv[0] size");
            return nullptr;
        }
        input.resize(typelen);
        if (napi_get_value_string_utf8(env, argv[0], input.data(), typelen + 1, &typelen) != napi_ok) {
            HILOG_ERROR("URL:: can not get argv[0] value");
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
            HILOG_ERROR("URL:: can not get argv[0] size");
            return nullptr;
        }
        input.resize(typelen);
        if (napi_get_value_string_utf8(env, argv[0], input.data(), typelen + 1, &typelen) != napi_ok) {
            HILOG_ERROR("URL:: can not get argv[0] value");
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
        auto object = new (std::nothrow) URLSearchParams();
        if (object == nullptr) {
            HILOG_ERROR("SeachParamsConstructor:: memory allocation failed, object is nullptr");
            return nullptr;
        }
        napi_status status = napi_wrap(env, thisVar, object,
            [](napi_env environment, void *data, void *hint) {
                auto obj = reinterpret_cast<URLSearchParams*>(data);
                if (obj != nullptr) {
                    delete obj;
                    obj = nullptr;
                }
            }, nullptr, nullptr);
        if (status != napi_ok && object != nullptr) {
            HILOG_ERROR("SeachParamsConstructor:: napi_wrap failed");
            delete object;
            object = nullptr;
        }
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
                HILOG_ERROR("URLSearchParams:: can not get napiStr size");
                return nullptr;
            }
            if (arraySize > 0) {
                std::string cstr = "";
                cstr.resize(arraySize);
                if (napi_get_value_string_utf8(env, napiStr, cstr.data(), arraySize + 1, &arraySize) != napi_ok) {
                    HILOG_ERROR("URLSearchParams:: can not get name value");
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
            HILOG_INFO("URLSearchParams:: One arg needs to be specified");
            return nullptr;
        }
        URLSearchParams *object = nullptr;
        napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object));
        if (object == nullptr) {
            return nullptr;
        }
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
            HILOG_INFO("URLSearchParams:: One arg needs to be specified");
            return nullptr;
        }
        URLSearchParams *object = nullptr;
        napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object));
        if (object == nullptr) {
            return nullptr;
        }
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
            HILOG_INFO("URLSearchParams:: Two args needs to be specified");
            return nullptr;
        }
        URLSearchParams *object = nullptr;
        napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object));
        if (object == nullptr) {
            return nullptr;
        }
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
            HILOG_INFO("URLSearchParams:: One arg needs to be specified");
            return nullptr;
        }
        URLSearchParams *object = nullptr;
        napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object));
        if (object == nullptr) {
            return nullptr;
        }
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
        if (object == nullptr) {
            return nullptr;
        }
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
        if (object == nullptr) {
            return nullptr;
        }
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
        if (object == nullptr) {
            return nullptr;
        }
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
        if (object == nullptr) {
            return nullptr;
        }
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
        if (object == nullptr) {
            return nullptr;
        }
        napi_value result = object->IterByValues(env);
        return result;
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
            HILOG_ERROR("URLSearchParams:: can not get argv[0] size");
            return nullptr;
        }
        input.resize(typelen);
        if (napi_get_value_string_utf8(env, argv[0], input.data(), typelen + 1, &typelen) != napi_ok) {
            HILOG_ERROR("URLSearchParams:: can not get argv[0] value");
            return nullptr;
        }
        std::vector<KeyValue> params{};
        StringAnalyzing(input, params);
        napi_value arr = nullptr;
        napi_create_array(env, &arr);
        size_t j = 0;
        for (size_t i = 0; i < params.size(); i++) {
            napi_value result = nullptr;
            napi_value result1 = nullptr;
            napi_create_string_utf8(env, params[i].first.c_str(), params[i].first.size(), &result);
            napi_set_element(env, arr, j, result);
            napi_create_string_utf8(env, params[i].second.c_str(), params[i].second.size(), &result1);
            napi_set_element(env, arr, j + 1, result1);
            // 2 step, j, j + 1
            j += 2;
        }
        return arr;
    }

    static napi_value FixUSVstring(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value argv[1] = {0};
        size_t argc = 1;
        char16_t* inputStr = nullptr;
        napi_value resultStr = nullptr;
        size_t inputSize = 0;
        napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
        if (napi_get_value_string_utf16(env, argv[0], nullptr, 0, &inputSize) != napi_ok) {
            HILOG_ERROR("url:: get args failed.");
            return nullptr;
        }
        if (inputSize > 0) {
            inputStr = new (std::nothrow) char16_t[inputSize + 1]();
            if (inputStr == nullptr) {
                HILOG_ERROR("url:: memory allocation failed, inputStr is nullptr");
                return resultStr;
            }
            napi_get_value_string_utf16(env, argv[0], inputStr, inputSize + 1, &inputSize);
        }
        for (size_t i = 0; i < inputSize; i++) {
            char16_t c = *(inputStr + i);
            // 0xD800: minimum value of low proxy term. 0xF800: key bit mode for dividing high proxy and low proxy.
            if (!((c & 0xF800) == 0xD800)) {
                continue;
            } else if ((c & 0x400) != 0 || i == inputSize - 1) { // 0x400: Determine is component of low proxy.
                *(inputStr + i) = 0xFFFD; // 0xFFFD: Invalid character.
            } else {
                char16_t d = *(inputStr + i + 1);
                // 0xDC00: minimum value of high proxy item. 0xFC00: Check if it meets the requirements of high proxy.
                if ((d & 0xFC00) == 0xDC00) {
                    i++;
                } else {
                    *(inputStr + i) = 0xFFFD; // 0xFFFD: Invalid character.
                }
            }
        }
        napi_create_string_utf16(env, inputStr, inputSize, &resultStr);
        if (inputStr != nullptr) {
            delete[] inputStr;
            inputStr = nullptr;
        }
        return resultStr;
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
            DECLARE_NAPI_GETTER_SETTER("encodeSearch", GetEncodeSearch, SetEncodeSearch),
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

    napi_value Init(napi_env env, napi_value exports)
    {
        napi_property_descriptor desc[] = {
            DECLARE_NAPI_FUNCTION("stringParmas", StringParmas),
            DECLARE_NAPI_FUNCTION("fixUSVstring", FixUSVstring),
        };
        NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
        SeachParamsInit(env, exports);
        ParamsInit(env, exports);
        UrlInit(env, exports);
        return exports;
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

    static napi_module_with_js UrlModule = {
        .nm_version = 1,
        .nm_flags = 0,
        .nm_filename = nullptr,
        .nm_register_func = Init,
        .nm_modname = "url",
        .nm_priv = reinterpret_cast<void*>(0),
        .nm_get_abc_code = NAPI_url_GetABCCode,
        .nm_get_js_code = NAPI_url_GetJSCode,
    };
    extern "C" __attribute__((constructor)) void UrlRegisterModule()
    {
        napi_module_with_js_register(&UrlModule);
    }
} // namespace
