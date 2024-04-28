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

interface ReceiveObject {
  obj: Object;
  spaces?: string | number;
}

interface NativeConvertXml {
  new(): NativeConvertXml;
  convert(strXml: string, options?: Object): ReceiveObject;
  convertToJSObject(strXml: string, options?: Object): ReceiveObject;
}
interface ConvertXML {
  ConvertXml: NativeConvertXml;
}
declare function requireInternal(s: string): ConvertXML;
const convertXml = requireInternal('convertxml');

const LESS_SIGN_INDEX = 3;
const TypeErrorCode = 401;
class BusinessError extends Error {
  code: number;
  constructor(msg: string) {
    super(msg);
    this.name = 'BusinessError';
    this.code = TypeErrorCode;
  }
}

class ConvertXML {
  convertxmlclass: NativeConvertXml;
  constructor() {
    this.convertxmlclass = new convertXml.ConvertXml();
  }
  convert(strXml: string, options?: Object): ReceiveObject {
    strXml = dealXml(strXml);
    let converted: ReceiveObject = this.convertxmlclass.convert(strXml, options);
    let strEnd: string = '';
    if (Object.prototype.hasOwnProperty.call(converted, 'spaces')) {
      let space: string | number | undefined = converted.spaces;
      delete converted.spaces;
    }
    return converted;
  }

  convertToJSObject(strXml: string, options?: Object): ReceiveObject {
    if (typeof strXml !== 'string') {
      throw new BusinessError(`Parameter error.The type of ${strXml} must be string`);
    }
    if (options && !(typeof options === 'undefined' || options === null) && typeof options !== 'object') {
      throw new BusinessError(`Parameter error.The type of ${options} must be object`);
    }
    strXml = dealXml(strXml);
    let converted: ReceiveObject;
    if (arguments.length === 1) {
      converted = this.convertxmlclass.convert(strXml);
    } else {
      converted = this.convertxmlclass.convert(strXml, options);
    }
    if (Object.prototype.hasOwnProperty.call(converted, 'spaces')) {
      let space: string | number | undefined = converted.spaces;
      delete converted.spaces;
    }
    return converted;
  }
}

function dealXml(strXml: string): string {
  strXml = strXml.replace(/(<!\[CDATA\[[\s\S]*?\]\]>)|(>\s+<)/g,  function(match, group) {
    if (group) {
      return group;
    } else {
      return '><';
    }
  }).trim();
  strXml = strXml.replace(/\]\]><!\[CDATA/g, ']]> <![CDATA');
  return strXml.replace(/<!\[CDATA\[[\s\S]*?\]\]>/g, function(match) {
   return match.replace(/\\/g, '\\\\').replace(/[\r\n\t\v]/g, function(suit) {
     switch(suit) {
       case '\n': return '\\n';
       case '\r': return '\\r';
       case '\t': return '\\t';
       case '\v': return '\\v';
       default: return suit;
     }
   })
  });
}

export default {
  ConvertXML: ConvertXML
};
