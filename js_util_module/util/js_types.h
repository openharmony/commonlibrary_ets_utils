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
 
#include <cstring>
#include <sys/sysinfo.h>
#include <unistd.h>

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "native_engine/native_engine.h"

#ifndef UTIL_JS_TYPES_H_
#define UTIL_JS_TYPES_H_

namespace OHOS::Util {
    class Types {
    public:
        /**
         * Constructor of Types.
         *
         */
        explicit Types() {}

        /**
         * Destructor of Types.
         */
        virtual ~Types() {}

        /**
         * Check whether the entered value is of arraybuffer type.
         *
         * @param env NAPI environment parameters.
         * @param src Object to be tested.
         */
        napi_value IsAnyArrayBuffer(napi_env env, napi_value src) const;

        /**
         * Check whether the entered value is a built-in arraybufferview auxiliary type.
         *
         * @param env NAPI environment parameters.
         * @param src Object to be tested.
         */
        napi_value IsArrayBufferView(napi_env env, napi_value src) const;

        /**
         * Check whether the entered value is an arguments object type.
         *
         * @param env NAPI environment parameters.
         * @param src Object to be tested.
         */
        napi_value IsArgumentsObject(napi_env env, napi_value src) const;

        /**
         * Check whether the entered value is of arraybuffer type.
         *
         * @param env NAPI environment parameters.
         * @param src Object to be tested.
         */
        napi_value IsArrayBuffer(napi_env env, napi_value src) const;

        /**
         * Check whether the input value is an asynchronous function type.
         *
         * @param env NAPI environment parameters.
         * @param src Object to be tested.
         */
        napi_value IsAsyncFunction(napi_env env, napi_value src) const;

        /**
         * Check whether the entered value is a bigint64array type.
         *
         * @param env NAPI environment parameters.
         * @param src Object to be tested.
         */
        napi_value IsBigInt64Array(napi_env env, napi_value src) const;

        /**
         * Check whether the entered value is a biguint64array type.
         *
         * @param env NAPI environment parameters.
         * @param src Object to be tested.
         */
        napi_value IsBigUint64Array(napi_env env, napi_value src) const;

        /**
         * Check whether the entered value is a Boolean object type.
         *
         * @param env NAPI environment parameters.
         * @param src Object to be tested.
         */
        napi_value IsBooleanObject(napi_env env, napi_value src) const;

        /**
         * Check whether the entered value is Boolean or number or string or symbol object type.
         *
         * @param env NAPI environment parameters.
         * @param src Object to be tested.
         */
        napi_value IsBoxedPrimitive(napi_env env, napi_value src) const;

        /**
         * Check whether the entered value is of DataView type.
         *
         * @param env NAPI environment parameters.
         * @param src Object to be tested.
         */
        napi_value IsDataView(napi_env env, napi_value src) const;

        /**
         * Check whether the entered value is of type date.
         *
         * @param env NAPI environment parameters.
         * @param src Object to be tested.
         */
        napi_value IsDate(napi_env env, napi_value src) const;

        /**
         * Check whether the entered value is of type native external.
         *
         * @param env NAPI environment parameters.
         * @param src Object to be tested.
         */
        napi_value IsExternal(napi_env env, napi_value src) const;

        /**
         * Check whether the entered value is the type of float32array array.
         *
         * @param env NAPI environment parameters.
         * @param src Object to be tested.
         */
        napi_value IsFloat32Array(napi_env env, napi_value src) const;

        /**
         * Check whether the entered value is the type of float64array array.
         *
         * @param env NAPI environment parameters.
         * @param src Object to be tested.
         */
        napi_value IsFloat64Array(napi_env env, napi_value src) const;

        /**
         * Check whether the value entered is the type of generator function.
         *
         * @param env NAPI environment parameters.
         * @param src Object to be tested.
         */
        napi_value IsGeneratorFunction(napi_env env, napi_value src) const;

        /**
         * Check whether the value entered is the type of generator object.
         *
         * @param env NAPI environment parameters.
         * @param src Object to be tested.
         */
        napi_value IsGeneratorObject(napi_env env, napi_value src) const;

        /**
         * Check whether the value entered is the type of int8 array.
         *
         * @param env NAPI environment parameters.
         * @param src Object to be tested.
         */
        napi_value IsInt8Array(napi_env env, napi_value src) const;

        /**
         * Check whether the value entered is the type of int16 array.
         *
         * @param env NAPI environment parameters.
         * @param src Object to be tested.
         */
        napi_value IsInt16Array(napi_env env, napi_value src) const;

        /**
         * Check whether the value entered is the type of int32 array.
         *
         * @param env NAPI environment parameters.
         * @param src Object to be tested.
         */
        napi_value IsInt32Array(napi_env env, napi_value src) const;

        /**
         * Check whether the value entered is the type of map.
         *
         * @param env NAPI environment parameters.
         * @param src Object to be tested.
         */
        napi_value IsMap(napi_env env, napi_value src) const;

        /**
         * Check whether the entered value is the iterator type of map.
         *
         * @param env NAPI environment parameters.
         * @param src Object to be tested.
         */
        napi_value IsMapIterator(napi_env env, napi_value src) const;

        /**
         * Check whether the entered value is the module name space type of object.
         *
         * @param env NAPI environment parameters.
         * @param src Object to be tested.
         */
        napi_value IsModuleNamespaceObject(napi_env env, napi_value src) const;

        /**
         * Check whether the entered value is of type error.
         *
         * @param env NAPI environment parameters.
         * @param src Object to be tested.
         */
        napi_value IsNativeError(napi_env env, napi_value src) const;

        /**
         * Check whether the entered value is the number type of object.
         *
         * @param env NAPI environment parameters.
         * @param src Object to be tested.
         */
        napi_value IsNumberObject(napi_env env, napi_value src) const;

        /**
         * Check whether the entered value is the type of promise.
         *
         * @param env NAPI environment parameters.
         * @param src Object to be tested.
         */
        napi_value IsPromise(napi_env env, napi_value src) const;

        /**
         * Check whether the entered value is the type of proxy.
         *
         * @param env NAPI environment parameters.
         * @param src Object to be tested.
         */
        napi_value IsProxy(napi_env env, napi_value src) const;

        /**
         * Check whether the entered value is the type of regexp.
         *
         * @param env NAPI environment parameters.
         * @param src Object to be tested.
         */
        napi_value IsRegExp(napi_env env, napi_value src) const;

        /**
         * Check whether the entered value is the type of set.
         *
         * @param env NAPI environment parameters.
         * @param src Object to be tested.
         */
        napi_value IsSet(napi_env env, napi_value src) const;

        /**
         * Check whether the entered value is the iterator type of set.
         *
         * @param env NAPI environment parameters.
         * @param src Object to be tested.
         */
        napi_value IsSetIterator(napi_env env, napi_value src) const;

        /**
         * Check whether the entered value is the type of sharedarraybuffer.
         *
         * @param env NAPI environment parameters.
         * @param src Object to be tested.
         */
        napi_value IsSharedArrayBuffer(napi_env env, napi_value src) const;

        /**
         * Check whether the entered value is the string type of object.
         *
         * @param env NAPI environment parameters.
         * @param src Object to be tested.
         */
        napi_value IsStringObject(napi_env env, napi_value src) const;

        /**
         * Check whether the entered value is the symbol type of object.
         *
         * @param env NAPI environment parameters.
         * @param src Object to be tested.
         */
        napi_value IsSymbolObject(napi_env env, napi_value src) const;

        /**
         * Check whether the entered value is the type of typedarray.
         *
         * @param env NAPI environment parameters.
         * @param src Object to be tested.
         */
        napi_value IsTypedArray(napi_env env, napi_value src) const;

        /**
         * Check whether the entered value is the type of uint8array.
         *
         * @param env NAPI environment parameters.
         * @param src Object to be tested.
         */
        napi_value IsUint8Array(napi_env env, napi_value src) const;

        /**
         * Check whether the entered value is the type of uint8clampedarray.
         *
         * @param env NAPI environment parameters.
         * @param src Object to be tested.
         */
        napi_value IsUint8ClampedArray(napi_env env, napi_value src) const;

        /**
         * Check whether the entered value is the type of uint16array.
         *
         * @param env NAPI environment parameters.
         * @param src Object to be tested.
         */
        napi_value IsUint16Array(napi_env env, napi_value src) const;

        /**
         * Check whether the entered value is the type of uint32array.
         *
         * @param env NAPI environment parameters.
         * @param src Object to be tested.
         */
        napi_value IsUint32Array(napi_env env, napi_value src) const;

        /**
         * Check whether the entered value is the type of weakmap.
         *
         * @param env NAPI environment parameters.
         * @param src Object to be tested.
         */
        napi_value IsWeakMap(napi_env env, napi_value src) const;

        /**
         * Check whether the entered value is the type of weakset.
         *
         * @param env NAPI environment parameters.
         * @param src Object to be tested.
         */
        napi_value IsWeakSet(napi_env env, napi_value src) const;
    };
}
#endif // UTIL_JS_TYPES_H_
