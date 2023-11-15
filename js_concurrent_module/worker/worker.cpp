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

#include "worker.h"

#include "commonlibrary/ets_utils/js_sys_module/timer/timer.h"
#include "helper/concurrent_helper.h"
#include "helper/error_helper.h"
#include "helper/hitrace_helper.h"
#if defined(OHOS_PLATFORM)
#include "parameters.h"
#endif

namespace Commonlibrary::Concurrent::WorkerModule {
using namespace OHOS::JsSysModule;
static constexpr int8_t NUM_WORKER_ARGS = 2;
static constexpr uint8_t NUM_SYNC_CALL_ARGS = 3;
static std::list<Worker *> g_workers;
static constexpr int MAX_WORKERS = 8;
static std::mutex g_workersMutex;
static constexpr uint8_t BEGIN_INDEX_OF_ARGUMENTS = 2;
static constexpr uint32_t DEFAULT_WAITING_LIMITATION = 5000;
static constexpr uint32_t SYNC_CALL_ID_MAX = 4294967295;

Worker::Worker(napi_env env, napi_ref thisVar)
    : hostEnv_(env), workerRef_(thisVar)
{}

napi_value Worker::InitWorker(napi_env env, napi_value exports)
{
    HITRACE_HELPER_METER_NAME(__PRETTY_FUNCTION__);
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("postMessage", PostMessage),
        DECLARE_NAPI_FUNCTION("terminate", Terminate),
        DECLARE_NAPI_FUNCTION("on", On),
        DECLARE_NAPI_FUNCTION("onSyncCall", OnSyncCall),
        DECLARE_NAPI_FUNCTION("once", Once),
        DECLARE_NAPI_FUNCTION("off", Off),
        DECLARE_NAPI_FUNCTION("addEventListener", AddEventListener),
        DECLARE_NAPI_FUNCTION("dispatchEvent", DispatchEvent),
        DECLARE_NAPI_FUNCTION("removeEventListener", RemoveEventListener),
        DECLARE_NAPI_FUNCTION("removeAllListener", RemoveAllListener),
        DECLARE_NAPI_FUNCTION("cancelTasks", CancelTask),
    };
    // for worker.ThreadWorker
    const char threadWorkerName[] = "ThreadWorker";
    napi_value threadWorkerClazz = nullptr;
    napi_define_class(env, threadWorkerName, sizeof(threadWorkerName), Worker::ThreadWorkerConstructor, nullptr,
        sizeof(properties) / sizeof(properties[0]), properties, &threadWorkerClazz);
    napi_set_named_property(env, exports, "ThreadWorker", threadWorkerClazz);

    // for worker.Worker
    const char workerName[] = "Worker";
    napi_value workerClazz = nullptr;
    napi_define_class(env, workerName, sizeof(workerName), Worker::WorkerConstructor, nullptr,
        sizeof(properties) / sizeof(properties[0]), properties, &workerClazz);
    napi_set_named_property(env, exports, "Worker", workerClazz);

    // for worker.LimitedWorker
    const char limitedWorkerName[] = "RestrictedWorker";
    napi_value limitedWorkerClazz = nullptr;
    napi_define_class(env, limitedWorkerName, sizeof(limitedWorkerName), Worker::LimitedWorkerConstructor, nullptr,
        sizeof(properties) / sizeof(properties[0]), properties, &limitedWorkerClazz);
    napi_set_named_property(env, exports, "RestrictedWorker", limitedWorkerClazz);
    return InitPort(env, exports);
}

napi_value Worker::InitPort(napi_env env, napi_value exports)
{
    NativeEngine* engine = reinterpret_cast<NativeEngine*>(env);
    if (engine->IsWorkerThread()) {
        Worker* worker = nullptr;
        for (auto item = g_workers.begin(); item != g_workers.end(); item++) {
            if ((*item)->IsSameWorkerEnv(env)) {
                worker = *item;
            }
        }
        if (worker == nullptr) {
            ErrorHelper::ThrowError(env, ErrorHelper::ERR_WORKER_NOT_RUNNING, "worker is null when InitWorker");
            return exports;
        }

        napi_property_descriptor properties[] = {
            DECLARE_NAPI_FUNCTION_WITH_DATA("postMessage", PostMessageToHost, worker),
            DECLARE_NAPI_FUNCTION_WITH_DATA("syncCall", SyncCall, worker),
            DECLARE_NAPI_FUNCTION_WITH_DATA("close", CloseWorker, worker),
            DECLARE_NAPI_FUNCTION_WITH_DATA("cancelTasks", ParentPortCancelTask, worker),
            DECLARE_NAPI_FUNCTION_WITH_DATA("addEventListener", ParentPortAddEventListener, worker),
            DECLARE_NAPI_FUNCTION_WITH_DATA("dispatchEvent", ParentPortDispatchEvent, worker),
            DECLARE_NAPI_FUNCTION_WITH_DATA("removeEventListener", ParentPortRemoveEventListener, worker),
            DECLARE_NAPI_FUNCTION_WITH_DATA("removeAllListener", ParentPortRemoveAllListener, worker),
        };
        napi_value workerPortObj = NapiHelper::GetGlobalObject(env);
        napi_define_properties(env, workerPortObj, sizeof(properties) / sizeof(properties[0]), properties);

        // 5. register worker name in DedicatedWorkerGlobalScope
        std::string name = worker->GetName();
        if (!name.empty()) {
            napi_value nameValue = nullptr;
            napi_create_string_utf8(env, name.c_str(), name.length(), &nameValue);
            napi_set_named_property(env, workerPortObj, "name", nameValue);
        }

        napi_set_named_property(env, workerPortObj, "self", workerPortObj);

        while (!engine->IsMainThread()) {
            engine = engine->GetHostEngine();
        }
        if (engine->IsTargetWorkerVersion(WorkerVersion::NEW)) {
            napi_set_named_property(env, exports, "workerPort", workerPortObj);
        } else {
            napi_set_named_property(env, exports, "parentPort", workerPortObj);
        }
        // register worker Port.
        napi_create_reference(env, workerPortObj, 1, &worker->workerPort_);
    }
    return exports;
}

napi_value Worker::LimitedWorkerConstructor(napi_env env, napi_callback_info cbinfo)
{
    if (CanCreateWorker(env, WorkerVersion::NEW)) {
        return Constructor(env, cbinfo, true);
    }
    ErrorHelper::ThrowError(env, ErrorHelper::ERR_WORKER_INITIALIZATION,
        "Using both Worker and LimitedWorker is not supported.");
    return nullptr;
}

napi_value Worker::ThreadWorkerConstructor(napi_env env, napi_callback_info cbinfo)
{
    HITRACE_HELPER_METER_NAME("ThreadWorkerConstructor: [Add Thread]");
    if (CanCreateWorker(env, WorkerVersion::NEW)) {
        return Constructor(env, cbinfo);
    }
    ErrorHelper::ThrowError(env, ErrorHelper::ERR_WORKER_INITIALIZATION,
        "Using both Worker and ThreadWorker is not supported.");
    return nullptr;
}

napi_value Worker::WorkerConstructor(napi_env env, napi_callback_info cbinfo)
{
    HITRACE_HELPER_METER_NAME("WorkerConstructor: [Add Thread]");
    if (CanCreateWorker(env, WorkerVersion::OLD)) {
        return Constructor(env, cbinfo);
    }
    ErrorHelper::ThrowError(env, ErrorHelper::ERR_WORKER_INITIALIZATION,
        "Using both Worker and other Workers is not supported.");
    return nullptr;
}

napi_value Worker::Constructor(napi_env env, napi_callback_info cbinfo, bool limitSign)
{
    napi_value thisVar = nullptr;
    void* data = nullptr;
    size_t argc = 2;  // 2: max args number is 2
    napi_value args[argc];
    napi_get_cb_info(env, cbinfo, &argc, args, &thisVar, &data);
    // check argv count
    if (argc < 1) {
        ErrorHelper::ThrowError(env,
            ErrorHelper::TYPE_ERROR, "the number of create worker param must be more than 1 with new");
        return nullptr;
    }
    // check 1st param is string
    if (!NapiHelper::IsString(env, args[0])) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "the type of Worker 1st param must be string.");
        return nullptr;
    }
    WorkerParams* workerParams = nullptr;
    if (argc == 2) {  // 2: max args number is 2
        workerParams = CheckWorkerArgs(env, args[1]);
        if (workerParams == nullptr) {
            HILOG_ERROR("Worker:: arguments check failed.");
            return nullptr;
        }
    }

    Worker* worker = nullptr;
    {
        int maxWorkers = MAX_WORKERS;
    #if defined(OHOS_PLATFORM)
        maxWorkers = OHOS::system::GetIntParameter<int>("persist.commonlibrary.maxworkers", MAX_WORKERS);
    #endif
        std::lock_guard<std::mutex> lock(g_workersMutex);
        if (static_cast<int>(g_workers.size()) >= maxWorkers) {
            ErrorHelper::ThrowError(env,
                ErrorHelper::ERR_WORKER_INITIALIZATION, "the number of workers exceeds the maximum.");
            return nullptr;
        }

        // 2. new worker instance
        worker = new Worker(env, nullptr);
        if (worker == nullptr) {
            ErrorHelper::ThrowError(env, ErrorHelper::ERR_WORKER_INITIALIZATION, "creat worker error");
            return nullptr;
        }
        g_workers.push_back(worker);
    }

    if (workerParams != nullptr) {
        if (!workerParams->name_.empty()) {
            worker->name_ = workerParams->name_;
        }
        // default classic
        worker->SetScriptMode(workerParams->type_);
        CloseHelp::DeletePointer(workerParams, false);
        workerParams = nullptr;
    }
    worker->isLimitedWorker_ = limitSign;

    // 3. execute in thread
    char* script = NapiHelper::GetString(env, args[0]);
    if (script == nullptr) {
        ErrorHelper::ThrowError(env,
            ErrorHelper::ERR_WORKER_INVALID_FILEPATH, "the file path is invaild, maybe path is null.");
        return nullptr;
    }
    napi_wrap(
        env, thisVar, worker,
        [](napi_env env, void* data, void* hint) {
            Worker* worker = reinterpret_cast<Worker*>(data);
            {
                std::lock_guard<std::recursive_mutex> lock(worker->liveStatusLock_);
                if (worker->UpdateHostState(INACTIVE)) {
                    if (worker->hostOnMessageSignal_ != nullptr &&
                        !uv_is_closing(reinterpret_cast<uv_handle_t*>(worker->hostOnMessageSignal_))) {
                        uv_close(reinterpret_cast<uv_handle_t*>(worker->hostOnMessageSignal_), [](uv_handle_t* handle) {
                            if (handle != nullptr) {
                                delete reinterpret_cast<uv_async_t*>(handle);
                                handle = nullptr;
                            }
                        });
                    }
                    if (worker->hostOnErrorSignal_ != nullptr &&
                        !uv_is_closing(reinterpret_cast<uv_handle_t*>(worker->hostOnErrorSignal_))) {
                        uv_close(reinterpret_cast<uv_handle_t*>(worker->hostOnErrorSignal_), [](uv_handle_t* handle) {
                            if (handle != nullptr) {
                                delete reinterpret_cast<uv_async_t*>(handle);
                                handle = nullptr;
                            }
                        });
                    }
                    if (worker->hostOnSyncCallSignal_ != nullptr &&
                        !uv_is_closing(reinterpret_cast<uv_handle_t*>(worker->hostOnSyncCallSignal_))) {
                        uv_close(reinterpret_cast<uv_handle_t*>(worker->hostOnSyncCallSignal_),
                            [](uv_handle_t* handle) {
                            if (handle != nullptr) {
                                delete reinterpret_cast<uv_async_t*>(handle);
                                handle = nullptr;
                            }
                        });
                    }
                    worker->ReleaseHostThreadContent();
                }
                if (!worker->IsRunning()) {
                    HILOG_DEBUG("worker:: worker is not in running");
                    return;
                }
                worker->TerminateInner();
            }
        },
        nullptr, &worker->workerRef_);
    worker->StartExecuteInThread(env, script);
    return thisVar;
}

Worker::WorkerParams* Worker::CheckWorkerArgs(napi_env env, napi_value argsValue)
{
    WorkerParams* workerParams = nullptr;
    if (NapiHelper::IsObject(env, argsValue)) {
        workerParams = new WorkerParams();
        napi_value nameValue = NapiHelper::GetNameProperty(env, argsValue, "name");
        if (NapiHelper::IsNotUndefined(env, nameValue)) {
            if (!NapiHelper::IsString(env, nameValue)) {
                CloseHelp::DeletePointer(workerParams, false);
                WorkerThrowError(env, ErrorHelper::TYPE_ERROR, "the type of name in worker must be string.");
                return nullptr;
            }
            char* nameStr = NapiHelper::GetString(env, nameValue);
            if (nameStr == nullptr) {
                CloseHelp::DeletePointer(workerParams, false);
                ErrorHelper::ThrowError(env, ErrorHelper::ERR_WORKER_INITIALIZATION, "the name of worker is null.");
                return nullptr;
            }
            workerParams->name_ = std::string(nameStr);
            CloseHelp::DeletePointer(nameStr, true);
        }
        napi_value typeValue = NapiHelper::GetNameProperty(env, argsValue, "type");
        if (NapiHelper::IsNotUndefined(env, typeValue)) {
            if (!NapiHelper::IsString(env, typeValue)) {
                CloseHelp::DeletePointer(workerParams, false);
                WorkerThrowError(env, ErrorHelper::TYPE_ERROR, "the type of type must be string.");
                return nullptr;
            }
            char* typeStr = NapiHelper::GetString(env, typeValue);
            if (typeStr == nullptr) {
                CloseHelp::DeletePointer(workerParams, false);
                ErrorHelper::ThrowError(env, ErrorHelper::ERR_WORKER_INITIALIZATION, "the type of worker is null.");
                return nullptr;
            }
            if (strcmp("classic", typeStr) == 0) {
                workerParams->type_ = CLASSIC;
                CloseHelp::DeletePointer(typeStr, true);
            } else {
                CloseHelp::DeletePointer(workerParams, false);
                CloseHelp::DeletePointer(typeStr, true);
                ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
                                        "the type must be classic, unsupport others now.");
                return nullptr;
            }
        }
    }
    return workerParams;
}

napi_value Worker::PostMessage(napi_env env, napi_callback_info cbinfo)
{
    HITRACE_HELPER_METER_NAME(__PRETTY_FUNCTION__);
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    if (argc < 1) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
            "Worker messageObject must be not null with postMessage");
        return nullptr;
    }
    napi_value* argv = new napi_value[argc];
    ObjectScope<napi_value> scope(argv, true);
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, cbinfo, &argc, argv, &thisVar, nullptr);
    Worker* worker = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void**>(&worker));

    if (worker == nullptr || worker->IsTerminated() || worker->IsTerminating()) {
        HILOG_ERROR("worker:: worker is nullptr when PostMessage, maybe worker is terminated");
        WorkerThrowError(env, ErrorHelper::ERR_WORKER_NOT_RUNNING, "maybe worker is terminated when PostMessage");
        return nullptr;
    }

    napi_value data = nullptr;
    napi_status serializeStatus = napi_ok;
    if (argc >= NUM_WORKER_ARGS) {
        if (!NapiHelper::IsArray(env, argv[1])) {
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "transfer list must be an Array");
            return nullptr;
        }
        serializeStatus = napi_serialize(env, argv[0], argv[1], &data);
    } else {
        serializeStatus = napi_serialize(env, argv[0], NapiHelper::GetUndefinedValue(env), &data);
    }
    if (serializeStatus != napi_ok || data == nullptr) {
        worker->HostOnMessageErrorInner();
        WorkerThrowError(env, ErrorHelper::ERR_WORKER_SERIALIZATION, "failed to serialize message.");
        return nullptr;
    }
    worker->PostMessageInner(data);
    return NapiHelper::GetUndefinedValue(env);
}

napi_value Worker::Terminate(napi_env env, napi_callback_info cbinfo)
{
    HITRACE_HELPER_METER_NAME(__PRETTY_FUNCTION__);
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, cbinfo, nullptr, nullptr, &thisVar, nullptr);
    Worker* worker = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void**>(&worker));
    if (worker == nullptr) {
        HILOG_ERROR("worker:: worker is nullptr when Terminate, maybe worker is terminated");
        WorkerThrowError(env, ErrorHelper::ERR_WORKER_NOT_RUNNING, "worker is nullptr when Terminate");
        return nullptr;
    }
    if (worker->IsTerminated() || worker->IsTerminating()) {
        HILOG_DEBUG("worker:: worker is not in running when Terminate");
        return nullptr;
    }
    worker->TerminateInner();
    return NapiHelper::GetUndefinedValue(env);
}

napi_value Worker::On(napi_env env, napi_callback_info cbinfo)
{
    return AddListener(env, cbinfo, PERMANENT);
}

napi_value Worker::Once(napi_env env, napi_callback_info cbinfo)
{
    return AddListener(env, cbinfo, ONCE);
}

napi_value Worker::OnSyncCall(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    if (argc != NUM_WORKER_ARGS) {
        ErrorHelper::ThrowError(env,
            ErrorHelper::TYPE_ERROR, "worker OnSyncCall param count must be 2");
        return nullptr;
    }
    // check 1st param is string
    napi_value thisVar = nullptr;
    void* data = nullptr;
    napi_value* args = new napi_value[argc];
    ObjectScope<napi_value> scope(args, true);
    napi_get_cb_info(env, cbinfo, &argc, args, &thisVar, &data);
    if (!NapiHelper::IsString(env, args[0])) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
            "BusinessError 401: Parameter error. The type of 'eventName' must be string");
        return nullptr;
    }
    std::string eventName = NapiHelper::GetString(env, args[0]);

    Worker* worker = nullptr;
    napi_unwrap(env, thisVar, (void**)&worker);
    if (worker == nullptr) {
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_WORKER_NOT_RUNNING, "maybe worker is terminated");
        return nullptr;
    }
    napi_ref listener = NapiHelper::CreateReference(env, args[1], 1);
    worker->AddSyncEventListener(eventName, listener);
    return nullptr;
}

napi_value Worker::Off(napi_env env, napi_callback_info cbinfo)
{
    return RemoveListener(env, cbinfo);
}

napi_value Worker::RemoveEventListener(napi_env env, napi_callback_info cbinfo)
{
    return RemoveListener(env, cbinfo);
}

napi_value Worker::AddEventListener(napi_env env, napi_callback_info cbinfo)
{
    return AddListener(env, cbinfo, PERMANENT);
}

napi_value Worker::AddListener(napi_env env, napi_callback_info cbinfo, ListenerMode mode)
{
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    if (argc < NUM_WORKER_ARGS) {
        ErrorHelper::ThrowError(env,
            ErrorHelper::TYPE_ERROR, "worker add listener param count must be not less than 2.");
        return nullptr;
    }
    // check 1st param is string
    napi_value thisVar = nullptr;
    void* data = nullptr;
    napi_value* args = new napi_value[argc];
    ObjectScope<napi_value> scope(args, true);
    napi_get_cb_info(env, cbinfo, &argc, args, &thisVar, &data);
    if (!NapiHelper::IsString(env, args[0])) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "Worker add listener 1st param must be string");
        return nullptr;
    }
    if (!NapiHelper::IsCallable(env, args[1])) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "Worker add listener 2st param must be callable");
        return nullptr;
    }
    Worker* worker = nullptr;
    napi_unwrap(env, thisVar, (void**)&worker);
    if (worker == nullptr) {
        HILOG_ERROR("worker:: worker is nullptr when addListener, maybe worker is terminated");
        WorkerThrowError(env, ErrorHelper::ERR_WORKER_NOT_RUNNING, "maybe worker is terminated");
        return nullptr;
    }

    napi_ref callback = NapiHelper::CreateReference(env, args[1], 1);
    auto listener = new WorkerListener(env, callback, mode);
    if (mode == ONCE && argc > NUM_WORKER_ARGS) {
        if (NapiHelper::IsObject(env, args[NUM_WORKER_ARGS])) {
            napi_value onceValue = NapiHelper::GetNameProperty(env, args[NUM_WORKER_ARGS], "once");
            bool isOnce = NapiHelper::GetBooleanValue(env, onceValue);
            if (!isOnce) {
                listener->SetMode(PERMANENT);
            }
        }
    }
    char* typeStr = NapiHelper::GetString(env, args[0]);
    worker->AddListenerInner(env, typeStr, listener);
    CloseHelp::DeletePointer(typeStr, true);
    return NapiHelper::GetUndefinedValue(env);
}

napi_value Worker::RemoveListener(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    if (argc < 1) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "the remove listener param must be not less than 1");
        return nullptr;
    }
    // check 1st param is string
    napi_value thisVar = nullptr;
    void* data = nullptr;
    napi_value* args = new napi_value[argc];
    ObjectScope<napi_value> scope(args, true);
    napi_get_cb_info(env, cbinfo, &argc, args, &thisVar, &data);
    if (!NapiHelper::IsString(env, args[0])) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "the type of remove listener 1st param must be string");
        return nullptr;
    }

    Worker* worker = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void**>(&worker));
    if (worker == nullptr) {
        HILOG_ERROR("worker:: worker is nullptr when RemoveListener, maybe worker is terminated");
        WorkerThrowError(env, ErrorHelper::ERR_WORKER_NOT_RUNNING, "maybe worker is terminated");
        return nullptr;
    }

    if (argc > 1 && !NapiHelper::IsCallable(env, args[1])) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "the type of remove listener 2st param must be callable");
        return nullptr;
    }

    char* typeStr = NapiHelper::GetString(env, args[0]);
    if (typeStr == nullptr) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "the type of remove listener type must be not null");
        return nullptr;
    }

    napi_ref callback = nullptr;
    if (argc > 1 && NapiHelper::IsCallable(env, args[1])) {
        napi_create_reference(env, args[1], 1, &callback);
    }
    worker->RemoveListenerInner(env, typeStr, callback);
    CloseHelp::DeletePointer(typeStr, true);
    NapiHelper::DeleteReference(env, callback);
    return NapiHelper::GetUndefinedValue(env);
}

void CallWorkCallback(napi_env env, napi_value recv, size_t argc, const napi_value* argv, const char* type)
{
    napi_value callback = nullptr;
    napi_get_named_property(env, recv, type, &callback);
    if (NapiHelper::IsCallable(env, callback)) {
        napi_value callbackResult = nullptr;
        napi_call_function(env, NapiHelper::GetGlobalObject(env), callback, argc, argv, &callbackResult);
    }
}

napi_value Worker::DispatchEvent(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = 1;
    napi_value args[1];
    napi_value thisVar = nullptr;
    void* data = nullptr;
    napi_get_cb_info(env, cbinfo, &argc, args, &thisVar, &data);
    if (argc < 1) {
        ErrorHelper::ThrowError(env,
            ErrorHelper::TYPE_ERROR, "the count of event param must be more than 1 in DispatchEvent");
        return NapiHelper::CreateBooleanValue(env, false);
    }

    // check 1st param is event
    if (!NapiHelper::IsObject(env, args[0])) {
        ErrorHelper::ThrowError(env,
            ErrorHelper::TYPE_ERROR, "the type of event 1st param must be Event in DispatchEvent");
        return NapiHelper::CreateBooleanValue(env, false);
    }

    Worker* worker = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void**>(&worker));
    if (worker == nullptr) {
        HILOG_ERROR("worker:: worker is nullptr when DispatchEvent, maybe worker is terminated");
        WorkerThrowError(env, ErrorHelper::ERR_WORKER_NOT_RUNNING, "maybe worker has been terminated");
        return NapiHelper::CreateBooleanValue(env, false);
    }

    napi_value typeValue = NapiHelper::GetNameProperty(env, args[0], "type");
    if (!NapiHelper::IsString(env, typeValue)) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "the type of event type must be string");
        return NapiHelper::CreateBooleanValue(env, false);
    }

    napi_value obj = NapiHelper::GetReferenceValue(env, worker->workerRef_);

    char* typeStr = NapiHelper::GetString(env, typeValue);
    if (typeStr == nullptr) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "dispatchEvent event type must be not null");
        return NapiHelper::CreateBooleanValue(env, false);
    }
    if (strcmp(typeStr, "error") == 0) {
        CallWorkCallback(env, obj, 1, args, "onerror");
    } else if (strcmp(typeStr, "messageerror") == 0) {
        CallWorkCallback(env, obj, 1, args, "onmessageerror");
    } else if (strcmp(typeStr, "message") == 0) {
        CallWorkCallback(env, obj, 1, args, "onmessage");
    }

    worker->HandleEventListeners(env, obj, 1, args, typeStr);

    CloseHelp::DeletePointer(typeStr, true);
    return NapiHelper::CreateBooleanValue(env, true);
}

napi_value Worker::RemoveAllListener(napi_env env, napi_callback_info cbinfo)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, cbinfo, nullptr, nullptr, &thisVar, nullptr);
    Worker* worker = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void**>(&worker));
    if (worker == nullptr) {
        HILOG_ERROR("worker:: worker is nullptr when RemoveAllListener, maybe worker is terminated");
        WorkerThrowError(env, ErrorHelper::ERR_WORKER_NOT_RUNNING, "maybe worker is terminated");
        return nullptr;
    }

    worker->RemoveAllListenerInner();
    return NapiHelper::GetUndefinedValue(env);
}

napi_value Worker::CancelTask(napi_env env, napi_callback_info cbinfo)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, cbinfo, nullptr, nullptr, &thisVar, nullptr);
    Worker* worker = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void**>(&worker));
    if (worker == nullptr) {
        HILOG_ERROR("worker:: worker is nullptr when CancelTask, maybe worker is terminated");
        return nullptr;
    }

    if (worker->IsTerminated() || worker->IsTerminating()) {
        HILOG_INFO("worker:: worker is not in running");
        return nullptr;
    }

    if (!worker->ClearWorkerTasks()) {
        HILOG_ERROR("worker:: clear worker task error");
    }
    return NapiHelper::GetUndefinedValue(env);
}

napi_value Worker::PostMessageToHost(napi_env env, napi_callback_info cbinfo)
{
    HITRACE_HELPER_METER_NAME(__PRETTY_FUNCTION__);
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    if (argc < 1) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "Worker param count must be more than 1 with new");
        return nullptr;
    }
    napi_value* argv = new napi_value[argc];
    ObjectScope<napi_value> scope(argv, true);
    Worker* worker = nullptr;
    napi_get_cb_info(env, cbinfo, &argc, argv, nullptr, reinterpret_cast<void**>(&worker));

    if (worker == nullptr) {
        HILOG_ERROR("worker:: when post message to host occur worker is nullptr");
        WorkerThrowError(env, ErrorHelper::ERR_WORKER_NOT_RUNNING, "worker is nullptr when post message to host");
        return nullptr;
    }

    if (!worker->IsRunning()) {
        // if worker is not running, don't send any message to host thread
        HILOG_DEBUG("worker:: when post message to host occur worker is not in running.");
        return nullptr;
    }

    napi_value data = nullptr;
    napi_status serializeStatus = napi_ok;
    if (argc >= NUM_WORKER_ARGS) {
        if (!NapiHelper::IsArray(env, argv[1])) {
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "Transfer list must be an Array");
            return nullptr;
        }
        serializeStatus = napi_serialize(env, argv[0], argv[1], &data);
    } else {
        serializeStatus = napi_serialize(env, argv[0], NapiHelper::GetUndefinedValue(env), &data);
    }

    if (serializeStatus != napi_ok || data == nullptr) {
        worker->WorkerOnMessageErrorInner();
        WorkerThrowError(env, ErrorHelper::ERR_WORKER_SERIALIZATION, "failed to serialize message.");
        return nullptr;
    }
    worker->PostMessageToHostInner(data);
    return NapiHelper::GetUndefinedValue(env);
}

napi_value Worker::SyncCall(napi_env env, napi_callback_info cbinfo)
{
    HITRACE_HELPER_METER_NAME(__PRETTY_FUNCTION__);
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    if (argc < NUM_SYNC_CALL_ARGS) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "syncCall param must be equal or more than 3");
        return nullptr;
    }
    napi_value* args = new napi_value[argc];
    ObjectScope<napi_value> scope(args, true);
    Worker* worker = nullptr;
    napi_get_cb_info(env, cbinfo, &argc, args, nullptr, reinterpret_cast<void**>(&worker));
    if (worker == nullptr) {
        HILOG_ERROR("worker:: worker is null when syncCall to host");
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_WORKER_NOT_RUNNING, "worker is null when syncCall to host");
        return nullptr;
    }

    if (!worker->IsRunning()) {
        // if worker is not running, don't send any message to host thread
        HILOG_DEBUG("worker:: when post message to host occur worker is not in running.");
        return nullptr;
    }

    if (!NapiHelper::IsString(env, args[0])) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
            "BusinessError 401: Parameter error. The type of 'eventName' must be string");
        return nullptr;
    }
    if (!NapiHelper::IsString(env, args[1])) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
            "BusinessError 401: Parameter error. The type of 'methodName' must be string");
        return nullptr;
    }
    if (!NapiHelper::IsNumber(env, args[2])) { // 2: the index of argument "timeLimitation"
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR,
            "BusinessError 401: Parameter error. The type of 'timeLimitation' must be number");
        return nullptr;
    }
    napi_value value = nullptr;
    value = worker->SyncFunc(env, args, argc, worker);
    return value;
}

napi_value Worker::SyncFunc(napi_env env, napi_value* args, size_t argc, Worker* worker)
{
    napi_status serializeStatus = napi_ok;
    napi_value data = nullptr;
    napi_value argsArray;
    napi_create_array_with_length(env, argc - 1, &argsArray);
    size_t index = 0;
    uint32_t timeLimitation = 0;
    for (size_t i = 0; i < argc; i++) {
        if (i == 2) { // 2: index of time limitation arg
            timeLimitation = NapiHelper::GetUint32Value(env, args[i]);
            continue;
        }
        napi_set_element(env, argsArray, index, args[i]);
        index++;
    }
    if (timeLimitation <= 0 || timeLimitation > DEFAULT_WAITING_LIMITATION) {
        timeLimitation = DEFAULT_WAITING_LIMITATION;
    }

    // defautly not transfer
    serializeStatus = napi_serialize(env, argsArray, NapiHelper::GetUndefinedValue(env), &data);
    if (serializeStatus != napi_ok || data == nullptr) {
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_WORKER_SERIALIZATION, "failed to serialize message.");
        return nullptr;
    }
    worker->hostSyncEventQueue_.Push(worker->syncCallId_, data);

    std::lock_guard<std::recursive_mutex> lock(worker->liveStatusLock_);
    if (env != nullptr && !worker->HostIsStop()) {
        worker->InitSyncCallStatus(env);
        uv_async_send(worker->hostOnSyncCallSignal_);
    } else {
        HILOG_ERROR("worker:: worker host engine is nullptr when SyncCall.");
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_WORKER_NOT_RUNNING, "worker is null");
        return nullptr;
    }

    {
        std::unique_lock lock(worker->syncMessageMutex_);
        if (!worker->cv_.wait_for(lock, std::chrono::milliseconds(timeLimitation), [worker]() {
            return !worker->workerSyncEventQueue_.IsEmpty() || !worker->syncEventSuccess_;
        })) {
            worker->IncreaseSyncCallId();
            HILOG_ERROR("worker:: syncCall has exceeded the waiting time limitation, skip this turn.");
            ErrorHelper::ThrowError(env, ErrorHelper::ERR_EXCEED_WAITING_LIMITATION,
                std::to_string(timeLimitation).c_str());
            return nullptr;
        }
    }
    worker->IncreaseSyncCallId();
    if (!worker->syncEventSuccess_) {
        worker->HandleSyncCallError(env);
        return nullptr;
    }
    if (!worker->workerSyncEventQueue_.DeQueue(&data)) {
        HILOG_ERROR("worker:: message returned from host is empty when SyncCall");
        return nullptr;
    }
    napi_value res = nullptr;
    serializeStatus = napi_deserialize(env, data, &res);
    if (serializeStatus != napi_ok || res == nullptr) {
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_WORKER_SERIALIZATION, "failed to serialize message.");
        return nullptr;
    }
    return res;
}


void Worker::InitSyncCallStatus(napi_env env)
{
    // worker side event data queue shall be empty before uv_async_send
    workerSyncEventQueue_.Clear(env);
    ClearSyncCallError(env);
    syncEventSuccess_ = true;
}

void Worker::IncreaseSyncCallId()
{
    if (UNLIKELY(syncCallId_ == SYNC_CALL_ID_MAX)) {
        syncCallId_ = 1;
    } else {
        syncCallId_++;
    }
}

napi_value Worker::CloseWorker(napi_env env, napi_callback_info cbinfo)
{
    HITRACE_HELPER_METER_NAME(__PRETTY_FUNCTION__);
    Worker* worker = nullptr;
    napi_get_cb_info(env, cbinfo, nullptr, nullptr, nullptr, (void**)&worker);
    if (worker != nullptr) {
        worker->CloseInner();
    } else {
        WorkerThrowError(env, ErrorHelper::ERR_WORKER_NOT_RUNNING, "worker is null");
        return nullptr;
    }
    return NapiHelper::GetUndefinedValue(env);
}

napi_value Worker::ParentPortCancelTask(napi_env env, napi_callback_info cbinfo)
{
    Worker* worker = nullptr;
    napi_get_cb_info(env, cbinfo, nullptr, nullptr, nullptr, reinterpret_cast<void**>(&worker));
    if (worker == nullptr) {
        HILOG_ERROR("worker:: worker is nullptr when CancelTask, maybe worker is terminated");
        return nullptr;
    }

    if (worker->IsTerminated() || worker->IsTerminating()) {
        HILOG_INFO("worker:: worker is not in running");
        return nullptr;
    }

    if (!worker->ClearWorkerTasks()) {
        HILOG_ERROR("worker:: clear worker task error");
    }
    return NapiHelper::GetUndefinedValue(env);
}

napi_value Worker::ParentPortAddEventListener(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    if (argc < NUM_WORKER_ARGS) {
        ErrorHelper::ThrowError(env,
            ErrorHelper::TYPE_ERROR, "worker listener param count must be more than WORKPARAMNUM.");
        return nullptr;
    }

    napi_value* args = new napi_value[argc];
    ObjectScope<napi_value> scope(args, true);
    Worker* worker = nullptr;
    napi_get_cb_info(env, cbinfo, &argc, args, nullptr, reinterpret_cast<void**>(&worker));

    if (!NapiHelper::IsString(env, args[0])) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "the type of worker listener 1st param must be string.");
        return nullptr;
    }

    if (!NapiHelper::IsCallable(env, args[1])) {
        ErrorHelper::ThrowError(env,
            ErrorHelper::TYPE_ERROR, "the type of worker listener 2st param must be callable.");
        return nullptr;
    }

    if (worker == nullptr || !worker->IsNotTerminate()) {
        HILOG_ERROR("worker:: when post message to host occur worker is nullptr");
        WorkerThrowError(env, ErrorHelper::ERR_WORKER_NOT_RUNNING, "worker is not running.");
        return nullptr;
    }

    napi_ref callback = NapiHelper::CreateReference(env, args[1], 1);
    auto listener = new WorkerListener(env, callback, PERMANENT);
    if (argc > NUM_WORKER_ARGS && NapiHelper::IsObject(env, args[NUM_WORKER_ARGS])) {
        napi_value onceValue = NapiHelper::GetNameProperty(env, args[NUM_WORKER_ARGS], "once");
        bool isOnce = NapiHelper::GetBooleanValue(env, onceValue);
        if (isOnce) {
            listener->SetMode(ONCE);
        }
    }
    char* typeStr = NapiHelper::GetString(env, args[0]);
    worker->ParentPortAddListenerInner(env, typeStr, listener);
    CloseHelp::DeletePointer(typeStr, true);
    return NapiHelper::GetUndefinedValue(env);
}

napi_value Worker::ParentPortDispatchEvent(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = 1;
    napi_value args[1];
    Worker* worker = nullptr;
    napi_get_cb_info(env, cbinfo, &argc, args, nullptr, reinterpret_cast<void**>(&worker));
    if (argc < 1) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "DispatchEvent param count must be more than 1.");
        return NapiHelper::CreateBooleanValue(env, false);
    }

    if (!NapiHelper::IsObject(env, args[0])) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "worker DispatchEvent 1st param must be Event.");
        return NapiHelper::CreateBooleanValue(env, false);
    }

    napi_value typeValue = NapiHelper::GetNameProperty(env, args[0], "type");
    if (!NapiHelper::IsString(env, typeValue)) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "worker event type must be string.");
        return NapiHelper::CreateBooleanValue(env, false);
    }

    if (worker == nullptr || !worker->IsNotTerminate()) {
        HILOG_ERROR("worker:: when post message to host occur worker is nullptr");
        WorkerThrowError(env, ErrorHelper::ERR_WORKER_NOT_RUNNING, "worker is nullptr.");
        return NapiHelper::CreateBooleanValue(env, false);
    }

    char* typeStr = NapiHelper::GetString(env, typeValue);
    if (typeStr == nullptr) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "worker listener type must be not null.");
        return NapiHelper::CreateBooleanValue(env, false);
    }

    napi_value obj = NapiHelper::GetReferenceValue(env, worker->workerPort_);

    if (strcmp(typeStr, "error") == 0) {
        CallWorkCallback(env, obj, 1, args, "onerror");
    } else if (strcmp(typeStr, "messageerror") == 0) {
        CallWorkCallback(env, obj, 1, args, "onmessageerror");
    } else if (strcmp(typeStr, "message") == 0) {
        CallWorkCallback(env, obj, 1, args, "onmessage");
    }

    worker->ParentPortHandleEventListeners(env, obj, 1, args, typeStr, true);

    CloseHelp::DeletePointer(typeStr, true);
    return NapiHelper::CreateBooleanValue(env, true);
}

napi_value Worker::ParentPortRemoveEventListener(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    if (argc < 1) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "worker listener param count must be more than 2.");
        return nullptr;
    }

    napi_value* args = new napi_value[argc];
    ObjectScope<napi_value> scope(args, true);
    Worker* worker = nullptr;
    napi_get_cb_info(env, cbinfo, &argc, args, nullptr, reinterpret_cast<void**>(&worker));

    if (!NapiHelper::IsString(env, args[0])) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "the type of worker listener 1st param must be string.");
        return nullptr;
    }

    if (argc > 1 && !NapiHelper::IsCallable(env, args[1])) {
        ErrorHelper::ThrowError(env,
            ErrorHelper::TYPE_ERROR, "the type of worker listener 2st param must be callable with on.");
        return nullptr;
    }

    if (worker == nullptr || !worker->IsNotTerminate()) {
        HILOG_ERROR("worker:: when post message to host occur worker is nullptr");
        WorkerThrowError(env, ErrorHelper::ERR_WORKER_NOT_RUNNING, "worker is not running.");
        return nullptr;
    }

    napi_ref callback = nullptr;
    if (argc > 1 && NapiHelper::IsCallable(env, args[1])) {
        napi_create_reference(env, args[1], 1, &callback);
    }

    char* typeStr = NapiHelper::GetString(env, args[0]);
    if (typeStr == nullptr) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "worker listener type must be not null.");
        return nullptr;
    }
    worker->ParentPortRemoveListenerInner(env, typeStr, callback);
    CloseHelp::DeletePointer(typeStr, true);
    NapiHelper::DeleteReference(env, callback);
    return NapiHelper::GetUndefinedValue(env);
}

napi_value Worker::ParentPortRemoveAllListener(napi_env env, napi_callback_info cbinfo)
{
    Worker* worker = nullptr;
    napi_get_cb_info(env, cbinfo, nullptr, nullptr, nullptr, reinterpret_cast<void**>(&worker));

    if (worker == nullptr || !worker->IsNotTerminate()) {
        HILOG_ERROR("worker:: when post message to host occur worker is nullptr");
        WorkerThrowError(env, ErrorHelper::ERR_WORKER_NOT_RUNNING,
            "worker is nullptr when ParentPortRemoveAllListener");
        return nullptr;
    }

    worker->ParentPortRemoveAllListenerInner();
    return NapiHelper::GetUndefinedValue(env);
}

void Worker::GetContainerScopeId(napi_env env)
{
    NativeEngine* hostEngine = reinterpret_cast<NativeEngine*>(env);
    scopeId_ = hostEngine->GetContainerScopeIdFunc();
}

void Worker::AddSyncEventListener(const std::string &eventName, napi_ref listener)
{
    syncEventListeners_.insert_or_assign(eventName, listener);
}

void Worker::ClearSyncEventListener()
{
    for (auto iter = syncEventListeners_.begin(); iter != syncEventListeners_.end(); iter++) {
        napi_ref objRef = iter->second;
        NapiHelper::DeleteReference(hostEnv_, objRef);
    }
    syncEventListeners_.clear();
}

void Worker::StartExecuteInThread(napi_env env, const char* script)
{
    // 1. init hostOnMessageSignal_ in host loop
    uv_loop_t* loop = NapiHelper::GetLibUV(env);
    if (loop == nullptr) {
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_WORKER_NOT_RUNNING, "engine loop is null");
        CloseHelp::DeletePointer(script, true);
        return;
    }
    GetContainerScopeId(env);
    hostOnMessageSignal_ = new uv_async_t;
    uv_async_init(loop, hostOnMessageSignal_, reinterpret_cast<uv_async_cb>(Worker::HostOnMessage));
    hostOnMessageSignal_->data = this;
    hostOnErrorSignal_ = new uv_async_t;
    uv_async_init(loop, hostOnErrorSignal_, reinterpret_cast<uv_async_cb>(Worker::HostOnError));
    hostOnErrorSignal_->data = this;
    hostOnSyncCallSignal_ = new uv_async_t;
    uv_async_init(loop, hostOnSyncCallSignal_, reinterpret_cast<uv_async_cb>(Worker::HostOnSyncCall));
    hostOnSyncCallSignal_->data = this;

    // 2. copy the script
    script_ = std::string(script);
    CloseHelp::DeletePointer(script, true);

    // 3. create WorkerRunner to Execute
    if (!runner_) {
        runner_ = std::make_unique<WorkerRunner>(WorkerStartCallback(ExecuteInThread, this));
    }
    if (runner_) {
        runner_->Execute(); // start a new thread
    } else {
        HILOG_ERROR("runner_ is nullptr");
    }
}

void Worker::ExecuteInThread(const void* data)
{
    HITRACE_HELPER_START_TRACE(__PRETTY_FUNCTION__);
    auto worker = reinterpret_cast<Worker*>(const_cast<void*>(data));
    // 1. create a runtime, nativeengine
    napi_env workerEnv = nullptr;
    {
        std::lock_guard<std::recursive_mutex> lock(worker->liveStatusLock_);
        if (worker->HostIsStop()) {
            CloseHelp::DeletePointer(worker, false);
            return;
        }
        napi_env env = worker->GetHostEnv();
        if (worker->isLimitedWorker_) {
            napi_create_limit_runtime(env, &workerEnv);
        } else {
            napi_create_runtime(env, &workerEnv);
        }
        if (workerEnv == nullptr) {
            ErrorHelper::ThrowError(env, ErrorHelper::ERR_WORKER_NOT_RUNNING, "Worker create runtime error");
            return;
        }
        // mark worker env is workerThread
        reinterpret_cast<NativeEngine*>(workerEnv)->MarkWorkerThread();
        // for load balance in taskpool
        reinterpret_cast<NativeEngine*>(env)->IncreaseSubEnvCounter();

        worker->SetWorkerEnv(workerEnv);
    }

    uv_loop_t* loop = worker->GetWorkerLoop();
    if (loop == nullptr) {
        HILOG_ERROR("worker:: Worker loop is nullptr");
        return;
    }

    // 2. add some preparation for the worker
    if (worker->PrepareForWorkerInstance()) {
        worker->workerOnMessageSignal_ = new uv_async_t;
        uv_async_init(loop, worker->workerOnMessageSignal_, reinterpret_cast<uv_async_cb>(Worker::WorkerOnMessage));
        worker->workerOnMessageSignal_->data = worker;
#if !defined(WINDOWS_PLATFORM) && !defined(MAC_PLATFORM)
        uv_async_init(loop, &worker->ddebuggerOnPostTaskSignal_, reinterpret_cast<uv_async_cb>(
            Worker::HandleDebuggerTask));
#endif
        worker->UpdateWorkerState(RUNNING);
        // in order to invoke worker send before subThread start
        uv_async_send(worker->workerOnMessageSignal_);
        HITRACE_HELPER_FINISH_TRACE;
        // 3. start worker loop
        worker->Loop();
    } else {
        HILOG_ERROR("worker:: worker PrepareForWorkerInstance fail");
        worker->UpdateWorkerState(TERMINATED);
        HITRACE_HELPER_FINISH_TRACE;
    }
    worker->ReleaseWorkerThreadContent();
    std::lock_guard<std::recursive_mutex> lock(worker->liveStatusLock_);
    if (worker->HostIsStop()) {
        CloseHelp::DeletePointer(worker, false);
    } else {
        worker->PublishWorkerOverSignal();
    }
}

bool Worker::PrepareForWorkerInstance()
{
    std::vector<uint8_t> scriptContent;
    std::string workerAmi;
    {
        std::lock_guard<std::recursive_mutex> lock(liveStatusLock_);
        if (HostIsStop()) {
            return false;
        }
        // 1. init worker async func
        auto workerEngine = reinterpret_cast<NativeEngine*>(workerEnv_);

        auto hostEngine = reinterpret_cast<NativeEngine*>(hostEnv_);
        // 2. init worker environment
#if !defined(WINDOWS_PLATFORM) && !defined(MAC_PLATFORM)
        workerEngine->SetDebuggerPostTaskFunc(
            std::bind(&Worker::DebuggerOnPostTask, this, std::placeholders::_1));
#endif
        if (!hostEngine->CallInitWorkerFunc(workerEngine)) {
            HILOG_ERROR("worker:: CallInitWorkerFunc error");
            return false;
        }
        // 3. get uril content
        if (!hostEngine->CallGetAssetFunc(script_, scriptContent, workerAmi)) {
            HILOG_ERROR("worker:: CallGetAssetFunc error");
            return false;
        }
    }
    // add timer interface
    Timer::RegisterTime(workerEnv_);
    HILOG_DEBUG("worker:: stringContent size is %{public}zu", scriptContent.size());
    napi_value execScriptResult = nullptr;
    napi_run_actor(workerEnv_, scriptContent, workerAmi.c_str(), &execScriptResult);
    if (execScriptResult == nullptr) {
        // An exception occurred when running the script.
        HILOG_ERROR("worker:: run script exception occurs, will handle exception");
        HandleException();
        return false;
    }

    // 4. register worker name in DedicatedWorkerGlobalScope
    if (!name_.empty()) {
        napi_value nameValue = nullptr;
        napi_create_string_utf8(workerEnv_, name_.c_str(), name_.length(), &nameValue);
        NapiHelper::SetNamePropertyInGlobal(workerEnv_, "name", nameValue);
    }
    return true;
}

void Worker::HostOnMessage(const uv_async_t* req)
{
    HITRACE_HELPER_METER_NAME(__PRETTY_FUNCTION__);
    Worker* worker = static_cast<Worker*>(req->data);
    if (worker == nullptr) {
        HILOG_ERROR("worker:: worker is null when host onmessage.");
        return;
    }
    worker->HostOnMessageInner();
}

void Worker::HostOnMessageInner()
{
    if (hostEnv_ == nullptr || HostIsStop()) {
        HILOG_ERROR("worker:: host thread maybe is over when host onmessage.");
        return;
    }

    NativeEngine* engine = reinterpret_cast<NativeEngine*>(hostEnv_);
    if (!engine->InitContainerScopeFunc(scopeId_)) {
        HILOG_WARN("worker:: InitContainerScopeFunc error when HostOnMessageInner begin(only stage model)");
    }

    napi_value obj = NapiHelper::GetReferenceValue(hostEnv_, workerRef_);
    napi_value callback = NapiHelper::GetNameProperty(hostEnv_, obj, "onmessage");
    bool isCallable = NapiHelper::IsCallable(hostEnv_, callback);

    MessageDataType data = nullptr;
    while (hostMessageQueue_.DeQueue(&data)) {
        // receive close signal.
        if (data == nullptr) {
            HILOG_DEBUG("worker:: worker received close signal");
            uv_close(reinterpret_cast<uv_handle_t*>(hostOnMessageSignal_), [](uv_handle_t* handle) {
                if (handle != nullptr) {
                    delete reinterpret_cast<uv_async_t*>(handle);
                    handle = nullptr;
                }
            });
            uv_close(reinterpret_cast<uv_handle_t*>(hostOnErrorSignal_), [](uv_handle_t* handle) {
                if (handle != nullptr) {
                    delete reinterpret_cast<uv_async_t*>(handle);
                    handle = nullptr;
                }
            });
            CloseHostCallback();
            return;
        }
        // handle data, call worker onMessage function to handle.
        napi_status status = napi_ok;
        HandleScope scope(hostEnv_, status);
        NAPI_CALL_RETURN_VOID(hostEnv_, status);
        napi_value result = nullptr;
        status = napi_deserialize(hostEnv_, data, &result);
        if (status != napi_ok || result == nullptr) {
            HostOnMessageErrorInner();
            continue;
        }
        napi_value event = nullptr;
        napi_create_object(hostEnv_, &event);
        napi_set_named_property(hostEnv_, event, "data", result);
        napi_value argv[1] = { event };
        if (isCallable) {
            napi_value callbackResult = nullptr;
            napi_call_function(hostEnv_, NapiHelper::GetGlobalObject(hostEnv_), callback, 1, argv, &callbackResult);
        }
        // handle listeners.
        HandleEventListeners(hostEnv_, obj, 1, argv, "message");
        HandleHostException();
    }
    if (!engine->FinishContainerScopeFunc(scopeId_)) {
        HILOG_WARN("worker:: FinishContainerScopeFunc error when HostOnMessageInner end(only stage model)");
    }
}

void Worker::HostOnSyncCall(const uv_async_t* req)
{
    HITRACE_HELPER_METER_NAME(__PRETTY_FUNCTION__);
    Worker* worker = static_cast<Worker*>(req->data);
    if (worker == nullptr) {
        HILOG_ERROR("worker:: worker is null");
        return;
    }
    worker->HostOnSyncCallInner();
}

void Worker::HostOnSyncCallInner()
{
    if (hostEnv_ == nullptr || HostIsStop()) {
        HILOG_ERROR("worker:: host thread maybe is over when host onmessage.");
        syncEventSuccess_ = false;
        cv_.notify_one();
        return;
    }

    NativeEngine* engine = reinterpret_cast<NativeEngine*>(hostEnv_);
    if (!engine->InitContainerScopeFunc(scopeId_)) {
        HILOG_WARN("worker:: InitContainerScopeFunc error when HostOnMessageInner begin(only stage model)");
    }

    if (hostSyncEventQueue_.IsEmpty()) {
        HILOG_ERROR("worker:: message queue is empty when hostOnSyncCall");
        syncEventSuccess_ = false;
        cv_.notify_one();
        return;
    }
    MessageDataType data = nullptr;
    uint32_t currentCallId = 0;
    size_t size = hostSyncEventQueue_.GetSize();
    for (size_t i = 0; i < size; i++) {
        std::pair<uint32_t, MessageDataType> pair = hostSyncEventQueue_.Front();
        hostSyncEventQueue_.Pop();
        if (pair.first == syncCallId_) {
            currentCallId = pair.first;
            data = pair.second;
            break;
        }
        napi_delete_serialization_data(hostEnv_, pair.second);
    }
    napi_value argsArray = nullptr;
    napi_status status = napi_ok;
    status = napi_deserialize(hostEnv_, data, &argsArray);
    if (status != napi_ok || argsArray == nullptr) {
        AddSyncCallError(ErrorHelper::ERR_WORKER_SERIALIZATION);
        syncEventSuccess_ = false;
        cv_.notify_one();
        return;
    }
    napi_value eventName = nullptr;
    napi_get_element(hostEnv_, argsArray, 0, &eventName);
    napi_value methodName = nullptr;
    napi_get_element(hostEnv_, argsArray, 1, &methodName);

    std::string eventNameStr = NapiHelper::GetString(hostEnv_, eventName);
    auto iter = syncEventListeners_.find(eventNameStr);
    if (iter == syncEventListeners_.end()) {
        HILOG_ERROR("worker:: there is no listener for sync event: %{public}s", eventNameStr.c_str());
        AddSyncCallError(ErrorHelper::ERR_TRIGGER_NONEXIST_EVENT);
        syncEventSuccess_ = false;
        cv_.notify_one();
        return;
    }
    napi_ref objRef = iter->second;
    napi_value obj = NapiHelper::GetReferenceValue(hostEnv_, objRef);
    Func(data, methodName, obj, argsArray, currentCallId);
}

void Worker::Func(MessageDataType data, napi_value methodName, napi_value obj,
                  napi_value argsArray, uint32_t currentCallId)
{
    napi_status status = napi_ok;
    bool hasProperty = false;
    napi_has_property(hostEnv_, obj, methodName, &hasProperty);
    if (!hasProperty) {
        const char* methodNameStr = NapiHelper::GetString(hostEnv_, methodName);
        HILOG_ERROR("worker:: binding obj for sync event has no method: %{public}s", methodNameStr);
        AddSyncCallError(ErrorHelper::ERR_CALL_METHOD_ON_BINDING_OBJ);
        syncEventSuccess_ = false;
        cv_.notify_one();
        return;
    }
    napi_value method = nullptr;
    napi_get_property(hostEnv_, obj, methodName, &method);
    if (!NapiHelper::IsCallable(hostEnv_, method)) {
        const char* methodNameStr = NapiHelper::GetString(hostEnv_, methodName);
        HILOG_ERROR("worker:: method %{public}s is not callable", methodNameStr);
        AddSyncCallError(ErrorHelper::ERR_CALL_METHOD_ON_BINDING_OBJ);
        syncEventSuccess_ = false;
        cv_.notify_one();
        return;
    }
    uint32_t argc = 0;
    napi_get_array_length(hostEnv_, argsArray, &argc);
    napi_value* args = nullptr;
    ObjectScope<napi_value> scope(args, true);
    if (argc > BEGIN_INDEX_OF_ARGUMENTS) {
        args = new napi_value[argc - BEGIN_INDEX_OF_ARGUMENTS];
        for (uint32_t index = 0; index < argc - BEGIN_INDEX_OF_ARGUMENTS; index++) {
            napi_get_element(hostEnv_, argsArray, index + BEGIN_INDEX_OF_ARGUMENTS, &args[index]);
        }
    }

    napi_value res = nullptr;
    napi_call_function(
        hostEnv_, NapiHelper::GetGlobalObject(hostEnv_), method, argc - BEGIN_INDEX_OF_ARGUMENTS, args, &res);
    bool hasPendingException = NapiHelper::IsExceptionPending(hostEnv_);
    if (hasPendingException) {
        napi_value exception = nullptr;
        napi_get_and_clear_last_exception(hostEnv_, &exception);
        data = nullptr;
        status = napi_serialize(hostEnv_, exception, NapiHelper::GetUndefinedValue(hostEnv_), &data);
        if (status != napi_ok || data == nullptr) {
            ErrorHelper::ThrowError(hostEnv_, ErrorHelper::ERR_WORKER_SERIALIZATION);
            if (args != nullptr) {
                delete[] args;
            }
            return;
        }
        AddSyncCallError(ErrorHelper::ERR_DURING_SYNC_CALL, data);
        syncEventSuccess_ = false;
        cv_.notify_one();
        if (args != nullptr) {
            delete[] args;
        }
        return;
    }
    // defautly not transfer
    status = napi_serialize(hostEnv_, res, NapiHelper::GetUndefinedValue(hostEnv_), &data);
    if (status != napi_ok || data == nullptr) {
        AddSyncCallError(ErrorHelper::ERR_WORKER_SERIALIZATION);
        syncEventSuccess_ = false;
        cv_.notify_one();
        if (args != nullptr) {
            delete[] args;
        }
        return;
    }
    // drop and destruct result if timeout
    if (currentCallId != syncCallId_ || currentCallId == 0) {
        napi_delete_serialization_data(hostEnv_, data);
        cv_.notify_one();
        if (args != nullptr) {
            delete[] args;
        }
        return;
    }
    workerSyncEventQueue_.EnQueue(data);
    syncEventSuccess_ = true;
    cv_.notify_one();
    if (args != nullptr) {
        delete[] args;
    }
}

void Worker::AddSyncCallError(int32_t errCode, napi_value errData)
{
    syncEventErrors_.push({errCode, errData});
}

void Worker::HandleSyncCallError(napi_env env)
{
    while (!syncEventErrors_.empty()) {
        std::pair<int32_t, napi_value> pair = syncEventErrors_.front();
        syncEventErrors_.pop();
        int32_t errCode = pair.first;
        if (errCode == ErrorHelper::ERR_DURING_SYNC_CALL) {
            napi_status status = napi_ok;
            napi_value exception = nullptr;
            status = napi_deserialize(env, pair.second, &exception);
            if (status != napi_ok || exception == nullptr) {
                ErrorHelper::ThrowError(env, ErrorHelper::ERR_WORKER_SERIALIZATION);
                return;
            }
            napi_throw(env, exception);
        } else {
            ErrorHelper::ThrowError(env, errCode);
        }
    }
}

void Worker::ClearSyncCallError(napi_env env)
{
    while (!syncEventErrors_.empty()) {
        std::pair<int32_t, napi_value> pair = syncEventErrors_.front();
        syncEventErrors_.pop();
        if (pair.second != nullptr) {
            napi_delete_serialization_data(env, pair.second);
        }
    }
}

void Worker::CallHostFunction(size_t argc, const napi_value* argv, const char* methodName) const
{
    if (hostEnv_ == nullptr) {
        HILOG_ERROR("worker:: host thread maybe is over");
        return;
    }
    if (HostIsStop()) {
        HILOG_ERROR("worker:: host thread maybe is over");
        WorkerThrowError(hostEnv_, ErrorHelper::ERR_WORKER_NOT_RUNNING,
            "host thread maybe is over when CallHostFunction");
        return;
    }
    napi_value obj = NapiHelper::GetReferenceValue(hostEnv_, workerRef_);
    napi_value callback = NapiHelper::GetNameProperty(hostEnv_, obj, methodName);
    bool isCallable = NapiHelper::IsCallable(hostEnv_, callback);
    if (!isCallable) {
        HILOG_DEBUG("worker:: host thread %{public}s is not Callable", methodName);
        return;
    }
    napi_value callbackResult = nullptr;
    napi_call_function(hostEnv_, NapiHelper::GetGlobalObject(hostEnv_), callback, argc, argv, &callbackResult);
    HandleHostException();
}

void Worker::CloseHostCallback()
{
    {
        napi_status status = napi_ok;
        HandleScope scope(hostEnv_, status);
        NAPI_CALL_RETURN_VOID(hostEnv_, status);
        napi_value exitValue = nullptr;
        if (isErrorExit_) {
            napi_create_int32(hostEnv_, 1, &exitValue); // 1 : exit because of error
        } else {
            napi_create_int32(hostEnv_, 0, &exitValue); // 0 : exit normally
        }
        napi_value argv[1] = { exitValue };
        CallHostFunction(1, argv, "onexit");
        napi_value obj = NapiHelper::GetReferenceValue(hostEnv_, workerRef_);
        // handle listeners
        HandleEventListeners(hostEnv_, obj, 1, argv, "exit");
    }
    CloseHelp::DeletePointer(this, false);
}

void Worker::HostOnError(const uv_async_t* req)
{
    Worker* worker = static_cast<Worker*>(req->data);
    if (worker == nullptr) {
        HILOG_ERROR("worker:: worker is null");
        return;
    }
    worker->HostOnErrorInner();
    worker->TerminateInner();
}

void Worker::HostOnErrorInner()
{
    if (hostEnv_ == nullptr || HostIsStop()) {
        HILOG_ERROR("worker:: host thread maybe is over when host onerror.");
        return;
    }
    napi_status status = napi_ok;
    HandleScope scope(hostEnv_, status);
    NAPI_CALL_RETURN_VOID(hostEnv_, status);
    NativeEngine* hostEngine = reinterpret_cast<NativeEngine*>(hostEnv_);
    if (!hostEngine->InitContainerScopeFunc(scopeId_)) {
        HILOG_WARN("worker:: InitContainerScopeFunc error when onerror begin(only stage model)");
    }

    napi_value obj = NapiHelper::GetReferenceValue(hostEnv_, workerRef_);
    napi_value callback = NapiHelper::GetNameProperty(hostEnv_, obj, "onerror");
    bool isCallable = NapiHelper::IsCallable(hostEnv_, callback);

    MessageDataType data;
    while (errorQueue_.DeQueue(&data)) {
        napi_value result = nullptr;
        napi_deserialize(hostEnv_, data, &result);

        napi_value argv[1] = { result };
        if (isCallable) {
            napi_value callbackResult = nullptr;
            napi_call_function(hostEnv_, NapiHelper::GetGlobalObject(hostEnv_), callback, 1, argv, &callbackResult);
        }
        // handle listeners
        bool isHandle = HandleEventListeners(hostEnv_, obj, 1, argv, "error");
        if (!isCallable && !isHandle) {
            napi_value businessError = ErrorHelper::ObjectToError(hostEnv_, result);
            napi_throw(hostEnv_, businessError);
            HandleHostException();
            return;
        }
        HandleHostException();
    }
    if (!hostEngine->FinishContainerScopeFunc(scopeId_)) {
        HILOG_WARN("worker:: FinishContainerScopeFunc error when onerror end(only stage model)");
    }
}

void Worker::PostMessageInner(MessageDataType data)
{
    if (IsTerminated()) {
        HILOG_DEBUG("worker:: worker has been terminated when PostMessageInner.");
        return;
    }
    workerMessageQueue_.EnQueue(data);
    std::lock_guard<std::mutex> lock(workerOnmessageMutex_);
    if (workerOnMessageSignal_ != nullptr && uv_is_active((uv_handle_t*)workerOnMessageSignal_)) {
        uv_async_send(workerOnMessageSignal_);
    }
}

void Worker::HostOnMessageErrorInner()
{
    if (hostEnv_ == nullptr || HostIsStop()) {
        HILOG_ERROR("worker:: host thread maybe is over");
        return;
    }
    napi_value obj = NapiHelper::GetReferenceValue(hostEnv_, workerRef_);
    CallHostFunction(0, nullptr, "onmessageerror");
    // handle listeners
    HandleEventListeners(hostEnv_, obj, 0, nullptr, "messageerror");
}

void Worker::TerminateInner()
{
    if (IsTerminated() || IsTerminating()) {
        HILOG_INFO("worker:: worker is not in running when TerminateInner");
        return;
    }
    // 1. Update State
    UpdateWorkerState(TERMINATEING);
    // 2. send null signal
    PostMessageInner(NULL);
}

void Worker::CloseInner()
{
    UpdateWorkerState(TERMINATEING);
    TerminateWorker();
}

bool Worker::UpdateWorkerState(RunnerState state)
{
    bool done = false;
    do {
        RunnerState oldState = runnerState_.load(std::memory_order_acquire);
        if (oldState >= state) {
            // make sure state sequence is start, running, terminating, terminated
            return false;
        }
        done = runnerState_.compare_exchange_strong(oldState, state);
    } while (!done);
    return true;
}

bool Worker::UpdateHostState(HostState state)
{
    bool done = false;
    do {
        HostState oldState = hostState_.load(std::memory_order_acquire);
        if (oldState >= state) {
            // make sure state sequence is ACTIVE, INACTIVE
            return false;
        }
        done = hostState_.compare_exchange_strong(oldState, state);
    } while (!done);
    return true;
}

void Worker::TerminateWorker()
{
    HITRACE_HELPER_METER_NAME(__PRETTY_FUNCTION__);
    // when there is no active handle, worker loop will stop automatic.
    {
        std::lock_guard<std::mutex> lock(workerOnmessageMutex_);
        uv_close(reinterpret_cast<uv_handle_t*>(workerOnMessageSignal_), [](uv_handle_t* handle) {
            if (handle != nullptr) {
                delete reinterpret_cast<uv_async_t*>(handle);
                handle = nullptr;
            }
        });
    }
#if !defined(WINDOWS_PLATFORM) && !defined(MAC_PLATFORM)
    uv_close(reinterpret_cast<uv_handle_t*>(&ddebuggerOnPostTaskSignal_), nullptr);
#endif
    CloseWorkerCallback();
    uv_loop_t* loop = GetWorkerLoop();
    if (loop != nullptr) {
        if (g_workers.size() <= 1) {
            Timer::ClearEnvironmentTimer(workerEnv_);
        }
        uv_stop(loop);
    }
    UpdateWorkerState(TERMINATED);
}

void Worker::PublishWorkerOverSignal()
{
    // post NULL tell host worker is not running
    if (!HostIsStop()) {
        hostMessageQueue_.EnQueue(NULL);
        uv_async_send(hostOnMessageSignal_);
    }
}

void Worker::WorkerOnMessage(const uv_async_t* req)
{
    HITRACE_HELPER_METER_NAME(__PRETTY_FUNCTION__);
    Worker* worker = static_cast<Worker*>(req->data);
    if (worker == nullptr) {
        HILOG_ERROR("worker::worker is null");
        return;
    }
    worker->WorkerOnMessageInner();
}

void Worker::WorkerOnMessageInner()
{
    if (IsTerminated()) {
        return;
    }
    napi_status status;
    napi_handle_scope scope = nullptr;
    status = napi_open_handle_scope(workerEnv_, &scope);
    if (status != napi_ok || scope == nullptr) {
        HILOG_ERROR("worker:: WorkerOnMessage open handle scope failed.");
        return;
    }
    MessageDataType data = nullptr;
    while (!IsTerminated() && workerMessageQueue_.DeQueue(&data)) {
        if (data == nullptr) {
            HILOG_DEBUG("worker:: worker reveive terminate signal");
            // Close handlescope need before TerminateWorker
            napi_close_handle_scope(workerEnv_, scope);
            TerminateWorker();
            return;
        }
        napi_value result = nullptr;
        status = napi_deserialize(workerEnv_, data, &result);
        if (status != napi_ok || result == nullptr) {
            WorkerOnMessageErrorInner();
            continue;
        }

        napi_value event = nullptr;
        napi_create_object(workerEnv_, &event);
        napi_set_named_property(workerEnv_, event, "data", result);
        napi_value argv[1] = { event };
        CallWorkerFunction(1, argv, "onmessage", true);

        napi_value obj = NapiHelper::GetReferenceValue(workerEnv_, this->workerPort_);
        ParentPortHandleEventListeners(workerEnv_, obj, 1, argv, "message", true);
    }
    napi_close_handle_scope(workerEnv_, scope);
}

bool Worker::HandleEventListeners(napi_env env, napi_value recv, size_t argc, const napi_value* argv, const char* type)
{
    std::string listener(type);
    auto iter = eventListeners_.find(listener);
    if (iter == eventListeners_.end()) {
        HILOG_DEBUG("worker:: there is no listener for type %{public}s in host thread", type);
        return false;
    }

    std::list<WorkerListener*>& listeners = iter->second;
    std::list<WorkerListener*>::iterator it = listeners.begin();
    while (it != listeners.end()) {
        WorkerListener* data = *it++;
        napi_value callbackObj = NapiHelper::GetReferenceValue(env, data->callback_);
        if (!NapiHelper::IsCallable(env, callbackObj)) {
            HILOG_DEBUG("worker:: host thread listener %{public}s is not callable", type);
            return false;
        }
        napi_value callbackResult = nullptr;
        napi_call_function(env, NapiHelper::GetGlobalObject(env), callbackObj, argc, argv, &callbackResult);
        if (!data->NextIsAvailable()) {
            listeners.remove(data);
            CloseHelp::DeletePointer(data, false);
        }
    }
    return true;
}

void Worker::HandleHostException() const
{
    if (!NapiHelper::IsExceptionPending(hostEnv_)) {
        return;
    }
    auto hostEngine = reinterpret_cast<NativeEngine*>(hostEnv_);
    hostEngine->HandleUncaughtException();
}

void Worker::HandleException()
{
    if (!NapiHelper::IsExceptionPending(workerEnv_)) {
        return;
    }

    napi_status status = napi_ok;
    HandleScope scope(workerEnv_, status);
    NAPI_CALL_RETURN_VOID(workerEnv_, status);
    napi_value exception;
    napi_get_and_clear_last_exception(workerEnv_, &exception);
    if (exception == nullptr) {
        return;
    }

    napi_value obj = ErrorHelper::TranslateErrorEvent(workerEnv_, exception);

    // WorkerGlobalScope onerror
    WorkerOnErrorInner(obj);

    if (hostEnv_ != nullptr) {
        napi_value data = nullptr;
        napi_serialize(workerEnv_, obj, NapiHelper::GetUndefinedValue(workerEnv_), &data);
        {
            std::lock_guard<std::recursive_mutex> lock(liveStatusLock_);
            if (!HostIsStop()) {
                errorQueue_.EnQueue(data);
                uv_async_send(hostOnErrorSignal_);
            }
        }
    } else {
        HILOG_ERROR("worker:: host engine is nullptr.");
    }
}

void Worker::WorkerOnMessageErrorInner()
{
    isErrorExit_ = true;
    CallWorkerFunction(0, nullptr, "onmessageerror", true);
    napi_value obj = NapiHelper::GetReferenceValue(workerEnv_, this->workerPort_);
    ParentPortHandleEventListeners(workerEnv_, obj, 0, nullptr, "messageerror", true);
}

void Worker::PostMessageToHostInner(MessageDataType data)
{
    std::lock_guard<std::recursive_mutex> lock(liveStatusLock_);
    if (hostEnv_ != nullptr && !HostIsStop()) {
        hostMessageQueue_.EnQueue(data);
        uv_async_send(hostOnMessageSignal_);
    } else {
        HILOG_ERROR("worker:: worker host engine is nullptr when PostMessageToHostInner.");
    }
}

bool Worker::WorkerListener::operator==(const WorkerListener& listener) const
{
    napi_value obj = NapiHelper::GetReferenceValue(listener.env_, listener.callback_);
    napi_value compareObj = NapiHelper::GetReferenceValue(env_, callback_);
    // the env of listener and cmp listener must be same env because of Synchronization method
    return NapiHelper::StrictEqual(env_, compareObj, obj);
}

void Worker::AddListenerInner(napi_env env, const char* type, const WorkerListener* listener)
{
    std::string typestr(type);
    auto iter = eventListeners_.find(typestr);
    if (iter == eventListeners_.end()) {
        std::list<WorkerListener*> listeners;
        listeners.emplace_back(const_cast<WorkerListener*>(listener));
        eventListeners_[typestr] = listeners;
    } else {
        std::list<WorkerListener*>& listenerList = iter->second;
        std::list<WorkerListener*>::iterator it = std::find_if(
            listenerList.begin(), listenerList.end(), Worker::FindWorkerListener(env, listener->callback_));
        if (it != listenerList.end()) {
            return;
        }
        listenerList.emplace_back(const_cast<WorkerListener*>(listener));
    }
}

void Worker::RemoveListenerInner(napi_env env, const char* type, napi_ref callback)
{
    std::string typestr(type);
    auto iter = eventListeners_.find(typestr);
    if (iter == eventListeners_.end()) {
        return;
    }
    std::list<WorkerListener*>& listenerList = iter->second;
    if (callback != nullptr) {
        std::list<WorkerListener*>::iterator it =
            std::find_if(listenerList.begin(), listenerList.end(), Worker::FindWorkerListener(env, callback));
        if (it != listenerList.end()) {
            CloseHelp::DeletePointer(*it, false);
            listenerList.erase(it);
        }
    } else {
        for (auto it = listenerList.begin(); it != listenerList.end(); it++) {
            CloseHelp::DeletePointer(*it, false);
        }
        eventListeners_.erase(typestr);
    }
}

Worker::~Worker()
{
    if (!HostIsStop()) {
        ReleaseHostThreadContent();
    }
    RemoveAllListenerInner();
    ClearSyncEventListener();
}

void Worker::RemoveAllListenerInner()
{
    for (auto iter = eventListeners_.begin(); iter != eventListeners_.end(); iter++) {
        std::list<WorkerListener*>& listeners = iter->second;
        for (auto item = listeners.begin(); item != listeners.end(); item++) {
            WorkerListener* listener = *item;
            CloseHelp::DeletePointer(listener, false);
        }
    }
    eventListeners_.clear();
}

void Worker::ReleaseHostThreadContent()
{
    // 1. clear message send to host thread
    hostMessageQueue_.Clear(hostEnv_);
    hostSyncEventQueue_.Clear(hostEnv_);
    // 2. clear error queue send to host thread
    errorQueue_.Clear(hostEnv_);
    if (!HostIsStop()) {
        napi_status status = napi_ok;
        HandleScope scope(hostEnv_, status);
        NAPI_CALL_RETURN_VOID(hostEnv_, status);
        // 3. set thisVar's nativepointer be null
        napi_value thisVar = NapiHelper::GetReferenceValue(hostEnv_, workerRef_);
        Worker* worker = nullptr;
        napi_remove_wrap(hostEnv_, thisVar, reinterpret_cast<void**>(&worker));
        hostEnv_ = nullptr;
        // 4. set workerRef_ be null
        workerRef_ = nullptr;
    }
}

void Worker::WorkerOnErrorInner(napi_value error)
{
    isErrorExit_ = true;
    napi_value argv[1] = { error };
    CallWorkerFunction(1, argv, "onerror", false);
    napi_value obj = NapiHelper::GetReferenceValue(workerEnv_, this->workerPort_);
    ParentPortHandleEventListeners(workerEnv_, obj, 1, argv, "error", false);
}

bool Worker::CallWorkerFunction(size_t argc, const napi_value* argv, const char* methodName, bool tryCatch)
{
    if (workerEnv_ == nullptr) {
        HILOG_ERROR("Worker:: worker is not running when call workerPort.%{public}s.", methodName);
        return false;
    }
    napi_value callback = NapiHelper::GetNamePropertyInParentPort(workerEnv_, workerPort_, methodName);
    bool isCallable = NapiHelper::IsCallable(workerEnv_, callback);
    if (!isCallable) {
        HILOG_DEBUG("worker:: workerPort.%{public}s is not Callable", methodName);
        return false;
    }
    napi_value workerPortObj = NapiHelper::GetReferenceValue(workerEnv_, workerPort_);
    napi_value callbackResult = nullptr;
    napi_call_function(workerEnv_, workerPortObj, callback, argc, argv, &callbackResult);
    if (tryCatch && callbackResult == nullptr) {
        HILOG_DEBUG("worker:: workerPort.%{public}s handle exception", methodName);
        HandleException();
        return false;
    }
    return true;
}

void Worker::CloseWorkerCallback()
{
    CallWorkerFunction(0, nullptr, "onclose", true);
    // off worker inited environment
    {
        std::lock_guard<std::recursive_mutex> lock(liveStatusLock_);
        if (HostIsStop()) {
            return;
        }
        auto hostEngine = reinterpret_cast<NativeEngine*>(hostEnv_);
        if (!hostEngine->CallOffWorkerFunc(reinterpret_cast<NativeEngine*>(workerEnv_))) {
            HILOG_ERROR("worker:: CallOffWorkerFunc error");
        }
    }
}

void Worker::ReleaseWorkerThreadContent()
{
    HITRACE_HELPER_METER_NAME(__PRETTY_FUNCTION__);
    auto hostEngine = reinterpret_cast<NativeEngine*>(hostEnv_);
    auto workerEngine = reinterpret_cast<NativeEngine*>(workerEnv_);
    if (hostEngine != nullptr && workerEngine != nullptr) {
        if (!hostEngine->DeleteWorker(workerEngine)) {
            HILOG_ERROR("worker:: DeleteWorker error");
        }
        hostEngine->DecreaseSubEnvCounter();
    }
    // 1. remove worker instance count
    {
        std::lock_guard<std::mutex> lock(g_workersMutex);
        std::list<Worker*>::iterator it = std::find(g_workers.begin(), g_workers.end(), this);
        if (it != g_workers.end()) {
            g_workers.erase(it);
        }
    }

    ParentPortRemoveAllListenerInner();

    // 2. delete worker's parentPort
    NapiHelper::DeleteReference(workerEnv_, workerPort_);
    workerPort_ = nullptr;

    // 3. clear message send to worker thread
    workerMessageQueue_.Clear(workerEnv_);
    workerSyncEventQueue_.Clear(workerEnv_);
    CloseHelp::DeletePointer(reinterpret_cast<NativeEngine*>(workerEnv_), false);
    workerEnv_ = nullptr;
}

void Worker::ParentPortAddListenerInner(napi_env env, const char* type, const WorkerListener* listener)
{
    std::string typestr(type);
    auto iter = parentPortEventListeners_.find(typestr);
    if (iter == parentPortEventListeners_.end()) {
        std::list<WorkerListener*> listeners;
        listeners.emplace_back(const_cast<WorkerListener*>(listener));
        parentPortEventListeners_[typestr] = listeners;
    } else {
        std::list<WorkerListener*>& listenerList = iter->second;
        std::list<WorkerListener*>::iterator it = std::find_if(
            listenerList.begin(), listenerList.end(), Worker::FindWorkerListener(env, listener->callback_));
        if (it != listenerList.end()) {
            return;
        }
        listenerList.emplace_back(const_cast<WorkerListener*>(listener));
    }
}

void Worker::ParentPortRemoveAllListenerInner()
{
    for (auto iter = parentPortEventListeners_.begin(); iter != parentPortEventListeners_.end(); iter++) {
        std::list<WorkerListener*>& listeners = iter->second;
        for (auto item = listeners.begin(); item != listeners.end(); item++) {
            WorkerListener* listener = *item;
            CloseHelp::DeletePointer(listener, false);
        }
    }
    parentPortEventListeners_.clear();
}

void Worker::ParentPortRemoveListenerInner(napi_env env, const char* type, napi_ref callback)
{
    std::string typestr(type);
    auto iter = parentPortEventListeners_.find(typestr);
    if (iter == parentPortEventListeners_.end()) {
        return;
    }
    std::list<WorkerListener*>& listenerList = iter->second;
    if (callback != nullptr) {
        std::list<WorkerListener*>::iterator it =
            std::find_if(listenerList.begin(), listenerList.end(), Worker::FindWorkerListener(env, callback));
        if (it != listenerList.end()) {
            CloseHelp::DeletePointer(*it, false);
            listenerList.erase(it);
        }
    } else {
        for (auto it = listenerList.begin(); it != listenerList.end(); it++) {
            CloseHelp::DeletePointer(*it, false);
        }
        parentPortEventListeners_.erase(typestr);
    }
}

void Worker::ParentPortHandleEventListeners(napi_env env, napi_value recv, size_t argc,
                                            const napi_value* argv, const char* type, bool tryCatch)
{
    std::string listener(type);
    auto iter = parentPortEventListeners_.find(listener);
    if (iter == parentPortEventListeners_.end()) {
        HILOG_DEBUG("worker:: there is no listener for type %{public}s in worker thread", type);
        return;
    }

    std::list<WorkerListener*>& listeners = iter->second;
    std::list<WorkerListener*>::iterator it = listeners.begin();
    while (it != listeners.end()) {
        WorkerListener* data = *it++;
        napi_value callbackObj = NapiHelper::GetReferenceValue(env, data->callback_);
        if (!NapiHelper::IsCallable(env, callbackObj)) {
            HILOG_DEBUG("worker:: workerPort.addEventListener %{public}s is not callable", type);
            return;
        }
        napi_value workerPortObj = NapiHelper::GetReferenceValue(env, workerPort_);
        napi_value callbackResult = nullptr;
        napi_call_function(env, workerPortObj, callbackObj, argc, argv, &callbackResult);
        if (!data->NextIsAvailable()) {
            listeners.remove(data);
            CloseHelp::DeletePointer(data, false);
        }
        if (tryCatch && callbackResult == nullptr) {
            HandleException();
            return;
        }
    }
}

void Worker::WorkerThrowError(napi_env env, int32_t errCode, const char* errMessage)
{
    auto engine = reinterpret_cast<NativeEngine*>(env);
    while (!engine->IsMainThread()) {
        engine = engine->GetHostEngine();
    }
    if (engine->IsTargetWorkerVersion(WorkerVersion::NEW)) {
        ErrorHelper::ThrowError(env, errCode, errMessage);
    }
}

bool Worker::CanCreateWorker(napi_env env, WorkerVersion target)
{
    auto engine = reinterpret_cast<NativeEngine*>(env);
    while (!engine->IsMainThread()) {
        engine = engine->GetHostEngine();
    }
    if (engine->CheckAndSetWorkerVersion(WorkerVersion::NONE, target) || engine->IsTargetWorkerVersion(target)) {
        return true;
    }
    return false;
}

#if !defined(WINDOWS_PLATFORM) && !defined(MAC_PLATFORM)
void Worker::HandleDebuggerTask(const uv_async_t* req)
{
    Worker* worker = DereferenceHelp::DereferenceOf(&Worker::ddebuggerOnPostTaskSignal_, req);
    if (worker == nullptr) {
        HILOG_ERROR("worker::worker is null");
        return;
    }

    worker->debuggerTask_();
}

void Worker::DebuggerOnPostTask(std::function<void()>&& task)
{
    if (IsTerminated()) {
        HILOG_ERROR("worker:: worker has been terminated.");
        return;
    }
    if (uv_is_active((uv_handle_t*)&ddebuggerOnPostTaskSignal_)) {
        debuggerTask_ = std::move(task);
        uv_async_send(&ddebuggerOnPostTaskSignal_);
    }
}
#endif

} // namespace Commonlibrary::Concurrent::WorkerModule
