/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#ifndef TEST_CONVERTXML_UNITTEST_TEST_H
#define TEST_CONVERTXML_UNITTEST_TEST_H

#include "native_engine.h"

#include "gtest/gtest.h"

#if defined(ENABLE_TASKPOOL_FFRT)
#include "c/executor_task.h"
#include "ffrt_inner.h"
#endif

namespace Commonlibrary::Concurrent::TaskPoolModule {
class NativeEngineTest : public testing::Test {
public:
    NativeEngineTest();
    virtual ~NativeEngineTest();
    void SetUp() override {}
    void TearDown() override {}

    static napi_value IsConcurrent(napi_env env, napi_value argv[], size_t argc);
    static napi_value GetTaskPoolInfo(napi_env env, napi_value argv[], size_t argc);
    static napi_value TerminateTask(napi_env env, napi_value argv[], size_t argc);
    static napi_value Execute(napi_env env, napi_value argv[], size_t argc);
    static napi_value ExecuteDelayed(napi_env env, napi_value argv[], size_t argc);
    static napi_value Cancel(napi_env env, napi_value argv[], size_t argc);

    class ExceptionScope {
    public:
        explicit ExceptionScope(napi_env env) : env_(env) {}
        ~ExceptionScope()
        {
            napi_value exception = nullptr;
            napi_get_and_clear_last_exception(env_, &exception);
        }
    private:
        napi_env env_ = nullptr;
    };

protected:
    NativeEngine *engine_ = nullptr;
};

class SendableUtils {
public:
    SendableUtils() = default;
    ~SendableUtils() = default;

    static napi_value CreateSendableClass(napi_env env);
    static napi_value CreateSendableInstance(napi_env env);
    static napi_value Foo(napi_env env, napi_callback_info info);
    static napi_value Bar(napi_env env, napi_callback_info info);
};
} // namespace Commonlibrary::Concurrent::TaskPoolModule
#endif // TEST_CONVERTXML_UNITTEST_TEST_H