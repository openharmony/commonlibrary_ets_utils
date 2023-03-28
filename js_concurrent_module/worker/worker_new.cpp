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

#include "worker_new.h"

#include "helper/error_helper.h"
#include "hitrace_meter.h"
#include "plugin/timer.h"

namespace Commonlibrary::Concurrent::WorkerModule {
using namespace Commonlibrary::Concurrent::Common::Plugin;
const static int MAXWORKERS = 8;
static std::list<NewWorker *> g_newWorkers;
static std::mutex g_newWorkersMutex;

NewWorker::NewWorker(napi_env env, napi_ref thisVar)
    : hostEnv_(env), workerRef_(thisVar)
{}

napi_value NewWorker::InitWorker(napi_env env, napi_value exports)
{
    HITRACE_METER_NAME(HITRACE_TAG_COMMONLIBRARY, __PRETTY_FUNCTION__);
    NativeEngine* engine = reinterpret_cast<NativeEngine*>(env);
    const char className[] = "ThreadWorker";
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("postMessage", PostMessage),
        DECLARE_NAPI_FUNCTION("terminate", Terminate),
        DECLARE_NAPI_FUNCTION("on", On),
        DECLARE_NAPI_FUNCTION("once", Once),
        DECLARE_NAPI_FUNCTION("off", Off),
        DECLARE_NAPI_FUNCTION("addEventListener", AddEventListener),
        DECLARE_NAPI_FUNCTION("dispatchEvent", DispatchEvent),
        DECLARE_NAPI_FUNCTION("removeEventListener", RemoveEventListener),
        DECLARE_NAPI_FUNCTION("removeAllListener", RemoveAllListener),
        DECLARE_NAPI_FUNCTION("cancelTasks", CancelTask),
    };
    napi_value workerClazz = nullptr;
    napi_define_class(env, className, sizeof(className), NewWorker::WorkerConstructor, nullptr,
        sizeof(properties) / sizeof(properties[0]), properties, &workerClazz);
    napi_set_named_property(env, exports, "ThreadWorker", workerClazz);

    if (!engine->IsMainThread()) {
        if (g_newWorkers.size() == 0) {
            HILOG_INFO("worker:: The new worker is not used.");
            return exports;
        }
        NewWorker* worker = nullptr;
        for (auto item = g_newWorkers.begin(); item != g_newWorkers.end(); item++) {
            if ((*item)->IsSameWorkerEnv(env)) {
                worker = *item;
            }
        }
        if (worker == nullptr) {
            ErrorHelper::ThrowError(env, ErrorHelper::WORKENOTRUNNING_ERROR, "worker is null when InitWorker");
            return exports;
        }

        napi_property_descriptor properties[] = {
            DECLARE_NAPI_FUNCTION_WITH_DATA("postMessage", PostMessageToHost, worker),
            DECLARE_NAPI_FUNCTION_WITH_DATA("close", CloseWorker, worker),
            DECLARE_NAPI_FUNCTION_WITH_DATA("cancelTasks", WorkerPortCancelTask, worker),
            DECLARE_NAPI_FUNCTION_WITH_DATA("addEventListener", WorkerPortAddEventListener, worker),
            DECLARE_NAPI_FUNCTION_WITH_DATA("dispatchEvent", WorkerPortDispatchEvent, worker),
            DECLARE_NAPI_FUNCTION_WITH_DATA("removeEventListener", WorkerPortRemoveEventListener, worker),
            DECLARE_NAPI_FUNCTION_WITH_DATA("removeAllListener", WorkerPortRemoveAllListener, worker),
        };
        napi_value workerPortObj = nullptr;
        napi_create_object(env, &workerPortObj);
        napi_define_properties(env, workerPortObj, sizeof(properties) / sizeof(properties[0]), properties);

        // 5. register worker name in DedicatedWorkerGlobalScope
        std::string workerName = worker->GetName();
        if (!workerName.empty()) {
            napi_value nameValue = nullptr;
            napi_create_string_utf8(env, workerName.c_str(), workerName.length(), &nameValue);
            napi_set_named_property(env, workerPortObj, "name", nameValue);
        }
        napi_set_named_property(env, exports, "workerPort", workerPortObj);

        // register worker parentPort.
        napi_create_reference(env, workerPortObj, 1, &worker->workerPort_);
    }
    return exports;
}

napi_value NewWorker::WorkerConstructor(napi_env env, napi_callback_info cbinfo)
{
    // check argv count
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    if (argc < 1) {
        ErrorHelper::ThrowError(env,
            ErrorHelper::TYPE_ERROR, "the number of create worker param must be more than 1 with new");
        return nullptr;
    }

    // check 1st param is string
    napi_value thisVar = nullptr;
    void* data = nullptr;
    napi_value* args = new napi_value[argc];
    ObjectScope<napi_value> scope(args, true);
    napi_get_cb_info(env, cbinfo, &argc, args, &thisVar, &data);
    if (!NapiHelper::IsString(args[0])) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "the type of Worker 1st param must be string.");
        return nullptr;
    }
    NewWorker* worker = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_newWorkersMutex);
        if (g_newWorkers.size() >= MAXWORKERS) {
            ErrorHelper::ThrowError(env,
                ErrorHelper::WORKERINITIALIZATION_ERROR, "the number of workers exceeds the maximum.");
            return nullptr;
        }

        // 2. new worker instance
        worker = new NewWorker(env, nullptr);
        if (worker == nullptr) {
            ErrorHelper::ThrowError(env, ErrorHelper::WORKERINITIALIZATION_ERROR, "creat worker error");
            return nullptr;
        }
        g_newWorkers.push_back(worker);
    }

    if (argc > 1 && NapiHelper::IsObject(args[1])) {
        napi_value nameValue = NapiHelper::GetNameProperty(env, args[1], "name");
        if (NapiHelper::IsNotUndefined(nameValue)) {
            if (NapiHelper::IsString(nameValue)) {
                char* nameStr = NapiHelper::GetString(env, nameValue);
                if (nameStr == nullptr) {
                    ErrorHelper::ThrowError(env,
                        ErrorHelper::WORKERINITIALIZATION_ERROR, "the name of worker is null.");
                    return nullptr;
                }
                worker->name_ = std::string(nameStr);
                CloseHelp::DeletePointer(nameStr, true);
            } else {
                ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "the type of name in worker must be string.");
                return nullptr;
            }
        }

        napi_value typeValue = NapiHelper::GetNameProperty(env, args[1], "type");
        if (NapiHelper::IsNotUndefined(typeValue)) {
            if (NapiHelper::IsString(typeValue)) {
                char* typeStr = NapiHelper::GetString(env, typeValue);
                if (typeStr == nullptr) {
                    ErrorHelper::ThrowError(env,
                        ErrorHelper::WORKERINITIALIZATION_ERROR, "the type of worker is null.");
                    return nullptr;
                }
                if (strcmp("classic", typeStr) == 0) {
                    worker->SetScriptMode(CLASSIC);
                    CloseHelp::DeletePointer(typeStr, true);
                } else {
                    ErrorHelper::ThrowError(env,
                        ErrorHelper::TYPE_ERROR, "the type must be classic, unsupport others now.");
                    CloseHelp::DeletePointer(typeStr, true);
                    CloseHelp::DeletePointer(worker, false);
                    return nullptr;
                }
            } else {
                ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "the type of type must be string.");
                return nullptr;
            }
        }
    }

    // 3. execute in thread
    char* script = NapiHelper::GetString(env, args[0]);
    if (script == nullptr) {
        ErrorHelper::ThrowError(env,
            ErrorHelper::WORKERFILEPATH_ERROR, "the file path is invaild, maybe path is null.");
        return nullptr;
    }
    napi_wrap(
        env, thisVar, worker,
        [](napi_env env, void* data, void* hint) {
            NewWorker* worker = reinterpret_cast<NewWorker*>(data);
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
                    worker->ReleaseHostThreadContent();
                }
                if (!worker->IsRunning()) {
                    HILOG_INFO("worker:: worker is not in running");
                    return;
                }
                worker->TerminateInner();
            }
        },
        nullptr, nullptr);
    napi_create_reference(env, thisVar, 1, &worker->workerRef_);
    worker->StartExecuteInThread(env, script);
    return thisVar;
}

napi_value NewWorker::PostMessage(napi_env env, napi_callback_info cbinfo)
{
    HITRACE_METER_NAME(HITRACE_TAG_COMMONLIBRARY, __PRETTY_FUNCTION__);
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    if (argc < 1) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "Worker messageObject must be not null with postMessage");
        return nullptr;
    }
    napi_value* argv = new napi_value[argc];
    ObjectScope<napi_value> scope(argv, true);
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, cbinfo, &argc, argv, &thisVar, nullptr);
    NewWorker* worker = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void**>(&worker));

    if (worker == nullptr || worker->IsTerminated() || worker->IsTerminating()) {
        ErrorHelper::ThrowError(env, ErrorHelper::WORKENOTRUNNING_ERROR, "maybe worker is terminated when PostMessage");
        return nullptr;
    }

    napi_value data = nullptr;
    napi_status serializeStatus = napi_ok;
    if (argc >= WORKERPARAMNUM) {
        if (!NapiHelper::IsArray(argv[1])) {
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "transfer list must be an Array");
            return nullptr;
        }
        serializeStatus = napi_serialize(env, argv[0], argv[1], &data);
    } else {
        serializeStatus = napi_serialize(env, argv[0], NapiHelper::GetUndefinedValue(env), &data);
    }
    if (serializeStatus != napi_ok || data == nullptr) {
        worker->HostOnMessageErrorInner();
        ErrorHelper::ThrowError(env, ErrorHelper::WORKERSERIALIZATION_ERROR, "failed to serialize message.");
        return nullptr;
    }
    worker->PostMessageInner(data);
    return NapiHelper::GetUndefinedValue(env);
}

napi_value NewWorker::Terminate(napi_env env, napi_callback_info cbinfo)
{
    HITRACE_METER_NAME(HITRACE_TAG_COMMONLIBRARY, __PRETTY_FUNCTION__);
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, cbinfo, nullptr, nullptr, &thisVar, nullptr);
    NewWorker* worker = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void**>(&worker));
    if (worker == nullptr) {
        ErrorHelper::ThrowError(env, ErrorHelper::WORKENOTRUNNING_ERROR, "worker is nullptr when Terminate");
        return nullptr;
    }
    if (worker->IsTerminated() || worker->IsTerminating()) {
        HILOG_INFO("worker:: worker is not in running when Terminate");
        return nullptr;
    }
    worker->TerminateInner();
    return NapiHelper::GetUndefinedValue(env);
}

napi_value NewWorker::On(napi_env env, napi_callback_info cbinfo)
{
    return AddListener(env, cbinfo, PERMANENT);
}

napi_value NewWorker::Once(napi_env env, napi_callback_info cbinfo)
{
    return AddListener(env, cbinfo, ONCE);
}

napi_value NewWorker::Off(napi_env env, napi_callback_info cbinfo)
{
    return RemoveListener(env, cbinfo);
}

napi_value NewWorker::RemoveEventListener(napi_env env, napi_callback_info cbinfo)
{
    return RemoveListener(env, cbinfo);
}

napi_value NewWorker::AddEventListener(napi_env env, napi_callback_info cbinfo)
{
    return AddListener(env, cbinfo, PERMANENT);
}

napi_value NewWorker::AddListener(napi_env env, napi_callback_info cbinfo, ListenerMode mode)
{
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    if (argc < WORKERPARAMNUM) {
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
    if (!NapiHelper::IsString(args[0])) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "Worker add listener 1st param must be string");
        return nullptr;
    }
    if (!NapiHelper::IsCallable(env, args[1])) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "Worker add listener 2st param must be callable");
        return nullptr;
    }
    NewWorker* worker = nullptr;
    napi_unwrap(env, thisVar, (void**)&worker);
    if (worker == nullptr) {
        ErrorHelper::ThrowError(env, ErrorHelper::WORKENOTRUNNING_ERROR, "maybe worker is terminated");
        return nullptr;
    }

    napi_ref callback = NapiHelper::CreateReference(env, args[1], 1);
    auto listener = new WorkerListener(env, callback, mode);
    if (mode == ONCE && argc > WORKERPARAMNUM) {
        if (NapiHelper::IsObject(args[WORKERPARAMNUM])) {
            napi_value onceValue = NapiHelper::GetNameProperty(env, args[WORKERPARAMNUM], "once");
            bool isOnce = NapiHelper::GetBooleanValue(env, onceValue);
            if (!isOnce) {
                listener->SetMode(PERMANENT);
            }
        }
    }
    char* typeStr = NapiHelper::GetString(env, args[0]);
    if (typeStr == nullptr) {
        CloseHelp::DeletePointer(listener, false);
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "worker listener type must be string.");
        return nullptr;
    }
    worker->AddListenerInner(env, typeStr, listener);
    CloseHelp::DeletePointer(typeStr, true);
    return NapiHelper::GetUndefinedValue(env);
}

napi_value NewWorker::RemoveListener(napi_env env, napi_callback_info cbinfo)
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
    if (!NapiHelper::IsString(args[0])) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "the type of remove listener 1st param must be string");
        return nullptr;
    }

    NewWorker* worker = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void**>(&worker));
    if (worker == nullptr) {
        ErrorHelper::ThrowError(env, ErrorHelper::WORKENOTRUNNING_ERROR, "maybe worker is terminated");
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

void NewCallWorkCallback(napi_env env, napi_value recv, size_t argc, const napi_value* argv, const char* type)
{
    napi_value callback = nullptr;
    napi_get_named_property(env, recv, type, &callback);
    if (NapiHelper::IsCallable(env, callback)) {
        napi_value callbackResult = nullptr;
        napi_call_function(env, recv, callback, argc, argv, &callbackResult);
    }
}

napi_value NewWorker::DispatchEvent(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    if (argc < 1) {
        ErrorHelper::ThrowError(env,
            ErrorHelper::TYPE_ERROR, "the count of event param must be more than 1 in DispatchEvent");
        return NapiHelper::CreateBooleanValue(env, false);
    }

    // check 1st param is string
    napi_value thisVar = nullptr;
    void* data = nullptr;
    napi_value* args = new napi_value[argc];
    ObjectScope<napi_value> scope(args, true);
    napi_get_cb_info(env, cbinfo, &argc, args, &thisVar, &data);

    if (!NapiHelper::IsObject(args[0])) {
        ErrorHelper::ThrowError(env,
            ErrorHelper::TYPE_ERROR, "the type of event 1st param must be Event in DispatchEvent");
        return NapiHelper::CreateBooleanValue(env, false);
    }

    NewWorker* worker = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void**>(&worker));
    if (worker == nullptr) {
        ErrorHelper::ThrowError(env, ErrorHelper::WORKENOTRUNNING_ERROR, "maybe worker has been terminated");
        return NapiHelper::CreateBooleanValue(env, false);
    }

    napi_value typeValue = NapiHelper::GetNameProperty(env, args[0], "type");
    if (!NapiHelper::IsString(typeValue)) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "the type of event type must be string");
        return NapiHelper::CreateBooleanValue(env, false);
    }

    napi_value obj = NapiHelper::GetReferenceValue(env, worker->workerRef_);
    napi_value argv[1] = { args[0] };

    char* typeStr = NapiHelper::GetString(env, typeValue);
    if (typeStr == nullptr) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "dispatchEvent event type must be not null");
        return NapiHelper::CreateBooleanValue(env, false);
    }
    if (strcmp(typeStr, "error") == 0) {
        NewCallWorkCallback(env, obj, 1, argv, "onerror");
    } else if (strcmp(typeStr, "messageerror") == 0) {
        NewCallWorkCallback(env, obj, 1, argv, "onmessageerror");
    } else if (strcmp(typeStr, "message") == 0) {
        NewCallWorkCallback(env, obj, 1, argv, "onmessage");
    }

    worker->HandleEventListeners(env, obj, 1, argv, typeStr);

    CloseHelp::DeletePointer(typeStr, true);
    return NapiHelper::CreateBooleanValue(env, true);
}

napi_value NewWorker::RemoveAllListener(napi_env env, napi_callback_info cbinfo)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, cbinfo, nullptr, nullptr, &thisVar, nullptr);
    NewWorker* worker = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void**>(&worker));
    if (worker == nullptr) {
        ErrorHelper::ThrowError(env, ErrorHelper::WORKENOTRUNNING_ERROR, "maybe worker is terminated");
        return nullptr;
    }

    worker->RemoveAllListenerInner();
    return NapiHelper::GetUndefinedValue(env);
}

napi_value NewWorker::CancelTask(napi_env env, napi_callback_info cbinfo)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, cbinfo, nullptr, nullptr, &thisVar, nullptr);
    NewWorker* worker = nullptr;
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

napi_value NewWorker::PostMessageToHost(napi_env env, napi_callback_info cbinfo)
{
    HITRACE_METER_NAME(HITRACE_TAG_COMMONLIBRARY, __PRETTY_FUNCTION__);
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    if (argc < 1) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "Worker param count must be more than 1 with new");
        return nullptr;
    }
    napi_value* argv = new napi_value[argc];
    ObjectScope<napi_value> scope(argv, true);
    NewWorker* worker = nullptr;
    napi_get_cb_info(env, cbinfo, &argc, argv, nullptr, reinterpret_cast<void**>(&worker));

    if (worker == nullptr) {
        ErrorHelper::ThrowError(env, ErrorHelper::WORKENOTRUNNING_ERROR, "worker is nullptr when post message to host");
        return nullptr;
    }

    if (!worker->IsRunning()) {
        // if worker is not running, don't send any message to host thread
        HILOG_INFO("worker:: when post message to host occur worker is not in running.");
        return nullptr;
    }

    napi_value data = nullptr;
    napi_status serializeStatus = napi_ok;
    if (argc >= WORKERPARAMNUM) {
        if (!NapiHelper::IsArray(argv[1])) {
            ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "Transfer list must be an Array");
            return nullptr;
        }
        serializeStatus = napi_serialize(env, argv[0], argv[1], &data);
    } else {
        serializeStatus = napi_serialize(env, argv[0], NapiHelper::GetUndefinedValue(env), &data);
    }

    if (serializeStatus != napi_ok || data == nullptr) {
        worker->WorkerOnMessageErrorInner();
        ErrorHelper::ThrowError(env, ErrorHelper::WORKERSERIALIZATION_ERROR, "failed to serialize message.");
        return nullptr;
    }
    worker->PostMessageToHostInner(data);
    return NapiHelper::GetUndefinedValue(env);
}

napi_value NewWorker::CloseWorker(napi_env env, napi_callback_info cbinfo)
{
    HITRACE_METER_NAME(HITRACE_TAG_COMMONLIBRARY, __PRETTY_FUNCTION__);
    NewWorker* worker = nullptr;
    napi_get_cb_info(env, cbinfo, nullptr, nullptr, nullptr, (void**)&worker);
    if (worker != nullptr) {
        worker->CloseInner();
    } else {
        ErrorHelper::ThrowError(env, ErrorHelper::WORKENOTRUNNING_ERROR, "worker is null");
        return nullptr;
    }
    return NapiHelper::GetUndefinedValue(env);
}

napi_value NewWorker::WorkerPortCancelTask(napi_env env, napi_callback_info cbinfo)
{
    NewWorker* worker = nullptr;
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

napi_value NewWorker::WorkerPortAddEventListener(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    if (argc < WORKERPARAMNUM) {
        ErrorHelper::ThrowError(env,
            ErrorHelper::TYPE_ERROR, "worker listener param count must be more than WORKPARAMNUM.");
        return nullptr;
    }

    napi_value* args = new napi_value[argc];
    ObjectScope<napi_value> scope(args, true);
    NewWorker* worker = nullptr;
    napi_get_cb_info(env, cbinfo, &argc, args, nullptr, reinterpret_cast<void**>(&worker));

    if (!NapiHelper::IsString(args[0])) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "the type of worker listener 1st param must be string.");
        return nullptr;
    }

    if (!NapiHelper::IsCallable(env, args[1])) {
        ErrorHelper::ThrowError(env,
            ErrorHelper::TYPE_ERROR, "the type of worker listener 2st param must be callable.");
        return nullptr;
    }

    if (worker == nullptr || !worker->IsRunning()) {
        ErrorHelper::ThrowError(env, ErrorHelper::WORKENOTRUNNING_ERROR, "worker is not running.");
        return nullptr;
    }

    napi_ref callback = NapiHelper::CreateReference(env, args[1], 1);
    auto listener = new WorkerListener(env, callback, PERMANENT);
    if (argc > WORKERPARAMNUM && NapiHelper::IsObject(args[WORKERPARAMNUM])) {
        napi_value onceValue = NapiHelper::GetNameProperty(env, args[WORKERPARAMNUM], "once");
        bool isOnce = NapiHelper::GetBooleanValue(env, onceValue);
        if (isOnce) {
            listener->SetMode(ONCE);
        }
    }
    char* typeStr = NapiHelper::GetString(env, args[0]);
    if (typeStr == nullptr) {
        CloseHelp::DeletePointer(listener, false);
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "the type of worker listener type must be not null.");
        return nullptr;
    }
    worker->ParentPortAddListenerInner(env, typeStr, listener);
    CloseHelp::DeletePointer(typeStr, true);
    return NapiHelper::GetUndefinedValue(env);
}

napi_value NewWorker::WorkerPortDispatchEvent(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    if (argc < 1) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "DispatchEvent param count must be more than 1.");
        return NapiHelper::CreateBooleanValue(env, false);
    }

    napi_value* args = new napi_value[argc];
    ObjectScope<napi_value> scope(args, true);
    NewWorker* worker = nullptr;
    napi_get_cb_info(env, cbinfo, &argc, args, nullptr, reinterpret_cast<void**>(&worker));

    if (!NapiHelper::IsObject(args[0])) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "worker DispatchEvent 1st param must be Event.");
        return NapiHelper::CreateBooleanValue(env, false);
    }

    napi_value typeValue = NapiHelper::GetNameProperty(env, args[0], "type");
    if (!NapiHelper::IsString(typeValue)) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "worker event type must be string.");
        return NapiHelper::CreateBooleanValue(env, false);
    }

    if (worker == nullptr || !worker->IsRunning()) {
        ErrorHelper::ThrowError(env, ErrorHelper::WORKENOTRUNNING_ERROR, "worker is nullptr.");
        return NapiHelper::CreateBooleanValue(env, false);
    }

    napi_value argv[1] = { args[0] };
    char* typeStr = NapiHelper::GetString(env, typeValue);
    if (typeStr == nullptr) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "worker listener type must be not null.");
        return NapiHelper::CreateBooleanValue(env, false);
    }

    napi_value obj = NapiHelper::GetReferenceValue(env, worker->workerPort_);

    if (strcmp(typeStr, "error") == 0) {
        NewCallWorkCallback(env, obj, 1, argv, "onerror");
    } else if (strcmp(typeStr, "messageerror") == 0) {
        NewCallWorkCallback(env, obj, 1, argv, "onmessageerror");
    } else if (strcmp(typeStr, "message") == 0) {
        NewCallWorkCallback(env, obj, 1, argv, "onmessage");
    }

    worker->ParentPortHandleEventListeners(env, obj, 1, argv, typeStr);

    CloseHelp::DeletePointer(typeStr, true);
    return NapiHelper::CreateBooleanValue(env, true);
}

napi_value NewWorker::WorkerPortRemoveEventListener(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    if (argc < 1) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "worker listener param count must be more than 2.");
        return nullptr;
    }

    napi_value* args = new napi_value[argc];
    ObjectScope<napi_value> scope(args, true);
    NewWorker* worker = nullptr;
    napi_get_cb_info(env, cbinfo, &argc, args, nullptr, reinterpret_cast<void**>(&worker));

    if (!NapiHelper::IsString(args[0])) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "the type of worker listener 1st param must be string.");
        return nullptr;
    }

    if (argc > 1 && !NapiHelper::IsCallable(env, args[1])) {
        ErrorHelper::ThrowError(env,
            ErrorHelper::TYPE_ERROR, "the type of worker listener 2st param must be callable with on.");
        return nullptr;
    }

    if (worker == nullptr || !worker->IsRunning()) {
        ErrorHelper::ThrowError(env, ErrorHelper::WORKENOTRUNNING_ERROR, "worker is not running.");
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

napi_value NewWorker::WorkerPortRemoveAllListener(napi_env env, napi_callback_info cbinfo)
{
    NewWorker* worker = nullptr;
    napi_get_cb_info(env, cbinfo, nullptr, nullptr, nullptr, reinterpret_cast<void**>(&worker));

    if (worker == nullptr || !worker->IsRunning()) {
        ErrorHelper::ThrowError(env,
            ErrorHelper::WORKENOTRUNNING_ERROR, "worker is nullptr when WorkerPortRemoveAllListener");
        return nullptr;
    }

    worker->ParentPortRemoveAllListenerInner();
    return NapiHelper::GetUndefinedValue(env);
}

void NewWorker::GetContainerScopeId(napi_env env)
{
    NativeEngine* hostEngine = reinterpret_cast<NativeEngine*>(env);
    scopeId_ = hostEngine->GetContainerScopeIdFunc();
}

void NewWorker::StartExecuteInThread(napi_env env, const char* script)
{
    // 1. init hostOnMessageSignal_ in host loop
    uv_loop_t* loop = NapiHelper::GetLibUV(env);
    if (loop == nullptr) {
        ErrorHelper::ThrowError(env, ErrorHelper::WORKENOTRUNNING_ERROR, "engine loop is null");
        CloseHelp::DeletePointer(script, true);
        return;
    }
    GetContainerScopeId(env);
    hostOnMessageSignal_ = new uv_async_t;
    uv_async_init(loop, hostOnMessageSignal_, reinterpret_cast<uv_async_cb>(NewWorker::HostOnMessage));
    hostOnMessageSignal_->data = this;
    hostOnErrorSignal_ = new uv_async_t;
    uv_async_init(loop, hostOnErrorSignal_, reinterpret_cast<uv_async_cb>(NewWorker::HostOnError));
    hostOnErrorSignal_->data = this;

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

void NewWorker::ExecuteInThread(const void* data)
{
    StartTrace(HITRACE_TAG_COMMONLIBRARY, "Before ReleaseWorkerThreadContent");
    auto worker = reinterpret_cast<NewWorker*>(const_cast<void*>(data));
    // 1. create a runtime, nativeengine
    napi_env workerEnv = nullptr;
    {
        std::lock_guard<std::recursive_mutex> lock(worker->liveStatusLock_);
        if (worker->HostIsStop()) {
            CloseHelp::DeletePointer(worker, false);
            return;
        }
        napi_env env = worker->GetHostEnv();
        napi_create_runtime(env, &workerEnv);
        if (workerEnv == nullptr) {
            ErrorHelper::ThrowError(env, ErrorHelper::WORKENOTRUNNING_ERROR, "Worker create runtime error");
            return;
        }
        // mark worker env is subThread
        reinterpret_cast<NativeEngine*>(workerEnv)->MarkSubThread();
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
        uv_async_init(loop, worker->workerOnMessageSignal_, reinterpret_cast<uv_async_cb>(NewWorker::WorkerOnMessage));
        worker->workerOnMessageSignal_->data = worker;
#if !defined(WINDOWS_PLATFORM) && !defined(MAC_PLATFORM)
        uv_async_init(loop, &worker->ddebuggerOnPostTaskSignal_, reinterpret_cast<uv_async_cb>(
            NewWorker::HandleDebuggerTask));
#endif
        worker->UpdateWorkerState(RUNNING);
        // in order to invoke worker send before subThread start
        uv_async_send(worker->workerOnMessageSignal_);
        // 3. start worker loop
        worker->Loop();
    } else {
        HILOG_ERROR("worker:: worker PrepareForWorkerInstance failure");
        worker->UpdateWorkerState(TERMINATED);
    }
    FinishTrace(HITRACE_TAG_COMMONLIBRARY);
    worker->ReleaseWorkerThreadContent();
    StartTrace(HITRACE_TAG_COMMONLIBRARY, "After ReleaseWorkerThreadContent");
    std::lock_guard<std::recursive_mutex> lock(worker->liveStatusLock_);
    if (worker->HostIsStop()) {
        CloseHelp::DeletePointer(worker, false);
    } else {
        worker->PublishWorkerOverSignal();
    }
    FinishTrace(HITRACE_TAG_COMMONLIBRARY);
}

bool NewWorker::PrepareForWorkerInstance()
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
            std::bind(&NewWorker::DebuggerOnPostTask, this, std::placeholders::_1));
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
    HILOG_INFO("worker:: stringContent size is %{public}zu", scriptContent.size());
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

void NewWorker::HostOnMessage(const uv_async_t* req)
{
    HITRACE_METER_NAME(HITRACE_TAG_COMMONLIBRARY, __PRETTY_FUNCTION__);
    NewWorker* worker = static_cast<NewWorker*>(req->data);
    if (worker == nullptr) {
        HILOG_ERROR("worker:: worker is null when host onmessage.");
        return;
    }
    worker->HostOnMessageInner();
}

void NewWorker::HostOnMessageInner()
{
    if (hostEnv_ == nullptr || HostIsStop()) {
        HILOG_ERROR("worker:: host thread maybe is over when host onmessage.");
        return;
    }
    NativeEngine* engine = reinterpret_cast<NativeEngine*>(hostEnv_);
    if (!engine->InitContainerScopeFunc(scopeId_)) {
        HILOG_ERROR("worker:: InitContainerScopeFunc error when HostOnMessageInner begin(only stage model)");
    }
    napi_value obj = NapiHelper::GetReferenceValue(hostEnv_, workerRef_);
    napi_value callback = NapiHelper::GetNameProperty(hostEnv_, obj, "onmessage");
    bool isCallable = NapiHelper::IsCallable(hostEnv_, callback);

    MessageDataType data = nullptr;
    while (hostMessageQueue_.DeQueue(&data)) {
        // receive close signal.
        if (data == nullptr) {
            HILOG_INFO("worker:: worker received close signal");
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
        if (!isCallable) {
            // onmessage is not func, no need to continue
            HILOG_ERROR("worker:: worker onmessage is not a callable");
            continue;
        }
        // handle data, call worker onMessage function to handle.
        napi_value result = nullptr;
        napi_status status = napi_deserialize(hostEnv_, data, &result);
        if (status != napi_ok || result == nullptr) {
            HostOnMessageErrorInner();
            continue;
        }
        napi_value event = nullptr;
        napi_create_object(hostEnv_, &event);
        napi_set_named_property(hostEnv_, event, "data", result);
        napi_value argv[1] = { event };
        napi_value callbackResult = nullptr;
        napi_call_function(hostEnv_, obj, callback, 1, argv, &callbackResult);
        // handle listeners.
        HandleEventListeners(hostEnv_, obj, 1, argv, "message");
    }
    if (!engine->FinishContainerScopeFunc(scopeId_)) {
        HILOG_ERROR("worker:: FinishContainerScopeFunc error when HostOnMessageInner end(only stage model)");
    }
}

void NewWorker::CallHostFunction(size_t argc, const napi_value* argv, const char* methodName) const
{
    if (hostEnv_ == nullptr) {
        HILOG_ERROR("worker:: host thread maybe is over");
        return;
    }
    if (HostIsStop()) {
        ErrorHelper::ThrowError(hostEnv_,
            ErrorHelper::WORKENOTRUNNING_ERROR, "host thread maybe is over when CallHostFunction");
        return;
    }
    napi_value obj = NapiHelper::GetReferenceValue(hostEnv_, workerRef_);
    napi_value callback = NapiHelper::GetNameProperty(hostEnv_, obj, methodName);
    bool isCallable = NapiHelper::IsCallable(hostEnv_, callback);
    if (!isCallable) {
        HILOG_ERROR("worker:: worker %{public}s is not Callable", methodName);
        return;
    }
    napi_value callbackResult = nullptr;
    napi_call_function(hostEnv_, obj, callback, argc, argv, &callbackResult);
}

void NewWorker::CloseHostCallback() const
{
    napi_value exitValue = nullptr;
    napi_create_int32(hostEnv_, 1, &exitValue);
    napi_value argv[1] = { exitValue };
    CallHostFunction(1, argv, "onexit");
    CloseHelp::DeletePointer(this, false);
}

void NewWorker::HostOnError(const uv_async_t* req)
{
    NewWorker* worker = static_cast<NewWorker*>(req->data);
    if (worker == nullptr) {
        HILOG_ERROR("worker::worker is null");
        return;
    }
    worker->HostOnErrorInner();
    worker->TerminateInner();
}

void NewWorker::HostOnErrorInner()
{
    if (hostEnv_ == nullptr || HostIsStop()) {
        HILOG_ERROR("worker:: host thread maybe is over when host onerror.");
        return;
    }
    NativeEngine* hostEngine = reinterpret_cast<NativeEngine*>(hostEnv_);
    if (!hostEngine->InitContainerScopeFunc(scopeId_)) {
        HILOG_ERROR("worker:: InitContainerScopeFunc error when onerror begin(only stage model)");
    }

    napi_value obj = NapiHelper::GetReferenceValue(hostEnv_, workerRef_);
    napi_value callback = NapiHelper::GetNameProperty(hostEnv_, obj, "onerror");
    bool isCallable = NapiHelper::IsCallable(hostEnv_, callback);
    if (!isCallable) {
        HILOG_ERROR("worker:: worker onerror is not Callable");
        return;
    }
    MessageDataType data;
    while (errorQueue_.DeQueue(&data)) {
        napi_value result = nullptr;
        napi_deserialize(hostEnv_, data, &result);

        napi_value argv[1] = { result };
        napi_value callbackResult = nullptr;
        napi_call_function(hostEnv_, obj, callback, 1, argv, &callbackResult);

        // handle listeners
        HandleEventListeners(hostEnv_, obj, 1, argv, "error");
    }
    if (!hostEngine->FinishContainerScopeFunc(scopeId_)) {
        HILOG_ERROR("worker:: FinishContainerScopeFunc error when onerror end(only stage model)");
    }
}

void NewWorker::PostMessageInner(MessageDataType data)
{
    if (IsTerminated()) {
        HILOG_INFO("worker:: worker has been terminated when PostMessageInner.");
        return;
    }
    workerMessageQueue_.EnQueue(data);
    if (workerOnMessageSignal_ != nullptr && uv_is_active((uv_handle_t*)workerOnMessageSignal_)) {
        uv_async_send(workerOnMessageSignal_);
    }
}

void NewWorker::HostOnMessageErrorInner()
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

void NewWorker::TerminateInner()
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

void NewWorker::CloseInner()
{
    UpdateWorkerState(TERMINATEING);
    TerminateWorker();
}

bool NewWorker::UpdateWorkerState(RunnerState state)
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

bool NewWorker::UpdateHostState(HostState state)
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

void NewWorker::TerminateWorker()
{
    HITRACE_METER_NAME(HITRACE_TAG_COMMONLIBRARY, __PRETTY_FUNCTION__);
    // when there is no active handle, worker loop will stop automatic.
    uv_close(reinterpret_cast<uv_handle_t*>(workerOnMessageSignal_), [](uv_handle_t* handle) {
        if (handle != nullptr) {
            delete reinterpret_cast<uv_async_t*>(handle);
            handle = nullptr;
        }
    });
#if !defined(WINDOWS_PLATFORM) && !defined(MAC_PLATFORM)
    uv_close(reinterpret_cast<uv_handle_t*>(&ddebuggerOnPostTaskSignal_), nullptr);
#endif
    CloseWorkerCallback();
    uv_loop_t* loop = GetWorkerLoop();
    if (loop != nullptr) {
        if (g_newWorkers.size() <= 1) {
            Timer::ClearEnvironmentTimer(workerEnv_);
        }
        uv_stop(loop);
    }
    UpdateWorkerState(TERMINATED);
}

void NewWorker::PublishWorkerOverSignal()
{
    // post NULL tell host worker is not running
    if (!HostIsStop()) {
        hostMessageQueue_.EnQueue(NULL);
        uv_async_send(hostOnMessageSignal_);
    }
}

void NewWorker::WorkerOnMessage(const uv_async_t* req)
{
    HITRACE_METER_NAME(HITRACE_TAG_COMMONLIBRARY, __PRETTY_FUNCTION__);
    NewWorker* worker = static_cast<NewWorker*>(req->data);
    if (worker == nullptr) {
        HILOG_ERROR("worker::worker is null");
        return;
    }
    worker->WorkerOnMessageInner();
}

void NewWorker::WorkerOnMessageInner()
{
    if (IsTerminated()) {
        return;
    }
    MessageDataType data = nullptr;
    while (!IsTerminated() && workerMessageQueue_.DeQueue(&data)) {
        if (data == nullptr) {
            HILOG_INFO("worker:: worker reveive terminate signal");
            TerminateWorker();
            return;
        }
        napi_value result = nullptr;
        napi_status status = napi_deserialize(workerEnv_, data, &result);
        if (status != napi_ok || result == nullptr) {
            WorkerOnMessageErrorInner();
            continue;
        }

        napi_value event = nullptr;
        napi_create_object(workerEnv_, &event);
        napi_set_named_property(workerEnv_, event, "data", result);
        napi_value argv[1] = { event };
        bool callFeedback = CallWorkerFunction(1, argv, "onmessage", true);
        if (!callFeedback) {
            HILOG_ERROR("worker:: call WorkerGlobalScope onmessage error");
        }
    }
}

void NewWorker::HandleEventListeners(napi_env env, napi_value recv, size_t argc,
                                     const napi_value* argv, const char* type)
{
    std::string listener(type);
    auto iter = eventListeners_.find(listener);
    if (iter == eventListeners_.end()) {
        HILOG_INFO("worker:: there is no listener for type %{public}s", type);
        return;
    }

    std::list<WorkerListener*>& listeners = iter->second;
    std::list<WorkerListener*>::iterator it = listeners.begin();
    while (it != listeners.end()) {
        WorkerListener* data = *it++;
        napi_value callbackObj = NapiHelper::GetReferenceValue(env, data->callback_);
        napi_value callbackResult = nullptr;
        napi_call_function(env, recv, callbackObj, argc, argv, &callbackResult);
        if (!data->NextIsAvailable()) {
            listeners.remove(data);
            CloseHelp::DeletePointer(data, false);
        }
    }
}

void NewWorker::TranslateErrorEvent(napi_env env, napi_value error, napi_value *obj)
{
    napi_create_object(env, obj);

    // add lineno
    napi_value lineno = nullptr;
    napi_create_int32(env, 0, &lineno);
    napi_set_named_property(env, *obj, "lineno", lineno);

    // add colno
    napi_value colno = nullptr;
    napi_create_int32(env, 0, &colno);
    napi_set_named_property(env, *obj, "colno", colno);

    // add message
    napi_value msgValue = nullptr;
    napi_coerce_to_string(env, error, &msgValue);
    napi_set_named_property(env, *obj, "message", msgValue);

    // add error
    napi_set_named_property(env, *obj, "error", error);
}

void NewWorker::HandleException()
{
    napi_value exception;
    napi_get_and_clear_last_exception(workerEnv_, &exception);
    if (exception == nullptr) {
        return;
    }

    napi_value obj;
    NewWorker::TranslateErrorEvent(workerEnv_, exception, &obj);

    // add filename
    napi_value filenameValue = nullptr;
    napi_create_string_utf8(workerEnv_, script_.c_str(), script_.length(), &filenameValue);
    napi_set_named_property(workerEnv_, obj, "filename", filenameValue);

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

void NewWorker::WorkerOnMessageErrorInner()
{
    CallWorkerFunction(0, nullptr, "onmessageerror", true);
}

void NewWorker::PostMessageToHostInner(MessageDataType data)
{
    std::lock_guard<std::recursive_mutex> lock(liveStatusLock_);
    if (hostEnv_ != nullptr && !HostIsStop()) {
        hostMessageQueue_.EnQueue(data);
        uv_async_send(hostOnMessageSignal_);
    } else {
        HILOG_ERROR("worker:: worker host engine is nullptr when PostMessageToHostInner.");
    }
}

bool NewWorker::WorkerListener::operator==(const WorkerListener& listener) const
{
    napi_value obj = NapiHelper::GetReferenceValue(listener.env_, listener.callback_);
    napi_value compareObj = NapiHelper::GetReferenceValue(env_, callback_);
    // the env of listener and cmp listener must be same env because of Synchronization method
    return NapiHelper::StrictEqual(env_, compareObj, obj);
}

void NewWorker::AddListenerInner(napi_env env, const char* type, const WorkerListener* listener)
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
            listenerList.begin(), listenerList.end(), NewWorker::FindWorkerListener(env, listener->callback_));
        if (it != listenerList.end()) {
            return;
        }
        listenerList.emplace_back(const_cast<WorkerListener*>(listener));
    }
}

void NewWorker::RemoveListenerInner(napi_env env, const char* type, napi_ref callback)
{
    std::string typestr(type);
    auto iter = eventListeners_.find(typestr);
    if (iter == eventListeners_.end()) {
        return;
    }
    std::list<WorkerListener*>& listenerList = iter->second;
    if (callback != nullptr) {
        std::list<WorkerListener*>::iterator it =
            std::find_if(listenerList.begin(), listenerList.end(), NewWorker::FindWorkerListener(env, callback));
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

NewWorker::~NewWorker()
{
    if (!HostIsStop()) {
        ReleaseHostThreadContent();
    }
    RemoveAllListenerInner();
}

void NewWorker::RemoveAllListenerInner()
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

void NewWorker::ReleaseHostThreadContent()
{
    // 1. clear message send to host thread
    hostMessageQueue_.Clear(hostEnv_);
    // 2. clear error queue send to host thread
    errorQueue_.Clear(hostEnv_);
    if (!HostIsStop()) {
        // 3. set thisVar's nativepointer be null
        napi_value thisVar = NapiHelper::GetReferenceValue(hostEnv_, workerRef_);
        NewWorker* worker = nullptr;
        napi_remove_wrap(hostEnv_, thisVar, reinterpret_cast<void**>(&worker));
        // 4. set workerRef_ be null
        NapiHelper::DeleteReference(hostEnv_, workerRef_);
    }
    hostEnv_ = nullptr;
    workerRef_ = nullptr;
}

void NewWorker::WorkerOnErrorInner(napi_value error)
{
    napi_value argv[1] = { error };
    CallWorkerFunction(1, argv, "onerror", false);
}

bool NewWorker::CallWorkerFunction(size_t argc, const napi_value* argv, const char* methodName, bool tryCatch)
{
    if (workerEnv_ == nullptr) {
        HILOG_ERROR("Worker:: worker is not running when CallWorkerFunction %{public}s.", methodName);
        return false;
    }
    napi_value callback = NapiHelper::GetNamePropertyInParentPort(workerEnv_, workerPort_, methodName);
    bool isCallable = NapiHelper::IsCallable(workerEnv_, callback);
    if (!isCallable) {
        HILOG_ERROR("worker:: WorkerGlobalScope %{public}s is not Callable", methodName);
        return false;
    }
    napi_value undefinedValue = NapiHelper::GetUndefinedValue(workerEnv_);
    napi_value callbackResult = nullptr;
    napi_call_function(workerEnv_, undefinedValue, callback, argc, argv, &callbackResult);
    if (tryCatch && callbackResult == nullptr) {
        // handle exception
        HandleException();
        return false;
    }
    return true;
}

void NewWorker::CloseWorkerCallback()
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

void NewWorker::ReleaseWorkerThreadContent()
{
    HITRACE_METER_NAME(HITRACE_TAG_COMMONLIBRARY, __PRETTY_FUNCTION__);
    auto hostEngine = reinterpret_cast<NativeEngine*>(hostEnv_);
    auto workerEngine = reinterpret_cast<NativeEngine*>(workerEnv_);
    if (hostEngine != nullptr && workerEngine != nullptr) {
        hostEngine->DeleteWorker(hostEngine, workerEngine);
    }
    // 1. remove worker instance count
    {
        std::lock_guard<std::mutex> lock(g_newWorkersMutex);
        std::list<NewWorker*>::iterator it = std::find(g_newWorkers.begin(), g_newWorkers.end(), this);
        if (it != g_newWorkers.end()) {
            g_newWorkers.erase(it);
        }
    }

    ParentPortRemoveAllListenerInner();

    // 2. delete worker's parentPort
    NapiHelper::DeleteReference(workerEnv_, workerPort_);
    workerPort_ = nullptr;

    // 3. clear message send to worker thread
    workerMessageQueue_.Clear(workerEnv_);
    // 4. delete NativeEngine created in worker thread
    reinterpret_cast<NativeEngine*>(workerEnv_)->DeleteEngine();
    CloseHelp::DeletePointer(reinterpret_cast<NativeEngine*>(workerEnv_), false);
    workerEnv_ = nullptr;
}

void NewWorker::ParentPortAddListenerInner(napi_env env, const char* type, const WorkerListener* listener)
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
            listenerList.begin(), listenerList.end(), NewWorker::FindWorkerListener(env, listener->callback_));
        if (it != listenerList.end()) {
            return;
        }
        listenerList.emplace_back(const_cast<WorkerListener*>(listener));
    }
}

void NewWorker::ParentPortRemoveAllListenerInner()
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

void NewWorker::ParentPortRemoveListenerInner(napi_env env, const char* type, napi_ref callback)
{
    std::string typestr(type);
    auto iter = parentPortEventListeners_.find(typestr);
    if (iter == parentPortEventListeners_.end()) {
        return;
    }
    std::list<WorkerListener*>& listenerList = iter->second;
    if (callback != nullptr) {
        std::list<WorkerListener*>::iterator it =
            std::find_if(listenerList.begin(), listenerList.end(), NewWorker::FindWorkerListener(env, callback));
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

void NewWorker::ParentPortHandleEventListeners(napi_env env, napi_value recv,
                                               size_t argc, const napi_value* argv, const char* type)
{
    std::string listener(type);
    auto iter = parentPortEventListeners_.find(listener);
    if (iter == parentPortEventListeners_.end()) {
        HILOG_INFO("worker:: there is no listener for type %{public}s", type);
        return;
    }

    std::list<WorkerListener*>& listeners = iter->second;
    std::list<WorkerListener*>::iterator it = listeners.begin();
    while (it != listeners.end()) {
        WorkerListener* data = *it++;
        napi_value callbackObj = NapiHelper::GetReferenceValue(env, data->callback_);
        napi_value callbackResult = nullptr;
        napi_call_function(env, recv, callbackObj, argc, argv, &callbackResult);
        if (!data->NextIsAvailable()) {
            listeners.remove(data);
            CloseHelp::DeletePointer(data, false);
        }
    }
}

#if !defined(WINDOWS_PLATFORM) && !defined(MAC_PLATFORM)
void NewWorker::HandleDebuggerTask(const uv_async_t* req)
{
    NewWorker* worker = DereferenceHelp::DereferenceOf(&NewWorker::ddebuggerOnPostTaskSignal_, req);
    if (worker == nullptr) {
        HILOG_ERROR("worker::worker is null");
        return;
    }

    worker->debuggerTask_();
}

void NewWorker::DebuggerOnPostTask(std::function<void()>&& task)
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
