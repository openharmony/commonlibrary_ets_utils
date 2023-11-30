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
#include "taskpool.h"
#include "utils/log.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
static constexpr char ONRECEIVEDATA_STR[] = "onReceiveData";
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
    if (!NapiHelper::IsFunction(env, args[0])) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: the first param of task must be function");
        return nullptr;
    }
    uint32_t taskId = CreateTaskByFunc(env, thisVar, args[0], args, argc);
    uint32_t* idPointer = new uint32_t(taskId);
    napi_wrap(env, thisVar, idPointer, Destructor, nullptr, nullptr);
    return thisVar;
}

void Task::Destructor(napi_env env, void* data, [[maybe_unused]] void* hint)
{
    uint32_t* taskId = reinterpret_cast<uint32_t*>(data);
    TaskManager::GetInstance().DecreaseRefCount(env, *taskId);
    delete taskId;
}

uint32_t Task::CreateTaskByFunc(napi_env env, napi_value task, napi_value func, napi_value* args, size_t argc)
{
    napi_value argsArray;
    napi_create_array_with_length(env, argc - 1, &argsArray);
    for (size_t i = 0; i < argc - 1; i++) {
        napi_set_element(env, argsArray, i, args[i + 1]);
    }

    napi_value taskId = NapiHelper::CreateUint32(env, TaskManager::GetInstance().GenerateTaskId());
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION(SETTRANSFERLIST_STR, SetTransferList),
        DECLARE_NAPI_FUNCTION(ONRECEIVEDATA_STR, OnReceiveData),
    };

    napi_set_named_property(env, task, TASKID_STR, taskId);
    napi_define_properties(env, task, sizeof(properties) / sizeof(properties[0]), properties);
    napi_set_named_property(env, task, FUNCTION_STR, args[0]);
    napi_set_named_property(env, task, ARGUMENTS_STR, argsArray);
    return NapiHelper::GetUint32Value(env, taskId);
}

napi_value Task::SetTransferList(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = 1;
    napi_value args[1];
    napi_value thisVar;
    napi_value undefined = NapiHelper::GetUndefinedValue(env);
    napi_get_cb_info(env, cbinfo, &argc, args, &thisVar, nullptr);
    if (argc > 1) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
                                "taskpool:: the number of setTransferList parma must be less than 2");
        return nullptr;
    }
    if (argc == 0) {
        HILOG_DEBUG("taskpool:: set task params not transfer");
        napi_set_named_property(env, thisVar, TRANSFERLIST_STR, undefined);
        return nullptr;
    }

    // setTransferList(ArrayBuffer[]), check ArrayBuffer[]
    if (!NapiHelper::IsArray(env, args[0])) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskpool:: setTransferList first param must be array");
        return nullptr;
    }
    uint32_t arrayLength = NapiHelper::GetArrayLength(env, args[0]);
    if (arrayLength == 0) {
        HILOG_DEBUG("taskpool:: set task params not transfer");
        napi_set_named_property(env, thisVar, TRANSFERLIST_STR, undefined);
        return nullptr;
    }
    for (size_t i = 0; i < arrayLength; i++) {
        napi_value elementVal;
        napi_get_element(env, args[0], i, &elementVal);
        if (!NapiHelper::IsArrayBuffer(env, elementVal)) {
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
                                    "taskpool:: the element in array must be arraybuffer");
            return nullptr;
        }
    }
    HILOG_DEBUG("taskpool:: check setTransferList param success");

    napi_set_named_property(env, thisVar, TRANSFERLIST_STR, args[0]);
    return nullptr;
}

napi_value Task::IsCanceled(napi_env env, napi_callback_info cbinfo)
{
    bool isCanceled = false;
    auto engine = reinterpret_cast<NativeEngine*>(env);
    if (!engine->IsTaskPoolThread()) {
        HILOG_ERROR("taskpool:: call isCanceled not in taskpool thread");
        return NapiHelper::CreateBooleanValue(env, isCanceled);
    }
    // Get taskInfo and query task cancel state
    void* data = engine->GetCurrentTaskInfo();
    if (data == nullptr) {
        HILOG_ERROR("taskpool:: call isCanceled not in Concurrent function");
    } else {
        TaskInfo* taskInfo = static_cast<TaskInfo*>(data);
        isCanceled = taskInfo->isCanceled;
    }
    return NapiHelper::CreateBooleanValue(env, isCanceled);
}

napi_value Task::OnReceiveData(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    if (argc >= 2) { // 2: the number of parmas
        HILOG_ERROR("taskpool:: the number of OnReceiveData parma must be less than 2");
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
            "taskpool:: the number of OnReceiveData parma must be less than 2");
        return nullptr;
    }

    napi_value thisVar;
    if (argc == 0) {
        HILOG_INFO("taskpool:: Set taskpool.Task.onReceiveData to undefined");
        napi_get_cb_info(env, cbinfo, &argc, nullptr, &thisVar, nullptr);
        napi_value id = NapiHelper::GetNameProperty(env, thisVar, "taskId");
        uint32_t taskId = NapiHelper::GetUint32Value(env, id);
        TaskManager::GetInstance().RegisterCallback(env, taskId, nullptr);
        return nullptr;
    }

    napi_value args[1];
    napi_get_cb_info(env, cbinfo, &argc, args, &thisVar, nullptr);
    napi_valuetype type;
    NAPI_CALL(env, napi_typeof(env, args[0], &type));
    if (type != napi_function) {
        HILOG_ERROR("taskpool:: OnReceiveData's parameter should be function");
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
            "taskpool:: OnReceiveData's parameter should be function");
        return nullptr;
    }
    // store callbackInfo
    napi_value id = NapiHelper::GetNameProperty(env, thisVar, "taskId");
    uint32_t taskId = NapiHelper::GetUint32Value(env, id);
    napi_ref callbackRef = Helper::NapiHelper::CreateReference(env, args[0], 1);
    std::shared_ptr<CallbackInfo> callbackInfo = std::make_shared<CallbackInfo>(env, 1, callbackRef);
    callbackInfo->onCallbackSignal = new uv_async_t;
    auto loop = NapiHelper::GetLibUV(env);
    uv_async_init(loop, callbackInfo->onCallbackSignal, TaskPool::ExecuteCallback);
    TaskManager::GetInstance().RegisterCallback(env, taskId, callbackInfo);
    return nullptr;
}
} // namespace Commonlibrary::Concurrent::TaskPoolModule