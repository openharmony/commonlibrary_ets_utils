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

#ifndef JS_CONCURRENT_MODULE_WORKER_WORKER_RUNNER_H
#define JS_CONCURRENT_MODULE_WORKER_WORKER_RUNNER_H

#include <functional>

#include "native_engine/native_engine.h"
#include "thread.h"

namespace Commonlibrary::Concurrent::WorkerModule {
struct WorkerStartCallback {
    using CallbackFunction = std::function<void(void*)>;

    explicit WorkerStartCallback(CallbackFunction function = nullptr, void* dataArgs = nullptr)
        : callback(function), data(dataArgs)
    {}

    CallbackFunction callback;
    void* data;
};

class WorkerRunner {
public:
    // real thread execute
    class WorkerInnerRunner : public Thread {
    public:
        explicit WorkerInnerRunner(const WorkerRunner* runner);
        ~WorkerInnerRunner() = default;
        void Run() override;

    private:
        const WorkerRunner* runner_;
    };

    explicit WorkerRunner(WorkerStartCallback callback);
    ~WorkerRunner();

    bool Execute();
    void Run() const;
    void Stop();

private:
    WorkerInnerRunner* workerInnerRunner_ {nullptr};
    WorkerStartCallback callback_;
    uv_thread_t selfThreadId_ {0};
};
} // namespace Commonlibrary::Concurrent::WorkerModule
#endif // JS_CONCURRENT_MODULE_WORKER_WORKER_RUNNER_H
