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
#include "utils/log.h"
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
    TaskManager &taskManager = TaskManager::GetInstance();
    uint32_t taskId = taskManager.GenerateTaskId();
    ASSERT_TRUE(taskId == 1);
}

HWTEST_F(NativeEngineTest, TaskpoolTest004, testing::ext::TestSize.Level0)
{
    TaskManager &taskManager = TaskManager::GetInstance();
    uint32_t executeId = taskManager.GenerateExecuteId();
    ASSERT_TRUE(executeId == 1);
}

HWTEST_F(NativeEngineTest, TaskpoolTest005, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    napi_value taskClass = nullptr;
    napi_value result = TaskPool::InitTaskPool(env, taskClass);
    usleep(50000);
    ASSERT_TRUE(result == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest006, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    Worker* worker = Worker::WorkerConstructor(env);
    usleep(50000);
    ASSERT_NE(worker, nullptr);
    uint32_t workers = TaskManager::GetInstance().GetRunningWorkers();
    ASSERT_TRUE(workers == 0);
}

HWTEST_F(NativeEngineTest, TaskpoolTest007, testing::ext::TestSize.Level0)
{
    uint32_t executeId = 10;
    TaskManager &taskManager = TaskManager::GetInstance();
    TaskInfo* taskInfo = taskManager.GetTaskInfo(executeId);
    ASSERT_TRUE(taskInfo == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest008, testing::ext::TestSize.Level0)
{
    uint32_t executeId = 0;
    TaskManager &taskManager = TaskManager::GetInstance();
    TaskInfo *result = taskManager.PopTaskInfo(executeId);
    ASSERT_TRUE(result == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest009, testing::ext::TestSize.Level0)
{
    uint32_t executeId = 0;
    TaskManager &taskManager = TaskManager::GetInstance();
    bool result = taskManager.UpdateExecuteState(executeId, ExecuteState::NOT_FOUND);
    ASSERT_TRUE(!result);
    result = taskManager.UpdateExecuteState(executeId, ExecuteState::RUNNING);
    ASSERT_TRUE(!result);
    result = taskManager.UpdateExecuteState(executeId, ExecuteState::WAITING);
    ASSERT_TRUE(!result);
    result = taskManager.UpdateExecuteState(executeId, ExecuteState::CANCELED);
    ASSERT_TRUE(!result);
}

HWTEST_F(NativeEngineTest, TaskpoolTest010, testing::ext::TestSize.Level0)
{
    TaskManager &taskManager = TaskManager::GetInstance();
    std::pair<uint32_t, Priority> result = taskManager.DequeueExecuteId();
    ASSERT_TRUE(result.first == 0);
    ASSERT_TRUE(result.second == Priority::HIGH);
}

HWTEST_F(NativeEngineTest, TaskpoolTest011, testing::ext::TestSize.Level0)
{
    TaskManager &taskManager = TaskManager::GetInstance();
    uint32_t result = taskManager.GetTaskNum();
    ASSERT_TRUE(result == 0);
}

HWTEST_F(NativeEngineTest, TaskpoolTest012, testing::ext::TestSize.Level0)
{
    ExecuteQueue executeQueue;
    uint32_t result = executeQueue.DequeueExecuteId();
    ASSERT_TRUE(result == 0);
}

HWTEST_F(NativeEngineTest, TaskpoolTest013, testing::ext::TestSize.Level0)
{
    ExecuteQueue executeQueue;
    bool result = executeQueue.IsEmpty();
    ASSERT_TRUE(result);
}

HWTEST_F(NativeEngineTest, TaskpoolTest014, testing::ext::TestSize.Level0)
{
    ExecuteQueue executeQueue;
    uint32_t result = executeQueue.GetTaskNum();
    ASSERT_TRUE(result == 0);
}

HWTEST_F(NativeEngineTest, TaskpoolTest015, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManger = TaskManager::GetInstance();
    taskManger.InitTaskManager(env);
    uint32_t executeId = taskManger.GenerateExecuteId();
    ExecuteQueue executeQueue;
    bool result = executeQueue.IsEmpty();
    ASSERT_TRUE(result);
    uint32_t taskNum = executeQueue.GetTaskNum();
    ASSERT_TRUE(taskNum == 0);
    executeQueue.EnqueueExecuteId(executeId);
    result = executeQueue.IsEmpty();
    ASSERT_TRUE(!result);
    taskNum = executeQueue.GetTaskNum();
    ASSERT_TRUE(taskNum != 0);
    uint32_t dequeueExecuteId = executeQueue.DequeueExecuteId();
    ASSERT_TRUE(dequeueExecuteId == executeId);
    result = executeQueue.IsEmpty();
    ASSERT_TRUE(result);
    executeQueue.EnqueueExecuteId(executeId);
    executeQueue.EnqueueExecuteId(executeId);
    taskNum = executeQueue.GetTaskNum();
    ASSERT_TRUE(taskNum == 2);
    dequeueExecuteId = executeQueue.DequeueExecuteId();
    ASSERT_TRUE(dequeueExecuteId == executeId);
    taskNum = executeQueue.GetTaskNum();
    ASSERT_TRUE(taskNum == 1);
    dequeueExecuteId = executeQueue.DequeueExecuteId();
    ASSERT_TRUE(dequeueExecuteId == executeId);
    taskNum = executeQueue.GetTaskNum();
    ASSERT_TRUE(taskNum == 0);
}

napi_callback_info napi_create_cbinfo(napi_env env,
                                      size_t* argc,
                                      NativeValue** argv)
{
    NativeCallbackInfo* info = new NativeCallbackInfo;

    if (argv != nullptr && argc != nullptr) {
        info->argc = *argc;
        info->argv = argv;
        return reinterpret_cast<napi_callback_info>(info);
    }
    return nullptr;
}

HWTEST_F(NativeEngineTest, TaskpoolTest016, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    size_t argc = 0;
    NativeValue* argv0[] = {nullptr};
    napi_callback_info cbinfo = napi_create_cbinfo(env, &argc, argv0);
    napi_value result = TaskGroup::TaskGroupConstructor(env, cbinfo);
    ASSERT_TRUE(result == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest017, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    size_t argc = 0;
    NativeValue* argv0[] = {nullptr};
    napi_callback_info cbinfo = napi_create_cbinfo(env, &argc, argv0);
    napi_value result = TaskGroup::TaskGroupConstructor(env, cbinfo);
    ASSERT_TRUE(result == nullptr);
    size_t argc1 = 10;
    NativeValue* argv1[] = {nullptr};
    napi_callback_info cbinfo1 = napi_create_cbinfo(env, &argc1, argv1);
    napi_value result1 = TaskGroup::AddTask(env, cbinfo1);
    ASSERT_TRUE(result1 == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest018, testing::ext::TestSize.Level0)
{
    uint32_t number = 10;
    napi_env env = reinterpret_cast<napi_env>(engine_);
    napi_value value = NapiHelper::CreateUint32(env, number);
    napi_value result = TaskPool::InitTaskPool(env, value);
    usleep(50000);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest019, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManger = TaskManager::GetInstance();
    taskManger.InitTaskManager(env);
    uint32_t taskId = taskManger.GenerateTaskId();
    napi_value task = NapiHelper::CreateUint32(env, taskId);
    uint32_t executeId = taskManger.GenerateExecuteId();
    ASSERT_TRUE(executeId == 3);
    TaskInfo* taskInfo = taskManger.GenerateTaskInfoFromTask(env, task, executeId);
    ASSERT_TRUE(taskInfo == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest020, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManger = TaskManager::GetInstance();
    taskManger.InitTaskManager(env);
    uint32_t taskId = taskManger.GenerateTaskId();
    uint32_t executeId = taskManger.GenerateExecuteId();
    uint32_t taskId1 = 10;
    uint32_t executeId1 = 20;
    taskManger.StoreRunningInfo(taskId, executeId);
    taskManger.StoreRunningInfo(taskId1, executeId1);
    taskManger.PopRunningInfo(taskId, executeId);
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

napi_value TestFunction(napi_env env)
{
    napi_value result = nullptr;
    const char* message = "test taskpool";
    size_t length = strlen(message);
    napi_create_string_utf8(env, message, length, &result);
    return result;
}

HWTEST_F(NativeEngineTest, TaskpoolTest021, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManger = TaskManager::GetInstance();
    taskManger.InitTaskManager(env);
    uint32_t taskId = taskManger.GenerateTaskId();
    uint32_t executeId = taskManger.GenerateExecuteId();
    napi_value args = NapiHelper::CreateUint32(env, taskId);
    napi_value task = nullptr;
    napi_value function = NapiHelper::GetNameProperty(env, task, FUNCTION_STR);
    napi_value function1 = TestFunction(env);
    TaskInfo* taskInfo = taskManger.GenerateTaskInfo(env, function, args, taskId, executeId);
    ASSERT_TRUE(taskInfo == nullptr);
    taskInfo = taskManger.GenerateTaskInfo(env, function1, args, taskId, executeId);
    ASSERT_TRUE(taskInfo == nullptr);
    TaskInfo* taskInfo1 = taskManger.GetTaskInfo(executeId);
    ASSERT_TRUE(taskInfo1 == nullptr);
    TaskInfo* taskInfo2 = taskManger.PopTaskInfo(executeId);
    ASSERT_TRUE(taskInfo2 == nullptr);
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

HWTEST_F(NativeEngineTest, TaskpoolTest022, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManger = TaskManager::GetInstance();
    taskManger.InitTaskManager(env);
    uint32_t executeId = taskManger.GenerateExecuteId();
    taskManger.AddExecuteState(executeId);
    bool result = taskManger.UpdateExecuteState(executeId, ExecuteState::RUNNING);
    ASSERT_TRUE(result);
    result = taskManger.UpdateExecuteState(executeId, ExecuteState::CANCELED);
    ASSERT_TRUE(result);
    result = taskManger.UpdateExecuteState(executeId, ExecuteState::NOT_FOUND);
    ASSERT_TRUE(result);
    result = taskManger.UpdateExecuteState(executeId, ExecuteState::WAITING);
    ASSERT_TRUE(result);
    taskManger.RemoveExecuteState(executeId);
    ASSERT_TRUE(executeId == 6);
    result = taskManger.UpdateExecuteState(executeId, ExecuteState::RUNNING);
    ASSERT_TRUE(!result);
    result = taskManger.UpdateExecuteState(executeId, ExecuteState::CANCELED);
    ASSERT_TRUE(!result);
    result = taskManger.UpdateExecuteState(executeId, ExecuteState::NOT_FOUND);
    ASSERT_TRUE(!result);
    result = taskManger.UpdateExecuteState(executeId, ExecuteState::WAITING);
    ASSERT_TRUE(!result);
}

HWTEST_F(NativeEngineTest, TaskpoolTest023, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManger = TaskManager::GetInstance();
    taskManger.InitTaskManager(env);
    taskManger.PopTaskEnvInfo(env);
    uint32_t taskId = taskManger.GenerateTaskId();
    ASSERT_TRUE(taskId == 5);
    uint32_t executeId = taskManger.GenerateExecuteId();
    ASSERT_TRUE(executeId == 7);
}

HWTEST_F(NativeEngineTest, TaskpoolTest024, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManger = TaskManager::GetInstance();
    taskManger.InitTaskManager(env);
    uint32_t executeId = taskManger.GenerateExecuteId();
    taskManger.EnqueueExecuteId(executeId);
    ASSERT_TRUE(executeId == 8);
    std::pair<uint32_t, Priority> result = taskManger.DequeueExecuteId();
    ASSERT_TRUE(result.first == 0);
    ASSERT_TRUE(result.second == 0);
}

HWTEST_F(NativeEngineTest, TaskpoolTest025, testing::ext::TestSize.Level0)
{
    TaskGroupManager &taskGroupManager = TaskGroupManager::GetInstance();
    uint32_t groupExecuteId = taskGroupManager.GenerateGroupExecuteId();
    bool result = taskGroupManager.IsRunning(groupExecuteId);
    ASSERT_TRUE(!result);
}

HWTEST_F(NativeEngineTest, TaskpoolTest026, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManger = TaskManager::GetInstance();
    taskManger.InitTaskManager(env);
    uint32_t executeId = taskManger.GenerateExecuteId();
    taskManger.CancelTask(env, executeId);
    ASSERT_TRUE(executeId == 9);
}

HWTEST_F(NativeEngineTest, TaskpoolTest027, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManger = TaskManager::GetInstance();
    taskManger.InitTaskManager(env);
    uint32_t executeId = taskManger.GenerateExecuteId();
    taskManger.TryTriggerLoadBalance();
    ASSERT_TRUE(executeId == 10);
}

HWTEST_F(NativeEngineTest, TaskpoolTest028, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskManager& taskManger = TaskManager::GetInstance();
    taskManger.InitTaskManager(env);
    uint32_t executeId = taskManger.GenerateExecuteId();
    uint64_t duration = 10;
    taskManger.UpdateExecutedInfo(duration);
    ASSERT_TRUE(executeId == 11);
}

HWTEST_F(NativeEngineTest, TaskpoolTest029, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    size_t argc = 0;
    NativeValue* argv[] = {nullptr};
    napi_callback_info cbinfo = napi_create_cbinfo(env, &argc, argv);
    napi_value result = Task::TaskConstructor(env, cbinfo);
    ASSERT_TRUE(result == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest030, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    size_t argc = 0;
    NativeValue* argv[] = {nullptr};
    napi_callback_info cbinfo = napi_create_cbinfo(env, &argc, argv);
    napi_value result = Task::TaskConstructor(env, cbinfo);
    ASSERT_TRUE(result == nullptr);
    napi_value result1 = Task::SetTransferList(env, cbinfo);
    ASSERT_TRUE(result1 == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest031, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    size_t argc = 10;
    NativeValue* argv[] = {nullptr};
    napi_callback_info cbinfo = napi_create_cbinfo(env, &argc, argv);
    napi_value result = Task::TaskConstructor(env, cbinfo);
    ASSERT_TRUE(result == nullptr);
    napi_value func = TestFunction(env);
    uint32_t number = 10;
    napi_value value = NapiHelper::CreateUint32(env, number);
    napi_value* args = new napi_value[argc];
    Task::CreateTaskByFunc(env, value, func, args, argc);
    ASSERT_TRUE(args != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest032, testing::ext::TestSize.Level0)
{
    TaskGroupManager &taskGroupManager = TaskGroupManager::GetInstance();
    uint32_t groupId = taskGroupManager.GenerateGroupId();
    ASSERT_TRUE(groupId == 2);
}

HWTEST_F(NativeEngineTest, TaskpoolTest033, testing::ext::TestSize.Level0)
{
    TaskGroupManager &taskGroupManager = TaskGroupManager::GetInstance();
    uint32_t groupExecuteId = taskGroupManager.GenerateGroupExecuteId();
    ASSERT_TRUE(groupExecuteId == 2);
}

HWTEST_F(NativeEngineTest, TaskpoolTest034, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskGroupManager &taskGroupManager = TaskGroupManager::GetInstance();
    uint32_t groupId = taskGroupManager.GenerateGroupId();
    uint32_t groupExecuteId = taskGroupManager.GenerateGroupExecuteId();
    napi_value value = NapiHelper::CreateUint32(env, groupId);
    napi_ref reference = NapiHelper::CreateReference(env, value, groupExecuteId);
    taskGroupManager.AddTask(groupId, reference);
    ASSERT_NE(reference, nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest035, testing::ext::TestSize.Level0)
{
    TaskGroupManager &taskGroupManager = TaskGroupManager::GetInstance();
    uint32_t groupId = taskGroupManager.GenerateGroupId();
    const std::list<napi_ref>&tasks = taskGroupManager.GetTasksByGroup(groupId);
    ASSERT_TRUE(tasks.size() == 0);
}

HWTEST_F(NativeEngineTest, TaskpoolTest036, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskGroupManager &taskGroupManager = TaskGroupManager::GetInstance();
    uint32_t groupId = taskGroupManager.GenerateGroupId();
    taskGroupManager.ClearTasks(env, groupId);
    ASSERT_TRUE(groupId == 5);
}

HWTEST_F(NativeEngineTest, TaskpoolTest037, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskGroupManager &taskGroupManager = TaskGroupManager::GetInstance();
    uint32_t groupId = taskGroupManager.GenerateGroupId();
    uint32_t groupExecuteId = taskGroupManager.GenerateGroupExecuteId();
    uint32_t taskNum = 10;
    GroupInfo* groupInfo = taskGroupManager.GenerateGroupInfo(env, taskNum, groupId, groupExecuteId);
    ASSERT_TRUE(groupInfo != nullptr);
    ASSERT_TRUE(groupInfo->groupId == 6);
    ASSERT_TRUE(groupInfo->taskNum == 10);
    ASSERT_TRUE(groupInfo->finishedTask == 0);
    ASSERT_TRUE(groupInfo->executeIds.size() == 0);
    ASSERT_TRUE(groupInfo->deferred == nullptr);
    ASSERT_TRUE(groupInfo->resArr != nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest038, testing::ext::TestSize.Level0)
{
    napi_env env = reinterpret_cast<napi_env>(engine_);
    TaskGroupManager &taskGroupManager = TaskGroupManager::GetInstance();
    uint32_t groupId = taskGroupManager.GenerateGroupId();
    ASSERT_TRUE(groupId == 7);
    uint32_t groupExecuteId = taskGroupManager.GenerateGroupExecuteId();
    uint32_t taskNum = 10;
    GroupInfo* groupInfo = taskGroupManager.GenerateGroupInfo(env, taskNum, groupId, groupExecuteId);
    taskGroupManager.ClearGroupInfo(env, groupExecuteId, groupInfo);
    ASSERT_TRUE(groupExecuteId == 5);
}

HWTEST_F(NativeEngineTest, TaskpoolTest039, testing::ext::TestSize.Level0)
{
    TaskGroupManager &taskGroupManager = TaskGroupManager::GetInstance();
    uint32_t groupExecuteId = taskGroupManager.GenerateGroupExecuteId();
    GroupInfo* groupInfo = taskGroupManager.GetGroupInfoByExecutionId(groupExecuteId);
    ASSERT_TRUE(groupInfo == nullptr);
}

HWTEST_F(NativeEngineTest, TaskpoolTest040, testing::ext::TestSize.Level0)
{
    TaskGroupManager &taskGroupManager = TaskGroupManager::GetInstance();
    uint32_t groupId = taskGroupManager.GenerateGroupId();
    taskGroupManager.ClearExecuteId(groupId);
    ASSERT_TRUE(groupId == 8);
}