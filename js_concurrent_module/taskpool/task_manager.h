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
#include <shared_mutex>
#include <unordered_map>
#include <unordered_set>

#include "task_queue.h"
#include "napi/native_api.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
static constexpr char FUNCTION_STR[] = "function";
static constexpr char ARGUMENTS_STR[] = "arguments";
static constexpr char TASKID_STR[] = "taskId";
static constexpr char TASKINFO_STR[] = "taskInfo";

class Worker;

class TaskManager {
public:
    TaskManager();
    ~TaskManager();

    static TaskManager &GetInstance();

    static napi_value IsCanceled(napi_env env, napi_callback_info cbinfo);

    uint32_t GenerateTaskId();
    uint32_t GenerateExecuteId();
    bool EraseTaskInfo(uint32_t executeId);
    TaskInfo* GetTaskInfo(uint32_t executeId);
    TaskInfo* PopTaskInfo(uint32_t executeId);
    bool MarkCanceledState(uint32_t executeId);
    void StoreRunningInfo(uint32_t taskId, uint32_t executeId);
    void AddExecuteState(uint32_t executeId);
    bool UpdateExecuteState(uint32_t executeId, ExecuteState state);
    void RemoveExecuteState(uint32_t executeId);
    void PopRunningInfo(uint32_t taskId, uint32_t executeId);
    void EnqueueExecuteId(uint32_t executeId, Priority priority = Priority::DEFAULT);
    std::pair<uint32_t, Priority> DequeueExecuteId();
    void CancelTask(napi_env env, uint32_t taskId);
    void NotifyWorkerIdle(Worker *worker);
    void NotifyWorkerCreated(Worker *worker);
    void PopTaskEnvInfo(napi_env env);
    void InitTaskManager(napi_env env);
    void UpdateExecutedInfo(uint64_t duration);
    TaskInfo* GenerateTaskInfo(napi_env env, napi_value func, napi_value args, uint32_t taskId, uint32_t executeId);
    void ReleaseTaskContent(TaskInfo* taskInfo);
    void TryTriggerLoadBalance();
    uint32_t GetTaskNum();
    uint32_t GetThreadNum();
    uint32_t GetRunningWorkers();

private:
    TaskManager(const TaskManager &) = delete;
    TaskManager& operator=(const TaskManager &) = delete;
    TaskManager(TaskManager &&) = delete;
    TaskManager& operator=(TaskManager &&) = delete;

    ExecuteState QueryExecuteState(uint32_t executeId);
    void NotifyExecuteTask();
    void CreateWorker(napi_env env);
    void NotifyWorkerAdded(Worker *worker);
    void StoreTaskInfo(uint32_t executeId, TaskInfo* taskInfo);

    // for load balance
    void RunTaskManager();
    void CreateOrDeleteWorkers(int32_t targetNum);
    void StoreTaskEnvInfo(napi_env env);
    bool HasTaskEnvInfo(napi_env env);
    int32_t ComputeSuitableThreadNum();
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

    bool isInitialized_ = false;
    std::mutex initMutex_;
    std::array<std::unique_ptr<ExecuteQueue>, Priority::NUMBER> taskQueues_ {};
    std::mutex taskQueuesMutex_;
};
} // namespace Commonlibrary::Concurrent::TaskPoolModule
#endif // JS_CONCURRENT_MODULE_TASKPOOL_TASK_MANAGER_H_