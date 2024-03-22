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

#include "message_queue.h"
#include "tools/log.h"

namespace Commonlibrary::Concurrent::WorkerModule {
void MessageQueue::EnQueue(MessageDataType data)
{
    queueLock_.lock();
    queue_.push(data);
    queueLock_.unlock();
}

bool MessageQueue::DeQueue(MessageDataType *data)
{
    queueLock_.lock();
    if (queue_.empty()) {
        queueLock_.unlock();
        return false;
    }
    if (data != nullptr) {
        *data = queue_.front();
        queue_.pop();
    } else {
        HILOG_ERROR("worker:: data is nullptr.");
    }
    queueLock_.unlock();
    return true;
}

bool MessageQueue::IsEmpty() const
{
    return queue_.empty();
}

void MessageQueue::Clear(napi_env env)
{
    queueLock_.lock();
    size_t size = queue_.size();
    for (size_t i = 0; i < size; i++) {
        MessageDataType data = queue_.front();
        napi_delete_serialization_data(env, data);
        queue_.pop();
    }
    queueLock_.unlock();
}

void MarkedMessageQueue::Push(uint32_t id, MessageDataType data)
{
    std::unique_lock<std::mutex> lock(queueLock_);
    queue_.push({id, data});
}

void MarkedMessageQueue::Pop()
{
    std::unique_lock<std::mutex> lock(queueLock_);
    queue_.pop();
}

std::pair<uint32_t, MessageDataType> MarkedMessageQueue::Front()
{
    std::unique_lock<std::mutex> lock(queueLock_);
    return queue_.front();
}

bool MarkedMessageQueue::IsEmpty()
{
    std::unique_lock<std::mutex> lock(queueLock_);
    return queue_.empty();
}

void MarkedMessageQueue::Clear(napi_env env)
{
    std::unique_lock<std::mutex> lock(queueLock_);
    while (!queue_.empty()) {
        std::pair<uint32_t, MessageDataType> pair = queue_.front();
        napi_delete_serialization_data(env, pair.second);
        queue_.pop();
    }
}
}  // namespace Commonlibrary::Concurrent::WorkerModule
