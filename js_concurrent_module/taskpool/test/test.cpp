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
#include "worker.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {

static constexpr uint32_t MAX_TIMEOUT_TIME = 600000;

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

} // namespace Commonlibrary::Concurrent::TaskPoolModule