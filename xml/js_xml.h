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

#ifndef XML_JS_XML_H_
#define XML_JS_XML_H_

#include <algorithm>
#include <cstring>
#include <map>
#include <string>
#include <vector>
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "utils/log.h"

namespace OHOS::xml {
    class XmlSerializer {
    public:
        /**
         * Constructor for XmlSerializer.
         *
         * @param pStart is the pointer.
         * @param bufferLengthis the length of the ArrayBuffer or
         * DataView memory used to receive the written xml information.
         */
        XmlSerializer(char *pStart, size_t bufferLength, const std::string &encoding = "utf-8") :pStart_(pStart),
            iLength_(bufferLength), encoding_(encoding) {};

        /**
         * XmlSerializer destructor.
         */
        ~XmlSerializer() {}

        /**
         * Set the Attributes method.
         *
         * @param name The parameter is the key value of the property.
         * @param value The parameter is the value of the property.
         */
        void SetAttributes(const std::string &name, const std::string &value);

        /**
         * Writes an empty element.
         *
         * @param name The parameter is the element name of the empty element.
         */
        void AddEmptyElement(std::string name);

        /**
         * Set the Declaration method.
         */
        void SetDeclaration();

        /**
         * Writes the element start tag with the given name.
         *
         * @param name The parameter is the element name of the current element.
         */
        void StartElement(const std::string &name);

        /**
         * Write element end tag.
         */
        void EndElement();

        /**
         * The namespace into which the current element tag is written.
         *
         * @param prefix The parameter is the prefix of the current element and its children.
         * @param nsTemp The parameter is the namespace of the current element and its children.
         */
        void SetNamespace(std::string prefix, const std::string &nsTemp);

        /**
         * Write the comment property.
         *
         * @param comment The parameter is the comment content of the current element.
         */
        void SetComment(const std::string &comment);

        /**
         * Write CDATA attributes.
         *
         * @param comment The parameter is the content of the CDATA attribute.
         */
        void SetCData(std::string data);

        /**
         * Write CDATA attributes.
         *
         * @param comment The parameter is the content of the CDATA attribute.
         */
        void SetText(const std::string &text);

        /**
         * Write DocType property.
         *
         * @param text The parameter is the content of the DocType property.
         */
        void SetDocType(const std::string &text);

        /**
         * write an escape.
         *
         * @param s The parameter is the passed in escaped string.
         */
        void WriteEscaped(std::string s);

        /**
         * SplicNsp functio.
         */
        void SplicNsp();

        /**
         * NextItem function.
         */
        void NextItem();

        /**
         * Throw exception function.
         */
        std::string XmlSerializerError();

        /**
         * Process the value of the string passed by napi.
         *
         * @param env The parameter is NAPI environment variables.
         * @param napiStr The parameter is pass parameters.
         * @param result The parameter is return the processed value.
         */
        static napi_status DealNapiStrValue(napi_env env, const napi_value napiStr, std::string &result);

    private:
        char *pStart_;
        size_t iPos_ = 0;
        size_t iLength_;
        std::string xmlSerializerError_;
        std::string encoding_;
        size_t depth_ = 0;
        std::string type;
        std::vector<std::string> elementStack = { "", "", ""};
        std::map<int, std::map<int, std::string>> multNsp;
        int CurNspNum = 0;
        std::string out_;
        bool isHasDecl = false;
    };

    enum class TagEnum {
        XML_DECLARATION = -1,
        START_DOCUMENT,
        END_DOCUMENT,
        START_TAG,
        END_TAG,
        TEXT,
        CDSECT,
        COMMENT,
        DOCDECL,
        INSTRUCTION,
        ENTITY_REFERENCE,
        WHITESPACE,
        ELEMENTDECL,
        ENTITYDECL,
        ATTLISTDECL,
        NOTATIONDECL,
        PARAMETER_ENTITY_REF,
        OK,
        ERROR1
    };

    enum class TextEnum {
        ATTRI,
        TEXT,
        ENTITY_DECL
    };
    class XmlPullParser {
    public:
        class ParseInfo {
        public:
            /**
             * Get the current depth of the element.
             * @param env The parameter is NAPI environment variables.
             * @param info The parameter is the current depth of the returned element.
             */
            static napi_value GetDepth(napi_env env, napi_callback_info info);

            /**
             * Get the current column number, starting at 1.
             * @param env The parameter is NAPI environment variables.
             * @param info The parameter is to return the current line number.
             */
            static napi_value GetColumnNumber(napi_env env, napi_callback_info info);

            /**
             * Get the current line number, starting at 1.
             * @param env The parameter is NAPI environment variables.
             * @param info The parameter is to return the current column number.
             */
            static napi_value GetLineNumber(napi_env env, napi_callback_info info);

            /**
             * Get the number of attributes of the current start tag.
             * @param env The parameter is NAPI environment variables.
             * @param info The parameter is the number of attributes of the current start tag.
             */
            static napi_value GetAttributeCount(napi_env env, napi_callback_info info);

            /**
             * Get the current element name.
             * @param env The parameter is NAPI environment variables.
             * @param info The parameter is to return the current element name.
             */
            static napi_value GetName(napi_env env, napi_callback_info info);

            /**
             * Get the namespace of the current element.
             * @param env The parameter is NAPI environment variables.
             * @param info The parameter is the namespace that returns the current element.
             */
            static napi_value GetNamespace(napi_env env, napi_callback_info info);

            /**
             * Get the current element prefix.
             * @param env The parameter is NAPI environment variables.
             * @param info The parameter is to return the current element prefix.
             */
            static napi_value GetPrefix(napi_env env, napi_callback_info info);

            /**
             * Get the text content of the current event.
             * @param env The parameter is NAPI environment variables.
             * @param info The parameter is to return the text content of the current event.
             */
            static napi_value GetText(napi_env env, napi_callback_info info);

            /**
             * Check whether the current element is an empty element.
             * @param env The parameter is NAPI environment variables.
             * @param info The parameter is to returns true The current element is an empty element.
             */
            static napi_value IsEmptyElementTag(napi_env env, napi_callback_info info);

            /**
             * Determines whether the current text event contains only space characters.
             * @param env The parameter is NAPI environment variables.
             * @param info The parameter is to returns true, the current text event contains only space characters.
             */
            static napi_value IsWhitespace(napi_env env, napi_callback_info info);
        };
        struct TagText {
            const std::string START_CDATA = "<![CDATA[";
            const std::string END_CDATA = "]]>";
            const std::string START_COMMENT = "<!--";
            const std::string END_COMMENT = "-->";
            const std::string COMMENT_DOUBLE_DASH = "--";
            const std::string END_PROCESSING_INSTRUCTION = "?>";
            const std::string START_DOCTYPE = "<!DOCTYPE";
            const std::string SYSTEM = "SYSTEM";
            const std::string PUBLIC = "PUBLIC";
            const std::string DOUBLE_QUOTE = "\"";
            const std::string SINGLE_QUOTE = "\\";
            const std::string START_ELEMENT = "<!ELEMENT";
            const std::string EMPTY = "EMPTY";
            const std::string ANY = "ANY";
            const std::string START_ATTLIST = "<!ATTLIST";
            const std::string NOTATION = "NOTATION";
            const std::string REQUIRED = "REQUIRED";
            const std::string IMPLIED = "IMPLIED";
            const std::string FIXED = "FIXED";
            const std::string START_ENTITY = "<!ENTITY";
            const std::string NDATA = "NDATA";
            const std::string START_NOTATION = "<!NOTATION";
            const std::string ILLEGAL_TYPE = "Wrong event type";
            const std::string START_PROCESSING_INSTRUCTION = "<?";
            const std::string XML = "xml ";
        };
        struct SrcLinkList {
            SrcLinkList* next;
            std::string strBuffer;
            int position;
            size_t max;
            SrcLinkList()
            {
                this->next = nullptr;
                this->strBuffer = "";
                this->position = -1;
                this->max = -1;
            };
            SrcLinkList(SrcLinkList* pNext, const std::string &strTemp, int iPos, int iMax) :next(pNext),
                strBuffer(strTemp), position(iPos), max(iMax) {}
        };
        XmlPullParser(const std::string &strXml, const std::string &encoding) : strXml_(strXml),
            encoding_(encoding) {};
        ~XmlPullParser()
        {
            while (srcLinkList_) {
                PopSrcLinkList();
            }
        };
        int GetDepth() const;
        int GetColumnNumber() const;
        int GetLineNumber() const;
        int GetAttributeCount() const;
        std::string GetName() const;
        std::string GetNamespace() const;
        std::string GetPrefix() const;
        std::string GetText() const;
        bool IsEmptyElementTag() const;
        bool IsWhitespace() const;
        void PushSrcLinkList(std::string strBuffer);
        void PopSrcLinkList();
        bool DealLength(size_t minimum);
        void Replace(std::string &strTemp, std::string strSrc, std::string strDes) const;
        size_t GetNSCount(size_t iTemp);
        void Parse(napi_env env, napi_value thisVar);
        std::string GetNamespace(const std::string &prefix);
        napi_value DealOptionInfo(napi_env env, napi_value napiObj);
        TagEnum ParseTagType(bool inDeclaration);
        void SkipText(std::string chars);
        int PriorDealChar();
        void SkipChar(char expected);
        std::string ParseNameInner(size_t start);
        std::string ParseName();
        void SkipInvalidChar();
        void ParseEntity(std::string& out, bool isEntityToken, bool throwOnResolveFailure, TextEnum textEnum);
        std::string ParseTagValue(char delimiter, bool resolveEntities, bool throwOnResolveFailure, TextEnum textEnum);
        bool ParseNsp();
        void ParseStartTag(bool xmldecl, bool throwOnResolveFailure);
        void ParseDeclaration();
        void ParseEndTag();
        std::string ParseDelimiterInfo(std::string delimiter, bool returnText);
        std::string ParseDelimiter(bool returnText);
        bool ParserDoctInnerInfo(bool requireSystemName, bool assignFields);
        void ParseComment(bool returnText);
        void ParseSpecText();
        void ParseInnerEleDec();
        void ParseInnerAttriDecl();
        void ParseEntityDecl();
        void ParseInneNotaDecl();
        void ReadInternalSubset();
        void ParseDoctype(bool saveDtdText);
        TagEnum ParseOneTag();
        void ParserPriorDeal();
        void ParseInstruction();
        void ParseText();
        void ParseCdect();
        std::string XmlPullParserError() const;
        bool ParseAttri(napi_env env, napi_value thisVar) const;
        bool ParseToken(napi_env env, napi_value thisVar) const;
        void ParseNspFunction();
        void ParseNspFunc(size_t &i, const std::string &attrName, bool &any);
        void ParseInnerAttriDeclFunc(int &c);
        TagEnum DealExclamationGroup();
        void ParseEntityFunc(size_t start, std::string &out, bool isEntityToken, TextEnum textEnum);
        bool ParseStartTagFuncDeal(bool throwOnResolveFailure);
        bool ParseStartTagFunc(bool xmldecl, bool throwOnResolveFailure);
        TagEnum ParseOneTagFunc();
        size_t ParseTagValueInner(size_t &start, std::string &result, char delimiter, TextEnum textEnum, bool bFlag);
        bool ParseTagValueFunc(char &c, bool bFlag, TextEnum textEnum, size_t &start, std::string &result);
        void MakeStrUpper(std::string &src) const;
        TagEnum DealLtGroup();
        void DealWhiteSpace(unsigned char c);

    private:
        bool bDoctype_ = false;
        bool bIgnoreNS_ = false;
        bool bStartDoc_ = true;
        napi_value tagFunc_ = nullptr;
        napi_value attrFunc_ = nullptr;
        napi_value tokenFunc_ = nullptr;
        TagText tagText_;
        std::string strXml_ = "";
        std::string version_ = "";
        std::string encoding_ = "";
        std::string prefix_ = "";
        std::string namespace_ = "";
        std::string name_ = "";
        std::string text_ = "";
        std::string sysInfo_ = "";
        std::string pubInfo_ = "";
        std::string keyInfo_ = "";
        std::string xmlPullParserError_ = "";
        std::vector<size_t> nspCounts_;
        std::vector<std::string> nspStack_;
        std::vector<std::string> elementStack_;
        std::vector<std::string> attributes;
        std::map<std::string, std::string> documentEntities;
        std::map<std::string, std::map<std::string, std::string>> defaultAttributes;
        std::map<std::string, std::string> DEFAULT_ENTITIES = {
            {"lt;", "<"}, {"gt;", ">"}, {"amp;", "&"}, {"apos;", "'"}, {"quot;", "\""}
        };
        size_t position_ = 0;
        size_t depth = 0;
        size_t max_ = 0;
        size_t bufferStartLine_ = 0;
        size_t bufferStartColumn_ = 0;
        size_t attriCount_ = 0;
        TagEnum type = TagEnum::START_DOCUMENT;
        bool bWhitespace_ = false;
        SrcLinkList* srcLinkList_ = new SrcLinkList;
        bool bEndFlag_ = false;
        bool bAlone_ = false;
        bool bUnresolved_ = false;
        bool relaxed = false;
        bool bKeepNsAttri = false;
        bool bDocDecl = false;
    };
} // namespace OHOS::Xml
#endif // XML_JS_XML_H_
