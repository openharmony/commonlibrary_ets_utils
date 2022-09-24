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

#include "js_convertxml.h"
#include "securec.h"
#include "utils/log.h"
namespace OHOS::Xml {
    ConvertXml::ConvertXml()
    {
            spaceType_ = SpaceType::T_INIT;
            strSpace_ = "";
            iSpace_ = 0;
    }
    std::string ConvertXml::GetNodeType(const xmlElementType enumType) const
    {
        std::string strResult = "";
        switch (enumType) {
            case xmlElementType::XML_ELEMENT_NODE:
                strResult = "element";
                break;
            case xmlElementType::XML_ATTRIBUTE_NODE:
                strResult = "attribute";
                break;
            case xmlElementType::XML_TEXT_NODE:
                strResult = "text";
                break;
            case xmlElementType::XML_CDATA_SECTION_NODE:
                strResult = "cdata";
                break;
            case xmlElementType::XML_ENTITY_REF_NODE:
                strResult = "entity_ref";
                break;
            case xmlElementType::XML_ENTITY_NODE:
                strResult = "entity";
                break;
            case xmlElementType::XML_PI_NODE:
                strResult = "instruction";
                break;
            case xmlElementType::XML_COMMENT_NODE:
                strResult = "comment";
                break;
            case xmlElementType::XML_DOCUMENT_NODE:
                strResult = "document";
                break;
            case xmlElementType::XML_DOCUMENT_TYPE_NODE:
                strResult = "document_type";
                break;
            case xmlElementType::XML_DOCUMENT_FRAG_NODE:
                strResult = "document_frag";
                break;
            case xmlElementType::XML_DTD_NODE:
                strResult = "doctype";
                break;
#ifdef LIBXML_DOCB_ENABLED
            case xmlElementType::XML_DOCB_DOCUMENT_NODE:
                strResult =  "docb_document";
                break;
#endif
            default:
                break;
        }
        return strResult;
    }

    void ConvertXml::SetKeyValue(napi_env env, const napi_value &object, const std::string strKey,
                                 const std::string strValue) const
    {
        napi_value attrValue = nullptr;
        napi_create_string_utf8(env, strValue.c_str(), NAPI_AUTO_LENGTH, &attrValue);
        napi_set_named_property(env, object, strKey.c_str(), attrValue);
    }
    std::string ConvertXml::Trim(std::string strXmltrim) const
    {
        if (strXmltrim.empty()) {
            return "";
        }
        size_t i = 0;
        size_t strlen = strXmltrim.size();
        for (; i < strlen;) {
            if (strXmltrim[i] == ' ') {
                i++;
            } else {
                break;
            }
        }
        strXmltrim = strXmltrim.substr(i);
        strlen = strXmltrim.size();
        for (i = strlen - 1; i != 0; i--) {
            if (strXmltrim[i] == ' ') {
                strXmltrim.pop_back();
            } else {
                break;
            }
        }
        return strXmltrim;
    }

    void ConvertXml::GetPrevNodeList(napi_env env, xmlNodePtr curNode)
    {
        while (curNode->prev != nullptr) {
            curNode = curNode->prev;
            napi_value elementsObject = nullptr;
            napi_create_object(env, &elementsObject);
            char *curContent = nullptr;
            if (curNode->type == xmlElementType::XML_PI_NODE && !options_.ignoreInstruction) {
                SetKeyValue(env, elementsObject, options_.type, GetNodeType(curNode->type));
                SetKeyValue(env, elementsObject, options_.name, reinterpret_cast<const char*>(curNode->name));
                curContent = reinterpret_cast<char*>(xmlNodeGetContent(curNode));
                if (curContent != nullptr) {
                    SetKeyValue(env, elementsObject, options_.instruction, curContent);
                    xmlFree(reinterpret_cast<void*>(curContent));
                }
                prevObj_.push_back(elementsObject);
            }
            if (curNode->type == xmlElementType::XML_COMMENT_NODE && !options_.ignoreComment) {
                SetKeyValue(env, elementsObject, options_.type, GetNodeType(curNode->type));
                curContent = reinterpret_cast<char*>(xmlNodeGetContent(curNode));
                if (curContent != nullptr) {
                    SetKeyValue(env, elementsObject, options_.comment, curContent);
                    xmlFree(reinterpret_cast<void*>(curContent));
                }
                prevObj_.push_back(elementsObject);
            }
            if (curNode->type == xmlElementType::XML_DTD_NODE && !options_.ignoreDoctype) {
                SetKeyValue(env, elementsObject, options_.type, GetNodeType(curNode->type));
                SetKeyValue(env, elementsObject, options_.doctype,
                            reinterpret_cast<const char*>(curNode->name));
                prevObj_.push_back(elementsObject);
            }
        }
    }

    void ConvertXml::SetAttributes(napi_env env, xmlNodePtr curNode, const napi_value &elementsObject) const
    {
        xmlAttr *attr = curNode->properties;
        if (attr && !options_.ignoreAttributes) {
            napi_value attrTitleObj = nullptr;
            napi_create_object(env, &attrTitleObj);
            while (attr) {
                SetKeyValue(env, attrTitleObj, reinterpret_cast<const char*>(attr->name),
                            reinterpret_cast<const char*>(attr->children->content));
                attr = attr->next;
            }
            napi_set_named_property(env, elementsObject, options_.attributes.c_str(), attrTitleObj);
        }
    }

    void ConvertXml::SetXmlElementType(napi_env env, xmlNodePtr curNode, const napi_value &elementsObject,
                                       bool &bFlag) const
    {
        char *curContent = reinterpret_cast<char*>(xmlNodeGetContent(curNode));
        if (curNode->type == xmlElementType::XML_PI_NODE && !options_.ignoreInstruction) {
            if (curContent != nullptr) {
                SetKeyValue(env, elementsObject, options_.instruction, curContent);
                bFlag = true;
            }
        } else if (curNode->type == xmlElementType::XML_COMMENT_NODE && !options_.ignoreComment) {
            if (curContent != nullptr) {
                SetKeyValue(env, elementsObject, options_.comment, curContent);
                bFlag = true;
            }
        } else if (curNode->type == xmlElementType::XML_CDATA_SECTION_NODE && !options_.ignoreCdata) {
            if (curContent != nullptr) {
                SetKeyValue(env, elementsObject, options_.cdata, curContent);
                bFlag = true;
            }
        }
        if (curContent != nullptr) {
            xmlFree(reinterpret_cast<void*>(curContent));
        }
    }
    void ConvertXml::SetNodeInfo(napi_env env, xmlNodePtr curNode, const napi_value &elementsObject) const
    {
        if (curNode->type == xmlElementType::XML_TEXT_NODE) {
            return;
        } else {
            if (curNode->type == xmlElementType::XML_PI_NODE) {
                if (!options_.ignoreInstruction) {
                    SetKeyValue(env, elementsObject, options_.type, GetNodeType(curNode->type));
                }
            } else {
                    SetKeyValue(env, elementsObject, options_.type, GetNodeType(curNode->type));
            }
            if ((curNode->type != xmlElementType::XML_COMMENT_NODE) &&
                (curNode->type != xmlElementType::XML_CDATA_SECTION_NODE)) {
                if (!(curNode->type == xmlElementType::XML_PI_NODE && options_.ignoreInstruction)) {
                    SetKeyValue(env, elementsObject, options_.name, reinterpret_cast<const char*>(curNode->name));
                }
            }
        }
    }

    void ConvertXml::SetEndInfo(napi_env env, xmlNodePtr curNode, const napi_value &elementsObject,
                                bool &bFlag) const
    {
        SetKeyValue(env, elementsObject, options_.type, GetNodeType(curNode->type));
        if (curNode->type == xmlElementType::XML_ELEMENT_NODE) {
            SetKeyValue(env, elementsObject, options_.name, reinterpret_cast<const char*>(curNode->name));
            bFlag = true;
        } else if (curNode->type == xmlElementType::XML_TEXT_NODE) {
            char *curContent = reinterpret_cast<char*>(xmlNodeGetContent(curNode));
            if (options_.trim) {
                if (curContent != nullptr) {
                    SetKeyValue(env, elementsObject, options_.text,
                                Trim(curContent));
                }
            } else {
                if (curContent != nullptr) {
                    SetKeyValue(env, elementsObject, options_.text, curContent);
                }
            }
            if (curContent != nullptr) {
                xmlFree(reinterpret_cast<void*>(curContent));
            }
            if (!options_.ignoreText) {
                bFlag = true;
            }
        }
    }

    void ConvertXml::SetPrevInfo(napi_env env, const napi_value &recvElement, int flag, int32_t &index1) const
    {
        if (!prevObj_.empty() && !flag) {
            for (size_t i = (prevObj_.size() - 1); i > 0; --i) {
                napi_set_element(env, recvElement, index1++, prevObj_[i]);
            }
            napi_set_element(env, recvElement, index1++, prevObj_[0]);
        }
    }

    void ConvertXml::GetXMLInfo(napi_env env, xmlNodePtr curNode, const napi_value &object, int flag)
    {
        napi_value elements = nullptr;
        napi_create_array(env, &elements);
        napi_value recvElement = nullptr;
        napi_create_array(env, &recvElement);
        xmlNodePtr pNode = curNode;
        int32_t index = 0;
        int32_t index1 = 0;
        bool bFlag = false;
        while (pNode != nullptr) {
            bFlag = false;
            napi_value elementsObject = nullptr;
            napi_create_object(env, &elementsObject);
            SetNodeInfo(env, pNode, elementsObject);
            SetAttributes(env, pNode, elementsObject);
            napi_value tempElement = nullptr;
            napi_create_array(env, &tempElement);
            napi_value elementObj = nullptr;
            napi_create_object(env, &elementObj);
            char *curContent = reinterpret_cast<char*>(xmlNodeGetContent(pNode));
            if (curContent != nullptr) {
                if (pNode->children != nullptr) {
                    curNode = pNode->children;
                    GetXMLInfo(env, curNode, elementsObject, 1);
                    bFlag = true;
                } else {
                    SetXmlElementType(env, pNode, elementsObject, bFlag);
                    SetEndInfo(env, pNode, elementsObject, bFlag);
                }
                xmlFree(reinterpret_cast<void*>(curContent));
            }
            SetPrevInfo(env, recvElement, flag, index1);
            if (elementsObject != nullptr && bFlag) {
                napi_set_element(env, recvElement, index1++, elementsObject);
                elementsObject = nullptr;
            }
            index++;
            pNode = pNode->next;
        }
        if (bFlag) {
            napi_set_named_property(env, object, options_.elements.c_str(), recvElement);
        }
    }

    void ConvertXml::SetSpacesInfo(napi_env env, const napi_value &object) const
    {
        napi_value iTemp = nullptr;
        switch (spaceType_) {
            case (SpaceType::T_INT32):
                napi_create_int32(env, iSpace_, &iTemp);
                napi_set_named_property(env, object, "spaces", iTemp);
                break;
            case (SpaceType::T_STRING):
                SetKeyValue(env, object, "spaces", strSpace_);
                break;
            case (SpaceType::T_INIT):
                SetKeyValue(env, object, "spaces", strSpace_);
                break;
            default:
                break;
            }
    }

    napi_value ConvertXml::Convert(napi_env env, std::string strXml)
    {
        xmlDocPtr doc = nullptr;
        xmlNodePtr curNode = nullptr;
        napi_status status = napi_ok;
        napi_value object = nullptr;
        status = napi_create_object(env, &object);
        if (status != napi_ok) {
            return nullptr;
        }
        Replace(strXml, "\\r", "\r");
        Replace(strXml, "\\n", "\n");
        Replace(strXml, "\\v", "\v");
        Replace(strXml, "\\t", "\t");
        Replace(strXml, "]]><![CDATA", "]]> <![CDATA");
        size_t len = strXml.size();
        doc = xmlParseMemory(strXml.c_str(), len);
        if (!doc) {
            xmlFreeDoc(doc);
            DealSingleLine(env, strXml, object);
            return object;
        }
        napi_value subObject = nullptr;
        napi_value subSubObject = nullptr;
        napi_create_object(env, &subSubObject);
        napi_create_object(env, &subObject);
        if (doc != nullptr && doc->version != nullptr) {
            SetKeyValue(env, subSubObject, "version", (const char*)doc->version);
        }
        if (doc != nullptr && doc->encoding != nullptr) {
            SetKeyValue(env, subSubObject, "encoding", (const char*)doc->encoding);
        }
        if (!options_.ignoreDeclaration && strXml.find("xml") != std::string::npos) {
            napi_set_named_property(env, subObject, options_.attributes.c_str(), subSubObject);
            napi_set_named_property(env, object, options_.declaration.c_str(), subObject);
        }
        if (doc != nullptr) {
            curNode = xmlDocGetRootElement(doc);
            GetPrevNodeList(env, curNode);
            GetXMLInfo(env, curNode, object, 0);
        }
        SetSpacesInfo(env, object);
        return object;
    }

    napi_status ConvertXml::DealNapiStrValue(napi_env env, const napi_value napi_StrValue, std::string &result) const
    {
        std::string buffer = "";
        size_t bufferSize = 0;
        napi_status status = napi_ok;
        status = napi_get_value_string_utf8(env, napi_StrValue, nullptr, -1, &bufferSize);
        if (status != napi_ok) {
            HILOG_ERROR("can not get buffer size");
            return status;
        }
        buffer.reserve(bufferSize + 1);
        buffer.resize(bufferSize);
        if (bufferSize > 0) {
            status = napi_get_value_string_utf8(env, napi_StrValue, buffer.data(), bufferSize + 1, &bufferSize);
            if (status != napi_ok) {
                HILOG_ERROR("can not get buffer value");
                return status;
            }
        }
        if (buffer.data() != nullptr) {
            result = buffer;
        }
        return status;
    }

    void ConvertXml::DealSpaces(napi_env env, const napi_value napiObj)
    {
        napi_value recvTemp = nullptr;
        napi_get_named_property(env, napiObj, "spaces", &recvTemp);
        napi_valuetype valuetype = napi_undefined;
        napi_typeof(env, recvTemp, &valuetype);
        if (valuetype == napi_string) {
            DealNapiStrValue(env, recvTemp, strSpace_);
            spaceType_ = SpaceType::T_STRING;
        } else if (valuetype == napi_number) {
            int32_t iTemp;
            if (napi_get_value_int32(env, recvTemp, &iTemp) == napi_ok) {
                iSpace_ = iTemp;
                spaceType_ = SpaceType::T_INT32;
            }
        }
    }

    void ConvertXml::DealIgnore(napi_env env, const napi_value napiObj)
    {
        std::vector<std::string> vctIgnore = {"compact", "trim", "ignoreDeclaration", "ignoreInstruction",
                                              "ignoreAttributes", "ignoreComment", "ignoreCDATA",
                                              "ignoreDoctype", "ignoreText"};
        size_t vctLength = vctIgnore.size();
        for (size_t i = 0; i < vctLength; ++i) {
            napi_value recvTemp = nullptr;
            bool bRecv = false;
            napi_get_named_property(env, napiObj, vctIgnore[i].c_str(), &recvTemp);
            if ((napi_get_value_bool(env, recvTemp, &bRecv)) == napi_ok) {
                switch (i) {
                    case 0:
                        options_.compact = bRecv;
                        break;
                    case 1: // 1:trim
                        options_.trim = bRecv;
                        break;
                    case 2: // 2:ignoreDeclaration
                        options_.ignoreDeclaration = bRecv;
                        break;
                    case 3: // 3:ignoreInstruction
                        options_.ignoreInstruction = bRecv;
                        break;
                    case 4: // 4:ignoreAttributes
                        options_.ignoreAttributes = bRecv;
                        break;
                    case 5: // 5:ignoreComment
                        options_.ignoreComment = bRecv;
                        break;
                    case 6: // 6:ignoreCdata
                        options_.ignoreCdata = bRecv;
                        break;
                    case 7: // 7:ignoreDoctype
                        options_.ignoreDoctype = bRecv;
                        break;
                    case 8: // 8:ignoreText
                        options_.ignoreText = bRecv;
                        break;
                    default:
                        break;
                }
            }
        }
    }

    void ConvertXml::SetDefaultKey(size_t i, const std::string strRecv)
    {
        switch (i) {
            case 0:
                options_.declaration = strRecv;
                break;
            case 1:
                options_.instruction = strRecv;
                break;
            case 2: // 2:attributes
                options_.attributes = strRecv;
                break;
            case 3: // 3:text
                options_.text = strRecv;
                break;
            case 4: // 4:cdata
                options_.cdata = strRecv;
                break;
            case 5: // 5:doctype
                options_.doctype = strRecv;
                break;
            case 6: // 6:comment
                options_.comment = strRecv;
                break;
            case 7: // 7:parent
                options_.parent = strRecv;
                break;
            case 8: // 8:type
                options_.type = strRecv;
                break;
            case 9: // 9:name
                options_.name = strRecv;
                break;
            case 10: // 10:elements
                options_.elements = strRecv;
                break;
            default:
                break;
        }
    }

    void ConvertXml::DealOptions(napi_env env, const napi_value napiObj)
    {
        std::vector<std::string> vctOptions = {"declarationKey", "instructionKey", "attributesKey", "textKey",
                                              "cdataKey", "doctypeKey", "commentKey", "parentKey", "typeKey",
                                              "nameKey", "elementsKey"};
        size_t vctLength = vctOptions.size();
        for (size_t i = 0; i < vctLength; ++i) {
            napi_value recvTemp = nullptr;
            std::string strRecv = "";
            napi_get_named_property(env, napiObj, vctOptions[i].c_str(), &recvTemp);
            if ((DealNapiStrValue(env, recvTemp, strRecv)) == napi_ok) {
                SetDefaultKey(i, strRecv);
            }
        }
        DealIgnore(env, napiObj);
        DealSpaces(env, napiObj);
    }

    void ConvertXml::DealSingleLine(napi_env env, std::string &strXml, const napi_value &object)
    {
        size_t iXml = 0;
        if ((iXml = strXml.find("xml")) != std::string::npos) {
            xmlInfo_.bXml = true;
            napi_value declObj = nullptr;
            napi_create_object(env, &declObj);
            napi_value attrObj = nullptr;
            bool bFlag = false;
            napi_create_object(env, &attrObj);
            if (strXml.find("version=") != std::string::npos) {
                xmlInfo_.bVersion = true;
                SetKeyValue(env, attrObj, "version", "1.0");
                bFlag = true;
            }
            if (strXml.find("encoding=") != std::string::npos) {
                xmlInfo_.bEncoding = false;
                SetKeyValue(env, attrObj, "encoding", "utf-8");
                bFlag = true;
            }
            if (bFlag) {
                napi_set_named_property(env, declObj, options_.attributes.c_str(), attrObj);
                napi_set_named_property(env, object, options_.declaration.c_str(), declObj);
            } else {
                napi_set_named_property(env, object, options_.declaration.c_str(), declObj);
            }
            if (strXml.find(">", iXml) == strXml.size() - 1) {
                strXml = "";
            } else {
                strXml = strXml.substr(0, strXml.rfind("<", iXml)) + strXml.substr(strXml.find(">", iXml) + 1);
            }
        }
        size_t iCount = 0;
        size_t iLen = strXml.size();
        for (; iCount < iLen; ++iCount) {
            if (strXml[iCount] != ' ' && strXml[iCount] != '\v' &&
                strXml[iCount] != '\t' && strXml[iCount] != '\n') {
                break;
            }
        }
        if (iCount < iLen) {
            DealComplex(env, strXml, object);
        }
    }

    void ConvertXml::DealComplex(napi_env env, std::string &strXml, const napi_value &object) const
    {
        if (strXml.find("<!DOCTYPE") != std::string::npos) {
            strXml = strXml + "<node></node>";
        } else {
            strXml = "<node>" + strXml + "</node>";
        }
        xmlDocPtr doc = nullptr;
        xmlNodePtr curNode = nullptr;
        size_t len = strXml.size();
        doc = xmlParseMemory(strXml.c_str(), static_cast<int>(len));
        if (!doc) {
            xmlFreeDoc(doc);
        }
        if (doc) {
            curNode = xmlDocGetRootElement(doc);
            curNode = curNode->children;
            napi_value elements = nullptr;
            napi_create_array(env, &elements);
            bool bHasEle = false;
            int index = 0;
            bool bCData = false;
            if (strXml.find("<![CDATA") != strXml.rfind("<![CDATA")) {
                bCData = true;
            }
            while (curNode != nullptr) {
                napi_value elementsObject = nullptr;
                napi_create_object(env, &elementsObject);
                SetNodeInfo(env, curNode, elementsObject);
                SetXmlElementType(env, curNode, elementsObject, bHasEle);
                SetEndInfo(env, curNode, elementsObject, bHasEle);
                napi_set_element(env, elements, index++, elementsObject);
                DealCDataInfo(bCData, curNode);
            }
            if (bHasEle) {
                napi_set_named_property(env, object, options_.elements.c_str(), elements);
            }
            xmlFreeDoc(doc);
        }
    }

    void ConvertXml::Replace(std::string &str, const std::string src, const std::string dst) const
    {
        size_t index = 0;
        while ((index = str.find(src)) != std::string::npos) {
            str.replace(index, src.size(), dst);
        }
    }

    void ConvertXml::DealCDataInfo(bool bCData, xmlNodePtr &curNode) const
    {
        if (bCData && curNode->type == xmlElementType::XML_CDATA_SECTION_NODE &&
            curNode->next && curNode->next->type == xmlElementType::XML_TEXT_NODE &&
            curNode->next->next && curNode->next->next->type == xmlElementType::XML_CDATA_SECTION_NODE) {
                char *curContent = reinterpret_cast<char*>(xmlNodeGetContent(curNode->next));
                if (curContent != nullptr) {
                    std::string strTemp = reinterpret_cast<char*>(curContent);
                    Replace(strTemp, " ", "");
                    Replace(strTemp, "\v", "");
                    Replace(strTemp, "\t", "");
                    Replace(strTemp, "\n", "");
                    if (strTemp == "") {
                        curNode = curNode->next->next;
                    }
                    xmlFree(reinterpret_cast<void*>(curContent));
                }
            } else {
                curNode = curNode->next;
            }
    }
} // namespace OHOS::Xml
