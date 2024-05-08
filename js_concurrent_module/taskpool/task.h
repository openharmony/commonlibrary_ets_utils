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
#include <shared_mutex>
#include <string>
#include <uv.h>

#include "helper/concurrent_helper.h"
#include "napi/native_api.h"
#include "utils.h"
#include "tools/log.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
using namespace Commonlibrary::Platform;

enum ExecuteState { NOT_FOUND, WAITING, RUNNING, CANCELED, FINISHED, DELAYED };
enum TaskType { TASK, FUNCTION_TASK, SEQRUNNER_TASK, COMMON_TASK, GROUP_COMMON_TASK, GROUP_FUNCTION_TASK };

struct GroupInfo;
class Worker;
struct TaskInfo {
    napi_deferred deferred = nullptr;
    Priority priority {Priority::DEFAULT};
    void* serializationFunction = nullptr;
    void* serializationArguments = nullptr;
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
};

class Task {
public:
    Task(napi_env env, TaskType taskType, std::string name);
    Task() = default;
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
    static napi_value GetTotalDuration(napi_env env, napi_callback_info info);
    static napi_value GetCPUDuration(napi_env env, napi_callback_info info);
    static napi_value GetIODuration(napi_env env, napi_callback_info info);
    static napi_value GetTaskDuration(napi_env env, napi_callback_info& info, std::string durationType);
    static napi_value GetName(napi_env env, [[maybe_unused]] napi_callback_info info);

    static Task* GenerateTask(napi_env env, napi_value task, napi_value func,
                              napi_value name, napi_value* args, size_t argc);
    static Task* GenerateFunctionTask(napi_env env, napi_value func, napi_value* args, size_t argc, TaskType type);
    static TaskInfo* GenerateTaskInfo(napi_env env, napi_value func, napi_value args,
                                      napi_value transferList, napi_value cloneList, Priority priority,
                                      bool defaultTransfer = true, bool defaultCloneSendable = false);
    static void TaskDestructor(napi_env env, void* data, void* hint);

    static void ThrowNoDependencyError(napi_env env);
    static void ExecuteListenerCallback(const uv_async_t* req);
    static void ExecuteListenerCallback(ListenerCallBackInfo* listenerCallBackInfo);

    void StoreTaskId(uint64_t taskId);
    napi_value GetTaskInfoPromise(napi_env env, napi_value task, TaskType taskType = TaskType::COMMON_TASK,
                                  Priority priority = Priority::DEFAULT);
    TaskInfo* GetTaskInfo(napi_env env, napi_value task, Priority priority);
    void UpdateTaskType(TaskType taskType);
    bool IsRepeatableTask() const;
    bool IsGroupTask() const;
    bool IsGroupCommonTask() const;
    bool IsGroupFunctionTask() const;
    bool IsCommonTask() const;
    bool IsSeqRunnerTask() const;
    bool IsFunctionTask() const;
    bool IsLongTask() const;
    bool IsInitialized() const;
    void IncreaseRefCount();
    void DecreaseRefCount();
    bool IsReadyToHandle() const;
    void NotifyPendingTask();
    void CancelPendingTask(napi_env env);
    bool UpdateTask(uint64_t startTime, void* worker);
    napi_value DeserializeValue(napi_env env, bool isFunc, bool isArgs);
    void StoreTaskDuration();
    bool CanForSequenceRunner(napi_env env);
    bool CanForTaskGroup(napi_env env);
    bool CanExecute(napi_env env);
    bool CanExecuteDelayed(napi_env env);
    void SetHasDependency(bool hasDependency);
    bool HasDependency() const;
    void TryClearHasDependency();

private:
    Task(const Task &) = delete;
    Task& operator=(const Task &) = delete;
    Task(Task &&) = delete;
    Task& operator=(Task &&) = delete;

public:
    napi_env env_ = nullptr;
    TaskType taskType_ {TaskType::TASK};
    std::string name_ {};
    uint64_t taskId_ {};
    std::atomic<ExecuteState> taskState_ {ExecuteState::NOT_FOUND};
    uint64_t groupId_ {}; // 0 for task outside taskgroup
    uint64_t seqRunnerId_ {}; // 0 for task without seqRunner
    TaskInfo* currentTaskInfo_ {};
    std::list<TaskInfo*> pendingTaskInfos_ {}; // for a common task executes multiple times
    void* result_ = nullptr;
    uv_async_t* onResultSignal_ = nullptr;
    std::atomic<bool> success_ {true};
    std::atomic<uint64_t> startTime_ {};
    std::atomic<uint64_t> cpuTime_ {};
    std::atomic<uint64_t> ioTime_ {};
    void* worker_ {nullptr};
    napi_ref taskRef_ {};
    std::atomic<uint32_t> taskRefCount_ {};
    std::recursive_mutex taskMutex_ {};
    bool hasDependency_ {false};
    bool isLongTask_ = {false};
    uv_async_t* onStartExecutionSignal_ = nullptr;
    ListenerCallBackInfo* onEnqueuedCallBackInfo = nullptr;
    ListenerCallBackInfo* onStartExecutionCallBackInfo = nullptr;
    ListenerCallBackInfo* onExecutionFailedCallBackInfo = nullptr;
    ListenerCallBackInfo* onExecutionSucceededCallBackInfo = nullptr;
};

struct CallbackInfo {
    CallbackInfo(napi_env env, uint32_t count, napi_ref ref)
        : hostEnv(env), refCount(count), callbackRef(ref), onCallbackSignal(nullptr) {}
    ~CallbackInfo()
    {
        napi_delete_reference(hostEnv, callbackRef);
        if (onCallbackSignal != nullptr) {
            Common::Helper::ConcurrentHelper::UvHandleClose(onCallbackSignal);
        }
    }

    napi_env hostEnv;
    uint32_t refCount;
    napi_ref callbackRef;
    uv_async_t* onCallbackSignal;
    Worker* worker;
};

struct TaskResultInfo {
    TaskResultInfo(napi_env env, napi_env curEnv, uint64_t id, void* args) : hostEnv(env), workerEnv(curEnv),
        taskId(id), serializationArgs(args) {}
    ~TaskResultInfo() = default;

    napi_env hostEnv;
    napi_env workerEnv;
    uint64_t taskId;
    void* serializationArgs;
};
} // namespace Commonlibrary::Concurrent::TaskPoolModule
#endif // JS_CONCURRENT_MODULE_TASKPOOL_TASK_H