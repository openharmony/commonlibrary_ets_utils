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

#ifndef JS_CONCURRENT_MODULE_TASKPOOL_THREAD_H_
#define JS_CONCURRENT_MODULE_TASKPOOL_THREAD_H_

#include <uv.h>

namespace Commonlibrary::Concurrent::TaskPoolModule {
class Thread {
public:
    Thread();
    virtual ~Thread() = default;
    bool Start();
    virtual void Run() = 0;

    uv_thread_t GetThreadId() const
    {
        return tid_;
    }

private:
    uv_thread_t tid_ {0};
};
}  // namespace Commonlibrary::Concurrent::TaskPoolModule
#endif // JS_CONCURRENT_MODULE_TASKPOOL_THREAD_H_