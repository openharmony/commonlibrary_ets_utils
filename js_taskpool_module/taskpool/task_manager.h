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

#ifndef JS_TASKPOOL_MODULE_TASKPOOL_TASK_H_
#define JS_TASKPOOL_MODULE_TASKPOOL_TASK_H_

#include <memory>
#include <uv.h>

#include "napi/native_api.h"

namespace Commonlibrary::TaskPoolModule {
using TaskDataType = napi_value;

enum TaskState { NOT_FOUND, WAITING, RUNNING, TERMINATED, CANCELED };

struct TaskInfo {
    napi_env env = nullptr;
    napi_deferred deferred = nullptr;
    napi_value promise = nullptr;
    napi_value result = nullptr;
    napi_value serializationData = nullptr;
    uv_async_t *taskSignal = nullptr;
    uint32_t taskId;
    uint32_t executeId;
};

class Task {
public:
    Task() = default;
    ~Task() = default;

    static napi_value TaskConstructor(napi_env env, napi_callback_info cbinfo);

    napi_ref objRef_;
    uint32_t executeId_;
    uint32_t taskId_;
};

class TaskManager {
public:
    static uint32_t GenerateTaskId();
    static uint32_t GenerateExecuteId();
    static TaskInfo* PopTaskInfo(uint32_t executeId);
    static void ClearTaskInfo();
    static void StoreTaskInfo(uint32_t executeId, TaskInfo* taskInfo);
    static void StoreStateInfo(uint32_t executeId, TaskState state);
    static void StoreRunningInfo(uint32_t taskId, uint32_t executeId);
    static bool UpdateState(uint32_t executeId, TaskState state);
    static void ReleaseTaskContent(TaskInfo* taskInfo);
    static void PopRunningInfo(uint32_t taskId, uint32_t executeId);
    static void CancelTask(napi_env env, uint32_t taskId);
    static TaskState QueryState(uint32_t executeId);

private:
    TaskManager() = delete;
    ~TaskManager() = delete;

    TaskManager(const TaskManager &) = delete;
    TaskManager& operator=(const TaskManager &) = delete;
    TaskManager(TaskManager &&) = delete;
    TaskManager& operator=(TaskManager &&) = delete;
};
} // namespace Commonlibrary::TaskPoolModule
#endif // JS_TASKPOOL_MODULE_TASKPOOL_TASK_H_