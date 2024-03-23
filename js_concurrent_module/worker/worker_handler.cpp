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

#include "utils/log.h"

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
        }

        if (!isMainThread) {
            HILOG_INFO("worker:: WorkerEventHandler::GetInstance runner run");
            runner->Run();
        }
        return instance;
    }

    void WorkerEventHandler::ProcessEvent([[maybe_unused]] const InnerEvent::Pointer &event)
    {}
}