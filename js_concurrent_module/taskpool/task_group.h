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

#ifndef JS_CONCURRENT_MODULE_TASKPOOL_TASK_GROUP_H
#define JS_CONCURRENT_MODULE_TASKPOOL_TASK_GROUP_H

#include <list>

#include "task.h"
#include "task_manager.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
class TaskGroup {
public:
    static napi_value TaskGroupConstructor(napi_env env, napi_callback_info cbinfo);
    static napi_value AddTask(napi_env env, napi_callback_info cbinfo);

private:
    TaskGroup() = delete;
    ~TaskGroup() = delete;
    TaskGroup(const TaskGroup &) = delete;
    TaskGroup& operator=(const TaskGroup &) = delete;
    TaskGroup(TaskGroup &&) = delete;
    TaskGroup& operator=(TaskGroup &&) = delete;

    static void Destructor(napi_env env, void* nativeObject, void* finalize);
};

struct GroupInfo {
    napi_deferred deferred = nullptr;
    uint32_t groupId {};
    uint32_t taskNum {};
    uint32_t finishedTask {};
    std::list<uint32_t> executeIds {};
    napi_ref resArr = nullptr;
};

} // namespace Commonlibrary::Concurrent::TaskPoolModule
#endif // JS_CONCURRENT_MODULE_TASKPOOL_TASK_GROUP_H