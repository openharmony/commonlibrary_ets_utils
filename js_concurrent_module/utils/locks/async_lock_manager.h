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

#ifndef JS_CONCURRENT_MODULE_UTILS_LOCKS_ASYNC_LOCK_MANAGER_H
#define JS_CONCURRENT_MODULE_UTILS_LOCKS_ASYNC_LOCK_MANAGER_H

#include <string>
#include <cstdint>
#include <unordered_map>

#include "common.h"
#include "async_lock.h"

namespace Commonlibrary::Concurrent::LocksModule {

struct AsyncLockIdentity {
    bool isAnonymous;
    uint32_t id;
    std::string name;
};

struct AsyncLockDependency {
    tid_t waiterTid;
    tid_t holderTid;
    std::string name;
    std::string creationStacktrace;
};

class AsyncLockManager {
public:
    static napi_value Init(napi_env env, napi_value exports);
    static napi_value Constructor(napi_env env, napi_callback_info cbinfo);
    static napi_value Request(napi_env env, napi_callback_info cbinfo);
    static void Destructor(napi_env env, void *nativeObject, void *finalize);

    static napi_value LockAsync(napi_env env, napi_callback_info cbinfo);
    static napi_value Query(napi_env env, napi_callback_info cbinfo);
    static napi_value QueryAll(napi_env env, napi_callback_info cbinfo);

    static tid_t GetCurrentTid();
    static void DumpLocksInfoForThread(tid_t targetTid, std::string &result);

    AsyncLockManager() = delete;
    AsyncLockManager(const AsyncLockManager &) = delete;
    AsyncLockManager &operator=(const AsyncLockManager &) = delete;
    AsyncLockManager(AsyncLockManager &&) = delete;
    AsyncLockManager &operator=(AsyncLockManager &&) = delete;
    ~AsyncLockManager() = delete;

private:
    static napi_value CreateLockStates(napi_env env, const std::function<bool(const AsyncLockIdentity& ident)> &pred);
    static napi_value CreateLockState(napi_env env, AsyncLock *asyncLock);
    static void Request(uint32_t id);
    static void Request(const std::string &name);
    static AsyncLock *FindAsyncLock(AsyncLockIdentity *id);
    static bool GetLockMode(napi_env env, napi_value val, LockMode &mode);
    static bool GetLockOptions(napi_env env, napi_value val, LockOptions &options);

    static void CollectLockDependencies(std::vector<AsyncLockDependency> &dependencies);
    static void CheckDeadlocksAndLogWarning();

    static std::mutex lockMutex;
    static std::unordered_map<std::string, std::shared_ptr<AsyncLock>> lockMap;
    static std::unordered_map<uint32_t, std::shared_ptr<AsyncLock>> anonymousLockMap;
    static std::atomic<uint32_t> nextId;
};

}  // namespace Commonlibrary::Concurrent::LocksModule

#endif
