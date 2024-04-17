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

#include "worker_handler.h"
#include "worker.h"

#include "tools/log.h"

using Worker = Commonlibrary::Concurrent::WorkerModule::Worker;
namespace OHOS::AppExecFwk {
    WorkerEventHandler::WorkerEventHandler(const std::shared_ptr<EventRunner> &runner) : EventHandler(runner)
    {}

    std::shared_ptr<WorkerEventHandler> WorkerEventHandler::GetInstance()
    {
        std::shared_ptr<EventRunner> runner = EventRunner::Current();
        if (runner.get() == nullptr) {
            HILOG_ERROR("HYQ:: failed to create EventRunner");
            runner = EventRunner::Create(false);
        }
        if (runner.get() == nullptr) {
            HILOG_ERROR("worker:: failed to create EventRunner");
            return nullptr;
        }
        auto instance = std::make_shared<WorkerEventHandler>(runner);
        return instance;
    }

    void WorkerEventHandler::ProcessEvent([[maybe_unused]] const InnerEvent::Pointer &event)
    {}

    void WorkerLoopHandler::OnReadable(int32_t)
    {
        HILOG_DEBUG("worker:: OnReadable is triggered");
        OnTriggered();
    }

    void WorkerLoopHandler::OnWritable(int32_t)
    {
        HILOG_DEBUG("worker:: OnWritable is triggered");
        OnTriggered();
    }

    void WorkerLoopHandler::OnTriggered()
    {
        HILOG_DEBUG("worker:: OnTriggered is triggered");
        uv_run(uvLoop_, UV_RUN_NOWAIT);

        auto eventHandler = GetOwner();
        if (!eventHandler) {
            return;
        }

        int32_t timeout = uv_backend_timeout(uvLoop_);
        if (timeout < 0) {
            if (haveTimerTask_) {
                eventHandler->RemoveTask(TIMER_TASK);
            }
            return;
        }

        int64_t timeStamp = static_cast<int64_t>(uv_now(uvLoop_)) + timeout;
        // we don't check timestamp in emulator for computer clock is inaccurate
    #ifndef RUNTIME_EMULATOR
        if (timeStamp == lastTimeStamp_) {
            return;
        }
    #endif

        if (haveTimerTask_) {
            eventHandler->RemoveTask(TIMER_TASK);
        }

        auto callback = [wp = weak_from_this()] {
            auto sp = wp.lock();
            if (sp) {
                // Timer task is triggered, so there is no timer task now.
                sp->haveTimerTask_ = false;
                sp->OnTriggered();
            }
        };
        eventHandler->PostTask(callback, TIMER_TASK, timeout);
        lastTimeStamp_ = timeStamp;
        haveTimerTask_ = true;
    }
}