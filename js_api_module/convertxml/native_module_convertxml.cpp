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

#include "utils/log.h"
#include "js_convertxml.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

extern const char _binary_js_convertxml_js_start[];
extern const char _binary_js_convertxml_js_end[];
extern const char _binary_convertxml_abc_start[];
extern const char _binary_convertxml_abc_end[];

namespace OHOS::Xml {
    static napi_value ConvertXmlConstructor(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        void *data = nullptr;
        napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, &data);
        auto objectInfo = new ConvertXml();
        napi_wrap(
            env, thisVar, objectInfo,
            [](napi_env environment, void *data, void *hint) {
                auto obj = reinterpret_cast<ConvertXml*>(data);
                if (obj != nullptr) {
                    delete obj;
                }
            },
            nullptr, nullptr);
        return thisVar;
    }

    static napi_value Convert(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t requireMaxArgc = 2; // 2:MaxArgc
        size_t requireMinArgc = 1;
        size_t argc = 2; // 2:The number of parameters is 2
        napi_value args[2] = {nullptr};
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc <= requireMaxArgc, "Wrong number of arguments(Over)");
        NAPI_ASSERT(env, argc >= requireMinArgc, "Wrong number of arguments(Less)");
        std::string strXml;
        napi_valuetype valuetype;
        ConvertXml *object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object)));
        if (args[0] == nullptr) {
            NAPI_CALL(env, napi_throw_error(env, "", "parameter is empty"));
            return nullptr;
        } else {
            NAPI_CALL(env, napi_typeof(env, args[0], &valuetype));
            NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument typr. String expected.");
            object->DealNapiStrValue(env, args[0], strXml);
        }
        if (args[1] != nullptr) {
            object->DealOptions(env, args[1]);
        }
        napi_value result = object->Convert(env, strXml);
        return result;
    }


    static napi_value ConvertXmlInit(napi_env env, napi_value exports)
    {
        const char *convertXmlClassName = "ConvertXml";
        napi_value convertXmlClass = nullptr;
        napi_property_descriptor convertXmlDesc[] = {
            DECLARE_NAPI_FUNCTION("convert", Convert)
        };
        NAPI_CALL(env, napi_define_class(env, convertXmlClassName, strlen(convertXmlClassName), ConvertXmlConstructor,
                                         nullptr, sizeof(convertXmlDesc) / sizeof(convertXmlDesc[0]), convertXmlDesc,
                                         &convertXmlClass));
        napi_property_descriptor desc[] = {
            DECLARE_NAPI_PROPERTY("ConvertXml", convertXmlClass)
        };
        NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
        return exports;
    }

    static napi_module convertXmlModule = {
        .nm_version = 1,
        .nm_flags = 0,
        .nm_filename = nullptr,
        .nm_register_func = ConvertXmlInit,
        .nm_modname = "convertxml",
        .nm_priv = reinterpret_cast<void*>(0),
        .reserved = { 0 },
    };

    extern "C" __attribute__((constructor)) void RegisterModule()
    {
        napi_module_register(&convertXmlModule);
    }

    extern "C"
    __attribute__((visibility("default"))) void NAPI_convertxml_GetJSCode(const char **buf, int *bufLen)
    {
        if (buf != nullptr) {
            *buf = _binary_js_convertxml_js_start;
        }
        if (bufLen != nullptr) {
            *bufLen = _binary_js_convertxml_js_end - _binary_js_convertxml_js_start;
        }
    }
    extern "C"
    __attribute__((visibility("default"))) void NAPI_convertxml_GetABCCode(const char** buf, int* buflen)
    {
        if (buf != nullptr) {
            *buf = _binary_convertxml_abc_start;
        }
        if (buflen != nullptr) {
            *buflen = _binary_convertxml_abc_end - _binary_convertxml_abc_start;
        }
    }
} // namespace OHOS::Xml
