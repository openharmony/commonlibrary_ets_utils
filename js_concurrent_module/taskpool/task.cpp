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

    napi_value* args = new napi_value[argc];
    ObjectScope<napi_value> scope(args, true);
    napi_value thisVar;
    napi_value func = nullptr;
    napi_value name = NapiHelper::CreateEmptyString(env);
    napi_get_cb_info(env, cbinfo, &argc, args, &thisVar, nullptr);
    // if the first is task name, the second might be func
    if (argc > 1 && NapiHelper::IsString(env, args[0])) {
        name = args[0];
        func = args[1];
        // the fun params start from the third
        args += 2; // 2: skip name and func
        argc -= 2; // 2: skip name and func
    } else {
        func = args[0];
        // the fun params start from the second
        args += 1;
        argc -= 1;
    }

    // check 1st or 2nd param is func
    if (!NapiHelper::IsFunction(env, func)) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
                                "taskpool:: the first or second param of task must be function");
        return nullptr;
    }
    uint32_t taskId = CreateTaskByFunc(env, thisVar, func, name, args, argc);
    uint32_t* idPointer = new uint32_t(taskId);
    napi_wrap(env, thisVar, idPointer, Destructor, nullptr, nullptr);
    return thisVar;
}

void Task::Destructor(napi_env env, void* data, [[maybe_unused]] void* hint)
{
    uint32_t* taskId = reinterpret_cast<uint32_t*>(data);
    TaskManager::GetInstance().ReleaseTaskData(env, *taskId);
    delete taskId;
}

uint32_t Task::CreateTaskByFunc(napi_env env, napi_value task, napi_value func,
                                napi_value name, napi_value* args, size_t argc)
{
    napi_value argsArray;
    napi_create_array_with_length(env, argc, &argsArray);
    for (size_t i = 0; i < argc; i++) {
        napi_set_element(env, argsArray, i, args[i]);
    }

    napi_value taskId = NapiHelper::CreateUint32(env, TaskManager::GetInstance().GenerateTaskId());
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION(SETTRANSFERLIST_STR, SetTransferList),
        DECLARE_NAPI_FUNCTION(ONRECEIVEDATA_STR, OnReceiveData),
        DECLARE_NAPI_FUNCTION(ADD_DEPENDENCY_STR, AddDependency),
        DECLARE_NAPI_FUNCTION(REMOVE_DEPENDENCY_STR, RemoveDependency),
        DECLARE_NAPI_GETTER(TASK_TOTAL_TIME, GetTotalDuration),
        DECLARE_NAPI_GETTER(TASK_CPU_TIME, GetCPUDuration),
        DECLARE_NAPI_GETTER(TASK_IO_TIME, GetIODuration)
    };

    // add task name to task
    napi_set_named_property(env, task, NAME, name);
    napi_set_named_property(env, task, FUNCTION_STR, func);
    napi_set_named_property(env, task, TASKID_STR, taskId);
    napi_define_properties(env, task, sizeof(properties) / sizeof(properties[0]), properties);
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

napi_value Task::SendData(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    napi_value args[argc];
    napi_value thisVar;
    napi_get_cb_info(env, cbinfo, &argc, args, &thisVar, nullptr);

    napi_value argsArray;
    napi_create_array_with_length(env, argc, &argsArray);
    for (size_t i = 0; i < argc; i++) {
        napi_set_element(env, argsArray, i, args[i]);
    }

    auto engine = reinterpret_cast<NativeEngine*>(env);
    if (!engine->IsTaskPoolThread()) {
        HILOG_ERROR("taskpool:: SendData is not called in the taskpool thread");
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_NOT_IN_TASKPOOL_THREAD);
        return nullptr;
    }
    TaskInfo* taskInfo = nullptr;
    void* data = engine->GetCurrentTaskInfo();
    if (data == nullptr) {
        HILOG_ERROR("taskpool:: SendData is not called in the concurrent function");
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_NOT_IN_CONCURRENT_FUNCTION);
        return nullptr;
    } else {
        taskInfo = static_cast<TaskInfo*>(data);
    }

    napi_value undefined = NapiHelper::GetUndefinedValue(env);
    napi_value serializationArgs;
    napi_status status = napi_serialize(env, argsArray, undefined, &serializationArgs);
    if (status != napi_ok || serializationArgs == nullptr) {
        std::string errMessage = "taskpool:: failed to serialize function";
        HILOG_ERROR("taskpool:: failed to serialize function in SendData");
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_WORKER_SERIALIZATION, errMessage.c_str());
        return nullptr;
    }
    TaskResultInfo* resultInfo = new TaskResultInfo(taskInfo->env, taskInfo->taskId, serializationArgs);
    return TaskManager::GetInstance().NotifyCallbackExecute(env, resultInfo, taskInfo);
}

napi_value Task::AddDependency(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    if (argc == 0) {
        std::string errMessage = "taskpool:: addDependency has no params";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, errMessage.c_str());
        return nullptr;
    }

    napi_status status = napi_ok;
    HandleScope scope(env, status);
    napi_value args[argc];
    napi_value task;
    napi_get_cb_info(env, cbinfo, &argc, args, &task, nullptr);
    napi_value taskId = NapiHelper::GetNameProperty(env, task, TASKID_STR);
    uint32_t taskIdVal = NapiHelper::GetUint32Value(env, taskId);
    std::set<uint32_t> idSet;
    for (size_t i = 0; i < argc; i++) {
        if (!NapiHelper::HasNameProperty(env, args[i], TASKID_STR)) {
            std::string errMessage = "taskpool:: addDependency param is not task";
            HILOG_ERROR("%{public}s", errMessage.c_str());
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, errMessage.c_str());
            return nullptr;
        } else {
            napi_value id = NapiHelper::GetNameProperty(env, args[i], TASKID_STR);
            uint32_t idVal = NapiHelper::GetUint32Value(env, id);
            if (idVal == taskIdVal) {
                HILOG_ERROR("taskpool:: there is a circular dependency");
                ErrorHelper::ThrowError(env, ErrorHelper::ERR_CIRCULAR_DEPENDENCY);
                return nullptr;
            }
            idSet.emplace(idVal);
        }
    }
    if (!TaskManager::GetInstance().StoreTaskDependency(taskIdVal, idSet)) {
        HILOG_ERROR("taskpool:: there is a circular dependency");
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_CIRCULAR_DEPENDENCY);
    }
    return nullptr;
}

napi_value Task::RemoveDependency(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    if (argc == 0) {
        std::string errMessage = "taskpool:: removeDependency has no params";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, errMessage.c_str());
        return nullptr;
    }

    napi_status status = napi_ok;
    HandleScope scope(env, status);
    napi_value args[argc];
    napi_value task;
    napi_get_cb_info(env, cbinfo, &argc, args, &task, nullptr);
    napi_value taskId = NapiHelper::GetNameProperty(env, task, TASKID_STR);
    uint32_t taskIdVal = NapiHelper::GetUint32Value(env, taskId);
    for (size_t i = 0; i < argc; i++) {
        if (!NapiHelper::HasNameProperty(env, args[i], TASKID_STR)) {
            std::string errMessage = "taskpool:: removeDependency param is not task";
            HILOG_ERROR("%{public}s", errMessage.c_str());
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, errMessage.c_str());
            return nullptr;
        }
        napi_value dependentId = NapiHelper::GetNameProperty(env, args[i], TASKID_STR);
        uint32_t dependentIdVal = NapiHelper::GetUint32Value(env, dependentId);
        if (!TaskManager::GetInstance().RemoveTaskDependency(taskIdVal, dependentIdVal)) {
            HILOG_ERROR("taskpool:: the dependency does not exist");
            ErrorHelper::ThrowError(env, ErrorHelper::ERR_INEXISTENT_DEPENDENCY);
            return nullptr;
        }
    }
    return nullptr;
}

napi_value Task::GetTaskDuration(napi_env env, napi_callback_info& cbinfo, std::string durationType)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, cbinfo, nullptr, nullptr, &thisVar, nullptr);
    napi_value taskId = NapiHelper::GetNameProperty(env, thisVar, TASKID_STR);
    uint32_t taskIdVal = NapiHelper::GetUint32Value(env, taskId);
    uint64_t totalDuration = TaskManager::GetInstance().GetTaskDuration(taskIdVal, durationType);
    napi_value result = nullptr;
    napi_create_bigint_uint64(env, totalDuration, &result);
    return result;
}

napi_value Task::GetTotalDuration(napi_env env, napi_callback_info cbinfo)
{
    return GetTaskDuration(env, cbinfo, TASK_TOTAL_TIME);
}

napi_value Task::GetCPUDuration(napi_env env, napi_callback_info cbinfo)
{
    return GetTaskDuration(env, cbinfo, TASK_CPU_TIME);
}

napi_value Task::GetIODuration(napi_env env, napi_callback_info cbinfo)
{
    return GetTaskDuration(env, cbinfo, TASK_IO_TIME);
}
} // namespace Commonlibrary::Concurrent::TaskPoolModule