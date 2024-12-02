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

#ifndef JS_CONCURRENT_MODULE_UTILS_CONDITION_NAPI_H
#define JS_CONCURRENT_MODULE_UTILS_CONDITION_NAPI_H

#include <deque>
#include "helper/napi_helper.h"

namespace Commonlibrary::Concurrent::Condition {
enum class SettleBy {
    WAITTIME = 1,
    NOTIFY = 2,
    CLEANCV = 3
};

struct PromiseInfo {
public:
    napi_env env = nullptr;
    napi_deferred deferred = nullptr;
    SettleBy resolved;
    uv_timer_t* timer = nullptr;
    napi_threadsafe_function tsfn_;

    bool operator==(const PromiseInfo& other) const
    {
        return deferred == other.deferred;
    }
    PromiseInfo(napi_env env, napi_deferred deferred, SettleBy resolved, uv_timer_t* timer,
        napi_threadsafe_function tsfn) : env(env), deferred(deferred), resolved(resolved), timer(timer), tsfn_(tsfn) {}
};

class ConditionVariable {
public:
    static napi_value Init(napi_env env, napi_value exports);
    static napi_value Constructor(napi_env env, napi_callback_info cbinfo);
    static napi_value Wait(napi_env env, napi_callback_info cbinfo);
    static napi_value WaitFor(napi_env env, napi_callback_info cbinfo);
    static napi_value Notify(napi_env env, napi_callback_info cbinfo);
    static napi_value NotifyOne(napi_env env, napi_callback_info cbinfo);
    static void Destructor(napi_env env, void *nativeObject, [[maybe_unused]] void *finalizeHint);
    ConditionVariable() : refCount_(0) {}
    explicit ConditionVariable(ConditionVariable *cond) :promiseQueue_(cond->promiseQueue_),
        refCount_(cond->GetRefCount()) {}
    int32_t GetRefCount();

private:

    static void ResolveDeferred(std::shared_ptr<PromiseInfo> promiseInfo, napi_threadsafe_function tsfn);
    static void CallJsCallback(napi_env env, napi_value js_callback, void* context, void* data);
    static void ScheduleAsyncWork(std::shared_ptr<PromiseInfo> promiseInfo, ConditionVariable* obj);
    void PromiseProcessFlow(ConditionVariable *cond, SettleBy settleBy, bool notifyAll);
    bool TimerTask(std::shared_ptr<PromiseInfo> pi, ConditionVariable *cond, uv_timer_t* timer,
        uint64_t milliseconds);
    napi_threadsafe_function CreateThreadSafeFunction(napi_env env);
    static void EnvCleanupHook(void* arg);
    void AddEnvCleanupHook(std::shared_ptr<PromiseInfo> pi);
    void RemoveEnvCleanupHook(std::shared_ptr<PromiseInfo> pi);
    void CleanupHookEnvPromise(std::shared_ptr<PromiseInfo> pi, ConditionVariable *cond);
    std::deque<std::shared_ptr<PromiseInfo>> promiseQueue_;  // 存储 Promise 的队列
    std::mutex queueMtx_;                // 保护队列的互斥锁
    uint32_t refCount_;
};

struct WorkerData {
    std::shared_ptr<PromiseInfo> pi;
    ConditionVariable* cond;
};

}  // namespace Commonlibrary::Concurrent::Condition

#endif // JS_CONCURRENT_MODULE_UTILS_CONDITION_NAPI_H