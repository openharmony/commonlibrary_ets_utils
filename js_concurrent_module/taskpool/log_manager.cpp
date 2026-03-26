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

#include "log_manager.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
static constexpr uint32_t MAX_LOG_SIZE = 50000;
static constexpr uint32_t LOG_PRINT_SIZE = 750;
static constexpr uint32_t WAITING_INTERVAL = 1000; // 1000: 1s

void LogManager::PrintLog()
{
    std::vector<std::string> outputContainer;
    {
        std::lock_guard<std::mutex> lock(logQueueMutex_);
        size_t count = std::min<size_t>(LOG_PRINT_SIZE, logQueue_.size()); // print LOG_PRINT_SIZE(100) tasks per round
        outputContainer.reserve(count);
        while (count > 0) {
            outputContainer.push_back(logQueue_.front());
            logQueue_.pop();
            count--;
        }
        size_ -= count;
    }
    for (size_t i = 0; i < outputContainer.size(); i++) {
        HILOG_INFO("taskpool::%{public}s", outputContainer[i].c_str());
    }
}

bool LogManager::PushLog(const std::string& msg)
{
    std::lock_guard<std::mutex> lock(logQueueMutex_);
    if (size_ > MAX_LOG_SIZE) {
        std::queue<std::string> empty;
        std::swap(logQueue_, empty);
        size_ = 0;
        HILOG_WARN("taskpool::The number of Logs has exceeded 50000 and has been cleared");
    }
    logQueue_.push(msg);
    size_++;
    uint64_t nowTime = ConcurrentHelper::GetMilliseconds();
    if (size_ >= LOG_PRINT_SIZE && (nowTime - printTime_) >= WAITING_INTERVAL) {
        printTime_ = nowTime;
        return true;
    }
    return false;
}

bool LogManager::IsNeedPrint()
{
    std::lock_guard<std::mutex> lock(logQueueMutex_);
    uint64_t nowTime = ConcurrentHelper::GetMilliseconds();
    if ((nowTime - printTime_) >= WAITING_INTERVAL) {
        printTime_ = nowTime;
        return true;
    }
    return false;
}
} // namespace Commonlibrary::Concurrent::TaskPoolModule