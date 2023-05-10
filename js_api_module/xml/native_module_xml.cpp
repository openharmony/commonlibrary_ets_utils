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

#include "js_xml.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

extern const char _binary_js_xml_js_start[];
extern const char _binary_js_xml_js_end[];
extern const char _binary_xml_abc_start[];
extern const char _binary_xml_abc_end[];

namespace OHOS::xml {
    static napi_value XmlSerializerConstructor(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        void *data = nullptr;
        size_t argc = 0;
        napi_value args[2] = { 0 }; // 2:The number of parameters is 2
        XmlSerializer *object = nullptr;
        size_t iLength = 0;
        size_t offPos = 0;
        napi_value arraybuffer = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, nullptr, &thisVar, &data));
        NAPI_ASSERT(env, argc == 1 || argc == 2, "Wrong number of arguments"); // 2: number of args
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, &data));
        if (args[0] == nullptr) {
            NAPI_CALL(env, napi_throw_error(env, "", "parameter is empty"));
            return nullptr;
        } else {
            bool bFlag = false;
            napi_is_arraybuffer(env, args[0], &bFlag);
            if (bFlag) {
                napi_get_arraybuffer_info(env, args[0], &data, &iLength);
            }
            napi_is_dataview(env, args[0], &bFlag);
            if (bFlag) {
                napi_get_dataview_info(env, args[0], &iLength, &data, &arraybuffer, &offPos);
            }
        }
        if (argc == 1) {
            object = new XmlSerializer(reinterpret_cast<char*>(data), iLength);
        } else if (argc == 2) { // 2:When the input parameter is set to 2
            std::string encoding = "";
            napi_valuetype valuetype;
            NAPI_CALL(env, napi_typeof(env, args[1], &valuetype));
            NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument typr. String expected.");
            napi_status status = napi_ok;
            status = XmlSerializer::DealNapiStrValue(env, args[1], encoding);
            if (status == napi_ok) {
                object = new XmlSerializer(reinterpret_cast<char*>(data), iLength, encoding);
            }
        }
        napi_wrap(
            env, thisVar, object,
            [](napi_env environment, void *data, void *hint) {
                auto obj = reinterpret_cast<XmlSerializer*>(data);
                if (obj != nullptr) {
                    delete obj;
                }
            },
            nullptr, nullptr);
        return thisVar;
    }

    static napi_value XmlPullParserConstructor(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        void *data = nullptr;
        size_t argc = 0;
        napi_value args[2] = { 0 }; // 2:two args
        XmlPullParser *object = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, nullptr, &thisVar, &data));
        NAPI_ASSERT(env, argc == 1 || argc == 2, "Wrong number of arguments"); // 2:two args
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, &data));
        napi_valuetype valuetype = napi_null;
        NAPI_CALL(env, napi_typeof(env, args[0], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument typr. Object(DataView or ArrayBuffer) expected.");
        bool bFlag = false;
        size_t len = 0;
        size_t offPos = 0;
        napi_value arraybuffer = nullptr;
        NAPI_ASSERT(env, args[0] != nullptr, "Wrong argument value. (not nullptr expected).");
        napi_is_arraybuffer(env, args[0], &bFlag);
        if (bFlag) {
            napi_get_arraybuffer_info(env, args[0], &data, &len);
        }
        napi_is_dataview(env, args[0], &bFlag);
        if (bFlag) {
            napi_get_dataview_info(env, args[0], &len, &data, &arraybuffer, &offPos);
        }
        if (data) {
            std::string strEnd(reinterpret_cast<char*>(data), len);
            strEnd = strEnd.substr(0, std::strlen(strEnd.c_str()));
            std::string cleanStr;
            for (char c : strEnd) {
                if (c != '\r' && c != '\n') {
                    cleanStr += c;
              }
            }
            if (argc == 1) {
                object = new XmlPullParser(cleanStr, "utf-8");
            } else if (argc == 2) { // 2:When the input parameter is set to 2
                NAPI_CALL(env, napi_typeof(env, args[1], &valuetype));
                NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument typr. String expected.");
                std::string strEncoding = "";
                XmlSerializer::DealNapiStrValue(env, args[1], strEncoding);
                object = new XmlPullParser(cleanStr, strEncoding);
            }
        }
        napi_wrap(
            env, thisVar, object,
            [](napi_env env, void *data, void *hint) {
                auto obj = reinterpret_cast<XmlPullParser*>(data);
                if (obj != nullptr) {
                    delete obj;
                }
            },
            nullptr, nullptr);
        return thisVar;
    }

    static napi_value SetAttributes(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 0;
        napi_value args[2] = { 0 }; // 2:two args
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, nullptr, &thisVar, nullptr));
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc == 2, "Wrong number of arguments"); // 2: number of args
        napi_valuetype valuetype = napi_null;
        NAPI_ASSERT(env, args[0] != nullptr, "Parameter is empty.");
        NAPI_ASSERT(env, args[1] != nullptr, "Parameter is empty.");
        NAPI_CALL(env, napi_typeof(env, args[0], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument typr. Object expected.");
        NAPI_CALL(env, napi_typeof(env, args[1], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument typr. Object expected.");
        XmlSerializer *object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object)));
        std::string name;
        std::string value;
        XmlSerializer::DealNapiStrValue(env, args[0], name);
        XmlSerializer::DealNapiStrValue(env, args[1], value);
        object->SetAttributes(name, value);
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &result));
        return result;
    }

    static napi_value AddEmptyElement(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 1;
        napi_value args[1] = { 0 };
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc == 1, "Wrong number of arguments(Over)");
        if (args[0] == nullptr) {
            NAPI_CALL(env, napi_throw_error(env, "", "parameter is empty"));
            return nullptr;
        } else {
            napi_valuetype valuetype;
            NAPI_CALL(env, napi_typeof(env, args[0], &valuetype));
            NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument typr. String expected.");
            XmlSerializer *object = nullptr;
            NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object)));
            std::string name;
            object->DealNapiStrValue(env, args[0], name);
            object->AddEmptyElement(name);
        }
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &result));
        return result;
    }

    static napi_value SetDeclaration(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        XmlSerializer *object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object)));
        object->SetDeclaration();
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &result));
        return result;
    }

    static napi_value StartElement(napi_env env, napi_callback_info info)
    {
        size_t argc = 1;
        napi_value args[1] = { 0 };
        napi_value thisVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, nullptr, &thisVar, nullptr));
        NAPI_ASSERT(env, argc == 1, "Wrong number of arguments");
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));
        NAPI_ASSERT(env, args[0] != nullptr, "Parameter is empty.");
        XmlSerializer *object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object)));
        std::string name;
        object->DealNapiStrValue(env, args[0], name);
        object->StartElement(name);
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &result));
        return result;
    }

    static napi_value EndElement(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        XmlSerializer *object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object)));
        object->EndElement();
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &result));
        return result;
    }

    static napi_value SetNamespace(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 0;
        napi_value args[2] = { 0 }; // 2:two args
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, nullptr, &thisVar, nullptr));
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc == 2, "Wrong number of arguments"); // 2:two args
        napi_valuetype valuetype = napi_null;
        NAPI_ASSERT(env, args[0] != nullptr, "Parameter is empty.");
        NAPI_ASSERT(env, args[1] != nullptr, "Parameter is empty.");
        NAPI_CALL(env, napi_typeof(env, args[0], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument typr. Object expected.");
        NAPI_CALL(env, napi_typeof(env, args[1], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument typr. Object expected.");
        XmlSerializer *object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object)));
        std::string prefix;
        std::string nsTemp;
        XmlSerializer::DealNapiStrValue(env, args[0], prefix);
        XmlSerializer::DealNapiStrValue(env, args[1], nsTemp);
        object->SetNamespace(prefix, nsTemp);
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &result));
        return result;
    }

    static napi_value SetComment(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 0;
        napi_value args[1] = { 0 };
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, nullptr, &thisVar, nullptr));
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc == 1, "Wrong number of arguments");
        napi_valuetype valuetype = napi_null;
        NAPI_ASSERT(env, args[0] != nullptr, "Parameter is empty).");
        NAPI_CALL(env, napi_typeof(env, args[0], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument typr. String expected");
        XmlSerializer *object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object)));
        std::string comment;
        object->DealNapiStrValue(env, args[0], comment);
        object->SetComment(comment);
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &result));
        return result;
    }

    static napi_value SetCData(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 0;
        napi_value args[1] = { 0 };
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, nullptr, &thisVar, nullptr));
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc == 1, "Wrong number of arguments");
        napi_valuetype valuetype = napi_null;
        NAPI_ASSERT(env, args[0] != nullptr, "Parameter is empty).");
        NAPI_CALL(env, napi_typeof(env, args[0], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument typr. String expected");
        XmlSerializer *object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object)));
        std::string data;
        XmlSerializer::DealNapiStrValue(env, args[0], data);
        object->SetCData(data);
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &result));
        return result;
    }

    static napi_value SetText(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 0;
        napi_value args[1] = { 0 };
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, nullptr, &thisVar, nullptr));
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc == 1, "Wrong number of arguments");
        napi_valuetype valuetype = napi_null;
        NAPI_ASSERT(env, args[0] != nullptr, "Parameter is empty).");
        NAPI_CALL(env, napi_typeof(env, args[0], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument typr. String expected.");
        XmlSerializer *object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object)));
        std::string text;
        XmlSerializer::DealNapiStrValue(env, args[0], text);
        object->SetText(text);
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &result));
        return result;
    }

    static napi_value SetDocType(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 0;
        napi_value args[1] = { 0 };
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, nullptr, &thisVar, nullptr));
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc == 1, "Wrong number of arguments");
        napi_valuetype valuetype = napi_null;
        NAPI_ASSERT(env, args[0] != nullptr, "Parameter is empty).");
        NAPI_CALL(env, napi_typeof(env, args[0], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument typr. String expected.");
        XmlSerializer *object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object)));
        std::string text;
        XmlSerializer::DealNapiStrValue(env, args[0], text);
        object->SetDocType(text);
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &result));
        return result;
    }
    static napi_value XmlSerializerError(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        XmlSerializer *object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object)));
        std::string temp = object->XmlSerializerError();
        size_t templen = temp.size();
        NAPI_CALL(env, napi_create_string_utf8(env, temp.c_str(), templen, &result));
        return result;
    }

    static napi_value XmlSerializerInit(napi_env env, napi_value exports)
    {
        const char *xmlSerializerClass = "XmlSerializer";
        napi_value xmlClass = nullptr;
        napi_property_descriptor xmlDesc[] = {
            DECLARE_NAPI_FUNCTION("setAttributes", SetAttributes),
            DECLARE_NAPI_FUNCTION("addEmptyElement", AddEmptyElement),
            DECLARE_NAPI_FUNCTION("setDeclaration", SetDeclaration),
            DECLARE_NAPI_FUNCTION("startElement", StartElement),
            DECLARE_NAPI_FUNCTION("endElement", EndElement),
            DECLARE_NAPI_FUNCTION("setNamespace", SetNamespace),
            DECLARE_NAPI_FUNCTION("setComment", SetComment),
            DECLARE_NAPI_FUNCTION("setCDATA", SetCData),
            DECLARE_NAPI_FUNCTION("setText", SetText),
            DECLARE_NAPI_FUNCTION("setDocType", SetDocType),
            DECLARE_NAPI_FUNCTION("XmlSerializerError", XmlSerializerError)
        };
        NAPI_CALL(env, napi_define_class(env, xmlSerializerClass, strlen(xmlSerializerClass), XmlSerializerConstructor,
                                         nullptr, sizeof(xmlDesc) / sizeof(xmlDesc[0]), xmlDesc, &xmlClass));
        napi_property_descriptor desc[] = {
            DECLARE_NAPI_PROPERTY("XmlSerializer", xmlClass)
        };
        napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
        return exports;
    }

    static napi_value Parse(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 0;
        napi_value args[1] = { 0 };
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, nullptr, &thisVar, nullptr));
        NAPI_ASSERT(env, argc == 1, "Wrong number of arguments, one expected.");
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));
        NAPI_ASSERT(env, args[0] != nullptr, "Parameter is empty.");
        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, args[0], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument typr. Object expected.");
        XmlPullParser *object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object)));
        object->DealOptionInfo(env, args[0]);
        object->Parse(env, thisVar);
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &result));
        return result;
    }

    static napi_value XmlPullParserError(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        XmlPullParser *object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object)));
        std::string temp = object->XmlPullParserError();
        size_t templen = temp.size();
        NAPI_CALL(env, napi_create_string_utf8(env, temp.c_str(), templen, &result));
        return result;
    }

    static napi_value XmlPullParserInit(napi_env env, napi_value exports)
    {
        const char *xmlPullParserClass = "XmlPullParser";
        napi_value xmlClass = nullptr;
        napi_property_descriptor xmlDesc[] = {
            DECLARE_NAPI_FUNCTION("parse", Parse),
            DECLARE_NAPI_FUNCTION("XmlPullParserError", XmlPullParserError)
        };
        NAPI_CALL(env, napi_define_class(env, xmlPullParserClass, strlen(xmlPullParserClass),
            XmlPullParserConstructor, nullptr, sizeof(xmlDesc) / sizeof(xmlDesc[0]),
            xmlDesc, &xmlClass));
        napi_property_descriptor desc[] = {
            DECLARE_NAPI_PROPERTY("XmlPullParser", xmlClass)
        };
        napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
        return exports;
    };

    static napi_value Init(napi_env env, napi_value exports)
    {
        XmlSerializerInit(env, exports);
        XmlPullParserInit(env, exports);
        return exports;
    }

    extern "C"
    __attribute__((visibility("default"))) void NAPI_xml_GetJSCode(const char **buf, int *bufLen)
    {
        if (buf != nullptr) {
            *buf = _binary_js_xml_js_start;
        }

        if (bufLen != nullptr) {
            *bufLen = _binary_js_xml_js_end - _binary_js_xml_js_start;
        }
    }
    extern "C"
    __attribute__((visibility("default"))) void NAPI_xml_GetABCCode(const char** buf, int* buflen)
    {
        if (buf != nullptr) {
            *buf = _binary_xml_abc_start;
        }
        if (buflen != nullptr) {
            *buflen = _binary_xml_abc_end - _binary_xml_abc_start;
        }
    }


    static napi_module xmlModule = {
        .nm_version = 1,
        .nm_flags = 0,
        .nm_filename = nullptr,
        .nm_register_func = Init,
        .nm_modname = "xml",
        .nm_priv = reinterpret_cast<void*>(0),
        .reserved = {0},
    };
    extern "C" __attribute__((constructor)) void RegisterModule()
    {
        napi_module_register(&xmlModule);
    }
} // namespace OHOS::Xml
