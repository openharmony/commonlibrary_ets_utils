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

#ifndef JS_CONCURRENT_MODULE_COMMON_HELPER_NAPI_HELPER_H
#define JS_CONCURRENT_MODULE_COMMON_HELPER_NAPI_HELPER_H

#include <uv.h>
#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace Commonlibrary::Concurrent::Common::Helper {
class NapiHelper {
public:
    static bool IsString(napi_env env, napi_value value);
    static bool IsTypeForNapiValue(napi_env env, napi_value param, napi_valuetype expectType);
    static bool IsNotUndefined(napi_env env, napi_value value);
    static bool IsArray(napi_env env, napi_value value);
    static bool IsFunction(napi_env env, napi_value object);
    static bool IsArrayBuffer(napi_env env, napi_value value);
    static bool IsNumber(napi_env env, napi_value value);
    static bool IsCallable(napi_env env, napi_value value);
    static bool IsCallable(napi_env env, napi_ref value);
    static bool IsAsyncFunction(napi_env env, napi_value value);
    static bool IsConcurrentFunction(napi_env env, napi_value value);
    static bool IsGeneratorFunction(napi_env env, napi_value value);
    static bool IsStrictEqual(napi_env env, napi_value lhs, napi_value rhs);
    static bool IsSendable(napi_env env, napi_value value);
    static size_t GetCallbackInfoArgc(napi_env env, napi_callback_info cbInfo);
    static napi_value GetNamePropertyInParentPort(napi_env env, napi_ref parentPort, const char* name);
    static void SetNamePropertyInGlobal(napi_env env, const char* name, napi_value value);
    static napi_value GetUndefinedValue(napi_env env);
    static bool IsObject(napi_env env, napi_value value);
    static char* GetString(napi_env env, napi_value value);
    static napi_value CreateBooleanValue(napi_env env, bool value);
    static napi_value GetGlobalObject(napi_env env);
    static napi_ref CreateReference(napi_env env, napi_value value, uint32_t refcount);
    static napi_value CreateUint32(napi_env env, uint32_t value);
    static uv_loop_t* GetLibUV(napi_env env);
    static napi_value GetReferenceValue(napi_env env, napi_ref ref);
    static void DeleteReference(napi_env env, napi_ref ref);
    static napi_value GetNameProperty(napi_env env, napi_value obj, const char* name);
    static bool HasNameProperty(napi_env env, napi_value obj, const char* name);
    static bool GetBooleanValue(napi_env env, napi_value value);
    static bool StrictEqual(napi_env env, napi_value value, napi_value cmpValue);
    static std::string GetConstructorName(napi_env env, napi_value object);
    static napi_value CreateObject(napi_env env);
    static napi_value CreatePromise(napi_env env, napi_deferred* deferred);
    static napi_value CreateEmptyString(napi_env env);
    static uint32_t GetArrayLength(napi_env env, napi_value array);
    static uint32_t GetUint32Value(napi_env env, napi_value value);
    static int32_t GetInt32Value(napi_env env, napi_value value);
    static bool IsExceptionPending(napi_env env);
    static std::string GetPrintString(napi_env env, napi_value value);
    static napi_value CreateUint64(napi_env env, uint64_t value);
    static uint64_t GetUint64Value(napi_env env, napi_value value, bool lossless = false);
    static napi_value GetElement(napi_env env, napi_value value, uint32_t index);
};
} // namespace Commonlibrary::Concurrent::Common::Helper
#endif // JS_CONCURRENT_MODULE_COMMON_HELPER_NAPI_HELPER_H
