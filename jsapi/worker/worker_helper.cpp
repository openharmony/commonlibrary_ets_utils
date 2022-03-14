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

#include "worker_helper.h"

#include "native_engine/native_value.h"

namespace OHOS::CCRuntime::Worker {
const static int32_t MAXCHARLENGTH = 200;

bool NapiValueHelp::IsString(napi_value value)
{
    auto valNative = reinterpret_cast<NativeValue*>(value);
    return valNative == nullptr ? false : valNative->TypeOf() == NATIVE_STRING;
}

bool NapiValueHelp::IsArray(napi_value value)
{
    auto valNative = reinterpret_cast<NativeValue*>(value);
    return valNative == nullptr ? false : valNative->IsArray();
}

bool NapiValueHelp::IsConstructor(napi_env env, napi_callback_info cbInfo)
{
    napi_value* funcObj = nullptr;
    napi_get_new_target(env, cbInfo, funcObj);
    return funcObj != nullptr;
}

size_t NapiValueHelp::GetCallbackInfoArgc(napi_env env, napi_callback_info cbInfo)
{
    size_t argc = 0;
    napi_get_cb_info(env, cbInfo, &argc, nullptr, nullptr, nullptr);
    return argc;
}

napi_value NapiValueHelp::GetNamePropertyInParentPort(napi_env env, napi_ref parentPort, const char* name)
{
    napi_value obj = nullptr;
    napi_get_reference_value(env, parentPort, &obj);

    napi_value value = nullptr;
    napi_get_named_property(env, obj, name, &value);

    return value;
}

napi_value NapiValueHelp::GetUndefinedValue(napi_env env)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    return result;
}

bool NapiValueHelp::IsCallable(napi_env env, napi_value value)
{
    bool result = false;
    napi_is_callable(env, value, &result);
    return result;
}

bool NapiValueHelp::IsCallable(napi_env env, napi_ref value)
{
    napi_value obj = nullptr;
    napi_get_reference_value(env, value, &obj);
    if (obj == nullptr) {
        return false;
    }
    return IsCallable(env, obj);
}

void NapiValueHelp::SetNamePropertyInGlobal(napi_env env, const char* name, napi_value value)
{
    napi_value object = nullptr;
    napi_get_global(env, &object);
    napi_set_named_property(env, object, name, value);
}

bool NapiValueHelp::IsObject(napi_value value)
{
    auto nativeValue = reinterpret_cast<NativeValue*>(value);
    return nativeValue->TypeOf() == NATIVE_OBJECT;
}

char* NapiValueHelp::GetString(napi_env env, napi_value value)
{
    size_t bufferSize = 0;
    size_t strLength = 0;
    napi_get_value_string_utf8(env, value, nullptr, 0, &bufferSize);
    if (bufferSize > MAXCHARLENGTH) {
        return nullptr;
    }
    char* buffer = new char[bufferSize + 1] { 0 };
    napi_get_value_string_utf8(env, value, buffer, bufferSize + 1, &strLength);
    return buffer;
}

napi_value NapiValueHelp::GetBooleanValue(napi_env env, bool value)
{
    napi_value result = nullptr;
    napi_get_boolean(env, value, &result);
    return result;
}
} // namespace OHOS::CCRuntime::Worker