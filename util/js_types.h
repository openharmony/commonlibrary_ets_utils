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
         * @param env NAPI environment parameters.
         */
        explicit Types(napi_env env);

        /**
         * Destructor of Types.
         */
        virtual ~Types() {}

        /**
         * Check whether the entered value is of arraybuffer type.
         *
         * @param src Object to be tested.
         */
        napi_value IsAnyArrayBuffer(napi_value src) const;

        /**
         * Check whether the entered value is a built-in arraybufferview auxiliary type.
         *
         * @param src Object to be tested.
         */
        napi_value IsArrayBufferView(napi_value src) const;

        /**
         * Check whether the entered value is an arguments object type.
         *
         * @param src Object to be tested.
         */
        napi_value IsArgumentsObject(napi_value src) const;

        /**
         * Check whether the entered value is of arraybuffer type.
         *
         * @param src Object to be tested.
         */
        napi_value IsArrayBuffer(napi_value src) const;

        /**
         * Check whether the input value is an asynchronous function type.
         *
         * @param src Object to be tested.
         */
        napi_value IsAsyncFunction(napi_value src) const;

        /**
         * Check whether the entered value is a bigint64array type.
         *
         * @param src Object to be tested.
         */
        napi_value IsBigInt64Array(napi_value src) const;

        /**
         * Check whether the entered value is a biguint64array type.
         *
         * @param src Object to be tested.
         */
        napi_value IsBigUint64Array(napi_value src) const;

        /**
         * Check whether the entered value is a Boolean object type.
         *
         * @param src Object to be tested.
         */
        napi_value IsBooleanObject(napi_value src) const;

        /**
         * Check whether the entered value is Boolean or number or string or symbol object type.
         *
         * @param src Object to be tested.
         */
        napi_value IsBoxedPrimitive(napi_value src) const;

        /**
         * Check whether the entered value is of DataView type.
         *
         * @param src Object to be tested.
         */
        napi_value IsDataView(napi_value src) const;

        /**
         * Check whether the entered value is of type date.
         *
         * @param src Object to be tested.
         */
        napi_value IsDate(napi_value src) const;

        /**
         * Check whether the entered value is of type native external.
         *
         * @param src Object to be tested.
         */
        napi_value IsExternal(napi_value src) const;

        /**
         * Check whether the entered value is the type of float32array array.
         *
         * @param src Object to be tested.
         */
        napi_value IsFloat32Array(napi_value src) const;

        /**
         * Check whether the entered value is the type of float64array array.
         *
         * @param src Object to be tested.
         */
        napi_value IsFloat64Array(napi_value src) const;

        /**
         * Check whether the value entered is the type of generator function.
         *
         * @param src Object to be tested.
         */
        napi_value IsGeneratorFunction(napi_value src) const;

        /**
         * Check whether the value entered is the type of generator object.
         *
         * @param src Object to be tested.
         */
        napi_value IsGeneratorObject(napi_value src) const;

        /**
         * Check whether the value entered is the type of int8 array.
         *
         * @param src Object to be tested.
         */
        napi_value IsInt8Array(napi_value src) const;

        /**
         * Check whether the value entered is the type of int16 array.
         *
         * @param src Object to be tested.
         */
        napi_value IsInt16Array(napi_value src) const;

        /**
         * Check whether the value entered is the type of int32 array.
         *
         * @param src Object to be tested.
         */
        napi_value IsInt32Array(napi_value src) const;

        /**
         * Check whether the value entered is the type of map.
         *
         * @param src Object to be tested.
         */
        napi_value IsMap(napi_value src) const;

        /**
         * Check whether the entered value is the iterator type of map.
         *
         * @param src Object to be tested.
         */
        napi_value IsMapIterator(napi_value src) const;

        /**
         * Check whether the entered value is the module name space type of object.
         *
         * @param src Object to be tested.
         */
        napi_value IsModuleNamespaceObject(napi_value src) const;

        /**
         * Check whether the entered value is of type error.
         *
         * @param src Object to be tested.
         */
        napi_value IsNativeError(napi_value src) const;

        /**
         * Check whether the entered value is the number type of object.
         *
         * @param src Object to be tested.
         */
        napi_value IsNumberObject(napi_value src) const;

        /**
         * Check whether the entered value is the type of promise.
         *
         * @param src Object to be tested.
         */
        napi_value IsPromise(napi_value src) const;

        /**
         * Check whether the entered value is the type of proxy.
         *
         * @param src Object to be tested.
         */
        napi_value IsProxy(napi_value src) const;

        /**
         * Check whether the entered value is the type of regexp.
         *
         * @param src Object to be tested.
         */
        napi_value IsRegExp(napi_value src) const;

        /**
         * Check whether the entered value is the type of set.
         *
         * @param src Object to be tested.
         */
        napi_value IsSet(napi_value src) const;

        /**
         * Check whether the entered value is the iterator type of set.
         *
         * @param src Object to be tested.
         */
        napi_value IsSetIterator(napi_value src) const;

        /**
         * Check whether the entered value is the type of sharedarraybuffer.
         *
         * @param src Object to be tested.
         */
        napi_value IsSharedArrayBuffer(napi_value src) const;

        /**
         * Check whether the entered value is the string type of object.
         *
         * @param src Object to be tested.
         */
        napi_value IsStringObject(napi_value src) const;

        /**
         * Check whether the entered value is the symbol type of object.
         *
         * @param src Object to be tested.
         */
        napi_value IsSymbolObject(napi_value src) const;

        /**
         * Check whether the entered value is the type of typedarray.
         *
         * @param src Object to be tested.
         */
        napi_value IsTypedArray(napi_value src) const;

        /**
         * Check whether the entered value is the type of uint8array.
         *
         * @param src Object to be tested.
         */
        napi_value IsUint8Array(napi_value src) const;

        /**
         * Check whether the entered value is the type of uint8clampedarray.
         *
         * @param src Object to be tested.
         */
        napi_value IsUint8ClampedArray(napi_value src) const;

        /**
         * Check whether the entered value is the type of uint16array.
         *
         * @param src Object to be tested.
         */
        napi_value IsUint16Array(napi_value src) const;

        /**
         * Check whether the entered value is the type of uint32array.
         *
         * @param src Object to be tested.
         */
        napi_value IsUint32Array(napi_value src) const;

        /**
         * Check whether the entered value is the type of weakmap.
         *
         * @param src Object to be tested.
         */
        napi_value IsWeakMap(napi_value src) const;

        /**
         * Check whether the entered value is the type of weakset.
         *
         * @param src Object to be tested.
         */
        napi_value IsWeakSet(napi_value src) const;

    private:
        napi_env env_;
    };
}
#endif // UTIL_JS_TYPES_H_
