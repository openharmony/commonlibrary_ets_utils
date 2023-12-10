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

#ifndef JS_CONCURRENT_MODULE_TASKPOOL_TASK_MANAGER_H
#define JS_CONCURRENT_MODULE_TASKPOOL_TASK_MANAGER_H

#include <array>
#include <list>
#include <memory>
#include <mutex>
#include <set>
#include <shared_mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "task.h"
#include "task_queue.h"
#include "napi/native_api.h"
#include "worker.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
using namespace Commonlibrary::Concurrent::Common;

static constexpr char ARGUMENTS_STR[] = "arguments";
static constexpr char FUNCTION_NAME[] = "name";
static constexpr char FUNCTION_STR[] = "function";
static constexpr char GROUP_ID_STR[] = "groupId";
static constexpr char TASKID_STR[] = "taskId";
static constexpr char TASKINFO_STR[] = "taskInfo";
static constexpr char TRANSFERLIST_STR[] = "transferList";
static constexpr char ADD_DEPENDENCY_STR[] = "addDependency";
static constexpr char REMOVE_DEPENDENCY_STR[] = "removeDependency";

class TaskManager {
public:
    TaskManager();
    ~TaskManager();

    static TaskManager& GetInstance();

    uint32_t GenerateTaskId();
    uint32_t GenerateExecuteId();
    TaskInfo* GetTaskInfo(uint32_t executeId);
    TaskInfo* PopTaskInfo(uint32_t executeId);
    void StoreRunningInfo(uint32_t taskId, uint32_t executeId);
    void AddExecuteState(uint32_t executeId);
    bool UpdateExecuteState(uint32_t executeId, ExecuteState state);
    void RemoveExecuteState(uint32_t executeId);
    void PopRunningInfo(uint32_t taskId, uint32_t executeId);
    void EnqueueExecuteId(uint32_t executeId, Priority priority = Priority::DEFAULT);
    std::pair<uint32_t, Priority> DequeueExecuteId();
    void CancelTask(napi_env env, uint32_t taskId);
    TaskInfo* GenerateTaskInfo(napi_env env, napi_value func, napi_value args, uint32_t taskId, uint32_t executeId,
                               napi_value transferList = nullptr);
    TaskInfo* GenerateTaskInfoFromTask(napi_env env, napi_value task, uint32_t executeId);
    void ReleaseTaskContent(TaskInfo* taskInfo);

    // for worker state
    void NotifyWorkerIdle(Worker* worker);
    void NotifyWorkerCreated(Worker* worker);
    void NotifyWorkerRunning(Worker* worker);
    void RemoveWorker(Worker* worker);
    void RestoreWorker(Worker* worker);

    // for load balance
    void InitTaskManager(napi_env env);
    void UpdateExecutedInfo(uint64_t duration);
    void TryTriggerExpand();

    // for taskpool state
    uint32_t GetTaskNum();
    uint32_t GetIdleWorkers();
    uint32_t GetThreadNum();
    uint32_t GetRunningWorkers();
    uint32_t GetTimeoutWorkers();
    void GetIdleWorkersList(uint32_t step);
    bool ReadThreadInfo(Worker* worker, char* buf, uint32_t size);

    // for get thread info
    napi_value GetThreadInfos(napi_env env);

    // for get task info
    napi_value GetTaskInfos(napi_env env);

    // for countTrace for worker
    void CountTraceForWorker();

    std::shared_ptr<CallbackInfo> GetCallbackInfo(uint32_t taskId);
    void RegisterCallback(napi_env env, uint32_t taskId, std::shared_ptr<CallbackInfo> callbackInfo);
    void IncreaseRefCount(uint32_t taskId);
    void DecreaseRefCount(napi_env env, uint32_t taskId);
    napi_value NotifyCallbackExecute(napi_env env, TaskResultInfo* resultInfo, TaskInfo* taskInfo);

    // for task dependency
    bool CheckDependencyByExecuteId(uint32_t executeId);
    void NotifyPendingExecuteInfo(uint32_t taskId);
    bool StoreTaskDependency(uint32_t taskId, std::set<uint32_t> taskIdSet);
    bool RemoveTaskDependency(uint32_t taskId, uint32_t dependentId);
    bool CheckCircularDependency(std::set<uint32_t> dependentIdSet, std::set<uint32_t> idSet, uint32_t taskId);
    void EnqueuePendingExecuteInfo(uint32_t executeId, Priority priority);
    std::pair<uint32_t, Priority> DequeuePendingExecuteInfo(uint32_t executeId);
    void StoreDependentTaskInfo(std::set<uint32_t> dependTaskIdSet, uint32_t taskId);
    void RemoveDependentTaskInfo(uint32_t dependentTaskId, uint32_t taskId);

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
    void CancelExecution(napi_env env, uint32_t executeId);

    // for load balance
    void RunTaskManager();
    void CheckForBlockedWorkers();
    void TryExpand();
    void NotifyShrink(uint32_t targetNum);
    void TriggerShrink(uint32_t step);
    uint32_t ComputeSuitableThreadNum();
    static void NotifyExpand(const uv_async_t* req);
    static void TriggerLoadBalance(const uv_timer_t* req = nullptr);

    std::atomic<int32_t> currentExecuteId_ = 1; // 1: executeId begin from 1, 0 for exception
    std::atomic<int32_t> currentTaskId_ = 1; // 1: task will begin from 1, 0 for func

    // <executeId, TaskInfo>
    std::unordered_map<uint32_t, TaskInfo*> taskInfos_ {};
    std::shared_mutex taskInfosMutex_;

    // <executeId, executeState>
    std::unordered_map<uint32_t, ExecuteState> executeStates_ {};
    std::shared_mutex executeStatesMutex_;

    // <taskId, <executeId1, executeId2, ...>>
    std::unordered_map<uint32_t, std::list<uint32_t>> runningInfos_ {};
    std::shared_mutex runningInfosMutex_;

    // <taskId, <dependent taskId1, dependent taskId2, ...>>, update when removeDependency or executeTask
    std::unordered_map<uint32_t, std::set<uint32_t>> dependTaskInfos_ {};
    std::shared_mutex dependTaskInfosMutex_;

    // <<pendingExecuteId1, priority>, <pendingExecuteId2, priority>, ...>
    std::unordered_map<uint32_t, Priority> pendingExecuteInfos_ {};

    // <dependent taskId, <taskId1, taskId2, ...>>, update when removeDependency or executeTask
    std::unordered_map<uint32_t, std::set<uint32_t>> dependentTaskInfos_ {};

    std::unordered_set<Worker*> workers_ {};
    std::unordered_set<Worker*> idleWorkers_ {};
    std::unordered_set<Worker*> timeoutWorkers_ {};
    std::recursive_mutex workersMutex_;

    // for load balance
    napi_env hostEnv_ = nullptr;
    uv_loop_t* loop_ = nullptr;
    uv_timer_t* timer_ = nullptr;
    uv_async_t* expandHandle_ = nullptr;
    std::atomic<bool> suspend_ = false;
    std::atomic<uint32_t> retryCount_ = 0;
    std::atomic<uint32_t> expandingCount_ = 0;
    std::atomic<uint32_t> totalExecCount_ = 0;
    std::atomic<uint64_t> totalExecTime_ = 0;
    std::atomic<bool> needChecking_ = false;

    // for task priority
    uint32_t highPrioExecuteCount_ = 0;
    uint32_t mediumPrioExecuteCount_ = 0;
    std::array<std::unique_ptr<ExecuteQueue>, Priority::NUMBER> taskQueues_ {};
    std::mutex taskQueuesMutex_;

    std::atomic<bool> isInitialized_ = false;

    std::mutex callbackMutex_;
    std::map<uint32_t, std::shared_ptr<CallbackInfo>> callbackTable_ {};
    std::vector<Worker*> freeList_ {};
    friend class TaskGroupManager;
};

class TaskGroupManager {
public:
    TaskGroupManager() = default;
    ~TaskGroupManager() = default;

    static TaskGroupManager &GetInstance();

    uint32_t GenerateGroupId();
    uint32_t GenerateGroupExecuteId();
    void AddTask(uint32_t groupId, napi_ref task);
    const std::list<napi_ref>& GetTasksByGroup(uint32_t groupId);
    void ClearTasks(napi_env env, uint32_t groupId);

    GroupInfo* GenerateGroupInfo(napi_env env, uint32_t taskNum, uint32_t groupId, uint32_t groupExecuteId);
    void ClearGroupInfo(napi_env env, uint32_t groupExecuteId, GroupInfo* groupInfo);
    void CancelGroup(napi_env env, uint32_t groupId);
    void RemoveExecuteId(uint32_t groupId, uint32_t groupExecuteId);
    void ClearExecuteId(uint32_t groupId);
    bool IsRunning(uint32_t groupExecuteId);
    GroupInfo* GetGroupInfoByExecutionId(uint32_t groupExecuteId);

private:
    TaskGroupManager(const TaskGroupManager &) = delete;
    TaskGroupManager& operator=(const TaskGroupManager &) = delete;
    TaskGroupManager(TaskGroupManager &&) = delete;
    TaskGroupManager& operator=(TaskGroupManager &&) = delete;

    void StoreExecuteId(uint32_t groupId, uint32_t groupExecuteId);

    void StoreRunningExecuteId(uint32_t groupExecuteId);
    void RemoveRunningExecuteId(uint32_t groupExecuteId);

    void AddGroupInfoById(uint32_t groupExecuteId, GroupInfo* info);
    void RemoveGroupInfoById(uint32_t groupExecuteId);

    void CancelGroupExecution(uint32_t executeId);

    std::atomic<uint32_t> groupId_ = 0;
    std::atomic<uint32_t> groupExecuteId_ = 1; // 1: 0 reserved for those tasks not in any group

    // <groupId, <groupExecuteId1, groupExecuteId2, ...>>
    std::unordered_map<uint32_t, std::list<uint32_t>> groupExecuteIds_ {};
    std::mutex groupExecuteIdsMutex_;

    // <groupId, <task1, task2, ...>>
    std::unordered_map<uint32_t, std::list<napi_ref>> tasks_ {};
    std::shared_mutex tasksMutex_;

    // <groupExecuteId1, groupExecuteId2, ...>
    std::unordered_set<uint32_t> runningGroupExecutions_ {};
    std::shared_mutex groupExecutionsMutex_;

    // <<groupExecuteId1, GroupInfo1>, <groupExecuteId2, GroupInfo2>, ...>
    std::unordered_map<uint32_t, GroupInfo*> groupInfoMap_ {};
    std::shared_mutex groupInfoMapMutex_;
};
} // namespace Commonlibrary::Concurrent::TaskPoolModule
#endif // JS_CONCURRENT_MODULE_TASKPOOL_TASK_MANAGER_H