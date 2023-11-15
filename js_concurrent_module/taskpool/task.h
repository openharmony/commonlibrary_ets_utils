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

#ifndef JS_CONCURRENT_MODULE_TASKPOOL_TASK_H
#define JS_CONCURRENT_MODULE_TASKPOOL_TASK_H

#include <string>
#include <uv.h>

#include "helper/concurrent_helper.h"
#include "napi/native_api.h"
#include "utils.h"
#include "utils/log.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
using namespace Commonlibrary::Platform;

enum ExecuteState { NOT_FOUND, WAITING, RUNNING, CANCELED };

struct GroupInfo;
class Task {
public:
    static napi_value TaskConstructor(napi_env env, napi_callback_info cbinfo);
    static napi_value SetTransferList(napi_env env, napi_callback_info cbinfo);
    static uint32_t CreateTaskByFunc(napi_env env, napi_value task, napi_value func, napi_value* args, size_t argc);
    static napi_value IsCanceled(napi_env env, napi_callback_info cbinfo);
    static napi_value OnReceiveData(napi_env env, napi_callback_info cbinfo);
    static void Destructor(napi_env env, void* nativeObject, void* finalize);

private:
    Task() = delete;
    ~Task() = delete;
    Task(const Task &) = delete;
    Task& operator=(const Task &) = delete;
    Task(Task &&) = delete;
    Task& operator=(Task &&) = delete;
};

struct TaskInfo {
    napi_env env = nullptr;
    napi_deferred deferred = nullptr;
    napi_value result = nullptr;
    napi_value serializationFunction = nullptr;
    napi_value serializationArguments = nullptr;
    uv_async_t* onResultSignal = nullptr;
    uint32_t taskId {};
    uint32_t executeId {};
    uint32_t groupExecuteId {}; // 0 for task outside taskgroup
    bool success {true};
    bool isCanceled {};
    void* worker {nullptr};
    Priority priority {Priority::DEFAULT};
    std::string funcName {};
};

struct CallbackInfo {
    CallbackInfo(napi_env env, uint32_t count, napi_ref ref) : hostEnv(env),
        refCount(count), callbackRef(ref), onCallbackSignal(nullptr) {}
    ~CallbackInfo()
    {
        napi_delete_reference(hostEnv, callbackRef);
        if (onCallbackSignal != nullptr) {
            Common::Helper::ConcurrentHelper::UvHandleClose(onCallbackSignal);
        }
    }

    napi_env hostEnv;
    uint32_t refCount;
    napi_ref callbackRef;
    uv_async_t* onCallbackSignal;
};

struct TaskResultInfo {
    TaskResultInfo(napi_env env, uint32_t id, napi_value args) : hostEnv(env),
        taskId(id), serializationArgs(args) {}
    ~TaskResultInfo() = default;

    napi_env hostEnv;
    uint32_t taskId;
    napi_value serializationArgs;
};
} // namespace Commonlibrary::Concurrent::TaskPoolModule
#endif // JS_CONCURRENT_MODULE_TASKPOOL_TASK_H