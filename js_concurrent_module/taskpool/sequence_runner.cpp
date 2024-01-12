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

#include "helper/error_helper.h"
#include "helper/napi_helper.h"
#include "helper/object_helper.h"
#include "task_manager.h"
#include "utils/log.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
using namespace Commonlibrary::Concurrent::Common::Helper;
static constexpr char EXECUTE_STR[] = "execute";
static constexpr char SEQ_RUNNER_ID_STR[] = "seqRunnerId";

napi_value SequenceRunner::SeqRunnerConstructor(napi_env env, napi_callback_info cbinfo)
{
    uint32_t priority = Priority::DEFAULT;
    // get input args out of env and cbinfo
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    napi_value* args = new napi_value[argc];
    ObjectScope<napi_value> scope(args, true);
    napi_value thisVar;
    napi_get_cb_info(env, cbinfo, &argc, args, &thisVar, nullptr);

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
    napi_value seqRunnerId;
    uint32_t id = TaskManager::GetInstance().GenerateSeqRunnerId();
    napi_create_uint32(env, id, &seqRunnerId);
    napi_value seqRunnerExecuteFunc;
    napi_create_function(env, EXECUTE_STR, NAPI_AUTO_LENGTH, Execute, NULL, &seqRunnerExecuteFunc);
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_PROPERTY(SEQ_RUNNER_ID_STR, seqRunnerId),
        DECLARE_NAPI_FUNCTION(EXECUTE_STR, Execute),
    };
    napi_define_properties(env, thisVar, sizeof(properties) / sizeof(properties[0]), properties);
    HILOG_DEBUG("seqRunner:: construct seqRunner %{public}d.", id);
    // register seqRunner to TaskGroupManager
    RegisterSeqRunner(env, id, static_cast<Priority>(priority));

    uint32_t* data = new uint32_t();
    *data = id;
    napi_wrap(env, thisVar, data, Destructor, nullptr, nullptr);
    return thisVar;
}

napi_value SequenceRunner::Execute(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    if (argc < 1) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "seqRunner:: number of params at least one");
        return nullptr;
    }
    // check the first param is task
    napi_value* args = new napi_value[argc];
    napi_value thisVar;
    ObjectScope<napi_value> scope(args, true);
    napi_get_cb_info(env, cbinfo, &argc, args, &thisVar, nullptr);
    napi_valuetype type;
    napi_typeof(env, args[0], &type);
    if (type != napi_object || !NapiHelper::HasNameProperty(env, args[0], TASKID_STR)) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "seqRunner:: first param must be task.");
        return nullptr;
    }
    // get seqRunnerId from napi object
    napi_value napiSeqRunnerId = NapiHelper::GetNameProperty(env, thisVar, SEQ_RUNNER_ID_STR);
    uint32_t seqRunnerId = NapiHelper::GetUint32Value(env, napiSeqRunnerId);
    SeqRunnerInfo* seqRunnerInfo = TaskGroupManager::GetInstance().GetSeqRunnerInfoById(seqRunnerId);
    if (seqRunnerInfo == nullptr) {
        HILOG_ERROR("seqRunner:: object not exist.");
        return nullptr;
    }
    // task with dependence is not allowed
    napi_value napiTaskId = NapiHelper::GetNameProperty(env, args[0], TASKID_STR);
    uint32_t taskId = NapiHelper::GetUint32Value(env, napiTaskId);
    std::string errMessage = "";
    if (TaskManager::GetInstance().IsDependentByTaskId(taskId)) {
        errMessage = "seqRunner:: dependent task not allowed.";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_ADD_DEPENDENT_TASK_TO_SEQRUNNER, errMessage.c_str());
        return nullptr;
    }
    if (TaskManager::GetInstance().IsExecutedByTaskId(taskId)) {
        errMessage = "taskpool:: SequenceRunner cannot execute seqRunnerTask or executedTask";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, errMessage.c_str());
        return nullptr;
    }
    if (TaskManager::GetInstance().IsGroupTask(taskId)) {
        errMessage = "taskpool:: SequenceRunner cannot execute groupTask";
        HILOG_ERROR("%{public}s", errMessage.c_str());
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, errMessage.c_str());
        return nullptr;
    }
    TaskManager::GetInstance().StoreTaskType(taskId, false);
    // generate TaskInfo with seqRunnerId
    uint32_t executeId = TaskManager::GetInstance().GenerateExecuteId();
    TaskInfo* taskInfo = TaskManager::GetInstance().GenerateTaskInfoFromTask(env, args[0], executeId);
    if (taskInfo == nullptr) {
        HILOG_ERROR("seqRunner:: seqRunnerInfo is nullptr");
        return nullptr;
    }
    taskInfo->seqRunnerId = seqRunnerId;
    napi_value promise = NapiHelper::CreatePromise(env, &taskInfo->deferred);
    TaskManager::GetInstance().StoreRunningInfo(taskInfo->taskId, executeId);
    // decide whether to execute it immediately or enqueue
    if (seqRunnerInfo->currentExeId == 0) {
        HILOG_DEBUG("seqRunner:: execute %{public}d in seqRunner %{public}d immediately.", executeId, seqRunnerId);
        ExecuteTaskImmediately(executeId, seqRunnerInfo->priority);
        seqRunnerInfo->currentExeId = executeId;
    } else {
        HILOG_DEBUG("seqRunner:: add %{public}d to seqRunner %{public}d.", executeId, seqRunnerId);
        TaskGroupManager::GetInstance().AddTaskToSeqRunner(seqRunnerId, taskInfo);
    }
    return promise;
}

void SequenceRunner::ExecuteTaskImmediately(uint32_t executeId, Priority priority)
{
    TaskManager::GetInstance().AddExecuteState(executeId);
    TaskManager::GetInstance().EnqueueExecuteId(executeId, priority);
    TaskManager::GetInstance().TryTriggerExpand();
}

void SequenceRunner::Destructor(napi_env env, void* data, [[maybe_unused]] void* hint)
{
    uint32_t* seqRunnerId = static_cast<uint32_t*>(data);
    TaskGroupManager::GetInstance().ClearSeqRunner(*seqRunnerId);
    delete seqRunnerId;
}

void SequenceRunner::RegisterSeqRunner(napi_env env, uint32_t seqRunnerId, Priority pri)
{
    SeqRunnerInfo* seqRunnerInfo = new SeqRunnerInfo();
    seqRunnerInfo->priority = pri;
    TaskGroupManager::GetInstance().AddSeqRunnerInfoById(seqRunnerId, seqRunnerInfo);
}
}
