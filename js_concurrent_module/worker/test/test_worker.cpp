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

#include "test.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "tools/log.h"
#include "worker.h"

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

// worker constructor
napi_value Worker_Constructor(napi_env env, napi_value global)
{
    std::string funcName = "WorkerConstructor";
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::WorkerConstructor, nullptr, &cb);

    napi_value result = nullptr;
    napi_value argv[2] = { nullptr };

    std::string script = "entry/ets/workers/worker.ts";
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
HWTEST_F(NativeEngineTest, WorkerConstructorTest001, testing::ext::TestSize.Level0)
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
    ASSERT_EQ(scriptResult, "entry/ets/workers/worker.ts");

    result = Worker_Terminate(env, global);

    ASSERT_TRUE(result != nullptr);
}

//worker PostMessage
HWTEST_F(NativeEngineTest, PostMessageTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);

    napi_value result = nullptr;
    result = Worker_Constructor(env, global);

    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    bool status = worker->UpdateWorkerState(Worker::RunnerState::RUNNING);

    napi_value argv[1] = { nullptr };
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);
    std::string funcName = "PostMessage";
    napi_value cb = nullptr;
    if (status) {
        napi_create_function(env, funcName.c_str(), funcName.size(), Worker::PostMessage, worker, &cb);
        napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    }
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

//worker PostMessage
HWTEST_F(NativeEngineTest, PostMessageTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);

    napi_value result = nullptr;
    result = Worker_Constructor(env, global);

    napi_value argv[1] = { nullptr };
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);

    std::string funcName = "PostMessage";
    napi_value cb = nullptr;

    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::PostMessage, nullptr, &cb);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);

    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

//worker PostMessage
HWTEST_F(NativeEngineTest, PostMessageTest003, testing::ext::TestSize.Level0)
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

    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

//worker PostMessage
HWTEST_F(NativeEngineTest, PostMessageToHostTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);

    napi_value result = nullptr;
    result = Worker_Constructor(env, global);

    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    bool status = worker->UpdateWorkerState(Worker::RunnerState::RUNNING);
    napi_value argv[1] = { nullptr };
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);

    std::string funcName = "PostMessageToHost";
    napi_value cb = nullptr;
    if (status) {
        napi_create_function(env, funcName.c_str(), funcName.size(), Worker::PostMessageToHost, worker, &cb);
        napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    }
    uv_async_t* req = new uv_async_t;
    req->data = worker;
    Worker::HostOnMessage(req);

    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

//worker PostMessageToHost
HWTEST_F(NativeEngineTest, PostMessageToHostTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);

    napi_value result = nullptr;
    result = Worker_Constructor(env, global);

    napi_value argv[1] = { nullptr };
    std::string message = "host";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);

    std::string funcName = "PostMessageToHost";
    napi_value cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::PostMessageToHost, nullptr, &cb);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);

    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

//worker PostMessageToHost
HWTEST_F(NativeEngineTest, PostMessageToHostTest003, testing::ext::TestSize.Level0)
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
    bool status = worker->UpdateWorkerState(Worker::RunnerState::RUNNING);
    napi_value argv[2] = { nullptr };
    std::string message = "";
    napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);
    std::string funcName = "PostMessageToHost";
    argv[1] = propNames;
    napi_value cb = nullptr;
    if (status) {
        napi_create_function(env, funcName.c_str(), funcName.size(), Worker::PostMessageToHost, worker, &cb);
        napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    }
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

//worker EventListener
HWTEST_F(NativeEngineTest, EventListenerTest001, testing::ext::TestSize.Level0)
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

    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

//worker EventListener
HWTEST_F(NativeEngineTest, EventListenerTest002, testing::ext::TestSize.Level0)
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
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

//worker DispatchEvent
HWTEST_F(NativeEngineTest, DispatchEventTest001, testing::ext::TestSize.Level0)
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
    std::string type = "zhangsan";
    napi_create_string_utf8(env, type.c_str(), type.length(), &typeValue);

    napi_value object = nullptr;
    napi_create_object(env, &object);
    napi_set_named_property(env, object, "type", typeValue);
    argv[0] = object;

    funcName = "DispatchEvent";
    cb = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::DispatchEvent, worker, &cb);
    napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);

    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

//worker ParentPortAddEventListener
HWTEST_F(NativeEngineTest, ParentPortAddEventListenerTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    bool status = worker->UpdateWorkerState(Worker::RunnerState::RUNNING);
    // ------- workerEnv---------
    napi_env workerEnv = nullptr;
    napi_create_runtime(env, &workerEnv);
    napi_value workerGlobal = nullptr;
    napi_get_global(workerEnv, &workerGlobal);

    napi_value argv[3] = {nullptr};
    std::string message = "host";
    napi_create_string_utf8(workerEnv, message.c_str(), message.length(), &argv[0]);
    auto func = [](napi_env env, napi_callback_info info) -> napi_value {
        return nullptr;
    };

    std::string funcName = "ParentPortAddEventListener";
    napi_value cb = nullptr;
    napi_value funcValue = nullptr;
    napi_create_function(workerEnv, "testFunc", NAPI_AUTO_LENGTH, func, nullptr, &funcValue);
    argv[1] = funcValue;
    napi_value myobject = nullptr;
    napi_create_object(workerEnv, &myobject);
    argv[2] = myobject;
    napi_value callResult = nullptr;
    if (status) {
        napi_create_function(workerEnv, funcName.c_str(), funcName.size(),
                             Worker::ParentPortAddEventListener, worker, &cb);
        napi_call_function(workerEnv, workerGlobal, cb, sizeof(argv) / sizeof(argv[0]), argv, &callResult);
    }
    // ------- workerEnv---------
    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

//worker ParentPortRemoveAllListener
HWTEST_F(NativeEngineTest, ParentPortRemoveAllListenerTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    bool status = worker->UpdateWorkerState(Worker::RunnerState::RUNNING);

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
    if (status) {
        napi_create_function(workerEnv, funcName.c_str(), funcName.size(),
                             Worker::ParentPortRemoveAllListener, worker, &cb);
        napi_call_function(workerEnv, workerGlobal, cb, sizeof(argv) / sizeof(argv[0]), argv, &callResult);
    }
    // ------- workerEnv---------

    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

//worker ParentPortDispatchEvent
HWTEST_F(NativeEngineTest, ParentPortDispatchEventTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    bool status = worker->UpdateWorkerState(Worker::RunnerState::RUNNING);

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
    if (status) {
        napi_create_function(workerEnv, funcName.c_str(), funcName.size(),
                             Worker::ParentPortDispatchEvent, worker, &cb);
        napi_call_function(workerEnv, workerGlobal, cb, sizeof(argv) / sizeof(argv[0]), argv, &callResult);
    }
    // ------- workerEnv---------

    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}

//worker ParentPortRemoveEventListener
HWTEST_F(NativeEngineTest, ParentPortRemoveEventListenerTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value global;
    napi_get_global(env, &global);
    napi_value result = nullptr;
    result = Worker_Constructor(env, global);
    Worker* worker = nullptr;
    napi_unwrap(env, result, reinterpret_cast<void**>(&worker));
    bool status = worker->UpdateWorkerState(Worker::RunnerState::RUNNING);

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
    if (status) {
        napi_create_function(workerEnv, funcName.c_str(), funcName.size(),
                             Worker::ParentPortAddEventListener, worker, &cb);
        napi_call_function(workerEnv, workerGlobal, cb, sizeof(argv) / sizeof(argv[0]), argv, &callResult);
    }
    // ------- workerEnv---------

    result = Worker_Terminate(env, global);
    ASSERT_TRUE(result != nullptr);
}