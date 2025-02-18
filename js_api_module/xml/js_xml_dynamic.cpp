/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "js_xml_dynamic.h"
#include "securec.h"
#include "tools/ets_error.h"

using namespace OHOS::Tools;
namespace OHOS::xml {
    napi_status XmlDynamicSerializer::DealNapiStrValue(napi_env env, const napi_value napiStr, std::string &result)
    {
        std::string buffer = "";
        size_t bufferSize = 0;
        napi_status status = napi_get_value_string_utf8(env, napiStr, nullptr, -1, &bufferSize);
        if (status != napi_ok) {
            HILOG_ERROR("XmlDynamicSerializer:: can not get buffer size");
            return status;
        }
        buffer.reserve(bufferSize + 1);
        buffer.resize(bufferSize);
        if (bufferSize > 0) {
            status = napi_get_value_string_utf8(env, napiStr, buffer.data(), bufferSize + 1, &bufferSize);
            if (status != napi_ok) {
                HILOG_ERROR("XmlDynamicSerializer:: can not get buffer value");
                return status;
            }
        }
        if (buffer.data() != nullptr) {
            result = buffer;
        }
        return status;
    }

    void XmlDynamicSerializer::SplicNsp()
    {
        elementStack_[depth_ * 3] = elementStack_[(depth_ - 1) * 3]; // 3: number of args
        elementStack_[depth_ * 3 + 1] = elementStack_[(depth_ - 1) * 3 + 1]; // 3: number of args
        if (multNsp_[depth_ - 1].size() == 0) {
            return;
        }
        if (type_ == "isAttri" || type_ == "isStart") {
            for (int i = 0; i < curNspNum; ++i) {
                out_.append(" xmlns:");
                out_.append(multNsp_[depth_ - 1][i * 2]); // 2: number of args
                out_.append("=\"");
                out_.append(multNsp_[depth_ - 1][i * 2 + 1]); // 2: number of args
                out_.append("\"");
            }
            multNsp_[depth_ - 1].clear();
            curNspNum = 0;
        }
    }

    void XmlDynamicSerializer::NextItem()
    {
        out_.append("\r\n");
        for (size_t i = 0; i < depth_; i++) {
            out_.append("  ");
        }
    }

    std::string XmlDynamicSerializer::Replace(std::string str, const std::string &subStr, const std::string &repStr)
    {
        size_t iPos = 0;
        size_t subLen = subStr.length();
        size_t step = repStr.length();
        while ((iPos = str.find(subStr, iPos)) != std::string::npos) {
            str = str.substr(0, iPos) + repStr + str.substr(iPos + subLen);
            iPos += step;
        }
        return str;
    }

    void XmlDynamicSerializer::BufferCopy()
    {
        if (strXml_.length() + out_.length() <= MAX_XML_LENGTH) {
            strXml_.append(out_);
        } else {
            errorCode_ = ErrorCodeEnum::BUFFER_OVERFLOW;
            xmlSerializerError_ = "The length has exceeded the upper limit";
            ErrorHelper::ThrowError(env_, errorCode_, xmlSerializerError_.c_str());
        }
    }

    void XmlDynamicSerializer::SetDeclaration()
    {
        if (strXml_.length() > 0) {
            xmlSerializerError_ = "illegal position for xml";
            errorCode_ = ErrorCodeEnum::ILLEGAL_POSITION;
            ErrorHelper::ThrowError(env_, errorCode_, xmlSerializerError_.c_str());
            return;
        }
        out_ = "";
        out_.append("<?xml version=\"1.0\"");
        out_.append(" encoding=\"");
        out_.append(encoding_);
        out_.append("\"?>");
        type_ = "isDecl";
        BufferCopy();
    }

    void XmlDynamicSerializer::SetNamespace(std::string prefix, const std::string &nsTemp)
    {
        out_ = "";
        if (type_ == "isStart" || type_ == "isAttri") {
            SplicNsp();
            out_.append(">");
        }
        elementStack_[depth_ * 3] = prefix; // 3: number of args
        elementStack_[depth_ * 3 + 1] = nsTemp; // 3: number of args
        multNsp_[depth_][curNspNum * 2] = elementStack_[depth_ * 3]; // 3: number of args 2: number of args
        multNsp_[depth_][curNspNum * 2 + 1] = elementStack_[depth_ * 3 + 1]; // 3: number of args 2: number of args
        ++curNspNum;
        type_ = "isNsp";
        BufferCopy();
    }

    void XmlDynamicSerializer::StartElement(const std::string &name)
    {
        out_ = "";
        if (type_ == "isStart" || type_ == "isAttri") {
            SplicNsp();
            out_.append(">");
        }
        if (type_ != "") {
            NextItem();
        }
        elementStack_[depth_ * 3 + 2] = name; // 3: number of args 2: number of args
        out_.append("<");
        if (elementStack_[depth_ * 3] != "") { // 3: number of args
            out_.append(elementStack_[depth_ * 3]); // 3: number of args
            out_.append(":");
        } else if (depth_ != 0) {
            if (elementStack_[(depth_ - 1) * 3] != "") { // 3: number of args
                elementStack_[depth_ * 3] = elementStack_[(depth_ - 1) * 3]; // 3: number of args
                out_.append(elementStack_[depth_ * 3]); // 3: number of args
                out_.append(":");
            }
        }
        out_.append(elementStack_[depth_ * 3 + 2]); // 3: number of args 2: number of args
        type_ = "isStart";
        ++depth_;
        elementNum_++;
        elementStack_.push_back("");
        elementStack_.push_back("");
        elementStack_.push_back("");
        BufferCopy();
    }

    void XmlDynamicSerializer::SetAttributes(const std::string &name, const std::string &value)
    {
        out_ = "";
        if (type_ != "isStart" && type_ != "isAttri") {
            xmlSerializerError_ = "illegal position for xml";
            errorCode_ = ErrorCodeEnum::ILLEGAL_POSITION;
            ErrorHelper::ThrowError(env_, errorCode_, xmlSerializerError_.c_str());
            return;
        }
        out_.append(" ");
        out_.append(name);
        out_.append("=\"");
        WriteEscaped(value);
        out_.append("\"");
        type_ = "isAttri";
        BufferCopy();
    }

    void XmlDynamicSerializer::EndElement()
    {
        if (elementNum_ < 1) {
            xmlSerializerError_ = "There is no match between the startElement and the endElement";
            errorCode_ = ErrorCodeEnum::NO_ELEMENT_MATCH;
            ErrorHelper::ThrowError(env_, errorCode_, xmlSerializerError_.c_str());
            return;
        }
        elementNum_--;
        out_ = "";
        if (type_ == "isStart" || type_ == "isAttri") {
            SplicNsp();
            out_.append("/>");
            type_ = "isEndTag";
            --depth_;
            BufferCopy();
            return;
        }
        --depth_;
        if (type_ != "isText") {
            NextItem();
        }
        out_.append("</");
        if (elementStack_[depth_ * 3] != "") { // 3: number of args
            out_.append(elementStack_[depth_ * 3]); // 3: number of args
            out_.append(":");
        }
        out_.append(elementStack_[depth_ * 3 + 2]); // 3: number of args 2: number of args
        elementStack_[depth_ * 3] = ""; // 3: number of args
        elementStack_[depth_ * 3 + 1] = ""; // 3: number of args
        type_ = "isEndTag";
        out_.append(">");
        BufferCopy();
    }

    void XmlDynamicSerializer::AddEmptyElement(std::string name)
    {
        out_ = "";
        if (type_ == "isStart" || type_ == "isAttri") {
            SplicNsp();
            out_.append(">");
        }
        if (type_ != "") {
            NextItem();
        }
        out_.append("<");
        out_.append(name);
        out_.append("/>");
        type_ = "isAddEmpElem";
        BufferCopy();
    }

    void XmlDynamicSerializer::SetText(const std::string &text)
    {
        out_ = "";
        if (type_ == "isStart" || type_ == "isAttri") {
            SplicNsp();
            out_.append(">");
        }
        WriteEscaped(text);
        type_ = "isText";
        BufferCopy();
    }

    void XmlDynamicSerializer::SetComment(const std::string &comment)
    {
        out_ = "";
        if (type_ == "isStart" || type_ == "isAttri") {
            SplicNsp();
            out_.append(">");
        }
        if (type_ != "") {
            NextItem();
        }
        out_ += "<!--" + comment + "-->";
        type_ = "isCom";
        BufferCopy();
    }

    void XmlDynamicSerializer::SetCData(std::string data)
    {
        out_ = "";
        if (type_ == "isStart" || type_ == "isAttri") {
            SplicNsp();
            out_.append(">");
        }
        if (type_ != "") {
            NextItem();
        }
        data = Replace(data, "]]>", "]]]]><![CDATA[>");
        out_ += "<![CDATA[" + data + "]]>";
        type_ = "isCData";
        BufferCopy();
    }

    void XmlDynamicSerializer::SetDocType(const std::string &text)
    {
        out_ = "";
        if (type_ == "isStart" || type_ == "isAttri") {
            SplicNsp();
            out_.append(">");
        }
        if (type_ != "") {
            NextItem();
        }
        out_ += "<!DOCTYPE " + text + ">";
        type_ = "isDocType";
        BufferCopy();
    }

    void XmlDynamicSerializer::WriteEscaped(std::string s)
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
                    out_.append("&gt;");
                    break;
                case '<':
                    out_.append("&lt;");
                    break;
                default:
                    out_ += c;
            }
        }
    }

    size_t XmlDynamicSerializer::GetXmlBufferLength()
    {
        return strXml_.length();
    }

    bool XmlDynamicSerializer::GetXmlBuffer(void *data, uint32_t length)
    {
        if (data == nullptr) {
            HILOG_ERROR("XmlDynamicSerializer:: GetXmlBuffer data is NULL");
            return false;
        }
        if (memcpy_s(data, length, reinterpret_cast<const void*>(strXml_.data()), strXml_.length()) != EOK) {
            HILOG_ERROR("XmlDynamicSerializer:: GetXmlBuffer copy xml buffer error");
            return false;
        }
        return true;
    }
} // namespace OHOS::Xml