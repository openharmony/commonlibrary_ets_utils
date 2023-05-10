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

#include "js_xml.h"
#include "securec.h"
namespace OHOS::xml {
    napi_status XmlSerializer::DealNapiStrValue(napi_env env, const napi_value napiStr, std::string &result)
    {
        std::string buffer = "";
        size_t bufferSize = 0;
        napi_status status = napi_ok;
        status = napi_get_value_string_utf8(env, napiStr, nullptr, -1, &bufferSize);
        buffer.reserve(bufferSize + 1);
        buffer.resize(bufferSize);
        if (status != napi_ok) {
            HILOG_ERROR("can not get buffer size");
            return status;
        }
        if (bufferSize > 0) {
            status = napi_get_value_string_utf8(env, napiStr, buffer.data(), bufferSize + 1, &bufferSize);
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
    void XmlSerializer::SplicNsp()
    {
        elementStack[depth_ * 3] = elementStack[(depth_ - 1) * 3]; // 3: number of args
        elementStack[depth_ * 3 + 1] = elementStack[(depth_ - 1) * 3 + 1]; // 3: number of args
        if (multNsp[depth_ - 1].size() == 0) {
            return;
        }
        if (type == "isAttri" || type == "isStart") {
            for (int i = 0; i < CurNspNum; ++i) {
                out_.append(" xmlns:");
                out_.append(multNsp[depth_ - 1][i * 2]); // 2: number of args
                out_.append("=\"");
                out_.append(multNsp[depth_ - 1][i * 2 + 1]); // 2: number of args
                out_.append("\"");
            }
            multNsp[depth_ - 1].clear();
            CurNspNum = 0;
        }
    }

    void XmlSerializer::NextItem()
    {
        out_.append("\r\n");
        for (size_t i = 0; i < depth_; i++) {
            out_.append("  ");
        }
    }
    std::string Replace(std::string str, const std::string &str1, const std::string &str2)
    {
        size_t iPos = 0;
        while ((iPos = str.find(str1)) != std::string::npos) {
            str = str.substr(0, iPos) + str2 + str.substr(iPos + str1.size());
        }
        return str;
    }
    void XmlSerializer::SetDeclaration()
    {
        if (isHasDecl) {
            xmlSerializerError_ = "illegal position for declaration";
            return;
        }
        isHasDecl = true;
        out_ = "";
        out_.append("<?xml version=\"1.0\"");
        out_.append(" encoding=\"");
        out_.append(encoding_);
        out_.append("\"?>");
        type = "isDecl";
        size_t iLenTemp = out_.length();
        if (iLength_ > iPos_ + iLenTemp - 1) {
            if (memcpy_s(pStart_ + iPos_, iLength_ - iPos_, out_.c_str(), iLenTemp) == EOK) {
                iPos_ += iLenTemp;
            } else {
                HILOG_ERROR("SetDeclaration memcpy_s failed");
            }
        }
    }
    void XmlSerializer::SetNamespace(std::string prefix, const std::string &nsTemp)
    {
        out_ = "";
        if (type == "isStart" || type == "isAttri") {
            SplicNsp();
            out_.append(">");
        }
        elementStack[depth_ * 3] = prefix; // 3: number of args
        elementStack[depth_ * 3 + 1] = nsTemp; // 3: number of args
        multNsp[depth_][CurNspNum * 2] = elementStack[depth_ * 3]; // 3: number of args 2: number of args
        multNsp[depth_][CurNspNum * 2 + 1] = elementStack[depth_ * 3 + 1]; // 3: number of args 2: number of args
        ++CurNspNum;
        type = "isNsp";
        size_t iLenTemp = out_.length();
        if (iLength_ > iPos_ + iLenTemp - 1) {
            if (memcpy_s(pStart_ + iPos_, iLength_ - iPos_, out_.c_str(), iLenTemp) == EOK) {
                iPos_ += iLenTemp;
            } else {
                HILOG_ERROR("SetNamespace memcpy_s failed");
            }
        }
    }
    void XmlSerializer::StartElement(const std::string &name)
    {
        out_ = "";
        if (type == "isStart" || type == "isAttri") {
            SplicNsp();
            out_.append(">");
        }
        if (type != "" && type != "isDecl") {
            NextItem();
        }
        elementStack[depth_ * 3 + 2] = name; // 3: number of args 2: number of args
        out_.append("<");
        if (elementStack[depth_ * 3] != "") { // 3: number of args
            out_.append(elementStack[depth_ * 3]); // 3: number of args
            out_.append(":");
        } else if (depth_ != 0) {
            if (elementStack[(depth_ - 1) * 3] != "") { // 3: number of args
                elementStack[depth_ * 3] = elementStack[(depth_ - 1) * 3]; // 3: number of args
                out_.append(elementStack[depth_ * 3]); // 3: number of args
                out_.append(":");
            }
        }
        out_.append(elementStack[depth_ * 3 + 2]); // 3: number of args 2: number of args
        type = "isStart";
        ++depth_;
        elementStack.push_back("");
        elementStack.push_back("");
        elementStack.push_back("");
        size_t iLenTemp = out_.length();
        if (iLength_ > iPos_ + iLenTemp - 1) {
            if (memcpy_s(pStart_ + iPos_, iLength_ - iPos_, out_.c_str(), iLenTemp) == EOK) {
                iPos_ += iLenTemp;
            } else {
                HILOG_ERROR("StartElement memcpy_s failed");
            }
        }
    }
    void XmlSerializer::SetAttributes(const std::string &name, const std::string &value)
    {
        out_ = "";
        if (type != "isStart" && type != "isAttri") {
            xmlSerializerError_ = "illegal position for attribute";
        }
        out_.append(" ");
        out_.append(name);
        out_.append("=\"");
        WriteEscaped(value);
        out_.append("\"");
        type = "isAttri";
        size_t iLenTemp = out_.length();
        if (iLength_ > iPos_ + iLenTemp - 1) {
            if (memcpy_s(pStart_ + iPos_, iLength_ - iPos_, out_.c_str(), iLenTemp) == EOK) {
                iPos_ += iLenTemp;
            } else {
                HILOG_ERROR("SetAttributes memcpy_s failed");
            }
        }
    }
    void XmlSerializer::EndElement()
    {
        out_ = "";
        size_t iLenTemp = 0;
        if (type == "isStart" || type == "isAttri") {
            SplicNsp();
            out_.append("/>");
            type = "isEndTag";
            --depth_;
            iLenTemp = out_.length();
            if (iLength_ > iPos_ + iLenTemp - 1) {
                if (memcpy_s(pStart_ + iPos_, iLength_ - iPos_, out_.c_str(), iLenTemp) == EOK) {
                    iPos_ += iLenTemp;
                } else {
                HILOG_ERROR("StartElement memcpy_s failed");
                }
            }
            return;
        }
        --depth_;
        if (type != "isText") {
            NextItem();
        }
        out_.append("</");
        if (elementStack[depth_ * 3] != "") { // 3: number of args
            out_.append(elementStack[depth_ * 3]); // 3: number of args
            out_.append(":");
        }
        out_.append(elementStack[depth_ * 3 + 2]); // 3: number of args 2: number of args
        elementStack[depth_ * 3] = ""; // 3: number of args
        elementStack[depth_ * 3 + 1] = ""; // 3: number of args
        type = "isEndTag";
        out_.append(">");
        iLenTemp = out_.length();
        if (iLength_ > iPos_ + iLenTemp - 1) {
            if (memcpy_s(pStart_ + iPos_, iLength_ - iPos_, out_.c_str(), iLenTemp) == EOK) {
                iPos_ += iLenTemp;
            } else {
                HILOG_ERROR("EndElement memcpy_s failed");
            }
        }
    }
    void XmlSerializer::AddEmptyElement(std::string name)
    {
        out_ = "";
        if (type == "isStart" || type == "isAttri") {
            SplicNsp();
            out_.append(">");
        }
        if (type != "") {
            NextItem();
        }
        out_.append("<");
        out_.append(name);
        out_.append("/>");
        type = "isAddEmpElem";
        size_t iLenTemp = out_.length();
        if (iLength_ > iPos_ + iLenTemp - 1) {
            if (memcpy_s(pStart_ + iPos_, iLength_ - iPos_, out_.c_str(), iLenTemp) == EOK) {
                iPos_ += iLenTemp;
            } else {
                HILOG_ERROR("AddEmptyElement memcpy_s failed");
            }
        }
    }
    void XmlSerializer::SetText(const std::string &text)
    {
        out_ = "";
        if (type == "isStart" || type == "isAttri") {
            SplicNsp();
            out_.append(">");
        }
        WriteEscaped(text);
        type = "isText";
        size_t iLenTemp = out_.length();
        if (iLength_ > iPos_ + iLenTemp - 1) {
            if (memcpy_s(pStart_ + iPos_, iLength_ - iPos_, out_.c_str(), iLenTemp) == EOK) {
                iPos_ += iLenTemp;
            } else {
                HILOG_ERROR("SetText memcpy_s failed");
            }
        }
    }
    void XmlSerializer::SetComment(const std::string &comment)
    {
        out_ = "";
        if (type == "isStart" || type == "isAttri") {
            SplicNsp();
            out_.append(">");
        }
        if (type != "") {
            NextItem();
        }
        out_ += "<!--" + comment + "-->";
        type = "isCom";
        size_t iLenTemp = out_.length();
        if (iLength_ > iPos_ + iLenTemp - 1) {
            if (memcpy_s(pStart_ + iPos_, iLength_ - iPos_, out_.c_str(), iLenTemp) == EOK) {
                iPos_ += iLenTemp;
            } else {
                HILOG_ERROR("SetComment memcpy_s failed");
            }
        }
    }
    void XmlSerializer::SetCData(std::string data)
    {
        out_ = "";
        if (type == "isStart" || type == "isAttri") {
            SplicNsp();
            out_.append(">");
        }
        if (type != "") {
            NextItem();
        }
        data = Replace(data, "]]>", "]]]]><![CDATA[>");
        out_ += "<![CDATA[" + data + "]]>";
        type = "isCData";
        size_t iLenTemp = out_.length();
        if (iLength_ > iPos_ + iLenTemp - 1) {
            if (memcpy_s(pStart_ + iPos_, iLength_ - iPos_, out_.c_str(), iLenTemp) == EOK) {
                iPos_ += iLenTemp;
            } else {
                HILOG_ERROR("SetCData memcpy_s failed");
            }
        }
    }
    void XmlSerializer::SetDocType(const std::string &text)
    {
        out_ = "";
        if (type == "isStart" || type == "isAttri") {
            SplicNsp();
            out_.append(">");
        }
        if (type != "") {
            NextItem();
        }
        out_ += "<!DOCTYPE " + text + ">";
        type = "isDocType";
        size_t iLenTemp = out_.length();
        if (iLength_ > iPos_ + iLenTemp - 1) {
            if (memcpy_s(pStart_ + iPos_, iLength_ - iPos_, out_.c_str(), iLenTemp) == EOK) {
                iPos_ += iLenTemp;
            } else {
                HILOG_ERROR("SetDocType memcpy_s failed");
            }
        }
    }

    void XmlSerializer::WriteEscaped(std::string s)
    {
        size_t len = s.length();
        for (size_t i = 0; i < len; ++i) {
            char c = s[i];
            switch (c) {
                case '\'':
                    out_.append("&apos;");
                    break;
                case '\"':
                    out_.append("&quot;");
                    break;
                case '&':
                    out_.append("&amp;");
                    break;
                case '>':
                    out_.append("&gt");
                    break;
                case '<':
                    out_.append("&lt;");
                    break;
                default:
                    out_ += c;
            }
        }
    }

    std::string XmlSerializer::XmlSerializerError()
    {
        return xmlSerializerError_;
    }

    napi_value XmlPullParser::DealOptionInfo(napi_env env, napi_value napiObj)
    {
        std::vector<std::string> vctOptions = {
            "supportDoctype", "ignoreNameSpace", "tagValueCallbackFunction",
            "attributeValueCallbackFunction", "tokenValueCallbackFunction"
        };
        size_t vctLength = vctOptions.size();
        for (size_t i = 0; i < vctLength; ++i) {
            napi_value recvTemp = nullptr;
            bool bRecv = false;
            napi_get_named_property(env, napiObj, vctOptions[i].c_str(), &recvTemp);
            napi_valuetype valuetype;
            NAPI_CALL(env, napi_typeof(env, recvTemp, &valuetype));
            if (valuetype == napi_boolean && (napi_get_value_bool(env, recvTemp, &bRecv)) == napi_ok) {
                switch (i) {
                    case 0: // 0:supportDoctype
                        bDoctype_ = bRecv;
                        break;
                    case 1: // 1:ignoreNameSpace
                        bIgnoreNS_ = bRecv;
                        break;
                    default:
                        break;
                }
            } else if (valuetype == napi_function) {
                NAPI_ASSERT(env, recvTemp != nullptr, "Parameter is empty.");
                switch (i) {
                    case 2: // 2:tagValueCallbackFunction
                        tagFunc_ = recvTemp;
                        break;
                    case 3: // 3:attributeValueCallbackFunction
                        attrFunc_ = recvTemp;
                        break;
                    case 4: // 4:tokenValueCallbackFunction
                        tokenFunc_ = recvTemp;
                        break;
                    default:
                        break;
                }
            }
        }
        return nullptr;
    }

    void XmlPullParser::PushSrcLinkList(std::string strBuffer)
    {
        auto pNew = new SrcLinkList;
        srcLinkList_->next = pNew;
        pNew->strBuffer = strBuffer;
        pNew->position = 0;
        pNew->max = strBuffer.size();
        srcLinkList_ = pNew;
    }

    void XmlPullParser::PopSrcLinkList()
    {
        SrcLinkList* pTemp = srcLinkList_;
        if (srcLinkList_) {
            srcLinkList_ = srcLinkList_->next;
        }
        if (pTemp != nullptr) {
            delete pTemp;
            pTemp = nullptr;
        }
    }

    bool XmlPullParser::DealLength(size_t minimum)
    {
        while (srcLinkList_->next != nullptr) {
            if (position_ < max_) {
                xmlPullParserError_ = "Unbalanced entity!";
            }
            PopSrcLinkList();
            if (max_ - position_ >= minimum) {
                return true;
            }
        }
        for (size_t i = 0; i < position_; i++) {
            if (strXml_[i] == '\n') {
                bufferStartLine_++;
                bufferStartColumn_ = 0;
            } else {
                bufferStartColumn_++;
            }
        }
        if (keyInfo_ != "") {
            keyInfo_.append(strXml_, 0, position_);
        }

        if (max_ != position_) {
            max_ -= position_;
            for (size_t j = 0; j < max_; ++j) {
                strXml_[j] = strXml_[position_ + j];
            }
        } else {
            max_ = 0;
        }
        if (position_ != strXml_.size()) {
            position_ = 0;
        }
        if (strXml_.size() - max_ > 0 && position_ == 0) {
            max_ += strXml_.size() - max_;
            if (max_ >= minimum) {
                return true;
            }
        }
        return false;
    }

    size_t XmlPullParser::GetNSCount(size_t iTemp)
    {
        if (iTemp > depth) {
            xmlPullParserError_ = " IndexOutOfBoundsException";
        }
        return nspCounts_[depth];
    }

    std::string XmlPullParser::XmlPullParserError() const
    {
        return xmlPullParserError_;
    }

    bool XmlPullParser::ParseToken(napi_env env, napi_value thisVar) const
    {
        napi_value returnVal = nullptr;
        size_t argc = 2; // 2: number of args
        napi_value key = nullptr;
        napi_create_int32(env, (int)type, &key);
        napi_value parseInfo  = nullptr;
        napi_create_object(env, &parseInfo);
        auto object = new ParseInfo();
        napi_wrap(env, parseInfo, object, nullptr, nullptr, nullptr);
        static napi_property_descriptor xmlDesc[] = {
            DECLARE_NAPI_FUNCTION("getDepth", XmlPullParser::ParseInfo::GetDepth),
            DECLARE_NAPI_FUNCTION("getColumnNumber", XmlPullParser::ParseInfo::GetColumnNumber),
            DECLARE_NAPI_FUNCTION("getLineNumber", XmlPullParser::ParseInfo::GetLineNumber),
            DECLARE_NAPI_FUNCTION("getAttributeCount", XmlPullParser::ParseInfo::GetAttributeCount),
            DECLARE_NAPI_FUNCTION("getName", XmlPullParser::ParseInfo::GetName),
            DECLARE_NAPI_FUNCTION("getNamespace", XmlPullParser::ParseInfo::GetNamespace),
            DECLARE_NAPI_FUNCTION("getPrefix", XmlPullParser::ParseInfo::GetPrefix),
            DECLARE_NAPI_FUNCTION("getText", XmlPullParser::ParseInfo::GetText),
            DECLARE_NAPI_FUNCTION("isEmptyElementTag", XmlPullParser::ParseInfo::IsEmptyElementTag),
            DECLARE_NAPI_FUNCTION("isWhitespace", XmlPullParser::ParseInfo::IsWhitespace)
        };
        napi_define_properties(env, parseInfo, sizeof(xmlDesc) / sizeof(xmlDesc[0]), xmlDesc);
        napi_set_named_property(env, parseInfo, "MainInfo", thisVar);
        napi_value argv[2] = {key, parseInfo}; // 2: number of args
        napi_call_function(env, parseInfo, tokenFunc_, argc, argv, &returnVal);
        bool bRec = false;
        napi_get_value_bool(env, returnVal, &bRec);
        if (object != nullptr) {
            delete object;
            object = nullptr;
        }
        return bRec;
    }

    bool XmlPullParser::ParseAttri(napi_env env, napi_value thisVar) const
    {
        for (size_t i = 0; i < attriCount_; ++i) {
            napi_value returnVal = nullptr;
            size_t argc = 3; // 3: number of args
            napi_value global = nullptr;
            napi_get_global(env, &global);
            napi_value key = nullptr;
            napi_create_string_utf8(env, attributes[i * 4 + 2].c_str(), // 4 and 2: number of args
                attributes[i * 4 + 2].size(), &key); // 4 and 2: number of args
            napi_value value = nullptr;
            napi_create_string_utf8(env, attributes[i * 4 + 3].c_str(), // 4 and 3: number of args
                attributes[i * 4 + 3].size(), &value); // 3 and 4: number of args
            napi_value argv[3] = {key, value, thisVar};
            napi_call_function(env, global, attrFunc_, argc, argv, &returnVal);
            bool bRec = false;
            napi_get_value_bool(env, returnVal, &bRec);
            if (!bRec) {
                return bRec;
            }
        }
        return true;
    }

    void XmlPullParser::Parse(napi_env env, napi_value thisVar)
    {
        if (tagFunc_ || attrFunc_ || tokenFunc_) {
            while (type != TagEnum::END_DOCUMENT) {
                ParseOneTag();
                bool bRec = false;
                if (tagFunc_) {
                    napi_value returnVal = nullptr;
                    size_t argc = 3; // 3: number of args
                    napi_value global = nullptr;
                    napi_get_global(env, &global);
                    napi_value key = nullptr;
                    napi_create_string_utf8(env, name_.c_str(), name_.size(), &key);
                    napi_value value = nullptr;
                    napi_create_string_utf8(env, text_.c_str(), text_.size(), &value);
                    napi_value argv[3] = {key, value, thisVar};
                    napi_call_function(env, global, tagFunc_, argc, argv, &returnVal);
                    napi_get_value_bool(env, returnVal, &bRec);
                }
                if (tagFunc_ && type == TagEnum::START_TAG && !bRec) {
                    break;
                }
                if (attrFunc_ && attriCount_) {
                    bRec = ParseAttri(env, thisVar);
                    attriCount_ = 0;
                }
                if (attrFunc_ && attriCount_ && !bRec) {
                    break;
                }
                if (tokenFunc_) {
                    bRec = ParseToken(env, thisVar);
                }
                if (tokenFunc_ && !bRec) {
                    break;
                }
            }
        }
    }

    TagEnum XmlPullParser::DealExclamationGroup()
    {
        switch (strXml_[position_ + 2]) { // 2:  number of args
            case 'D':
                return TagEnum::DOCDECL;
            case '[':
                return TagEnum::CDSECT;
            case '-':
                return TagEnum::COMMENT;
            case 'E':
                switch (strXml_[position_ + 3]) { // 3:  number of args
                    case 'L':
                        return TagEnum::ELEMENTDECL;
                    case 'N':
                        return TagEnum::ENTITYDECL;
                    default:
                        break;
                }
                xmlPullParserError_ = "Unexpected <!";
                break;
            case 'A':
                return TagEnum::ATTLISTDECL;
            case 'N':
                return TagEnum::NOTATIONDECL;
            default:
                break;
        }
        return TagEnum::ERROR1;
    }

    TagEnum XmlPullParser::DealLtGroup()
    {
        if (position_ + 3 >= max_ && !DealLength(4)) { // 4: number of args 3: number of args
            xmlPullParserError_ = ("Dangling <");
        }
        char cTemp = strXml_[position_ + 1];
        if (cTemp == '/') {
            return TagEnum::END_TAG;
        } else if (cTemp == '?') {
            std::string strXml = strXml_.substr(position_ + 2, 4);  // 2 and 4:position and length
            MakeStrUpper(strXml);
            if (max_ >= position_ + 5 && strXml == tagText_.XML) { // 5: number of args
                return TagEnum::XML_DECLARATION;
            } else {
                return TagEnum::INSTRUCTION;
            }
        } else if (cTemp == '!') {
            return DealExclamationGroup();
        } else {
            return TagEnum::START_TAG;
        }
    }

    TagEnum XmlPullParser::ParseTagType(bool inDeclaration)
    {
        if (bStartDoc_) {
            bStartDoc_ = false;
            return TagEnum::START_DOCUMENT;
        }
        if (position_ >= max_ && !DealLength(1)) {
            return TagEnum::END_DOCUMENT;
        }
        switch (strXml_[position_]) {
            case '&':
                return TagEnum::ENTITY_REFERENCE;
            case '<':
                return DealLtGroup();
            case '%':
                return inDeclaration ? TagEnum::PARAMETER_ENTITY_REF : TagEnum::TEXT;
            default:
                return TagEnum::TEXT;
        }
    }

    void XmlPullParser::MakeStrUpper(std::string &src) const
    {
        size_t i = 0;

        while (i < src.size()) {
            if (src[i] >= 'A' && src[i] <= 'Z') {
                src[i] += 32; // 32: number of args
            }
            ++i;
        }
    }

    void XmlPullParser::SkipText(std::string chars)
    {
        if (position_ + chars.size() > max_ && !DealLength(chars.size())) {
            xmlPullParserError_ = "expected: '" + chars + "' but was EOF";
            return;
        }
        size_t len = chars.length();
        if (strXml_.substr(position_, len) != chars) {
            xmlPullParserError_ = "expected: \"" + chars + "\" but was \"" + strXml_.substr(position_, len) + "...\"";
        }
        position_ += len;
    }

    int XmlPullParser::PriorDealChar()
    {
        if (position_ < max_ || DealLength(1)) {
            return strXml_[position_];
        }
        return -1;
    }
    void XmlPullParser::SkipChar(char expected)
    {
        int c = PriorDealChar();
        if (c != expected) {
            xmlPullParserError_ = "expected:";
            if (c == -1) {
                return;
            }
        }
        position_++;
    }

    std::string XmlPullParser::ParseNameInner(size_t start)
    {
        std::string result = "";
        char c = 0;
        while (true) {
            if (position_ >= max_) {
                result.append(strXml_, start, position_ - start);
                if (!DealLength(1)) {
                    return result;
                }
                start = position_;
            }
            c = strXml_[position_];
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                (c >= '0' && c <= '9') || c == '_' || c == '-' ||
                c == ':' || c == '.') {
                position_++;
                continue;
            }
            result.append(strXml_, start, position_ - start);
            return result;
        }
    }

    std::string XmlPullParser::ParseName()
    {
        if (position_ >= max_ && !DealLength(1)) {
            xmlPullParserError_ = "name expected";
            return "";
        }
        size_t start = position_;
        char c = strXml_[position_];
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
            c == '_' || c == ':' || relaxed) {
            position_++;
        } else {
            xmlPullParserError_ = "name expected";
            return "";
        }
        return ParseNameInner(start);
    }

    void XmlPullParser::SkipInvalidChar()
    {
        while (position_ < max_ || DealLength(1)) {
            unsigned char temp = strXml_[position_];
            if (temp > ' ') {
                break;
            }
            position_++;
        }
    }

    void XmlPullParser::ParseEntityFunc(size_t start, std::string &out, bool isEntityToken, TextEnum textEnum)
    {
        std::string strEntity = out.substr(start + 1, out.length() - 1);
        if (isEntityToken) {
            name_ = strEntity;
        }
        if (strEntity.size() && strEntity[0] == '#') {
            int c = 0;
            if (strEntity.size() >= 2 && strEntity[1] == 'x') { // 2: number of args
                c = std::stoi(strEntity.substr(2), nullptr, 16); // 16: number of args 2: number of args
            } else {
                c = std::stoi(strEntity.substr(1), nullptr);
            }
            out = "";
            out += static_cast<char>(c);
            bUnresolved_ = false;
            return;
        }
        if (textEnum == TextEnum::ENTITY_DECL) {
            return;
        }
        if (DEFAULT_ENTITIES.count(strEntity) != 0) {
            out = "";
            bUnresolved_ = false;
            out.append(strEntity);
            return;
        }
        std::string resolved = " ";
        if (documentEntities.size() != 0 && (resolved = strEntity) != "") {
            out = "";
            bUnresolved_ = false;
            if (bDocDecl) {
                PushSrcLinkList(resolved);
            } else {
                out.append(resolved);
            }
            return;
        }
        if (sysInfo_ != "") {
            out = "";
            return;
        }
        bUnresolved_ = true;
    }

    void XmlPullParser::ParseEntity(std::string &out, bool isEntityToken, bool throwOnResolveFailure, TextEnum textEnum)
    {
        size_t start = out.length();
        if (strXml_[position_++] != '&') {
            xmlPullParserError_ = "Should not be reached";
        }
        out += '&';
        while (true) {
            int c = PriorDealChar();
            if (c == ';') {
                out += ';';
                position_++;
                break;
            } else if (c >= 128 || // 128: number of args
                (c >= '0' && c <= '9') ||
                (c >= 'a' && c <= 'z') ||
                (c >= 'A' && c <= 'Z') ||
                c == '_' ||
                c == '-' ||
                c == '#') {
                position_++;
                out.push_back(static_cast<char>(c));
            } else if (relaxed) {
                return;
            } else {
                xmlPullParserError_ = "unterminated entity ref";
            }
        }
        ParseEntityFunc(start, out, isEntityToken, textEnum);
    }

    bool XmlPullParser::ParseTagValueFunc(char &c, bool bFlag, TextEnum textEnum,
        size_t &start, std::string &result)
    {
        if (c == '\r') {
            if ((position_ + 1 < max_ || DealLength(2)) && strXml_[position_ + 1] == '\n') { // 2: number of args
                position_++;
            }
            c = (textEnum == TextEnum::ATTRI) ? ' ' : '\n';
        } else if (c == '\n') {
            c = ' ';
        } else if (c == '&') {
            bWhitespace_ = false;
            ParseEntity(result, false, bFlag, textEnum);
            start = position_;
            return false;
        } else if (c == '<') {
            if (textEnum == TextEnum::ATTRI) {
                xmlPullParserError_ = "Illegal: \"<\" inside attribute value";
            }
            bWhitespace_ = false;
        } else if (c == ']') {
            if ((position_ + 2 < max_ || DealLength(3)) && // 2: number of args 3: number of args
                strXml_[position_ + 1] == ']' && strXml_[position_ + 2] == '>') { // 2: number of args
                xmlPullParserError_ = "Illegal: \"]]>\" outside CDATA section";
            }
            bWhitespace_ = false;
        } else if (c == '%') {
            xmlPullParserError_ = "This parser doesn't support parameter entities";
        } else {
            xmlPullParserError_ = "AssertionError";
        }
        return true;
    }

    void XmlPullParser::DealWhiteSpace(unsigned char c)
    {
        if (bWhitespace_ && c <= ' ') {
            bWhitespace_ = true;
        } else {
            bWhitespace_ = false;
        }
    }

    size_t XmlPullParser::ParseTagValueInner(size_t &start, std::string &result,
        char delimiter, TextEnum textEnum, bool bFlag)
    {
        if (position_ >= max_) {
            if (start < position_) {
                result.append(strXml_, start, position_ - start);
            }
            if (!DealLength(1)) {
                result = (result != "" ? result : "");
                return 0;
            }
            start = position_;
        }
        unsigned char c = strXml_[position_];
        if (c == delimiter ||
            (delimiter == ' ' && (c <= ' ' || c == '>')) ||
            (c == '&' && !bFlag)) {
                return 1;
            }
        if (c != '\r' && (c != '\n' || textEnum != TextEnum::ATTRI) &&
            c != '&' && c != '<' && (c != ']' || textEnum != TextEnum::TEXT)
            && (c != '%' || textEnum != TextEnum::ENTITY_DECL)) {
                DealWhiteSpace(c);
                position_++;
                return 2; // 2: break flag
        }
        result.append(strXml_, start, position_ - start);
        return c;
    }

    std::string XmlPullParser::ParseTagValue(char delimiter, bool resolveEntities,
        bool throwOnResolveFailure, TextEnum textEnum)
    {
        size_t start = position_;
        std::string result = "";
        if (textEnum == TextEnum::TEXT && text_ != "") {
            result.append(text_);
        }
        while (true) {
            char cRecv = static_cast<char>(ParseTagValueInner(start, result, delimiter, textEnum, resolveEntities));
            if (cRecv == 0) {
                return result;
            } else if (cRecv == 1) {
                break;
            } else if (cRecv == 2) { // 2: break flag
                continue;
            } else if (!ParseTagValueFunc(cRecv, throwOnResolveFailure, textEnum, start, result)) {
                continue;
            }
            ++position_;
            result = result + static_cast<char>(cRecv);
            start = position_;
        }
        result.append(strXml_, start, position_ - start);
        return result;
    }

    std::string XmlPullParser::GetNamespace(const std::string &prefix)
    {
        size_t temp = GetNSCount(depth) << 1;
        if (temp) {
            size_t i = temp - 2; // 2: number of args
            for (; i >= 0; i -= 2) { // 2: number of args
                if (prefix == "" && nspStack_[i] == "") {
                    return nspStack_[i + 1];
                } else if (prefix == nspStack_[i]) {
                    return nspStack_[i + 1];
                }
                if (!i) {
                    break;
                }
            }
        }
        return "";
    }

    void XmlPullParser::Replace(std::string& strTemp, std::string strSrc, std::string strDes) const
    {
        size_t iPos = 0;
        while ((iPos = strTemp.find(strSrc)) != std::string::npos) {
            strTemp.replace(iPos, strSrc.size(), strDes);
        }
    }
    void XmlPullParser::ParseNspFunc(size_t &i, const std::string &attrName, bool &any)
    {
        size_t j = (nspCounts_[depth]++) << 1;
        size_t uiSize = nspStack_.size();
        if (uiSize < j + 2) { // 2: number of args
            nspStack_.resize(j + 16); // 16: number of args
        }
        nspStack_[j] = attrName;
        nspStack_[j + 1] = attributes[i + 3]; // 3: number of args
        if (attrName != "" && attributes[i + 3].empty()) { // 3: number of args
            xmlPullParserError_ = "illegal empty namespace";
        }
        if (bKeepNsAttri) {
            attributes[i] = "http://www.w3.org/2000/xmlns/";
            any = true;
        } else {
            for (size_t iCount = i; iCount < ((--attriCount_) << 2); ++iCount) { // 2: number of args
                attributes[iCount] = attributes[iCount + 4]; // 4: number of args
            }
            i -= 4; // 4:
        }
    }
    void XmlPullParser::ParseNspFunction()
    {
        int i = (attriCount_ << 2) - 4; // 4: number of args 2: number of args
        for (; i >= 0; i -= 4) { // 4: number of args
            std::string attrName = attributes[i + 2]; // 2: number of args
            size_t cut = attrName.find(':');
            if (cut == 0 && !relaxed) {
                xmlPullParserError_ = "illegal attribute name: ";
            } else if (cut != std::string::npos) {
                std::string attrPrefix = attrName.substr(0, cut);
                attrName = attrName.substr(cut + 1);
                std::string attrNs = GetNamespace(attrPrefix);
                if (attrNs == "" && !relaxed) {
                    xmlPullParserError_ = ("Undefined Prefix: " + attrPrefix + " in ");
                }
                attributes[i] = attrNs;
                attributes[i + 1] = attrPrefix;
                attributes[i + 2] = attrName; // 2: number of args
            }
        }
    }
    bool XmlPullParser::ParseNsp()
    {
        bool any = false;
        size_t cut = 0;
        for (size_t i = 0; i < (attriCount_ << 2); i += 4) { // 2 and 4: number of args
            std::string attrName = attributes[i + 2]; // 2: number of args
            cut = attrName.find(':');
            std::string prefix;
            if (cut != std::string::npos) {
                prefix = attrName.substr(0, cut);
                attrName = attrName.substr(cut + 1);
            } else if (attrName == ("xmlns")) {
                prefix = attrName;
                attrName = "";
            } else {
                continue;
            }
            if (!(prefix == "xmlns")) {
                any = true;
            } else {
                ParseNspFunc(i, attrName, any);
            }
        }
        if (any) {
            ParseNspFunction();
        }
        cut = name_.find(':');
        if (cut == 0) {
            xmlPullParserError_ = "illegal tag name: " + name_;
        }
        if (cut != std::string::npos) {
            prefix_ = name_.substr(0, cut);
            name_ = name_.substr(cut + 1);
        }
        namespace_ = GetNamespace(prefix_);
        return any;
    }

    bool XmlPullParser::ParseStartTagFuncDeal(bool throwOnResolveFailure)
    {
        std::string attrName = ParseName();
        int i = (attriCount_++) * 4; // 4: number of args
        attributes.resize(attributes.size() + 4); // 4: number of args
        attributes[i] = "";
        attributes[i + 1] = "";
        attributes[i + 2] = attrName; // 2: number of args
        SkipInvalidChar();
        if (position_ >= max_ && !DealLength(1)) {
            xmlPullParserError_ = "UNEXPECTED_EOF";
            return false;
        }
        if (strXml_[position_] == '=') {
            position_++;
            SkipInvalidChar();
            if (position_ >= max_) {
                xmlPullParserError_ = "UNEXPECTED_EOF";
                return false;
            }
            char delimiter = strXml_[position_];
            if (delimiter == '\'' || delimiter == '"') {
                position_++;
            } else if (relaxed) {
                delimiter = ' ';
            } else {
                xmlPullParserError_ = "attr value delimiter missing!";
            }
            attributes[i + 3] = ParseTagValue(delimiter, true, // 3: number of args
                throwOnResolveFailure, TextEnum::ATTRI); // 3: number of args
            if (delimiter != ' ' && PriorDealChar() == delimiter) {
                position_++;
            }
        } else {
            attributes[i + 3] = attrName; // 3: number of args
        }
        return true;
    }

    bool XmlPullParser::ParseStartTagFunc(bool xmldecl, bool throwOnResolveFailure)
    {
        while (true) {
            SkipInvalidChar();
            if (position_ >= max_ && DealLength(1)) {
                xmlPullParserError_ = "UNEXPECTED_EOF";
                return false;
            }
            unsigned char temp = strXml_[position_];
            if (xmldecl) {
                if (temp == '?') {
                    position_++;
                    SkipChar('>');
                    return false;
                }
            } else {
                if (temp == '/') {
                    bEndFlag_ = true;
                    position_++;
                    SkipInvalidChar();
                    SkipChar('>');
                    break;
                } else if (temp == '>') {
                    position_++;
                    break;
                }
            }
            bool bRecv = ParseStartTagFuncDeal(throwOnResolveFailure);
            if (!bRecv) {
                return false;
            }
        }
        return true;
    }

    void XmlPullParser::ParseStartTag(bool xmldecl, bool throwOnResolveFailure)
    {
        if (!xmldecl) {
            SkipChar('<');
        }
        name_ = ParseName();
        attriCount_ = 0;
        bool bRecv = ParseStartTagFunc(xmldecl, throwOnResolveFailure);
        if (!bRecv) {
            return;
        }
        size_t sp = depth++ * 4; // 4: number of args
        elementStack_.resize(sp + 4); // 4: number of args
        elementStack_[sp + 3] = name_; // 3: number of args
        if (depth >= nspCounts_.size()) {
            nspCounts_.resize(depth + 4); // 4: number of args
        }
        nspCounts_[depth] = nspCounts_[depth - 1];
        if (!bIgnoreNS_) {
            ParseNsp();
        } else {
            namespace_ = "";
        }
        if (defaultAttributes.size() != 0) {
            std::map<std::string, std::string> elementDefaultAttributes = defaultAttributes[name_];
            if (elementDefaultAttributes.size() != 0) {
                for (std::map<std::string, std::string>::iterator iter = elementDefaultAttributes.begin();
                    iter != elementDefaultAttributes.end(); ++iter) {
                }
            }
        }
        elementStack_[sp] = namespace_;
        elementStack_[sp + 1] = prefix_;
        elementStack_[sp + 2] = name_; // 2: number of args
    }

    void XmlPullParser::ParseDeclaration()
    {
        if (bufferStartLine_ != 0 || bufferStartColumn_ != 0 || position_ != 0) {
            xmlPullParserError_ = "processing instructions must not start with xml";
        }
        SkipText(tagText_.START_PROCESSING_INSTRUCTION);
        ParseStartTag(true, true);
        if (attriCount_ < 1 || attributes[2] != "version") { // 2: number of args
            xmlPullParserError_ = "version expected";
        }
        version_ = attributes[3]; // 3: number of args
        size_t pos = 1;
        if (pos < attriCount_ && (attributes[2 + 4]) == "encoding") { // 4: number of args 2: number of args
            encoding_ = attributes[3 + 4]; // 3: number of args 4: number of args
            pos++;
        }
        if (pos < attriCount_ && (attributes[4 * pos + 2]) == "standalone") { // 4: number of args 2: number of args
            std::string st = attributes[3 + 4 * pos]; // 3: number of args 4: number of args
            if (st == "yes") {
                bAlone_ = true;
            } else if (st == "no") {
                bAlone_ = false;
            } else {
                xmlPullParserError_ = "illegal standalone value: " + st;
            }
            pos++;
        }
        if (pos != attriCount_) {
            xmlPullParserError_ = "unexpected attributes in XML declaration";
        }
        bWhitespace_ = true;
        text_ = "";
    }

    void XmlPullParser::ParseEndTag()
    {
        SkipChar('<');
        SkipChar('/');
        name_ = ParseName();
        SkipInvalidChar();
        SkipChar('>');
        if (depth == 0) {
            xmlPullParserError_ = "read end tag " + name_ + " with no tags open";
            type = TagEnum::COMMENT;
            return;
        }
        size_t sp = (depth - 1) * 4; // 4: number of args
        if (name_ == elementStack_[sp + 3]) { // 3: number of args
            namespace_ = elementStack_[sp];
            prefix_ = elementStack_[sp + 1];
            name_ = elementStack_[sp + 2]; // 2: number of args
        } else if (!relaxed) {
            xmlPullParserError_ = "expected: /" + elementStack_[sp + 3] + " read: " + name_; // 3: number of args
        }
    }

    std::string XmlPullParser::ParseDelimiterInfo(std::string delimiter, bool returnText)
    {
        size_t start = position_;
        std::string result = "";
        if (returnText && text_ != "") {
            result.append(text_);
        }
        bool bFlag = true;
        while (bFlag) {
            if (position_ + (delimiter).length() > max_) {
                if (start < position_ && returnText) {
                    result.append(strXml_, start, position_ - start);
                }
                if (!DealLength(delimiter.length())) {
                    type = TagEnum::COMMENT;
                    return "";
                }
                start = position_;
            }
            size_t i = 0;
            for (; i < delimiter.length(); i++) {
                if (strXml_[position_ + i] != delimiter[i]) {
                    position_++;
                    break;
                }
            }
            if (i == delimiter.length()) {
                bFlag = false;
            }
        }
        size_t end = position_;
        position_ += delimiter.length();
        if (!returnText) {
            return "";
        } else {
            result.append(strXml_, start, end - start);
            return result;
        }
    }
    std::string XmlPullParser::ParseDelimiter(bool returnText)
    {
        int quote = PriorDealChar();
        std::string delimiter;
        if (quote == '"') {
            delimiter = tagText_.DOUBLE_QUOTE;
        } else if (quote == '\'') {
            delimiter = tagText_.SINGLE_QUOTE;
        } else {
            xmlPullParserError_ = "Expected a quoted std::string ";
        }
        position_++;
        return ParseDelimiterInfo(delimiter, returnText);
    }

    bool XmlPullParser::ParserDoctInnerInfo(bool requireSystemName, bool assignFields)
    {
        SkipInvalidChar();
        int c = PriorDealChar();
        if (c == 'S') {
            SkipText(tagText_.SYSTEM);
        } else if (c == 'P') {
            SkipText(tagText_.PUBLIC);
            SkipInvalidChar();
            if (assignFields) {
                pubInfo_ = ParseDelimiter(true);
            } else {
                ParseDelimiter(false);
            }
        } else {
            return false;
        }
        SkipInvalidChar();
        if (!requireSystemName) {
            int delimiter = PriorDealChar();
            if (delimiter != '"' && delimiter != '\'') {
                return true; // no system name!
            }
        }
        if (assignFields) {
            sysInfo_ = ParseDelimiter(true);
        } else {
            ParseDelimiter(false);
        }
        return true;
    }

    void XmlPullParser::ParseComment(bool returnText)
    {
        SkipText(tagText_.START_COMMENT);
        if (relaxed) {
            std::string strTemp = ParseDelimiterInfo(tagText_.END_COMMENT, returnText);
            if (returnText) {
                text_ = strTemp;
            }
        }
        std::string commentText = ParseDelimiterInfo(tagText_.COMMENT_DOUBLE_DASH, returnText);
        if (PriorDealChar() != '>') {
            xmlPullParserError_ = "Comments may not contain -- ";
        }
        position_++;
        if (returnText) {
            text_ = commentText;
        }
    }
    void XmlPullParser::ParseSpecText()
    {
        SkipInvalidChar();
        int c = PriorDealChar();
        if (c == '(') {
            int iDepth = 0;
            do {
                if (c == '(') {
                    iDepth++;
                } else if (c == ')') {
                    iDepth--;
                } else if (c == -1) {
                    xmlPullParserError_ = "Unterminated element content spec ";
                }
                position_++;
                c = PriorDealChar();
            } while (iDepth > 0);
            if (c == '*' || c == '?' || c == '+') {
                position_++;
            }
        } else if (c == tagText_.EMPTY[0]) {
            SkipText(tagText_.EMPTY);
        } else if (c == tagText_.ANY[0]) {
            SkipText(tagText_.ANY);
        } else {
            xmlPullParserError_ = "Expected element content spec ";
        }
    }
    void XmlPullParser::ParseInnerEleDec()
    {
        SkipText(tagText_.START_ELEMENT);
        SkipInvalidChar();
        ParseName();
        ParseSpecText();
        SkipInvalidChar();
        SkipChar('>');
    }

    void XmlPullParser::ParseInnerAttriDeclFunc(int &c)
    {
        if (c == '(') {
            position_++;
            while (true) {
                SkipInvalidChar();
                ParseName();
                SkipInvalidChar();
                c = PriorDealChar();
                if (c == ')') {
                    position_++;
                    break;
                } else if (c == '|') {
                    position_++;
                } else {
                    xmlPullParserError_ = "Malformed attribute type ";
                }
            }
        } else {
            ParseName();
        }
    }

    void XmlPullParser::ParseInnerAttriDecl()
    {
        SkipText(tagText_.START_ATTLIST);
        SkipInvalidChar();
        std::string elementName = ParseName();
        while (true) {
            SkipInvalidChar();
            int c = PriorDealChar();
            if (c == '>') {
                position_++;
                return;
            }
            std::string attributeName = ParseName();
            SkipInvalidChar();
            if (position_ + 1 >= max_ && !DealLength(2)) { // 2: lengths
                xmlPullParserError_ = "Malformed attribute list ";
            }
            if (strXml_[position_] == tagText_.NOTATION[0] && strXml_[position_ + 1] == tagText_.NOTATION[1]) {
                SkipText(tagText_.NOTATION);
                SkipInvalidChar();
            }
            c = PriorDealChar();
            ParseInnerAttriDeclFunc(c);
            SkipInvalidChar();
            c = PriorDealChar();
            if (c == '#') {
                position_++;
                c = PriorDealChar();
                if (c == 'R') {
                    SkipText(tagText_.REQUIRED);
                } else if (c == 'I') {
                    SkipText(tagText_.IMPLIED);
                } else if (c == 'F') {
                    SkipText(tagText_.FIXED);
                } else {
                    xmlPullParserError_ = "Malformed attribute type";
                }
                SkipInvalidChar();
                c = PriorDealChar();
            }
            if (c == '"' || c == '\'') {
                position_++;
                std::string value = ParseTagValue(static_cast<char>(c), true, true, TextEnum::ATTRI);
                if (PriorDealChar() == c) {
                    position_++;
                }
            }
        }
    }

    void XmlPullParser::ParseEntityDecl()
    {
        SkipText(tagText_.START_ENTITY);
        bool generalEntity = true;
        SkipInvalidChar();
        if (PriorDealChar() == '%') {
            generalEntity = false;
            position_++;
            SkipInvalidChar();
        }
        std::string name = ParseName();
        SkipInvalidChar();
        int quote = PriorDealChar();
        std::string entityValue;
        if (quote == '"' || quote == '\'') {
            position_++;
            entityValue = ParseTagValue(static_cast<char>(quote), true, false, TextEnum::ENTITY_DECL);
            if (PriorDealChar() == quote) {
                position_++;
            }
        } else if (ParserDoctInnerInfo(true, false)) {
            entityValue = "";
            SkipInvalidChar();
            if (PriorDealChar() == tagText_.NDATA[0]) {
                SkipText(tagText_.NDATA);
                SkipInvalidChar();
                ParseName();
            }
        } else {
            xmlPullParserError_ = "Expected entity value or external ID";
        }
        if (generalEntity && bDocDecl) {
            documentEntities[name] = entityValue;
        }
        SkipInvalidChar();
        SkipChar('>');
    }
    void XmlPullParser::ParseInneNotaDecl()
    {
        SkipText(tagText_.START_NOTATION);
        SkipInvalidChar();
        ParseName();
        if (!ParserDoctInnerInfo(false, false)) {
            xmlPullParserError_ = "Expected external ID or public ID for notation ";
        }
        SkipInvalidChar();
        SkipChar('>');
    }
    void XmlPullParser::ReadInternalSubset()
    {
        SkipChar('[');
        while (true) {
            SkipInvalidChar();
            if (PriorDealChar() == ']') {
                position_++;
                return;
            }
            TagEnum declarationType = ParseTagType(true);
            switch (declarationType) {
                case TagEnum::ELEMENTDECL:
                    ParseInnerEleDec();
                    break;
                case TagEnum::ATTLISTDECL:
                    ParseInnerAttriDecl();
                    break;
                case TagEnum::ENTITYDECL:
                    ParseEntityDecl();
                    break;
                case TagEnum::NOTATIONDECL:
                    ParseInneNotaDecl();
                    break;
                case TagEnum::INSTRUCTION:
                    SkipText(tagText_.START_PROCESSING_INSTRUCTION);
                    ParseDelimiterInfo(tagText_.END_PROCESSING_INSTRUCTION, false);
                    break;
                case TagEnum::COMMENT:
                    ParseComment(false);
                    break;
                case TagEnum::PARAMETER_ENTITY_REF:
                    xmlPullParserError_ = "Parameter entity references are not supported ";
                    break;
                default:
                    xmlPullParserError_ = "Unexpected token";
                    break;
            }
        }
    }
    void XmlPullParser::ParseDoctype(bool saveDtdText)
    {
        SkipText(tagText_.START_DOCTYPE);
        size_t startPosition = 0;
        if (saveDtdText) {
            keyInfo_ = "";
            startPosition = position_;
        }
        SkipInvalidChar();
        ParseName();
        ParserDoctInnerInfo(true, true);
        SkipInvalidChar();
        if (PriorDealChar() == '[') {
            ReadInternalSubset();
        }
        SkipInvalidChar();
        if (saveDtdText) {
            keyInfo_.append(strXml_, 0, position_);
            keyInfo_ = keyInfo_.substr(startPosition);
            text_ = keyInfo_;
            keyInfo_ = "";
        }
        SkipChar('>');
    }

    void XmlPullParser::ParseText()
    {
        text_ = ParseTagValue('<', false, false, TextEnum::TEXT);
        std::string strTemp = text_;
        Replace(strTemp, "\r", "");
        Replace(strTemp, "\n", "");
        Replace(strTemp, " ", "");
        if ((depth == 0 && bWhitespace_) || strTemp.size() == 0) {
            type = TagEnum::WHITESPACE;
        }
    }

    void XmlPullParser::ParseCdect()
    {
        SkipText(tagText_.START_CDATA);
        text_ = ParseDelimiterInfo(tagText_.END_CDATA, true);
    }

    TagEnum XmlPullParser::ParseOneTagFunc()
    {
        switch (type) {
            case TagEnum::START_DOCUMENT:
                return type;
            case TagEnum::START_TAG: {
                ParseStartTag(false, false);
                return type; }
            case TagEnum::END_TAG: {
                ParseEndTag();
                return type; }
            case TagEnum::END_DOCUMENT:
                return type;
            case TagEnum::ENTITY_REFERENCE: {
                std::string entityTextBuilder;
                ParseEntity(entityTextBuilder, true, false, TextEnum::TEXT);
                text_ = entityTextBuilder;
                return TagEnum::OK;
                }
            case TagEnum::TEXT:
                ParseText();
                return TagEnum::OK;
            case TagEnum::CDSECT:
                ParseCdect();
                return TagEnum::OK;
            case TagEnum::COMMENT:
                ParseComment(true);
                return TagEnum::OK;
            case TagEnum::INSTRUCTION:
                ParseInstruction();
                return TagEnum::OK;
            case TagEnum::DOCDECL:
                ParseDoctype(true);
                return TagEnum::OK;
            default:
                xmlPullParserError_ = "Unexpected token";
                return TagEnum::ERROR1;
        }
    }

    TagEnum XmlPullParser::ParseOneTag()
    {
        if (type == TagEnum::END_TAG) {
            depth--;
        }
        if (bEndFlag_) {
            bEndFlag_ = false;
            type = TagEnum::END_TAG;
            return type;
        }
        ParserPriorDeal();
        while (true) {
            TagEnum typeTem = ParseOneTagFunc();
            if (typeTem == TagEnum::ERROR1) {
                return TagEnum::ERROR1;
            } else if (typeTem != TagEnum::OK) {
                return typeTem;
            }
            if (depth == 0 && (type == TagEnum::ENTITY_REFERENCE || type == TagEnum::TEXT || type == TagEnum::CDSECT)) {
                xmlPullParserError_ = "Unexpected token";
            }
            if (type == TagEnum::DOCDECL && (!bDoctype_)) {
                ParseOneTag();
            }
            return type;
        }
    }

    void XmlPullParser::ParserPriorDeal()
    {
        type = ParseTagType(false);
        if (type == TagEnum::XML_DECLARATION) {
            ParseDeclaration();
            type = ParseTagType(false);
        }
        text_ = "";
        bWhitespace_ = true;
        prefix_ = "";
        name_ = "";
        namespace_ = "";
        attriCount_ = 0;
    }

    void XmlPullParser::ParseInstruction()
    {
        SkipText(tagText_.START_PROCESSING_INSTRUCTION);
        text_ = ParseDelimiterInfo(tagText_.END_PROCESSING_INSTRUCTION, true);
    }

    int XmlPullParser::GetColumnNumber() const
    {
        size_t result = bufferStartColumn_;
        for (size_t i = 0; i < position_; i++) {
            if (strXml_[i] == '\n') {
                result = 0;
            } else {
                result++;
            }
        }
        return result + 1;
    }
    int XmlPullParser::GetDepth() const
    {
        return depth;
    }
    int XmlPullParser::GetLineNumber() const
    {
        int result = bufferStartLine_;
        for (size_t i = 0; i < position_; i++) {
            if (strXml_[i] == '\n') {
                result++;
            }
        }
        return result + 1;
    }
    std::string XmlPullParser::GetName() const
    {
        return name_;
    }

    std::string XmlPullParser::GetPrefix() const
    {
        return prefix_;
    }
    std::string XmlPullParser::GetText() const
    {
        if (type < TagEnum::TEXT || (type == TagEnum::ENTITY_REFERENCE && bUnresolved_)) {
            return "";
        } else {
            return text_;
        }
    }
    bool XmlPullParser::IsEmptyElementTag() const
    {
        return bEndFlag_;
    }

    int XmlPullParser::GetAttributeCount() const
    {
        return attriCount_;
    }

    bool XmlPullParser::IsWhitespace() const
    {
        return bWhitespace_;
    }

    std::string XmlPullParser::GetNamespace() const
    {
        return namespace_;
    }

    napi_value XmlPullParser::ParseInfo::GetColumnNumber(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value mainVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        void *obj = nullptr;
        napi_get_named_property(env, thisVar, "MainInfo", &mainVar);
        NAPI_CALL(env, napi_unwrap(env, mainVar, &obj));
        napi_value result = nullptr;
        if (obj != nullptr) {
            int temp = (reinterpret_cast<XmlPullParser *>(obj))->GetColumnNumber();
            napi_create_int32(env, temp, &result);
        }
        return result;
    }

    napi_value XmlPullParser::ParseInfo::GetDepth(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value mainVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        void *obj = nullptr;
        napi_get_named_property(env, thisVar, "MainInfo", &mainVar);
        NAPI_CALL(env, napi_unwrap(env, mainVar, &obj));
        napi_value result = nullptr;
        if (obj != nullptr) {
            int temp = (reinterpret_cast<XmlPullParser *>(obj))->GetDepth();
            napi_create_int32(env, temp, &result);
        }
        return result;
    }

    napi_value XmlPullParser::ParseInfo::GetLineNumber(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value mainVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        void *obj = nullptr;
        napi_get_named_property(env, thisVar, "MainInfo", &mainVar);
        NAPI_CALL(env, napi_unwrap(env, mainVar, &obj));
        napi_value result = nullptr;
        if (obj != nullptr) {
            int temp = (reinterpret_cast<XmlPullParser *>(obj))->GetLineNumber();
            napi_create_int32(env, temp, &result);
        }
        return result;
    }

    napi_value XmlPullParser::ParseInfo::GetName(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value mainVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        void *obj = nullptr;
        napi_get_named_property(env, thisVar, "MainInfo", &mainVar);
        NAPI_CALL(env, napi_unwrap(env, mainVar, &obj));
        napi_value result = nullptr;
        if (obj != nullptr) {
            std::string temp = (reinterpret_cast<XmlPullParser *>(obj))->GetName();
            napi_create_string_utf8(env, temp.c_str(), temp.length(), &result);
        }
        return result;
    }

    napi_value XmlPullParser::ParseInfo::GetNamespace(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value mainVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        void *obj = nullptr;
        napi_get_named_property(env, thisVar, "MainInfo", &mainVar);
        NAPI_CALL(env, napi_unwrap(env, mainVar, &obj));
        napi_value result = nullptr;
        if (obj != nullptr) {
            std::string temp = (reinterpret_cast<XmlPullParser *>(obj))->GetNamespace();
            napi_create_string_utf8(env, temp.c_str(), temp.length(), &result);
        }
        return result;
    }

    napi_value XmlPullParser::ParseInfo::GetPrefix(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value mainVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        void *obj = nullptr;
        napi_get_named_property(env, thisVar, "MainInfo", &mainVar);
        NAPI_CALL(env, napi_unwrap(env, mainVar, &obj));
        napi_value result = nullptr;
        if (obj != nullptr) {
            std::string temp = (reinterpret_cast<XmlPullParser *>(obj))->GetPrefix();
            napi_create_string_utf8(env, temp.c_str(), temp.length(), &result);
        }
        return result;
    }

    napi_value XmlPullParser::ParseInfo::GetText(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value mainVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        void *obj = nullptr;
        napi_get_named_property(env, thisVar, "MainInfo", &mainVar);
        NAPI_CALL(env, napi_unwrap(env, mainVar, &obj));
        napi_value result = nullptr;
        if (obj != nullptr) {
            std::string temp = (reinterpret_cast<XmlPullParser *>(obj))->GetText();
            napi_create_string_utf8(env, temp.c_str(), temp.length(), &result);
        }
        return result;
    }

    napi_value XmlPullParser::ParseInfo::IsEmptyElementTag(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value mainVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        void *obj = nullptr;
        napi_get_named_property(env, thisVar, "MainInfo", &mainVar);
        NAPI_CALL(env, napi_unwrap(env, mainVar, &obj));
        napi_value result = nullptr;
        if (obj != nullptr) {
            bool temp = (reinterpret_cast<XmlPullParser *>(obj))->IsEmptyElementTag();
            napi_get_boolean(env, temp, &result);
        }
        return result;
    }

    napi_value XmlPullParser::ParseInfo::IsWhitespace(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value mainVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        void *obj = nullptr;
        napi_get_named_property(env, thisVar, "MainInfo", &mainVar);
        NAPI_CALL(env, napi_unwrap(env, mainVar, &obj));
        napi_value result = nullptr;
        if (obj != nullptr) {
            bool temp = (reinterpret_cast<XmlPullParser *>(obj))->IsWhitespace();
            napi_get_boolean(env, temp, &result);
        }
        return result;
    }

    napi_value XmlPullParser::ParseInfo::GetAttributeCount(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value mainVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
        void *obj = nullptr;
        napi_get_named_property(env, thisVar, "MainInfo", &mainVar);
        NAPI_CALL(env, napi_unwrap(env, mainVar, &obj));
        napi_value result = nullptr;
        if (obj != nullptr) {
            int temp = (reinterpret_cast<XmlPullParser *>(obj))->GetAttributeCount();
            napi_create_int32(env, temp, &result);
        }
        return result;
    }
} // namespace OHOS::Xml

