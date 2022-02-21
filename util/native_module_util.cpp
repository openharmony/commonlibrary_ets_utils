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
#include <vector>

#include "js_textdecoder.h"
#include "js_textencoder.h"
#include "js_base64.h"
#include "js_types.h"

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "securec.h"
#include "utils/log.h"

extern const char _binary_util_js_js_start[];
extern const char _binary_util_js_js_end[];
extern const char _binary_util_abc_start[];
extern const char _binary_util_abc_end[];
namespace OHOS::Util {
    static char* ApplyMemory(const size_t length)
    {
        char *type = new char[length + 1];
        if (memset_s(type, length + 1, '\0', length + 1) != 0) {
                HILOG_ERROR("type memset_s failed");
                delete[] type;
                return nullptr;
        }
        return type;
    }

    static std::string temp = "cdfijoOs";
    static std::string DealWithPrintf(const std::string &format, const std::vector<std::string> &value)
    {
        size_t i = 0;
        size_t j = 0;
        std::string str;
        size_t formatSize = format.size();
        size_t valueSize = value.size();
        while (i < formatSize && j < valueSize) {
            if (format[i] == '%' && (i + 1 < formatSize && format[i + 1] == '%')) {
                str += '%';
                i += 2; // 2:The array goes back two digits.
            } else if (format[i] == '%' && (i + 1 < formatSize && (temp.find(format[i + 1])) != std::string::npos)) {
                if (format[i + 1] == 'c') {
                    j++;
                } else {
                    str += value[j++];
                }
                i += 2; // 2:The array goes back two digits.
            } else if (format[i] == '%' && (i + 1 < formatSize && (temp.find(format[i + 1])) == std::string::npos)) {
                str += '%';
                i++;
            }
            if (i < formatSize && format[i] != '%') {
                size_t pos = 0;
                if ((pos = format.find('%', i)) == std::string::npos) {
                    str += format.substr(i);
                    break;
                } else {
                    str += format.substr(i, pos - i);
                    i = pos;
                }
            }
        }
        while (j < valueSize) {
            str += " " + value[j++];
        }
        if (i < formatSize) {
            str += format.substr(i);
        }
        return str;
    }

    static napi_value FormatString(napi_env env, std::string &str)
    {
        std::string res;
        size_t strSize = str.size();
        for (size_t i = 0; i < strSize; ++i) {
            if (str[i] == '%' && (i + 1 < strSize && temp.find(str[i + 1]) != std::string::npos)) {
                if (str[i + 1] == 'o') {
                    res += "o ";
                } else if (str[i + 1] == 'O') {
                    res += "O ";
                } else if (str[i + 1] == 'i') {
                    res += "i ";
                } else if (str[i + 1] == 'j') {
                    res += "j ";
                } else if (str[i + 1] == 'd') {
                    res += "d ";
                } else if (str[i + 1] == 's') {
                    res += "s ";
                } else if (str[i + 1] == 'f') {
                    res += "f ";
                } else if (str[i + 1] == 'c') {
                    res += "c ";
                }
                i++;
            } else if (str[i] == '%' && (i + 1 < strSize && str[i + 1] == '%')) {
                i++;
            }
        }
        res = res.substr(0, res.size() - 1);
        napi_value result = nullptr;
        napi_create_string_utf8(env, res.c_str(), res.size(), &result);
        return result;
    }

    static napi_value DealWithFormatString(napi_env env, napi_callback_info info)
    {
        size_t argc = 0;
        napi_get_cb_info(env, info, &argc, nullptr, nullptr, nullptr);
        napi_value *argv = nullptr;
        NAPI_ASSERT(env, argc > 0, "argc == 0");
        argv = new napi_value[argc + 1];
        if (memset_s(argv, argc + 1, 0, argc + 1) != 0) {
            HILOG_ERROR("argv memset error");
            delete []argv;
            return nullptr;
        }
        napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
        char *format = nullptr;
        size_t formatsize = 0;
        napi_get_value_string_utf8(env, argv[0], nullptr, 0, &formatsize);
        NAPI_ASSERT(env, formatsize > 0, "formatsize == 0");
        format = new char[formatsize + 1];
        if (memset_s(format, formatsize + 1, 0, formatsize + 1) != 0) {
            HILOG_ERROR("format memset error");
            delete []format;
            return nullptr;
        }
        napi_get_value_string_utf8(env, argv[0], format, formatsize + 1, &formatsize);
        std::string str = format;
        delete []format;
        delete []argv;
        argv = nullptr;
        format = nullptr;
        return FormatString(env, str);
    }

    static std::string PrintfString(const std::string &format, const std::vector<std::string> &value)
    {
        std::string printInfo = DealWithPrintf(format, value);
        return printInfo;
    }

    static napi_value Printf(napi_env env, napi_callback_info info)
    {
        napi_value result = nullptr;
        size_t argc = 0;
        napi_get_cb_info(env, info, &argc, nullptr, nullptr, nullptr);
        napi_value *argv = nullptr;
        if (argc > 0) {
            argv = new napi_value[argc];
            napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
            char *format = nullptr;
            size_t formatsize = 0;
            napi_get_value_string_utf8(env, argv[0], nullptr, 0, &formatsize);
            if (formatsize > 0) {
                format = new char[formatsize + 1];
            }
            napi_get_value_string_utf8(env, argv[0], format, formatsize + 1, &formatsize);
            std::string printInfo;
            std::vector<std::string> value;
            for (size_t i = 1; i < argc; i++) {
                char *valueString = nullptr;
                size_t valuesize = 0;
                napi_get_value_string_utf8(env, argv[i], nullptr, 0, &valuesize);
                if (valuesize > 0) {
                    valueString = new char[valuesize + 1];
                }
                napi_get_value_string_utf8(env, argv[i], valueString, valuesize + 1, &valuesize);
                value.push_back(valueString);
                delete []valueString;
                valueString = nullptr;
            }
            printInfo = PrintfString(format, value);
            napi_create_string_utf8(env, printInfo.c_str(), printInfo.size(), &result);
            delete []format;
            delete []argv;
            argv = nullptr;
            format = nullptr;
            return result;
        }
        napi_value res = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &res));
        return res;
    }

    static napi_value GetErrorString(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value result = nullptr;
        std::string errInfo;
        size_t argc = 1;
        napi_value argv = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &argv, &thisVar, nullptr));
        int32_t err = 0;
        NAPI_CALL(env, napi_get_value_int32(env, argv, &err));
        errInfo = strerror(err);
        NAPI_CALL(env, napi_create_string_utf8(env, errInfo.c_str(), errInfo.size(), &result));
        return result;
    }

    static void SetVec(const napi_status fatSta, const napi_status bomSta, const bool fat, const bool bom,
        std::vector<int> &paraVec)
    {
        if (paraVec.size() != 2) {
            return;
        }
        if (fatSta == napi_ok) {
            if (fat) {
                paraVec[0] = 1;
            } else {
                paraVec[0] = 0;
            }
        }
        if (bomSta == napi_ok) {
            if (bom) {
                paraVec[1] = 1;
            } else {
                paraVec[1] = 0;
            }
        }
    }

    static napi_value GetSecPara(napi_env env, napi_value valData, std::vector<int> &paraVec)
    {
        napi_value messageKeyFatal = nullptr;
        const char *messageKeyStrFatal = "fatal";
        napi_value messageKeyIgnorebom = nullptr;
        const char *messageKeyStrIgnorebom = "ignoreBOM";
        napi_value resultFatal = nullptr;
        napi_value resultIgnorebom = nullptr;
        bool bResultFat = false;
        bool bResultIgnbom = false;
        NAPI_CALL(env, napi_create_string_utf8(env, messageKeyStrFatal, strlen(messageKeyStrFatal),
            &messageKeyFatal));
        NAPI_CALL(env, napi_create_string_utf8(env, messageKeyStrIgnorebom, strlen(messageKeyStrIgnorebom),
            &messageKeyIgnorebom));
        NAPI_CALL(env, napi_get_property(env, valData, messageKeyFatal, &resultFatal));
        NAPI_CALL(env, napi_get_property(env, valData, messageKeyIgnorebom, &resultIgnorebom));
        napi_status naFat = napi_get_value_bool(env, resultFatal, &bResultFat);
        napi_status naBom = napi_get_value_bool(env, resultIgnorebom, &bResultIgnbom);
        SetVec(naFat, naBom, bResultFat, bResultIgnbom, paraVec);
        return nullptr;
    }

    static napi_value TextdecoderConstructor(napi_env env, napi_callback_info info)
    {
        size_t tempArgc = 0;
        napi_value thisVar = nullptr;
        napi_get_cb_info(env, info, &tempArgc, nullptr, &thisVar, nullptr);
        size_t argc = 0;
        void *data = nullptr;
        char *type = nullptr;
        size_t typeLen = 0;
        std::vector<int> paraVec(2, 0); // 2: Specifies the size of the container to be applied for.
        if (tempArgc == 1) {
            argc = 1;
            napi_value argv = nullptr;
            NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &argv, nullptr, &data));
            // first para
            NAPI_CALL(env, napi_get_value_string_utf8(env, argv, nullptr, 0, &typeLen));
            if (typeLen > 0) {
                type = ApplyMemory(typeLen);
            }
            napi_get_value_string_utf8(env, argv, type, typeLen + 1, &typeLen);
        } else if (tempArgc == 2) { // 2: The number of parameters is 2.
            argc = 2; // 2: The number of parameters is 2.
            napi_value argv[2] = { 0 };
            NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, &data));
            // first para
            NAPI_CALL(env, napi_get_value_string_utf8(env, argv[0], nullptr, 0, &typeLen));
            if (typeLen > 0) {
                type = ApplyMemory(typeLen);
            }
            napi_get_value_string_utf8(env, argv[0], type, typeLen + 1, &typeLen);
            // second para
            GetSecPara(env, argv[1], paraVec);
        }
        std::string enconding = "utf-8";
        if (type != nullptr) {
            enconding = type;
        }
        delete []type;
        type = nullptr;
        auto objectInfo = new TextDecoder(env, enconding, paraVec);
        NAPI_CALL(env, napi_wrap(
            env, thisVar, objectInfo,
            [](napi_env env, void *data, void *hint) {
                auto objInfo = (TextDecoder*)data;
                if (objInfo != nullptr) {
                    delete objInfo;
                }
            },
            nullptr, nullptr));
        return thisVar;
    }

    static napi_value TextdecoderDecode(napi_env env, napi_callback_info info)
    {
        size_t tempArgc = 2;
        napi_value thisVar = nullptr;
        napi_get_cb_info(env, info, &tempArgc, nullptr, &thisVar, nullptr);
        size_t argc = 0;
        void *dataPara = nullptr;
        napi_typedarray_type type;
        size_t length = 0;
        void *data = nullptr;
        napi_value arraybuffer = nullptr;
        size_t byteOffset = 0;
        bool iStream = false;
        TextDecoder *textDecoder = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&textDecoder));
        napi_value valStr = nullptr;
        if (tempArgc == 1) {
            argc = 1;
            napi_value argv = nullptr;
            NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &argv, nullptr, &dataPara));
            // first para
            NAPI_CALL(env, napi_get_typedarray_info(env, argv, &type, &length, &data, &arraybuffer, &byteOffset));
            valStr = textDecoder->Decode(argv, iStream);
        } else if (tempArgc == 2) { // 2: The number of parameters is 2.
            argc = 2; // 2: The number of parameters is 2.
            napi_value argv[2] = { 0 };
            NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, &dataPara));
            // first para
            NAPI_CALL(env, napi_get_typedarray_info(env, argv[0], &type, &length, &data, &arraybuffer, &byteOffset));
            // second para
            napi_value messageKeyStream = nullptr;
            const char *messageKeyStrStream = "stream";

            napi_value resultStream = nullptr;
            NAPI_CALL(env, napi_create_string_utf8(env, messageKeyStrStream, strlen(messageKeyStrStream),
                &messageKeyStream));
            NAPI_CALL(env, napi_get_property(env, argv[1], messageKeyStream, &resultStream));
            NAPI_CALL(env, napi_get_value_bool(env, resultStream, &iStream));
            valStr = textDecoder->Decode(argv[0], iStream);
        }
        return valStr;
    }

    static napi_value TextdecoderGetEncoding(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        TextDecoder *textDecoder = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&textDecoder));
        napi_value retVal = textDecoder->GetEncoding();
        return retVal;
    }

    static napi_value TextdecoderGetFatal(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        TextDecoder *textDecoder = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&textDecoder));
        napi_value retVal = textDecoder->GetFatal();
        return retVal;
    }

    static napi_value TextdecoderGetIgnoreBOM(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        TextDecoder *textDecoder = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&textDecoder));
        napi_value retVal = textDecoder->GetIgnoreBOM();
        return retVal;
    }

    // Encoder
    static napi_value TextEncoderConstructor(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        void *data = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, &data));

        auto object = new TextEncoder(env);
        NAPI_CALL(env, napi_wrap(
            env, thisVar, object,
            [](napi_env env, void *data, void *hint) {
                auto obj = (TextEncoder*)data;
                if (obj != nullptr) {
                    delete obj;
                }
            },
            nullptr, nullptr));
        return thisVar;
    }

    static napi_value GetEncoding(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));

        TextEncoder *object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));

        return object->GetEncoding();
    }

    static napi_value Encode(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t requireArgc = 1;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));

        NAPI_ASSERT(env, argc >= requireArgc, "Wrong number of arguments");

        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, args, &valuetype));

        NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");

        TextEncoder *object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));

        napi_value result = object->Encode(args);

        return result;
    }

    static napi_value EncodeInto(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t requireArgc = 2;
        size_t argc = 2;
        napi_value args[2] = { nullptr };
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));

        NAPI_ASSERT(env, argc >= requireArgc, "Wrong number of arguments");

        napi_valuetype valuetype0;
        NAPI_CALL(env, napi_typeof(env, args[0], &valuetype0));

        napi_typedarray_type valuetype1;
        size_t length = 0;
        void *data = nullptr;
        napi_value arraybuffer = nullptr;
        size_t byteOffset = 0;
        NAPI_CALL(env, napi_get_typedarray_info(env, args[1], &valuetype1, &length, &data, &arraybuffer, &byteOffset));

        NAPI_ASSERT(env, valuetype0 == napi_string, "Wrong argument type. String expected.");
        NAPI_ASSERT(env, valuetype1 == napi_uint8_array, "Wrong argument type. napi_uint8_array expected.");

        TextEncoder *object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));

        napi_value result = object->EncodeInto(args[0], args[1]);

        return result;
    }
    static napi_value TextcoderInit(napi_env env, napi_value exports)
    {
        const char *textEncoderClassName = "TextEncoder";
        napi_value textEncoderClass = nullptr;
        static napi_property_descriptor textEncoderDesc[] = {
            DECLARE_NAPI_GETTER("encoding", GetEncoding),
            DECLARE_NAPI_FUNCTION("encode", Encode),
            DECLARE_NAPI_FUNCTION("encodeInto", EncodeInto),
        };
        NAPI_CALL(env, napi_define_class(env, textEncoderClassName, strlen(textEncoderClassName),
                                         TextEncoderConstructor, nullptr,
                                         sizeof(textEncoderDesc) / sizeof(textEncoderDesc[0]),
                                         textEncoderDesc, &textEncoderClass));

        const char *textDecoderClassName = "TextDecoder";
        napi_value textDecoderClass = nullptr;
        static napi_property_descriptor textdecoderDesc[] = {
            DECLARE_NAPI_FUNCTION("decode", TextdecoderDecode),
            DECLARE_NAPI_GETTER("encoding", TextdecoderGetEncoding),
            DECLARE_NAPI_GETTER("fatal", TextdecoderGetFatal),
            DECLARE_NAPI_GETTER("ignoreBOM", TextdecoderGetIgnoreBOM),
        };
        NAPI_CALL(env, napi_define_class(env, textDecoderClassName, strlen(textDecoderClassName),
                                         TextdecoderConstructor, nullptr,
                                         sizeof(textdecoderDesc) / sizeof(textdecoderDesc[0]),
                                         textdecoderDesc, &textDecoderClass));

        static napi_property_descriptor desc[] = {
            DECLARE_NAPI_PROPERTY("TextEncoder", textEncoderClass),
            DECLARE_NAPI_PROPERTY("TextDecoder", textDecoderClass),
        };

        NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
        return exports;
    }

    static napi_value Base64Constructor(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        void *data = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, &data));
        auto objectInfo = new Base64(env);
        napi_wrap(
            env, thisVar, objectInfo,
            [](napi_env env, void *data, void *hint) {
                auto objInfo = (Base64*)data;
                if (objInfo != nullptr) {
                    delete objInfo;
                }
            },
            nullptr, nullptr);
        return thisVar;
    }

    static napi_value EncodeBase64(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t requireArgc = 1;
        size_t argc = 1;
        napi_value args[1] = { nullptr };
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc >= requireArgc, "Wrong number of arguments");
        napi_typedarray_type valuetype0;
        size_t length = 0;
        void *data = nullptr;
        napi_value arraybuffer = nullptr;
        size_t byteOffset = 0;
        NAPI_CALL(env, napi_get_typedarray_info(env, args[0], &valuetype0, &length, &data, &arraybuffer, &byteOffset));
        NAPI_ASSERT(env, valuetype0 == napi_uint8_array, "Wrong argument type. napi_uint8_array expected.");
        Base64 *object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value result = object->EncodeSync(args[0]);
        return result;
    }

    static napi_value EncodeToString(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t requireArgc = 1;
        size_t argc = 1;
        napi_value args[1] = { nullptr };
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc >= requireArgc, "Wrong number of arguments");
        napi_typedarray_type valuetype0;
        size_t length = 0;
        void *data = nullptr;
        napi_value arraybuffer = nullptr;
        size_t byteOffset = 0;
        NAPI_CALL(env, napi_get_typedarray_info(env, args[0], &valuetype0, &length, &data, &arraybuffer, &byteOffset));
        NAPI_ASSERT(env, valuetype0 == napi_uint8_array, "Wrong argument type. napi_uint8_array expected.");
        Base64 *object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value result = object->EncodeToStringSync(args[0]);
        return result;
    }

    static napi_value DecodeBase64(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t requireArgc = 1;
        size_t argc = 1;
        napi_value args[1] = { nullptr };
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc >= requireArgc, "Wrong number of arguments");
        napi_typedarray_type valuetype0;
        napi_valuetype valuetype1;
        size_t length = 0;
        void *data = nullptr;
        napi_value arraybuffer = nullptr;
        size_t byteOffset = 0;
        NAPI_CALL(env, napi_typeof(env, args[0], &valuetype1));
        if (valuetype1 != napi_valuetype::napi_string) {
            NAPI_CALL(env, napi_get_typedarray_info(env, args[0], &valuetype0, &length,
                                                    &data, &arraybuffer, &byteOffset));
        }
        if ((valuetype1 != napi_valuetype::napi_string) && (valuetype0 != napi_typedarray_type::napi_uint8_array)) {
            napi_throw_error(env, nullptr, "The parameter type is incorrect");
        }
        Base64 *object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value result = object->DecodeSync(args[0]);
        return result;
    }
    static napi_value EncodeAsync(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t requireArgc = 1;
        size_t argc = 1;
        napi_value args[1] = { nullptr };
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc >= requireArgc, "Wrong number of arguments");
        napi_typedarray_type valuetype0;
        size_t length = 0;
        void *data = nullptr;
        napi_value arraybuffer = nullptr;
        size_t byteOffset = 0;
        NAPI_CALL(env, napi_get_typedarray_info(env, args[0], &valuetype0, &length, &data, &arraybuffer, &byteOffset));
        NAPI_ASSERT(env, valuetype0 == napi_uint8_array, "Wrong argument type. napi_uint8_array expected.");
        Base64 *object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value result = object->Encode(args[0]);
        return result;
    }
    static napi_value EncodeToStringAsync(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t requireArgc = 1;
        size_t argc = 1;
        napi_value args[1] = { nullptr };
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc >= requireArgc, "Wrong number of arguments");
        napi_typedarray_type valuetype0;
        size_t length = 0;
        void *data = nullptr;
        napi_value arraybuffer = nullptr;
        size_t byteOffset = 0;
        NAPI_CALL(env, napi_get_typedarray_info(env, args[0], &valuetype0, &length, &data, &arraybuffer, &byteOffset));
        NAPI_ASSERT(env, valuetype0 == napi_uint8_array, "Wrong argument type. napi_uint8_array expected.");
        Base64 *object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value result = object->EncodeToString(args[0]);
        return result;
    }
    static napi_value DecodeAsync(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t requireArgc = 1;
        size_t argc = 1;
        napi_value args[1] = { nullptr };
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc >= requireArgc, "Wrong number of arguments");
        napi_typedarray_type valuetype0;
        napi_valuetype valuetype1;
        size_t length = 0;
        void *data = nullptr;
        napi_value arraybuffer = nullptr;
        size_t byteOffset = 0;
        NAPI_CALL(env, napi_typeof(env, args[0], &valuetype1));
        if (valuetype1 != napi_valuetype::napi_string) {
            NAPI_CALL(env, napi_get_typedarray_info(env, args[0], &valuetype0,
                                                    &length, &data, &arraybuffer, &byteOffset));
        }
        if ((valuetype1 != napi_valuetype::napi_string) && (valuetype0 != napi_typedarray_type::napi_uint8_array)) {
            napi_throw_error(env, nullptr, "The parameter type is incorrect");
        }
        Base64 *object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value result = object->Decode(args[0]);
        return result;
    }

    // Types
    static napi_value CreateExternalType(napi_env env, napi_callback_info info)
    {
        napi_value result = nullptr;
        const char testStr[] = "test";
        napi_status status = napi_create_external(
            env, (void*)testStr,
            [](napi_env env, void* data, void* hint) {},
            (void*)testStr, &result);
        if (status != napi_ok) {
            return  NULL;
        }
        return result;
    }

    static napi_value TypesConstructor(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        void* data = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, &data));
        auto objectInfo = new Types(env);
        napi_wrap(
            env, thisVar, objectInfo,
            [](napi_env env, void* data, void* hint) {
                auto objectInfo = (Types*)data;
                if (objectInfo != nullptr) {
                    delete objectInfo;
                }
            },
            nullptr, nullptr);
        return thisVar;
    }

    static napi_value IsAnyArrayBuffer(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        Types* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value rst = object->IsAnyArrayBuffer(args);
        return rst;
    }

    static napi_value IsArrayBufferView(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        Types* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value rst = object->IsArrayBufferView(args);
        return rst;
    }

    static napi_value IsArgumentsObject(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        Types* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value rst = object->IsArgumentsObject(args);
        return rst;
    }

    static napi_value IsArrayBuffer(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        Types* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value rst = object->IsArrayBuffer(args);
        return rst;
    }

    static napi_value IsAsyncFunction(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        Types* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value rst = object->IsAsyncFunction(args);
        return rst;
    }

    static napi_value IsBigInt64Array(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        Types* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value rst = object->IsBigInt64Array(args);
        return rst;
    }

    static napi_value IsBigUint64Array(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        Types* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value rst = object->IsBigUint64Array(args);
        return rst;
    }

    static napi_value IsBooleanObject(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        Types* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value rst = object->IsBooleanObject(args);
        return rst;
    }

    static napi_value IsBoxedPrimitive(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        Types* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value rst = object->IsBoxedPrimitive(args);
        return rst;
    }

    static napi_value IsDataView(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        Types* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value rst = object->IsDataView(args);
        return rst;
    }

    static napi_value IsDate(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        Types* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value rst = object->IsDate(args);
        return rst;
    }

    static napi_value IsExternal(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        Types* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value rst = object->IsExternal(args);
        return rst;
    }

    static napi_value IsFloat32Array(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        Types* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value rst = object->IsFloat32Array(args);
        return rst;
    }

    static napi_value IsFloat64Array(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        Types* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value rst = object->IsFloat64Array(args);
        return rst;
    }

    static napi_value IsGeneratorFunction(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        Types* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value rst = object->IsGeneratorFunction(args);
        return rst;
    }

    static napi_value IsGeneratorObject(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t requireArgc = 1;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc >= requireArgc, "Wrong number of arguments");
        Types* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value result = object->IsGeneratorObject(args);
        return result;
    }

    static napi_value IsInt8Array(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t requireArgc = 1;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc >= requireArgc, "Wrong number of arguments");
        Types* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value result = object->IsInt8Array(args);
        return result;
    }

    static napi_value IsInt16Array(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t requireArgc = 1;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc >= requireArgc, "Wrong number of arguments");
        Types* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value result = object->IsInt16Array(args);
        return result;
    }

    static napi_value IsInt32Array(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t requireArgc = 1;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc >= requireArgc, "Wrong number of arguments");
        Types* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value result = object->IsInt32Array(args);
        return result;
    }

    static napi_value IsMap(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t requireArgc = 1;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc >= requireArgc, "Wrong number of arguments");
        Types* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value result = object->IsMap(args);
        return result;
    }

    static napi_value IsMapIterator(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t requireArgc = 1;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc >= requireArgc, "Wrong number of arguments");
        Types* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value result = object->IsMapIterator(args);
        return result;
    }

    static napi_value IsModuleNamespaceObject(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t requireArgc = 1;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc >= requireArgc, "Wrong number of arguments");
        Types* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value result = object->IsModuleNamespaceObject(args);
        return result;
    }

    static napi_value IsNativeError(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t requireArgc = 1;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc >= requireArgc, "Wrong number of arguments");
        Types* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value result = object->IsNativeError(args);
        return result;
    }

    static napi_value IsNumberObject(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t requireArgc = 1;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc >= requireArgc, "Wrong number of arguments");
        Types* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value result = object->IsNumberObject(args);
        return result;
    }

    static napi_value IsPromise(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t requireArgc = 1;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc >= requireArgc, "Wrong number of arguments");
        Types* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value result = object->IsPromise(args);
        return result;
    }

    static napi_value IsProxy(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t requireArgc = 1;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc >= requireArgc, "Wrong number of arguments");
        Types* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value result = object->IsProxy(args);
        return result;
    }

    static napi_value IsRegExp(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t requireArgc = 1;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc >= requireArgc, "Wrong number of arguments");
        Types* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value result = object->IsRegExp(args);
        return result;
    }

    static napi_value IsSet(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t requireArgc = 1;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc >= requireArgc, "Wrong number of arguments");
        Types* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value result = object->IsSet(args);
        return result;
    }

    static napi_value IsSetIterator(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t requireArgc = 1;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc >= requireArgc, "Wrong number of arguments");
        Types* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value result = object->IsSetIterator(args);
        return result;
    }

    static napi_value IsSharedArrayBuffer(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t requireArgc = 1;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc >= requireArgc, "Wrong number of arguments");
        Types* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value result = object->IsSharedArrayBuffer(args);
        return result;
    }

    static napi_value IsStringObject(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t requireArgc = 1;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc >= requireArgc, "Wrong number of arguments");
        Types* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value result = object->IsStringObject(args);
        return result;
    }

    static napi_value IsSymbolObject(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t requireArgc = 1;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc >= requireArgc, "Wrong number of arguments");
        Types* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value result = object->IsSymbolObject(args);
        return result;
    }

    static napi_value IsTypedArray(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t requireArgc = 1;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc >= requireArgc, "Wrong number of arguments");
        Types* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value result = object->IsTypedArray(args);
        return result;
    }

    static napi_value IsUint8Array(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t requireArgc = 1;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc >= requireArgc, "Wrong number of arguments");
        Types* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value result = object->IsUint8Array(args);
        return result;
    }

    static napi_value IsUint8ClampedArray(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t requireArgc = 1;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc >= requireArgc, "Wrong number of arguments");
        Types* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value result = object->IsUint8ClampedArray(args);
        return result;
    }

    static napi_value IsUint16Array(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t requireArgc = 1;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc >= requireArgc, "Wrong number of arguments");
        Types* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value result = object->IsUint16Array(args);
        return result;
    }

    static napi_value IsUint32Array(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t requireArgc = 1;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc >= requireArgc, "Wrong number of arguments");
        Types* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value result = object->IsUint32Array(args);
        return result;
    }

    static napi_value IsWeakMap(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t requireArgc = 1;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc >= requireArgc, "Wrong number of arguments");
        Types* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value result = object->IsWeakMap(args);
        return result;
    }

    static napi_value IsWeakSet(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t requireArgc = 1;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc >= requireArgc, "Wrong number of arguments");
        Types* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        napi_value result = object->IsWeakSet(args);
        return result;
    }

    static napi_value TypeofInit(napi_env env, napi_value exports)
    {
        const char* typeofClassName = "Types";
        napi_value typeofClass = nullptr;
        static napi_property_descriptor typeofDesc[] = {
            DECLARE_NAPI_FUNCTION("isBigInt64Array", IsBigInt64Array),
            DECLARE_NAPI_FUNCTION("isBigUint64Array", IsBigUint64Array),
            DECLARE_NAPI_FUNCTION("isBooleanObject", IsBooleanObject),
            DECLARE_NAPI_FUNCTION("isBoxedPrimitive", IsBoxedPrimitive),
            DECLARE_NAPI_FUNCTION("isAnyArrayBuffer", IsAnyArrayBuffer),
            DECLARE_NAPI_FUNCTION("isArrayBufferView", IsArrayBufferView),
            DECLARE_NAPI_FUNCTION("isArgumentsObject", IsArgumentsObject),
            DECLARE_NAPI_FUNCTION("isArrayBuffer", IsArrayBuffer),
            DECLARE_NAPI_FUNCTION("isDataView", IsDataView),
            DECLARE_NAPI_FUNCTION("isDate", IsDate),
            DECLARE_NAPI_FUNCTION("isExternal", IsExternal),
            DECLARE_NAPI_FUNCTION("isFloat32Array", IsFloat32Array),
            DECLARE_NAPI_FUNCTION("isFloat64Array", IsFloat64Array),
            DECLARE_NAPI_FUNCTION("isGeneratorFunction", IsGeneratorFunction),
            DECLARE_NAPI_FUNCTION("isGeneratorObject", IsGeneratorObject),
            DECLARE_NAPI_FUNCTION("isInt8Array", IsInt8Array),
            DECLARE_NAPI_FUNCTION("isInt16Array", IsInt16Array),
            DECLARE_NAPI_FUNCTION("isInt32Array", IsInt32Array),
            DECLARE_NAPI_FUNCTION("isMap", IsMap),
            DECLARE_NAPI_FUNCTION("isMapIterator", IsMapIterator),
            DECLARE_NAPI_FUNCTION("isModuleNamespaceObject", IsModuleNamespaceObject),
            DECLARE_NAPI_FUNCTION("isNativeError", IsNativeError),
            DECLARE_NAPI_FUNCTION("isNumberObject", IsNumberObject),
            DECLARE_NAPI_FUNCTION("isPromise", IsPromise),
            DECLARE_NAPI_FUNCTION("isProxy", IsProxy),
            DECLARE_NAPI_FUNCTION("isRegExp", IsRegExp),
            DECLARE_NAPI_FUNCTION("isSet", IsSet),
            DECLARE_NAPI_FUNCTION("isSetIterator", IsSetIterator),
            DECLARE_NAPI_FUNCTION("isSharedArrayBuffer", IsSharedArrayBuffer),
            DECLARE_NAPI_FUNCTION("isStringObject", IsStringObject),
            DECLARE_NAPI_FUNCTION("isSymbolObject", IsSymbolObject),
            DECLARE_NAPI_FUNCTION("isTypedArray", IsTypedArray),
            DECLARE_NAPI_FUNCTION("isUint8Array", IsUint8Array),
            DECLARE_NAPI_FUNCTION("isUint8ClampedArray", IsUint8ClampedArray),
            DECLARE_NAPI_FUNCTION("isUint16Array", IsUint16Array),
            DECLARE_NAPI_FUNCTION("isUint32Array", IsUint32Array),
            DECLARE_NAPI_FUNCTION("isWeakMap", IsWeakMap),
            DECLARE_NAPI_FUNCTION("isWeakSet", IsWeakSet),
            DECLARE_NAPI_FUNCTION("isAsyncFunction", IsAsyncFunction),
        };
        NAPI_CALL(env, napi_define_class(env, typeofClassName, strlen(typeofClassName), TypesConstructor,
                                         nullptr, sizeof(typeofDesc) / sizeof(typeofDesc[0]), typeofDesc,
                                         &typeofClass));
        static napi_property_descriptor desc[] = { DECLARE_NAPI_PROPERTY("Types", typeofClass) };
        NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
        return exports;
    }

    static napi_value Base64Init(napi_env env, napi_value exports)
    {
        const char *base64ClassName = "Base64";
        napi_value base64Class = nullptr;
        static napi_property_descriptor base64Desc[] = {
            DECLARE_NAPI_FUNCTION("encodeSync", EncodeBase64),
            DECLARE_NAPI_FUNCTION("encodeToStringSync", EncodeToString),
            DECLARE_NAPI_FUNCTION("decodeSync", DecodeBase64),
            DECLARE_NAPI_FUNCTION("encode", EncodeAsync),
            DECLARE_NAPI_FUNCTION("encodeToString", EncodeToStringAsync),
            DECLARE_NAPI_FUNCTION("decode", DecodeAsync),
        };
        NAPI_CALL(env, napi_define_class(env, base64ClassName, strlen(base64ClassName), Base64Constructor,
                                         nullptr, sizeof(base64Desc) / sizeof(base64Desc[0]), base64Desc,
                                         &base64Class));
        static napi_property_descriptor desc[] = {
            DECLARE_NAPI_PROPERTY("Base64", base64Class)
        };
        NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
        return exports;
    }

    static napi_value UtilInit(napi_env env, napi_value exports)
    {
        static napi_property_descriptor desc[] = {
            DECLARE_NAPI_FUNCTION("printf", Printf),
            DECLARE_NAPI_FUNCTION("geterrorstring", GetErrorString),
            DECLARE_NAPI_FUNCTION("dealwithformatstring", DealWithFormatString),
            DECLARE_NAPI_FUNCTION("createExternalType", CreateExternalType),
        };
        NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
        TextcoderInit(env, exports);
        Base64Init(env, exports);
        TypeofInit(env, exports);
        return exports;
    }

    // util module define
    static napi_module utilModule = {
        .nm_version = 1,
        .nm_flags = 0,
        .nm_filename = nullptr,
        .nm_register_func = UtilInit,
        .nm_modname = "util",
        .nm_priv = ((void*)0),
        .reserved = {0},
    };

    // util module register
    extern "C"
    __attribute__((constructor)) void RegisterModule()
    {
        napi_module_register(&utilModule);
    }

    // util JS register
    extern "C"
    __attribute__((visibility("default"))) void NAPI_util_GetJSCode(const char **buf, int *buflen)
    {
        if (buf != nullptr) {
            *buf = _binary_util_js_js_start;
        }
        if (buflen != nullptr) {
            *buflen = _binary_util_js_js_end - _binary_util_js_js_start;
        }
    }
    extern "C"
    __attribute__((visibility("default"))) void NAPI_util_GetABCCode(const char** buf, int* buflen)
    {
        if (buf != nullptr) {
            *buf = _binary_util_abc_start;
        }
        if (buflen != nullptr) {
            *buflen = _binary_util_abc_end - _binary_util_abc_start;
        }
    }
}