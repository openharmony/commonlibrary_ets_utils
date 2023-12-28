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
interface NativeUri {
  new(input: string): NativeUri;
  normalize(): string;
  equals(other: NativeUri): boolean;
  equalsTo(other: NativeUri): boolean;
  checkIsAbsolute(): boolean;
  toString(): string;
  scheme: string | null;
  authority: string | null;
  ssp: string;
  userInfo: string | null;
  host: string | null;
  port: string;
  path: string | null;
  query: string | null;
  fragment: string | null;
  isFailed: string;
}
interface UriInterface {
  Uri: NativeUri;
}
declare function requireInternal(s: string): UriInterface;
const uri = requireInternal('uri');

const TypeErrorCodeId = 401;
const SyntaxErrorCodeId = 10200002;

class BusinessError extends Error {
  code: number;
  constructor(msg: string) {
    super(msg)
    this.name = 'BusinessError';
    this.code = TypeErrorCodeId;
  }
}

class URI {
  uricalss: NativeUri;
  constructor(input: string) {
    if (typeof input !== 'string' || input.length === 0) {
      throw new BusinessError(`Parameter error.The type of ${input} must be string`);
    }
    this.uricalss = new uri.Uri(input);
    let errStr: string = this.uricalss.isFailed;
    if (errStr.length !== 0) {
      let err : BusinessError = new BusinessError('Syntax Error. Invalid Uri string');
      err.code = SyntaxErrorCodeId;
      throw err;
    }
  }
  toString(): string {
    return toAscllString(this.uricalss.toString());
  }

  equals(other: URI): boolean {
    return this.uricalss.equals(other.uricalss);
  }

  equalsTo(other: URI): boolean {
    if (other instanceof URI) {
      return this.uricalss.equals(other.uricalss);
    }
    throw new BusinessError(`Parameter error.The type of ${other} must be URI`);
  }

  checkIsAbsolute(): boolean {
    return this.uricalss.checkIsAbsolute();
  }

  normalize(): URI {
    let uriStr: string = this.uricalss.normalize();
    return createNewUri(uriStr);
  }

  get scheme(): string | null {
    return this.uricalss.scheme;
  }

  get authority(): string | null {
    if (this.uricalss.authority === null) {
      return this.uricalss.authority;
    }
    let thisAuthority: string = this.uricalss.authority;
    if (thisAuthority.indexOf('[') !== -1) {
      let arr: string[] = thisAuthority.split('[');
      let brr: string[] = arr[1].split(']');
      arr[1] = '[' + brr[0] + ']';
      arr[2] = brr[1];
      arr[0] = decodeURIComponent(arr[0]);
      arr[2] = decodeURIComponent(arr[2]);
      return arr.join('');
    } else {
      return decodeURIComponent(thisAuthority);
    }
  }

  get ssp(): string {
    let thisSsp: string = this.uricalss.ssp;
    if (thisSsp.indexOf('[') !== -1) {
      let arr: string[] = thisSsp.split('[');
      let brr: string[] = arr[1].split(']');
      arr[1] = '[' + brr[0] + ']';
      arr[2] = brr[1];
      arr[0] = decodeURIComponent(arr[0]);
      arr[2] = decodeURIComponent(arr[2]);
      return arr.join('');
    } else {
      return decodeURIComponent(thisSsp);
    }
  }

  get userInfo(): string | null {
    return this.uricalss.userInfo === null ? this.uricalss.userInfo : decodeURIComponent(this.uricalss.userInfo);
  }

  get host(): string | null {
    return this.uricalss.host;
  }

  get port(): string {
    return this.uricalss.port;
  }

  get path(): string | null {
    return this.uricalss.path === null ? this.uricalss.path : decodeURIComponent(this.uricalss.path);
  }

  get query(): string | null {
    return this.uricalss.query === null ? this.uricalss.query : decodeURIComponent(this.uricalss.query);
  }

  get fragment(): string | null {
    return this.uricalss.fragment === null ? this.uricalss.fragment : decodeURIComponent(this.uricalss.fragment);
  }
}

function toAscllString(uriStr: string): string {
  return encodeURI(uriStr).replace(/%5B/g, '[').replace(/%5D/g, ']').replace(/%25/g, '%');
}

function createNewUri(uriStr: string): URI {
  return new URI(uriStr);
}

export default {
  URI: URI
};