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
#include "native_module_uri.h"

#include "js_uri.h"
#include "securec.h"
#include "tools/log.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

extern const char _binary_js_uri_js_start[];
extern const char _binary_js_uri_js_end[];
extern const char _binary_uri_abc_start[];
extern const char _binary_uri_abc_end[];

namespace OHOS::Uri {
    static napi_value UriConstructor(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        void *data = nullptr;
        size_t argc = 1;
        napi_value argv[1] = { 0 };
        Uri *object = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, argv[0], &valuetype));
        if (valuetype == napi_string) {
            std::string type = "";
            size_t typelen = 0;
            NAPI_CALL(env, napi_get_value_string_utf8(env, argv[0], nullptr, 0, &typelen));
            type.resize(typelen);
            NAPI_CALL(env, napi_get_value_string_utf8(env, argv[0], type.data(), typelen + 1, &typelen));
            object = new Uri(type);
        }
        NAPI_CALL(env, napi_wrap(env, thisVar, object,
            [](napi_env environment, void *data, void *hint) {
            auto obj = reinterpret_cast<Uri*>(data);
            if (obj != nullptr) {
                delete obj;
            }
        }, nullptr, nullptr));
        return thisVar;
    }

    static napi_value Normalize(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        Uri *muri = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&muri)));
        std::string normalizeUri = muri->Normalize();
        napi_value result = nullptr;
        size_t tempLen = normalizeUri.size();
        NAPI_CALL(env, napi_create_string_utf8(env, normalizeUri.c_str(), tempLen, &result));
        return result;
    }

    static napi_value Equals(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value result = nullptr;
        size_t argc = 1;
        napi_value argv[1] = { 0 };
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));

        Uri *muri = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&muri)));
        Uri *other = nullptr;
        NAPI_CALL(env, napi_unwrap(env, argv[0], reinterpret_cast<void**>(&other)));

        bool flag = muri->Equals(*other);
        NAPI_CALL(env, napi_get_boolean(env, flag, &result));
        return result;
    }

    static napi_value IsAbsolute(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        Uri *muri = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&muri)));
        bool flag = muri->IsAbsolute();
        NAPI_CALL(env, napi_get_boolean(env, flag, &result));
        return result;
    }

    static napi_value IsFailed(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        Uri *muri = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&muri)));
        std::string temp = muri->IsFailed();
        size_t templen = temp.size();
        NAPI_CALL(env, napi_create_string_utf8(env, temp.c_str(), templen, &result));
        return result;
    }

    static napi_value UriToString(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        Uri *muri = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&muri)));
        std::string temp = muri->ToString();
        size_t templen = temp.size();
        NAPI_CALL(env, napi_create_string_utf8(env, temp.c_str(), templen, &result));
        return result;
    }

    static napi_value IsRelative(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        Uri *muri = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&muri)));
        bool flag = muri->IsRelative();
        NAPI_CALL(env, napi_get_boolean(env, flag, &result));
        return result;
    }

    static napi_value IsOpaque(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        Uri *muri = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&muri)));
        bool flag = muri->IsOpaque();
        NAPI_CALL(env, napi_get_boolean(env, flag, &result));
        return result;
    }

    static napi_value IsHierarchical(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        Uri *muri = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&muri)));
        bool flag = muri->IsHierarchical();
        NAPI_CALL(env, napi_get_boolean(env, flag, &result));
        return result;
    }

    static napi_value AddQueryValue(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value result = nullptr;
        size_t argc = 2;
        napi_value argv[2] = { nullptr };
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
        Uri *muri = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&muri)));
        std::string key = "";
        size_t keyLen = 0;
        NAPI_CALL(env, napi_get_value_string_utf8(env, argv[0], nullptr, 0, &keyLen));
        key.resize(keyLen);
        NAPI_CALL(env, napi_get_value_string_utf8(env, argv[0], key.data(), keyLen + 1, &keyLen));
        std::string value = "";
        size_t valueLen = 0;
        NAPI_CALL(env, napi_get_value_string_utf8(env, argv[1], nullptr, 0, &valueLen));
        value.resize(valueLen);
        NAPI_CALL(env, napi_get_value_string_utf8(env, argv[1], value.data(), valueLen + 1, &valueLen));
        std::string temp = muri->AddQueryValue(key, value);
        if (temp.empty()) {
            napi_get_null(env, &result);
            return result;
        }
        size_t templen = temp.size();
        NAPI_CALL(env, napi_create_string_utf8(env, temp.c_str(), templen, &result));
        return result;
    }

    static napi_value AddSegment(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value result = nullptr;
        size_t argc = 1;
        napi_value argv[1] = { nullptr };
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
        Uri *muri = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&muri)));
        std::string segment = "";
        size_t segmentLen = 0;
        NAPI_CALL(env, napi_get_value_string_utf8(env, argv[0], nullptr, 0, &segmentLen));
        segment.resize(segmentLen);
        NAPI_CALL(env, napi_get_value_string_utf8(env, argv[0], segment.data(), segmentLen + 1, &segmentLen));
        std::string temp = muri->AddSegment(segment);
        if (temp.empty()) {
            napi_get_null(env, &result);
            return result;
        }
        size_t tempLen = temp.size();
        NAPI_CALL(env, napi_create_string_utf8(env, temp.c_str(), tempLen, &result));
        return result;
    }

    static napi_value GetSegment(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        Uri *muri = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&muri)));
        std::vector<std::string> temp = muri->GetSegment();
        if (temp.empty()) {
            napi_get_null(env, &result);
            return result;
        }
        napi_value segment = nullptr;
        napi_create_array(env, &result);
        size_t size = temp.size();
        for (size_t i = 0; i < size; i++) {
            napi_create_string_utf8(env, temp[i].c_str(), temp[i].length(), &segment);
            napi_set_element(env, result, i, segment);
        }
        return result;
    }

    static napi_value GetScheme(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        Uri *muri = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&muri)));
        std::string temp = muri->GetScheme();
        if (temp.empty()) {
            napi_get_null(env, &result);
            return result;
        }
        size_t templen = temp.size();
        NAPI_CALL(env, napi_create_string_utf8(env, temp.c_str(), templen, &result));
        return result;
    }

    static napi_value GetAuthority(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        Uri *muri = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&muri)));
        std::string temp = muri->GetAuthority();
        if (temp.empty()) {
            napi_get_null(env, &result);
            return result;
        }
        size_t templen = temp.size();
        NAPI_CALL(env, napi_create_string_utf8(env, temp.c_str(), templen, &result));
        return result;
    }

    static napi_value GetSsp(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        Uri *muri = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&muri)));
        std::string temp = muri->GetSsp();
        size_t templen = temp.size();
        NAPI_CALL(env, napi_create_string_utf8(env, temp.c_str(), templen, &result));
        return result;
    }

    static napi_value GetUserinfo(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        Uri *muri = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&muri)));
        std::string temp = muri->GetUserinfo();
        if (temp.empty()) {
            napi_get_null(env, &result);
            return result;
        }
        size_t templen = temp.size();
        NAPI_CALL(env, napi_create_string_utf8(env, temp.c_str(), templen, &result));
        return result;
    }

    static napi_value GetHost(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        Uri *muri = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&muri)));
        std::string temp = muri->GetHost();
        if (temp.empty()) {
            napi_get_null(env, &result);
            return result;
        }
        size_t templen = temp.size();
        NAPI_CALL(env, napi_create_string_utf8(env, temp.c_str(), templen, &result));
        return result;
    }

    static napi_value GetPort(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        Uri *muri = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&muri)));
        std::string temp = muri->GetPort();
        size_t templen = temp.size();
        NAPI_CALL(env, napi_create_string_utf8(env, temp.c_str(), templen, &result));
        return result;
    }

    static napi_value GetPath(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        Uri *muri = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&muri)));
        std::string temp = muri->GetPath();
        if (temp.empty()) {
            napi_get_null(env, &result);
            return result;
        }
        size_t templen = temp.size();
        NAPI_CALL(env, napi_create_string_utf8(env, temp.c_str(), templen, &result));
        return result;
    }

    static napi_value GetQuery(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        Uri *muri = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&muri)));
        std::string temp = muri->GetQuery();
        if (temp.empty()) {
            napi_get_null(env, &result);
            return result;
        }
        size_t templen = temp.size();
        NAPI_CALL(env, napi_create_string_utf8(env, temp.c_str(), templen, &result));
        return result;
    }

    static napi_value GetFragment(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        Uri *muri = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&muri)));
        std::string temp = muri->GetFragment();
        if (temp.empty()) {
            napi_get_null(env, &result);
            return result;
        }
        size_t templen = temp.size();
        NAPI_CALL(env, napi_create_string_utf8(env, temp.c_str(), templen, &result));
        return result;
    }

    static napi_value ClearQuery(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        Uri *muri = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&muri)));
        std::string temp = muri->ClearQuery();
        if (temp.empty()) {
            napi_get_null(env, &result);
            return result;
        }
        size_t tempLen = temp.size();
        NAPI_CALL(env, napi_create_string_utf8(env, temp.c_str(), tempLen, &result));
        return result;
    }

    napi_value UriInit(napi_env env, napi_value exports)
    {
        const char *uriClassName = "uri";
        napi_value uriClass = nullptr;
        napi_property_descriptor uriDesc[] = {
            DECLARE_NAPI_FUNCTION("normalize", Normalize),
            DECLARE_NAPI_FUNCTION("equals", Equals),
            DECLARE_NAPI_FUNCTION("checkIsAbsolute", IsAbsolute),
            DECLARE_NAPI_FUNCTION("toString", UriToString),
            DECLARE_NAPI_FUNCTION("checkIsRelative", IsRelative),
            DECLARE_NAPI_FUNCTION("checkIsOpaque", IsOpaque),
            DECLARE_NAPI_FUNCTION("checkIsHierarchical", IsHierarchical),
            DECLARE_NAPI_FUNCTION("addQueryValue", AddQueryValue),
            DECLARE_NAPI_FUNCTION("getSegment", GetSegment),
            DECLARE_NAPI_FUNCTION("addSegment", AddSegment),
            DECLARE_NAPI_FUNCTION("clearQuery", ClearQuery),
            DECLARE_NAPI_GETTER("scheme", GetScheme),
            DECLARE_NAPI_GETTER("authority", GetAuthority),
            DECLARE_NAPI_GETTER("ssp", GetSsp),
            DECLARE_NAPI_GETTER("userInfo", GetUserinfo),
            DECLARE_NAPI_GETTER("host", GetHost),
            DECLARE_NAPI_GETTER("port", GetPort),
            DECLARE_NAPI_GETTER("path", GetPath),
            DECLARE_NAPI_GETTER("query", GetQuery),
            DECLARE_NAPI_GETTER("fragment", GetFragment),
            DECLARE_NAPI_GETTER("isFailed", IsFailed),
        };
        NAPI_CALL(env, napi_define_class(env, uriClassName, strlen(uriClassName), UriConstructor,
                                         nullptr, sizeof(uriDesc) / sizeof(uriDesc[0]), uriDesc, &uriClass));
        napi_property_descriptor desc[] = {
            DECLARE_NAPI_PROPERTY("Uri", uriClass)
        };
        napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
        return exports;
    }

    extern "C"
    __attribute__((visibility("default"))) void NAPI_uri_GetJSCode(const char **buf, int *bufLen)
    {
        if (buf != nullptr) {
            *buf = _binary_js_uri_js_start;
        }
        if (bufLen != nullptr) {
            *bufLen = _binary_js_uri_js_end - _binary_js_uri_js_start;
        }
    }
    extern "C"
    __attribute__((visibility("default"))) void NAPI_uri_GetABCCode(const char** buf, int* buflen)
    {
        if (buf != nullptr) {
            *buf = _binary_uri_abc_start;
        }
        if (buflen != nullptr) {
            *buflen = _binary_uri_abc_end - _binary_uri_abc_start;
        }
    }

    static napi_module_with_js UriModule = {
        .nm_version = 1,
        .nm_flags = 0,
        .nm_filename = nullptr,
        .nm_register_func = UriInit,
        .nm_modname = "uri",
        .nm_priv = reinterpret_cast<void*>(0),
        .nm_get_abc_code = NAPI_uri_GetABCCode,
        .nm_get_js_code = NAPI_uri_GetJSCode,
    };
    extern "C" __attribute__((constructor)) void UriRegisterModule()
    {
        napi_module_with_js_register(&UriModule);
    }
} // namespace OHOS::Uri
