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
#include "tools/log.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
using namespace Commonlibrary::Concurrent::Common::Helper;

napi_value TaskGroup::TaskGroupConstructor(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = 1;
    napi_value args[1];
    napi_value thisVar;
    napi_get_cb_info(env, cbinfo, &argc, args, &thisVar, nullptr);
    if (argc > 1) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskGroup:: the number of params must be zero or one");
        return nullptr;
    }
    napi_value name;
    if (argc == 1) {
        // check 1st param is taskGroupName
        if (!NapiHelper::IsString(env, args[0])) {
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskGroup:: the first param must be string");
            return nullptr;
        }
        name = args[0];
    } else {
        name = NapiHelper::CreateEmptyString(env);
    }
    TaskGroup* group = new TaskGroup();
    uint64_t groupId = reinterpret_cast<uint64_t>(group);
    group->groupId_ = groupId;
    TaskGroupManager::GetInstance().StoreTaskGroup(groupId, group);
    napi_value napiGroupId = NapiHelper::CreateUint64(env, groupId);
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_PROPERTY(GROUP_ID_STR, napiGroupId),
        DECLARE_NAPI_FUNCTION_WITH_DATA("addTask", AddTask, thisVar),
    };
    napi_set_named_property(env, thisVar, NAME, name);
    napi_define_properties(env, thisVar, sizeof(properties) / sizeof(properties[0]), properties);
    napi_wrap(env, thisVar, group, TaskGroupDestructor, nullptr, nullptr);
    napi_create_reference(env, thisVar, 0, &group->groupRef_);
    return thisVar;
}

void TaskGroup::TaskGroupDestructor(napi_env env, void* data, [[maybe_unused]] void* hint)
{
    TaskGroup* group = static_cast<TaskGroup*>(data);
    TaskGroupManager::GetInstance().ReleaseTaskGroupData(env, group);
    delete group;
}

napi_value TaskGroup::AddTask(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    std::string errMessage = "";
    if (argc < 1) {
        errMessage = "taskGroup:: the number of params must be at least one";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, errMessage.c_str());
        return nullptr;
    }
    napi_value* args = new napi_value[argc];
    ObjectScope<napi_value> scope(args, true);
    napi_value thisVar;
    napi_get_cb_info(env, cbinfo, &argc, args, &thisVar, nullptr);
    napi_value napiGroupId = NapiHelper::GetNameProperty(env, thisVar, GROUP_ID_STR);
    uint64_t groupId = NapiHelper::GetUint64Value(env, napiGroupId);
    TaskGroup* group = TaskGroupManager::GetInstance().GetTaskGroup(groupId);
    if (group->groupState_ != ExecuteState::NOT_FOUND) {
        errMessage = "taskpool:: executed taskGroup cannot addTask";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, errMessage.c_str());
        return nullptr;
    }
    napi_valuetype type = napi_undefined;
    napi_typeof(env, args[0], &type);
    if (type == napi_object) {
        Task* task = nullptr;
        napi_unwrap(env, args[0], reinterpret_cast<void**>(&task));
        if (task == nullptr) {
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskGroup:: the type of the params must be task");
            return nullptr;
        }
        if (!task->CanForTaskGroup(env)) {
            return nullptr;
        }
        task->taskType_ = TaskType::GROUP_COMMON_TASK;
        task->groupId_ = groupId;
        napi_reference_ref(env, task->taskRef_, nullptr);
        TaskGroupManager::GetInstance().AddTask(groupId, task->taskRef_, task->taskId_);
        return nullptr;
    } else if (type == napi_function) {
        napi_value napiTask = NapiHelper::CreateObject(env);
        Task* task = Task::GenerateFunctionTask(env, args[0], args + 1, argc - 1, TaskType::GROUP_FUNCTION_TASK);
        if (task == nullptr) {
            return nullptr;
        }
        task->groupId_ = groupId;
        TaskManager::GetInstance().StoreTask(task->taskId_, task);
        napi_wrap(env, napiTask, task, Task::TaskDestructor, nullptr, nullptr);
        napi_create_reference(env, napiTask, 1, &task->taskRef_);
        TaskGroupManager::GetInstance().AddTask(groupId, task->taskRef_, task->taskId_);
        return nullptr;
    }
    ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskGroup:: first param must be object or function");
    return nullptr;
}

uint32_t TaskGroup::GetTaskIndex(uint32_t taskId)
{
    uint32_t index = 0;
    for (uint32_t id : taskIds_) {
        if (taskId == id) {
            break;
        }
        index++;
    }
    return index;
}

void TaskGroup::NotifyGroupTask(napi_env env)
{
    std::lock_guard<std::recursive_mutex> lock(taskGroupMutex_);
    if (pendingGroupInfos_.empty()) {
        return;
    }
    groupState_ = ExecuteState::WAITING;
    currentGroupInfo_ = pendingGroupInfos_.front();
    pendingGroupInfos_.pop_front();
    for (auto iter = taskRefs_.begin(); iter != taskRefs_.end(); iter++) {
        napi_value napiTask = NapiHelper::GetReferenceValue(env, *iter);
        Task* task = nullptr;
        napi_unwrap(env, napiTask, reinterpret_cast<void**>(&task));
        if (task == nullptr) {
            HILOG_ERROR("taskpool::ExecuteGroup task is nullptr");
            return;
        }
        napi_reference_ref(env, task->taskRef_, nullptr);
        Priority priority = currentGroupInfo_->priority;
        if (task->IsGroupCommonTask()) {
            task->GetTaskInfo(env, napiTask, priority);
        } else {
            reinterpret_cast<NativeEngine*>(env)->IncreaseSubEnvCounter();
        }
        task->IncreaseRefCount();
        TaskManager::GetInstance().IncreaseRefCount(task->taskId_);
        task->taskState_ = ExecuteState::WAITING;
        TaskManager::GetInstance().EnqueueTaskId(task->taskId_, priority);
        TaskManager::GetInstance().TryTriggerExpand();
    }
}

void TaskGroup::CancelPendingGroup(napi_env env)
{
    if (pendingGroupInfos_.empty()) {
        return;
    }
    napi_value error = ErrorHelper::NewError(env, 0, "taskpool:: taskGroup has been canceled");
    auto pendingIter = pendingGroupInfos_.begin();
    auto engine = reinterpret_cast<NativeEngine*>(env);
    for (; pendingIter != pendingGroupInfos_.end(); ++pendingIter) {
        for (size_t i = 0; i < taskIds_.size(); i++) {
            engine->DecreaseSubEnvCounter();
        }
        GroupInfo* info = *pendingIter;
        napi_reject_deferred(env, info->deferred, error);
        napi_reference_unref(env, groupRef_, nullptr);
        delete info;
    }
    pendingIter = pendingGroupInfos_.begin();
    pendingGroupInfos_.erase(pendingIter, pendingGroupInfos_.end());
}
} // namespace Commonlibrary::Concurrent::TaskPoolModule