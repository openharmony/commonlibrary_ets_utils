/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "condition_task.h"
#include "condition_variable.h"
#include "helper/napi_helper.h"
#include "helper/object_helper.h"
#include "tools/log.h"
#include "uv.h"

namespace Commonlibrary::Concurrent::Condition {

ConditionTask::ConditionTask(ConditionVariable *cond, napi_env env, napi_deferred deferred, uint64_t timeout)
    : cond_(cond), env_(env), deferred_(deferred)
{
    AddEnvCleanupHook();
    InitTimer(timeout);
}

ConditionTask::~ConditionTask()
{
    CloseTimer();
}

napi_env ConditionTask::GetEnv()
{
    return env_;
}

void ConditionTask::Finish(ConditionTaskFinishReason reason)
{
    switch (reason) {
        case ConditionTaskFinishReason::NOTIFY:
            StopTimer();
            RemoveEnvCleanupHook();
            ResolvePromise();
            break;
        case ConditionTaskFinishReason::TIMEOUT:
            RemoveEnvCleanupHook();
            ResolvePromise();
            break;
        case ConditionTaskFinishReason::ENV_CLEANUP:
            StopTimer();
            RejectPromise();
            break;
    }
    delete this;
}

void ConditionTask::EnvCleanup(void *arg)
{
    ConditionTask *task {static_cast<ConditionTask *>(arg)};
    task->cond_->FindAndFinishTask(task, ConditionTaskFinishReason::ENV_CLEANUP);
}

void ConditionTask::ResolvePromise()
{
    napi_status status {napi_ok};
    Common::Helper::HandleScope scope(env_, status);
    NAPI_CALL_RETURN_VOID(env_, status);

    napi_value result;
    NAPI_CALL_RETURN_VOID(env_, napi_get_undefined(env_, &result));
    NAPI_CALL_RETURN_VOID(env_, napi_resolve_deferred(env_, deferred_, result));
}

void ConditionTask::RejectPromise()
{
    napi_status status {napi_ok};
    Common::Helper::HandleScope scope(env_, status);
    NAPI_CALL_RETURN_VOID(env_, status);

    napi_value result;
    NAPI_CALL_RETURN_VOID(env_, napi_get_undefined(env_, &result));
    NAPI_CALL_RETURN_VOID(env_, napi_reject_deferred(env_, deferred_, result));
}

void ConditionTask::InitTimer(uint64_t timeout)
{
    if (timeout < 0) {
        return;
    }
    timer_ = new uv_timer_t;
    uv_timer_init(Common::Helper::NapiHelper::GetLibUV(env_), timer_);
    timer_->data = this;
    uv_timer_start(
        timer_,
        [](uv_timer_t *timer) {
            ConditionTask *task {static_cast<ConditionTask *>(timer->data)};
            task->cond_->FindAndFinishTask(task, ConditionTaskFinishReason::TIMEOUT);
        },
        timeout, 0);
}

void ConditionTask::StopTimer()
{
    if (timer_ == nullptr) {
        return;
    }
    uv_timer_stop(timer_);
}

void ConditionTask::CloseTimer()
{
    if (timer_ == nullptr) {
        return;
    }
    uv_close(reinterpret_cast<uv_handle_t *>(timer_), [](uv_handle_t *handle) { delete handle; });
}

void ConditionTask::AddEnvCleanupHook()
{
    NAPI_CALL_RETURN_VOID(env_, napi_add_env_cleanup_hook(env_, EnvCleanup, this));
}

void ConditionTask::RemoveEnvCleanupHook()
{
    NAPI_CALL_RETURN_VOID(env_, napi_remove_env_cleanup_hook(env_, EnvCleanup, this));
}

}  // namespace Commonlibrary::Concurrent::Condition
