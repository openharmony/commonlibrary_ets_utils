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
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/sysinfo.h>
#include <uv.h>

namespace Commonlibrary::Platform {
constexpr int NUM_OF_DATA = 15; // 15:Data size
const std::string PROCESS_START_TIME_DATA = "PROCESS_START_TIME";

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

double GetProcessStartRealtime()
{
    double startRealtime = 0;
    char buf[NUM_OF_DATA] = { 0 };
    size_t length = sizeof(buf);
    auto envNum = uv_os_getenv(PROCESS_START_TIME_DATA.c_str(), buf, &length);
    if (envNum == UV_ENOENT || strlen(buf) == 0) {
        HILOG_ERROR("process:: Failed to get process start env");
        startRealtime = 0;
    } else {
        startRealtime = std::strtod(buf, nullptr);
    }
    return startRealtime;
}
} // namespace Commonlibrary::Platform