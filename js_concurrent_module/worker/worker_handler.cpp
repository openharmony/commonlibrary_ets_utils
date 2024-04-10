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

    std::shared_ptr<WorkerEventHandler> WorkerEventHandler::GetInstance(void* worker, bool isMainThread)
    {
        std::shared_ptr<EventRunner> runner = EventRunner::Current();
        if (runner.get() == nullptr) {
            runner = EventRunner::Create(false);
        }
        if (runner.get() == nullptr) {
            HILOG_ERROR("worker:: failed to create EventRunner");
            return nullptr;
        }
        auto instance = std::make_shared<WorkerEventHandler>(runner);

        if (worker != nullptr) {
            Worker* workerInstance = reinterpret_cast<Worker*>(worker);
            workerInstance->SetEventRunner(runner);
            workerInstance->SetWorkerHandle(instance.get());
            auto uvLoop = workerInstance->GetWorkerLoop();
            if (uvLoop == nullptr) {
                HILOG_ERROR("worker:: Failed to get uv loop");
                return nullptr;
            }
            auto fd = uvLoop != nullptr ? uv_backend_fd(uvLoop) : -1;
            if (fd < 0) {
                HILOG_ERROR("worker:: Failed to get backend fd from uv loop");
                return nullptr;
            }

            uv_run(uvLoop, UV_RUN_NOWAIT);

            if (instance != nullptr) {
                uint32_t events = FILE_DESCRIPTOR_INPUT_EVENT | FILE_DESCRIPTOR_OUTPUT_EVENT;
                instance->AddFileDescriptorListener(fd, events,
                                                    std::make_shared<WorkerLoopHandler>(uvLoop), "workerLoopTask");
            }
        }

        if (!isMainThread && worker != nullptr) {
            HILOG_DEBUG("worker:: worker start run.");
            runner->Run();
        }
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