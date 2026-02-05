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

#ifndef JS_CONCURRENT_MODULE_TASKPOOL_TASK_GROUP_MANAGER_H
#define JS_CONCURRENT_MODULE_TASKPOOL_TASK_GROUP_MANAGER_H

#include "task_group.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
class TaskGroupManager {
public:
    static TaskGroupManager& GetInstance();

    void AddTask(uint64_t groupId, napi_ref taskRef, uint32_t taskId);
    void StoreTaskGroup(uint64_t groupId, TaskGroup* taskGroup);
    void RemoveTaskGroup(uint64_t groupId);
    TaskGroup* GetTaskGroup(uint64_t groupId);
    void CancelGroup(napi_env env, uint64_t groupId);
    void CancelGroupTask(napi_env env, uint32_t taskId, TaskGroup* group);
    void ReleaseTaskGroupData(napi_env env, TaskGroup* group);
    bool UpdateGroupState(uint64_t groupId);
    void TimeoutGroup(napi_env env, uint64_t groupId);

private:
    TaskGroupManager() = default;
    ~TaskGroupManager() = default;
    TaskGroupManager(const TaskGroupManager &) = delete;
    TaskGroupManager& operator=(const TaskGroupManager &) = delete;
    TaskGroupManager(TaskGroupManager &&) = delete;
    TaskGroupManager& operator=(TaskGroupManager &&) = delete;

    // <groupId, TaskGroup>
    std::unordered_map<uint64_t, TaskGroup*> taskGroups_ {};
    std::mutex taskGroupsMutex_;
    friend class NativeEngineTest;
};
} // namespace Commonlibrary::Concurrent::TaskPoolModule
#endif // JS_CONCURRENT_MODULE_TASKPOOL_TASK_GROUP_MANAGER_H