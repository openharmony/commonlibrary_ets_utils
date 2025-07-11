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
#include <unistd.h>
#include <sys/syscall.h>

#include <ctime>
#include <latch>
#include <thread>
#include <gtest/gtest.h>

#include "ark_native_engine.h"
#include "locks/async_lock.h"
#include "locks/async_lock_manager.h"
#include "locks/lock_request.h"
#include "test/unittest/common/test_common.h"

using namespace Commonlibrary::Concurrent::LocksModule;

class LocksTest : public testing::Test {
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
        InitLocks();
    }

    static void InitLocks()
    {
        napi_env env {GetEnv()};
        napi_value exports;
        ASSERT_CHECK_CALL(napi_create_object(env, &exports));
        AsyncLockManager::Init(env, exports);

        napi_value locks;
        ASSERT_CHECK_CALL(napi_get_named_property(env, exports, "locks", &locks));
        ASSERT_CHECK_CALL(napi_get_named_property(env, locks, "AsyncLock", &asyncLockClass_));
        ASSERT_CHECK_CALL(napi_get_named_property(env, locks, "AsyncLockOptions", &asyncLockOptions_));
        ASSERT_CHECK_CALL(napi_get_named_property(env, asyncLockClass_, "request", &asyncLockRequest_));
        ASSERT_CHECK_CALL(napi_create_int32(env, LockMode::LOCK_MODE_SHARED, &sharedMode_));
        ASSERT_CHECK_CALL(napi_create_int32(env, LockMode::LOCK_MODE_EXCLUSIVE, &exclusiveMode_));
        ASSERT_CHECK_CALL(napi_get_undefined(env, &undefined_));
    }

    static void TriggerGC()
    {
        panda::JSNApi::TriggerGC(vm_, panda::JSNApi::TRIGGER_GC_TYPE::FULL_GC);
        panda::JSNApi::TriggerGC(vm_, panda::JSNApi::TRIGGER_GC_TYPE::SHARED_FULL_GC);
    }

    static const uint64_t defaultTimeout {100};

    void SetUp() override
    {
        napi_env env {GetEnv()};
        ASSERT_CHECK_CALL(napi_open_handle_scope(env, &scope_));
    }

    void TearDown() override
    {
        napi_env env {GetEnv()};
        napi_value exception;
        ASSERT_CHECK_CALL(napi_get_and_clear_last_exception(env, &exception));
        ASSERT_CHECK_CALL(napi_close_handle_scope(env, scope_));
    }

    static void DestroyEngine()
    {
        delete engine_;
        engine_ = nullptr;
        panda::JSNApi::DestroyJSVM(vm_);
    }

    static napi_env GetEnv()
    {
        return reinterpret_cast<napi_env>(engine_);
    }

    static void Loop(LoopMode mode)
    {
        engine_->Loop(mode);
    }

    template <typename P>
    static void LoopUntil(const P &pred)
    {
        static constexpr size_t timeoutNs = 10000000UL;
        timespec timeout = {0, timeoutNs};
        while (!pred()) {
            Loop(LOOP_NOWAIT);
            nanosleep(&timeout, nullptr);
        }
    }

    static napi_value CreateFunction(const char *name, napi_value (*callback)(napi_env, napi_callback_info),
        void *data = nullptr)
    {
        napi_value result;
        napi_status status = napi_create_function(GetEnv(), name, NAPI_AUTO_LENGTH, callback, data, &result);
        EXPECT_EQ(status, napi_ok);
        return result;
    }

    static void Sleep()
    {
        timespec ts{0, 100U * 1000U * 1000U}; // 100ms
        nanosleep(&ts, nullptr);
    }

protected:
    static thread_local NativeEngine *engine_;
    static thread_local EcmaVM *vm_;
    static thread_local napi_value asyncLockClass_;
    static thread_local napi_value asyncLockOptions_;
    static thread_local napi_value asyncLockRequest_;
    static thread_local napi_value sharedMode_;
    static thread_local napi_value exclusiveMode_;
    static thread_local napi_value undefined_;

private:
    napi_handle_scope scope_ {nullptr};
};

thread_local NativeEngine *LocksTest::engine_ = nullptr;
thread_local EcmaVM *LocksTest::vm_ = nullptr;
thread_local napi_value LocksTest::asyncLockClass_ {nullptr};
thread_local napi_value LocksTest::asyncLockOptions_ {nullptr};
thread_local napi_value LocksTest::asyncLockRequest_ {nullptr};
thread_local napi_value LocksTest::sharedMode_ {nullptr};
thread_local napi_value LocksTest::exclusiveMode_ {nullptr};
thread_local napi_value LocksTest::undefined_ {nullptr};

static napi_value ExclusiveLockSingleCb(napi_env env, napi_callback_info info)
{
    bool *isCalled = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, nullptr, reinterpret_cast<void **>(&isCalled));
    *isCalled = true;
    napi_value undefined;
    napi_get_undefined(env, &undefined);
    return undefined;
}

TEST_F(LocksTest, ExclusiveLockSingle)
{
    napi_env env = GetEnv();
    std::unique_ptr<AsyncLock> lock = std::make_unique<AsyncLock>(1);
    bool isCalled = false;
    napi_value callback = CreateFunction("exclusivelocksingle", ExclusiveLockSingleCb, &isCalled);
    napi_ref callback_ref;
    napi_create_reference(env, callback, 1, &callback_ref);

    LockOptions options;
    napi_value result = lock->LockAsync(env, callback_ref, LOCK_MODE_EXCLUSIVE, options);
    bool isPromise = false;
    napi_is_promise(env, result, &isPromise);
    ASSERT_TRUE(isPromise);
    Loop(LOOP_ONCE);
    ASSERT_TRUE(isCalled);
}

struct CallbackData {
    std::atomic<bool> executing = false;
    std::atomic<bool> fail = false;
    std::atomic<uint32_t> callCount = 0;
};

static napi_value ExclusiveLockMultiCb(napi_env env, napi_callback_info info)
{
    napi_value undefined;
    napi_get_undefined(env, &undefined);
    CallbackData *data = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, nullptr, reinterpret_cast<void **>(&data));
    data->callCount += 1;
    bool prev = data->executing.exchange(true);
    if (prev) {
        // The callback is executing now by another thread.
        // Fail the test
        data->fail = true;
        return undefined;
    }

    LocksTest::Sleep();

    data->executing = false;
    return undefined;
}

TEST_F(LocksTest, ExclusiveLockMulti)
{
    std::unique_ptr<AsyncLock> lock = std::make_unique<AsyncLock>(1);
    AsyncLock *lockPtr = lock.get();
    CallbackData callbackData;
    std::thread t([lockPtr, &callbackData] () {
        LocksTest::InitializeEngine();
        napi_env env = GetEnv();
        napi_value callback = CreateFunction("exclusivelockmulti", ExclusiveLockMultiCb, &callbackData);
        napi_ref callback_ref;
        napi_create_reference(env, callback, 1, &callback_ref);
        LockOptions options;
        lockPtr->LockAsync(env, callback_ref, LOCK_MODE_EXCLUSIVE, options);
        Loop(LOOP_ONCE);
        LocksTest::DestroyEngine();
    });
    napi_env env = GetEnv();
    napi_value callback = CreateFunction("exclusivelockmulti", ExclusiveLockMultiCb, &callbackData);
    napi_ref callback_ref;
    napi_create_reference(env, callback, 1, &callback_ref);

    LockOptions options;
    lock->LockAsync(env, callback_ref, LOCK_MODE_EXCLUSIVE, options);
    Loop(LOOP_ONCE);
    t.join();
    ASSERT_EQ(callbackData.callCount, 2U);
    ASSERT_FALSE(callbackData.fail);
}

TEST_F(LocksTest, SharedLockSingle)
{
    napi_env env = GetEnv();
    std::unique_ptr<AsyncLock> lock = std::make_unique<AsyncLock>(1);
    bool isCalled = false;
    napi_value callback = CreateFunction("sharedlocksingle", ExclusiveLockSingleCb, &isCalled);
    napi_ref callback_ref;
    napi_create_reference(env, callback, 1, &callback_ref);

    LockOptions options;
    lock->LockAsync(env, callback_ref, LOCK_MODE_SHARED, options);
    ASSERT_TRUE(isCalled);
}

struct SharedMultiCallbackData: public CallbackData {
    explicit SharedMultiCallbackData(std::latch &barrier): CallbackData(), barrier(barrier)
    {
    }

    std::latch &barrier;
};

static napi_value MainSharedLockMultiCb(napi_env env, napi_callback_info info)
{
    napi_value undefined;
    napi_get_undefined(env, &undefined);
    SharedMultiCallbackData *data = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, nullptr, reinterpret_cast<void **>(&data));
    data->barrier.arrive_and_wait();
    data->callCount += 1;

    LocksTest::Sleep();
    data->executing.exchange(true);

    if (data->callCount != 2) {
        data->fail = true;
        return undefined;
    }

    data->executing = false;
    return undefined;
}

static napi_value SharedLockMultiCb(napi_env env, napi_callback_info info)
{
    napi_value undefined;
    napi_get_undefined(env, &undefined);
    CallbackData *data = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, nullptr, reinterpret_cast<void **>(&data));
    data->callCount += 1;

    LocksTest::Sleep();

    if (data->callCount != 2) {
        data->fail = true;
        return undefined;
    }

    data->executing = false;
    return undefined;
}

TEST_F(LocksTest, SharedLockMulti)
{
    std::unique_ptr<AsyncLock> lock = std::make_unique<AsyncLock>(1);
    AsyncLock *lockPtr = lock.get();
    std::latch barrier(2U);
    SharedMultiCallbackData callbackData(barrier);
    std::thread t([lockPtr, &callbackData, &barrier] () {
        LocksTest::InitializeEngine();
        napi_env env = GetEnv();
        napi_value callback = CreateFunction("sharedlockmulti", SharedLockMultiCb, &callbackData);
        napi_ref callback_ref;
        napi_create_reference(env, callback, 1, &callback_ref);
        LockOptions options;
        barrier.arrive_and_wait();
        lockPtr->LockAsync(env, callback_ref, LOCK_MODE_SHARED, options);
        LocksTest::DestroyEngine();
    });
    napi_env env = GetEnv();
    napi_value callback = CreateFunction("sharedlockmulti", MainSharedLockMultiCb, &callbackData);
    napi_ref callback_ref;
    napi_create_reference(env, callback, 1, &callback_ref);

    LockOptions options;
    lock->LockAsync(env, callback_ref, LOCK_MODE_SHARED, options);
    t.join();
    ASSERT_FALSE(callbackData.fail);
    ASSERT_EQ(callbackData.callCount, 2U);
}

struct IsAvailableCallbackData {
    IsAvailableCallbackData(std::latch &b, std::latch &e): begin(b), end(e)
    {
    }

    std::atomic<uint32_t> callCount = 0;
    std::latch &begin;
    std::latch &end;
};

static napi_value IsAvailableCb(napi_env env, napi_callback_info info)
{
    IsAvailableCallbackData *data = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, nullptr, reinterpret_cast<void **>(&data));
    data->callCount += 1;
    data->begin.arrive_and_wait();
    data->end.arrive_and_wait();
    napi_value undefined;
    napi_get_undefined(env, &undefined);
    return undefined;
}

TEST_F(LocksTest, IsAvailable)
{
    std::unique_ptr<AsyncLock> lock = std::make_unique<AsyncLock>(1);
    AsyncLock *lockPtr = lock.get();
    std::latch begin(2U);
    std::latch end(2U);
    IsAvailableCallbackData data(begin, end);
    std::thread t([lockPtr, &data] () {
        LocksTest::InitializeEngine();
        data.begin.arrive_and_wait();
        napi_env env = GetEnv();
        napi_value callback = CreateFunction("isavailable", IsAvailableCb, &data);
        napi_ref callback_ref;
        napi_create_reference(env, callback, 1, &callback_ref);
        LockOptions options;
        options.isAvailable = true;
        lockPtr->LockAsync(env, callback_ref, LOCK_MODE_EXCLUSIVE, options);
        data.end.arrive_and_wait();
        LocksTest::DestroyEngine();
    });
    napi_env env = GetEnv();
    napi_value callback = CreateFunction("isavailable", IsAvailableCb, &data);
    napi_ref callback_ref;
    napi_create_reference(env, callback, 1, &callback_ref);

    LockOptions options;

    lock->LockAsync(env, callback_ref, LOCK_MODE_EXCLUSIVE, options);
    LoopUntil([&data] () { return data.callCount > 0; });
    t.join();
    ASSERT_EQ(data.callCount, 1U);
}

static napi_value AsyncExclusiveCb(napi_env env, napi_callback_info info)
{
    napi_value undefined;
    napi_get_undefined(env, &undefined);

    CallbackData *data;
    napi_get_cb_info(env, info, nullptr, nullptr, nullptr, reinterpret_cast<void **>(&data));

    data->callCount += 1;
    bool prev = data->executing.exchange(true);
    if (prev) {
        data->fail = true;
        return undefined;
    }

    napi_status status;

    napi_value promise;
    napi_deferred deferred;
    status = napi_create_promise(env, &deferred, &promise);
    EXPECT_EQ(status, napi_ok);

    status = napi_resolve_deferred(env, deferred, undefined);
    EXPECT_EQ(status, napi_ok);

    LocksTest::Sleep();
    data->executing = false;

    return promise;
}

TEST_F(LocksTest, PendingRequestAfterEnvDestroyed)
{
    CallbackData callbackData;

    napi_env env {GetEnv()};
    napi_value lockName;
    ASSERT_CHECK_CALL(napi_create_string_utf8(env, "lockName", NAPI_AUTO_LENGTH, &lockName));
    napi_value requestArgs[1] {lockName};
    napi_value lock;
    ASSERT_CHECK_CALL(napi_call_function(env, undefined_, asyncLockRequest_, 1, requestArgs, &lock));
    napi_value lockAsync;
    ASSERT_CHECK_CALL(napi_get_named_property(env, lock, "lockAsync", &lockAsync));
    napi_value callback = CreateFunction("PendingRequestAfterEnvDestroyed", AsyncExclusiveCb, &callbackData);
    napi_value args[1] {callback};
    napi_value result;
    ASSERT_CHECK_CALL(napi_call_function(env, lock, lockAsync, 1, args, &result));
    bool isPromise {false};
    ASSERT_CHECK_CALL(napi_is_promise(env, result, &isPromise));
    ASSERT_TRUE(isPromise);

    std::thread t([&callbackData]() {
        LocksTest::InitializeEngine();

        napi_env env {GetEnv()};
        napi_value lockName;
        ASSERT_CHECK_CALL(napi_create_string_utf8(env, "lockName", NAPI_AUTO_LENGTH, &lockName));
        napi_value requestArgs[1] {lockName};
        napi_value lock;
        ASSERT_CHECK_CALL(napi_call_function(env, undefined_, asyncLockRequest_, 1, requestArgs, &lock));
        napi_value lockAsync;
        ASSERT_CHECK_CALL(napi_get_named_property(env, lock, "lockAsync", &lockAsync));
        napi_value callback = CreateFunction("PendingRequestAfterEnvDestroyed", AsyncExclusiveCb, &callbackData);
        napi_value args[1] {callback};
        napi_value result;
        ASSERT_CHECK_CALL(napi_call_function(env, lock, lockAsync, 1, args, &result));
        bool isPromise {false};
        ASSERT_CHECK_CALL(napi_is_promise(env, result, &isPromise));
        ASSERT_TRUE(isPromise);

        LocksTest::DestroyEngine();
    });

    t.join();
    Loop(LOOP_ONCE);
    ASSERT_EQ(callbackData.callCount, 1U);
    ASSERT_FALSE(callbackData.fail);
}

static napi_value AsyncSharedCb(napi_env env, napi_callback_info info)
{
    napi_value undefined;
    napi_get_undefined(env, &undefined);

    CallbackData *data;
    napi_get_cb_info(env, info, nullptr, nullptr, nullptr, reinterpret_cast<void **>(&data));

    data->callCount += 1;
    LocksTest::Sleep();

    napi_status status;

    napi_value promise;
    napi_deferred deferred;
    status = napi_create_promise(env, &deferred, &promise);
    EXPECT_EQ(status, napi_ok);

    status = napi_resolve_deferred(env, deferred, undefined);
    EXPECT_EQ(status, napi_ok);

    data->executing = false;
    return promise;
}

TEST_F(LocksTest, SharedModeWithEnvDestroyed)
{
    CallbackData callbackData;

    napi_env env {GetEnv()};
    napi_value lockName;
    ASSERT_CHECK_CALL(napi_create_string_utf8(env, "lockName", NAPI_AUTO_LENGTH, &lockName));
    napi_value requestArgs[1] {lockName};
    napi_value lock;
    ASSERT_CHECK_CALL(napi_call_function(env, undefined_, asyncLockRequest_, 1, requestArgs, &lock));
    napi_value lockAsync;
    ASSERT_CHECK_CALL(napi_get_named_property(env, lock, "lockAsync", &lockAsync));
    napi_value callback = CreateFunction("SharedModeWithEnvDestroyed", AsyncSharedCb, &callbackData);
    size_t argc {2};
    napi_value args[2] {callback, sharedMode_};
    napi_value result;
    ASSERT_CHECK_CALL(napi_call_function(env, lock, lockAsync, argc, args, &result));
    bool isPromise {false};
    ASSERT_CHECK_CALL(napi_is_promise(env, result, &isPromise));
    ASSERT_TRUE(isPromise);

    std::thread t([&callbackData]() {
        LocksTest::InitializeEngine();

        napi_env env {GetEnv()};
        napi_value lockName;
        ASSERT_CHECK_CALL(napi_create_string_utf8(env, "lockName", NAPI_AUTO_LENGTH, &lockName));
        napi_value requestArgs[1] {lockName};
        napi_value lock;
        ASSERT_CHECK_CALL(napi_call_function(env, undefined_, asyncLockRequest_, 1, requestArgs, &lock));
        napi_value lockAsync;
        ASSERT_CHECK_CALL(napi_get_named_property(env, lock, "lockAsync", &lockAsync));
        napi_value callback = CreateFunction("SharedModeWithEnvDestroyed", AsyncSharedCb, &callbackData);
        size_t argc {2};
        napi_value args[2] {callback, sharedMode_};
        napi_value result;
        ASSERT_CHECK_CALL(napi_call_function(env, lock, lockAsync, argc, args, &result));
        bool isPromise {false};
        ASSERT_CHECK_CALL(napi_is_promise(env, result, &isPromise));
        ASSERT_TRUE(isPromise);

        LocksTest::DestroyEngine();
    });

    t.join();
    ASSERT_EQ(callbackData.callCount, 2U);
}

static napi_value AsyncTimeoutCb(napi_env env, napi_callback_info info)
{
    napi_value undefined;
    napi_get_undefined(env, &undefined);

    IsAvailableCallbackData *data;
    napi_get_cb_info(env, info, nullptr, nullptr, nullptr, reinterpret_cast<void **>(&data));
    data->callCount += 1;
    data->begin.arrive_and_wait();

    napi_status status;

    napi_value promise;
    napi_deferred deferred;
    status = napi_create_promise(env, &deferred, &promise);
    EXPECT_EQ(status, napi_ok);

    status = napi_resolve_deferred(env, deferred, undefined);
    EXPECT_EQ(status, napi_ok);

    data->end.arrive_and_wait();
    return promise;
}

TEST_F(LocksTest, TimeoutLockWithEnvDestroyedTest)
{
    std::latch begin(2U);
    std::latch end(2U);
    IsAvailableCallbackData callbackData(begin, end);

    napi_env env {GetEnv()};
    napi_value lockName;
    ASSERT_CHECK_CALL(napi_create_string_utf8(env, "lockName", NAPI_AUTO_LENGTH, &lockName));
    napi_value requestArgs[1] {lockName};
    napi_value lock;
    ASSERT_CHECK_CALL(napi_call_function(env, undefined_, asyncLockRequest_, 1, requestArgs, &lock));
    napi_value lockAsync;
    ASSERT_CHECK_CALL(napi_get_named_property(env, lock, "lockAsync", &lockAsync));
    napi_value callback = CreateFunction("TimeoutLockWithEnvDestroyedTest", AsyncTimeoutCb, &callbackData);
    napi_value args[1] {callback};
    napi_value result;
    ASSERT_CHECK_CALL(napi_call_function(env, lock, lockAsync, 1, args, &result));

    std::thread t([&callbackData]() {
        LocksTest::InitializeEngine();
        callbackData.begin.arrive_and_wait();

        napi_env env {GetEnv()};
        napi_value lockName;
        ASSERT_CHECK_CALL(napi_create_string_utf8(env, "lockName", NAPI_AUTO_LENGTH, &lockName));
        napi_value requestArgs[1] {lockName};
        napi_value lock;
        ASSERT_CHECK_CALL(napi_call_function(env, undefined_, asyncLockRequest_, 1, requestArgs, &lock));
        napi_value lockAsync;
        ASSERT_CHECK_CALL(napi_get_named_property(env, lock, "lockAsync", &lockAsync));
        napi_value callback = CreateFunction("TimeoutLockWithEnvDestroyedTest", AsyncTimeoutCb, &callbackData);
        napi_value options;
        ASSERT_CHECK_CALL(napi_new_instance(env, asyncLockOptions_, 0, nullptr, &options));
        napi_value timeout;
        ASSERT_CHECK_CALL(napi_create_int32(env, defaultTimeout, &timeout));
        ASSERT_CHECK_CALL(napi_set_named_property(env, options, "timeout", timeout));
        size_t argc {3};
        napi_value args[3] {callback, exclusiveMode_, options};
        napi_value result;
        ASSERT_CHECK_CALL(napi_call_function(env, lock, lockAsync, argc, args, &result));

        LocksTest::Sleep();
        callbackData.end.arrive_and_wait();
        LocksTest::DestroyEngine();
    });

    Loop(LOOP_ONCE);
    t.join();
    ASSERT_EQ(callbackData.callCount, 1U);
}

struct Defer {
    napi_deferred deferred;
    CallbackData *data;
};

static napi_value AsyncCb(napi_env env, napi_callback_info info)
{
    napi_value undefined;
    napi_get_undefined(env, &undefined);

    CallbackData *data;
    napi_get_cb_info(env, info, nullptr, nullptr, nullptr, reinterpret_cast<void **>(&data));

    napi_status status;

    napi_value promise;
    auto defer {new Defer};
    defer->data = data;
    status = napi_create_promise(env, &defer->deferred, &promise);
    EXPECT_EQ(status, napi_ok);

    uv_loop_t *loop;
    napi_get_uv_event_loop(env, &loop);
    uv_timer_t *timer = new uv_timer_t;
    uv_timer_init(loop, timer);
    timer->data = defer;
    uv_timer_start(
        timer,
        [](uv_timer_t *timer) {
            Defer *defer = reinterpret_cast<Defer *>(timer->data);
            napi_env env {LocksTest::GetEnv()};
            LocksTest::Sleep();

            napi_value undefined;
            napi_get_undefined(env, &undefined);

            defer->data->callCount += 1;

            napi_status status = napi_resolve_deferred(env, defer->deferred, undefined);
            EXPECT_EQ(status, napi_ok);

            delete defer;
            uv_close(reinterpret_cast<uv_handle_t *>(timer), [](uv_handle_t *timer) { delete timer; });
        },
        LocksTest::defaultTimeout, 0);

    EXPECT_EQ(status, napi_ok);

    data->executing = false;
    return promise;
}

TEST_F(LocksTest, PendingSharedRequestAfterGC)
{
    CallbackData callbackData;
    napi_env env {GetEnv()};
    TriggerGC();
    {
        napi_handle_scope scope;
        napi_open_handle_scope(env, &scope);

        napi_value lockName;
        ASSERT_CHECK_CALL(napi_create_string_utf8(env, "lockName", NAPI_AUTO_LENGTH, &lockName));
        napi_value requestArgs[1] {lockName};
        napi_value lock;
        ASSERT_CHECK_CALL(napi_call_function(env, undefined_, asyncLockRequest_, 1, requestArgs, &lock));
        napi_value lockAsync;
        ASSERT_CHECK_CALL(napi_get_named_property(env, lock, "lockAsync", &lockAsync));
        napi_value callback = CreateFunction("SharedModeWithEnvDestroyed", AsyncCb, &callbackData);
        size_t argc {2};
        napi_value args[2] {callback, sharedMode_};
        napi_value result;
        ASSERT_CHECK_CALL(napi_call_function(env, lock, lockAsync, argc, args, &result));

        bool isPromise {false};
        ASSERT_CHECK_CALL(napi_is_promise(env, result, &isPromise));
        ASSERT_TRUE(isPromise);

        napi_close_handle_scope(env, scope);
    }
    {
        napi_handle_scope scope;
        napi_open_handle_scope(env, &scope);

        napi_value lockName;
        ASSERT_CHECK_CALL(napi_create_string_utf8(env, "lockName", NAPI_AUTO_LENGTH, &lockName));
        napi_value requestArgs[1] {lockName};
        napi_value lock;
        ASSERT_CHECK_CALL(napi_call_function(env, undefined_, asyncLockRequest_, 1, requestArgs, &lock));
        napi_value lockAsync;
        ASSERT_CHECK_CALL(napi_get_named_property(env, lock, "lockAsync", &lockAsync));
        napi_value callback = CreateFunction("SharedModeWithEnvDestroyed", AsyncCb, &callbackData);
        size_t argc {2};
        napi_value args[2] {callback, sharedMode_};
        napi_value result;
        ASSERT_CHECK_CALL(napi_call_function(env, lock, lockAsync, argc, args, &result));

        bool isPromise {false};
        ASSERT_CHECK_CALL(napi_is_promise(env, result, &isPromise));
        ASSERT_TRUE(isPromise);

        napi_close_handle_scope(env, scope);
    }
    TriggerGC();

    Loop(LOOP_ONCE);
    ASSERT_EQ(callbackData.callCount, 2U);
}
