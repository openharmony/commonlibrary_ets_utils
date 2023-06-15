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

#include "task_group.h"

#include "helper/error_helper.h"
#include "helper/napi_helper.h"
#include "helper/object_helper.h"
#include "napi/native_api.h"
#include "utils/log.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
using namespace Commonlibrary::Concurrent::Common::Helper;

napi_value TaskGroup::TaskGroupConstructor(napi_env env, napi_callback_info cbinfo)
{
    napi_value thisVar;
    napi_get_cb_info(env, cbinfo, nullptr, nullptr, &thisVar, nullptr);

    napi_value groupId;
    uint32_t id = TaskGroupManager::GetInstance().GenerateGroupId();
    napi_create_uint32(env, id, &groupId);
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_PROPERTY(GROUP_ID_STR, groupId),
        DECLARE_NAPI_FUNCTION_WITH_DATA("addTask", AddTask, thisVar),
    };
    napi_define_properties(env, thisVar, sizeof(properties) / sizeof(properties[0]), properties);
    uint32_t* data = new uint32_t();
    *data = id;
    napi_wrap(env, thisVar, data, Destructor, nullptr, nullptr);

    return thisVar;
}

void TaskGroup::Destructor(napi_env env, void* data, [[maybe_unused]] void* hint)
{
    uint32_t* groupId = reinterpret_cast<uint32_t*>(data);
    TaskGroupManager::GetInstance().ClearTasks(env, *groupId);
    delete groupId;
}

napi_value TaskGroup::AddTask(napi_env env, napi_callback_info cbinfo)
{
    // Check the argc
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    if (argc < 1) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskGroup:: the number of params must be at least one");
        return nullptr;
    }

    // Check the first param is task or func
    napi_value* args = new napi_value[argc];
    ObjectScope<napi_value> scope(args, true);

    // Get groupId from this
    napi_value thisVar;
    napi_get_cb_info(env, cbinfo, &argc, args, &thisVar, nullptr);
    napi_value groupIdVal = NapiHelper::GetNameProperty(env, thisVar, GROUP_ID_STR);
    uint32_t groupId = NapiHelper::GetUint32Value(env, groupIdVal);

    napi_valuetype type;
    napi_typeof(env, args[0], &type);
    if (type == napi_object) {
        napi_ref taskRef = NapiHelper::CreateReference(env, args[0], 1);
        TaskGroupManager::GetInstance().AddTask(groupId, taskRef);
        return nullptr;
    } else if (type == napi_function) {
        napi_value task = nullptr;
        napi_create_object(env, &task);
        Task::CreateTaskByFunc(env, task, args[0], args, argc);
        napi_ref taskRef = NapiHelper::CreateReference(env, task, 1);
        TaskGroupManager::GetInstance().AddTask(groupId, taskRef);
        return nullptr;
    }
    ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskGroup:: first param must be object or function");
    return nullptr;
}
} // namespace Commonlibrary::Concurrent::TaskPoolModule