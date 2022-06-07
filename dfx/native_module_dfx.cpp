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

#include "napi/native_api.h"
#include "native_engine.h"
#include "napi/native_node_api.h"
#include "securec.h"
#include "utils/log.h"

namespace OHOS::Js_sys_module::Dfx {
    constexpr int NUMBER_OF_PARAMETER_TWO = 2;
    static napi_value DumpHeapSnapshot(napi_env env, napi_callback_info info)
    {
        size_t argc = NUMBER_OF_PARAMETER_TWO;
        size_t requireArgc = NUMBER_OF_PARAMETER_TWO;
        napi_get_cb_info(env, info, &argc, nullptr, nullptr, nullptr);
        NAPI_ASSERT(env, argc <= requireArgc, "Wrong number of arguments");
        napi_value argv[NUMBER_OF_PARAMETER_TWO] = { 0 };
        napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
        std::string tempStr = "";
        size_t tempStrsize = 0;
        if (napi_get_value_string_utf8(env, argv[0], nullptr, 0, &tempStrsize) != napi_ok) {
            HILOG_ERROR("can not get argv[0] size");
            return nullptr;
        }
        tempStr.reserve(tempStrsize + 1);
        tempStr.resize(tempStrsize);
        if (napi_get_value_string_utf8(env, argv[0], tempStr.data(), tempStrsize + 1, &tempStrsize) != napi_ok) {
            HILOG_ERROR("can not get argv[0] value");
            return nullptr;
        }
        std::string pathStr = tempStr;
        bool isVmMode = true;
        napi_get_value_bool(env, argv[1], &isVmMode);
        NativeEngine *engine = reinterpret_cast<NativeEngine*>(env);
        engine->DumpHeapSnapshot(pathStr, isVmMode);
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &result));
        return result;
    }

    static napi_value BuildNativeAndJsStackTrace(napi_env env, napi_callback_info info)
    {
        napi_value result = nullptr;
        NativeEngine *engine = reinterpret_cast<NativeEngine*>(env);
        std::string stackTraceResult = "";
        bool temp = engine->BuildNativeAndJsStackTrace(stackTraceResult);
        NAPI_CALL(env, napi_create_string_utf8(env, stackTraceResult.c_str(), stackTraceResult.size(), &result));
        if (temp) {
            return result;
        } else {
            return nullptr;
        }
    }

    static napi_value StartHeapTracking(napi_env env, napi_callback_info info)
    {
        size_t argc = NUMBER_OF_PARAMETER_TWO;
        size_t requireArgc = NUMBER_OF_PARAMETER_TWO;
        napi_get_cb_info(env, info, &argc, nullptr, nullptr, nullptr);
        NAPI_ASSERT(env, argc <= requireArgc, "Wrong number of arguments");
        napi_value argv[NUMBER_OF_PARAMETER_TWO] = { 0 };
        napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
        double timeInterval = 0;
        if (napi_get_value_double(env, argv[0], &timeInterval) != napi_ok) {
            HILOG_ERROR("can not get argv[0] value");
            return nullptr;
        }
        bool isVmMode = true;
        if (napi_get_value_bool(env, argv[1], &isVmMode) != napi_ok) {
            HILOG_ERROR("can not get argv[1] value");
            return nullptr;
        }
        NativeEngine *engine = reinterpret_cast<NativeEngine*>(env);
        auto startResult = engine->StartHeapTracking(timeInterval, isVmMode);
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_boolean(env, startResult, &result));
        return result;
    }

    static napi_value StopHeapTracking(napi_env env, napi_callback_info info)
    {
        size_t argc = 1;
        size_t requireArgc = 1;
        napi_get_cb_info(env, info, &argc, nullptr, nullptr, nullptr);
        NAPI_ASSERT(env, argc <= requireArgc, "Wrong number of arguments");
        napi_value argv = nullptr;
        napi_get_cb_info(env, info, &argc, &argv, nullptr, nullptr);
        std::string tempStr = "";
        size_t tempStrsize = 0;
        if (napi_get_value_string_utf8(env, argv, nullptr, 0, &tempStrsize) != napi_ok) {
            HILOG_ERROR("can not get argv size");
            return nullptr;
        }
        tempStr.reserve(tempStrsize);
        tempStr.resize(tempStrsize);
        if (napi_get_value_string_utf8(env, argv, tempStr.data(), tempStrsize + 1, &tempStrsize) != napi_ok) {
            HILOG_ERROR("can not get argv value");
            return nullptr;
        }
        std::string filePath = tempStr;
        NativeEngine *engine = reinterpret_cast<NativeEngine*>(env);
        auto stopResult = engine->StopHeapTracking(filePath);
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_boolean(env, stopResult, &result));
        return result;
    }

    static napi_value PrintStatisticResult(napi_env env, napi_callback_info info)
    {
        NativeEngine *engine = reinterpret_cast<NativeEngine*>(env);
        engine->PrintStatisticResult();
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &result));
        return result;
    }

    static napi_value StartRuntimeStat(napi_env env, napi_callback_info info)
    {
        NativeEngine *engine = reinterpret_cast<NativeEngine*>(env);
        engine->StartRuntimeStat();
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &result));
        return result;
    }

    static napi_value StopRuntimeStat(napi_env env, napi_callback_info info)
    {
        NativeEngine *engine = reinterpret_cast<NativeEngine*>(env);
        engine->StopRuntimeStat();
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &result));
        return result;
    }

    static napi_value GetArrayBufferSize(napi_env env, napi_callback_info info)
    {
        NativeEngine *engine = reinterpret_cast<NativeEngine*>(env);
        auto value = static_cast<uint32_t>(engine->GetArrayBufferSize());
        napi_value result = nullptr;
        NAPI_CALL(env, napi_create_uint32(env, value, &result));
        return result;
    }

    static napi_value GetHeapTotalSize(napi_env env, napi_callback_info info)
    {
        NativeEngine *engine = reinterpret_cast<NativeEngine*>(env);
        auto value = static_cast<uint32_t>(engine->GetHeapTotalSize());
        napi_value result = nullptr;
        NAPI_CALL(env, napi_create_uint32(env, value, &result));
        return result;
    }

    static napi_value GetHeapUsedSize(napi_env env, napi_callback_info info)
    {
        NativeEngine *engine = reinterpret_cast<NativeEngine*>(env);
        auto value = static_cast<uint32_t>(engine->GetHeapUsedSize());
        napi_value result = nullptr;
        NAPI_CALL(env, napi_create_uint32(env, value, &result));
        return result;
    }

    static napi_value DfxInit(napi_env env, napi_value exports)
    {
        static napi_property_descriptor desc[] = {
            DECLARE_NAPI_FUNCTION("dumpHeapSnapshot", DumpHeapSnapshot),
            DECLARE_NAPI_FUNCTION("buildNativeAndJsStackTrace", BuildNativeAndJsStackTrace),
            DECLARE_NAPI_FUNCTION("startHeapTracking", StartHeapTracking),
            DECLARE_NAPI_FUNCTION("stopHeapTracking", StopHeapTracking),
            DECLARE_NAPI_FUNCTION("printStatisticResult", PrintStatisticResult),
            DECLARE_NAPI_FUNCTION("startRuntimeStat", StartRuntimeStat),
            DECLARE_NAPI_FUNCTION("stopRuntimeStat", StopRuntimeStat),
            DECLARE_NAPI_FUNCTION("getArrayBufferSize", GetArrayBufferSize),
            DECLARE_NAPI_FUNCTION("getHeapTotalSize", GetHeapTotalSize),
            DECLARE_NAPI_FUNCTION("getHeapUsedSize", GetHeapUsedSize),
        };
        NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
        return exports;
    }

    // dfx module define
    static napi_module dfxModule = {
        .nm_version = 1,
        .nm_flags = 0,
        .nm_filename = nullptr,
        .nm_register_func = DfxInit,
        .nm_modname = "dfx",
        .nm_priv = reinterpret_cast<void*>(0),
        .reserved = {0},
    };

    // dfx module register
    extern "C"
    __attribute__((constructor)) void RegisterModule()
    {
        napi_module_register(&dfxModule);
    }
} // namespace OHOS::Js_sys_module::Dfx
