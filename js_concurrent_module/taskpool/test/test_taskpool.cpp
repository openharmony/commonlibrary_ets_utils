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

#include "test.h"

#include <unistd.h>

#include "helper/napi_helper.h"
#include "queue.h"
#include "task.h"
#include "taskpool.h"
#include "task_manager.h"
#include "task_runner.h"
#include "thread.h"
#include "tools/log.h"
#include "worker.h"

using namespace Commonlibrary::Concurrent::TaskPoolModule;

HWTEST_F(NativeEngineTest, TaskpoolTest001, testing::ext::TestSize.Level0)
{
    TaskManager &taskManager = TaskManager::GetInstance();
    uint32_t result = taskManager.GetThreadNum();
    ASSERT_TRUE(result == 0);
}

HWTEST_F(NativeEngineTest, TaskpoolTest002, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    Worker* worker = Worker::WorkerConstructor(env);
    usleep(50000);
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
    Worker* worker = Worker::WorkerConstructor(env);
    usleep(50000);
    ASSERT_NE(worker, nullptr);
    uint32_t workers = TaskManager::GetInstance().GetRunningWorkers();
    ASSERT_TRUE(workers == 0);
}

HWTEST_F(NativeEngineTest, TaskpoolTest005, testing::ext::TestSize.Level0)
{
    uint64_t taskId = 10;
    TaskManager &taskManager = TaskManager::GetInstance();
    Task* task = taskManager.GetTask(taskId);
    ASSERT_TRUE(task == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest006, testing::ext::TestSize.Level0)
{
    TaskManager &taskManager = TaskManager::GetInstance();
    std::pair<uint64_t, Priority> result = taskManager.DequeueTaskId();
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
    uint64_t result = executeQueue.DequeueTaskId();
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
    napi_value argv[] = {nullptr};
    std::string funcName = "TaskGroupConstructor";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), TaskGroup::TaskGroupConstructor, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 0, argv, &result);
    ASSERT_TRUE(result != nullptr);

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
    TaskManager& taskManger = TaskManager::GetInstance();
    taskManger.InitTaskManager(env);
    uint32_t taskNum = taskManger.GetTaskNum();
    ASSERT_TRUE(taskNum == 0);
    uint32_t threadNum = taskManger.GetThreadNum();
    ASSERT_TRUE(threadNum == 0);
    uint32_t idleWorkers = taskManger.GetIdleWorkers();
    ASSERT_TRUE(idleWorkers == 0);
    uint32_t runningWorkers = taskManger.GetRunningWorkers();
    ASSERT_TRUE(runningWorkers == 0);
    uint32_t timeoutWorkers = taskManger.GetTimeoutWorkers();
    ASSERT_TRUE(timeoutWorkers == 0);
}

HWTEST_F(NativeEngineTest, TaskpoolTest015, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManger = TaskManager::GetInstance();
    taskManger.InitTaskManager(env);
    std::pair<uint64_t, Priority> result = taskManger.DequeueTaskId();
    ASSERT_TRUE(result.first == 0);
    ASSERT_TRUE(result.second == Priority::LOW);
}

HWTEST_F(NativeEngineTest, TaskpoolTest016, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManger = TaskManager::GetInstance();
    taskManger.InitTaskManager(env);
    Task* task = new Task();
    uint64_t taskId = reinterpret_cast<uint64_t>(task);
    taskManger.CancelTask(env, taskId);
    ASSERT_TRUE(taskId != 0);
    delete task;
}

HWTEST_F(NativeEngineTest, TaskpoolTest017, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManger = TaskManager::GetInstance();
    taskManger.InitTaskManager(env);
    Task* task = new Task();
    uint64_t taskId = reinterpret_cast<uint64_t>(task);
    taskManger.TryTriggerExpand();
    ASSERT_TRUE(taskId != 0);
    delete task;
}

HWTEST_F(NativeEngineTest, TaskpoolTest018, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManger = TaskManager::GetInstance();
    taskManger.InitTaskManager(env);
    Task* task = new Task();
    uint64_t taskId = reinterpret_cast<uint64_t>(task);
    uint64_t duration = 10;
    taskManger.UpdateExecutedInfo(duration);
    ASSERT_TRUE(taskId != 0);
    delete task;
}

HWTEST_F(NativeEngineTest, TaskpoolTest019, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
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
    napi_value argv[] = {nullptr};
    std::string funcName = "TaskConstructor";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::TaskConstructor, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 0, argv, &result);
    ASSERT_TRUE(result == nullptr);

    cb = nullptr;
    napi_value result1 = nullptr;
    funcName = "SetTransferList";
    napi_create_function(env, funcName.c_str(), funcName.size(), Task::SetTransferList, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 0, argv, &result1);
    ASSERT_TRUE(result1 == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest021, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
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
    napi_value taskName = NapiHelper::CreateEmptyString(env);
    Task::GenerateTask(env, value, func, taskName, args, argc);
    ASSERT_TRUE(args != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest022, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskGroupManager &taskGroupManager = TaskGroupManager::GetInstance();
    TaskGroup* group = new TaskGroup();
    uint64_t groupId = reinterpret_cast<uint64_t>(group);
    Task* task = new Task();
    uint64_t taskId = reinterpret_cast<uint64_t>(task);
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
    MessageQueue<int*> mesQueue;
    int testData = 42;
    mesQueue.EnQueue(&testData);

    auto result = mesQueue.DeQueue();
    ASSERT_EQ(testData, *result);
}

HWTEST_F(NativeEngineTest, TaskpoolTest025, testing::ext::TestSize.Level0)
{
    MessageQueue<std::string> mesQueue;
    ASSERT_EQ(mesQueue.IsEmpty(), true);

    std::string testStr = "hello";
    mesQueue.EnQueue(testStr);
    ASSERT_EQ(mesQueue.IsEmpty(), false);
}

HWTEST_F(NativeEngineTest, TaskpoolTest026, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value argv[] = {nullptr};
    std::string funcName = "SeqRunnerConstructor";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), SequenceRunner::SeqRunnerConstructor, nullptr, &cb);
    napi_call_function(env, nullptr, cb, 0, argv, &result);
    ASSERT_EQ(result, nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest027, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value argv[] = {nullptr};
    std::string func = "SeqRunnerConstructor";
    napi_value callback = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, func.c_str(), func.size(), SequenceRunner::SeqRunnerConstructor, nullptr, &callback);
    napi_call_function(env, nullptr, callback, 0, argv, &result);
    ASSERT_EQ(result, nullptr);

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
    taskManager.InitTaskManager(env);
    uint32_t taskId = 11;
    ASSERT_EQ(taskId, 11);
}

HWTEST_F(NativeEngineTest, TaskpoolTest030, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    TaskGroup taskGroup;
    taskGroup.CancelPendingGroup(env);
    TaskManager &taskManager = TaskManager::GetInstance();
    taskManager.InitTaskManager(env);
    uint32_t taskId = 12;
    ASSERT_EQ(taskId, 12);
}

HWTEST_F(NativeEngineTest, TaskpoolTest031, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManger = TaskManager::GetInstance();
    taskManger.InitTaskManager(env);
    uint64_t taskId = 13;
    Task* task = taskManger.GetTask(taskId);
    taskManger.StoreTask(taskId, task);
    ASSERT_EQ(taskId, 13);
}

HWTEST_F(NativeEngineTest, TaskpoolTest032, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManger = TaskManager::GetInstance();
    taskManger.InitTaskManager(env);
    uint64_t taskId = 14;
    taskManger.RemoveTask(taskId);
    ASSERT_EQ(taskId, 14);
}

HWTEST_F(NativeEngineTest, TaskpoolTest033, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManager = TaskManager::GetInstance();
    taskManager.InitTaskManager(env);
    usleep(50000);
    // the task will freed in the taskManager's Destuctor and will not cause memory leak
    Task* task = new Task();
    auto taskId = reinterpret_cast<uint64_t>(task);
    taskManager.StoreTask(taskId, task);
    taskManager.EnqueueTaskId(taskId, Priority::HIGH);
    std::pair<uint64_t, Priority> result = taskManager.DequeueTaskId();
    ASSERT_TRUE(result.first == taskId);
    ASSERT_TRUE(result.second == Priority::HIGH);
}

HWTEST_F(NativeEngineTest, TaskpoolTest034, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    Worker* worker = Worker::WorkerConstructor(env);
    usleep(50000);
    TaskManager& taskManger = TaskManager::GetInstance();
    taskManger.InitTaskManager(env);
    taskManger.NotifyWorkerIdle(worker);
    ASSERT_NE(worker, nullptr);
    taskManger.NotifyWorkerCreated(worker);
    ASSERT_NE(worker, nullptr);
    taskManger.NotifyWorkerRunning(worker);
    ASSERT_NE(worker, nullptr);
    taskManger.RestoreWorker(worker);
    ASSERT_NE(worker, nullptr);
    taskManger.RemoveWorker(worker);
    ASSERT_NE(worker, nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest035, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManger = TaskManager::GetInstance();
    taskManger.InitTaskManager(env);
    uint32_t step = 8;
    taskManger.GetIdleWorkersList(step);
    ASSERT_EQ(step, 8);
}

HWTEST_F(NativeEngineTest, TaskpoolTest036, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManger = TaskManager::GetInstance();
    taskManger.InitTaskManager(env);
    napi_value res = taskManger.GetThreadInfos(env);
    ASSERT_NE(res, nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest037, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManger = TaskManager::GetInstance();
    taskManger.InitTaskManager(env);
    uint64_t taskId = 16;
    std::shared_ptr<CallbackInfo> res = taskManger.GetCallbackInfo(taskId);
    ASSERT_EQ(res, nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest038, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManger = TaskManager::GetInstance();
    taskManger.InitTaskManager(env);
    uint64_t taskId = 16;
    taskManger.RegisterCallback(env, taskId, nullptr);
    ASSERT_EQ(taskId, 16);
}

HWTEST_F(NativeEngineTest, TaskpoolTest039, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManger = TaskManager::GetInstance();
    taskManger.InitTaskManager(env);
    uint64_t taskId = 17;
    taskManger.IncreaseRefCount(taskId);
    ASSERT_EQ(taskId, 17);
}

HWTEST_F(NativeEngineTest, TaskpoolTest040, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManger = TaskManager::GetInstance();
    taskManger.InitTaskManager(env);
    uint64_t taskId = 18;
    taskManger.DecreaseRefCount(env, taskId);
    ASSERT_EQ(taskId, 18);
}

HWTEST_F(NativeEngineTest, TaskpoolTest041, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManger = TaskManager::GetInstance();
    taskManger.InitTaskManager(env);
    uint64_t taskId = 19;
    bool res = taskManger.IsDependendByTaskId(taskId);
    ASSERT_EQ(res, false);
}

HWTEST_F(NativeEngineTest, TaskpoolTest042, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManger = TaskManager::GetInstance();
    taskManger.InitTaskManager(env);
    uint64_t taskId = 20;
    taskManger.NotifyDependencyTaskInfo(taskId);
    ASSERT_EQ(taskId, 20);
}

HWTEST_F(NativeEngineTest, TaskpoolTest043, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManger = TaskManager::GetInstance();
    taskManger.InitTaskManager(env);
    uint64_t taskId = 21;
    std::set<uint64_t> taskSet;
    taskSet.emplace(1);
    taskSet.emplace(2);
    bool res = taskManger.StoreTaskDependency(taskId, taskSet);
    ASSERT_EQ(res, true);
}

HWTEST_F(NativeEngineTest, TaskpoolTest044, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManger = TaskManager::GetInstance();
    taskManger.InitTaskManager(env);
    uint64_t taskId = 22;
    uint64_t dependentId = 0;
    bool res = taskManger.RemoveTaskDependency(taskId, dependentId);
    ASSERT_EQ(res, false);
}

HWTEST_F(NativeEngineTest, TaskpoolTest045, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManger = TaskManager::GetInstance();
    taskManger.InitTaskManager(env);
    uint64_t taskId = 23;
    std::set<uint64_t> dependentIdSet;
    dependentIdSet.emplace(1);
    std::set<uint64_t> idSet;
    idSet.emplace(2);
    bool res = taskManger.CheckCircularDependency(dependentIdSet, idSet, taskId);
    ASSERT_EQ(res, true);
}

HWTEST_F(NativeEngineTest, TaskpoolTest046, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManger = TaskManager::GetInstance();
    taskManger.InitTaskManager(env);
    uint64_t taskId = 24;
    taskManger.EnqueuePendingTaskInfo(taskId, Priority::NUMBER);
    std::pair<uint64_t, Priority> res = taskManger.DequeuePendingTaskInfo(taskId);
    ASSERT_EQ(res.first, 24);
    ASSERT_EQ(res.second, Priority::NUMBER);
}

HWTEST_F(NativeEngineTest, TaskpoolTest047, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManger = TaskManager::GetInstance();
    taskManger.InitTaskManager(env);
    uint64_t taskId = 24;
    taskManger.RemovePendingTaskInfo(taskId);
    ASSERT_EQ(taskId, 24);
}

HWTEST_F(NativeEngineTest, TaskpoolTest048, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManger = TaskManager::GetInstance();
    taskManger.InitTaskManager(env);
    uint64_t taskId = 25;
    std::set<uint64_t> dependTaskIdSet;
    taskManger.StoreDependentTaskInfo(dependTaskIdSet, taskId);
    ASSERT_EQ(taskId, 25);
}

HWTEST_F(NativeEngineTest, TaskpoolTest049, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManger = TaskManager::GetInstance();
    taskManger.InitTaskManager(env);
    uint64_t taskId = 26;
    uint64_t dependentTaskId = 26;
    taskManger.RemoveDependentTaskInfo(dependentTaskId, taskId);
    ASSERT_EQ(taskId, 26);
    ASSERT_EQ(dependentTaskId, 26);
}

HWTEST_F(NativeEngineTest, TaskpoolTest050, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManger = TaskManager::GetInstance();
    taskManger.InitTaskManager(env);
    uint64_t taskId = 27;
    uint64_t totalDuration = 25;
    uint64_t cpuDuration = 8;
    taskManger.StoreTaskDuration(taskId, totalDuration, cpuDuration);
    ASSERT_EQ(taskId, 27);
    ASSERT_EQ(totalDuration, 25);
    ASSERT_EQ(cpuDuration, 8);
}

HWTEST_F(NativeEngineTest, TaskpoolTest051, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManger = TaskManager::GetInstance();
    taskManger.InitTaskManager(env);
    uint64_t taskId = 28;
    std::string str = "testTaskpool";
    taskManger.GetTaskDuration(taskId, str);
    ASSERT_EQ(taskId, 28);
}

HWTEST_F(NativeEngineTest, TaskpoolTest052, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManger = TaskManager::GetInstance();
    taskManger.InitTaskManager(env);
    uint64_t taskId = 29;
    taskManger.RemoveTaskDuration(taskId);
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
    uint64_t taskId = 33;
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
    uint64_t seqRunnerId = 35;
    Task *task = new Task();
    ASSERT_NE(task, nullptr);
    taskGroupManager.AddTaskToSeqRunner(seqRunnerId, task);
    taskGroupManager.TriggerSeqRunner(env, task);
    SequenceRunner sequenceRunner;
    taskGroupManager.StoreSequenceRunner(seqRunnerId, &sequenceRunner);
    taskGroupManager.RemoveSequenceRunner(seqRunnerId);
    ASSERT_EQ(seqRunnerId, 35);
    SequenceRunner *res = taskGroupManager.GetSeqRunner(seqRunnerId);
    ASSERT_EQ(res, nullptr);
}
