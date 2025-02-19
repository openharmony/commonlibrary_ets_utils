/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "task_group_manager.h"

#include "helper/hitrace_helper.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
TaskGroupManager& TaskGroupManager::GetInstance()
{
    static TaskGroupManager groupManager;
    return groupManager;
}

void TaskGroupManager::AddTask(uint64_t groupId, napi_ref taskRef, uint32_t taskId)
{
    std::lock_guard<std::mutex> lock(taskGroupsMutex_);
    auto groupIter = taskGroups_.find(groupId);
    if (groupIter == taskGroups_.end()) {
        HILOG_DEBUG("taskpool:: taskGroup has been released");
        return;
    }
    auto taskGroup = reinterpret_cast<TaskGroup*>(groupIter->second);
    if (taskGroup == nullptr) {
        HILOG_ERROR("taskpool:: taskGroup is null");
        return;
    }
    taskGroup->taskRefs_.push_back(taskRef);
    taskGroup->taskNum_++;
    taskGroup->taskIds_.push_back(taskId);
}

void TaskGroupManager::ReleaseTaskGroupData(napi_env env, TaskGroup* group)
{
    HILOG_DEBUG("taskpool:: ReleaseTaskGroupData group");
    TaskGroupManager::GetInstance().RemoveTaskGroup(group->groupId_);
    {
        std::lock_guard<std::recursive_mutex> lock(group->taskGroupMutex_);
        if (group->onRejectResultSignal_ != nullptr) {
            if (!ConcurrentHelper::IsUvClosing(group->onRejectResultSignal_)) {
                ConcurrentHelper::UvHandleClose(group->onRejectResultSignal_);
            } else {
                delete group->onRejectResultSignal_;
                group->onRejectResultSignal_ = nullptr;
            }
        }
        if (group->isValid_) {
            for (uint32_t taskId : group->taskIds_) {
                Task* task = TaskManager::GetInstance().GetTask(taskId);
                if (task == nullptr || !task->IsValid()) {
                    continue;
                }
                napi_reference_unref(task->env_, task->taskRef_, nullptr);
            }
        }
        if (group->currentGroupInfo_ != nullptr) {
            delete group->currentGroupInfo_;
            group->currentGroupInfo_ = nullptr;
        }
    }
    group->CancelPendingGroup(env);
}

void TaskGroupManager::CancelGroup(napi_env env, uint64_t groupId)
{
    std::string strTrace = "CancelGroup: groupId: " + std::to_string(groupId);
    HITRACE_HELPER_METER_NAME(strTrace);
    HILOG_INFO("taskpool:: %{public}s", strTrace.c_str());
    TaskGroup* taskGroup = GetTaskGroup(groupId);
    if (taskGroup == nullptr) {
        HILOG_ERROR("taskpool:: CancelGroup group is nullptr");
        return;
    }
    if (taskGroup->groupState_ == ExecuteState::CANCELED) {
        return;
    }
    {
        std::lock_guard<std::recursive_mutex> lock(taskGroup->taskGroupMutex_);
        if (taskGroup->currentGroupInfo_ == nullptr || taskGroup->groupState_ == ExecuteState::NOT_FOUND ||
            taskGroup->groupState_ == ExecuteState::FINISHED) {
            std::string errMsg = "taskpool:: taskGroup is not executed or has been executed";
            HILOG_ERROR("%{public}s", errMsg.c_str());
            ErrorHelper::ThrowError(env, ErrorHelper::ERR_CANCEL_NONEXIST_TASK_GROUP, errMsg.c_str());
            return;
        }
    }
    ExecuteState groupState = taskGroup->groupState_;
    taskGroup->groupState_ = ExecuteState::CANCELED;
    taskGroup->CancelPendingGroup(env);
    std::lock_guard<std::recursive_mutex> lock(taskGroup->taskGroupMutex_);
    if (taskGroup->currentGroupInfo_->finishedTaskNum != taskGroup->taskNum_) {
        for (uint32_t taskId : taskGroup->taskIds_) {
            CancelGroupTask(env, taskId, taskGroup);
        }
        if (taskGroup->currentGroupInfo_->finishedTaskNum == taskGroup->taskNum_) {
            napi_value error = ErrorHelper::NewError(env, 0, "taskpool:: taskGroup has been canceled");
            taskGroup->RejectResult(env, error);
            return;
        }
    }
    if (groupState == ExecuteState::WAITING && taskGroup->currentGroupInfo_ != nullptr) {
        auto engine = reinterpret_cast<NativeEngine*>(env);
        for (size_t i = 0; i < taskGroup->taskIds_.size(); i++) {
            engine->DecreaseSubEnvCounter();
        }
        napi_value error = ErrorHelper::NewError(env, 0, "taskpool:: taskGroup has been canceled");
        taskGroup->RejectResult(env, error);
    }
}

void TaskGroupManager::CancelGroupTask(napi_env env, uint32_t taskId, TaskGroup* group)
{
    HILOG_DEBUG("taskpool:: CancelGroupTask task:%{public}s", std::to_string(taskId).c_str());
    auto task = TaskManager::GetInstance().GetTask(taskId);
    if (task == nullptr) {
        HILOG_INFO("taskpool:: CancelGroupTask task is nullptr");
        return;
    }
    std::lock_guard<std::recursive_mutex> lock(task->taskMutex_);
    if (task->taskState_ == ExecuteState::WAITING && task->currentTaskInfo_ != nullptr &&
        TaskManager::GetInstance().EraseWaitingTaskId(task->taskId_, task->currentTaskInfo_->priority)) {
        reinterpret_cast<NativeEngine*>(env)->DecreaseSubEnvCounter();
        task->DecreaseTaskRefCount();
        TaskManager::GetInstance().DecreaseRefCount(env, taskId);
        delete task->currentTaskInfo_;
        task->currentTaskInfo_ = nullptr;
        if (group->currentGroupInfo_ != nullptr) {
            group->currentGroupInfo_->finishedTaskNum++;
        }
    }
    task->taskState_ = ExecuteState::CANCELED;
}

void TaskGroupManager::StoreTaskGroup(uint64_t groupId, TaskGroup* taskGroup)
{
    std::lock_guard<std::mutex> lock(taskGroupsMutex_);
    taskGroups_.emplace(groupId, taskGroup);
}

void TaskGroupManager::RemoveTaskGroup(uint64_t groupId)
{
    std::lock_guard<std::mutex> lock(taskGroupsMutex_);
    taskGroups_.erase(groupId);
}

TaskGroup* TaskGroupManager::GetTaskGroup(uint64_t groupId)
{
    std::lock_guard<std::mutex> lock(taskGroupsMutex_);
    auto groupIter = taskGroups_.find(groupId);
    if (groupIter == taskGroups_.end()) {
        return nullptr;
    }
    return reinterpret_cast<TaskGroup*>(groupIter->second);
}

bool TaskGroupManager::UpdateGroupState(uint64_t groupId)
{
    HILOG_DEBUG("taskpool:: UpdateGroupState groupId:%{public}s", std::to_string(groupId).c_str());
    // During the modification process of the group, prevent other sub threads from performing other
    // operations on the group pointer, which may cause the modification to fail.
    std::lock_guard<std::mutex> lock(taskGroupsMutex_);
    auto groupIter = taskGroups_.find(groupId);
    if (groupIter == taskGroups_.end()) {
        return false;
    }
    TaskGroup* group = reinterpret_cast<TaskGroup*>(groupIter->second);
    if (group == nullptr || group->groupState_ == ExecuteState::CANCELED) {
        HILOG_DEBUG("taskpool:: UpdateGroupState taskGroup has been released or canceled");
        return false;
    }
    group->groupState_ = ExecuteState::RUNNING;
    return true;
}
} // namespace Commonlibrary::Concurrent::TaskPoolModule