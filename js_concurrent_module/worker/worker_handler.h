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

#ifndef JS_CONCURRENT_MODULE_WORKER_WORKERHANDLER_H
#define JS_CONCURRENT_MODULE_WORKER_WORKERHANDLER_H

#include <event_handler.h>
#include <uv.h>

namespace OHOS::AppExecFwk {
constexpr char TIMER_TASK[] = "uv_timer_task";

class WorkerLoopHandler : public FileDescriptorListener,
    public std::enable_shared_from_this<WorkerLoopHandler> {
public:
    explicit WorkerLoopHandler(uv_loop_t* uvLoop) : uvLoop_(uvLoop) {}
    void OnReadable(int32_t) override;
    void OnWritable(int32_t) override;

private:
    void OnTriggered();

private:
    uv_loop_t* uvLoop_ = nullptr;
    int64_t lastTimeStamp_ = 0;
    bool haveTimerTask_ = false;
};

class WorkerEventHandler : public EventHandler {
public:
    explicit WorkerEventHandler(const std::shared_ptr<EventRunner> &runner);
    ~WorkerEventHandler() = default;

    static std::shared_ptr<WorkerEventHandler> GetInstance();

    /**
     * Process the event. Developers should override this method.
     *
     * @param event The event should be processed.
     */
    void ProcessEvent(const InnerEvent::Pointer &event) override;
};
} // namespace OHOS::AppExecFwk::WorkerEventHandler
#endif // JS_CONCURRENT_MODULE_WORKER_WORKERHANDLER_H