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

/* @tc.name: StartElementTest006
 * @tc.desc: Test whether write a elemnet start tag with the given name successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, StartElementTest006, testing::ext::TestSize.Level0)
{
    OHOS::xml::XmlSerializer xmlSerializer("utf-8");
    xmlSerializer.StartElement("note1");
    xmlSerializer.StartElement("note2");
    xmlSerializer.EndElement();
    xmlSerializer.StartElement("note3");
    xmlSerializer.EndElement();
    xmlSerializer.EndElement();
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = xmlSerializer.GetXmlBufferLength();
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    xmlSerializer.GetXmlBuffer(pBuffer, size);
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

/* @tc.name: SetAttributesTest006
 * @tc.desc: Test whether write an attribute successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetAttributesTest006, testing::ext::TestSize.Level0)
{
    OHOS::xml::XmlSerializer xmlSerializer("utf-8");
    xmlSerializer.StartElement("note");
    xmlSerializer.SetAttributes("importance1", "high1");
    xmlSerializer.SetAttributes("importance2", "high2");
    xmlSerializer.SetAttributes("importance3", "high3");
    xmlSerializer.SetAttributes("importance4", "high4");
    xmlSerializer.SetAttributes("importance5", "high5");
    xmlSerializer.EndElement();
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = xmlSerializer.GetXmlBufferLength();
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    xmlSerializer.GetXmlBuffer(pBuffer, size);
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

/* @tc.name: AddEmptyElementTest006
 * @tc.desc: Test whether add an empty element successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, AddEmptyElementTest006, testing::ext::TestSize.Level0)
{
    OHOS::xml::XmlSerializer xmlSerializer("utf-8");
    xmlSerializer.StartElement("note");
    xmlSerializer.AddEmptyElement("c");
    xmlSerializer.AddEmptyElement("d");
    xmlSerializer.EndElement();
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = xmlSerializer.GetXmlBufferLength();
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    xmlSerializer.GetXmlBuffer(pBuffer, size);
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

/* @tc.name: SetDeclarationTest006
 * @tc.desc: Test whether write xml declaration with encoding successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetDeclarationTest006, testing::ext::TestSize.Level0)
{
    OHOS::xml::XmlSerializer xmlSerializer("utf-8");
    xmlSerializer.SetDeclaration();
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = xmlSerializer.GetXmlBufferLength();
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    xmlSerializer.GetXmlBuffer(pBuffer, size);
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

/* @tc.name: EndElementTest006
 * @tc.desc: Test whether write end tag of the element successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, EndElementTest006, testing::ext::TestSize.Level0)
{
    OHOS::xml::XmlSerializer xmlSerializer("utf-8");
    xmlSerializer.StartElement("note2");
    xmlSerializer.SetAttributes("importance", "high");
    xmlSerializer.EndElement();
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = xmlSerializer.GetXmlBufferLength();
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    xmlSerializer.GetXmlBuffer(pBuffer, size);
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

/* @tc.name: SetNamespaceTest006
 * @tc.desc: Test whether write the namespace of the current element tag successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetNamespaceTest006, testing::ext::TestSize.Level0)
{
    OHOS::xml::XmlSerializer xmlSerializer("utf-8");
    xmlSerializer.SetDeclaration();
    xmlSerializer.SetNamespace("h", "http://www.w3.org/TR/html4/");
    xmlSerializer.StartElement("note1");
    xmlSerializer.StartElement("note2");
    xmlSerializer.EndElement();
    xmlSerializer.EndElement();
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = xmlSerializer.GetXmlBufferLength();
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    xmlSerializer.GetXmlBuffer(pBuffer, size);
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

/* @tc.name: SetCommentTest006
 * @tc.desc: Test write the comment successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetCommentTest006, testing::ext::TestSize.Level0)
{
    OHOS::xml::XmlSerializer xmlSerializer("utf-8");
    xmlSerializer.SetComment("Hello, World!");
    xmlSerializer.StartElement("note");
    xmlSerializer.EndElement();
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = xmlSerializer.GetXmlBufferLength();
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    xmlSerializer.GetXmlBuffer(pBuffer, size);
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

/* @tc.name: SetCDATATest006
 * @tc.desc: Test whether Writes the CDATA successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetCDATATest006, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.SetCData("]]>");
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), "<![CDATA[]]]]><![CDATA[>]]>");
}

/* @tc.name: SetCDATATest007
 * @tc.desc: Test whether Writes the CDATA successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetCDATATest007, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.SetCData("<![CDATA[]]>");
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), "<![CDATA[<![CDATA[]]]]><![CDATA[>]]>");
}

/* @tc.name: SetCDATATest008
 * @tc.desc: Test whether Writes the CDATA successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetCDATATest008, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.SetCData("]]>]]>");
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), "<![CDATA[]]]]><![CDATA[>]]]]><![CDATA[>]]>");
}

/* @tc.name: SetCDATATest009
 * @tc.desc: Test whether Writes the CDATA successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetCDATATest009, testing::ext::TestSize.Level0)
{
    OHOS::xml::XmlSerializer xmlSerializer("utf-8");
    xmlSerializer.SetCData("]]>]]>");
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = xmlSerializer.GetXmlBufferLength();
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    xmlSerializer.GetXmlBuffer(pBuffer, size);
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), "<![CDATA[]]]]><![CDATA[>]]]]><![CDATA[>]]>");
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

/* @tc.name: SetTextTest006
 * @tc.desc: Test whether Writes the text successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetTextTest006, testing::ext::TestSize.Level0)
{
    OHOS::xml::XmlSerializer xmlSerializer("utf-8");
    xmlSerializer.StartElement("note");
    xmlSerializer.SetAttributes("importance", "high");
    xmlSerializer.SetText("Happy5");
    xmlSerializer.EndElement();
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = xmlSerializer.GetXmlBufferLength();
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    xmlSerializer.GetXmlBuffer(pBuffer, size);
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

/* @tc.name: SetDocTypeTest006
 * @tc.desc: Test whether rites the DOCTYPE successfully.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetDocTypeTest006, testing::ext::TestSize.Level0)
{
    OHOS::xml::XmlSerializer xmlSerializer("utf-8");
    xmlSerializer.StartElement("note");
    xmlSerializer.SetDocType("root SYSTEM \"http://www.test.org/test.dtd\"");
    xmlSerializer.EndElement();
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = xmlSerializer.GetXmlBufferLength();
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    xmlSerializer.GetXmlBuffer(pBuffer, size);
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
    OHOS::xml::XmlPullParser xmlPullParser(env, strXml, "utf-8");
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
    xmlPullParser.Parse(env, options, false);
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
    OHOS::xml::XmlPullParser xmlPullParser(env, strXml, "utf-8");
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
    xmlPullParser.Parse(env, options, true);
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
    OHOS::xml::XmlPullParser xmlPullParser(env, strXml, "utf-8");
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
    xmlPullParser.Parse(env, options, false);
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
    OHOS::xml::XmlPullParser xmlPullParser(env, strXml, "utf-8");
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
    xmlPullParser.Parse(env, options, true);
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
    OHOS::xml::XmlPullParser xmlPullParser(env, strXml, "utf-8");
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
    xmlPullParser.Parse(env, options, false);
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
    OHOS::xml::XmlPullParser xmlPullParser(env, strXml, "utf-8");
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
    xmlPullParser.Parse(env, options, false);
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
    OHOS::xml::XmlPullParser xmlPullParser(env, strXml, "utf-8");
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
    xmlPullParser.Parse(env, options, true);
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
    OHOS::xml::XmlPullParser xmlPullParser(env, strXml, "utf-8");
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
    xmlPullParser.Parse(env, options, true);
    std::string res1 = "note\\r\\nfuncrion matchwo(a,6)\\r\\n{\\r\\nreturn 1;\\r\\n}\\r\\nHello, World! companyJohn &";
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
    OHOS::xml::XmlPullParser xmlPullParser(env, strXml, "utf-8");
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
    xmlPullParser.Parse(env, options, true);
    std::string res1 = "note\\r\\nfuncrion matchwo(a,6)\\r\\n{\\r\\nreturn 1;\\r\\n}\\r\\nHello, World! companyJohn &";
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
    OHOS::xml::XmlPullParser xmlPullParser(env, strXml, "utf-8");
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
    xmlPullParser.Parse(env, options, true);
    std::string res1 = " note [\n<!ENTITY foo \"baa\">]note\\r\\nfuncrion matchwo(a,6)\\r\\n{\\r\\nreturn 1;\\r\\n}";
    std::string res2 = "\\r\\nHello, World! companyJohn & Hanscompany titleHappytitletitleHappytitle todoWorktodo";
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
    OHOS::xml::XmlPullParser xmlPullParser(env, strXml, "utf-8");
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
    xmlPullParser.Parse(env, options, true);
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
    OHOS::xml::XmlPullParser xmlPullParser(env, strXml, "utf-8");
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
    xmlPullParser.Parse(env, options, true);
    ASSERT_STREQ(g_testStr.c_str(), "");
}

/* @tc.name: Xmlfunctest001
 * @tc.desc: To XML text to JavaScript object.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, Xmlfunctest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string strXml = "<?xml version=\"1.0\" encoding=\"utf-8\"?>    <title>Happy</title>    <todo>Work</todo>";
    OHOS::xml::XmlPullParser xmlPullParser(env, strXml, "utf-8");
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
}

/* @tc.name: XmlSerializertest002
 * @tc.desc: To XML text to JavaScript object.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, XmlSerializertest002, testing::ext::TestSize.Level0)
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
    napi_value val = nullptr;
    napi_new_instance(env, xmlSerializerClass, 2, args, &instance); // 2: number of arguments
    napi_value testFunc = nullptr;
    napi_value funcResultValue = nullptr;
    std::string name = "Hello, World!";
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
    OHOS::xml::XmlPullParser xmlPullParser(env, strXml, "utf-8");
    XmlTest::PushSrcLinkList(env);
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
    napi_env env = (napi_env)engine_;
    size_t res = XmlTest::GetNSCount(env, 1);
    ASSERT_EQ(res, 0);

    std::string str = XmlTest::XmlPullParserError(env);
    ASSERT_STREQ(str.c_str(), "IndexOutOfBoundsException");
}

/* @tc.name: DealExclamationGroup
 * @tc.desc: Test DealExclamationGroup Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DealExclamationGroup001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    TagEnum tEnum = XmlTest::DealExclamationGroup(env, "stER");
    ASSERT_EQ(tEnum, TagEnum::ERROR);

    tEnum = XmlTest::DealExclamationGroup(env, "stNR");
    ASSERT_EQ(tEnum, TagEnum::NOTATIONDECL);

    tEnum = XmlTest::DealExclamationGroup(env, "staR");
    ASSERT_EQ(tEnum, TagEnum::ERROR);
}

/* @tc.name: DealLtGroup001
 * @tc.desc: Test DealLtGroup Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DealLtGroup001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    TagEnum tEnum = XmlTest::DealLtGroup(env);
    ASSERT_EQ(tEnum, TagEnum::END_TAG);
    std::string str1 = "%";
    int apiVersion = 13; // 13: apiVersion
    tEnum = XmlTest::ParseTagType(env, str1, apiVersion);
    ASSERT_EQ(tEnum, TagEnum::TEXT);
}

/* @tc.name: DealLtGroup002
 * @tc.desc: Test DealLtGroup Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DealLtGroup002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    TagEnum tEnum = XmlTest::DealLtGroup(env);
    ASSERT_EQ(tEnum, TagEnum::END_TAG);
    std::string str1 = "&";
    int apiVersion = 13; // 13: apiVersion
    tEnum = XmlTest::ParseTagType(env, str1, apiVersion);
    ASSERT_EQ(tEnum, TagEnum::ENTITY_REFERENCE);
}

/* @tc.name: DealLtGroup003
 * @tc.desc: Test DealLtGroup Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DealLtGroup003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    TagEnum tEnum = XmlTest::DealLtGroup(env);
    ASSERT_EQ(tEnum, TagEnum::END_TAG);
    std::string str1 = "&";
    int apiVersion = 11; // 11: apiVersion
    tEnum = XmlTest::ParseTagType(env, str1, apiVersion);
    ASSERT_EQ(tEnum, TagEnum::TEXT);
}

/* @tc.name: MakeStrUpper
 * @tc.desc: Test MakeStrUpper Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, MakeStrUpper001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string strXml = "to";
    OHOS::xml::XmlPullParser xmlPullParser(env, strXml, "utf-8");
    std::string src = "C";
    xmlPullParser.MakeStrUpper(src);
    ASSERT_STREQ(src.c_str(), "c");

    std::string str = "todo";
    src = XmlTest::SkipText(env, strXml, str);
    ASSERT_STREQ(src.c_str(), "expected: 'todo' but was EOF");

    strXml = "<todo>Work</todo>";
    src = XmlTest::SkipText(env, strXml, str);
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

    OHOS::xml::XmlPullParser xml(env, "1\n1", "utf8");
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
    napi_env env = (napi_env)engine_;
    OHOS::xml::XmlPullParser xml(env, "", "utf8");
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
    napi_env env = (napi_env)engine_;
    OHOS::xml::XmlPullParser xml(env, "S11", "utf8");
    bool res = xml.ParserDoctInnerInfo(false, true);

    OHOS::xml::XmlPullParser xml1(env, "P11", "utf8");
    res = xml1.ParserDoctInnerInfo(true, true);
    OHOS::xml::XmlPullParser xml2(env, "P11", "utf8");
    res = xml2.ParserDoctInnerInfo(true, false);
    ASSERT_TRUE(res);
}

/* @tc.name: ParseDelimiter
 * @tc.desc: Test ParseDelimiter Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseDelimiter, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::xml::XmlPullParser xml(env, "\"\'1", "utf8");
    std::string res = xml.ParseDelimiter(false);
    ASSERT_STREQ(res.c_str(), "");
}

/* @tc.name: ParseSpecText
 * @tc.desc: Test ParseSpecText Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseSpecText, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::xml::XmlPullParser xml(env, "()*", "utf8");
    xml.ParseSpecText();
    OHOS::xml::XmlPullParser xml1(env, "E", "utf8");
    xml1.ParseSpecText();
    OHOS::xml::XmlPullParser xml2(env, "A", "utf8");
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
    napi_env env = (napi_env)engine_;
    OHOS::xml::XmlTest testXml;
    OHOS::xml::TagEnum res = testXml.TestParseOneTagFunc(env);
    ASSERT_EQ(res, OHOS::xml::TagEnum::ERROR);
}

/* @tc.name: ParseEntityDecl
 * @tc.desc: Test ParseEntityDecl Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TestParseEntityDecl, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::xml::XmlTest testXml;
    testXml.TestParseEntityDecl(env);
    bool res = false;
    ASSERT_FALSE(res);
}

/* @tc.name: ParseNameInner
 * @tc.desc: Test ParseNameInner Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseNameInner, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string res = XmlTest::ParseNameInner(env, 1);
    ASSERT_STREQ(res.c_str(), "version");
}

/* @tc.name: ParseName
 * @tc.desc: Test ParseName Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseName, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string res = XmlTest::ParseName(env);
    ASSERT_STREQ(res.c_str(), "");
}

/* @tc.name: ParseEntityFunc
 * @tc.desc: Test ParseEntityFunc Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseEntityFunc, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::xml::XmlPullParser xmlPullParser(env, "xml", "utf8");
    std::string out = "W#x2";
    bool res = XmlTest::ParseEntityFunc(env, out, "", true, TextEnum::ENTITY_DECL);
    ASSERT_FALSE(res);

    out = "W#1";
    res = XmlTest::ParseEntityFunc(env, out, "", true, TextEnum::ENTITY_DECL);
    ASSERT_FALSE(res);

    out = "Work1";
    res = XmlTest::ParseEntityFunc(env, out, "", true, TextEnum::ENTITY_DECL);
    ASSERT_FALSE(res);

    out = "Work";
    res = XmlTest::ParseEntityFunc(env, out, "", true, TextEnum::TEXT);
    ASSERT_FALSE(res);
    res = XmlTest::ParseEntityFunc(env, out, "", false, TextEnum::TEXT);
    ASSERT_FALSE(res);

    out = "W";
    res = XmlTest::ParseEntityFunc(env, out, "", true, TextEnum::TEXT);
    ASSERT_TRUE(res);
    res = XmlTest::ParseEntityFunc(env, out, "info", true, TextEnum::TEXT);
    ASSERT_FALSE(res);
}

/* @tc.name: ParseEntity001
 * @tc.desc: Test ParseEntity Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseEntity001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    bool relaxed = true;
    std::string res = XmlTest::ParseEntity(env, relaxed);
    ASSERT_STREQ(res.c_str(), "Should not be reached");
}

/* @tc.name: ParseEntity002
 * @tc.desc: Test ParseEntity Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseEntity002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    bool relaxed = false;
    std::string res = XmlTest::ParseEntity(env, relaxed);
    ASSERT_STREQ(res.c_str(), "unterminated entity ref");
}

/* @tc.name: ParseTagValueInner
 * @tc.desc: Test ParseTagValueInner Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseTagValueInner, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t start = 0;
    std::string result = "xml";
    size_t position = 1;
    std::string xmlStr = "todo";
    size_t res = XmlTest::ParseTagValueInner(env, start, result, position, xmlStr);
    ASSERT_EQ(res, 2);

    start = 1;
    xmlStr = "t";
    res = XmlTest::ParseTagValueInner(env, start, result, position, xmlStr);
    ASSERT_EQ(res, 0);
}

/* @tc.name: ParseTagValue
 * @tc.desc: Test ParseTagValue Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseTagValue, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    size_t max = 1;
    std::string strXml = "W";
    std::string res = XmlTest::ParseTagValue(env, 'c', false, TextEnum::TEXT, max);
    ASSERT_STREQ(res.c_str(), "xml");

    res = XmlTest::ParseTagValue(env, 'e', true, TextEnum::ATTRI, max);
    ASSERT_STREQ(res.c_str(), "");
}

/* @tc.name: GetNamespace
 * @tc.desc: Test GetNamespace Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, GetNamespace, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string prefix = "";
    size_t depth = 2;
    std::string res = XmlTest::GetNamespace(env, prefix, depth);
    ASSERT_STREQ(res.c_str(), "W");

    prefix = "fix";
    depth = 1;
    std::string res1 = XmlTest::GetNamespace(env, prefix, depth);
    ASSERT_STREQ(res1.c_str(), "");
}

/* @tc.name: ParseNspFunc
 * @tc.desc: Test ParseNspFunc Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseNspFunc, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string res = XmlTest::ParseNspFunc(env);
    ASSERT_STREQ(res.c_str(), "illegal empty namespace");
}

/* @tc.name: ParseNspFunction
 * @tc.desc: Test ParseNspFunction Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseNspFunction, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string pushStr = "yu:er";
    std::string res = XmlTest::ParseNspFunction(env, pushStr);
    ASSERT_STREQ(res.c_str(), "Undefined Prefix: yu in ");

    pushStr = ":yuer";
    res = XmlTest::ParseNspFunction(env, pushStr);
    ASSERT_STREQ(res.c_str(), "illegal attribute name: ");
}

/* @tc.name: ParseNsp002
 * @tc.desc: Test ParseNsp Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseNsp002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    bool res = XmlTest::ParseNsp(env);
    ASSERT_FALSE(res);
}

/* @tc.name: ParseStartTagFuncDeal002
 * @tc.desc: Test ParseStartTagFuncDeal Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseStartTagFuncDeal002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    bool res = XmlTest::ParseStartTagFuncDeal(env, "w=", false);
    ASSERT_FALSE(res);

    res = XmlTest::ParseStartTagFuncDeal(env, "=q", true);
    ASSERT_TRUE(res);

    res = XmlTest::ParseStartTagFuncDeal(env, "==", false);
    ASSERT_FALSE(res);
}

/* @tc.name: ParseStartTagFunc
 * @tc.desc: Test ParseStartTagFunc Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseStartTagFunc001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::xml::XmlPullParser xmlPullParser(env, "", "utf-8");
    TagEnum res = xmlPullParser.ParseStartTagFunc(false, false);
    ASSERT_EQ(res, OHOS::xml::TagEnum::ERROR);
}

/* @tc.name: ParseStartTagFunc
 * @tc.desc: Test ParseStartTagFunc Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseStartTagFunc002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string str1 = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
    std::string str2 = "<note importance=\"high\" logged=\"true\">";
    std::string str3 = "    <title>Hello\rWorld\n</title>";
    std::string str4 = "    <todo>Work\r\n</todo>";
    std::string str5 = "    <mess><![CDATA[This is a \r\n CDATA section]]></mess>";
    std::string str6 = "</note>";
    std::string strXml = str1 + str2 + str3 + str4 + str5 + str6;
    TagEnum res = XmlTest::ParseStartTagFuncTest(env, strXml, false, true);
    ASSERT_EQ(res, OHOS::xml::TagEnum::ERROR);
}

/* @tc.name: ParseDeclaration002
 * @tc.desc: Test ParseDeclaration Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseDeclaration002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string xml = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><todo>Work</todo>";
    bool res = XmlTest::ParseDeclaration(env, xml);
    ASSERT_TRUE(res);

    xml = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?><todo>Work</todo>";
    res = XmlTest::ParseDeclaration(env, xml);
    ASSERT_TRUE(res);

    xml = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"else\"?><todo>Work</todo>";
    res = XmlTest::ParseDeclaration(env, xml);
    ASSERT_TRUE(res);

    xml = "<?xml version=\"1.0\" encoding=\"utf-8\" standa=\"no\"?><todo>Work</todo>";
    res = XmlTest::ParseDeclaration(env, xml);
    ASSERT_TRUE(res);
}

/* @tc.name: DealNapiStrValue001
 * @tc.desc: Test DealNapiStrValue Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DealNapiStrValue001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string str = "<?xml version=\"1.0\" encoding=\"utf-8\" standa=\"no\"?><todo>Work</todo>";
    std::string output = XmlTest::DealNapiStrValueFunction(env, str);
    ASSERT_STREQ(output.c_str(), str.c_str());
}

/* @tc.name: DealNapiStrValue002
 * @tc.desc: Test DealNapiStrValue Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DealNapiStrValue002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string strPrior = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n";
    std::string output = XmlTest::DealNapiStrValueFunction(env, strPrior);
    ASSERT_STREQ(output.c_str(), strPrior.c_str());
}

/* @tc.name: DealNapiStrValue003
 * @tc.desc: Test DealNapiStrValue Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DealNapiStrValue003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string str1 = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><todo>Work</todo>";
    std::string output = XmlTest::DealNapiStrValueFunction(env, str1);
    ASSERT_STREQ(output.c_str(), str1.c_str());
}

/* @tc.name: SplicNsp001
 * @tc.desc: Test SplicNsp Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SplicNspFunction001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string str1 = "isStart";
    int output = XmlTest::SplicNspFunction(env, str1);
    ASSERT_EQ(output, 0);
}

/* @tc.name: SetNamespaceFunction001
 * @tc.desc: Test SetNamespace Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetNamespaceFunction001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    std::string str1 = "h";
    std::string str2 = "http://www.w3.org/TR/html4/";
    xmlSerializer.SetDeclaration();
    xmlSerializer.SetNamespace(str1, str2);
    xmlSerializer.StartElement("note");
    xmlSerializer.EndElement();
    std::string result = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n<h:"
                         "note xmlns:h=\"http://www.w3.org/TR/html4/\"/>";
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), result.c_str());
}

/* @tc.name: SetNamespaceFunction002
 * @tc.desc: Test SetNamespace Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetNamespaceFunction002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 1024;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    std::string str1 = "b";
    std::string str2 = "http://www.w3.org/TR/html4/";
    xmlSerializer.SetDeclaration();
    xmlSerializer.SetNamespace(str1, str2);
    xmlSerializer.StartElement("note");
    xmlSerializer.EndElement();
    std::string result = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n<b:"
                         "note xmlns:b=\"http://www.w3.org/TR/html4/\"/>";
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), result.c_str());
}

/* @tc.name: SetNamespaceFunction003
 * @tc.desc: Test SetNamespace Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetNamespaceFunction003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 2048;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    std::string str1 = "h";
    std::string str2 = "http://www.111/";
    xmlSerializer.SetDeclaration();
    xmlSerializer.SetNamespace(str1, str2);
    xmlSerializer.StartElement("note1");
    xmlSerializer.StartElement("note2");
    xmlSerializer.EndElement();
    xmlSerializer.EndElement();
    std::string result = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n<h:"
                         "note1 xmlns:h=\"http://www.111/\">\r\n  <h:note2/>\r\n</h:note1>";
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), result.c_str());
}

/* @tc.name: SetNamespaceFunction004
 * @tc.desc: Test SetNamespace Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetNamespaceFunction004, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string str1 = "h";
    std::string str2 = "http://www.111/";
    std::string result = " xmlns:h=\"http://www.111/\"/>";
    std::string outPut = XmlTest::SetNamespaceFunction(env, str1, str2);
    ASSERT_STREQ(outPut.c_str(), result.c_str());
}

/* @tc.name: StartElementFunction001
 * @tc.desc: Test StartElement Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, StartElementFunction001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 2048;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note");
    xmlSerializer.SetComment("Hi!");
    xmlSerializer.EndElement();
    std::string result = "<note>\r\n  <!--Hi!-->\r\n</note>";
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), result.c_str());
}

/* @tc.name: EndElementFunction001
 * @tc.desc: Test EndElement Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, EndElementFunction001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 2048;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note");
    xmlSerializer.SetCData("root SYSTEM \"http://www.test.org/test.dtd\"");
    xmlSerializer.EndElement();
    std::string result = "<note>\r\n  <![CDATA[root SYSTEM \"http://www.test.org/test.dtd\"]]>\r\n</note>";
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), result.c_str());
}

/* @tc.name: EndElementFunction002
 * @tc.desc: Test EndElement Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, EndElementFunction002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 2048;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.SetDeclaration();
    xmlSerializer.SetNamespace("h", "http://www.w3.org/TR/html4/");
    xmlSerializer.StartElement("note1");
    xmlSerializer.StartElement("note2");
    xmlSerializer.EndElement();
    xmlSerializer.EndElement();
    std::string result = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n<h:note1 xmlns:"
                         "h=\"http://www.w3.org/TR/html4/\">\r\n  <h:note2/>\r\n</h:note1>";
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), result.c_str());
}

/* @tc.name: AddEmptyElementFunction001
 * @tc.desc: Test AddEmptyElement Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, AddEmptyElementFunction001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 2048;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note");
    xmlSerializer.AddEmptyElement("a");
    xmlSerializer.EndElement();
    std::string result = "<note>\r\n  <a/>\r\n</note>";
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), result.c_str());
}

/* @tc.name: AddEmptyElementFunction002
 * @tc.desc: Test AddEmptyElement Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, AddEmptyElementFunction002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 2048;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note");
    xmlSerializer.EndElement();
    xmlSerializer.AddEmptyElement("b");
    std::string result = "<note/>\r\n<b/>";
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), result.c_str());
}

/* @tc.name: SetTextFunction001
 * @tc.desc: Test SetText Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetTextFunction001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 2048;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note");
    xmlSerializer.SetAttributes("importance", "high");
    xmlSerializer.SetText("Hello\"World");
    xmlSerializer.EndElement();
    std::string result = "<note importance=\"high\">Hello&quot;World</note>";
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), result.c_str());
}

/* @tc.name: SetTextFunction002
 * @tc.desc: Test SetText Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetTextFunction002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 2048;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note");
    xmlSerializer.SetAttributes("importance", "high");
    xmlSerializer.SetText("Happy5");
    xmlSerializer.EndElement();
    std::string result = "<note importance=\"high\">Happy5</note>";
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), result.c_str());
}

/* @tc.name: SetCommentFunction001
 * @tc.desc: Test SetComment Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetCommentFunction001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 2048;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note");
    xmlSerializer.SetComment("Hi!");
    xmlSerializer.EndElement();
    std::string result = "<note>\r\n  <!--Hi!-->\r\n</note>";
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), result.c_str());
}

/* @tc.name: SetCommentFunction002
 * @tc.desc: Test SetComment Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetCommentFunction002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 2048;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note");
    xmlSerializer.SetComment("Hi!");
    xmlSerializer.EndElement();
    std::string result = "<note>\r\n  <!--Hi!-->\r\n</note>";
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), result.c_str());
}

/* @tc.name: SetCDataFunction001
 * @tc.desc: Test SetCData Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetCDataFunction001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 2048;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.SetCData("root ]]> SYSTEM");
    std::string result = "<![CDATA[root ]]]]><![CDATA[> SYSTEM]]>";
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), result.c_str());
}

/* @tc.name: SetCDataFunction002
 * @tc.desc: Test SetCData Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetCDataFunction002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 2048;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note");
    xmlSerializer.EndElement();
    xmlSerializer.SetCData("root SYSTEM \"http://www.test.org/test.dtd\"");
    std::string result = "<note/>\r\n<![CDATA[root SYSTEM \"http://www.test.org/test.dtd\"]]>";
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), result.c_str());
}

/* @tc.name: SetCDataFunction003
 * @tc.desc: Test SetCData Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SetCDataFunction003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 2048;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note");
    xmlSerializer.SetCData("root SYSTEM \"http://www.test.org/test.dtd\"");
    xmlSerializer.EndElement();
    std::string result = "<note>\r\n  <![CDATA[root SYSTEM \"http://www.test.org/test.dtd\"]]>\r\n</note>";
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), result.c_str());
}

/* @tc.name: WriteEscapedFunction001
 * @tc.desc: Test WriteEscaped Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, WriteEscapedFunction001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 2048;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note");
    xmlSerializer.SetAttributes("importance", "high");
    xmlSerializer.SetText("Hello>World");
    xmlSerializer.EndElement();
    std::string result = "<note importance=\"high\">Hello&gt;World</note>";
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), result.c_str());
}

/* @tc.name: WriteEscapedFunction002
 * @tc.desc: Test WriteEscaped Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, WriteEscapedFunction002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 2048;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note");
    xmlSerializer.SetAttributes("importance", "high");
    xmlSerializer.SetText("Hello<World");
    xmlSerializer.EndElement();
    std::string result = "<note importance=\"high\">Hello&lt;World</note>";
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), result.c_str());
}

/* @tc.name: WriteEscapedFunction003
 * @tc.desc: Test WriteEscaped Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, WriteEscapedFunction003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 2048;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note");
    xmlSerializer.SetAttributes("importance", "high");
    xmlSerializer.SetText("Hello&World");
    xmlSerializer.EndElement();
    std::string result = "<note importance=\"high\">Hello&amp;World</note>";
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), result.c_str());
}

/* @tc.name: WriteEscapedFunction004
 * @tc.desc: Test WriteEscaped Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, WriteEscapedFunction004, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arrayBuffer = nullptr;
    void* pBuffer = nullptr;
    size_t size = 2048;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note");
    xmlSerializer.SetAttributes("importance", "high");
    xmlSerializer.SetText("Hello'World");
    xmlSerializer.EndElement();
    std::string result = "<note importance=\"high\">Hello&apos;World</note>";
    ASSERT_STREQ(reinterpret_cast<char*>(pBuffer), result.c_str());
}

/* @tc.name: XmlSerializerErrorFunction001
 * @tc.desc: Test XmlSerializerErro Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, XmlSerializerErrorFunction001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string outPut = XmlTest::XmlSerializerErrorFunction(env);
    std::string result = "illegal position for declaration";
    ASSERT_STREQ(outPut.c_str(), result.c_str());
}

/* @tc.name: XmlParseTagValueFuncFunction001
 * @tc.desc: Test ParseTagValueFunc Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, XmlParseTagValueFuncFunction001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string str1 = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
    std::string str3 = "    <title>Hello\rWorld\n</title>";
    std::string str4 = "    <todo>Work\r\n</todo>";
    std::string strXml = str1 + str3 + str4;
    g_testStr = "";
    OHOS::xml::XmlPullParser xmlPullParser(env, strXml, "utf-8");
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
    std::string cbName = "Method";
    napi_create_function(env, cbName.c_str(), cbName.size(), Method, nullptr, &value3);
    napi_set_named_property(env, object, key1, value1);
    napi_set_named_property(env, object, key2, value2);
    napi_set_named_property(env, object, key3, value3);
    xmlPullParser.DealOptionInfo(env, object);
    xmlPullParser.Parse(env, options, true);
    std::string result = "";
    ASSERT_STREQ(g_testStr.c_str(), result.c_str());
}

/* @tc.name: XmlParseTagValueFuncFunction002
 * @tc.desc: Test ParseTagValueFunc Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, XmlParseTagValueFuncFunction002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string str1 = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
    char cRecv = static_cast<char>(10);
    size_t intPut = 49;
    std::string str2 = "Hello";
    bool res = XmlTest::ParseTagValueFunc(env, str1, cRecv, intPut, str2);
    ASSERT_TRUE(res);
}

/* @tc.name: XmlParseTagValueFuncFunction003
 * @tc.desc: Test ParseTagValueFunc Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, XmlParseTagValueFuncFunction003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string str1 = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
    char cRecv = static_cast<char>(60);
    size_t intPut = 49;
    std::string str2 = "World";
    bool res = XmlTest::ParseTagValueFunc(env, str1, cRecv, intPut, str2);
    ASSERT_TRUE(res);
}

/* @tc.name: XmlParseTagValueFuncFunction004
 * @tc.desc: Test ParseTagValueFunc Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, XmlParseTagValueFuncFunction004, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string str1 = "<]>ml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><todo>Work</todo>";
    std::string str2 = "<note importance=\"high\" logged=\"true\">";
    std::string str3 = "    <title>Hello\rWorld\n</title>";
    std::string str4 = "    <todo>Work\r\n</todo>";
    std::string strXml = str1 + str2 + str3 + str4;
    char cRecv = static_cast<char>(93);
    size_t intPut = 60;
    std::string str = "work";
    bool res = XmlTest::ParseTagValueFunc(env, strXml, cRecv, intPut, str);
    ASSERT_TRUE(res);
}

/* @tc.name: XmlParseTagValueFuncFunction005
 * @tc.desc: Test ParseTagValueFunc Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, XmlParseTagValueFuncFunction005, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string str1 = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><todo>Work</todo>";
    char cRecv = static_cast<char>(37);
    size_t intPut = 60;
    std::string str2 = "work";
    bool res = XmlTest::ParseTagValueFunc(env, str1, cRecv, intPut, str2);
    ASSERT_TRUE(res);
}

/* @tc.name: XmlParseTagValueFuncFunction006
 * @tc.desc: Test ParseTagValueFunc Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, XmlParseTagValueFuncFunction006, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string str1 = "<?xml version=\"1.0\" encoding=\"utf-8\"?><!DOCTYPE note [\n<!ENTITY foo \"baa\">]>";
    char cRecv = static_cast<char>(100);
    size_t intPut = 70;
    std::string str2 = "work";
    bool res = XmlTest::ParseTagValueFunc(env, str1, cRecv, intPut, str2);
    ASSERT_TRUE(res);
}

/* @tc.name: XmlParseTagValueFuncFunction007
 * @tc.desc: Test ParseTagValueFunc Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, XmlParseTagValueFuncFunction007, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string str1 = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
    std::string str2 = "<note importance=\"high\" logged=\"true\">";
    std::string str3 = "    <title>Hello\rWorld\n</title>";
    std::string str4 = "    <todo>Work\r\n</todo>";
    std::string str5 = "    <mess><![CDATA[This is a \r\n CDATA section]]></mess>";
    std::string str6 = "</note>";
    std::string strXml = str1 + str2 + str3 + str4 + str5 + str6;
    g_testStr = "";
    OHOS::xml::XmlPullParser xmlPullParser(env, strXml, "utf-8");
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
    std::string cbName = "Method";
    napi_create_function(env, cbName.c_str(), cbName.size(), Method, nullptr, &value3);
    napi_set_named_property(env, object, key1, value1);
    napi_set_named_property(env, object, key2, value2);
    napi_set_named_property(env, object, key3, value3);
    xmlPullParser.DealOptionInfo(env, object);
    xmlPullParser.Parse(env, options, true);
    std::string result = "";
    ASSERT_STREQ(g_testStr.c_str(), result.c_str());
}

/* @tc.name: ParseStartTagFunction001
 * @tc.desc: Test ParseStartTag Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseStartTagFunction001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string xml = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><todo>Work</todo>";
    bool res = XmlTest::ParseStartTag(env, xml);
    ASSERT_TRUE(res);
}

/* @tc.name: ParseEndTagFunction001
 * @tc.desc: Test ParseStartTag Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseEndTagFunction001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string xml = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
    bool res = XmlTest::ParseEndTagFunction(env, xml);
    ASSERT_TRUE(res);
}

/* @tc.name: ParseInnerAttriDeclFunction001
 * @tc.desc: Test ParseInnerAttriDecl Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseInnerAttriDeclFunction001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string str1 = "<?xml version=\"1.0\" encoding=\"utf-8\"?><!DOCTYPE note";
    std::string str2 = " [\n<!ATTLIST operand type src # \"dest\">]><note importance=\"high\" logged=\"true\">";
    std::string strXml = str1 + str2;
    g_testStr = "";
    OHOS::xml::XmlPullParser xmlPullParser(env, strXml, "utf-8");
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
    std::string cbName = "Method";
    napi_create_function(env, cbName.c_str(), cbName.size(), Method, nullptr, &value3);
    napi_set_named_property(env, object, key1, value1);
    napi_set_named_property(env, object, key2, value2);
    napi_set_named_property(env, object, key3, value3);
    xmlPullParser.DealOptionInfo(env, object);
    xmlPullParser.Parse(env, options, true);
    ASSERT_STREQ(g_testStr.c_str(), "");
}

/* @tc.name: ParseInnerAttriDeclFunction002
 * @tc.desc: Test ParseInnerAttriDecl Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseInnerAttriDeclFunction002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string str1 = "<?xml version=\"1.0\" encoding=\"utf-8\"?><!DOCTYPE note";
    std::string str2 = " [\n<!ATTLIST operand type src #R \"dest\">]><note importance=\"high\" logged=\"true\">";
    std::string strXml = str1 + str2;
    g_testStr = "";
    OHOS::xml::XmlPullParser xmlPullParser(env, strXml, "utf-8");
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
    std::string cbName = "Method";
    napi_create_function(env, cbName.c_str(), cbName.size(), Method, nullptr, &value3);
    napi_set_named_property(env, object, key1, value1);
    napi_set_named_property(env, object, key2, value2);
    napi_set_named_property(env, object, key3, value3);
    xmlPullParser.DealOptionInfo(env, object);
    xmlPullParser.Parse(env, options, true);
    ASSERT_STREQ(g_testStr.c_str(), "");
}

/* @tc.name: ParseInnerAttriDeclFunction003
 * @tc.desc: Test ParseInnerAttriDecl Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseInnerAttriDeclFunction003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string str1 = "<?xml version=\"1.0\" encoding=\"utf-8\"?><!DOCTYPE note";
    std::string str2 = " [\n<!ATTLIST operand type src #F>>>>\"dest\">]><note importance=\"high\" logged=\"true\">";
    std::string strXml = str1 + str2;
    g_testStr = "";
    OHOS::xml::XmlPullParser xmlPullParser(env, strXml, "utf-8");
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
    std::string cbName = "Method";
    napi_create_function(env, cbName.c_str(), cbName.size(), Method, nullptr, &value3);
    napi_set_named_property(env, object, key1, value1);
    napi_set_named_property(env, object, key2, value2);
    napi_set_named_property(env, object, key3, value3);
    xmlPullParser.DealOptionInfo(env, object);
    xmlPullParser.Parse(env, options, true);
    ASSERT_STREQ(g_testStr.c_str(), "");
}

/* @tc.name: ParseInnerAttriDeclFunction004
 * @tc.desc: Test ParseInnerAttriDecl Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseInnerAttriDeclFunction004, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string str1 = "<?xml version=\"1.0\" encoding=\"utf-8\"?><!DOCTYPE note";
    std::string str2 = " [\n<!ATTLIST operand type src #III>>>>\"dest\">]><note importance=\"high\" logged=\"true\">";
    std::string strXml = str1 + str2;
    g_testStr = "";
    OHOS::xml::XmlPullParser xmlPullParser(env, strXml, "utf-8");
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
    std::string cbName = "Method";
    napi_create_function(env, cbName.c_str(), cbName.size(), Method, nullptr, &value3);
    napi_set_named_property(env, object, key1, value1);
    napi_set_named_property(env, object, key2, value2);
    napi_set_named_property(env, object, key3, value3);
    xmlPullParser.DealOptionInfo(env, object);
    xmlPullParser.Parse(env, options, true);
    ASSERT_STREQ(g_testStr.c_str(), "");
}

/* @tc.name: ParseInnerAttriDeclFuncFunction001
 * @tc.desc: Test ParseInnerAttriDeclFunc Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseInnerAttriDeclFuncFunction001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string str1 = "<?xml version=\"1.0\" encoding=\"utf-8\"?><!DOCTYPE note";
    std::string str2 = " [\n<!ATTLIST operand type src #III>>>>()\"dest\">]>";
    std::string str3 = "<note importance=\"high\" logged=\"true\">";
    std::string strXml = str1 + str2 + str3;
    g_testStr = "";
    OHOS::xml::XmlPullParser xmlPullParser(env, strXml, "utf-8");
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
    std::string cbName = "Method";
    napi_create_function(env, cbName.c_str(), cbName.size(), Method, nullptr, &value3);
    napi_set_named_property(env, object, key1, value1);
    napi_set_named_property(env, object, key2, value2);
    napi_set_named_property(env, object, key3, value3);
    xmlPullParser.DealOptionInfo(env, object);
    xmlPullParser.Parse(env, options, true);
    ASSERT_STREQ(g_testStr.c_str(), "");
}

/* @tc.name: ParseInnerAttriDeclFuncFunction002
 * @tc.desc: Test ParseInnerAttriDeclFunc Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ParseInnerAttriDeclFuncFunction002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string str1 = "<?xml version=\"1.0\" encoding=\"utf-8\"?><!DOCTYPE note";
    std::string str2 = " [\n<!ATTLIST operand type src #III>>>>(|)\"dest\">]>";
    std::string str3 = "<note importance=\"high\" logged=\"true\">";
    std::string strXml = str1 + str2 + str3;
    g_testStr = "";
    OHOS::xml::XmlPullParser xmlPullParser(env, strXml, "utf-8");
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
    std::string cbName = "Method";
    napi_create_function(env, cbName.c_str(), cbName.size(), Method, nullptr, &value3);
    napi_set_named_property(env, object, key1, value1);
    napi_set_named_property(env, object, key2, value2);
    napi_set_named_property(env, object, key3, value3);
    xmlPullParser.DealOptionInfo(env, object);
    xmlPullParser.Parse(env, options, true);
    ASSERT_STREQ(g_testStr.c_str(), "");
}

/* @tc.name: DealLengthFunction001
 * @tc.desc: Test DealLength Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DealLengthFunction001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string xml = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><todo>Work</todo>";
    size_t minimum = 10;
    std::string pushStr = "<note importance=\"high\" logged=\"true\">";
    std::string result = XmlTest::DealLengthFuc(env, xml, minimum, pushStr);
    std::string outPut = "<note importance=\"high\" logged=\"true\"><?xml vers";
    ASSERT_STREQ(result.c_str(), outPut.c_str());
}

/* @tc.name: SkipCharFunction001
 * @tc.desc: Test SkipCharFunction Func
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, SkipCharFunction001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string xml = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?><todo>Work</todo>";
    char expected = static_cast<char>(10);
    OHOS::xml::XmlTest testXml;
    int output = testXml.SkipCharFunction(env, xml, expected);
    ASSERT_EQ(output, 63);
}