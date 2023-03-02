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

#ifndef JS_CONCURRENT_MODULE_TASKPOOL_TASK_RUNNER_H_
#define JS_CONCURRENT_MODULE_TASKPOOL_TASK_RUNNER_H_

#include <functional>

#include "native_engine/native_engine.h"
#include "thread.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
struct TaskStartCallback {
    using CallbackFunction = std::function<void(void*)>;

    explicit TaskStartCallback(CallbackFunction function = nullptr, void* dataArgs = nullptr)
        : callback(function), data(dataArgs)
    {}

    CallbackFunction callback;
    void* data;
};

class TaskRunner {
public:
    // real thread execute
    class TaskInnerRunner : public Thread {
    public:
        explicit TaskInnerRunner(const TaskRunner* runner);
        ~TaskInnerRunner() = default;
        void Run() override;

    private:
        const TaskRunner* runner_;
    };

    explicit TaskRunner(TaskStartCallback callback);
    ~TaskRunner();

    bool Execute();
    void Run() const;
    void Stop();

private:
    TaskInnerRunner* taskInnerRunner_ {nullptr};
    TaskStartCallback callback_;
    uv_thread_t selfThreadId_ {0};
};
} // namespace Commonlibrary::Concurrent::TaskPoolModule
#endif // JS_CONCURRENT_MODULE_TASKPOOL_TASK_RUNNER_H_
