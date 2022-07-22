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

#ifndef CONVERTXML_JS_CONVERTXML_H_
#define CONVERTXML_JS_CONVERTXML_H_

#include <string>
#include <vector>
#include "libxml/parser.h"
#include "libxml/tree.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS::Xml {
    enum class SpaceType {
        T_INT32,
        T_STRING,
        T_INIT = -1
    };

    struct Options {
        std::string declaration = "_declaration";
        std::string instruction = "_instruction";
        std::string attributes = "_attributes";
        std::string text = "_text";
        std::string cdata = "_cdata";
        std::string doctype = "_doctype";
        std::string comment = "_comment";
        std::string parent = "_parent";
        std::string type = "_type";
        std::string name = "_name";
        std::string elements = "_elements";
        bool compact = false;
        bool trim = false;
        bool nativetype = false;
        bool nativetypeattributes = false;
        bool addparent = false;
        bool alwaysArray = false;
        bool alwaysChildren = false;
        bool instructionHasAttributes = false;
        bool ignoreDeclaration = false;
        bool ignoreInstruction = false;
        bool ignoreAttributes = false;
        bool ignoreComment = false;
        bool ignoreCdata = false;
        bool ignoreDoctype = false;
        bool ignoreText = false;
        bool spaces = false;
    };

    struct XmlInfo {
        bool bXml = false;
        bool bVersion = false;
        std::string strVersion = "";
        bool bEncoding = false;
        std::string strEncoding = "";
    };

    class ConvertXml {
    public:
        /**
         * To convert XML text to JavaScript object.
         */
        explicit ConvertXml();

        /**
         * The destructor of the ConvertXml.
         */
        virtual ~ConvertXml() {}

        /**
         * To convert XML text to JavaScript object.
         *
         * @param env NAPI environment parameters.
         * @param strXml A string of XML text.
         */
        napi_value Convert(napi_env env, std::string strXml);

        /**
         * Converts the string of js to string of C++.
         *
         * @param env NAPI environment parameters.
         * @param napi_StrValue JS layer incoming stringing.
         * @param result The C++ layer accepts stringing.
         */
        napi_status DealNapiStrValue(napi_env env, const napi_value napi_StrValue, std::string &result) const;

        /**
         * Handles user input of optional information
         *
         * @param env NAPI environment parameters.
         * @param napiObj Get the option parameter from js to the napi layer
         */
        void DealOptions(napi_env env, const napi_value napiObj);

    private:
        void SetAttributes(napi_env env, xmlNodePtr curNode, const napi_value &elementsObject) const;
        void SetXmlElementType(napi_env env, xmlNodePtr curNode, const napi_value &elementsObject, bool &bFlag) const;
        void SetNodeInfo(napi_env env, xmlNodePtr curNode, const napi_value &elementsObject) const;
        void SetEndInfo(napi_env env, xmlNodePtr curNode, const napi_value &elementsObject, bool &bFlag) const;
        void GetXMLInfo(napi_env env, xmlNodePtr curNode, const napi_value &object, int flag = 0);
        std::string GetNodeType(const xmlElementType enumType) const;
        void SetKeyValue(napi_env env, const napi_value &object, const std::string strKey,
                         const std::string strValue) const;
        std::string Trim(std::string strXmltrim) const;
        void GetPrevNodeList(napi_env env, xmlNodePtr curNode);
        void DealSpaces(napi_env env, const napi_value napiObj);
        void DealIgnore(napi_env env, const napi_value napiObj);
        void SetPrevInfo(napi_env env, const napi_value &recvElement, int flag, int32_t &index1) const;
        void SetDefaultKey(size_t i, const std::string strRecv);
        void SetSpacesInfo(napi_env env, const napi_value &object) const;
        void DealSingleLine(napi_env env, std::string &strXml, const napi_value &object);
        void DealComplex(napi_env env, std::string &strXml, const napi_value &object) const;
        void Replace(std::string &str, const std::string src, const std::string dst) const;
        void DealCDataInfo(bool bCData, xmlNodePtr &curNode) const;

        SpaceType spaceType_;
        int32_t iSpace_;
        std::string strSpace_;
        Options options_;
        std::vector<napi_value> prevObj_;
        XmlInfo xmlInfo_;
    };
} // namespace OHOS::Xml
#endif // CONVERTXML_JS_CONVERTXML_H_
