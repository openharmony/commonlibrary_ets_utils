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

#ifndef JS_CONCURRENT_MODULE_TASKPOOL_TASK_H
#define JS_CONCURRENT_MODULE_TASKPOOL_TASK_H

#include <list>
#include <map>
#include <mutex>
#include <set>
#include <shared_mutex>
#include <string>
#include <tuple>
#include <unordered_map>
#include <uv.h>

#include "helper/concurrent_helper.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "utils.h"
#include "tools/log.h"
#if defined(ENABLE_TASKPOOL_EVENTHANDLER)
#include "event_handler.h"
#endif

#if defined(ENABLE_TASKPOOL_FFRT)
#include "c/executor_task.h"
#include "ffrt_inner.h"
#endif

namespace Commonlibrary::Concurrent::TaskPoolModule {
using namespace Commonlibrary::Platform;

extern const std::unordered_map<Priority, napi_event_priority> g_napiPriorityMap;
enum ExecuteState { NOT_FOUND, WAITING, RUNNING, CANCELED, FINISHED, DELAYED, ENDING};
enum TaskType {
    TASK,
    FUNCTION_TASK,
    SEQRUNNER_TASK,
    COMMON_TASK,
    GROUP_COMMON_TASK,
    GROUP_FUNCTION_TASK,
    ASYNCRUNNER_TASK
};

struct GroupInfo;
class Worker;
struct TaskInfo {
    napi_deferred deferred = nullptr;
    Priority priority = Priority::DEFAULT;
    void* serializationFunction = nullptr;
    void* serializationArguments = nullptr;
};

struct TaskCurrentInfo {
    std::string name {};
    uint32_t taskId {};
    ExecuteState taskState {ExecuteState::NOT_FOUND};
    uint64_t startTime {};
};

struct ListenerCallBackInfo {
    ListenerCallBackInfo(napi_env env, napi_ref callbackRef, napi_value taskError) : env_(env),
        callbackRef_(callbackRef), taskError_(taskError) {}
    ~ListenerCallBackInfo()
    {
        napi_delete_reference(env_, callbackRef_);
    }
    napi_env env_;
    napi_ref callbackRef_;
    napi_value taskError_;
    std::string type_;
};

struct CancelTaskMessage {
    CancelTaskMessage(ExecuteState state, uint32_t taskId) : state(state), taskId(taskId) {}
    ~CancelTaskMessage() = default;

    ExecuteState state;
    uint32_t taskId;
};

struct DiscardTaskMessage {
    DiscardTaskMessage(napi_env env, uint32_t taskId, int32_t errCode, bool isWaiting) : env(env),
        taskId(taskId), errCode(errCode), isWaiting(isWaiting) {}
    ~DiscardTaskMessage() = default;
    
    napi_env env;
    uint32_t taskId;
    int32_t errCode;
    bool isWaiting;
};

class Task {
public:
    Task() = default;
    Task(napi_env env, TaskType taskType, const char* name) : env_(env), taskType_(taskType), name_(name) {}

    ~Task() = default;

    static napi_value TaskConstructor(napi_env env, napi_callback_info cbinfo);
    static napi_value LongTaskConstructor(napi_env env, napi_callback_info cbinfo);
    static napi_value SetTransferList(napi_env env, napi_callback_info cbinfo);
    static napi_value SetCloneList(napi_env env, napi_callback_info cbinfo);
    static napi_value IsCanceled(napi_env env, napi_callback_info cbinfo);
    static napi_value OnReceiveData(napi_env env, napi_callback_info cbinfo);
    static napi_value SendData(napi_env env, napi_callback_info cbinfo);
    static napi_value AddDependency(napi_env env, napi_callback_info cbinfo);
    static napi_value RemoveDependency(napi_env env, napi_callback_info cbinfo);
    static napi_value OnEnqueued(napi_env env, napi_callback_info cbinfo);
    static napi_value OnStartExecution(napi_env env, napi_callback_info cbinfo);
    static napi_value OnExecutionFailed(napi_env env, napi_callback_info cbinfo);
    static napi_value OnExecutionSucceeded(napi_env env, napi_callback_info cbinfo);
    static napi_value IsDone(napi_env env, napi_callback_info cbinfo);
    static napi_value GetTotalDuration(napi_env env, napi_callback_info info);
    static napi_value GetCPUDuration(napi_env env, napi_callback_info info);
    static napi_value GetIODuration(napi_env env, napi_callback_info info);
    static napi_value GetTaskDuration(napi_env env, napi_callback_info& info, std::string durationType);
    static napi_value GetName(napi_env env, napi_callback_info info);
    static napi_value GetTaskId(napi_env env, napi_callback_info info);

    static Task* GenerateTask(napi_env env, napi_value task, napi_value func,
                              napi_value name, napi_value* args, size_t argc);
    static Task* GenerateFunctionTask(napi_env env, napi_value func, napi_value* args, size_t argc, TaskType type);
    static TaskInfo* GenerateTaskInfo(napi_env env, napi_value func, napi_value args,
                                      napi_value transferList, napi_value cloneList, Priority priority,
                                      bool defaultTransfer = true, bool defaultCloneSendable = false);
    static void TaskDestructor(napi_env env, void* data, void* hint);

    static void ThrowNoDependencyError(napi_env env);
    static void StartExecutionCallback(const uv_async_t* req);
    static void StartExecutionTask(ListenerCallBackInfo* listenerCallBackInfo);
    static void ExecuteListenerCallback(ListenerCallBackInfo* listenerCallBackInfo, uint32_t taskId);
    static void CleanupHookFunc(void* arg);
    static void Cancel(const uv_async_t* req);
    static void DiscardTask(const uv_async_t* req);
    static bool VerifyAndPostResult(Task* task, Priority priority);

    void StoreTaskId(uint32_t taskId);
    napi_value GetTaskInfoPromise(napi_env env, napi_value task, TaskType taskType = TaskType::COMMON_TASK,
                                  Priority priority = Priority::DEFAULT);
    TaskInfo* GetTaskInfo(napi_env env, napi_value task, Priority priority);
    void UpdateTaskType(TaskType taskType);
    void UpdatePeriodicTask();
    bool IsRepeatableTask() const;
    bool IsGroupTask() const;
    bool IsGroupCommonTask() const;
    bool IsGroupFunctionTask() const;
    bool IsCommonTask() const;
    bool IsSeqRunnerTask() const;
    bool IsFunctionTask() const;
    bool IsLongTask() const;
    bool IsPeriodicTask() const;
    bool IsMainThreadTask() const;
    bool IsExecuted() const;
    void IncreaseRefCount();
    void DecreaseRefCount();
    bool IsReadyToHandle() const;
    void NotifyPendingTask();
    void CancelPendingTask(napi_env env);
    bool UpdateTask(uint64_t startTime, void* worker);
    napi_value DeserializeValue(napi_env env, napi_value* func, napi_value* args);
    void StoreTaskDuration();
    bool CanForSequenceRunner(napi_env env);
    bool CanForTaskGroup(napi_env env);
    bool CanExecute(napi_env env);
    bool CanExecuteDelayed(napi_env env);
    bool CanExecutePeriodically(napi_env env);
    void SetHasDependency(bool hasDependency);
    bool HasDependency() const;
    void TryClearHasDependency();
    void ClearDelayedTimers();
    void IncreaseTaskLifecycleCount();
    void DecreaseTaskLifecycleCount();
    bool ShouldDeleteTask(bool needUnref = true);
    bool CheckStartExecution(Priority priority);
    bool IsValid();
    void SetValid(bool isValid);
    bool CanForAsyncRunner(napi_env env);
    bool IsAsyncRunnerTask();
    void SetTaskId(uint32_t taskId);
    void TriggerCancel(CancelTaskMessage* message);
    void CancelInner(ExecuteState state);
    bool IsSameEnv(napi_env env);
    void DiscardAsyncRunnerTask(DiscardTaskMessage* message);
    void DiscardInner(DiscardTaskMessage* message);
    void ReleaseData();
    void DisposeCanceledTask();
    Worker* GetWorker() const;
    napi_env GetEnv() const;
    uint32_t GetTaskId() const;
    bool IsRealyCanceled();
    bool UpdateTaskStateToWaiting();
    bool UpdateTaskStateToRunning();
    bool UpdateTaskStateToCanceled();
    bool UpdateTaskStateToFinished();
    bool UpdateTaskStateToDelayed();
    bool UpdateTaskStateToEnding();
    void TriggerEnqueueCallback();
    void StoreEnqueueTime();

    static std::tuple<napi_value, napi_value, napi_value, napi_value> GetSerializeParams(napi_env env,
                                                                                         napi_value napiTask);
    static std::tuple<void*, void*> GetSerializeResult(napi_env env, napi_value func, napi_value args,
        std::tuple<napi_value, napi_value, bool, bool> transferAndCloneParams);

private:
    Task(const Task &) = delete;
    Task& operator=(const Task &) = delete;
    Task(Task &&) = delete;
    Task& operator=(Task &&) = delete;

    void InitHandle(napi_env env);

public:
    napi_env env_ = nullptr;
    std::atomic<TaskType> taskType_ {TaskType::TASK};
    std::string name_ {};
    uint32_t taskId_ {};
    std::atomic<ExecuteState> taskState_ {ExecuteState::NOT_FOUND};
    uint64_t groupId_ {}; // 0 for task outside taskgroup
    uint64_t runnerId_ {}; // 0 for task without runner
    TaskInfo* currentTaskInfo_ {};
    std::list<TaskInfo*> pendingTaskInfos_ {}; // for a common task executes multiple times
    void* result_ = nullptr;
    std::atomic<bool> success_ {true};
    std::atomic<uint64_t> startTime_ {};
    std::atomic<uint64_t> cpuTime_ {};
    std::atomic<uint64_t> ioTime_ {};
    void* worker_ {nullptr};
    napi_ref taskRef_ {};
    std::atomic<uint32_t> taskRefCount_ {};
    std::recursive_mutex taskMutex_ {};
    bool hasDependency_ {false};
    bool isLongTask_ {false};
    bool defaultTransfer_ {true};
    bool defaultCloneSendable_ {false};
    std::atomic<bool> isValid_ {true};
    std::atomic<uint32_t> lifecycleCount_ {0}; // when lifecycleCount_ is 0, the task pointer can be deleted
    uv_async_t* onStartCancelSignal_ = nullptr;
    uv_async_t* onStartDiscardSignal_ = nullptr;
    ListenerCallBackInfo* onEnqueuedCallBackInfo_ = nullptr;
    ListenerCallBackInfo* onStartExecutionCallBackInfo_ = nullptr;
    ListenerCallBackInfo* onExecutionFailedCallBackInfo_ = nullptr;
    ListenerCallBackInfo* onExecutionSucceededCallBackInfo_ = nullptr;

    // for periodic task
    bool isPeriodicTask_ {false};
    uv_timer_t* timer_ {nullptr};
    Priority periodicTaskPriority_ {Priority::DEFAULT};

    std::set<uv_timer_t*> delayedTimers_ {}; // task delayed timer

    bool isMainThreadTask_ {false};
    Priority asyncTaskPriority_ {Priority::DEFAULT};
    std::atomic<bool> isCancelToFinish_ {false};

    std::string enqueueTime_ {};
    std::string runningTime_ {};
    std::string finishedTime_ {};
};

struct CallbackInfo {
    CallbackInfo(napi_env env, uint32_t count, napi_ref ref)
        : hostEnv(env), refCount(count), callbackRef(ref) {}
    ~CallbackInfo()
    {
        napi_delete_reference(hostEnv, callbackRef);
    }

    napi_env hostEnv;
    uint32_t refCount;
    napi_ref callbackRef;
    std::string type;
};

struct TaskResultInfo {
    TaskResultInfo(napi_env workerEnv, uint32_t taskId, void* args)
        : workerEnv(workerEnv), taskId(taskId), serializationArgs(args) {}
    ~TaskResultInfo() = default;

    napi_env workerEnv;
    uint32_t taskId;
    void* serializationArgs;
};
} // namespace Commonlibrary::Concurrent::TaskPoolModule
#endif // JS_CONCURRENT_MODULE_TASKPOOL_TASK_H