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

#ifndef XML_JS_XML_DYNAMIC_H
#define XML_JS_XML_DYNAMIC_H

#include <algorithm>
#include <cstring>
#include <map>
#include <string>
#include <vector>
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "native_engine/native_engine.h"
#include "tools/log.h"

namespace OHOS::xml {
    constexpr uint32_t MAX_XML_LENGTH = 100000;
    constexpr uint32_t INIT_XML_LENGTH = 8 * 1024;
    enum ErrorCodeEnum {
        BUFFER_OVERFLOW = 10200062,
        ILLEGAL_POSITION = 10200063,
        NO_ELEMENT_MATCH = 10200064
    };

    class XmlDynamicSerializer {
    public:

        /**
         * Constructor for XmlDynamicSerializer.
         *
         * @param encoding Is the encoding format of XML serializer.
         */
        explicit XmlDynamicSerializer(napi_env env,
                                      const std::string &encoding = "utf-8") :env_(env), encoding_(encoding)
        {
            strXml_.reserve(INIT_XML_LENGTH);
        }

        /**
         * XmlDynamicSerializer destructor.
         */
        ~XmlDynamicSerializer() {}

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
         *
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
         *
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
         * @param data The parameter is the content of the CDATA attribute.
         */
        void SetCData(std::string data);

        /**
         * Write text attributes.
         *
         * @param text The parameter is the content between the elements.
         */
        void SetText(const std::string &text);

        /**
         * Write DocType property.
         *
         * @param text The parameter is the content of the DocType property.
         */
        void SetDocType(const std::string &text);

        /**
         * Write an escape.
         *
         * @param s The parameter is the passed in escaped string.
         */
        void WriteEscaped(std::string s);

        /**
         * Set namespace splice.
         */
        void SplicNsp();

        /**
         * Set next item.
         */
        void NextItem();

        /**
         * Get XML serializer buffer length.
         *
         * @return XML serializer buffer length.
         */
        size_t GetXmlBufferLength();

        /**
         * Get XML serializer buffer.
         *
         * @param data The parameter is the point of XML serializer buffer
         * @param length The parameter is the length of XML serializer buffer.
         * @return If the funtion copy buffer failed return false, else return true.
         */
        bool GetXmlBuffer(void* data, uint32_t length);

        /**
         * Process the value of the string passed by napi.
         *
         * @param env The parameter is NAPI environment variables.
         * @param napiStr The parameter is pass parameters.
         * @param result The parameter is return the processed value.
         * @return Native API status.
         */
        static napi_status DealNapiStrValue(napi_env env, const napi_value napiStr, std::string &result);

        friend class XmlTest;

    private:
        void BufferCopy();
        std::string Replace(std::string str, const std::string &subStr, const std::string &repStr);
        int32_t curNspNum {0};
        int32_t elementNum_ {0};
        size_t depth_ {0};
        ErrorCodeEnum errorCode_;
        napi_env env_ {nullptr};
        std::string xmlSerializerError_ {""};
        std::string encoding_ {""};
        std::string out_ {""};
        std::string strXml_ {""};
        std::string type_ {""};
        std::vector<std::string> elementStack_ = { "", "", ""};
        std::map<int, std::map<int, std::string>> multNsp_;
    };
} // namespace OHOS::Xml
#endif // XML_JS_XML_DYNAMIC_H