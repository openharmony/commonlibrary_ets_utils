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
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "dfx_hisys_event.h"
#include "log_manager.h"
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

    void StoreTask(Task* task);
    bool RemoveTask(uint32_t taskId);
    void RemoveRunningTask(uint32_t taskId);
    Task* GetTask(uint32_t taskId);
    Task* GetTaskForPerform(uint32_t taskId);
    void EnqueueTaskId(uint32_t taskId, Priority priority = Priority::DEFAULT);
    bool EraseWaitingTaskId(uint32_t taskId, Priority priority);
    std::pair<uint32_t, Priority> DequeueTaskId();
    void CancelTask(napi_env env, uint32_t taskId);
    void CancelSeqRunnerTask(napi_env env, Task* task);
    void ReleaseTaskData(napi_env env, Task* task, bool shouldDeleteTask = true);

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
    bool ReadThreadInfo(pid_t tid, char* buf, uint32_t size);

    // for get thread info
    napi_value GetThreadInfos(napi_env env);

    // for get task info
    napi_value GetTaskInfos(napi_env env);

    // for countTrace for worker
    void CountTraceForWorker(bool needLog = false);
    void CountTraceForWorkerWithoutLock(bool needLog = false);

    void RegisterCallback(napi_env env, uint32_t taskId, std::shared_ptr<CallbackInfo> callbackInfo,
        const std::string& type);
    void IncreaseSendDataRefCount(uint32_t taskId);
    void DecreaseSendDataRefCount(napi_env env, uint32_t taskId, Task* task = nullptr);
    void ExecuteSendData(napi_env env, TaskResultInfo* resultInfo, uint32_t taskId, Task* task);

    // for task dependency
    bool IsDependendByTaskId(uint32_t taskId);
    bool IsDependentByTaskId(uint32_t dependentTaskId);
    void NotifyDependencyTaskInfo(uint32_t taskId);
    void RemoveDependencyById(uint32_t dependentTaskId, uint32_t taskId);
    bool StoreTaskDependency(uint32_t taskId, std::set<uint32_t> taskIdSet);
    bool RemoveTaskDependency(uint32_t taskId, uint32_t dependentId);
    bool CheckCircularDependency(std::set<uint32_t> dependentIdSet, std::set<uint32_t> idSet, uint32_t taskId);
    void EnqueuePendingTaskInfo(uint32_t taskId, Priority priority);
    std::pair<uint32_t, Priority> DequeuePendingTaskInfo(uint32_t taskId);
    void RemovePendingTaskInfo(uint32_t taskId);
    void StoreDependentTaskInfo(std::set<uint32_t> dependTaskIdSet, uint32_t taskId);
    void RemoveDependentTaskInfo(uint32_t dependentTaskId, uint32_t taskId);
    std::string GetTaskDependInfoToString(uint32_t taskId);

    bool PostTask(std::function<void()> task, const char* taskName, Priority priority = Priority::DEFAULT);

    // for duration
    void StoreTaskDuration(uint32_t taskId, uint64_t totalDuration, uint64_t cpuDuration);
    uint64_t GetTaskDuration(uint32_t taskId, std::string durationType);
    void RemoveTaskDuration(uint32_t taskId);
    void StoreLongTaskInfo(uint32_t taskId, Worker* worker);
    void RemoveLongTaskInfo(uint32_t taskId);
    void TerminateTask(uint32_t taskId);
    Worker* GetLongTaskInfo(uint32_t taskId);

    void StoreTaskEnqueueTime(uint32_t taskId, std::string enqueueTimeStamp);
    void RemoveTaskEnqueueTime(uint32_t taskId);
    std::string GetTaskEnqueueTime(uint32_t taskId);

    // for callback
    void ReleaseCallBackInfo(Task* task);

    void UpdateSystemAppFlag();

    bool IsSystemApp() const
    {
        return isSystemApp_;
    }

    bool EnableFfrt() const
    {
        return globalEnableFfrtFlag_ || (isSystemApp_ && !disableFfrtFlag_);
    }

    void BatchRejectDeferred(napi_env env, std::list<napi_deferred> deferreds, std::string error);
    uint32_t CalculateTaskId(uint64_t id);
    void ClearDependentTask(uint32_t taskId);
    void UvReportHisysEvent(Worker* worker, std::string methodName, std::string funName, std::string message,
                            int32_t code);
    napi_value CancelError(napi_env env, int32_t errCode, const char* errMessage = nullptr,
                           napi_value result = nullptr, bool success = false);
    void SetIsPerformIdle(bool performIdle);
    bool IsPerformIdle() const;

    // for taskpool support interop
    void AttachWorkerToAniVm(Worker* worker);
    void DetachWorkerFromAniVm(Worker* worker);

    uint32_t GetNonIdleTaskNum();
    uint32_t GetTotalTaskNum() const;
    bool ExecuteTaskStartExecution(uint32_t taskId, Priority priority);
    CallbackInfo* GetSenddataCallback(uint32_t taskId);
    std::string GetFuncNameFromData(void* data);
    bool IsValidTask(Task* task);

    void PushLog(const std::string& msg)
    {
        logManager_.PushLog(msg);
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
    template <bool needCheckIdle> void TryExpandWithCheckIdle();
    void NotifyShrink(uint32_t targetNum);
    void TriggerShrink(uint32_t step);
    uint32_t ComputeSuitableThreadNum();
    uint32_t ComputeSuitableIdleNum();
    void DispatchAndTryExpandInner();
    static void TryExpand(const uv_timer_t* req = nullptr);
    static void DispatchAndTryExpand(const uv_async_t* req);
    static void TriggerLoadBalance(const uv_timer_t* req);
    static void PrintWaitingTime(const uv_timer_t* req);

    bool IsChooseIdle();
    std::pair<uint32_t, Priority> GetTaskByPriority(const std::unique_ptr<ExecuteQueue>& taskQueue, Priority priority);
    void IncreaseTaskNum(Priority priority);
    void DecreaseTaskNum(Priority priority);
    void RemoveDependTaskByTaskId(uint32_t taskId);
    void RemoveDependentTaskByTaskId(uint32_t taskId);
    void CheckTasksAndReportHisysEvent();
    void WorkerAliveAndReport(Worker* worker);
    void WriteHisysForFfrtAndUv(Worker* worker, HisyseventParams* hisyseventParams);
    void AddCountTraceForWorkerLog(bool needLog, int64_t threadNum, int64_t idleThreadNum, int64_t timeoutThreadNum);
    std::tuple<napi_env, napi_event_priority> GetTaskEnvAndPriority(uint32_t taskId);
    void IncreaseTaskIdSalt();
    void TimerStop(uv_timer_t*& timer, const char* errMessage);
    void TimerInit(uv_timer_t*& timer, bool startFlag, uv_timer_cb cb, uint64_t repeat);

    // <taskId, Task>
    std::unordered_map<uint32_t, Task*> tasks_ {};
    std::unordered_map<uint32_t, Task*> runningTasks_ {};
    std::recursive_mutex tasksMutex_;

    // <taskId, <dependent taskId1, dependent taskId2, ...>>, update when removeDependency or executeTask
    std::unordered_map<uint32_t, std::set<uint32_t>> dependTaskInfos_ {};
    std::shared_mutex dependTaskInfosMutex_;

    // <dependent taskId, <taskId1, taskId2, ...>>, update when removeDependency or executeTask
    std::unordered_map<uint32_t, std::set<uint32_t>> dependentTaskInfos_ {};
    std::shared_mutex dependentTaskInfosMutex_;

    // <<pendingTaskId1, priority>, <pendingTaskId2, priority>, ...>
    std::unordered_map<uint32_t, Priority> pendingTaskInfos_ {};
    std::shared_mutex pendingTaskInfosMutex_;

    // <<taskId1, <totalDuration1, cpuDuration1>>, <taskId2, <totalDuration2, cpuDuration2>>, ...>
    std::unordered_map<uint32_t, std::pair<uint64_t, uint64_t>> taskDurationInfos_ {};
    std::shared_mutex taskDurationInfosMutex_;

    // record the longTasks and workers for efficiency
    std::unordered_map<uint32_t, Worker*> longTasksMap_ {};
    std::shared_mutex longTasksMutex_{};

    std::unordered_set<Worker*> workers_ {};
    std::unordered_set<Worker*> idleWorkers_ {};
    std::unordered_set<Worker*> timeoutWorkers_ {};
    std::recursive_mutex workersMutex_;

    std::unordered_map<uint32_t, std::string> taskEnqueueTimeMap_ {};
    std::mutex taskEnqueueTimeMutex_;

    // for load balance
    napi_env hostEnv_ = nullptr;
    uv_loop_t* loop_ = nullptr;
    uv_timer_t* balanceTimer_ = nullptr;
    uv_timer_t* expandTimer_ = nullptr;
    uv_async_t* dispatchHandle_ = nullptr;
    std::atomic<bool> suspend_ = false;
    std::atomic<uint32_t> retryCount_ = 0;
    std::atomic<uint32_t> expandingCount_ = 0;
    std::atomic<uint32_t> nonIdleTaskNum_ = 0;
    std::atomic<uint32_t> totalTaskNum_ = 0;
    std::atomic<uint32_t> totalExecCount_ = 0;
    std::atomic<uint64_t> totalExecTime_ = 0;
    std::atomic<bool> needChecking_ = false;
    std::atomic<bool> isHandleInited_ = false;
    std::atomic<uint32_t> timerTriggered_ = false;
    std::atomic<uint64_t> preDequeneTime_ = 0;
    std::atomic<uint64_t> reportTime_ = 0;

    // for task priority
    uint32_t highPrioExecuteCount_ = 0;
    uint32_t mediumPrioExecuteCount_ = 0;
    std::array<std::unique_ptr<ExecuteQueue>, Priority::NUMBER> taskQueues_ {};
    std::mutex taskQueuesMutex_;

    std::atomic<bool> isInitialized_ = false;
    std::atomic<bool> isSystemApp_ = false;
    int disableFfrtFlag_ = 0; // 0 means enable ffrt
    int globalEnableFfrtFlag_ = 0; // 0 means not global enable ffrt

    std::mutex callbackMutex_;
    std::map<uint32_t, std::shared_ptr<CallbackInfo>> callbackTable_ {};
    std::vector<Worker*> freeList_ {};
    uint32_t maxThreads_ = ConcurrentHelper::GetMaxThreads();

#if defined(ENABLE_TASKPOOL_EVENTHANDLER)
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> mainThreadHandler_ {};
#endif
    std::atomic<bool> isPerformIdle_ = false;
    std::atomic<uint32_t> taskIdSalt_ = 1;

    uv_timer_t* waitingTimer_ = nullptr;

    LogManager logManager_;

    friend class TaskGroupManager;
    friend class NativeEngineTest;
};
} // namespace Commonlibrary::Concurrent::TaskPoolModule
#endif // JS_CONCURRENT_MODULE_TASKPOOL_TASK_MANAGER_H