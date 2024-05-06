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

#include "napi/native_api.h"
#include "sequence_runner.h"
#include "task.h"
#include "task_queue.h"
#include "task_group.h"
#include "worker.h"


namespace Commonlibrary::Concurrent::TaskPoolModule {
using namespace Commonlibrary::Concurrent::Common;

static constexpr char ARGUMENTS_STR[] = "arguments";
static constexpr char NAME[] = "name";
static constexpr char FUNCTION_STR[] = "function";
static constexpr char GROUP_ID_STR[] = "groupId";
static constexpr char TASKID_STR[] = "taskId";
static constexpr char TASKINFO_STR[] = "taskInfo";
static constexpr char TRANSFERLIST_STR[] = "transferList";
static constexpr char CLONE_LIST_STR[] = "cloneList";
static constexpr char ADD_DEPENDENCY_STR[] = "addDependency";
static constexpr char REMOVE_DEPENDENCY_STR[] = "removeDependency";
static constexpr char TASK_CPU_TIME[] = "cpuDuration";
static constexpr char TASK_IO_TIME[] = "ioDuration";
static constexpr char TASK_TOTAL_TIME[] = "totalDuration";
static constexpr char DEFAULT_TRANSFER_STR[] = "defaultTransfer";
static constexpr char DEFAULT_CLONE_SENDABLE_STR[] = "defaultCloneSendable";

class TaskGroup;

class TaskManager {
public:
    static TaskManager& GetInstance();

    void StoreTask(uint64_t taskId, Task* task);
    void RemoveTask(uint64_t taskId);
    Task* GetTask(uint64_t taskId);
    void EnqueueTaskId(uint64_t taskId, Priority priority = Priority::DEFAULT);
    std::pair<uint64_t, Priority> DequeueTaskId();
    void CancelTask(napi_env env, uint64_t taskId);
    void ReleaseTaskData(napi_env env, Task* task);

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

    // for get task name
    std::string GetTaskName(uint64_t taskId);

    // for countTrace for worker
    void CountTraceForWorker();

    std::shared_ptr<CallbackInfo> GetCallbackInfo(uint64_t taskId);
    void RegisterCallback(napi_env env, uint64_t taskId, std::shared_ptr<CallbackInfo> callbackInfo);
    void IncreaseRefCount(uint64_t taskId);
    void DecreaseRefCount(napi_env env, uint64_t taskId);
    napi_value NotifyCallbackExecute(napi_env env, TaskResultInfo* resultInfo, Task* task);
    MsgQueue* GetMessageQueue(const uv_async_t* req);

    // for task dependency
    bool IsDependendByTaskId(uint64_t taskId);
    bool IsDependentByTaskId(uint64_t dependentTaskId);
    void NotifyDependencyTaskInfo(uint64_t taskId);
    void RemoveDependencyById(uint64_t dependentTaskId, uint64_t taskId);
    bool StoreTaskDependency(uint64_t taskId, std::set<uint64_t> taskIdSet);
    bool RemoveTaskDependency(uint64_t taskId, uint64_t dependentId);
    bool CheckCircularDependency(std::set<uint64_t> dependentIdSet, std::set<uint64_t> idSet, uint64_t taskId);
    void EnqueuePendingTaskInfo(uint64_t taskId, Priority priority);
    std::pair<uint64_t, Priority> DequeuePendingTaskInfo(uint64_t taskId);
    void RemovePendingTaskInfo(uint64_t taskId);
    void StoreDependentTaskInfo(std::set<uint64_t> dependTaskIdSet, uint64_t taskId);
    void RemoveDependentTaskInfo(uint64_t dependentTaskId, uint64_t taskId);
    std::string GetTaskDependInfoToString(uint64_t taskId);

    // for duration
    void StoreTaskDuration(uint64_t taskId, uint64_t totalDuration, uint64_t cpuDuration);
    uint64_t GetTaskDuration(uint64_t taskId, std::string durationType);
    void RemoveTaskDuration(uint64_t taskId);
    void StoreLongTaskInfo(uint64_t taskId, Worker* worker);
    void RemoveLongTaskInfo(uint64_t taskId);
    void TerminateTask(uint64_t taskId);
    Worker* GetLongTaskInfo(uint64_t taskId);

    // for callback
    void ReleaseCallBackInfo(Task* task);

    bool CheckSystemApp();
    bool IsSystemApp() const
    {
        return isSystemApp_;
    }

private:
    TaskManager();
    ~TaskManager();
    TaskManager(const TaskManager &) = delete;
    TaskManager& operator=(const TaskManager &) = delete;
    TaskManager(TaskManager &&) = delete;
    TaskManager& operator=(TaskManager &&) = delete;

    void CreateWorkers(napi_env env, uint32_t num = 1);
    void NotifyExecuteTask();
    void NotifyWorkerAdded(Worker* worker);

    // for load balance
    void RunTaskManager();
    void CheckForBlockedWorkers();
    void TryExpand();
    void NotifyShrink(uint32_t targetNum);
    void TriggerShrink(uint32_t step);
    uint32_t ComputeSuitableThreadNum();
    static void NotifyExpand(const uv_async_t* req);
    static void TriggerLoadBalance(const uv_timer_t* req = nullptr);

    // <taskId, Task>
    std::unordered_map<uint64_t, Task*> tasks_ {};
    std::recursive_mutex tasksMutex_;

    // <taskId, <dependent taskId1, dependent taskId2, ...>>, update when removeDependency or executeTask
    std::unordered_map<uint64_t, std::set<uint64_t>> dependTaskInfos_ {};
    std::shared_mutex dependTaskInfosMutex_;

    // <dependent taskId, <taskId1, taskId2, ...>>, update when removeDependency or executeTask
    std::unordered_map<uint64_t, std::set<uint64_t>> dependentTaskInfos_ {};
    std::shared_mutex dependentTaskInfosMutex_;

    // <<pendingTaskId1, priority>, <pendingTaskId2, priority>, ...>
    std::unordered_map<uint64_t, Priority> pendingTaskInfos_ {};
    std::shared_mutex pendingTaskInfosMutex_;

    // <<taskId1, <totalDuration1, cpuDuration1>>, <taskId2, <totalDuration2, cpuDuration2>>, ...>
    std::unordered_map<uint64_t, std::pair<uint64_t, uint64_t>> taskDurationInfos_ {};
    std::shared_mutex taskDurationInfosMutex_;

    // record the longTasks and workers for efficiency
    std::unordered_map<uint64_t, Worker*> longTasksMap_ {};
    std::shared_mutex longTasksMutex_{};

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
    std::atomic<uint32_t> totalExecCount_ = 0;
    std::atomic<uint64_t> totalExecTime_ = 0;
    std::atomic<bool> needChecking_ = false;
    std::atomic<bool> isHandleInited_ = false;

    // for task priority
    uint32_t highPrioExecuteCount_ = 0;
    uint32_t mediumPrioExecuteCount_ = 0;
    std::array<std::unique_ptr<ExecuteQueue>, Priority::NUMBER> taskQueues_ {};
    std::mutex taskQueuesMutex_;

    std::atomic<bool> isInitialized_ = false;

    std::atomic<bool> isSystemApp_ = false;

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

    void AddTask(uint64_t groupId, napi_ref taskRef, uint64_t taskId);
    void StoreTaskGroup(uint64_t groupId, TaskGroup* taskGroup);
    void RemoveTaskGroup(uint64_t groupId);
    TaskGroup* GetTaskGroup(uint64_t groupId);
    void CancelGroup(napi_env env, uint64_t groupId);
    void CancelGroupTask(napi_env env, uint64_t taskId, TaskGroup* group);
    void ReleaseTaskGroupData(napi_env env, TaskGroup* group);
    bool UpdateGroupState(uint64_t groupId);

    void AddTaskToSeqRunner(uint64_t seqRunnerId, Task* task);
    bool TriggerSeqRunner(napi_env env, Task* lastTask);
    void StoreSequenceRunner(uint64_t seqRunnerId, SequenceRunner* seqRunner);
    void RemoveSequenceRunner(uint64_t seqRunnerId);
    SequenceRunner* GetSeqRunner(uint64_t seqRunnerId);

private:
    TaskGroupManager(const TaskGroupManager &) = delete;
    TaskGroupManager& operator=(const TaskGroupManager &) = delete;
    TaskGroupManager(TaskGroupManager &&) = delete;
    TaskGroupManager& operator=(TaskGroupManager &&) = delete;

    // <groupId, TaskGroup>
    std::unordered_map<uint64_t, TaskGroup*> taskGroups_ {};
    std::mutex taskGroupsMutex_;

    // <seqRunnerId, SequenceRunner>
    std::unordered_map<uint64_t, SequenceRunner*> seqRunners_ {};
    std::mutex seqRunnersMutex_;
};
} // namespace Commonlibrary::Concurrent::TaskPoolModule
#endif // JS_CONCURRENT_MODULE_TASKPOOL_TASK_MANAGER_H