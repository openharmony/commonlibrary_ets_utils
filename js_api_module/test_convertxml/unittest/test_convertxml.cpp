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
#include "js_xml.h"
#include "utils/log.h"

using namespace OHOS::Xml;

/* @tc.name: ConvertXmlTest001
 * @tc.desc: Convert the xml object containing only declaration items to a js object.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, ConvertXmlTest001, testing::ext::TestSize.Level0)
{
    size_t size = 1024;
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
    size_t size = 1024;
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
    size_t size = 1024;
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
    size_t size = 1024;
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
    size_t size = 1024;
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
    size_t size = 1024;
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