/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
interface ArkPrivate {
  FastBuffer: number;
  Load(key: number): Object;
}
let FastBufferInner = undefined;
let arkPritvate: ArkPrivate = globalThis.ArkPrivate || undefined;
FastBufferInner = arkPritvate.Load(arkPritvate.FastBuffer);

function getTypeName(obj: unknown): string {
  if (obj === null) {
    return 'null';
  }
  if (typeof obj !== 'object') {
    return typeof obj;
  }
  if (obj != null && obj.constructor != null) {
    return obj.constructor.name;
  }
  return 'unknown';
}

class BusinessError extends Error {
  code: number;
  constructor(message: string, errorNumber: number) {
    super(message);
    this.name = 'BusinessError';
    this.code = errorNumber;
  }
}

const ERROR_CODES = {
  TYPE_ERROR: 401, // 401: TYPE_ ERROR code value
  RANGE_ERROR: 10200001, // 10200001: RANGE_ERROR code value
  BUFFER_SIZE_ERROR: 10200009, // 10200009: BUFFER_SIZE_ERROR code value
  PROPERTY_TYPE_ERROR: 10200013 // 10200013: TYPE_ ERROR code value
};

let errorMap = {
  'typeError': ERROR_CODES.TYPE_ERROR,
  'rangeError': ERROR_CODES.RANGE_ERROR,
  'bufferSizeError': ERROR_CODES.BUFFER_SIZE_ERROR,
  'typeErrorForProperty': ERROR_CODES.PROPERTY_TYPE_ERROR
};

enum TypeErrorCategories {
  COMMON = 0,
  SIZE,
  ENCODING,
  PROPERTY
};
enum RangeErrorCategories {
  WHOLE = 0,
  LEFT
};

const UINT32MAX = 4294967295;
const INT64_MAX = 2n ** 63n - 1n;
const UINT64_MAX = 2n ** 64n - 1n;

class ErrorMessage {
  public errorNumber: number = 0;
  public argument: string = '';
  public types: string[] = [];
  public receivedObj: unknown = '';
  public rangeLeft: string | bigint | number = 0;
  public rangeRight: string | bigint | number = 0;

  public typeErrorCat: TypeErrorCategories = TypeErrorCategories.COMMON;
  public rangeErrorCat: RangeErrorCategories = RangeErrorCategories.WHOLE;

  constructor(errNo: number, argument?: string) {
    this.errorNumber = errNo;
    this.argument = argument === undefined ? '' : argument;
  }

  public setTypeInfo(types: string[], receivedObj: unknown): ErrorMessage {
    this.types = types;
    this.receivedObj = receivedObj;
    return this;
  }

  public setSizeTypeInfo(types: string[], receivedObj: unknown): ErrorMessage {
    this.types = types;
    this.receivedObj = receivedObj;
    this.typeErrorCat = TypeErrorCategories.SIZE;
    return this;
  }

  public setEncodingTypeInfo(types: string[], receivedObj: unknown): ErrorMessage {
    this.types = types;
    this.receivedObj = receivedObj;
    this.typeErrorCat = TypeErrorCategories.ENCODING;
    return this;
  }

  public setProperty(argument: string): ErrorMessage {
    this.typeErrorCat = TypeErrorCategories.PROPERTY;
    this.argument = argument;
    return this;
  }

  public setRangeInfo(rangeLeft: string | bigint | number, rangeRight: string | bigint | number, receivedObj: unknown): ErrorMessage {
    this.rangeLeft = rangeLeft;
    this.rangeRight = rangeRight;
    this.receivedObj = receivedObj;
    return this;
  }

  public setRangeLeftInfo(rangeLeft: string | number, receivedObj: unknown): ErrorMessage {
    this.rangeLeft = rangeLeft;
    this.receivedObj = receivedObj;
    this.rangeErrorCat = RangeErrorCategories.LEFT;
    return this;
  }

  public setSizeInfo(receivedObj: string): ErrorMessage {
    this.receivedObj = receivedObj;
    return this;
  }

  private getErrorTypeStrings(types: string[]): string {
    let ret = types.join(', ');
    ret = ret.replace(',', ' or');
    return ret;
  }

  private getArgumentStr(flag: boolean): string {
    if (flag) {
      return 'Parameter error. The type of "' + this.argument + '" must be ';
    } else {
      return 'The type of "' + this.argument + '" must be ';
    }
  }

  private getTypeString(flag: boolean): string {
    let str = '';
    switch (this.typeErrorCat) {
      case TypeErrorCategories.COMMON:
        str += this.getArgumentStr(flag) + this.getErrorTypeStrings(this.types) +
          '. Received value is: ' + getTypeName(this.receivedObj);
        break;
      case TypeErrorCategories.SIZE:
        str += this.getArgumentStr(flag) + this.getErrorTypeStrings(this.types) +
          ' and the value cannot be negative. Received value is: ' + getTypeName(this.receivedObj);
        break;
      case TypeErrorCategories.ENCODING:
        str += this.getArgumentStr(flag) + this.getErrorTypeStrings(this.types) +
          '. the encoding ' + this.receivedObj + ' is unknown';
        break;
      case TypeErrorCategories.PROPERTY:
        str += this.argument + ' cannot be set for the buffer that has only a getter';
      default:
        break;
    }
    return str;
  }

  private getRangeString(): string {
    let str = '';
    switch (this.rangeErrorCat) {
      case RangeErrorCategories.WHOLE:
        str += 'The value of "' + this.argument + '" is out of range. It must be >= ' + this.rangeLeft +
          ' and <= ' + this.rangeRight + '. Received value is: ' + this.receivedObj;
        break;
      case RangeErrorCategories.LEFT:
        str += 'The value of "' + this.argument + '" is out of range. It must be >= ' + this.rangeLeft +
          '. Received value is: ' + this.receivedObj;
        break;
      default:
        break;
    }
    return str;
  }

  public getString(): string {
    let str = '';
    switch (this.errorNumber) {
      case ERROR_CODES.TYPE_ERROR:
        str = this.getTypeString(true);
        break;
      case ERROR_CODES.PROPERTY_TYPE_ERROR:
        str = this.getTypeString(false);
        break;
      case ERROR_CODES.RANGE_ERROR:
        str = this.getRangeString();
        break;
      case ERROR_CODES.BUFFER_SIZE_ERROR:
        str = 'The buffer size must be a multiple of ' + this.receivedObj;
        break;
      default:
        break;
    }
    return str;
  }
}

const initialPoolSize: number = 8 * 1024; // 8 * 1024 : initialPoolSize number
let poolSize: number;
let poolOffset: number;
let pool: FastBuffer;
const oneByte: number = 1; // 1 : one Byte
const twoBytes: number = 2; // 2 : two Bytes
const threeBytes: number = 3; // 3 : three Bytes
const fourBytes: number = 4; // 4 : four Bytes
const fiveBytes: number = 5; // 5 : five Bytes
const sixBytes: number = 6; // 6 : six Bytes
const sevenBytes: number = 7; // 7 : seven Bytes
const eightBytes: number = 8; // 8 : eight Bytes

type TypedArray = Int8Array | Uint8Array | Uint8ClampedArray | Int16Array | Uint16Array |
Int32Array | Uint32Array | Float32Array | Float64Array;
type BackingType = FastBuffer | TypedArray | DataView | ArrayBuffer | SharedArrayBuffer;

const bufferEncoding = ['ascii', 'utf8', 'utf-8', 'utf16le', 'utf-16le', 'ucs2', 'ucs-2',
  'base64', 'base64url', 'latin1', 'binary', 'hex'];

interface FastBuffer {
    length: number;
    buffer: ArrayBuffer;
    byteOffset: number;
    fill(value: string | FastBuffer | Uint8Array | number, offset?: number, end?: number, encoding?: string): FastBuffer;
    compare(target: FastBuffer | Uint8Array, targetStart?: number, targetEnd?: number, sourceStart?: number, sourceEnd?: number): -1 | 0 | 1;
    copy(target: FastBuffer | Uint8Array, targetStart?: number, sourceStart?: number, sourceEnd?: number): number;
    equals(otherBuffer: Uint8Array | FastBuffer): boolean;
    includes(value: string | number | FastBuffer | Uint8Array, byteOffset?: number, encoding?: string): boolean;
    indexOf(value: string | number | FastBuffer | Uint8Array, byteOffset?: number, encoding?: string): number;
    keys(): IterableIterator<number>;
    values(): IterableIterator<number>;
    entries(): IterableIterator<[
        number,
        number
    ]>;
    lastIndexOf(value: string | number | FastBuffer | Uint8Array, byteOffset?: number, encoding?: string): number;
    readBigInt64BE(offset?: number): bigint;
    readBigInt64LE(offset?: number): bigint;
    readBigUInt64BE(offset?: number): bigint;
    readBigUInt64LE(offset?: number): bigint;
    readDoubleBE(offset?: number): number;
    readDoubleLE(offset?: number): number;
    readFloatBE(offset?: number): number;
    readFloatLE(offset?: number): number;
    readInt8(offset?: number): number;
    readInt16BE(offset?: number): number;
    readInt16LE(offset?: number): number;
    readInt32BE(offset?: number): number;
    readInt32LE(offset?: number): number;
    readIntBE(offset: number, byteLength: number): number;
    readIntLE(offset: number, byteLength: number): number;
    readUInt8(offset?: number): number;
    readUInt16BE(offset?: number): number;
    readUInt16LE(offset?: number): number;
    readUInt32BE(offset?: number): number;
    readUInt32LE(offset?: number): number;
    readUIntBE(offset: number, byteLength: number): number;
    readUIntLE(offset: number, byteLength: number): number;
    subarray(start?: number, end?: number): FastBuffer;
    swap16(): FastBuffer;
    swap32(): FastBuffer;
    swap64(): FastBuffer;
    toJSON(): Object;
    toString(encoding?: string, start?: number, end?: number): string;
    write(str: string, offset?: number, length?: number, encoding?: string): number;
    writeBigInt64BE(value: bigint, offset?: number): number;
    writeBigInt64LE(value: bigint, offset?: number): number;
    writeBigUInt64BE(value: bigint, offset?: number): number;
    writeBigUInt64LE(value: bigint, offset?: number): number;
    writeDoubleBE(value: number, offset?: number): number;
    writeDoubleLE(value: number, offset?: number): number;
    writeFloatBE(value: number, offset?: number): number;
    writeFloatLE(value: number, offset?: number): number;
    writeInt8(value: number, offset?: number): number;
    writeInt16BE(value: number, offset?: number): number;
    writeInt16LE(value: number, offset?: number): number;
    writeInt32BE(value: number, offset?: number): number;
    writeInt32LE(value: number, offset?: number): number;
    writeIntBE(value: number, offset: number, byteLength: number): number;
    writeIntLE(value: number, offset: number, byteLength: number): number;
    writeUInt8(value: number, offset?: number): number;
    writeUInt16BE(value: number, offset?: number): number;
    writeUInt16LE(value: number, offset?: number): number;
    writeUInt32BE(value: number, offset?: number): number;
    writeUInt32LE(value: number, offset?: number): number;
    writeUIntBE(value: number, offset: number, byteLength: number): number;
    writeUIntLE(value: number, offset: number, byteLength: number): number;
}

class FastBuffer extends FastBufferInner {
  constructor(value: number | FastBuffer | Uint8Array | ArrayBuffer | SharedArrayBuffer | Array<number> | string,
    byteOffsetOrEncoding?: number | string, length?: number) {
      super(value, byteOffsetOrEncoding, length);
  }

  toString(encoding: string = 'utf8', start: number = 0, end: number = this.length): string {
    if (!encoding) {
      encoding = 'utf8';
    }
    let enc = encoding.toLowerCase();
    start = isNaN(start) ? 0 : (Number(start) < 0 ? 0 : Number(start));
    end = isNaN(end) ? 0 : Number(end);
    if (start >= this.length || start > end) {
      return '';
    }
    return super.toString(enc, start, end);
  }

  indexOf(value: string | number | FastBuffer | Uint8Array, byteOffset: number = 0, encoding: string = 'utf8'): number {
    if (typeof byteOffset === 'string') {
      encoding = byteOffset;
    }
    if (typeof byteOffset !== 'number') {
      byteOffset = 0;
    }
    if (!encoding) {
      encoding = 'utf8';
    }
    encoding = encoding.toLowerCase();
    return super.indexOf(value, byteOffset, encoding);
  }

  lastIndexOf(value: string | number | FastBuffer | Uint8Array, byteOffset: number = this.length,
    encoding: string = 'utf8'): number {
    if (typeof byteOffset === 'string') {
      encoding = byteOffset;
    }
    if (typeof byteOffset !== 'number') {
      byteOffset = 0;
    }
    if (!encoding) {
      encoding = 'utf8';
    }
    encoding = encoding.toLowerCase();
    return super.lastIndexOf(value, byteOffset, encoding);
  }

  copy(target: FastBuffer | Uint8Array, targetStart: number = 0, sourceStart: number = 0,
    sourceEnd: number = this.length): number {
    targetStart = isNaN(targetStart) ? 0 : Number(targetStart);
    sourceStart = isNaN(sourceStart) ? 0 : Number(sourceStart);
    sourceEnd = isNaN(sourceEnd) ? this.length : Number(sourceEnd);
    return super.copy(target, targetStart, sourceStart, sourceEnd);
  }

  fill(value: string | FastBuffer | Uint8Array | number, offset: number = 0, end: number = this.length,
    encoding: string = 'utf8'): FastBuffer {
    return super.fill(value, offset, end, encoding);
  }

  compare(target: FastBuffer | Uint8Array, targetStart: number = 0, targetEnd: number = target.length,
    sourceStart: number = 0, sourceEnd: number = this.length): 0 | 1 | -1 {
    if (targetStart === null) {
      targetStart = 0;
    }
    if (targetEnd === null) {
      targetEnd = target.length;
    }
    if (sourceStart === null) {
      sourceStart = 0;
    }
    if (sourceEnd === null) {
      sourceEnd = this.length;
    }
    typeErrorCheck(target, ['FastBuffer', 'Uint8Array'], 'target');
    return super.compare(target, targetStart, targetEnd, sourceStart, sourceEnd);
  }

  toJSON(): Object {
    if (this.length <= 0) {
      return { type: 'FastBuffer', data: [] };
    }
    let data = new Array<number>;
    let len = this.length;
    for (let i = 0; i < len; i++) {
      data.push(this[i]);
    }
    return { type: 'FastBuffer', data: data };
  }

  subarray(start: number = 0, end: number = this.length): FastBuffer {
    let newBuf: FastBuffer = new FastBuffer(0);
    start = isNaN(start) ? 0 : Number(start);
    end = isNaN(end) ? 0 : Number(end);
    end = (end > this.length) ? this.length : end;
    if (start < 0 || end < 0 || end <= start) {
      return newBuf;
    }
    return new FastBuffer(this.buffer, start, end - start);
  }

  writeBigInt64BE(value: bigint, offset: number = 0): number {
    if (typeof value !== 'bigint') {
      throw typeError(value, 'value', ['bigint']);
    }
    rangeErrorCheck(value, 'value', -INT64_MAX, INT64_MAX);
    return super.writeBigInt64BE(value, offset);
  }

  writeBigInt64LE(value: bigint, offset: number = 0): number {
    if (typeof value !== 'bigint') {
      throw typeError(value, 'value', ['bigint']);
    }
    rangeErrorCheck(value, 'value', -INT64_MAX, INT64_MAX);
    return super.writeBigInt64LE(value, offset);
  }
  
  writeBigUInt64BE(value: bigint, offset: number = 0): number {
    if (typeof value !== 'bigint') {
      throw typeError(value, 'value', ['bigint']);
    }
    rangeErrorCheck(value, 'value', 0, UINT64_MAX);
    return super.writeBigUInt64BE(value, offset);
  }

  writeBigUInt64LE(value: bigint, offset: number = 0): number {
    if (typeof value !== 'bigint') {
      throw typeError(value, 'value', ['bigint']);
    }
    rangeErrorCheck(value, 'value', 0, UINT64_MAX);
    return super.writeBigUInt64LE(value, offset);
  }

  swap16(): FastBuffer {
    const len = this.length;
    const dealLen: number = twoBytes;
    if (len % dealLen !== 0) {
      throw bufferSizeError('16-bits');
    }
    return this.reverseBits(dealLen);
  }

  swap32(): FastBuffer {
    const len = this.length;
    const dealLen: number = 4; // Process every 4 bits
    if (len % dealLen !== 0) {
      throw bufferSizeError('32-bits');
    }
    return this.reverseBits(dealLen);
  }

  swap64(): FastBuffer {
    const len = this.length;
    const dealLen: number = eightBytes;
    if (len % dealLen !== 0) {
      throw bufferSizeError('64-bits');
    }
    return this.reverseBits(dealLen);
  }

  reverseBits(dealNum: number): FastBuffer {
    const len: number = this.length;
    const dealLen: number = dealNum;
    for (let i = 0; i < len / dealLen; i++) {
      let times: number = 0;
      let startIndex: number = dealLen * i;
      let endIndex: number = startIndex + dealLen - 1;
      while (times < dealLen / twoBytes) {
        let tmp = this[startIndex + times];
        this[startIndex + times] = this[endIndex - times];
        this[endIndex - times] = tmp;
        times++;
      }
    }
    return this;
  }

  [Symbol.iterator](): IterableIterator<[number, number]> {
    return this.entries();
  }
}

function createBufferFromArrayBuffer(value: ArrayBuffer | SharedArrayBuffer,
  offsetOrEncoding?: number | string, length?: number): FastBuffer {
  offsetOrEncoding = isNaN(Number(offsetOrEncoding)) ? 0 : Number(offsetOrEncoding);
  const maxLength: number = value.byteLength - offsetOrEncoding;
  if (length === undefined) {
    length = maxLength;
  } else {
    length = isNaN(Number(length)) ? 0 : Number(length);
  }
  rangeErrorCheck(offsetOrEncoding, 'byteOffset', 0, value.byteLength);
  rangeErrorCheck(length, 'length', 0, maxLength);
  return new FastBuffer(value, offsetOrEncoding, length);
}

function from(value: FastBuffer | Uint8Array | ArrayBuffer | SharedArrayBuffer | string | object | Array<number>,
  offsetOrEncoding?: number | string, length?: number): FastBuffer {
  if (typeof value === 'string') {
    if (!offsetOrEncoding || typeof offsetOrEncoding === 'number') {
      offsetOrEncoding = 'utf8';
    }
    return fromString(value, offsetOrEncoding);
  }
  if (value instanceof FastBuffer || value instanceof Uint8Array || value instanceof Array) {
    return new FastBuffer(value);
  }
  if (value instanceof ArrayBuffer || value instanceof SharedArrayBuffer) {
    return createBufferFromArrayBuffer(value, offsetOrEncoding, length);
  }
  if (typeof value === 'object' && value !== null) {
    const valueOf = value.valueOf && value.valueOf();
    if (valueOf != null && valueOf !== value &&
      (typeof valueOf === 'string' || typeof valueOf === 'object')) {
      return from(valueOf, offsetOrEncoding, length);
    }
    if (typeof value[Symbol.toPrimitive] === 'function') {
      const primitive = value[Symbol.toPrimitive]('string');
      if (typeof primitive === 'string' && typeof offsetOrEncoding === 'string') {
        return fromString(primitive, offsetOrEncoding);
      }
    }
  }
  throw typeError(value, 'value', ['FastBuffer', 'ArrayBuffer', 'Array', 'Array-like', 'string', 'object']);
}

function alloc(size: number, fill?: string | FastBuffer | number, encoding?: string): FastBuffer {
  if (size < 0 || size > UINT32MAX) {
    let msg = 'Parameter error. The type of "size" must be ' + 'number' +
      ' and the value cannot be negative. Received value is: ' + Number(size).toString();
    throw new BusinessError(msg, errorMap.typeError);
  }
  const buf = new FastBuffer(size);
  if (arguments.length === twoBytes && fill !== undefined && fill !== 0) {
    buf.fill(fill);
    return buf;
  } else if (arguments.length === 3) { // 3 is array->maxIndex
    if (!encoding) {
      encoding = 'utf-8';
    }
    buf.fill(fill, 0, buf.length, encoding);
    return buf;
  }
  buf.fill(0);
  return buf;
}

function allocUninitialized(size: number): FastBuffer {
  if (size < 0 || size > UINT32MAX) {
    let msg = 'Parameter error. The type of "size" must be ' + 'number' +
      ' and the value cannot be negative. Received value is: ' + Number(size).toString();
    throw new BusinessError(msg, errorMap.typeError);
  }
  const buf = new FastBuffer(size);
  return buf;
}

function bufferSizeError(size: string): BusinessError {
  let msg = new ErrorMessage(errorMap.bufferSizeError).setSizeInfo(size).getString();
  return new BusinessError(msg, errorMap.bufferSizeError);
}

function typeError(param: unknown, paramName: string, excludedTypes: string[]): BusinessError {
  let msg = new ErrorMessage(errorMap.typeError, paramName).setTypeInfo(excludedTypes, param).getString();
  return new BusinessError(msg, errorMap.typeError);
}

function rangeErrorCheck(param: number | bigint, paramName: string,
                         rangeLeft: number | bigint, rangeRight: number | bigint): void {
  if (param < rangeLeft || param > rangeRight) {
    throw rangeError(paramName, rangeLeft, rangeRight, param);
  }
}

function rangeError(paramName: string, rangeLeft: string | bigint | number, rangeRight: string | bigint | number,
  receivedValue: number | bigint): BusinessError {
  let msg =
    new ErrorMessage(errorMap.rangeError, paramName).setRangeInfo(rangeLeft, rangeRight, receivedValue).getString();
  return new BusinessError(msg, errorMap.rangeError);
}

function fromString(value: string, encoding: string): FastBuffer {
  let enc = encoding.toLowerCase();
  return new FastBuffer(value, enc);
}

function createPool(): void {
  poolSize = initialPoolSize;
  pool = new FastBuffer(poolSize);
  poolOffset = 0;
}

function alignPool(): void {
  if (poolOffset & 0x7) {
    poolOffset |= 0x7; // 0x7 : align offset based of 8-bits
    poolOffset++;
  }
}

function allocUninitializedFromPool(size: number): FastBuffer {
  if (size < 0 || size > UINT32MAX) {
    let msg = 'Parameter error. The type of "size" must be ' + 'number' +
      ' and the value cannot be negative. Received value is: ' + Number(size).toString();
    throw new BusinessError(msg, errorMap.typeError);
  }
  return new FastBuffer(size);
}

function getBase64ByteLength(str: string): number {
  let bytes = str.length;
  let pos = 0;
  while (bytes > 1 && (pos = str.indexOf('=', pos)) !== -1) { // Find '=' in str and calculate the length of str
    bytes--;
    pos++;
  }
  return (bytes * threeBytes) >>> twoBytes;
}

function getUtf8ByteLength(str: string): number {
  let byteLength = 0;
  for (let i = 0; i < str.length; i++) {
    const code = str.charCodeAt(i);
    if (code >= 0xD800 && code <= 0xDBFF) {
      const nextCode = str.charCodeAt(i + 1);
      if (nextCode >= 0xDC00 && nextCode <= 0xDFFF) {
        byteLength += 4;
        i++;
        continue;
      }
    }
    if (code <= 0x7F) {
      byteLength += 1;
    } else if (code <= 0x7FF) {
      byteLength += 2;
    } else if (code <= 0xFFFF) {
      byteLength += 3;
    }
  }
  return byteLength;
}

function getEncodingByteLength(str: string, type: string): number {
  type = type.toLowerCase();
  switch (type) {
    case 'utf8':
    case 'utf-8':
      return getUtf8ByteLength(str);
    case 'ucs2':
    case 'ucs-2':
      return str.length * twoBytes;
    case 'ascii':
      return str.length;
    case 'binary':
    case 'latin1':
      return str.length;
    case 'utf16le':
    case 'utf-16le':
      return str.length * twoBytes;
    case 'base64':
    case 'base64url':
      return getBase64ByteLength(str);
    case 'hex':
      return str.length >>> 1; // 1 : one-half
    default:
      return undefined;
  }
}

function isTypedArray(self: unknown): boolean {
  let typeArr = [Int8Array, Uint8Array, Uint8ClampedArray, Int16Array, Uint16Array,
    Int32Array, Uint32Array, Float32Array, Float64Array];
  for (let i = 0, len = typeArr.length; i < len; i++) {
    if (self instanceof typeArr[i]) {
      return true;
    }
  }
  return false;
}

function byteLength(string: string | BackingType, encoding: string = 'utf8'): number {
    if (string instanceof FastBuffer) {
      return string.length;
    } else if (typeof string === 'string') {
      if (string.length === 0) {
        return 0;
      }
      if (!encoding) {
        encoding = 'utf8';
      }
      return getEncodingByteLength(string, encoding);
    } else {
        if (isTypedArray(string) || string instanceof DataView ||
          string instanceof ArrayBuffer || string instanceof SharedArrayBuffer) {
          return string.byteLength;
        }
        throw typeError(string, 'string', ['string', 'FastBuffer', 'ArrayBuffer']);
    }
}


function isBuffer(obj: Object): boolean {
  return obj instanceof FastBuffer;
}

function isEncoding(enc: string): boolean {
  if (!enc) {
    return false;
  }
  enc = enc.toLowerCase();
  if (bufferEncoding.includes(enc)) {
    return true;
  }
  return false;
}

function transcode(source: FastBuffer | Uint8Array, fromEnc: string, toEnc: string): FastBuffer {
  typeErrorCheck(source, ['FastBuffer', 'Uint8Array'], 'source');
  typeErrorCheck(fromEnc, ['string'], 'fromEnc');
  typeErrorCheck(toEnc, ['string'], 'toEnc');
  let from = source.toString(fromEnc);
  return fromString(from, toEnc);
}

function compare(buf1: FastBuffer | Uint8Array, buf2: FastBuffer | Uint8Array): 1 | 0 | -1 {
  if (!(buf1 instanceof FastBuffer) && !(buf1 instanceof Uint8Array)) {
    throw new BusinessError(new ErrorMessage(errorMap.typeError, 'buf1').setTypeInfo(['FastBuffer', 'Uint8Array'],
      getTypeName(buf1)).getString(), errorMap.typeError);
  }
  if (!(buf2 instanceof FastBuffer) && !(buf2 instanceof Uint8Array)) {
    throw new BusinessError(new ErrorMessage(errorMap.typeError, 'buf2').setTypeInfo(['FastBuffer', 'Uint8Array'],
      getTypeName(buf2)).getString(), errorMap.typeError);
  }

  let tempBuf: FastBuffer;
  if (buf1 instanceof FastBuffer) {
    return buf1.compare(buf2, 0, buf2.length, 0, buf1.length);
  } else {
    tempBuf = new FastBuffer(buf1);
    return tempBuf.compare(buf2, 0, buf2.length, 0, tempBuf.length);
  }
}

function typeErrorCheck(param: unknown, types: string[], paramName: string): void {
  let typeName = getTypeName(param);
  if (!types.includes(typeName)) {
    throw typeError(param, paramName, types);
  }
}

function concat(list: FastBuffer[] | Uint8Array[], totalLength?: number): FastBuffer {
  typeErrorCheck(list, ['Array'], 'list');
  if (!(typeof totalLength === 'number' || typeof totalLength === 'undefined' || 'null')) {
    throw typeError(totalLength, 'totalLength', ['number']);
  }
  if (list.length === 0) {
    return new FastBuffer(0);
  }
  if (!totalLength) {
    totalLength = 0;
    for (let i = 0, len = list.length; i < len; i++) {
      let buf = list[i];
      if (buf instanceof Uint8Array || buf instanceof FastBuffer) {
        totalLength += list[i].length;
      }
    }
  }

  rangeErrorCheck(totalLength, 'totalLength', 0, UINT32MAX);

  let buffer = allocUninitializedFromPool(totalLength);
  let offset = 0;
  for (let i = 0, len = list.length; i < len; i++) {
    const buf = list[i];
    if (buf instanceof Uint8Array) {
      buf.forEach((val) => buffer[offset++] = val);
    } else if (buf instanceof FastBuffer) {
      buf.copy(buffer, offset);
      offset += buf.length;
    }
  }
  return buffer;
}

export default {
  FastBuffer,
  from,
  alloc,
  allocUninitializedFromPool,
  allocUninitialized,
  byteLength,
  isBuffer,
  isEncoding,
  compare,
  concat,
  transcode
};
