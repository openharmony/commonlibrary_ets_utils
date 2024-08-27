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

#include "task_runner.h"

#include "helper/concurrent_helper.h"
#include "helper/object_helper.h"
#include "tools/log.h"

namespace Commonlibrary::Concurrent::TaskPoolModule {
using namespace Commonlibrary::Concurrent::Common::Helper;
TaskRunner::TaskRunner(TaskStartCallback callback) : callback_(callback), selfThreadId_(uv_thread_self()) {}

TaskRunner::~TaskRunner()
{
    CloseHelp::DeletePointer(taskInnerRunner_, false);
}

void TaskRunner::TaskInnerRunner::Run()
{
    if (LIKELY(runner_ != nullptr)) {
        runner_->Run();
    } else { // LCOV_EXCL_BR_LINE
        HILOG_FATAL("taskpool:: runner_ is null");
    }
}

TaskRunner::TaskInnerRunner::TaskInnerRunner(const TaskRunner* runner) : runner_(runner) {}

void TaskRunner::Run() const
{
    if (LIKELY(callback_.callback != nullptr)) {
        callback_.callback(callback_.data);
    } else { // LCOV_EXCL_BR_LINE
        HILOG_FATAL("taskpool:: callback_.callback is null");
    }
}

bool TaskRunner::Execute()
{
    taskInnerRunner_ = new TaskInnerRunner(this);
    return taskInnerRunner_->Start();
}
} // namespace Commonlibrary::Concurrent::TaskPoolModule
