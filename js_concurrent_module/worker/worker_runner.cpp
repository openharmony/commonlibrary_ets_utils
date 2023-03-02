/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "worker_runner.h"

#include "helper/object_helper.h"

namespace Commonlibrary::Concurrent::WorkerModule {
WorkerRunner::WorkerRunner(WorkerStartCallback callback) : callback_(callback), selfThreadId_(uv_thread_self()) {}

WorkerRunner::~WorkerRunner()
{
    Common::Helper::CloseHelp::DeletePointer(workerInnerRunner_, false);
}

void WorkerRunner::WorkerInnerRunner::Run()
{
    if (runner_ != nullptr) {
        runner_->Run();
    }
}

WorkerRunner::WorkerInnerRunner::WorkerInnerRunner(const WorkerRunner* runner) : runner_(runner) {}

void WorkerRunner::Run() const
{
    if (callback_.callback != nullptr) {
        callback_.callback(callback_.data);
    }
}

bool WorkerRunner::Execute()
{
    workerInnerRunner_ = new WorkerInnerRunner(this);
    return workerInnerRunner_->Start();
}
} // namespace Commonlibrary::Concurrent::WorkerModule
