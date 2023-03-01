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

#include "task.h"

#include "helper/error_helper.h"
#include "helper/object_helper.h"
#include "task_manager.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
using namespace Commonlibrary::Concurrent::Common::Helper;

napi_value Task::TaskConstructor(napi_env env, napi_callback_info cbinfo)
{
    // check argv count
    size_t argc = 0;
    napi_get_cb_info(env, cbinfo, &argc, nullptr, nullptr, nullptr);
    if (argc < 1) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: create task need more than one param");
        return nullptr;
    }

    // check 1st param is func
    napi_value thisVar = nullptr;
    void* data = nullptr;
    napi_value* args = new napi_value[argc];
    ObjectScope<napi_value> scope(args, true);
    napi_get_cb_info(env, cbinfo, &argc, args, &thisVar, &data);
    napi_valuetype type;
    NAPI_CALL(env, napi_typeof(env, args[0], &type));
    if (type != napi_function) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: the first param of task must be function");
        return nullptr;
    }

    napi_value argsArray;
    napi_create_array_with_length(env, argc - 1, &argsArray);
    for (size_t i = 0; i < argc - 1; i++) {
        napi_set_element(env, argsArray, i, args[i + 1]);
    }

    napi_value object = nullptr;
    napi_create_object(env, &object);
    napi_set_named_property(env, object, "func", args[0]);
    napi_set_named_property(env, object, "args", argsArray);

    Task* task = new (std::nothrow) Task();
    napi_ref objRef = nullptr;
    napi_create_reference(env, object, 1, &objRef);
    task->objRef_ = objRef;
    task->taskId_ = TaskManager::GetInstance().GenerateTaskId();
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
} // namespace Commonlibrary::Concurrent::TaskPoolModule