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

#ifndef JS_CONCURRENT_MODULE_TASKPOOL_TASK_GROUP_H
#define JS_CONCURRENT_MODULE_TASKPOOL_TASK_GROUP_H

#include <list>

#include "task.h"
#include "task_manager.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
struct GroupInfo {
public:
    int32_t GetFailedIndex()
    {
        return failedIndex;
    }

    void SetFailedIndex(int32_t index)
    {
        failedIndex = index;
    }
    
    bool HasException()
    {
        return failedIndex != -1;
    }

    uint32_t finishedTaskNum {};
    napi_ref resArr {nullptr};
    Priority priority {Priority::DEFAULT};
    napi_deferred deferred {nullptr};

private:
    int32_t failedIndex {-1}; // -1: the initial value means no exception
};

class TaskGroup {
public:
    explicit TaskGroup(napi_env env) : env_(env) {}
    TaskGroup() = default;
    ~TaskGroup() = default;

    static napi_value TaskGroupConstructor(napi_env env, napi_callback_info cbinfo);
    static napi_value AddTask(napi_env env, napi_callback_info cbinfo);
    static void HostEnvCleanupHook(void* data);
    static void StartRejectResult(const uv_async_t* req);

    uint32_t GetTaskIndex(uint32_t taskId);
    void NotifyGroupTask(napi_env env);
    void CancelPendingGroup(napi_env env);
    void CancelGroupTask(napi_env env, uint32_t taskId);
    void RejectResult(napi_env env, napi_value res);
    void RejectResult(napi_env env);
    void InitHandle(napi_env env);
    void TriggerRejectResult();
    bool IsSameEnv(napi_env env);
    bool IsTimeoutTaskGroup() const;
    void SetTimeout(uint32_t timeout);
    bool UpdateStateToTimeout();
    bool IsNotFoundState();
    bool IsWaitingState();
    bool IsRunningState();
    bool IsCanceledState();
    bool IsFinishedState();
    bool IsTimeoutState();
    void ClearTimeoutTimer();

private:
    TaskGroup(const TaskGroup &) = delete;
    TaskGroup& operator=(const TaskGroup &) = delete;
    TaskGroup(TaskGroup &&) = delete;
    TaskGroup& operator=(TaskGroup &&) = delete;

    static void TaskGroupDestructor(napi_env env, void* data, void* hint);

    friend class NativeEngineTest;
public:
    napi_env env_ = nullptr;
    uint64_t groupId_ {};
    GroupInfo* currentGroupInfo_ {};
    std::list<GroupInfo*> pendingGroupInfos_ {};
    std::list<napi_ref> taskRefs_ {};
    std::list<uint32_t> taskIds_ {};
    uint32_t taskNum_ {};
    std::atomic<ExecuteState> groupState_ {ExecuteState::NOT_FOUND};
    napi_ref groupRef_ {};
    std::recursive_mutex taskGroupMutex_ {};
    uv_async_t* onRejectResultSignal_ = nullptr;
    std::atomic<bool> isValid_ {true};
    uint32_t timeout_ {0};
    uv_timer_t* timer_ {nullptr}; // timeout timer
};
} // namespace Commonlibrary::Concurrent::TaskPoolModule
#endif // JS_CONCURRENT_MODULE_TASKPOOL_TASK_GROUP_H