/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "async_stack_helper.h"
#include <dlfcn.h>
#include "tools/log.h"
namespace Commonlibrary::Concurrent::Common::Helper {

typedef uint64_t(*CollectAsyncStackFunc)(uint64_t, size_t);
thread_local static CollectAsyncStackFunc g_collectAsyncStackFunc = nullptr;
typedef void(*SetStackIdFunc)(uint64_t);
thread_local static SetStackIdFunc g_setStackIdFunc = nullptr;

void AsyncStackHelper::LoadDfxAsyncStackFunc()
{
    // libasync_stack.z.so has dlopen in appspawn
    g_collectAsyncStackFunc = (CollectAsyncStackFunc)(dlsym(RTLD_DEFAULT, "DfxCollectStackWithDepth"));
    g_setStackIdFunc = (SetStackIdFunc)(dlsym(RTLD_DEFAULT, "DfxSetSubmitterStackId"));
    if ((g_collectAsyncStackFunc == nullptr) || (g_setStackIdFunc == nullptr)) {
        HILOG_ERROR("DfxAsyncStackFunc failed.");
    }
}

uint64_t AsyncStackHelper::CollectAsyncStack(ConcurrentAsyncType asyncType)
{
    if (!g_collectAsyncStackFunc) {
        HILOG_ERROR("DfxCollectStackWithDepth is not loaded in taskpool/worker.");
        return 0U;
    }
    const uint64_t type = static_cast<uint64_t>(asyncType);
    const size_t depth = 16; // stack depth
    return g_collectAsyncStackFunc(type, depth);
}

void AsyncStackHelper::SetStackId(uint64_t id)
{
    if (!g_setStackIdFunc) {
        HILOG_ERROR("DfxSetSubmitterStackId is not loaded in taskpool/worker.");
        return;
    }
    g_setStackIdFunc(id);
}
} // Commonlibrary::Concurrent::Common::Helper
