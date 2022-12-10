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
struct TaskInfo {
    napi_env env = nullptr;
    napi_deferred deferred = nullptr;
    napi_value promise = nullptr;
    napi_value result = nullptr;
    napi_value serializationData = nullptr;
    bool canceled = false;
    uint32_t taskId;
    uv_async_t *taskSignal = nullptr;
};

class Task {
public:
    Task() = default;
    ~Task() = default;

    static napi_value TaskConstructor(napi_env env, napi_callback_info cbinfo);

    uint32_t taskId_;
    napi_ref objRef_;
};
} // namespace Commonlibrary::TaskPoolModule
#endif // JS_TASKPOOL_MODULE_TASKPOOL_TASK_H_