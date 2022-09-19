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

interface NativeURLSearchParams{
    new(input ?: object | string | Iterable<[]> | null | undefined) : NativeURLSearchParams;
    append(params1 : string, params2 : string) : void;
    set(setname : string, setvalues : string) : void;
    sort() : void;
    has(hasname : string) : boolean;
    toString() : string;
    keys() : Object;
    values(): Object;
    getAll(getAllname : string) : string[];
    get(getname : string) : string;
    entries() : Object;
    delete(deletename : string) : void;
    updateParams() : void; 
    array : string[] | number[];
}

interface NativeURLParams{
    new(input ?: object | string | Iterable<[]> | null | undefined) : NativeURLParams;
    append(params1 : string, params2 : string) : void;
    set(setname : string, setvalues : string) : void;
    sort() : void;
    has(hasname : string) : boolean;
    toString() : string;
    keys() : Object;
    values(): Object;
    getAll(getAllname : string) : string[];
    get(getname : string) : string;
    entries() : Object;
    delete(deletename : string) : void;
    updateParams() : void;
    array : string[] | number[];
}

interface NativeUrl{
    new(input : string, base ?: string | NativeUrl) : NativeUrl;
    new() : NativeUrl;
    protocol : string;
    username : string;
    password : string;
    hash : string;
    search : string;
    hostname : string;
    host : string;
    port : string;
    href(input : string) : void;
    pathname : string;
    onOrOff : boolean;
    GetIsIpv6 : boolean;
    parseURL(input : string, base ?: string | NativeUrl | URL) : NativeUrl;
}
interface UrlInterface{
    URLSearchParams1 : NativeURLSearchParams;
    Url : NativeUrl;
    URLParams1 : NativeURLParams;
    stringParmas(input : string) : string[];
}

declare function requireInternal(s : string) : UrlInterface;
const UrlInterface = requireInternal('url');


var seachParamsArr : Array<string> = [];
const TypeErrorCodeId = 401;
const SyntaxErrorCodeId = 10200002;

class BusinessError extends Error {
    code : number;
    constructor(msg:string) {
        super(msg)
        this.name = 'BusinessError';
        this.code = TypeErrorCodeId;
    }
}

class URLParams {
    urlcalss:NativeURLParams;
    constructor(input : object | string | Iterable<[]> | null | undefined) {
        let out : string[] = parameterProcess(input);
        this.urlcalss = new UrlInterface.URLParams1();
        this.urlcalss.array = out;
    }

    append(params1 : string, params2 : string) : void {
        if (arguments.length === 0 || typeof arguments[0] !== 'string') {
            let err = new BusinessError(`Parameter error.The type of ${arguments[0]} must be string`);
            throw err;
        }
        if (arguments.length === 1 || typeof arguments[1] !== 'string') {
            let error = new BusinessError(`Parameter error.The type of ${arguments[1]} must be string`);
            throw error;
        }
        this.urlcalss.append(params1, params2);
    }

    set(setname : string, setvalues : string) : void {
        if (arguments.length === 0 || typeof arguments[0] !== 'string') {
            let err = new BusinessError(`Parameter error.The type of ${arguments[0]} must be string`);
            throw err;
        }
        if (arguments.length === 1 || typeof arguments[1] !== 'string') {
            let error = new BusinessError(`Parameter error.The type of ${arguments[1]} must be string`);
            throw error;
        }
        this.urlcalss.set(setname, setvalues);
    }

    sort() : void {
        this.urlcalss.sort();
    }

    has(hasname : string) : boolean {
        return this.urlcalss.has(hasname);
    }

    toString() : string {
        return this.urlcalss.toString();
    }

    keys() : Object {
        return this.urlcalss.keys();
    }

    values() : Object {
        return this.urlcalss.values();
    }

    getAll() : string[] {
        if ((arguments.length !== 1) || (typeof arguments[0] !== 'string')) { 
            let err = new BusinessError(`Parameter error.The type of ${arguments[0]} must be string`);
            throw err;
        }
        return this.urlcalss.getAll(arguments[0]);
    }

    get(getname : string) : string {
        if (arguments.length === 0 || typeof arguments[0] !== 'string') {
            let err = new BusinessError(`Parameter error.The type of ${arguments[0]} must be string`);
            throw err;
        }
        return this.urlcalss.get(getname);
    }

    entries() : Object {
        return this.urlcalss.entries();
    }

    delete(deletename : string) : void {
        if (arguments.length === 0 || typeof arguments[0] !== 'string') {
            let err = new BusinessError(`Parameter error.The type of ${arguments[0]} must be string`);
            throw err;
        }
        this.urlcalss.delete(deletename);
    }

    forEach(objfun : Function, thisArg ?: Object) {
        if (typeof objfun !== 'function') {
            let err = new BusinessError(`Parameter error.The type of ${objfun} must be function`);
            throw err;
        }
        let array = this.urlcalss.array;
        if (array.length == 0) {
            return;
        }
        let size = array.length - 1;
        for (let i = 0; i < size; i += 2) { // 2:Searching for the number and number of keys and values 2
            let key = array[i];
            let value = array[i + 1];
            objfun.call(thisArg, value, key, this);
        }
    }

    [Symbol.iterator]() : Object {
        return this.urlcalss.entries();
    }

    updateParams(input : string) : void {
        let out = [];
        out = parameterProcess(input);
        this.urlcalss.array = out;
    }
}

class URLSearchParams {
    urlcalss : NativeURLSearchParams;
    constructor(input : object | string | Iterable<[]> | null | undefined) {
        let out : string[] = parameterProcessing(input);
        this.urlcalss = new UrlInterface.URLSearchParams1();
        this.urlcalss.array = out;
    }
    append(params1 : string, params2 : string) {
        this.urlcalss.append(params1, params2);
    }

    set(setname : string, setvalues : string) {
        this.urlcalss.set(setname, setvalues);
    }

    sort() {
        this.urlcalss.sort();
    }

    has(hasname : string) {
        return this.urlcalss.has(hasname);
    }

    toString() {
        return this.urlcalss.toString();
    }

    keys() {
        return this.urlcalss.keys();
    }

    values() {
        return this.urlcalss.values();
    }

    getAll(getAllname : string) {
        return this.urlcalss.getAll(getAllname);
    }

    get(getname : string) {
        return this.urlcalss.get(getname);
    }

    entries() {

        return this.urlcalss.entries();
    }

    delete(deletename : string) {
        this.urlcalss.delete(deletename);
    }

    forEach(objfun : Function, thisArg ?: Object) {
        let array = this.urlcalss.array;
        if (array.length == 0) {
            return;
        }

        let size = array.length - 1;
        for (let i = 0; i < size; i += 2) { // 2:Searching for the number and number of keys and values 2
            let key = array[i];
            let value = array[i + 1];
            objfun.call(thisArg, value, key, this);
        }
    }

    [Symbol.iterator]() {
        return this.urlcalss.entries();
    }

    updateParams(input : string) {
        let out = [];
        out = parameterProcessing(input);
        this.urlcalss.array = out;
    }
}

function toHleString(arg : string | symbol | number) {
    return arg.toString();
}

function parameterProcess(input : object | string | Iterable<[]>) {
    if (input === null || typeof input === 'undefined') {
        seachParamsArr = [];
        return seachParamsArr;
    } else if (typeof input === 'object' || typeof input === 'function') {
        return sysObjectParams(input);
    } else {
        return initToStringSeachParams(input);
    }
}

function parameterProcessing(input : object | string | Iterable<[]>) {
    if (input === null || typeof input === 'undefined') {
        seachParamsArr = [];
        return  seachParamsArr;
    } else if (typeof input === 'object' || typeof input === 'function') {
        return initObjectSeachParams(input);
    } else {
        return initToStringSeachParams(input);
    }
}

function sysObjectParams(input : object | Iterable<[]>) {
    if (typeof input[Symbol.iterator] === 'function') {
        return iteratorMethodThrow(input as Iterable<[string]>);
    }
    return recordMethod(input);
}

function initObjectSeachParams(input : object | Iterable<[]>) {
    if (typeof input[Symbol.iterator] === 'function') {
        return iteratorMethod(input as Iterable<[string]>);
    }
    return recordMethod(input);
}

function recordMethod(input : object) {
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
    return  seachParamsArr;
}

function iteratorMethodThrow(input : Iterable<[string]>) {
    let pairs = [];
    seachParamsArr = [];
    for (const pair of input) {
        if ((typeof pair !== 'object' && typeof pair !== 'function') || pair === null || typeof pair[Symbol.iterator] !== 'function') {
            let err = new BusinessError(`Parameter error.The type of ${input} must be string[][]`);
            throw err;        
        }
        const convertedPair = [];
        for (let element of pair) {
            convertedPair.push(element);
        }
        pairs.push(convertedPair);
    }

    for (const pair of pairs) {
        if (pair.length !== 2) { // 2:Searching for the number and number of keys and values 2
            let err = new BusinessError(`Parameter error.The type of ${input} must be string[][]`);
            throw err;
        }
        seachParamsArr.push(pair[0], pair[1]);
    }
    return  seachParamsArr;
}

function iteratorMethod(input : Iterable<[string]>) {
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
    return  seachParamsArr;
}

function initToStringSeachParams(input : string) {
    if (typeof input !== 'string') {
        let err = new BusinessError(`Parameter error.The type of ${input} must be string`);
        throw err;
    }
    if (input[0] === '?') {
        input = input.slice(1);
    }
    let strVal = decodeURI(input);
    seachParamsArr = UrlInterface.stringParmas(strVal);
    return seachParamsArr;
}

class URL {
    href_ : string = '';
    search_ : string = '';
    origin_ : string = '';
    username_ : string = '';
    password_ : string = '';
    hostname_ : string = '';
    host_ : string = '';
    hash_ : string = '';
    protocol_ : string = '';
    pathname_ : string = '';
    port_ : string = '';
    searchParamsClass_ !: URLSearchParams|URLParams;
    c_info !: NativeUrl;
    public constructor()
    public constructor(inputUrl: string, inputBase?: string | URL)
    public constructor(inputUrl?: string, inputBase?: string | URL) {
        if (arguments.length === 0) {
        }
        let nativeUrl !: NativeUrl;
        if (arguments.length === 1) {
            inputUrl = arguments[0];
            if (typeof inputUrl === 'string' && inputUrl.length > 0) {
                nativeUrl = new UrlInterface.Url(inputUrl);
            } else {
                console.error('Input parameter error');
            }
        }

        if (arguments.length === 2) { // 2:The number of parameters is 2
            inputUrl = arguments[0];
            inputBase = arguments[1];
            if (typeof inputUrl === 'string') {
                if (typeof inputBase === 'string') {
                    if (inputBase.length > 0) {
                        nativeUrl = new UrlInterface.Url(inputUrl, inputBase);
                    } else {
                        console.error('Input parameter error');
                        return;
                    }
                } else if (typeof inputBase === 'object') {
                    let nativeBase = inputBase.getInfo();
                    nativeUrl = new UrlInterface.Url(inputUrl, nativeBase);
                }
            }
        }
        if (arguments.length === 1 || arguments.length === 2) { // 2:The number of parameters is 2
            this.c_info = nativeUrl;
            if (nativeUrl.onOrOff) {
                this.search_ = encodeURI(nativeUrl.search);
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
                this.set_href();
            } else {
                console.error('constructor failed');
            }
        }
    }

    parseURL(inputUrl : string, inputBase ?: string | NativeUrl | URL) {   
        if (typeof arguments[0] !== 'string') {
            let err = new BusinessError(`Parameter error.The type of ${arguments[0]} must be string`);
            throw err;
        }
        let nativeUrl !: NativeUrl;
        if (arguments.length === 1) {
            inputUrl = arguments[0];
            nativeUrl = new UrlInterface.Url(inputUrl);
        }
        if (arguments.length === 2) { // 2:The number of parameters is 2
            if (typeof inputBase === 'string') {
                if (inputBase.length > 0) {
                    nativeUrl = new UrlInterface.Url(inputUrl, inputBase);
                } else {
                    let err = new BusinessError(`Parameter error.The type of ${inputBase} must be string`);
                    throw err;
                }
            } else if (typeof inputBase === 'object') {
                let nativeBase = inputBase.getInfo();
                nativeUrl = new UrlInterface.Url(inputUrl, nativeBase);
            } else {
                let err = new BusinessError(`Parameter error.The type of ${inputBase} must be string or URL`);
                throw err;
            }  
        }
        this.c_info = nativeUrl;
        if (nativeUrl.onOrOff) {
            this.search_ = encodeURI(nativeUrl.search);
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
            this.set_href();
        } else {
            let err = new BusinessError('Syntax Error. Invalid Url string');
            err.code = SyntaxErrorCodeId;
            throw err;
        }
    }

    getInfo() {
        return this.c_info;
    }
    toString() {
        return this.href_;
    }

    get protocol() {
        return this.protocol_;
    }
    set protocol(scheme) {
        if (scheme.length === 0) {
            return;
        }
        if (this.protocol_ === 'file:'
            && (this.host_ === '' || this.host_ == null)) {
            return;
        }
        this.c_info.protocol = scheme;
        this.protocol_ = this.c_info.protocol;
        this.set_href()
    }
    get origin() {
        let kOpaqueOrigin = 'null';
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
    get username() {
        return this.username_;
    }
    set username(input) {
        if (this.host_ == null || this.host_ === '' || this.protocol_ === 'file:') {
            return;
        }
        const usname_ = encodeURI(input);
        this.c_info.username = usname_;
        this.username_ = this.c_info.username;
        this.set_href();
    }
    get password() {
        return this.password_;
    }
    set password(input) {
        if (this.host_ == null || this.host_ === '' || this.protocol_ === 'file:') {
            return;
        }
        const passwd_ = encodeURI(input);
        this.c_info.password = passwd_;
        this.password_ = this.c_info.password;
        this.set_href();
    }
    get hash() {
        return this.hash_;
    }
    set hash(fragment) {
        const fragment_ = encodeURI(fragment);
        this.c_info.hash = fragment_;
        this.hash_ = this.c_info.hash;
        this.set_href();
    }
    get search() {
        return this.search_;
    }
    set search(query) {
        const query_ = encodeURI(query);
        this.c_info.search = query_;
        this.search_ = this.c_info.search;
        this.searchParamsClass_.updateParams(this.search_);
        this.set_href();
    }
    get hostname() {
        return this.hostname_;
    }
    set hostname(hostname) {
        this.c_info.hostname = hostname;
        if (this.c_info.GetIsIpv6) {
            this.hostname_ = this.c_info.hostname;
        } else {
            this.hostname_ = encodeURI(this.c_info.hostname);
        }
        this.set_href();
    }
    get host() {
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
        this.set_href();
    }
    get port() {
        return this.port_;
    }
    set port(port) {
        if (this.host_ === '' || this.protocol_ === 'file:' || port === '') {
            return;
        }
        this.c_info.port = port;
        this.port_ = this.c_info.port;
        this.set_href();
    }
    get href() {
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
            this.set_href();
        }
    }

    get pathname() {
        return this.pathname_;
    }
    set pathname(path) {
        const path_ = encodeURI(path);
        this.c_info.pathname = path_;
        this.pathname_ = this.c_info.pathname;
        this.set_href();
    }

    get searchParams() {
        return this.searchParamsClass_;
    }

    toJSON() {
        return this.href_;
    }
    set_href() {
        let temp = this.protocol_;
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
}
