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

#include <thread>
#include <uv.h>

#include "ark_native_engine.h"
#include "helper/error_helper.h"
#include "message_queue.h"
#include "test.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "tools/log.h"
#include "worker.h"

#define SELLP_MS 200

#define ASSERT_CHECK_CALL(call)   \
    {                             \
        ASSERT_EQ(call, napi_ok); \
    }

#define ASSERT_CHECK_VALUE_TYPE(env, value, type)               \
    {                                                           \
        napi_valuetype valueType = napi_undefined;              \
        ASSERT_TRUE(value != nullptr);                          \
        ASSERT_CHECK_CALL(napi_typeof(env, value, &valueType)); \
        ASSERT_EQ(valueType, type);                             \
    }

using namespace Commonlibrary::Concurrent::WorkerModule;

namespace Commonlibrary::Concurrent::WorkerModule {

static constexpr uint32_t GLOBAL_CALL_ID_MAX = 4294967295;
static constexpr size_t GLOBAL_CALL_MAX_COUNT = 65535;

class WorkersTest : public testing::Test {
public:
    static void SetUpTestSuite()
    {
        InitializeEngine();
    }

    static void TearDownTestSuite()
    {
        DestroyEngine();
    }

    static void InitializeEngine()
    {
        panda::RuntimeOption option;
        option.SetGcType(panda::RuntimeOption::GC_TYPE::GEN_GC);
        const int64_t poolSize = 0x1000000;  // 16M
        option.SetGcPoolSize(poolSize);
        option.SetLogLevel(panda::RuntimeOption::LOG_LEVEL::ERROR);
        option.SetDebuggerLibraryPath("");
        vm_ = panda::JSNApi::CreateJSVM(option);
        ASSERT_TRUE(vm_ != nullptr);
        engine_ = new ArkNativeEngine(vm_, nullptr);
        engine_->SetInitWorkerFunc([](NativeEngine*) {});
    }

    static void DestroyEngine()
    {
        if (engine_) {
            delete engine_;
            engine_ = nullptr;
        }
        panda::JSNApi::DestroyJSVM(vm_);
    }

    static napi_env GetEnv()
    {
        return reinterpret_cast<napi_env>(engine_);
    }

    static void WorkerOnMessage(const uv_async_t *req)
    {
        Worker *worker = static_cast<Worker*>(req->data);
        ASSERT_NE(worker, nullptr);
        napi_env workerEnv = worker->GetWorkerEnv();
        void *data = nullptr;
        while (worker->workerMessageQueue_.DeQueue(&data)) {
            if (data == nullptr) {
                return;
            }
            napi_value result = nullptr;
            napi_status status = napi_deserialize(workerEnv, data, &result);
            ASSERT_TRUE(status == napi_ok);
            uint32_t number = 0;
            status = napi_get_value_uint32(workerEnv, result, &number);
            ASSERT_TRUE(status == napi_ok);
            ASSERT_TRUE(number != 0);
            napi_delete_serialization_data(workerEnv, data);
            number = 1000; // 1000 : test number
            napi_value numVal = nullptr;
            status = napi_create_uint32(workerEnv, number, &numVal);
            ASSERT_TRUE(status == napi_ok);
            napi_value undefined = nullptr;
            napi_get_undefined(workerEnv, &undefined);
            void *workerData = nullptr;
            status = napi_serialize_inner(workerEnv, numVal, undefined, undefined, false, true, &workerData);
            ASSERT_TRUE(status == napi_ok);
            ASSERT_NE(workerData, nullptr);
            worker->PostMessageToHostInner(workerData);
        }
    }

    static void HostOnMessage(const uv_async_t *req)
    {
        Worker *worker = static_cast<Worker*>(req->data);
        ASSERT_NE(worker, nullptr);
        void *data = nullptr;
        while (worker->hostMessageQueue_.DeQueue(&data)) {
            if (data == nullptr) {
                return;
            }
            napi_env hostEnv = worker->GetHostEnv();
            napi_value result = nullptr;
            napi_status status = napi_deserialize(hostEnv, data, &result);
            ASSERT_TRUE(status == napi_ok);
            uint32_t number = 0;
            status = napi_get_value_uint32(hostEnv, result, &number);
            ASSERT_TRUE(status == napi_ok);
            ASSERT_EQ(number, 1000); // 1000 : test number
            napi_delete_serialization_data(hostEnv, data);
        }
    }

    static void WorkerThreadFunction(void *data)
    {
        auto worker = reinterpret_cast<Worker*>(data);
        napi_env hostEnv = worker->GetHostEnv();
        ASSERT_NE(hostEnv, nullptr);
        napi_env workerEnv  = nullptr;
        napi_status status = napi_create_runtime(hostEnv, &workerEnv);
        ASSERT_TRUE(status == napi_ok);
        worker->SetWorkerEnv(workerEnv);
        uv_loop_t *workerLoop = nullptr;
        status = napi_get_uv_event_loop(workerEnv, &workerLoop);
        ASSERT_TRUE(status == napi_ok);
        worker->workerOnMessageSignal_ = new uv_async_t;
        uv_async_init(workerLoop, worker->workerOnMessageSignal_, reinterpret_cast<uv_async_cb>(WorkerOnMessage));
        worker->workerOnMessageSignal_->data = worker;
        worker->Loop();
    }

    static void UpdateMainThreadWorkerFlag(Worker *worker, bool isMainThreadWorker)
    {
        worker->isMainThreadWorker_ = isMainThreadWorker;
    }

    static void InitHostHandle(Worker *worker, uv_loop_t *loop)
    {
        worker->hostOnMessageSignal_ = new uv_async_t;
        uv_async_init(loop, worker->hostOnMessageSignal_, reinterpret_cast<uv_async_cb>(HostOnMessage));
        worker->hostOnMessageSignal_->data = worker;
    }

    static void PostMessage(Worker *worker, void *message)
    {
        worker->PostMessageInner(message);
    }

    static void UpdateWorkerState(Worker *worker, Worker::RunnerState state)
    {
        bool done = false;
        do {
            Worker::RunnerState oldState = worker->runnerState_.load(std::memory_order_acquire);
            done = worker->runnerState_.compare_exchange_strong(oldState, state);
        } while (!done);
    }

    static void SetCloseWorkerProp(Worker *worker, napi_env env)
    {
        worker->SetWorkerEnv(env);
        uv_loop_t* loop = worker->GetWorkerLoop();
        ASSERT_TRUE(loop != nullptr);
        worker->workerOnMessageSignal_ = new uv_async_t;
        uv_async_init(loop, worker->workerOnMessageSignal_, reinterpret_cast<uv_async_cb>(
            UpdateMainThreadWorkerFlag));
        worker->workerOnMessageSignal_->data = worker;
        uv_async_init(loop, &worker->debuggerOnPostTaskSignal_, reinterpret_cast<uv_async_cb>(
            UpdateWorkerState));
    }

    static void SetWorkerRef(Worker *worker, napi_env env)
    {
        std::string funcName = "onmessage";
        auto func = [](napi_env env, napi_callback_info info) -> napi_value {
            return nullptr;
        };
        napi_value funcValue = nullptr;
        napi_create_function(env, funcName.c_str(), NAPI_AUTO_LENGTH, func, nullptr, &funcValue);
        napi_value obj = NapiHelper::CreateObject(env);
        napi_set_named_property(env, obj, funcName.c_str(), funcValue);
        napi_ref ref = NapiHelper::CreateReference(env, obj, 1);
        worker->workerRef_ = ref;
    }

    static void SetWorkerHostEnv(Worker *worker, napi_env env, bool setNull)
    {
        if (setNull) {
            worker->hostEnv_ = nullptr;
            return;
        }
        worker->hostEnv_ = env;
    }

    static void UpdateHostState(Worker *worker, Worker::HostState state)
    {
        bool done = false;
        do {
            Worker::HostState oldState = worker->hostState_.load(std::memory_order_acquire);
            done = worker->hostState_.compare_exchange_strong(oldState, state);
        } while (!done);
    }

    static void SetMainThread(Worker *worker, bool isMainThreadWorker)
    {
        worker->isMainThreadWorker_ = isMainThreadWorker;
    }

    static void SetLimitedWorker(Worker *worker, bool isLimitedWorker)
    {
        worker->isLimitedWorker_ = isLimitedWorker;
    }

    static void RemoveGlobalCallObject(Worker *worker, napi_env env)
    {
        worker->globalCallId_ = GLOBAL_CALL_ID_MAX;
        worker->IncreaseGlobalCallId();
        ASSERT_TRUE(worker->globalCallId_ == 1);
        napi_value obj = NapiHelper::CreateObject(env);
        napi_ref ref = NapiHelper::CreateReference(env, obj, 1);
        std::string instanceName = "host";
        std::string messageStr = "message";
        worker->AddGlobalCallObject(instanceName, ref);
        ASSERT_FALSE(worker->RemoveGlobalCallObject(messageStr));
        ASSERT_TRUE(worker->RemoveGlobalCallObject(instanceName));
        napi_env workerEnv = nullptr;
        napi_create_runtime(env, &workerEnv);
        worker->workerEnv_ = workerEnv;
        worker->isRelativePath_ = true;
        worker->PrepareForWorkerInstance();
        UpdateHostState(worker, Worker::HostState::INACTIVE);
        worker->PrepareForWorkerInstance();
        worker->EraseWorker();
        ClearWorkerHandle(worker);
        Worker::ExecuteInThread(reinterpret_cast<void*>(worker));
    }

    static void TestHostOnMessageInner(Worker *worker, napi_env env)
    {
        uv_async_t* req = new uv_async_t;
        req->data = nullptr;
        Worker::HostOnMessage(req);
        delete req;
        worker->hostEnv_ = nullptr;
        worker->HostOnMessageInner();
        worker->hostEnv_ = env;
        UpdateHostState(worker, Worker::HostState::INACTIVE);
        worker->HostOnMessageInner();
        worker->UpdateHostState(Worker::HostState::ACTIVE);
        UpdateHostState(worker, Worker::HostState::ACTIVE);
        worker->hostMessageQueue_.Clear(env);
        worker->HostOnMessageInner();
        MessageDataType data1 = nullptr;
        napi_value undefined = NapiHelper::GetUndefinedValue(env);
        napi_serialize_inner(env, undefined, undefined,
                             undefined, false, true, &data1);
        worker->hostMessageQueue_.EnQueue(data1);
        SetWorkerRef(worker, env);
        worker->HostOnMessageInner();
        MessageDataType data2 = nullptr;
        napi_serialize_inner(env, undefined, undefined,
                             undefined, false, true, &data2);
        worker->hostMessageQueue_.EnQueue(data2);
        std::string funcName = "onmessage";
        napi_value funcValue = nullptr;
        napi_create_string_utf8(env, funcName.c_str(), funcName.length(), &funcValue);
        napi_value obj = NapiHelper::CreateObject(env);
        napi_set_named_property(env, obj, funcName.c_str(), funcValue);
        napi_ref ref = NapiHelper::CreateReference(env, obj, 1);
        worker->workerRef_ = ref;
        worker->HostOnMessageInner();
    }

    static void HostOnGlobalCall(Worker *worker, napi_env env)
    {
        uv_async_t* req = new uv_async_t;
        req->data = nullptr;
        Worker::HostOnGlobalCall(req);
        delete req;
        req->data = worker;
        worker->hostEnv_ = nullptr;
        worker->HostOnGlobalCallInner();
        worker->hostEnv_ = env;
        UpdateHostState(worker, Worker::HostState::INACTIVE);
        worker->HostOnGlobalCallInner();
        UpdateHostState(worker, Worker::HostState::ACTIVE);
        worker->HostOnGlobalCallInner();
        napi_value undefined = NapiHelper::GetUndefinedValue(env);
        for (size_t index = 0; index <= GLOBAL_CALL_MAX_COUNT; ++index) {
            MessageDataType data = nullptr;
            napi_serialize_inner(env, undefined, undefined,
                                 undefined, false, true, &data);
            worker->hostGlobalCallQueue_.Push(index, data);
        }
        worker->HostOnGlobalCallInner();
        worker->hostGlobalCallQueue_.Clear(env);
        size_t argc = 2;
        napi_value args[2] = { nullptr };
        std::string instanceName = "workerInstance";
        std::string methodName = "onmessage";
        napi_create_string_utf8(env, instanceName.c_str(), instanceName.length(), &args[0]);
        napi_create_string_utf8(env, methodName.c_str(), methodName.length(), &args[1]);
        MessageDataType data = nullptr;
        napi_value argsArray;
        napi_create_array_with_length(env, argc, &argsArray);
        size_t index = 0;
        for (size_t i = 0; i < argc; i++) {
            napi_set_element(env, argsArray, index, args[i]);
            index++;
        }
        napi_serialize_inner(env, argsArray, undefined, undefined, false, true, &data);
        worker->hostGlobalCallQueue_.Push(1, data);
        worker->globalCallId_ = 1;
        worker->HostOnGlobalCallInner();
    }

    static void HostOnGlobalCallInner001(Worker *worker, napi_env env)
    {
        napi_value undefined = NapiHelper::GetUndefinedValue(env);
        size_t argc = 2;
        napi_value args[2] = { nullptr };
        std::string instanceName = "workerInstance";
        std::string methodName = "onmessage";
        napi_create_string_utf8(env, instanceName.c_str(), instanceName.length(), &args[0]);
        napi_create_string_utf8(env, methodName.c_str(), methodName.length(), &args[1]);
        MessageDataType data = nullptr;
        napi_value argsArray;
        napi_create_array_with_length(env, argc, &argsArray);
        size_t index = 0;
        for (size_t i = 0; i < argc; i++) {
            napi_set_element(env, argsArray, index, args[i]);
            index++;
        }
        napi_serialize_inner(env, argsArray, undefined, undefined, false, true, &data);
        worker->hostGlobalCallQueue_.Push(1, data);
        worker->globalCallId_ = 1;
        napi_value obj = NapiHelper::CreateObject(env);
        napi_ref ref = NapiHelper::CreateReference(env, obj, 1);
        worker->AddGlobalCallObject(instanceName, ref);
        worker->HostOnGlobalCallInner();
    }

    static void HostOnGlobalCallInner002(Worker *worker, napi_env env)
    {
        napi_value undefined = NapiHelper::GetUndefinedValue(env);
        size_t argc = 2;
        napi_value args[2] = { nullptr };
        std::string instanceName = "workerInstance";
        std::string methodName = "onmessage";
        napi_create_string_utf8(env, instanceName.c_str(), instanceName.length(), &args[0]);
        napi_create_string_utf8(env, methodName.c_str(), methodName.length(), &args[1]);
        MessageDataType data = nullptr;
        napi_value argsArray;
        napi_create_array_with_length(env, argc, &argsArray);
        size_t index = 0;
        for (size_t i = 0; i < argc; i++) {
            napi_set_element(env, argsArray, index, args[i]);
            index++;
        }
        napi_serialize_inner(env, argsArray, undefined, undefined, false, true, &data);
        worker->hostGlobalCallQueue_.Push(1, data);
        worker->globalCallId_ = 1;
        napi_value obj = NapiHelper::CreateObject(env);
        napi_value methodValue;
        napi_create_string_utf8(env, methodName.c_str(), methodName.length(), &methodValue);
        napi_set_named_property(env, obj, methodName.c_str(), methodValue);
        napi_ref ref = NapiHelper::CreateReference(env, obj, 1);
        worker->AddGlobalCallObject(instanceName, ref);
        worker->HostOnGlobalCallInner();
    }

    static void HostOnGlobalCallInner003(Worker *worker, napi_env env)
    {
        napi_value undefined = NapiHelper::GetUndefinedValue(env);
        size_t argc = 2;
        napi_value args[2] = { nullptr };
        std::string instanceName = "workerInstance";
        std::string methodName = "onmessage";
        napi_create_string_utf8(env, instanceName.c_str(), instanceName.length(), &args[0]);
        napi_create_string_utf8(env, methodName.c_str(), methodName.length(), &args[1]);
        MessageDataType data = nullptr;
        napi_value argsArray;
        napi_create_array_with_length(env, argc, &argsArray);
        size_t index = 0;
        for (size_t i = 0; i < argc; i++) {
            napi_set_element(env, argsArray, index, args[i]);
            index++;
        }
        napi_serialize_inner(env, argsArray, undefined, undefined, false, true, &data);
        worker->hostGlobalCallQueue_.Push(1, data);
        worker->globalCallId_ = 1;
        napi_value obj = NapiHelper::CreateObject(env);
        auto func = [](napi_env env, napi_callback_info info) -> napi_value {
            return nullptr;
        };
        napi_value funcValue = nullptr;
        napi_create_function(env, methodName.c_str(), NAPI_AUTO_LENGTH, func, nullptr, &funcValue);
        napi_set_named_property(env, obj, methodName.c_str(), funcValue);
        napi_ref ref = NapiHelper::CreateReference(env, obj, 1);
        worker->AddGlobalCallObject(instanceName, ref);
        worker->HostOnGlobalCallInner();
    }

    static void HostOnGlobalCallInner004(Worker *worker, napi_env env)
    {
        napi_value undefined = NapiHelper::GetUndefinedValue(env);
        size_t argc = 3;
        napi_value args[3] = { nullptr };
        std::string instanceName = "workerInstance";
        std::string methodName = "onmessage";
        std::string argValue = "test";
        napi_create_string_utf8(env, instanceName.c_str(), instanceName.length(), &args[0]);
        napi_create_string_utf8(env, methodName.c_str(), methodName.length(), &args[1]);
        napi_create_string_utf8(env, argValue.c_str(), argValue.length(), &args[2]); // 2: the index of test argument
        MessageDataType data = nullptr;
        napi_value argsArray;
        napi_create_array_with_length(env, argc, &argsArray);
        size_t index = 0;
        for (size_t i = 0; i < argc; i++) {
            napi_set_element(env, argsArray, index, args[i]);
            index++;
        }
        napi_serialize_inner(env, argsArray, undefined, undefined, false, true, &data);
        worker->hostGlobalCallQueue_.Push(1, data);
        worker->globalCallId_ = 1;
        napi_value obj = NapiHelper::CreateObject(env);
        auto func = [](napi_env env, napi_callback_info info) -> napi_value {
            return nullptr;
        };
        napi_value funcValue = nullptr;
        napi_create_function(env, methodName.c_str(), NAPI_AUTO_LENGTH, func, nullptr, &funcValue);
        napi_set_named_property(env, obj, methodName.c_str(), funcValue);
        napi_ref ref = NapiHelper::CreateReference(env, obj, 1);
        worker->AddGlobalCallObject(instanceName, ref);
        Worker::LimitedWorkerConstructor(env, nullptr);
        worker->HostOnGlobalCallInner();
    }

    static void HostOnGlobalCallInner005(Worker *worker, napi_env env)
    {
        napi_value undefined = NapiHelper::GetUndefinedValue(env);
        size_t argc = 2;
        napi_value args[3] = { nullptr };
        std::string instanceName = "workerInstance";
        std::string methodName = "onmessage";
        napi_create_string_utf8(env, instanceName.c_str(), instanceName.length(), &args[0]);
        napi_create_string_utf8(env, methodName.c_str(), methodName.length(), &args[1]);
        MessageDataType data = nullptr;
        napi_value argsArray;
        napi_create_array_with_length(env, argc, &argsArray);
        size_t index = 0;
        for (size_t i = 0; i < argc; i++) {
            napi_set_element(env, argsArray, index, args[i]);
            index++;
        }
        napi_serialize_inner(env, argsArray, undefined, undefined, false, true, &data);
        worker->hostGlobalCallQueue_.Push(0, data);
        worker->globalCallId_ = 0;
        napi_value obj = NapiHelper::CreateObject(env);
        auto func = [](napi_env env, napi_callback_info info) -> napi_value {
            return NapiHelper::GetUndefinedValue(env);
        };
        napi_value funcValue = nullptr;
        napi_create_function(env, methodName.c_str(), NAPI_AUTO_LENGTH, func, nullptr, &funcValue);
        napi_set_named_property(env, obj, methodName.c_str(), funcValue);
        napi_ref ref = NapiHelper::CreateReference(env, obj, 1);
        worker->AddGlobalCallObject(instanceName, ref);
        worker->HostOnGlobalCallInner();
    }

    static void HostOnGlobalCallInner006(Worker *worker, napi_env env)
    {
        napi_value undefined = NapiHelper::GetUndefinedValue(env);
        size_t argc = 2;
        napi_value args[3] = { nullptr };
        std::string instanceName = "workerInstance";
        std::string methodName = "onmessage";
        napi_create_string_utf8(env, instanceName.c_str(), instanceName.length(), &args[0]);
        napi_create_string_utf8(env, methodName.c_str(), methodName.length(), &args[1]);
        MessageDataType data = nullptr;
        napi_value argsArray;
        napi_create_array_with_length(env, argc, &argsArray);
        size_t index = 0;
        for (size_t i = 0; i < argc; i++) {
            napi_set_element(env, argsArray, index, args[i]);
            index++;
        }
        napi_serialize_inner(env, argsArray, undefined, undefined, false, true, &data);
        worker->hostGlobalCallQueue_.Push(1, data);
        worker->globalCallId_ = 1;
        napi_value obj = NapiHelper::CreateObject(env);
        auto func = [](napi_env env, napi_callback_info info) -> napi_value {
            return NapiHelper::GetUndefinedValue(env);
        };
        napi_value funcValue = nullptr;
        napi_create_function(env, methodName.c_str(), NAPI_AUTO_LENGTH, func, nullptr, &funcValue);
        napi_set_named_property(env, obj, methodName.c_str(), funcValue);
        napi_ref ref = NapiHelper::CreateReference(env, obj, 1);
        worker->AddGlobalCallObject(instanceName, ref);
        worker->HostOnGlobalCallInner();
    }

    static void HandleGlobalCall(Worker *worker, napi_env env)
    {
        worker->AddGlobalCallError(ErrorHelper::ERR_WORKER_SERIALIZATION);
        worker->HandleGlobalCallError(env);
        napi_value exception = nullptr;
        napi_get_and_clear_last_exception(env, &exception);
        ASSERT_TRUE(exception != nullptr);
        napi_value errorData = nullptr;
        napi_value undefined = NapiHelper::GetUndefinedValue(env);
        napi_serialize_inner(env, undefined, undefined, undefined,
                             false, true, reinterpret_cast<void**>(&errorData));
        worker->AddGlobalCallError(ErrorHelper::ERR_WORKER_SERIALIZATION, errorData);
        worker->AddGlobalCallError(ErrorHelper::ERR_CALL_METHOD_ON_BINDING_OBJ);
        worker->ClearGlobalCallError(env);
        worker->hostEnv_ = nullptr;
        worker->CallHostFunction(0, nullptr, "onmessageerror");
        worker->hostEnv_ = env;
        UpdateHostState(worker, Worker::HostState::INACTIVE);
        worker->CallHostFunction(0, nullptr, "onmessageerror");
        UpdateHostState(worker, Worker::HostState::ACTIVE);
        std::string funcName = "onmessage";
        SetWorkerRef(worker, env);
        napi_value argv[1] = { nullptr };
        napi_create_string_utf8(env, funcName.c_str(), funcName.length(), &argv[0]);
        worker->CallHostFunction(1, argv, "onmessage");
    }

    static void HostOnError(Worker *worker, napi_env env)
    {
        worker->hostEnv_ = nullptr;
        worker->HostOnErrorInner();
        worker->HostOnMessageErrorInner();
        worker->hostEnv_ = env;
        UpdateHostState(worker, Worker::HostState::INACTIVE);
        worker->HostOnErrorInner();
        worker->HostOnMessageErrorInner();
        UpdateHostState(worker, Worker::HostState::ACTIVE);
        std::string funcName = "onerror";
        napi_value funcValue = nullptr;
        napi_create_string_utf8(env, funcName.c_str(), funcName.length(), &funcValue);
        napi_value obj = NapiHelper::CreateObject(env);
        napi_set_named_property(env, obj, funcName.c_str(), funcValue);
        napi_ref ref = NapiHelper::CreateReference(env, obj, 1);
        worker->workerRef_ = ref;
        MessageDataType data = nullptr;
        napi_value undefined = NapiHelper::GetUndefinedValue(env);
        napi_serialize_inner(env, undefined, undefined, undefined, false, true, &data);
        worker->errorQueue_.EnQueue(data);
        worker->HostOnErrorInner();
    }

    static void HostOnErrorInner(Worker *worker, napi_env env)
    {
        std::string funcName = "onerror";
        auto func = [](napi_env env, napi_callback_info info) -> napi_value {
            return nullptr;
        };
        napi_value funcValue = nullptr;
        napi_create_function(env, funcName.c_str(), NAPI_AUTO_LENGTH, func, nullptr, &funcValue);
        napi_value obj = NapiHelper::CreateObject(env);
        napi_set_named_property(env, obj, funcName.c_str(), funcValue);
        napi_ref ref = NapiHelper::CreateReference(env, obj, 1);
        worker->workerRef_ = ref;
        MessageDataType data = nullptr;
        napi_value undefined = NapiHelper::GetUndefinedValue(env);
        napi_serialize_inner(env, undefined, undefined, undefined, false, true, &data);
        worker->errorQueue_.EnQueue(data);
        worker->HostOnErrorInner();
        UpdateWorkerState(worker, Worker::RunnerState::TERMINATED);
        worker->PostMessageInner(nullptr);
        UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
        worker->workerOnMessageSignal_ = nullptr;
        worker->PostMessageInner(nullptr);
    }

    static void HostOnMessageErrorInner(Worker *worker, napi_env env)
    {
        SetWorkerRef(worker, env);
        worker->HostOnMessageErrorInner();
        UpdateWorkerState(worker, Worker::RunnerState::TERMINATED);
        worker->TerminateInner();
        UpdateWorkerState(worker, Worker::RunnerState::TERMINATEING);
        worker->TerminateInner();
        worker->isTerminated_ = true;
        worker->CloseInner();
    }

    static void PublishWorkerOverSignal(Worker *worker, napi_env env)
    {
        UpdateHostState(worker, Worker::HostState::INACTIVE);
        worker->PublishWorkerOverSignal();
        UpdateHostState(worker, Worker::HostState::ACTIVE);
        uv_loop_t *loop = nullptr;
        napi_status status = napi_get_uv_event_loop(env, &loop);
        ASSERT_TRUE(status == napi_ok);
        InitHostHandle(worker, loop);
        worker->isMainThreadWorker_ = false;
        worker->PublishWorkerOverSignal();
        worker->isMainThreadWorker_ = true;
        worker->isLimitedWorker_ = true;
        worker->PublishWorkerOverSignal();
        worker->isMainThreadWorker_ = true;
        worker->isLimitedWorker_ = false;
        worker->PublishWorkerOverSignal();
        worker->PostWorkerErrorTask();
        worker->PostWorkerMessageTask();
        worker->PostWorkerGlobalCallTask();
        worker->EraseWorker();
        worker->PostWorkerOverTask();
        worker->PostWorkerErrorTask();
        worker->PostWorkerMessageTask();
        worker->PostWorkerGlobalCallTask();
    }

    static void TestWorkerOnMessageInner(Worker *worker, napi_env env)
    {
        uv_async_t* req = new uv_async_t;
        req->data = nullptr;
        Worker::WorkerOnMessage(req);
        delete req;
        UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
        worker->workerEnv_ = nullptr;
        worker->WorkerOnMessageInner();
        napi_env workerEnv = nullptr;
        napi_create_runtime(env, &workerEnv);
        worker->workerEnv_ = workerEnv;
        worker->WorkerOnMessageInner();
        MessageDataType data = nullptr;
        napi_value undefined = NapiHelper::GetUndefinedValue(env);
        napi_serialize_inner(env, undefined, undefined, undefined, false, true, &data);
        worker->workerMessageQueue_.EnQueue(data);
        std::string funcName = "onmessage";
        auto func = [](napi_env env, napi_callback_info info) -> napi_value {
            return nullptr;
        };
        napi_value funcValue = nullptr;
        napi_create_function(workerEnv, funcName.c_str(), NAPI_AUTO_LENGTH, func, nullptr, &funcValue);
        napi_value obj = NapiHelper::CreateObject(workerEnv);
        napi_set_named_property(workerEnv, obj, funcName.c_str(), funcValue);
        napi_ref ref = NapiHelper::CreateReference(workerEnv, obj, 1);
        worker->workerPort_ = ref;
        worker->WorkerOnMessageInner();
        SetCloseWorkerProp(worker, workerEnv);
        worker->workerMessageQueue_.EnQueue(nullptr);
        worker->WorkerOnMessageInner();
    }

    static void HandleEventListeners(Worker *worker, napi_env env)
    {
        napi_value obj = NapiHelper::CreateObject(env);
        napi_ref callback = NapiHelper::CreateReference(env, obj, 1);
        auto listener = new Worker::WorkerListener(env, callback, Worker::ListenerMode::PERMANENT);
        std::string funcName = "onmessage";
        worker->AddListenerInner(env, "onmessage", listener);
        worker->HandleEventListeners(env, nullptr, 0, nullptr, "onmessage");
        worker->eventListeners_.clear();
        auto func = [](napi_env env, napi_callback_info info) -> napi_value {
            return nullptr;
        };
        napi_value obj2 = NapiHelper::CreateObject(env);
        napi_value funcValue = nullptr;
        napi_create_function(env, funcName.c_str(), NAPI_AUTO_LENGTH, func, nullptr, &funcValue);
        napi_ref callback2 = NapiHelper::CreateReference(env, funcValue, 1);
        auto listener2 = new Worker::WorkerListener(env, callback2, Worker::ListenerMode::ONCE);
        worker->AddListenerInner(env, "onmessage", listener2);
        worker->HandleEventListeners(env, obj2, 0, nullptr, "onmessage");
        worker->eventListeners_.clear();
    }

    static void HandleHostException(Worker *worker, napi_env env)
    {
        worker->HandleHostException();
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_WORKER_NOT_RUNNING, "Worker create runtime error");
        worker->HandleHostException();
        napi_value exception = nullptr;
        napi_get_and_clear_last_exception(env, &exception);
        ASSERT_TRUE(exception != nullptr);
        napi_env workerEnv = nullptr;
        napi_create_runtime(env, &workerEnv);
        worker->workerEnv_ = workerEnv;
        worker->HandleException();
        ErrorHelper::ThrowError(workerEnv, ErrorHelper::ERR_WORKER_NOT_RUNNING, "Worker create runtime error");
        std::string funcName = "onerror";
        auto func = [](napi_env env, napi_callback_info info) -> napi_value {
            return nullptr;
        };
        napi_value funcValue = nullptr;
        napi_create_function(workerEnv, funcName.c_str(), NAPI_AUTO_LENGTH, func, nullptr, &funcValue);
        napi_value obj = NapiHelper::CreateObject(workerEnv);
        napi_set_named_property(workerEnv, obj, funcName.c_str(), funcValue);
        napi_ref ref = NapiHelper::CreateReference(workerEnv, obj, 1);
        worker->workerPort_ = ref;
        uv_loop_t* loop = worker->GetWorkerLoop();
        ASSERT_TRUE(loop != nullptr);
        worker->hostOnErrorSignal_ = new uv_async_t;
        uv_async_init(loop, worker->hostOnErrorSignal_,
                      reinterpret_cast<uv_async_cb>(UpdateMainThreadWorkerFlag));
        worker->hostOnErrorSignal_->data = worker;
        worker->isMainThreadWorker_ = false;
        worker->HandleException();
        worker->isMainThreadWorker_ = true;
        worker->isLimitedWorker_ = true;
        ErrorHelper::ThrowError(workerEnv, ErrorHelper::ERR_WORKER_NOT_RUNNING, "Worker create runtime error");
        worker->HandleException();
        worker->isLimitedWorker_ = false;
        ErrorHelper::ThrowError(workerEnv, ErrorHelper::ERR_WORKER_NOT_RUNNING, "Worker create runtime error");
        worker->HandleException();
        ErrorHelper::ThrowError(workerEnv, ErrorHelper::ERR_WORKER_NOT_RUNNING, "Worker create runtime error");
        napi_get_and_clear_last_exception(workerEnv, &exception);
        worker->hostEnv_ = nullptr;
        worker->HandleUncaughtException(exception);
        worker->hostEnv_ = env;
        UpdateHostState(worker, Worker::HostState::INACTIVE);
        worker->HandleUncaughtException(exception);
    }

    static void PostMessageToHostInner(Worker *worker, napi_env env)
    {
        worker->hostEnv_ = nullptr;
        worker->PostMessageToHostInner(nullptr);
        worker->hostEnv_ = env;
        UpdateHostState(worker, Worker::HostState::INACTIVE);
        worker->PostMessageToHostInner(nullptr);
        UpdateHostState(worker, Worker::HostState::ACTIVE);
        uv_loop_t *loop = nullptr;
        napi_status status = napi_get_uv_event_loop(env, &loop);
        ASSERT_TRUE(status == napi_ok);
        InitHostHandle(worker, loop);
        worker->isMainThreadWorker_ = false;
        worker->PostMessageToHostInner(nullptr);
        worker->isMainThreadWorker_ = true;
        worker->isLimitedWorker_ = true;
        worker->PostMessageToHostInner(nullptr);
        worker->isMainThreadWorker_ = true;
        worker->isLimitedWorker_ = false;
        worker->PostMessageToHostInner(nullptr);
    }

    static void RemoveListenerInner(Worker *worker, napi_env env)
    {
        worker->RemoveListenerInner(env, "onmessage", nullptr);
        std::string funcName = "onmessage";
        auto func = [](napi_env env, napi_callback_info info) -> napi_value {
            return nullptr;
        };
        napi_value funcValue = nullptr;
        napi_create_function(env, funcName.c_str(), NAPI_AUTO_LENGTH, func, nullptr, &funcValue);
        napi_ref callback = NapiHelper::CreateReference(env, funcValue, 1);
        auto listener = new Worker::WorkerListener(env, callback, Worker::ListenerMode::PERMANENT);
        worker->AddListenerInner(env, "onmessage", listener);
        worker->RemoveListenerInner(env, "onmessage", nullptr);

        napi_env workerEnv = nullptr;
        napi_create_runtime(env, &workerEnv);
        worker->workerEnv_ = workerEnv;
        std::string errorFuncName = "onerror";
        auto errorFunc = [](napi_env env, napi_callback_info info) -> napi_value {
            return nullptr;
        };
        napi_value errorFuncValue = nullptr;
        napi_create_function(workerEnv, errorFuncName.c_str(), NAPI_AUTO_LENGTH,
                             errorFunc, nullptr, &errorFuncValue);
        napi_value obj = NapiHelper::CreateObject(workerEnv);
        napi_set_named_property(workerEnv, obj, "onerror", errorFuncValue);
        napi_ref ref = NapiHelper::CreateReference(workerEnv, obj, 1);
        worker->workerPort_ = ref;
        worker->CallWorkerFunction(0, nullptr, "onerror", true);
        worker->CallWorkerFunction(0, nullptr, "onerror", false);

        std::string undefinedFuncName = "OnMessageError";
        auto undefinedFunc = [](napi_env env, napi_callback_info info) -> napi_value {
            return NapiHelper::GetUndefinedValue(env);
        };
        napi_value undefinedFuncValue = nullptr;
        napi_create_function(workerEnv, undefinedFuncName.c_str(), NAPI_AUTO_LENGTH,
                             undefinedFunc, nullptr, &undefinedFuncValue);
        napi_value obj2 = NapiHelper::CreateObject(workerEnv);
        napi_set_named_property(workerEnv, obj2, "OnMessageError", undefinedFuncValue);
        napi_ref ref2 = NapiHelper::CreateReference(workerEnv, obj2, 1);
        worker->workerPort_ = ref2;
        worker->CallWorkerFunction(0, nullptr, "OnMessageError", true);
    }

    static void ParentPortAddListenerInnerTest(Worker *worker, napi_env env)
    {
        UpdateHostState(worker, Worker::HostState::INACTIVE);
        worker->CloseWorkerCallback();
        UpdateHostState(worker, Worker::HostState::ACTIVE);
        std::string funcName = "onmessage";
        auto func = [](napi_env env, napi_callback_info info) -> napi_value {
            return nullptr;
        };
        napi_value funcValue = nullptr;
        napi_create_function(env, funcName.c_str(), NAPI_AUTO_LENGTH, func, nullptr, &funcValue);
        napi_ref callback = NapiHelper::CreateReference(env, funcValue, 1);
        auto listener = new Worker::WorkerListener(env, callback, Worker::ListenerMode::PERMANENT);
        worker->ParentPortAddListenerInner(env, "onmessage", listener);
        worker->ParentPortAddListenerInner(env, "onmessage", listener);
        napi_value obj = NapiHelper::CreateObject(env);
        napi_ref ref = NapiHelper::CreateReference(env, obj, 1);
        auto listener2 = new Worker::WorkerListener(env, ref, Worker::ListenerMode::ONCE);
        worker->ParentPortAddListenerInner(env, "onmessage", listener2);
        worker->ParentPortRemoveAllListenerInner();
    }

    static void ParentPortRemoveListenerInnerTest(Worker *worker, napi_env env)
    {
        std::string funcName = "onmessage";
        auto func = [](napi_env env, napi_callback_info info) -> napi_value {
            return nullptr;
        };
        napi_value funcValue = nullptr;
        napi_create_function(env, funcName.c_str(), NAPI_AUTO_LENGTH, func, nullptr, &funcValue);
        napi_ref callback = NapiHelper::CreateReference(env, funcValue, 1);
        auto listener = new Worker::WorkerListener(env, callback, Worker::ListenerMode::PERMANENT);
        worker->ParentPortAddListenerInner(env, "onmessage", listener);

        std::string errorFuncName = "onerror";
        auto errorFunc = [](napi_env env, napi_callback_info info) -> napi_value {
            return nullptr;
        };
        napi_value errorFuncValue = nullptr;
        napi_create_function(env, errorFuncName.c_str(), NAPI_AUTO_LENGTH,
                             errorFunc, nullptr, &errorFuncValue);
        napi_ref callback2 = NapiHelper::CreateReference(env, errorFuncValue, 1);
        auto listener2 = new Worker::WorkerListener(env, callback2, Worker::ListenerMode::PERMANENT);
        worker->ParentPortAddListenerInner(env, "onerror", listener2);
        worker->ParentPortRemoveListenerInner(env, "OnMessageError", nullptr);

        std::string hostFuncName = "host";
        auto hostFunc = [](napi_env env, napi_callback_info info) -> napi_value {
            return nullptr;
        };
        napi_value hostFuncValue = nullptr;
        napi_create_function(env, hostFuncName.c_str(), NAPI_AUTO_LENGTH,
                             hostFunc, nullptr, &hostFuncValue);
        napi_ref callback3 = NapiHelper::CreateReference(env, hostFuncValue, 1);
        worker->ParentPortRemoveListenerInner(env, "onmessage", callback);
        worker->ParentPortRemoveListenerInner(env, "onmessage", callback3);
        worker->ParentPortRemoveListenerInner(env, "onerror", nullptr);
    }

    static void ParentPortHandleEventListeners(Worker *worker, napi_env env) //todo:
    {
        napi_value recv = NapiHelper::CreateObject(env);
        std::string funcName = "onmessage";
        auto func = [](napi_env env, napi_callback_info info) -> napi_value {
            return nullptr;
        };
        napi_value funcValue = nullptr;
        napi_create_function(env, funcName.c_str(), NAPI_AUTO_LENGTH, func, nullptr, &funcValue);
        napi_ref callback = NapiHelper::CreateReference(env, funcValue, 1);
        auto listener = new Worker::WorkerListener(env, callback, Worker::ListenerMode::ONCE);
        worker->ParentPortAddListenerInner(env, "onmessage", listener);
        worker->ParentPortHandleEventListeners(env, recv, 0, nullptr, "onmessage", true);

        std::string errorFuncName = "onerror";
        auto errorFunc = [](napi_env env, napi_callback_info info) -> napi_value {
            return nullptr;
        };
        napi_value errorFuncValue = nullptr;
        napi_create_function(env, errorFuncName.c_str(), NAPI_AUTO_LENGTH,
                             errorFunc, nullptr, &errorFuncValue);
        napi_ref callback2 = NapiHelper::CreateReference(env, errorFuncValue, 1);
        auto listener2 = new Worker::WorkerListener(env, callback2, Worker::ListenerMode::PERMANENT);
        worker->ParentPortAddListenerInner(env, "onerror", listener2);

        std::string hostFuncName = "host";
        napi_value hostFuncValue = nullptr;
        napi_create_string_utf8(env, hostFuncName.c_str(), hostFuncName.length(), &hostFuncValue);
        napi_ref callback3 = NapiHelper::CreateReference(env, hostFuncValue, 1);
        auto listener3 = new Worker::WorkerListener(env, callback3, Worker::ListenerMode::PERMANENT);
        worker->ParentPortAddListenerInner(env, "onerror", listener3);
        worker->ParentPortHandleEventListeners(env, recv, 0, nullptr, "onerror", false);
    }
    
    static void DebuggerOnPostTask(Worker* worker, napi_env env)
    {
        napi_env workerEnv = nullptr;
        napi_create_runtime(env, &workerEnv);
        auto workerEngine = reinterpret_cast<NativeEngine*>(workerEnv);
        workerEngine->CheckAndSetWorkerVersion(WorkerVersion::NONE, WorkerVersion::NEW);
        workerEngine->MarkWorkerThread();
        Worker::CanCreateWorker(workerEnv, WorkerVersion::NEW);
        Worker::WorkerThrowError(workerEnv, ErrorHelper::ERR_WORKER_NOT_RUNNING,
            "host thread maybe is over when CallHostFunction");
        napi_value exception = nullptr;
        napi_get_and_clear_last_exception(workerEnv, &exception);
        worker->SetWorkerEnv(workerEnv);
        uv_loop_t* loop = worker->GetWorkerLoop();
        ASSERT_TRUE(loop != nullptr);
        uv_async_init(loop, &worker->debuggerOnPostTaskSignal_, reinterpret_cast<uv_async_cb>(
            UpdateMainThreadWorkerFlag));
        worker->debuggerOnPostTaskSignal_.data = worker;
        std::function<void()> myTask = []() {
            return;
        };
        UpdateWorkerState(worker, Worker::RunnerState::TERMINATED);
        worker->DebuggerOnPostTask(std::move(myTask));
        UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
        worker->DebuggerOnPostTask(std::move(myTask));
    }

    static void CloseHostHandle(Worker* worker, napi_env env)
    {
        uv_loop_t *loop = nullptr;
        napi_status status = napi_get_uv_event_loop(env, &loop);
        ASSERT_TRUE(status == napi_ok);
        InitHostHandle(worker, loop);
        worker->hostOnErrorSignal_ = new uv_async_t;
        uv_async_init(loop, worker->hostOnErrorSignal_,
                      reinterpret_cast<uv_async_cb>(UpdateMainThreadWorkerFlag));
        worker->hostOnErrorSignal_->data = worker;
        worker->hostOnGlobalCallSignal_ = new uv_async_t;
        uv_async_init(loop, worker->hostOnGlobalCallSignal_,
                      reinterpret_cast<uv_async_cb>(UpdateMainThreadWorkerFlag));
        worker->hostOnGlobalCallSignal_->data = worker;
        worker->CloseHostHandle();
    }

    static void ClearWorkerHandle(Worker* worker)
    {
        worker->CloseHostHandle();
        if (worker->isLimitedWorker_) {
            napi_remove_env_cleanup_hook(worker->hostEnv_, Worker::LimitedWorkerHostEnvCleanCallback, worker);
        } else {
            napi_remove_env_cleanup_hook(worker->hostEnv_, Worker::WorkerHostEnvCleanCallback, worker);
        }
    }
protected:
    static thread_local NativeEngine *engine_;
    static thread_local EcmaVM *vm_;
};

thread_local NativeEngine *WorkersTest::engine_ = nullptr;
thread_local EcmaVM *WorkersTest::vm_ = nullptr;
}

// worker constructor
napi_value Worker_Constructor(napi_env env, napi_value global)
{
    std::string funcName = "WorkerConstructor";
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::WorkerConstructor, nullptr, &cb);

    napi_value result = nullptr;
    napi_value argv[2] = { nullptr };
    std::string script = "entry/ets/workers/@worker.ts";
    napi_create_string_utf8(env, script.c_str(), script.length(), &argv[0]);
    std::string type = "classic";
    std::string name = "WorkerThread";
    napi_value typeValue = nullptr;
    napi_value nameValue = nullptr;
    napi_create_string_utf8(env, name.c_str(), name.length(), &nameValue);
    napi_create_string_utf8(env, type.c_str(), type.length(), &typeValue);

    napi_value object = nullptr;
    napi_create_object(env, &object);

    napi_set_named_property(env, object, "name", nameValue);
    napi_set_named_property(env, object, "type", typeValue);
    argv[1]  = object;

    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    uv_sleep(SELLP_MS);
    napi_env newEnv = nullptr;
    napi_create_runtime(env, &newEnv);
    return result;
}

// worker terminate
napi_value Worker_Terminate(napi_env env, napi_value global)
{
    std::string funcName = "Terminate";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::Terminate, nullptr, &cb);
    napi_call_function(env, global, cb, 0, nullptr, &result);
    return result;
}

// worker WorkerConstructor
HWTEST_F(WorkersTest, WorkerConstructorTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);

    napi_value result = nullptr;
    result = Worker_Constructor(env, global);

    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    std::string nameResult = worker->GetName();
    ASSERT_EQ(nameResult, "WorkerThread");
    std::string scriptResult = worker->GetScript();
    ASSERT_EQ(scriptResult, "entry/ets/workers/@worker.ts");
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);

    ASSERT_TRUE(result != nullptr);
}

//worker PostMessage
HWTEST_F(WorkersTest, PostMessageTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);

    napi_value result = nullptr;
    result = Worker_Constructor(env, global);

    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));

    napi_value argv[1] = { nullptr };
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);
    std::string funcName = "PostMessage";
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::PostMessage, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

//worker PostMessage
HWTEST_F(WorkersTest, PostMessageTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);

    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));

    napi_value argv[1] = { nullptr };
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);

    std::string funcName = "PostMessage";
    napi_value cb = nullptr;

    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::PostMessage, nullptr, &cb);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);

    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

//worker PostMessage
HWTEST_F(WorkersTest, PostMessageTest003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);

    napi_value result = nullptr;
    result = Worker_Constructor(env, global);

    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    worker->UpdateWorkerState(Worker::RunnerState::TERMINATED);

    napi_value argv[1] = { nullptr };
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);
    std::string funcName = "PostMessage";
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::PostMessage, worker, &cb);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);

    uv_async_t* req = new uv_async_t;
    req->data = worker;
    Worker::WorkerOnMessage(req);

    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

//worker PostMessage
HWTEST_F(WorkersTest, PostMessageToHostTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);

    napi_value result = nullptr;
    result = Worker_Constructor(env, global);

    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    napi_value argv[1] = { nullptr };
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);

    std::string funcName = "PostMessageToHost";
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::PostMessageToHost, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    uv_async_t* req = new uv_async_t;
    req->data = worker;
    Worker::HostOnMessage(req);

    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

//worker PostMessageToHost
HWTEST_F(WorkersTest, PostMessageToHostTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);

    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));

    napi_value argv[1] = { nullptr };
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);

    std::string funcName = "PostMessageToHost";
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::PostMessageToHost, nullptr, &cb);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);

    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

//worker PostMessageToHost
HWTEST_F(WorkersTest, PostMessageToHostTest003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);

    napi_value arrayresult = nullptr;
    ASSERT_CHECK_CALL(napi_create_object(env, &arrayresult));
    ASSERT_CHECK_VALUE_TYPE(env, arrayresult, napi_object);
    const char testStr[] = "1234567";
    napi_value strAttribute = nullptr;
    ASSERT_CHECK_CALL(napi_create_string_utf8(env, testStr, strlen(testStr), &strAttribute));
    ASSERT_CHECK_VALUE_TYPE(env, strAttribute, napi_string);
    ASSERT_CHECK_CALL(napi_set_named_property(env, arrayresult, "strAttribute", strAttribute));

    napi_value retStrAttribute = nullptr;
    ASSERT_CHECK_CALL(napi_get_named_property(env, arrayresult, "strAttribute", &retStrAttribute));
    ASSERT_CHECK_VALUE_TYPE(env, retStrAttribute, napi_string);

    int32_t testNumber = 12345; // 12345 : indicates any number
    napi_value numberAttribute = nullptr;
    ASSERT_CHECK_CALL(napi_create_int32(env, testNumber, &numberAttribute));
    ASSERT_CHECK_VALUE_TYPE(env, numberAttribute, napi_number);
    ASSERT_CHECK_CALL(napi_set_named_property(env, arrayresult, "numberAttribute", numberAttribute));

    napi_value propNames = nullptr;
    ASSERT_CHECK_CALL(napi_get_property_names(env, arrayresult, &propNames));
    ASSERT_CHECK_VALUE_TYPE(env, propNames, napi_object);

    napi_value result = nullptr;
    result = Worker_Constructor(env, global);

    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    napi_value argv[2] = { nullptr };
    std::string message = "";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);
    std::string funcName = "PostMessageToHost";
    argv[1] = propNames;
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::PostMessageToHost, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::STARTING);
    worker->UpdateWorkerState(Worker::RunnerState::RUNNING);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

//worker EventListener
HWTEST_F(WorkersTest, EventListenerTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));

    napi_value argv[2] = {nullptr};
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);
    auto func = [](napi_env env, napi_callback_info info) -> napi_value {
        return nullptr;
    };
    napi_value funcValue = nullptr;
    napi_create_function(env, "testFunc", NAPI_AUTO_LENGTH, func, nullptr, &funcValue);
    argv[1] = funcValue;

    std::string funcName = "On";
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::On, worker, &cb);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);

    funcName = "Once";
    cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::Once, worker, &cb);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);

    funcName = "AddEventListener";
    cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::AddEventListener, worker, &cb);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);

    funcName = "RemoveEventListener";
    cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::RemoveEventListener, worker, &cb);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);

    funcName = "Off";
    cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::Off, worker, &cb);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);

    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

//worker EventListener
HWTEST_F(WorkersTest, EventListenerTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    napi_value argv[3] = {nullptr};
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);
    auto func = [](napi_env env, napi_callback_info info) -> napi_value {
        return nullptr;
    };
    napi_value funcValue = nullptr;
    napi_create_function(env, "testFunc", NAPI_AUTO_LENGTH, func, nullptr, &funcValue);
    argv[1] = funcValue;
    std::string funcName = "On";
    napi_value cb = nullptr;
    funcName = "Once";
    cb = nullptr;
    napi_value myobject = nullptr;
    napi_create_object(env, &myobject);

    argv[2] = myobject;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::Once, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

//worker DispatchEvent
HWTEST_F(WorkersTest, DispatchEventTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    napi_value argv1[2] = {nullptr};
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv1[0]);
    auto func = [](napi_env env, napi_callback_info info) -> napi_value {
        return nullptr;
    };
    napi_value funcValue = nullptr;
    napi_create_function(env, "testFunc", NAPI_AUTO_LENGTH, func, nullptr, &funcValue);
    argv1[1] = funcValue;

    std::string funcName = "Once";
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::Once, worker, &cb);
    napi_call_function(env, global, cb, sizeof(argv1) / sizeof(argv1[0]), argv1, &result);
    napi_value argv[1] = {nullptr};

    napi_value typeValue = nullptr;
    std::string type = "message";
    napi_create_string_utf8(env, type.c_str(), type.length(), &typeValue);

    napi_value object = nullptr;
    napi_create_object(env, &object);
    napi_set_named_property(env, object, "type", typeValue);
    argv[0] = object;

    funcName = "DispatchEvent";
    cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::DispatchEvent, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

//worker ParentPortAddEventListener
HWTEST_F(WorkersTest, ParentPortAddEventListenerTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    napi_value argv[3] = {nullptr};
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);
    auto func = [](napi_env env, napi_callback_info info) -> napi_value {
        return nullptr;
    };

    std::string funcName = "ParentPortAddEventListener";
    napi_value cb = nullptr;
    napi_value funcValue = nullptr;
    napi_create_function(env, "testFunc", NAPI_AUTO_LENGTH, func, nullptr, &funcValue);
    argv[1] = funcValue;
    ASSERT_TRUE(funcValue != nullptr);
    napi_value myobject = nullptr;
    napi_create_object(env, &myobject);
    argv[2] = myobject;
    napi_value callResult = nullptr;
    // ------- workerEnv---------
    napi_create_function(env, funcName.c_str(), funcName.size(),
                         Worker::ParentPortAddEventListener, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &callResult);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

//worker ParentPortRemoveAllListener
HWTEST_F(WorkersTest, ParentPortRemoveAllListenerTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    // ------- workerEnv---------
    napi_env workerEnv = nullptr;
    napi_create_runtime(env, &workerEnv);
    napi_value workerGlobal = nullptr;
    napi_get_global(workerEnv, &workerGlobal);
    napi_value argv[1] = {nullptr};
    std::string message = "host";
    napi_create_string_utf8(workerEnv, message.c_str(), message.length(), &argv[0]);
    std::string funcName = "ParentPortRemoveAllListener";
    napi_value cb = nullptr;

    napi_value callResult = nullptr;
    // ------- workerEnv---------
    napi_create_function(workerEnv, funcName.c_str(), funcName.size(),
                         Worker::ParentPortRemoveAllListener, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(workerEnv, workerGlobal, cb, sizeof(argv) / sizeof(argv[0]), argv, &callResult);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

//worker ParentPortDispatchEvent
HWTEST_F(WorkersTest, ParentPortDispatchEventTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    // ------- workerEnv---------
    napi_env workerEnv = nullptr;
    napi_create_runtime(env, &workerEnv);
    napi_value workerGlobal = nullptr;
    napi_get_global(workerEnv, &workerGlobal);

    napi_value argv[1] = {nullptr};
    napi_value objresult = nullptr;
    napi_create_object(workerEnv, &objresult);
    napi_value cb = nullptr;
    std::string funcName = "ParentPortDispatchEvent";
    napi_value messageKey = nullptr;
    const char* messageKeyStr = "type";
    napi_create_string_latin1(workerEnv, messageKeyStr, strlen(messageKeyStr), &messageKey);
    napi_value messageValue = nullptr;
    const char* messageValueStr = "message";
    napi_create_string_latin1(workerEnv, messageValueStr, strlen(messageValueStr), &messageValue);
    napi_set_property(workerEnv, objresult, messageKey, messageValue);
    argv[0] = objresult;
    napi_value callResult = nullptr;
    // ------- workerEnv---------
    napi_create_function(workerEnv, funcName.c_str(), funcName.size(),
                         Worker::ParentPortDispatchEvent, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(workerEnv, workerGlobal, cb, sizeof(argv) / sizeof(argv[0]), argv, &callResult);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

//worker ParentPortRemoveEventListener
HWTEST_F(WorkersTest, ParentPortRemoveEventListenerTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    // ------- workerEnv---------
    napi_env workerEnv = nullptr;
    napi_create_runtime(env, &workerEnv);
    napi_value workerGlobal = nullptr;
    napi_get_global(workerEnv, &workerGlobal);

    napi_value argv[2] = {nullptr};
    std::string message = "host";
    napi_create_string_utf8(workerEnv, message.c_str(), message.length(), &argv[0]);
    auto func = [](napi_env env, napi_callback_info info) -> napi_value {
        return nullptr;
    };
    std::string funcName = "ParentPortRemoveEventListener";
    napi_value cb = nullptr;
    napi_value funcValue = nullptr;
    napi_create_function(workerEnv, "testFunc", NAPI_AUTO_LENGTH, func, nullptr, &funcValue);
    argv[1] = funcValue;

    napi_value callResult = nullptr;
    // ------- workerEnv---------
    napi_create_function(workerEnv, funcName.c_str(), funcName.size(),
                         Worker::ParentPortRemoveEventListener, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(workerEnv, workerGlobal, cb, sizeof(argv) / sizeof(argv[0]), argv, &callResult);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

//worker GlobalCall
HWTEST_F(WorkersTest, GlobalCallTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    // ------- workerEnv---------
    napi_env workerEnv = nullptr;
    napi_create_runtime(env, &workerEnv);
    napi_value workerGlobal = nullptr;
    napi_get_global(workerEnv, &workerGlobal);

    napi_value argv[3] = {nullptr};
    std::string instanceName = "host";
    std::string methodName = "postMessage";
    int32_t timeout = 300;
    napi_create_string_utf8(workerEnv, instanceName.c_str(), instanceName.length(), &argv[0]);
    napi_create_string_utf8(workerEnv, methodName.c_str(), methodName.length(), &argv[1]);
    napi_create_int32(workerEnv, timeout, &argv[2]);

    std::string funcName = "GlobalCall";
    napi_value cb = nullptr;
    napi_value callResult = nullptr;
    // ------- workerEnv---------
    napi_create_function(workerEnv, funcName.c_str(), funcName.size(),
                         Worker::GlobalCall, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(workerEnv, workerGlobal, cb, sizeof(argv) / sizeof(argv[0]), argv, &callResult);
    uv_async_t* req = new uv_async_t;
    req->data = worker;
    Worker::HostOnGlobalCall(req);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

//messageQueue DeQueue_QUEUE_IS_NULL
HWTEST_F(WorkersTest, MessageQueueTest001, testing::ext::TestSize.Level0)
{
    MessageQueue queue;
    ASSERT_TRUE(queue.IsEmpty());
    MessageDataType data = nullptr;
    ASSERT_FALSE(queue.DeQueue(&data));
}

//messageQueue DeQueue_DATA_IS_NULL
HWTEST_F(WorkersTest, MessageQueueTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    MessageQueue queue;
    MessageDataType data = nullptr;
    napi_value undefined = NapiHelper::GetUndefinedValue(env);
    napi_serialize_inner(env, undefined, undefined, undefined, false, true, &data);
    queue.EnQueue(data);
    ASSERT_TRUE(queue.DeQueue(nullptr));
    queue.Clear(env);
}

//messageQueue MARKEDMESSAGEQUEUE
HWTEST_F(WorkersTest, MarkedMessageQueue001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    MarkedMessageQueue queue;
    MessageDataType data = nullptr;
    napi_value undefined = NapiHelper::GetUndefinedValue(env);
    napi_serialize_inner(env, undefined, undefined, undefined, false, true, &data);
    queue.Push(1, data);
    queue.Pop();
    ASSERT_TRUE(queue.IsEmpty());

    MessageDataType dataType = nullptr;
    napi_serialize_inner(env, undefined, undefined, undefined, false, true, &dataType);
    queue.Push(2, dataType);
    std::pair<uint32_t, MessageDataType> pair = queue.Front();
    ASSERT_EQ(pair.first, 2);
    queue.Clear(env);
    ASSERT_TRUE(queue.IsEmpty());
}

HWTEST_F(WorkersTest, WorkerTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    napi_env workerEnv = nullptr;
    napi_create_runtime(env, &workerEnv);
    napi_value exports = nullptr;
    napi_create_object(workerEnv, &exports);
    Worker::InitWorker(workerEnv, exports);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));

    napi_value argv[2] = {nullptr};
    std::string instanceName = "MainThread";
    napi_create_string_utf8(env, instanceName.c_str(), instanceName.length(), &argv[0]);
    napi_value obj = nullptr;
    napi_create_object(env, &obj);
    argv[1] = obj;

    std::string funcName = "RegisterGlobalCallObject";
    napi_value cb = nullptr;
    napi_value callResult = nullptr;
    // ------- workerEnv---------
    napi_create_function(env, funcName.c_str(), funcName.size(),
                         Worker::RegisterGlobalCallObject, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &callResult);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));

    napi_value argv[1] = {nullptr};
    std::string instanceName = "MainThread";
    napi_create_string_utf8(env, instanceName.c_str(), instanceName.length(), &argv[0]);

    std::string funcName = "UnregisterGlobalCallObject";
    napi_value cb = nullptr;
    napi_value callResult = nullptr;
    // ------- workerEnv---------
    napi_create_function(env, funcName.c_str(), funcName.size(),
                        Worker::UnregisterGlobalCallObject, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &callResult);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest004, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_env workerEnv = nullptr;
    napi_create_runtime(env, &workerEnv);
    napi_value workerGlobal = nullptr;
    napi_get_global(workerEnv, &workerGlobal);
    NativeEngine::GetMainThreadEngine()->CheckAndSetWorkerVersion(WorkerVersion::OLD, WorkerVersion::NONE);

    std::string funcName = "LimitedWorkerConstructor";
    napi_value cb = nullptr;
    napi_create_function(workerEnv, funcName.c_str(), funcName.size(), Worker::LimitedWorkerConstructor, nullptr, &cb);

    napi_value result = nullptr;
    napi_value argv[2] = { nullptr };
    std::string script = "entry/ets/workers/@worker.ts";
    napi_create_string_utf8(workerEnv, script.c_str(), script.length(), &argv[0]);
    std::string type = "classic";
    std::string name = "WorkerThread";
    napi_value typeValue = nullptr;
    napi_value nameValue = nullptr;
    napi_create_string_utf8(workerEnv, name.c_str(), name.length(), &nameValue);
    napi_create_string_utf8(workerEnv, type.c_str(), type.length(), &typeValue);

    napi_value object = nullptr;
    napi_create_object(workerEnv, &object);

    napi_set_named_property(workerEnv, object, "name", nameValue);
    napi_set_named_property(workerEnv, object, "type", typeValue);
    argv[1] = object;

    napi_call_function(workerEnv, workerGlobal, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    uv_sleep(200);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(workerEnv, workerGlobal);
    NativeEngine::GetMainThreadEngine()->CheckAndSetWorkerVersion(WorkerVersion::NEW, WorkerVersion::OLD);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest005, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_env workerEnv = nullptr;
    napi_create_runtime(env, &workerEnv);
    napi_value workerGlobal = nullptr;
    napi_get_global(workerEnv, &workerGlobal);
    NativeEngine::GetMainThreadEngine()->CheckAndSetWorkerVersion(WorkerVersion::OLD, WorkerVersion::NONE);
    std::string funcName = "ThreadWorkerConstructor";
    napi_value cb = nullptr;
    napi_create_function(workerEnv, funcName.c_str(), funcName.size(), Worker::ThreadWorkerConstructor, nullptr, &cb);

    napi_value result = nullptr;
    napi_value argv[2] = { nullptr };
    std::string script = "entry/ets/workers/@worker.ts";
    napi_create_string_utf8(workerEnv, script.c_str(), script.length(), &argv[0]);
    std::string type = "classic";
    std::string name = "WorkerThread";
    napi_value typeValue = nullptr;
    napi_value nameValue = nullptr;
    napi_create_string_utf8(workerEnv, name.c_str(), name.length(), &nameValue);
    napi_create_string_utf8(workerEnv, type.c_str(), type.length(), &typeValue);

    napi_value object = nullptr;
    napi_create_object(workerEnv, &object);

    napi_set_named_property(workerEnv, object, "name", nameValue);
    napi_set_named_property(workerEnv, object, "type", typeValue);
    argv[1] = object;

    napi_call_function(workerEnv, workerGlobal, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    uv_sleep(200);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(workerEnv, workerGlobal);
    NativeEngine::GetMainThreadEngine()->CheckAndSetWorkerVersion(WorkerVersion::NEW, WorkerVersion::OLD);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest006, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);

    napi_value result = nullptr;
    result = Worker_Constructor(env, global);

    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));

    napi_value argv[1] = { nullptr };
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);
    std::string funcName = "PostMessageWithSharedSendable";
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::PostMessageWithSharedSendable, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest007, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);

    napi_value result = nullptr;
    result = Worker_Constructor(env, global);

    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));

    napi_value argv[1] = { nullptr };

    std::string funcName = "RemoveAllListener";
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::RemoveAllListener, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(env, global, cb, 1, argv, &result);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest008, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);

    napi_value result = nullptr;
    result = Worker_Constructor(env, global);

    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));

    napi_value argv[1] = { nullptr };
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);
    
    std::string funcName = "CancelTask";
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::CancelTask, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest009, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);

    napi_value result = nullptr;
    result = Worker_Constructor(env, global);

    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    napi_value argv[1] = { nullptr };
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);

    std::string funcName = "PostMessageWithSharedSendableToHost";
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(),
                         Worker::PostMessageWithSharedSendableToHost, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    uv_async_t* req = new uv_async_t;
    req->data = worker;
    Worker::HostOnMessage(req);

    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest010, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);

    napi_value result = nullptr;
    result = Worker_Constructor(env, global);

    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    napi_value argv[1] = { nullptr };
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);

    std::string funcName = "ParentPortCancelTask";
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::ParentPortCancelTask, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest011, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);

    napi_value result = nullptr;
    result = Worker_Constructor(env, global);

    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    uv_async_t* req = new uv_async_t;
    req->data = nullptr;
    Worker::HostOnError(req);
    req->data = worker;
    Worker::HostOnError(req);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    ASSERT_TRUE(result != nullptr);
}

//worker PostMessage
HWTEST_F(WorkersTest, WorkerTest012, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);

    napi_value result = nullptr;
    result = Worker_Constructor(env, global);

    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));

    napi_value argv[1] = { nullptr };
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);
    std::string funcName = "PostMessage";
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::PostMessage, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);

    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}


HWTEST_F(WorkersTest, CloseWorkerTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    std::string funcName = "CloseWorker";
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::CloseWorker, nullptr, &cb);
    napi_call_function(env, global, cb, 0, nullptr, &result);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, CloseWorkerTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);

    napi_value result = nullptr;
    result = Worker_Constructor(env, global);

    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    ASSERT_TRUE(worker != nullptr);

    std::string funcName = "CloseWorker";
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::CloseWorker, worker, &cb);
    napi_env workerEnv = nullptr;
    napi_create_runtime(env, &workerEnv);
    SetCloseWorkerProp(worker, workerEnv);
    napi_call_function(env, global, cb, 0, nullptr, &result);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, InitWorkerTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value exports = nullptr;
    napi_create_object(env, &exports);
    Worker::InitWorker(env, exports);
    ASSERT_TRUE(exports != nullptr);
}

HWTEST_F(WorkersTest, InitWorkerTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_env workerEnv = nullptr;
    napi_create_runtime(env, &workerEnv);
    napi_value workerGlobal = nullptr;
    napi_get_global(workerEnv, &workerGlobal);
    NativeEngine::GetMainThreadEngine()->CheckAndSetWorkerVersion(WorkerVersion::OLD, WorkerVersion::NONE);
    std::string funcName = "ThreadWorkerConstructor";
    napi_value cb = nullptr;
    napi_create_function(workerEnv, funcName.c_str(), funcName.size(), Worker::ThreadWorkerConstructor, nullptr, &cb);

    napi_value result = nullptr;
    napi_value argv[2] = { nullptr };
    std::string script = "entry/ets/workers/@worker.ts";
    napi_create_string_utf8(workerEnv, script.c_str(), script.length(), &argv[0]);
    std::string type = "classic";
    std::string name = "WorkerThread";
    napi_value typeValue = nullptr;
    napi_value nameValue = nullptr;
    napi_create_string_utf8(workerEnv, name.c_str(), name.length(), &nameValue);
    napi_create_string_utf8(workerEnv, type.c_str(), type.length(), &typeValue);

    napi_value object = nullptr;
    napi_create_object(workerEnv, &object);

    napi_set_named_property(workerEnv, object, "name", nameValue);
    napi_set_named_property(workerEnv, object, "type", typeValue);
    argv[1] = object;

    napi_call_function(workerEnv, workerGlobal, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    uv_sleep(200);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    ASSERT_TRUE(worker != nullptr);
    worker->SetWorkerEnv(workerEnv);
    NativeEngine* workerEngine = reinterpret_cast<NativeEngine*>(workerEnv);
    workerEngine->MarkRestrictedWorkerThread();
    napi_value exports = nullptr;
    napi_create_object(env, &exports);
    Worker::InitWorker(workerEnv, exports);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    NativeEngine::GetMainThreadEngine()->CheckAndSetWorkerVersion(WorkerVersion::NEW, WorkerVersion::OLD);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, InitWorkerTest003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_env workerEnv = nullptr;
    napi_create_runtime(env, &workerEnv);
    napi_value workerGlobal = nullptr;
    napi_get_global(workerEnv, &workerGlobal);
    NativeEngine::GetMainThreadEngine()->CheckAndSetWorkerVersion(WorkerVersion::OLD, WorkerVersion::NONE);
    std::string funcName = "ThreadWorkerConstructor";
    napi_value cb = nullptr;
    napi_create_function(workerEnv, funcName.c_str(), funcName.size(), Worker::ThreadWorkerConstructor, nullptr, &cb);

    napi_value result = nullptr;
    napi_value argv[2] = { nullptr };
    std::string script = "entry/ets/workers/@worker.ts";
    napi_create_string_utf8(workerEnv, script.c_str(), script.length(), &argv[0]);
    std::string type = "classic";
    std::string name = "WorkerThread";
    napi_value typeValue = nullptr;
    napi_value nameValue = nullptr;
    napi_create_string_utf8(workerEnv, name.c_str(), name.length(), &nameValue);
    napi_create_string_utf8(workerEnv, type.c_str(), type.length(), &typeValue);

    napi_value object = nullptr;
    napi_create_object(workerEnv, &object);

    napi_set_named_property(workerEnv, object, "name", nameValue);
    napi_set_named_property(workerEnv, object, "type", typeValue);
    argv[1] = object;

    napi_call_function(workerEnv, workerGlobal, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);

    uv_sleep(200);
    Worker* worker = new Worker(env, nullptr);
    napi_wrap(env, result, worker, nullptr, nullptr, nullptr);
    worker->SetWorkerEnv(workerEnv);
    NativeEngine* workerEngine = reinterpret_cast<NativeEngine*>(workerEnv);
    workerEngine->MarkTaskPoolThread();
    napi_value exports = nullptr;
    napi_create_object(env, &exports);
    Worker::InitWorker(workerEnv, exports);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    NativeEngine::GetMainThreadEngine()->CheckAndSetWorkerVersion(WorkerVersion::NEW, WorkerVersion::OLD);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, InitWorkerTest004, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_env workerEnv = nullptr;
    napi_create_runtime(env, &workerEnv);
    napi_value workerGlobal = nullptr;
    napi_get_global(workerEnv, &workerGlobal);
    NativeEngine::GetMainThreadEngine()->CheckAndSetWorkerVersion(WorkerVersion::OLD, WorkerVersion::NONE);
    std::string funcName = "ThreadWorkerConstructor";
    napi_value cb = nullptr;
    napi_create_function(workerEnv, funcName.c_str(), funcName.size(), Worker::ThreadWorkerConstructor, nullptr, &cb);

    napi_value result = nullptr;
    napi_value argv[2] = { nullptr };
    std::string script = "entry/ets/workers/@worker.ts";
    napi_create_string_utf8(workerEnv, script.c_str(), script.length(), &argv[0]);
    std::string type = "classic";
    std::string name = "WorkerThread";
    napi_value typeValue = nullptr;
    napi_value nameValue = nullptr;
    napi_create_string_utf8(workerEnv, name.c_str(), name.length(), &nameValue);
    napi_create_string_utf8(workerEnv, type.c_str(), type.length(), &typeValue);

    napi_value object = nullptr;
    napi_create_object(workerEnv, &object);

    napi_set_named_property(workerEnv, object, "name", nameValue);
    napi_set_named_property(workerEnv, object, "type", typeValue);
    argv[1] = object;

    napi_call_function(workerEnv, workerGlobal, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);

    uv_sleep(200);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    worker->SetWorkerEnv(workerEnv);
    NativeEngine* workerEngine = reinterpret_cast<NativeEngine*>(workerEnv);
    workerEngine->MarkWorkerThread();
    napi_value exports = nullptr;
    napi_create_object(workerEnv, &exports);
    Worker::InitWorker(workerEnv, exports);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    NativeEngine::GetMainThreadEngine()->CheckAndSetWorkerVersion(WorkerVersion::NEW, WorkerVersion::OLD);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, InitWorkerTest005, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_env workerEnv = nullptr;
    napi_create_runtime(env, &workerEnv);
    napi_value workerGlobal = nullptr;
    napi_get_global(workerEnv, &workerGlobal);
    NativeEngine::GetMainThreadEngine()->CheckAndSetWorkerVersion(WorkerVersion::OLD, WorkerVersion::NONE);
    std::string funcName = "LimitedWorkerConstructor";
    napi_value cb = nullptr;
    napi_create_function(workerEnv, funcName.c_str(), funcName.size(), Worker::LimitedWorkerConstructor, nullptr, &cb);

    napi_value result = nullptr;
    napi_value argv[2] = { nullptr };
    std::string script = "entry/ets/workers/@worker.ts";
    napi_create_string_utf8(workerEnv, script.c_str(), script.length(), &argv[0]);
    std::string type = "classic";
    std::string name = "WorkerThread";
    napi_value typeValue = nullptr;
    napi_value nameValue = nullptr;
    napi_create_string_utf8(workerEnv, name.c_str(), name.length(), &nameValue);
    napi_create_string_utf8(workerEnv, type.c_str(), type.length(), &typeValue);

    napi_value object = nullptr;
    napi_create_object(workerEnv, &object);

    napi_set_named_property(workerEnv, object, "name", nameValue);
    napi_set_named_property(workerEnv, object, "type", typeValue);
    argv[1] = object;

    napi_call_function(workerEnv, workerGlobal, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);

    uv_sleep(200);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    worker->SetWorkerEnv(workerEnv);
    NativeEngine* workerEngine = reinterpret_cast<NativeEngine*>(workerEnv);
    workerEngine->MarkRestrictedWorkerThread();
    napi_value exports = nullptr;
    napi_create_object(workerEnv, &exports);
    Worker::InitWorker(workerEnv, exports);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    NativeEngine::GetMainThreadEngine()->CheckAndSetWorkerVersion(WorkerVersion::NEW, WorkerVersion::OLD);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, ConstructorTest001, testing::ext::TestSize.Level0)
{
    napi_env env = WorkersTest::GetEnv();
    Worker *worker = new Worker(env, nullptr);
    ASSERT_TRUE(worker != nullptr);
    napi_env hostEnv = worker->GetHostEnv();
    ASSERT_TRUE(env == hostEnv);
    napi_env workerEnv = worker->GetWorkerEnv();
    ASSERT_TRUE(workerEnv == nullptr);
    delete worker;
    worker = nullptr;
}

HWTEST_F(WorkersTest, ConstructorTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global = nullptr;
    napi_get_global(env, &global);

    std::string funcName = "LimitedWorkerConstructor";
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::LimitedWorkerConstructor, nullptr, &cb);

    napi_value result = nullptr;
    napi_value argv[2] = { nullptr };
    std::string script = "entry/ets/workers/@worker.ts";
    napi_create_string_utf8(env, script.c_str(), script.length(), &argv[0]);
    std::string type = "classic";
    std::string name = "WorkerThread";
    napi_value typeValue = nullptr;
    napi_value nameValue = nullptr;
    napi_create_string_utf8(env, name.c_str(), name.length(), &nameValue);
    napi_create_string_utf8(env, type.c_str(), type.length(), &typeValue);

    napi_value object = nullptr;
    napi_create_object(env, &object);

    napi_set_named_property(env, object, "name", nameValue);
    napi_set_named_property(env, object, "type", typeValue);
    argv[1] = object;

    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    uv_sleep(200);
    ASSERT_TRUE(result == nullptr);
}

HWTEST_F(WorkersTest, ConstructorTest003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global = nullptr;
    napi_get_global(env, &global);

    std::string funcName = "ThreadWorkerConstructor";
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::ThreadWorkerConstructor, nullptr, &cb);

    napi_value result = nullptr;
    napi_value argv[2] = { nullptr };
    std::string script = "entry/ets/workers/@worker.ts";
    napi_create_string_utf8(env, script.c_str(), script.length(), &argv[0]);
    std::string type = "classic";
    std::string name = "WorkerThread";
    napi_value typeValue = nullptr;
    napi_value nameValue = nullptr;
    napi_create_string_utf8(env, name.c_str(), name.length(), &nameValue);
    napi_create_string_utf8(env, type.c_str(), type.length(), &typeValue);

    napi_value object = nullptr;
    napi_create_object(env, &object);

    napi_set_named_property(env, object, "name", nameValue);
    napi_set_named_property(env, object, "type", typeValue);
    argv[1] = object;

    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    uv_sleep(200);
    ASSERT_TRUE(result == nullptr);
}

HWTEST_F(WorkersTest, ConstructorTest004, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_env workerEnv = nullptr;
    napi_create_runtime(env, &workerEnv);
    napi_value workerGlobal = nullptr;
    napi_get_global(workerEnv, &workerGlobal);
    NativeEngine::GetMainThreadEngine()->CheckAndSetWorkerVersion(WorkerVersion::OLD, WorkerVersion::NONE);
    std::string funcName = "ThreadWorkerConstructor";
    napi_value cb = nullptr;
    napi_create_function(workerEnv, funcName.c_str(), funcName.size(), Worker::ThreadWorkerConstructor, nullptr, &cb);

    napi_value result = nullptr;
    napi_call_function(workerEnv, workerGlobal, cb, 0, nullptr, &result);
    uv_sleep(200);
    NativeEngine::GetMainThreadEngine()->CheckAndSetWorkerVersion(WorkerVersion::NEW, WorkerVersion::OLD);
    ASSERT_TRUE(result == nullptr);
}

HWTEST_F(WorkersTest, ConstructorTest005, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_env workerEnv = nullptr;
    napi_create_runtime(env, &workerEnv);
    napi_value workerGlobal = nullptr;
    napi_get_global(workerEnv, &workerGlobal);
    NativeEngine::GetMainThreadEngine()->CheckAndSetWorkerVersion(WorkerVersion::OLD, WorkerVersion::NONE);
    std::string funcName = "ThreadWorkerConstructor";
    napi_value cb = nullptr;
    napi_create_function(workerEnv, funcName.c_str(), funcName.size(), Worker::ThreadWorkerConstructor, nullptr, &cb);

    napi_value result = nullptr;
    napi_value argv[2] = { nullptr };
    int32_t script = 200;
    napi_create_int32(workerEnv, script, &argv[0]);
    std::string type = "classic";
    std::string name = "WorkerThread";
    napi_value typeValue = nullptr;
    napi_value nameValue = nullptr;
    napi_create_string_utf8(workerEnv, name.c_str(), name.length(), &nameValue);
    napi_create_string_utf8(workerEnv, type.c_str(), type.length(), &typeValue);

    napi_value object = nullptr;
    napi_create_object(workerEnv, &object);

    napi_set_named_property(workerEnv, object, "name", nameValue);
    napi_set_named_property(workerEnv, object, "type", typeValue);
    argv[1] = object;

    napi_call_function(workerEnv, workerGlobal, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    NativeEngine::GetMainThreadEngine()->CheckAndSetWorkerVersion(WorkerVersion::NEW, WorkerVersion::OLD);
    uv_sleep(200);
    ASSERT_TRUE(result == nullptr);
}

HWTEST_F(WorkersTest, ConstructorTest006, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_env workerEnv = nullptr;
    napi_create_runtime(env, &workerEnv);
    napi_value workerGlobal = nullptr;
    napi_get_global(workerEnv, &workerGlobal);
    auto engine = reinterpret_cast<NativeEngine*>(workerEnv);
    engine->CheckAndSetWorkerVersion(WorkerVersion::NONE, WorkerVersion::OLD);
    Worker::ThreadWorkerConstructor(workerEnv, nullptr);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(workerEnv, &exception);
    ASSERT_TRUE(exception != nullptr);
    engine->CheckAndSetWorkerVersion(WorkerVersion::OLD, WorkerVersion::NEW);
    Worker::WorkerConstructor(workerEnv, nullptr);
    napi_get_and_clear_last_exception(workerEnv, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(WorkersTest, ConstructorTest007, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_env workerEnv = nullptr;
    napi_create_runtime(env, &workerEnv);
    napi_value workerGlobal = nullptr;
    napi_get_global(workerEnv, &workerGlobal);
    std::string funcName = "WorkerConstructor";
    napi_value cb = nullptr;
    napi_create_function(workerEnv, funcName.c_str(), funcName.size(), Worker::WorkerConstructor, nullptr, &cb);

    napi_value result = nullptr;
    napi_value argv[2] = { nullptr };
    std::string script = "entry/ets/workers/@worker.ts";
    napi_create_string_utf8(workerEnv, script.c_str(), script.length(), &argv[0]);
    argv[1] = nullptr;
    napi_call_function(workerEnv, workerGlobal, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    uv_sleep(200);
    ASSERT_TRUE(result != nullptr);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
}

HWTEST_F(WorkersTest, ConstructorTest008, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_env workerEnv = nullptr;
    napi_create_runtime(env, &workerEnv);
    napi_value workerGlobal = nullptr;
    napi_get_global(workerEnv, &workerGlobal);
    std::string funcName = "WorkerConstructor";
    napi_value cb = nullptr;
    napi_create_function(workerEnv, funcName.c_str(), funcName.size(), Worker::WorkerConstructor, nullptr, &cb);

    napi_value result = nullptr;
    napi_value argv[3] = { nullptr };
    std::string script = "entry/ets/workers/@worker.ts";
    napi_create_string_utf8(workerEnv, script.c_str(), script.length(), &argv[0]);
    argv[1] = nullptr;
    napi_call_function(workerEnv, workerGlobal, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    uv_sleep(200);
    ASSERT_TRUE(result != nullptr);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(WorkersTest, ConstructorTest009, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_env workerEnv = nullptr;
    napi_create_runtime(env, &workerEnv);
    napi_value workerGlobal = nullptr;
    napi_get_global(workerEnv, &workerGlobal);
    std::string funcName = "WorkerConstructor";
    napi_value cb = nullptr;
    napi_create_function(workerEnv, funcName.c_str(), funcName.size(), Worker::WorkerConstructor, nullptr, &cb);

    napi_value result = nullptr;
    napi_value argv[2] = { nullptr };
    argv[0] = NapiHelper::CreateEmptyString(workerEnv);
    std::string type = "classic";
    napi_value typeValue = nullptr;
    napi_create_string_utf8(workerEnv, type.c_str(), type.length(), &typeValue);
    napi_value nameValue = NapiHelper::CreateEmptyString(workerEnv);;

    napi_value object = nullptr;
    napi_create_object(workerEnv, &object);

    napi_set_named_property(workerEnv, object, "name", nameValue);
    napi_set_named_property(workerEnv, object, "type", typeValue);
    argv[1] = object;

    napi_call_function(workerEnv, workerGlobal, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    uv_sleep(200);
    ASSERT_TRUE(result != nullptr);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(WorkersTest, CheckWorkerArgs001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value undefined = NapiHelper::GetUndefinedValue(env);
    napi_value object1 = nullptr;
    napi_create_object(env, &object1);
    napi_set_named_property(env, object1, "name", undefined);
    napi_set_named_property(env, object1, "type", undefined);
    Worker::CheckWorkerArgs(env, object1);

    uint32_t name2 = 100; //test 100
    napi_value nameValue2 = nullptr;
    napi_create_uint32(env, name2, &nameValue2);
    napi_value object2 = nullptr;
    napi_create_object(env, &object2);
    napi_set_named_property(env, object2, "name", nameValue2);
    Worker::CheckWorkerArgs(env, object2);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);

    uint32_t type3 = 100; //test 100
    std::string name3 = "WorkerThread";
    napi_value typeValue3 = nullptr;
    napi_value nameValue3 = nullptr;
    napi_create_string_utf8(env, name3.c_str(), name3.length(), &nameValue3);
    napi_create_uint32(env, type3, &typeValue3);
    napi_value object3 = nullptr;
    napi_create_object(env, &object3);
    napi_set_named_property(env, object3, "name", nameValue3);
    napi_set_named_property(env, object3, "type", typeValue3);
    Worker::CheckWorkerArgs(env, object3);
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(WorkersTest, WorkerTest013, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    napi_value argv[1] = { nullptr };
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);
    std::string funcName = "PostMessage";
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::PostMessage, worker, &cb);
    napi_call_function(env, global, cb, 0, nullptr, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
    cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::PostMessage, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::TERMINATEING);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::PostMessage, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::TERMINATED);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest014, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    napi_value argv[1] = { nullptr };
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);
    std::string funcName = "PostMessage";
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::PostMessage, nullptr, &cb);
    napi_call_function(env, nullptr, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest015, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);

    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));

    napi_value argv[2] = { nullptr };
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);
    std::string arr = "arr";
    napi_create_string_utf8(env, arr.c_str(), arr.length(), &argv[1]);
    std::string funcName = "PostMessage";
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::PostMessage, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest016, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));

    napi_value argv[2] = { nullptr };
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);
    napi_create_array_with_length(env, 1, &argv[1]);
    std::string funcName = "PostMessage";
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::PostMessage, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest017, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));

    napi_value argv[2] = {nullptr};
    std::string instanceName = "MainThread";
    napi_create_string_utf8(env, instanceName.c_str(), instanceName.length(), &argv[0]);
    napi_value obj = nullptr;
    napi_create_object(env, &obj);
    argv[1] = obj;

    std::string funcName = "RegisterGlobalCallObject";
    napi_value cb = nullptr;
    napi_value callResult = nullptr;
    // ------- workerEnv---------
    napi_create_function(env, funcName.c_str(), funcName.size(),
                         Worker::RegisterGlobalCallObject, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(env, global, cb, 0, nullptr, &callResult);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest018, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));

    napi_value argv[2] = {nullptr};
    std::string instanceName = "MainThread";
    napi_create_string_utf8(env, instanceName.c_str(), instanceName.length(), &argv[0]);
    napi_value obj = nullptr;
    napi_create_object(env, &obj);
    argv[1] = obj;

    std::string funcName = "RegisterGlobalCallObject";
    napi_value cb = nullptr;
    napi_value callResult = nullptr;
    // ------- workerEnv---------
    napi_create_function(env, funcName.c_str(), funcName.size(),
                         Worker::RegisterGlobalCallObject, nullptr, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(env, nullptr, cb, sizeof(argv) / sizeof(argv[0]), argv, &callResult);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest019, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));

    napi_value argv[2] = {nullptr};
    std::string instanceName = "MainThread";
    napi_create_string_utf8(env, instanceName.c_str(), instanceName.length(), &argv[0]);

    std::string funcName = "UnregisterGlobalCallObject";
    napi_create_string_utf8(env, funcName.c_str(), funcName.length(), &argv[1]);
    napi_value cb = nullptr;
    napi_value callResult = nullptr;
    // ------- workerEnv---------
    napi_create_function(env, funcName.c_str(), funcName.size(),
                        Worker::UnregisterGlobalCallObject, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &callResult);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest020, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));

    napi_value argv[1] = {nullptr};
    std::string instanceName = "MainThread";
    napi_create_string_utf8(env, instanceName.c_str(), instanceName.length(), &argv[0]);

    std::string funcName = "UnregisterGlobalCallObject";
    napi_value cb = nullptr;
    napi_value callResult = nullptr;
    // ------- workerEnv---------
    napi_create_function(env, funcName.c_str(), funcName.size(),
                        Worker::UnregisterGlobalCallObject, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(env, global, cb, 0, nullptr, &callResult);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest021, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));

    napi_value argv[1] = {nullptr};
    std::string instanceName = "MainThread";
    napi_create_string_utf8(env, instanceName.c_str(), instanceName.length(), &argv[0]);

    std::string funcName = "UnregisterGlobalCallObject";
    napi_value cb = nullptr;
    napi_value callResult = nullptr;
    // ------- workerEnv---------
    napi_create_function(env, funcName.c_str(), funcName.size(),
                        Worker::UnregisterGlobalCallObject, nullptr, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(env, nullptr, cb, sizeof(argv) / sizeof(argv[0]), argv, &callResult);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest022, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));

    napi_value argv[2] = {nullptr};
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);

    std::string funcName = "AddEventListener";
    napi_create_string_utf8(env, funcName.c_str(), funcName.length(), &argv[1]);
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::AddEventListener, worker, &cb);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest023, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));

    napi_value argv[1] = {nullptr};
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);

    std::string funcName = "AddEventListener";
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::AddEventListener, worker, &cb);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest024, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));

    napi_value argv[2] = {nullptr};
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);

    std::string funcName = "AddEventListener";
    napi_create_string_utf8(env, funcName.c_str(), funcName.length(), &argv[1]);
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::AddEventListener, nullptr, &cb);
    napi_call_function(env, nullptr, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest025, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));

    napi_value argv[2] = {nullptr};
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);
    auto func = [](napi_env env, napi_callback_info info) -> napi_value {
        return nullptr;
    };
    napi_value funcValue = nullptr;
    napi_create_function(env, "testFunc", NAPI_AUTO_LENGTH, func, nullptr, &funcValue);
    argv[1] = funcValue;

    std::string funcName = "RemoveEventListener";
    napi_value cb = nullptr;

    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::RemoveEventListener, worker, &cb);
    napi_call_function(env, global, cb, 0, nullptr, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest026, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));

    napi_value argv[2] = {nullptr};
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);
    auto func = [](napi_env env, napi_callback_info info) -> napi_value {
        return nullptr;
    };
    napi_value funcValue = nullptr;
    napi_create_function(env, "testFunc", NAPI_AUTO_LENGTH, func, nullptr, &funcValue);
    argv[1] = funcValue;

    std::string funcName = "RemoveEventListener";
    napi_value cb = nullptr;

    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::RemoveEventListener, nullptr, &cb);
    napi_call_function(env, nullptr, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest027, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));

    napi_value argv[2] = {nullptr};
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);

    std::string funcName = "RemoveEventListener";
    napi_create_string_utf8(env, funcName.c_str(), funcName.length(), &argv[1]);
    napi_value cb = nullptr;

    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::RemoveEventListener, worker, &cb);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest028, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    SetWorkerRef(worker, env);
    napi_value argv[1] = {nullptr};
    napi_value typeValue = nullptr;
    std::string type = "message";
    napi_create_string_utf8(env, type.c_str(), type.length(), &typeValue);
    napi_value object = nullptr;
    napi_create_object(env, &object);
    napi_set_named_property(env, object, "type", typeValue);
    argv[0] = object;

    std::string funcName = "DispatchEvent";
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::DispatchEvent, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest029, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));

    std::string funcName = "DispatchEvent";
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::DispatchEvent, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(env, global, cb, 0, nullptr, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest030, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    SetWorkerRef(worker, env);
    napi_value argv[1] = {nullptr};
    std::string type = "message";
    napi_create_string_utf8(env, type.c_str(), type.length(), &argv[0]);

    std::string funcName = "DispatchEvent";
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::DispatchEvent, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest031, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    SetWorkerRef(worker, env);
    napi_value argv[1] = {nullptr};
    napi_value typeValue = nullptr;
    std::string type = "message";
    napi_create_string_utf8(env, type.c_str(), type.length(), &typeValue);
    napi_value object = nullptr;
    napi_create_object(env, &object);
    napi_set_named_property(env, object, "type", typeValue);
    argv[0] = object;
    std::string funcName = "DispatchEvent";
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::DispatchEvent, nullptr, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(env, nullptr, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest032, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    SetWorkerRef(worker, env);
    napi_value argv[1] = {nullptr};
    napi_value typeValue = nullptr;
    uint32_t type = 100; //test 100
    napi_create_uint32(env, type, &typeValue);
    napi_value object = nullptr;
    napi_create_object(env, &object);
    napi_set_named_property(env, object, "type", typeValue);
    argv[0] = object;
    std::string funcName = "DispatchEvent";
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::DispatchEvent, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest033, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    napi_value argv[1] = { nullptr };
    std::string funcName = "RemoveAllListener";
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::RemoveAllListener, nullptr, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(env, nullptr, cb, 1, argv, &result);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest034, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    napi_value argv[1] = { nullptr };
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);
    std::string funcName = "CancelTask";
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::CancelTask, nullptr, &cb);
    napi_call_function(env, nullptr, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest035, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    napi_value argv[1] = { nullptr };
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);
    std::string funcName = "CancelTask";
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::CancelTask, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::TERMINATED);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest036, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    napi_value argv[1] = { nullptr };
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);
    std::string funcName = "CancelTask";
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::CancelTask, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::TERMINATEING);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest037, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    napi_value argv[1] = { nullptr };
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);
    std::string funcName = "CancelTask";
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::CancelTask, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    SetWorkerHostEnv(worker, env, true);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    SetWorkerHostEnv(worker, env, false);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest038, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    napi_value argv[1] = { nullptr };
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);
    std::string funcName = "PostMessageToHost";
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::PostMessageToHost, worker, &cb);
    napi_call_function(env, global, cb, 0, nullptr, &result);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest039, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    napi_value argv[1] = { nullptr };
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);
    std::string funcName = "PostMessageToHost";
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::PostMessageToHost, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::TERMINATED);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest040, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    // ------- workerEnv---------
    napi_env workerEnv = nullptr;
    napi_create_runtime(env, &workerEnv);
    napi_value workerGlobal = nullptr;
    napi_get_global(workerEnv, &workerGlobal);
    std::string funcName = "GlobalCall";
    napi_value cb = nullptr;
    napi_value callResult = nullptr;
    // ------- workerEnv---------
    napi_create_function(workerEnv, funcName.c_str(), funcName.size(),
                         Worker::GlobalCall, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(workerEnv, workerGlobal, cb, 0, nullptr, &callResult);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest041, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    // ------- workerEnv---------
    napi_env workerEnv = nullptr;
    napi_create_runtime(env, &workerEnv);
    napi_value workerGlobal = nullptr;
    napi_get_global(workerEnv, &workerGlobal);
    napi_value argv[3] = {nullptr};
    std::string instanceName = "host";
    std::string methodName = "postMessage";
    int32_t timeout = 300;
    napi_create_string_utf8(workerEnv, instanceName.c_str(), instanceName.length(), &argv[0]);
    napi_create_string_utf8(workerEnv, methodName.c_str(), methodName.length(), &argv[1]);
    napi_create_int32(workerEnv, timeout, &argv[2]);
    std::string funcName = "GlobalCall";
    napi_value cb = nullptr;
    napi_value callResult = nullptr;
    // ------- workerEnv---------
    napi_create_function(workerEnv, funcName.c_str(), funcName.size(),
                         Worker::GlobalCall, nullptr, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(workerEnv, nullptr, cb, sizeof(argv) / sizeof(argv[0]), argv, &callResult);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest042, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    // ------- workerEnv---------
    napi_env workerEnv = nullptr;
    napi_create_runtime(env, &workerEnv);
    napi_value workerGlobal = nullptr;
    napi_get_global(workerEnv, &workerGlobal);
    napi_value argv[3] = {nullptr};
    std::string instanceName = "host";
    std::string methodName = "postMessage";
    int32_t timeout = 300;
    napi_create_string_utf8(workerEnv, instanceName.c_str(), instanceName.length(), &argv[0]);
    napi_create_string_utf8(workerEnv, methodName.c_str(), methodName.length(), &argv[1]);
    napi_create_int32(workerEnv, timeout, &argv[2]);
    std::string funcName = "GlobalCall";
    napi_value cb = nullptr;
    napi_value callResult = nullptr;
    // ------- workerEnv---------
    napi_create_function(workerEnv, funcName.c_str(), funcName.size(),
                         Worker::GlobalCall, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::TERMINATED);
    napi_call_function(workerEnv, workerGlobal, cb, sizeof(argv) / sizeof(argv[0]), argv, &callResult);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest043, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    // ------- workerEnv---------
    napi_env workerEnv = nullptr;
    napi_create_runtime(env, &workerEnv);
    napi_value workerGlobal = nullptr;
    napi_get_global(workerEnv, &workerGlobal);
    napi_value argv[3] = {nullptr};
    std::string instanceName = "host";
    std::string methodName = "postMessage";
    int32_t timeout = 300;
    napi_create_string_utf8(workerEnv, instanceName.c_str(), instanceName.length(), &argv[0]);
    napi_create_string_utf8(workerEnv, methodName.c_str(), methodName.length(), &argv[2]);
    napi_create_int32(workerEnv, timeout, &argv[1]);
    std::string funcName = "GlobalCall";
    napi_value cb = nullptr;
    napi_value callResult = nullptr;
    // ------- workerEnv---------
    napi_create_function(workerEnv, funcName.c_str(), funcName.size(),
                         Worker::GlobalCall, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(workerEnv, workerGlobal, cb, sizeof(argv) / sizeof(argv[0]), argv, &callResult);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest044, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    // ------- workerEnv---------
    napi_env workerEnv = nullptr;
    napi_create_runtime(env, &workerEnv);
    napi_value workerGlobal = nullptr;
    napi_get_global(workerEnv, &workerGlobal);

    napi_value argv[3] = {nullptr};
    std::string instanceName = "host";
    std::string methodName = "postMessage";
    std::string timeout = "timeout";
    napi_create_string_utf8(workerEnv, instanceName.c_str(), instanceName.length(), &argv[0]);
    napi_create_string_utf8(workerEnv, methodName.c_str(), methodName.length(), &argv[1]);
    napi_create_string_utf8(workerEnv, timeout.c_str(), timeout.length(), &argv[2]);

    std::string funcName = "GlobalCall";
    napi_value cb = nullptr;
    napi_value callResult = nullptr;
    // ------- workerEnv---------
    napi_create_function(workerEnv, funcName.c_str(), funcName.size(),
                         Worker::GlobalCall, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(workerEnv, workerGlobal, cb, sizeof(argv) / sizeof(argv[0]), argv, &callResult);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest045, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    // ------- workerEnv---------
    napi_env workerEnv = nullptr;
    napi_create_runtime(env, &workerEnv);
    napi_value workerGlobal = nullptr;
    napi_get_global(workerEnv, &workerGlobal);

    napi_value argv[3] = {nullptr};
    std::string instanceName = "host";
    std::string methodName = "postMessage";
    int32_t timeout = 300;
    napi_create_string_utf8(workerEnv, instanceName.c_str(), instanceName.length(), &argv[0]);
    napi_create_string_utf8(workerEnv, methodName.c_str(), methodName.length(), &argv[1]);
    napi_create_int32(workerEnv, timeout, &argv[2]);

    std::string funcName = "GlobalCall";
    napi_value cb = nullptr;
    napi_value callResult = nullptr;
    // ------- workerEnv---------
    napi_create_function(workerEnv, funcName.c_str(), funcName.size(),
                         Worker::GlobalCall, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    UpdateHostState(worker, Worker::HostState::INACTIVE);
    napi_call_function(workerEnv, workerGlobal, cb, sizeof(argv) / sizeof(argv[0]), argv, &callResult);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest046, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    // ------- workerEnv---------
    napi_env workerEnv = nullptr;
    napi_create_runtime(env, &workerEnv);
    napi_value workerGlobal = nullptr;
    napi_get_global(workerEnv, &workerGlobal);

    napi_value argv[3] = {nullptr};
    std::string instanceName = "host";
    std::string methodName = "postMessage";
    int32_t timeout = 300;
    napi_create_string_utf8(workerEnv, instanceName.c_str(), instanceName.length(), &argv[0]);
    napi_create_string_utf8(workerEnv, methodName.c_str(), methodName.length(), &argv[1]);
    napi_create_int32(workerEnv, timeout, &argv[2]);

    std::string funcName = "GlobalCall";
    napi_value cb = nullptr;
    napi_value callResult = nullptr;
    // ------- workerEnv---------
    napi_create_function(workerEnv, funcName.c_str(), funcName.size(),
                         Worker::GlobalCall, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    SetMainThread(worker, true);
    SetLimitedWorker(worker, false);
    napi_call_function(workerEnv, workerGlobal, cb, sizeof(argv) / sizeof(argv[0]), argv, &callResult);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    SetMainThread(worker, false);
    napi_call_function(workerEnv, workerGlobal, cb, sizeof(argv) / sizeof(argv[0]), argv, &callResult);
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    SetMainThread(worker, true);
    SetLimitedWorker(worker, true);
    napi_call_function(workerEnv, workerGlobal, cb, sizeof(argv) / sizeof(argv[0]), argv, &callResult);
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    worker->EraseWorker();
    SetLimitedWorker(worker, false);
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest047, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    napi_value argv[1] = { nullptr };
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);
    std::string funcName = "ParentPortCancelTask";
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::ParentPortCancelTask, nullptr, &cb);
    napi_call_function(env, nullptr, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest048, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    napi_value argv[1] = { nullptr };
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);
    std::string funcName = "ParentPortCancelTask";
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::ParentPortCancelTask, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::TERMINATED);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest049, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    napi_value argv[1] = { nullptr };
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);
    std::string funcName = "ParentPortCancelTask";
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::ParentPortCancelTask, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::TERMINATEING);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest050, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    napi_value argv[1] = { nullptr };
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);
    std::string funcName = "ParentPortCancelTask";
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::ParentPortCancelTask, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    SetWorkerHostEnv(worker, env, true);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    SetWorkerHostEnv(worker, env, false);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest051, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    std::string funcName = "ParentPortAddEventListener";
    napi_value cb = nullptr;
    napi_value callResult = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(),
                         Worker::ParentPortAddEventListener, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(env, global, cb, 0, nullptr, &callResult);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest052, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    napi_value argv[3] = {nullptr};
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);
    std::string funcName = "ParentPortAddEventListener";
    napi_create_string_utf8(env, funcName.c_str(), funcName.length(), &argv[1]);
    napi_value myobject = nullptr;
    napi_create_object(env, &myobject);
    argv[2] = myobject;
    napi_value cb = nullptr;
    napi_value callResult = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(),
                         Worker::ParentPortAddEventListener, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &callResult);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest053, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    napi_value argv[3] = {nullptr};
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);
    auto func = [](napi_env env, napi_callback_info info) -> napi_value {
        return nullptr;
    };
    std::string funcName = "ParentPortAddEventListener";
    napi_value cb = nullptr;
    napi_value funcValue = nullptr;
    napi_create_function(env, "testFunc", NAPI_AUTO_LENGTH, func, nullptr, &funcValue);
    argv[1] = funcValue;
    ASSERT_TRUE(funcValue != nullptr);
    napi_value myobject = nullptr;
    napi_create_object(env, &myobject);
    argv[2] = myobject;
    napi_value callResult = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(),
                         Worker::ParentPortAddEventListener, nullptr, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(env, nullptr, cb, sizeof(argv) / sizeof(argv[0]), argv, &callResult);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest054, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    napi_value argv[3] = {nullptr};
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);
    auto func = [](napi_env env, napi_callback_info info) -> napi_value {
        return nullptr;
    };
    std::string funcName = "ParentPortAddEventListener";
    napi_value cb = nullptr;
    napi_value funcValue = nullptr;
    napi_create_function(env, "testFunc", NAPI_AUTO_LENGTH, func, nullptr, &funcValue);
    argv[1] = funcValue;
    ASSERT_TRUE(funcValue != nullptr);
    napi_value myobject = nullptr;
    napi_create_object(env, &myobject);
    argv[2] = myobject;
    napi_value callResult = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(),
                         Worker::ParentPortAddEventListener, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::TERMINATED);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &callResult);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest055, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    napi_value argv[3] = {nullptr};
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);
    auto func = [](napi_env env, napi_callback_info info) -> napi_value {
        return nullptr;
    };
    std::string funcName = "ParentPortAddEventListener";
    napi_value cb = nullptr;
    napi_value funcValue = nullptr;
    napi_create_function(env, "testFunc", NAPI_AUTO_LENGTH, func, nullptr, &funcValue);
    argv[1] = funcValue;
    ASSERT_TRUE(funcValue != nullptr);
    napi_value myobject = nullptr;
    napi_create_object(env, &myobject);
    napi_value onceValue = NapiHelper::CreateBooleanValue(env, true);
    napi_set_named_property(env, myobject, "once", onceValue);
    argv[2] = myobject;
    napi_value callResult = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(),
                         Worker::ParentPortAddEventListener, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &callResult);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest056, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    napi_value argv[2] = {nullptr};
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);
    auto func = [](napi_env env, napi_callback_info info) -> napi_value {
        return nullptr;
    };
    std::string funcName = "ParentPortAddEventListener";
    napi_value cb = nullptr;
    napi_value funcValue = nullptr;
    napi_create_function(env, "testFunc", NAPI_AUTO_LENGTH, func, nullptr, &funcValue);
    argv[1] = funcValue;
    ASSERT_TRUE(funcValue != nullptr);
    napi_value callResult = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(),
                         Worker::ParentPortAddEventListener, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &callResult);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest057, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    napi_value argv[3] = {nullptr};
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);
    auto func = [](napi_env env, napi_callback_info info) -> napi_value {
        return nullptr;
    };
    std::string funcName = "ParentPortAddEventListener";
    napi_value cb = nullptr;
    napi_value funcValue = nullptr;
    napi_create_function(env, "testFunc", NAPI_AUTO_LENGTH, func, nullptr, &funcValue);
    argv[1] = funcValue;
    ASSERT_TRUE(funcValue != nullptr);
    napi_create_string_utf8(env, funcName.c_str(), funcName.length(), &argv[2]);
    napi_value callResult = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(),
                         Worker::ParentPortAddEventListener, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &callResult);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest058, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    // ------- workerEnv---------
    napi_env workerEnv = nullptr;
    napi_create_runtime(env, &workerEnv);
    napi_value workerGlobal = nullptr;
    napi_get_global(workerEnv, &workerGlobal);
    napi_value cb = nullptr;
    std::string funcName = "ParentPortDispatchEvent";
    napi_value callResult = nullptr;
    napi_create_function(workerEnv, funcName.c_str(), funcName.size(),
                         Worker::ParentPortDispatchEvent, worker, &cb);
    napi_call_function(workerEnv, workerGlobal, cb, 0, nullptr, &callResult);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest059, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    // ------- workerEnv---------
    napi_env workerEnv = nullptr;
    napi_create_runtime(env, &workerEnv);
    napi_value workerGlobal = nullptr;
    napi_get_global(workerEnv, &workerGlobal);

    napi_value argv[1] = {nullptr};
    napi_value cb = nullptr;
    std::string funcName = "ParentPortDispatchEvent";
    const char* messageKeyStr = "type";
    napi_create_string_latin1(workerEnv, messageKeyStr, strlen(messageKeyStr), &argv[0]);
    napi_value callResult = nullptr;
    napi_create_function(workerEnv, funcName.c_str(), funcName.size(),
                         Worker::ParentPortDispatchEvent, worker, &cb);
    napi_call_function(workerEnv, workerGlobal, cb, sizeof(argv) / sizeof(argv[0]), argv, &callResult);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest060, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    // ------- workerEnv---------
    napi_env workerEnv = nullptr;
    napi_create_runtime(env, &workerEnv);
    napi_value workerGlobal = nullptr;
    napi_get_global(workerEnv, &workerGlobal);

    napi_value argv[1] = {nullptr};

    napi_value cb = nullptr;
    std::string funcName = "ParentPortDispatchEvent";
    napi_value typeValue = nullptr;
    uint32_t type = 100; //test 100
    napi_create_uint32(env, type, &typeValue);
    napi_value object = nullptr;
    napi_create_object(workerEnv, &object);
    napi_set_named_property(workerEnv, object, "type", typeValue);
    argv[0] = object;
    napi_value callResult = nullptr;
    napi_create_function(workerEnv, funcName.c_str(), funcName.size(),
                         Worker::ParentPortDispatchEvent, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(workerEnv, workerGlobal, cb, sizeof(argv) / sizeof(argv[0]), argv, &callResult);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest061, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    // ------- workerEnv---------
    napi_env workerEnv = nullptr;
    napi_create_runtime(env, &workerEnv);
    napi_value workerGlobal = nullptr;
    napi_get_global(workerEnv, &workerGlobal);
    napi_value argv[1] = {nullptr};
    napi_value objresult = nullptr;
    napi_create_object(workerEnv, &objresult);
    napi_value cb = nullptr;
    std::string funcName = "ParentPortDispatchEvent";
    napi_value messageKey = nullptr;
    const char* messageKeyStr = "type";
    napi_create_string_latin1(workerEnv, messageKeyStr, strlen(messageKeyStr), &messageKey);
    napi_value messageValue = nullptr;
    const char* messageValueStr = "message";
    napi_create_string_latin1(workerEnv, messageValueStr, strlen(messageValueStr), &messageValue);
    napi_set_property(workerEnv, objresult, messageKey, messageValue);
    argv[0] = objresult;
    napi_value callResult = nullptr;
    napi_create_function(workerEnv, funcName.c_str(), funcName.size(),
                         Worker::ParentPortDispatchEvent, nullptr, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(workerEnv, nullptr, cb, sizeof(argv) / sizeof(argv[0]), argv, &callResult);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest062, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    // ------- workerEnv---------
    napi_env workerEnv = nullptr;
    napi_create_runtime(env, &workerEnv);
    napi_value workerGlobal = nullptr;
    napi_get_global(workerEnv, &workerGlobal);

    napi_value argv[1] = {nullptr};
    napi_value objresult = nullptr;
    napi_create_object(workerEnv, &objresult);
    napi_value cb = nullptr;
    std::string funcName = "ParentPortDispatchEvent";
    napi_value messageKey = nullptr;
    const char* messageKeyStr = "type";
    napi_create_string_latin1(workerEnv, messageKeyStr, strlen(messageKeyStr), &messageKey);
    napi_value messageValue = nullptr;
    const char* messageValueStr = "message";
    napi_create_string_latin1(workerEnv, messageValueStr, strlen(messageValueStr), &messageValue);
    napi_set_property(workerEnv, objresult, messageKey, messageValue);
    argv[0] = objresult;
    napi_value callResult = nullptr;
    napi_create_function(workerEnv, funcName.c_str(), funcName.size(),
                         Worker::ParentPortDispatchEvent, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::TERMINATED);
    napi_call_function(workerEnv, workerGlobal, cb, sizeof(argv) / sizeof(argv[0]), argv, &callResult);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest063, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));

    std::string funcName = "ParentPortRemoveEventListener";
    napi_value cb = nullptr;

    napi_create_function(env, funcName.c_str(), funcName.size(),
                         Worker::ParentPortRemoveEventListener, worker, &cb);
    napi_call_function(env, global, cb, 0, nullptr, &result);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest064, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));

    napi_value argv[2] = {nullptr};
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);
    auto func = [](napi_env env, napi_callback_info info) -> napi_value {
        return nullptr;
    };
    napi_value funcValue = nullptr;
    napi_create_function(env, "testFunc", NAPI_AUTO_LENGTH, func, nullptr, &funcValue);
    argv[1] = funcValue;

    std::string funcName = "ParentPortRemoveEventListener";
    napi_value cb = nullptr;

    napi_create_function(env, funcName.c_str(), funcName.size(),
                         Worker::ParentPortRemoveEventListener, nullptr, &cb);
    napi_call_function(env, nullptr, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest065, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));

    napi_value argv[2] = {nullptr};
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);
    auto func = [](napi_env env, napi_callback_info info) -> napi_value {
        return nullptr;
    };
    napi_value funcValue = nullptr;
    napi_create_function(env, "testFunc", NAPI_AUTO_LENGTH, func, nullptr, &funcValue);
    argv[1] = funcValue;

    std::string funcName = "ParentPortRemoveEventListener";
    napi_value cb = nullptr;

    napi_create_function(env, funcName.c_str(), funcName.size(),
                         Worker::ParentPortRemoveEventListener, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::TERMINATED);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest066, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));

    napi_value argv[2] = {nullptr};
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);

    std::string funcName = "ParentPortRemoveEventListener";
    napi_create_string_utf8(env, funcName.c_str(), funcName.length(), &argv[1]);
    napi_value cb = nullptr;

    napi_create_function(env, funcName.c_str(), funcName.size(),
                         Worker::ParentPortRemoveEventListener, worker, &cb);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest067, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));

    napi_value argv[1] = {nullptr};
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);

    std::string funcName = "ParentPortRemoveEventListener";
    napi_value cb = nullptr;

    napi_create_function(env, funcName.c_str(), funcName.size(),
                         Worker::ParentPortRemoveEventListener, worker, &cb);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest068, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    // ------- workerEnv---------
    napi_env workerEnv = nullptr;
    napi_create_runtime(env, &workerEnv);
    napi_value workerGlobal = nullptr;
    napi_get_global(workerEnv, &workerGlobal);
    napi_value argv[1] = {nullptr};
    std::string message = "host";
    napi_create_string_utf8(workerEnv, message.c_str(), message.length(), &argv[0]);
    std::string funcName = "ParentPortRemoveAllListener";
    napi_value cb = nullptr;

    napi_value callResult = nullptr;
    // ------- workerEnv---------
    napi_create_function(workerEnv, funcName.c_str(), funcName.size(),
                         Worker::ParentPortRemoveAllListener, nullptr, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::RUNNING);
    napi_call_function(workerEnv, nullptr, cb, sizeof(argv) / sizeof(argv[0]), argv, &callResult);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest069, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    // ------- workerEnv---------
    napi_env workerEnv = nullptr;
    napi_create_runtime(env, &workerEnv);
    napi_value workerGlobal = nullptr;
    napi_get_global(workerEnv, &workerGlobal);
    napi_value argv[1] = {nullptr};
    std::string message = "host";
    napi_create_string_utf8(workerEnv, message.c_str(), message.length(), &argv[0]);
    std::string funcName = "ParentPortRemoveAllListener";
    napi_value cb = nullptr;

    napi_value callResult = nullptr;
    // ------- workerEnv---------
    napi_create_function(workerEnv, funcName.c_str(), funcName.size(),
                         Worker::ParentPortRemoveAllListener, worker, &cb);
    UpdateWorkerState(worker, Worker::RunnerState::TERMINATED);
    napi_call_function(workerEnv, workerGlobal, cb, sizeof(argv) / sizeof(argv[0]), argv, &callResult);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest070, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    RemoveGlobalCallObject(worker, env);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
}

HWTEST_F(WorkersTest, WorkerTest071, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    TestHostOnMessageInner(worker, env);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest072, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    HostOnGlobalCall(worker, env);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest073, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    HostOnGlobalCallInner001(worker, env);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest074, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    HostOnGlobalCallInner002(worker, env);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest075, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    HostOnGlobalCallInner003(worker, env);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest076, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    HostOnGlobalCallInner004(worker, env);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception != nullptr);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest077, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    HostOnGlobalCallInner005(worker, env);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest078, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    HostOnGlobalCallInner006(worker, env);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest079, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    HandleGlobalCall(worker, env);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest080, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    HostOnError(worker, env);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest081, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    HostOnErrorInner(worker, env);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest082, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    HostOnMessageErrorInner(worker, env);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest083, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    PublishWorkerOverSignal(worker, env);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    ClearWorkerHandle(worker);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest084, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    TestWorkerOnMessageInner(worker, env);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest085, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    HandleEventListeners(worker, env);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest086, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    HandleHostException(worker, env);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest087, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    PostMessageToHostInner(worker, env);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest088, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    RemoveListenerInner(worker, env);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest089, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    ParentPortAddListenerInnerTest(worker, env);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest090, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    ParentPortHandleEventListeners(worker, env);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest091, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    DebuggerOnPostTask(worker, env);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, WorkerTest092, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    CloseHostHandle(worker, env);
    worker->EraseWorker();
    ClearWorkerHandle(worker);
    napi_value exception = nullptr;
    napi_get_and_clear_last_exception(env, &exception);
    ASSERT_TRUE(exception == nullptr);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

HWTEST_F(WorkersTest, PostMessageTest004, testing::ext::TestSize.Level0)
{
    napi_env env = WorkersTest::GetEnv();
    Worker *worker = new Worker(env, nullptr);
    UpdateMainThreadWorkerFlag(worker, false);
    ASSERT_TRUE(worker != nullptr);
    uv_loop_t *loop = nullptr;
    napi_status status = napi_get_uv_event_loop(env, &loop);
    ASSERT_TRUE(status == napi_ok);
    InitHostHandle(worker, loop);

    std::thread t(WorkerThreadFunction, worker);
    t.detach();
    uint32_t number = 200; // 200 : test number
    napi_value numVal = nullptr;
    status = napi_create_uint32(env, number, &numVal);
    ASSERT_TRUE(status == napi_ok);
    void *data = nullptr;
    napi_value undefined = nullptr;
    napi_get_undefined(env, &undefined);
    status = napi_serialize(env, numVal, undefined, undefined, &data);
    ASSERT_TRUE(status == napi_ok);
    uv_sleep(1000); // 1000 : for post and receive message
    PostMessage(worker, data);
    uv_sleep(1000); // 1000 : for post and receive message
}