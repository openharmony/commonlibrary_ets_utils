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
#include "utils/log.h"
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

//worker constructor
napi_value Worker_Constructor(napi_env env, napi_value global) {
    HILOG_INFO("HYQ Worker_Constructor");
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

//worker terminate
napi_value Worker_Terminate(napi_env env, napi_value global) {
    std::string funcName = "Terminate";
    napi_value cb = nullptr;
    napi_value result = nullptr;
    napi_create_function(env, funcName.c_str(), funcName.size(), Worker::Terminate, nullptr, &cb);
    napi_call_function(env, global, cb, 0, nullptr, &result);
    return result;
}

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

// HWTEST_F(NativeEngineTest, PostMessageTest001, testing::ext::TestSize.Level0)
// {
//     napi_env env = (napi_env)engine_;
//     napi_value global;
//     napi_get_global(env, &global);


//     napi_value result = nullptr;
//     result = Worker_Constructor(env, global);

//     napi_value argv[1] = { nullptr };
//     std::string message = "from host";
//     napi_create_string_utf8(env, message.c_str(), message.length(), &argv[0]);

//     std::string funcName = "PostMessage";
//     napi_value cb = nullptr;

//     napi_create_function(env, funcName.c_str(), funcName.size(), Worker::PostMessage, nullptr, &cb);
//     napi_call_function(env, global, cb, sizeof(argv) / sizeof(argv[0]), argv, &result);

//     result = Worker_Terminate(env, global);
//     ASSERT_TRUE(result != nullptr);
// }


// HWTEST_F(NativeEngineTest, PostMessageTest002, testing::ext::TestSize.Level0)
// {
//     napi_env env = (napi_env)engine_;
    
//     napi_env newEnv = nullptr;
//     napi_create_runtime(env, &newEnv);
    
//     ASSERT_TRUE(newEnv != nullptr);
// }
