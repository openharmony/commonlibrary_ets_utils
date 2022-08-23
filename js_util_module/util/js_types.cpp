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

#include "js_types.h"
#include <cstring>
#include <sys/types.h>
#include "securec.h"
#include "utils/log.h"

namespace OHOS::Util {
    napi_value Types::IsAnyArrayBuffer(napi_env env, napi_value src) const
    {
        bool flag = false;
        bool rstFlag = false;
        napi_value rst = nullptr;
        napi_status napiRst = napi_is_arraybuffer(env, src, &rstFlag);
        if (napiRst == napi_ok && rstFlag) {
            flag = true;
            napi_get_boolean(env, flag, &rst);
            return rst;
        }
        napiRst = napi_is_shared_array_buffer(env, src, &rstFlag);
        if (napiRst == napi_ok && rstFlag) {
            flag = true;
        }
        napi_get_boolean(env, flag, &rst);
        return rst;
    }

    napi_value Types::IsArrayBufferView(napi_env env, napi_value src) const
    {
        napi_valuetype valuetype = napi_undefined;
        napi_value rst = nullptr;
        bool flag = false;
        napi_status rstStatus = napi_typeof(env, src, &valuetype);
        if ((valuetype != napi_valuetype::napi_object) || (rstStatus != napi_ok)) {
            napi_get_boolean(env, flag, &rst);
            return rst;
        }
        bool rstFlag = false;
        napi_status napiRst = napi_is_dataview(env, src, &rstFlag);
        if (napiRst == napi_ok && rstFlag) {
            napi_get_boolean(env, rstFlag, &rst);
            return rst;
        }
        napi_typedarray_type type = napi_int8_array;
        size_t byteOffset = 0;
        size_t length = 0;
        void* resultData = nullptr;
        napi_value resultBuffer = nullptr;
        napi_status rstSta = napi_get_typedarray_info(env, src, &type, &length,
                                                      &resultData, &resultBuffer, &byteOffset);
        if (rstSta == napi_ok) {
            switch (type) {
                case napi_typedarray_type::napi_int8_array:
                case napi_typedarray_type::napi_uint8_array:
                case napi_typedarray_type::napi_uint8_clamped_array:
                case napi_typedarray_type::napi_int16_array:
                case napi_typedarray_type::napi_uint16_array:
                case napi_typedarray_type::napi_int32_array:
                case napi_typedarray_type::napi_uint32_array:
                case napi_typedarray_type::napi_float32_array:
                case napi_typedarray_type::napi_float64_array:
                    flag = true;
                    break;
                default:
                    flag = false;
                    break;
            }
        }
        napi_get_boolean(env, flag, &rst);
        return rst;
    }

    napi_value Types::IsArgumentsObject(napi_env env, napi_value src) const
    {
        napi_valuetype result = napi_undefined;
        bool flag = false;
        napi_typeof(env, src, &result);
        if (result == napi_object) {
            NAPI_CALL(env, napi_is_arguments_object(env, src, &flag));
        }
        napi_value rst = nullptr;
        napi_get_boolean(env, flag, &rst);
        return rst;
    }

    napi_value Types::IsArrayBuffer(napi_env env, napi_value src) const
    {
        bool flag = false;
        NAPI_CALL(env, napi_is_arraybuffer(env, src, &flag));
        napi_value rst = nullptr;
        napi_get_boolean(env, flag, &rst);
        return rst;
    }

    napi_value Types::IsAsyncFunction(napi_env env, napi_value src) const
    {
        napi_valuetype result = napi_undefined;
        bool flag = false;
        napi_typeof(env, src, &result);
        if (result == napi_function) {
            NAPI_CALL(env, napi_is_async_function(env, src, &flag));
        }
        napi_value rst = nullptr;
        napi_get_boolean(env, flag, &rst);
        return rst;
    }

    napi_value Types::IsBigInt64Array(napi_env env, napi_value src) const
    {
        bool flag = false;
        NAPI_CALL(env, napi_is_big_int64_array(env, src, &flag));
        napi_value rst = nullptr;
        napi_get_boolean(env, flag, &rst);
        return rst;
    }

    napi_value Types::IsBigUint64Array(napi_env env, napi_value src) const
    {
        bool flag = false;
        NAPI_CALL(env, napi_is_big_uint64_array(env, src, &flag));
        napi_value rst = nullptr;
        napi_get_boolean(env, flag, &rst);
        return rst;
    }

    napi_value Types::IsBooleanObject(napi_env env, napi_value src) const
    {
        napi_valuetype result = napi_undefined;
        napi_typeof(env, src, &result);
        bool flag = false;
        if (result == napi_object) {
            NAPI_CALL(env, napi_is_boolean_object(env, src, &flag));
        }
        napi_value rst = nullptr;
        napi_get_boolean(env, flag, &rst);
        return rst;
    }

    napi_value Types::IsBoxedPrimitive(napi_env env, napi_value src) const
    {
        bool flag = false;
        bool rstNum = false;
        bool rstStr = false;
        bool rstBool = false;
        bool rstSym = false;
        NAPI_CALL(env, napi_get_value_bool(env, IsNumberObject(env, src), &rstNum));
        NAPI_CALL(env, napi_get_value_bool(env, IsStringObject(env, src), &rstStr));
        NAPI_CALL(env, napi_get_value_bool(env, IsBooleanObject(env, src), &rstBool));
        NAPI_CALL(env, napi_get_value_bool(env, IsSymbolObject(env, src), &rstSym));
        if (rstNum || rstStr || rstBool || rstSym) {
            flag = true;
        }
        napi_value result = nullptr;
        napi_get_boolean(env, flag, &result);
        return result;
    }

    napi_value Types::IsDataView(napi_env env, napi_value src) const
    {
        bool flag = false;
        NAPI_CALL(env, napi_is_dataview(env, src, &flag));
        napi_value rst = nullptr;
        napi_get_boolean(env, flag, &rst);
        return rst;
    }

    napi_value Types::IsDate(napi_env env, napi_value src) const
    {
        bool flag = false;
        NAPI_CALL(env, napi_is_date(env, src, &flag));
        napi_value rst = nullptr;
        napi_get_boolean(env, flag, &rst);
        return rst;
    }

    napi_value Types::IsExternal(napi_env env, napi_value src) const
    {
        napi_valuetype type = napi_undefined;
        bool flag = false;
        napi_typeof(env, src, &type);
        if (type == napi_valuetype::napi_external) {
            flag = true;
        }
        napi_value rst = nullptr;
        napi_get_boolean(env, flag, &rst);
        return rst;
    }

    napi_value Types::IsFloat32Array(napi_env env, napi_value src) const
    {
        napi_valuetype valuetype = napi_undefined;
        bool flag = false;
        napi_value result = nullptr;
        napi_typeof(env, src, &valuetype);
        if (valuetype == napi_valuetype::napi_object) {
            napi_typedarray_type type = napi_int8_array;
            size_t byteOffset = 0;
            size_t length = 0;
            void* resultData = nullptr;
            napi_value resultBuffer = nullptr;
            napi_get_typedarray_info(env, src, &type, &length,
                                     &resultData, &resultBuffer, &byteOffset);
            if (type == napi_typedarray_type::napi_float32_array) {
                flag = true;
            }
        }
        napi_get_boolean(env, flag, &result);
        return result;
    }

    napi_value Types::IsFloat64Array(napi_env env, napi_value src) const
    {
        napi_valuetype valuetype = napi_undefined;
        bool flag = false;
        napi_value result = nullptr;
        napi_typeof(env, src, &valuetype);
        if (valuetype == napi_valuetype::napi_object) {
            napi_typedarray_type type = napi_int8_array;
            size_t byteOffset = 0;
            size_t length = 0;
            void* resultData = nullptr;
            napi_value resultBuffer = nullptr;
            napi_get_typedarray_info(env, src, &type, &length,
                                     &resultData, &resultBuffer, &byteOffset);
            if (type == napi_typedarray_type::napi_float64_array) {
                flag = true;
            }
        }
        napi_get_boolean(env, flag, &result);
        return result;
    }

    napi_value Types::IsGeneratorFunction(napi_env env, napi_value src) const
    {
        napi_valuetype result = napi_undefined;
        napi_typeof(env, src, &result);
        bool flag = false;
        if (result == napi_function) {
            NAPI_CALL(env, napi_is_generator_function(env, src, &flag));
        }
        napi_value rst = nullptr;
        napi_get_boolean(env, flag, &rst);
        return rst;
    }

    napi_value Types::IsGeneratorObject(napi_env env, napi_value src) const
    {
        bool flag = false;
        NAPI_CALL(env, napi_is_generator_object(env, src, &flag));
        napi_value result = nullptr;
        napi_get_boolean(env, flag, &result);
        return result;
    }

    napi_value Types::IsInt8Array(napi_env env, napi_value src) const
    {
        napi_valuetype valuetype = napi_undefined;
        bool flag = false;
        napi_value result = nullptr;
        napi_typeof(env, src, &valuetype);
        if (valuetype == napi_valuetype::napi_object) {
            napi_typedarray_type type = napi_int8_array;
            size_t byteOffset = 0;
            size_t length = 0;
            void* resultData = nullptr;
            napi_value resultBuffer = nullptr;
            napi_status rstSta = napi_get_typedarray_info(env, src, &type, &length,
                                                          &resultData, &resultBuffer, &byteOffset);
            if ((rstSta == napi_ok) && (type == napi_typedarray_type::napi_int8_array)) {
                flag = true;
            }
        }
        napi_get_boolean(env, flag, &result);
        return result;
    }

    napi_value Types::IsInt16Array(napi_env env, napi_value src) const
    {
        napi_valuetype valuetype = napi_undefined;
        bool flag = false;
        napi_value result = nullptr;
        napi_typeof(env, src, &valuetype);
        if (valuetype == napi_valuetype::napi_object) {
            napi_typedarray_type type = napi_int8_array;
            size_t byteOffset = 0;
            size_t length = 0;
            void* resultData = nullptr;
            napi_value resultBuffer = nullptr;
            napi_get_typedarray_info(env, src, &type, &length,
                                     &resultData, &resultBuffer, &byteOffset);
            if (type == napi_typedarray_type::napi_int16_array) {
                flag = true;
            }
        }
        napi_get_boolean(env, flag, &result);
        return result;
    }

    napi_value Types::IsInt32Array(napi_env env, napi_value src) const
    {
        napi_valuetype valuetype = napi_undefined;
        bool flag = false;
        napi_value result = nullptr;
        napi_typeof(env, src, &valuetype);
        if (valuetype == napi_valuetype::napi_object) {
            napi_typedarray_type type = napi_int8_array;
            size_t byteOffset = 0;
            size_t length = 0;
            void* resultData = nullptr;
            napi_value resultBuffer = nullptr;
            napi_get_typedarray_info(env, src, &type, &length,
                                     &resultData, &resultBuffer, &byteOffset);
            if (type == napi_typedarray_type::napi_int32_array) {
                flag = true;
            }
        }
        napi_get_boolean(env, flag, &result);
        return result;
    }

    napi_value Types::IsMap(napi_env env, napi_value src) const
    {
        bool flag = false;
        NAPI_CALL(env, napi_is_map(env, src, &flag));
        napi_value result = nullptr;
        napi_get_boolean(env, flag, &result);
        return result;
    }

    napi_value Types::IsMapIterator(napi_env env, napi_value src) const
    {
        bool flag = false;
        NAPI_CALL(env, napi_is_map_iterator(env, src, &flag));
        napi_value result = nullptr;
        napi_get_boolean(env, flag, &result);
        return result;
    }

    napi_value Types::IsModuleNamespaceObject(napi_env env, napi_value src) const
    {
        bool flag = false;
        NAPI_CALL(env, napi_is_module_namespace_object(env, src, &flag));
        napi_value result = nullptr;
        napi_get_boolean(env, flag, &result);
        return result;
    }

    napi_value Types::IsNativeError(napi_env env, napi_value src) const
    {
        bool flag = false;
        NAPI_CALL(env, napi_is_error(env, src, &flag));
        napi_value result = nullptr;
        napi_get_boolean(env, flag, &result);
        return result;
    }

    napi_value Types::IsNumberObject(napi_env env, napi_value src) const
    {
        bool flag = false;
        napi_valuetype valuetype = napi_undefined;
        napi_value result = nullptr;
        napi_typeof(env, src, &valuetype);
        if (valuetype == napi_valuetype::napi_object) {
            NAPI_CALL(env, napi_is_number_object(env, src, &flag));
        }
        napi_get_boolean(env, flag, &result);
        return result;
    }

    napi_value Types::IsPromise(napi_env env, napi_value src) const
    {
        bool flag = false;
        NAPI_CALL(env, napi_is_promise(env, src, &flag));
        napi_value result = nullptr;
        napi_get_boolean(env, flag, &result);
        return result;
    }

    napi_value Types::IsProxy(napi_env env, napi_value src) const
    {
        bool flag = false;
        NAPI_CALL(env, napi_is_proxy(env, src, &flag));
        napi_value result = nullptr;
        napi_get_boolean(env, flag, &result);
        return result;
    }

    napi_value Types::IsRegExp(napi_env env, napi_value src) const
    {
        bool flag = false;
        NAPI_CALL(env, napi_is_reg_exp(env, src, &flag));
        napi_value result = nullptr;
        napi_get_boolean(env, flag, &result);
        return result;
    }

    napi_value Types::IsSet(napi_env env, napi_value src) const
    {
        bool flag = false;
        NAPI_CALL(env, napi_is_set(env, src, &flag));
        napi_value result = nullptr;
        napi_get_boolean(env, flag, &result);
        return result;
    }

    napi_value Types::IsSetIterator(napi_env env, napi_value src) const
    {
        bool flag = false;
        NAPI_CALL(env, napi_is_set_iterator(env, src, &flag));
        napi_value result = nullptr;
        napi_get_boolean(env, flag, &result);
        return result;
    }

    napi_value Types::IsSharedArrayBuffer(napi_env env, napi_value src) const
    {
        bool flag = false;
        NAPI_CALL(env, napi_is_shared_array_buffer(env, src, &flag));
        napi_value result = nullptr;
        napi_get_boolean(env, flag, &result);
        return result;
    }

    napi_value Types::IsStringObject(napi_env env, napi_value src) const
    {
        bool flag = false;
        napi_valuetype valuetype = napi_undefined;
        napi_typeof(env, src, &valuetype);
        if (valuetype == napi_valuetype::napi_object) {
            NAPI_CALL(env, napi_is_string_object(env, src, &flag));
        }
        napi_value result = nullptr;
        napi_get_boolean(env, flag, &result);
        return result;
    }

    napi_value Types::IsSymbolObject(napi_env env, napi_value src) const
    {
        bool flag = false;
        napi_value result = nullptr;
        napi_valuetype valuetype = napi_undefined;
        napi_typeof(env, src, &valuetype);
        if (valuetype == napi_valuetype::napi_object) {
            NAPI_CALL(env, napi_is_symbol_object(env, src, &flag));
        }
        napi_get_boolean(env, flag, &result);
        return result;
    }

    napi_value Types::IsTypedArray(napi_env env, napi_value src) const
    {
        bool flag = false;
        NAPI_CALL(env, napi_is_typedarray(env, src, &flag));
        napi_value result = nullptr;
        napi_get_boolean(env, flag, &result);
        return result;
    }

    napi_value Types::IsUint8Array(napi_env env, napi_value src) const
    {
        napi_valuetype valuetype = napi_undefined;
        bool flag = false;
        napi_value result = nullptr;
        napi_typeof(env, src, &valuetype);
        if (valuetype == napi_valuetype::napi_object) {
            napi_typedarray_type type = napi_int8_array;
            size_t byteOffset = 0;
            size_t length = 0;
            void* resultData = nullptr;
            napi_value resultBuffer = nullptr;
            napi_get_typedarray_info(env, src, &type, &length,
                                     &resultData, &resultBuffer, &byteOffset);
            if (type == napi_typedarray_type::napi_uint8_array) {
                flag = true;
            }
        }
        napi_get_boolean(env, flag, &result);
        return result;
    }

    napi_value Types::IsUint8ClampedArray(napi_env env, napi_value src) const
    {
        napi_valuetype valuetype = napi_undefined;
        bool flag = false;
        napi_value result = nullptr;
        napi_typeof(env, src, &valuetype);
        if (valuetype == napi_valuetype::napi_object) {
            napi_typedarray_type type = napi_int8_array;
            size_t byteOffset = 0;
            size_t length = 0;
            void* resultData = nullptr;
            napi_value resultBuffer = nullptr;
            napi_get_typedarray_info(env, src, &type, &length,
                                     &resultData, &resultBuffer, &byteOffset);
            if (type == napi_typedarray_type::napi_uint8_clamped_array) {
                flag = true;
            }
        }
        napi_get_boolean(env, flag, &result);
        return result;
    }

    napi_value Types::IsUint16Array(napi_env env, napi_value src) const
    {
        napi_valuetype valuetype = napi_undefined;
        bool flag = false;
        napi_value result = nullptr;
        napi_typeof(env, src, &valuetype);
        if (valuetype == napi_valuetype::napi_object) {
            napi_typedarray_type type = napi_int8_array;
            size_t byteOffset = 0;
            size_t length = 0;
            void* resultData = nullptr;
            napi_value resultBuffer = nullptr;
            napi_get_typedarray_info(env, src, &type, &length,
                                     &resultData, &resultBuffer, &byteOffset);
            if (type == napi_typedarray_type::napi_uint16_array) {
                flag = true;
            }
        }
        napi_get_boolean(env, flag, &result);
        return result;
    }

    napi_value Types::IsUint32Array(napi_env env, napi_value src) const
    {
        napi_valuetype valuetype = napi_undefined;
        bool flag = false;
        napi_value result = nullptr;
        napi_typeof(env, src, &valuetype);
        if (valuetype == napi_valuetype::napi_object) {
            napi_typedarray_type type = napi_int8_array;
            size_t byteOffset = 0;
            size_t length = 0;
            void* resultData = nullptr;
            napi_value resultBuffer = nullptr;
            napi_get_typedarray_info(env, src, &type, &length,
                                     &resultData, &resultBuffer, &byteOffset);
            if (type == napi_typedarray_type::napi_uint32_array) {
                flag = true;
            }
        }
        napi_get_boolean(env, flag, &result);
        return result;
    }

    napi_value Types::IsWeakMap(napi_env env, napi_value src) const
    {
        bool flag = false;
        NAPI_CALL(env, napi_is_weak_map(env, src, &flag));
        napi_value result = nullptr;
        napi_get_boolean(env, flag, &result);
        return result;
    }

    napi_value Types::IsWeakSet(napi_env env, napi_value src) const
    {
        bool flag = false;
        NAPI_CALL(env, napi_is_weak_set(env, src, &flag));
        napi_value result = nullptr;
        napi_get_boolean(env, flag, &result);
        return result;
    }
}
