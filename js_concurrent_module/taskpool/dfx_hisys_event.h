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

#ifndef JS_CONCURRENT_MODULE_TASKPOOL_DFX_HISYS_EVENT_H
#define JS_CONCURRENT_MODULE_TASKPOOL_DFX_HISYS_EVENT_H

#include "hisysevent.h"

struct HisyseventParams {
    std::string methodName;
    std::string funName;
    std::string logType;
    std::string message;
    int32_t code;
    uint64_t waitTime;
    int32_t pid;
    int32_t tid;
};

namespace Commonlibrary::Concurrent::TaskPoolModule {

class DfxHisysEvent {
public:
    DfxHisysEvent() = default;
    ~DfxHisysEvent() = default;

    static int32_t WriteFfrtAndUv(HisyseventParams* hisyseventParams);
    
    static constexpr int32_t TASK_TIMEOUT = 10001; // execute task timeout
    static constexpr int32_t WORKER_ALIVE = 10002; // worker uv alive
};
} // namespace Commonlibrary::Concurrent::TaskPoolModule
#endif // JS_CONCURRENT_MODULE_TASKPOOL_DFX_HISYS_EVENT_H