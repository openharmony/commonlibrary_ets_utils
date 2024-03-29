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
#include "sequence_runner.h"

#include <cinttypes>

#include "helper/error_helper.h"
#include "helper/napi_helper.h"
#include "helper/object_helper.h"
#include "task_manager.h"
#include "tools/log.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
using namespace Commonlibrary::Concurrent::Common::Helper;
static constexpr char EXECUTE_STR[] = "execute";
static constexpr char SEQ_RUNNER_ID_STR[] = "seqRunnerId";

napi_value SequenceRunner::SeqRunnerConstructor(napi_env env, napi_callback_info cbinfo)
{
    // get input args out of env and cbinfo
    size_t argc = 1;
    napi_value args[1];
    napi_value thisVar;
    napi_get_cb_info(env, cbinfo, &argc, args, &thisVar, nullptr);

    uint32_t priority = Priority::DEFAULT;
    if (argc > 0) {
        HILOG_DEBUG("seqRunner:: parse priority params.");
        if (!NapiHelper::IsNumber(env, args[0])) {
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "seqRunner:: first param must be priority.");
            return nullptr;
        }
        priority = NapiHelper::GetUint32Value(env, args[0]);
        if (priority >= Priority::NUMBER) {
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "seqRunner:: priority value unvalied.");
            return nullptr;
        }
    }
    // update seqRunner.seqRunnerId
    SequenceRunner* seqRunner = new SequenceRunner();
    uint64_t seqRunnerId = reinterpret_cast<uint64_t>(seqRunner);
    napi_value napiSeqRunnerId = NapiHelper::CreateUint64(env, seqRunnerId);
    TaskGroupManager::GetInstance().StoreSequenceRunner(seqRunnerId, seqRunner);
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_PROPERTY(SEQ_RUNNER_ID_STR, napiSeqRunnerId),
        DECLARE_NAPI_FUNCTION(EXECUTE_STR, Execute),
    };
    napi_define_properties(env, thisVar, sizeof(properties) / sizeof(properties[0]), properties);
    HILOG_DEBUG("seqRunner:: construct seqRunner %{public}" PRIu64 ".", seqRunnerId);

    seqRunner->priority_ = static_cast<Priority>(priority);
    seqRunner->seqRunnerId_ = seqRunnerId;
    napi_wrap(env, thisVar, seqRunner, SequenceRunnerDestructor, nullptr, nullptr);
    napi_create_reference(env, thisVar, 0, &seqRunner->seqRunnerRef_);
    return thisVar;
}

napi_value SequenceRunner::Execute(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = 1;
    napi_value args[1];
    napi_value thisVar;
    napi_get_cb_info(env, cbinfo, &argc, args, &thisVar, nullptr);
    std::string errMessage = "";
    if (argc < 1) {
        errMessage = "seqRunner:: number of params at least one";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, errMessage.c_str());
        return nullptr;
    }
    if (!NapiHelper::IsObject(env, args[0]) || !NapiHelper::HasNameProperty(env, args[0], TASKID_STR)) {
        errMessage = "seqRunner:: first param must be task.";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, errMessage.c_str());
        return nullptr;
    }
    napi_value napiSeqRunnerId = NapiHelper::GetNameProperty(env, thisVar, SEQ_RUNNER_ID_STR);
    uint64_t seqRunnerId = NapiHelper::GetUint64Value(env, napiSeqRunnerId);
    SequenceRunner* seqRunner = TaskGroupManager::GetInstance().GetSeqRunner(seqRunnerId);
    if (seqRunner == nullptr) {
        return nullptr;
    }
    napi_reference_ref(env, seqRunner->seqRunnerRef_, nullptr);
    Task* task = nullptr;
    napi_unwrap(env, args[0], reinterpret_cast<void**>(&task));
    if (task == nullptr) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "taskGroup:: the type of the params must be task");
        return nullptr;
    }
    if (!task->CanForSequenceRunner(env)) {
        return nullptr;
    }
    task->seqRunnerId_ = seqRunnerId;
    napi_value promise = task->GetTaskInfoPromise(env, args[0], TaskType::SEQRUNNER_TASK, seqRunner->priority_);
    if (promise == nullptr) {
        return nullptr;
    }
    if (seqRunner->currentTaskId_ == 0) {
        HILOG_DEBUG("seqRunner:: task %{public}" PRIu64 " in seqRunner %{public}" PRIu64 " immediately.",
                    task->taskId_, seqRunnerId);
        seqRunner->currentTaskId_ = task->taskId_;
        task->IncreaseRefCount();
        task->taskState_ = ExecuteState::WAITING;
        ExecuteTaskImmediately(task->taskId_, seqRunner->priority_);
    } else {
        HILOG_DEBUG("seqRunner:: add %{public}" PRIu64 " to seqRunner %{public}" PRIu64 ".",
                    task->taskId_, seqRunnerId);
        TaskGroupManager::GetInstance().AddTaskToSeqRunner(seqRunnerId, task);
    }
    return promise;
}

void SequenceRunner::ExecuteTaskImmediately(uint64_t taskId, Priority priority)
{
    TaskManager::GetInstance().EnqueueTaskId(taskId, priority);
    TaskManager::GetInstance().TryTriggerExpand();
}

void SequenceRunner::SequenceRunnerDestructor(napi_env env, void* data, [[maybe_unused]] void* hint)
{
    SequenceRunner* seqRunner = static_cast<SequenceRunner*>(data);
    TaskGroupManager::GetInstance().RemoveSequenceRunner(seqRunner->seqRunnerId_);
    delete seqRunner;
}
} // namespace Commonlibrary::Concurrent::TaskPoolModule