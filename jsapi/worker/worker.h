/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_CCRUNTIME_JSAPI_WORKER_H
#define FOUNDATION_CCRUNTIME_JSAPI_WORKER_H

#include <list>
#include <map>
#include <mutex>
#include <uv.h>
#include "message_queue.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "native_engine/native_engine.h"
#include "utils/log.h"
#include "worker_helper.h"
#include "worker_runner.h"

namespace OHOS::CCRuntime::Worker {
class Worker;

class Worker {
public:
    static const int8_t WORKERPARAMNUM = 2;

    enum RunnerState { STARTING, RUNNING, TERMINATEING, TERMINATED };
    enum HostState { ACTIVE, INACTIVE };
    enum ListenerMode { ONCE, PERMANENT };

    enum ScriptMode { CLASSIC, MODULE };

    struct WorkerListener {
        WorkerListener() : callback_(nullptr), worker_(nullptr), mode_(PERMANENT) {}

        explicit WorkerListener(Worker* worker) : callback_(nullptr), worker_(worker), mode_(PERMANENT) {}

        WorkerListener(Worker* worker, ListenerMode mode) : callback_(nullptr), worker_(worker), mode_(mode) {}

        ~WorkerListener()
        {
            callback_ = nullptr;
            worker_ = nullptr;
        }

        bool NextIsAvailable() const
        {
            return mode_ != ONCE;
        }

        void SetCallable(napi_env env, napi_value value)
        {
            napi_create_reference(env, value, 1, &callback_);
        }

        void SetMode(ListenerMode mode)
        {
            mode_ = mode;
        }

        bool operator==(const WorkerListener& listener) const;

        napi_ref callback_ {NULL};
        Worker* worker_ {nullptr};
        ListenerMode mode_ {PERMANENT};
    };

    struct FindWorkerListener {
        FindWorkerListener(napi_env env, napi_ref ref) : env_(env), ref_(ref) {}

        bool operator()(const WorkerListener* listener) const
        {
            napi_value compareObj = nullptr;
            napi_get_reference_value(env_, listener->callback_, &compareObj);

            napi_value obj = nullptr;
            napi_get_reference_value(env_, ref_, &obj);
            bool isEqual = false;
            napi_strict_equals(env_, compareObj, obj, &isEqual);
            return isEqual;
        }

        napi_env env_ {nullptr};
        napi_ref ref_ {nullptr};
    };

    Worker(napi_env env, napi_ref thisVar);
    ~Worker();

    static void HostOnMessage(const uv_async_t* req);
    static void HostOnError(const uv_async_t* req);
    static void WorkerOnMessage(const uv_async_t* req);
    static void ExecuteInThread(const void* data);

    static napi_value PostMessage(napi_env env, napi_callback_info cbinfo);
    static napi_value PostMessageToHost(napi_env env, napi_callback_info cbinfo);
    static napi_value Terminate(napi_env env, napi_callback_info cbinfo);
    static napi_value CloseWorker(napi_env env, napi_callback_info cbinfo);
    static napi_value On(napi_env env, napi_callback_info cbinfo);
    static napi_value Once(napi_env env, napi_callback_info cbinfo);
    static napi_value Off(napi_env env, napi_callback_info cbinfo);
    static napi_value AddEventListener(napi_env env, napi_callback_info cbinfo);
    static napi_value DispatchEvent(napi_env env, napi_callback_info cbinfo);
    static napi_value RemoveEventListener(napi_env env, napi_callback_info cbinfo);
    static napi_value RemoveAllListener(napi_env env, napi_callback_info cbinfo);

    static napi_value AddListener(napi_env env, napi_callback_info cbinfo, ListenerMode mode);
    static napi_value RemoveListener(napi_env env, napi_callback_info cbinfo);

    static napi_value WorkerConstructor(napi_env env, napi_callback_info cbinfo);
    static napi_value InitWorker(napi_env env, napi_value exports);

    static napi_value CancelTask(napi_env env, napi_callback_info cbinfo);
    static napi_value ParentPortCancelTask(napi_env env, napi_callback_info cbinfo);

    static napi_value ParentPortAddEventListener(napi_env env, napi_callback_info cbinfo);
    static napi_value ParentPortRemoveAllListener(napi_env env, napi_callback_info cbinfo);
    static napi_value ParentPortDispatchEvent(napi_env env, napi_callback_info cbinfo);
    static napi_value ParentPortRemoveEventListener(napi_env env, napi_callback_info cbinfo);

    void StartExecuteInThread(napi_env env, const char* script);

    bool UpdateWorkerState(RunnerState state);
    bool UpdateHostState(HostState state);

    bool IsRunning() const
    {
        return runnerState_.load(std::memory_order_acquire) == RUNNING;
    }

    bool IsTerminated() const
    {
        return runnerState_.load(std::memory_order_acquire) >= TERMINATED;
    }

    bool IsTerminating() const
    {
        return runnerState_.load(std::memory_order_acquire) == TERMINATEING;
    }

    void SetScriptMode(ScriptMode mode)
    {
        scriptMode_ = mode;
    }

    void AddListenerInner(napi_env env, const char* type, const WorkerListener* listener);
    void RemoveListenerInner(napi_env env, const char* type, napi_ref callback);
    void RemoveAllListenerInner();

    uv_loop_t* GetWorkerLoop() const
    {
        if (workerEnv_ != nullptr) {
            return reinterpret_cast<NativeEngine*>(workerEnv_)->GetUVLoop();
        }
        return nullptr;
    }

    void SetWorkerEnv(napi_env workerEnv)
    {
        workerEnv_ = workerEnv;
    }

    std::string GetScript() const
    {
        return script_;
    }

    std::string GetName() const
    {
        return name_;
    }

    bool ClearWorkerTasks()
    {
        if (hostEnv_ != nullptr) {
            workerMessageQueue_.Clear(hostEnv_);
            return true;
        }
        return false;
    }

    bool HostIsStop() const
    {
        return hostState_.load(std::memory_order_acquire) == INACTIVE;
    }

    bool IsSameWorkerEnv(napi_env env) const
    {
        return workerEnv_ == env;
    }

    void Loop()
    {
        if (workerEnv_ != nullptr) {
            reinterpret_cast<NativeEngine*>(workerEnv_)->Loop(LOOP_DEFAULT);
        }
    }

private:
    void WorkerOnMessageInner();
    void HostOnMessageInner();
    void HostOnErrorInner();
    void HostOnMessageErrorInner();
    void WorkerOnMessageErrorInner();
    void WorkerOnErrorInner(napi_value error);

    void HandleException();
    bool CallWorkerFunction(size_t argc, const napi_value* argv, const char* methodName, bool tryCatch);
    void CallHostFunction(size_t argc, const napi_value* argv, const char* methodName) const;

    void HandleEventListeners(napi_env env, napi_value recv, size_t argc, const napi_value* argv, const char* type);
    void ParentPortHandleEventListeners(napi_env env, napi_value recv,
                                        size_t argc, const napi_value* argv, const char* type);
    void TerminateInner();

    void PostMessageInner(MessageDataType data);
    void PostMessageToHostInner(MessageDataType data);

    void TerminateWorker();
    void CloseInner();

    void PublishWorkerOverSignal();
    void CloseWorkerCallback();
    void CloseHostCallback() const;

    void ReleaseWorkerThreadContent();
    void ReleaseHostThreadContent();
    bool PrepareForWorkerInstance();
    void ParentPortAddListenerInner(napi_env env, const char* type, const WorkerListener* listener);
    void ParentPortRemoveAllListenerInner();
    void ParentPortRemoveListenerInner(napi_env env, const char* type, napi_ref callback);
    void PreparePandafile();

    napi_env GetHostEnv() const
    {
        return hostEnv_;
    }

    napi_env GetWorkerEnv() const
    {
        return workerEnv_;
    }

    std::string script_ {};
    std::string name_ {};
    ScriptMode scriptMode_ {CLASSIC};

    MessageQueue workerMessageQueue_ {};
    MessageQueue hostMessageQueue_ {};
    MessageQueue errorQueue_ {};

    uv_async_t workerOnMessageSignal_ {};
    uv_async_t hostOnMessageSignal_ {};
    uv_async_t hostOnErrorSignal_ {};

    std::atomic<RunnerState> runnerState_ {STARTING};
    std::atomic<HostState> hostState_ {ACTIVE};
    std::unique_ptr<WorkerRunner> runner_ {};

    napi_env hostEnv_ {nullptr};
    napi_env workerEnv_ {nullptr};

    napi_ref workerWrapper_ {nullptr};
    napi_ref parentPort_ {nullptr};

    std::map<std::string, std::list<WorkerListener*>> eventListeners_ {};
    std::map<std::string, std::list<WorkerListener*>> parentPortEventListeners_ {};

    std::recursive_mutex liveStatusLock_ {};
};
} // namespace OHOS::CCRuntime::Worker
#endif // FOUNDATION_CCRUNTIME_JSAPI_WORKER_H
