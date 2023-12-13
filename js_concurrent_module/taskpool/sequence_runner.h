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

#include <queue>
#include "task.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {

class SequenceRunner {
public:
    static napi_value SeqRunnerConstructor(napi_env env, napi_callback_info cbinfo);
    static napi_value Execute(napi_env env, napi_callback_info cbinfo);

private:
    SequenceRunner() = delete;
    ~SequenceRunner() = delete;
    SequenceRunner(const SequenceRunner &) = delete;
    SequenceRunner& operator=(const SequenceRunner &) = delete;
    SequenceRunner(SequenceRunner &&) = delete;
    SequenceRunner& operator=(SequenceRunner &&) = delete;
    static void RegisterSeqRunner(napi_env env, uint32_t seqRunnerId, Priority pri);
    static void Destructor(napi_env env, void* nativeObject, void* finalize);
    static void ExecuteTaskImmediately(uint32_t executeId, Priority priority);
};

struct SeqRunnerInfo {
    bool releasable {false};
    uint32_t currentExeId {};
    napi_ref ref = nullptr;
    Priority priority {Priority::DEFAULT};
    std::queue<TaskInfo*> seqRunnerTasks {};
};
}

#endif // JS_CONCURRENT_MODULE_TASKPOOL_RUNNER_H