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
    super(msg)
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
    if (converted.hasOwnProperty('spaces')) {
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
    let converted: ReceiveObject = this.convertxmlclass.convert(strXml, options);
    let strEnd: string = '';
    if (converted.hasOwnProperty('spaces')) {
      let space: string | number | undefined = converted.spaces;
      delete converted.spaces;
    }
    return converted;
  }
}

function dealXml(strXml: string): string {
  let idx: number = 0;
  let idxSec: number = 0;
  let idxThir: number = 0;
  let idxCData: number = 0;
  let idxCDataSec: number = 0;
  while ((idx = strXml.indexOf(']]><![CDATA')) !== -1) {
    strXml = strXml.substring(0, idx + LESS_SIGN_INDEX) + ' ' + strXml.substring(idx + LESS_SIGN_INDEX);
  }
  while ((idx = strXml.indexOf('>', idxSec)) !== -1) {
    idxThir = strXml.indexOf('<', idx);
    strXml = dealPriorReplace(strXml, idx, idxThir);
    idxSec = strXml.indexOf('<', idx);
    if (idxSec !== -1) {
      idxCData = strXml.indexOf('<![CDATA', idxCDataSec);
      if (idxSec === idxCData) {
        idxSec = strXml.indexOf(']]>', idxCData);
        strXml = dealLaterReplace(strXml, idxCData, idxSec);
        idxCDataSec = idxSec;
      }
    } else {
      break;
    }
  }
  return strXml;
}

function dealPriorReplace(strXml: string, idx: number, idxThir: number): string {
  let i: number = idx + 1;
  for (; i < idxThir; i++) {
    let cXml: string = strXml.charAt(i);
    if (cXml !== '\n' && cXml !== '\v' && cXml !== '\t' && cXml !== ' ') {
      break;
    }
  }
  let j: number = idx + 1;
  if (i === idxThir) {
    strXml = strXml.substring(0, j) + strXml.substring(idxThir);
  }
  let endIdx = strXml.indexOf('<', idx);
  if (j < endIdx) {
    let temp: string = strXml.substring(j, endIdx);
    if (temp.indexOf('\n') !== -1 || temp.indexOf('\v') !== -1 || temp.indexOf('\t') !== -1) {
      let pattern: RegExp = /[\n\v\t]/g;
      let result: string = temp.replace(pattern, function (match) {
        switch (match) {
          case '\n':
            return '\\n';
          case '\v':
            return '\\v';
          case '\t':
            return '\\t';
          default:
            return match;
        }
      });
      strXml = strXml.substring(0, j) + result + strXml.substring(endIdx);
    }
  }
  return strXml;
}

function dealLaterReplace(strXml: string, idx: number, idxThir: number): string {
  let i: number = idx + 1;
  let res = strXml.substring(i, idxThir);
  if (res.indexOf('\n') !== -1 || res.indexOf('\v') !== -1 || res.indexOf('\t') !== -1 || res.indexOf('\r') !== -1) {
    let pattern: RegExp = /[\n\v\t\r]/g;
    let result: string = res.replace(pattern, function (match) {
      switch (match) {
        case '\n':
          return '\\n';
        case '\v':
          return '\\v';
        case '\t':
          return '\\t';
        case '\r':
          return '\\r';
        default:
          return match;
      }
    });
    strXml = strXml.substring(0, i) + result + strXml.substring(idxThir);
  }
  return strXml;
}

export default {
  ConvertXML: ConvertXML
};
