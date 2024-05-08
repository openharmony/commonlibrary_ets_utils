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

interface NativeURLSearchParams {
  new(input?: object | string | Iterable<[]> | null | undefined): NativeURLSearchParams;
  append(params1: string, params2: string): void;
  set(setname: string, setvalues: string): void;
  sort(): void;
  has(hasname: string): boolean;
  toString(): string;
  keys(): Array<string>;
  values(): Array<string>;
  getAll(getAllname: string): string[];
  get(getname: string): string;
  entries(): Array<Array<string>>;
  delete(deletename: string): void;
  updateParams(): void;
  array: string[];
  initialValue: Record<string, string[]>;
}

interface NativeURLParams {
  new(input?: object | string | Iterable<[]> | null | undefined): NativeURLParams;
  append(params1: string, params2: string): void;
  set(setname: string, setvalues: string): void;
  sort(): void;
  has(hasname: string): boolean;
  toString(): string;
  keys(): Array<string>;
  values(): Array<string>;
  getAll(getAllname: string): string[];
  get(getname: string): string;
  entries(): Array<Array<string>>;
  delete(deletename: string): void;
  updateParams(): void;
  array: string[];
  initialValue: Record<string, string[]>;
}

interface NativeUrl {
  new(input: string, base?: string | NativeUrl): NativeUrl;
  new(): NativeUrl;
  protocol: string;
  username: string;
  password: string;
  hash: string;
  search: string;
  hostname: string;
  host: string;
  port: string;
  href(input: string): void;
  pathname: string;
  onOrOff: boolean;
  GetIsIpv6: boolean;
  parseURL(input: string, base?: string | NativeUrl | URL): NativeUrl;
}
interface UrlInterface {
  URLSearchParams1: NativeURLSearchParams;
  Url: NativeUrl;
  URLParams1: NativeURLParams;
  stringParmas(input: string): string[];
}

declare function requireInternal(s: string): UrlInterface;
const UrlInterface = requireInternal('url');


let seachParamsArr: Array<string> = [];
const typeErrorCodeId = 401; // 401:ErrorCodeId
const syntaxErrorCodeId = 10200002; // 10200002:syntaxErrorCodeId

class BusinessError extends Error {
  code: number;
  constructor(msg: string) {
    super(msg);
    this.name = 'BusinessError';
    this.code = typeErrorCodeId;
  }
}

function decodeSafelyInner(input: string): string {
    if (input === undefined || input === '') {
        return input;
    }
    input = input.replaceAll('+', ' ');
    const regex = /(%[a-f0-9A-F]{2})|[^%]+/g;
    let outStr = input.match(regex).map(part => {
      if (part.startsWith('%')) {
        try {
          return decodeURIComponent(part);
        } catch (e) {
          return part;
        }
      }
      return part;
    }).join('');
    return outStr;
}

function customEncodeURIComponent(str: string | number): string {
  const hexStrLen = 2; // 2:String length of hexadecimal encoded values
  const hexAdecimal = 16; // 16:Hexadecimal number system
  const regex = /[!'()~]/g;
  return encodeURIComponent(str).replace(regex, function (c) {
    let hex = c.charCodeAt(0).toString(hexAdecimal);
    return '%' + (hex.length < hexStrLen ? '0' : '') + hex.toUpperCase();
  })
    .replace(/(%20)+/g, '+');
}

function customEncodeForToString(str: string | number): string {
  const hexStrLen = 2; // 2:String length of hexadecimal encoded values
  const hexAdecimal = 16; // 16:Hexadecimal number system
  const regex = /[!'()~]/g;
  return encodeURIComponent(str).replace(regex, function (c) {
    let hex = c.charCodeAt(0).toString(hexAdecimal);
    return '%' + (hex.length < hexStrLen ? '0' : '') + hex.toUpperCase();
  })
    .replace(/%20|%2B|%3D|%25/g, function (match) {
      switch (match) {
        case '%20':
          return '+';
        case '%2B':
          return '+';
        case '%3D':
          return '=';
        case '%25':
          return '%';
        default:
          return match;
      }
    });
}

function convertArrayToObj(arr: string[]): Record<string, string[]> {
    return arr.reduce((obj, val, index) => {
      if (index % 2 === 0) { // 2:Even subscripts exist as key values
        if (!obj[val]) {
          obj[val] = [];
        }
        obj[val].push(arr[index + 1]);
      }
      return obj;
    }, {});
  }

class URLParams {
  urlClass: NativeURLParams;
  parentUrl: URL | null = null;
  constructor(input: object | string | Iterable<[]> | null | undefined) {
    let out: string[] = parameterProcess(input);
    this.urlClass = new UrlInterface.URLParams1();
    this.urlClass.array = out;
    this.urlClass.initialValue = convertArrayToObj(out);
  }

  append(params1: string, params2: string): void {
    if (arguments.length === 0 || typeof params1 !== 'string') {
      throw new BusinessError(`Parameter error.The type of ${params1} must be string`);
    }
    if (arguments.length === 1 || typeof params2 !== 'string') {
      throw new BusinessError(`Parameter error.The type of ${params2} must be string`);
    }
    params1 = customEncodeURIComponent(params1);
    params2 = customEncodeURIComponent(params2);
    this.urlClass.append(params1, params2);
    if (this.parentUrl !== null) {
      this.parentUrl.c_info.search = this.toString();
      this.parentUrl.search_ = this.parentUrl.c_info.search;
      this.parentUrl.setHref();
    }
  }

  set(setName: string, setValues: string): void {
    if (arguments.length === 0 || typeof setName !== 'string') {
      throw new BusinessError(`Parameter error.The type of ${setName} must be string`);
    }
    if (arguments.length === 1 || typeof setValues !== 'string') {
      throw new BusinessError(`Parameter error.The type of ${setValues} must be string`);
    }
    setName = customEncodeURIComponent(setName);
    setValues = customEncodeURIComponent(setValues);
    this.urlClass.set(setName, setValues);
    if (Object.prototype.hasOwnProperty.call(this.urlClass.initialValue, setName)) {
      delete this.urlClass.initialValue[setName];
    }
    if (this.parentUrl !== null) {
      this.parentUrl.c_info.search = this.toString();
      this.parentUrl.search_ = this.parentUrl.c_info.search;
      this.parentUrl.setHref();
    }
  }

  sort(): void {
    this.urlClass.sort();
    if (this.parentUrl !== null) {
      this.parentUrl.c_info.search = this.toString();
      this.parentUrl.search_ = this.parentUrl.c_info.search;
      this.parentUrl.setHref();
    }
  }

  has(hasname: string): boolean {
    if (typeof hasname !== 'string') {
      throw new BusinessError(`Parameter error.The type of ${hasname} must be string`);
    }
    return this.urlClass.has(hasname);
  }

  toString(): string {
    let outPut: string = '';
    let arrayLen: number = this.urlClass.array.length;
    for (let pos: number = 0; pos < arrayLen; pos += 2) { // 2:Even subscripts exist as key values
      let key: string = this.urlClass.array[pos];
      let value: string = this.urlClass.array[pos + 1];
      let shouldEncode: boolean = Object.entries(this.urlClass.initialValue).some(([k, v]) => k === key && v.includes(value));
      if (shouldEncode) {
        key = customEncodeForToString(key);
        value = customEncodeForToString(value).replace(/=/g, '%3D');
      }
      outPut += `${pos > 0 ? '&' : ''}${key}=${value}`;
    }
    return outPut;
  }

  keys(): Object {
    let temp = this.urlClass.keys();
    temp = temp.map(value => decodeSafelyInner(value));
    return temp;
  }

  values(): Object {
    let temp = this.urlClass.values();
    temp = temp.map(value => decodeSafelyInner(value));
    return temp;
  }

  getAll(getAllname: string): string[] {
    if ((arguments.length !== 1) || (typeof getAllname !== 'string')) {
      throw new BusinessError(`Parameter error.The type of ${getAllname} must be string`);
    }
    let outPut: string[] = this.urlClass.getAll(getAllname);
    outPut = outPut.map(function(encodedString) {
        return decodeSafelyInner(encodedString);
    });
    return outPut;
  }

  get(getname: string): string {
    if (arguments.length === 0 || typeof getname !== 'string') {
      throw new BusinessError(`Parameter error.The type of ${getname} must be string`);
    }
    return decodeSafelyInner(this.urlClass.get(getname));
  }

  entries(): Object {
    let temp = this.urlClass.entries();
    temp = temp.map(innerArr => {
        return innerArr.map(value => decodeSafelyInner(value));
    });
    return temp;
  }

  delete(deleteName: string): void {
    if (arguments.length === 0 || typeof deleteName !== 'string') {
      throw new BusinessError(`Parameter error.The type of ${deleteName} must be string`);
    }
    this.urlClass.delete(deleteName);
    if (Object.prototype.hasOwnProperty.call(this.urlClass.initialValue, deleteName)) {
      delete this.urlClass.initialValue[deleteName];
    }
    if (this.parentUrl !== null) {
      this.parentUrl.c_info.search = this.toString();
      this.parentUrl.search_ = this.parentUrl.c_info.search;
      this.parentUrl.setHref();
    }
  }

  forEach(objfun: Function, thisArg?: Object): void {
    if (typeof objfun !== 'function') {
      throw new BusinessError(`Parameter error.The type of ${objfun} must be function`);
    }
    let array = this.urlClass.array;
    if (array.length === 0) {
      return;
    }
    if (typeof thisArg === 'undefined' || thisArg === null) {
      thisArg = this;
    }
    let size = array.length - 1;
    for (let i = 0; i < size; i += 2) { // 2:Searching for the number and number of keys and values 2
      let key = array[i].length === 0 ? array[i] : decodeSafelyInner(array[i]);
      let value = array[i + 1].length === 0 ? array[i + 1] : decodeSafelyInner(array[i + 1]);
      objfun.call(thisArg, value, key, this);
    }
  }

  [Symbol.iterator](): Object {
    return this.urlClass.entries();
  }

  updateParams(input: string): void {
    let out = [];
    out = parameterProcess(input);
    this.urlClass.array = out;
  }
}

class URLSearchParams {
  urlClass: NativeURLSearchParams;
  parentUrl: URL | null = null;
  constructor(input: object | string | Iterable<[]> | null | undefined) {
    let out: string[] = parameterProcessing(input);
    this.urlClass = new UrlInterface.URLSearchParams1();
    this.urlClass.array = out;
    this.urlClass.initialValue = convertArrayToObj(out);
  }
  append(params1: string, params2: string): void {
    if (arguments.length === 0 || typeof params1 !== 'string') {
      throw new BusinessError(`Parameter error.The type of ${params1} must be string`);
    }
    if (arguments.length === 1 || typeof params2 !== 'string') {
      throw new BusinessError(`Parameter error.The type of ${params2} must be string`);
    }
    params1 = customEncodeURIComponent(params1);
    params2 = customEncodeURIComponent(params2);
    this.urlClass.append(params1, params2);
    if (this.parentUrl !== null) {
      this.parentUrl.c_info.search = this.toString();
      this.parentUrl.search_ = this.parentUrl.c_info.search;
      this.parentUrl.setHref();
    }
  }

  set(setName: string, setValues: string): void {
    if (arguments.length === 0 || typeof setName !== 'string') {
      throw new BusinessError(`Parameter error.The type of ${setName} must be string`);
    }
    if (arguments.length === 1 || typeof setValues !== 'string') {
      throw new BusinessError(`Parameter error.The type of ${setValues} must be string`);
    }
    setName = customEncodeURIComponent(setName);
    setValues = customEncodeURIComponent(setValues);
    this.urlClass.set(setName, setValues);
    if (Object.prototype.hasOwnProperty.call(this.urlClass.initialValue, setName)) {
      delete this.urlClass.initialValue[setName];
    }
    if (this.parentUrl !== null) {
      this.parentUrl.c_info.search = this.toString();
      this.parentUrl.search_ = this.parentUrl.c_info.search;
      this.parentUrl.setHref();
    }
  }

  sort(): void {
    this.urlClass.sort();
    if (this.parentUrl !== null) {
      this.parentUrl.c_info.search = this.toString();
      this.parentUrl.search_ = this.parentUrl.c_info.search;
      this.parentUrl.setHref();
    }
  }

  has(hasname: string): boolean {
    if (typeof hasname !== 'string') {
      throw new BusinessError(`Parameter error.The type of ${hasname} must be string`);
    }
    return this.urlClass.has(hasname);
  }

  toString(): string {
    let outPut: string = '';
    let arrayLen: number = this.urlClass.array.length;
    for (let pos: number = 0; pos < arrayLen; pos += 2) { // 2:Even subscripts exist as key values
      let key: string = this.urlClass.array[pos];
      let value: string = this.urlClass.array[pos + 1];
      let shouldEncode: boolean = Object.entries(this.urlClass.initialValue).some(([k, v]) => k === key && v.includes(value));
      if (shouldEncode) {
        key = customEncodeForToString(key);
        value = customEncodeForToString(value).replace(/=/g, '%3D');
      }
      outPut += `${pos > 0 ? '&' : ''}${key}=${value}`;
    }
    return outPut;
  }

  keys(): Object {
    let temp = this.urlClass.keys();
    temp = temp.map(value => decodeSafelyInner(value));
    return temp;
  }

  values(): Object {
    let temp = this.urlClass.values();
    temp = temp.map(value => decodeSafelyInner(value));
    return temp;
  }

  getAll(getAllname: string): string[] {
    let outPut: string[] = this.urlClass.getAll(getAllname);
    outPut = outPut.map(function(encodedString) {
        return decodeSafelyInner(encodedString);
    });
    return outPut;
  }

  get(getname: string): string {
    return decodeSafelyInner(this.urlClass.get(getname));
  }

  entries(): Object {
    let temp = this.urlClass.entries();
    temp = temp.map(innerArr => {
        return innerArr.map(value => decodeSafelyInner(value));
    });
    return temp;
  }

  delete(deleteName: string): void {
    this.urlClass.delete(deleteName);
    if (Object.prototype.hasOwnProperty.call(this.urlClass.initialValue, deleteName)) {
      delete this.urlClass.initialValue[deleteName];
    }
    if (this.parentUrl !== null) {
      this.parentUrl.c_info.search = this.toString();
      this.parentUrl.search_ = this.parentUrl.c_info.search;
      this.parentUrl.setHref();
    }
  }

  forEach(objfun: Function, thisArg?: Object): void {
    let array = this.urlClass.array;
    if (array.length === 0) {
      return;
    }
    if (typeof thisArg === 'undefined' || thisArg === null) {
      thisArg = this;
    }
    let size = array.length - 1;
    for (let i = 0; i < size; i += 2) { // 2:Searching for the number and number of keys and values 2
      let key = array[i].length === 0 ? array[i] : decodeSafelyInner(array[i]);
      let value = array[i + 1].length === 0 ? array[i + 1] : decodeSafelyInner(array[i + 1]);
      objfun.call(thisArg, value, key, this);
    }
  }

  [Symbol.iterator](): Object {
    return this.urlClass.entries();
  }

  updateParams(input: string): void {
    let out = [];
    out = parameterProcessing(input);
    this.urlClass.array = out;
  }
}

function toHleString(arg: string | symbol | number): string {
  return arg.toString();
}

function parameterProcess(input: object | string | Iterable<[]>): Array<string> {
  if (input === null || typeof input === 'undefined' || input === '') {
    seachParamsArr = [];
    return seachParamsArr;
  } else if (typeof input === 'object' || typeof input === 'function') {
    return sysObjectParams(input);
  } else {
    return initToStringSeachParams(input);
  }
}

function parameterProcessing(input: object | string | Iterable<[]>): Array<string> {
  if (input === null || typeof input === 'undefined' || input === '') {
    seachParamsArr = [];
    return seachParamsArr;
  } else if (typeof input === 'object' || typeof input === 'function') {
    return initObjectSeachParams(input);
  } else {
    return initToStringSeachParams(input);
  }
}

function sysObjectParams(input: object | Iterable<[]>): Array<string> {
  if (typeof input[Symbol.iterator] === 'function') {
    return iteratorMethodThrow(input as Iterable<[string]>);
  }
  return recordMethod(input);
}

function initObjectSeachParams(input: object | Iterable<[]>): Array<string> {
  if (typeof input[Symbol.iterator] === 'function') {
    return iteratorMethod(input as Iterable<[string]>);
  }
  return recordMethod(input);
}

function recordMethod(input: object) : Array<string> {
  const keys = Reflect.ownKeys(input);
  seachParamsArr = [];
  for (let i = 0; i <= keys.length; i++) {
    const key = keys[i];
    const desc = Reflect.getOwnPropertyDescriptor(input, key);
    if (desc !== undefined && desc.enumerable) {
      const typedKey = toHleString(key);
      const typedValue = toHleString(input[key]);
      seachParamsArr.push(typedKey, typedValue);
    }
  }
  return seachParamsArr;
}

function iteratorMethodThrow(input: Iterable<[string]>): Array<string> {
  let pairs = [];
  seachParamsArr = [];
  for (const pair of input) {
    if ((typeof pair !== 'object' && typeof pair !== 'function') || pair === null || typeof pair[Symbol.iterator] !== 'function') {
      throw new BusinessError(`Parameter error.The type of ${input} must be string[][]`);
    }
    const convertedPair = [];
    for (let element of pair) {
      convertedPair.push(element);
    }
    pairs.push(convertedPair);
  }

  for (const pair of pairs) {
    if (pair.length !== 2) { // 2:Searching for the number and number of keys and values 2
      throw new BusinessError(`Parameter error.The type of ${input} must be string[][]`);
    }
    seachParamsArr.push(pair[0], pair[1]);
  }
  return seachParamsArr;
}

function iteratorMethod(input: Iterable<[string]>): Array<string> {
  let pairs = [];
  seachParamsArr = [];
  for (const pair of input) {
    const convertedPair = [];
    for (let element of pair) {
      convertedPair.push(element);
    }
    pairs.push(convertedPair);
  }

  for (const pair of pairs) {
    if (pair.length !== 2) { // 2:Searching for the number and number of keys and values 2
      console.error('key-value-is-worong');
    }
    seachParamsArr.push(pair[0], pair[1]);
  }
  return seachParamsArr;
}

function decodeURISafely(input: string): string {
  if (input === '') {
    return input;
  }
  const regex = /(%[0-9A-Fa-f]{2})|[^%]+/g;
  return input.match(regex).map(part => {
    if (part.startsWith('%') && part !== '%26' && part !== '%3D' && part !== '%25' && part !== '%2B') {
      try {
        return decodeURIComponent(part);
      } catch (e) {
        return part;
      }
    }
    return part;
  }).join('');
}

function initToStringSeachParams(input: string): Array<string> {
  if (typeof input !== 'string') {
    throw new BusinessError(`Parameter error.The type of ${input} must be string`);
  }
  if (input[0] === '?') {
    input = input.slice(1);
  }
  let strVal: string = decodeURISafely(input);
  seachParamsArr = UrlInterface.stringParmas(strVal);
  return seachParamsArr;
}

class URL {
  href_: string = '';
  search_: string = '';
  origin_: string = '';
  username_: string = '';
  password_: string = '';
  hostname_: string = '';
  host_: string = '';
  hash_: string = '';
  protocol_: string = '';
  pathname_: string = '';
  port_: string = '';
  searchParamsClass_ !: URLSearchParams;
  URLParamsClass_ !: URLParams;
  c_info !: NativeUrl;
  public constructor();
  public constructor(inputUrl: string, inputBase?: string | URL);
  public constructor(inputUrl?: string, inputBase?: string | URL) {
    if (arguments.length === 0) {
    }
    let nativeUrl !: NativeUrl;
    if (arguments.length === 1 || (arguments.length === 2 && (typeof inputBase === 'undefined' || inputBase === null))) {
      if (typeof inputUrl === 'string' && inputUrl.length > 0) {
        nativeUrl = new UrlInterface.Url(inputUrl);
      } else {
        console.error('Input parameter error');
      }
    } else if (arguments.length === 2) { // 2:The number of parameters is 2
      if (typeof inputUrl === 'string') {
        if (typeof inputBase === 'string') {
          if (inputBase.length > 0) {
            nativeUrl = new UrlInterface.Url(inputUrl, inputBase);
          } else {
            console.error('Input parameter error');
            return;
          }
        } else if (typeof inputBase === 'object') {
          let nativeBase: NativeUrl = inputBase.getInfo();
          nativeUrl = new UrlInterface.Url(inputUrl, nativeBase);
        }
      }
    }
    if (arguments.length === 1 || arguments.length === 2) { // 2:The number of parameters is 2
      this.c_info = nativeUrl;
      if (nativeUrl.onOrOff) {
        this.search_ = nativeUrl.search;
        this.username_ = encodeURI(nativeUrl.username);
        this.password_ = encodeURI(nativeUrl.password);
        if (nativeUrl.GetIsIpv6) {
          this.hostname_ = nativeUrl.hostname;
          this.host_ = nativeUrl.host;
        } else {
          this.hostname_ = encodeURI(nativeUrl.hostname);
          this.host_ = encodeURI(nativeUrl.host);
        }
        this.hash_ = encodeURI(nativeUrl.hash);
        this.protocol_ = encodeURI(nativeUrl.protocol);
        this.pathname_ = encodeURI(nativeUrl.pathname);
        this.port_ = nativeUrl.port;
        this.origin_ = nativeUrl.protocol + '//' + nativeUrl.host;
        this.searchParamsClass_ = new URLSearchParams(this.search_);
        this.URLParamsClass_ = new URLParams(this.search_);
        this.URLParamsClass_.parentUrl = this;
        this.searchParamsClass_.parentUrl = this;
        this.setHref();
      } else {
        console.error('constructor failed');
      }
    }
  }

  static parseURL(inputUrl: string, inputBase?: string | NativeUrl | URL): URL {
    if (typeof inputUrl !== 'string') {
      throw new BusinessError(`Parameter error.The type of ${inputUrl} must be string`);
    }
    let nativeUrl !: NativeUrl;
    if (arguments.length === 1 || (arguments.length === 2 && (typeof inputBase === 'undefined' || inputBase === null))) {
      nativeUrl = new UrlInterface.Url(inputUrl);
    } else if (arguments.length === 2) { // 2:The number of parameters is 2
      if (typeof inputBase === 'string') {
        if (inputBase.length > 0) {
          nativeUrl = new UrlInterface.Url(inputUrl, inputBase);
        } else {
          throw new BusinessError(`Parameter error.The type of ${inputBase} must be string`);
        }
      } else if (typeof inputBase === 'object') {
        let nativeBase: NativeUrl = inputBase.getInfo();
        nativeUrl = new UrlInterface.Url(inputUrl, nativeBase);
      } else {
        throw new BusinessError(`Parameter error.The type of ${inputBase} must be string or URL`);
      }
    }
    let urlHelper = new URL();
    urlHelper.c_info = nativeUrl;
    if (nativeUrl.onOrOff) {
      urlHelper.search_ = nativeUrl.search;
      urlHelper.username_ = encodeURI(nativeUrl.username);
      urlHelper.password_ = encodeURI(nativeUrl.password);
      if (nativeUrl.GetIsIpv6) {
        urlHelper.hostname_ = nativeUrl.hostname;
        urlHelper.host_ = nativeUrl.host;
      } else {
        urlHelper.hostname_ = encodeURI(nativeUrl.hostname);
        urlHelper.host_ = encodeURI(nativeUrl.host);
      }
      urlHelper.hash_ = encodeURI(nativeUrl.hash);
      urlHelper.protocol_ = encodeURI(nativeUrl.protocol);
      urlHelper.pathname_ = encodeURI(nativeUrl.pathname);
      urlHelper.port_ = nativeUrl.port;
      urlHelper.origin_ = nativeUrl.protocol + '//' + nativeUrl.host;
      urlHelper.searchParamsClass_ = new URLSearchParams(urlHelper.search_);
      urlHelper.URLParamsClass_ = new URLParams(urlHelper.search_);
      urlHelper.URLParamsClass_.parentUrl = urlHelper;
      urlHelper.searchParamsClass_.parentUrl = urlHelper;
      urlHelper.setHref();
    } else {
      let err : BusinessError = new BusinessError('Syntax Error. Invalid Url string');
      err.code = syntaxErrorCodeId;
      throw err;
    }
    return urlHelper;
  }

  getInfo(): NativeUrl {
    return this.c_info;
  }
  toString(): string {
    return this.href_;
  }

  get protocol(): string {
    return this.protocol_;
  }
  set protocol(scheme) {
    if (scheme.length === 0) {
      return;
    }
    if (this.protocol_ === 'file:' && (this.host_ === '' || this.host_ === null)) {
      return;
    }
    this.c_info.protocol = scheme;
    this.protocol_ = this.c_info.protocol;
    this.setHref();
  }
  get origin(): string {
    let kOpaqueOrigin: string = 'null';
    switch (this.protocol_) {
      case 'ftp:':
      case 'gopher:':
      case 'http:':
      case 'https:':
      case 'ws:':
      case 'wss:':
        return this.origin_;
    }
    return kOpaqueOrigin;
  }
  get username(): string {
    return this.username_;
  }
  set username(input) {
    if (this.host_ === null || this.host_ === '' || this.protocol_ === 'file:') {
      return;
    }
    const usname_ = encodeURI(input);
    this.c_info.username = usname_;
    this.username_ = this.c_info.username;
    this.setHref();
  }
  get password(): string {
    return this.password_;
  }
  set password(input) {
    if (this.host_ === null || this.host_ === '' || this.protocol_ === 'file:') {
      return;
    }
    const passwd_ = encodeURI(input);
    this.c_info.password = passwd_;
    this.password_ = this.c_info.password;
    this.setHref();
  }
  get hash(): string {
    return this.hash_;
  }
  set hash(fragment) {
    const fragment_ = encodeURI(fragment);
    this.c_info.hash = fragment_;
    this.hash_ = this.c_info.hash;
    this.setHref();
  }
  get search(): string {
    return this.search_;
  }
  set search(query) {
    const query_ = encodeURI(query);
    this.c_info.search = query_;
    this.search_ = this.c_info.search;
    this.searchParamsClass_.updateParams(this.search_);
    this.URLParamsClass_.updateParams(this.search_);
    this.setHref();
  }
  get hostname(): string {
    return this.hostname_;
  }
  set hostname(hostname) {
    this.c_info.hostname = hostname;
    if (this.c_info.GetIsIpv6) {
      this.hostname_ = this.c_info.hostname;
    } else {
      this.hostname_ = encodeURI(this.c_info.hostname);
    }
    this.setHref();
  }
  get host(): string {
    return this.host_;
  }
  set host(host_) {
    this.c_info.host = host_;
    if (this.c_info.GetIsIpv6) {
      this.host_ = this.c_info.host;
      this.hostname_ = this.c_info.hostname;
      this.port_ = this.c_info.port;
    } else {
      this.host_ = encodeURI(this.c_info.host);
      this.hostname_ = encodeURI(this.c_info.hostname);
      this.port_ = this.c_info.port;
    }
    this.setHref();
  }
  get port(): string {
    return this.port_;
  }
  set port(port) {
    if (this.host_ === '' || this.protocol_ === 'file:' || port === '') {
      return;
    }
    this.c_info.port = port;
    this.port_ = this.c_info.port;
    this.setHref();
  }
  get href(): string {
    return this.href_;
  }
  set href(href_) {
    this.c_info.href(href_);
    if (this.c_info.onOrOff) {
      this.search_ = encodeURI(this.c_info.search);
      this.username_ = encodeURI(this.c_info.username);
      this.password_ = encodeURI(this.c_info.password);
      if (this.c_info.GetIsIpv6) {
        this.hostname_ = this.c_info.hostname;
        this.host_ = this.c_info.host;
      } else {
        this.hostname_ = encodeURI(this.c_info.hostname);
        this.host_ = encodeURI(this.c_info.host);
      }
      this.hash_ = encodeURI(this.c_info.hash);
      this.protocol_ = encodeURI(this.c_info.protocol);
      this.pathname_ = encodeURI(this.c_info.pathname);
      this.port_ = this.c_info.port;
      this.origin_ = this.protocol_ + '//' + this.host_;
      this.searchParamsClass_.updateParams(this.search_);
      this.URLParamsClass_.updateParams(this.search_);
      this.setHref();
    }
  }

  get pathname(): string {
    return this.pathname_;
  }
  set pathname(path) {
    const path_ = encodeURI(path);
    this.c_info.pathname = path_;
    this.pathname_ = this.c_info.pathname;
    this.setHref();
  }

  get searchParams(): URLSearchParams {
    return this.searchParamsClass_;
  }

  get params(): URLParams {
    return this.URLParamsClass_;
  }

  toJSON(): string {
    return this.href_;
  }
  setHref(): void {
    let temp: string = this.protocol_;
    if (this.hostname_ !== '') {
      temp += '//';
      if (this.password_ !== '' || this.username_ !== '') {
        if (this.username_ !== '') {
          temp += this.username_;
        }
        if (this.password_ !== '') {
          temp += ':';
          temp += this.password_;
        }
        temp += '@';
      }
      temp += this.hostname_;
      if (this.port_ !== '') {
        temp += ':';
        temp += this.port_;
      }
    } else if (this.protocol_ === 'file:') {
      temp += '//';
    }
    temp += this.pathname_;
    if (this.search_) {
      temp += this.search_;
    }
    if (this.hash_) {
      temp += this.hash_;
    }
    this.href_ = temp;
  }
}

export default {
  URLSearchParams: URLSearchParams,
  URL: URL,
  URLParams: URLParams,
};