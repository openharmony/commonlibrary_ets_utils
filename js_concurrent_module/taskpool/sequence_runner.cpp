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

bool SequenceRunner::SeqRunnerConstructorInner(napi_env env, napi_value &thisVar, SequenceRunner *seqRunner)
{
    // update seqRunner.seqRunnerId
    uint64_t seqRunnerId = reinterpret_cast<uint64_t>(seqRunner);
    napi_value napiSeqRunnerId = NapiHelper::CreateUint64(env, seqRunnerId);
    TaskGroupManager::GetInstance().StoreSequenceRunner(seqRunnerId, seqRunner);
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_PROPERTY(SEQ_RUNNER_ID_STR, napiSeqRunnerId),
        DECLARE_NAPI_FUNCTION(EXECUTE_STR, Execute),
    };
    napi_define_properties(env, thisVar, sizeof(properties) / sizeof(properties[0]), properties);
    HILOG_INFO("taskpool:: construct seqRunner name is %{public}s, seqRunnerid %{public}s.",
               seqRunner->seqName_.c_str(), std::to_string(seqRunnerId).c_str());

    seqRunner->seqRunnerId_ = seqRunnerId;
    napi_status status = napi_wrap(env, thisVar, seqRunner, SequenceRunnerDestructor, nullptr, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR("taskpool::SeqRunnerConstructorInner napi_wrap return value is %{public}d", status);
        SequenceRunnerDestructor(env, seqRunner, nullptr);
        return false;
    }
    return true;
}

napi_value SequenceRunner::SeqRunnerConstructor(napi_env env, napi_callback_info cbinfo)
{
    // get input args out of env and cbinfo
    size_t argc = 2; // 2: The maximum number of parameters is 2
    napi_value args[2]; // 2: The maximum number of parameters is 2
    napi_value thisVar;
    napi_get_cb_info(env, cbinfo, &argc, args, &thisVar, nullptr);

    uint32_t priority = Priority::DEFAULT;
    std::string name = "";
    if (argc == 2) { // 2: The number of parameters is 2, if the first is seqRunner name, the second must be priority
        if (NapiHelper::IsString(env, args[0]) && NapiHelper::IsNumber(env, args[1])) {
            name = NapiHelper::GetString(env, args[0]);
            priority = NapiHelper::GetUint32Value(env, args[1]);
            if (priority >= Priority::NUMBER) {
                ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "priority value unvalied.");
                return nullptr;
            }
        } else {
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
                "the type of first param must be string and the type of second param must be string.");
            return nullptr;
        }
    } else if (argc == 1) {
        if (NapiHelper::IsString(env, args[0])) {
            name = NapiHelper::GetString(env, args[0]);
        } else if (NapiHelper::IsNumber(env, args[0])) {
            priority = NapiHelper::GetUint32Value(env, args[0]);
            if (priority >= Priority::NUMBER) {
                ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "priority value unvalied.");
                return nullptr;
            }
        } else {
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "the type of first param must be string or number.");
            return nullptr;
        }
    }

    SequenceRunner* seqRunner = nullptr;
    if (name != "") {
        seqRunner = SequenceRunnerManager::GetInstance().CreateOrGetGlobalRunner(env, thisVar, argc, name, priority);
        if (seqRunner == nullptr) {
            HILOG_ERROR("taskpool:: create or get globalRunner failed");
            return nullptr;
        }
    } else {
        seqRunner = new SequenceRunner();
        seqRunner->priority_ = static_cast<Priority>(priority);
        napi_create_reference(env, thisVar, 0, &seqRunner->seqRunnerRef_);
    }

    if (!SeqRunnerConstructorInner(env, thisVar, seqRunner)) {
        HILOG_ERROR("taskpool:: SeqRunnerConstructorInner failed");
        return nullptr;
    }
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
        HILOG_ERROR("taskpool:: %{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "the number of param at least one.");
        return nullptr;
    }
    if (!NapiHelper::IsObject(env, args[0]) || !NapiHelper::HasNameProperty(env, args[0], TASKID_STR)) {
        errMessage = "seqRunner:: first param must be task.";
        HILOG_ERROR("taskpool:: %{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "the type of the first param must be task.");
        return nullptr;
    }
    napi_value napiSeqRunnerId = NapiHelper::GetNameProperty(env, thisVar, SEQ_RUNNER_ID_STR);
    uint64_t seqRunnerId = NapiHelper::GetUint64Value(env, napiSeqRunnerId);
    SequenceRunner* seqRunner = TaskGroupManager::GetInstance().GetSeqRunner(seqRunnerId);
    if (seqRunner == nullptr) {
        return nullptr;
    }
    Task* task = nullptr;
    napi_unwrap(env, args[0], reinterpret_cast<void**>(&task));
    if (task == nullptr) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "the type of param must be task.");
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
    napi_reference_ref(env, seqRunner->seqRunnerRef_, nullptr);
    if (seqRunner->currentTaskId_ == 0) {
        HILOG_INFO("taskpool:: taskId %{public}s in seqRunner %{public}s immediately.",
                   std::to_string(task->taskId_).c_str(), std::to_string(seqRunnerId).c_str());
        seqRunner->currentTaskId_ = task->taskId_;
        task->IncreaseRefCount();
        task->taskState_ = ExecuteState::WAITING;
        ExecuteTaskImmediately(task->taskId_, seqRunner->priority_);
    } else {
        HILOG_INFO("taskpool:: add taskId: %{public}s to seqRunner %{public}s.",
                   std::to_string(task->taskId_).c_str(), std::to_string(seqRunnerId).c_str());
        TaskGroupManager::GetInstance().AddTaskToSeqRunner(seqRunnerId, task);
    }
    return promise;
}

void SequenceRunner::ExecuteTaskImmediately(uint64_t taskId, Priority priority)
{
    TaskManager::GetInstance().EnqueueTaskId(taskId, priority);
}

void SequenceRunner::SequenceRunnerDestructor(napi_env env, void* data, [[maybe_unused]] void* hint)
{
    SequenceRunner* seqRunner = static_cast<SequenceRunner*>(data);
    if (seqRunner->isGlobalRunner_) {
        SequenceRunnerManager::GetInstance().GlobalSequenceRunnerDestructor(env, seqRunner);
    } else {
        TaskGroupManager::GetInstance().RemoveSequenceRunner(seqRunner->seqRunnerId_);
        napi_delete_reference(env, seqRunner->seqRunnerRef_);
        delete seqRunner;
    }
}
} // namespace Commonlibrary::Concurrent::TaskPoolModule