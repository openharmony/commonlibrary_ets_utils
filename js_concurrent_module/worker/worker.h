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

#ifndef JS_CONCURRENT_MODULE_WORKER_WORKER_H
#define JS_CONCURRENT_MODULE_WORKER_WORKER_H

#include <condition_variable>
#include <list>
#include <map>
#include <mutex>

#include "helper/napi_helper.h"
#include "helper/object_helper.h"
#include "message_queue.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "native_engine/native_engine.h"
#include "worker_runner.h"

namespace Commonlibrary::Concurrent::WorkerModule {
using namespace Commonlibrary::Concurrent::Common::Helper;

class Worker {
public:
    enum RunnerState { STARTING, RUNNING, TERMINATEING, TERMINATED };
    enum HostState { ACTIVE, INACTIVE };
    enum ListenerMode { ONCE, PERMANENT };
    enum ScriptMode { CLASSIC, MODULE };

    using DebuggerPostTask = std::function<void()>;

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

    struct WorkerParams {
        std::string name_ {};
        ScriptMode type_ {CLASSIC};
    };

    /**
    * Creates a worker instance.
    *
    * @param env NAPI environment parameters.
    * @param thisVar URL of the script to be executed by the worker.
    */
    Worker(napi_env env, napi_ref thisVar);

    /**
        * The destructor of the Worker.
        */
    ~Worker();

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
    * Post a message, if has sendable objects in it pass sendable objects' reference.
    *
    * @param env NAPI environment parameters.
    * @param thisVar The callback information of the js layer.
    */
    static napi_value PostMessageWithSharedSendable(napi_env env, napi_callback_info cbinfo);

    /**
    * postMessage implementation
    *
    * @param env NAPI environment parameters.
    * @param thisVar The callback information of the js layer.
    */
    static napi_value CommonPostMessage(napi_env env, napi_callback_info cbinfo, bool cloneSendable);

    /**
     * Add event listeners to host.
     *
     * @param env NAPI environment parameters.
     * @param cbinfo The callback information of the js layer.
     */
    static napi_value PostMessageToHost(napi_env env, napi_callback_info cbinfo);

    /**
    * Post a message, if has sendable objects in it pass sendable objects' reference.
    *
    * @param env NAPI environment parameters.
    * @param thisVar The callback information of the js layer.
    */
    static napi_value PostMessageWithSharedSendableToHost(napi_env env, napi_callback_info cbinfo);

    /**
    * postMessage implementation
    *
    * @param env NAPI environment parameters.
    * @param thisVar The callback information of the js layer.
    */
    static napi_value CommonPostMessageToHost(napi_env env, napi_callback_info cbinfo, bool cloneSendable);

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
    static napi_value LimitedWorkerConstructor(napi_env env, napi_callback_info cbinfo);
    static napi_value ThreadWorkerConstructor(napi_env env, napi_callback_info cbinfo);
    static napi_value WorkerConstructor(napi_env env, napi_callback_info cbinfo);
    static napi_value Constructor(napi_env env, napi_callback_info cbinfo, bool limitSign = false,
                                  WorkerVersion version = WorkerVersion::NONE);

    /**
     * Initialize the worker and port.
     *
     * @param env NAPI environment parameters.
     * @param cbinfo The callback information of the js layer.
     */
    static napi_value InitWorker(napi_env env, napi_value exports);
    static napi_value InitPort(napi_env env, napi_value exports);

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
    static napi_value ParentPortCancelTask(napi_env env, napi_callback_info cbinfo);

    /**
     * The parent port adds an event listener.
     *
     * @param env NAPI environment parameters.
     * @param cbinfo The callback information of the js layer.
     */
    static napi_value ParentPortAddEventListener(napi_env env, napi_callback_info cbinfo);

    /**
     * The parent port removes all event listener.
     *
     * @param env NAPI environment parameters.
     * @param cbinfo The callback information of the js layer.
     */
    static napi_value ParentPortRemoveAllListener(napi_env env, napi_callback_info cbinfo);

    /**
     * The parent port dispatch the event listener.
     *
     * @param env NAPI environment parameters.
     * @param cbinfo The callback information of the js layer.
     */
    static napi_value ParentPortDispatchEvent(napi_env env, napi_callback_info cbinfo);

    /**
     * The parent port removes the event listener.
     *
     * @param env NAPI environment parameters.
     * @param cbinfo The callback information of the js layer.
     */
    static napi_value ParentPortRemoveEventListener(napi_env env, napi_callback_info cbinfo);

    /**
     * Register a globalCallObject on host side.
     *
     * @param env NAPI environment parameters.
     * @param cbinfo The callback information of the js layer.
     */
    static napi_value RegisterGlobalCallObject(napi_env env, napi_callback_info cbinfo);

    /**
     * Unregister the specific globalCallObject on host side.
     *
     * @param env NAPI environment parameters.
     * @param cbinfo The callback information of the js layer.
     */
    static napi_value UnregisterGlobalCallObject(napi_env env, napi_callback_info cbinfo);

    /**
     * Post a global synchronized call request to an object registered on host side.
     *
     * @param env NAPI environment parameters.
     * @param cbinfo The callback information of the js layer.
     */
    static napi_value GlobalCall(napi_env env, napi_callback_info cbinfo);

    static void HostOnGlobalCall(const uv_async_t* req);

    static bool CanCreateWorker(napi_env env, WorkerVersion target);

    static WorkerParams* CheckWorkerArgs(napi_env env, napi_value argsValue);

    static void WorkerThrowError(napi_env env, int32_t errCode, const char* errMessage = nullptr);

    void StartExecuteInThread(napi_env env, const char* script);

    bool UpdateWorkerState(RunnerState state);
    bool UpdateHostState(HostState state);

    bool IsNotTerminate() const
    {
        return runnerState_.load(std::memory_order_acquire) <= RUNNING;
    }

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
        if (workerEnvCallback_) {
            workerEnvCallback_(workerEnv_);
        }
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
            return;
        }
    }

    void RegisterCallbackForWorkerEnv(std::function<void (napi_env)> callback)
    {
        workerEnvCallback_ = callback;
        if (workerEnv_ != nullptr) {
            workerEnvCallback_(workerEnv_);
        }
    }

    napi_env GetWorkerEnv() const
    {
        return workerEnv_;
    }

    napi_env GetHostEnv() const
    {
        return hostEnv_;
    }

private:
    void WorkerOnMessageInner();
    void HostOnMessageInner();
    void HostOnErrorInner();
    void HostOnMessageErrorInner();
    void HostOnGlobalCallInner();
    void WorkerOnMessageErrorInner();
    void WorkerOnErrorInner(napi_value error);

    void HandleHostException() const;
    void HandleException();
    void HandleUncaughtException(napi_value exception);
    bool CallWorkerFunction(size_t argc, const napi_value* argv, const char* methodName, bool tryCatch);
    void CallHostFunction(size_t argc, const napi_value* argv, const char* methodName) const;

    bool HandleEventListeners(napi_env env, napi_value recv, size_t argc, const napi_value* argv, const char* type);
    void ParentPortHandleEventListeners(napi_env env, napi_value recv, size_t argc,
                                        const napi_value* argv, const char* type, bool tryCatch);
    void TerminateInner();

    void PostMessageInner(MessageDataType data);
    void PostMessageToHostInner(MessageDataType data);

    void TerminateWorker();
    void CloseInner();

    void PublishWorkerOverSignal();
    void CloseWorkerCallback();
    void CloseHostCallback();

    void ReleaseWorkerThreadContent();
    void ReleaseHostThreadContent();
    bool PrepareForWorkerInstance();
    void ParentPortAddListenerInner(napi_env env, const char* type, const WorkerListener* listener);
    void ParentPortRemoveAllListenerInner();
    void ParentPortRemoveListenerInner(napi_env env, const char* type, napi_ref callback);
    void GetContainerScopeId(napi_env env);

    void AddGlobalCallObject(const std::string &instanceName, napi_ref obj);
    bool RemoveGlobalCallObject(const std::string &instanceName);
    void ClearGlobalCallObject();
    void AddGlobalCallError(int32_t errCode, napi_value errData = nullptr);
    void HandleGlobalCallError(napi_env env);
    void ClearGlobalCallError(napi_env env);
    void InitGlobalCallStatus(napi_env env);
    void IncreaseGlobalCallId();

#if !defined(WINDOWS_PLATFORM) && !defined(MAC_PLATFORM)
    static void HandleDebuggerTask(const uv_async_t* req);
    void DebuggerOnPostTask(std::function<void()>&& task);
#endif

    std::string script_ {};
    std::string fileName_ {};
    std::string name_ {};
    ScriptMode scriptMode_ {CLASSIC};
    bool isLimitedWorker_ {false};
    bool isRelativePath_ {false};
    int32_t scopeId_ {-1};

    MessageQueue workerMessageQueue_ {};
    MessageQueue hostMessageQueue_ {};
    std::mutex globalCallMutex_;
    MarkedMessageQueue hostGlobalCallQueue_ {};
    MessageQueue workerGlobalCallQueue_ {};
    MessageQueue errorQueue_ {};

    uv_async_t* workerOnMessageSignal_ = nullptr;
    uv_async_t* hostOnMessageSignal_ = nullptr;
    uv_async_t* hostOnErrorSignal_ = nullptr;
    uv_async_t* hostOnGlobalCallSignal_ = nullptr;
#if !defined(WINDOWS_PLATFORM) && !defined(MAC_PLATFORM)
    uv_async_t debuggerOnPostTaskSignal_ {};
    std::mutex debuggerMutex_;
    std::queue<DebuggerPostTask> debuggerQueue_ {};
#endif

    std::atomic<RunnerState> runnerState_ {STARTING};
    std::atomic<HostState> hostState_ {ACTIVE};
    std::unique_ptr<WorkerRunner> runner_ {};

    std::atomic<bool> isErrorExit_ = false;

    napi_env hostEnv_ {nullptr};
    napi_env workerEnv_ {nullptr};

    napi_ref workerRef_ {nullptr};
    napi_ref workerPort_ {nullptr};

    std::map<std::string, std::list<WorkerListener*>> eventListeners_ {};
    std::map<std::string, std::list<WorkerListener*>> parentPortEventListeners_ {};
    std::unordered_map<std::string, napi_ref> globalCallObjects_ {};
    std::queue<std::pair<int32_t, napi_value>> globalCallErrors_ {};
    std::atomic<uint32_t> globalCallId_ = 1; // 0: reserved for error check

    std::recursive_mutex liveStatusLock_ {};
    std::mutex workerOnmessageMutex_ {};

    std::condition_variable cv_;
    std::atomic<bool> globalCallSuccess_ = true;
    std::function<void(napi_env)> workerEnvCallback_;
};
} // namespace Commonlibrary::Concurrent::WorkerModule
#endif // JS_CONCURRENT_MODULE_WORKER_WORKER_H
