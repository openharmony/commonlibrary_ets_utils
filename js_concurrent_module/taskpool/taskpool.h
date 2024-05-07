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

#ifndef JS_CONCURRENT_MODULE_TASKPOOL_TASKPOOL_H
#define JS_CONCURRENT_MODULE_TASKPOOL_TASKPOOL_H

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "native_engine/native_engine.h"
#include "task.h"
#include "task_group.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
using namespace Commonlibrary::Concurrent::Common;

struct TaskMessage {
    napi_deferred deferred = nullptr;
    Priority priority {Priority::DEFAULT};
    uint64_t taskId {};
};

class TaskPool {
public:
    static napi_value InitTaskPool(napi_env env, napi_value exports);
    static void ExecuteCallback(const uv_async_t* req);
    static void HandleTaskResult(const uv_async_t* req);

private:
    TaskPool() = delete;
    ~TaskPool() = delete;
    TaskPool(const TaskPool &) = delete;
    TaskPool& operator=(const TaskPool &) = delete;
    TaskPool(TaskPool &&) = delete;
    TaskPool& operator=(TaskPool &&) = delete;

    static void DelayTask(uv_timer_t* handle);
    static napi_value Execute(napi_env env, napi_callback_info cbinfo);
    static napi_value ExecuteDelayed(napi_env env, napi_callback_info cbinfo);
    static napi_value Cancel(napi_env env, napi_callback_info cbinfo);
    static napi_value GetTaskPoolInfo(napi_env env, [[maybe_unused]] napi_callback_info cbinfo);
    static napi_value TerminateTask(napi_env env, napi_callback_info cbinfo);
    static napi_value IsConcurrent(napi_env env, napi_callback_info cbinfo);

    static void UpdateGroupInfoByResult(napi_env env, Task* task, napi_value res, bool success);
    static void ExecuteTask(napi_env env, Task* task, Priority priority = Priority::DEFAULT);
    static napi_value ExecuteGroup(napi_env env, napi_value taskGroup, Priority priority);

    static void TriggerTask(Task* task);
    static void ExecuteCallbackInner(MsgQueue& msgQueue);
    friend class TaskManager;
};

class CallbackScope {
public:
    CallbackScope(napi_env env, napi_env workerEnv, uint64_t taskId, napi_status& status): env_(env),
        workerEnv_(workerEnv), taskId_(taskId)
    {
        status = napi_open_handle_scope(env_, &scope_);
    }
    ~CallbackScope()
    {
        TaskManager::GetInstance().DecreaseRefCount(env_, taskId_);
        if (workerEnv_ != nullptr) {
            auto workerEngine = reinterpret_cast<NativeEngine*>(workerEnv_);
            workerEngine->DecreaseListeningCounter();
        }

        if (scope_ != nullptr) {
            napi_close_handle_scope(env_, scope_);
        }
    }
private:
    napi_env env_;
    napi_env workerEnv_;
    uint64_t taskId_;
    napi_handle_scope scope_ = nullptr;
};
} // namespace Commonlibrary::Concurrent::TaskPoolModule
#endif // JS_CONCURRENT_MODULE_TASKPOOL_TASKPOOL_H