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
  checkIsRelative(): boolean;
  checkIsOpaque(): boolean;
  checkIsHierarchical(): boolean;
  addQueryValue(key: string, value: string): string;
  addSegment(pathSegment: string):string;
  getSegment(): string[];
  clearQuery(): string;
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
  encodedUserInfo: string | null;
  encodedPath: string | null;
  encodedQuery: string | null;
  encodedFragment: string | null;
  encodedAuthority: string | null;
  encodedSSP: string | null;
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
    super(msg);
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
    console.debug('URI:: uri src is: ' + input);
    this.uricalss = new uri.Uri(input);
    let errStr: string = this.uricalss.isFailed;
    if (errStr.length !== 0) {
      let err : BusinessError = new BusinessError('Syntax Error. Invalid Uri string');
      err.code = SyntaxErrorCodeId;
      throw err;
    }
  }

  static createFromParts(scheme: string, ssp: string, fragment: string): URI {
    if (scheme === null || typeof scheme !== 'string') {
      throw new BusinessError('The input parameter scheme is invalid');
    }
    if (ssp === null || typeof ssp !== 'string') {
      throw new BusinessError('The input parameter ssp is invalid');
    }
    if (typeof fragment !== 'string') {
      throw new BusinessError('The input parameter fragment is invalid');
    }
    let uriStr: string = scheme;
    uriStr += ':' + encodeURIComponent(ssp);
    if (fragment !== null && fragment !== '') {
      uriStr += '#' + encodeURIComponent(fragment);
    }
    return createNewUri(uriStr);
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

  checkRelative(): boolean {
    return this.uricalss.checkIsRelative();
  }

  checkOpaque(): boolean {
    return this.uricalss.checkIsOpaque();
  }

  checkHierarchical(): boolean {
    return this.uricalss.checkIsHierarchical();
  }

  addQueryValue(key: string, value: string): URI {
    if (key === null || typeof key !== 'string') {
      throw new BusinessError('The input parameter key is invalid');
    }
    if (value === null || typeof value !== 'string') {
      throw new BusinessError('The input parameter value is invalid');
    }
    let uriStr = this.uricalss.addQueryValue(encodeURIComponent(key), encodeURIComponent(value));
    return createNewUri(uriStr);
  }

  addEncodedSegment(pathSegment: string): URI {
    if (pathSegment === null || typeof pathSegment !== 'string') {
      throw new BusinessError('The input pathSegment value is invalid');
    }
    let uriStr: string = this.uricalss.addSegment(pathSegment);
    return createNewUri(uriStr);
  }

  addSegment(pathSegment: string): URI {
    if (pathSegment === null || typeof pathSegment !== 'string') {
      throw new BusinessError('The input pathSegment value is invalid');
    }
    let uriStr = this.uricalss.addSegment(encodeURIComponent(pathSegment));
    return createNewUri(uriStr);
  }

  getQueryValue(key: string): string | null {
    if (key === null || typeof key !== 'string') {
      throw new BusinessError('The input parameter key is invalid');
    }
    let value: string | null = null;
    if (this.uricalss.query === null) {
      return null;
    }
    let queryStrs: string[] = decodeURIComponent(this.uricalss.query).split('&') || [];
    for (let item of queryStrs) {
      if (key === '' && item === '') {
        return '';
      }
      let str = item.split('=') || [];
      if (str.length === 1 && str[0] === key) {
        return '';
      }
      if (str.length === 2 && str[0] === key) {
        return str[1].replace(/\+/g, ' ');
      }
    }
    return value;
  }

  getQueryNames(): string[] {
    let names: Set<string> = new Set<string>();
    if (this.uricalss.query === null) {
      return [];
    }
    let start: number = 0;
    while (start < this.uricalss.query.length) {
      let next: number = this.uricalss.query.indexOf('&', start);
      let end: number = (next === -1) ? this.uricalss.query.length : next;
      let separator: number = this.uricalss.query.indexOf('=', start);
      if (separator > end || separator === -1) {
        separator = end;
      }
      let name: string = this.uricalss.query.substring(start, separator);
      names.add(decodeURIComponent(name));
      start = end + 1;
    }
    return Array.from(names);
  }

  getQueryValues(key: string): string[] {
    if (key === null || typeof key !== 'string') {
      throw new BusinessError('The input parameter key is invalid');
    }
    let values = new Array();
    if (this.uricalss.query === null) {
      return values;
    }
    let queryStrs: string[] = decodeURIComponent(this.uricalss.query).split('&') || [];
    for (let item of queryStrs) {
      if (key === '' && item === '') {
        values.push(item);
      }
      let str = item.split('=') || [];
      if (str.length === 1 && str[0] === key) {
        values.push('');
      }
      if (str.length === 2 && str[0] === key) {
        values.push(str[1]);
      }
    }
    return values;
  }

  getBooleanQueryValue(key: string, defaultValue: boolean): boolean {
    if (key === null || typeof key !== 'string') {
      throw new BusinessError('The input parameter key is invalid');
    }
    if (defaultValue === null || typeof defaultValue !== 'boolean') {
      throw new BusinessError('The input parameter defaultValue is invalid');
    }
    let flag = this.getQueryValue(key);
    if (flag == null) {
      return defaultValue;
    }
    flag = flag.toLocaleLowerCase();
    return 'false' !== flag && '0' !== flag;
  }

  getLastSegment(): string {
    let segments = this.uricalss.getSegment();
    if (!segments) {
      return '';
    }
    return decodeURIComponent(segments[segments.length - 1]);
  }

  getSegment(): string[] {
    let array = new Array();
    let segments = this.uricalss.getSegment();
    if (segments) {
      segments.forEach(element => {
        array.push(decodeURIComponent(element));
      });
    }
    return array;
  }

  clearQuery(): URI {
    let uriStr: string = this.uricalss.clearQuery();
    return createNewUri(uriStr);
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

  get encodedUserInfo(): string | null {
    return this.uricalss.userInfo;
  }

  get encodedPath(): string | null {
    return this.uricalss.path;
  }

  get encodedQuery(): string | null {
    return this.uricalss.query;
  }

  get encodedFragment(): string | null {
    return this.uricalss.fragment;
  }

  get encodedAuthority(): string | null {
    if (this.uricalss.authority === null) {
      return null;
    }
    let thisAuthority: string = this.uricalss.authority;
    if (thisAuthority.indexOf('[') !== -1) {
      let arr: string[] = thisAuthority.split('[');
      let brr: string[] = arr[1].split(']');
      arr[1] = '[' + brr[0] + ']';
      arr[2] = brr[1];
      return arr.join('');
    } else {
      return thisAuthority;
    }
  }

  get encodedSSP(): string {
    let thisSsp: string = this.uricalss.ssp;
    if (thisSsp.indexOf('[') !== -1) {
      let arr: string[] = thisSsp.split('[');
      let brr: string[] = arr[1].split(']');
      arr[1] = '[' + brr[0] + ']';
      arr[2] = brr[1];
      return arr.join('');
    } else {
      return thisSsp;
    }
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