/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "process_report_helper.h"

#include <unistd.h>
#if defined(ENABLE_RECORD)
#include "ability_manager_client.h"
#include "xcollie/process_kill_reason.h"
#endif
#include "tools/log.h"

namespace OHOS::JsSysModule::Process {

void ProcessReportHelper::RecordAppWithReason(int32_t pid, int32_t signal, const std::string &exitMsg)
{
#if defined(ENABLE_RECORD)
    int32_t uid = static_cast<int32_t>(getuid());
    OHOS::AAFwk::ExitReasonCompability exitReasonData;
    exitReasonData.killId = OHOS::HiviewDFX::ProcessKillReason::KillEventId::REASON_KILL_PROCESS;
    exitReasonData.killMsg = exitMsg + ", signal:" + std::to_string(signal);
    auto instance = OHOS::AAFwk::AbilityManagerClient::GetInstance();
    if (instance == nullptr) {
        HILOG_WARN("Process:: AbilityManagerClient instance is nullptr");
        return;
    }
    instance->RecordAppWithReason(pid, uid, exitReasonData);
#endif
}
} // namespace OHOS::JsSysModule::Process
