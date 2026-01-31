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

#ifndef TEST_XML_UNITTEST_TEST_H
#define TEST_XML_UNITTEST_TEST_H

#include "native_engine.h"
#include "gtest/gtest.h"

#include "ark_native_engine.h"
#include "native_engine/native_create_env.h"
#include "native_engine.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include <thread>
#include <condition_variable>

class ArkRuntimeChecker {
public:
    ArkRuntimeChecker() = default;

    static void CreateArkRuntimeEngine(napi_env& workerEnv)
    {
        panda::RuntimeOption option;
        option.SetGcType(panda::RuntimeOption::GC_TYPE::GEN_GC);
        const int64_t poolSize = 0x1000000;
        option.SetGcPoolSize(poolSize);
        option.SetLogLevel(panda::RuntimeOption::LOG_LEVEL::ERROR);
        option.SetDebuggerLibraryPath("");
        panda::ecmascript::EcmaVM* vm = panda::JSNApi::CreateJSVM(option);
        workerEnv = reinterpret_cast<napi_env>(new (std::nothrow) ArkNativeEngine(vm, nullptr));
        auto cleanEnv = [vm]() {
            if (vm != nullptr) {
                panda::JSNApi::DestroyJSVM(vm);
            }
        };
        ArkNativeEngine* arkEngine = reinterpret_cast<ArkNativeEngine*>(workerEnv);
        arkEngine->SetCleanEnv(cleanEnv);
    }
};

class NativeEngineTest : public testing::Test {
public:
    NativeEngineTest();
    virtual ~NativeEngineTest();
    void SetUp() override {}
    void TearDown() override {}
protected:
    NativeEngine *engine_;
    template<typename TestBodyFunc>

    void RunInNapiTestEnv(TestBodyFunc&& testBody)
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

        std::thread* newThread = new std::thread([this, testBody]() {
            napi_env env = nullptr;
            napi_create_ark_runtime(&env);
            if (env == nullptr) {
                ASSERT_TRUE(false);
                return;
            }
            testBody(env);
        });
        newThread->join();
        delete newThread;
    }
};

#endif // TEST_XML_UNITTEST_TEST_H
