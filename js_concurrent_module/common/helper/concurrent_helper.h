/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef JS_CONCURRENT_MODULE_COMMON_HELPER_CONCURRENT_HELPER_H
#define JS_CONCURRENT_MODULE_COMMON_HELPER_CONCURRENT_HELPER_H

#include <chrono>
#include <uv.h>
#if defined(OHOS_PLATFORM)
#include <unistd.h>
#elif defined(WINDOWS_PLATFORM)
#include <windows.h>
#elif defined(MAC_PLATFORM) || defined(IOS_PLATFORM)
#include <sys/sysctl.h>
#elif defined(ANDROID_PLATFORM)
#include <sys/sysinfo.h>
#endif

#if __GNUC__
#define LIKELY(x) __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define LIKELY(x) (!!(x))
#define UNLIKELY(x) (!!(x))
#endif // __GNUC__

namespace Commonlibrary::Concurrent::Common::Helper {
class ConcurrentHelper {
public:
    using UvCallback = void(*)(const uv_async_t*);

    ConcurrentHelper() = delete;
    ~ConcurrentHelper() = delete;

    static uint32_t GetMaxThreads()
    {
#if defined(OHOS_PLATFORM)
        return sysconf(_SC_NPROCESSORS_ONLN) - 1;
#elif defined(WINDOWS_PLATFORM)
        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        return sysInfo.dwNumberOfProcessors - 1;
#elif defined(MAC_PLATFORM) || defined(IOS_PLATFORM)
        int32_t numCpu = 0;
        size_t size = sizeof(numCpu);
        sysctlbyname("hw.ncpu", &numCpu, &size, nullptr, 0);
        return numCpu - 1;
#elif defined(ANDROID_PLATFORM)
        return get_nprocs() - 1;
#else
        return 1; // 1: default number
#endif
    }

    static uint64_t GetMilliseconds()
    {
        auto now = std::chrono::system_clock::now();
        auto millisecs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
        return millisecs.count();
    }

    static void UvHandleInit(uv_loop_t* loop, uv_async_t*& handle, UvCallback func, void* data = nullptr)
    {
        handle = new uv_async_t;
        handle->data = data;
        uv_async_init(loop, handle, reinterpret_cast<uv_async_cb>(func));
    }

    template<typename T>
    static void UvHandleClose(T* handle)
    {
        uv_close(reinterpret_cast<uv_handle_t*>(handle), [](uv_handle_t* handle) {
            if (handle != nullptr) {
                delete reinterpret_cast<T*>(handle);
                handle = nullptr;
            }
        });
    }
};
} // namespace Commonlibrary::Concurrent::Common::Helper
#endif // JS_CONCURRENT_MODULE_COMMON_HELPER_CONCURRENT_HELPER_H