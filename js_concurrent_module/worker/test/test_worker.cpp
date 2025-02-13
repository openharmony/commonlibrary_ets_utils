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

#include <condition_variable>
#include <thread>
#include <uv.h>
#include <mutex>
#include <vector>

#include "ark_native_engine.h"
#include "message_queue.h"
#include "test.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "../../common/helper/napi_helper.h"
#include "native_engine/native_create_env.h"
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

static constexpr int MAX_WORKERS_COUNT = 64;
static constexpr int MAX_ARKRUNTIME_COUNT = 64;
static constexpr int MAX_JSTHREAD_COUNT = 80;
static constexpr int NUM_10 = 10;
static constexpr int NUM_1 = 1;
static constexpr int TIME_1000MS = 1000;
static constexpr int TIME_500MS = 500;
static constexpr int TIME_100MS = 100;

class ArkRuntimeChecker {
public:
    ArkRuntimeChecker() = default;
    static void CreateArkRuntimeEngine(napi_env& workerEnv);
    void CreateArkRuntimeList(uint32_t runtimeCount);
    void DeleteArkRuntimeList();
    int32_t GetFailedCount();

    std::mutex mutexlock_;
    std::condition_variable condition_;
    std::thread* threadArray_[MAX_ARKRUNTIME_COUNT + NUM_10] = { nullptr };
    std::atomic<uint32_t> failCount_{0};
    bool notified_ = false;
};

void ArkRuntimeChecker::CreateArkRuntimeEngine(napi_env& workerEnv)
{
    panda::RuntimeOption option;
    option.SetGcType(panda::RuntimeOption::GC_TYPE::GEN_GC);
    const int64_t poolSize = 0x1000000;
    option.SetGcPoolSize(poolSize);
    option.SetLogLevel(panda::RuntimeOption::LOG_LEVEL::ERROR);
    option.SetDebuggerLibraryPath("");
    panda::ecmascript::EcmaVM* vm = panda::JSNApi::CreateJSVM(option);
    ASSERT_NE(vm, nullptr);
    workerEnv = reinterpret_cast<napi_env>(new (std::nothrow) ArkNativeEngine(vm, nullptr));
    auto cleanEnv = [vm]() {
        if (vm != nullptr) {
            panda::JSNApi::DestroyJSVM(vm);
        }
    };
    ArkNativeEngine* arkEngine = (ArkNativeEngine*)workerEnv;
    arkEngine->SetCleanEnv(cleanEnv);
}

void ArkRuntimeChecker::CreateArkRuntimeList(uint32_t runtimeCount)
{
    NativeCreateEnv::RegCreateNapiEnvCallback([] (napi_env *env) {
        ArkRuntimeChecker::CreateArkRuntimeEngine(*env);
        return napi_status::napi_ok;
    });
    NativeCreateEnv::RegDestroyNapiEnvCallback([] (napi_env *env) {
        ArkNativeEngine* engine = reinterpret_cast<ArkNativeEngine*>(*env);
        delete engine;
        return napi_status::napi_ok;
    });
    for (int k = 0; k < runtimeCount; k++) {
        threadArray_[k] = new std::thread([this] {
            napi_env newenv = nullptr;
            napi_create_ark_runtime(&newenv);
            if (newenv == nullptr) {
                this->failCount_++;
                return;
            }
            std::unique_lock<std::mutex> lock(this->mutexlock_);
            if (!this->notified_) {
                this->condition_.wait(lock);
            }
            if (newenv != nullptr) {
                napi_destroy_ark_runtime(&newenv);
            }
        });
    }
    uv_sleep(TIME_1000MS);
}

void ArkRuntimeChecker::DeleteArkRuntimeList()
{
    mutexlock_.lock();
    notified_ = true;
    condition_.notify_all();
    mutexlock_.unlock();
    for (int k = 0; k < MAX_ARKRUNTIME_COUNT + NUM_10; k++) {
        if (threadArray_[k] != nullptr) {
            threadArray_[k]->join();
        }
    }
}

int32_t ArkRuntimeChecker::GetFailedCount()
{
    return failCount_.load();
}

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

class WorkerChecker {
public:
    WorkerChecker() = default;
    void CreateWorkerHostEngine(napi_env& workerEnv);
    static napi_value CreateWorker(bool limited, napi_env workerEnv, napi_value workerGlobal);
    void CreateWorkerList(bool limited, uint32_t workerNum);
    void DeleteWorkerList();
    int32_t GetFailedCount();

    napi_env workerHostEnv_{nullptr};
    std::atomic<uint32_t> failCount_{0};
    std::vector<Worker*> workerlist_;

    bool notified_ = false;
    std::mutex mutexlock_;
    std::condition_variable condition_;
};

void WorkerChecker::CreateWorkerHostEngine(napi_env& workerEnv)
{
    panda::RuntimeOption option;
    option.SetGcType(panda::RuntimeOption::GC_TYPE::GEN_GC);
    const int64_t poolSize = 0x1000000;
    option.SetGcPoolSize(poolSize);
    option.SetLogLevel(panda::RuntimeOption::LOG_LEVEL::ERROR);
    option.SetDebuggerLibraryPath("");
    panda::ecmascript::EcmaVM* vm = panda::JSNApi::CreateJSVM(option);
    ASSERT_NE(vm, nullptr);

    workerEnv = reinterpret_cast<napi_env>(new (std::nothrow) ArkNativeEngine(vm, nullptr));
    ArkNativeEngine* arkEngine = (ArkNativeEngine*)workerEnv;
    arkEngine->SetInitWorkerFunc([this](NativeEngine*) {
        std::unique_lock<std::mutex> lock(this->mutexlock_);
        if (!this->notified_) {
            this->condition_.wait(lock);
        }
    });
    auto cleanEnv = [vm]() {
        if (vm != nullptr) {
            panda::JSNApi::DestroyJSVM(vm);
        }
    };
    arkEngine->SetCleanEnv(cleanEnv);
}

napi_value WorkerChecker::CreateWorker(bool limited, napi_env workerEnv, napi_value workerGlobal)
{
    std::string funcName = "ThreadWorkerConstructor";
    if (limited) {
        funcName = "LimitedWorkerConstructor";
    }
    napi_value cb = nullptr;
    if (!limited) {
        napi_create_function(workerEnv, funcName.c_str(), funcName.size(),
            Worker::ThreadWorkerConstructor, nullptr, &cb);
    } else {
        napi_create_function(workerEnv, funcName.c_str(), funcName.size(),
            Worker::LimitedWorkerConstructor, nullptr, &cb);
    }
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
    return result;
}

void WorkerChecker::CreateWorkerList(bool limited, uint32_t workerNum)
{
    CreateWorkerHostEngine(workerHostEnv_);
    ASSERT_NE(workerHostEnv_, nullptr);
    NativeEngine::GetMainThreadEngine()->CheckAndSetWorkerVersion(WorkerVersion::OLD, WorkerVersion::NONE);
    napi_value workerGlobal = nullptr;
    napi_get_global(workerHostEnv_, &workerGlobal);

    for (int k = 0; k < workerNum; k++) {
        TryCatch tryCatch(workerHostEnv_);
        napi_value result = WorkerChecker::CreateWorker(limited, workerHostEnv_, workerGlobal);
        if (tryCatch.HasCaught()) {
            tryCatch.ClearException();
        }
        if (result == nullptr) {
            failCount_++;
        } else {
            Worker* worker = nullptr;
            napi_unwrap(workerHostEnv_, result, reinterpret_cast<void**>(&worker));
            if (worker != nullptr) {
                workerlist_.push_back(worker);
            }
        }
    }
    uv_sleep(TIME_100MS);
}

void WorkerChecker::DeleteWorkerList()
{
    ArkNativeEngine* workerHostEngine = reinterpret_cast<ArkNativeEngine*>(workerHostEnv_);
    mutexlock_.lock();
    notified_ = true;
    condition_.notify_all();
    mutexlock_.unlock();
    uv_sleep(TIME_500MS);

    for (auto worker : workerlist_) {
        worker->EraseWorker();
        WorkersTest::ClearWorkerHandle(worker);
    }
    workerlist_.clear();
    napi_value workerGlobal = nullptr;
    napi_get_global(workerHostEnv_, &workerGlobal);
    Worker_Terminate(workerHostEnv_, workerGlobal);
    NativeEngine::GetMainThreadEngine()->CheckAndSetWorkerVersion(WorkerVersion::NEW, WorkerVersion::OLD);
    delete workerHostEngine;
}

int32_t WorkerChecker::GetFailedCount()
{
    return failCount_.load();
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
    SetCloseWorkerProp(worker, env);
    napi_call_function(env, global, cb, 0, nullptr, &result);
    worker->EraseWorker();
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

HWTEST_F(WorkersTest, MaxLimitTest0010, testing::ext::TestSize.Level0)
{
    auto checker = std::make_shared<WorkerChecker>();
    checker->CreateWorkerList(false, MAX_WORKERS_COUNT + NUM_10);
    int32_t failed = checker->GetFailedCount();
    checker->DeleteWorkerList();
    ASSERT_EQ(failed, NUM_10);
}

HWTEST_F(WorkersTest, MaxLimitTest0011, testing::ext::TestSize.Level0)
{
    auto checker = std::make_shared<WorkerChecker>();
    checker->CreateWorkerList(false, MAX_WORKERS_COUNT - NUM_1);
    int32_t failed = checker->GetFailedCount();
    ASSERT_EQ(failed, 0);

    auto checker2 = std::make_shared<WorkerChecker>();
    checker2->CreateWorkerList(false, NUM_1);
    failed = checker2->GetFailedCount();
    ASSERT_EQ(failed, 0);

    auto checker3 = std::make_shared<WorkerChecker>();
    checker3->CreateWorkerList(false, NUM_1);
    failed = checker3->GetFailedCount();
    ASSERT_EQ(failed, NUM_1);
    checker3->DeleteWorkerList();

    checker2->DeleteWorkerList();
    auto checker4 = std::make_shared<WorkerChecker>();
    checker4->CreateWorkerList(false, NUM_1);
    failed = checker4->GetFailedCount();
    ASSERT_EQ(failed, 0);
    checker4->DeleteWorkerList();

    checker->DeleteWorkerList();
}

HWTEST_F(WorkersTest, MaxLimitTest0020, testing::ext::TestSize.Level0)
{
    auto checker = std::make_shared<ArkRuntimeChecker>();
    checker->CreateArkRuntimeList(MAX_ARKRUNTIME_COUNT + NUM_10);
    int32_t failed = checker->GetFailedCount();
    checker->DeleteArkRuntimeList();
    ASSERT_EQ(failed, NUM_10);
}

HWTEST_F(WorkersTest, MaxLimitTest0021, testing::ext::TestSize.Level0)
{
    auto checker = std::make_shared<ArkRuntimeChecker>();
    checker->CreateArkRuntimeList(MAX_ARKRUNTIME_COUNT - NUM_1);
    int32_t failed = checker->GetFailedCount();
    ASSERT_EQ(failed, 0);

    auto checker2 = std::make_shared<ArkRuntimeChecker>();
    checker2->CreateArkRuntimeList(NUM_10);
    failed = checker2->GetFailedCount();
    ASSERT_EQ(failed, NUM_10 - NUM_1);

    auto checker3 = std::make_shared<ArkRuntimeChecker>();
    checker3->CreateArkRuntimeList(NUM_1);
    failed = checker3->GetFailedCount();
    ASSERT_EQ(failed, NUM_1);

    checker3->DeleteArkRuntimeList();
    checker2->DeleteArkRuntimeList();

    auto checker4 = std::make_shared<ArkRuntimeChecker>();
    checker4->CreateArkRuntimeList(NUM_1);
    failed = checker4->GetFailedCount();
    ASSERT_EQ(failed, 0);
    checker4->DeleteArkRuntimeList();

    checker->DeleteArkRuntimeList();
}

HWTEST_F(WorkersTest, MaxLimitTest0030, testing::ext::TestSize.Level0)
{
    auto arkRuntimeChecker = std::make_shared<ArkRuntimeChecker>();
    arkRuntimeChecker->CreateArkRuntimeList(MAX_ARKRUNTIME_COUNT + NUM_10);
    int32_t failed1 = arkRuntimeChecker->GetFailedCount();
    auto workerChecker = std::make_shared<WorkerChecker>();
    workerChecker->CreateWorkerList(false, MAX_WORKERS_COUNT);
    int32_t failed2 = workerChecker->GetFailedCount();
    workerChecker->DeleteWorkerList();
    ASSERT_EQ(failed2, MAX_WORKERS_COUNT + MAX_ARKRUNTIME_COUNT - MAX_JSTHREAD_COUNT);
    arkRuntimeChecker->DeleteArkRuntimeList();
    ASSERT_EQ(failed1, NUM_10);
}

HWTEST_F(WorkersTest, MaxLimitTest0031, testing::ext::TestSize.Level0)
{
    auto arkRuntimeChecker = std::make_shared<ArkRuntimeChecker>();
    arkRuntimeChecker->CreateArkRuntimeList(MAX_ARKRUNTIME_COUNT + NUM_10);
    int32_t failed1 = arkRuntimeChecker->GetFailedCount();
    auto workerChecker2 = std::make_shared<WorkerChecker>();
    workerChecker2->CreateWorkerList(false, MAX_JSTHREAD_COUNT - MAX_ARKRUNTIME_COUNT - NUM_10);
    int32_t failed2 = workerChecker2->GetFailedCount();

    auto workerChecker3 = std::make_shared<WorkerChecker>();
    workerChecker3->CreateWorkerList(true, NUM_10 + NUM_1);
    int32_t failed3 = workerChecker3->GetFailedCount();
    ASSERT_EQ(failed3, NUM_1);
    workerChecker3->DeleteWorkerList();

    workerChecker2->DeleteWorkerList();
    ASSERT_EQ(failed2, 0);
    arkRuntimeChecker->DeleteArkRuntimeList();
    ASSERT_EQ(failed1, NUM_10);
}

HWTEST_F(WorkersTest, MaxLimitTest004, testing::ext::TestSize.Level0)
{
    auto workerChecker = std::make_shared<WorkerChecker>();
    workerChecker->CreateWorkerList(false, MAX_WORKERS_COUNT + NUM_10);
    int32_t failed1 = workerChecker->GetFailedCount();
    auto arkRuntimeChecker = std::make_shared<ArkRuntimeChecker>();
    arkRuntimeChecker->CreateArkRuntimeList(MAX_ARKRUNTIME_COUNT);
    int32_t failed2 = arkRuntimeChecker->GetFailedCount();
    workerChecker->DeleteWorkerList();
    ASSERT_EQ(failed1, NUM_10);
    arkRuntimeChecker->DeleteArkRuntimeList();
    ASSERT_EQ(failed2, MAX_WORKERS_COUNT + MAX_ARKRUNTIME_COUNT - MAX_JSTHREAD_COUNT);
}

HWTEST_F(WorkersTest, MaxLimitTest005, testing::ext::TestSize.Level0)
{
    auto workerChecker = std::make_shared<WorkerChecker>();
    workerChecker->CreateWorkerList(false, MAX_WORKERS_COUNT - NUM_10);
    int32_t failed1 = workerChecker->GetFailedCount();
    ASSERT_EQ(failed1, 0);

    auto arkRuntimeChecker = std::make_shared<ArkRuntimeChecker>();
    arkRuntimeChecker->CreateArkRuntimeList(MAX_JSTHREAD_COUNT - MAX_WORKERS_COUNT + NUM_10 - NUM_1);
    int32_t failed2 = arkRuntimeChecker->GetFailedCount();
    ASSERT_EQ(failed2, 0);

    auto arkRuntimeChecker2 = std::make_shared<ArkRuntimeChecker>();
    arkRuntimeChecker2->CreateArkRuntimeList(NUM_10);
    failed2 = arkRuntimeChecker2->GetFailedCount();
    ASSERT_EQ(failed2, NUM_10 - NUM_1);
    arkRuntimeChecker2->DeleteArkRuntimeList();

    auto arkRuntimeChecker3 = std::make_shared<ArkRuntimeChecker>();
    arkRuntimeChecker3->CreateArkRuntimeList(NUM_1);
    failed2 = arkRuntimeChecker3->GetFailedCount();
    ASSERT_EQ(failed2, 0);

    auto arkRuntimeChecker4 = std::make_shared<ArkRuntimeChecker>();
    arkRuntimeChecker4->CreateArkRuntimeList(NUM_1);
    failed2 = arkRuntimeChecker4->GetFailedCount();
    ASSERT_EQ(failed2, NUM_1);

    arkRuntimeChecker3->DeleteArkRuntimeList();
    arkRuntimeChecker4->DeleteArkRuntimeList();

    workerChecker->DeleteWorkerList();
    arkRuntimeChecker->DeleteArkRuntimeList();
}