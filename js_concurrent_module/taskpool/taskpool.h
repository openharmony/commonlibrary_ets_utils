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

    static void ExecuteFunction(napi_env env, TaskInfo* taskInfo, Priority priority = Priority::DEFAULT);
    static napi_value ExecuteGroup(napi_env env, napi_value taskGroup, Priority priority);
    static void HandleTaskResult(const uv_async_t* req);
    static void UpdateGroupInfoByResult(napi_env env, TaskInfo* taskInfo, napi_value res, bool success);

    friend class TaskManager;
};
} // namespace Commonlibrary::Concurrent::TaskPoolModule
#endif // JS_CONCURRENT_MODULE_TASKPOOL_TASKPOOL_H