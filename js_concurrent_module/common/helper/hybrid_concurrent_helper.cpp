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

#include "hybrid_concurrent_helper.h"
#include "tools/log.h"
#include "parameters.h"

namespace Commonlibrary::Concurrent::Common::Helper {
std::atomic_bool globalEnableConcurrencyInteropFlag = false;
ani_vm* globalAniVm = nullptr;
std::once_flag g_globalAniVmInitFlag;

ani_vm* ANIHelper::GetAniVm()
{
    std::call_once(g_globalAniVmInitFlag, &InitializeAniVm);
    return globalAniVm;
}

void ANIHelper::InitializeAniVm()
{
    const int flag = OHOS::system::GetIntParameter<int>("persist.commonlibrary.concurrencysupportinterop", 0);
    globalEnableConcurrencyInteropFlag = (flag != 0);
    const std::string logMsg = globalEnableConcurrencyInteropFlag
        ? "worker and taskpool support interop."
        : "worker and taskpool do not support interop.";
    HILOG_INFO("ANIHelper:: %{public}s", logMsg.c_str());
    if (!globalEnableConcurrencyInteropFlag) {
        return;
    }
    ani_size res = 0;
    ani_status status = ANI_GetCreatedVMs(&globalAniVm, 1, &res);
    if (status != ANI_OK) {
        HILOG_ERROR("ANIHelper:: ANI_GetCreatedVMs failed.");
        globalAniVm = nullptr;
    }
}

bool ANIHelper::IsConcurrencySupportInterop()
{
    return globalEnableConcurrencyInteropFlag;
}
} // Commonlibrary::Concurrent::Common::Helper