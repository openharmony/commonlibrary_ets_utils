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
#include "utils.h"

namespace Commonlibrary::Concurrent {
napi_value Utils::Init(napi_env env, napi_value exports)
{
    Commonlibrary::Concurrent::LocksModule::AsyncLockManager::Init(env, exports);
    Commonlibrary::Concurrent::JsonManager::Init(env, exports);
    return exports;
}
}  // namespace Commonlibrary::Concurrent