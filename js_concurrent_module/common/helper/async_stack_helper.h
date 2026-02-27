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
#ifndef JS_CONCURRENT_MODULE_COMMON_HELPER_ASYNC_STACK_H
#define JS_CONCURRENT_MODULE_COMMON_HELPER_ASYNC_STACK_H
#include <iostream>

namespace Commonlibrary::Concurrent::Common::Helper {

class AsyncStackHelper {
public:
    typedef enum {
        ASYNC_TYPE_ARKTS_WORKER   = 1ULL << 24,
        ASYNC_TYPE_ARKTS_TASKPOOL = 1ULL << 25,
    } ConcurrentAsyncType;

    static void LoadDfxAsyncStackFunc();
    static uint64_t CollectAsyncStack(ConcurrentAsyncType asyncType);
    static void SetStackId(uint64_t id);
};

} // Commonlibrary::Concurrent::Common::Helper
#endif // JS_CONCURRENT_MODULE_COMMON_HELPER_ASYNC_STACK_H