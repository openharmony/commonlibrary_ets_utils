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

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "native_engine/native_engine.h"

namespace Commonlibrary::TaskPoolModule {
using TaskDataType = napi_value;
class Task {
public:
    enum TaskPriority {LOW, HIGH, IMMEDIATE};

    Task() = default;
    Task(napi_ref func, napi_value args);
    ~Task() = default;

    void Run();

    void Cancel()
    {
        canceled_ = true;
    }

    bool IsCanceled()
    {
        return canceled_;
    }

    void SetPriority(TaskPriority priority)
    {
        priority_ = priority;
    }

    napi_deferred deferred_;
    napi_env env_;
    napi_value taskData_; // Serialization Data

private:
    std::atomic<TaskPriority> priority_ {LOW};
    std::atomic<bool> canceled_ {false};
    TaskDataType serializationData;
};
} // namespace Commonlibrary::TaskPoolModule
#endif // JS_TASKPOOL_MODULE_TASKPOOL_TASK_H_