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

#include "process_helper.h"

#include <cstdint>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/sysinfo.h>

namespace Commonlibrary::Platform {

void ProcessExit(int signal)
{
    quick_exit(signal);
}

int GetSysConfig(int number)
{
    auto configinfo = static_cast<int32_t>(sysconf(number));
    return configinfo;
}

double GetSysTimer()
{
    struct sysinfo information = {0};
    time_t systimer = 0;
    if (sysinfo(&information)) {
        HILOG_ERROR("Failed to get sysinfo");
        return SYS_INFO_FAILED;
    }
    systimer = information.uptime;
    return static_cast<double>(systimer);
}

int GetThreadId()
{
    auto proTid = static_cast<int64_t>(gettid());
    return proTid;
}

int GetThreadPRY(int tid)
{
    int32_t pri = getpriority(PRIO_PROCESS, tid);
    return pri;
}
} // namespace Commonlibrary::Platform