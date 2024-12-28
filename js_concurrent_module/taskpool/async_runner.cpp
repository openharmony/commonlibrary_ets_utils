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

#include "async_runner_manager.h"
#include "helper/error_helper.h"
#include "helper/napi_helper.h"
#include "task_manager.h"
#include "tools/log.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
using namespace Commonlibrary::Concurrent::Common::Helper;
static constexpr char EXECUTE_STR[] = "execute";

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

bool AsyncRunner::AsyncRunnerConstructorInner(napi_env env, napi_value &thisVar, AsyncRunner *asyncRunner)
{
    uint64_t asyncRunnerId = reinterpret_cast<uint64_t>(asyncRunner);
    asyncRunner->asyncRunnerId_ = asyncRunnerId;
    napi_value napiAsyncRunnerId = NapiHelper::CreateUint64(env, asyncRunnerId);
    AsyncRunnerManager::GetInstance().StoreAsyncRunner(asyncRunnerId, asyncRunner);
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION(EXECUTE_STR, Execute),
    };
    napi_define_properties(env, thisVar, sizeof(properties) / sizeof(properties[0]), properties);
    HILOG_INFO("taskpool:: construct asyncRunner name is %{public}s, asyncRunnerId %{public}s.",
               asyncRunner->name_.c_str(), std::to_string(asyncRunnerId).c_str());
    napi_status status = napi_wrap(env, thisVar, asyncRunner, AsyncRunnerDestructor, nullptr, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR("taskpool:: AsyncRunnerConstructorInner napi_wrap return value is %{public}d.", status);
        AsyncRunnerDestructor(env, asyncRunner, nullptr);
        return false;
    }
    return true;
}

napi_value AsyncRunner::Execute(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = 2; // 2 : task, Priority
    napi_value args[2];
    napi_value thisVar;
    napi_get_cb_info(env, cbinfo, &argc, args, &thisVar, nullptr);
    if (argc < 1 || argc > 2) { // 2 : task, Priority
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "The numbers of params not more than two or less one.");
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
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "The type of param must be task.");
        return nullptr;
    }
    if (!task->CanForAsyncRunner(env)) {
        return nullptr;
    }
    uint32_t priority = NapiHelper::GetUint32Value(env, napiPriority);
    task->asyncRunnerId_ = asyncRunner->asyncRunnerId_;
    task->asyncTaskPriority_ = static_cast<Priority>(priority);
    napi_value promise = task->GetTaskInfoPromise(env, napiTask, TaskType::ASYNCRUNNER_TASK, task->asyncTaskPriority_);
    if (promise == nullptr) {
        return nullptr;
    }
    if (!asyncRunner->IncreaseAsyncRunnerRef(env)) {
        return nullptr;
    }

    if (!AddTasksToAsyncRunner(asyncRunner, task)) {
        ExecuteTaskImmediately(asyncRunner, task);
    }
    
    return promise;
}

AsyncRunner* AsyncRunner::CheckAndCreateAsyncRunner(napi_env env, napi_value &thisVar, napi_value name,
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
        napi_create_reference(env, thisVar, 0, &asyncRunner->asyncRunnerRef_);
    }
    return asyncRunner;
}

bool AsyncRunner::CheckExecuteArgs(napi_env env, napi_value napiTask, napi_value napiPriority)
{
    if (!NapiHelper::IsObject(env, napiTask) || !NapiHelper::HasNameProperty(env, napiTask, TASKID_STR)) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "First param must be task.");
        return false;
    }
    if (napiPriority != nullptr) {
        if (!NapiHelper::IsNumber(env, napiPriority)) {
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "The type of the second param must be number.");
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
                std::to_string(task->taskId_).c_str(), std::to_string(asyncRunner->asyncRunnerId_).c_str());
    asyncRunner->runningCount_.fetch_add(1);
    task->IncreaseRefCount();
    TaskManager::GetInstance().IncreaseRefCount(task->taskId_);
    task->taskState_ = ExecuteState::WAITING;
    TaskManager::GetInstance().EnqueueTaskId(task->taskId_, task->asyncTaskPriority_);
}

bool AsyncRunner::AddTasksToAsyncRunner(AsyncRunner* asyncRunner, Task* task)
{
    Task* frontTask = nullptr;
    {
        std::unique_lock<std::shared_mutex> asyncRunnerLock(asyncRunner->waitingTasksMutex_);
        if (asyncRunner->runningCount_ < asyncRunner->runningCapacity_) {
            return false;
        }
        if (asyncRunner->waitingCapacity_ && asyncRunner->waitingTasks_.size() == asyncRunner->waitingCapacity_) {
            frontTask = asyncRunner->waitingTasks_.front();
            asyncRunner->waitingTasks_.pop_front();
            frontTask->taskState_ = ExecuteState::CANCELED;
        }
        asyncRunner->waitingTasks_.push_back(task);
    }

    if (frontTask != nullptr) {
        asyncRunner->TriggerRejectErrorTimer(frontTask, ErrorHelper::ERR_ASYNCRUNNER_TASK_DISCARDED);
    }
    return true;
}

void AsyncRunner::AsyncRunnerDestructor(napi_env env, void* data, [[maybe_unused]] void* hint)
{
    AsyncRunner* asyncRunner = static_cast<AsyncRunner*>(data);
    if (env == nullptr || asyncRunner == nullptr) {
        return;
    }
    if (asyncRunner->isGlobalRunner_) {
        AsyncRunnerManager::GetInstance().GlobalAsyncRunnerDestructor(env, asyncRunner);
    } else {
        AsyncRunnerManager::GetInstance().RemoveAsyncRunner(asyncRunner->asyncRunnerId_);
        napi_delete_reference(env, asyncRunner->asyncRunnerRef_);
        delete asyncRunner;
    }
}

bool AsyncRunner::RemoveWaitingTask(Task* task)
{
    bool flag = false;
    {
        std::unique_lock<std::shared_mutex> lock(waitingTasksMutex_);
        for (auto iter = waitingTasks_.begin(); iter != waitingTasks_.end();) {
            auto taskTmp = *iter;
            if (taskTmp == task) {
                iter = waitingTasks_.erase(iter);
                flag = true;
                break;
            } else {
                iter++;
            }
        }
    }
    if (flag) {
        TriggerRejectErrorTimer(task, ErrorHelper::ERR_ASYNCRUNNER_TASK_CANCELED);
    }
    return flag;
}

void AsyncRunner::TriggerRejectErrorTimer(Task* task, int32_t errCode, bool isWaiting)
{
    uv_loop_t* loop = NapiHelper::GetLibUV(task->env_);
    if (loop == nullptr) {
        return;
    }
    uv_update_time(loop);
    uv_timer_t* timer = new uv_timer_t;
    uv_timer_init(loop, timer);
    ErrorMessage* errorMessage = new ErrorMessage();
    errorMessage->errCode = errCode;
    errorMessage->taskId = task->taskId_;
    errorMessage->isWaiting = isWaiting;
    timer->data = errorMessage;
    uv_timer_start(timer, reinterpret_cast<uv_timer_cb>(RejectError), 0, 0);
    NativeEngine* engine = reinterpret_cast<NativeEngine*>(task->env_);
    if (engine->IsMainThread()) {
        uv_async_send(&loop->wq_async);
    } else {
        uv_work_t* work = new uv_work_t;
        uv_queue_work_with_qos(loop, work, [](uv_work_t*) {},
                               [](uv_work_t* work, int32_t) { delete work; }, uv_qos_user_initiated);
    }
}

void AsyncRunner::RejectError(uv_timer_t* handle)
{
    ErrorMessage* errorMessage = static_cast<ErrorMessage*>(handle->data);
    auto task = TaskManager::GetInstance().GetTask(errorMessage->taskId);
    if (task == nullptr) {
        HILOG_DEBUG("taskpool:: task is nullptr.");
    } else {
        napi_status status = napi_ok;
        HandleScope scope(task->env_, status);
        if (status != napi_ok) {
            HILOG_ERROR("taskpool:: napi_open_handle_scope failed");
            return;
        }
        TaskManager::GetInstance().DecreaseRefCount(task->env_, task->taskId_);
        napi_value error = ErrorHelper::NewError(task->env_, errorMessage->errCode);
        napi_reject_deferred(task->env_, task->currentTaskInfo_->deferred, error);
        TaskGroupManager::GetInstance().DisposeCanceledTask(task->env_, task);
        TaskManager::GetInstance().RemoveTask(errorMessage->taskId);
        auto asyncRunner = AsyncRunnerManager::GetInstance().GetAsyncRunner(task->asyncRunnerId_);
        if (asyncRunner != nullptr && !errorMessage->isWaiting) {
            asyncRunner->DecreaseAsyncRunnerRef(task->env_);
        }
    }
    uv_timer_stop(handle);
    ConcurrentHelper::UvHandleClose(handle);
    delete errorMessage;
    errorMessage = nullptr;
}

void AsyncRunner::TriggerWaitingTask()
{
    std::unique_lock<std::shared_mutex> lock(waitingTasksMutex_);
    runningCount_.fetch_sub(1);
    Task* task = nullptr;
    while (runningCount_ < runningCapacity_) {
        if (waitingTasks_.empty()) {
            HILOG_DEBUG("taskpool:: asyncRunner %{public}s empty.", std::to_string(asyncRunnerId_).c_str());
            break;
        }
        task = waitingTasks_.front();
        waitingTasks_.pop_front();
        runningCount_.fetch_add(1);
        task->IncreaseRefCount();
        TaskManager::GetInstance().IncreaseRefCount(task->taskId_);
        task->taskState_ = ExecuteState::WAITING;
        HILOG_DEBUG("taskpool:: Trig task %{public}s in asyncRunner %{public}s.",
                    std::to_string(task->taskId_).c_str(), std::to_string(asyncRunnerId_).c_str());
        TaskManager::GetInstance().EnqueueTaskId(task->taskId_, task->asyncTaskPriority_);
    }
}

AsyncRunner* AsyncRunner::CreateGlobalRunner(const std::string &name, uint32_t runningCapacity,
                                             uint32_t waitingCapacity)
{
    AsyncRunner* asyncRunner = new AsyncRunner();
    asyncRunner->waitingCapacity_ = waitingCapacity;
    asyncRunner->runningCapacity_ = runningCapacity;
    asyncRunner->isGlobalRunner_ = true;
    asyncRunner->name_ = name;
    return asyncRunner;
}

void AsyncRunner::CreateGlobalRef(napi_env env, napi_value thisVar)
{
    std::unique_lock<std::shared_mutex> asyncLock(asyncRunnerMutex_);
    count_++;
    auto tmpIter = globalAsyncRunnerRef_.find(env);
    if (tmpIter == globalAsyncRunnerRef_.end()) {
        napi_ref gloableAsyncRunnerRef = nullptr;
        napi_create_reference(env, thisVar, 0, &gloableAsyncRunnerRef);
        globalAsyncRunnerRef_.emplace(env, gloableAsyncRunnerRef);
    }
}

bool AsyncRunner::DecreaseAsyncRunnerRef(napi_env env)
{
    std::unique_lock<std::shared_mutex> lock(asyncRunnerMutex_);
    if (isGlobalRunner_) {
        auto iter = globalAsyncRunnerRef_.find(env);
        if (iter == globalAsyncRunnerRef_.end()) {
            return false;
        }
        napi_reference_unref(env, iter->second, nullptr);
    } else {
        napi_reference_unref(env, asyncRunnerRef_, nullptr);
    }
    return true;
}

bool AsyncRunner::IncreaseAsyncRunnerRef(napi_env env)
{
    std::unique_lock<std::shared_mutex> lock(asyncRunnerMutex_);
    if (isGlobalRunner_) {
        auto iter = globalAsyncRunnerRef_.find(env);
        if (iter == globalAsyncRunnerRef_.end()) {
            return false;
        }
        napi_reference_ref(env, iter->second, nullptr);
    } else {
        napi_reference_ref(env, asyncRunnerRef_, nullptr);
    }
    return true;
}

uint64_t AsyncRunner::DecreaseAsyncCount()
{
    return --count_;
}

void AsyncRunner::RemoveGlobalAsyncRunnerRef(napi_env env)
{
    std::unique_lock<std::shared_mutex> lock(asyncRunnerMutex_);
    auto iter = globalAsyncRunnerRef_.find(env);
    if (iter != globalAsyncRunnerRef_.end()) {
        napi_delete_reference(env, iter->second);
        globalAsyncRunnerRef_.erase(iter);
    }
}

bool AsyncRunner::CheckGlobalRunnerParams(napi_env env, uint32_t runningCapacity, uint32_t waitingCapacity)
{
    if (runningCapacity != runningCapacity_) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "AsyncRunner runningCapacity can not changed.");
        return false;
    }
    if (waitingCapacity != waitingCapacity_) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "AsyncRunner waitingCapacity can not changed.");
        return false;
    }
    return true;
}
} // namespace Commonlibrary::Concurrent::TaskPoolModule