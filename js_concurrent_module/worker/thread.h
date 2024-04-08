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

#ifndef JS_CONCURRENT_MODULE_WORKER_THREAD_H
#define JS_CONCURRENT_MODULE_WORKER_THREAD_H

#include <pthread.h>
#include <uv.h>

namespace Commonlibrary::Concurrent::WorkerModule {
class Thread {
public:
    Thread();
    virtual ~Thread() = default;
    bool Start();
    static void* ThreadFunction(void* arg);
    virtual void Run() = 0;

    pthread_t GetThreadId() const
    {
        return tId_;
    }

private:
    pthread_t tId_ {0};
};
}  // namespace Commonlibrary::Concurrent::WorkerModule

#endif // #define JS_CONCURRENT_MODULE_WORKER_THREAD_H
