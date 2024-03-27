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
#include "napi_helper.h"
#include "tools/log.h"

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

using namespace Commonlibrary::Concurrent::Common::Helper;

HWTEST_F(NativeEngineTest, IsStringTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value value = nullptr;
    std::string str = "test";
    napi_create_string_utf8(env, str.c_str(), NAPI_AUTO_LENGTH, &value);
    bool res;
    res = NapiHelper::IsString(env, value);
    ASSERT_TRUE(res);
}

HWTEST_F(NativeEngineTest, IsNotUndefinedTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value value = nullptr;
    ASSERT_CHECK_CALL(napi_get_undefined(env, &value));
    bool res;
    res = NapiHelper::IsNotUndefined(env, value) ? false : true;
    ASSERT_TRUE(res);
}

HWTEST_F(NativeEngineTest, IsArrayTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;

    napi_value result = nullptr;
    ASSERT_CHECK_CALL(napi_create_object(env, &result));
    ASSERT_CHECK_VALUE_TYPE(env, result, napi_object);

    const char testStr[] = "1234567";
    napi_value strAttribute = nullptr;
    ASSERT_CHECK_CALL(napi_create_string_utf8(env, testStr, strlen(testStr), &strAttribute));
    ASSERT_CHECK_VALUE_TYPE(env, strAttribute, napi_string);
    ASSERT_CHECK_CALL(napi_set_named_property(env, result, "strAttribute", strAttribute));

    napi_value retStrAttribute = nullptr;
    ASSERT_CHECK_CALL(napi_get_named_property(env, result, "strAttribute", &retStrAttribute));
    ASSERT_CHECK_VALUE_TYPE(env, retStrAttribute, napi_string);

    int32_t testNumber = 12345; // 12345 : indicates any number
    napi_value numberAttribute = nullptr;
    ASSERT_CHECK_CALL(napi_create_int32(env, testNumber, &numberAttribute));
    ASSERT_CHECK_VALUE_TYPE(env, numberAttribute, napi_number);
    ASSERT_CHECK_CALL(napi_set_named_property(env, result, "numberAttribute", numberAttribute));

    napi_value propNames = nullptr;
    ASSERT_CHECK_CALL(napi_get_property_names(env, result, &propNames));
    ASSERT_CHECK_VALUE_TYPE(env, propNames, napi_object);
    bool isArray = false;
    isArray = NapiHelper::IsArray(env, propNames);

    ASSERT_TRUE(isArray);
}

HWTEST_F(NativeEngineTest, IsFunctionTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;

    auto func = [](napi_env env, napi_callback_info info) -> napi_value {
        return nullptr;
    };

    napi_value value = nullptr;
    napi_create_function(env, "testFunc", NAPI_AUTO_LENGTH, func, nullptr, &value);
    ASSERT_NE(value, nullptr);
    bool res;
    res = NapiHelper::IsFunction(env, value);
    ASSERT_TRUE(res);
}

HWTEST_F(NativeEngineTest, GetCallbackInfoArgc001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;

    auto func = [](napi_env env, napi_callback_info info) -> napi_value {
        napi_value thisVar;
        napi_value* argv = nullptr;
        size_t argc = 0;
        void* data = nullptr;

        argc = NapiHelper::GetCallbackInfoArgc(env, info);
        if (argc > 0) {
            argv = new napi_value[argc];
        }
        napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);

        napi_value result = nullptr;
        napi_create_object(env, &result);

        napi_value messageKey = nullptr;
        const char* messageKeyStr = "message";
        napi_create_string_latin1(env, messageKeyStr, strlen(messageKeyStr), &messageKey);
        napi_value messageValue = nullptr;
        const char* messageValueStr = "OK";
        napi_create_string_latin1(env, messageValueStr, strlen(messageValueStr), &messageValue);
        napi_set_property(env, result, messageKey, messageValue);

        if (argv != nullptr) {
            delete []argv;
        }
        return result;
    };

    napi_value funcValue = nullptr;
    napi_create_function(env, "testFunc", NAPI_AUTO_LENGTH, func, nullptr, &funcValue);
    ASSERT_NE(funcValue, nullptr);

    bool result = false;
    napi_is_callable(env, funcValue, &result);
    ASSERT_TRUE(result);
}

HWTEST_F(NativeEngineTest, GetNamePropertyInParentPort001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;

    napi_value result = nullptr;
    napi_ref resultRef = nullptr;

    napi_create_object(env, &result);
    resultRef = NapiHelper::CreateReference(env, result, 1);

    napi_value value = nullptr;

    value = NapiHelper::GetNamePropertyInParentPort(env, resultRef, "test");
    ASSERT_TRUE(value != nullptr);
}

HWTEST_F(NativeEngineTest, GetUndefinedValue001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value value = nullptr;

    value = NapiHelper::GetUndefinedValue(env);
    bool res = false;
    res = NapiHelper::IsNotUndefined(env, value) ? false : true;
    ASSERT_TRUE(res);
}

HWTEST_F(NativeEngineTest, IsCallable001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;

    auto func = [](napi_env env, napi_callback_info info) -> napi_value {
        napi_value thisVar;
        napi_value* argv = nullptr;
        size_t argc = 0;
        void* data = nullptr;

        napi_get_cb_info(env, info, &argc, nullptr, nullptr, nullptr);
        if (argc > 0) {
            argv = new napi_value[argc];
        }
        napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);

        napi_value result = nullptr;
        napi_create_object(env, &result);

        napi_value messageKey = nullptr;
        const char* messageKeyStr = "message";
        napi_create_string_latin1(env, messageKeyStr, strlen(messageKeyStr), &messageKey);
        napi_value messageValue = nullptr;
        const char* messageValueStr = "OK";
        napi_create_string_latin1(env, messageValueStr, strlen(messageValueStr), &messageValue);
        napi_set_property(env, result, messageKey, messageValue);

        if (argv != nullptr) {
            delete []argv;
        }
        return result;
    };

    napi_value funcValue = nullptr;
    napi_create_function(env, "testFunc", NAPI_AUTO_LENGTH, func, nullptr, &funcValue);
    ASSERT_NE(funcValue, nullptr);

    bool result = false;
    result = NapiHelper::IsCallable(env, funcValue);
    ASSERT_TRUE(result);
}

HWTEST_F(NativeEngineTest, IsCallable002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;

    auto func = [](napi_env env, napi_callback_info info) -> napi_value {
        napi_value thisVar;
        napi_value* argv = nullptr;
        size_t argc = 0;
        void* data = nullptr;

        napi_get_cb_info(env, info, &argc, nullptr, nullptr, nullptr);
        if (argc > 0) {
            argv = new napi_value[argc];
        }
        napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);

        napi_value result = nullptr;
        napi_create_object(env, &result);

        napi_value messageKey = nullptr;
        const char* messageKeyStr = "message";
        napi_create_string_latin1(env, messageKeyStr, strlen(messageKeyStr), &messageKey);
        napi_value messageValue = nullptr;
        const char* messageValueStr = "OK";
        napi_create_string_latin1(env, messageValueStr, strlen(messageValueStr), &messageValue);
        napi_set_property(env, result, messageKey, messageValue);

        if (argv != nullptr) {
            delete []argv;
        }
        return result;
    };

    napi_value funcValue = nullptr;
    napi_create_function(env, "testFunc", NAPI_AUTO_LENGTH, func, nullptr, &funcValue);
    ASSERT_NE(funcValue, nullptr);

    napi_ref resultRef = nullptr;
    resultRef = NapiHelper::CreateReference(env, funcValue, 1);

    bool result = false;
    result = NapiHelper::IsCallable(env, resultRef);
    ASSERT_TRUE(result);
}

HWTEST_F(NativeEngineTest, SetNamePropertyInGlobal001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;

    const char testStr[] = "1234567";
    napi_value strAttribute = nullptr;
    ASSERT_CHECK_CALL(napi_create_string_utf8(env, testStr, strlen(testStr), &strAttribute));
    ASSERT_CHECK_VALUE_TYPE(env, strAttribute, napi_string);


    NapiHelper::SetNamePropertyInGlobal(env, "strAttribute", strAttribute);
    napi_value result = nullptr;
    result = NapiHelper::GetGlobalObject(env);

    napi_value retStrAttribute = nullptr;
    ASSERT_CHECK_CALL(napi_get_named_property(env, result, "strAttribute", &retStrAttribute));
    ASSERT_CHECK_VALUE_TYPE(env, retStrAttribute, napi_string);
}

HWTEST_F(NativeEngineTest, IsObject001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value value = nullptr;

    napi_create_object(env, &value);
    bool res = false;
    res = NapiHelper::IsObject(env, value);
    ASSERT_TRUE(res);
}

HWTEST_F(NativeEngineTest, GetString001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value value = nullptr;

    std::string str = "test";
    napi_create_string_utf8(env, str.c_str(), NAPI_AUTO_LENGTH, &value);
    bool res = false;
    res = NapiHelper::IsString(env, value);
    ASSERT_TRUE(res);

    char* script = NapiHelper::GetString(env, value);
    std::string s(script);
    ASSERT_EQ(str, s);
}

HWTEST_F(NativeEngineTest, CreateBooleanValue001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value value = nullptr;

    value = NapiHelper::CreateBooleanValue(env, true);
    ASSERT_CHECK_VALUE_TYPE(env, value, napi_boolean);
}

HWTEST_F(NativeEngineTest, GetGlobalObject001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value value = nullptr;

    value = NapiHelper::GetGlobalObject(env);
    ASSERT_CHECK_VALUE_TYPE(env, value, napi_object);
}

HWTEST_F(NativeEngineTest, CreateReference001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;

    napi_value result = nullptr;
    napi_ref resultRef = nullptr;

    napi_create_object(env, &result);
    resultRef = NapiHelper::CreateReference(env, result, 1);

    uint32_t resultRefCount = 0;

    napi_reference_ref(env, resultRef, &resultRefCount);
    ASSERT_EQ(resultRefCount, (uint32_t)2); // 2 : means count of resultRef

    napi_reference_unref(env, resultRef, &resultRefCount);
    ASSERT_EQ(resultRefCount, (uint32_t)1);

    NapiHelper::DeleteReference(env, resultRef);
}

HWTEST_F(NativeEngineTest, CreateUint32001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    uint32_t testValue = UINT32_MAX;
    napi_value result = nullptr;
    result = NapiHelper::CreateUint32(env, testValue);
    ASSERT_CHECK_VALUE_TYPE(env, result, napi_number);
}

HWTEST_F(NativeEngineTest, GetLibUV001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    uv_loop_t* loop;
    loop = NapiHelper::GetLibUV(env);

    ASSERT_NE(loop, nullptr);
}

HWTEST_F(NativeEngineTest, GetReferenceValue001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;

    napi_value result = nullptr;
    napi_ref resultRef = nullptr;

    napi_create_object(env, &result);
    napi_create_reference(env, result, 1, &resultRef);

    napi_value refValue = nullptr;

    refValue = NapiHelper::GetReferenceValue(env, resultRef);

    ASSERT_NE(refValue, nullptr);

    NapiHelper::DeleteReference(env, resultRef);
}

HWTEST_F(NativeEngineTest, DeleteReference001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;

    napi_value result = nullptr;
    napi_ref resultRef = nullptr;

    napi_create_object(env, &result);
    napi_create_reference(env, result, 1, &resultRef);
    ASSERT_NE(resultRef, nullptr);

    NapiHelper::DeleteReference(env, resultRef);
}

HWTEST_F(NativeEngineTest, GetNameProperty001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;

    const char testStr[] = "1234567";
    napi_value strAttribute = nullptr;
    ASSERT_CHECK_CALL(napi_create_string_utf8(env, testStr, strlen(testStr), &strAttribute));
    ASSERT_CHECK_VALUE_TYPE(env, strAttribute, napi_string);


    NapiHelper::SetNamePropertyInGlobal(env, "strAttribute", strAttribute);
    napi_value result = nullptr;
    result = NapiHelper::GetGlobalObject(env);

    napi_value retStrAttribute = nullptr;
    retStrAttribute = NapiHelper::GetNameProperty(env, result, "strAttribute");
    ASSERT_CHECK_VALUE_TYPE(env, retStrAttribute, napi_string);
}

HWTEST_F(NativeEngineTest, GetBooleanValue001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value result = nullptr;
    result = NapiHelper::CreateBooleanValue(env, true);
    ASSERT_CHECK_VALUE_TYPE(env, result, napi_boolean);

    bool resultValue = false;
    resultValue = NapiHelper::GetBooleanValue(env, result);
    ASSERT_TRUE(resultValue);
}

HWTEST_F(NativeEngineTest, StrictEqual001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;

    const char* testStringStr = "test";
    napi_value testString = nullptr;
    napi_create_string_utf8(env, testStringStr, strlen(testStringStr), &testString);
    bool isStrictEquals = false;

    isStrictEquals = NapiHelper::StrictEqual(env, testString, testString);
    ASSERT_TRUE(isStrictEquals);
}

HWTEST_F(NativeEngineTest, GetConstructorName001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;

    napi_value testClass = nullptr;
    napi_define_class(
        env, "TestClass", NAPI_AUTO_LENGTH,
        [](napi_env env, napi_callback_info info) -> napi_value {
            napi_value thisVar = nullptr;
            napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);

            return thisVar;
        }, nullptr, 0, nullptr, &testClass);

    napi_value testInstance = nullptr;
    ASSERT_CHECK_CALL(napi_new_instance(env, testClass, 0, nullptr, &testInstance));

    std::string resVal = NapiHelper::GetConstructorName(env, testInstance);
    ASSERT_EQ(resVal, "TestClass");
}
