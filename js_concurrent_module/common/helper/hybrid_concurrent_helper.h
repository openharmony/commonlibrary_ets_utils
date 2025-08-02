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

#ifndef JS_CONCURRENT_MODULE_COMMON_HELPER_HYBRID_CONCURRENT_HELPER_H
#define JS_CONCURRENT_MODULE_COMMON_HELPER_HYBRID_CONCURRENT_HELPER_H

#include <mutex>
#include "ani.h"

namespace Commonlibrary::Concurrent::Common::Helper {
extern std::atomic_bool globalEnableConcurrencyInteropFlag;
extern ani_vm* globalAniVm;
extern std::once_flag g_globalAniVmInitFlag;

class ANIHelper {
public:
    ANIHelper() = delete;
    ~ANIHelper() = delete;

    static ani_vm* GetAniVm();
    static void InitializeAniVm();
    static bool IsConcurrencySupportInterop();
};
} // namespace Commonlibrary::Concurrent::Common::Helper
#endif // JS_CONCURRENT_MODULE_COMMON_HELPER_HYBRID_CONCURRENT_HELPER_H