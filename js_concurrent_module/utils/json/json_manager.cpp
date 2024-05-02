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

#include <sys/types.h>
#include <unistd.h>

#include "json_manager.h"
#include "tools/log.h"

namespace Commonlibrary::Concurrent {
const std::string JSON_NAME = "JSON";
const std::string ASON_NAME = "ASON";
const std::string PARSE_NAME = "parse";
const std::string PARSE_SENDABLE_NAME = "parseSendable";
const std::string STRINGIFY_NAME = "stringify";

bool JsonManager::GeJsonFunction(napi_env env, napi_value global, const std::string &jsonName, napi_value &jsonFunction)
{
    napi_value jsonKey;
    napi_create_string_utf8(env, jsonName.c_str(), jsonName.size(), &jsonKey);
    napi_get_property(env, global, jsonKey, &jsonFunction);
    bool validFunction = false;
    napi_is_callable(env, jsonFunction, &validFunction);
    if (!validFunction) {
        HILOG_ERROR("Get ASON function for %{public}s failed.", jsonName.c_str());
    }
    return validFunction;
}

napi_value JsonManager::Init(napi_env env, napi_value exports)
{
    napi_value global;
    napi_value ason;
    napi_value parseSendable;
    napi_value stringify;
    napi_value jsonKey;
    napi_value jsonValue;
    NAPI_CALL(env, napi_create_object(env, &ason));
    napi_get_global(env, &global);
    napi_create_string_utf8(env, JSON_NAME.c_str(), JSON_NAME.size(), &jsonKey);
    napi_get_property(env, global, jsonKey, &jsonValue);
    if (!(GeJsonFunction(env, jsonValue, PARSE_SENDABLE_NAME, parseSendable) &&
          GeJsonFunction(env, jsonValue, STRINGIFY_NAME, stringify))) {
        return exports;
    }
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_PROPERTY(PARSE_NAME.c_str(), parseSendable),
        DECLARE_NAPI_PROPERTY(STRINGIFY_NAME.c_str(), stringify),
    };
    napi_define_properties(env, ason, sizeof(desc) / sizeof(desc[0]), desc);
    napi_set_named_property(env, exports, ASON_NAME.c_str(), ason);
    return exports;
}
}

