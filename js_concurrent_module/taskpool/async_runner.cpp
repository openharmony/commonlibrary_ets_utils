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
#include "async_runner.h"

#include <cinttypes>
#include "base_runner_manager.h"
#include "async_runner_manager.h"
#include "helper/error_helper.h"
#include "helper/napi_helper.h"
#include "task_manager.h"
#include "tools/log.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
using namespace Commonlibrary::Concurrent::Common::Helper;
static constexpr char EXECUTE_STR[] = "execute";

BaseRunnerManager& AsyncRunner::GetManager()
{
    return AsyncRunnerManager::GetInstance();
}

napi_value AsyncRunner::AsyncRunnerConstructor(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = 3; // 3 : name, runningCapacity, waitingCapacity
    napi_value args[3];
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, cbinfo, &argc, args, &thisVar, nullptr);
    napi_value runningCapacity = nullptr;
    napi_value name = nullptr;
    napi_value waitingCapacity = nullptr;
    if (argc == 3) { // 3: AsyncRunner(name, runningCapacity, waitingCapacity)
        name = args[0];
        runningCapacity = args[1]; // 1: the index of argument runningCapacity
        waitingCapacity = args[2]; // 2: the index of argument waitingCapacity
    } else if (argc == 2) { // 2: AsyncRunner(name, runningCapacity) or AsyncRunner(runningCapacity, waitingCapacity)
        if (NapiHelper::IsString(env, args[0])) {
            name = args[0];
            runningCapacity = args[1];
        } else {
            runningCapacity = args[0];
            waitingCapacity = args[1];
        }
    } else if (argc == 1) { // 1: AsyncRunner(runningCapacity)
        runningCapacity = args[0];
    } else {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "The numbers of params not more than three or less one.");
        return nullptr;
    }
    AsyncRunner* asyncRunner = CheckAndCreateAsyncRunner(env, thisVar, name, runningCapacity, waitingCapacity);
    if (asyncRunner == nullptr) {
        HILOG_ERROR("taskpool:: create asyncRunner failed.");
        return nullptr;
    }

    if (AsyncRunnerConstructorInner(env, thisVar, asyncRunner)) {
        return thisVar;
    }
    return nullptr;
}

bool AsyncRunner::AsyncRunnerConstructorInner(napi_env env, napi_value& thisVar, AsyncRunner* asyncRunner)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION(EXECUTE_STR, Execute),
    };
    return RunnerConstructorInner(env, thisVar, asyncRunner, properties,
                                  sizeof(properties) / sizeof(properties[0]));
}

void AsyncRunner::LogRunnerConstructor(std::string name, uint64_t runnerId)
{
    HILOG_INFO("taskpool:: construct asyncRunner name is %{public}s, asyncRunnerId %{public}s.",
               name_.c_str(), std::to_string(runnerId).c_str());
}

void AsyncRunner::LogRunnerConstructorInnerReturn(napi_status status)
{
    HILOG_ERROR("taskpool:: AsyncRunnerConstructorInner napi_wrap return value is %{public}d.", status);
}

napi_value AsyncRunner::Execute(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = 2; // 2 : task, Priority
    napi_value args[2];
    napi_value thisVar;
    napi_get_cb_info(env, cbinfo, &argc, args, &thisVar, nullptr);
    if (argc < 1 || argc > 2) { // 2 : task, Priority
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
            "The numbers of execute's params not more than two or less one.");
        return nullptr;
    }
    napi_value napiTask = args[0];
    napi_value napiPriority = nullptr;
    if (argc > 1) {
        napiPriority = args[1];
    }
    if (!CheckExecuteArgs(env, napiTask, napiPriority)) {
        return nullptr;
    }

    AsyncRunner* asyncRunner = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void**>(&asyncRunner));
    if (asyncRunner == nullptr) {
        return nullptr;
    }
    Task* task = nullptr;
    napi_unwrap(env, napiTask, reinterpret_cast<void**>(&task));
    if (task == nullptr) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "The type of execute's param must be task.");
        return nullptr;
    }
    if (!task->CanForAsyncRunner(env)) {
        return nullptr;
    }
    if (napiPriority != nullptr) {
        uint32_t priority = NapiHelper::GetUint32Value(env, napiPriority);
        task->asyncTaskPriority_ = static_cast<Priority>(priority);
    }
    task->runnerId_ = asyncRunner->runnerId_;
    napi_value promise = task->GetTaskInfoPromise(env, napiTask, TaskType::ASYNCRUNNER_TASK, task->asyncTaskPriority_);
    if (promise == nullptr) {
        return nullptr;
    }
    if (!AsyncRunnerManager::GetInstance().FindRunnerAndRef(asyncRunner->runnerId_)) {
        return nullptr;
    }
    if (!AddTasksToAsyncRunner(asyncRunner, task)) {
        ExecuteTaskImmediately(asyncRunner, task);
    }
    
    return promise;
}

AsyncRunner* AsyncRunner::CheckAndCreateAsyncRunner(napi_env env, napi_value& thisVar, napi_value name,
                                                    napi_value runningCapacity, napi_value waitingCapacity)
{
    std::string nameValue = "";
    std::uint32_t runningCapacityVal = 0;
    std::int32_t waitingCapacityValue = 0;
    if (name != nullptr) {
        if (!NapiHelper::IsString(env, name)) {
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "Name must be string.");
            return nullptr;
        }
        nameValue = NapiHelper::GetString(env, name);
    }
    if (runningCapacity == nullptr) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "RunningCapacity cannot be empty.");
        return nullptr;
    }
    if (!NapiHelper::IsNumber(env, runningCapacity)) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "RunningCapacity must be number.");
        return nullptr;
    }
    runningCapacityVal = NapiHelper::GetUint32Value(env, runningCapacity);
    if (runningCapacityVal < 1) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "RunningCapacity must be greater than zero.");
        return nullptr;
    }
    if (waitingCapacity != nullptr) {
        if (!NapiHelper::IsNumber(env, waitingCapacity)) {
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "WaitingCapacity must be number.");
            return nullptr;
        }
        waitingCapacityValue = NapiHelper::GetInt32Value(env, waitingCapacity);
        if (waitingCapacityValue < 0) {
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
                "WaitingCapacity must be greater than or equal zero.");
            return nullptr;
        }
    }
    AsyncRunner* asyncRunner = nullptr;
    std::uint32_t waitingCapacityVal = static_cast<std::uint32_t>(waitingCapacityValue);
    if (nameValue != "") {
        asyncRunner = AsyncRunnerManager::GetInstance().CreateOrGetGlobalRunner(env, thisVar, nameValue,
                                                                                runningCapacityVal, waitingCapacityVal);
    } else {
        asyncRunner = new AsyncRunner();
        asyncRunner->runningCapacity_ = runningCapacityVal;
        asyncRunner->waitingCapacity_ = waitingCapacityVal;
    }
    return asyncRunner;
}

bool AsyncRunner::CheckExecuteArgs(napi_env env, napi_value napiTask, napi_value napiPriority)
{
    if (!NapiHelper::IsObject(env, napiTask) || !NapiHelper::HasNameProperty(env, napiTask, TASKID_STR)) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "execute's first param must be task.");
        return false;
    }
    if (napiPriority != nullptr) {
        if (!NapiHelper::IsNumber(env, napiPriority)) {
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "The type of execute's second param must be number.");
            return false;
        }
        uint32_t priority = NapiHelper::GetUint32Value(env, napiPriority);
        if (priority >= Priority::NUMBER) {
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "Priority value is error.");
            return false;
        }
    }
    return true;
}

void AsyncRunner::ExecuteTaskImmediately(AsyncRunner* asyncRunner, Task* task)
{
    HILOG_DEBUG("taskpool:: task %{public}s in asyncRunner %{public}s immediately.",
                std::to_string(task->taskId_).c_str(), std::to_string(asyncRunner->runnerId_).c_str());
    task->IncreaseRefCount();
    TaskManager::GetInstance().IncreaseSendDataRefCount(task->taskId_);
    task->UpdateTaskStateToWaiting();
    TaskManager::GetInstance().EnqueueTaskId(task->taskId_, task->asyncTaskPriority_);
}

bool AsyncRunner::AddTasksToAsyncRunner(AsyncRunner* asyncRunner, Task* task)
{
    Task* frontTask = nullptr;
    {
        std::unique_lock<std::shared_mutex> asyncRunnerLock(asyncRunner->taskMutex_);
        if (asyncRunner->runningCount_ < asyncRunner->runningCapacity_) {
            asyncRunner->runningCount_.fetch_add(1);
            return false;
        }
        if (asyncRunner->waitingCapacity_ && asyncRunner->tasks_.size() == asyncRunner->waitingCapacity_) {
            frontTask = asyncRunner->tasks_.front();
            asyncRunner->tasks_.pop_front();
            frontTask->taskState_ = ExecuteState::CANCELED;
        }
        asyncRunner->tasks_.push_back(task);
    }

    if (frontTask != nullptr) {
        asyncRunner->TriggerRejectErrorTimer(frontTask, ErrorHelper::ERR_ASYNCRUNNER_TASK_DISCARDED);
    }
    return true;
}

void AsyncRunner::TriggerRejectErrorTimer(Task* task, int32_t errCode, bool isWaiting)
{
    if (task == nullptr || !task->IsValid()) {
        return;
    }
    if (!isWaiting) {
        AsyncRunnerManager::GetInstance().UnrefAndDestroyRunner(this);
    }
    DiscardTaskMessage* message = new DiscardTaskMessage(task->env_, task->taskId_, errCode, isWaiting);
    task->DiscardAsyncRunnerTask(message);
}

void AsyncRunner::TriggerWaitingTask()
{
    std::unique_lock<std::shared_mutex> lock(taskMutex_);
    DecreaseRunningCount();
    Task* task = nullptr;
    while (runningCount_ < runningCapacity_) {
        if (tasks_.empty()) {
            HILOG_DEBUG("taskpool:: asyncRunner %{public}s empty.", std::to_string(runnerId_).c_str());
            break;
        }
        task = tasks_.front();
        tasks_.pop_front();
        runningCount_.fetch_add(1);
        task->IncreaseRefCount();
        TaskManager::GetInstance().IncreaseSendDataRefCount(task->taskId_);
        task->taskState_ = ExecuteState::WAITING;
        HILOG_DEBUG("taskpool:: Trig task %{public}s in asyncRunner %{public}s.",
                    std::to_string(task->taskId_).c_str(), std::to_string(runnerId_).c_str());
        TaskManager::GetInstance().EnqueueTaskId(task->taskId_, task->asyncTaskPriority_);
    }
}

AsyncRunner* AsyncRunner::CreateGlobalRunner(const std::string& name, uint32_t runningCapacity,
                                             uint32_t waitingCapacity)
{
    AsyncRunner* asyncRunner = new AsyncRunner();
    asyncRunner->waitingCapacity_ = waitingCapacity;
    asyncRunner->runningCapacity_ = runningCapacity;
    asyncRunner->isGlobalRunner_ = true;
    asyncRunner->name_ = name;
    return asyncRunner;
}

void AsyncRunner::DecreaseRunningCount()
{
    if (runningCount_ > 0) {
        runningCount_.fetch_sub(1);
    }
}
} // namespace Commonlibrary::Concurrent::TaskPoolModule