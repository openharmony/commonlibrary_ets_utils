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
    ASSERT_TRUE(result.second == Priority::MEDIUM);
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
