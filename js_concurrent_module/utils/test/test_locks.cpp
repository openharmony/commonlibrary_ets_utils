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
};

thread_local NativeEngine *LocksTest::engine_ = nullptr;
thread_local EcmaVM *LocksTest::vm_ = nullptr;

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

TEST_F(LocksTest, SharedLockMulti)
{
    std::unique_ptr<AsyncLock> lock = std::make_unique<AsyncLock>(1);
    AsyncLock *lockPtr = lock.get();
    std::latch barrier(2U);
    SharedMultiCallbackData callbackData(barrier);
    std::thread t([lockPtr, &callbackData, &barrier] () {
        LocksTest::InitializeEngine();
        napi_env env = GetEnv();
        napi_value callback = CreateFunction("sharedlockmulti", ExclusiveLockMultiCb, &callbackData);
        napi_ref callback_ref;
        napi_create_reference(env, callback, 1, &callback_ref);
        LockOptions options;
        barrier.arrive_and_wait();
        lockPtr->LockAsync(env, callback_ref, LOCK_MODE_SHARED, options);
        Loop(LOOP_ONCE);
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
