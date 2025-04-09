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

#include "dfx_hisys_event.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {

int32_t DfxHisysEvent::WriteFfrtAndUv(HisyseventParams* hisyseventParams)
{
    int32_t ret = HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::ARKTS_TASKPOOL,
        "ARK_FFRT_UV",
        OHOS::HiviewDFX::HiSysEvent::EventType::FAULT,
        "PID", hisyseventParams->pid,
        "TID", hisyseventParams->tid,
        "METHOD", hisyseventParams->methodName,
        "FUNCTION_NAME", hisyseventParams->funName,
        "TYPE", hisyseventParams->logType,
        "MESSAGE", hisyseventParams->message,
        "CODE", hisyseventParams->code,
        "WAIT_TIME", std::to_string(hisyseventParams->waitTime));
    delete hisyseventParams;
    hisyseventParams = nullptr;
    return ret;
}
} // namespace Commonlibrary::Concurrent::TaskPoolModule