/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "napi/native_node_api.h"
#include "tools/log.h"

namespace {
const std::string SENDABLE_ARRAY_NAME = "SendableArray";
const std::string SENDABLE_SET_NAME = "SendableSet";
const std::string SENDABLE_MAP_NAME = "SendableMap";
const std::string SENDABLE_INT8_ARRAY = "SendableInt8Array";
const std::string SENDABLE_UINT8_ARRAY = "SendableUint8Array";
const std::string SENDABLE_INT16_ARRAY = "SendableInt16Array";
const std::string SENDABLE_UINT16_ARRAY = "SendableUint16Array";
const std::string SENDABLE_INT32_ARRAY = "SendableInt32Array";
const std::string SENDABLE_UINT32_ARRAY = "SendableUint32Array";
const std::string SENDABLE_ARRAY_BUFFER = "SendableArrayBuffer";
const std::string BIT_VECTOR = "BitVector";
const int ARK_PRIVATE_BIT_VECTOR_INDEX = 14;
const std::string SENDABLE_UINT8_CLAMPED_ARRAY = "SendableUint8ClampedArray";
const std::string SENDABLE_FLOAT32_ARRAY = "SendableFloat32Array";
}  // namespace

static bool GetCollectionFunction(napi_env env, napi_value global, const std::string collectionName,
                                  napi_value &collectionFunction)
{
    napi_value collectionKey;
    NAPI_CALL_BASE(env, napi_create_string_utf8(env, collectionName.c_str(),
                   collectionName.size(), &collectionKey), false);
    NAPI_CALL_BASE(env, napi_get_property(env, global, collectionKey, &collectionFunction), false);
    bool validFunction = false;
    NAPI_CALL_BASE(env, napi_is_callable(env, collectionFunction, &validFunction), false);
    if (!validFunction) {
        HILOG_ERROR("Get function for %{public}s failed.", collectionName.c_str());
    }
    return validFunction;
}

static void GetBitVectorFunction(napi_env env, napi_value global, napi_value &bitVector)
{
    napi_value arkPrivateClass = nullptr;
    napi_value arkPrivateKey = nullptr;
    std::string arkPrivateStr = "ArkPrivate";
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, arkPrivateStr.c_str(),
                          arkPrivateStr.size(), &arkPrivateKey));
    NAPI_CALL_RETURN_VOID(env, napi_get_property(env, global, arkPrivateKey, &arkPrivateClass));

    napi_value loadFunction = nullptr;
    napi_value loadKey = nullptr;
    std::string loadStr = "Load";
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, loadStr.c_str(), loadStr.size(), &loadKey));
    NAPI_CALL_RETURN_VOID(env, napi_get_property(env, arkPrivateClass, loadKey, &loadFunction));

    napi_value bitVectorIndex = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, ARK_PRIVATE_BIT_VECTOR_INDEX, &bitVectorIndex));
    napi_value argv[1] = { bitVectorIndex };
    napi_call_function(env, arkPrivateClass, loadFunction, 1, argv, &bitVector);
}

static napi_value InitArkTSCollections(napi_env env, napi_value exports)
{
    napi_value global;
    napi_value sendableArrayValue;
    napi_value sendableSetValue;
    napi_value sendableMapValue;
    napi_value sendableInt8Array;
    napi_value sendableUint8Array;
    napi_value sendableInt16Array;
    napi_value sendableUint16Array;
    napi_value sendableInt32Array;
    napi_value sendableUint32Array;
    napi_value sendableArrayBuffer;
    napi_value bitVector;
    napi_value sendableUint8ClampedArray;
    napi_value sendableFloat32Array;

    napi_get_global(env, &global);
    if (!GetCollectionFunction(env, global, SENDABLE_ARRAY_NAME, sendableArrayValue)) {
        return exports;
    }
    if (!GetCollectionFunction(env, global, SENDABLE_SET_NAME, sendableSetValue)) {
        return exports;
    }
    if (!GetCollectionFunction(env, global, SENDABLE_MAP_NAME, sendableMapValue)) {
        return exports;
    }
    if (!GetCollectionFunction(env, global, SENDABLE_ARRAY_BUFFER, sendableArrayBuffer)) {
        return exports;
    }
    if (!GetCollectionFunction(env, global, SENDABLE_INT8_ARRAY, sendableInt8Array)) {
        return exports;
    }
    if (!GetCollectionFunction(env, global, SENDABLE_UINT8_ARRAY, sendableUint8Array)) {
        return exports;
    }
    if (!GetCollectionFunction(env, global, SENDABLE_INT16_ARRAY, sendableInt16Array)) {
        return exports;
    }
    if (!GetCollectionFunction(env, global, SENDABLE_UINT16_ARRAY, sendableUint16Array)) {
        return exports;
    }
    if (!GetCollectionFunction(env, global, SENDABLE_INT32_ARRAY, sendableInt32Array)) {
        return exports;
    }
    if (!GetCollectionFunction(env, global, SENDABLE_UINT32_ARRAY, sendableUint32Array)) {
        return exports;
    }
    if (!GetCollectionFunction(env, global, SENDABLE_UINT8_CLAMPED_ARRAY, sendableUint8ClampedArray)) {
        return exports;
    }

    if (!GetCollectionFunction(env, global, SENDABLE_FLOAT32_ARRAY, sendableFloat32Array)) {
        return exports;
    }

    GetBitVectorFunction(env, global, bitVector);

    napi_property_descriptor desc[] = {
        DECLARE_NAPI_PROPERTY("Array", sendableArrayValue),
        DECLARE_NAPI_PROPERTY("Set", sendableSetValue),
        DECLARE_NAPI_PROPERTY("Map", sendableMapValue),
        DECLARE_NAPI_PROPERTY("ArrayBuffer", sendableArrayBuffer),
        DECLARE_NAPI_PROPERTY("Int8Array", sendableInt8Array),
        DECLARE_NAPI_PROPERTY("Uint8Array", sendableUint8Array),
        DECLARE_NAPI_PROPERTY("Int16Array", sendableInt16Array),
        DECLARE_NAPI_PROPERTY("Uint16Array", sendableUint16Array),
        DECLARE_NAPI_PROPERTY("Int32Array", sendableInt32Array),
        DECLARE_NAPI_PROPERTY("Uint32Array", sendableUint32Array),
        DECLARE_NAPI_PROPERTY("BitVector", bitVector),
        DECLARE_NAPI_PROPERTY("Uint8ClampedArray", sendableUint8ClampedArray),
        DECLARE_NAPI_PROPERTY("Float32Array", sendableFloat32Array),
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    return exports;
}

static napi_module_with_js sendableArrayModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = InitArkTSCollections,
    .nm_modname = "arkts.collections",  // @ohos.arkts.collections
    .nm_priv = ((void *)0),
};

extern "C" __attribute__((constructor)) void ArkTSCollectionsRegisterModule()
{
    napi_module_with_js_register(&sendableArrayModule);
}
