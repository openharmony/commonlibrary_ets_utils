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

#include "test.h"

#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include "js_convertxml.h"
#include "utils/log.h"

#define ASSERT_CHECK_CALL(call)   \
    {                             \
        ASSERT_EQ(call, napi_ok); \
    }

#define ASSERT_CHECK_VALUE_TYPE(env, value, type)               \
    {                                                           \
        napi_valuetype valueType = napi_undefined;              \
        ASSERT_TRUE((value != nullptr));                        \
        ASSERT_CHECK_CALL(napi_typeof(env, value, &valueType)); \
        ASSERT_EQ(valueType, type);                             \
    }


/* @tc.name: ConstructorTest001
 * @tc.desc: Test whether write a elemnet start tag with the given name successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConstructorTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::Xml::ConvertXml convertXml = OHOS::Xml::ConvertXml();
    std::string str1 = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
    std::string str2 = "<note importance=\"high\" logged=\"true\"><todo>Play</todo></note>";
    std::string strXml = str1 + str2;
    napi_valuetype valuetype = napi_undefined;

    napi_typeof(env, convertXml.Convert(env, strXml), &valuetype);
    bool isObj = valuetype == napi_valuetype::napi_object;
    ASSERT_TRUE(isObj);
}

/* @tc.name: ConstructorTest002
 * @tc.desc: Test whether write a elemnet start tag with the given name successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConstructorTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string str1 = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
    std::string str2 = "<note importance=\"high\" logged=\"true\"><todo>Play</todo></note>";
    std::string strXml = str1 + str2;
    napi_value object = nullptr;
    const char* utf8Name = "_declaration";
    napi_create_object(env, &object);
    bool isHas = false;
    OHOS::Xml::ConvertXml convertXml = OHOS::Xml::ConvertXml();
    
    object = convertXml.Convert(env, strXml);
    napi_has_named_property(env, object, utf8Name, &isHas);
    ASSERT_TRUE(isHas);
}

/* @tc.name: ConstructorTest003
 * @tc.desc: Test whether write a elemnet start tag with the given name successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConstructorTest003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string str1 = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
    std::string str2 = "<note importance=\"high\" logged=\"true\"><todo>Play</todo></note>";
    std::string strXml = str1 + str2;
    napi_value object = nullptr;
    const char* utf8Name = "_declaration";
    napi_create_object(env, &object);
    bool isHas = false;
    OHOS::Xml::ConvertXml convertXml = OHOS::Xml::ConvertXml();
    
    object = convertXml.Convert(env, strXml);
    napi_has_named_property(env, object,utf8Name, &isHas);
    ASSERT_TRUE(isHas);
}

/* @tc.name: ConvertTest001
 * @tc.desc: Test whether write a elemnet start tag with the given name successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConvertTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::Xml::ConvertXml convertXml = OHOS::Xml::ConvertXml();
    std::string str1 = "<?xml version=\"1.0\" encoding=\"utf-8\"?><note importance=\"high\" logged=\"true\">    ";
    std::string str2 = "<title>Happy</title>    <todo>Work</todo>    <todo>Play</todo></note>";
    std::string strXml = str1 + str2;
    napi_valuetype valuetype = napi_undefined;

    napi_typeof(env, convertXml.Convert(env, strXml), &valuetype);
    bool isObj = valuetype == napi_valuetype::napi_object;
    ASSERT_TRUE(isObj);
}

/* @tc.name: ConvertTest002
 * @tc.desc: Test whether write a elemnet start tag with the given name successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConvertTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string str1 = "<?xml version=\"1.0\" encoding=\"utf-8\"?><note importance=\"high\" logged=\"true\">    ";
    std::string str2 = "<title>Happy</title>    <todo>Work</todo>    <todo>Play</todo></note>";
    std::string strXml = str1 + str2;
    napi_value object = nullptr;
    const char* utf8Name = "_declaration";
    napi_create_object(env, &object);
    bool isHas = false;
    OHOS::Xml::ConvertXml convertXml = OHOS::Xml::ConvertXml();
    
    object = convertXml.Convert(env, strXml);
    napi_has_named_property(env, object, utf8Name, &isHas);
    ASSERT_TRUE(isHas);
}

/* @tc.name: ConvertTest003
 * @tc.desc: Test whether write a elemnet start tag with the given name successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConvertTest003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string str1 = "<?xml version=\"1.0\" encoding=\"utf-8\"?><note importance=\"high\" logged=\"true\">    ";
    std::string str2 = "<title>Happy</title>    <todo>Work</todo>    <todo>Play</todo></note>";
    std::string strXml = str1 + str2;
    napi_value object = nullptr;
    const char* utf8Name = "_elements";
    napi_create_object(env, &object);
    bool isHas = false;
    OHOS::Xml::ConvertXml convertXml = OHOS::Xml::ConvertXml();
    
    object = convertXml.Convert(env, strXml);
    napi_has_named_property(env, object, utf8Name, &isHas);
    ASSERT_TRUE(isHas);
}
