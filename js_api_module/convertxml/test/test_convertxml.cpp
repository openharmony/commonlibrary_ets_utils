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

#include "test_convertxml.h"
#include "test.h"

#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include "js_convertxml.h"
#include "js_xml.h"
#include "native_module_convertxml.h"
#include "tools/log.h"

using namespace OHOS::Xml;

napi_value setProperty(napi_env env, napi_value obj, std::vector<std::string> proVec)
{
    const size_t boolNum = 8; // 8 : the counts of the bool value
    napi_value val = nullptr;
    for (size_t i = 0; i < proVec.size();) {
        if (i < boolNum) {
            napi_get_boolean(env, false, &val);
            napi_set_named_property(env, obj, proVec[i].c_str(), val);
            i++;
        } else {
            napi_create_string_utf8(env, proVec[i + 1].c_str(), proVec[i + 1].size(), &val);
            napi_set_named_property(env, obj, proVec[i].c_str(), val);
            i += 2; // 2 : the length of the value and property
        }
    }
    return obj;
}

/* @tc.name: ConvertXmlTest001
 * @tc.desc: Convert the xml object containing only declaration items to a js object.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConvertXmlTest001, testing::ext::TestSize.Level0)
{
    size_t size = 1024; // 1024 : the size is 1024 byte
    void* pBuffer = nullptr;
    napi_value arrayBuffer = nullptr;
    napi_env env = (napi_env)engine_;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.SetDeclaration();

    Options op;
    ConvertXml *convertXml = new ConvertXml();
    std::string xmlStr(reinterpret_cast<char*>(pBuffer));
    napi_value jsObj = convertXml->Convert(env, xmlStr);

    // Do not set start tag '<'
    napi_value declarationObj = nullptr;
    napi_value declarationProp = nullptr;
    napi_value versionVal = nullptr;
    napi_value encodingVal = nullptr;
    napi_get_named_property(env, jsObj, op.declaration.c_str(), &declarationObj);
    napi_get_named_property(env, declarationObj, op.attributes.c_str(), &declarationProp);
    napi_get_named_property(env, declarationProp, "version", &versionVal);
    napi_get_named_property(env, declarationProp, "encoding", &encodingVal);

    std::string verisonStr;
    std::string encodingStr;
    convertXml->DealNapiStrValue(env, versionVal, verisonStr);
    convertXml->DealNapiStrValue(env, encodingVal, encodingStr);
    EXPECT_STREQ(verisonStr.c_str(), "1.0");
    EXPECT_STREQ(encodingStr.c_str(), "utf-8");
}

/* @tc.name: ConvertXmlTest002
 * @tc.desc: Convert the xml object containing an empty element to a js object.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConvertXmlTest002, testing::ext::TestSize.Level0)
{
    size_t size = 1024; // 1024 : the size is 1024 byte
    void* pBuffer = nullptr;
    napi_value arrayBuffer = nullptr;
    napi_env env = (napi_env)engine_;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note1");
    xmlSerializer.EndElement();

    Options op;
    ConvertXml *convertXml = new ConvertXml();
    std::string xmlStr(reinterpret_cast<char*>(pBuffer));
    napi_value jsObj = convertXml->Convert(env, xmlStr);

    napi_value element = nullptr;
    napi_value elements = nullptr;
    napi_value nameVal = nullptr;
    napi_value typeVal = nullptr;
    napi_get_named_property(env, jsObj, op.elements.c_str(), &elements);
    napi_get_element(env, elements, 0, &element);
    napi_get_named_property(env, element, op.name.c_str(), &nameVal);
    napi_get_named_property(env, element, op.type.c_str(), &typeVal);

    std::string nameStr;
    std::string typeStr;
    convertXml->DealNapiStrValue(env, nameVal, nameStr);
    convertXml->DealNapiStrValue(env, typeVal, typeStr);
    EXPECT_STREQ(nameStr.c_str(), "note1");
    EXPECT_STREQ(typeStr.c_str(), "element");
}

/* @tc.name: ConvertXmlTest003
 * @tc.desc: Convert the xml object containing attributes to a js object.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConvertXmlTest003, testing::ext::TestSize.Level0)
{
    size_t size = 1024; // 1024 : the size is 1024 byte
    void* pBuffer = nullptr;
    napi_value arrayBuffer = nullptr;
    napi_env env = (napi_env)engine_;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.StartElement("note1");
    xmlSerializer.SetAttributes("colour", "red");
    xmlSerializer.SetAttributes("shape", "circle");
    xmlSerializer.EndElement();

    Options op;
    ConvertXml *convertXml = new ConvertXml();
    std::string xmlStr(reinterpret_cast<char*>(pBuffer));
    napi_value jsObj = convertXml->Convert(env, xmlStr);

    napi_value elements = nullptr;
    napi_value element = nullptr;
    napi_value attributes = nullptr;
    napi_value colour = nullptr;
    napi_value shape = nullptr;
    napi_get_named_property(env, jsObj, op.elements.c_str(), &elements);
    napi_get_element(env, elements, 0, &element);
    napi_get_named_property(env, element, op.attributes.c_str(), &attributes);
    napi_get_named_property(env, attributes, "colour", &colour);
    napi_get_named_property(env, attributes, "shape", &shape);

    std::string colourStr;
    std::string shapeStr;
    convertXml->DealNapiStrValue(env, colour, colourStr);
    convertXml->DealNapiStrValue(env, shape, shapeStr);
    EXPECT_STREQ(colourStr.c_str(), "red");
    EXPECT_STREQ(shapeStr.c_str(), "circle");
}

/* @tc.name: ConvertXmlTest004
 * @tc.desc: Convert the xml object containing comment to a js object.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConvertXmlTest004, testing::ext::TestSize.Level0)
{
    size_t size = 1024; // 1024 : the size is 1024 byte
    void* pBuffer = nullptr;
    napi_value arrayBuffer = nullptr;
    napi_env env = (napi_env)engine_;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.SetComment("This is a comment");
    xmlSerializer.StartElement("note1");
    xmlSerializer.EndElement();

    Options op;
    ConvertXml *convertXml = new ConvertXml();
    std::string xmlStr(reinterpret_cast<char*>(pBuffer));
    napi_value jsObj = convertXml->Convert(env, xmlStr);

    napi_value elements = nullptr;
    napi_value element = nullptr;
    napi_value commentType = nullptr;
    napi_value commentText = nullptr;
    EXPECT_EQ(napi_get_named_property(env, jsObj, op.elements.c_str(), &elements), napi_status::napi_ok);
    EXPECT_EQ(napi_get_element(env, elements, 0, &element), napi_status::napi_ok);
    EXPECT_EQ(napi_get_named_property(env, element, op.type.c_str(), &commentType), napi_status::napi_ok);
    EXPECT_EQ(napi_get_named_property(env, element, op.comment.c_str(), &commentText), napi_status::napi_ok);

    std::string commentTypeStr;
    std::string commentTextStr;
    convertXml->DealNapiStrValue(env, commentType, commentTypeStr);
    convertXml->DealNapiStrValue(env, commentText, commentTextStr);
    EXPECT_STREQ(commentTypeStr.c_str(), "comment");
    EXPECT_STREQ(commentTextStr.c_str(), "This is a comment");
}

/* @tc.name: ConvertXmlTest005
 * @tc.desc: Convert the xml object containing cdata to a js object.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConvertXmlTest005, testing::ext::TestSize.Level0)
{
    size_t size = 1024; // 1024 : the size is 1024 byte
    void* pBuffer = nullptr;
    napi_value arrayBuffer = nullptr;
    napi_env env = (napi_env)engine_;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.SetCData("function foo() {}");
    xmlSerializer.StartElement("note1");
    xmlSerializer.EndElement();

    Options op;
    ConvertXml *convertXml = new ConvertXml();
    std::string xmlStr(reinterpret_cast<char*>(pBuffer));
    napi_value jsObj = convertXml->Convert(env, xmlStr);

    napi_value elements = nullptr;
    napi_value element = nullptr;
    napi_value cdataType = nullptr;
    napi_value cdataText = nullptr;
    EXPECT_EQ(napi_get_named_property(env, jsObj, op.elements.c_str(), &elements), napi_status::napi_ok);
    EXPECT_EQ(napi_get_element(env, elements, 0, &element), napi_status::napi_ok);
    EXPECT_EQ(napi_get_named_property(env, element, op.type.c_str(), &cdataType), napi_status::napi_ok);
    EXPECT_EQ(napi_get_named_property(env, element, op.cdata.c_str(), &cdataText), napi_status::napi_ok);

    std::string cdataTypeStr;
    std::string cdataTextStr;
    convertXml->DealNapiStrValue(env, cdataType, cdataTypeStr);
    convertXml->DealNapiStrValue(env, cdataText, cdataTextStr);
    EXPECT_STREQ(cdataTypeStr.c_str(), "cdata");
    EXPECT_STREQ(cdataTextStr.c_str(), "function foo() {}");
}

/* @tc.name: ConvertXmlTest006
 * @tc.desc: Convert the xml object containing doctype to a js object.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConvertXmlTest006, testing::ext::TestSize.Level0)
{
    size_t size = 1024; // 1024 : the size is 1024 byte
    void* pBuffer = nullptr;
    napi_value arrayBuffer = nullptr;
    napi_env env = (napi_env)engine_;
    napi_create_arraybuffer(env, size, &pBuffer, &arrayBuffer);
    OHOS::xml::XmlSerializer xmlSerializer(reinterpret_cast<char*>(pBuffer), size, "utf-8");
    xmlSerializer.SetDocType("root SYSTEM \"http://www.test.org/test.dtd\"");
    xmlSerializer.StartElement("note1");
    xmlSerializer.EndElement();

    Options op;
    ConvertXml *convertXml = new ConvertXml();
    std::string xmlStr(reinterpret_cast<char*>(pBuffer));
    napi_value jsObj = convertXml->Convert(env, xmlStr);

    napi_value elements = nullptr;
    napi_value element = nullptr;
    napi_value docType = nullptr;
    napi_value docText = nullptr;
    EXPECT_EQ(napi_get_named_property(env, jsObj, op.elements.c_str(), &elements), napi_status::napi_ok);
    EXPECT_EQ(napi_get_element(env, elements, 0, &element), napi_status::napi_ok);
    EXPECT_EQ(napi_get_named_property(env, element, op.type.c_str(), &docType), napi_status::napi_ok);
    EXPECT_EQ(napi_get_named_property(env, element, op.doctype.c_str(), &docText), napi_status::napi_ok);

    std::string docTypeStr;
    std::string docStr;
    convertXml->DealNapiStrValue(env, docType, docTypeStr);
    convertXml->DealNapiStrValue(env, docText, docStr);
    EXPECT_STREQ(docTypeStr.c_str(), "doctype");
    EXPECT_STREQ(docStr.c_str(), "root");
}

/* @tc.name: ConstructorTest001
 * @tc.desc: Convert the xml object containing doctype to a js object.
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
 * @tc.desc: Convert the xml object containing doctype to a js object.
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
 * @tc.desc: Convert the xml object containing doctype to a js object.
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
    napi_has_named_property(env, object, utf8Name, &isHas);
    ASSERT_TRUE(isHas);
}

/* @tc.name: ConvertTest001
 * @tc.desc: Convert the xml object containing doctype to a js object.
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
 * @tc.desc: Convert the xml object containing doctype to a js object.
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
 * @tc.desc: Convert the xml object containing doctype to a js object.
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

/* @tc.name: DealOptionsTest001
 * @tc.desc: Convert the xml object containing doctype to a js object.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DealOptionsTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value obj = nullptr;
    napi_create_object(env, &obj);

    std::vector<std::string> proVec = {"trim", "ignoreDeclaration", "ignoreInstruction", "ignoreAttributes",
        "ignoreComment", "ignoreCDATA", "ignoreDoctype", "ignoreText", "declarationKey", "_declaration",
        "instructionKey", "_instruction", "attributesKey", "_attributes", "textKey", "_text", "cdataKey", "_cdata",
        "doctypeKey", "_doctype", "commentKey", "_comment", "parentKey", "_parent", "typeKey", "_type",
        "nameKey", "_name", "elementsKey", "_elements"};

    obj = setProperty(env, obj, proVec);
    OHOS::Xml::ConvertXml convertXml = OHOS::Xml::ConvertXml();
    convertXml.DealOptions(env, obj);
    bool isHas = false;
    napi_has_named_property(env, obj, "textKey", &isHas);
    ASSERT_TRUE(isHas);
}

/* @tc.name: NativeModuleConvertXmlTest001
 * @tc.desc: Convert the xml object containing doctype to a js object.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, NativeModuleConvertXmlTest001, testing::ext::TestSize.Level1)
{
    napi_env env = (napi_env)engine_;
    napi_value exports = nullptr;
    napi_create_object(env, &exports);
    OHOS::Xml::ConvertXmlInit(env, exports);
    napi_value convertXmlClass = nullptr;
    napi_get_named_property(env, exports, "ConvertXml", &convertXmlClass);

    napi_value instance = nullptr;
    napi_new_instance(env, convertXmlClass, 0, nullptr, &instance);

    napi_value args[2]; // 2: number of arguments
    std::string firXml = "<?xml version=\"1.0\" encoding=\"utf-8\"?><note importance=\"high\" logged=\"true\">";
    std::string secXml = "<title>Happy</title></note>";
    std::string strXml = firXml + secXml;
    napi_create_string_utf8(env, strXml.c_str(), strXml.size(), &args[0]);

    napi_value obj = nullptr;
    napi_create_object(env, &obj);
    std::vector<std::string> proVec = {"trim", "ignoreDeclaration", "ignoreInstruction", "ignoreAttributes",
        "ignoreComment", "ignoreCDATA", "ignoreDoctype", "ignoreText", "declarationKey", "_declaration",
        "instructionKey", "_instruction", "attributesKey", "_attributes", "textKey", "_text", "cdataKey", "_cdata",
        "doctypeKey", "_doctype", "commentKey", "_comment", "parentKey", "_parent", "typeKey", "_type",
        "nameKey", "_name", "elementsKey", "_elements"};
    args[1] = setProperty(env, obj, proVec);

    napi_value funcResultValue = nullptr;
    napi_value testFunc = nullptr;
    napi_get_named_property(env, instance, "convert", &testFunc);
    napi_call_function(env, instance, testFunc, 2, args, &funcResultValue); // 2: number of arguments
    ASSERT_NE(funcResultValue, nullptr);
}

HWTEST_F(NativeEngineTest, TrimTest001, testing::ext::TestSize.Level1)
{
    CxmlTest::Trim("");
    std::string res = CxmlTest::Trim(" #e ");
    ASSERT_STREQ(res.c_str(), "#e");
}

HWTEST_F(NativeEngineTest, GetNodeTypeTest001, testing::ext::TestSize.Level1)
{
    xmlElementType enumType = XML_ATTRIBUTE_NODE;
    std::string res = CxmlTest::GetNodeType(enumType);
    ASSERT_STREQ(res.c_str(), "attribute");
    enumType = XML_ENTITY_REF_NODE;
    CxmlTest::GetNodeType(enumType);
    enumType = XML_ENTITY_NODE;
    CxmlTest::GetNodeType(enumType);
    enumType = XML_PI_NODE;
    CxmlTest::GetNodeType(enumType);
    enumType = XML_DOCUMENT_NODE;
    CxmlTest::GetNodeType(enumType);
    enumType = XML_DOCUMENT_TYPE_NODE;
    CxmlTest::GetNodeType(enumType);
    enumType = XML_DOCUMENT_FRAG_NODE;
    CxmlTest::GetNodeType(enumType);
    enumType = XML_DOCB_DOCUMENT_NODE;
    CxmlTest::GetNodeType(enumType);
    enumType = XML_XINCLUDE_END;
    res = CxmlTest::GetNodeType(enumType);
    ASSERT_STREQ(res.c_str(), "");
}

HWTEST_F(NativeEngineTest, GetPrevNodeListTest001, testing::ext::TestSize.Level1)
{
    napi_env env = (napi_env)engine_;
    xmlNodePtr curNode = new xmlNode;
    xmlNodePtr curNode1 = new xmlNode;
    curNode->prev = curNode1;
    curNode1->prev = nullptr;
    curNode1->type = XML_PI_NODE;
    curNode1->name =  reinterpret_cast<const xmlChar *>("Hello world!");
    curNode1->content = const_cast<xmlChar *>(reinterpret_cast<const xmlChar *>("Hello world!"));
    CxmlTest::GetPrevNodeList(env, curNode);
    ASSERT_TRUE(curNode != nullptr);
    delete curNode;
    delete curNode1;
}

HWTEST_F(NativeEngineTest, SetXmlElementTypeTest001, testing::ext::TestSize.Level1)
{
    napi_env env = (napi_env)engine_;
    napi_value elementsObject = nullptr;
    napi_create_object(env, &elementsObject);
    xmlNodePtr curNode1 = new xmlNode;
    curNode1->type = XML_PI_NODE;
    curNode1->name =  reinterpret_cast<const xmlChar *>("Hello world!");
    curNode1->content = const_cast<xmlChar *>(reinterpret_cast<const xmlChar *>("Hello world!"));
    bool flag = false;
    CxmlTest::SetXmlElementType(env, curNode1, elementsObject, flag);
    flag = false;
    curNode1->type = XML_COMMENT_NODE;
    CxmlTest::SetXmlElementType(env, curNode1, elementsObject, flag);
    delete curNode1;
    ASSERT_TRUE(flag);
}

HWTEST_F(NativeEngineTest, SetNodeInfoTest001, testing::ext::TestSize.Level1)
{
    napi_env env = (napi_env)engine_;
    napi_value elementsObject = nullptr;
    napi_create_object(env, &elementsObject);
    xmlNodePtr curNode1 = new xmlNode;
    curNode1->type = XML_PI_NODE;
    curNode1->name =  reinterpret_cast<const xmlChar *>("Hello world!");
    curNode1->content = const_cast<xmlChar *>(reinterpret_cast<const xmlChar *>("Hello world!"));
    bool flag = true;
    CxmlTest::SetNodeInfo(env, curNode1, elementsObject);
    delete curNode1;
    ASSERT_TRUE(flag);
}

HWTEST_F(NativeEngineTest, DealSpacesTest001, testing::ext::TestSize.Level1)
{
    napi_env env = (napi_env)engine_;
    napi_value napiObj = nullptr;
    napi_create_object(env, &napiObj);
    napi_value spacesValue;
    napi_create_string_utf8(env, "hello world", NAPI_AUTO_LENGTH, &spacesValue);
    napi_set_named_property(env, napiObj, "spaces", spacesValue);
    bool flag = true;
    CxmlTest::DealSpaces(env, napiObj);
    ASSERT_TRUE(flag);
}

HWTEST_F(NativeEngineTest, DealSpacesTest002, testing::ext::TestSize.Level1)
{
    napi_env env = (napi_env)engine_;
    napi_value napiObj = nullptr;
    napi_create_object(env, &napiObj);
    napi_value spacesValue;
    napi_create_int32(env, 123, &spacesValue); // 123: number of test number
    napi_set_named_property(env, napiObj, "spaces", spacesValue);
    bool flag = true;
    CxmlTest::DealSpaces(env, napiObj);
    ASSERT_TRUE(flag);
}

HWTEST_F(NativeEngineTest, SetDefaultKeyTest001, testing::ext::TestSize.Level1)
{
    size_t i = 15; // 15: number of default number
    std::string key = "hello";
    CxmlTest::SetDefaultKey(i, key);
    ASSERT_STREQ(key.c_str(), "hello");
}

HWTEST_F(NativeEngineTest, DealSingleLineTest001, testing::ext::TestSize.Level1)
{
    napi_env env = (napi_env)engine_;
    std::string key = "xmlsss<zyyzyy>ssa";
    napi_value napiObj = nullptr;
    napi_create_object(env, &napiObj);
    CxmlTest::DealSingleLine(env, key, napiObj);
    ASSERT_STREQ(key.c_str(), "<node>xmlsss<zyyzyy>ssassa</node>");
}

HWTEST_F(NativeEngineTest, DealComplexTest001, testing::ext::TestSize.Level1)
{
    napi_env env = (napi_env)engine_;
    std::string key = "xmlsss<!DOCTYPE>ssa";
    napi_value napiObj = nullptr;
    napi_create_object(env, &napiObj);
    CxmlTest::DealComplex(env, key, napiObj);
    ASSERT_STREQ(key.c_str(), "xmlsss<!DOCTYPE>ssa<node></node>");
}

HWTEST_F(NativeEngineTest, ReplaceTest001, testing::ext::TestSize.Level1)
{
    std::string str = "xmlsss<!DOCTYPE>ssa";
    std::string src = "sss";
    std::string dst = "zyy";
    CxmlTest::Replace(str, src, dst);
    ASSERT_STREQ(str.c_str(), "xmlzyy<!DOCTYPE>ssa");
}

HWTEST_F(NativeEngineTest, DealCDataInfo001, testing::ext::TestSize.Level1)
{
    bool flag = true;
    xmlNodePtr curNode = new xmlNode;
    xmlNodePtr curNode1 = new xmlNode;
    xmlNodePtr curNode2 = new xmlNode;
    curNode->next = curNode1;
    curNode->type = XML_CDATA_SECTION_NODE;
    curNode1->type = XML_TEXT_NODE;
    curNode1->next = curNode2;
    curNode2->type = XML_CDATA_SECTION_NODE;
    curNode1->name =  reinterpret_cast<const xmlChar *>("Hello world!");
    curNode1->content = const_cast<xmlChar *>(reinterpret_cast<const xmlChar *>("Hello world!"));
    CxmlTest::DealCDataInfo(flag, curNode);
    delete curNode2;
    delete curNode1;
    delete curNode;
    ASSERT_TRUE(flag);
}