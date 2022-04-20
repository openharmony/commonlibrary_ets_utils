/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
declare function requireInternal(s : string) : any;
const uri = requireInternal("uri");

class URI {
    uricalss : any
    constructor(input:any) {
        if (typeof input !== 'string' || input.length === 0) {
            throw new Error("input type err");
        }
        this.uricalss = new uri.Uri(input);
        let errStr = this.uricalss.isFailed;
        if (errStr.length !== 0) {
            throw new Error(errStr);
        }
    }
    toString() {
        return toAscllString(this.uricalss.toString());
    }

    equals(other:any) {
        return this.uricalss.equals(other.uricalss);
    }

    checkIsAbsolute() {
        return this.uricalss.checkIsAbsolute();
    }

    normalize() {
        let uriStr = this.uricalss.normalize();
        return createNewUri(uriStr);
    }

    get scheme() {
        return this.uricalss.scheme;
    }

    get authority() {
        return this.uricalss.authority;
    }

    get ssp() {
        return this.uricalss.ssp;
    }

    get userInfo() {
        return this.uricalss.userInfo;
    }

    get host() {
        return this.uricalss.host;
    }

    get port() {
        return this.uricalss.port;
    }

    get path() {
        return this.uricalss.path;
    }

    get query() {
        return this.uricalss.query;
    }

    get fragment() {
        return this.uricalss.fragment;
    }

}

function toAscllString(uriStr:any) {
    if (uriStr.indexOf('[') !== -1) {
        let arr = uriStr.split("[");
        let brr = arr[1].split("]");
        arr[1] = '[' + brr[0] + ']';
        arr[2] = brr[1];
        arr[0] = encodeURI(arr[0]);
        arr[2] = encodeURI(arr[2]);
        return arr.join('');
    } else {
        return encodeURI(uriStr);
    }
}

function createNewUri(uriStr : string) {
    return new URI(uriStr);
}

export default  {
    URI : URI
}