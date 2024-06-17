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
#include <fstream>
#include <sstream>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/sysinfo.h>

namespace Commonlibrary::Platform {
static long g_hz = sysconf(_SC_CLK_TCK);

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
    std::string path = "/proc/" + std::to_string(getpid()) + "/stat";
    std::ifstream statFile(path);
    if (!statFile.is_open()) {
        HILOG_ERROR("process:: Failed to open config file");
        return startRealtime;
    }
    std::string strLine;
    std::getline(statFile, strLine);
    if (strLine.empty()) {
        HILOG_ERROR("process:: No valid content was read");
        statFile.close();
        return startRealtime;
    }
    std::istringstream iss(strLine);
    int count = 1;
    std::string word;
    while (iss >> word) {
        if (count == 22) { // 22 : starttime
            if (g_hz == -1) {
                statFile.close();
                return startRealtime;
            }
            startRealtime = word.empty() ? 0 : std::strtod(word.c_str(), nullptr);
            startRealtime = (startRealtime * 1000) / static_cast<double>(g_hz); // 1000 : // Calculate milliseconds
            break;
        }
        count++;
    }
    statFile.close();
    return startRealtime;
}
} // namespace Commonlibrary::Platform