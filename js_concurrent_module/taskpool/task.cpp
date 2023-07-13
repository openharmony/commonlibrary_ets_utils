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
#include "utils/log.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
static constexpr char SETTRANSFERLIST_STR[] = "setTransferList";
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
    if (!NapiHelper::IsFunction(args[0])) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: the first param of task must be function");
        return nullptr;
    }
    CreateTaskByFunc(env, thisVar, args[0], args, argc);
    return thisVar;
}

void Task::CreateTaskByFunc(napi_env env, napi_value task, napi_value func, napi_value* args, size_t argc)
{
    napi_value argsArray;
    napi_create_array_with_length(env, argc - 1, &argsArray);
    for (size_t i = 0; i < argc - 1; i++) {
        napi_set_element(env, argsArray, i, args[i + 1]);
    }

    napi_value taskId = NapiHelper::CreateUint32(env, TaskManager::GetInstance().GenerateTaskId());

    napi_value setTransferListFunc;
    napi_create_function(env, SETTRANSFERLIST_STR, NAPI_AUTO_LENGTH, SetTransferList, NULL, &setTransferListFunc);

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_PROPERTY(FUNCTION_STR, args[0]),
        DECLARE_NAPI_PROPERTY(ARGUMENTS_STR, argsArray),
        DECLARE_NAPI_PROPERTY(TASKID_STR, taskId),
        DECLARE_NAPI_FUNCTION(SETTRANSFERLIST_STR, SetTransferList),
    };
    napi_define_properties(env, task, sizeof(properties) / sizeof(properties[0]), properties);
}

napi_value Task::SetTransferList(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = 1;
    napi_value args[1];
    napi_value thisVar;
    napi_get_cb_info(env, cbinfo, &argc, args, &thisVar, nullptr);
    if (argc > 1) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
                                "taskpool:: the number of setTransferList parma must be less than 2");
        return nullptr;
    }
    if (argc == 0) {
        HILOG_DEBUG("taskpool:: set task params not transfer");
        return nullptr;
    }

    // setTransferList(ArrayBuffer[]), check ArrayBuffer[]
    if (!NapiHelper::IsArray(args[0])) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: setTransferList first param must be array");
        return nullptr;
    }
    uint32_t arrayLength = NapiHelper::GetArrayLength(env, args[0]);
    for (size_t i = 0; i < arrayLength; i++) {
        napi_value elementVal;
        napi_get_element(env, args[0], i, &elementVal);
        if (!NapiHelper::IsArrayBuffer(elementVal)) {
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
                                    "taskpool:: the element in array must be arraybuffer");
            return nullptr;
        }
    }
    HILOG_DEBUG("taskpool:: check setTransferList param success");

    napi_set_named_property(env, thisVar, TRANSFERLIST_STR, args[0]);
    return nullptr;
}
} // namespace Commonlibrary::Concurrent::TaskPoolModule