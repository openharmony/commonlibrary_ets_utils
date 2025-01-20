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
#include "condition/condition_variable.h"

using namespace Commonlibrary::Concurrent::Condition;

class ConditionTest : public testing::Test {
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

    static napi_value CreateFunction(const char *name, napi_value (*callback)(napi_env, napi_callback_info),
        void *data = nullptr)
    {
        napi_value result;
        napi_status status = napi_create_function(GetEnv(), name, NAPI_AUTO_LENGTH, callback, data, &result);
        EXPECT_EQ(status, napi_ok);
        return result;
    }

protected:
    static thread_local NativeEngine *engine_;
    static thread_local EcmaVM *vm_;
};

thread_local NativeEngine *ConditionTest::engine_ = nullptr;
thread_local EcmaVM *ConditionTest::vm_ = nullptr;

static napi_value CreateConditionVariableInstances(napi_env env)
{
    napi_value exports;
    napi_create_object(env, &exports);
    napi_value exportsResult = Commonlibrary::Concurrent::Condition::ConditionVariable::Init(env, exports);
    napi_value locks;
    napi_status status = napi_get_named_property(env, exportsResult, "locks", &locks);
    if (status != napi_ok) {
        return nullptr;
    }

    napi_value conditionClass;
    status = napi_get_named_property(env, locks, "ConditionVariable", &conditionClass);
    if (status != napi_ok) {
        return nullptr;
    }

    napi_value instance;
    status = napi_new_instance(env, conditionClass, 0, nullptr, &instance);
    if (status != napi_ok) {
        return nullptr;
    }

    napi_ref napi_ref;
    status = napi_create_reference(env, instance, 1, &napi_ref);
    if (status != napi_ok) {
        return nullptr;
    }

    napi_value thisVar;
    status = napi_get_reference_value(env, napi_ref, &thisVar);
    if (status != napi_ok) {
        return nullptr;
    }

    return thisVar;
}

TEST_F(ConditionTest, WaitTest)
{
    ConditionTest::InitializeEngine();
    napi_env env = ConditionTest::GetEnv();
    ASSERT_NE(env, nullptr);
    napi_value thisVar = CreateConditionVariableInstances(env);
    ASSERT_NE(thisVar, nullptr);

    // 调用 wait 方法
    napi_value waitFn;
    napi_status status = napi_get_named_property(env, thisVar, "wait", &waitFn);
    ASSERT_EQ(status, napi_ok);
    napi_value promise;
    status = napi_call_function(env, thisVar, waitFn, 0, nullptr, &promise);
    ASSERT_EQ(status, napi_ok);
    ASSERT_TRUE(promise != nullptr);
    bool isPromise = false;
    napi_is_promise(env, promise, &isPromise);
    ASSERT_TRUE(isPromise);

    // 调用notify 方法
    napi_value notifyFn;
    status = napi_get_named_property(env, thisVar, "notifyAll", &notifyFn);
    ASSERT_EQ(status, napi_ok);
    status = napi_call_function(env, thisVar, notifyFn, 0, nullptr, nullptr);
    ASSERT_EQ(status, napi_ok);

    // 调用notify 方法
    status = napi_call_function(env, thisVar, notifyFn, 0, nullptr, nullptr);
    ASSERT_EQ(status, napi_ok);
}

TEST_F(ConditionTest, WaitForTest)
{
    ConditionTest::InitializeEngine();
    napi_env env = ConditionTest::GetEnv();
    ASSERT_NE(env, nullptr);
    napi_value thisVar = CreateConditionVariableInstances(env);
    ASSERT_NE(thisVar, nullptr);

    // 调用 wait 方法
    napi_value waitFn;
    napi_status status = napi_get_named_property(env, thisVar, "waitFor", &waitFn);
    ASSERT_EQ(status, napi_ok);
    napi_value promise;
    napi_value milliseconds = nullptr;
    double millis = 3000;
    napi_create_double(env, millis, &milliseconds);
    napi_value argv[] = {milliseconds};
    status = napi_call_function(env, thisVar, waitFn, 1, argv, &promise);
    ASSERT_EQ(status, napi_ok);
    ASSERT_TRUE(promise != nullptr);
    bool isPromise = false;
    napi_is_promise(env, promise, &isPromise);
    ASSERT_TRUE(isPromise);

    // 调用notifyOne 方法
    napi_value notifyFn;
    status = napi_get_named_property(env, thisVar, "notifyOne", &notifyFn);
    ASSERT_EQ(status, napi_ok);
    status = napi_call_function(env, thisVar, notifyFn, 0, nullptr, nullptr);
    ASSERT_EQ(status, napi_ok);

    // 调用notifyOne 方法
    status = napi_call_function(env, thisVar, notifyFn, 0, nullptr, nullptr);
    ASSERT_EQ(status, napi_ok);
}

TEST_F(ConditionTest, NotifyOneTest)
{
    ConditionTest::InitializeEngine();
    napi_env env = ConditionTest::GetEnv();
    ASSERT_NE(env, nullptr);
    napi_value thisVar = CreateConditionVariableInstances(env);
    ASSERT_NE(thisVar, nullptr);

    // 调用notify 方法
    napi_value notifyFn;
    napi_status status = napi_get_named_property(env, thisVar, "notifyOne", &notifyFn);
    ASSERT_EQ(status, napi_ok);
    status = napi_call_function(env, thisVar, notifyFn, 0, nullptr, nullptr);
    ASSERT_EQ(status, napi_ok);

    // 调用notify 方法
    status = napi_call_function(env, thisVar, notifyFn, 0, nullptr, nullptr);
    ASSERT_EQ(status, napi_ok);
}

TEST_F(ConditionTest, NotifyAllTest)
{
    ConditionTest::InitializeEngine();
    napi_env env = ConditionTest::GetEnv();
    ASSERT_NE(env, nullptr);
    napi_value thisVar = CreateConditionVariableInstances(env);
    ASSERT_NE(thisVar, nullptr);

    // 调用notify 方法
    napi_value notifyFn;
    napi_status status = napi_get_named_property(env, thisVar, "notifyAll", &notifyFn);
    ASSERT_EQ(status, napi_ok);
    status = napi_call_function(env, thisVar, notifyFn, 0, nullptr, nullptr);
    ASSERT_EQ(status, napi_ok);

    // 调用notify 方法
    status = napi_call_function(env, thisVar, notifyFn, 0, nullptr, nullptr);
    ASSERT_EQ(status, napi_ok);
}

TEST_F(ConditionTest, ConstructorTest)
{
    ConditionTest::InitializeEngine();
    napi_env env = ConditionTest::GetEnv();
    ASSERT_NE(env, nullptr);
    napi_value thisVar = CreateConditionVariableInstances(env);
    ASSERT_NE(thisVar, nullptr);
}

TEST_F(ConditionTest, ConstructorTest2)
{
    ConditionTest::InitializeEngine();
    napi_env env = ConditionTest::GetEnv();
    ASSERT_NE(env, nullptr);
    ConditionVariable *cond = new ConditionVariable();
    ASSERT_NE(cond, nullptr);
    ASSERT_EQ(cond->GetRefCount(), 0);
    ConditionVariable *cond2 = new ConditionVariable(cond);
    ASSERT_NE(cond2, nullptr);
    ASSERT_EQ(cond2->GetRefCount(), 0);
    cond->Destructor(env, static_cast<void *>(cond), nullptr);
    cond->Destructor(env, static_cast<void *>(cond2), nullptr);
}

TEST_F(ConditionTest, RquestTest)
{
    ConditionTest::InitializeEngine();
    napi_env env = ConditionTest::GetEnv();
    ASSERT_NE(env, nullptr);
    napi_value exports;
    napi_create_object(env, &exports);
    napi_value exportsResult = Commonlibrary::Concurrent::Condition::ConditionVariable::Init(env, exports);
    napi_value locks;
    napi_status status = napi_get_named_property(env, exportsResult, "locks", &locks);
    ASSERT_EQ(status, napi_ok);
    napi_value conditionClass;
    status = napi_get_named_property(env, locks, "ConditionVariable", &conditionClass);
    ASSERT_EQ(status, napi_ok);

    napi_value constructorArgs1[1] = { 0 };
    std::string name = "condName1";
    napi_create_string_utf8(env, name.c_str(), name.size(), &constructorArgs1[0]);

    // 调用request 方法
    napi_value notifyFn;
    status = napi_get_named_property(env, conditionClass, "request", &notifyFn);
    ASSERT_EQ(status, napi_ok);
    napi_value condRequest = nullptr;
    napi_value undefine = nullptr;
    status = napi_call_function(env, undefine, notifyFn, 1, constructorArgs1, &condRequest);
    ASSERT_EQ(status, napi_ok);
    ASSERT_NE(condRequest, nullptr);
    napi_ref napi_ref;
    status = napi_create_reference(env, condRequest, 1, &napi_ref);
    ASSERT_EQ(status, napi_ok);
    napi_value thisVar;
    status = napi_get_reference_value(env, napi_ref, &thisVar);
    ASSERT_EQ(status, napi_ok);

    // 调用 wait 方法
    napi_value waitFn;
    status = napi_get_named_property(env, thisVar, "wait", &waitFn);
    ASSERT_EQ(status, napi_ok);
    napi_value promise;
    status = napi_call_function(env, thisVar, waitFn, 0, nullptr, &promise);
    ASSERT_EQ(status, napi_ok);
    ASSERT_TRUE(promise != nullptr);
    bool isPromise = false;
    napi_is_promise(env, promise, &isPromise);
    ASSERT_TRUE(isPromise);

    // 调用notify 方法
    status = napi_get_named_property(env, thisVar, "notifyAll", &notifyFn);
    ASSERT_EQ(status, napi_ok);
    status = napi_call_function(env, thisVar, notifyFn, 0, nullptr, nullptr);
    ASSERT_EQ(status, napi_ok);
}

TEST_F(ConditionTest, NotifyAfterEnvDestroyedTest)
{
    ConditionTest::InitializeEngine();
    napi_env env = ConditionTest::GetEnv();
    ASSERT_NE(env, nullptr);
    napi_value thisVar = CreateConditionVariableInstances(env);
    ASSERT_NE(thisVar, nullptr);

    // 调用 wait 方法
    napi_value waitFn;
    napi_status status = napi_get_named_property(env, thisVar, "wait", &waitFn);
    ASSERT_EQ(status, napi_ok);
    napi_value promise;
    status = napi_call_function(env, thisVar, waitFn, 0, nullptr, &promise);
    ASSERT_EQ(status, napi_ok);
    ASSERT_TRUE(promise != nullptr);
    bool isPromise = false;
    napi_is_promise(env, promise, &isPromise);
    ASSERT_TRUE(isPromise);

    std::thread t([&thisVar]() {
        ConditionTest::InitializeEngine();
        napi_env env = ConditionTest::GetEnv();
        napi_value notifyFn;
        napi_status status = napi_get_named_property(env, thisVar, "notifyOne", &notifyFn);
        ASSERT_EQ(status, napi_ok);
        napi_value result;
        status = napi_call_function(env, thisVar, notifyFn, 0, nullptr, &result);
        ASSERT_EQ(status, napi_ok);
        ConditionTest::DestroyEngine();
    });
    t.join();

    napi_value notifyFn;
    status = napi_get_named_property(env, thisVar, "notifyAll", &notifyFn);
    ASSERT_EQ(status, napi_ok);
    status = napi_call_function(env, thisVar, notifyFn, 0, nullptr, nullptr);
    ASSERT_EQ(status, napi_ok);
}

TEST_F(ConditionTest, NotifyOneAfterEnvDestroyedTest)
{
    ConditionTest::InitializeEngine();
    napi_env env = ConditionTest::GetEnv();
    ASSERT_NE(env, nullptr);
    napi_value thisVar = CreateConditionVariableInstances(env);
    ASSERT_NE(thisVar, nullptr);
    
    // 调用 wait 方法
    napi_value waitFn;
    napi_status status = napi_get_named_property(env, thisVar, "waitFor", &waitFn);
    ASSERT_EQ(status, napi_ok);
    napi_value promise;
    napi_value milliseconds = nullptr;
    double millis = 500;
    napi_create_double(env, millis, &milliseconds);
    napi_value argv[] = {milliseconds};
    status = napi_call_function(env, thisVar, waitFn, 1, argv, &promise);
    ASSERT_EQ(status, napi_ok);
    ASSERT_TRUE(promise != nullptr);
    bool isPromise = false;
    napi_is_promise(env, promise, &isPromise);
    ASSERT_TRUE(isPromise);

    std::thread t([&thisVar]() {
        ConditionTest::InitializeEngine();
        napi_env env = ConditionTest::GetEnv();
        napi_value notifyFn;
        napi_status status = napi_get_named_property(env, thisVar, "notifyOne", &notifyFn);
        ASSERT_EQ(status, napi_ok);
        napi_value result;
        status = napi_call_function(env, thisVar, notifyFn, 0, nullptr, &result);
        ASSERT_EQ(status, napi_ok);
        ConditionTest::DestroyEngine();
    });
    t.join();
}