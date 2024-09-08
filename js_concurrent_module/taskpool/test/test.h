/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#ifndef TEST_CONVERTXML_UNITTEST_TEST_H
#define TEST_CONVERTXML_UNITTEST_TEST_H

#include "native_engine.h"

#include "gtest/gtest.h"

#if defined(ENABLE_TASKPOOL_FFRT)
#include "c/executor_task.h"
#include "ffrt_inner.h"
#endif

namespace Commonlibrary::Concurrent::TaskPoolModule {
class NativeEngineTest : public testing::Test {
public:
    NativeEngineTest();
    virtual ~NativeEngineTest();
    void SetUp() override {}
    void TearDown() override {}

    static napi_value IsConcurrent(napi_env env, napi_value argv[], size_t argc);
    static napi_value GetTaskPoolInfo(napi_env env, napi_value argv[], size_t argc);
    static napi_value TerminateTask(napi_env env, napi_value argv[], size_t argc);
    static napi_value Execute(napi_env env, napi_value argv[], size_t argc);
    static napi_value ExecuteDelayed(napi_env env, napi_value argv[], size_t argc);
    static napi_value Cancel(napi_env env, napi_value argv[], size_t argc);
    static void DelayTask(uv_timer_t* handle);
    static napi_value ExecuteGroup(napi_env env, napi_value taskGroup);
    static napi_value ExecutePeriodically(napi_env env, napi_value argv[], size_t argc);
    static void PeriodicTaskCallback(uv_timer_t* handle);
    static void UpdateGroupInfoByResult(napi_env env, uv_timer_t* handle, napi_value res, bool success);

    static void TaskGroupDestructor(napi_env env, void* data);
    static void SequenceRunnerDestructor(napi_env env, void* data);
    static void TryTriggerExpand();
    static void CheckForBlockedWorkers(napi_env env);
    static void TriggerShrink(napi_env env);
    static void foo(const uv_async_t* req);
    static void NotifyShrink(napi_env env);
    static void TryExpand(napi_env env);
    static void CancelTask(napi_env env);
    static void NotifyWorkerIdle(napi_env env);
    static void EnqueueTaskId(napi_env env);
    static void GetTaskByPriority(napi_env env);
    static void RestoreWorker(napi_env env);
    static void StoreDependentId(uint64_t taskId, uint64_t dependentId);
    static void StoreDependentTaskId(uint64_t taskId, uint64_t dependentId);
    static void StoreTaskDuration(uint64_t taskId);
    static void InitTaskManager(napi_env env);
    static void NotifyDependencyTaskInfo(napi_env env);
    static void StoreTaskDependency(napi_env env);
    static void RemoveTaskDependency(napi_env env);
    static void ReleaseTaskData(napi_env env);
    static void CheckTask(napi_env env);
    static void CancelGroupTask(napi_env env);
    static void TriggerSeqRunner(napi_env env);
    static void UpdateGroupState(napi_env env);
    static void ReleaseWorkerHandles(napi_env env);
    static void DebuggerOnPostTask(napi_env env);
    static void PerformTask(napi_env env);
    static void NotifyHandleTaskResult(napi_env env);
    static void TaskResultCallback(napi_env env);
    static void HandleFunctionException(napi_env env);
    static void* WorkerConstructor(napi_env env);

    class ExceptionScope {
    public:
        explicit ExceptionScope(napi_env env) : env_(env) {}
        ~ExceptionScope()
        {
            napi_value exception = nullptr;
            napi_get_and_clear_last_exception(env_, &exception);
        }
    private:
        napi_env env_ = nullptr;
    };

protected:
    NativeEngine *engine_ = nullptr;
};

class SendableUtils {
public:
    SendableUtils() = default;
    ~SendableUtils() = default;

    static napi_value CreateSendableClass(napi_env env);
    static napi_value CreateSendableInstance(napi_env env);
    static napi_value Foo(napi_env env, napi_callback_info info);
    static napi_value Bar(napi_env env, napi_callback_info info);
};
} // namespace Commonlibrary::Concurrent::TaskPoolModule
#endif // TEST_CONVERTXML_UNITTEST_TEST_H