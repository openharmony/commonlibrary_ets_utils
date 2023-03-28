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

#ifndef JS_CONCURRENT_MODULE_WORKER_WORKER_NEW_H_
#define JS_CONCURRENT_MODULE_WORKER_WORKER_NEW_H_

#include <list>
#include <map>
#include <mutex>
#include "message_queue.h"
#include "helper/napi_helper.h"
#include "helper/object_helper.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "native_engine/native_engine.h"
#include "utils/log.h"
#include "worker_runner.h"

namespace Commonlibrary::Concurrent::WorkerModule {
using namespace Commonlibrary::Concurrent::Common::Helper;

class NewWorker {
public:
    static const int8_t WORKERPARAMNUM = 2;
    static const int32_t TYPE_ERROR = 401;
    static const int32_t WORKERINITIALIZATION_ERROR = 10200003;
    static const int32_t WORKENOTRUNNING_ERROR = 10200004;
    static const int32_t WORKERUNSUPPORTED_ERROR = 10200005;
    static const int32_t WORKERSERIALIZATION_ERROR = 10200006;
    static const int32_t WORKERFILEPATH_ERROR = 10200007;

    enum RunnerState { STARTING, RUNNING, TERMINATEING, TERMINATED };
    enum HostState { ACTIVE, INACTIVE };
    enum ListenerMode { ONCE, PERMANENT };

    enum ScriptMode { CLASSIC, MODULE };

    struct WorkerListener {
        WorkerListener(napi_env env, napi_ref callback, ListenerMode mode)
            : env_(env), callback_(callback), mode_(mode)
        {}

        ~WorkerListener()
        {
            NapiHelper::DeleteReference(env_, callback_);
            callback_ = nullptr;
        }

        bool NextIsAvailable() const
        {
            return mode_ != ONCE;
        }

        void SetMode(ListenerMode mode)
        {
            mode_ = mode;
        }

        bool operator==(const WorkerListener& listener) const;

        napi_env env_ {NULL};
        napi_ref callback_ {NULL};
        ListenerMode mode_ {PERMANENT};
    };

    struct FindWorkerListener {
        FindWorkerListener(napi_env env, napi_ref ref) : env_(env), ref_(ref) {}

        bool operator()(const WorkerListener* listener) const
        {
            napi_value compareObj = NapiHelper::GetReferenceValue(env_, listener->callback_);
            napi_value obj = NapiHelper::GetReferenceValue(env_, ref_);
            // the env of listener and cmp listener must be same env because of Synchronization method
            return NapiHelper::StrictEqual(env_, compareObj, obj);
        }

        napi_env env_ {nullptr};
        napi_ref ref_ {nullptr};
    };

    /**
    * Creates a worker instance.
    *
    * @param env NAPI environment parameters.
    * @param thisVar URL of the script to be executed by the worker.
    */
    NewWorker(napi_env env, napi_ref thisVar);

    /**
        * The destructor of the Worker.
        */
    ~NewWorker();

    /**
     * The host thread receives the information.
     *
     * @param req The value of the object passed in by the js layer.
     */
    static void HostOnMessage(const uv_async_t* req);

    /**
     * The host thread receives the information.
     *
     * @param req The value of the object passed in by the js layer.
     */
    static void HostOnError(const uv_async_t* req);

    /**
     * The worker thread receives the information.
     *
     * @param req The value of the object passed in by the js layer.
     */
    static void WorkerOnMessage(const uv_async_t* req);

    /**
     * ExecuteIn in thread.
     *
     * @param data The worker pointer.
     */
    static void ExecuteInThread(const void* data);

    /**
    * Post a message.
    *
    * @param env NAPI environment parameters.
    * @param thisVar The callback information of the js layer.
    */
    static napi_value PostMessage(napi_env env, napi_callback_info cbinfo);

    /**
     * Add event listeners to host.
     *
     * @param env NAPI environment parameters.
     * @param cbinfo The callback information of the js layer.
     */
    static napi_value PostMessageToHost(napi_env env, napi_callback_info cbinfo);

    /**
     * Terminates the worker thread to stop the worker from receiving messages.
     *
     * @param env NAPI environment parameters.
     * @param cbinfo The callback information of the js layer.
     */
    static napi_value Terminate(napi_env env, napi_callback_info cbinfo);

    /**
     * Close the worker.
     *
     * @param env NAPI environment parameters.
     * @param cbinfo The callback information of the js layer.
     */
    static napi_value CloseWorker(napi_env env, napi_callback_info cbinfo);

    /**
     * Adds an event listener to the worker.
     *
     * @param env NAPI environment parameters.
     * @param cbinfo The callback information of the js layer.
     */
    static napi_value On(napi_env env, napi_callback_info cbinfo);

    /**
     * Adds an event listener to the worker and removes the event listener automatically after it is invoked once.
     *
     * @param env NAPI environment parameters.
     * @param cbinfo The callback information of the js layer.
     */
    static napi_value Once(napi_env env, napi_callback_info cbinfo);

    /**
     * Removes an event listener to the worker.
     *
     * @param env NAPI environment parameters.
     * @param cbinfo The callback information of the js layer.
     */
    static napi_value Off(napi_env env, napi_callback_info cbinfo);

    /**
     * Add event listeners.
     *
     * @param env NAPI environment parameters.
     * @param cbinfo The callback information of the js layer.
     */
    static napi_value AddEventListener(napi_env env, napi_callback_info cbinfo);

    /**
     * Dispatch the event.
     *
     * @param env NAPI environment parameters.
     * @param cbinfo The callback information of the js layer.
     */
    static napi_value DispatchEvent(napi_env env, napi_callback_info cbinfo);

    /**
     * Remove the event listener.
     *
     * @param env NAPI environment parameters.
     * @param cbinfo The callback information of the js layer.
     */
    static napi_value RemoveEventListener(napi_env env, napi_callback_info cbinfo);

    /**
     * Remove all listener.
     *
     * @param env NAPI environment parameters.
     * @param cbinfo The callback information of the js layer.
     */
    static napi_value RemoveAllListener(napi_env env, napi_callback_info cbinfo);

    /**
     * Add the listener.
     *
     * @param env NAPI environment parameters.
     * @param cbinfo The callback information of the js layer.
     */
    static napi_value AddListener(napi_env env, napi_callback_info cbinfo, ListenerMode mode);

    /**
     * Remove the listener.
     *
     * @param env NAPI environment parameters.
     * @param cbinfo The callback information of the js layer.
     */
    static napi_value RemoveListener(napi_env env, napi_callback_info cbinfo);

    /**
     * The constructor of worker.
     *
     * @param env NAPI environment parameters.
     * @param cbinfo The callback information of the js layer.
     */
    static napi_value WorkerConstructor(napi_env env, napi_callback_info cbinfo);

    /**
     * Initialize the worker.
     *
     * @param env NAPI environment parameters.
     * @param cbinfo The callback information of the js layer.
     */
    static napi_value InitWorker(napi_env env, napi_value exports);

    /**
     * Cancel the task.
     *
     * @param env NAPI environment parameters.
     * @param cbinfo The callback information of the js layer.
     */
    static napi_value CancelTask(napi_env env, napi_callback_info cbinfo);

    /**
     * The parent port cancels the task.
     *
     * @param env NAPI environment parameters.
     * @param cbinfo The callback information of the js layer.
     */
    static napi_value WorkerPortCancelTask(napi_env env, napi_callback_info cbinfo);

    /**
     * The parent port adds an event listener.
     *
     * @param env NAPI environment parameters.
     * @param cbinfo The callback information of the js layer.
     */
    static napi_value WorkerPortAddEventListener(napi_env env, napi_callback_info cbinfo);

    /**
     * The parent port removes all event listener.
     *
     * @param env NAPI environment parameters.
     * @param cbinfo The callback information of the js layer.
     */
    static napi_value WorkerPortRemoveAllListener(napi_env env, napi_callback_info cbinfo);

    /**
     * The parent port dispatch the event listener.
     *
     * @param env NAPI environment parameters.
     * @param cbinfo The callback information of the js layer.
     */
    static napi_value WorkerPortDispatchEvent(napi_env env, napi_callback_info cbinfo);

    /**
     * The parent port removes the event listener.
     *
     * @param env NAPI environment parameters.
     * @param cbinfo The callback information of the js layer.
     */
    static napi_value WorkerPortRemoveEventListener(napi_env env, napi_callback_info cbinfo);

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
            return NapiHelper::GetLibUV(workerEnv_);
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

    void Loop() const
    {
        uv_loop_t* loop = GetWorkerLoop();
        if (loop != nullptr) {
            uv_run(loop, UV_RUN_DEFAULT);
        } else {
            HILOG_ERROR("worker:: Worker loop is nullptr when start worker loop");
            return;
        }
    }

    static void TranslateErrorEvent(napi_env env, napi_value error, napi_value *obj);

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
    void GetContainerScopeId(napi_env env);

#if !defined(WINDOWS_PLATFORM) && !defined(MAC_PLATFORM)
    static void HandleDebuggerTask(const uv_async_t* req);
    void DebuggerOnPostTask(std::function<void()>&& task);
#endif

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
    int32_t scopeId_ {-1};

    MessageQueue workerMessageQueue_ {};
    MessageQueue hostMessageQueue_ {};
    MessageQueue errorQueue_ {};

    uv_async_t* workerOnMessageSignal_= nullptr;
    uv_async_t* hostOnMessageSignal_ = nullptr;
    uv_async_t* hostOnErrorSignal_ = nullptr;
#if !defined(WINDOWS_PLATFORM) && !defined(MAC_PLATFORM)
    uv_async_t ddebuggerOnPostTaskSignal_ {};
    std::function<void()> debuggerTask_;
#endif

    std::atomic<RunnerState> runnerState_ {STARTING};
    std::atomic<HostState> hostState_ {ACTIVE};
    std::unique_ptr<WorkerRunner> runner_ {};

    napi_env hostEnv_ {nullptr};
    napi_env workerEnv_ {nullptr};

    napi_ref workerRef_ {nullptr};
    napi_ref workerPort_ {nullptr};

    std::map<std::string, std::list<WorkerListener*>> eventListeners_ {};
    std::map<std::string, std::list<WorkerListener*>> parentPortEventListeners_ {};

    std::recursive_mutex liveStatusLock_ {};
};
} // namespace Commonlibrary::Concurrent::WorkerModule
#endif // JS_CONCURRENT_MODULE_WORKER_WORKER_NEW_H_
