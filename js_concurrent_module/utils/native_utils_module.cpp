/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "utils.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

extern const char _binary_utils_abc_start[];
extern const char _binary_utils_abc_end[];

extern "C" __attribute__((visibility("default"))) void NAPI_utils_GetABCCode(const char** buf, int* buflen)
{
    if (buf != nullptr) {
        *buf = _binary_utils_abc_start;
    }
    if (buflen != nullptr) {
        *buflen = _binary_utils_abc_end - _binary_utils_abc_start;
    }
}

static napi_module_with_js g_utilsModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Commonlibrary::Concurrent::Utils::Init,
    .nm_modname = "arkts.utils",
    .nm_priv = reinterpret_cast<void *>(0),
    .nm_get_abc_code = NAPI_utils_GetABCCode,
};

extern "C" __attribute__((constructor)) void LocksRegister()
{
    napi_module_with_js_register(&g_utilsModule);
}
