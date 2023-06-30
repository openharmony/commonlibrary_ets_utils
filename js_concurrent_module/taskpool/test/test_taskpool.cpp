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