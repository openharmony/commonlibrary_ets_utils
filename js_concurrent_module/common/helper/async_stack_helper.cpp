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

// libasync_stack.z.so has dlopen in appspawn, so dlsym directly
typedef uint64_t(*CollectAsyncStackFunc)(uint64_t, size_t);
thread_local static CollectAsyncStackFunc g_collectAsyncStackFunc =
    (CollectAsyncStackFunc)(dlsym(RTLD_DEFAULT, "DfxCollectStackWithDepth"));
typedef void(*SetStackIdFunc)(uint64_t);
thread_local static SetStackIdFunc g_setStackIdFunc =
    (SetStackIdFunc)(dlsym(RTLD_DEFAULT, "DfxSetSubmitterStackId"));
typedef uint64_t(*GetStackIdFunc)(void);
thread_local static GetStackIdFunc g_getStackIdFunc =
    (GetStackIdFunc)(dlsym(RTLD_DEFAULT, "DfxGetSubmitterStackId"));
typedef void(*ReleaseStackIdFunc)(uint64_t);
thread_local static ReleaseStackIdFunc g_releaseStackIdFunc =
    (ReleaseStackIdFunc)(dlsym(RTLD_DEFAULT, "ReleaseAsyncContext"));
typedef AsyncStackHelper::DfxAsyncMode(*GetAsyncStackModeFunc)(void);
thread_local static GetAsyncStackModeFunc g_getAsyncStackModeFunc =
    (GetAsyncStackModeFunc)(dlsym(RTLD_DEFAULT, "GetAsyncStackMode"));

void AsyncStackHelper::CheckLoadDfxAsyncStackFunc()
{
    if ((g_collectAsyncStackFunc == nullptr) || (g_setStackIdFunc == nullptr) ||
        (g_getStackIdFunc == nullptr) || (g_releaseStackIdFunc == nullptr)) {
        // GetAsyncStackMode is not support in libasync_stack.z.so, so check it later
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

uint64_t AsyncStackHelper::GetStackId()
{
    if (!g_getStackIdFunc) {
        HILOG_ERROR("DfxGetSubmitterStackId is not loaded in taskpool/worker.");
        return 0U;
    }
    return g_getStackIdFunc();
}

void AsyncStackHelper::ReleaseStackId(uint64_t id)
{
    if (!g_releaseStackIdFunc) {
        HILOG_ERROR("ReleaseAsyncContext is not loaded in taskpool/worker.");
        return;
    }
    g_releaseStackIdFunc(id);
}

AsyncStackHelper::DfxAsyncMode AsyncStackHelper::GetAsyncStackMode()
{
    if (!g_getAsyncStackModeFunc) {
        // GetAsyncStackMode is not support in libasync_stack.z.so, so debug level hilog
        HILOG_DEBUG("GetAsyncStackMode is not loaded in taskpool/worker.");
        return AsyncStackHelper::DfxAsyncMode::MODE_LAST_STACKTRACE;
    }
    return g_getAsyncStackModeFunc();
}
} // Commonlibrary::Concurrent::Common::Helper
