/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "concurrent_helper.h"
#include "tools/log.h"

namespace Commonlibrary::Concurrent::Common::Helper {
using SystemMemoryLevel = ConcurrentHelper::SystemMemoryLevel;

static constexpr double LOW_MEMORY_RATIO = 0.2;
static constexpr double MODERATE_MEMORY_RATIO = 0.5;
static const char* AVAILABLE_MEM = "MemAvailable:";
static const char* MEM_INFO = "/proc/meminfo";
static const char* TOTAL_MEM = "MemTotal:";

#if defined(OHOS_PLATFORM)
uint64_t ConcurrentHelper::ParseLine(const std::string& line)
{
    std::istringstream iss(line);
    std::string key;
    uint64_t value;
    std::string unit;
    if (iss >> key >> value >> unit) {
        return value;
    }
    return 0;
}

std::optional<double> ConcurrentHelper::GetSystemMemoryRatio()
{
    uint64_t totalMemory = 0;
    uint64_t availableMemory = 0;
    std::ifstream meminfo(MEM_INFO);
    if (!meminfo.is_open()) {
        HILOG_ERROR("ConcurrentHelper:: Open %{public}s failed", MEM_INFO);
        return std::nullopt;
    }
    std::string line;
    while (std::getline(meminfo, line)) {
        if (line.find(TOTAL_MEM) == 0) {
            totalMemory = ParseLine(line);
        } else if (line.find(AVAILABLE_MEM) == 0) {
            availableMemory = ParseLine(line);
        }
    }
    if (totalMemory == 0) {
        HILOG_ERROR("ConcurrentHelper:: Failed to read the MemTotal.");
        return std::nullopt;
    }
    return static_cast<double>(availableMemory) / static_cast<double>(totalMemory);
}

SystemMemoryLevel ConcurrentHelper::GetMemoryLevel()
{
    const auto ratio = GetSystemMemoryRatio();
    if (!ratio.has_value()) { // error happens when read memory info, just return the MEMORY_LEVEL_LOW
        return SystemMemoryLevel::MEMORY_LEVEL_LOW;
    }
    if (ratio.value() > MODERATE_MEMORY_RATIO) {
        return SystemMemoryLevel::MEMORY_LEVEL_NORMAL;
    } else if (ratio.value() > LOW_MEMORY_RATIO) {
        return SystemMemoryLevel::MEMORY_LEVEL_MODERATE;
    } else {
        return SystemMemoryLevel::MEMORY_LEVEL_LOW;
    }
}
#endif

bool ConcurrentHelper::IsLowMemory()
{
#if defined(OHOS_PLATFORM)
    return GetMemoryLevel() == SystemMemoryLevel::MEMORY_LEVEL_LOW;
#else
    return false;
#endif
}

bool ConcurrentHelper::IsModerateMemory()
{
#if defined(OHOS_PLATFORM)
    return GetMemoryLevel() == SystemMemoryLevel::MEMORY_LEVEL_MODERATE;
#else
    return false;
#endif
}

std::string ConcurrentHelper::GetCurrentTimeStampWithMS()
{
    auto tp = std::chrono::system_clock::now();
    auto timeVal = std::chrono::system_clock::to_time_t(tp);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()) % 1000; // 1000: modulo
    int outMs = static_cast<int>(ms.count());

    std::tm localTm {};
    localTm.tm_isdst = -1; // -1: the system automatically determines whether it is in daylight saving time

#if defined(OHOS_PLATFORM) || defined(MAC_PLATFORM) || defined(IOS_PLATFORM)
    if (localtime_r(&timeVal, &localTm) == nullptr) { // LCOV_EXCL_BR_LINE
        return "TIME_ERR";
    }
#elif defined(WINDOWS_PLATFORM)
    if (localtime_s(&localTm, &timeVal) != 0) {
        return "TIME_ERR";
    }
#else
    return ""; // default result
#endif

    std::ostringstream oss;

    // formatted output: Day(2 digits) Hour:Minute:Second(each 2 digits).Milliseconds(3 digits)
    const int kTimeFieldWidth = 2;
    const int kMillisecondWidth = 3;

    oss << std::setfill('0')
        << std::setw(kTimeFieldWidth) << localTm.tm_mday << " "
        << std::setw(kTimeFieldWidth) << localTm.tm_hour << ":"
        << std::setw(kTimeFieldWidth) << localTm.tm_min << ":"
        << std::setw(kTimeFieldWidth) << localTm.tm_sec << "."
        << std::setw(kMillisecondWidth) << outMs;

    return oss.str();
}
} // namespace Commonlibrary::Concurrent::TaskPoolModule