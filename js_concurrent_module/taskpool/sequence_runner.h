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

#ifndef JS_CONCURRENT_MODULE_TASKPOOL_RUNNER_H
#define JS_CONCURRENT_MODULE_TASKPOOL_RUNNER_H

#include <deque>
#include <unordered_map>

#include "task.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
class SequenceRunner {
public:
    SequenceRunner() = default;
    ~SequenceRunner() = default;
    explicit SequenceRunner(Priority priority, const std::string& name = "", bool isGlobal = false)
        : priority_(priority), seqName_(name), isGlobalRunner_(isGlobal) {}
    static napi_value SeqRunnerConstructor(napi_env env, napi_callback_info cbinfo);
    static napi_value Execute(napi_env env, napi_callback_info cbinfo);
    bool RemoveWaitingTask(Task* task);
    void AddTask(Task* task);
    void IncreaseSeqCount();
    uint64_t DecreaseSeqCount();
    void TriggerTask(napi_env env);

private:
    SequenceRunner(const SequenceRunner &) = delete;
    SequenceRunner& operator=(const SequenceRunner &) = delete;
    SequenceRunner(SequenceRunner &&) = delete;
    SequenceRunner& operator=(SequenceRunner &&) = delete;

    static void ExecuteTaskImmediately(uint32_t taskId, Priority priority);
    static void SequenceRunnerDestructor(napi_env env, void* data, void* hint);
    static bool SeqRunnerConstructorInner(napi_env env, napi_value& thisVar, SequenceRunner* seqRunner);

    friend class NativeEngineTest;
public:
    uint64_t seqRunnerId_ {};
    std::atomic<uint64_t> currentTaskId_ {};
    Priority priority_ {Priority::DEFAULT};
    std::deque<Task*> seqRunnerTasks_ {};
    std::shared_mutex seqRunnerMutex_;

    // for global SequenceRunner
    std::string seqName_ {};
    bool isGlobalRunner_ {false};
    std::atomic<uint64_t> refCount_ {1};
};
} // namespace Commonlibrary::Concurrent::TaskPoolModule
#endif // JS_CONCURRENT_MODULE_TASKPOOL_RUNNER_H