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

interface ReceiveObject{
    obj : Object;
    spaces ?: string | number;
}

interface NativeConvertXml {
    new() : NativeConvertXml;
    convert(strXml : string, options ?: Object) : ReceiveObject;
}
interface ConvertXML {
    ConvertXml : NativeConvertXml;
}
declare function requireInternal(s : string) : ConvertXML;
const convertXml = requireInternal("ConvertXML");

class ConvertXML {
    convertxmlclass : NativeConvertXml;
    constructor() {
        this.convertxmlclass = new convertXml.ConvertXml();
    }
    convert(strXml : string, options ?: Object) {
        strXml = DealXml(strXml);
        let converted = this.convertxmlclass.convert(strXml, options);
        let strEnd : string = '';
        if (converted.hasOwnProperty("spaces")) {
            let space : string | number | undefined = converted.spaces;
            delete converted.spaces;
            strEnd = JSON.stringify(converted, null, space);
        }
        let idx : number = 0;
        while ((idx = strEnd.indexOf('\\t')) != -1) {
            strEnd = strEnd.substring(0, idx) + '\t' + strEnd.substring(idx + 2);
        }
        while ((idx = strEnd.indexOf('\\n')) != -1) {
            strEnd = strEnd.substring(0, idx) + '\n' + strEnd.substring(idx + 2);
        }
        while ((idx = strEnd.indexOf('\\')) != -1) {
            strEnd = strEnd.substring(0, idx) + '' + strEnd.substring(idx + 1);
        }
        return strEnd;
    }
}

function DealXml(strXml : string)
{
    let idx : number = 0;
    let idxSec : number = 0;
    let idxThir : number = 0;
    let idxCData : number = 0;
    let idxCDataSec : number = 0;
    while ((idx = strXml.indexOf(']]><![CDATA')) != -1) {
        strXml = strXml.substring(0, idx + 3) + ' ' + strXml.substring(idx + 3);
    }
    while ((idx = strXml.indexOf('>', idxSec)) != -1) {
        idxThir = strXml.indexOf('<', idx);
        strXml = DealPriorReplace(strXml, idx, idxThir);
        if (strXml.indexOf('<', idx) != -1) {
            idxCData = strXml.indexOf('<![CDATA', idxCDataSec);
            idxSec = strXml.indexOf('<', idx);
            if (idxSec == idxCData) {
                idxSec = strXml.indexOf(']]>', idxCData);
                strXml = DealLaterReplace(strXml, idx, idxThir);
                idxCDataSec = idxSec;
            }
        }
        else {
            break;
        }
    }
    return strXml;
}

function DealPriorReplace(strXml : string, idx : number, idxThir : number)
{
    let i : number = idx + 1;
    for (; i < idxThir ; i++) {
        let cXml : string = strXml.charAt(i);
        if (cXml != '\n' && cXml != '\v' && cXml != '\t' && cXml != ' ')
        {
            break;
        }
    }
    let j : number = idx + 1;
    for (; j < strXml.indexOf('<', idx) ; j++) {
        let cXml : string = strXml.charAt(j);
        if (i != idxThir) {
            switch (cXml) {
                case '\n':
                    strXml = strXml.substring(0, j) + '\\n' + strXml.substring(j + 1);
                    break;
                case '\v':
                    strXml = strXml.substring(0, j) + '\\v' + strXml.substring(j + 1);
                    break;
                case '\t':
                    strXml = strXml.substring(0, j) + '\\t' + strXml.substring(j + 1);
                    break;
                default:
                    break;
            }
        } else {
            strXml = strXml.substring(0, j) + strXml.substring(j + 1);
            --j;
        }
    }
    return strXml;
}

function DealLaterReplace(strXml : string, idx : number, idxThir : number)
{
    let i : number = idx + 1;
    for (; i < idxThir ; i++) {
        let cXml : string = strXml.charAt(i)
        switch (cXml) {
            case '\n':
                strXml = strXml.substring(0, i) + '\\n' + strXml.substring(i + 1);
                break;
            case '\v':
                strXml = strXml.substring(0, i) + '\\v' + strXml.substring(i + 1);
                break;
            case '\t':
                strXml = strXml.substring(0, i) + '\\t' + strXml.substring(i + 1);
                break;
            default:
                break;
        }
    }
    return strXml;
}

export default ConvertXML