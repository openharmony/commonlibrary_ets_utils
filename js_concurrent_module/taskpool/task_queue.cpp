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
void TaskQueue::EnqueueTask(std::unique_ptr<Task> task)
{
    std::unique_lock<std::mutex> lock(mtx_);
    tasks_.push(std::move(task));
}

std::unique_ptr<Task> TaskQueue::DequeueTask()
{
    std::unique_lock<std::mutex> lock(mtx_);
    if (!tasks_.empty()) {
        std::unique_ptr<Task> task = std::move(tasks_.front());
        tasks_.pop();
        return task;
    }

    return nullptr;
}

bool TaskQueue::IsEmpty() const
{
    std::unique_lock<std::mutex> lock(mtx_);
    return tasks_.empty();
}
} // namespace Commonlibrary::Concurrent::TaskPoolModule