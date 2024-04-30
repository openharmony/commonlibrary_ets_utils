/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <unistd.h>
#include <sys/types.h>

#include "json_manager.h"
#include "tools/log.h"

namespace Commonlibrary::Concurrent {
const std::string JSON_NAME = "JSON";
const std::string PARSE_NAME = "parse";
const std::string PARSE_SENDABLE_NAME = "parseSendable";
const std::string STRINGIFY_NAME = "stringify";

bool JsonManager::GeJsonFunction(napi_env env, napi_value global, std::string jsonName, napi_value &jsonFunction)
{
    napi_value jsonKey;
    napi_create_string_utf8(env, jsonName.c_str(), jsonName.size(), &jsonKey);
    napi_get_property(env, global, jsonKey, &jsonFunction);
    bool validFunction = false;
    napi_is_callable(env, jsonFunction, &validFunction);
    if (!validFunction) {
        HILOG_ERROR("Get JSON function for %{public}s failed.", jsonName.c_str());
    }
    return validFunction;
}

napi_value JsonManager::Init(napi_env env, napi_value exports)
{
    napi_value global;
    napi_value json;
    napi_value parse;
    napi_value parseSendable;
    napi_value stringify;
    NAPI_CALL(env, napi_create_object(env, &json));
    napi_get_global(env, &global);
    if (!(GeJsonFunction(env, global, PARSE_NAME, parse) &&
          GeJsonFunction(env, global, PARSE_SENDABLE_NAME, parseSendable) &&
          GeJsonFunction(env, global, STRINGIFY_NAME, stringify))) {
        return exports;
    }
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_PROPERTY("parse", parse),
        DECLARE_NAPI_PROPERTY("parseSendable", parseSendable),
        DECLARE_NAPI_PROPERTY("stringify", stringify),
    };
    napi_define_properties(env, json, sizeof(desc) / sizeof(desc[0]), desc);
    napi_set_named_property(env, exports, "json", json);
    return exports;
}
}

