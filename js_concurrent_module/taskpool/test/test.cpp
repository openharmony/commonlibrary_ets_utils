/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "sequence_runner.h"
#include "task.h"
#include "task_group.h"
#include "task_manager.h"
#include "taskpool.h"
#include "utils.h"
#include "worker.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {

static constexpr uint32_t MAX_TIMEOUT_TIME = 600000;
static constexpr uint32_t FINSHED_TASK_ = 5;
static constexpr uint32_t TASK_NUMS_ = 7;
static constexpr uint64_t UINT64_ZERO_ = 0;
static constexpr uint32_t UINT32_ZERO_ = 0;
static constexpr uint32_t UINT32_ONE_ = 1;
static constexpr size_t SIZE_TWO_ = 2;
static constexpr size_t SIZE_THREE_ = 3;

napi_ref CreateReference(napi_env env)
{
    napi_value obj = NapiHelper::CreateObject(env);
    return NapiHelper::CreateReference(env, obj, 1);
}

napi_value SendableUtils::CreateSendableClass(napi_env env)
{
    auto constructor = [](napi_env env, napi_callback_info info) -> napi_value {
        napi_value thisVar = nullptr;
        napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
        return thisVar;
    };

    napi_property_descriptor props[] = {
        DECLARE_NAPI_FUNCTION("foo", Foo),
        DECLARE_NAPI_FUNCTION("bar", Bar),
    };

    napi_value sendableClass = nullptr;
    napi_define_sendable_class(env, "SendableClass", NAPI_AUTO_LENGTH, constructor, nullptr,
                               sizeof(props) / sizeof(props[0]), props, nullptr, &sendableClass);
    return sendableClass;
}

napi_value SendableUtils::CreateSendableInstance(napi_env env)
{
    napi_value cls = SendableUtils::CreateSendableClass(env);
    napi_value instance = nullptr;
    napi_new_instance(env, cls, 0, nullptr, &instance);
    return instance;
}

napi_value SendableUtils::Foo(napi_env env, napi_callback_info info)
{
    return nullptr;
}

napi_value SendableUtils::Bar(napi_env env, napi_callback_info info)
{
    return nullptr;
}

napi_value NativeEngineTest::IsConcurrent(napi_env env, napi_value argv[], size_t argc)
{
    std::string funcName = "IsConcurrent";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), TaskPool::IsConcurrent, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &result);
    return result;
}

napi_value NativeEngineTest::GetTaskPoolInfo(napi_env env, napi_value argv[], size_t argc)
{
    std::string funcName = "GetTaskPoolInfo";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), TaskPool::GetTaskPoolInfo, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &result);
    return result;
}

napi_value NativeEngineTest::TerminateTask(napi_env env, napi_value argv[], size_t argc)
{
    std::string funcName = "TerminateTask";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), TaskPool::TerminateTask, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &result);
    return result;
}

napi_value NativeEngineTest::Execute(napi_env env, napi_value argv[], size_t argc)
{
    std::string funcName = "Execute";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), TaskPool::Execute, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &result);
    return result;
}

napi_value NativeEngineTest::ExecuteDelayed(napi_env env, napi_value argv[], size_t argc)
{
    std::string funcName = "ExecuteDelayed";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), TaskPool::ExecuteDelayed, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &result);
    return result;
}

napi_value NativeEngineTest::Cancel(napi_env env, napi_value argv[], size_t argc)
{
    std::string funcName = "Cancel";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), TaskPool::Cancel, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &result);
    return result;
}

void NativeEngineTest::TaskGroupDestructor(napi_env env, void* data)
{
    void* hint = nullptr;
    TaskGroup::TaskGroupDestructor(env, data, hint);
}

void NativeEngineTest::SequenceRunnerDestructor(napi_env env, void* data)
{
    void* hint = nullptr;
    SequenceRunner::SequenceRunnerDestructor(env, data, hint);
}

napi_value NativeEngineTest::ExecutePeriodically(napi_env env, napi_value argv[], size_t argc)
{
    std::string funcName = "ExecutePeriodically";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), TaskPool::ExecutePeriodically, nullptr, &cb);
    napi_call_function(env, nullptr, cb, argc, argv, &result);
    return result;
}

napi_value NativeEngineTest::ExecuteGroup(napi_env env, napi_value taskGroup)
{
    return TaskPool::ExecuteGroup(env, taskGroup, Priority::DEFAULT);
}

void NativeEngineTest::DelayTask(uv_timer_t* handle)
{
    TaskPool::DelayTask(handle);
}

void NativeEngineTest::PeriodicTaskCallback(uv_timer_t* handle)
{
    TaskPool::PeriodicTaskCallback(handle);
}

void NativeEngineTest::UpdateGroupInfoByResult(napi_env env, uv_timer_t* handle, napi_value res, bool success)
{
    Task* task = reinterpret_cast<Task*>(handle->data);
    TaskPool::UpdateGroupInfoByResult(env, task, res, success);
}

void NativeEngineTest::TryTriggerExpand()
{
    TaskManager& taskManager = TaskManager::GetInstance();
    taskManager.isHandleInited_ = false;
    taskManager.TryTriggerExpand();
    taskManager.isHandleInited_ = true;
    taskManager.needChecking_ = false;
}

void NativeEngineTest::CheckForBlockedWorkers(napi_env env)
{
    Worker* worker = Worker::WorkerConstructor(env);
    worker->workerEnv_ = nullptr;
    TaskManager& taskManager = TaskManager::GetInstance();
    taskManager.workers_.clear();
    taskManager.workers_.insert(worker);
    taskManager.GetThreadInfos(env);

    worker->workerEnv_ = env;
    worker->state_ = WorkerState::RUNNING;
    worker->startTime_ = ConcurrentHelper::GetMilliseconds() - MAX_TIMEOUT_TIME;
    worker->idleState_ = true;
    taskManager.CheckForBlockedWorkers();

    worker->state_ = WorkerState::RUNNING;
    worker->startTime_ = ConcurrentHelper::GetMilliseconds() - MAX_TIMEOUT_TIME;
    worker->idleState_ = false;
    worker->hasLongTask_ = true;
    taskManager.CheckForBlockedWorkers();

    worker->state_ = WorkerState::RUNNING;
    worker->startTime_ = ConcurrentHelper::GetMilliseconds() - MAX_TIMEOUT_TIME;
    worker->idleState_ = false;
    worker->hasLongTask_ = false;
    taskManager.idleWorkers_.insert(worker);
    taskManager.CheckForBlockedWorkers();
    taskManager.timeoutWorkers_.clear();
}

void NativeEngineTest::foo(const uv_async_t* req)
{
    return;
}

void NativeEngineTest::TriggerShrink(napi_env env)
{
    uint32_t step = 1;
    TaskManager& taskManager = TaskManager::GetInstance();
    taskManager.idleWorkers_.clear();
    Worker* worker = Worker::WorkerConstructor(env);
    worker->workerEnv_ = env;
    taskManager.idleWorkers_.insert(worker);
    taskManager.globalEnableFfrtFlag_ = true;
    worker->InitFfrtInfo();
    worker->ffrtTaskHandle_ = reinterpret_cast<void*>(env);
    taskManager.GetIdleWorkers();

    taskManager.freeList_.emplace_back(worker);
    worker->state_ = WorkerState::RUNNING;
    taskManager.TriggerShrink(step);

    taskManager.idleWorkers_.clear();
    taskManager.idleWorkers_.insert(worker);
    taskManager.freeList_.emplace_back(worker);
    worker->idlePoint_ = ConcurrentHelper::GetMilliseconds();
    worker->state_ = WorkerState::IDLE;
    worker->hasLongTask_ = false;
    taskManager.TriggerShrink(step);

    taskManager.idleWorkers_.clear();
    taskManager.idleWorkers_.insert(worker);
    taskManager.freeList_.emplace_back(worker);
    worker->idlePoint_ = ConcurrentHelper::GetMilliseconds() - MAX_TIMEOUT_TIME;
    worker->state_ = WorkerState::IDLE;
    worker->hasLongTask_ = false;
    uv_loop_t* loop = worker->GetWorkerLoop();
    ConcurrentHelper::UvHandleInit(loop, worker->clearWorkerSignal_, NativeEngineTest::foo, worker);
    taskManager.TriggerShrink(step);
    taskManager.idleWorkers_.clear();
    taskManager.globalEnableFfrtFlag_ = false;
}

void NativeEngineTest::NotifyShrink(napi_env env)
{
    uint32_t step = 1;
    TaskManager& taskManager = TaskManager::GetInstance();
    taskManager.workers_.clear();
    taskManager.timeoutWorkers_.clear();

    Worker* worker1 = Worker::WorkerConstructor(env);
    Worker* worker2 = Worker::WorkerConstructor(env);
    worker1->workerEnv_ = env;
    worker2->workerEnv_ = env;
    uv_loop_t* loop1 = worker1->GetWorkerLoop();
    ConcurrentHelper::UvHandleInit(loop1, worker1->clearWorkerSignal_, NativeEngineTest::foo, worker1);
    uv_loop_t* loop2 = worker2->GetWorkerLoop();
    ConcurrentHelper::UvHandleInit(loop2, worker2->clearWorkerSignal_, NativeEngineTest::foo, worker2);
    taskManager.workers_.insert(worker1);
    taskManager.workers_.insert(worker2);
    taskManager.timeoutWorkers_.insert(worker1);
    taskManager.timeoutWorkers_.insert(worker2);
    taskManager.NotifyShrink(step);

    taskManager.workers_.clear();
    taskManager.timeoutWorkers_.clear();
    taskManager.workers_.insert(worker1);
    taskManager.idleWorkers_.insert(nullptr);
    taskManager.NotifyShrink(step);

    taskManager.idleWorkers_.clear();
    taskManager.idleWorkers_.insert(worker2);
    worker2->hasLongTask_ = true;
    taskManager.NotifyShrink(step);

    worker2->hasLongTask_ = false;
    worker2->hasExecuted_ = true;
    taskManager.NotifyShrink(step);

    worker2->hasExecuted_ = false;
    taskManager.workers_.clear();
    taskManager.NotifyShrink(step);
}

void NativeEngineTest::TryExpand(napi_env env)
{
    TaskManager& taskManager = TaskManager::GetInstance();
    taskManager.workers_.clear();
    taskManager.timeoutWorkers_.clear();
    taskManager.idleWorkers_.clear();
    Worker* worker = Worker::WorkerConstructor(env);
    worker->workerEnv_ = env;
    uv_loop_t* loop = worker->GetWorkerLoop();
    ConcurrentHelper::UvHandleInit(loop, worker->performTaskSignal_, NativeEngineTest::foo, worker);
    taskManager.idleWorkers_.insert(worker);
    taskManager.TryExpand();
    taskManager.nonIdleTaskNum_ = 1;
    taskManager.TryExpand();
    taskManager.nonIdleTaskNum_ = 0;
    taskManager.TryExpand();
    taskManager.idleWorkers_.clear();
    taskManager.suspend_ = true;
    taskManager.TryExpand();
}

void NativeEngineTest::CancelTask(napi_env env)
{
    TaskManager& taskManager = TaskManager::GetInstance();
    Task* task = new Task();
    task->taskType_ = TaskType::COMMON_TASK;
    task->taskId_ = reinterpret_cast<uint64_t>(task);
    napi_value val = NapiHelper::CreateObject(env);
    napi_ref ref = NapiHelper::CreateReference(env, val, 0);
    task->taskRef_ = ref;
    taskManager.StoreTask(task->taskId_, task);
    task->taskState_ = ExecuteState::CANCELED;
    taskManager.CancelTask(env, task->taskId_);

    uv_loop_t* loop = NapiHelper::GetLibUV(env);
    task->timer_ = new uv_timer_t;
    uv_timer_init(loop, task->timer_);
    task->taskState_ = ExecuteState::RUNNING;
    task->isPeriodicTask_ = true;
    taskManager.CancelTask(env, task->taskId_);

    task->isPeriodicTask_ = false;
    task->taskType_ = TaskType::SEQRUNNER_TASK;
    taskManager.CancelTask(env, task->taskId_);

    task->taskState_ = ExecuteState::FINISHED;
    taskManager.CancelTask(env, task->taskId_);

    TaskInfo* taskInfo = new TaskInfo();
    task->taskState_ = ExecuteState::WAITING;
    task->currentTaskInfo_ = taskInfo;
    taskManager.CancelTask(env, task->taskId_);
    taskManager.tasks_.clear();
    delete task;
}

void NativeEngineTest::NotifyWorkerIdle(napi_env env)
{
    TaskManager& taskManager = TaskManager::GetInstance();
    Task* task = new Task();
    task->taskId_ = reinterpret_cast<uint64_t>(task);
    taskManager.taskQueues_[Priority::DEFAULT]->EnqueueTaskId(task->taskId_);
    Worker* worker = Worker::WorkerConstructor(env);
    worker->workerEnv_ = env;
    uv_loop_t* loop = worker->GetWorkerLoop();
    ConcurrentHelper::UvHandleInit(loop, worker->performTaskSignal_, NativeEngineTest::foo, worker);
    worker->state_ = WorkerState::BLOCKED;
    taskManager.NotifyWorkerIdle(worker);
    worker->state_ = WorkerState::IDLE;
    taskManager.NotifyWorkerIdle(worker);
    delete task;
}

void NativeEngineTest::EnqueueTaskId(napi_env env)
{
    TaskManager& taskManager = TaskManager::GetInstance();
    Task* task = new Task();
    task->taskId_ = reinterpret_cast<uint64_t>(task);
    taskManager.StoreTask(task->taskId_, task);
    napi_value obj = Helper::NapiHelper::CreateObject(env);
    napi_ref callbackRef = Helper::NapiHelper::CreateReference(env, obj, 1);
    task->onEnqueuedCallBackInfo_ = new ListenerCallBackInfo(env, callbackRef, nullptr);
    taskManager.EnqueueTaskId(task->taskId_);

    taskManager.workers_.clear();
    Worker* worker = Worker::WorkerConstructor(env);
    worker->state_ = WorkerState::RUNNING;
    taskManager.workers_.insert(worker);
    taskManager.IsChooseIdle();
    taskManager.workers_.clear();
    Helper::NapiHelper::DeleteReference(env, callbackRef);
    delete task;
}

void NativeEngineTest::GetTaskByPriority(napi_env env)
{
    TaskManager& taskManager = TaskManager::GetInstance();
    Task* task = new Task();
    task->taskId_ = reinterpret_cast<uint64_t>(task);
    taskManager.StoreTask(task->taskId_, task);
    auto& mediumTaskQueue = taskManager.taskQueues_[Priority::DEFAULT];
    uint64_t id = mediumTaskQueue->DequeueTaskId();
    while (id != 0) {
        id = mediumTaskQueue->DequeueTaskId();
    }
    taskManager.EnqueueTaskId(task->taskId_);
    std::set<uint64_t> set{task->taskId_};
    taskManager.dependTaskInfos_.emplace(task->taskId_, std::move(set));
    taskManager.GetTaskByPriority(mediumTaskQueue, Priority::DEFAULT);
    taskManager.dependTaskInfos_.clear();
    taskManager.tasks_.clear();
    delete task;
}

void NativeEngineTest::RestoreWorker(napi_env env)
{
    TaskManager& taskManager = TaskManager::GetInstance();
    Worker* worker = Worker::WorkerConstructor(env);
    taskManager.suspend_ = false;
    worker->state_ = WorkerState::BLOCKED;
    taskManager.RestoreWorker(worker);

    Task* task = new Task();
    task->taskId_ = reinterpret_cast<uint64_t>(task);
    taskManager.EnqueueTaskId(task->taskId_);
    worker->state_ = WorkerState::IDLE;
    worker->workerEnv_ = env;
    uv_loop_t* loop = worker->GetWorkerLoop();
    ConcurrentHelper::UvHandleInit(loop, worker->performTaskSignal_, NativeEngineTest::foo, worker);
    taskManager.RestoreWorker(worker);
}

void NativeEngineTest::StoreDependentId(uint64_t taskId, uint64_t dependentId)
{
    TaskManager& taskManager = TaskManager::GetInstance();
    std::set<uint64_t> set{ dependentId };
    taskManager.dependTaskInfos_.emplace(taskId, std::move(set));
}

void NativeEngineTest::StoreDependentTaskId(uint64_t taskId, uint64_t dependentId)
{
    TaskManager& taskManager = TaskManager::GetInstance();
    std::set<uint64_t> set{ dependentId };
    taskManager.dependentTaskInfos_.emplace(taskId, std::move(set));
}

void NativeEngineTest::StoreTaskDuration(uint64_t taskId)
{
    TaskManager& taskManager = TaskManager::GetInstance();
    uint64_t durationId = taskId + MAX_TIMEOUT_TIME;
    std::pair<uint64_t, uint64_t> durationData = std::make_pair(taskId, durationId);
    taskManager.taskDurationInfos_.emplace(taskId, std::move(durationData));
}

void NativeEngineTest::InitTaskManager(napi_env env)
{
    napi_env taskEnv = nullptr;
    napi_create_runtime(env, &taskEnv);
    NativeEngine* taskEngine = reinterpret_cast<NativeEngine*>(taskEnv);
    taskEngine->MarkTaskPoolThread();
    TaskManager& taskManager = TaskManager::GetInstance();
    taskManager.globalEnableFfrtFlag_ = true;
    taskManager.InitTaskManager(taskEnv);
    taskManager.DecreaseRefCount(env, 0);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
}

void NativeEngineTest::NotifyDependencyTaskInfo(napi_env env)
{
    TaskManager& taskManager = TaskManager::GetInstance();
    Task* task = new Task();
    task->taskId_ = reinterpret_cast<uint64_t>(task);
    task->env_ = env;
    Worker* worker = Worker::WorkerConstructor(env);
    worker->workerEnv_ = env;
    task->worker_ = worker;
    uint64_t id = task->taskId_ + MAX_TIMEOUT_TIME;
    std::set<uint64_t> set{ task->taskId_, id };
    taskManager.dependentTaskInfos_.emplace(task->taskId_, std::move(set));
    taskManager.NotifyDependencyTaskInfo(task->taskId_);
    std::set<uint64_t> set1{ task->taskId_, id };
    taskManager.dependentTaskInfos_.emplace(task->taskId_, std::move(set1));
    taskManager.EnqueuePendingTaskInfo(0, Priority::DEFAULT);
    taskManager.EnqueuePendingTaskInfo(id, Priority::DEFAULT);
    taskManager.EnqueuePendingTaskInfo(task->taskId_, Priority::DEFAULT);
    taskManager.NotifyDependencyTaskInfo(task->taskId_);
    std::set<uint64_t> set2{ task->taskId_, id };
    taskManager.dependentTaskInfos_.emplace(task->taskId_, std::move(set2));
    taskManager.IsDependentByTaskId(task->taskId_);
}

void NativeEngineTest::StoreTaskDependency(napi_env env)
{
    TaskManager& taskManager = TaskManager::GetInstance();
    Task* task = new Task();
    task->taskId_ = reinterpret_cast<uint64_t>(task);
    task->env_ = env;
    Task* task1 = new Task();
    task1->taskId_ = reinterpret_cast<uint64_t>(task1);
    task1->env_ = env;
    Task* task2 = new Task();
    task2->taskId_ = reinterpret_cast<uint64_t>(task1);
    task2->env_ = env;
    taskManager.dependTaskInfos_.clear();
    uint64_t id1 = task->taskId_;
    uint64_t id2 = task->taskId_ + MAX_TIMEOUT_TIME;
    uint64_t id3 = task1->taskId_;
    uint64_t id4 = task1->taskId_ + MAX_TIMEOUT_TIME;
    uint64_t id5 = task2->taskId_;
    uint64_t id6 = task2->taskId_ + MAX_TIMEOUT_TIME;
    std::set<uint64_t> set{ id2, id3 };
    taskManager.dependTaskInfos_.emplace(id1, std::move(set));
    std::set<uint64_t> taskId{ id1, id2 };
    taskManager.StoreTaskDependency(id3, taskId);
    taskManager.StoreTaskDependency(id5, taskId);
    std::set<uint64_t> set1{ id4, id5 };
    taskManager.dependTaskInfos_.emplace(id3, std::move(set1));
    taskManager.StoreTaskDependency(id1, taskId);
    std::set<uint64_t> set2{ id6 };
    std::set<uint64_t> set3{ id1 };
    taskManager.dependTaskInfos_.emplace(id5, std::move(set3));
    taskManager.StoreTaskDependency(id1, taskId);
    taskManager.dependTaskInfos_.emplace(id5, std::move(set2));
    taskManager.StoreTaskDependency(id1, taskId);
    taskManager.dependTaskInfos_.clear();
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
}

void NativeEngineTest::RemoveTaskDependency(napi_env env)
{
    TaskManager& taskManager = TaskManager::GetInstance();
    Task* task = new Task();
    task->taskId_ = reinterpret_cast<uint64_t>(task);
    uint64_t id = task->taskId_ + MAX_TIMEOUT_TIME;
    Task* task1 = new Task();
    task1->taskId_ = reinterpret_cast<uint64_t>(task1);
    uint64_t id2 = task1->taskId_ + MAX_TIMEOUT_TIME;
    taskManager.dependTaskInfos_.clear();
    std::set<uint64_t> set{ id };
    taskManager.dependTaskInfos_.emplace(task->taskId_, std::move(set));
    taskManager.RemoveTaskDependency(task->taskId_, task1->taskId_);
    taskManager.RemoveTaskDependency(task->taskId_, id);
    std::set<uint64_t> set2{ id };
    taskManager.dependentTaskInfos_.emplace(task->taskId_, std::move(set2));
    std::set<uint64_t> dependentTaskIdSet{ task->taskId_ };
    taskManager.StoreDependentTaskInfo(dependentTaskIdSet, task1->taskId_);
    taskManager.RemoveDependentTaskInfo(task->taskId_, id2);
    taskManager.RemoveDependentTaskInfo(task->taskId_, id);
    taskManager.GetTaskDependInfoToString(task1->taskId_);
    taskManager.taskDurationInfos_.emplace(task->taskId_, std::make_pair(UINT64_ZERO_, task1->taskId_));
    taskManager.StoreTaskDuration(task->taskId_, UINT64_ZERO_, UINT64_ZERO_);
    taskManager.GetTaskDuration(task->taskId_, "");
    taskManager.RemoveTaskDuration(task->taskId_);
}

void NativeEngineTest::ReleaseTaskData(napi_env env)
{
    TaskManager& taskManager = TaskManager::GetInstance();
    Task* task = new Task();
    task->taskId_ = reinterpret_cast<uint64_t>(task);
    task->onResultSignal_ = nullptr;
    task->taskType_ = TaskType::FUNCTION_TASK;
    taskManager.StoreTask(task->taskId_, task);
    taskManager.ReleaseTaskData(env, task);
    task->taskType_ = TaskType::GROUP_FUNCTION_TASK;
    taskManager.StoreTask(task->taskId_, task);
    taskManager.ReleaseTaskData(env, task);
    std::set<uint64_t> set{ task->taskId_ };
    taskManager.dependTaskInfos_.emplace(task->taskId_, std::move(set));
    task->taskType_ = TaskType::COMMON_TASK;
    taskManager.StoreTask(task->taskId_, task);
    taskManager.ReleaseTaskData(env, task);
    Task* task1 = new Task();
    task1->taskId_ = reinterpret_cast<uint64_t>(task1);
    task1->onEnqueuedCallBackInfo_ = new ListenerCallBackInfo(env, nullptr, nullptr);
    task1->onStartExecutionCallBackInfo_ = new ListenerCallBackInfo(env, nullptr, nullptr);
    task1->onExecutionFailedCallBackInfo_ = new ListenerCallBackInfo(env, nullptr, nullptr);
    task1->onExecutionSucceededCallBackInfo_ = new ListenerCallBackInfo(env, nullptr, nullptr);
    taskManager.ReleaseCallBackInfo(task1);
    Task* task2 = new Task();
    task2->taskId_ = reinterpret_cast<uint64_t>(task2);
    task2->isMainThreadTask_ = true;
    taskManager.ReleaseCallBackInfo(task2);
    task2->isMainThreadTask_ = false;
    taskManager.ReleaseCallBackInfo(task2);
    auto loop = NapiHelper::GetLibUV(env);
    ConcurrentHelper::UvHandleInit(loop, task->onStartExecutionSignal_, NativeEngineTest::foo, task2);
    taskManager.ReleaseCallBackInfo(task2);
}

void NativeEngineTest::CheckTask(napi_env env)
{
    TaskManager& taskManager = TaskManager::GetInstance();
    Task* task = new Task();
    task->taskId_ = reinterpret_cast<uint64_t>(task);
    Task* task1 = new Task();
    task1->taskId_ = reinterpret_cast<uint64_t>(task1);
    taskManager.StoreTask(task->taskId_, task);
    taskManager.CheckTask(task1->taskId_);

    TaskGroupManager& groupManager = TaskGroupManager::GetInstance();
    TaskGroup* group = new TaskGroup();
    napi_value obj = NapiHelper::CreateObject(env);
    napi_ref ref = NapiHelper::CreateReference(env, obj, 1);
    group->groupRef_ = ref;
    uint64_t groupId = reinterpret_cast<uint64_t>(group);
    groupManager.StoreTaskGroup(groupId, nullptr);
    groupManager.AddTask(groupId, nullptr, task->taskId_);
    groupManager.taskGroups_.clear();
    groupManager.StoreTaskGroup(groupId, group);
    group->groupState_ = ExecuteState::CANCELED;
    groupManager.CancelGroup(env, groupId);

    group->groupState_ = ExecuteState::WAITING;
    groupManager.CancelGroup(env, groupId);

    GroupInfo* groupInfo = new GroupInfo();
    groupInfo->finishedTask = FINSHED_TASK_;
    group->currentGroupInfo_ = groupInfo;
    group->groupState_ = ExecuteState::NOT_FOUND;
    groupManager.CancelGroup(env, groupId);

    group->groupState_ = ExecuteState::FINISHED;
    groupManager.CancelGroup(env, groupId);

    group->groupState_ = ExecuteState::RUNNING;
    group->taskNum_ = FINSHED_TASK_;
    group->taskIds_.push_back(task->taskId_);
    groupManager.CancelGroup(env, groupId);

    group->taskNum_ = TASK_NUMS_;
    group->groupState_ = ExecuteState::WAITING;
    napi_value resArr;
    napi_create_array_with_length(env, group->taskIds_.size(), &resArr);
    napi_ref arrRef = NapiHelper::CreateReference(env, resArr, 1);
    groupInfo->resArr = arrRef;
    napi_value promise = NapiHelper::CreatePromise(env, &groupInfo->deferred);
    groupManager.CancelGroup(env, groupId);
}

void NativeEngineTest::CancelGroupTask(napi_env env)
{
    TaskGroupManager& groupManager = TaskGroupManager::GetInstance();
    TaskManager& taskManager = TaskManager::GetInstance();
    TaskGroup* group = new TaskGroup();
    Task* task = new Task();
    task->taskId_ = reinterpret_cast<uint64_t>(task);
    taskManager.StoreTask(task->taskId_, task);
    task->taskState_ = ExecuteState::RUNNING;
    groupManager.CancelGroupTask(env, task->taskId_, group);
    task->taskState_ = ExecuteState::WAITING;
    groupManager.CancelGroupTask(env, task->taskId_, group);
    task->taskState_ = ExecuteState::WAITING;
    TaskInfo* taskInfo = new TaskInfo();
    task->currentTaskInfo_ = taskInfo;
    groupManager.CancelGroupTask(env, task->taskId_, group);

    Task* task1 = new Task();
    task1->taskId_ = reinterpret_cast<uint64_t>(task1);
    SequenceRunner* seqRunner = new SequenceRunner();
    uint64_t seqRunnerId = reinterpret_cast<uint64_t>(seqRunner);
    groupManager.StoreSequenceRunner(seqRunnerId, seqRunner);
    groupManager.AddTaskToSeqRunner(seqRunnerId, task1);
}

void NativeEngineTest::TriggerSeqRunner(napi_env env)
{
    TaskGroupManager& groupManager = TaskGroupManager::GetInstance();
    Task* task = new Task();
    task->taskId_ = reinterpret_cast<uint64_t>(task);
    Task* task1 = new Task();
    task1->taskId_ = reinterpret_cast<uint64_t>(task1);
    SequenceRunner* seqRunner = new SequenceRunner();
    uint64_t seqRunnerId = reinterpret_cast<uint64_t>(seqRunner);
    seqRunner->priority_ = Priority::DEFAULT;
    task->seqRunnerId_ = seqRunnerId;
    groupManager.StoreSequenceRunner(seqRunnerId, seqRunner);
    seqRunner->isGlobalRunner_ = true;
    bool res = groupManager.TriggerSeqRunner(env, task);
    ASSERT_FALSE(res);
    seqRunner->globalSeqRunnerRef_.emplace(env, CreateReference(env));
    seqRunner->currentTaskId_ = task1->taskId_;
    groupManager.TriggerSeqRunner(env, task);
    seqRunner->isGlobalRunner_ = false;
    seqRunner->seqRunnerRef_ = CreateReference(env);
    seqRunner->currentTaskId_ = task->taskId_;
    groupManager.TriggerSeqRunner(env, task);
    seqRunner->seqRunnerRef_ = CreateReference(env);
    task1->taskState_ = ExecuteState::CANCELED;
    seqRunner->seqRunnerTasks_.push(task1);
    TaskInfo* taskInfo = new TaskInfo();
    task1->currentTaskInfo_ = taskInfo;
    seqRunner->currentTaskId_ = task->taskId_;
    groupManager.TriggerSeqRunner(env, task);
    seqRunner->seqRunnerRef_ = CreateReference(env);
    TaskInfo* taskInfo1 = new TaskInfo();
    task1->currentTaskInfo_ = taskInfo1;
    seqRunner->seqRunnerTasks_.push(task1);
    seqRunner->seqRunnerTasks_.push(task);
    task->taskState_ = ExecuteState::RUNNING;
    seqRunner->currentTaskId_ = task->taskId_;
    groupManager.TriggerSeqRunner(env, task);
}

void NativeEngineTest::UpdateGroupState(napi_env env)
{
    TaskGroupManager& groupManager = TaskGroupManager::GetInstance();
    TaskGroup* group = new TaskGroup();
    uint64_t groupId = reinterpret_cast<uint64_t>(group);
    Task* task = new Task();
    task->taskId_ = reinterpret_cast<uint64_t>(task);
    groupManager.StoreTaskGroup(groupId, group);
    groupManager.UpdateGroupState(task->taskId_);
    group->groupState_ = ExecuteState::CANCELED;
    groupManager.UpdateGroupState(groupId);
    group->groupState_ = ExecuteState::WAITING;
    groupManager.UpdateGroupState(groupId);

    SequenceRunnerManager& runnerManager = SequenceRunnerManager::GetInstance();
    napi_value obj = NapiHelper::CreateObject(env);
    SequenceRunner* seqRunner = runnerManager.CreateOrGetGlobalRunner(env, obj, SIZE_THREE_, "test", UINT32_ONE_);
    seqRunner->priority_ = Priority::MEDIUM;
    runnerManager.CreateOrGetGlobalRunner(env, obj, SIZE_TWO_, "test", UINT32_ZERO_);
    runnerManager.CreateOrGetGlobalRunner(env, obj, SIZE_TWO_, "test", UINT32_ONE_);
    runnerManager.DecreaseSeqCount(seqRunner);
    runnerManager.RemoveGlobalSeqRunnerRef(env, seqRunner);
    runnerManager.RemoveSequenceRunner("func");
    runnerManager.RemoveSequenceRunner("test");
}

void NativeEngineTest::ReleaseWorkerHandles(napi_env env)
{
    ExceptionScope scope(env);
    Worker* worker = Worker::WorkerConstructor(env);
    napi_env workerEnv;
    napi_create_runtime(env, &workerEnv);
    worker->workerEnv_ = workerEnv;
    NativeEngine* workerEngine = reinterpret_cast<NativeEngine*>(workerEnv);
    uv_loop_t* loop = worker->GetWorkerLoop();
    ConcurrentHelper::UvHandleInit(loop, worker->performTaskSignal_,
                                   NativeEngineTest::foo, worker);
    ConcurrentHelper::UvHandleInit(loop, worker->debuggerOnPostTaskSignal_,
                                   NativeEngineTest::foo, worker);
    ConcurrentHelper::UvHandleInit(loop, worker->clearWorkerSignal_,
                                   NativeEngineTest::foo, worker);
    uv_async_t* req = new uv_async_t;
    req->data = worker;
    Worker::ReleaseWorkerHandles(req);
    workerEngine->IncreaseListeningCounter();
    Worker::ReleaseWorkerHandles(req);
    workerEngine->DecreaseListeningCounter();
    workerEngine->IncreaseWaitingRequestCounter();
    Worker::ReleaseWorkerHandles(req);
    workerEngine->DecreaseWaitingRequestCounter();
    workerEngine->IncreaseSubEnvCounter();
    Worker::ReleaseWorkerHandles(req);
}

void NativeEngineTest::DebuggerOnPostTask(napi_env env)
{
    ExceptionScope scope(env);
    Worker* worker = Worker::WorkerConstructor(env);
    worker->workerEnv_ = env;
    uv_loop_t* loop = worker->GetWorkerLoop();
    ConcurrentHelper::UvHandleInit(loop, worker->debuggerOnPostTaskSignal_,
                                   NativeEngineTest::foo, worker);
    std::function<void()> myTask = []() {
        return;
    };
    worker->DebuggerOnPostTask(std::move(myTask));
    uv_async_t* req = new uv_async_t;
    std::function<void()> myTask1 = []() {
        return;
    };
    worker->debuggerQueue_.push(myTask1);
    req->data = worker;
    worker->debuggerMutex_.unlock();
    Worker::HandleDebuggerTask(req);
    worker->workerEnv_ = nullptr;
    worker->ReleaseWorkerThreadContent();
    napi_env workerEnv;
    napi_create_runtime(env, &workerEnv);
    worker->workerEnv_ = workerEnv;
    worker->hostEnv_ = nullptr;
    worker->state_ = WorkerState::BLOCKED;
    worker->ReleaseWorkerThreadContent();
    napi_env workerEnv1;
    napi_create_runtime(env, &workerEnv1);
    worker->hostEnv_ = env;
    worker->workerEnv_ = workerEnv1;
    worker->state_ = WorkerState::IDLE;
    worker->ReleaseWorkerThreadContent();
}

void NativeEngineTest::PerformTask(napi_env env)
{
    ExceptionScope scope(env);
    TaskManager& taskManager = TaskManager::GetInstance();
    Worker* worker = Worker::WorkerConstructor(env);
    napi_env workerEnv;
    napi_create_runtime(env, &workerEnv);
    worker->workerEnv_ = workerEnv;

    TaskGroup* group = new TaskGroup();
    uint64_t groupId = reinterpret_cast<uint64_t>(group);
    group->groupState_ = ExecuteState::WAITING;
    TaskGroupManager& groupManager = TaskGroupManager::GetInstance();
    groupManager.StoreTaskGroup(groupId, group);

    Task* task = new Task();
    task->taskId_ = reinterpret_cast<uint64_t>(task);
    taskManager.StoreTask(task->taskId_, task);
    Priority priority = Priority::DEFAULT;
    auto& mediumTaskQueue = taskManager.taskQueues_[priority];
    uint64_t id = mediumTaskQueue->DequeueTaskId();
    while (id != 0) {
        id = mediumTaskQueue->DequeueTaskId();
    }
    mediumTaskQueue->EnqueueTaskId(task->taskId_);

    uv_async_t* req = new uv_async_t;
    req->data = worker;
    task->taskState_ = ExecuteState::CANCELED;
    Worker::PerformTask(req);
    task->taskState_ = ExecuteState::WAITING;
    task->taskType_ = TaskType::GROUP_COMMON_TASK;
    task->groupId_ = task->taskId_;
    Worker::PerformTask(req);
    task->groupId_ = groupId;
    Worker::PerformTask(req);
}

void NativeEngineTest::NotifyHandleTaskResult(napi_env env)
{
    ExceptionScope scope(env);
    Worker* worker = Worker::WorkerConstructor(env);
    napi_env workerEnv;
    napi_create_runtime(env, &workerEnv);
    worker->workerEnv_ = workerEnv;
    Task* task = new Task();
    task->taskId_ = reinterpret_cast<uint64_t>(task);
    task->env_ = workerEnv;
    uv_loop_t* loop = NapiHelper::GetLibUV(env);
    ConcurrentHelper::UvHandleInit(loop, task->onResultSignal_, NativeEngineTest::foo, task);
    Task* task1 = new Task();
    task1->taskId_ = reinterpret_cast<uint64_t>(task1);
    worker->currentTaskId_.push_back(task1->taskId_);
    task->worker_ = worker;
    task->IncreaseRefCount();
    Worker::NotifyHandleTaskResult(task);
    task->DecreaseRefCount();
    task->isMainThreadTask_ = true;
    Worker::NotifyHandleTaskResult(task);
    TaskManager::GetInstance().StoreTask(task->taskId_, task);
    Worker::NotifyHandleTaskResult(task);
}

void NativeEngineTest::TaskResultCallback(napi_env env)
{
    ExceptionScope scope(env);
    Worker* worker = Worker::WorkerConstructor(env);
    napi_env workerEnv;
    napi_create_runtime(env, &workerEnv);
    worker->workerEnv_ = workerEnv;
    Task* task = new Task();
    task->taskId_ = reinterpret_cast<uint64_t>(task);
    task->env_ = workerEnv;
    task->taskRefCount_.fetch_add(1);
    task->worker_ = worker;
    task->cpuTime_ = UINT64_ZERO_;
    Worker::TaskResultCallback(env, nullptr, false, reinterpret_cast<void*>(task));
    task->taskRefCount_.fetch_add(1);
    task->cpuTime_ = task->taskId_;
    Worker::TaskResultCallback(env, nullptr, true, reinterpret_cast<void*>(task));

    worker->priority_ = Priority::LOW;
    worker->ResetWorkerPriority();
    TaskManager& taskManager = TaskManager::GetInstance();
    taskManager.globalEnableFfrtFlag_ = true;
    worker->priority_ = Priority::HIGH;
    worker->ResetWorkerPriority();
    taskManager.globalEnableFfrtFlag_ = false;
    worker->state_ = WorkerState::BLOCKED;
    worker->UpdateExecutedInfo();
    worker->state_ = WorkerState::IDLE;
    worker->UpdateExecutedInfo();

    uint64_t id = task->taskId_ + MAX_TIMEOUT_TIME;
    std::unordered_set<uint64_t> set{ task->taskId_, id };
    worker->longTasksSet_ = std::move(set);
    worker->TerminateTask(task->taskId_);
}

void NativeEngineTest::HandleFunctionException(napi_env env)
{
    ExceptionScope scope(env);
    Worker* worker = Worker::WorkerConstructor(env);
    napi_env workerEnv;
    napi_create_runtime(env, &workerEnv);
    worker->workerEnv_ = workerEnv;
    Task* task = new Task();
    task->taskId_ = reinterpret_cast<uint64_t>(task);
    task->env_ = workerEnv;
    TaskResultInfo* resultInfo = new TaskResultInfo(env, workerEnv, task->taskId_, nullptr);
    TaskManager::GetInstance().NotifyCallbackExecute(env, resultInfo, task);
    task->IncreaseRefCount();
    uv_loop_t* loop = NapiHelper::GetLibUV(env);
    ConcurrentHelper::UvHandleInit(loop, task->onResultSignal_, NativeEngineTest::foo, task);
    task->worker_ = worker;
    Worker::HandleFunctionException(env, task);
    task->IncreaseRefCount();
    Worker::HandleFunctionException(env, task);
}
} // namespace Commonlibrary::Concurrent::TaskPoolModule