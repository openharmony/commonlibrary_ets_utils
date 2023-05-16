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
  new(value: object, strEncoding?: string): NativeXmlPullParser;
  parse(options: object): void;
  XmlPullParserError(): string;
}

interface NativeXMLSerializer {
  new(value: object, strEncoding?: string): NativeXMLSerializer;
  setAttributes(name: string, value: string): void;
  addEmptyElement(name: string): void;
  setDeclaration(): void;
  startElement(name: string): void;
  endElement(): void;
  setNamespace(prefix: string, namespace: string): void;
  setComment(text: string): void;
  setCDATA(text: string): void;
  setText(text: string): void;
  setDocType(text: string): void;
  XmlSerializerError(): string;
}

interface Xml {
  XmlSerializer: NativeXMLSerializer;
  XmlPullParser: NativeXmlPullParser;
}
const ARGUMENT_LENGTH_TWO = 2;
const TypeErrorCode = 401;
class BusinessError extends Error {
  code: number;
  constructor(msg: string) {
    super(msg);
    this.name = 'BusinessError'
    this.code = TypeErrorCode;
  }
}

declare function requireInternal(s: string): Xml;
const XML = requireInternal('xml');
class XmlSerializer {
  xmlSerializerClass: NativeXMLSerializer;
  constructor(obj: object, inputStr: string) {
    if (typeof obj !== 'object') {
      throw new BusinessError(`Parameter error.The type of ${obj} must be object`);
    }
    if (arguments.length === 1 ||
        (arguments.length === ARGUMENT_LENGTH_TWO && (typeof inputStr === 'undefined' || inputStr === null))) {
      const inputType: string = 'utf-8';
      this.xmlSerializerClass = new XML.XmlSerializer(obj, inputType);
    } else if (arguments.length === ARGUMENT_LENGTH_TWO && (typeof inputStr === 'string' && inputStr.length !== 0)) {
      let strTemp: string = inputStr;
      if (strTemp.toLowerCase() !== 'utf-8') {
        throw new Error('Just support utf-8');
      }
      this.xmlSerializerClass = new XML.XmlSerializer(obj, inputStr);
    } else {
      throw new BusinessError(`Parameter error.The type of ${inputStr} must be string`);
    }
    let errStr: string = this.xmlSerializerClass.XmlSerializerError();
    if (errStr.length !== 0) {
      throw new Error(errStr);
    }
  }
  setAttributes(name: string, value: string): void {
    if (typeof name !== 'string' || name.length === 0) {
      throw new BusinessError(`Parameter error.The type of ${name} must be string`);
    }
    if (typeof value !== 'string') {
      throw new BusinessError(`Parameter error.The type of ${value} must be string`);
    }

    this.xmlSerializerClass.setAttributes(name, value);
    let errStr: string = this.xmlSerializerClass.XmlSerializerError();
    if (errStr.length !== 0) {
      throw new Error(errStr);
    }
  }
  addEmptyElement(name: string): void {
    if (typeof name !== 'string' || name.length === 0) {
      throw new BusinessError(`Parameter error.The type of ${name} must be string`);
    }
    this.xmlSerializerClass.addEmptyElement(name);
    let errStr: string = this.xmlSerializerClass.XmlSerializerError();
    if (errStr.length !== 0) {
      throw new Error(errStr);
    }
  }
  setDeclaration(): void {
    this.xmlSerializerClass.setDeclaration();
    let errStr: string = this.xmlSerializerClass.XmlSerializerError();
    if (errStr.length !== 0) {
      throw new Error(errStr);
    }
  }
  startElement(name: string): void {
    if (typeof name !== 'string' || name.length === 0) {
      throw new BusinessError(`Parameter error.The type of ${name} must be string`);
    }
    this.xmlSerializerClass.startElement(name);
    let errStr: string = this.xmlSerializerClass.XmlSerializerError();
    if (errStr.length !== 0) {
      throw new Error(errStr);
    }
  }
  endElement(): void {
    this.xmlSerializerClass.endElement();
    let errStr: string = this.xmlSerializerClass.XmlSerializerError();
    if (errStr.length !== 0) {
      throw new Error(errStr);
    }
  }
  setNamespace(prefix: string, ns: string): void {
    if (typeof prefix !== 'string' || prefix.length === 0) {
      throw new BusinessError(`Parameter error.The type of ${prefix} must be string`);
    }
    if (typeof ns !== 'string' || ns.length === 0) {
      throw new BusinessError(`Parameter error.The type of ${ns} must be string`);
    }
    this.xmlSerializerClass.setNamespace(prefix, ns);
    let errStr: string = this.xmlSerializerClass.XmlSerializerError();
    if (errStr.length !== 0) {
      throw new Error(errStr);
    }
  }
  setComment(text: string): void {
    if (typeof text !== 'string' || text.length === 0) {
      let error = new BusinessError(`Parameter error.The type of ${text} must be string`);
      throw error;
    }
    this.xmlSerializerClass.setComment(text);
    let errStr: string = this.xmlSerializerClass.XmlSerializerError();
    if (errStr.length !== 0) {
      throw new Error(errStr);
    }
  }
  setCDATA(text: string): void {
    if (typeof text !== 'string' || text.length === 0) {
      throw new BusinessError(`Parameter error.The type of ${text} must be string`);
    }
    this.xmlSerializerClass.setCDATA(text);
    let errStr: string = this.xmlSerializerClass.XmlSerializerError();
    if (errStr.length !== 0) {
      throw new Error(errStr);
    }
  }
  setText(text: string): void {
    if (typeof text !== 'string' || text.length === 0) {
      throw new BusinessError(`Parameter error.The type of ${text} must be string`);
    }
    this.xmlSerializerClass.setText(text);
    let errStr: string = this.xmlSerializerClass.XmlSerializerError();
    if (errStr.length !== 0) {
      throw new Error(errStr);
    }
  }
  setDocType(text: string): void {
    if (typeof text !== 'string' || text.length === 0) {
      throw new BusinessError(`Parameter error.The type of ${text} must be string`);
    }
    this.xmlSerializerClass.setDocType(text);
    let errStr: string = this.xmlSerializerClass.XmlSerializerError();
    if (errStr.length !== 0) {
      throw new Error(errStr);
    }
  }
}

class XmlPullParser {
  xmlPullParserClass: NativeXmlPullParser;
  constructor(obj: object, inputStr: string) {
    if (typeof obj !== 'object') {
      throw new BusinessError(`Parameter error.The type of ${obj} must be object`);
    }
    if (arguments.length === 1 ||
        (arguments.length === ARGUMENT_LENGTH_TWO && (typeof inputStr === 'undefined' || inputStr === null))) {
      let str: string = 'utf-8';
      this.xmlPullParserClass = new XML.XmlPullParser(obj, str);
    } else if (arguments.length === ARGUMENT_LENGTH_TWO && (typeof inputStr ===
      'string' && inputStr.length !== 0)) {
      let strTemp: string = inputStr;
      if (strTemp.toLowerCase() !== 'utf-8') {
        throw new Error('Just support utf-8');
      }
      this.xmlPullParserClass = new XML.XmlPullParser(obj, inputStr);
    } else {
      throw new BusinessError(`Parameter error.The type of ${inputStr} must be string`);
    }
    let errStr: string = this.xmlPullParserClass.XmlPullParserError();
    if (errStr.length !== 0) {
      throw new Error(errStr);
    }
  }
  parse(options: object): void {
    if (typeof options !== 'object') {
      throw new BusinessError(`Parameter error.The type of ${options} must be object`);
    }
    this.xmlPullParserClass.parse(options);
    let errStr: string = this.xmlPullParserClass.XmlPullParserError();
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
  XmlSerializer: XmlSerializer,
  XmlPullParser: XmlPullParser,
  EventType,
}
