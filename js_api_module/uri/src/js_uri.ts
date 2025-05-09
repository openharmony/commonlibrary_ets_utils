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
  getLastSegment(): string | null;
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
      throw new BusinessError(`Parameter error. The type of ${input} must be string`);
    }
    this.uricalss = new uri.Uri(input);
    let errStr: string = this.uricalss.isFailed;
    if (errStr.length !== 0) {
      let err : BusinessError = new BusinessError(`Syntax Error. Invalid Uri string: The ${errStr}`);
      err.code = SyntaxErrorCodeId;
      throw err;
    }
  }

  static createFromParts(scheme: string, ssp: string, fragment: string): URI {
    if (scheme === null || typeof scheme !== 'string') {
      throw new BusinessError(`Parameter error. The type of ${scheme} must be string`);
    }
    if (ssp === null || typeof ssp !== 'string') {
      throw new BusinessError(`Parameter error. The type of ${ssp} must be string`);
    }
    if (typeof fragment !== 'string') {
      throw new BusinessError(`Parameter error. The type of ${fragment} must be string`);
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
    throw new BusinessError(`Parameter error. The type of ${other} must be URI`);
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
      throw new BusinessError(`Parameter error. The type of ${key} must be string`);
    }
    if (value === null || typeof value !== 'string') {
      throw new BusinessError(`Parameter error. The type of ${value} must be string`);
    }
    let uriStr = this.uricalss.addQueryValue(encodeURIComponent(key), encodeURIComponent(value));
    return createNewUri(uriStr);
  }

  addEncodedSegment(pathSegment: string): URI {
    if (pathSegment === null || typeof pathSegment !== 'string') {
      throw new BusinessError(`Parameter error. The type of ${pathSegment} must be string`);
    }
    let uriStr: string = this.uricalss.addSegment(pathSegment);
    return createNewUri(uriStr);
  }

  addSegment(pathSegment: string): URI {
    if (pathSegment === null || typeof pathSegment !== 'string') {
      throw new BusinessError(`Parameter error. The type of ${pathSegment} must be string`);
    }
    let uriStr = this.uricalss.addSegment(encodeURIComponent(pathSegment));
    return createNewUri(uriStr);
  }

  getQueryValue(key: string): string | null {
    if (key === null || typeof key !== 'string') {
      throw new BusinessError(`Parameter error. The type of ${key} must be string`);
    }
    let value: string | null = null;
    if (this.uricalss.query === null) {
      return null;
    }
    let queryStrs: string[] = this.uricalss.query.split('&') || [];
    for (let item of queryStrs) {
      if (key === '' && item === '') {
        return '';
      }
      let str = item.split('=') || [];
      if (str.length === 1 && this.decodeSafelyInner(str[0]) === key) {
        return '';
      } else if (this.decodeSafelyInner(str[0]) === key) {
        return this.decodeSafelyInner(item.substring(str[0].length + 1).replace(/\+/g, ' '));
      }
    }
    return value;
  }

  decodeSafelyOut(input: string): string {
    let decodedString: string = '';
    let decodedTemp: string = '';
    let index: number = 0;
    while (index < input.length) {
      if (input[index] === '%' && /[0-9A-Fa-f]{2}/.test(input.slice(index + 1, index + 3))) {
        const encodedChar = input.slice(index, index + 3);
        try {
          decodedString += decodeURIComponent(decodedTemp + encodedChar);
          decodedTemp = '';
        } catch (e) {
          decodedTemp += encodedChar;
        }
        index += 3;
        continue;
      }
      if (decodedTemp === '') {
        decodedString += input[index];
      } else {
        decodedString += decodedTemp;
        decodedString += input[index];
        decodedTemp = '';
      }
      index++;
    }
    return decodedTemp === '' ? decodedString : decodedString += decodedTemp;
  }

  decodeSafelyInner(input: string): string {
    if (input === undefined || input === '') {
      return input;
    }
    let strVal: string = '';
    try {
      strVal = decodeURIComponent(input);
    } catch (e) {
      strVal = this.decodeSafelyOut(input);
    }
    return strVal;
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
      names.add(this.decodeSafelyInner(name));
      start = end + 1;
    }
    return Array.from(names);
  }

  getQueryValues(key: string): string[] {
    if (key === null || typeof key !== 'string') {
      throw new BusinessError(`Parameter error. The type of ${key} must be string`);
    }
    let values = new Array();
    if (this.uricalss.query === null) {
      return values;
    }
    let queryStrs: string[] = this.uricalss.query.split('&') || [];
    for (let item of queryStrs) {
      if (key === '' && item === '') {
        values.push(item);
      }
      let str = item.split('=') || [];
      if (str.length === 1 && this.decodeSafelyInner(str[0]) === key) {
        values.push('');
      } else if (this.decodeSafelyInner(str[0]) === key) {
        values.push(this.decodeSafelyInner(item.substring(str[0].length + 1)));
      }
    }
    return values;
  }

  getBooleanQueryValue(key: string, defaultValue: boolean): boolean {
    if (key === null || typeof key !== 'string') {
      throw new BusinessError(`Parameter error. The type of ${key} must be string`);
    }
    if (defaultValue === null || typeof defaultValue !== 'boolean') {
      throw new BusinessError(`Parameter error. The type of ${key} must be boolean`);
    }
    let flag = this.getQueryValue(key);
    if (flag == null) {
      return defaultValue;
    }
    flag = flag.toLocaleLowerCase();
    return 'false' !== flag && '0' !== flag;
  }

  getLastSegment(): string {
    let segments = this.uricalss.getLastSegment();
    if (!segments) {
      return '';
    }
    return this.decodeSafelyInner(segments);
  }

  getSegment(): string[] {
    let array = new Array();
    let encodedPath: string | null = this.encodedPath;
    if (encodedPath) {
      const result: Array<string> = [];
      encodedPath.split('/').forEach(segment => {
        if (segment !== '') {
          result.push(this.decodeSafelyInner(segment));
        }
      });
      return result;
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
 
  set scheme(input: string | null) {
    if (input === null || input.length === 0) {
      return;
    }
    this.uricalss.scheme = input;
  }

  set path(input: string | null) {
    if (input === null) {
      return;
    }
    this.uricalss.path = encodeURI(input);
  }

  set ssp(input: string | null) {
    if (input === null) {
      return;
    }
    this.uricalss.ssp = encodeURI(input);
  }

  get authority(): string | null {
    if (this.uricalss.authority === null) {
      return null;
    }
    let thisAuthority: string = this.uricalss.authority;
    if (thisAuthority.indexOf('[') !== -1) {
      let arr: string[] = thisAuthority.split('[');
      let brr: string[] = arr[1].split(']');
      arr[1] = '[' + brr[0] + ']';
      arr[2] = brr[1];
      arr[0] = this.decodeSafelyInner(arr[0]);
      arr[2] = this.decodeSafelyInner(arr[2]);
      return arr.join('');
    } else {
      return this.decodeSafelyInner(thisAuthority);
    }
  }

  set authority(input: string | null) {
    if (input === null) {
      return;
    }
    this.uricalss.authority = encodeURI(input);
  }

  get ssp(): string {
    let thisSsp: string = this.uricalss.ssp;
    if (thisSsp.indexOf('[') !== -1) {
      let arr: string[] = thisSsp.split('[');
      let brr: string[] = arr[1].split(']');
      arr[1] = '[' + brr[0] + ']';
      arr[2] = brr[1];
      arr[0] = this.decodeSafelyInner(arr[0]);
      arr[2] = this.decodeSafelyInner(arr[2]);
      return arr.join('');
    } else {
      return this.decodeSafelyInner(thisSsp);
    }
  }

  get userInfo(): string | null {
    return this.uricalss.userInfo === null ? null : this.decodeSafelyInner(this.uricalss.userInfo);
  }

  set userInfo(input: string | null) {
    if (input === null) {
      return;
    }
    this.uricalss.userInfo = encodeURIComponent(input);
  }

  get host(): string | null {
    return this.uricalss.host;
  }

  get port(): string {
    return this.uricalss.port;
  }

  get path(): string | null {
    return this.uricalss.path === null ? null : this.decodeSafelyInner(this.uricalss.path);
  }

  get query(): string | null {
    return this.uricalss.query === null ? null : this.decodeSafelyInner(this.uricalss.query);
  }

  set query(input: string | null) {
    if (input === null) {
      return;
    }
    this.uricalss.query = encodeURIComponent(input);
  }

  get fragment(): string | null {
    return this.uricalss.fragment === null ? null : this.decodeSafelyInner(this.uricalss.fragment);
  }

  set fragment(input: string | null) {
    if (input === null) {
      return;
    }
    this.uricalss.fragment = encodeURIComponent(input);
  }

  set encodedUserInfo(input: string | null) {
    if (input === null) {
      return;
    }
    this.uricalss.userInfo = input;
  }

  set encodedPath(input: string | null) {
    if (input === null) {
      return;
    }
    this.uricalss.path = input;
  }

  set encodedQuery(input: string | null) {
    if (input === null) {
      return;
    }
    this.uricalss.query = input;
  }

  set encodedFragment(input: string | null) {
    if (input === null) {
      return;
    }
    this.uricalss.fragment = input;
  }

  set encodedAuthority(input: string | null) {
    if (input === null) {
      return;
    }
    this.uricalss.authority = input;
  }

  set encodedSSP(input: string | null) {
    if (input === null) {
      return;
    }
    this.uricalss.ssp = input;
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