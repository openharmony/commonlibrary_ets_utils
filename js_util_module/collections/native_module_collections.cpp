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

#include "js_native_api.h"
#include "napi/native_node_api.h"
#include "tools/log.h"

namespace {
const std::string SHARED_ARRAY_NAME = "SharedArray";
const std::string SHARED_SET_NAME = "SharedSet";
const std::string SHARED_MAP_NAME = "SharedMap";
}  // namespace

static bool GetCollectionFunction(napi_env env, napi_value global, std::string collectionName,
                                  napi_value &collectionFunction)
{
    napi_value collectionKey;
    napi_create_string_utf8(env, collectionName.c_str(), collectionName.size(), &collectionKey);
    napi_get_property(env, global, collectionKey, &collectionFunction);
    bool validFunction = false;
    napi_is_callable(env, collectionFunction, &validFunction);
    if (!validFunction) {
        HILOG_ERROR("Get function for %{public}s failed.", collectionName.c_str());
    }
    return validFunction;
}

static napi_value InitArkTSCollections(napi_env env, napi_value exports)
{
    napi_value global;
    napi_value sharedArrayValue;
    napi_value sharedSetValue;
    napi_value sharedMapValue;
    napi_get_global(env, &global);
    if (!GetCollectionFunction(env, global, SHARED_ARRAY_NAME, sharedArrayValue)) {
        return exports;
    }
    if (!GetCollectionFunction(env, global, SHARED_SET_NAME, sharedSetValue)) {
        return exports;
    }
    if (!GetCollectionFunction(env, global, SHARED_MAP_NAME, sharedMapValue)) {
        return exports;
    }

    napi_property_descriptor desc[] = {
        DECLARE_NAPI_PROPERTY("Array", sharedArrayValue),
        DECLARE_NAPI_PROPERTY("Set", sharedSetValue),
        DECLARE_NAPI_PROPERTY("Map", sharedMapValue),
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