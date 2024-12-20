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

#include "thread.h"

namespace Commonlibrary::Concurrent::WorkerModule {
Thread::Thread() : tId_() {}

bool Thread::Start()
{
    int ret = uv_thread_create(&tId_, [](void* arg) {
#if defined IOS_PLATFORM || defined MAC_PLATFORM
        pthread_setname_np("WorkerThread");
#else
        pthread_setname_np(pthread_self(), "WorkerThread");
#endif
        Thread* thread = reinterpret_cast<Thread*>(arg);
        thread->Run();
    }, this);
    return ret != 0;
}
}  // namespace Commonlibrary::Concurrent::WorkerModule