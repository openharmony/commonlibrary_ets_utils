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

#ifndef JS_CONCURRENT_MODULE_TASKPOOL_TASKPOOL_H_
#define JS_CONCURRENT_MODULE_TASKPOOL_TASKPOOL_H_

#include <mutex>

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "native_engine/native_engine.h"
#include "task.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
enum Priority {
    HIGH,
    MEDIUM,
    LOW
};

class TaskPool {
public:
    static napi_value InitTaskPool(napi_env env, napi_value exports);

private:
    TaskPool() = delete;
    ~TaskPool() = delete;
    TaskPool(const TaskPool &) = delete;
    TaskPool& operator=(const TaskPool &) = delete;
    TaskPool(TaskPool &&) = delete;
    TaskPool& operator=(TaskPool &&) = delete;

    static napi_value Execute(napi_env env, napi_callback_info cbinfo);
    static napi_value Cancel(napi_env env, napi_callback_info cbinfo);

    static napi_value ExecuteFunction(napi_env env, napi_value object);
    static napi_value ExecuteTask(napi_env env, Task* task);
    static void HandleTaskResult(const uv_async_t* req);

    friend class TaskManager;
};
} // namespace Commonlibrary::Concurrent::TaskPoolModule
#endif // JS_CONCURRENT_MODULE_TASKPOOL_TASKPOOL_H_