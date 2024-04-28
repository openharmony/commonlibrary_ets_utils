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

#include "test_xml.h"
#include "test.h"

#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include "js_xml.h"
#include "native_module_xml.h"
#include "securec.h"
#include "tools/log.h"

using namespace OHOS::xml;

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

static std::string g_testStr = "";
napi_value Method(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    size_t argc = 0;
    napi_value args[6] = { 0 }; // 6:six args
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, nullptr, &thisVar, nullptr));
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));

    napi_value name = args[0];
    napi_value value = args[1];

    std::string buffer1 = "";
    size_t bufferSize1 = 0;
    napi_get_value_string_utf8(env, name, nullptr, 0, &bufferSize1);
    buffer1.reserve(bufferSize1 + 1);
    buffer1.resize(bufferSize1);
    napi_get_value_string_utf8(env, name, buffer1.data(), bufferSize1 + 1, &bufferSize1);

    std::string buffer2 = "";
    size_t bufferSize2 = 0;
    napi_get_value_string_utf8(env, value, nullptr, 0, &bufferSize2);
    buffer2.reserve(bufferSize2 + 1);
    buffer2.resize(bufferSize2);
    napi_get_value_string_utf8(env, value, buffer2.data(), bufferSize2 + 1, &bufferSize2);
    g_testStr += buffer1 + buffer2;
    napi_value result = nullptr;
    napi_get_boolean(env, true, &result);
    return result;
}

napi_value TokenValueCallbackFunction(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    size_t argc = 0;
    napi_value args[6] = { 0 }; // 6:six args
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, nullptr, &thisVar, nullptr));
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));
    napi_value value = args[1];
    napi_value value3 = nullptr;
    napi_get_named_property(env, value, "getDepth", &value3);
    napi_get_named_property(env, value, "getColumnNumber", &value3);
    napi_get_named_property(env, value, "getLineNumber", &value3);
    napi_get_named_property(env, value, "getAttributeCount", &value3);
    napi_get_named_property(env, value, "getName", &value3);
    napi_get_named_property(env, value, "getNamespace", &value3);
    napi_get_named_property(env, value, "getPrefix", &value3);
    napi_get_named_property(env, value, "getText", &value3);
    napi_get_named_property(env, value, "isEmptyElementTag", &value3);
    napi_get_named_property(env, value, "isWhitespace", &value3);
    napi_valuetype result2 = napi_null;
    napi_typeof(env, value3, &result2);
    napi_value returnVal = nullptr;
    napi_call_function(env, thisVar, value3, 0, nullptr, &returnVal);

    napi_value result = nullptr;
    napi_get_boolean(env, true, &result);
    return result;
}

napi_value TokenValueCallbackFunc(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    size_t argc = 0;
    napi_value args[6] = { 0 }; // 6:six args
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, nullptr, &thisVar, nullptr));
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));
    napi_value value = args[1];
    napi_value value3 = nullptr;
    napi_get_named_property(env, value, "getDepth", &value3);
    napi_value returnVal = nullptr;
    napi_call_function(env, thisVar, value3, 0, nullptr, &returnVal);
    napi_get_named_property(env, value, "getColumnNumber", &value3);
    napi_call_function(env, thisVar, value3, 0, nullptr, &returnVal);
    napi_get_named_property(env, value, "getLineNumber", &value3);
    napi_call_function(env, thisVar, value3, 0, nullptr, &returnVal);
    napi_get_named_property(env, value, "getAttributeCount", &value3);
    napi_call_function(env, thisVar, value3, 0, nullptr, &returnVal);
    napi_get_named_property(env, value, "getName", &value3);
    napi_call_function(env, thisVar, value3, 0, nullptr, &returnVal);
    napi_get_named_property(env, value, "getNamespace", &value3);
    napi_call_function(env, thisVar, value3, 0, nullptr, &returnVal);
    napi_get_named_property(env, value, "getPrefix", &value3);
    napi_call_function(env, thisVar, value3, 0, nullptr, &returnVal);
    napi_get_named_property(env, value, "getText", &value3);
    napi_call_function(env, thisVar, value3, 0, nullptr, &returnVal);
    napi_get_named_property(env, value, "isEmptyElementTag", &value3);
    napi_call_function(env, thisVar, value3, 0, nullptr, &returnVal);
    napi_get_named_property(env, value, "isWhitespace", &value3);
    napi_call_function(env, thisVar, value3, 0, nullptr, &returnVal);

    napi_value result = nullptr;
    napi_get_boolean(env, true, &result);
    return result;
}

/* @tc.name: StartElementTest001
 * @tc.desc: Test whether write a elemnet start tag with the given name successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, StartElementTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note1");
    xmlSerializer.EndElement();
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), "<note1/>");
}

/* @tc.name: StartElementTest002
 * @tc.desc: Test whether write a elemnet start tag with the given name successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, StartElementTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note1");
    xmlSerializer.EndElement();
    xmlSerializer.StartElement("note2");
    xmlSerializer.EndElement();
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), "<note1/>\r\n<note2/>");
}

/* @tc.name: StartElementTest003
 * @tc.desc: Test whether write a elemnet start tag with the given name successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, StartElementTest003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note1");
    xmlSerializer.StartElement("note2");
    xmlSerializer.EndElement();
    xmlSerializer.EndElement();
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), "<note1>\r\n  <note2/>\r\n</note1>");
}

/* @tc.name: StartElementTest004
 * @tc.desc: Test whether write a elemnet start tag with the given name successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, StartElementTest004, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note1");
    xmlSerializer.StartElement("note2");
    xmlSerializer.StartElement("note3");
    xmlSerializer.EndElement();
    xmlSerializer.EndElement();
    xmlSerializer.EndElement();
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), "<note1>\r\n  <note2>\r\n    <note3/>\r\n  </note2>\r\n</note1>");
}

/* @tc.name: StartElementTest005
 * @tc.desc: Test whether write a elemnet start tag with the given name successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, StartElementTest005, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note1");
    xmlSerializer.StartElement("note2");
    xmlSerializer.EndElement();
    xmlSerializer.StartElement("note3");
    xmlSerializer.EndElement();
    xmlSerializer.EndElement();
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), "<note1>\r\n  <note2/>\r\n  <note3/>\r\n</note1>");
}

/* @tc.name: SetAttributesTest001
 * @tc.desc: Test whether write an attribute successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetAttributesTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note");
    xmlSerializer.SetAttributes("importance1", "high1");
    xmlSerializer.EndElement();
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), "<note importance1=\"high1\"/>");
}

/* @tc.name: SetAttributesTest002
 * @tc.desc: Test whether write an attribute successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetAttributesTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note");
    xmlSerializer.SetAttributes("importance1", "high1");
    xmlSerializer.SetAttributes("importance2", "high2");
    xmlSerializer.EndElement();
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), "<note importance1=\"high1\" importance2=\"high2\"/>");
}

/* @tc.name: SetAttributesTest003
 * @tc.desc: Test whether write an attribute successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetAttributesTest003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note");
    xmlSerializer.SetAttributes("importance1", "high1");
    xmlSerializer.SetAttributes("importance2", "high2");
    xmlSerializer.SetAttributes("importance3", "high3");
    xmlSerializer.EndElement();
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer),
                 "<note importance1=\"high1\" importance2=\"high2\" importance3=\"high3\"/>");
}

/* @tc.name: SetAttributesTest004
 * @tc.desc: Test whether write an attribute successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetAttributesTest004, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note");
    xmlSerializer.SetAttributes("importance1", "high1");
    xmlSerializer.SetAttributes("importance2", "high2");
    xmlSerializer.SetAttributes("importance3", "high3");
    xmlSerializer.SetAttributes("importance4", "high4");
    xmlSerializer.EndElement();
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer),
                 "<note importance1=\"high1\" importance2=\"high2\" importance3=\"high3\" importance4=\"high4\"/>");
}

/* @tc.name: SetAttributesTest005
 * @tc.desc: Test whether write an attribute successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetAttributesTest005, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note");
    xmlSerializer.SetAttributes("importance1", "high1");
    xmlSerializer.SetAttributes("importance2", "high2");
    xmlSerializer.SetAttributes("importance3", "high3");
    xmlSerializer.SetAttributes("importance4", "high4");
    xmlSerializer.SetAttributes("importance5", "high5");
    xmlSerializer.EndElement();
    std::string strPrior = "<note importance1=\"high1\" importance2=\"high2\" ";
    std::string strBack = "importance3=\"high3\" importance4=\"high4\" importance5=\"high5\"/>";
    std::string strEnd = strPrior + strBack;
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), strEnd.c_str());
}

/* @tc.name: AddEmptyElementTest001
 * @tc.desc: Test whether add an empty element successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, AddEmptyElementTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note");
    xmlSerializer.AddEmptyElement("a");
    xmlSerializer.EndElement();
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), "<note>\r\n  <a/>\r\n</note>");
}

/* @tc.name: AddEmptyElementTest002
 * @tc.desc: Test whether add an empty element successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, AddEmptyElementTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note");
    xmlSerializer.EndElement();
    xmlSerializer.AddEmptyElement("b");
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), "<note/>\r\n<b/>");
}

/* @tc.name: AddEmptyElementTest003
 * @tc.desc: Test whether add an empty element successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, AddEmptyElementTest003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.AddEmptyElement("c");
    xmlSerializer.StartElement("note");
    xmlSerializer.EndElement();
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), "<c/>\r\n<note/>");
}

/* @tc.name: AddEmptyElementTest004
 * @tc.desc: Test whether add an empty element successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, AddEmptyElementTest004, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.AddEmptyElement("d");
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), "<d/>");
}

/* @tc.name: AddEmptyElementTest005
 * @tc.desc: Test whether add an empty element successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, AddEmptyElementTest005, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note");
    xmlSerializer.AddEmptyElement("c");
    xmlSerializer.AddEmptyElement("d");
    xmlSerializer.EndElement();
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), "<note>\r\n  <c/>\r\n  <d/>\r\n</note>");
}

/* @tc.name: SetDeclarationTest001
 * @tc.desc: Test whether write xml declaration with encoding successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetDeclarationTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.SetDeclaration();
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), "<?xml version=\"1.0\" encoding=\"utf-8\"?>");
}

/* @tc.name: SetDeclarationTest002
 * @tc.desc: Test whether write xml declaration with encoding successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetDeclarationTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.SetDeclaration();
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), "<?xml version=\"1.0\" encoding=\"utf-8\"?>");
}

/* @tc.name: SetDeclarationTest003
 * @tc.desc: Test whether write xml declaration with encoding successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetDeclarationTest003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.SetDeclaration();
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), "<?xml version=\"1.0\" encoding=\"utf-8\"?>");
}

/* @tc.name: SetDeclarationTest004
 * @tc.desc: Test whether write xml declaration with encoding successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetDeclarationTest004, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.SetDeclaration();
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), "<?xml version=\"1.0\" encoding=\"utf-8\"?>");
}

/* @tc.name: SetDeclarationTest005
 * @tc.desc: Test whether write xml declaration with encoding successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetDeclarationTest005, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.SetDeclaration();
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), "<?xml version=\"1.0\" encoding=\"utf-8\"?>");
}

/* @tc.name: EndElementTest001
 * @tc.desc: Test whether write end tag of the element successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, EndElementTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note");
    xmlSerializer.EndElement();
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), "<note/>");
}

/* @tc.name: EndElementTest002
 * @tc.desc: Test whether write end tag of the element successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, EndElementTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note");
    xmlSerializer.SetAttributes("importance", "high");
    xmlSerializer.EndElement();
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), "<note importance=\"high\"/>");
}

/* @tc.name: EndElementTest003
 * @tc.desc: Test whether write end tag of the element successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, EndElementTest003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note");
    xmlSerializer.SetAttributes("impo", "hi");
    xmlSerializer.EndElement();
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), "<note impo=\"hi\"/>");
}

/* @tc.name: EndElementTest004
 * @tc.desc: Test whether write end tag of the element successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, EndElementTest004, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note1");
    xmlSerializer.StartElement("note2");
    xmlSerializer.EndElement();
    xmlSerializer.EndElement();
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), "<note1>\r\n  <note2/>\r\n</note1>");
}

/* @tc.name: EndElementTest005
 * @tc.desc: Test whether write end tag of the element successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, EndElementTest005, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note2");
    xmlSerializer.SetAttributes("importance", "high");
    xmlSerializer.EndElement();
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), "<note2 importance=\"high\"/>");
}

/* @tc.name: SetNamespaceTest001
 * @tc.desc: Test whether write the namespace of the current element tag successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetNamespaceTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.SetDeclaration();
    xmlSerializer.SetNamespace("h", "http://www.w3.org/TR/html4/");
    xmlSerializer.StartElement("note");
    xmlSerializer.EndElement();
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer),
                 "<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n<h:note xmlns:h=\"http://www.w3.org/TR/html4/\"/>");
}

/* @tc.name: SetNamespaceTest002
 * @tc.desc: Test whether write the namespace of the current element tag successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetNamespaceTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.SetDeclaration();
    xmlSerializer.SetNamespace("b", "http://www.w3.org/TR/html4/");
    xmlSerializer.StartElement("note");
    xmlSerializer.EndElement();
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer),
                 "<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n<b:note xmlns:b=\"http://www.w3.org/TR/html4/\"/>");
}

/* @tc.name: SetNamespaceTest003
 * @tc.desc: Test whether write the namespace of the current element tag successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetNamespaceTest003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.SetDeclaration();
    xmlSerializer.SetNamespace("h", "http://www.111/");
    xmlSerializer.StartElement("note");
    xmlSerializer.EndElement();
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer),
                 "<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n<h:note xmlns:h=\"http://www.111/\"/>");
}

/* @tc.name: SetNamespaceTest004
 * @tc.desc: Test whether write the namespace of the current element tag successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetNamespaceTest004, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.SetDeclaration();
    xmlSerializer.SetNamespace("h", "http://www.w3.org/TR/html4/");
    xmlSerializer.StartElement("note1");
    xmlSerializer.StartElement("note2");
    xmlSerializer.EndElement();
    xmlSerializer.EndElement();
    std::string strPrior = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n";
    std::string strBack = "<h:note1 xmlns:h=\"http://www.w3.org/TR/html4/\">\r\n  <h:note2/>\r\n</h:note1>";
    std::string strEnd = strPrior + strBack;
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), strEnd.c_str());
}

/* @tc.name: SetNamespaceTest005
 * @tc.desc: Test whether write the namespace of the current element tag successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetNamespaceTest005, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.SetDeclaration();
    xmlSerializer.SetNamespace("h", "http://www.w3.org/TR/html4/");
    xmlSerializer.StartElement("note1");
    xmlSerializer.StartElement("note2");
    xmlSerializer.EndElement();
    xmlSerializer.EndElement();
    std::string strPrior = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n";
    std::string strBack = "<h:note1 xmlns:h=\"http://www.w3.org/TR/html4/\">\r\n  <h:note2/>\r\n</h:note1>";
    std::string strEnd = strPrior + strBack;
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), strEnd.c_str());
}

/* @tc.name: SetCommentTest001
 * @tc.desc: Test write the comment successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetCommentTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note");
    xmlSerializer.SetComment("Hi!");
    xmlSerializer.EndElement();
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), "<note>\r\n  <!--Hi!-->\r\n</note>");
}

/* @tc.name: SetCommentTest002
 * @tc.desc: Test write the comment successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetCommentTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note");
    xmlSerializer.SetComment("Hello, World!");
    xmlSerializer.EndElement();
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), "<note>\r\n  <!--Hello, World!-->\r\n</note>");
}

/* @tc.name: SetCommentTest003
 * @tc.desc: Test write the comment successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetCommentTest003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.SetComment("Hello, World!");
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), "<!--Hello, World!-->");
}

/* @tc.name: SetCommentTest004
 * @tc.desc: Test write the comment successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetCommentTest004, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.SetComment("Hello, World!");
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), "<!--Hello, World!-->");
}

/* @tc.name: SetCommentTest005
 * @tc.desc: Test write the comment successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetCommentTest005, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.SetComment("Hello, World!");
    xmlSerializer.StartElement("note");
    xmlSerializer.EndElement();
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), "<!--Hello, World!-->\r\n<note/>");
}

/* @tc.name: Test001
 * @tc.desc: Test .
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetCDATATest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.SetCData("root SYSTEM");
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), "<![CDATA[root SYSTEM]]>");
}

/* @tc.name: SetCDATATest002
 * @tc.desc: Test whether Writes the CDATA successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetCDATATest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.SetCData("root SYSTEM \"http://www.test.org/test.dtd\"");
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), "<![CDATA[root SYSTEM \"http://www.test.org/test.dtd\"]]>");
}

/* @tc.name: SetCDATATest003
 * @tc.desc: Test whether Writes the CDATA successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetCDATATest003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note");
    xmlSerializer.EndElement();
    xmlSerializer.SetCData("root SYSTEM \"http://www.test.org/test.dtd\"");
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer),
                 "<note/>\r\n<![CDATA[root SYSTEM \"http://www.test.org/test.dtd\"]]>");
}

/* @tc.name: SetCDATATest004
 * @tc.desc: Test whether Writes the CDATA successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetCDATATest004, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.SetCData("root SYSTEM \"http://www.test.org/test.dtd\"");
    xmlSerializer.StartElement("note");
    xmlSerializer.EndElement();
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer),
                 "<![CDATA[root SYSTEM \"http://www.test.org/test.dtd\"]]>\r\n<note/>");
}

/* @tc.name: SetCDATATest005
 * @tc.desc: Test whether Writes the CDATA successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetCDATATest005, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note");
    xmlSerializer.SetCData("root SYSTEM \"http://www.test.org/test.dtd\"");
    xmlSerializer.EndElement();
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer),
                 "<note>\r\n  <![CDATA[root SYSTEM \"http://www.test.org/test.dtd\"]]>\r\n</note>");
}

/* @tc.name: SetTextTest001
 * @tc.desc: Test whether Writes the text successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetTextTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note");
    xmlSerializer.SetAttributes("importance", "high");
    xmlSerializer.SetText("Happy1");
    xmlSerializer.EndElement();
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), "<note importance=\"high\">Happy1</note>");
}

/* @tc.name: SetTextTest002
 * @tc.desc: Test whether Writes the text successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetTextTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note");
    xmlSerializer.SetAttributes("importance", "high");
    xmlSerializer.SetText("Happy2");
    xmlSerializer.EndElement();
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), "<note importance=\"high\">Happy2</note>");
}

/* @tc.name: SetTextTest003
 * @tc.desc: Test whether Writes the text successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetTextTest003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note");
    xmlSerializer.SetAttributes("importance", "high");
    xmlSerializer.SetText("Happy3");
    xmlSerializer.EndElement();
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), "<note importance=\"high\">Happy3</note>");
}

/* @tc.name: SetTextTest004
 * @tc.desc: Test whether Writes the text successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetTextTest004, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note");
    xmlSerializer.SetAttributes("importance", "high");
    xmlSerializer.SetText("Happy4");
    xmlSerializer.EndElement();
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), "<note importance=\"high\">Happy4</note>");
}

/* @tc.name: SetTextTest005
 * @tc.desc: Test whether Writes the text successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetTextTest005, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note");
    xmlSerializer.SetAttributes("importance", "high");
    xmlSerializer.SetText("Happy5");
    xmlSerializer.EndElement();
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), "<note importance=\"high\">Happy5</note>");
}
/* @tc.name: SetDocTypeTest001
 * @tc.desc: Test whether rites the DOCTYPE successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetDocTypeTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.SetDocType("root SYSTEM");
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), "<!DOCTYPE root SYSTEM>");
}
/* @tc.name: SetDocTypeTest002
 * @tc.desc: Test whether rites the DOCTYPE successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetDocTypeTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.SetDocType("root SYSTEM \"http://www.test.org/test.dtd\"");
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), "<!DOCTYPE root SYSTEM \"http://www.test.org/test.dtd\">");
}

/* @tc.name: SetDocTypeTest003
 * @tc.desc: Test whether rites the DOCTYPE successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetDocTypeTest003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note");
    xmlSerializer.EndElement();
    xmlSerializer.SetDocType("root SYSTEM \"http://www.test.org/test.dtd\"");
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer),
                 "<note/>\r\n<!DOCTYPE root SYSTEM \"http://www.test.org/test.dtd\">");
}

/* @tc.name: SetDocTypeTest004
 * @tc.desc: Test whether rites the DOCTYPE successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetDocTypeTest004, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.SetDocType("root SYSTEM \"http://www.test.org/test.dtd\"");
    xmlSerializer.StartElement("note");
    xmlSerializer.EndElement();
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer),
                 "<!DOCTYPE root SYSTEM \"http://www.test.org/test.dtd\">\r\n<note/>");
}

/* @tc.name: SetDocTypeTest005
 * @tc.desc: Test whether rites the DOCTYPE successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetDocTypeTest005, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note");
    xmlSerializer.SetDocType("root SYSTEM \"http://www.test.org/test.dtd\"");
    xmlSerializer.EndElement();
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer),
                 "<note>\r\n  <!DOCTYPE root SYSTEM \"http://www.test.org/test.dtd\">\r\n</note>");
}

/* @tc.name: XmlParseTest001
 * @tc.desc: To XML text to JavaScript object.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, XmlParseTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string str1 = "<?xml version=\"1.0\" encoding=\"utf-8\"?><!DOCTYPE note [\n<!ENTITY foo \"baa\">]>";
    std::string str2 = "<note importance=\"high\" logged=\"true\">";
    std::string str3 = "<![CDATA[\r\nfuncrion matchwo(a,6)\r\n{\r\nreturn 1;\r\n}\r\n]]>";
    std::string str4 = "<!--Hello, World!--> <company>John &amp; Hans</company> <title>Happy</title>";
    std::string str5 = "<title>Happy</title> <todo>Work</todo> <todo>Play</todo> <?go there?>";
    std::string str6 = "<a><b/></a> <h:table xmlns:h=\"http://www.w3.org/TR/html4/\"> <h:tr>";
    std::string str7 = "<h:td>Apples</h:td> <h:td>Bananas</h:td> </h:tr>";
    std::string str8 = "</h:table></note>";
    std::string strXml = str1 + str2 + str3 + str4 + str5 + str6 + str7 + str8;
    g_testStr = "";
    OHOS::xml::XmlPullParser xmlPullParser(strXml, "utf-8");
    napi_value options = nullptr;
    napi_create_object(env, &options);
    const char* key1 = "supportDoctype";
    const char* key2 = "ignoreNameSpace";
    const char* key3 = "tagValueCallbackFunction";
    napi_value object = nullptr;
    napi_create_object(env, &object);
    napi_value value1 = nullptr;
    napi_value value2 = nullptr;
    napi_get_boolean(env, true, &value1);
    napi_get_boolean(env, false, &value2);
    napi_value value3 = nullptr;
    std::string cbName = "Method";
    napi_create_function(env, cbName.c_str(), cbName.size(), Method, nullptr, &value3);
    napi_set_named_property(env, object, key1, value1);
    napi_set_named_property(env, object, key2, value2);
    napi_set_named_property(env, object, key3, value3);
    xmlPullParser.DealOptionInfo(env, object);
    xmlPullParser.Parse(env, options);
    std::string res1 = " note [\n<!ENTITY foo \"baa\">]note\r\nfuncrion matchwo(a,6)\r\n{\r\nreturn 1;\r\n}\r\n";
    std::string res2 = "Hello, World! companyJohn & Hanscompany titleHappytitletitleHappytitle";
    std::string res3 = " todoWorktodo todoPlaytodo go thereabba table trtdApplestd tdBananastd trtablenote";
    std::string result = res1 + res2 + res3;
    ASSERT_STREQ(g_testStr.c_str(), result.c_str());
}

/* @tc.name: XmlParseTest002
 * @tc.desc: To XML text to JavaScript object.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, XmlParseTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string str1 = "<?xml version=\"1.0\" encoding=\"utf-8\"?><!DOCTYPE note [\n<!ENTITY foo \"baa\">]>";
    std::string str2 = "<note importance=\"high\" logged=\"true\">";
    std::string str3 = "    <![CDATA[\r\nfuncrion matchwo(a,6)\r\n{\r\nreturn 1;\r\n}\r\n]]>";
    std::string str4 = "    <!--Hello, World!-->    <company>John &amp; Hans</company>    <title>Happy</title>";
    std::string str5 = "    <title>Happy</title>    <todo>Work</todo>    <todo>Play</todo>    <?go there?>";
    std::string str6 = "    <a><b/></a>    <h:table xmlns:h=\"http://www.w3.org/TR/html4/\">        <h:tr>";
    std::string str7 = "            <h:td>Apples</h:td>            <h:td>Bananas</h:td>        </h:tr>";
    std::string str8 = "    </h:table></note>";
    std::string strXml = str1 + str2 + str3 + str4 + str5 + str6 + str7 + str8;
    g_testStr = "";
    OHOS::xml::XmlPullParser xmlPullParser(strXml, "utf-8");
    napi_value options = nullptr;
    napi_create_object(env, &options);
    const char* key1 = "supportDoctype";
    const char* key2 = "ignoreNameSpace";
    const char* key3 = "attributeValueCallbackFunction";
    napi_value object = nullptr;
    napi_create_object(env, &object);
    napi_value value1 = nullptr;
    napi_value value2 = nullptr;
    napi_get_boolean(env, false, &value1);
    napi_get_boolean(env, true, &value2);
    napi_value value3 = nullptr;
    std::string cbName = "Method";
    napi_create_function(env, cbName.c_str(), cbName.size(), Method, nullptr, &value3);
    napi_set_named_property(env, object, key1, value1);
    napi_set_named_property(env, object, key2, value2);
    napi_set_named_property(env, object, key3, value3);
    xmlPullParser.DealOptionInfo(env, object);
    xmlPullParser.Parse(env, options);
    ASSERT_STREQ(g_testStr.c_str(), "importancehighloggedtruexmlns:hhttp://www.w3.org/TR/html4/");
}

/* @tc.name: XmlParseTest003
 * @tc.desc: To XML text to JavaScript object.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, XmlParseTest003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string str1 = "<?xml version=\"1.0\" encoding=\"utf-8\"?><!DOCTYPE note [\n<!ENTITY foo \"baa\">]>";
    std::string str2 = "<note importance=\"high\" logged=\"true\">";
    std::string str3 = "<![CDATA[\r\nfuncrion matchwo(a,6)\r\n{\r\nreturn 1;\r\n}\r\n]]>";
    std::string str4 = "<!--Hello, World!--> <company>John &amp; Hans</company> <title>Happy</title>";
    std::string str5 = "<title>Happy</title> <todo>Work</todo> <todo>Play</todo> <?go there?>";
    std::string str6 = "<a><b/></a> <h:table xmlns:h=\"http://www.w3.org/TR/html4/\"> <h:tr>";
    std::string str7 = "<h:td>Apples</h:td> <h:td>Bananas</h:td> </h:tr>";
    std::string str8 = "</h:table></note>";
    std::string strXml = str1 + str2 + str3 + str4 + str5 + str6 + str7 + str8;
    g_testStr = "";
    OHOS::xml::XmlPullParser xmlPullParser(strXml, "utf-8");
    napi_value options = nullptr;
    napi_create_object(env, &options);
    const char* key1 = "supportDoctype";
    const char* key2 = "ignoreNameSpace";
    const char* key3 = "tagValueCallbackFunction";
    napi_value object = nullptr;
    napi_create_object(env, &object);
    napi_value value1 = nullptr;
    napi_value value2 = nullptr;
    napi_get_boolean(env, false, &value1);
    napi_get_boolean(env, true, &value2);
    napi_value value3 = nullptr;
    std::string cbName = "Method";
    napi_create_function(env, cbName.c_str(), cbName.size(), Method, nullptr, &value3);
    napi_set_named_property(env, object, key1, value1);
    napi_set_named_property(env, object, key2, value2);
    napi_set_named_property(env, object, key3, value3);
    xmlPullParser.DealOptionInfo(env, object);
    xmlPullParser.Parse(env, options);
    std::string res1 = "note\r\nfuncrion matchwo(a,6)\r\n{\r\nreturn 1;\r\n}\r\nHello, World! companyJohn &";
    std::string res2 = " Hanscompany titleHappytitletitleHappytitle todoWorktodo todoPlaytodo go thereabba h:table";
    std::string res3 = " h:trh:tdApplesh:td h:tdBananash:td h:trh:tablenote";
    std::string result = res1 + res2 + res3;
    ASSERT_STREQ(g_testStr.c_str(), result.c_str());
}

/* @tc.name: XmlParseTest004
 * @tc.desc: To XML text to JavaScript object.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, XmlParseTest004, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string str1 = "<?xml version=\"1.0\" encoding=\"utf-8\"?><!DOCTYPE note [\n<!ENTITY foo \"baa\">]>";
    std::string str2 = "<note importance=\"high\" logged=\"true\">";
    std::string str3 = "    <![CDATA[\r\nfuncrion matchwo(a,6)\r\n{\r\nreturn 1;\r\n}\r\n]]>";
    std::string str4 = "    <!--Hello, World!-->    <company>John &amp; Hans</company>    <title>Happy</title>";
    std::string str5 = "    <title>Happy</title>    <todo>Work</todo>    <todo>Play</todo>    <?go there?>";
    std::string str6 = "    <a><b/></a>    <h:table xmlns:h=\"http://www.w3.org/TR/html4/\">        <h:tr>";
    std::string str7 = "            <h:td>Apples</h:td>            <h:td>Bananas</h:td>        </h:tr>";
    std::string str8 = "    </h:table></note>";
    std::string strXml = str1 + str2 + str3 + str4 + str5 + str6 + str7 + str8;
    g_testStr = "";
    OHOS::xml::XmlPullParser xmlPullParser(strXml, "utf-8");
    napi_value options = nullptr;
    napi_create_object(env, &options);
    const char* key1 = "supportDoctype";
    const char* key2 = "ignoreNameSpace";
    const char* key3 = "attributeValueCallbackFunction";
    napi_value object = nullptr;
    napi_create_object(env, &object);
    napi_value value1 = nullptr;
    napi_value value2 = nullptr;
    napi_get_boolean(env, true, &value1);
    napi_get_boolean(env, true, &value2);
    napi_value value3 = nullptr;
    std::string cbName = "Method";
    napi_create_function(env, cbName.c_str(), cbName.size(), Method, nullptr, &value3);
    napi_set_named_property(env, object, key1, value1);
    napi_set_named_property(env, object, key2, value2);
    napi_set_named_property(env, object, key3, value3);
    xmlPullParser.DealOptionInfo(env, object);
    xmlPullParser.Parse(env, options);
    ASSERT_STREQ(g_testStr.c_str(), "importancehighloggedtruexmlns:hhttp://www.w3.org/TR/html4/");
}

/* @tc.name: XmlParseTest005
 * @tc.desc: To XML text to JavaScript object.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, XmlParseTest005, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string str1 = "<?xml version=\"1.0\" encoding=\"utf-8\"?><!DOCTYPE note [\n<!ENTITY foo \"baa\">]>";
    std::string str2 = "<note importance=\"high\" logged=\"true\">";
    std::string str3 = "<![CDATA[\r\nfuncrion matchwo(a,6)\r\n{\r\nreturn 1;\r\n}\r\n]]>";
    std::string str4 = "<!--Hello, World!--> <company>John &amp; Hans</company> <title>Happy</title>";
    std::string str5 = "<title>Happy</title> <todo>Work</todo> <todo>Play</todo> <?go there?>";
    std::string str6 = "<a><b/></a> <h:table xmlns:h=\"http://www.w3.org/TR/html4/\"> <h:tr>";
    std::string str7 = "<h:td>Apples</h:td> <h:td>Bananas</h:td> </h:tr>";
    std::string str8 = "</h:table></note>";
    std::string strXml = str1 + str2 + str3 + str4 + str5 + str6 + str7 + str8;
    g_testStr = "";
    OHOS::xml::XmlPullParser xmlPullParser(strXml, "utf-8");
    napi_value options = nullptr;
    napi_create_object(env, &options);
    const char* key1 = "supportDoctype";
    const char* key2 = "ignoreNameSpace";
    const char* key3 = "tagValueCallbackFunction";
    napi_value object = nullptr;
    napi_create_object(env, &object);
    napi_value value1 = nullptr;
    napi_value value2 = nullptr;
    napi_get_boolean(env, true, &value1);
    napi_get_boolean(env, true, &value2);
    napi_value value3 = nullptr;
    std::string cbName = "Method";
    napi_create_function(env, cbName.c_str(), cbName.size(), Method, nullptr, &value3);
    napi_set_named_property(env, object, key1, value1);
    napi_set_named_property(env, object, key2, value2);
    napi_set_named_property(env, object, key3, value3);
    xmlPullParser.DealOptionInfo(env, object);
    xmlPullParser.Parse(env, options);
    std::string res1 = " note [\n<!ENTITY foo \"baa\">]note\r\nfuncrion matchwo(a,6)\r\n{\r\nreturn 1;\r\n}\r\n";
    std::string res2 = "Hello, World! companyJohn & Hanscompany titleHappytitletitleHappytitle todoWorktodo";
    std::string res3 = " todoPlaytodo go thereabba h:table h:trh:tdApplesh:td h:tdBananash:td h:trh:tablenote";
    std::string result = res1 + res2 + res3;
    ASSERT_STREQ(g_testStr.c_str(), result.c_str());
}

/* @tc.name: XmlParseTest006
 * @tc.desc: To XML text to JavaScript object.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, XmlParseTest006, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string str1 = "<?xml version=\"1.0\" encoding=\"utf-8\"?><!DOCTYPE note [\n<!ENTITY foo \"baa\">]>";
    std::string str2 = "<note importance=\"high\" logged=\"true\">";
    std::string str3 = "<![CDATA[\r\nfuncrion matchwo(a,6)\r\n{\r\nreturn 1;\r\n}\r\n]]>";
    std::string str4 = "<!--Hello, World!--> <company>John &amp; Hans</company> <title>Happy</title>";
    std::string str5 = "<title>Happy</title> <todo>Work</todo> <todo>Play</todo> <?go there?>";
    std::string str6 = "<a><b/></a> <h:table xmlns:h=\"http://www.w3.org/TR/html4/\"> <h:tr>";
    std::string str7 = "<h:td>Apples</h:td> <h:td>Bananas</h:td> </h:tr>";
    std::string str8 = "</h:table></note>";
    std::string strXml = str1 + str2 + str3 + str4 + str5 + str6 + str7 + str8;
    OHOS::xml::XmlPullParser xmlPullParser(strXml, "utf-8");
    napi_value options = nullptr;
    napi_create_object(env, &options);
    const char* key1 = "supportDoctype";
    const char* key2 = "ignoreNameSpace";
    const char* key3 = "tokenValueCallbackFunction";
    napi_value object = nullptr;
    napi_create_object(env, &object);
    napi_value value1 = nullptr;
    napi_value value2 = nullptr;
    napi_get_boolean(env, true, &value1);
    napi_get_boolean(env, true, &value2);
    napi_value value3 = nullptr;
    std::string cbName = "TokenValueCallbackFunction";
    napi_create_function(env, cbName.c_str(), cbName.size(), TokenValueCallbackFunction, nullptr, &value3);
    napi_set_named_property(env, object, key1, value1);
    napi_set_named_property(env, object, key2, value2);
    napi_set_named_property(env, object, key3, value3);
    xmlPullParser.DealOptionInfo(env, object);
    xmlPullParser.Parse(env, options);
    std::string res1 = " note [\n<!ENTITY foo \"baa\">]note\r\nfuncrion matchwo(a,6)\r\n{\r\nreturn 1;\r\n}\r\n";
    std::string res2 = "Hello, World! companyJohn & Hanscompany titleHappytitletitleHappytitle todoWorktodo";
    std::string res3 = " todoPlaytodo go thereabba h:table h:trh:tdApplesh:td h:tdBananash:td h:trh:tablenote";
    std::string result = res1 + res2 + res3;
    ASSERT_STREQ(g_testStr.c_str(), result.c_str());
}

/* @tc.name: XmlParseTest007
 * @tc.desc: To XML text to JavaScript object.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, XmlParseTest007, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string str1 = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
    std::string str2 = "<note importance=\"high\" logged=\"true\">";
    std::string str3 = "    <![CDATA[\r\nfuncrion matchwo(a,6)\r\n{\r\nreturn 1;\r\n}\r\n]]>";
    std::string str4 = "    <!--Hello, World!-->    <company>John &amp; Hans</company>    <title>Happy</title>";
    std::string str5 = "    <title>Happy</title>    <todo>Work</todo>    <todo>Play</todo>    <?go there?>";
    std::string str6 = "    <a><b/></a>    <h:table xmlns:h=\"http://www.w3.org/TR/html4/\">        <h:tr>";
    std::string str7 = "            <h:td>Apples</h:td>            <h:td>Bananas</h:td>        </h:tr>";
    std::string str8 = "    </h:table></note>";
    std::string strXml = str1 + str2 + str3 + str4 + str5 + str6 + str7 + str8;
    g_testStr = "";
    OHOS::xml::XmlPullParser xmlPullParser(strXml, "utf-8");
    napi_value options = nullptr;
    napi_create_object(env, &options);
    const char* key1 = "supportDoctype";
    const char* key2 = "ignoreNameSpace";
    const char* key3 = "attributeValueCallbackFunction";
    napi_value object = nullptr;
    napi_create_object(env, &object);
    napi_value value1 = nullptr;
    napi_value value2 = nullptr;
    napi_get_boolean(env, true, &value1);
    napi_get_boolean(env, true, &value2);
    napi_value value3 = nullptr;
    std::string cbName = "Method";
    napi_create_function(env, cbName.c_str(), cbName.size(), Method, nullptr, &value3);
    napi_set_named_property(env, object, key1, value1);
    napi_set_named_property(env, object, key2, value2);
    napi_set_named_property(env, object, key3, value3);
    xmlPullParser.DealOptionInfo(env, object);
    xmlPullParser.Parse(env, options);
    ASSERT_STREQ(g_testStr.c_str(), "importancehighloggedtruexmlns:hhttp://www.w3.org/TR/html4/");
}

/* @tc.name: XmlParseTest008
 * @tc.desc: To XML text to JavaScript object.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, XmlParseTest008, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string str1 = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
    std::string str2 = "<note importance=\"high\" logged=\"true\">";
    std::string str3 = "<![CDATA[\r\nfuncrion matchwo(a,6)\r\n{\r\nreturn 1;\r\n}\r\n]]>";
    std::string str4 = "<!--Hello, World!--> <company>John &amp; Hans</company> <title>Happy</title>";
    std::string str5 = "<title>Happy</title> <todo>Work</todo> <todo>Play</todo> <?go there?>";
    std::string str6 = "<a><b/></a> <h:table xmlns:h=\"http://www.w3.org/TR/html4/\"> <h:tr>";
    std::string str7 = "<h:td>Apples</h:td> <h:td>Bananas</h:td> </h:tr>";
    std::string str8 = "</h:table></note>";
    std::string strXml = str1 + str2 + str3 + str4 + str5 + str6 + str7 + str8;
    g_testStr = "";
    OHOS::xml::XmlPullParser xmlPullParser(strXml, "utf-8");
    napi_value options = nullptr;
    napi_create_object(env, &options);
    const char* key1 = "supportDoctype";
    const char* key2 = "ignoreNameSpace";
    const char* key3 = "tagValueCallbackFunction";
    napi_value object = nullptr;
    napi_create_object(env, &object);
    napi_value value1 = nullptr;
    napi_value value2 = nullptr;
    napi_get_boolean(env, true, &value1);
    napi_get_boolean(env, true, &value2);
    napi_value value3 = nullptr;
    std::string cbName = "Method";
    napi_create_function(env, cbName.c_str(), cbName.size(), Method, nullptr, &value3);
    napi_set_named_property(env, object, key1, value1);
    napi_set_named_property(env, object, key2, value2);
    napi_set_named_property(env, object, key3, value3);
    xmlPullParser.DealOptionInfo(env, object);
    xmlPullParser.Parse(env, options);
    std::string res1 = "note\r\nfuncrion matchwo(a,6)\r\n{\r\nreturn 1;\r\n}\r\nHello, World! companyJohn &";
    std::string res2 = " Hanscompany titleHappytitletitleHappytitle todoWorktodo todoPlaytodo go thereabba h:table ";
    std::string res3 = "h:trh:tdApplesh:td h:tdBananash:td h:trh:tablenote";
    std::string result = res1 + res2 + res3;
    ASSERT_STREQ(g_testStr.c_str(), result.c_str());
}

/* @tc.name: XmlParseTest009
 * @tc.desc: To XML text to JavaScript object.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, XmlParseTest009, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string str1 = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
    std::string str2 = "<note importance=\"high\" logged=\"true\">";
    std::string str3 = "<![CDATA[\r\nfuncrion matchwo(a,6)\r\n{\r\nreturn 1;\r\n}\r\n]]>";
    std::string str4 = "<!--Hello, World!--> <company>John &amp; Hans</company> <title>Happy</title>";
    std::string str5 = "<title>Happy</title> <todo>Work</todo> <todo>Play</todo> <?go there?>";
    std::string str6 = "<a><b/></a> <h:table xmlns:h=\"http://www.w3.org/TR/html4/\"> <h:tr>";
    std::string str7 = "<h:td>Apples</h:td> <h:td>Bananas</h:td> </h:tr>";
    std::string str8 = "</h:table></note>";
    std::string strXml = str1 + str2 + str3 + str4 + str5 + str6 + str7 + str8;
    OHOS::xml::XmlPullParser xmlPullParser(strXml, "utf-8");
    napi_value options = nullptr;
    napi_create_object(env, &options);
    const char* key1 = "supportDoctype";
    const char* key2 = "ignoreNameSpace";
    const char* key3 = "tokenValueCallbackFunction";
    napi_value object = nullptr;
    napi_create_object(env, &object);
    napi_value value1 = nullptr;
    napi_value value2 = nullptr;
    napi_get_boolean(env, true, &value1);
    napi_get_boolean(env, true, &value2);
    napi_value value3 = nullptr;
    std::string cbName = "TokenValueCallbackFunc";
    napi_create_function(env, cbName.c_str(), cbName.size(), TokenValueCallbackFunc, nullptr, &value3);
    napi_set_named_property(env, object, key1, value1);
    napi_set_named_property(env, object, key2, value2);
    napi_set_named_property(env, object, key3, value3);
    xmlPullParser.DealOptionInfo(env, object);
    xmlPullParser.Parse(env, options);
    std::string res1 = "note\r\nfuncrion matchwo(a,6)\r\n{\r\nreturn 1;\r\n}\r\nHello, World! companyJohn &";
    std::string res2 = " Hanscompany titleHappytitletitleHappytitle todoWorktodo todoPlaytodo go thereabba h:table";
    std::string res3 = " h:trh:tdApplesh:td h:tdBananash:td h:trh:tablenote";
    std::string result = res1 + res2 + res3;
    ASSERT_STREQ(g_testStr.c_str(), result.c_str());
}

/* @tc.name: XmlParseTest0010
 * @tc.desc: To XML text to JavaScript object.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, XmlParseTest0010, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string str1 = "<?xml version=\"1.0\" encoding=\"utf-8\"?><!DOCTYPE note [\n<!ENTITY foo \"baa\">]>";
    std::string str2 = "<note importance=\"high\" logged=\"true\">";
    std::string str3 = "<![CDATA[\r\nfuncrion matchwo(a,6)\r\n{\r\nreturn 1;\r\n}\r\n]]>";
    std::string str4 = "<!--Hello, World!--> <company>John &amp; Hans</company> <title>Happy</title>";
    std::string str5 = "<title>Happy</title> <todo>Work</todo> <todo>Play</todo> <?go there?>";
    std::string str6 = "<a><b/></a> <h:table xml:h=\"http://www.w3.org/TR/html4/\"> <h:tr>";
    std::string str7 = "<h:td>Apples</h:td> <h:td>Bananas</h:td> </h:tr>";
    std::string str8 = "</h:table></note>";
    std::string strXml = str1 + str2 + str3 + str4 + str5 + str6 + str7 + str8;
    g_testStr = "";
    OHOS::xml::XmlPullParser xmlPullParser(strXml, "utf-8");
    napi_value options = nullptr;
    napi_create_object(env, &options);
    const char* key1 = "supportDoctype";
    const char* key2 = "ignoreNameSpace";
    const char* key3 = "tagValueCallbackFunction";
    napi_value object = nullptr;
    napi_create_object(env, &object);
    napi_value value1 = nullptr;
    napi_value value2 = nullptr;
    napi_get_boolean(env, true, &value1);
    napi_get_boolean(env, false, &value2);
    napi_value value3 = nullptr;
    std::string cbName = "Method";
    napi_create_function(env, cbName.c_str(), cbName.size(), Method, nullptr, &value3);
    napi_set_named_property(env, object, key1, value1);
    napi_set_named_property(env, object, key2, value2);
    napi_set_named_property(env, object, key3, value3);
    xmlPullParser.DealOptionInfo(env, object);
    xmlPullParser.Parse(env, options);
    std::string res1 = " note [\n<!ENTITY foo \"baa\">]note\r\nfuncrion matchwo(a,6)\r\n{\r\nreturn 1;\r\n}\r\n";
    std::string res2 = "Hello, World! companyJohn & Hanscompany titleHappytitletitleHappytitle todoWorktodo";
    std::string res3 = " todoPlaytodo go thereabba table trtdApplestd tdBananastd trtablenote";
    std::string result = res1 + res2 + res3;
    ASSERT_STREQ(g_testStr.c_str(), result.c_str());
}

/* @tc.name: XmlParseTest0011
 * @tc.desc: To XML text to JavaScript object.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, XmlParseTest0011, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string str1 = "<?xml version=\"1.0\" encoding=\"utf-8\"?><!DOCTYPE note";
    std::string str2 = " [\n<!ATTLIST operand type src \"dest\">]><note importance=\"high\" logged=\"true\">";
    std::string str3 = "    <![CDATA[\r\nfuncrion matchwo(a,6)\r\n{\r\nreturn 1;\r\n}\r\n]]>";
    std::string str4 = "    <!--Hello, World!-->    <company>John &amp; Hans</company>    <title>Happy</title>";
    std::string str5 = "    <title>Happy</title>    <todo>Work</todo>    <todo>Play</todo>    <?go there?>";
    std::string str6 = "    <a><b/></a>    <h:table xmlns:h=\"http://www.w3.org/TR/html4/\">        <h:tr>";
    std::string str7 = "            <h:td>Apples</h:td>            <h:td>Bananas</h:td>        </h:tr>";
    std::string str8 = "    </h:table></note>";
    std::string strXml = str1 + str2 + str3 + str4 + str5 + str6 + str7 + str8;
    g_testStr = "";
    OHOS::xml::XmlPullParser xmlPullParser(strXml, "utf-8");
    napi_value options = nullptr;
    napi_create_object(env, &options);
    const char* key1 = "supportDoctype";
    const char* key2 = "ignoreNameSpace";
    const char* key3 = "tokenValueCallbackFunction";
    napi_value object = nullptr;
    napi_create_object(env, &object);
    napi_value value1 = nullptr;
    napi_value value2 = nullptr;
    napi_get_boolean(env, true, &value1);
    napi_get_boolean(env, true, &value2);
    napi_value value3 = nullptr;
    std::string cbName = "TokenValueCallbackFunction";
    napi_create_function(env, cbName.c_str(), cbName.size(), TokenValueCallbackFunction, nullptr, &value3);
    napi_set_named_property(env, object, key1, value1);
    napi_set_named_property(env, object, key2, value2);
    napi_set_named_property(env, object, key3, value3);
    xmlPullParser.DealOptionInfo(env, object);
    xmlPullParser.Parse(env, options);
    ASSERT_STREQ(g_testStr.c_str(), "");
}

/* @tc.name: XmlParseTest0012
 * @tc.desc: To XML text to JavaScript object.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, XmlParseTest0012, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string str1 = "<?xml version=\"1.0\" encoding=\"utf-8\"?><!DOCTYPE note [\n<!ELEMENT>]>";
    std::string str2 = "<note importance=\"high\" logged=\"true\">";
    std::string str3 = "    <![CDATA[\r\nfuncrion matchwo(a,6)\r\n{\r\nreturn 1;\r\n}\r\n]]>";
    std::string str4 = "    <!--Hello, World!-->    <company>John &amp; Hans</company>    <title>Happy</title>";
    std::string str5 = "    <title>Happy</title>    <todo>Work</todo>    <todo>Play</todo>    <?go there?>";
    std::string str6 = "    <a><b/></a>    <h:table xmlns:h=\"http://www.w3.org/TR/html4/\">        <h:tr>";
    std::string str7 = "            <h:td>Apples</h:td>            <h:td>Bananas</h:td>        </h:tr>";
    std::string str8 = "    </h:table></note>";
    std::string strXml = str1 + str2 + str3 + str4 + str5 + str6 + str7 + str8;
    OHOS::xml::XmlPullParser xmlPullParser(strXml, "utf-8");
    napi_value options = nullptr;
    napi_create_object(env, &options);
    const char* key1 = "supportDoctype";
    const char* key2 = "ignoreNameSpace";
    const char* key3 = "tokenValueCallbackFunction";
    napi_value object = nullptr;
    napi_create_object(env, &object);
    napi_value value1 = nullptr;
    napi_value value2 = nullptr;
    napi_get_boolean(env, true, &value1);
    napi_get_boolean(env, true, &value2);
    napi_value value3 = nullptr;
    std::string cbName = "TokenValueCallbackFunction";
    napi_create_function(env, cbName.c_str(), cbName.size(), TokenValueCallbackFunction, nullptr, &value3);
    napi_set_named_property(env, object, key1, value1);
    napi_set_named_property(env, object, key2, value2);
    napi_set_named_property(env, object, key3, value3);
    xmlPullParser.DealOptionInfo(env, object);
    xmlPullParser.Parse(env, options);
    ASSERT_STREQ(g_testStr.c_str(), "");
}

/* @tc.name: Xmlfunctest001
 * @tc.desc: To XML text to JavaScript object.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, Xmlfunctest001, testing::ext::TestSize.Level0)
{
    std::string strXml = "<?xml version=\"1.0\" encoding=\"utf-8\"?>    <title>Happy</title>    <todo>Work</todo>";
    OHOS::xml::XmlPullParser xmlPullParser(strXml, "utf-8");
    ASSERT_EQ(xmlPullParser.GetColumnNumber(), 1);
    ASSERT_EQ(xmlPullParser.GetDepth(), 0);
    ASSERT_EQ(xmlPullParser.GetLineNumber(), 1);
    ASSERT_STREQ(xmlPullParser.GetName().c_str(), "");
    ASSERT_STREQ(xmlPullParser.GetPrefix().c_str(), "");
    ASSERT_STREQ(xmlPullParser.GetText().c_str(), "");
    ASSERT_FALSE(xmlPullParser.IsEmptyElementTag());
    ASSERT_EQ(xmlPullParser.GetAttributeCount(), 0);
    ASSERT_FALSE(xmlPullParser.IsWhitespace());
    ASSERT_STREQ(xmlPullParser.GetNamespace().c_str(), "");
}

/* @tc.name: XmlSerializertest001
 * @tc.desc: To XML text to JavaScript object.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, XmlSerializertest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value exports = nullptr;
    napi_create_object(env, &exports);
    OHOS::xml::XmlSerializerInit(env, exports);
    napi_value xmlSerializerClass = nullptr;
    napi_get_named_property(env, exports, "XmlSerializer", &xmlSerializerClass);

    napi_value args[2]; // 2: number of arguments
    size_t length = 2048; // allocate an ArrayBuffer with a size of 2048 bytes
    void* pBuffer = nullptr;
    napi_create_arraybuffer(env, length, &pBuffer, &args[0]);
    std::string encoding = "utf-8";
    napi_create_string_utf8(env, encoding.c_str(), encoding.size(), &args[1]);
    napi_value instance = nullptr;
    napi_new_instance(env, xmlSerializerClass, 2, args, &instance); // 2: number of arguments

    std::string name = "importance";
    napi_create_string_utf8(env, name.c_str(), name.size(), &args[0]);
    std::string value = "high";
    napi_create_string_utf8(env, value.c_str(), value.size(), &args[1]);
    napi_value testFunc = nullptr;
    napi_get_named_property(env, instance, "setAttributes", &testFunc);
    napi_value funcResultValue = nullptr;
    napi_call_function(env, instance, testFunc, 2, args, &funcResultValue); // 2: number of arguments
    ASSERT_NE(funcResultValue, nullptr);

    napi_get_named_property(env, instance, "setDeclaration", &testFunc);
    napi_call_function(env, instance, testFunc, 0, nullptr, &funcResultValue);
    ASSERT_NE(funcResultValue, nullptr);

    name = "note";
    napi_value val = nullptr;
    napi_create_string_utf8(env, name.c_str(), name.size(), &val);
    napi_get_named_property(env, instance, "startElement", &testFunc);
    napi_call_function(env, instance, testFunc, 1, &val, &funcResultValue);
    ASSERT_NE(funcResultValue, nullptr);

    napi_get_named_property(env, instance, "endElement", &testFunc);
    napi_call_function(env, instance, testFunc, 0, nullptr, &funcResultValue);
    ASSERT_NE(funcResultValue, nullptr);

    name = "h";
    napi_create_string_utf8(env, name.c_str(), name.size(), &args[0]);
    value = "http://www.w3.org/TR/html4/";
    napi_create_string_utf8(env, value.c_str(), value.size(), &args[1]);
    napi_get_named_property(env, instance, "setNamespace", &testFunc);
    napi_call_function(env, instance, testFunc, 2, args, &funcResultValue); // 2: number of arguments
    ASSERT_NE(funcResultValue, nullptr);

    name = "Hello, World!";
    napi_create_string_utf8(env, name.c_str(), name.size(), &val);
    napi_get_named_property(env, instance, "setComment", &testFunc);
    napi_call_function(env, instance, testFunc, 1, &val, &funcResultValue);
    ASSERT_NE(funcResultValue, nullptr);

    name = "root SYSTEM";
    napi_create_string_utf8(env, name.c_str(), name.size(), &val);
    napi_get_named_property(env, instance, "setCDATA", &testFunc);
    napi_call_function(env, instance, testFunc, 1, &val, &funcResultValue);
    ASSERT_NE(funcResultValue, nullptr);

    name = "Happy";
    napi_create_string_utf8(env, name.c_str(), name.size(), &val);
    napi_get_named_property(env, instance, "setText", &testFunc);
    napi_call_function(env, instance, testFunc, 1, &val, &funcResultValue);
    ASSERT_NE(funcResultValue, nullptr);

    name = "root SYSTEM \"http://www.test.org/test.dtd\"";
    napi_create_string_utf8(env, name.c_str(), name.size(), &val);
    napi_get_named_property(env, instance, "setDocType", &testFunc);
    napi_call_function(env, instance, testFunc, 1, &val, &funcResultValue);
    ASSERT_NE(funcResultValue, nullptr);

    napi_get_named_property(env, instance, "XmlSerializerError", &testFunc);
    napi_call_function(env, instance, testFunc, 0, nullptr, &funcResultValue);
    ASSERT_NE(funcResultValue, nullptr);
}

/* @tc.name: XmlPullParsertest001
 * @tc.desc: To XML text to JavaScript object.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, XmlPullParsertest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value exports = nullptr;
    napi_create_object(env, &exports);
    OHOS::xml::XmlPullParserInit(env, exports);
    napi_value xmlPullParserClass = nullptr;
    napi_get_named_property(env, exports, "XmlPullParser", &xmlPullParserClass);

    std::string firStr = "<?xml version=\"1.0\" encoding=\"utf-8\"?><note importance=\"high\" logged=\"true\">";
    std::string secStr = " <title>Happy</title><todo>Work</todo><todo>Play</todo></note>";
    std::string strXml = firStr + secStr;
    napi_value args[2]; // 2: number of arguments
    void* pBuffer = nullptr;
    size_t strLen = strXml.size();
    napi_create_arraybuffer(env, strLen, &pBuffer, &args[0]);
    memcpy_s(pBuffer, strLen, strXml.c_str(), strLen);
    std::string encoding = "utf-8";
    napi_create_string_utf8(env, encoding.c_str(), encoding.size(), &args[1]);
    napi_value instance = nullptr;
    napi_new_instance(env, xmlPullParserClass, 2, args, &instance); // 2: number of arguments

    napi_value obj = nullptr;
    napi_create_object(env, &obj);
    napi_value val;
    napi_get_boolean(env, true, &val);
    napi_set_named_property(env, obj, "supportDoctype", val);
    napi_set_named_property(env, obj, "ignoreNameSpace", val);
    std::string cbName = "Method";
    napi_create_function(env, cbName.c_str(), cbName.size(), Method, nullptr, &val);
    napi_set_named_property(env, obj, "tokenValueCallbackFunction", val);
    napi_value funcResultValue = nullptr;
    napi_value testFunc = nullptr;
    napi_get_named_property(env, instance, "parse", &testFunc);
    napi_call_function(env, instance, testFunc, 1, &obj, &funcResultValue);
    ASSERT_NE(funcResultValue, nullptr);

    napi_get_named_property(env, instance, "XmlPullParserError", &testFunc);
    napi_call_function(env, instance, testFunc, 0, nullptr, &funcResultValue);
    ASSERT_NE(funcResultValue, nullptr);
}

/* @tc.name: SetDeclaration
 * @tc.desc: Test SetDeclaration Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, Xmltest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;

    XmlTest::SetDeclaration(env);
    XmlTest::SetNamespace(env);
    XmlTest::StartElement(env);
    XmlTest::WriteEscaped(env);
    XmlTest::XmlSerializerError(env);

    std::string strXml = "<?xml version=\"1.0\" encoding=\"utf-8\"?>    <title>Happy</title>    <todo>Work</todo>";
    OHOS::xml::XmlPullParser xmlPullParser(strXml, "utf-8");
    XmlTest::PushSrcLinkList();
    std::string strTemp = "xml version";
    xmlPullParser.Replace(strTemp, "xml", "convert");
    ASSERT_STREQ(strTemp.c_str(), "convert version");
}

/* @tc.name: GetNSCount
 * @tc.desc: Test GetNSCount Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, GetNSCount001, testing::ext::TestSize.Level0)
{
    size_t res = XmlTest::GetNSCount(1);
    ASSERT_EQ(res, 0);

    std::string str = XmlTest::XmlPullParserError();
    ASSERT_STREQ(str.c_str(), "IndexOutOfBoundsException");
}

/* @tc.name: DealExclamationGroup
 * @tc.desc: Test DealExclamationGroup Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DealExclamationGroup001, testing::ext::TestSize.Level0)
{
    TagEnum tEnum = XmlTest::DealExclamationGroup("stER");
    ASSERT_EQ(tEnum, TagEnum::ERROR1);

    tEnum = XmlTest::DealExclamationGroup("stNR");
    ASSERT_EQ(tEnum, TagEnum::NOTATIONDECL);

    tEnum = XmlTest::DealExclamationGroup("staR");
    ASSERT_EQ(tEnum, TagEnum::ERROR1);
}

/* @tc.name: DealLtGroup
 * @tc.desc: Test DealLtGroup Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DealLtGroup001, testing::ext::TestSize.Level0)
{
    TagEnum tEnum = XmlTest::DealLtGroup();
    ASSERT_EQ(tEnum, TagEnum::END_TAG);

    tEnum = XmlTest::ParseTagType();
    ASSERT_EQ(tEnum, TagEnum::TEXT);
}

/* @tc.name: MakeStrUpper
 * @tc.desc: Test MakeStrUpper Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, MakeStrUpper001, testing::ext::TestSize.Level0)
{
    std::string strXml = "to";
    OHOS::xml::XmlPullParser xmlPullParser(strXml, "utf-8");
    std::string src = "C";
    xmlPullParser.MakeStrUpper(src);
    ASSERT_STREQ(src.c_str(), "c");

    std::string str = "todo";
    src = XmlTest::SkipText(strXml, str);
    ASSERT_STREQ(src.c_str(), "expected: 'todo' but was EOF");

    strXml = "<todo>Work</todo>";
    src = XmlTest::SkipText(strXml, str);
    ASSERT_STREQ(src.c_str(), "expected: \"todo\" but was \"<tod...\"");
}

/* @tc.name: GetColumnNumber
 * @tc.desc: Test GetColumnNumber Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, GetColumnNumber, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::xml::XmlTest testXml;
    int res = testXml.TestGetColumnNumber(env);
    ASSERT_EQ(res, 2); // 2: ColumnNumber
}

/* @tc.name: GetLineNumber
 * @tc.desc: Test GetLineNumber Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, GetLineNumber, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::xml::XmlTest testXml;
    int res = testXml.TestGetLineNumber(env);

    OHOS::xml::XmlPullParser xml("1\n1", "utf8");
    xml.ParseInneNotaDecl();
    ASSERT_EQ(res, 2); // 2: LineNumber
}

/* @tc.name: GetText
 * @tc.desc: Test GetText Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, GetText, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::xml::XmlTest testXml;
    std::string res = testXml.TestGetText(env);
    ASSERT_STREQ(res.c_str(), "");
}

/* @tc.name: ParseStartTagFuncDeal
 * @tc.desc: Test ParseStartTagFuncDeal Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseStartTagFuncDeal, testing::ext::TestSize.Level0)
{
    OHOS::xml::XmlPullParser xml("", "utf8");
    bool res = xml.ParseStartTagFuncDeal(true);
    ASSERT_FALSE(res);
}

/* @tc.name: ParseNsp
 * @tc.desc: Test ParseNsp Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseNsp, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::xml::XmlTest testXml;
    bool res = testXml.TestParseNsp(env);
    ASSERT_FALSE(res);
}

/* @tc.name: ParseDeclaration
 * @tc.desc: Test ParseDeclaration Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseDeclaration, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::xml::XmlTest testXml;
    testXml.TestParseDeclaration(env);
    bool res = false;
    ASSERT_FALSE(res);
}

/* @tc.name: ParseDelimiterInfo
 * @tc.desc: Test ParseDelimiterInfo Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseDelimiterInfo, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::xml::XmlTest testXml;
    std::string res = testXml.TestParseDelimiterInfo(env);
    ASSERT_STREQ(res.c_str(), "");
}

/* @tc.name: ParseEndTag
 * @tc.desc: Test ParseEndTag Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseEndTag, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::xml::XmlTest testXml;
    bool res = testXml.TestParseEndTag(env);
    ASSERT_FALSE(res);
}

/* @tc.name: ParserDoctInnerInfo
 * @tc.desc: Test ParserDoctInnerInfo Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParserDoctInnerInfo, testing::ext::TestSize.Level0)
{
    OHOS::xml::XmlPullParser xml("S11", "utf8");
    bool res = xml.ParserDoctInnerInfo(false, true);

    OHOS::xml::XmlPullParser xml1("P11", "utf8");
    res = xml1.ParserDoctInnerInfo(true, true);
    OHOS::xml::XmlPullParser xml2("P11", "utf8");
    res = xml2.ParserDoctInnerInfo(true, false);
    ASSERT_TRUE(res);
}

/* @tc.name: ParseDelimiter
 * @tc.desc: Test ParseDelimiter Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseDelimiter, testing::ext::TestSize.Level0)
{
    OHOS::xml::XmlPullParser xml("\"\'1", "utf8");
    std::string res = xml.ParseDelimiter(false);
    ASSERT_STREQ(res.c_str(), "");
}

/* @tc.name: ParseSpecText
 * @tc.desc: Test ParseSpecText Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseSpecText, testing::ext::TestSize.Level0)
{
    OHOS::xml::XmlPullParser xml("()*", "utf8");
    xml.ParseSpecText();
    OHOS::xml::XmlPullParser xml1("E", "utf8");
    xml1.ParseSpecText();
    OHOS::xml::XmlPullParser xml2("A", "utf8");
    xml2.ParseSpecText();
    bool res = false;
    ASSERT_FALSE(res);
}

/* @tc.name: ParseComment
 * @tc.desc: Test ParseComment Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseComment, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::xml::XmlTest testXml;
    bool res = testXml.TestParseComment(env);
    ASSERT_FALSE(res);
}

/* @tc.name: ParseOneTagFunc
 * @tc.desc: Test ParseOneTagFunc Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseOneTagFunc, testing::ext::TestSize.Level0)
{
    OHOS::xml::XmlTest testXml;
    OHOS::xml::TagEnum res = testXml.TestParseOneTagFunc();
    ASSERT_EQ(res, OHOS::xml::TagEnum::ERROR1);
}

/* @tc.name: ParseEntityDecl
 * @tc.desc: Test ParseEntityDecl Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TestParseEntityDecl, testing::ext::TestSize.Level0)
{
    OHOS::xml::XmlTest testXml;
    testXml.TestParseEntityDecl();
    bool res = false;
    ASSERT_FALSE(res);
}

/* @tc.name: ParseNameInner
 * @tc.desc: Test ParseNameInner Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseNameInner, testing::ext::TestSize.Level0)
{
    std::string res = XmlTest::ParseNameInner(1);
    ASSERT_STREQ(res.c_str(), "version");
}

/* @tc.name: ParseName
 * @tc.desc: Test ParseName Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseName, testing::ext::TestSize.Level0)
{
    std::string res = XmlTest::ParseName();
    ASSERT_STREQ(res.c_str(), "");
}

/* @tc.name: ParseEntityFunc
 * @tc.desc: Test ParseEntityFunc Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseEntityFunc, testing::ext::TestSize.Level0)
{
    OHOS::xml::XmlPullParser xmlPullParser("xml", "utf8");
    std::string out = "W#x2";
    XmlTest::ParseEntityFunc(out, "", true, TextEnum::ENTITY_DECL);

    out = "W#1";
    XmlTest::ParseEntityFunc(out, "", true, TextEnum::ENTITY_DECL);

    out = "Work1";
    XmlTest::ParseEntityFunc(out, "", true, TextEnum::ENTITY_DECL);

    out = "Work";
    XmlTest::ParseEntityFunc(out, "", true, TextEnum::TEXT);
    XmlTest::ParseEntityFunc(out, "", false, TextEnum::TEXT);

    out = "W";
    XmlTest::ParseEntityFunc(out, "", true, TextEnum::TEXT);
    XmlTest::ParseEntityFunc(out, "info", true, TextEnum::TEXT);
}

/* @tc.name: ParseEntity
 * @tc.desc: Test ParseEntity Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseEntity, testing::ext::TestSize.Level0)
{
    std::string res = XmlTest::ParseEntity();
    ASSERT_STREQ(res.c_str(), "Should not be reached");
}

/* @tc.name: ParseTagValueInner
 * @tc.desc: Test ParseTagValueInner Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseTagValueInner, testing::ext::TestSize.Level0)
{
    size_t start = 0;
    std::string result = "xml";
    size_t position = 1;
    std::string xmlStr = "todo";
    size_t res = XmlTest::ParseTagValueInner(start, result, position, xmlStr);
    ASSERT_EQ(res, 2);

    start = 1;
    xmlStr = "t";
    res = XmlTest::ParseTagValueInner(start, result, position, xmlStr);
    ASSERT_EQ(res, 0);
}

/* @tc.name: ParseTagValue
 * @tc.desc: Test ParseTagValue Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseTagValue, testing::ext::TestSize.Level0)
{
    size_t max = 1;
    std::string strXml = "W";
    std::string res = XmlTest::ParseTagValue('c', false, TextEnum::TEXT, max);
    ASSERT_STREQ(res.c_str(), "xml");

    res = XmlTest::ParseTagValue('e', true, TextEnum::ATTRI, max);
    ASSERT_STREQ(res.c_str(), "");
}

/* @tc.name: GetNamespace
 * @tc.desc: Test GetNamespace Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, GetNamespace, testing::ext::TestSize.Level0)
{
    std::string prefix = "";
    size_t depth = 2;
    std::string res = XmlTest::GetNamespace(prefix, depth);
    ASSERT_STREQ(res.c_str(), "W");

    prefix = "fix";
    depth = 1;
    std::string res1 = XmlTest::GetNamespace(prefix, depth);
    ASSERT_STREQ(res1.c_str(), "");
}

/* @tc.name: ParseNspFunc
 * @tc.desc: Test ParseNspFunc Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseNspFunc, testing::ext::TestSize.Level0)
{
    std::string res = XmlTest::ParseNspFunc();
    ASSERT_STREQ(res.c_str(), "illegal empty namespace");
}

/* @tc.name: ParseNspFunction
 * @tc.desc: Test ParseNspFunction Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseNspFunction, testing::ext::TestSize.Level0)
{
    std::string pushStr = "yu:er";
    std::string res = XmlTest::ParseNspFunction(pushStr);
    ASSERT_STREQ(res.c_str(), "Undefined Prefix: yu in ");

    pushStr = ":yuer";
    res = XmlTest::ParseNspFunction(pushStr);
    ASSERT_STREQ(res.c_str(), "illegal attribute name: ");
}

/* @tc.name: ParseNsp002
 * @tc.desc: Test ParseNsp Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseNsp002, testing::ext::TestSize.Level0)
{
    bool res = XmlTest::ParseNsp();
    ASSERT_FALSE(res);
}

/* @tc.name: ParseStartTagFuncDeal002
 * @tc.desc: Test ParseStartTagFuncDeal Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseStartTagFuncDeal002, testing::ext::TestSize.Level0)
{
    bool res = XmlTest::ParseStartTagFuncDeal("w=", false);
    ASSERT_FALSE(res);

    res = XmlTest::ParseStartTagFuncDeal("=q", true);
    ASSERT_TRUE(res);

    res = XmlTest::ParseStartTagFuncDeal("==", false);
    ASSERT_TRUE(res);
}

/* @tc.name: ParseStartTagFunc
 * @tc.desc: Test ParseStartTagFunc Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseStartTagFunc, testing::ext::TestSize.Level0)
{
    OHOS::xml::XmlPullParser xmlPullParser("", "utf-8");
    bool res = xmlPullParser.ParseStartTagFunc(false, false);
    ASSERT_FALSE(res);
}

/* @tc.name: ParseDeclaration002
 * @tc.desc: Test ParseDeclaration Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseDeclaration002, testing::ext::TestSize.Level0)
{
    std::string xml = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><todo>Work</todo>";
    bool res = XmlTest::ParseDeclaration(xml);
    ASSERT_TRUE(res);

    xml = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?><todo>Work</todo>";
    res = XmlTest::ParseDeclaration(xml);
    ASSERT_TRUE(res);

    xml = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"else\"?><todo>Work</todo>";
    res = XmlTest::ParseDeclaration(xml);
    ASSERT_TRUE(res);

    xml = "<?xml version=\"1.0\" encoding=\"utf-8\" standa=\"no\"?><todo>Work</todo>";
    res = XmlTest::ParseDeclaration(xml);
    ASSERT_TRUE(res);
}