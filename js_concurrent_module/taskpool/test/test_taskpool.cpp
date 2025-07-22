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

#include "test.h"

#include <unistd.h>

#include "async_runner.h"
#include "async_runner_manager.h"
#include "helper/napi_helper.h"
#include "queue.h"
#include "sequence_runner_manager.h"
#include "task.h"
#include "taskpool.h"
#include "task_group_manager.h"
#include "task_manager.h"
#include "task_runner.h"
#include "thread.h"
#include "tools/log.h"
#include "uv.h"
#include "worker.h"

using namespace Commonlibrary::Concurrent::TaskPoolModule;
void GetSendableFunction(napi_env env, const char* str, napi_value& result)
{
    napi_value instance = SendableUtils::CreateSendableInstance(env);
    napi_value name = nullptr;
    napi_create_string_utf8(env, str, NAPI_AUTO_LENGTH, &name);
    napi_get_property(env, instance, name, &result);
}

napi_value GeneratorTask(napi_env env, napi_value thisVar)
{
    napi_value func = nullptr;
    GetSendableFunction(env, "foo", func);
    napi_value argv[] = { func };
    std::string funcName = "TaskConstructor";
    napi_value cb = nullptr;
    napi_value task = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::TaskConstructor, nullptr, &cb);
    napi_call_function(env, thisVar, cb, 1, argv, &task);
    return task;
}

napi_value GeneratorTaskGroup(napi_env env, napi_value thisVar)
{
    std::string funcName = "TaskGroupConstructor";
    napi_value argv[] = {};
    napi_value cb = nullptr;
    napi_value group = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), TaskGroup::TaskGroupConstructor, nullptr, &cb);
    napi_call_function(env, thisVar, cb, 0, argv, &group);
    return group;
}

napi_value GeneratorTaskGroupWithName(napi_env env, napi_value thisVar, const char* name)
{
    std::string funcName = "TaskGroupConstructor";
    napi_value str = nullptr;
    napi_create_string_utf8(env, name, NAPI_AUTO_LENGTH, &str);
    napi_value argv[] = { str };
    napi_value cb = nullptr;
    napi_value group = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), TaskGroup::TaskGroupConstructor, nullptr, &cb);
    napi_call_function(env, thisVar, cb, 1, argv, &group);
    return group;
}

napi_value CreateTaskObject(napi_env env, TaskType taskType = TaskType::TASK,
    ExecuteState state = ExecuteState::NOT_FOUND, bool needStoreTask = false)
{
    napi_value thisValue = NapiHelper::CreateObject(env);
    size_t argc = 0;
    napi_value func = nullptr;
    napi_create_string_utf8(env, "testFunc", NAPI_AUTO_LENGTH, &func);
    napi_value* args = new napi_value[1];
    ObjectScope<napi_value> objScope(args, true);
    napi_value taskName = NapiHelper::CreateEmptyString(env);
    Task* task = Task::GenerateTask(env, thisValue, func, taskName, args, argc);
    task->UpdateTaskType(taskType);
    if (state != ExecuteState::NOT_FOUND) {
        task->taskState_ = state;
    }
    if (!needStoreTask) {
        TaskManager::GetInstance().RemoveTask(task->taskId_);
    }
    if (napi_wrap(
        env, thisValue, task,
        [](napi_env environment, void* data, void* hint) {
            auto obj = reinterpret_cast<Task*>(data);
            if (obj != nullptr) {
                delete obj;
            }
        }, nullptr, nullptr) != napi_ok) {
        delete task;
        task = nullptr;
        return nullptr;
    }
    return thisValue;
}

napi_value CreateNullTaskObject(napi_env env)
{
    napi_value thisValue = NapiHelper::CreateObject(env);
    Task* task = nullptr;
    napi_wrap(
        env, thisValue, task,
        [](napi_env environment, void* data, void* hint) {
            auto obj = reinterpret_cast<Task*>(data);
            if (obj != nullptr) {
                delete obj;
            }
        }, nullptr, nullptr);
    return thisValue;
}

napi_value GetNapiString(napi_env env, const char* str)
{
    napi_value name = nullptr;
    napi_create_string_utf8(env, str, NAPI_AUTO_LENGTH, &name);
    return name;
}

HWTEST_F(NativeEngineTest, TaskpoolTest001, testing::ext::TestSize.Level0)
{
    TaskManager &taskManager = TaskManager::GetInstance();
    uint32_t result = taskManager.GetThreadNum();
    ASSERT_TRUE(result == 0);
}

HWTEST_F(NativeEngineTest, TaskpoolTest002, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    Worker* worker = reinterpret_cast<Worker*>(NativeEngineTest::WorkerConstructor(env));
    ASSERT_NE(worker, nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest003, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    napi_value taskClass = nullptr;
    napi_value result = TaskPool::InitTaskPool(env, taskClass);
    usleep(50000);
    ASSERT_TRUE(result == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest004, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    Worker* worker = reinterpret_cast<Worker*>(NativeEngineTest::WorkerConstructor(env));
    ASSERT_NE(worker, nullptr);
    uint32_t workers = TaskManager::GetInstance().GetRunningWorkers();
    ASSERT_TRUE(workers == 0);
}

HWTEST_F(NativeEngineTest, TaskpoolTest005, testing::ext::TestSize.Level0)
{
    uint32_t taskId = 10;
    TaskManager &taskManager = TaskManager::GetInstance();
    Task* task = taskManager.GetTask(taskId);
    ASSERT_TRUE(task == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest006, testing::ext::TestSize.Level0)
{
    TaskManager &taskManager = TaskManager::GetInstance();
    std::pair<uint32_t, Priority> result = taskManager.DequeueTaskId();
    ASSERT_TRUE(result.first == 0);
    ASSERT_TRUE(result.second == Priority::LOW);
}

HWTEST_F(NativeEngineTest, TaskpoolTest007, testing::ext::TestSize.Level0)
{
    TaskManager &taskManager = TaskManager::GetInstance();
    uint32_t result = taskManager.GetTaskNum();
    ASSERT_TRUE(result == 0);
}

HWTEST_F(NativeEngineTest, TaskpoolTest008, testing::ext::TestSize.Level0)
{
    ExecuteQueue executeQueue;
    uint32_t result = executeQueue.DequeueTaskId();
    ASSERT_TRUE(result == 0);
}

HWTEST_F(NativeEngineTest, TaskpoolTest009, testing::ext::TestSize.Level0)
{
    ExecuteQueue executeQueue;
    bool result = executeQueue.IsEmpty();
    ASSERT_TRUE(result);
}

HWTEST_F(NativeEngineTest, TaskpoolTest010, testing::ext::TestSize.Level0)
{
    ExecuteQueue executeQueue;
    uint32_t result = executeQueue.GetTaskNum();
    ASSERT_TRUE(result == 0);
}

HWTEST_F(NativeEngineTest, TaskpoolTest011, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value argv[] = {nullptr};
    std::string funcName = "TaskGroupConstructor";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), TaskGroup::TaskGroupConstructor, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 0, argv, &result);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest012, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value argv[] = {nullptr};
    std::string funcName = "TaskGroupConstructor";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), TaskGroup::TaskGroupConstructor, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 0, argv, &result);
    ASSERT_TRUE(result != nullptr);
    napi_value exception;
    napi_get_and_clear_last_exception(env, &exception);

    size_t argc1 = 0;
    napi_value argv1[] = {nullptr};
    funcName = "AddTask";
    cb = nullptr;
    napi_value result1 = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), TaskGroup::AddTask, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc1, argv1, &result1);
    ASSERT_TRUE(result1 == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest013, testing::ext::TestSize.Level0)
{
    uint32_t number = 10;
    napi_env env = reinterpret_cast<napi_env>(engine_);
    napi_value value = NapiHelper::CreateUint32(env, number);
    napi_value result = TaskPool::InitTaskPool(env, value);
    usleep(50000);
    ASSERT_TRUE(result != nullptr);
}

napi_value TestFunction(napi_env env)
{
    napi_value result = nullptr;
    const char* message = "test taskpool";
    size_t length = strlen(message);
    napi_create_string_utf8(env, message, length, &result);
    return result;
}

HWTEST_F(NativeEngineTest, TaskpoolTest014, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManager = TaskManager::GetInstance();
    taskManager.InitTaskManager(env);
    usleep(50000);
    uint32_t taskNum = taskManager.GetTaskNum();
    ASSERT_TRUE(taskNum == 0);
    uint32_t threadNum = taskManager.GetThreadNum();
    ASSERT_TRUE(threadNum != 0);
    uint32_t idleWorkers = taskManager.GetIdleWorkers();
    ASSERT_TRUE(idleWorkers != 0);
    uint32_t runningWorkers = taskManager.GetRunningWorkers();
    ASSERT_TRUE(runningWorkers == 0);
    uint32_t timeoutWorkers = taskManager.GetTimeoutWorkers();
    ASSERT_TRUE(timeoutWorkers == 0);
}

HWTEST_F(NativeEngineTest, TaskpoolTest015, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManager = TaskManager::GetInstance();
    std::pair<uint32_t, Priority> result = taskManager.DequeueTaskId();
    ASSERT_TRUE(result.first == 0);
    ASSERT_TRUE(result.second == Priority::LOW);
}

HWTEST_F(NativeEngineTest, TaskpoolTest016, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    ExceptionScope scope(env);
    TaskManager& taskManager = TaskManager::GetInstance();
    Task* task = new Task();
    uint32_t taskId = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task));
    taskManager.CancelTask(env, taskId);
    ASSERT_TRUE(taskId != 0);
    delete task;
}

HWTEST_F(NativeEngineTest, TaskpoolTest017, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManager = TaskManager::GetInstance();
    Task* task = new Task();
    uint32_t taskId = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task));
    taskManager.TryTriggerExpand();
    ASSERT_TRUE(taskId != 0);
    delete task;
}

HWTEST_F(NativeEngineTest, TaskpoolTest018, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManager = TaskManager::GetInstance();
    Task* task = new Task();
    uint32_t taskId = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task));
    uint64_t duration = 10;
    taskManager.UpdateExecutedInfo(duration);
    ASSERT_TRUE(taskId != 0);
    delete task;
}

HWTEST_F(NativeEngineTest, TaskpoolTest019, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value argv[] = {nullptr};
    std::string funcName = "TaskConstructor";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::TaskConstructor, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 0, argv, &result);
    ASSERT_TRUE(result == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest020, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value argv[] = {nullptr};
    std::string funcName = "TaskConstructor";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::TaskConstructor, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 0, argv, &result);
    ASSERT_TRUE(result == nullptr);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    cb = nullptr;
    napi_value result1 = nullptr;
    funcName = "SetTransferList";
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::SetTransferList, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 0, argv, &result1);
    ASSERT_TRUE(result1 != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest021, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    size_t argc = 10;
    napi_value argv[] = {nullptr};
    std::string funcName = "TaskConstructor";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::TaskConstructor, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 0, argv, &result);
    ASSERT_TRUE(result == nullptr);

    napi_value func = TestFunction(env);
    uint32_t number = 10;
    napi_value value = NapiHelper::CreateUint32(env, number);
    napi_value* args = new napi_value[argc];
    ObjectScope<napi_value> objScope(args, true);
    napi_value taskName = NapiHelper::CreateEmptyString(env);
    Task::GenerateTask(env, value, func, taskName, args, argc);
    ASSERT_TRUE(args != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest022, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    ExceptionScope scope(env);
    TaskGroupManager &taskGroupManager = TaskGroupManager::GetInstance();
    TaskGroup* group = new TaskGroup();
    uint64_t groupId = reinterpret_cast<uint64_t>(group);
    Task* task = new Task();
    uint32_t taskId = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task));
    napi_value value = NapiHelper::CreateUint64(env, groupId);
    napi_ref reference = NapiHelper::CreateReference(env, value, 0);
    taskGroupManager.AddTask(groupId, reference, taskId);
    ASSERT_NE(reference, nullptr);
    delete task;
    delete group;
}

HWTEST_F(NativeEngineTest, TaskpoolTest023, testing::ext::TestSize.Level0)
{
    TaskGroupManager &taskGroupManager = TaskGroupManager::GetInstance();
    TaskGroup* group = new TaskGroup();
    uint64_t groupId = reinterpret_cast<uint64_t>(group);
    TaskGroup* taskGroup = taskGroupManager.GetTaskGroup(groupId);
    ASSERT_TRUE(taskGroup == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest024, testing::ext::TestSize.Level0)
{
    auto task = new Task();
    task->taskId_ = 99;
    napi_env env = reinterpret_cast<napi_env>(engine_);
    auto worker = static_cast<Worker*>(WorkerConstructor(env));
    NativeEngineTest::StoreTaskId(worker, task->taskId_);
    auto res = NativeEngineTest::FindTaskId(worker, task->taskId_);
    ASSERT_TRUE(res == true);
    NativeEngineTest::RemoveTaskId(worker, task->taskId_);
    res = NativeEngineTest::FindTaskId(worker, task->taskId_);
    ASSERT_TRUE(res == false);
    delete task;
}

HWTEST_F(NativeEngineTest, TaskpoolTest025, testing::ext::TestSize.Level0)
{
    auto task = new Task();
    task->taskId_ = 99;
    napi_env env = reinterpret_cast<napi_env>(engine_);
    auto worker = static_cast<Worker*>(WorkerConstructor(env));
    auto res = NativeEngineTest::FindTaskId(worker, task->taskId_);
    ASSERT_TRUE(res == false);
    NativeEngineTest::RemoveTaskId(worker, task->taskId_);
    res = NativeEngineTest::FindTaskId(worker, task->taskId_);
    ASSERT_TRUE(res == false);
    delete task;
}

HWTEST_F(NativeEngineTest, TaskpoolTest026, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value argv[] = {nullptr};
    std::string funcName = "SeqRunnerConstructor";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), SequenceRunner::SeqRunnerConstructor, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 0, argv, &result);
    ASSERT_NE(result, nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest027, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value argv[] = {nullptr};
    std::string func = "SeqRunnerConstructor";
    napi_value callback = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, func.c_str(), func.size(), SequenceRunner::SeqRunnerConstructor, nullptr, &callback);
    napi_call_function(env, nullptr, callback, 0, argv, &result);
    ASSERT_NE(result, nullptr);

    size_t argc1 = 0;
    napi_value argv1[] = {nullptr};
    func = "Execute";
    callback = nullptr;
    napi_value result1 = nullptr;
    napi_create_function(env, func.c_str(), func.size(), SequenceRunner::Execute, nullptr, &callback);
    napi_call_function(env, nullptr, callback, argc1, argv1, &result1);
    ASSERT_TRUE(result1 == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest028, testing::ext::TestSize.Level0)
{
    TaskGroup taskGroup;
    uint32_t taskId = 10;
    uint32_t index = taskGroup.GetTaskIndex(taskId);
    ASSERT_EQ(index, 0);
}

HWTEST_F(NativeEngineTest, TaskpoolTest029, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    TaskGroup taskGroup;
    taskGroup.NotifyGroupTask(env);
    TaskManager &taskManager = TaskManager::GetInstance();
    uint32_t taskId = 11;
    ASSERT_EQ(taskId, 11);
}

HWTEST_F(NativeEngineTest, TaskpoolTest030, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    TaskGroup taskGroup;
    taskGroup.CancelPendingGroup(env);
    TaskManager &taskManager = TaskManager::GetInstance();
    uint32_t taskId = 12;
    ASSERT_EQ(taskId, 12);
}

HWTEST_F(NativeEngineTest, TaskpoolTest031, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManager = TaskManager::GetInstance();
    Task* task = new Task();
    taskManager.StoreTask(task);
    Task* res = taskManager.GetTask(task->taskId_);
    ASSERT_EQ(task, res);
}

HWTEST_F(NativeEngineTest, TaskpoolTest032, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManager = TaskManager::GetInstance();
    uint32_t taskId = 14;
    taskManager.RemoveTask(taskId);
    ASSERT_EQ(taskId, 14);
}

HWTEST_F(NativeEngineTest, TaskpoolTest033, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManager = TaskManager::GetInstance();
    usleep(50000);
    ResetTaskManager();
    // the task will freed in the taskManager's Destuctor and will not cause memory leak
    Task* task = new Task();
    auto taskId = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task));
    taskManager.EnqueueTaskId(taskId, Priority::HIGH);
    std::pair<uint32_t, Priority> result = taskManager.DequeueTaskId();
    ASSERT_TRUE(result.first == taskId);
    ASSERT_TRUE(result.second == Priority::HIGH);
}

HWTEST_F(NativeEngineTest, TaskpoolTest034, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    Worker* worker = reinterpret_cast<Worker*>(NativeEngineTest::WorkerConstructor(env));
    TaskManager& taskManager = TaskManager::GetInstance();
    taskManager.NotifyWorkerIdle(worker);
    ASSERT_NE(worker, nullptr);
    taskManager.NotifyWorkerCreated(worker);
    ASSERT_NE(worker, nullptr);
    taskManager.NotifyWorkerRunning(worker);
    ASSERT_NE(worker, nullptr);
    taskManager.RestoreWorker(worker);
    ASSERT_NE(worker, nullptr);
    taskManager.RemoveWorker(worker);
    ASSERT_NE(worker, nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest035, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManager = TaskManager::GetInstance();
    uint32_t step = 8;
    taskManager.GetIdleWorkersList(step);
    ASSERT_EQ(step, 8);
}

HWTEST_F(NativeEngineTest, TaskpoolTest036, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManager = TaskManager::GetInstance();
    napi_value res = taskManager.GetThreadInfos(env);
    ASSERT_NE(res, nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest037, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManager = TaskManager::GetInstance();
    uint32_t taskId = 99;
    auto callbackInfo = std::make_shared<CallbackInfo>(env, 1, nullptr);
    taskManager.RegisterCallback(env, taskId, callbackInfo, "TaskpoolTest037");
    auto res = callbackInfo->refCount;
    taskManager.DecreaseSendDataRefCount(env, taskId);
    ASSERT_EQ(res, 1);
}

HWTEST_F(NativeEngineTest, TaskpoolTest038, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManager = TaskManager::GetInstance();
    uint32_t taskId = 100;
    std::weak_ptr<CallbackInfo> wptr;
    {
        auto callbackInfo = std::make_shared<CallbackInfo>(env, 1, nullptr);
        wptr = callbackInfo;
        taskManager.RegisterCallback(env, taskId, callbackInfo, "TaskpoolTest038");
        taskManager.DecreaseSendDataRefCount(env, taskId);
    }
    bool res = wptr.expired();
    ASSERT_EQ(res, true);
}

HWTEST_F(NativeEngineTest, TaskpoolTest039, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManager = TaskManager::GetInstance();
    uint32_t taskId = 17;
    taskManager.IncreaseSendDataRefCount(taskId);
    ASSERT_EQ(taskId, 17);
}

HWTEST_F(NativeEngineTest, TaskpoolTest040, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManager = TaskManager::GetInstance();
    uint32_t taskId = 18;
    taskManager.DecreaseSendDataRefCount(env, taskId);
    ASSERT_EQ(taskId, 18);
}

HWTEST_F(NativeEngineTest, TaskpoolTest041, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManager = TaskManager::GetInstance();
    uint32_t taskId = 19;
    bool res = taskManager.IsDependendByTaskId(taskId);
    ASSERT_EQ(res, false);
}

HWTEST_F(NativeEngineTest, TaskpoolTest042, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManager = TaskManager::GetInstance();
    uint32_t taskId = 20;
    taskManager.NotifyDependencyTaskInfo(taskId);
    ASSERT_EQ(taskId, 20);
}

HWTEST_F(NativeEngineTest, TaskpoolTest043, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManager = TaskManager::GetInstance();
    uint32_t taskId = 21;
    std::set<uint32_t> taskSet;
    taskSet.emplace(1);
    taskSet.emplace(2);
    bool res = taskManager.StoreTaskDependency(taskId, taskSet);
    ASSERT_EQ(res, true);
}

HWTEST_F(NativeEngineTest, TaskpoolTest044, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManager = TaskManager::GetInstance();
    uint32_t taskId = 22;
    uint32_t dependentId = 0;
    bool res = taskManager.RemoveTaskDependency(taskId, dependentId);
    ASSERT_EQ(res, false);
}

HWTEST_F(NativeEngineTest, TaskpoolTest045, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManager = TaskManager::GetInstance();
    uint32_t taskId = 23;
    std::set<uint32_t> dependentIdSet;
    dependentIdSet.emplace(1);
    std::set<uint32_t> idSet;
    idSet.emplace(2);
    bool res = taskManager.CheckCircularDependency(dependentIdSet, idSet, taskId);
    ASSERT_EQ(res, true);
}

HWTEST_F(NativeEngineTest, TaskpoolTest046, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManager = TaskManager::GetInstance();
    uint32_t taskId = 24;
    taskManager.EnqueuePendingTaskInfo(taskId, Priority::NUMBER);
    std::pair<uint32_t, Priority> res = taskManager.DequeuePendingTaskInfo(taskId);
    ASSERT_EQ(res.first, 24);
    ASSERT_EQ(res.second, Priority::NUMBER);
}

HWTEST_F(NativeEngineTest, TaskpoolTest047, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManager = TaskManager::GetInstance();
    uint32_t taskId = 24;
    taskManager.RemovePendingTaskInfo(taskId);
    ASSERT_EQ(taskId, 24);
}

HWTEST_F(NativeEngineTest, TaskpoolTest048, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManager = TaskManager::GetInstance();
    uint32_t taskId = 25;
    std::set<uint32_t> dependTaskIdSet;
    taskManager.StoreDependentTaskInfo(dependTaskIdSet, taskId);
    ASSERT_EQ(taskId, 25);
}

HWTEST_F(NativeEngineTest, TaskpoolTest049, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManager = TaskManager::GetInstance();
    uint32_t taskId = 26;
    uint32_t dependentTaskId = 26;
    taskManager.RemoveDependentTaskInfo(dependentTaskId, taskId);
    ASSERT_EQ(taskId, 26);
    ASSERT_EQ(dependentTaskId, 26);
}

HWTEST_F(NativeEngineTest, TaskpoolTest050, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManager = TaskManager::GetInstance();
    uint32_t taskId = 27;
    uint64_t totalDuration = 25;
    uint64_t cpuDuration = 8;
    taskManager.StoreTaskDuration(taskId, totalDuration, cpuDuration);
    ASSERT_EQ(taskId, 27);
    ASSERT_EQ(totalDuration, 25);
    ASSERT_EQ(cpuDuration, 8);
}

HWTEST_F(NativeEngineTest, TaskpoolTest051, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManager = TaskManager::GetInstance();
    uint32_t taskId = 28;
    std::string str = "testTaskpool";
    taskManager.GetTaskDuration(taskId, str);
    ASSERT_EQ(taskId, 28);
}

HWTEST_F(NativeEngineTest, TaskpoolTest052, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManager = TaskManager::GetInstance();
    uint32_t taskId = 29;
    taskManager.RemoveTaskDuration(taskId);
    ASSERT_EQ(taskId, 29);
}

HWTEST_F(NativeEngineTest, TaskpoolTest053, testing::ext::TestSize.Level0)
{
    TaskGroupManager& taskGroupManager = TaskGroupManager::GetInstance();
    uint64_t groupId = 30;
    TaskGroup* group = taskGroupManager.GetTaskGroup(groupId);
    taskGroupManager.StoreTaskGroup(groupId, group);
    ASSERT_EQ(groupId, 30);
}

HWTEST_F(NativeEngineTest, TaskpoolTest054, testing::ext::TestSize.Level0)
{
    TaskGroupManager& taskGroupManager = TaskGroupManager::GetInstance();
    uint64_t groupId = 31;
    taskGroupManager.RemoveTaskGroup(groupId);
    ASSERT_EQ(groupId, 31);
}

HWTEST_F(NativeEngineTest, TaskpoolTest055, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskGroupManager& taskGroupManager = TaskGroupManager::GetInstance();
    uint64_t groupId = 32;
    taskGroupManager.CancelGroup(env, groupId);
    ASSERT_EQ(groupId, 32);
}

HWTEST_F(NativeEngineTest, TaskpoolTest056, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskGroupManager& taskGroupManager = TaskGroupManager::GetInstance();
    uint32_t taskId = 33;
    TaskGroup* group = taskGroupManager.GetTaskGroup(taskId);
    taskGroupManager.CancelGroupTask(env, taskId, group);
    ASSERT_EQ(taskId, 33);
}

HWTEST_F(NativeEngineTest, TaskpoolTest057, testing::ext::TestSize.Level0)
{
    TaskGroupManager& taskGroupManager = TaskGroupManager::GetInstance();
    uint64_t groupId = 34;
    taskGroupManager.UpdateGroupState(groupId);
    ASSERT_EQ(groupId, 34);
}

HWTEST_F(NativeEngineTest, TaskpoolTest058, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskGroupManager& taskGroupManager = TaskGroupManager::GetInstance();
    SequenceRunnerManager& sequenceRunnerManager = SequenceRunnerManager::GetInstance();
    uint64_t seqRunnerId = 35;
    Task* task = new Task();
    ASSERT_NE(task, nullptr);
    sequenceRunnerManager.AddTaskToSeqRunner(seqRunnerId, task);
    sequenceRunnerManager.TriggerSeqRunner(env, task);
    SequenceRunner sequenceRunner;
    sequenceRunnerManager.StoreSequenceRunner(seqRunnerId, &sequenceRunner);
    NativeEngineTest::RemoveSequenceRunner(seqRunnerId);
    ASSERT_EQ(seqRunnerId, 35);
    SequenceRunner* res = sequenceRunnerManager.GetSeqRunner(seqRunnerId);
    ASSERT_EQ(res, nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest059, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManager = TaskManager::GetInstance();
    taskManager.InitTaskManager(env);
    taskManager.TryTriggerExpand();
    usleep(50000);
    NativeEngineTest::TriggerShrink(env);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest060, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManager = TaskManager::GetInstance();
    ResetTaskManager();
    uint32_t taskId = 36;
    taskManager.EnqueueTaskId(taskId, Priority::LOW);
    ASSERT_EQ(taskId, 36);

    std::pair<uint32_t, Priority> result = taskManager.DequeueTaskId();
    ASSERT_TRUE(result.first == 36);
    ASSERT_TRUE(result.second == Priority::LOW);

    taskId = 37;
    taskManager.EnqueueTaskId(taskId, Priority::IDLE);
    ASSERT_EQ(taskId, 37);

    result = taskManager.DequeueTaskId();
    ASSERT_TRUE(result.first == 37);
    ASSERT_TRUE(result.second == Priority::IDLE);
    result = taskManager.DequeueTaskId();
    ASSERT_TRUE(result.first == 0);
    ASSERT_TRUE(result.second == Priority::LOW);
}

HWTEST_F(NativeEngineTest, TaskpoolTest061, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value argv[] = {nullptr, nullptr};
    std::string funcName = "TaskGroupConstructor";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), TaskGroup::TaskGroupConstructor, nullptr, &cb);

    napi_call_function(env, nullptr, cb, 2, argv, &result);
    ASSERT_TRUE(result == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest062, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value argv[] = {nullptr};
    std::string funcName = "TaskGroupConstructor";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), TaskGroup::TaskGroupConstructor, nullptr, &cb);

    napi_call_function(env, nullptr, cb, 1, argv, &result);
    ASSERT_TRUE(result == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest063, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    TaskGroup taskGroup;
    uint32_t taskId = 10;
    taskGroup.taskIds_.push_back(taskId);
    uint32_t index = taskGroup.GetTaskIndex(taskId);
    ASSERT_EQ(index, 0);
}

HWTEST_F(NativeEngineTest, TaskpoolTest064, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    TaskGroup taskGroup;
    uint32_t taskId = 11;
    taskGroup.taskIds_.push_back(taskId);
    uint32_t index = taskGroup.GetTaskIndex(1);
    ASSERT_EQ(index, 1);
}

HWTEST_F(NativeEngineTest, TaskpoolTest065, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    TaskGroup taskGroup;
    GroupInfo* groupInfo = new GroupInfo();
    taskGroup.pendingGroupInfos_.push_back(groupInfo);
    taskGroup.NotifyGroupTask(env);
    delete groupInfo;
    groupInfo = nullptr;
    ASSERT_TRUE(taskGroup.pendingGroupInfos_.empty());
}

HWTEST_F(NativeEngineTest, TaskpoolTest066, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    TaskGroupManager& taskGroupManager = TaskGroupManager::GetInstance();
    TaskGroup* taskGroup = new TaskGroup();
    uint64_t groupId = reinterpret_cast<uint64_t>(taskGroup);
    taskGroup->groupId_ = groupId;
    taskGroupManager.StoreTaskGroup(groupId, taskGroup);

    GroupInfo* groupInfo = new GroupInfo();
    groupInfo->priority = Priority::DEFAULT;
    taskGroup->pendingGroupInfos_.push_back(groupInfo);
    taskGroup->NotifyGroupTask(env);
    ASSERT_TRUE(taskGroup->pendingGroupInfos_.empty());
}

HWTEST_F(NativeEngineTest, TaskpoolTest067, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    TaskGroupManager& taskGroupManager = TaskGroupManager::GetInstance();
    TaskGroup* taskGroup = new TaskGroup();
    uint64_t groupId = reinterpret_cast<uint64_t>(taskGroup);
    taskGroup->groupId_ = groupId;
    taskGroupManager.StoreTaskGroup(groupId, taskGroup);

    Task* task = new Task();
    task->taskType_ = TaskType::COMMON_TASK;
    task->groupId_ = groupId;
    task->taskId_ = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task));
    napi_reference_ref(env, task->taskRef_, nullptr);
    napi_value thisValue = NapiHelper::CreateObject(env);
    napi_wrap(
        env, thisValue, task,
        [](napi_env environment, void* data, void* hint) {
            auto obj = reinterpret_cast<Task*>(data);
            if (obj != nullptr) {
                delete obj;
            }
        }, nullptr, nullptr);
    napi_ref ref = NapiHelper::CreateReference(env, thisValue, 1);
    taskGroupManager.AddTask(groupId, ref, task->taskId_);

    GroupInfo* groupInfo = new GroupInfo();
    groupInfo->priority = Priority::DEFAULT;
    taskGroup->pendingGroupInfos_.push_back(groupInfo);
    taskGroup->NotifyGroupTask(env);
    ASSERT_TRUE(taskGroup->pendingGroupInfos_.empty());
}

HWTEST_F(NativeEngineTest, TaskpoolTest068, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    TaskGroupManager& taskGroupManager = TaskGroupManager::GetInstance();
    TaskGroup taskGroup;

    GroupInfo* groupInfo = new GroupInfo();
    groupInfo->priority = Priority::DEFAULT;
    taskGroup.pendingGroupInfos_.push_back(groupInfo);
    uint32_t taskId = 68;
    taskGroup.taskIds_.push_back(taskId);
    taskGroup.CancelPendingGroup(env);

    ASSERT_TRUE(taskGroup.pendingGroupInfos_.empty());
}

HWTEST_F(NativeEngineTest, TaskpoolTest069, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    TaskManager& taskManager = TaskManager::GetInstance();
    Task* pointer = new Task();
    auto task = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(pointer));
    auto worker = reinterpret_cast<Worker*>(NativeEngineTest::WorkerConstructor(env));
    taskManager.StoreLongTaskInfo(task, worker);
    auto res = taskManager.GetLongTaskInfo(task);
    ASSERT_TRUE(worker == res);
    taskManager.TerminateTask(task);
    res = taskManager.GetLongTaskInfo(task);
    ASSERT_TRUE(res == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest070, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    TaskManager& taskManager = TaskManager::GetInstance();
    ResetTaskManager();
    auto worker = Worker::WorkerConstructor(env);
    usleep(50000);
    taskManager.NotifyWorkerCreated(worker);
    Task* task = new Task();
    task->isLongTask_ = true;
    taskManager.StoreTask(task);
    uint32_t id = task->taskId_;
    taskManager.EnqueueTaskId(id);
    usleep(50000);
    auto res = taskManager.GetLongTaskInfo(id);
    ASSERT_NE(res, nullptr);
    ResetTaskManager();
}

HWTEST_F(NativeEngineTest, TaskpoolTest071, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    TaskManager& taskManager = TaskManager::GetInstance();
    char buf[4096]; // 4096: buffer length for thread state
    auto worker = reinterpret_cast<Worker*>(NativeEngineTest::WorkerConstructor(env));
    uv_timer_t* handle = new uv_timer_t;
    handle->data = worker;
    auto tid = NativeEngineTest::GetWorkerTid(handle);
    bool res = taskManager.ReadThreadInfo(tid, buf, sizeof(buf));
    ASSERT_TRUE(res == true);
}

HWTEST_F(NativeEngineTest, TaskpoolTest072, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    TaskManager& taskManager = TaskManager::GetInstance();
    Task* task = new Task();
    auto id = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task));
    bool res = taskManager.IsDependendByTaskId(id);
    ASSERT_NE(res, true);
}

HWTEST_F(NativeEngineTest, TaskpoolTest073, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    TaskManager& taskManager = TaskManager::GetInstance();
    Task* task = new Task();
    taskManager.StoreTask(task);
    task = taskManager.GetTask(task->taskId_);
    ASSERT_TRUE(task != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest074, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    TaskManager& taskManager = TaskManager::GetInstance();
    auto res = taskManager.GetThreadInfos(env);
    ASSERT_TRUE(res != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest075, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    TaskManager& taskManager = TaskManager::GetInstance();
    auto res = taskManager.GetTaskInfos(env);
    ASSERT_TRUE(res != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest076, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    TaskManager& taskManager = TaskManager::GetInstance();
    Task* task = new Task();
    auto id = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task));
    taskManager.StoreTaskDuration(id, 0, 0);
    auto totalTime = taskManager.GetTaskDuration(id, TASK_TOTAL_TIME);
    auto cpuTime = taskManager.GetTaskDuration(id, TASK_CPU_TIME);
    ASSERT_TRUE(totalTime == 0);
    ASSERT_TRUE(cpuTime == 0);
    taskManager.StoreTaskDuration(id, 100, 100); // 100: 100 seconds
    totalTime = taskManager.GetTaskDuration(id, TASK_TOTAL_TIME);
    cpuTime = taskManager.GetTaskDuration(id, TASK_CPU_TIME);
    ASSERT_TRUE(totalTime == 100);
    ASSERT_TRUE(cpuTime == 100);
}

HWTEST_F(NativeEngineTest, TaskpoolTest077, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    TaskManager& taskManager = TaskManager::GetInstance();
    Task* task = new Task(env, TaskType::COMMON_TASK, "test");
    taskManager.StoreTask(task);
    auto res = task->name_;
    ASSERT_TRUE(strcmp(res.c_str(), "test") == 0);
}

HWTEST_F(NativeEngineTest, TaskpoolTest078, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value argv[] = {};
    napi_value result = NativeEngineTest::IsConcurrent(env, argv, 0);
    ASSERT_TRUE(result == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest079, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value argv[] = { nullptr };
    napi_value result = NativeEngineTest::IsConcurrent(env, argv, 1);
    ASSERT_TRUE(result == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest080, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value argv[] = { nullptr, nullptr };
    napi_value result = NativeEngineTest::IsConcurrent(env, argv, 2);
    ASSERT_TRUE(result == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest081, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value func = nullptr;
    GetSendableFunction(env, "foo", func);
    napi_value argv[] = { func };
    napi_value result = NativeEngineTest::IsConcurrent(env, argv, 1);
    bool res = true;
    napi_get_value_bool(env, result, &res);
    ASSERT_TRUE(res == false);
}

HWTEST_F(NativeEngineTest, TaskpoolTest082, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value argv[] = { nullptr };
    napi_value result = NativeEngineTest::GetTaskPoolInfo(env, argv, 1);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest083, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value argv[] = { nullptr };
    napi_value result = NativeEngineTest::TerminateTask(env, argv, 1);
    ASSERT_TRUE(result == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest084, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value func = nullptr;
    GetSendableFunction(env, "foo", func);
    napi_value argv[] = { func };
    napi_value result = NativeEngineTest::Execute(env, argv, 1);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest085, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value func = nullptr;
    GetSendableFunction(env, "foo", func);

    size_t delay = 1000;
    napi_value result = nullptr;
    napi_create_uint32(env, delay, &result);

    napi_value argv[] = { result, func };
    std::string funcName = "ExecuteDelayed";
    result = NativeEngineTest::ExecuteDelayed(env, argv, 2);
    ASSERT_TRUE(result == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest086, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    std::string func = "SeqRunnerConstructor";
    napi_value callback = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, func.c_str(), func.size(), SequenceRunner::SeqRunnerConstructor, nullptr, &callback);

    napi_value argv1[] = {nullptr};
    napi_create_uint32(env, 1, &argv1[0]);
    napi_call_function(env, nullptr, callback, 1, argv1, &result);
    ASSERT_NE(result, nullptr);

    napi_value argv2[2] = {nullptr};
    napi_create_uint32(env, 1, &argv2[0]);
    napi_create_string_utf8(env, "seq01", NAPI_AUTO_LENGTH, &argv2[1]);
    result = nullptr;
    napi_call_function(env, nullptr, callback, 2, argv2, &result);
    ASSERT_EQ(result, nullptr);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    napi_value argv3[2] = {nullptr};
    napi_create_string_utf8(env, "seq02", NAPI_AUTO_LENGTH, &argv3[0]);
    napi_create_uint32(env, 1, &argv3[1]);
    result = nullptr;
    napi_call_function(env, nullptr, callback, 2, argv3, &result);
    ASSERT_NE(result, nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest087, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    std::string func = "SeqRunnerConstructor";
    napi_value callback = nullptr;
    napi_create_function(env, func.c_str(), func.size(), SequenceRunner::SeqRunnerConstructor, nullptr, &callback);

    napi_value argv[2] = {nullptr};
    napi_create_string_utf8(env, "seq03", NAPI_AUTO_LENGTH, &argv[0]);
    napi_create_uint32(env, 1, &argv[1]);
    napi_value result = nullptr;
    napi_call_function(env, nullptr, callback, 2, argv, &result);
    ASSERT_NE(result, nullptr);

    func = "Execute";
    napi_value cb = nullptr;
    napi_value res = nullptr;
    napi_create_function(env, func.c_str(), func.size(), SequenceRunner::Execute, nullptr, &cb);

    napi_value argv1[] = {nullptr};
    napi_create_uint32(env, 1, &argv1[0]);
    napi_call_function(env, nullptr, cb, 1, argv1, &res);
    ASSERT_EQ(res, nullptr);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    napi_value thisValue = CreateTaskObject(env);
    napi_value argv2[] = {thisValue};
    res = nullptr;
    napi_call_function(env, nullptr, cb, 1, argv2, &res);
    ASSERT_NE(res, nullptr);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    napi_value napiSeqRunnerId = NapiHelper::GetNameProperty(env, result, "seqRunnerId");
    uint64_t seqId = NapiHelper::GetUint64Value(env, napiSeqRunnerId);
    SequenceRunner seq;
    SequenceRunnerManager::GetInstance().StoreSequenceRunner(seqId, &seq);

    res = nullptr;
    napi_call_function(env, nullptr, cb, 1, argv2, &res);
    ASSERT_NE(res, nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest088, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value argv[] = {nullptr};
    std::string func = "TaskGroupConstructor";
    napi_value callback = nullptr;
    napi_value taskGroupResult = nullptr;
    napi_create_function(env, func.c_str(), func.size(), TaskGroup::TaskGroupConstructor, nullptr, &callback);
    napi_call_function(env, nullptr, callback, 0, argv, &taskGroupResult);

    func = "AddTask";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, func.c_str(), func.size(), TaskGroup::AddTask, nullptr, &cb);
    
    napi_value napiGroupId = NapiHelper::GetNameProperty(env, taskGroupResult, "groupId");
    uint64_t groupId = NapiHelper::GetUint64Value(env, napiGroupId);

    TaskGroupManager& taskGroupManager = TaskGroupManager::GetInstance();
    TaskGroup* group = new TaskGroup();
    group->groupId_ = groupId;
    taskGroupManager.StoreTaskGroup(groupId, group);

    napi_value thisValue = CreateTaskObject(env);
    napi_value argv1[] = {thisValue};
    napi_call_function(env, nullptr, cb, 1, argv1, &result);
    ASSERT_NE(result, nullptr);

    napi_value thisValue2 = CreateTaskObject(env, TaskType::SEQRUNNER_TASK);
    napi_value argv2[] = {thisValue2};
    result = nullptr;
    napi_call_function(env, nullptr, cb, 1, argv2, &result);
    ASSERT_EQ(result, nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest089, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value argv[] = {nullptr};
    std::string func = "TaskGroupConstructor";
    napi_value callback = nullptr;
    napi_value taskGroupResult = nullptr;
    napi_create_function(env, func.c_str(), func.size(), TaskGroup::TaskGroupConstructor, nullptr, &callback);
    napi_call_function(env, nullptr, callback, 0, argv, &taskGroupResult);

    napi_value thisValue = NapiHelper::CreateObject(env);
    napi_value argv1[] = {thisValue};
    func = "AddTask";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, func.c_str(), func.size(), TaskGroup::AddTask, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv1, &result);
    ASSERT_EQ(result, nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest090, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value argv[] = {nullptr};
    std::string funcName = "TaskGroupConstructor";
    napi_value callback = nullptr;
    napi_value taskGroupResult = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), TaskGroup::TaskGroupConstructor, nullptr, &callback);
    napi_call_function(env, nullptr, callback, 0, argv, &taskGroupResult);

    auto func = [](napi_env environment, napi_callback_info info) -> napi_value {
        return nullptr;
    };
    napi_value thisValue = nullptr;
    napi_create_function(env, "testFunc", NAPI_AUTO_LENGTH, func, nullptr, &thisValue);
    napi_value argv1[] = {thisValue};
    funcName = "AddTask";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), TaskGroup::AddTask, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv1, &result);
    ASSERT_EQ(result, nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest091, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value argv[] = {nullptr};
    std::string funcName = "TaskGroupConstructor";
    napi_value callback = nullptr;
    napi_value taskGroupResult = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), TaskGroup::TaskGroupConstructor, nullptr, &callback);
    napi_call_function(env, nullptr, callback, 0, argv, &taskGroupResult);

    napi_value argv1[] = {};
    napi_create_uint32(env, 1, &argv1[0]);
    funcName = "AddTask";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), TaskGroup::AddTask, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv1, &result);
    ASSERT_EQ(result, nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest092, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value thisObj = nullptr;
    napi_create_object(env, &thisObj);

    napi_value taskIdValue = nullptr;
    napi_create_uint32(env, 1, &taskIdValue);
    napi_set_named_property(env, thisObj, "taskId", taskIdValue);

    std::string funcName = "OnReceiveData";
    napi_value cb = nullptr;
    napi_value result = nullptr;

    napi_value listenerFunc = nullptr;
    napi_create_function(env, "listener", NAPI_AUTO_LENGTH, [](napi_env env, napi_callback_info info) -> napi_value {
        napi_value undefined;
        napi_get_undefined(env, &undefined);
        return undefined;
    }, nullptr, &listenerFunc);

    napi_value argv1[2] = { listenerFunc, nullptr };
    napi_create_object(env, &argv1[1]);

    napi_create_function(env, funcName.c_str(), funcName.size(), Task::OnReceiveData, env, &cb);
    napi_call_function(env, thisObj, cb, 2, argv1, &result);

    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ExceptionScope scope(env);

    napi_value argv2[2] = { listenerFunc, nullptr };
    napi_create_object(env, &argv2[1]);

    result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::OnReceiveData, env, &cb);
    napi_call_function(env, thisObj, cb, 2, argv2, &result);
    ASSERT_TRUE(result == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest093, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global = NapiHelper::CreateObject(env);
    ExceptionScope scope(env);
    napi_value func = nullptr;
    GetSendableFunction(env, "foo", func);
    napi_value argv[] = { func };
    std::string funcName = "TaskConstructor";
    napi_value cb = nullptr;
    napi_value task = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::TaskConstructor, nullptr, &cb);
    napi_call_function(env, global, cb, 1, argv, &task);
    auto& taskManager = TaskManager::GetInstance();
    Task* pointer = nullptr;
    napi_unwrap(env, task, reinterpret_cast<void**>(&pointer));
    
    funcName = "OnReceiveData";
    cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::OnReceiveData, nullptr, &cb);
    napi_call_function(env, task, cb, 1, argv, &result);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest094, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value argv[] = { nullptr, nullptr };
    std::string funcName = "TaskConstructor";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::TaskConstructor, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 2, argv, &result);
    ASSERT_TRUE(result == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest095, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    const char* str = "Task";
    napi_value name = nullptr;
    napi_create_string_utf8(env, str, NAPI_AUTO_LENGTH, &name);
    napi_value argv[] = { name, nullptr };
    std::string funcName = "TaskConstructor";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::TaskConstructor, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 2, argv, &result);
    ASSERT_TRUE(result == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest096, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global = NapiHelper::CreateObject(env);
    ExceptionScope scope(env);
    napi_value func = nullptr;
    GetSendableFunction(env, "foo", func);
    napi_value argv[] = { func };
    std::string funcName = "TaskConstructor";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::TaskConstructor, nullptr, &cb);
    napi_call_function(env, global, cb, 1, argv, &result);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest097, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global = NapiHelper::CreateObject(env);
    ExceptionScope scope(env);
    napi_value func = nullptr;
    GetSendableFunction(env, "bar", func);
    napi_value argv[] = { func };
    std::string funcName = "LongTaskConstructor";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::LongTaskConstructor, nullptr, &cb);
    napi_call_function(env, global, cb, 1, argv, &result);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest098, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value argv[] = { nullptr };
    std::string funcName = "SendData";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::SendData, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &result);
    ASSERT_TRUE(result == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest099, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value argv[] = {};
    std::string funcName = "AddDependency";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::AddDependency, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 0, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest100, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global = NapiHelper::CreateObject(env);
    auto task = GeneratorTask(env, global);
    Task* pointer = nullptr;
    napi_unwrap(env, task, reinterpret_cast<void**>(&pointer));
    pointer->isPeriodicTask_ = true;

    napi_value argv[] = { nullptr };
    std::string funcName = "AddDependency";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::AddDependency, nullptr, &cb);
    napi_call_function(env, task, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest101, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global = NapiHelper::CreateObject(env);
    auto task = GeneratorTask(env, global);
    Task* pointer = nullptr;
    napi_unwrap(env, task, reinterpret_cast<void**>(&pointer));
    pointer->taskType_ = TaskType::COMMON_TASK;

    napi_value argv[] = { nullptr };
    std::string funcName = "AddDependency";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::AddDependency, nullptr, &cb);
    napi_call_function(env, task, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest102, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global = NapiHelper::CreateObject(env);
    auto task = GeneratorTask(env, global);
    auto dependentTask = GeneratorTask(env, global);

    napi_value argv[] = { task };
    std::string funcName = "AddDependency";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::AddDependency, nullptr, &cb);
    napi_call_function(env, task, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest103, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global = NapiHelper::CreateObject(env);
    auto task = GeneratorTask(env, global);
    napi_value obj;
    napi_create_object(env, &obj);
    auto dependentTask = GeneratorTask(env, obj);

    napi_value argv[] = { dependentTask };
    std::string funcName = "AddDependency";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::AddDependency, nullptr, &cb);
    napi_call_function(env, task, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest104, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value obj;
    napi_create_object(env, &obj);
    auto task = GeneratorTask(env, obj);
    napi_value argv[] = { task };
    NativeEngineTest::Execute(env, argv, 1);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest105, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value obj;
    napi_create_object(env, &obj);
    auto task = GeneratorTask(env, obj);
    napi_value argv[] = { task };
    NativeEngineTest::TerminateTask(env, argv, 1);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest106, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value argv[] = {};
    NativeEngineTest::TerminateTask(env, argv, 0);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest107, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value obj;
    napi_create_object(env, &obj);
    auto task = GeneratorTask(env, obj);
    Task* pointer = nullptr;
    napi_unwrap(env, task, reinterpret_cast<void**>(&pointer));
    ASSERT_TRUE(pointer != nullptr);
    pointer->isLongTask_ = true;
    napi_value argv[] = { task };
    NativeEngineTest::TerminateTask(env, argv, 1);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest108, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value argv[] = {};
    NativeEngineTest::Execute(env, argv, 0);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest109, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value obj;
    napi_create_object(env, &obj);
    auto task = GeneratorTask(env, obj);
    napi_value priority = nullptr;
    napi_create_uint32(env, 2, &priority); // 2: LOW priority
    napi_value argv[] = { task, priority };
    NativeEngineTest::Execute(env, argv, 2);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest110, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value obj;
    napi_create_object(env, &obj);
    auto task = GeneratorTask(env, obj);
    napi_value priority = nullptr;
    napi_create_uint32(env, 10, &priority); // 10: invalid priority
    napi_value argv[] = { task, priority };
    NativeEngineTest::Execute(env, argv, 2);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest111, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value obj;
    napi_create_object(env, &obj);
    auto group = GeneratorTaskGroup(env, obj);
    napi_value taskObj;
    napi_create_object(env, &taskObj);
    auto task = GeneratorTask(env, taskObj);
    napi_value argv[] = { task };
    std::string funcName = "AddTask";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), TaskGroup::AddTask, nullptr, &cb);
    napi_call_function(env, group, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest112, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value obj;
    napi_create_object(env, &obj);
    auto group = GeneratorTaskGroup(env, obj);
    napi_value func = nullptr;
    GetSendableFunction(env, "foo", func);
    napi_value argv[] = { func };
    std::string funcName = "AddTask";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), TaskGroup::AddTask, nullptr, &cb);
    napi_call_function(env, group, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest113, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value argv[] = {};
    NativeEngineTest::Cancel(env, argv, 0);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest114, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value argv[] = { nullptr };
    NativeEngineTest::Cancel(env, argv, 1);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest115, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value obj = nullptr;
    napi_create_object(env, &obj);
    auto task = GeneratorTask(env, obj);
    napi_value argv[] = { task };
    NativeEngineTest::Cancel(env, argv, 1);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest116, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value obj = nullptr;
    napi_create_object(env, &obj);
    auto task = GeneratorTask(env, obj);
    napi_value argv[] = { task };
    NativeEngineTest::Execute(env, argv, 1);
    NativeEngineTest::Cancel(env, argv, 1);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}


HWTEST_F(NativeEngineTest, TaskpoolTest117, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value obj;
    napi_create_object(env, &obj);
    auto task = GeneratorTask(env, obj);
    napi_value argv[] = { nullptr, nullptr };
    std::string funcName = "SetTransferList";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::SetTransferList, nullptr, &cb);
    napi_call_function(env, task, cb, 2, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest118, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value obj;
    napi_create_object(env, &obj);
    auto task = GeneratorTask(env, obj);
    napi_value array;
    napi_create_array_with_length(env, 1, &array);

    napi_value argv[] = { array };
    std::string funcName = "SetTransferList";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::SetTransferList, nullptr, &cb);
    napi_call_function(env, task, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest119, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value obj;
    napi_create_object(env, &obj);
    auto task = GeneratorTask(env, obj);
    napi_value argv[] = { nullptr, nullptr };
    std::string funcName = "SetCloneList";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::SetCloneList, nullptr, &cb);
    napi_call_function(env, task, cb, 2, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest120, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value obj;
    napi_create_object(env, &obj);
    auto task = GeneratorTask(env, obj);
    napi_value argv[] = { nullptr };
    std::string funcName = "SetCloneList";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::SetCloneList, nullptr, &cb);
    napi_call_function(env, task, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest121, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value obj;
    napi_create_object(env, &obj);
    auto task = GeneratorTask(env, obj);
    napi_value array;
    napi_create_array_with_length(env, 1, &array);
    napi_value argv[] = { array };
    std::string funcName = "SetCloneList";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::SetCloneList, nullptr, &cb);
    napi_call_function(env, task, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest122, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value argv[] = {};
    std::string funcName = "IsCanceled";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::IsCanceled, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 0, argv, &result);
    bool value = true;
    napi_get_value_bool(env, result, &value);
    ASSERT_TRUE(value == false);
}

HWTEST_F(NativeEngineTest, TaskpoolTest123, testing::ext::TestSize.Level0)
{
    auto task = std::make_unique<Task>();
    task->taskId_ = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task.get()));
    task->ioTime_ = 100;
    task->startTime_ = 0;
    task->StoreTaskDuration();
    auto& taskManager = TaskManager::GetInstance();
    auto res = taskManager.GetTaskDuration(task->taskId_, "totalDuration");
    ASSERT_TRUE(res != 0);
    res = taskManager.GetTaskDuration(task->taskId_, "cpuDuration");
    ASSERT_TRUE(res != 0);
    res = taskManager.GetTaskDuration(task->taskId_, "ioDuration");
    ASSERT_TRUE(res == 0);
}

HWTEST_F(NativeEngineTest, TaskpoolTest124, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    auto task = std::make_unique<Task>();
    task->taskId_ = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task.get()));
    task->SetHasDependency(true);
    auto res = task->CanForSequenceRunner(env);
    ASSERT_TRUE(res == false);
    task->TryClearHasDependency();
    res = task->CanForSequenceRunner(env);
    ASSERT_TRUE(res == true);
    task->taskType_ = TaskType::SEQRUNNER_TASK;
    res = task->CanForSequenceRunner(env);
    ASSERT_TRUE(res == false);
    task->taskType_ = TaskType::GROUP_COMMON_TASK;
    res = task->CanForSequenceRunner(env);
    ASSERT_TRUE(res == false);
    task->UpdatePeriodicTask();
    res = task->CanForSequenceRunner(env);
    ASSERT_TRUE(res == false);
}

HWTEST_F(NativeEngineTest, TaskpoolTest125, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    auto task = std::make_unique<Task>();
    task->ThrowNoDependencyError(env);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception!= nullptr);
    auto res = task->CanExecutePeriodically(env);
    ASSERT_TRUE(res == true);
    task->UpdatePeriodicTask();
    res = task->CanExecutePeriodically(env);
    ASSERT_TRUE(res == false);
}

HWTEST_F(NativeEngineTest, TaskpoolTest126, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    auto task = std::make_unique<Task>();
    auto res = task->CanForTaskGroup(env);
    ASSERT_TRUE(res == true);
    task->taskId_ = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task.get()));
    task->SetHasDependency(true);
    res = task->CanForTaskGroup(env);
    ASSERT_TRUE(res == false);
    task->taskType_ = TaskType::SEQRUNNER_TASK;
    res = task->CanForTaskGroup(env);
    ASSERT_TRUE(res == false);
    task->taskType_ = TaskType::GROUP_COMMON_TASK;
    res = task->CanForTaskGroup(env);
    ASSERT_TRUE(res == false);
    task->isLongTask_ = true;
    res = task->CanForTaskGroup(env);
    ASSERT_TRUE(res == false);
    task->UpdatePeriodicTask();
    res = task->CanForTaskGroup(env);
    ASSERT_TRUE(res == false);
}

HWTEST_F(NativeEngineTest, TaskpoolTest127, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    auto task = std::make_unique<Task>();
    auto res = task->CanExecute(env);
    task->taskType_ = TaskType::GROUP_COMMON_TASK;
    res = task->CanExecute(env);
    ASSERT_TRUE(res == false);
    task->taskType_ = TaskType::SEQRUNNER_TASK;
    res = task->CanExecute(env);
    ASSERT_TRUE(res == false);
    task->taskType_ = TaskType::COMMON_TASK;
    res = task->CanExecute(env);
    ASSERT_TRUE(res == true);
    task->SetHasDependency(true);
    res = task->CanExecute(env);
    ASSERT_TRUE(res == false);
    task->TryClearHasDependency();
    task->isLongTask_ = true;
    res = task->CanExecute(env);
    ASSERT_TRUE(res == false);
    task->UpdatePeriodicTask();
    res = task->CanExecute(env);
    ASSERT_TRUE(res == false);
}

HWTEST_F(NativeEngineTest, TaskpoolTest128, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    auto task = std::make_unique<Task>();
    auto res = task->CanExecuteDelayed(env);
    task->taskType_ = TaskType::GROUP_COMMON_TASK;
    res = task->CanExecuteDelayed(env);
    ASSERT_TRUE(res == false);
    task->taskType_ = TaskType::SEQRUNNER_TASK;
    res = task->CanExecuteDelayed(env);
    ASSERT_TRUE(res == false);
    task->taskType_ = TaskType::COMMON_TASK;
    res = task->CanExecuteDelayed(env);
    ASSERT_TRUE(res == true);
    task->isLongTask_ = true;
    res = task->CanExecute(env);
    ASSERT_TRUE(res == false);
    task->UpdatePeriodicTask();
    res = task->CanExecute(env);
    ASSERT_TRUE(res == false);
}

HWTEST_F(NativeEngineTest, TaskpoolTest129, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    SequenceRunner* seqRunner = new SequenceRunner();
    seqRunner->seqRunnerId_ = reinterpret_cast<uint64_t>(seqRunner);
    seqRunner->seqName_ = "seq01";
    void* data = static_cast<void*>(seqRunner);
    NativeEngineTest::SequenceRunnerDestructor(env, data);

    SequenceRunner* seqRunner2 = new SequenceRunner();
    seqRunner2->seqRunnerId_ = reinterpret_cast<uint64_t>(seqRunner2);
    seqRunner2->seqName_ = "seq02";
    seqRunner2->isGlobalRunner_ = true;
    void* data2 = static_cast<void*>(seqRunner2);
    NativeEngineTest::SequenceRunnerDestructor(env, data2);

    ASSERT_TRUE(true);
}

HWTEST_F(NativeEngineTest, TaskpoolTest130, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    TaskGroup* taskGroup = new TaskGroup();
    Task* task = new Task(env, TaskType::COMMON_TASK, "groupTask");
    TaskManager &taskManager = TaskManager::GetInstance();
    taskManager.StoreTask(task);
    uint32_t taskId = task->taskId_;
    taskGroup->taskIds_.push_back(taskId);
    napi_reference_ref(env, task->taskRef_, nullptr);
    
    taskGroup->taskIds_.push_back(2);

    GroupInfo* groupInfo = new GroupInfo();
    taskGroup->currentGroupInfo_ = groupInfo;

    void* data = static_cast<void*>(taskGroup);
    NativeEngineTest::TaskGroupDestructor(env, data);
    ASSERT_TRUE(true);
}

HWTEST_F(NativeEngineTest, TaskpoolTest131, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    Task* task = new Task();
    uint32_t taskId = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task));
    task->taskId_ = taskId;

    napi_value thisValue = NapiHelper::CreateObject(env);
    napi_ref ref = NapiHelper::CreateReference(env, thisValue, 0);
    std::shared_ptr<CallbackInfo> cbInfo = std::make_shared<CallbackInfo>(env, 1, ref);

    Worker* worker = reinterpret_cast<Worker*>(NativeEngineTest::WorkerConstructor(env));
    void* args = nullptr;
    TaskResultInfo* resultInfo = new TaskResultInfo(env, taskId, args);

    TaskManager& taskManager = TaskManager::GetInstance();
    taskManager.RegisterCallback(env, taskId, cbInfo, "TaskpoolTest131");
    taskManager.DecreaseSendDataRefCount(env, taskId);
    ASSERT_TRUE(true);
}


HWTEST_F(NativeEngineTest, TaskpoolTest132, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    Task* task = new Task();
    uint32_t taskId = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task));
    task->taskId_ = taskId;
    napi_value thisValue = NapiHelper::CreateObject(env);
    napi_ref ref = NapiHelper::CreateReference(env, thisValue, 0);
    
    CallbackInfo* cbInfo = new CallbackInfo(env, 1, ref);
    uv_async_t* req = new uv_async_t;
    req->data = cbInfo;

    Worker* worker = reinterpret_cast<Worker*>(NativeEngineTest::WorkerConstructor(env));
    void* args = nullptr;
    TaskResultInfo* resultInfo = new TaskResultInfo(env, taskId, args);
    
    TaskManager &taskManager = TaskManager::GetInstance();
    auto func = [](napi_env environment, napi_callback_info info) -> napi_value {
        return nullptr;
    };
    napi_value funValue = nullptr;
    napi_create_function(env, "testFunc", NAPI_AUTO_LENGTH, func, nullptr, &funValue);
    napi_value argv[] = {funValue};
    napi_ref callbackRef = NapiHelper::CreateReference(env, argv[0], 1);
    std::shared_ptr<CallbackInfo> callbackInfo = std::make_shared<CallbackInfo>(env, 1, callbackRef);
    taskManager.RegisterCallback(env, taskId, callbackInfo, "TaskpoolTest132");
    taskManager.DecreaseSendDataRefCount(env, taskId, task);
    ASSERT_TRUE(true);
}

HWTEST_F(NativeEngineTest, TaskpoolTest133, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    Task* task = new Task();
    uint32_t taskId = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task));
    task->taskId_ = taskId;
    napi_value thisValue = NapiHelper::CreateObject(env);
    napi_ref ref = NapiHelper::CreateReference(env, thisValue, 0);
    
    CallbackInfo* cbInfo = new CallbackInfo(env, 1, ref);
    uv_async_t* req = new uv_async_t;
    req->data = cbInfo;

    Worker* worker = reinterpret_cast<Worker*>(NativeEngineTest::WorkerConstructor(env));
    size_t argc = 1;
    napi_value argv[] = {nullptr};
    napi_create_uint32(env, 1, &argv[0]);
    napi_value argsArray;
    napi_create_array_with_length(env, argc, &argsArray);
    for (size_t i = 0; i < argc; i++) {
        napi_set_element(env, argsArray, i, argv[i]);
    }
    napi_value undefined = NapiHelper::GetUndefinedValue(env);
    void* serializationArgs = nullptr;
    napi_serialize_inner(env, argsArray, undefined, undefined, true, false, &serializationArgs);
    TaskResultInfo* resultInfo = new TaskResultInfo(env, taskId, serializationArgs);
    
    TaskManager &taskManager = TaskManager::GetInstance();
    auto func = [](napi_env environment, napi_callback_info info) -> napi_value {
        return nullptr;
    };
    napi_value funValue = nullptr;
    napi_create_function(env, "testFunc", NAPI_AUTO_LENGTH, func, nullptr, &funValue);
    napi_value argv1[] = {funValue};
    napi_ref callbackRef = NapiHelper::CreateReference(env, argv1[0], 1);
    std::shared_ptr<CallbackInfo> callbackInfo = std::make_shared<CallbackInfo>(env, 1, callbackRef);
    taskManager.RegisterCallback(env, taskId, callbackInfo, "TaskpoolTest133");
    taskManager.DecreaseSendDataRefCount(env, taskId, task);
    ASSERT_TRUE(true);
}

HWTEST_F(NativeEngineTest, TaskpoolTest134, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    Task* task = new Task();
    uint32_t taskId = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task));
    task->taskId_ = taskId;
    napi_value thisValue = NapiHelper::CreateObject(env);
    napi_ref ref = NapiHelper::CreateReference(env, thisValue, 0);
    
    CallbackInfo* cbInfo = new CallbackInfo(env, 1, ref);
    ASSERT_TRUE(cbInfo->refCount == 1);

    Worker* worker = reinterpret_cast<Worker*>(NativeEngineTest::WorkerConstructor(env));
    void* args = nullptr;
    TaskResultInfo* resultInfo = new TaskResultInfo(env, taskId, args);
    ASSERT_TRUE(true);
}

HWTEST_F(NativeEngineTest, TaskpoolTest135, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value argv[] = {};
    napi_value result = NativeEngineTest::TerminateTask(env, argv, 0);
    ASSERT_TRUE(result == nullptr);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    napi_value thisValue = NapiHelper::CreateObject(env);
    napi_value argv1[] = {thisValue};
    result = NativeEngineTest::TerminateTask(env, argv1, 1);
    ASSERT_TRUE(result == nullptr);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    napi_value napiTask = CreateTaskObject(env, TaskType::TASK, ExecuteState::NOT_FOUND, true);
    Task* task = nullptr;
    napi_unwrap(env, napiTask, reinterpret_cast<void**>(&task));
    task->isLongTask_ = true;
    napi_value argv2[] = {napiTask};
    result = NativeEngineTest::TerminateTask(env, argv2, 1);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest136, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value argv[] = {};
    napi_value result = NativeEngineTest::Execute(env, argv, 0);
    ASSERT_TRUE(result == nullptr);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    napi_value thisValue = NapiHelper::CreateObject(env);
    napi_value argv1[] = {thisValue};
    result = NativeEngineTest::Execute(env, argv1, 1);
    ASSERT_TRUE(result == nullptr);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    thisValue = CreateTaskObject(env);
    napi_value argv2[] = {thisValue};
    result = NativeEngineTest::Execute(env, argv2, 1);
    ASSERT_TRUE(result != nullptr);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    napi_create_string_utf8(env, "test", NAPI_AUTO_LENGTH, &argv2[0]);
    result = NativeEngineTest::Execute(env, argv2, 1);
    ASSERT_TRUE(result == nullptr);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    thisValue = CreateTaskObject(env);
    napi_value argv3[2] = {};
    argv3[0] = thisValue;
    napi_create_string_utf8(env, "test", NAPI_AUTO_LENGTH, &argv[1]);
    result = NativeEngineTest::Execute(env, argv3, 2);
    ASSERT_TRUE(result == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest137, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value thisValue = CreateTaskObject(env);
    napi_value argv[2] = {};
    argv[0] = thisValue;
    napi_create_uint32(env, 1, &argv[1]);
    napi_value result = NativeEngineTest::Execute(env, argv, 2);
    ASSERT_TRUE(result != nullptr);

    thisValue = CreateTaskObject(env, TaskType::SEQRUNNER_TASK);
    napi_value argv1[2] = {};
    argv1[0] = thisValue;
    napi_create_uint32(env, 1, &argv[1]);
    result = NativeEngineTest::Execute(env, argv1, 2);
    ASSERT_TRUE(result == nullptr);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    napi_value func = nullptr;
    GetSendableFunction(env, "foo", func);
    napi_value argv2[] = {func};
    result = NativeEngineTest::Execute(env, argv2, 1);
    ASSERT_TRUE(result != nullptr);

    thisValue = CreateTaskObject(env);
    napi_value argv3[2] = {};
    argv3[0] = thisValue;
    napi_create_uint32(env, 5, &argv3[1]);
    result = NativeEngineTest::Execute(env, argv3, 2);
    ASSERT_TRUE(result == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest138, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    size_t delayTime = 1000;
    
    napi_value argv[] = {nullptr};
    napi_value result = NativeEngineTest::ExecuteDelayed(env, argv, 1);
    ASSERT_TRUE(result == nullptr);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    napi_value func = nullptr;
    GetSendableFunction(env, "foo", func);
    napi_value num = nullptr;
    napi_create_uint32(env, delayTime, &num);
    napi_value argv1[] = { func, num };
    result = NativeEngineTest::ExecuteDelayed(env, argv1, 2);
    ASSERT_TRUE(result == nullptr);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    napi_value obj = NapiHelper::CreateObject(env);
    num = nullptr;
    napi_create_uint32(env, -100, &num);
    napi_value argv2[] = { num, obj };
    result = NativeEngineTest::ExecuteDelayed(env, argv2, 2);
    ASSERT_TRUE(result == nullptr);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    num = nullptr;
    napi_create_uint32(env, delayTime, &num);
    napi_value argv3[] = { num, obj, obj };
    result = NativeEngineTest::ExecuteDelayed(env, argv3, 3);
    ASSERT_TRUE(result == nullptr);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    napi_value argv4[] = { num, func};
    result = NativeEngineTest::ExecuteDelayed(env, argv4, 2);
    ASSERT_TRUE(result == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest139, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    size_t delayTime = 1000;
    napi_value result = nullptr;
    napi_value num = nullptr;
    napi_value priority = nullptr;
    napi_value exception = nullptr;

    napi_value obj = NapiHelper::CreateObject(env);
    napi_create_uint32(env, delayTime, &num);
    napi_create_uint32(env, 5, &priority);
    napi_value argv[] = { num, obj, priority };
    result = NativeEngineTest::ExecuteDelayed(env, argv, 3);
    ASSERT_TRUE(result == nullptr);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    obj = CreateNullTaskObject(env);
    napi_create_uint32(env, 1, &priority);
    napi_value argv2[] = { num, obj, priority };
    result = NativeEngineTest::ExecuteDelayed(env, argv2, 3);
    ASSERT_TRUE(result == nullptr);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    obj = CreateTaskObject(env, TaskType::GROUP_COMMON_TASK);
    napi_value argv3[] = { num, obj, priority };
    result = NativeEngineTest::ExecuteDelayed(env, argv3, 3);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(result == nullptr);

    obj = CreateTaskObject(env, TaskType::COMMON_TASK, ExecuteState::CANCELED);
    napi_value argv4[] = { num, obj, priority };
    result = NativeEngineTest::ExecuteDelayed(env, argv4, 3);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest140, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    TaskManager &taskManager = TaskManager::GetInstance();
    napi_value exception = nullptr;
    uv_loop_t* loop = NapiHelper::GetLibUV(env);
    uv_update_time(loop);
    uv_timer_t* handle = new uv_timer_t;
    uv_timer_init(loop, handle);
    TaskMessage* taskMessage = new TaskMessage();
    Task* task = new Task();
    task->env_ = env;
    TaskManager::GetInstance().StoreTask(task);
    taskMessage->taskId = task->taskId_;
    handle->data = taskMessage;
    NativeEngineTest::DelayTask(handle);
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);

    uv_loop_t* loop2 = NapiHelper::GetLibUV(env);
    uv_update_time(loop2);
    uv_timer_t* handle2 = new uv_timer_t;
    uv_timer_init(loop2, handle2);
    TaskMessage* taskMessage2 = new TaskMessage();
    Task* task2 = new Task();
    taskMessage2->taskId = task2->taskId_;
    handle2->data = taskMessage2;
    NativeEngineTest::DelayTask(handle2);
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest141, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value exception = nullptr;
    napi_value thisVar = NapiHelper::CreateObject(env);
    napi_value taskGroupResult = GeneratorTaskGroup(env, thisVar);
    napi_value napiGroupId = NapiHelper::GetNameProperty(env, taskGroupResult, "groupId");
    uint64_t groupId = NapiHelper::GetUint64Value(env, napiGroupId);

    TaskGroupManager& taskGroupManager = TaskGroupManager::GetInstance();
    TaskGroup* group = new TaskGroup();
    group->groupId_ = groupId;
    taskGroupManager.StoreTaskGroup(groupId, group);
    napi_value value = NapiHelper::CreateUint64(env, groupId);
    napi_ref reference = NapiHelper::CreateReference(env, value, 0);
    taskGroupManager.AddTask(groupId, reference, 1);
    napi_value result = NativeEngineTest::ExecuteGroup(env, taskGroupResult);
    ASSERT_TRUE(result == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest142, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value exception = nullptr;
    napi_value thisVar = NapiHelper::CreateObject(env);
    napi_value taskGroupResult = GeneratorTaskGroup(env, thisVar);
    napi_value napiGroupId = NapiHelper::GetNameProperty(env, taskGroupResult, "groupId");
    uint64_t groupId = NapiHelper::GetUint64Value(env, napiGroupId);

    TaskGroupManager& taskGroupManager = TaskGroupManager::GetInstance();
    TaskGroup* group = new TaskGroup();
    group->groupId_ = groupId;
    taskGroupManager.StoreTaskGroup(groupId, group);
    Task* task = new Task();
    task->taskType_ = TaskType::GROUP_COMMON_TASK;
    task->groupId_ = groupId;
    task->taskId_ = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task));
    napi_reference_ref(env, task->taskRef_, nullptr);
    napi_value thisValue = NapiHelper::CreateObject(env);
    napi_wrap(
        env, thisValue, task,
        [](napi_env environment, void* data, void* hint) {
            auto obj = reinterpret_cast<Task*>(data);
            if (obj != nullptr) {
                delete obj;
            }
        }, nullptr, nullptr);
    napi_ref ref = NapiHelper::CreateReference(env, thisValue, 1);
    taskGroupManager.AddTask(groupId, ref, task->taskId_);
    napi_value result = NativeEngineTest::ExecuteGroup(env, taskGroupResult);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest143, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value exception = nullptr;
    napi_value thisVar = NapiHelper::CreateObject(env);
    napi_value taskGroupResult = GeneratorTaskGroup(env, thisVar);
    napi_value napiGroupId = NapiHelper::GetNameProperty(env, taskGroupResult, "groupId");
    uint64_t groupId = NapiHelper::GetUint64Value(env, napiGroupId);

    TaskGroupManager& taskGroupManager = TaskGroupManager::GetInstance();
    TaskGroup* group = taskGroupManager.GetTaskGroup(groupId);
    GroupInfo* groupInfo = new GroupInfo();
    groupInfo->priority = Priority::DEFAULT;
    group->currentGroupInfo_ = groupInfo;
    taskGroupManager.StoreTaskGroup(groupId, group);
    napi_value result = NativeEngineTest::ExecuteGroup(env, taskGroupResult);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest144, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    size_t delayTime = 1000;
    
    napi_value argv[] = {nullptr};
    napi_value result = NativeEngineTest::ExecutePeriodically(env, argv, 1);
    ASSERT_TRUE(result == nullptr);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    napi_value func = nullptr;
    GetSendableFunction(env, "foo", func);
    napi_value num = nullptr;
    napi_create_uint32(env, delayTime, &num);
    napi_value argv1[] = { func, num };
    result = NativeEngineTest::ExecutePeriodically(env, argv1, 2);
    ASSERT_TRUE(result == nullptr);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    napi_value obj = NapiHelper::CreateObject(env);
    num = nullptr;
    napi_create_uint32(env, -100, &num);
    napi_value argv2[] = { num, obj };
    result = NativeEngineTest::ExecutePeriodically(env, argv2, 2);
    ASSERT_TRUE(result == nullptr);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    num = nullptr;
    napi_create_uint32(env, delayTime, &num);
    napi_value argv3[] = { num, obj, obj };
    result = NativeEngineTest::ExecutePeriodically(env, argv3, 3);
    ASSERT_TRUE(result == nullptr);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    napi_value argv4[] = { num, num };
    result = NativeEngineTest::ExecutePeriodically(env, argv4, 2);
    ASSERT_TRUE(result == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest145, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    size_t delayTime = 1000;
    napi_value result = nullptr;
    napi_value num = nullptr;
    napi_value priority = nullptr;
    napi_value exception = nullptr;

    napi_value obj = NapiHelper::CreateObject(env);
    napi_create_uint32(env, delayTime, &num);
    napi_create_uint32(env, 5, &priority);
    napi_value argv[] = { num, obj, priority };
    result = NativeEngineTest::ExecutePeriodically(env, argv, 3);
    ASSERT_TRUE(result == nullptr);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    obj = CreateNullTaskObject(env);
    napi_create_uint32(env, 1, &priority);
    napi_value argv2[] = { num, obj, priority };
    result = NativeEngineTest::ExecutePeriodically(env, argv2, 3);
    ASSERT_TRUE(result == nullptr);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    obj = CreateTaskObject(env, TaskType::GROUP_COMMON_TASK);
    napi_value argv3[] = { num, obj, priority };
    result = NativeEngineTest::ExecutePeriodically(env, argv3, 3);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(result == nullptr);

    obj = CreateTaskObject(env, TaskType::TASK);
    napi_value argv4[] = { num, obj, priority };
    result = NativeEngineTest::ExecutePeriodically(env, argv4, 3);
    ASSERT_TRUE(result == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest146, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value global = NapiHelper::CreateObject(env);
    auto task = GeneratorTask(env, global);
    size_t delayTime = 1000;
    napi_value result = nullptr;
    napi_value num = nullptr;
    napi_value priority = nullptr;
    napi_create_uint32(env, delayTime, &num);
    napi_create_uint32(env, 1, &priority);
    napi_value argv[] = { num, global, priority };
    result = NativeEngineTest::ExecutePeriodically(env, argv, 3);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest147, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    uv_timer_t* handle = new uv_timer_t;

    handle->data = nullptr;
    NativeEngineTest::PeriodicTaskCallback(handle);
    ASSERT_TRUE(true);
    Task* task = new Task();
    handle->data = task;
    NativeEngineTest::PeriodicTaskCallback(handle);
    ASSERT_TRUE(true);

    task->isPeriodicTask_ = true;
    handle->data = task;
    NativeEngineTest::PeriodicTaskCallback(handle);
    ASSERT_TRUE(true);
    
    task->taskState_ = ExecuteState::CANCELED;
    task->env_ = env;
    napi_value obj = NapiHelper::CreateObject(env);
    task->taskRef_ = NapiHelper::CreateReference(env, obj, 1);
    uv_loop_t* loop = NapiHelper::GetLibUV(env);
    task->timer_ = new uv_timer_t;
    uv_timer_init(loop, task->timer_);
    handle->data = task;
    NativeEngineTest::PeriodicTaskCallback(handle);
    ASSERT_TRUE(true);
}

HWTEST_F(NativeEngineTest, TaskpoolTest148, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    uv_timer_t* handle = new uv_timer_t;

    Task* task = new Task();
    uint32_t taskId = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task));
    task->taskId_ = taskId;
    task->taskType_ = TaskType::GROUP_COMMON_TASK;
    napi_value num = nullptr;
    napi_create_uint32(env, 1, &num);
    napi_ref callbackRef = Helper::NapiHelper::CreateReference(env, num, 1);
    task->onExecutionFailedCallBackInfo_ = new ListenerCallBackInfo(env, callbackRef, nullptr);
    handle->data = task;
    napi_value res = nullptr;
    NativeEngineTest::UpdateGroupInfoByResult(env, handle, res, true);
    ASSERT_TRUE(true);

    TaskGroupManager& taskGroupManager = TaskGroupManager::GetInstance();
    TaskGroup* group = new TaskGroup();
    uint64_t groupId = reinterpret_cast<uint64_t>(group);
    group->groupId_ = groupId;
    task->groupId_ = groupId;
    taskGroupManager.StoreTaskGroup(groupId, group);
    NativeEngineTest::UpdateGroupInfoByResult(env, handle, res, true);
    ASSERT_TRUE(true);

    group->taskIds_.push_back(taskId);
    GroupInfo* groupInfo = new GroupInfo();
    napi_value resArr;
    napi_create_array_with_length(env, group->taskIds_.size(), &resArr);
    napi_ref arrRef = NapiHelper::CreateReference(env, resArr, 1);
    groupInfo->resArr = arrRef;
    NapiHelper::CreatePromise(env, &groupInfo->deferred);
    group->currentGroupInfo_ = groupInfo;
    taskGroupManager.StoreTaskGroup(groupId, group);
    NativeEngineTest::UpdateGroupInfoByResult(env, handle, res, false);
    ASSERT_TRUE(true);
}

HWTEST_F(NativeEngineTest, TaskpoolTest149, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    uv_timer_t* handle = new uv_timer_t;
    TaskGroupManager& taskGroupManager = TaskGroupManager::GetInstance();
    TaskManager& taskManager = TaskManager::GetInstance();
    
    TaskGroup* group = new TaskGroup();
    uint64_t groupId = reinterpret_cast<uint64_t>(group);
    group->groupId_ = groupId;
    Task* task = new Task();
    taskManager.StoreTask(task);
    uint32_t taskId = task->taskId_;
    task->groupId_ = groupId;
    napi_value num = nullptr;
    napi_create_uint32(env, 1, &num);
    napi_ref callbackRef = Helper::NapiHelper::CreateReference(env, num, 1);
    task->onExecutionSucceededCallBackInfo_ = new ListenerCallBackInfo(env, callbackRef, nullptr);
    group->taskIds_.push_back(taskId);
    handle->data = task;

    GroupInfo* groupInfo = new GroupInfo();
    groupInfo->priority = Priority::DEFAULT;
    napi_value resArr;
    napi_create_array_with_length(env, group->taskIds_.size(), &resArr);
    napi_ref arrRef = NapiHelper::CreateReference(env, resArr, 1);
    groupInfo->resArr = arrRef;
    NapiHelper::CreatePromise(env, &groupInfo->deferred);
    group->currentGroupInfo_ = groupInfo;
    
    taskGroupManager.StoreTaskGroup(groupId, group);
    napi_value value = NapiHelper::CreateUint64(env, groupId);
    napi_ref reference = NapiHelper::CreateReference(env, value, 0);
    taskGroupManager.AddTask(groupId, reference, taskId);
    
    napi_value res = nullptr;
    napi_create_uint32(env, 1, &res);
    NativeEngineTest::UpdateGroupInfoByResult(env, handle, res, true);
    ASSERT_TRUE(true);
}

HWTEST_F(NativeEngineTest, TaskpoolTest150, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    NativeEngineTest::TryTriggerExpand();
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest150_1, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    NativeEngineTest::CheckForBlockedWorkers(env);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest150_2, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    NativeEngineTest::NotifyShrink(env);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest150_3, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    NativeEngineTest::TryExpand(env);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest150_4, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    NativeEngineTest::CancelTask(env);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest150_5, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    NativeEngineTest::NotifyWorkerIdle(env);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest150_6, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    NativeEngineTest::EnqueueTaskId(env);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest150_7, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    NativeEngineTest::GetTaskByPriority(env);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest150_8, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    NativeEngineTest::RestoreWorker(env);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest151, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value argv[] = {nullptr};
    std::string func = "TaskGroupConstructor";
    napi_value callback = nullptr;
    napi_value taskGroupResult = nullptr;
    napi_create_function(env, func.c_str(), func.size(), TaskGroup::TaskGroupConstructor, nullptr, &callback);
    napi_call_function(env, nullptr, callback, 0, argv, &taskGroupResult);

    func = "AddTask";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    
    napi_create_function(env, func.c_str(), func.size(), TaskGroup::AddTask, nullptr, &cb);
    napi_value napiGroupId = NapiHelper::GetNameProperty(env, taskGroupResult, "groupId");
    uint64_t groupId = NapiHelper::GetUint64Value(env, napiGroupId);
    TaskGroupManager& taskGroupManager = TaskGroupManager::GetInstance();
    TaskGroup* group = new TaskGroup();
    group->groupId_ = groupId;
    group->groupState_ = ExecuteState::RUNNING;
    taskGroupManager.StoreTaskGroup(groupId, group);
    napi_value argv1[] = {taskGroupResult};
    napi_call_function(env, nullptr, cb, 1, argv1, &result);
    ASSERT_TRUE(result == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest152, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    TaskGroupManager& taskGroupManager = TaskGroupManager::GetInstance();
    TaskGroup* taskGroup = new TaskGroup();
    uint64_t groupId = reinterpret_cast<uint64_t>(taskGroup);
    taskGroup->groupId_ = groupId;
    taskGroupManager.StoreTaskGroup(groupId, taskGroup);

    Task* task = new Task();
    task->taskType_ = TaskType::COMMON_TASK;
    task->groupId_ = groupId;
    task->taskId_ = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task));
    napi_reference_ref(env, task->taskRef_, nullptr);
    napi_value thisValue = NapiHelper::CreateObject(env);
    napi_wrap(
        env, thisValue, task,
        [](napi_env environment, void* data, void* hint) {
            auto obj = reinterpret_cast<Task*>(data);
            if (obj != nullptr) {
                delete obj;
            }
        }, nullptr, nullptr);
    napi_ref ref = NapiHelper::CreateReference(env, thisValue, 1);
    taskGroupManager.AddTask(groupId, nullptr, task->taskId_);

    GroupInfo* groupInfo = new GroupInfo();
    groupInfo->priority = Priority::DEFAULT;
    taskGroup->pendingGroupInfos_.push_back(groupInfo);
    taskGroup->NotifyGroupTask(env);
    ASSERT_TRUE(taskGroup->pendingGroupInfos_.empty());
}

HWTEST_F(NativeEngineTest, TaskpoolTest153, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    TaskGroupManager& taskGroupManager = TaskGroupManager::GetInstance();
    TaskGroup* taskGroup = new TaskGroup();
    uint64_t groupId = reinterpret_cast<uint64_t>(taskGroup);
    taskGroup->groupId_ = groupId;
    taskGroupManager.StoreTaskGroup(groupId, taskGroup);

    Task* task = new Task();
    task->taskType_ = TaskType::GROUP_COMMON_TASK;
    task->groupId_ = groupId;
    task->taskId_ = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task));
    napi_reference_ref(env, task->taskRef_, nullptr);
    napi_value thisValue = NapiHelper::CreateObject(env);
    napi_wrap(
        env, thisValue, task,
        [](napi_env environment, void* data, void* hint) {
            auto obj = reinterpret_cast<Task*>(data);
            if (obj != nullptr) {
                delete obj;
            }
        }, nullptr, nullptr);
    napi_ref ref = NapiHelper::CreateReference(env, thisValue, 1);
    taskGroupManager.AddTask(groupId, ref, task->taskId_);

    GroupInfo* groupInfo = new GroupInfo();
    groupInfo->priority = Priority::DEFAULT;
    taskGroup->pendingGroupInfos_.push_back(groupInfo);
    taskGroup->NotifyGroupTask(env);
    ASSERT_TRUE(taskGroup->pendingGroupInfos_.empty());
}

HWTEST_F(NativeEngineTest, TaskpoolTest154, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    Task* task = new Task();
    task->taskType_ = TaskType::GROUP_COMMON_TASK;
    task->taskId_ = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task));
    napi_value args[2] = {nullptr};
    napi_create_string_utf8(env, "generate", NAPI_AUTO_LENGTH, &args[0]);
    napi_value obj;
    napi_create_object(env, &obj);
    args[1] = obj;
    task = task->GenerateFunctionTask(env, nullptr, args, 2, TaskType::GROUP_FUNCTION_TASK);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest155, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    Task* task = new Task();
    napi_value result = CreateTaskObject(env);
    std::string func = "func";
    std::string name = "Task";
    napi_value funcValue = nullptr;
    napi_value nameValue = nullptr;
    napi_create_string_utf8(env, name.c_str(), name.length(), &nameValue);
    napi_create_string_utf8(env, func.c_str(), func.length(), &funcValue);
    napi_value undefined = nullptr;
    napi_get_undefined(env, &undefined);
    napi_set_named_property(env, result, ARGUMENTS_STR, nameValue);
    
    napi_set_named_property(env, result, FUNCTION_STR, funcValue);
    napi_set_named_property(env, result, NAME, nameValue);
    napi_value trueVal = NapiHelper::CreateBooleanValue(env, true);
    napi_set_named_property(env, result, DEFAULT_TRANSFER_STR, trueVal);
    napi_set_named_property(env, result, DEFAULT_CLONE_SENDABLE_STR, trueVal);

    napi_set_named_property(env, result, TRANSFERLIST_STR, nameValue);
    napi_set_named_property(env, result, TRANSFERLIST_STR, nameValue);
    TaskInfo* info = task->GetTaskInfo(env, result, Priority::DEFAULT);
    ASSERT_TRUE(info == nullptr);
    delete task;
}

HWTEST_F(NativeEngineTest, TaskpoolTest156, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global = NapiHelper::GetGlobalObject(env);
    napi_value thisValue = CreateTaskObject(env);
    Task* task = nullptr;
    napi_unwrap(env, thisValue, reinterpret_cast<void**>(&task));
    napi_value undefined = NapiHelper::GetUndefinedValue(env);
    napi_status status = napi_set_named_property(env, thisValue, CLONE_LIST_STR, undefined);
    napi_value array;
    napi_create_array_with_length(env, 1, &array);

    napi_value argv[] = { array };
    std::string funcName = "SetTransferList";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::SetTransferList, nullptr, &cb);
    napi_call_function(env, thisValue, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest157, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global = NapiHelper::GetGlobalObject(env);
    napi_value thisValue = CreateTaskObject(env);
    Task* task = nullptr;
    napi_unwrap(env, thisValue, reinterpret_cast<void**>(&task));

    napi_value array;
    napi_create_array_with_length(env, 1, &array);

    std::string funcName = "SetTransferList";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::SetTransferList, nullptr, &cb);
    napi_call_function(env, thisValue, cb, 0, nullptr, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest158, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global = NapiHelper::GetGlobalObject(env);
    napi_value thisValue = CreateTaskObject(env);
    Task* task = nullptr;
    napi_unwrap(env, thisValue, reinterpret_cast<void**>(&task));

    napi_value argv[1] = { nullptr };
    std::string funcName = "SetTransferList";
    napi_create_string_utf8(env, funcName.c_str(), funcName.size(), &argv[0]);
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::SetTransferList, nullptr, &cb);
    napi_call_function(env, thisValue, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest159, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global = NapiHelper::GetGlobalObject(env);
    napi_value thisValue = CreateTaskObject(env);
    Task* task = nullptr;
    napi_unwrap(env, thisValue, reinterpret_cast<void**>(&task));
    napi_value array;
    napi_create_array_with_length(env, 0, &array);

    napi_value argv[] = { array };
    std::string funcName = "SetTransferList";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::SetTransferList, nullptr, &cb);
    napi_call_function(env, thisValue, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest160, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global = NapiHelper::GetGlobalObject(env);
    napi_value thisValue = CreateTaskObject(env);
    Task* task = nullptr;
    napi_unwrap(env, thisValue, reinterpret_cast<void**>(&task));
    napi_value undefined = NapiHelper::GetUndefinedValue(env);
    napi_status status = napi_set_named_property(env, thisValue, TRANSFERLIST_STR, undefined);
    ASSERT_TRUE(status == napi_ok);
    napi_value argv[] = { nullptr, nullptr };
    std::string funcName = "SetCloneList";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::SetCloneList, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 2, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest161, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global = NapiHelper::GetGlobalObject(env);
    napi_value thisValue = CreateTaskObject(env);
    Task* task = nullptr;
    napi_unwrap(env, thisValue, reinterpret_cast<void**>(&task));

    napi_value array;
    napi_create_array_with_length(env, 0, &array);
    napi_value argv[] = { array };
    std::string funcName = "SetCloneList";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::SetCloneList, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest162, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global = NapiHelper::GetGlobalObject(env);
    napi_value thisValue = CreateTaskObject(env);
    Task* task = nullptr;
    napi_unwrap(env, thisValue, reinterpret_cast<void**>(&task));

    napi_value array;
    napi_create_array_with_length(env, 0, &array);
    napi_value argv[] = { array };
    std::string funcName = "SetCloneList";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::SetCloneList, nullptr, &cb);
    napi_call_function(env, thisValue, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest163, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global = NapiHelper::GetGlobalObject(env);
    napi_value thisValue = CreateTaskObject(env);
    Task* task = nullptr;
    napi_unwrap(env, thisValue, reinterpret_cast<void**>(&task));

    napi_value array;
    napi_create_array_with_length(env, 0, &array);
    napi_value argv[] = { array };
    std::string funcName = "SetCloneList";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::SetCloneList, nullptr, &cb);
    napi_call_function(env, thisValue, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest164, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_env taskEnv = nullptr;
    napi_create_runtime(env, &taskEnv);
    NativeEngine* taskEngine = reinterpret_cast<NativeEngine*>(taskEnv);
    taskEngine->MarkTaskPoolThread();
    napi_value global = NapiHelper::GetGlobalObject(env);
    napi_value thisValue = CreateTaskObject(taskEnv);

    std::string funcName = "IsCanceled";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(taskEnv, funcName.c_str(), funcName.size(), Task::IsCanceled, nullptr, &cb);
    napi_call_function(taskEnv, thisValue, cb, 0, nullptr, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(taskEnv, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest165, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_env taskEnv = nullptr;
    napi_create_runtime(env, &taskEnv);
    NativeEngine* taskEngine = reinterpret_cast<NativeEngine*>(taskEnv);
    taskEngine->MarkTaskPoolThread();
    napi_value global = NapiHelper::GetGlobalObject(env);
    napi_value thisValue = CreateTaskObject(taskEnv);
    std::string funcName = "IsCanceled";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(taskEnv, funcName.c_str(), funcName.size(), Task::IsCanceled, nullptr, &cb);
    napi_call_function(taskEnv, thisValue, cb, 0, nullptr, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(taskEnv, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest166, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global = NapiHelper::GetGlobalObject(env);
    napi_value argv[1] = { nullptr };
    napi_value undefined = NapiHelper::GetUndefinedValue(env);
    argv[0] = undefined;
    std::string funcName = "OnReceiveData";
    napi_value cb = nullptr;
    napi_value result = nullptr;

    napi_create_function(env, funcName.c_str(), funcName.size(), Task::OnReceiveData, nullptr, &cb);
    napi_call_function(env, global, cb, 1, argv, &result);

    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest167, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_env taskEnv = nullptr;
    napi_create_runtime(env, &taskEnv);
    NativeEngine* taskEngine = reinterpret_cast<NativeEngine*>(taskEnv);
    taskEngine->MarkTaskPoolThread();
    napi_value global = NapiHelper::GetGlobalObject(env);

    napi_value argv[] = { nullptr };
    std::string funcName = "SendData";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(taskEnv, funcName.c_str(), funcName.size(), Task::SendData, nullptr, &cb);
    napi_call_function(taskEnv, global, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest168, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_env taskEnv = nullptr;
    napi_create_runtime(env, &taskEnv);
    NativeEngine* taskEngine = reinterpret_cast<NativeEngine*>(taskEnv);
    taskEngine->MarkTaskPoolThread();
    napi_value global = NapiHelper::GetGlobalObject(env);
    napi_value argv[] = { nullptr };
    std::string funcName = "SendData";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(taskEnv, funcName.c_str(), funcName.size(), Task::SendData, nullptr, &cb);
    napi_call_function(taskEnv, global, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest169, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global = NapiHelper::CreateObject(env);
    napi_value thisValue = CreateTaskObject(env);

    napi_value dependentTask = CreateTaskObject(env);
    napi_value argv[] = { dependentTask };
    std::string funcName = "AddDependency";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::AddDependency, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest170, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global = NapiHelper::CreateObject(env);
    napi_value thisValue = CreateTaskObject(env);
    Task* task = nullptr;
    napi_unwrap(env, thisValue, reinterpret_cast<void**>(&task));
    ASSERT_TRUE(task != nullptr);
    
    napi_value dependentTask = CreateTaskObject(env);
    napi_value argv[] = { dependentTask };
    std::string funcName = "AddDependency";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::AddDependency, nullptr, &cb);
    task->isPeriodicTask_ = false;
    task->taskType_ = TaskType::GROUP_COMMON_TASK;
    napi_call_function(env, thisValue, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest171, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global = NapiHelper::CreateObject(env);
    napi_value thisValue = CreateTaskObject(env);
    Task* task = nullptr;
    napi_unwrap(env, thisValue, reinterpret_cast<void**>(&task));
    
    napi_value dependentTask = CreateTaskObject(env);
    napi_value argv[] = { dependentTask };
    std::string funcName = "AddDependency";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::AddDependency, nullptr, &cb);
    task->taskType_ = TaskType::TASK;
    task->isPeriodicTask_ = false;
    napi_call_function(env, thisValue, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest172, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global = NapiHelper::CreateObject(env);
    napi_value thisValue = CreateTaskObject(env);
    Task* task = nullptr;
    napi_unwrap(env, thisValue, reinterpret_cast<void**>(&task));
    
    napi_value argv[] = { nullptr };
    std::string funcName = "AddDependency";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::AddDependency, nullptr, &cb);
    task->taskType_ = TaskType::TASK;
    task->isPeriodicTask_ = false;
    napi_call_function(env, thisValue, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest173, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global = NapiHelper::CreateObject(env);
    napi_value thisValue = CreateTaskObject(env);
    Task* task = nullptr;
    napi_unwrap(env, thisValue, reinterpret_cast<void**>(&task));
    
    napi_value dependentTask = CreateTaskObject(env);
    Task* task1 = nullptr;
    napi_unwrap(env, dependentTask, reinterpret_cast<void**>(&task1));
    task1->isPeriodicTask_ = true;
    napi_value argv[] = { dependentTask };
    std::string funcName = "AddDependency";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::AddDependency, nullptr, &cb);
    task->taskType_ = TaskType::TASK;
    task->isPeriodicTask_ = false;
    napi_call_function(env, thisValue, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest174, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global = NapiHelper::CreateObject(env);
    napi_value thisValue = CreateTaskObject(env);
    Task* task = nullptr;
    napi_unwrap(env, thisValue, reinterpret_cast<void**>(&task));
    
    napi_value dependentTask = CreateTaskObject(env);
    Task* task1 = nullptr;
    napi_unwrap(env, dependentTask, reinterpret_cast<void**>(&task1));
    task1->taskType_ = TaskType::COMMON_TASK;
    napi_value argv[] = { dependentTask };
    std::string funcName = "AddDependency";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::AddDependency, nullptr, &cb);
    task->taskType_ = TaskType::TASK;
    task1->taskType_ = TaskType::COMMON_TASK;
    task->isPeriodicTask_ = false;
    task1->isPeriodicTask_ = false;
    napi_call_function(env, thisValue, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);

    task1->taskType_ = TaskType::SEQRUNNER_TASK;
    napi_call_function(env, thisValue, cb, 1, argv, &result);
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);

    task1->taskType_ = TaskType::GROUP_COMMON_TASK;
    napi_call_function(env, thisValue, cb, 1, argv, &result);
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);

    task1->taskType_ = TaskType::ASYNCRUNNER_TASK;
    napi_call_function(env, thisValue, cb, 1, argv, &result);
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest175, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global = NapiHelper::CreateObject(env);
    napi_value thisValue = CreateTaskObject(env);
    Task* task = nullptr;
    napi_unwrap(env, thisValue, reinterpret_cast<void**>(&task));
    
    napi_value dependentTask = CreateTaskObject(env);
    napi_value argv[] = { dependentTask };
    std::string funcName = "AddDependency";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::AddDependency, nullptr, &cb);
    task->isPeriodicTask_ = false;
    task->taskType_ = TaskType::SEQRUNNER_TASK;
    napi_call_function(env, thisValue, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);

    task->taskType_ = TaskType::COMMON_TASK;
    napi_call_function(env, thisValue, cb, 1, argv, &result);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);

    task->taskType_ = TaskType::ASYNCRUNNER_TASK;
    napi_call_function(env, thisValue, cb, 1, argv, &result);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest176, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string funcName = "RemoveDependency";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::RemoveDependency, nullptr, &cb);

    napi_call_function(env, nullptr, cb, 0, nullptr, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest177, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value thisValue = CreateTaskObject(env);
    Task* task = nullptr;
    napi_unwrap(env, thisValue, reinterpret_cast<void**>(&task));
    
    napi_value dependentTask = CreateTaskObject(env);
    napi_value argv[] = { dependentTask };
    std::string funcName = "RemoveDependency";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::RemoveDependency, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest178, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value thisValue = CreateTaskObject(env);
    Task* task = nullptr;
    napi_unwrap(env, thisValue, reinterpret_cast<void**>(&task));
    
    napi_value undefined = NapiHelper::GetUndefinedValue(env);
    napi_value dependentTask = CreateTaskObject(env);
    napi_set_named_property(env, dependentTask, TASKID_STR, undefined);
    napi_value argv[] = { dependentTask };
    std::string funcName = "RemoveDependency";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::RemoveDependency, nullptr, &cb);
    task->hasDependency_ = false;
    napi_call_function(env, thisValue, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);

    task->hasDependency_ = true;
    task->isPeriodicTask_ = true;
    napi_call_function(env, thisValue, cb, 1, argv, &result);
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);

    task->isPeriodicTask_ = false;
    task->taskType_ = TaskType::COMMON_TASK;
    napi_call_function(env, thisValue, cb, 1, argv, &result);
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);

    task->taskType_ = TaskType::TASK;
    napi_call_function(env, thisValue, cb, 1, argv, &result);
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest179, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value thisValue = CreateTaskObject(env);
    Task* task = nullptr;
    napi_unwrap(env, thisValue, reinterpret_cast<void**>(&task));

    napi_value dependentTask = CreateTaskObject(env);
    Task* task1 = nullptr;
    napi_unwrap(env, dependentTask, reinterpret_cast<void**>(&task1));
    napi_value argv[] = { dependentTask };
    std::string funcName = "RemoveDependency";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::RemoveDependency, nullptr, &cb);
    task->hasDependency_ = true;
    task->isPeriodicTask_ = false;
    task->taskType_ = TaskType::TASK;
    task1->hasDependency_ = false;
    napi_call_function(env, thisValue, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);

    task1->hasDependency_ = true;
    task1->isPeriodicTask_ = true;
    napi_call_function(env, thisValue, cb, 1, argv, &result);
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);

    task1->isPeriodicTask_ = false;
    task1->taskType_ = TaskType::COMMON_TASK;
    napi_call_function(env, thisValue, cb, 1, argv, &result);
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);

    task1->taskType_ = TaskType::TASK;
    napi_call_function(env, thisValue, cb, 1, argv, &result);
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);

    task->taskType_ = TaskType::ASYNCRUNNER_TASK;
    napi_call_function(env, thisValue, cb, 1, argv, &result);
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest180, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value thisValue = CreateTaskObject(env);
    Task* task = nullptr;
    napi_unwrap(env, thisValue, reinterpret_cast<void**>(&task));

    napi_value dependentTask = CreateTaskObject(env);
    Task* task1 = nullptr;
    napi_unwrap(env, dependentTask, reinterpret_cast<void**>(&task1));
    napi_value argv[] = { dependentTask };
    std::string funcName = "RemoveDependency";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::RemoveDependency, nullptr, &cb);
    task->hasDependency_ = true;
    task->isPeriodicTask_ = false;
    task->taskType_ = TaskType::TASK;
    task1->hasDependency_ = true;
    task1->isPeriodicTask_ = false;
    task1->taskType_ = TaskType::TASK;
    task1->hasDependency_ = false;
    NativeEngineTest::StoreDependentId(task->taskId_, task1->taskId_);
    napi_call_function(env, thisValue, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest181, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value func = nullptr;
    GetSendableFunction(env, "foo", func);
    napi_ref callbackRef = Helper::NapiHelper::CreateReference(env, func, 1);
    ListenerCallBackInfo* cbInfo = new ListenerCallBackInfo(env, callbackRef, nullptr);

    uv_async_t* req = new uv_async_t;
    req->data = cbInfo;
    Task::StartExecutionCallback(req);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest182, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ListenerCallBackInfo* cbInfo = new ListenerCallBackInfo(env, nullptr, nullptr);

    uv_async_t* req = new uv_async_t;
    req->data = cbInfo;
    Task::StartExecutionCallback(req);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest183, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ListenerCallBackInfo* cbInfo = new ListenerCallBackInfo(env, nullptr, nullptr);

    Task* task = new Task();
    auto taskId = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task));
    Task::ExecuteListenerCallback(cbInfo, taskId);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);

    napi_value func = nullptr;
    GetSendableFunction(env, "foo", func);
    napi_ref callbackRef = Helper::NapiHelper::CreateReference(env, func, 1);
    cbInfo = new ListenerCallBackInfo(env, callbackRef, nullptr);
    Task::ExecuteListenerCallback(cbInfo, taskId);
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);

    napi_value error = NapiHelper::CreateObject(env);
    cbInfo = new ListenerCallBackInfo(env, callbackRef, error);
    Task::ExecuteListenerCallback(cbInfo, taskId);
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest184, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value thisValue = CreateTaskObject(env);
    Task* task = nullptr;
    napi_unwrap(env, thisValue, reinterpret_cast<void**>(&task));

    std::string funcName = "OnEnqueued";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::OnEnqueued, nullptr, &cb);
    napi_call_function(env, thisValue, cb, 0, nullptr, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest185, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value thisValue = CreateTaskObject(env);
    Task* task = nullptr;
    napi_unwrap(env, thisValue, reinterpret_cast<void**>(&task));

    napi_value object = NapiHelper::CreateObject(env);
    napi_value argv[1] = { object };
    std::string funcName = "OnEnqueued";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::OnEnqueued, nullptr, &cb);
    napi_call_function(env, thisValue, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest186, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value thisValue = CreateTaskObject(env);
    Task* task = nullptr;
    napi_unwrap(env, thisValue, reinterpret_cast<void**>(&task));

    napi_value func = nullptr;
    GetSendableFunction(env, "foo", func);
    napi_value argv[1] = { func };
    std::string funcName = "OnEnqueued";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::OnEnqueued, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest187, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value thisValue = CreateTaskObject(env);
    Task* task = nullptr;
    napi_unwrap(env, thisValue, reinterpret_cast<void**>(&task));

    napi_value func = nullptr;
    GetSendableFunction(env, "foo", func);
    napi_value argv[1] = { func };
    std::string funcName = "OnEnqueued";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::OnEnqueued, nullptr, &cb);
    task->taskState_ = ExecuteState::WAITING;
    napi_call_function(env, thisValue, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);

    task->taskState_ = ExecuteState::NOT_FOUND;
    napi_call_function(env, thisValue, cb, 1, argv, &result);
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest188, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value thisValue = CreateTaskObject(env);
    Task* task = nullptr;
    napi_unwrap(env, thisValue, reinterpret_cast<void**>(&task));

    std::string funcName = "OnStartExecution";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::OnStartExecution, nullptr, &cb);
    napi_call_function(env, thisValue, cb, 0, nullptr, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest189, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value thisValue = CreateTaskObject(env);
    Task* task = nullptr;
    napi_unwrap(env, thisValue, reinterpret_cast<void**>(&task));

    napi_value object = NapiHelper::CreateObject(env);
    napi_value argv[1] = { object };
    std::string funcName = "OnStartExecution";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::OnStartExecution, nullptr, &cb);
    napi_call_function(env, thisValue, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest190, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value thisValue = CreateTaskObject(env);
    Task* task = nullptr;
    napi_unwrap(env, thisValue, reinterpret_cast<void**>(&task));

    napi_value func = nullptr;
    GetSendableFunction(env, "foo", func);
    napi_value argv[1] = { func };
    std::string funcName = "OnStartExecution";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::OnStartExecution, nullptr, &cb);
    napi_call_function(env, thisValue, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest191, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value thisValue = CreateTaskObject(env);
    Task* task = nullptr;
    napi_unwrap(env, thisValue, reinterpret_cast<void**>(&task));

    napi_value func = nullptr;
    GetSendableFunction(env, "foo", func);
    napi_value argv[1] = { func };
    std::string funcName = "OnStartExecution";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::OnStartExecution, nullptr, &cb);
    task->taskState_ = ExecuteState::WAITING;
    napi_call_function(env, thisValue, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);

    task->taskState_ = ExecuteState::NOT_FOUND;
    task->isMainThreadTask_ = false;
    napi_call_function(env, thisValue, cb, 1, argv, &result);
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);

    task->isMainThreadTask_ = true;
    napi_call_function(env, thisValue, cb, 1, argv, &result);
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest192, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value thisValue = CreateTaskObject(env);
    Task* task = nullptr;
    napi_unwrap(env, thisValue, reinterpret_cast<void**>(&task));

    std::string funcName = "OnExecutionFailed";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::OnExecutionFailed, nullptr, &cb);
    napi_call_function(env, thisValue, cb, 0, nullptr, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest193, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value thisValue = CreateTaskObject(env);
    Task* task = nullptr;
    napi_unwrap(env, thisValue, reinterpret_cast<void**>(&task));

    napi_value object = NapiHelper::CreateObject(env);
    napi_value argv[1] = { object };
    std::string funcName = "OnExecutionFailed";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::OnExecutionFailed, nullptr, &cb);
    napi_call_function(env, thisValue, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest194, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value thisValue = CreateTaskObject(env);
    Task* task = nullptr;
    napi_unwrap(env, thisValue, reinterpret_cast<void**>(&task));

    napi_value func = nullptr;
    GetSendableFunction(env, "foo", func);
    napi_value argv[1] = { func };
    std::string funcName = "OnExecutionFailed";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::OnExecutionFailed, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest195, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value thisValue = CreateTaskObject(env);
    Task* task = nullptr;
    napi_unwrap(env, thisValue, reinterpret_cast<void**>(&task));

    napi_value func = nullptr;
    GetSendableFunction(env, "foo", func);
    napi_value argv[1] = { func };
    std::string funcName = "OnExecutionFailed";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::OnExecutionFailed, nullptr, &cb);
    task->taskState_ = ExecuteState::WAITING;
    napi_call_function(env, thisValue, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);

    task->taskState_ = ExecuteState::NOT_FOUND;
    napi_call_function(env, thisValue, cb, 1, argv, &result);
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest196, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value thisValue = CreateTaskObject(env);
    Task* task = nullptr;
    napi_unwrap(env, thisValue, reinterpret_cast<void**>(&task));

    std::string funcName = "OnExecutionSucceeded";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::OnExecutionSucceeded, nullptr, &cb);
    napi_call_function(env, thisValue, cb, 0, nullptr, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest197, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value thisValue = CreateTaskObject(env);
    Task* task = nullptr;
    napi_unwrap(env, thisValue, reinterpret_cast<void**>(&task));

    napi_value object = NapiHelper::CreateObject(env);
    napi_value argv[1] = { object };
    std::string funcName = "OnExecutionSucceeded";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::OnExecutionSucceeded, nullptr, &cb);
    napi_call_function(env, thisValue, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest198, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value thisValue = CreateTaskObject(env);
    Task* task = nullptr;
    napi_unwrap(env, thisValue, reinterpret_cast<void**>(&task));

    napi_value func = nullptr;
    GetSendableFunction(env, "foo", func);
    napi_value argv[1] = { func };
    std::string funcName = "OnExecutionSucceeded";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::OnExecutionSucceeded, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest199, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value thisValue = CreateTaskObject(env);
    Task* task = nullptr;
    napi_unwrap(env, thisValue, reinterpret_cast<void**>(&task));

    napi_value func = nullptr;
    GetSendableFunction(env, "foo", func);
    napi_value argv[1] = { func };
    std::string funcName = "OnExecutionSucceeded";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::OnExecutionSucceeded, nullptr, &cb);
    task->taskState_ = ExecuteState::WAITING;
    napi_call_function(env, thisValue, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);

    task->taskState_ = ExecuteState::NOT_FOUND;
    napi_call_function(env, thisValue, cb, 1, argv, &result);
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest200, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value thisValue = CreateTaskObject(env);
    Task* task = nullptr;
    napi_unwrap(env, thisValue, reinterpret_cast<void**>(&task));

    std::string funcName = "IsDone";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::IsDone, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 0, nullptr, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest201, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value thisValue = CreateTaskObject(env);
    Task* task = nullptr;
    napi_unwrap(env, thisValue, reinterpret_cast<void**>(&task));

    std::string funcName = "IsDone";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::IsDone, nullptr, &cb);
    task->taskState_ = ExecuteState::WAITING;
    napi_call_function(env, thisValue, cb, 0, nullptr, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);

    task->taskState_ = ExecuteState::FINISHED;
    napi_call_function(env, thisValue, cb, 0, nullptr, &result);
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);

    task->taskState_ = ExecuteState::ENDING;
    napi_call_function(env, thisValue, cb, 0, nullptr, &result);
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest202, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value thisValue = CreateTaskObject(env);
    Task* task = nullptr;
    napi_unwrap(env, thisValue, reinterpret_cast<void**>(&task));
    NativeEngineTest::StoreTaskDuration(task->taskId_);
    std::string funcName = "GetTotalDuration";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::GetTotalDuration, nullptr, &cb);
    napi_call_function(env, thisValue, cb, 0, nullptr, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);

    funcName = "GetCPUDuration";
    cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::GetCPUDuration, nullptr, &cb);
    napi_call_function(env, thisValue, cb, 0, nullptr, &result);
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);

    funcName = "GetIODuration";
    cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::GetIODuration, nullptr, &cb);
    napi_call_function(env, thisValue, cb, 0, nullptr, &result);
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest203, testing::ext::TestSize.Level0)
{
    Task* task = new Task();
    task->IncreaseRefCount();
    task->DecreaseRefCount();
    task->taskType_ = TaskType::COMMON_TASK;
    ASSERT_TRUE(task->IsRepeatableTask());
    task->taskType_ = TaskType::GROUP_COMMON_TASK;
    ASSERT_TRUE(task->IsRepeatableTask());
    task->taskType_ = TaskType::GROUP_FUNCTION_TASK;
    ASSERT_TRUE(task->IsRepeatableTask());
    task->taskType_ = TaskType::TASK;
    ASSERT_TRUE(!task->IsRepeatableTask());
    delete task;
}

HWTEST_F(NativeEngineTest, TaskpoolTest204, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    Task* task = new Task();
    task->taskId_ = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task));
    napi_value thisValue = NapiHelper::CreateObject(env);
    napi_ref ref = NapiHelper::CreateReference(env, thisValue, 0);
    task->taskRef_ = ref;
    TaskInfo* taskInfo = new TaskInfo();
    task->pendingTaskInfos_.push_back(taskInfo);
    task->NotifyPendingTask();
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    delete task;
}

HWTEST_F(NativeEngineTest, TaskpoolTest205, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    Task* task = new Task();
    task->taskId_ = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task));
    napi_value thisValue = NapiHelper::CreateObject(env);
    napi_ref ref = NapiHelper::CreateReference(env, thisValue, 0);
    task->taskRef_ = ref;
    TaskInfo* taskInfo = new TaskInfo();
    napi_value promise = NapiHelper::CreatePromise(env, &taskInfo->deferred);
    task->pendingTaskInfos_.push_back(taskInfo);
    task->isPeriodicTask_ = false;
    task->CancelPendingTask(env);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);

    TaskInfo* taskInfo1 = new TaskInfo();
    napi_value promise1 = NapiHelper::CreatePromise(env, &taskInfo1->deferred);
    task->pendingTaskInfos_.push_back(taskInfo1);
    task->isPeriodicTask_ = true;
    task->CancelPendingTask(env);
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    delete task;
}

HWTEST_F(NativeEngineTest, TaskpoolTest206, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    Task* task = new Task();
    task->taskId_ = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task));
    napi_value thisValue = NapiHelper::CreateObject(env);
    napi_ref ref = NapiHelper::CreateReference(env, thisValue, 0);
    task->taskRef_ = ref;
    TaskInfo* taskInfo = new TaskInfo();
    task->taskType_ = TaskType::GROUP_FUNCTION_TASK;
    task->currentTaskInfo_ = taskInfo;
    task->taskRefCount_ = 1;
    Worker* worker = reinterpret_cast<Worker*>(NativeEngineTest::WorkerConstructor(env));
    task->worker_ = worker;

    napi_value func = nullptr;
    napi_value args = nullptr;
    task->DeserializeValue(env, &func, &args);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);

    task->startTime_ = task->taskId_;
    task->ioTime_ = task->taskId_;
    task->StoreTaskDuration();
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    delete task;
}

HWTEST_F(NativeEngineTest, TaskpoolTest207, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    Task* task = new Task();
    task->taskId_ = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task));
    task->taskType_ = TaskType::SEQRUNNER_TASK;
    task->hasDependency_ = false;
    task->isPeriodicTask_ = false;
    task->CanForSequenceRunner(env);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);

    task->taskType_ = TaskType::COMMON_TASK;
    task->CanForSequenceRunner(env);
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
    delete task;
}

HWTEST_F(NativeEngineTest, TaskpoolTest208, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    Task* task = new Task();
    task->taskId_ = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task));
    task->hasDependency_ = false;
    task->isPeriodicTask_ = true;
    task->CanForTaskGroup(env);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);

    task->isPeriodicTask_ = false;
    task->taskType_ = TaskType::COMMON_TASK;
    task->CanForTaskGroup(env);
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);

    task->taskType_ = TaskType::SEQRUNNER_TASK;
    task->CanForTaskGroup(env);
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);

    task->taskType_ = TaskType::GROUP_COMMON_TASK;
    task->CanForTaskGroup(env);
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);

    task->taskType_ = TaskType::TASK;
    task->isLongTask_ = true;
    task->CanForTaskGroup(env);
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
    delete task;
}

HWTEST_F(NativeEngineTest, TaskpoolTest209, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    Task* task = new Task();
    task->taskId_ = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task));
    task->taskType_ = TaskType::TASK;
    task->hasDependency_ = false;
    task->isLongTask_ = false;
    task->isPeriodicTask_ = true;
    task->CanExecute(env);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
    delete task;
}

HWTEST_F(NativeEngineTest, TaskpoolTest210, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    Task* task = new Task();
    task->taskId_ = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task));
    task->taskType_ = TaskType::COMMON_TASK;
    task->hasDependency_ = true;
    task->CanExecuteDelayed(env);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);

    task->taskType_ = TaskType::FUNCTION_TASK;
    task->isLongTask_ = true;
    task->hasDependency_ = false;
    task->CanExecuteDelayed(env);
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);

    task->taskType_ = TaskType::TASK;
    task->isPeriodicTask_ = true;
    task->CanExecuteDelayed(env);
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
    delete task;
}

HWTEST_F(NativeEngineTest, TaskpoolTest211, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    Task* task = new Task();
    task->taskId_ = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task));
    task->taskType_ = TaskType::COMMON_TASK;
    task->isPeriodicTask_ = false;
    task->CanExecutePeriodically(env);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);

    task->taskType_ = TaskType::COMMON_TASK;
    task->isPeriodicTask_ = true;
    task->CanExecutePeriodically(env);
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);

    task->taskType_ = TaskType::TASK;
    task->isPeriodicTask_ = true;
    task->CanExecutePeriodically(env);
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);

    task->taskType_ = TaskType::TASK;
    task->isPeriodicTask_ = false;
    task->hasDependency_ = true;
    task->CanExecutePeriodically(env);
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
    delete task;
}

HWTEST_F(NativeEngineTest, TaskpoolTest212, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    Task* task = new Task();
    task->taskId_ = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task));
    uv_loop_t* loop = NapiHelper::GetLibUV(env);
    uv_update_time(loop);
    uv_timer_t* timer = new uv_timer_t;
    uv_timer_init(loop, timer);
    TaskMessage* taskMessage = new TaskMessage();
    taskMessage->priority = Priority::DEFAULT;
    taskMessage->taskId = task->taskId_;
    napi_value promise = NapiHelper::CreatePromise(env, &taskMessage->deferred);
    timer->data = taskMessage;
    task->delayedTimers_.insert(nullptr);
    task->delayedTimers_.insert(timer);
    task->ClearDelayedTimers();
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    delete task;
}

HWTEST_F(NativeEngineTest, TaskpoolTest213, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    Task* task = new Task();
    task->taskId_ = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task));
    task->TryClearHasDependency();
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);

    NativeEngineTest::StoreDependentId(task->taskId_, task->taskId_);
    task->TryClearHasDependency();
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);

    Task* task1 = new Task();
    task1->taskId_ = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task1));
    NativeEngineTest::StoreDependentTaskId(task1->taskId_, task1->taskId_);
    task1->TryClearHasDependency();
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);

    NativeEngineTest::StoreDependentId(task1->taskId_, task1->taskId_);
    task1->TryClearHasDependency();
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    delete task;
}

HWTEST_F(NativeEngineTest, TaskpoolTest214, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    NativeEngineTest::InitTaskManager(env);
    NativeEngineTest::NotifyDependencyTaskInfo(env);
    NativeEngineTest::StoreTaskDependency(env);
    NativeEngineTest::RemoveTaskDependency(env);
    NativeEngineTest::ReleaseTaskData(env);
    NativeEngineTest::CancelGroupTask(env);
    NativeEngineTest::TriggerSeqRunner(env);
    NativeEngineTest::UpdateGroupState(env);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest215, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    std::string func = "SeqRunnerConstructor";
    napi_value callback = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, func.c_str(), func.size(), SequenceRunner::SeqRunnerConstructor, nullptr, &callback);
    napi_value argv[2] = {nullptr};
    napi_create_string_utf8(env, "seq04", NAPI_AUTO_LENGTH, &argv[0]);
    napi_create_uint32(env, 5, &argv[1]);
    result = nullptr;
    napi_call_function(env, nullptr, callback, 2, argv, &result);
    ASSERT_EQ(result, nullptr);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    napi_value argv1[1] = {nullptr};
    napi_create_string_utf8(env, "seq05", NAPI_AUTO_LENGTH, &argv1[0]);

    result = nullptr;
    napi_call_function(env, nullptr, callback, 1, argv1, &result);
    ASSERT_NE(result, nullptr);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    napi_value argv2[1] = {nullptr};
    napi_create_uint32(env, 5, &argv2[0]);
    result = nullptr;
    napi_call_function(env, nullptr, callback, 1, argv2, &result);
    ASSERT_EQ(result, nullptr);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    napi_value argv3[1] = {nullptr};
    result = nullptr;
    napi_call_function(env, nullptr, callback, 1, argv3, &result);
    ASSERT_EQ(result, nullptr);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    napi_value argv4[1] = {nullptr};
    napi_create_string_utf8(env, "seq05", NAPI_AUTO_LENGTH, &argv4[0]);
    result = nullptr;
    napi_call_function(env, nullptr, callback, 1, argv4, &result);
    ASSERT_NE(result, nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest216, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    std::string func = "SeqRunnerConstructor";
    napi_value SeqCallback = nullptr;
    napi_value SeqResult = nullptr;
    napi_create_function(env, func.c_str(), func.size(), SequenceRunner::SeqRunnerConstructor, nullptr, &SeqCallback);
    napi_value SeqArgv[1] = {nullptr};
    napi_create_string_utf8(env, "seq06", NAPI_AUTO_LENGTH, &SeqArgv[0]);
    napi_call_function(env, nullptr, SeqCallback, 1, SeqArgv, &SeqResult);

    std::string funcName = "Execute";
    napi_value callback = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), SequenceRunner::Execute, nullptr, &callback);

    napi_value napiSeqRunnerId = NapiHelper::GetNameProperty(env, SeqResult, "seqRunnerId");
    uint64_t seqId = NapiHelper::GetUint64Value(env, napiSeqRunnerId);
    SequenceRunner seq;
    SequenceRunnerManager::GetInstance().StoreSequenceRunner(seqId, &seq);

    napi_value thisValue = NapiHelper::CreateObject(env);
    napi_value num = nullptr;
    napi_create_uint32(env, 1, &num);
    napi_set_named_property(env, thisValue, "taskId", num);
    napi_value argv[] = {thisValue};
    napi_call_function(env, nullptr, callback, 1, argv, &result);
    ASSERT_EQ(result, nullptr);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    thisValue = CreateTaskObject(env, TaskType::COMMON_TASK);
    napi_value argv1[] = {thisValue};
    napi_call_function(env, nullptr, callback, 1, argv1, &result);
    ASSERT_EQ(result, nullptr);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    SequenceRunner seq1;
    seq1.currentTaskId_ = 1;
    NativeEngineTest::RemoveSequenceRunner(seqId);
    SequenceRunnerManager::GetInstance().StoreSequenceRunner(seqId, &seq1);
    thisValue = CreateTaskObject(env);
    napi_value argv2[] = {thisValue};
    napi_call_function(env, nullptr, callback, 1, argv2, &result);
    ASSERT_NE(result, nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest217, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    NativeEngineTest::NotifyHandleTaskResult(env);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest217_1, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    NativeEngineTest::TaskResultCallback(env);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest217_2, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    NativeEngineTest::HandleFunctionResult(env);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest217_3, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    NativeEngineTest::DebuggerOnPostTask(env);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest217_4, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    NativeEngineTest::ReleaseWorkerHandles(env);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest218, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value exception = nullptr;
    napi_value thisVar = NapiHelper::CreateObject(env);
    napi_value taskGroupResult = GeneratorTaskGroup(env, thisVar);
    napi_value napiGroupId = NapiHelper::GetNameProperty(env, taskGroupResult, "groupId");
    uint64_t groupId = NapiHelper::GetUint64Value(env, napiGroupId);

    TaskGroupManager& taskGroupManager = TaskGroupManager::GetInstance();
    TaskGroup* group = taskGroupManager.GetTaskGroup(groupId);
    GroupInfo* groupInfo = new GroupInfo();
    groupInfo->priority = Priority::DEFAULT;
    group->currentGroupInfo_ = groupInfo;
    group->groupState_ = ExecuteState::FINISHED;
    taskGroupManager.StoreTaskGroup(groupId, group);
    napi_value result = NativeEngineTest::ExecuteGroup(env, taskGroupResult);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest219, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value exception = nullptr;
    napi_value thisVar = NapiHelper::CreateObject(env);
    napi_value taskGroupResult = GeneratorTaskGroup(env, thisVar);
    napi_value napiGroupId = NapiHelper::GetNameProperty(env, taskGroupResult, "groupId");
    uint64_t groupId = NapiHelper::GetUint64Value(env, napiGroupId);

    TaskGroupManager& taskGroupManager = TaskGroupManager::GetInstance();
    TaskGroup* group = taskGroupManager.GetTaskGroup(groupId);
    GroupInfo* groupInfo = new GroupInfo();
    groupInfo->priority = Priority::DEFAULT;
    group->currentGroupInfo_ = groupInfo;
    group->groupState_ = ExecuteState::CANCELED;
    taskGroupManager.StoreTaskGroup(groupId, group);
    napi_value result = NativeEngineTest::ExecuteGroup(env, taskGroupResult);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest220, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    size_t delayTime = 1000;
    napi_value result = nullptr;
    napi_value num = nullptr;
    napi_value priority = nullptr;
    napi_value exception = nullptr;
    napi_create_uint32(env, delayTime, &num);
    napi_create_uint32(env, 1, &priority);

    napi_value obj = CreateTaskObject(env, TaskType::TASK, ExecuteState::FINISHED);
    napi_value argv[] = { num, obj, priority };
    result = NativeEngineTest::ExecuteDelayed(env, argv, 3);
    ASSERT_TRUE(result != nullptr);

    obj = CreateTaskObject(env);
    napi_value argv1[] = { num, obj, priority };
    result = NativeEngineTest::ExecuteDelayed(env, argv1, 3);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest221, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    size_t delayTime = 1000;
    napi_value result = nullptr;
    napi_value num = nullptr;
    napi_value priority = nullptr;
    napi_value exception = nullptr;
    napi_create_uint32(env, delayTime, &num);
    napi_create_uint32(env, 1, &priority);

    napi_value obj = CreateTaskObject(env, TaskType::COMMON_TASK, ExecuteState::FINISHED);
    napi_value argv[] = { num, obj, priority };
    result = NativeEngineTest::ExecuteDelayed(env, argv, 3);
    ASSERT_TRUE(result != nullptr);

    obj = CreateTaskObject(env);
    napi_value argv1[] = { num, obj, priority };
    result = NativeEngineTest::ExecuteDelayed(env, argv1, 3);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest222, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);

    napi_value thisValue = CreateTaskObject(env, TaskType::SEQRUNNER_TASK);
    napi_value argv[] = {thisValue};
    napi_value result = NativeEngineTest::Execute(env, argv, 1);
    ASSERT_TRUE(result == nullptr);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    napi_value obj = NapiHelper::CreateObject(env);
    auto group = GeneratorTaskGroup(env, obj);
    napi_value argv1[] = {group};
    result = NativeEngineTest::Execute(env, argv1, 1);
    ASSERT_TRUE(result != nullptr);

    napi_value value = nullptr;
    auto func = [](napi_env env, napi_callback_info info) -> napi_value {
        return nullptr;
    };
    napi_create_function(env, "testFunc", NAPI_AUTO_LENGTH, func, nullptr, &value);
    napi_value argv2[] = {value};
    result = NativeEngineTest::Execute(env, argv2, 1);
    ASSERT_TRUE(result == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest223, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    TaskManager &taskManager = TaskManager::GetInstance();
    napi_value exception = nullptr;
    uv_loop_t* loop = NapiHelper::GetLibUV(env);
    uv_update_time(loop);
    uv_timer_t* handle = new uv_timer_t;
    uv_timer_init(loop, handle);
    TaskMessage* taskMessage = new TaskMessage();

    napi_value global = NapiHelper::CreateObject(env);
    auto napiTask = GeneratorTask(env, global);
    Task* task = nullptr;
    napi_unwrap(env, napiTask, reinterpret_cast<void**>(&task));
    task->taskState_ = ExecuteState::CANCELED;
    
    taskMessage->taskId = task->taskId_;
    handle->data = taskMessage;
    NativeEngineTest::DelayTask(handle);
    ASSERT_TRUE(true);
}

HWTEST_F(NativeEngineTest, TaskpoolTest224, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    TaskManager &taskManager = TaskManager::GetInstance();
    napi_value exception = nullptr;
    uv_loop_t* loop = NapiHelper::GetLibUV(env);
    uv_update_time(loop);
    uv_timer_t* handle = new uv_timer_t;
    uv_timer_init(loop, handle);
    TaskMessage* taskMessage = new TaskMessage();

    napi_value global = NapiHelper::CreateObject(env);
    auto napiTask = GeneratorTask(env, global);
    Task* task = nullptr;
    napi_unwrap(env, napiTask, reinterpret_cast<void**>(&task));
    task->taskState_ = ExecuteState::DELAYED;
    taskMessage->taskId = task->taskId_;
    handle->data = taskMessage;
    NativeEngineTest::DelayTask(handle);
    ASSERT_TRUE(true);
}

HWTEST_F(NativeEngineTest, TaskpoolTest225, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    TaskManager &taskManager = TaskManager::GetInstance();
    napi_value exception = nullptr;
    uv_loop_t* loop = NapiHelper::GetLibUV(env);
    uv_update_time(loop);
    uv_timer_t* handle = new uv_timer_t;
    uv_timer_init(loop, handle);
    TaskMessage* taskMessage = new TaskMessage();

    napi_value global = NapiHelper::CreateObject(env);
    auto napiTask = GeneratorTask(env, global);
    Task* task = nullptr;
    napi_unwrap(env, napiTask, reinterpret_cast<void**>(&task));
    task->taskState_ = ExecuteState::FINISHED;
    taskMessage->taskId = task->taskId_;
    handle->data = taskMessage;
    NativeEngineTest::DelayTask(handle);
    ASSERT_TRUE(true);
}

HWTEST_F(NativeEngineTest, TaskpoolTest226, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value global = NapiHelper::CreateObject(env);
    auto napiTask = GeneratorTask(env, global);
    Task* task = nullptr;
    napi_unwrap(env, napiTask, reinterpret_cast<void**>(&task));
    task->taskType_ = TaskType::GROUP_COMMON_TASK;
    TaskPool::HandleTaskResult(task);
    ASSERT_TRUE(true);
}

HWTEST_F(NativeEngineTest, TaskpoolTest227, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);

    napi_value thisValue = CreateTaskObject(env, TaskType::SEQRUNNER_TASK, ExecuteState::CANCELED);
    Task* task = nullptr;
    napi_unwrap(env, thisValue, reinterpret_cast<void**>(&task));
    TaskInfo* taskInfo = new TaskInfo();
    task->currentTaskInfo_ = taskInfo;
    task->seqRunnerId_ = 1;
    napi_value num = nullptr;
    napi_create_uint32(env, 1, &num);
    napi_ref callbackRef = Helper::NapiHelper::CreateReference(env, num, 1);
    task->onExecutionFailedCallBackInfo_ = new ListenerCallBackInfo(env, callbackRef, nullptr);
    TaskPool::HandleTaskResult(task);
    ASSERT_TRUE(true);
}

HWTEST_F(NativeEngineTest, TaskpoolTest228, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);

    napi_value thisValue = CreateTaskObject(env);
    Task* task = nullptr;
    napi_unwrap(env, thisValue, reinterpret_cast<void**>(&task));
    TaskInfo* taskInfo = new TaskInfo();
    task->currentTaskInfo_ = taskInfo;
    task->success_ = true;
    napi_value num = nullptr;
    napi_create_uint32(env, 1, &num);
    napi_ref callbackRef = Helper::NapiHelper::CreateReference(env, num, 1);
    task->onExecutionSucceededCallBackInfo_ = new ListenerCallBackInfo(env, callbackRef, nullptr);
    void* resultData = nullptr;
    napi_value undefined = NapiHelper::GetUndefinedValue(env);
    napi_status status = napi_serialize_inner(env, num, undefined, undefined,
                                              true, false, &resultData);
    task->result_ = resultData;
    TaskPool::HandleTaskResult(task);
    ASSERT_TRUE(true);
}

HWTEST_F(NativeEngineTest, TaskpoolTest229, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    Task* task = new Task();
    uint32_t taskId = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task));
    task->taskId_ = taskId;
    napi_value thisValue = NapiHelper::CreateObject(env);
    napi_ref ref = NapiHelper::CreateReference(env, thisValue, 0);
    
    CallbackInfo* cbInfo = new CallbackInfo(env, 1, ref);

    Worker* worker = reinterpret_cast<Worker*>(NativeEngineTest::WorkerConstructor(env));
    void* args = nullptr;
    TaskResultInfo* resultInfo = new TaskResultInfo(env, taskId, args);
    TaskManager::GetInstance().ExecuteSendData(env, resultInfo, task);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest230, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    uv_timer_t* handle = new uv_timer_t;
    TaskGroupManager& taskGroupManager = TaskGroupManager::GetInstance();
    
    TaskGroup* group = new TaskGroup();
    uint64_t groupId = reinterpret_cast<uint64_t>(group);
    group->groupId_ = groupId;
    Task* task = new Task();
    uint32_t taskId = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task));
    task->taskId_ = taskId;
    task->groupId_ = groupId;

    group->taskIds_.push_back(taskId);
    handle->data = task;

    GroupInfo* groupInfo = new GroupInfo();
    groupInfo->priority = Priority::DEFAULT;
    napi_value resArr;
    napi_create_array_with_length(env, group->taskIds_.size(), &resArr);
    napi_ref arrRef = NapiHelper::CreateReference(env, resArr, 1);
    groupInfo->resArr = arrRef;
    NapiHelper::CreatePromise(env, &groupInfo->deferred);
    group->currentGroupInfo_ = groupInfo;
    group->taskNum_ = 2;
    taskGroupManager.StoreTaskGroup(groupId, group);
    napi_value value = NapiHelper::CreateUint64(env, groupId);
    napi_ref reference = NapiHelper::CreateReference(env, value, 0);
    taskGroupManager.AddTask(groupId, reference, taskId);
    
    napi_value res = nullptr;
    napi_create_uint32(env, 1, &res);
    NativeEngineTest::UpdateGroupInfoByResult(env, handle, res, true);
    ASSERT_TRUE(true);
}

HWTEST_F(NativeEngineTest, TaskpoolTest231, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value undefined = NapiHelper::GetUndefinedValue(env);
    napi_value thisVar = NapiHelper::CreateObject(env);
    napi_value taskGroup = GeneratorTaskGroup(env, thisVar);
    napi_value argv[] = {taskGroup};
    NativeEngineTest::Cancel(env, argv, 1);
    ASSERT_TRUE(true);
}

HWTEST_F(NativeEngineTest, TaskpoolTest232, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    NativeEngineTest::PerformTask(env);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest233, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    Task* task = new Task();
    task->SetValid(false);
    bool res = task->ShouldDeleteTask();
    ASSERT_TRUE(res);
    task->SetValid(true);
    task->lifecycleCount_ = 1;
    res = task->ShouldDeleteTask();
    ASSERT_TRUE(res == false);
    ASSERT_TRUE(task->lifecycleCount_ == 0);
}

HWTEST_F(NativeEngineTest, TaskpoolTest234, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string funcName = "GetName";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::GetName, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 0, nullptr, &result);
    std::string taskName = NapiHelper::GetString(env, result);
    ASSERT_STREQ(taskName.c_str(), "");
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest235, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string funcName = "GetTotalDuration";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::GetTotalDuration, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 0, nullptr, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest236, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global = NapiHelper::CreateObject(env);
    napi_value thisValue = CreateTaskObject(env);
    
    std::string funcName = "GetName";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::GetName, nullptr, &cb);
    napi_call_function(env, thisValue, cb, 0, nullptr, &result);
    std::string taskName = NapiHelper::GetString(env, result);
    ASSERT_STREQ(taskName.c_str(), "");
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest237, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    NativeEngineTest::WorkerPostTask(env);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest238, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    Task* task = new Task();
    uint32_t taskId = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task));
    task->taskId_ = taskId;
    task->isMainThreadTask_ = true;
    task->taskType_ = TaskType::FUNCTION_TASK;
    napi_value obj = NapiHelper::CreateObject(env);
    task->taskRef_ = NapiHelper::CreateReference(env, obj, 1);
    Task::TaskDestructor(env, task,  nullptr);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest239, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    Task* task = new Task();
    uint32_t taskId = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task));
    task->taskId_ = taskId;
    task->isMainThreadTask_ = false;
    task->taskType_ = TaskType::FUNCTION_TASK;
    napi_value obj = NapiHelper::CreateObject(env);
    task->taskRef_ = NapiHelper::CreateReference(env, obj, 1);
    task->lifecycleCount_.fetch_add(1);
    napi_add_env_cleanup_hook(env, Task::CleanupHookFunc, task);
    Task::TaskDestructor(env, task,  nullptr);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest240, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    Task::CleanupHookFunc(nullptr);
    Task* task = new Task();
    uint32_t taskId = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task));
    task->taskId_ = taskId;
    auto loop = NapiHelper::GetLibUV(env);
    ConcurrentHelper::UvHandleInit(loop, task->onStartExecutionSignal_, NativeEngineTest::foo, task);
    task->taskType_ = TaskType::FUNCTION_TASK;
    Task::CleanupHookFunc(task);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest241, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    Task* task = new Task();
    uint32_t taskId = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task));
    task->taskId_ = taskId;
    napi_value obj = NapiHelper::CreateObject(env);
    task->GetTaskInfoPromise(env, obj, TaskType::FUNCTION_TASK, Priority::DEFAULT);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest242, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value func = nullptr;
    GetSendableFunction(env, "foo", func);
    napi_value argv[1] = { func };
    std::string funcName = "OnStartExecution";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::OnStartExecution, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest243, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    Task* task = new Task();
    uint32_t taskId = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task));
    task->taskId_ = taskId;
    task->taskState_ = ExecuteState::CANCELED;
    task->UpdateTask(0, nullptr);
    task->isMainThreadTask_ = false;
    task->SetValid(false);
    Task::VerifyAndPostResult(task, Priority::DEFAULT);
    task->SetValid(true);
    Task::VerifyAndPostResult(task, Priority::DEFAULT);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest244, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    Task* task = new Task();
    uint32_t taskId = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task));
    task->taskId_ = taskId;
    task->isMainThreadTask_ = true;
    Task::VerifyAndPostResult(task, Priority::DEFAULT);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest245, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    Task* task = new Task();
    uint32_t taskId = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task));
    task->taskId_ = taskId;
    task->env_ = env;
    task->isMainThreadTask_ = false;
    task->onStartExecutionSignal_ = nullptr;
    task->CheckStartExecution(Priority::DEFAULT);
    auto loop = NapiHelper::GetLibUV(env);
    ConcurrentHelper::UvHandleInit(loop, task->onStartExecutionSignal_, NativeEngineTest::foo, task);
    task->SetValid(false);
    task->CheckStartExecution(Priority::DEFAULT);
    task->SetValid(true);
    task->CheckStartExecution(Priority::DEFAULT);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest246, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    Task* task = new Task();
    uint32_t taskId = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task));
    task->taskId_ = taskId;
    task->env_ = env;
    task->isMainThreadTask_ = true;
    task->onStartExecutionCallBackInfo_ = nullptr;
    task->CheckStartExecution(Priority::DEFAULT);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest247, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    Task* task = new Task();
    uint32_t taskId = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task));
    task->taskId_ = taskId;
    task->env_ = env;
    task->isMainThreadTask_ = false;
    auto loop = NapiHelper::GetLibUV(env);
    ConcurrentHelper::UvHandleInit(loop, task->onStartExecutionSignal_, NativeEngineTest::foo, task);
    TaskManager::GetInstance().ReleaseCallBackInfo(task);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest248, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    std::string func = "SeqRunnerConstructor";
    napi_value thisValue = NapiHelper::CreateObject(env);
    napi_value SeqCallback = nullptr;
    napi_value SeqResult = nullptr;
    napi_create_function(env, func.c_str(), func.size(), SequenceRunner::SeqRunnerConstructor, nullptr, &SeqCallback);
    napi_value SeqArgv[1] = {nullptr};
    napi_create_string_utf8(env, "seq248", NAPI_AUTO_LENGTH, &SeqArgv[0]);
    napi_call_function(env, thisValue, SeqCallback, 1, SeqArgv, &SeqResult);

    std::string funcName = "Execute";
    napi_value callback = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), SequenceRunner::Execute, nullptr, &callback);

    napi_value task = CreateTaskObject(env);
    napi_value argv[] = {task};
    napi_call_function(env, thisValue, callback, 1, argv, &result);
    ASSERT_NE(result, nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest249, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    std::string func = "SeqRunnerConstructor";
    napi_value thisValue = NapiHelper::CreateObject(env);
    napi_value SeqCallback = nullptr;
    napi_value SeqResult = nullptr;
    napi_create_function(env, func.c_str(), func.size(), SequenceRunner::SeqRunnerConstructor, nullptr, &SeqCallback);
    napi_value SeqArgv[1] = {nullptr};
    napi_create_string_utf8(env, "seq249", NAPI_AUTO_LENGTH, &SeqArgv[0]);
    napi_call_function(env, thisValue, SeqCallback, 1, SeqArgv, &SeqResult);

    std::string funcName = "Execute";
    napi_value callback = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), SequenceRunner::Execute, nullptr, &callback);

    napi_value task = CreateTaskObject(env);
    napi_value argv[] = {task};
    napi_call_function(env, thisValue, callback, 1, argv, &result);
    ASSERT_NE(result, nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest250, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    std::string funcName = "AsyncRunnerConstructor250";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), AsyncRunner::AsyncRunnerConstructor, nullptr, &cb);
    napi_value nameNumber = NapiHelper::CreateUint32(env, 5);
    napi_value runningCapacity = NapiHelper::CreateUint32(env, 5);
    napi_value waitingCapacity = nullptr;
    napi_create_int32(env, -1, &waitingCapacity);
    napi_value argv[] = {nameNumber, runningCapacity, waitingCapacity};
    napi_call_function(env, nullptr, cb, 3, argv, &result);
    ASSERT_EQ(result, nullptr);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    napi_value argv2[] = {runningCapacity, waitingCapacity};
    napi_call_function(env, nullptr, cb, 2, argv2, &result);
    ASSERT_EQ(result, nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest251, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    std::string funcName = "AsyncRunnerConstructor251";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), AsyncRunner::AsyncRunnerConstructor, nullptr, &cb);
    napi_value runningCapacity = NapiHelper::CreateUint32(env, 5);
    napi_value argv[] = {nullptr, runningCapacity};
    napi_call_function(env, nullptr, cb, 2, argv, &result);
    ASSERT_EQ(result, nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest252, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    std::string funcName = "AsyncRunnerConstructor252";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), AsyncRunner::AsyncRunnerConstructor, nullptr, &cb);
    napi_value name = GetNapiString(env, "async252");
    napi_value argv[] = {name, name};
    napi_call_function(env, nullptr, cb, 2, argv, &result);
    ASSERT_EQ(result, nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest253, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    std::string funcName = "AsyncRunnerConstructor253";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), AsyncRunner::AsyncRunnerConstructor, nullptr, &cb);
    napi_value runningCapacity = NapiHelper::CreateUint32(env, 0);
    napi_value argv[] = {runningCapacity};
    napi_call_function(env, nullptr, cb, 1, argv, &result);
    ASSERT_EQ(result, nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest254, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    std::string funcName = "AsyncRunnerConstructor254";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), AsyncRunner::AsyncRunnerConstructor, nullptr, &cb);
    napi_value runningCapacity = NapiHelper::CreateUint32(env, 5);
    napi_value name = GetNapiString(env, "async254");
    napi_value argv[] = {runningCapacity, name};
    napi_call_function(env, nullptr, cb, 2, argv, &result);
    ASSERT_EQ(result, nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest255, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    std::string funcName = "AsyncRunnerConstructor255";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), AsyncRunner::AsyncRunnerConstructor, nullptr, &cb);
    napi_value runningCapacity = NapiHelper::CreateUint32(env, 5);
    napi_value argv[] = {runningCapacity};
    napi_value obj = NapiHelper::CreateObject(env);
    napi_call_function(env, obj, cb, 1, argv, &result);
    ASSERT_NE(result, nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest256, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    NativeEngineTest::CheckAndCreateAsyncRunner(env, nullptr, nullptr, nullptr);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_NE(exception, nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest257, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);

    std::string asyncName = "async257";
    AsyncRunnerManager &asyncRunnerManager = AsyncRunnerManager::GetInstance();
    napi_value global = NapiHelper::GetGlobalObject(env);
    uint32_t capacity = 5;
    asyncRunnerManager.CreateOrGetGlobalRunner(env, global, asyncName, capacity, capacity);

    std::string funcName = "AsyncRunnerConstructor257";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), AsyncRunner::AsyncRunnerConstructor, nullptr, &cb);
    napi_value runningCapacity = NapiHelper::CreateUint32(env, 3);
    napi_value name = GetNapiString(env, asyncName.c_str());
    napi_value argv1[] = {name, runningCapacity};
    napi_call_function(env, nullptr, cb, 2, argv1, &result);
    ASSERT_EQ(result, nullptr);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    result = nullptr;
    napi_value runningCapacity2 = NapiHelper::CreateUint32(env, 5);
    napi_value argv2[] = {name, runningCapacity2};
    napi_call_function(env, nullptr, cb, 2, argv2, &result);
    ASSERT_EQ(result, nullptr);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    asyncRunnerManager.CreateOrGetGlobalRunner(env, global, asyncName, capacity, capacity);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_EQ(exception, nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest258, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    std::string funcName = "Execute258";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), AsyncRunner::Execute, nullptr, &cb);

    napi_value obj = NapiHelper::CreateObject(env);
    napi_value priority = NapiHelper::CreateUint32(env, 1);
    napi_value argv[] = {obj, priority};
    napi_call_function(env, nullptr, cb, 2, argv, &result);
    ASSERT_EQ(result, nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest259, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    std::string funcName = "Execute259";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), AsyncRunner::Execute, nullptr, &cb);

    napi_value argv1[] = {nullptr};
    napi_call_function(env, nullptr, cb, 0, argv1, &result);
    ASSERT_EQ(result, nullptr);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    napi_value priority = NapiHelper::CreateUint32(env, 1);
    napi_value argv2[] = {priority, priority, priority};
    result = nullptr;
    napi_call_function(env, nullptr, cb, 3, argv2, &result);
    ASSERT_EQ(result, nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest260, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    std::string funcName = "Execute260";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), AsyncRunner::Execute, nullptr, &cb);

    napi_value priority = NapiHelper::CreateUint32(env, 4);
    napi_value argv[] = {priority, priority};
    napi_call_function(env, nullptr, cb, 2, argv, &result);
    ASSERT_EQ(result, nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest261, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    std::string funcName = "Execute261";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), AsyncRunner::Execute, nullptr, &cb);

    napi_value obj = NapiHelper::CreateObject(env);
    napi_value task = GeneratorTask(env, obj);
    napi_value argv1[] = {task, task};
    napi_call_function(env, nullptr, cb, 2, argv1, &result);
    ASSERT_EQ(result, nullptr);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    napi_value priority = NapiHelper::CreateUint32(env, 4);
    napi_value argv2[] = {task, priority};
    result = nullptr;
    napi_call_function(env, nullptr, cb, 2, argv2, &result);
    ASSERT_EQ(result, nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest262, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    std::string funcName = "Execute262";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), AsyncRunner::Execute, nullptr, &cb);

    napi_value obj = NapiHelper::CreateObject(env);
    napi_value task = GeneratorTask(env, obj);
    napi_value priority = NapiHelper::CreateUint32(env, 1);
    napi_value argv[] = {task, priority};
    napi_call_function(env, nullptr, cb, 2, argv, &result);
    ASSERT_NE(result, nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest263, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value asyncGlobal = NapiHelper::CreateObject(env);
    std::string conName = "AsyncRunnerConstructor263";
    napi_value callback = nullptr;
    napi_value asyncResult = nullptr;
    napi_create_function(env, conName.c_str(), conName.size(), AsyncRunner::AsyncRunnerConstructor, nullptr, &callback);
    napi_value runningCapacity = NapiHelper::CreateUint32(env, 3);
    napi_value asyncArgv[] = {runningCapacity, runningCapacity};
    napi_call_function(env, asyncGlobal, callback, 2, asyncArgv, &asyncResult);

    std::string funcName = "Execute263";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), AsyncRunner::Execute, nullptr, &cb);
    napi_value obj = NapiHelper::CreateObject(env);
    napi_value task = GeneratorTask(env, obj);
    napi_value priority = NapiHelper::CreateUint32(env, 1);
    napi_value argv[] = {task, priority};
    napi_call_function(env, asyncGlobal, cb, 2, argv, &result);
    ASSERT_NE(result, nullptr);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    napi_value task2 = CreateTaskObject(env, TaskType::ASYNCRUNNER_TASK);
    napi_value argv2[] = {task2, priority};
    result = nullptr;
    napi_call_function(env, asyncGlobal, cb, 2, argv2, &result);
    ASSERT_EQ(result, nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest264, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value asyncGlobal = NapiHelper::CreateObject(env);
    std::string conName = "AsyncRunnerConstructor264";
    napi_value callback = nullptr;
    napi_value asyncResult = nullptr;
    napi_create_function(env, conName.c_str(), conName.size(), AsyncRunner::AsyncRunnerConstructor, nullptr, &callback);
    napi_value runningCapacity = NapiHelper::CreateUint32(env, 1);
    napi_value asyncArgv[] = {runningCapacity, runningCapacity};
    napi_call_function(env, asyncGlobal, callback, 2, asyncArgv, &asyncResult);

    std::string funcName = "Execute264";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), AsyncRunner::Execute, nullptr, &cb);
    napi_value task1 = CreateTaskObject(env);
    napi_value priority = NapiHelper::CreateUint32(env, 1);
    napi_value argv1[] = {task1, priority};
    napi_call_function(env, asyncGlobal, cb, 2, argv1, &result);
    ASSERT_NE(result, nullptr);

    napi_value task2 = CreateTaskObject(env);
    napi_value argv2[] = {task2, priority};
    result = nullptr;
    napi_call_function(env, asyncGlobal, cb, 2, argv2, &result);
    ASSERT_NE(result, nullptr);

    napi_value task3 = CreateTaskObject(env);
    napi_value argv3[] = {task3, priority};
    result = nullptr;
    napi_call_function(env, asyncGlobal, cb, 2, argv3, &result);
    ASSERT_NE(result, nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest265, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    void* data = nullptr;
    NativeEngineTest::AsyncRunnerDestructor(nullptr, data);
    NativeEngineTest::AsyncRunnerDestructor(env, data);
    AsyncRunner* asyncRunner = new AsyncRunner();
    asyncRunner->asyncRunnerId_ = 265;
    void* data2 = static_cast<void*>(asyncRunner);
    NativeEngineTest::AsyncRunnerDestructor(env, data2);
    ASSERT_TRUE(true);
}

HWTEST_F(NativeEngineTest, TaskpoolTest266, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    AsyncRunnerManager &asyncRunnerManager = AsyncRunnerManager::GetInstance();
    Task* task = new Task();
    task->taskType_ = TaskType::ASYNCRUNNER_TASK;
    asyncRunnerManager.TriggerAsyncRunner(env, task);

    AsyncRunner* asyncRunner = new AsyncRunner();
    asyncRunner->runningCapacity_ = 1;
    asyncRunner->waitingCapacity_ = 1;
    asyncRunner->runningCount_ = 1;
    asyncRunner->asyncRunnerId_ = reinterpret_cast<uint64_t>(asyncRunner);
    task->asyncRunnerId_ = asyncRunner->asyncRunnerId_;
    asyncRunnerManager.StoreAsyncRunner(asyncRunner->asyncRunnerId_, asyncRunner);

    Task* task2 = new Task();
    task2->asyncRunnerId_ = asyncRunner->asyncRunnerId_;
    TaskInfo* taskInfo = new TaskInfo();
    task2->currentTaskInfo_ = taskInfo;
    void* data2 = reinterpret_cast<void*>(task2);
    void* async = reinterpret_cast<void*>(asyncRunner);
    NativeEngineTest::AddTasksToAsyncRunner(async, data2);
    asyncRunnerManager.TriggerAsyncRunner(env, task);
    delete task;
    ASSERT_TRUE(true);
}

HWTEST_F(NativeEngineTest, TaskpoolTest267, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    AsyncRunnerManager &asyncRunnerManager = AsyncRunnerManager::GetInstance();
    Task* task = new Task();
    task->taskState_ = ExecuteState::FINISHED;
    asyncRunnerManager.CancelAsyncRunnerTask(env, task);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_NE(exception, nullptr);

    task->taskState_ = ExecuteState::ENDING;
    asyncRunnerManager.CancelAsyncRunnerTask(env, task);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_NE(exception, nullptr);

    task->taskState_ = ExecuteState::RUNNING;
    asyncRunnerManager.CancelAsyncRunnerTask(env, task);
    
    task->taskState_ = ExecuteState::WAITING;
    TaskInfo* taskInfo = new TaskInfo();
    task->currentTaskInfo_ = taskInfo;
    task->taskId_ = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task));
    asyncRunnerManager.CancelAsyncRunnerTask(env, task);
    delete task;
    ASSERT_TRUE(true);
}

HWTEST_F(NativeEngineTest, TaskpoolTest268, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    AsyncRunnerManager &asyncRunnerManager = AsyncRunnerManager::GetInstance();
    Task* task = new Task();
    
    AsyncRunner* asyncRunner = new AsyncRunner();
    asyncRunner->runningCapacity_ = 1;
    asyncRunner->waitingCapacity_ = 1;
    asyncRunner->runningCount_ = 1;
    asyncRunner->asyncRunnerId_ = reinterpret_cast<uint64_t>(asyncRunner);
    asyncRunner->IncreaseAsyncCount();
    task->asyncRunnerId_ = asyncRunner->asyncRunnerId_;
    asyncRunnerManager.StoreAsyncRunner(asyncRunner->asyncRunnerId_, asyncRunner);
    bool flag = asyncRunnerManager.TriggerAsyncRunner(env, task);
    ASSERT_TRUE(flag);
    delete task;
}

HWTEST_F(NativeEngineTest, TaskpoolTest269, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    AsyncRunnerManager &asyncRunnerManager = AsyncRunnerManager::GetInstance();

    std::string asyncName = "async269";
    napi_value obj = NapiHelper::CreateObject(env);
    AsyncRunner* asyncRunner = asyncRunnerManager.CreateOrGetGlobalRunner(env, obj, asyncName, 5, 0);
    asyncRunner->asyncRunnerId_ = reinterpret_cast<uint64_t>(asyncRunner);
    asyncRunner->isGlobalRunner_ = true;
    asyncRunnerManager.StoreAsyncRunner(asyncRunner->asyncRunnerId_, asyncRunner);

    Task* task = new Task();
    task->asyncRunnerId_ = asyncRunner->asyncRunnerId_;

    Task* task2 = new Task();
    task2->asyncRunnerId_ = asyncRunner->asyncRunnerId_;
    task2->taskState_ = ExecuteState::CANCELED;
    TaskInfo* taskInfo = new TaskInfo();
    task2->currentTaskInfo_ = taskInfo;
    void* data2 = reinterpret_cast<void*>(task2);
    void* async = reinterpret_cast<void*>(asyncRunner);
    NativeEngineTest::AddTasksToAsyncRunner(async, data2);
    asyncRunner->IncreaseAsyncCount();

    Task* task3 = new Task();
    task3->asyncRunnerId_ = asyncRunner->asyncRunnerId_;
    task3->taskId_ = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task3));
    void* data3 = reinterpret_cast<void*>(task3);
    NativeEngineTest::AddTasksToAsyncRunner(async, data3);
    asyncRunner->IncreaseAsyncCount();
    bool flag = asyncRunnerManager.TriggerAsyncRunner(env, task);
    ASSERT_TRUE(flag);

    delete task;
    delete task2;
    delete task3;
}

HWTEST_F(NativeEngineTest, TaskpoolTest270, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    
    napi_value asyncGlobal = NapiHelper::CreateObject(env);
    std::string conName = "AsyncRunnerConstructor270";
    napi_value callback = nullptr;
    napi_value asyncResult = nullptr;
    napi_create_function(env, conName.c_str(), conName.size(), AsyncRunner::AsyncRunnerConstructor, nullptr, &callback);
    std::string asyncName = "async270";
    napi_value name = GetNapiString(env, asyncName.c_str());
    napi_value runningCapacity = NapiHelper::CreateUint32(env, 1);
    napi_value asyncArgv[] = {name, runningCapacity, runningCapacity};
    napi_call_function(env, asyncGlobal, callback, 3, asyncArgv, &asyncResult);

    std::string funcName = "Execute270";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), AsyncRunner::Execute, nullptr, &cb);
    napi_value task = CreateTaskObject(env);
    napi_value priority = NapiHelper::CreateUint32(env, 1);
    napi_value argv[] = {task, priority};
    napi_call_function(env, asyncGlobal, cb, 2, argv, &result);
    ASSERT_NE(result, nullptr);

    std::string funcName2 = "Execute270-1";
    napi_value cb2 = nullptr;
    napi_value result2 = nullptr;
    napi_create_function(env, funcName2.c_str(), funcName2.size(), AsyncRunner::Execute, nullptr, &cb2);
    napi_value task2 = CreateTaskObject(env);
    napi_value argv2[] = {task2, priority};
    napi_call_function(env, asyncGlobal, cb2, 2, argv2, &result2);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_EQ(exception, nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest271, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    Task* task = new Task();
    task->taskType_ = TaskType::ASYNCRUNNER_TASK;
    bool flag = task->CanForSequenceRunner(env);
    ASSERT_FALSE(flag);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    flag = task->CanForTaskGroup(env);
    ASSERT_FALSE(flag);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    flag = task->CanExecute(env);
    ASSERT_FALSE(flag);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    flag = task->CanExecuteDelayed(env);
    ASSERT_FALSE(flag);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    flag = task->CanExecutePeriodically(env);
    ASSERT_FALSE(flag);
    delete task;
}

HWTEST_F(NativeEngineTest, TaskpoolTest272, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    Task* task = new Task();
    task->hasDependency_ = true;
    bool flag = task->CanForAsyncRunner(env);
    ASSERT_FALSE(flag);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    task->hasDependency_ = false;
    task->isPeriodicTask_ = true;
    flag = task->CanForAsyncRunner(env);
    ASSERT_FALSE(flag);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    task->isPeriodicTask_ = false;
    task->taskType_ = TaskType::COMMON_TASK;
    flag = task->CanForAsyncRunner(env);
    ASSERT_FALSE(flag);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    task->taskType_ = TaskType::SEQRUNNER_TASK;
    flag = task->CanForAsyncRunner(env);
    ASSERT_FALSE(flag);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);

    task->taskType_ = TaskType::GROUP_COMMON_TASK;
    flag = task->CanForAsyncRunner(env);
    ASSERT_FALSE(flag);
    delete task;
}

HWTEST_F(NativeEngineTest, TaskpoolTest273, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value thisValue = CreateTaskObject(env, TaskType::ASYNCRUNNER_TASK, ExecuteState::CANCELED);
    Task* task = nullptr;
    napi_unwrap(env, thisValue, reinterpret_cast<void**>(&task));
    TaskInfo* taskInfo = new TaskInfo();
    task->currentTaskInfo_ = taskInfo;
    task->asyncRunnerId_ = 1;
    napi_value num = nullptr;
    napi_create_int32(env, 1, &num);
    napi_ref callbackRef = NapiHelper::CreateReference(env, num, 1);
    task->onExecutionFailedCallBackInfo_ = new ListenerCallBackInfo(env, callbackRef, nullptr);
    TaskPool::HandleTaskResult(task);
    ASSERT_TRUE(true);
}

HWTEST_F(NativeEngineTest, TaskpoolTest274, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    TaskManager& taskManager = TaskManager::GetInstance();
    Task* task = new Task();
    task->taskType_ = TaskType::ASYNCRUNNER_TASK;
    taskManager.StoreTask(task);
    uint32_t taskId = task->taskId_;
    taskManager.CancelTask(env, taskId);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_EQ(exception, nullptr);
    taskManager.RemoveTask(taskId);
    delete task;
}

HWTEST_F(NativeEngineTest, TaskpoolTest275, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    TaskManager& taskManager = TaskManager::GetInstance();
    AsyncRunnerManager& asyncRunnerManager = AsyncRunnerManager::GetInstance();

    std::string asyncName = "async275";
    napi_value obj = NapiHelper::CreateObject(env);
    AsyncRunner* asyncRunner = asyncRunnerManager.CreateOrGetGlobalRunner(env, obj, asyncName, 5, 0);
    asyncRunner->asyncRunnerId_ = reinterpret_cast<uint64_t>(asyncRunner);
    asyncRunnerManager.StoreAsyncRunner(asyncRunner->asyncRunnerId_, asyncRunner);

    Task* task = new Task();
    task->asyncRunnerId_ = asyncRunner->asyncRunnerId_;
    TaskInfo* taskInfo = new TaskInfo();
    task->currentTaskInfo_ = taskInfo;
    task->taskType_ = TaskType::ASYNCRUNNER_TASK;
    task->taskState_ = ExecuteState::WAITING;
    taskManager.StoreTask(task);
    taskManager.CancelTask(env, task->taskId_);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_EQ(exception, nullptr);
    asyncRunner->waitingTasks_.push_back(task);

    Task* task2 = new Task();
    task2->env_ = env;
    task2->asyncRunnerId_ = asyncRunner->asyncRunnerId_;
    TaskInfo* taskInfo2 = new TaskInfo();
    task2->currentTaskInfo_ = taskInfo2;
    task2->taskType_ = TaskType::ASYNCRUNNER_TASK;
    task2->SetValid(false);
    asyncRunner->waitingTasks_.push_back(task2);
    taskManager.StoreTask(task2);
    taskManager.CancelTask(env, task2->taskId_);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_EQ(exception, nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest276, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    TaskManager& taskManager = TaskManager::GetInstance();
    AsyncRunner* asyncRunner = new AsyncRunner();
    asyncRunner->TriggerRejectErrorTimer(nullptr, 0);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_EQ(exception, nullptr);
    delete asyncRunner;

    Task* task = new Task();
    taskManager.StoreTask(task);
    DiscardTaskMessage* message = new DiscardTaskMessage(task->env_, task->taskId_, 0, true);
    task->DiscardAsyncRunnerTask(message);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_EQ(exception, nullptr);

    DiscardTaskMessage* message2 = new DiscardTaskMessage(task->env_, task->taskId_, 0, true);
    task->SetValid(false);
    task->taskType_ = TaskType::ASYNCRUNNER_TASK;
    task->DiscardAsyncRunnerTask(message2);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_EQ(exception, nullptr);

    task->SetValid(true);
    task->DiscardAsyncRunnerTask(nullptr);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_EQ(exception, nullptr);

    taskManager.RemoveTask(task->taskId_);
    delete task;
}

HWTEST_F(NativeEngineTest, TaskpoolTest277, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    TaskManager& taskManager = TaskManager::GetInstance();
    
    Task* task = new Task();
    TaskInfo* taskInfo = new TaskInfo();
    task->currentTaskInfo_ = taskInfo;
    task->taskType_ = TaskType::ASYNCRUNNER_TASK;
    task->isMainThreadTask_ = true;
    taskManager.StoreTask(task);
    DiscardTaskMessage* message = new DiscardTaskMessage(task->env_, 1, 0, true);
    task->DiscardAsyncRunnerTask(message);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_EQ(exception, nullptr);

    Task* task2 = new Task();
    TaskInfo* taskInfo2 = new TaskInfo();
    task2->currentTaskInfo_ = taskInfo2;
    task2->taskType_ = TaskType::ASYNCRUNNER_TASK;
    taskManager.StoreTask(task2);
    task2->DiscardInner(nullptr);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_EQ(exception, nullptr);

    task2->env_ = env;
    DiscardTaskMessage* message2 = new DiscardTaskMessage(nullptr, task2->taskId_, 0, true);
    task2->DiscardInner(message2);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_EQ(exception, nullptr);

    task2->SetValid(false);
    DiscardTaskMessage* message3 = new DiscardTaskMessage(task2->env_, task2->taskId_, 0, true);
    task2->DiscardInner(message3);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_EQ(exception, nullptr);
    taskManager.RemoveTask(task->taskId_);
    taskManager.RemoveTask(task2->taskId_);
    delete task;
    delete task2;
}

HWTEST_F(NativeEngineTest, TaskpoolTest278, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    TaskManager& taskManager = TaskManager::GetInstance();
    AsyncRunnerManager& asyncRunnerManager = AsyncRunnerManager::GetInstance();

    napi_value obj = NapiHelper::CreateObject(env);
    AsyncRunner* asyncRunner = asyncRunnerManager.CreateOrGetGlobalRunner(env, obj, "", 5, 0);
    asyncRunner->asyncRunnerId_ = reinterpret_cast<uint64_t>(asyncRunner);
    asyncRunnerManager.StoreAsyncRunner(asyncRunner->asyncRunnerId_, asyncRunner);

    Task* task = new Task();
    task->asyncRunnerId_ = asyncRunner->asyncRunnerId_;
    task->taskType_ = TaskType::ASYNCRUNNER_TASK;
    taskManager.StoreTask(task);
    asyncRunner->waitingTasks_.push_back(task);
    taskManager.CancelTask(env, task->taskId_);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_EQ(exception, nullptr);
    taskManager.RemoveTask(task->taskId_);
    delete task;
}

HWTEST_F(NativeEngineTest, TaskpoolTest279, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value argv[] = {nullptr};
    std::string funcName = "AsyncRunnerConstructor279";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), AsyncRunner::AsyncRunnerConstructor, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 0, argv, &result);
    ASSERT_EQ(result, nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest280, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    std::string funcName = "AsyncRunnerConstructor280";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), AsyncRunner::AsyncRunnerConstructor, nullptr, &cb);
    napi_value name = GetNapiString(env, "async280");
    napi_value runningCapacity = NapiHelper::CreateUint32(env, 5);
    napi_value argv[] = {name, runningCapacity};
    napi_call_function(env, nullptr, cb, 2, argv, &result);
    ASSERT_NE(result, nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest281, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value thisValue = CreateTaskObject(env);
    Task* task = nullptr;
    napi_unwrap(env, thisValue, reinterpret_cast<void**>(&task));

    napi_value dependentTask = CreateTaskObject(env);
    Task* task1 = nullptr;
    napi_unwrap(env, dependentTask, reinterpret_cast<void**>(&task1));
    napi_value argv[] = { dependentTask };
    std::string funcName = "RemoveDependency";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::RemoveDependency, nullptr, &cb);
    task->hasDependency_ = true;
    task->isPeriodicTask_ = false;
    task->taskType_ = TaskType::TASK;
    task1->hasDependency_ = true;
    task1->isPeriodicTask_ = false;
    task1->taskType_ = TaskType::ASYNCRUNNER_TASK;
    NativeEngineTest::StoreDependentId(task->taskId_, task1->taskId_);
    napi_call_function(env, thisValue, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest282, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    TaskManager& taskManager = TaskManager::GetInstance();
    AsyncRunnerManager& asyncRunnerManager = AsyncRunnerManager::GetInstance();
    
    Task* task = new Task();
    task->env_ = env;
    TaskInfo* taskInfo = new TaskInfo();
    task->currentTaskInfo_ = taskInfo;
    task->taskType_ = TaskType::ASYNCRUNNER_TASK;
    taskManager.StoreTask(task);

    std::string asyncName = "";
    napi_value obj = NapiHelper::CreateObject(env);
    AsyncRunner* asyncRunner = asyncRunnerManager.CreateOrGetGlobalRunner(env, obj, asyncName, 5, 0);
    asyncRunner->asyncRunnerId_ = reinterpret_cast<uint64_t>(asyncRunner);
    asyncRunnerManager.StoreAsyncRunner(asyncRunner->asyncRunnerId_, asyncRunner);
    task->asyncRunnerId_ = asyncRunner->asyncRunnerId_;
    napi_value obj2 = NapiHelper::CreateObject(env);
    task->taskRef_ = NapiHelper::CreateReference(env, obj2, 1);
    DiscardTaskMessage* message = new DiscardTaskMessage(task->env_, task->taskId_, 0, false);
    task->DiscardInner(message);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_EQ(exception, nullptr);
    taskManager.RemoveTask(task->taskId_);
    delete task;
}

HWTEST_F(NativeEngineTest, TaskpoolTest283, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    TaskManager& taskManager = TaskManager::GetInstance();
    AsyncRunnerManager& asyncRunnerManager = AsyncRunnerManager::GetInstance();
    
    uv_async_t* req = new uv_async_t;
    req->data = nullptr;
    Task::DiscardTask(req);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_EQ(exception, nullptr);

    Task* task = new Task();
    TaskInfo* taskInfo = new TaskInfo();
    task->currentTaskInfo_ = taskInfo;
    task->taskType_ = TaskType::ASYNCRUNNER_TASK;
    taskManager.StoreTask(task);
    DiscardTaskMessage* message = new DiscardTaskMessage(env, task->taskId_, 0, false);
    req->data = message;
    Task::DiscardTask(req);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_EQ(exception, nullptr);

    DiscardTaskMessage* message2 = new DiscardTaskMessage(nullptr, 283, 0, false);
    req->data = message2;
    Task::DiscardTask(req);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_EQ(exception, nullptr);

    task->env_ = env;
    task->SetValid(false);
    DiscardTaskMessage* message3 = new DiscardTaskMessage(env, task->taskId_, 0, false);
    req->data = message3;
    Task::DiscardTask(req);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_EQ(exception, nullptr);
    taskManager.RemoveTask(task->taskId_);
    delete task;
}

HWTEST_F(NativeEngineTest, TaskpoolTest284, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    AsyncRunnerManager &asyncRunnerManager = AsyncRunnerManager::GetInstance();
    Task* task = new Task();
    task->taskType_ = TaskType::ASYNCRUNNER_TASK;
    asyncRunnerManager.TriggerAsyncRunner(env, task);

    AsyncRunner* asyncRunner = new AsyncRunner();
    asyncRunner->runningCapacity_ = 1;
    asyncRunner->waitingCapacity_ = 1;
    asyncRunner->runningCount_ = 1;
    asyncRunner->asyncRunnerId_ = reinterpret_cast<uint64_t>(asyncRunner);
    task->asyncRunnerId_ = asyncRunner->asyncRunnerId_;
    asyncRunnerManager.StoreAsyncRunner(asyncRunner->asyncRunnerId_, asyncRunner);

    Task* task2 = new Task();
    task2->asyncRunnerId_ = asyncRunner->asyncRunnerId_;
    TaskInfo* taskInfo = new TaskInfo();
    task2->currentTaskInfo_ = taskInfo;
    task2->SetValid(false);
    void* data2 = reinterpret_cast<void*>(task2);
    void* async = reinterpret_cast<void*>(asyncRunner);
    NativeEngineTest::AddTasksToAsyncRunner(async, data2);
    asyncRunnerManager.TriggerAsyncRunner(env, task);
    delete task;
    delete task2;
    ASSERT_TRUE(true);
}

HWTEST_F(NativeEngineTest, TaskpoolTest285, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    TaskGroup::HostEnvCleanupHook(nullptr);
    TaskGroup* group = new TaskGroup();
    TaskGroup::HostEnvCleanupHook(group);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_EQ(exception, nullptr);
    delete group;
}

HWTEST_F(NativeEngineTest, TaskpoolTest286, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    uv_async_t* req = new uv_async_t;
    req->data = nullptr;
    TaskGroup::StartRejectResult(req);
    TaskGroup* group = new TaskGroup(env);
    req->data = group;
    TaskGroup::StartRejectResult(req);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_EQ(exception, nullptr);
    delete group;
    delete req;
}

HWTEST_F(NativeEngineTest, TaskpoolTest287, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    TaskGroup* group = new TaskGroup(env);
    group->CancelGroupTask(env, 0);
    napi_env runtimeEnv = nullptr;
    napi_create_runtime(env, &runtimeEnv);
    group->CancelGroupTask(runtimeEnv, 0);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_EQ(exception, nullptr);
    delete group;
}

HWTEST_F(NativeEngineTest, TaskpoolTest288, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    TaskGroup* group = new TaskGroup(env);
    uint64_t groupId = reinterpret_cast<uint64_t>(group);
    group->groupId_ = groupId;
    
    Task* task = new Task();
    uint32_t taskId = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task));
    task->taskId_ = taskId;
    task->groupId_ = groupId;
    group->taskIds_.push_back(taskId);
    GroupInfo* groupInfo = new GroupInfo();
    groupInfo->priority = Priority::DEFAULT;
    napi_value resArr;
    napi_create_array_with_length(env, group->taskIds_.size(), &resArr);
    napi_ref arrRef = NapiHelper::CreateReference(env, resArr, 1);
    groupInfo->resArr = arrRef;
    NapiHelper::CreatePromise(env, &groupInfo->deferred);
    group->currentGroupInfo_ = groupInfo;
    group->taskNum_ = 0;
    groupInfo->finishedTaskNum = 1;
    group->RejectResult(env);

    group->taskNum_ = 1;
    napi_value obj = NapiHelper::CreateObject(env);
    napi_ref ref = NapiHelper::CreateReference(env, obj, 1);
    group->groupRef_ = ref;
    group->RejectResult(env);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_EQ(exception, nullptr);
    delete group;
    delete task;
}

HWTEST_F(NativeEngineTest, TaskpoolTest289, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    TaskGroup* group = new TaskGroup(env);
    uv_loop_t* loop = NapiHelper::GetLibUV(env);
    ConcurrentHelper::UvHandleInit(loop, group->onRejectResultSignal_, NativeEngineTest::foo);
    group->TriggerRejectResult();
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_EQ(exception, nullptr);
    delete group;
}

HWTEST_F(NativeEngineTest, TaskpoolTest290, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    uint32_t id = 1;
    napi_value taskId = NapiHelper::CreateUint32(env, id);
    napi_value argv[] = { taskId };
    NativeEngineTest::Cancel(env, argv, 1);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest291, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    TaskManager& taskManager = TaskManager::GetInstance();
    Task* task = new Task();
    taskManager.StoreTask(task);
    task->taskType_ = TaskType::GROUP_COMMON_TASK;
    taskManager.CancelTask(env, task->taskId_);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);

    TaskInfo* taskInfo = new TaskInfo();
    task->currentTaskInfo_ = taskInfo;
    taskManager.CancelTask(env, task->taskId_);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
    task->taskState_ = ExecuteState::FINISHED;
    taskManager.CancelTask(env, task->taskId_);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);

    task->taskState_ = ExecuteState::ENDING;
    taskManager.CancelTask(env, task->taskId_);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);

    task->taskState_ = ExecuteState::RUNNING;
    task->groupId_ = 1;
    taskManager.CancelTask(env, task->taskId_);
    TaskGroup* group = new TaskGroup(env);
    group->groupId_ = reinterpret_cast<uint64_t>(group);
    TaskGroupManager::GetInstance().StoreTaskGroup(group->groupId_, group);
    task->groupId_ = group->groupId_;
    taskManager.CancelTask(env, task->taskId_);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    TaskManager::GetInstance().RemoveTask(task->taskId_);
    TaskGroupManager::GetInstance().RemoveTaskGroup(group->groupId_);
    delete taskInfo;
    delete task;
    delete group;
}

HWTEST_F(NativeEngineTest, TaskpoolTest292, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    Task* task = new Task();
    TaskManager::GetInstance().StoreTask(task);
    task->env_ = env;
    task->taskType_ = TaskType::COMMON_TASK;
    task->taskState_ = ExecuteState::RUNNING;
    task->isMainThreadTask_ = false;
    napi_env runtimeEnv = nullptr;
    napi_create_runtime(env, &runtimeEnv);
    TaskManager::GetInstance().CancelTask(runtimeEnv, task->taskId_);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    TaskManager::GetInstance().RemoveTask(task->taskId_);
    delete task;
}

HWTEST_F(NativeEngineTest, TaskpoolTest293, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    Task* task = new Task();
    task->env_ = env;
    TaskManager::GetInstance().StoreTask(task);
    task->isMainThreadTask_ = true;
    CancelTaskMessage* message = new CancelTaskMessage(ExecuteState::RUNNING, task->taskId_);
    task->TriggerCancel(message);
    usleep(100000); // 100000: is sleep 100ms

    task->isMainThreadTask_ = false;
    CancelTaskMessage* message2 = new CancelTaskMessage(ExecuteState::RUNNING, task->taskId_);
    task->SetValid(false);
    task->TriggerCancel(message2);
    CancelTaskMessage* message3 = new CancelTaskMessage(ExecuteState::RUNNING, task->taskId_);
    task->SetValid(true);
    task->TriggerCancel(message3);
    uv_loop_t* loop = NapiHelper::GetLibUV(env);
    ConcurrentHelper::UvHandleInit(loop, task->onStartCancelSignal_, NativeEngineTest::foo);
    CancelTaskMessage* message4 = new CancelTaskMessage(ExecuteState::RUNNING, task->taskId_);
    task->TriggerCancel(message4);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    task->ReleaseData();
}

HWTEST_F(NativeEngineTest, TaskpoolTest294, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    Task* task = new Task();
    task->env_ = env;
    task->CancelInner(ExecuteState::RUNNING);
    task->CancelInner(ExecuteState::WAITING);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    delete task;
}

HWTEST_F(NativeEngineTest, TaskpoolTest295, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    Task* task = new Task();
    task->env_ = env;
    task->CancelInner(ExecuteState::RUNNING);
    task->CancelInner(ExecuteState::WAITING);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    delete task;
}

HWTEST_F(NativeEngineTest, TaskpoolTest296, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    Task* task = new Task();
    task->env_ = env;
    uv_async_t* req = new uv_async_t;
    req->data = nullptr;
    Task::Cancel(req);
    task->taskId_ = 1000; // 1000: test number
    CancelTaskMessage* message = new CancelTaskMessage(ExecuteState::RUNNING, task->taskId_);
    req->data = message;
    Task::Cancel(req);
    TaskManager::GetInstance().StoreTask(task);
    CancelTaskMessage* message2 = new CancelTaskMessage(ExecuteState::RUNNING, task->taskId_);
    req->data = message2;
    Task::Cancel(req);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    delete req;
}

HWTEST_F(NativeEngineTest, TaskpoolTest297, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    
    AsyncRunner* asyncRunner = new AsyncRunner();
    asyncRunner->DecreaseAsyncCount();
    uint64_t refCount = asyncRunner->DecreaseAsyncCount();
    ASSERT_TRUE(refCount == 0);
    delete asyncRunner;
}

HWTEST_F(NativeEngineTest, TaskpoolTest298, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);

    Task* task = new Task();
    task->SetValid(false);
    task->taskType_ = TaskType::ASYNCRUNNER_TASK;
    bool res = task->ShouldDeleteTask();
    ASSERT_TRUE(res);
    
    AsyncRunner* asyncRunner = new AsyncRunner();
    asyncRunner->asyncRunnerId_ = reinterpret_cast<uint64_t>(asyncRunner);
    AsyncRunnerManager::GetInstance().StoreAsyncRunner(asyncRunner->asyncRunnerId_, asyncRunner);
    task->asyncRunnerId_ = asyncRunner->asyncRunnerId_;
    res = task->ShouldDeleteTask();
    ASSERT_TRUE(res);
    delete task;
}

HWTEST_F(NativeEngineTest, TaskpoolTest299, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    SequenceRunnerManager& sequenceRunnerManager = SequenceRunnerManager::GetInstance();
    SequenceRunner* seqRunner = new SequenceRunner();
    seqRunner->seqRunnerId_ = reinterpret_cast<uint64_t>(seqRunner);
    sequenceRunnerManager.StoreSequenceRunner(seqRunner->seqRunnerId_, seqRunner);
    Task* task = new Task();
    uint32_t taskId = TaskManager::GetInstance().CalculateTaskId(reinterpret_cast<uint64_t>(task));
    task->seqRunnerId_ = seqRunner->seqRunnerId_;
    task->taskId_ = taskId;
    task->taskType_ = TaskType::SEQRUNNER_TASK;
    sequenceRunnerManager.AddTaskToSeqRunner(seqRunner->seqRunnerId_, task);
    sequenceRunnerManager.RemoveWaitingTask(task);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    delete task;
}

HWTEST_F(NativeEngineTest, TaskpoolTest300, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    SequenceRunnerManager& sequenceRunnerManager = SequenceRunnerManager::GetInstance();
    Task* task = new Task();
    task->seqRunnerId_ = 1;
    sequenceRunnerManager.RemoveWaitingTask(task);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    delete task;
}

HWTEST_F(NativeEngineTest, TaskpoolTest301, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    SequenceRunner* seqRunner = new SequenceRunner();
    seqRunner->seqRunnerId_ = reinterpret_cast<uint64_t>(seqRunner);
    Task* task = new Task();
    seqRunner->RemoveWaitingTask(task);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    Task* task2 = new Task();
    seqRunner->AddTask(task2);
    seqRunner->RemoveWaitingTask(task);
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    delete task;
    delete task2;
}

HWTEST_F(NativeEngineTest, TaskpoolTest302, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    bool isFalse = SequenceRunnerManager::GetInstance().FindRunnerAndRef(302);
    ASSERT_FALSE(isFalse);
    SequenceRunner* seqRunner = new SequenceRunner();
    seqRunner->DecreaseSeqCount();
    isFalse = seqRunner->DecreaseSeqCount();
    ASSERT_FALSE(isFalse);
}

HWTEST_F(NativeEngineTest, TaskpoolTest303, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    SequenceRunner* seqRunner = new SequenceRunner();
    seqRunner->seqRunnerId_ = reinterpret_cast<uint64_t>(seqRunner);
    SequenceRunnerManager::GetInstance().StoreSequenceRunner(seqRunner->seqRunnerId_, seqRunner);
    Task* task = new Task();
    task->seqRunnerId_ = seqRunner->seqRunnerId_;
    bool isFalse = SequenceRunnerManager::GetInstance().TriggerSeqRunner(env, task);
    ASSERT_FALSE(isFalse);
    delete task;
}

HWTEST_F(NativeEngineTest, TaskpoolTest304, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    Task* task = new Task();
    task->seqRunnerId_ = 304;
    task->taskType_ = TaskType::SEQRUNNER_TASK;
    Task::CleanupHookFunc(task);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    delete task;
}

HWTEST_F(NativeEngineTest, TaskpoolTest305, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    Task* task = new Task();
    task->seqRunnerId_ = 305;
    task->env_ = env;
    task->taskType_ = TaskType::SEQRUNNER_TASK;
    napi_value obj = NapiHelper::CreateObject(env);
    task->taskRef_ = NapiHelper::CreateReference(env, obj, 1);
    task->CancelInner(ExecuteState::CANCELED);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    delete task;
}

HWTEST_F(NativeEngineTest, TaskpoolTest306, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    Task* task = new Task();
    task->taskId_ = 306;
    task->SetHasDependency(true);
    task->env_ = env;
    task->CancelInner(ExecuteState::DELAYED);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    delete task;
}

HWTEST_F(NativeEngineTest, TaskpoolTest307, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    uint32_t taskId = 307;
    std::set<uint32_t> taskIds{1};
    TaskManager::GetInstance().StoreTaskDependency(taskId, taskIds);
    TaskManager::GetInstance().ClearDependentTask(taskId);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest308, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    uint32_t taskId = 308;
    uint32_t taskId2 = 1308;
    std::set<uint32_t> taskIds{taskId2};
    TaskManager::GetInstance().StoreDependentTaskInfo(taskIds, taskId);
    TaskManager::GetInstance().ClearDependentTask(taskId2);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest309, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    uint32_t taskId = 309;
    Task* task = new Task();
    task->env_ = env;
    napi_value obj = NapiHelper::CreateObject(env);
    task->taskRef_ = NapiHelper::CreateReference(env, obj, 1);
    TaskManager::GetInstance().StoreTask(task);
    std::set<uint32_t> taskIds{taskId};
    TaskManager::GetInstance().StoreDependentTaskInfo(taskIds, task->taskId_);
    TaskManager::GetInstance().ClearDependentTask(taskId);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest310, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    uint32_t taskId = 310;
    Task* task = new Task();
    task->env_ = env;
    napi_value obj = NapiHelper::CreateObject(env);
    task->taskRef_ = NapiHelper::CreateReference(env, obj, 1);
    task->currentTaskInfo_ = new TaskInfo();
    TaskManager::GetInstance().StoreTask(task);
    std::set<uint32_t> taskIds{taskId};
    TaskManager::GetInstance().StoreDependentTaskInfo(taskIds, task->taskId_);
    TaskManager::GetInstance().ClearDependentTask(taskId);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest311, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    uint32_t taskId = 311;
    uint32_t taskId2 = 1311;
    std::set<uint32_t> taskIds{taskId2};
    TaskManager::GetInstance().StoreDependentTaskInfo(taskIds, taskId);
    TaskManager::GetInstance().RemoveDependentTaskInfo(taskId2, taskId);
    TaskManager::GetInstance().ClearDependentTask(taskId2);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest312, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value napiTask = CreateTaskObject(env, TaskType::TASK, ExecuteState::FINISHED, true);
    napi_value argv[] = {napiTask};
    napi_value result = NativeEngineTest::Execute(env, argv, 1);
    ASSERT_TRUE(result != nullptr);
    Task* task = nullptr;
    napi_unwrap(env, napiTask, reinterpret_cast<void**>(&task));
    TaskManager::GetInstance().RemoveTask(task->taskId_);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);

    napi_value napiTask2 = CreateTaskObject(env, TaskType::TASK, ExecuteState::CANCELED, true);
    Task* task2 = nullptr;
    napi_unwrap(env, napiTask2, reinterpret_cast<void**>(&task2));
    task2->isCancelToFinish_ = true;
    napi_value argv2[] = {napiTask2};
    result = NativeEngineTest::Execute(env, argv2, 1);
    ASSERT_TRUE(result != nullptr);
    TaskManager::GetInstance().RemoveTask(task2->taskId_);
}

HWTEST_F(NativeEngineTest, TaskpoolTest313, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    napi_value result = nullptr;
    napi_create_string_utf8(env, "test313", NAPI_AUTO_LENGTH, &result);
    napi_value error = TaskManager::GetInstance().CancelError(env, 0, nullptr, result);
    ASSERT_TRUE(NapiHelper::IsNotUndefined(env, error));

    napi_value obj = NapiHelper::CreateObject(env);
    napi_value result2 = nullptr;
    napi_create_string_utf8(env, "this is error", NAPI_AUTO_LENGTH, &result2);
    napi_set_named_property(env, obj, "error", result2);
    error = TaskManager::GetInstance().CancelError(env, 0, nullptr, obj);
    ASSERT_TRUE(NapiHelper::IsNotUndefined(env, error));
}

HWTEST_F(NativeEngineTest, TaskpoolTest314, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    TaskManager &taskManager = TaskManager::GetInstance();
    Task* task = new Task();
    uint32_t taskId = taskManager.CalculateTaskId(reinterpret_cast<uint64_t>(task));
    task->taskId_ = taskId;
    task->isValid_ = false;
    task->asyncTaskPriority_ = Priority::MEDIUM;
    void* data = reinterpret_cast<void*>(task);
    NativeEngineTest::PerformTask(env, data);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest315, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    NativeEngineTest::GetIdleTaskByPriority(env);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest316, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    NativeEngineTest::WorkerRunningScope(env);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest317, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    NativeEngineTest::SetNonIdleTaskNum(0);
    void* data = nullptr;
    NativeEngineTest::PerformTask(env, data);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest318, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    AsyncRunnerManager &asyncRunnerManager = AsyncRunnerManager::GetInstance();
    TaskManager &taskManager = TaskManager::GetInstance();
    Task* task = new Task();
    task->taskId_ = taskManager.CalculateTaskId(reinterpret_cast<uint64_t>(task));
    task->taskState_ = ExecuteState::RUNNING;
    asyncRunnerManager.CancelAsyncRunnerTask(env, task);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    delete task;

    Task* task2 = new Task();
    task2->taskId_ = taskManager.CalculateTaskId(reinterpret_cast<uint64_t>(task2));
    task2->taskState_ = ExecuteState::WAITING;
    asyncRunnerManager.CancelAsyncRunnerTask(env, task2);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    delete task2;

    Task* task3 = new Task();
    task3->taskId_ = taskManager.CalculateTaskId(reinterpret_cast<uint64_t>(task3));
    TaskInfo* taskInfo = new TaskInfo();
    task3->currentTaskInfo_ = taskInfo;
    task3->taskType_ = TaskType::ASYNCRUNNER_TASK;
    task3->taskState_ = ExecuteState::WAITING;
    asyncRunnerManager.CancelAsyncRunnerTask(env, task3);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    delete task3;
    delete taskInfo;
}

HWTEST_F(NativeEngineTest, TaskpoolTest319, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    AsyncRunnerManager &asyncRunnerManager = AsyncRunnerManager::GetInstance();
    TaskManager &taskManager = TaskManager::GetInstance();

    Task* task = new Task();
    TaskInfo* taskInfo = new TaskInfo();
    task->currentTaskInfo_ = taskInfo;
    task->taskType_ = TaskType::ASYNCRUNNER_TASK;
    task->taskState_ = ExecuteState::WAITING;
    taskManager.StoreTask(task);
    NativeEngineTest::EnqueueTaskIdToQueue(reinterpret_cast<void*>(task));
    asyncRunnerManager.CancelAsyncRunnerTask(env, task);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    taskManager.RemoveTask(task->taskId_);
    delete task;

    AsyncRunner* asyncRunner = new AsyncRunner();
    asyncRunner->asyncRunnerId_ = reinterpret_cast<uint64_t>(asyncRunner);
    asyncRunnerManager.StoreAsyncRunner(asyncRunner->asyncRunnerId_, asyncRunner);
    Task* task2 = new Task();
    TaskInfo* taskInfo2 = new TaskInfo();
    task2->currentTaskInfo_ = taskInfo2;
    task2->taskType_ = TaskType::ASYNCRUNNER_TASK;
    task2->taskState_ = ExecuteState::WAITING;
    taskManager.StoreTask(task2);
    task2->asyncRunnerId_ = asyncRunner->asyncRunnerId_;
    NativeEngineTest::EnqueueTaskIdToQueue(reinterpret_cast<void*>(task2));
    asyncRunnerManager.CancelAsyncRunnerTask(env, task2);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    taskManager.RemoveTask(task2->taskId_);
}

HWTEST_F(NativeEngineTest, TaskpoolTest320, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    AsyncRunnerManager &asyncRunnerManager = AsyncRunnerManager::GetInstance();
    TaskManager &taskManager = TaskManager::GetInstance();
    Task* task = new Task();
    asyncRunnerManager.RemoveWaitingTask(task);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);

    AsyncRunner* asyncRunner = new AsyncRunner();
    asyncRunner->asyncRunnerId_ = reinterpret_cast<uint64_t>(asyncRunner);
    asyncRunnerManager.StoreAsyncRunner(asyncRunner->asyncRunnerId_, asyncRunner);
    Task* task2 = new Task();
    task2->asyncRunnerId_ = asyncRunner->asyncRunnerId_;
    asyncRunnerManager.RemoveWaitingTask(task2);
    exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
   
    delete task;
    delete task2;
}

HWTEST_F(NativeEngineTest, TaskpoolTest321, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    SequenceRunnerManager& sequenceRunnerManager = SequenceRunnerManager::GetInstance();
    SequenceRunner* seqRunner = new SequenceRunner();
    seqRunner->seqRunnerId_ = reinterpret_cast<uint64_t>(seqRunner);
    sequenceRunnerManager.StoreSequenceRunner(seqRunner->seqRunnerId_, seqRunner);
    Task* task = new Task();
    task->seqRunnerId_ = seqRunner->seqRunnerId_;
    sequenceRunnerManager.RemoveWaitingTask(task);
    sequenceRunnerManager.SequenceRunnerDestructor(seqRunner);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    delete task;
}

HWTEST_F(NativeEngineTest, TaskpoolTest322, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    SequenceRunnerManager& sequenceRunnerManager = SequenceRunnerManager::GetInstance();
    napi_value thisVar = nullptr;
    size_t argc = 1;
    uint32_t priority = 1;
    std::string name = "TaskpoolTest322";
    SequenceRunner* seqRunner = sequenceRunnerManager.CreateOrGetGlobalRunner(env, thisVar, argc, name, priority);
    seqRunner->seqRunnerId_ = reinterpret_cast<uint64_t>(seqRunner);
    Task* task = new Task();
    task->seqRunnerId_ = seqRunner->seqRunnerId_;
    seqRunner->AddTask(task);
    sequenceRunnerManager.CreateOrGetGlobalRunner(env, thisVar, argc, name, priority);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
    seqRunner->RemoveWaitingTask(task);
    sequenceRunnerManager.StoreSequenceRunner(seqRunner->seqRunnerId_, seqRunner);
    sequenceRunnerManager.SequenceRunnerDestructor(seqRunner);
    delete task;
}

HWTEST_F(NativeEngineTest, TaskpoolTest323, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    ExceptionScope scope(env);
    TaskManager &taskManager = TaskManager::GetInstance();
    uint32_t taskId = taskManager.CalculateTaskId(0);
    ASSERT_TRUE(taskId != 0);
}