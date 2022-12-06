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

interface NativeXmlPullParser {
    new(value : object, strEncoding? : string) : NativeXmlPullParser;
    parse(options : object) : void;
    XmlPullParserError() : string;
}

interface NativeXMLSerializer {
    new(value : object, strEncoding? : string) : NativeXMLSerializer;
    setAttributes(name : string, value : string) : void;
    addEmptyElement(name : string) : void;
    setDeclaration() : void;
    startElement(name : string) : void;
    endElement() : void;
    setNamespace(prefix : string, namespace : string) : void;
    setComment(text : string) : void;
    setCDATA(text : string) : void;
    setText(text : string) : void;
    setDocType(text : string) : void;
    XmlSerializerError() : string;
}

interface Xml {
    XmlSerializer : NativeXMLSerializer;
    XmlPullParser : NativeXmlPullParser;
}

const TypeErrorCode = 401;
class BusinessError extends Error {
    code:number;
    constructor(msg:string) {
        super(msg)
        this.name = 'BusinessError'
        this.code = TypeErrorCode;
    }
}

declare function requireInternal(s : string) : Xml;
const XML = requireInternal('xml');
class XmlSerializer {
    xmlSerializerClass : NativeXMLSerializer;
    constructor() {
        if(typeof arguments[0] !== 'object') {
            let error = new BusinessError(`Parameter error.The type of ${arguments[0]} must be object`);
            throw error;
        }
        if (arguments.length === 1) {
            const inputType :string = 'utf-8';
            this.xmlSerializerClass = new XML.XmlSerializer(arguments[0], inputType);
        } else if (arguments.length === 2 && (typeof arguments[1] === 'string' && arguments[1].length !== 0)) {
            var strTemp = arguments[1];
            if (strTemp.toLowerCase() !== 'utf-8') {
                throw new Error('Just support utf-8');
            }
            this.xmlSerializerClass = new XML.XmlSerializer(arguments[0], arguments[1]);
        } else {
            let error = new BusinessError(`Parameter error.The type of ${arguments[1]} must be string`);
            throw error;
        }
        let errStr = this.xmlSerializerClass.XmlSerializerError();
        if (errStr.length !== 0) {
            throw new Error(errStr);
        }
    }
    setAttributes(name : string, value : string) {
        if (typeof name !== 'string' || name.length === 0) {
            let error = new BusinessError(`Parameter error.The type of ${name} must be string`);
            throw error;
        }
        if (typeof value !== 'string') {
            let error = new BusinessError(`Parameter error.The type of ${value} must be string`);
            throw error;
        }

        this.xmlSerializerClass.setAttributes(name, value);
        let errStr = this.xmlSerializerClass.XmlSerializerError();
        if (errStr.length !== 0) {
            throw new Error(errStr);
        }
    }
    addEmptyElement(name : string) {
        if (typeof name !== 'string' || name.length === 0) {
            let error = new BusinessError(`Parameter error.The type of ${name} must be string`);
            throw error;
        }
        this.xmlSerializerClass.addEmptyElement(name);
        let errStr = this.xmlSerializerClass.XmlSerializerError();
        if (errStr.length !== 0) {
            throw new Error(errStr);
        }
    }
    setDeclaration() {
        this.xmlSerializerClass.setDeclaration();
        let errStr = this.xmlSerializerClass.XmlSerializerError();
        if (errStr.length !== 0) {
            throw new Error(errStr);
        }
    }
    startElement(name : string) {
        if (typeof name !== 'string' || name.length === 0) {
            let error = new BusinessError(`Parameter error.The type of ${name} must be string`);
            throw error;
        }
        this.xmlSerializerClass.startElement(name);
        let errStr = this.xmlSerializerClass.XmlSerializerError();
        if (errStr.length !== 0) {
            throw new Error(errStr);
        }
    }
    endElement() {
        this.xmlSerializerClass.endElement();
        let errStr = this.xmlSerializerClass.XmlSerializerError();
        if (errStr.length !== 0) {
            throw new Error(errStr);
        }
    }
    setNamespace(prefix : string, ns : string) {
        if (typeof prefix !== 'string' || prefix.length === 0) {
            let error = new BusinessError(`Parameter error.The type of ${prefix} must be string`);
            throw error;
        }
        if (typeof ns !== 'string' || ns.length === 0) {
            let error = new BusinessError(`Parameter error.The type of ${ns} must be string`);
            throw error;
        }
        this.xmlSerializerClass.setNamespace(prefix, ns);
        let errStr = this.xmlSerializerClass.XmlSerializerError();
        if (errStr.length !== 0) {
            throw new Error(errStr);
        }
    }
    setComment(text : string) {
        if (typeof text !== 'string' || text.length === 0) {
            let error = new BusinessError(`Parameter error.The type of ${text} must be string`);
            throw error;
        }
        this.xmlSerializerClass.setComment(text);
        let errStr = this.xmlSerializerClass.XmlSerializerError();
        if (errStr.length !== 0) {
            throw new Error(errStr);
        }
    }
    setCDATA(text : string) {
        if (typeof text !== 'string' || text.length === 0) {
            let error = new BusinessError(`Parameter error.The type of ${text} must be string`);
            throw error;
        }
        this.xmlSerializerClass.setCDATA(text);
        let errStr = this.xmlSerializerClass.XmlSerializerError();
        if (errStr.length !== 0) {
            throw new Error(errStr);
        }
    }
    setText(text : string) {
        if (typeof text !== 'string' || text.length === 0) {
            let error = new BusinessError(`Parameter error.The type of ${text} must be string`);
            throw error;
        }
        this.xmlSerializerClass.setText(text);
        let errStr = this.xmlSerializerClass.XmlSerializerError();
        if (errStr.length !== 0) {
            throw new Error(errStr);
        }
    }
    setDocType(text : string) {
        if (typeof text !== 'string' || text.length === 0) {
            let error = new BusinessError(`Parameter error.The type of ${text} must be string`);
            throw error;
        }
        this.xmlSerializerClass.setDocType(text);
        let errStr = this.xmlSerializerClass.XmlSerializerError();
        if (errStr.length !== 0) {
            throw new Error(errStr);
        }
    }
}

class XmlPullParser {
    xmlPullParserClass : NativeXmlPullParser;
    constructor() {
        if(typeof arguments[0] !== 'object') {
            let error = new BusinessError(`Parameter error.The type of ${arguments[0]} must be object`);
            throw error;
        }
        if (arguments.length === 1) {
            let str = 'utf-8';
            this.xmlPullParserClass = new XML.XmlPullParser(arguments[0], str);
        } else if (arguments.length === 2 && (typeof arguments[1] === 'string' && arguments[1].length !== 0)) {
            var strTemp = arguments[1];
            if (strTemp.toLowerCase() !== 'utf-8') {
                throw new Error('Just support utf-8');
            }
            this.xmlPullParserClass = new XML.XmlPullParser(arguments[0], arguments[1]);
        } else {
            let error = new BusinessError(`Parameter error.The type of ${arguments[1]} must be string`);
            throw error;
        }
        let errStr = this.xmlPullParserClass.XmlPullParserError();
        if (errStr.length !== 0) {
            throw new Error(errStr);
        }
    }
    parse(options : object) {
        if(typeof options !== 'object') {
            let error = new BusinessError(`Parameter error.The type of ${options} must be object`);
            throw error;
        }
        this.xmlPullParserClass.parse(options);
        let errStr = this.xmlPullParserClass.XmlPullParserError();
        if (errStr.length !== 0) {
            throw new Error(errStr);
        }
    }
}

enum EventType {
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
    WHITESPACE
}

export default {
    XmlSerializer : XmlSerializer,
    XmlPullParser : XmlPullParser,
    EventType,
}
