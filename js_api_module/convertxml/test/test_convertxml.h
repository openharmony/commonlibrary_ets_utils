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

#ifndef TEST_CONVERTXML_H
#define TEST_CONVERTXML_H

#include "../js_convertxml.h"

namespace OHOS::Xml {
class CxmlTest {
public:
    CxmlTest() = default;
    ~CxmlTest() = default;
    static std::string Trim(napi_env env, std::string strXmltrim);
    static std::string GetNodeType(napi_env env, const xmlElementType enumType);
    static void GetPrevNodeList(napi_env env, xmlNodePtr curNode);
    static void SetXmlElementType(napi_env env, xmlNodePtr curNode, const napi_value &elementsObject, bool &bFlag);
    static void SetNodeInfo(napi_env env, xmlNodePtr curNode, const napi_value &elementsObject);
    static void DealSpaces(napi_env env, const napi_value napiObj);
    static void SetDefaultKey(napi_env env, size_t i, const std::string strRecv);
    static void DealSingleLine(napi_env env, std::string &strXml, const napi_value &object);
    static void DealComplex(napi_env env, std::string &strXml, const napi_value &object);
    static void Replace(napi_env env, std::string &str, const std::string src, const std::string dst);
    static void DealCDataInfo(napi_env env, bool bCData, xmlNodePtr &curNode);
    static void GetAnDSetPrevNodeList(napi_env env, xmlNodePtr curNode);
};

std::string CxmlTest::Trim(napi_env env, std::string strXmltrim)
{
    ConvertXml convert(env);
    return convert.Trim(strXmltrim);
}

std::string CxmlTest::GetNodeType(napi_env env, const xmlElementType enumType)
{
    ConvertXml convert(env);
    return convert.GetNodeType(enumType);
}

void CxmlTest::GetPrevNodeList(napi_env env, xmlNodePtr curNode)
{
    ConvertXml convert(env);
    convert.GetPrevNodeList(env, curNode);
}

void CxmlTest::SetXmlElementType(napi_env env, xmlNodePtr curNode, const napi_value &elementsObject, bool &bFlag)
{
    ConvertXml convert(env);
    convert.SetXmlElementType(env, curNode, elementsObject, bFlag);
}

void CxmlTest::SetNodeInfo(napi_env env, xmlNodePtr curNode, const napi_value &elementsObject)
{
    ConvertXml convert(env);
    convert.SetNodeInfo(env, curNode, elementsObject);
}

void CxmlTest::DealSpaces(napi_env env, const napi_value napiObj)
{
    ConvertXml convert(env);
    convert.DealSpaces(env, napiObj);
}

void CxmlTest::SetDefaultKey(napi_env env, size_t i, const std::string strRecv)
{
    ConvertXml convert(env);
    convert.SetDefaultKey(i, strRecv);
}

void CxmlTest::DealSingleLine(napi_env env, std::string &strXml, const napi_value &object)
{
    ConvertXml convert(env);
    convert.DealSingleLine(env, strXml, object);
}

void CxmlTest::DealComplex(napi_env env, std::string &strXml, const napi_value &object)
{
    ConvertXml convert(env);
    convert.DealComplex(env, strXml, object);
}

void CxmlTest::Replace(napi_env env, std::string &str, const std::string src, const std::string dst)
{
    ConvertXml convert(env);
    convert.Replace(str, src, dst);
}

void CxmlTest::DealCDataInfo(napi_env env, bool bCData, xmlNodePtr &curNode)
{
    ConvertXml convert(env);
    convert.DealCDataInfo(bCData, curNode);
}

void CxmlTest::GetAnDSetPrevNodeList(napi_env env, xmlNodePtr curNode)
{
    ConvertXml convert(env);
    convert.GetPrevNodeList(env, curNode);
    convert.GetPrevNodeList(env, curNode);

    napi_value recvElement = nullptr;
    napi_create_array(env, &recvElement);
    int32_t index = 0;
    convert.SetPrevInfo(env, recvElement, 0, index);
}
}
#endif // TEST_CONVERTXML_H
