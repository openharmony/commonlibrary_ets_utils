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

#ifndef JS_CONCURRENT_MODULE_TASKPOOL_TASK_QUEUE_H
#define JS_CONCURRENT_MODULE_TASKPOOL_TASK_QUEUE_H

#include <queue>

namespace Commonlibrary::Concurrent::TaskPoolModule {
class ExecuteQueue {
public:
    ExecuteQueue() = default;
    ~ExecuteQueue() = default;

    void EnqueueTaskId(uint64_t taskId);
    uint64_t DequeueTaskId();

    bool IsEmpty() const;
    uint32_t GetTaskNum() const
    {
        return tasks_.size();
    }

private:
    std::queue<uint64_t> tasks_ {};
};
} // namespace Commonlibrary::Concurrent::TaskPoolModule
#endif // JS_CONCURRENT_MODULE_TASKPOOL_TASK_QUEUE_H