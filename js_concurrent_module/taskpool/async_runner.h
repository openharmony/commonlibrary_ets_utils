/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef JS_CONCURRENT_MODULE_TASKPOOL_ASYNC_RUNNER_H
#define JS_CONCURRENT_MODULE_TASKPOOL_ASYNC_RUNNER_H

#include <deque>
#include <unordered_map>
#include "task.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {

struct ErrorMessage {
    uint32_t taskId {};
    int32_t errCode = 0;
    bool isWaiting = false;
};

class AsyncRunner {
public:
    AsyncRunner() = default;
    ~AsyncRunner() = default;

    static napi_value AsyncRunnerConstructor(napi_env env, napi_callback_info cbinfo);
    static napi_value Execute(napi_env env, napi_callback_info cbinfo);
    static AsyncRunner* CreateGlobalRunner(const std::string &name, uint32_t runningCapacity, uint32_t waitingCapacity);
    bool RemoveWaitingTask(Task* task, bool isReject = true);
    void TriggerRejectErrorTimer(Task* task, int32_t errCode, bool isWaiting = false);
    void TriggerWaitingTask();
    void CreateGlobalRef(napi_env env, napi_value thisVar);
    bool DecreaseAsyncRunnerRef(napi_env env);
    bool IncreaseAsyncRunnerRef(napi_env env);
    uint64_t DecreaseAsyncCount();
    void RemoveGlobalAsyncRunnerRef(napi_env env);
    bool CheckGlobalRunnerParams(napi_env env, uint32_t runningCapacity, uint32_t waitingCapacity);
private:
    AsyncRunner(const AsyncRunner &) = delete;
    AsyncRunner& operator=(const AsyncRunner &) = delete;
    AsyncRunner(AsyncRunner &&) = delete;
    AsyncRunner& operator=(AsyncRunner &&) = delete;

    static bool AsyncRunnerConstructorInner(napi_env env, napi_value &thisVar, AsyncRunner *asyncRunner);
    static void ExecuteTaskImmediately(AsyncRunner* asyncRunner, Task* task);
    static void AsyncRunnerDestructor(napi_env env, void* data, void* hint);
    static AsyncRunner* CheckAndCreateAsyncRunner(napi_env env, napi_value &thisVar, napi_value name,
                                                  napi_value runningCapacity, napi_value waitingCapacity);
    static bool CheckExecuteArgs(napi_env env, napi_value napiTask, napi_value napiPriority);
    static bool AddTasksToAsyncRunner(AsyncRunner* asyncRunner, Task* task);

    friend class NativeEngineTest;
public:
    uint64_t asyncRunnerId_ {};
    uint32_t runningCapacity_ {};
    uint32_t waitingCapacity_ {};
    std::atomic<uint32_t> runningCount_ {}; // running task count
    napi_ref asyncRunnerRef_ = nullptr;

    // for global AsyncRunner
    std::string name_ {};
    std::atomic<bool> isGlobalRunner_ {false};
    std::atomic<uint64_t> count_ {};
    std::unordered_map<napi_env, napi_ref> globalAsyncRunnerRef_ {};
    std::deque<Task*> waitingTasks_ {};
    std::shared_mutex asyncRunnerMutex_;
    std::shared_mutex waitingTasksMutex_;
};
} // namespace Commonlibrary::Concurrent::TaskPoolModule
#endif // JS_CONCURRENT_MODULE_TASKPOOL_ASYNC_RUNNER_H