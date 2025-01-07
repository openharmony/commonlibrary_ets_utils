/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef COMMONLIBRARY_ETS_UTILS_TOOLS_COMMON_HELPER_H
#define COMMONLIBRARY_ETS_UTILS_TOOLS_COMMON_HELPER_H

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "native_engine/native_engine.h"

namespace OHOS::Tools {
const int32_t ApiVersionMod = 100;

class ApiHelper {
public:
    ApiHelper() = default;
    ~ApiHelper() = default;

    static int32_t apiVersion;
    static int32_t GetApiVersion(napi_env env)
    {
        if (apiVersion == 0) {
            NativeEngine* engine = reinterpret_cast<NativeEngine*>(env);
            if (engine != nullptr) {
                apiVersion = engine->GetApiVersion() % ApiVersionMod;
            }
        }
        return apiVersion;
    }
};
int32_t ApiHelper::apiVersion = 0;
} // namespace OHOS::Tools
#endif /* COMMONLIBRARY_ETS_UTILS_TOOLS_COMMON_HELPER_H */
