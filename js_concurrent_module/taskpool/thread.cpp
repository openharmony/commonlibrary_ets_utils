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

#if defined(ENABLE_TASKPOOL_FFRT)
#include "ffrt_inner.h"
#endif
#include "task_manager.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
Thread::Thread() : tid_() {}

bool Thread::Start()
{
    if (TaskManager::GetInstance().IsSystemApp()) {
        HILOG_INFO("taskpool:: system apps use ffrt");
#if defined(ENABLE_TASKPOOL_FFRT)
        ffrt::task_attr task_attr;
        (void)ffrt_task_attr_init(&task_attr);
        ffrt_task_attr_set_name(&task_attr, "OS_TaskWorker");
        ffrt_task_attr_set_qos(&task_attr, ffrt_qos_user_initiated);
        ffrt_task_attr_set_local(&task_attr, true);

        auto task = [this]() {
            Thread* thread = reinterpret_cast<Thread*>(this);
            thread->Run();
        };
        ffrt::submit(task, {}, {}, task_attr);
#endif
        return 0;
    } else {
        HILOG_INFO("taskpool:: other apps do not use ffrt");
        int ret = uv_thread_create(&tid_, [](void* arg) {
#if defined IOS_PLATFORM || defined MAC_PLATFORM
            pthread_setname_np("OS_TaskWorker");
#else
            pthread_setname_np(pthread_self(), "OS_TaskWorker");
#endif
            Thread* thread = reinterpret_cast<Thread*>(arg);
            thread->Run();
        }, this);
        return ret != 0;
    }
}
}  // namespace Commonlibrary::Concurrent::TaskPoolModule