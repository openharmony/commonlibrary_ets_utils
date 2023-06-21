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

#ifndef TEST_CONVERTXML_H_
#define TEST_CONVERTXML_H_

#include "../js_convertxml.h"

namespace OHOS::Xml {
class CxmlTest {
public:
    CxmlTest() = default;
    ~CxmlTest() = default;
    static std::string Trim(std::string strXmltrim);
    static std::string GetNodeType(const xmlElementType enumType);
    static void GetPrevNodeList(napi_env env, xmlNodePtr curNode);
    static void SetXmlElementType(napi_env env, xmlNodePtr curNode, const napi_value &elementsObject, bool &bFlag);
    static void SetNodeInfo(napi_env env, xmlNodePtr curNode, const napi_value &elementsObject);
    static void DealSpaces(napi_env env, const napi_value napiObj);
};

std::string CxmlTest::Trim(std::string strXmltrim)
{
    ConvertXml convert;
    return convert.Trim(strXmltrim);
}

std::string CxmlTest::GetNodeType(const xmlElementType enumType)
{
    ConvertXml convert;
    return convert.GetNodeType(enumType);
}

void CxmlTest::GetPrevNodeList(napi_env env, xmlNodePtr curNode)
{
    ConvertXml convert;
    convert.GetPrevNodeList(env, curNode);
}

void CxmlTest::SetXmlElementType(napi_env env, xmlNodePtr curNode, const napi_value &elementsObject, bool &bFlag)
{
    ConvertXml convert;
    convert.SetXmlElementType(env, curNode, elementsObject, bFlag);
}

void CxmlTest::SetNodeInfo(napi_env env, xmlNodePtr curNode, const napi_value &elementsObject)
{
    ConvertXml convert;
    convert.SetNodeInfo(env, curNode, elementsObject);
}

void CxmlTest::DealSpaces(napi_env env, const napi_value napiObj)
{
    ConvertXml convert;
    convert.DealSpaces(env, napiObj);
}
}
#endif
