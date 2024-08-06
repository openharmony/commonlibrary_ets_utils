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
#include <securec.h>
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

HWTEST_F(NativeEngineTest, IsNotUndefinedTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value param = nullptr;
    std::string str = "test";
    napi_create_string_utf8(env, str.c_str(), NAPI_AUTO_LENGTH, &param);
    env = nullptr;
    bool res = NapiHelper::IsNotUndefined(env, param);
    ASSERT_FALSE(res);
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

HWTEST_F(NativeEngineTest, IsArrayTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value propNames = nullptr;
    bool res = NapiHelper::IsArray(env, propNames);

    ASSERT_FALSE(res);
}

HWTEST_F(NativeEngineTest, IsArrayTest003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;

    napi_value result = nullptr;
    ASSERT_CHECK_CALL(napi_create_object(env, &result));
    ASSERT_CHECK_VALUE_TYPE(env, result, napi_object);

    const char testStr[] = "1234567";
    napi_value strAttribute = nullptr;
    ASSERT_CHECK_CALL(napi_create_string_utf8(env, testStr, strlen(testStr), &strAttribute));

    bool res = NapiHelper::IsArray(env, strAttribute);

    ASSERT_FALSE(res);
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

HWTEST_F(NativeEngineTest, IsCallable003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;

    auto func = [](napi_env env, napi_callback_info info) -> napi_value {
        return nullptr;
    };

    napi_value funcValue = nullptr;
    napi_create_function(env, "testFunc", NAPI_AUTO_LENGTH, func, nullptr, &funcValue);
    ASSERT_NE(funcValue, nullptr);

    napi_ref resultRef = nullptr;

    bool result = false;
    result = NapiHelper::IsCallable(env, resultRef);
    ASSERT_FALSE(result);
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
    delete[] script;
    ASSERT_EQ(str, s);
}

HWTEST_F(NativeEngineTest, GetString002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value value = nullptr;

    size_t MAX_CHAR_LENGTH = 1025;
    char* buffer = new char[MAX_CHAR_LENGTH];
    memset_s(buffer, MAX_CHAR_LENGTH, 1, MAX_CHAR_LENGTH);
    napi_create_string_utf8(env, buffer, MAX_CHAR_LENGTH, &value);

    char* script = NapiHelper::GetString(env, value);
    size_t len = strlen(script);
    delete[] script;
    delete[] buffer;
    ASSERT_EQ(len, 1024);
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
    napi_set_named_property(env, testInstance, "constructor", testClass);
    std::string resVal = NapiHelper::GetConstructorName(env, testInstance);
    ASSERT_EQ(resVal, "TestClass");
}

HWTEST_F(NativeEngineTest, IsTypeForNapiValueTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value param = nullptr;
    ASSERT_FALSE(NapiHelper::IsTypeForNapiValue(env, param, napi_undefined));
}

HWTEST_F(NativeEngineTest, IsTypeForNapiValueTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value param = nullptr;
    std::string str = "test";
    napi_create_string_utf8(env, str.c_str(), NAPI_AUTO_LENGTH, &param);
    env = nullptr;
    ASSERT_FALSE(NapiHelper::IsTypeForNapiValue(env, param, napi_string));
}

HWTEST_F(NativeEngineTest, IsNumberTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    int32_t testNumber = 12345;
    napi_value numberAttribute = nullptr;
    ASSERT_CHECK_CALL(napi_create_int32(env, testNumber, &numberAttribute));

    bool res = NapiHelper::IsNumber(env, numberAttribute);
    ASSERT_TRUE(res);
}

HWTEST_F(NativeEngineTest, IsArrayBufferTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* arrayBufferPtr = nullptr;
    size_t arrayBufferSize = 1024;
    napi_create_arraybuffer(env, arrayBufferSize, &arrayBufferPtr, &arrayBuffer);
    ASSERT_NE(arrayBuffer, nullptr);
    ASSERT_NE(arrayBufferPtr, nullptr);

    bool res = NapiHelper::IsArrayBuffer(env, arrayBuffer);
    ASSERT_TRUE(res);
}

HWTEST_F(NativeEngineTest, IsArrayBufferTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arraybuffer = nullptr;

    bool res = NapiHelper::IsArrayBuffer(env, arraybuffer);
    ASSERT_FALSE(res);
}

HWTEST_F(NativeEngineTest, IsArrayBufferTest003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    int32_t testNumber = 12345;
    napi_value numberAttribute = nullptr;
    ASSERT_CHECK_CALL(napi_create_int32(env, testNumber, &numberAttribute));

    bool res = NapiHelper::IsArrayBuffer(env, numberAttribute);
    ASSERT_FALSE(res);
}

HWTEST_F(NativeEngineTest, IsAsyncFunctionTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value value = nullptr;

    bool res = NapiHelper::IsAsyncFunction(env, value);
    ASSERT_FALSE(res);
}

HWTEST_F(NativeEngineTest, IsConcurrentFunctionTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value value = nullptr;

    bool res = NapiHelper::IsConcurrentFunction(env, value);
    ASSERT_FALSE(res);
}

HWTEST_F(NativeEngineTest, IsGeneratorFunctionTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value value = nullptr;

    bool res = NapiHelper::IsGeneratorFunction(env, value);
    ASSERT_FALSE(res);
}

HWTEST_F(NativeEngineTest, IsStrictEqualTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value lhs = nullptr;
    std::string lstr = "test";
    napi_create_string_utf8(env, lstr.c_str(), NAPI_AUTO_LENGTH, &lhs);

    napi_value rhs = nullptr;
    std::string rstr = "test";
    napi_create_string_utf8(env, rstr.c_str(), NAPI_AUTO_LENGTH, &rhs);

    bool res = NapiHelper::IsStrictEqual(env, lhs, rhs);

    ASSERT_TRUE(res);
}

HWTEST_F(NativeEngineTest, IsStrictEqualTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value lhs = nullptr;
    std::string lstr = "test";
    napi_create_string_utf8(env, lstr.c_str(), NAPI_AUTO_LENGTH, &lhs);

    napi_value rhs = nullptr;
    std::string rstr = "test2";
    napi_create_string_utf8(env, rstr.c_str(), NAPI_AUTO_LENGTH, &rhs);

    bool res = NapiHelper::IsStrictEqual(env, lhs, rhs);
    ASSERT_FALSE(res);
}

HWTEST_F(NativeEngineTest, IsSendableTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value value = nullptr;
    ASSERT_CHECK_CALL(napi_create_sendable_map(env, &value));

    bool res = NapiHelper::IsSendable(env, value);
    ASSERT_TRUE(res);
}

HWTEST_F(NativeEngineTest, IsSendableTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value value = nullptr;

    bool res = NapiHelper::IsSendable(env, value);
    ASSERT_FALSE(res);
}

HWTEST_F(NativeEngineTest, HasNamePropertyTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value result = nullptr;
    ASSERT_CHECK_CALL(napi_create_object(env, &result));

    const char testStr[] = "1234567";
    napi_value strAttribute = nullptr;
    ASSERT_CHECK_CALL(napi_create_string_utf8(env, testStr, strlen(testStr), &strAttribute));
    const char* name = "strAttribute";
    ASSERT_CHECK_CALL(napi_set_named_property(env, result, name, strAttribute));

    bool res = NapiHelper::HasNameProperty(env, result, name);
    ASSERT_TRUE(res);
}

HWTEST_F(NativeEngineTest, HasNamePropertyTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value result = nullptr;
    ASSERT_CHECK_CALL(napi_create_object(env, &result));
    const char* name = "test";

    bool res = NapiHelper::HasNameProperty(env, result, name);
    ASSERT_FALSE(res);
}

HWTEST_F(NativeEngineTest, CreatePromiseTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_deferred deferred = nullptr;

    napi_value promise = NapiHelper::CreatePromise(env, &deferred);
    ASSERT_NE(promise, nullptr);
}

HWTEST_F(NativeEngineTest, GetArrayLengthTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;

    napi_value result = NapiHelper::CreateObject(env);
    auto func = [](napi_env env, napi_callback_info info) -> napi_value {
        return nullptr;
    };
    napi_value funcAttribute = nullptr;
    napi_create_function(env, "testFunc", NAPI_AUTO_LENGTH, func, nullptr, &funcAttribute);

    napi_value funcKey = nullptr;
    const char* funcKeyStr = "func";
    napi_create_string_latin1(env, funcKeyStr, strlen(funcKeyStr), &funcKey);
    napi_set_property(env, result, funcKey, funcAttribute);

    napi_value propFuncValue = nullptr;
    napi_get_property_names(env, result, &propFuncValue);

    uint32_t arrayLength = NapiHelper::GetArrayLength(env, propFuncValue);
    ASSERT_EQ(arrayLength, static_cast<uint32_t>(1));
}

HWTEST_F(NativeEngineTest, GetUint32ValueTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;

    uint32_t testValue = UINT32_MAX;
    napi_value result = nullptr;
    ASSERT_CHECK_CALL(napi_create_uint32(env, testValue, &result));
    ASSERT_CHECK_VALUE_TYPE(env, result, napi_number);

    uint32_t resultValue = NapiHelper::GetUint32Value(env, result);
    ASSERT_EQ(resultValue, UINT32_MAX);
}

HWTEST_F(NativeEngineTest, GetInt32ValueTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;

    int32_t testValue = INT32_MAX;
    napi_value result = nullptr;
    ASSERT_CHECK_CALL(napi_create_int32(env, testValue, &result));

    int32_t resultValue = NapiHelper::GetInt32Value(env, result);
    ASSERT_EQ(resultValue, INT32_MAX);
}

HWTEST_F(NativeEngineTest, IsExceptionPendingTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;

    napi_value funcValue = nullptr;
    napi_value exception = nullptr;
    napi_create_function(env, "testFunc", NAPI_AUTO_LENGTH,
        [](napi_env env, napi_callback_info info) -> napi_value {
            napi_value error = nullptr;
            napi_throw_error(env, "500", "Common error");
            return error;
        },
        nullptr, &funcValue);
    ASSERT_NE(funcValue, nullptr);

    napi_value recv = nullptr;
    napi_get_undefined(env, &recv);
    ASSERT_NE(recv, nullptr);
    napi_value funcResultValue = nullptr;
    napi_call_function(env, recv, funcValue, 0, nullptr, &funcResultValue);
    bool isExceptionPending = NapiHelper::IsExceptionPending(env);
    napi_is_exception_pending(env, &isExceptionPending);
    ASSERT_TRUE(isExceptionPending);

    napi_get_and_clear_last_exception(env, &exception);
}

HWTEST_F(NativeEngineTest, GetPrintStringTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;

    napi_value result = nullptr;

    std::string str = NapiHelper::GetPrintString(env, result);
    ASSERT_EQ(str, "");
}

HWTEST_F(NativeEngineTest, GetUint64ValueTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;

    uint64_t testValue = UINT64_MAX;
    napi_value result = nullptr;
    ASSERT_CHECK_CALL(napi_create_bigint_uint64(env, testValue, &result));

    int32_t resultValue = NapiHelper::GetUint64Value(env, result);
    ASSERT_EQ(resultValue, UINT64_MAX);
}

HWTEST_F(NativeEngineTest, GetElementTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;

    napi_value result = NapiHelper::CreateObject(env);

    const char testStr[] = "1234567";
    napi_value strAttribute = nullptr;
    ASSERT_CHECK_CALL(napi_create_string_utf8(env, testStr, strlen(testStr), &strAttribute));
    ASSERT_CHECK_VALUE_TYPE(env, strAttribute, napi_string);
    ASSERT_CHECK_CALL(napi_set_named_property(env, result, "strAttribute", strAttribute));

    napi_value retStrAttribute = nullptr;
    ASSERT_CHECK_CALL(napi_get_named_property(env, result, "strAttribute", &retStrAttribute));
    ASSERT_CHECK_VALUE_TYPE(env, retStrAttribute, napi_string);

    int32_t testNumber = 12345;
    napi_value numberAttribute = nullptr;
    ASSERT_CHECK_CALL(napi_create_int32(env, testNumber, &numberAttribute));
    ASSERT_CHECK_VALUE_TYPE(env, numberAttribute, napi_number);
    ASSERT_CHECK_CALL(napi_set_named_property(env, result, "numberAttribute", numberAttribute));

    napi_value propNames = nullptr;
    ASSERT_CHECK_CALL(napi_get_property_names(env, result, &propNames));
    ASSERT_CHECK_VALUE_TYPE(env, propNames, napi_object);
    bool isArray = NapiHelper::IsArray(env, propNames);
    ASSERT_TRUE(isArray);

    uint32_t arrayLength = NapiHelper::GetArrayLength(env, propNames);
    ASSERT_EQ(arrayLength, static_cast<uint32_t>(2));

    for (uint32_t i = 0; i < arrayLength; i++) {
        napi_value propName = NapiHelper::GetElement(env, propNames, i);
        ASSERT_CHECK_VALUE_TYPE(env, propName, napi_string);
    }
}