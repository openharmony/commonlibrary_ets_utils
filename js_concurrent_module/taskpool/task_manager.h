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

#include <array>
#include <list>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include <unordered_set>

#include "task.h"
#include "task_queue.h"
#include "napi/native_api.h"
#include "worker.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
static constexpr char FUNCTION_STR[] = "function";
static constexpr char ARGUMENTS_STR[] = "arguments";
static constexpr char TASKID_STR[] = "taskId";
static constexpr char TASKINFO_STR[] = "taskInfo";
static constexpr char TRANSFERLIST_STR[] = "transferList";
static constexpr char GROUP_ID_STR[] = "groupId";

class TaskManager {
public:
    TaskManager();
    ~TaskManager();

    static TaskManager& GetInstance();

    static napi_value IsCanceled(napi_env env, napi_callback_info cbinfo);

    uint32_t GenerateTaskId();
    uint32_t GenerateExecuteId();
    TaskInfo* GetTaskInfo(uint32_t executeId);
    TaskInfo* PopTaskInfo(uint32_t executeId);
    void StoreRunningInfo(uint32_t taskId, uint32_t executeId);
    void AddExecuteState(uint32_t executeId);
    bool UpdateExecuteState(uint32_t executeId, ExecuteState state);
    void RemoveExecuteState(uint32_t executeId);
    void PopRunningInfo(uint32_t taskId, uint32_t executeId);
    void PopTaskEnvInfo(napi_env env);
    void EnqueueExecuteId(uint32_t executeId, Priority priority = Priority::DEFAULT);
    std::pair<uint32_t, Priority> DequeueExecuteId();
    const std::list<uint32_t>& QueryRunningTask(napi_env env, uint32_t taskId);
    void CancelExecution(napi_env env, uint32_t executeId);
    TaskInfo* GenerateTaskInfo(napi_env env, napi_value func, napi_value args, uint32_t taskId, uint32_t executeId,
                               napi_value transferList = nullptr);
    TaskInfo* GenerateTaskInfoFromTask(napi_env env, napi_value task, uint32_t executeId);
    void ReleaseTaskContent(TaskInfo* taskInfo);
    void CancelTask(napi_env env, uint32_t taskId);

    // for worker state
    void NotifyWorkerIdle(Worker* worker);
    void NotifyWorkerCreated(Worker* worker);
    void RemoveWorker(Worker* worker);

    // for load balance
    void InitTaskManager(napi_env env);
    void TryTriggerLoadBalance();
    void UpdateExecutedInfo(uint64_t duration);

    // for taskpool state
    uint32_t GetTaskNum();
    uint32_t GetThreadNum();
    uint32_t GetIdleWorkers();
    uint32_t GetRunningWorkers();
    uint32_t GetTimeoutWorkers();

private:
    TaskManager(const TaskManager &) = delete;
    TaskManager& operator=(const TaskManager &) = delete;
    TaskManager(TaskManager &&) = delete;
    TaskManager& operator=(TaskManager &&) = delete;

    ExecuteState QueryExecuteState(uint32_t executeId);
    void CreateWorkers(napi_env env, uint32_t num = 1);
    void NotifyExecuteTask();
    void NotifyWorkerAdded(Worker* worker);
    void StoreTaskInfo(uint32_t executeId, TaskInfo* taskInfo);
    bool MarkCanceledState(uint32_t executeId);
    void CancelWaitingExecution(napi_env env, uint32_t executeId);

    // for load balance
    void RunTaskManager();
    void StoreTaskEnvInfo(napi_env env);
    void CheckForBlockedWorkers();
    void CreateOrDeleteWorkers(uint32_t targetNum);
    bool HasTaskEnvInfo(napi_env env);
    uint32_t ComputeSuitableThreadNum();
    static void RestartTimer(const uv_async_t* req);
    static void TriggerLoadBalance(const uv_timer_t* req = nullptr);

    std::atomic<int32_t> currentExecuteId_ = 1; // 1: executeId begin from 1, 0 for exception
    std::atomic<int32_t> currentTaskId_ = 1; // 1: task will begin from 1, 0 for func
    uint32_t highPrioExecuteCount_ = 0;
    uint32_t mediumPrioExecuteCount_ = 0;

    // <executeId, TaskInfo>
    std::unordered_map<uint32_t, TaskInfo*> taskInfos_ {};
    std::shared_mutex taskInfosMutex_;

    // <executeId, executeState>
    std::unordered_map<uint32_t, ExecuteState> executeStates_ {};
    std::shared_mutex executeStatesMutex_;

    // <taskId, <executeId1, executeId2, ...>>
    std::unordered_map<uint32_t, std::list<uint32_t>> runningInfos_ {};
    std::shared_mutex runningInfosMutex_;

    std::unordered_map<napi_env, uint32_t> taskEnvInfo_ {};
    std::shared_mutex taskEnvInfoMutex_;

    std::unordered_set<Worker*> workers_ {};
    std::unordered_set<Worker*> idleWorkers_ {};
    std::unordered_set<Worker*> timeoutWorkers_ {};
    std::recursive_mutex workersMutex_;

    // for load balance
    napi_env hostEnv_ = nullptr;
    uv_loop_t* loop_ = nullptr;
    uv_timer_t* timer_ = nullptr;
    uv_async_t* notifyRestartTimer_ = nullptr;
    std::atomic<bool> suspend_ = false;
    std::atomic<uint32_t> retryCount_ = 0;
    std::atomic<uint32_t> totalExecCount_ = 0;
    std::atomic<uint64_t> totalExecTime_ = 0;
    std::atomic<uint32_t> expandingCount_ = 0;
    std::atomic<uint64_t> nextCheckTime_ = 0;

    std::atomic<bool> isInitialized_ = false;
    std::array<std::unique_ptr<ExecuteQueue>, Priority::NUMBER> taskQueues_ {};
    std::mutex taskQueuesMutex_;
};

class TaskGroupManager {
public:
    TaskGroupManager() = default;
    ~TaskGroupManager() = default;

    static TaskGroupManager &GetInstance();

    uint32_t GenerateGroupId();
    void AddTask(uint32_t groupId, napi_ref task);
    const std::list<napi_ref>& GetTasksByGroup(uint32_t groupId);
    void ClearTasks(napi_env env, uint32_t groupId);

    GroupInfo* GenerateGroupInfo(napi_env env, uint32_t taskNum, uint32_t groupId);
    void ClearGroupInfo(napi_env env, GroupInfo* info);
    const std::list<GroupInfo*>& GetGroupInfo(uint32_t groupId);
    void StoreRunningGroupInfo(GroupInfo* info);
    bool IsRunning(GroupInfo* info);
    void CancelGroup(napi_env env, std::list<GroupInfo*> groupInfos);

private:
    TaskGroupManager(const TaskGroupManager &) = delete;
    TaskGroupManager& operator=(const TaskGroupManager &) = delete;
    TaskGroupManager(TaskGroupManager &&) = delete;
    TaskGroupManager& operator=(TaskGroupManager &&) = delete;

    void StoreGroupInfo(GroupInfo* info);
    void RemoveGroupInfo(GroupInfo* info);
    void RemoveRunningGroupInfo(GroupInfo* info);

    std::atomic<uint32_t> groupId_ = 0;

    // <groupId, <GroupInfo1, GroupInfo2, ...>>
    std::unordered_map<uint32_t, std::list<GroupInfo*>> groupInfos_ {};

    // <groupId, <task1, task2, ...>>
    std::unordered_map<uint32_t, std::list<napi_ref>> tasks_ {};

    // <groupInfo1, groupInfo2, ...>
    std::unordered_set<GroupInfo*> runningGroupInfos_ {};
    std::shared_mutex RunningGroupInfosMutex_;
};
} // namespace Commonlibrary::Concurrent::TaskPoolModule
#endif // JS_CONCURRENT_MODULE_TASKPOOL_TASK_MANAGER_H_