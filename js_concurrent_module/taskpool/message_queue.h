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

#ifndef JS_CONCURRENT_MODULE_TASKPOOL_MESSAGE_QUEUE_H
#define JS_CONCURRENT_MODULE_TASKPOOL_MESSAGE_QUEUE_H

#include <mutex>
#include <queue>

namespace Commonlibrary::Concurrent::TaskPoolModule {
template <typename MessageDataType>
class MessageQueue final {
public:
    void EnQueue(MessageDataType data)
    {
        std::lock_guard<std::mutex> lock(queueLock_);
        queue_.push(data);
    }

    MessageDataType DeQueue()
    {
        std::lock_guard<std::mutex> lock(queueLock_);
        if (queue_.empty()) {
            return nullptr;
        }

        auto data = queue_.front();
        queue_.pop();
        return data;
    }

    bool IsEmpty()
    {
        std::lock_guard<std::mutex> lock(queueLock_);
        return queue_.empty();
    }

private:
    std::mutex queueLock_;
    std::queue<MessageDataType> queue_;
};
}  // namespace Commonlibrary::Concurrent::TaskPoolModule
#endif // JS_CONCURRENT_MODULE_TASKPOOL_MESSAGE_QUEUE_H