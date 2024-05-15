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

#include "napi_helper.h"

#include "native_engine/native_value.h"

namespace Commonlibrary::Concurrent::Common::Helper {
static constexpr uint32_t MAX_CHAR_LENGTH = 1024;

bool NapiHelper::IsTypeForNapiValue(napi_env env, napi_value param, napi_valuetype expectType)
{
    napi_valuetype valueType = napi_undefined;
    if (param == nullptr) {
        return false;
    }
    if (napi_typeof(env, param, &valueType) != napi_ok) {
        return false;
    }

    return valueType == expectType;
}

bool NapiHelper::IsString(napi_env env, napi_value value)
{
    return IsTypeForNapiValue(env, value, napi_string);
}

bool NapiHelper::IsNotUndefined(napi_env env, napi_value value)
{
    if (value == nullptr) {
        return false;
    }
    napi_valuetype valueType = napi_undefined;
    if (napi_typeof(env, value, &valueType) != napi_ok) {
        return false;
    }

    return valueType != napi_undefined;
}

bool NapiHelper::IsArray(napi_env env, napi_value value)
{
    bool isArray = false;
    if (value == nullptr) {
        return false;
    }
    if (napi_is_array(env, value, &isArray) != napi_ok) {
        return false;
    }

    return isArray;
}

bool NapiHelper::IsFunction(napi_env env, napi_value object)
{
    return IsTypeForNapiValue(env, object, napi_function);
}

bool NapiHelper::IsArrayBuffer(napi_env env, napi_value value)
{
    bool result = false;
    if (value == nullptr) {
        return false;
    }
    if (napi_is_arraybuffer(env, value, &result) != napi_ok) {
        return false;
    }

    return result;
}

bool NapiHelper::IsNumber(napi_env env, napi_value value)
{
    return IsTypeForNapiValue(env, value, napi_number);
}

size_t NapiHelper::GetCallbackInfoArgc(napi_env env, napi_callback_info cbInfo)
{
    size_t argc = 0;
    napi_get_cb_info(env, cbInfo, &argc, nullptr, nullptr, nullptr);
    return argc;
}

napi_value NapiHelper::GetNamePropertyInParentPort(napi_env env, napi_ref parentPort, const char* name)
{
    napi_value obj = nullptr;
    napi_get_reference_value(env, parentPort, &obj);

    napi_value value = nullptr;
    napi_get_named_property(env, obj, name, &value);

    return value;
}

napi_value NapiHelper::GetUndefinedValue(napi_env env)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    return result;
}

bool NapiHelper::IsCallable(napi_env env, napi_value value)
{
    bool result = false;
    napi_is_callable(env, value, &result);
    return result;
}

bool NapiHelper::IsCallable(napi_env env, napi_ref value)
{
    napi_value obj = nullptr;
    napi_get_reference_value(env, value, &obj);
    if (obj == nullptr) {
        return false;
    }
    return IsCallable(env, obj);
}

bool NapiHelper::IsAsyncFunction(napi_env env, napi_value value)
{
    bool res = false;
    napi_is_async_function(env, value, &res);
    return res;
}

bool NapiHelper::IsConcurrentFunction(napi_env env, napi_value value)
{
    bool res = false;
    napi_is_concurrent_function(env, value, &res);
    return res;
}

bool NapiHelper::IsGeneratorFunction(napi_env env, napi_value value)
{
    bool res = false;
    napi_is_generator_function(env, value, &res);
    return res;
}

bool NapiHelper::IsStrictEqual(napi_env env, napi_value lhs, napi_value rhs)
{
    bool res = false;
    napi_strict_equals(env, lhs, rhs, &res);
    return res;
}

bool NapiHelper::IsSendable(napi_env env, napi_value value)
{
    bool res = false;
    napi_is_sendable(env, value, &res);
    return res;
}

void NapiHelper::SetNamePropertyInGlobal(napi_env env, const char* name, napi_value value)
{
    napi_value object = nullptr;
    napi_get_global(env, &object);
    napi_set_named_property(env, object, name, value);
}

bool NapiHelper::IsObject(napi_env env, napi_value value)
{
    return IsTypeForNapiValue(env, value, napi_object);
}

char* NapiHelper::GetString(napi_env env, napi_value value)
{
    size_t bufferSize = 0;
    size_t strLength = 0;
    napi_get_value_string_utf8(env, value, nullptr, 0, &bufferSize);
    if (bufferSize > MAX_CHAR_LENGTH) {
        bufferSize = MAX_CHAR_LENGTH;
    }
    char* buffer = new char[bufferSize + 1] { 0 };
    napi_get_value_string_utf8(env, value, buffer, bufferSize + 1, &strLength);
    return buffer;
}

napi_value NapiHelper::CreateBooleanValue(napi_env env, bool value)
{
    napi_value result = nullptr;
    napi_get_boolean(env, value, &result);
    return result;
}

napi_value NapiHelper::GetGlobalObject(napi_env env)
{
    napi_value object = nullptr;
    napi_get_global(env, &object);
    return object;
}

napi_ref NapiHelper::CreateReference(napi_env env, napi_value value, uint32_t refcount)
{
    napi_ref callback = nullptr;
    napi_create_reference(env, value, refcount, &callback);
    return callback;
}

napi_value NapiHelper::CreateUint32(napi_env env, uint32_t value)
{
    napi_value result = nullptr;
    napi_create_uint32(env, value, &result);
    return result;
}

uv_loop_t* NapiHelper::GetLibUV(napi_env env)
{
    uv_loop_t* loop;
    napi_get_uv_event_loop(env, &loop);
    return loop;
}

napi_value NapiHelper::GetReferenceValue(napi_env env, napi_ref ref)
{
    napi_value result = nullptr;
    napi_get_reference_value(env, ref, &result);
    return result;
}

void NapiHelper::DeleteReference(napi_env env, napi_ref ref)
{
    napi_delete_reference(env, ref);
}

napi_value NapiHelper::GetNameProperty(napi_env env, napi_value obj, const char* name)
{
    napi_value result = nullptr;
    napi_get_named_property(env, obj, name, &result);
    return result;
}

bool NapiHelper::HasNameProperty(napi_env env, napi_value obj, const char* name)
{
    bool result = false;
    napi_has_named_property(env, obj, name, &result);
    return result;
}

bool NapiHelper::GetBooleanValue(napi_env env, napi_value value)
{
    bool result = false;
    napi_get_value_bool(env, value, &result);
    return result;
}

bool NapiHelper::StrictEqual(napi_env env, napi_value value, napi_value cmpValue)
{
    bool isEqual = false;
    napi_strict_equals(env, value, cmpValue, &isEqual);
    return isEqual;
}

std::string NapiHelper::GetConstructorName(napi_env env, napi_value object)
{
    while (IsNotUndefined(env, object)) {
        napi_value func = nullptr;
        napi_get_own_property_descriptor(env, object, "constructor", &func);
        bool isInstanceof = false;
        napi_instanceof(env, object, func, &isInstanceof);
        if (IsNotUndefined(env, func) && isInstanceof) {
            napi_value ctorName = nullptr;
            napi_get_own_property_descriptor(env, func, "name", &ctorName);
            std::string name = GetPrintString(env, ctorName);
            if (name.size() > 0) {
                return name;
            }
        }
        napi_value result = nullptr;
        napi_get_prototype(env, object, &result);
        object = result;
    }
    return "";
}

napi_value NapiHelper::CreateObject(napi_env env)
{
    napi_value obj = nullptr;
    napi_create_object(env, &obj);
    return obj;
}

napi_value NapiHelper::CreatePromise(napi_env env, napi_deferred* deferred)
{
    napi_value promise = nullptr;
    napi_create_promise(env, deferred, &promise);
    return promise;
}

napi_value NapiHelper::CreateEmptyString(napi_env env)
{
    napi_value str = nullptr;
    const char* name = "";
    napi_create_string_utf8(env, name, 0, &str);
    return str;
}

uint32_t NapiHelper::GetArrayLength(napi_env env, napi_value array)
{
    uint32_t arrayLength = 0;
    napi_get_array_length(env, array, &arrayLength);
    return arrayLength;
}

uint32_t NapiHelper::GetUint32Value(napi_env env, napi_value value)
{
    uint32_t result = 0;
    napi_get_value_uint32(env, value, &result);
    return result;
}

int32_t NapiHelper::GetInt32Value(napi_env env, napi_value value)
{
    int32_t result = 0;
    napi_get_value_int32(env, value, &result);
    return result;
}

bool NapiHelper::IsExceptionPending(napi_env env)
{
    bool isExceptionPending = false;
    napi_is_exception_pending(env, &isExceptionPending);
    return isExceptionPending;
}

std::string NapiHelper::GetPrintString(napi_env env, napi_value value)
{
    std::string str;
    if (!IsTypeForNapiValue(env, value, napi_string)) {
        napi_value strValue = nullptr;
        if (napi_coerce_to_string(env, value, &strValue) != napi_ok) {
            return str;
        }
        value = strValue;
    }
    napi_get_print_string(env, value, str);
    return str;
}

napi_value NapiHelper::CreateUint64(napi_env env, uint64_t value)
{
    napi_value result;
    napi_create_bigint_uint64(env, value, &result);
    return result;
}

uint64_t NapiHelper::GetUint64Value(napi_env env, napi_value value, bool lossless)
{
    uint64_t result = 0;
    napi_get_value_bigint_uint64(env, value, &result, &lossless);
    return result;
}

napi_value NapiHelper::GetElement(napi_env env, napi_value value, uint32_t index)
{
    napi_value result;
    napi_get_element(env, value, index, &result);
    return result;
}
} // namespace Commonlibrary::Concurrent::Common::Helper