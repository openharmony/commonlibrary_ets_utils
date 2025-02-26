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
  parseXml(options: object): boolean;
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

interface NativeXMLDynamicSerializer {
  new(strEncoding?: string): NativeXMLDynamicSerializer;
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
  getOutput(): ArrayBuffer | undefined;
}

interface Xml {
  XmlSerializer: NativeXMLSerializer;
  XmlPullParser: NativeXmlPullParser;
  XmlDynamicSerializer: NativeXMLDynamicSerializer;
}

const ARGUMENT_LENGTH_ONE = 1;
const ARGUMENT_LENGTH_TWO = 2;
const TypeErrorCode = 401;
class BusinessError extends Error {
  code: number;
  constructor(msg: string) {
    super(msg);
    this.name = 'BusinessError';
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
        throw new BusinessError('Parameter error.Just support utf-8');
      }
      this.xmlSerializerClass = new XML.XmlSerializer(obj, inputStr);
    } else {
      throw new BusinessError(`Parameter error.The type of ${inputStr} must be string`);
    }
    let errStr: string = this.xmlSerializerClass.XmlSerializerError();
    if (errStr.length !== 0) {
      throw new BusinessError(errStr);
    }
  }

  setAttributes(name: string, value: string): void {
    if (typeof name !== 'string') {
      throw new BusinessError(`Parameter error.The type of ${name} must be string`);
    }
    if (name.length === 0) {
      throw new BusinessError(`Parameter error. Parameter cannot be empty`);
    }
    if (typeof value !== 'string') {
      throw new BusinessError(`Parameter error.The type of ${value} must be string`);
    }

    this.xmlSerializerClass.setAttributes(name, value);
    let errStr: string = this.xmlSerializerClass.XmlSerializerError();
    if (errStr.length !== 0) {
      throw new BusinessError(errStr);
    }
  }

  addEmptyElement(name: string): void {
    if (typeof name !== 'string') {
      throw new BusinessError(`Parameter error.The type of ${name} must be string`);
    }
    if (name.length === 0) {
      throw new BusinessError(`Parameter error. Parameter cannot be empty`);
    }

    this.xmlSerializerClass.addEmptyElement(name);
    let errStr: string = this.xmlSerializerClass.XmlSerializerError();
    if (errStr.length !== 0) {
      throw new BusinessError(errStr);
    }
  }

  setDeclaration(): void {
    this.xmlSerializerClass.setDeclaration();
    let errStr: string = this.xmlSerializerClass.XmlSerializerError();
    if (errStr.length !== 0) {
      throw new BusinessError(errStr);
    }
  }

  startElement(name: string): void {
    if (typeof name !== 'string') {
      throw new BusinessError(`Parameter error.The type of ${name} must be string`);
    }
    if (name.length === 0) {
      throw new BusinessError(`Parameter error. Parameter cannot be empty`);
    }
    this.xmlSerializerClass.startElement(name);
    let errStr: string = this.xmlSerializerClass.XmlSerializerError();
    if (errStr.length !== 0) {
      throw new BusinessError(errStr);
    }
  }

  endElement(): void {
    this.xmlSerializerClass.endElement();
    let errStr: string = this.xmlSerializerClass.XmlSerializerError();
    if (errStr.length !== 0) {
      throw new BusinessError(errStr);
    }
  }

  setNamespace(prefix: string, ns: string): void {
    if (typeof prefix !== 'string') {
      throw new BusinessError(`Parameter error.The type of ${prefix} must be string`);
    }
    if (prefix.length === 0) {
      throw new BusinessError(`Parameter error. Parameter cannot be empty`);
    }
    if (typeof ns !== 'string' || ns.length === 0) {
      throw new BusinessError(`Parameter error.The type of ${ns} must be string`);
    }
    this.xmlSerializerClass.setNamespace(prefix, ns);
    let errStr: string = this.xmlSerializerClass.XmlSerializerError();
    if (errStr.length !== 0) {
      throw new BusinessError(errStr);
    }
  }

  setComment(text: string): void {
    if (typeof text !== 'string') {
      let error = new BusinessError(`Parameter error.The type of ${text} must be string`);
      throw error;
    }
    if (text.length === 0) {
      throw new BusinessError(`Parameter error. Parameter cannot be empty`);
    }
    this.xmlSerializerClass.setComment(text);
    let errStr: string = this.xmlSerializerClass.XmlSerializerError();
    if (errStr.length !== 0) {
      throw new BusinessError(errStr);
    }
  }

  setCDATA(text: string): void {
    if (typeof text !== 'string') {
      throw new BusinessError(`Parameter error.The type of ${text} must be string`);
    }
    if (text.length === 0) {
      throw new BusinessError(`Parameter error. Parameter cannot be empty`);
    }
    this.xmlSerializerClass.setCDATA(text);
    let errStr: string = this.xmlSerializerClass.XmlSerializerError();
    if (errStr.length !== 0) {
      throw new BusinessError(errStr);
    }
  }

  setText(text: string): void {
    if (typeof text !== 'string') {
      throw new BusinessError(`Parameter error.The type of ${text} must be string`);
    }
    if (text.length === 0) {
      throw new BusinessError(`Parameter error. Parameter cannot be empty`);
    }
    this.xmlSerializerClass.setText(text);
    let errStr: string = this.xmlSerializerClass.XmlSerializerError();
    if (errStr.length !== 0) {
      throw new BusinessError(errStr);
    }
  }

  setDocType(text: string): void {
    if (typeof text !== 'string') {
      throw new BusinessError(`Parameter error.The type of ${text} must be string`);
    }
    if (text.length === 0) {
      throw new BusinessError(`Parameter error. Parameter cannot be empty`);
    }
    this.xmlSerializerClass.setDocType(text);
    let errStr: string = this.xmlSerializerClass.XmlSerializerError();
    if (errStr.length !== 0) {
      throw new BusinessError(errStr);
    }
  }
}

class XmlDynamicSerializer {
  xmlSerializerClass: NativeXMLDynamicSerializer;
  constructor(encoding?: string) {
    let input: string = 'utf-8';
    if (arguments.length === ARGUMENT_LENGTH_ONE) {
      if (typeof encoding !== 'string') {
        throw new BusinessError(`Parameter error.The type of ${encoding} must be string`);
      }
      if (encoding.length === 0 || encoding.toLowerCase() !== 'utf-8') {
        throw new BusinessError('Parameter error.Just support utf-8');
      }
    } 
    this.xmlSerializerClass = new XML.XmlDynamicSerializer(input);
  }

  getOutput(): ArrayBuffer | undefined {
    return this.xmlSerializerClass.getOutput();
  }

  setAttributes(name: string, value: string): void {
    if (typeof name !== 'string') {
      throw new BusinessError(`Parameter error.The type of ${name} must be string`);
    }
    if (name.length === 0) {
      throw new BusinessError(`Parameter error. Parameter cannot be empty`);
    }
    if (typeof value !== 'string') {
      throw new BusinessError(`Parameter error.The type of ${value} must be string`);
    }
    this.xmlSerializerClass.setAttributes(name, value);
  }

  addEmptyElement(name: string): void {
    if (typeof name !== 'string') {
      throw new BusinessError(`Parameter error.The type of ${name} must be string`);
    }
    if (name.length === 0) {
      throw new BusinessError(`Parameter error. Parameter cannot be empty`);
    }
    this.xmlSerializerClass.addEmptyElement(name);
  }

  setDeclaration(): void {
    this.xmlSerializerClass.setDeclaration();
  }

  startElement(name: string): void {
    if (typeof name !== 'string') {
      throw new BusinessError(`Parameter error.The type of ${name} must be string`);
    }
    if (name.length === 0) {
      throw new BusinessError(`Parameter error. Parameter cannot be empty`);
    }
    this.xmlSerializerClass.startElement(name);
  }

  endElement(): void {
    this.xmlSerializerClass.endElement();
  }

  setNamespace(prefix: string, ns: string): void {
    if (typeof prefix !== 'string') {
      throw new BusinessError(`Parameter error.The type of ${prefix} must be string`);
    }
    if (typeof ns !== 'string') {
      throw new BusinessError(`Parameter error.The type of ${ns} must be string`);
    }
    if (prefix.length === 0 || ns.length === 0) {
      throw new BusinessError(`Parameter error. Parameter cannot be empty`);
    }
    this.xmlSerializerClass.setNamespace(prefix, ns);
  }

  setComment(text: string): void {
    if (typeof text !== 'string') {
      throw new BusinessError(`Parameter error.The type of ${text} must be string`);
    }
    if (text.length === 0) {
      throw new BusinessError(`Parameter error. Parameter cannot be empty`);
    }
    this.xmlSerializerClass.setComment(text);
  }

  setCDATA(text: string): void {
    if (typeof text !== 'string') {
      throw new BusinessError(`Parameter error.The type of ${text} must be string`);
    }
    if (text.length === 0) {
      throw new BusinessError(`Parameter error. Parameter cannot be empty`);
    }
    this.xmlSerializerClass.setCDATA(text);
  }

  setText(text: string): void {
    if (typeof text !== 'string') {
      throw new BusinessError(`Parameter error.The type of ${text} must be string`);
    }
    if (text.length === 0) {
      throw new BusinessError(`Parameter error. Parameter cannot be empty`);
    }
    this.xmlSerializerClass.setText(text);
  }

  setDocType(text: string): void {
    if (typeof text !== 'string') {
      throw new BusinessError(`Parameter error.The type of ${text} must be string`);
    }
    if (text.length === 0) {
      throw new BusinessError(`Parameter error. Parameter cannot be empty`);
    }
    this.xmlSerializerClass.setDocType(text);
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
        throw new BusinessError('Parameter error.Just support utf-8');
      }
      this.xmlPullParserClass = new XML.XmlPullParser(obj, inputStr);
    } else {
      throw new BusinessError(`Parameter error.The type of ${inputStr} must be string`);
    }
    let errStr: string = this.xmlPullParserClass.XmlPullParserError();
    if (errStr.length !== 0) {
      throw new BusinessError(errStr);
    }
  }
  parse(options: object): void {
    if (typeof options !== 'object') {
      throw new BusinessError(`Parameter error.The type of ${options} must be object`);
    }
    this.xmlPullParserClass.parse(options);
    let errStr: string = this.xmlPullParserClass.XmlPullParserError();
    if (errStr.length !== 0) {
      throw new BusinessError(errStr);
    }
  }

  parseXml(options: object): void {
    if (typeof options !== 'object') {
      throw new BusinessError(`Parameter error.The type of ${options} must be object`);
    }
    if (this.xmlPullParserClass.parseXml(options)) {
      let errStr: string = this.xmlPullParserClass.XmlPullParserError();
      throw new BusinessError(errStr);
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
  XmlDynamicSerializer: XmlDynamicSerializer,
  EventType,
};
