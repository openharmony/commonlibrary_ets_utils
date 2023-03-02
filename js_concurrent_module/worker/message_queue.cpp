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
#include "utils/log.h"

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
        queueLock_.unlock();
    } else {
        HILOG_ERROR("worker:: data is nullptr.");
    }
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
}  // namespace Commonlibrary::Concurrent::WorkerModule
