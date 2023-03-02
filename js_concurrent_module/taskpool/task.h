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

#ifndef JS_CONCURRENT_MODULE_TASKPOOL_TASK_H_
#define JS_CONCURRENT_MODULE_TASKPOOL_TASK_H_

#include <mutex>
#include <queue>
#include <uv.h>

#include "napi/native_api.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
enum TaskState { NOT_FOUND, WAITING, RUNNING, TERMINATED, CANCELED };

class Task {
public:
    Task() = default;
    ~Task() = default;

    static napi_value TaskConstructor(napi_env env, napi_callback_info cbinfo);

    napi_ref objRef_;
    uint32_t executeId_ = 0;
    uint32_t taskId_ = 0;
};

struct TaskInfo {
    napi_env env = nullptr;
    napi_deferred deferred = nullptr;
    napi_value promise = nullptr;
    napi_value result = nullptr;
    napi_value serializationData = nullptr;
    uv_async_t *onResultSignal = nullptr;
    uint32_t taskId;
    uint32_t executeId;
    bool success = true;
    void *worker = nullptr;
};
} // namespace Commonlibrary::Concurrent::TaskPoolModule
#endif // JS_CONCURRENT_MODULE_TASKPOOL_TASK_H_