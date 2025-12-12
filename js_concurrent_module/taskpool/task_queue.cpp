/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "task_queue.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
void ExecuteQueue::EnqueueTaskId(uint32_t taskId)
{
    tasks_.emplace_back(taskId);
}

bool ExecuteQueue::EraseWaitingTaskId(uint32_t taskId)
{
    auto it = std::find(tasks_.begin(), tasks_.end(), taskId);
    if (it != tasks_.end()) {
        tasks_.erase(it);
        return true;
    }
    return false;
}

uint32_t ExecuteQueue::DequeueTaskId()
{
    if (!tasks_.empty()) {
        uint32_t taskId = tasks_.front();
        tasks_.pop_front();
        return taskId;
    }
    return 0;
}

bool ExecuteQueue::IsEmpty() const
{
    return tasks_.empty();
}

uint32_t ExecuteQueue::GetTaskNum() const
{
    return tasks_.size();
}

uint32_t ExecuteQueue::GetHead() const
{
    if (tasks_.empty()) {
        return 0;
    }
    return tasks_.front();
}
} // namespace Commonlibrary::Concurrent::TaskPoolModule