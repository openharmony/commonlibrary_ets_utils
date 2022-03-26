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

#ifndef JSAPI_WORKER_WORKER_HELPER_H_
#define JSAPI_WORKER_WORKER_HELPER_H_

#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS::CCRuntime::Worker {
class DereferenceHelp {
public:
    template<typename Inner, typename Outer>
    static Outer* DereferenceOf(const Inner Outer::*field, const Inner* pointer)
    {
        if (field != nullptr && pointer != nullptr) {
            auto fieldOffset = reinterpret_cast<uintptr_t>(&(static_cast<Outer*>(0)->*field));
            auto outPointer = reinterpret_cast<Outer*>(reinterpret_cast<uintptr_t>(pointer) - fieldOffset);
            return outPointer;
        }
        return nullptr;
    }
};

class NapiValueHelp {
public:
    static bool IsString(napi_value value);
    static bool IsArray(napi_value value);
    static bool IsConstructor(napi_env env, napi_callback_info cbInfo);
    static bool IsCallable(napi_env env, napi_value value);
    static bool IsCallable(napi_env env, napi_ref value);
    static size_t GetCallbackInfoArgc(napi_env env, napi_callback_info cbInfo);
    static napi_value GetNamePropertyInParentPort(napi_env env, napi_ref parentPort, const char* name);
    static void SetNamePropertyInGlobal(napi_env env, const char* name, napi_value value);
    static napi_value GetUndefinedValue(napi_env env);
    static bool IsObject(napi_value value);
    static char* GetString(napi_env env, napi_value value);
    static napi_value GetBooleanValue(napi_env env, bool value);
};

class CloseHelp {
public:
    template<typename T>
    static void DeletePointer(const T* value, bool isArray)
    {
        if (value == nullptr) {
            return;
        }
        if (isArray) {
            delete[] value;
        } else {
            delete value;
        }
    }
};

template<typename T>
class ObjectScope {
public:
    ObjectScope(const T* data, bool isArray) : data_(data), isArray_(isArray) {}
    ~ObjectScope()
    {
        if (data_ == nullptr) {
            return;
        }
        if (isArray_) {
            delete[] data_;
        } else {
            delete data_;
        }
    }

private:
    const T* data_;
    bool isArray_;
};
} // namespace OHOS::CCRuntime::Worker
#endif // JSAPI_WORKER_WORKER_HELPER_H_
