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

#include "locks/async_lock_manager.h"
#include "json/json_manager.h"
#include "tools/log.h"
#include "utils.h"

namespace Commonlibrary::Concurrent {
const std::string IS_SENDABLE_NAME = "isSendable";

napi_value InitGlobal(napi_env env, const std::string& valueName, bool isFunc, napi_value exports)
{
    napi_value global;
    napi_value key;
    napi_value value;
    napi_get_global(env, &global);
    napi_create_string_utf8(env, valueName.c_str(), valueName.size(), &key);
    napi_get_property(env, global, key, &value);

    if (isFunc) {
        bool validFunction = false;
        napi_is_callable(env, value, &validFunction);
        if (!validFunction) {
            HILOG_ERROR("Get function for %{public}s failed.", valueName.c_str());
            return exports;
        }
    }
    napi_set_named_property(env, exports, valueName.c_str(), value);

    return exports;
}
napi_value Utils::Init(napi_env env, napi_value exports)
{
    Commonlibrary::Concurrent::LocksModule::AsyncLockManager::Init(env, exports);
    Commonlibrary::Concurrent::JsonManager::Init(env, exports);
    InitGlobal(env, IS_SENDABLE_NAME, true, exports);

    return exports;
}
}  // namespace Commonlibrary::Concurrent