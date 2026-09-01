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

#ifndef PROCESS_PROCESS_REPORT_HELPER_H
#define PROCESS_PROCESS_REPORT_HELPER_H

#include <cstdint>
#include <string>

namespace OHOS::JsSysModule::Process {
class ProcessReportHelper {
public:
    /**
    * Record app exit reason before kill
    *
    * @param pid The process ID of the reported process.
    * @param signal The kill signal.
    * @param exitMsg The message of the process exit.
    */
    static void RecordAppWithReason(int32_t pid, int32_t signal, const std::string &exitMsg);
};
} // namespace OHOS::JsSysModule::Process
#endif // PROCESS_PROCESS_REPORT_HELPER_H
