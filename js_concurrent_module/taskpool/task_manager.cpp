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

#include "task_manager.h"

#include <list>
#include <shared_mutex>
#include <unordered_map>

#include "commonlibrary/ets_utils/js_concurrent_module/common/helper/error_helper.h"
#include "commonlibrary/ets_utils/js_concurrent_module/common/helper/object_helper.h"
#include "utils/log.h"

namespace Commonlibrary::ConcurrentModule {
using namespace Commonlibrary::ConcurrentModule::Helper;
static int32_t g_executeId = 0;
static int32_t g_taskId = 1; // 1: task will begin from 1, 0 for func
std::unordered_map<uint32_t, TaskInfo*> g_taskInfoMap;
std::unordered_map<uint32_t, TaskState> g_stateMap;
std::unordered_map<uint32_t, std::list<uint32_t>> g_runningInfoMap;
static std::mutex g_mutex;
static std::shared_mutex g_taskMutex;
static std::shared_mutex g_stateMutex;
static std::shared_mutex g_runningMutex;

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
    task->taskId_ = TaskManager::GenerateTaskId();
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

uint32_t TaskManager::GenerateTaskId()
{
    std::unique_lock<std::mutex> lock(g_mutex);
    return g_taskId++;
}

uint32_t TaskManager::GenerateExecuteId()
{
    std::unique_lock<std::mutex> lock(g_mutex);
    return g_executeId++;
}

void TaskManager::ClearTaskInfo()
{
    std::unique_lock<std::shared_mutex> lock(g_taskMutex);
    for (auto iter = g_taskInfoMap.begin(); iter != g_taskInfoMap.end(); iter++) {
        delete iter->second;
        iter->second = nullptr;
    }
    g_taskInfoMap.clear();
}

void TaskManager::StoreTaskInfo(uint32_t executeId, TaskInfo* taskInfo)
{
    std::unique_lock<std::shared_mutex> lock(g_taskMutex);
    g_taskInfoMap.emplace(executeId, taskInfo);
}

void TaskManager::StoreStateInfo(uint32_t executeId, TaskState state)
{
    std::unique_lock<std::shared_mutex> lock(g_stateMutex);
    g_stateMap.emplace(executeId, state);
}

void TaskManager::StoreRunningInfo(uint32_t taskId, uint32_t executeId)
{
    std::unique_lock<std::shared_mutex> lock(g_runningMutex);
    auto iter = g_runningInfoMap.find(taskId);
    if (iter == g_runningInfoMap.end()) {
        std::list<uint32_t> list {executeId};
        g_runningInfoMap.emplace(taskId, list);
    } else {
        iter->second.push_front(executeId);
    }
}

TaskInfo* TaskManager::PopTaskInfo(uint32_t executeId)
{
    std::unique_lock<std::shared_mutex> lock(g_taskMutex);
    auto iter = g_taskInfoMap.find(executeId);
    if (iter == g_taskInfoMap.end() || iter->second == nullptr) {
        return nullptr;
    }

    TaskInfo* taskInfo = iter->second;
    // remove the the taskInfo when executed
    g_taskInfoMap.erase(iter);
    return taskInfo;
}

void TaskManager::PopRunningInfo(uint32_t taskId, uint32_t executeId)
{
    std::unique_lock<std::shared_mutex> lock(g_runningMutex);
    auto iter = g_runningInfoMap.find(taskId);
    if (iter == g_runningInfoMap.end()) {
        return;
    }
    iter->second.remove(executeId);
}

TaskState TaskManager::QueryState(uint32_t executeId)
{
    std::shared_lock<std::shared_mutex> lock(g_stateMutex);
    auto iter = g_stateMap.find(executeId);
    if (iter == g_stateMap.end()) {
        HILOG_ERROR("taskpool:: failed to find the target task");
        return TaskState::NOT_FOUND;
    }
    return iter->second;
}

bool TaskManager::UpdateState(uint32_t executeId, TaskState state)
{
    std::unique_lock<std::shared_mutex> lock(g_stateMutex);
    auto iter = g_stateMap.find(executeId);
    if (iter == g_stateMap.end()) {
        return false;
    }
    if (state == TaskState::RUNNING) {
        iter->second = state;
    } else {
        g_stateMap.erase(iter);
    }
    return true;
}

void TaskManager::CancelTask(napi_env env, uint32_t taskId)
{
    std::unique_lock<std::shared_mutex> lock(g_runningMutex);
    auto iter = g_runningInfoMap.find(taskId);
    if (iter == g_runningInfoMap.end() || iter->second.empty()) {
        ErrorHelper::ThrowError(env, ErrorHelper::NOTEXIST_ERROR, "taskpool:: can not find the task");
        return;
    }
    int32_t result;
    for (auto item : iter->second) {
        TaskState state = QueryState(item);
        if (state == TaskState::NOT_FOUND) {
            result = ErrorHelper::NOTEXIST_ERROR;
            break;
        }
        UpdateState(item, TaskState::CANCELED);
        if (state == TaskState::WAITING) {
            TaskInfo* taskInfo = PopTaskInfo(item);
            ReleaseTaskContent(taskInfo);
        } else {
            result = ErrorHelper::RUNNING_ERROR;
        }
    }

    if (result == ErrorHelper::NOTEXIST_ERROR) {
        ErrorHelper::ThrowError(env, ErrorHelper::NOTEXIST_ERROR, "taskpool:: can not find the task");
    } else if (result == ErrorHelper::RUNNING_ERROR) {
        ErrorHelper::ThrowError(env, ErrorHelper::RUNNING_ERROR, "taskpool:: can not cancel the running task");
    } else {
        g_runningInfoMap.erase(iter);
    }
}

void TaskManager::ReleaseTaskContent(TaskInfo* taskInfo)
{
    if (taskInfo != nullptr && taskInfo->taskSignal != nullptr &&
        !uv_is_closing(reinterpret_cast<uv_handle_t*>(taskInfo->taskSignal))) {
        uv_close(reinterpret_cast<uv_handle_t*>(taskInfo->taskSignal), [](uv_handle_t* handle) {
            if (handle != nullptr) {
                delete reinterpret_cast<uv_async_t*>(handle);
                handle = nullptr;
            }
        });
    }
    delete taskInfo;
    taskInfo = nullptr;
}
} // namespace Commonlibrary::ConcurrentModule