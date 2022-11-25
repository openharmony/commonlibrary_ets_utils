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

#include "taskpool_runner.h"

#include "object_helper.h"

namespace CompilerRuntime::TaskPoolModule {
TaskPoolModule::TaskRunner(TaskStartCallback callback) : callback_(callback), selfThreadId_(uv_thread_self()) {}

TaskPoolModule::~TaskRunner()
{
    Helper::CloseHelp::DeletePointer(taskInnerRunner_, false);
}

void TaskPoolModule::TaskInnerRunner::Run()
{
    if (runner_ != nullptr) {
        runner_->Run();
    }
}

TaskPoolModule::TaskInnerRunner::TaskInnerRunner(const TaskRunner* runner) : runner_(runner) {}

void TaskRunner::Run() const
{
    if (callback_.callback != nullptr) {
        callback_.callback(callback_.data);
    }
}

bool TaskRunner::Execute()
{
    taskInnerRunner_ = new TaskInnerRunner(this);
    return taskInnerRunner_->Start();
}
} // namespace CompilerRuntime::TaskPoolModule
