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

#include "worker.h"

#include <shared_mutex>
#include <unistd.h>
#include <unordered_map>

#include "taskpool.h"
#include "napi_helper.h"
#include "object_helper.h"
#include "utils/log.h"

namespace Commonlibrary::TaskPoolModule {
using namespace CompilerRuntime::WorkerModule::Helper;

napi_value Task::TaskConstructor(napi_env env, napi_callback_info cbinfo)
{
    // check argv count
    size_t argc = 0;
    napi_get_cb_info(env, cbinfo, &argc, nullptr, nullptr, nullptr);
    if (argc < 1) {
        Worker::ThrowError(env, Worker::TYPE_ERROR, "the number of create task params must be more than 1 with new");
        return nullptr;
    }

    // check 1st param is string
    napi_value thisVar = nullptr;
    void* data = nullptr;
    napi_value* args = new napi_value[argc];
    ObjectScope<napi_value> scope(args, true);
    napi_get_cb_info(env, cbinfo, &argc, args, &thisVar, &data);
    Task* task = nullptr;
    task = new Task();
    if (task == nullptr) {
        Worker::ThrowError(env, Worker::INITIALIZATION_ERROR, "create task error");
        return nullptr;
    }

    TaskPool::GenerateTaskId(task);
    napi_wrap(
        env, thisVar, task,
        [](napi_env env, void *data, void *hint) {
            auto obj = reinterpret_cast<Task*>(data);
            if (obj != nullptr) {
                delete obj;
            }
        },
        nullptr, nullptr);
    return thisVar;
}

napi_value Task::Cancel(napi_env env, napi_callback_info cbinfo)
{
    return nullptr;
}
} // namespace Commonlibrary::TaskPoolModule