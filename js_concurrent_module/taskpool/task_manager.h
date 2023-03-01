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

#ifndef JS_CONCURRENT_MODULE_TASKPOOL_TASK_MANAGER_H_
#define JS_CONCURRENT_MODULE_TASKPOOL_TASK_MANAGER_H_

#include <list>
#include <memory>
#include <set>
#include <shared_mutex>
#include <unordered_map>

#include "task_queue.h"
#include "napi/native_api.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
class Worker;

class TaskManager {
public:
    TaskManager() = default;
    ~TaskManager();

    static TaskManager &GetInstance();

    uint32_t GenerateTaskId();
    uint32_t GenerateExecuteId();
    TaskInfo* PopTaskInfo(uint32_t executeId);
    void StoreTaskInfo(uint32_t executeId, TaskInfo* taskInfo);
    void StoreStateInfo(uint32_t executeId, TaskState state);
    void StoreRunningInfo(uint32_t taskId, uint32_t executeId);
    bool UpdateState(uint32_t executeId, TaskState state);
    void PopRunningInfo(uint32_t taskId, uint32_t executeId);
    void EnqueueTask(std::unique_ptr<Task> task);
    std::unique_ptr<Task> DequeueTask();
    void CancelTask(napi_env env, uint32_t taskId);
    void NotifyWorkerIdle(Worker *worker);
    void InitTaskRunner(napi_env env);
    TaskInfo* GenerateTaskInfo(napi_env env, napi_value object, uint32_t taskId, uint32_t executeId);
    void ReleaseTaskContent(TaskInfo* taskInfo);

private:
    TaskManager(const TaskManager &) = delete;
    TaskManager& operator=(const TaskManager &) = delete;
    TaskManager(TaskManager &&) = delete;
    TaskManager& operator=(TaskManager &&) = delete;

    TaskState QueryState(uint32_t executeId);

    bool NeedExpandWorker();

    void NotifyWorkerAdded(Worker *worker);
    void NotifyExecuteTask();

    int32_t currentExecuteId_ = 0;
    int32_t currentTaskId_ = 1; // 1: task will begin from 1, 0 for func
    std::mutex idMutex_;

    std::unordered_map<uint32_t, TaskInfo*> taskInfos_;
    std::shared_mutex taskInfosMutex_;

    std::unordered_map<uint32_t, TaskState> taskStates_;
    std::shared_mutex taskStatesMutex_;

    std::unordered_map<uint32_t, std::list<uint32_t>> runningInfos_;
    std::shared_mutex runningInfosMutex_;

    std::set<Worker*> workers_;
    std::set<Worker*> idleWorkers_;
    std::mutex workersMutex_;

    TaskQueue taskQueue_;
};
} // namespace Commonlibrary::Concurrent::TaskPoolModule
#endif // JS_CONCURRENT_MODULE_TASKPOOL_TASK_MANAGER_H_