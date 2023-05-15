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

#include "task.h"

#include "helper/error_helper.h"
#include "helper/napi_helper.h"
#include "helper/object_helper.h"
#include "task_manager.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
using namespace Commonlibrary::Concurrent::Common::Helper;

napi_value Task::TaskConstructor(napi_env env, napi_callback_info cbinfo)
{
    // check argv count
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    if (argc < 1) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: create task need more than one param");
        return nullptr;
    }

    // check 1st param is func
    napi_value* args = new napi_value[argc];
    ObjectScope<napi_value> scope(args, true);
    napi_value thisVar;
    napi_get_cb_info(env, cbinfo, &argc, args, &thisVar, nullptr);
    napi_valuetype type;
    napi_typeof(env, args[0], &type);
    if (type != napi_function) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: the first param of task must be function");
        return nullptr;
    }

    napi_value argsArray;
    napi_create_array_with_length(env, argc - 1, &argsArray);
    for (size_t i = 0; i < argc - 1; i++) {
        napi_set_element(env, argsArray, i, args[i + 1]);
    }

    napi_value taskId;
    napi_create_uint32(env, TaskManager::GetInstance().GenerateTaskId(), &taskId);

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_PROPERTY(FUNCTION_STR, args[0]),
        DECLARE_NAPI_PROPERTY(ARGUMENTS_STR, argsArray),
        DECLARE_NAPI_PROPERTY(TASKID_STR, taskId),
    };
    napi_define_properties(env, thisVar, sizeof(properties) / sizeof(properties[0]), properties);

    return thisVar;
}
} // namespace Commonlibrary::Concurrent::TaskPoolModule