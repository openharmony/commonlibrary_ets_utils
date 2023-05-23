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
  errorNumber: number;
  constructor(message: string, errorNumber: number) {
    super(message);
    this.name = 'BusinessError';
    this.errorNumber = errorNumber;
  }
}

let errorMap = {
  'typeError': 401,
  'rangeError': 10200001,
  'bufferSizeError': 10200009,
  'typeErrorForProperty': 10200013
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

const UINT32MAX = 4294967296;

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

  private getArgumentStr(): string {
    return 'The type of "' + this.argument + '" must be ';
  }

  private getTypeString(): string {
    let str = '';
    switch (this.typeErrorCat) {
      case TypeErrorCategories.COMMON:
        str += this.getArgumentStr() + this.getErrorTypeStrings(this.types) +
          '. Received value is: ' + getTypeName(this.receivedObj);
        break;
      case TypeErrorCategories.SIZE:
        str += this.getArgumentStr() + this.getErrorTypeStrings(this.types) +
          ' and the value cannot be negative. Received value is: ' + getTypeName(this.receivedObj);
        break;
      case TypeErrorCategories.ENCODING:
        str += this.getArgumentStr() + this.getErrorTypeStrings(this.types) +
          '. the encoding ' + this.receivedObj + ' is unknown';
        break;
      case TypeErrorCategories.PROPERTY:
        str += 'Cannot set property ' + this.argument + ' of Buffer which has only a getter';
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
      case 401: //TypeError
      case 10200013: //TypeErrorForProperty
        str = this.getTypeString();
        break;
      case 10200001: //RangeError
        str = this.getRangeString();
        break;
      case 10200009: //BufferSizeError
        str = 'Buffer size must be a multiple of ' + this.receivedObj;
        break;
      default:
        break;
    }
    return str;
  }
}

interface NativeBuffer {
  new(type: number, length: number): NativeBuffer;
  new(type: number, array: number[]): NativeBuffer;
  new(type: number, raw: NativeBuffer): NativeBuffer;
  new(type: number, str: string, encoding: string): NativeBuffer;
  new(type: number, arrayBuffer: ArrayBuffer, byteOffset?: number, length?: number): NativeBuffer;
  new(type: number, pool: NativeBuffer, poolOffset: number, length: number): NativeBuffer;
  setArray(array: Array<number>): undefined;
  getLength(): number;
  getByteOffset(): number;
  writeString(value: string, offset: number, length: number, encoding: string): number;
  fromString(str: string, encoding: string, size: number): NativeBuffer;
  fillString(value: string, offset: number, end: number, encoding: string): undefined;
  fillNumbers(value: number[], offset: number, end: number): undefined;
  fillBuffer(value: NativeBuffer, offset: number, end: number): undefined;
  writeInt32BE(value: number, offset: number): number;
  readInt32BE(offset: number): number;
  writeInt32LE(value: number, offset: number): number;
  readInt32LE(offset: number): number;
  writeUInt32BE(value: number, offset: number): number;
  readUInt32BE(offset: number): number;
  writeUInt32LE(value: number, offset: number): number;
  readUInt32LE(offset: number): number;
  getBufferData(): Array<number>;
  get(index: number): number;
  set(index: number, value: number): undefined;
  subBuffer(start: number, end: number): NativeBuffer;
  copy(target: NativeBuffer, targetStart: number, sourceStart: number, sourceEnd: number): number;
  compare(target: NativeBuffer, targetStart: number, sourceStart: number, length: number): number;
  toUtf8(start: number, end: number): string;
  toBase64(start: number, end: number): string;
  indexOf(value: string, byteOffset: number, encoding: string, isReverse: boolean): number;
}
interface NativeBlob {
  new(src: Array<number>): NativeBlob;
  new(blob: NativeBlob, start: number, end?: number): NativeBlob;
  arraybuffer(): Promise<ArrayBuffer>;
  text(): Promise<string>;
  getBytes(): Array<number>;
}
interface IBuffer {
  Buffer: NativeBuffer;
  Blob: NativeBlob;
  utf8ByteLength(str: string): number;
  utf8StringToNumbers(str: string): Array<number>;
}

declare function requireInternal(s: string): IBuffer;
const internalBuffer = requireInternal('buffer');
const bufferSymbol = Symbol('bufferClass');
const lengthSymbol = Symbol('bufferLength');
const bufferEncoding = ['ascii', 'utf8', 'utf-8', 'utf16le', 'utf-16le', 'ucs2', 'ucs-2',
  'base64', 'base64url', 'latin1', 'binary', 'hex'];

enum ParaType {
  NUMBER = 0,
  BUFFER,
  UINT8ARRAY,
  ARRAYBUFFER,
  NUMBERS,
  STRING
}
const initialPoolSize: number = 8 * 1024;
let poolSize: number;
let poolOffset: number;
let pool: Buffer;
const MAX_LENGTH = Math.pow(2, 32);
const oneByte: number = 1;
const twoBytes: number = 2;
const threeBytes: number = 3;
const fourBytes: number = 4;
const fiveBytes: number = 5;
const sixBytes: number = 6;
const sevenBytes: number = 7;
const eightBytes: number = 8;


type TypedArray = Int8Array | Uint8Array | Uint8ClampedArray | Int16Array | Uint16Array |
  Int32Array | Uint32Array | Float32Array | Float64Array;
type BackingType = Buffer | TypedArray | DataView | ArrayBuffer | SharedArrayBuffer;
const float64Array: Float64Array = new Float64Array(1);
const uInt8Float64Array: Uint8Array = new Uint8Array(float64Array.buffer);
const float32Array: Float32Array = new Float32Array(1);
const uInt8Float32Array: Uint8Array = new Uint8Array(float32Array.buffer);

function createPool(): void {
  poolSize = initialPoolSize;
  pool = new Buffer(poolSize);
  poolOffset = 0;
}

function alignPool(): void {
  if (poolOffset & 0x7) {
    poolOffset |= 0x7; // 0x7 : align offset based of 8-bits
    poolOffset++;
  }
}
type Options = {
  type: string,
  endings: string
};

const _log = console.log;

console.log = function (...args) {
  if (args.length === 1 && args[0] instanceof Buffer) {
    let buf: Buffer = args[0];
    let bufArr: Array<number> = buf[bufferSymbol].getBufferData();
    let getStr = function (bufArr: Array<number>, len: number) {
      let str = '';
      for (let i = 0; i < len; i++) {
        let strTemp = bufArr[i].toString(16);
        strTemp = (strTemp.length === 1) ? `0${strTemp}` : strTemp;
        str += ` ${strTemp}`;
      }
      return str;
    };
    let msg = '';
    if (bufArr.length > 50) {
      let bufStr = getStr(bufArr, 50); // 50: Maximum number of log displays
      msg = bufStr + ` ... ${bufArr.length - 50} more bytes`;
    } else {
      msg = getStr(bufArr, bufArr.length);
    }
    _log.call(console, `<Buffer ${msg}>`);
  } else {
    _log(...args);
  }
};

class Blob {
  blobClass: NativeBlob;
  private _size: number;
  private _type: string;

  public get size(): number {
    return this._size;
  }

  public get type(): string {
    return this._type;
  }

  constructor(sources: string[] | ArrayBuffer[] | TypedArray[] | DataView[] | Blob[], options: Options) {
    if (options === undefined) {
      options = {
        type: '',
        endings: 'transparent'
      };
    }
    typeErrorCheck(options, ['Object'], 'options');

    let type = options.type ? options.type : '';
    let endings = options.endings ? options.endings : 'transparent';
    if (endings !== 'transparent' && endings !== 'native') {
      throw new BusinessError('invalid arg value of options.endings', errorMap.typeError);
    }
    let arr: Array<number> = [];
    if (sources instanceof Array || isTypedArray(sources)) {
      for (const value of sources) {
        arr = arr.concat(this.normalizeSource(value));
      }
    } else {
      throw typeError(sources, 'sources', ['Iterable']);
    }
    this._size = arr.length;
    this._type = type;
    this.blobClass = new internalBuffer.Blob(arr);
  }

  normalizeSource(source: string | ArrayBuffer | TypedArray | DataView | Blob | Buffer): Array<number> {
    let ret: Array<number> = [];
    if (typeof source === 'string') {
      return internalBuffer.utf8StringToNumbers(source);
    } else if (source instanceof ArrayBuffer) {
      return Array.prototype.slice.call(new Uint8Array(source));
    } else if (isTypedArray(source)) {
      let numbers = Array.prototype.slice.call(source);
      let str = '';
      for (let i = 0, len = numbers.length; i < len; i++) {
        str += numbers[i].toString();
      }
      let charCodeArr = [];
      for (let i = 0, len = str.length; i < len; i++) {
        let code = str.charCodeAt(i);
        charCodeArr.push(code);
      }
      return charCodeArr;
    } else if (source instanceof DataView) {
      return Array.prototype.slice.call(new Uint8Array(source.buffer));
    } else if (source instanceof Blob) {
      return source.blobClass.getBytes();
    } else if (source instanceof Buffer) {
      for (let i = 0, len = source.length; i < len; i++) {
        ret[i] = source[i];
      }
      return ret;
    }
    return [];
  }

  arrayBuffer(): Promise<ArrayBuffer> {
    return this.blobClass.arraybuffer();
  }

  text(): Promise<string> {
    return this.blobClass.text();
  }

  slice(start?: number, end?: number, type?: string): Blob {
    let newBlob = Object.create(this);
    if (type !== undefined) {
      newBlob._type = type;
    }
    if (start === undefined) {
      return newBlob;
    }
    if (end === undefined) {
      newBlob.blobClass = new internalBuffer.Blob(this.blobClass, start);
      return newBlob;
    }
    if (start > end) {
      return newBlob;
    }
    if ((start > 0 && end < 0) || (start < 0 && end > 0)) {
      return newBlob;
    }
    newBlob.blobClass = new internalBuffer.Blob(this.blobClass, start, end);
    return newBlob;
  }
}

let utils = {
  eightBits: 0xFF,
  sixtyFourBit: 0xFFFFFFFFn,

  getLowerEight(value: number): number {
    return value & this.eightBits;
  },
  getLowerSixtyFour(value: bigint): bigint {
    return value & this.sixtyFourBit;
  }
};

enum Style {
  intBE = 0,
  intLE,
  uintBE,
  uintLE
};

class HandlerBuffer {
  get(obj: Buffer, prop: string): number | undefined {
    if (typeof prop === 'number') {
      if (prop >= obj.length) {
        return obj[prop];
      }
      return obj[bufferSymbol].get(prop);
    }
    return obj[prop];
  }
  set(obj: Buffer, prop: string | symbol, value: number): boolean {
    if (typeof prop === 'number') {
      if (prop >= obj.length) {
        return false;
      }
      value = utils.getLowerEight(value);
      obj[bufferSymbol].set(prop, value);
      return true;
    }

    if (prop === lengthSymbol || prop === bufferSymbol) {
      obj[prop] = value;
      return true;
    } else if (prop === 'length' || prop === 'buffer' || prop === 'byteOffset') {
      throw typeErrorForProperty(prop);
    }
    return false;
  }
  ownKeys(obj: Buffer): Array<string> {
    let keys: Array<string> = [];
    for (let i = 0, len = obj.length; i < len; i++) {
      keys.push(i.toString());
    }
    return keys;
  }
}
class Buffer {
  private _arrayBuffer: ArrayBuffer | undefined;

  public get length(): number {
    return this[lengthSymbol];
  }

  public get byteOffset(): number {
    return this[bufferSymbol].getByteOffset();
  }

  public get buffer(): ArrayBufferLike {
    if (this._arrayBuffer) {
      return this._arrayBuffer;
    }
    let arr = this[bufferSymbol].getBufferData();
    return new Uint8Array(arr).buffer;
  }

  constructor(value: number | Buffer | Uint8Array | ArrayBuffer | Array<number> | string,
    byteOffsetOrEncoding?: number | string, length?: number) {
    if (arguments.length === 1) {
      if (typeof value === 'number') {
        this[bufferSymbol] = new internalBuffer.Buffer(ParaType.NUMBER, value);
      } else if (value instanceof Buffer) {
        this[bufferSymbol] = new internalBuffer.Buffer(ParaType.BUFFER, value[bufferSymbol]);
      } else if (value instanceof Uint8Array) {
        this[bufferSymbol] = new internalBuffer.Buffer(ParaType.UINT8ARRAY, value);
      } else if (value instanceof Array) {
        this[bufferSymbol] = new internalBuffer.Buffer(ParaType.NUMBERS, value);
      }
    } else if (arguments.length === 3 && typeof byteOffsetOrEncoding === 'number' && typeof length === 'number') {
      if (value instanceof ArrayBuffer) {
        this[bufferSymbol] = new internalBuffer.Buffer(ParaType.ARRAYBUFFER, value, byteOffsetOrEncoding, length);
        this._arrayBuffer = value;
      } else if (value instanceof Buffer) {
        this[bufferSymbol] = new internalBuffer.Buffer(ParaType.BUFFER, value[bufferSymbol], byteOffsetOrEncoding, length);
      }
    } else if (arguments.length === 2 && typeof value === 'string' && typeof byteOffsetOrEncoding === 'string') {
      this[bufferSymbol] = new internalBuffer.Buffer(ParaType.STRING, value, byteOffsetOrEncoding);
    } else {
      this[bufferSymbol] = new internalBuffer.Buffer(ParaType.NUMBER, 0);
    }
    this[lengthSymbol] = this[bufferSymbol].getLength();
    return new Proxy(this, new HandlerBuffer());
  }

  /**
   * Fills buf with the specified value. If the offset and end are not given, the entire buf will be filled.
   * @since 9
   * @syscap SystemCapability.Utils.Lang 
   * @param value The value with which to fill buf
   * @param [offset = 0] Number of bytes to skip before starting to fill buf
   * @param [end = buf.length] Where to stop filling buf (not inclusive)
   * @param [encoding='utf8'] The encoding for value if value is a string
   * @return A reference to buf
   */
  fill(value: string | Buffer | Uint8Array | number, offset: number = 0, end: number = this.length,
    encoding: string = 'utf8'): Buffer {
    if (this.length === 0) {
      return this;
    }
    if (arguments.length === 2) {
      if (typeof offset === 'string') {
        encoding = offset;
        offset = 0;
      }
    } else if (arguments.length === 3) {
      if (typeof end === 'string') {
        encoding = end;
        end = this.length;
      }
    }
    if (typeof offset !== 'number') {
      typeErrorCheck(offset, ['number'], 'offset');
    }
    if (typeof end === 'number') {
      typeErrorCheck(end, ['number'], 'end');
    }
    if (typeof encoding !== 'string') {
      typeErrorCheck(encoding, ['string'], 'encoding');
    }

    const normalizedEncoding = encodingTypeErrorCheck(encoding);

    rangeErrorCheck(offset, 'offset', 0, UINT32MAX);
    rangeErrorCheck(end, 'end', 0, this.length);

    if (offset > end - 1) {
      return this;
    }
    if (typeof value === 'string') {
      if (normalizedEncoding === 'hex') {
        let numbers = hexStrtoNumbers(value);
        this[bufferSymbol].fillNumbers(numbers, offset, end);
      } else {
        this[bufferSymbol].fillString(value, offset, end, normalizedEncoding);
      }
    }
    if (typeof value === 'number') {
      let nums: Array<number> = [];
      nums.push(value & 0xFF); // 0xFF : get lower 8-bits
      this[bufferSymbol].fillNumbers(nums, offset, end);
    }
    if (value instanceof Buffer) {
      this[bufferSymbol].fillBuffer(value[bufferSymbol], offset, end);
    }
    if (value instanceof Uint8Array) {
      let nums = Array.from(value);
      this[bufferSymbol].fillNumbers(nums, offset, end);
    }
    return this;
  }

  write(str: string, offset: number = 0, length: number = this.length - offset, encoding: string = 'utf8'): number {
    typeErrorCheck(str, ['string'], 'str');
    if (arguments.length === 1) {
      return this[bufferSymbol].writeString(str, 0, length, 'utf8');
    } else if (arguments.length === 2) {
      if (typeof offset === 'string') {
        encoding = offset;
        let encode = encodingTypeErrorCheck(encoding);
        return this[bufferSymbol].writeString(str, 0, this.length, encode);
      } else if (typeof offset === 'number') {
        rangeErrorCheck(offset, 'offset', 0, this.length - 1);
        return this[bufferSymbol].writeString(str, offset, length, 'utf8');
      } else {
        throw typeError(offset, 'offset', ['number']);
      }
    } else if (arguments.length === 3) {
      typeErrorCheck(offset, ['number'], 'offset');
      rangeErrorCheck(offset, 'offset', 0, this.length - 1);
      if (typeof length === 'number') {
        rangeErrorCheck(length, 'length', 0, this.length);
        length = (length > this.length - offset) ? (this.length - offset) : length;
        return this[bufferSymbol].writeString(str, offset, length, 'utf8');
      } else if (typeof length === 'string') {
        encoding = length;
        length = this.length - offset;
        let encode = encodingTypeErrorCheck(encoding);
        return this[bufferSymbol].writeString(str, offset, length, encode);
      } else {
        throw typeError(length, 'length', ['number']);
      }
    } else {
      if (typeof offset !== 'number') {
        throw typeError(offset, 'offset', ['number']);
      } else if (typeof length !== 'number') {
        throw typeError(length, 'length', ['number']);
      } else {
        rangeErrorCheck(offset, 'offset', 0, this.length - 1);
        let encode = encodingTypeErrorCheck(encoding);
        length = (length > this.length - offset) ? (this.length - offset) : length;
        return this[bufferSymbol].writeString(str, offset, length, encode);
      }
    }
  }

  convertToBig64BE(value: bigint, offset: number): number {
    let byteValue = Number(utils.getLowerSixtyFour(value));
    let bitNum = 8;
    for (let i = bitNum - 1; i > 0; i--) {
      this[offset + i] = byteValue;
      if (i === 4) {
        byteValue = Number(utils.getLowerSixtyFour(value >> 32n));
      } else {
        byteValue = byteValue >> 8; // 8 means offset 8 bits
      }
    }
    this[offset] = byteValue;
    return offset + 8;
  }

  convertToBig64LE(value: bigint, offset: number): number {
    let byteValue = Number(utils.getLowerSixtyFour(value));
    let bitNum = 8;
    for (let i = 0; i < bitNum - 1; i++) {
      this[offset++] = byteValue;
      if (i === 3) {
        byteValue = Number(utils.getLowerSixtyFour(value >> 32n)); // 32 means offset 32-bits
      } else {
        byteValue = byteValue >> 8; // 8 means offset 8 bits
      }
    }
    this[offset++] = byteValue;
    return offset;
  }

  private readData(offset: number, byteLength: number, style: Style): number | undefined {
    rangeErrorCheck(byteLength, 'byteLength', oneByte, sixBytes);
    if (style === Style.intBE) {
      switch (byteLength) {
        case oneByte:
          return this.readInt8(offset);
        case twoBytes:
          return this.readInt16BE(offset);
        case threeBytes:
          return this.readInt24BE(offset);
        case fourBytes:
          return this.readInt32BE(offset);
        case fiveBytes:
          return this.readInt40BE(offset);
        case sixBytes:
          return this.readInt48BE(offset);
        default:
          break;
      }
    } else if (style === Style.intLE) {
      switch (byteLength) {
        case oneByte:
          return this.readInt8(offset);
        case twoBytes:
          return this.readInt16LE(offset);
        case threeBytes:
          return this.readInt24LE(offset);
        case fourBytes:
          return this.readInt32LE(offset);
        case fiveBytes:
          return this.readInt40LE(offset);
        case sixBytes:
          return this.readInt48LE(offset);
        default:
          break;
      }
    } else if (style === Style.uintLE) {
      switch (byteLength) {
        case oneByte:
          return this.readUInt8(offset);
        case twoBytes:
          return this.readUInt16LE(offset);
        case threeBytes:
          return this.readUInt24LE(offset);
        case fourBytes:
          return this.readUInt32LE(offset);
        case fiveBytes:
          return this.readUInt40LE(offset);
        case sixBytes:
          return this.readUInt48LE(offset);
        default:
          break;
      }
    } else if (style === Style.uintBE) {
      switch (byteLength) {
        case oneByte:
          return this.readUInt8(offset);
        case twoBytes:
          return this.readUInt16BE(offset);
        case threeBytes:
          return this.readUInt24BE(offset);
        case fourBytes:
          return this.readUInt32BE(offset);
        case fiveBytes:
          return this.readUInt40BE(offset);
        case sixBytes:
          return this.readUInt48BE(offset);
        default:
          break;
      }
    }
    return undefined;
  }

  private writeData(value: number, offset: number, byteLength: number, style: Style): number | undefined {
    rangeErrorCheck(byteLength, 'byteLength', oneByte, sixBytes);
    if (style === Style.intBE) {
      switch (byteLength) {
        case oneByte:
          return this.writeInt8(value, offset);
        case twoBytes:
          return this.writeInt16BE(value, offset);
        case threeBytes:
          return this.writeUInt24BE(value, offset);
        case fourBytes:
          return this.writeInt32BE(value, offset);
        case fiveBytes:
          return this.writeUInt40BE(value, offset);
        case sixBytes:
          return this.writeUInt48BE(value, offset);
        default:
          break;
      }
    } else if (style === Style.intLE) {
      switch (byteLength) {
        case oneByte:
          return this.writeUInt8(value, offset);
        case twoBytes:
          return this.writeUInt16LE(value, offset);
        case threeBytes:
          return this.writeUInt24LE(value, offset);
        case fourBytes:
          return this.writeUInt32LE(value, offset);
        case fiveBytes:
          return this.writeUInt40LE(value, offset);
        case sixBytes:
          return this.writeUInt48LE(value, offset);
        default:
          break;
      }
    } else if (style === Style.uintLE) {
      switch (byteLength) {
        case oneByte:
          return this.writeUInt8(value, offset);
        case twoBytes:
          return this.writeUInt16LE(value, offset);
        case threeBytes:
          return this.writeUInt24LE(value, offset);
        case fourBytes:
          return this.writeUInt32LE(value, offset);
        case fiveBytes:
          return this.writeUInt40LE(value, offset);
        case sixBytes:
          return this.writeUInt48LE(value, offset);
        default:
          break;
      }
    } else if (style === Style.uintBE) {
      switch (byteLength) {
        case oneByte:
          return this.writeUInt8(value, offset);
        case twoBytes:
          return this.writeUInt16BE(value, offset);
        case threeBytes:
          return this.writeUInt24BE(value, offset);
        case fourBytes:
          return this.writeUInt32BE(value, offset);
        case fiveBytes:
          return this.writeUInt40BE(value, offset);
        case sixBytes:
          return this.writeUInt48BE(value, offset);
        default:
          break;
      }
    }
    return undefined;
  }

  writeBigInt64BE(value: bigint, offset: number = 0): number {
    typeErrorCheck(value, ['bigint'], 'value');
    typeErrorCheck(offset, ['number'], 'offset');
    this.checkOffsetRange(offset, eightBytes);
    // 2n : 63n : 1n : The range of 64-bit BigInt value is from negative the 63st power of 2 to the 63st power of 2 minus 1
    rangeErrorCheck(value, 'value', -(2n ** 63n), 2n ** 63n, '-(2n ** 63n)', '2n ** 63n');
    return this.convertToBig64BE(value, offset);
  }

  readBigInt64BE(offset: number = 0): bigint {
    typeErrorCheck(offset, ['number'], 'offset');
    this.checkOffsetRange(offset, eightBytes);
    const val = (this[offset] << 24) + this.calculationBE(offset + 1, 3); // 24 : 3 : the first val for this[offset]
    return (BigInt(val) << 32n) + // 32 means offset 32-bits left
      BigInt(this.calculationBE(offset + 4, 4)); // 4 means offset 4 bytes
  }

  writeBigInt64LE(value: bigint, offset: number = 0): number {
    typeErrorCheck(value, ['bigint'], 'value');
    typeErrorCheck(offset, ['number'], 'offset');
    this.checkOffsetRange(offset, eightBytes);
    // 2n : 63n : 1n : The range of 64-bit BigInt value is from negative the 63st power of 2 to the 63st power of 2 minus 1
    rangeErrorCheck(value, 'value', -(2n ** 63n), 2n ** 63n, '-(2n ** 63n)', '2n ** 63n');

    return this.convertToBig64LE(value, offset);
  }

  private calculationLE(offset: number, count: number): number {
    let result: number = 0;
    for (let i = 0; i < count; i++) {
      result += this[offset++] * Math.pow(2, 8 * i); // 2 : 8 : Represents offset
    }
    return result;
  }

  private calculationBE(offset: number, count: number): number {
    let result: number = 0;
    for (let i = count - 1; i >= 0; i--) {
      result += this[offset++] * Math.pow(2, 8 * i); // 2 : 8 : Represents offset
    }
    return result;
  }

  readBigInt64LE(offset: number = 0): bigint {
    typeErrorCheck(offset, ['number'], 'offset');
    this.checkOffsetRange(offset, eightBytes);
    const val = this.calculationLE(offset + 4, 3) + (this[offset + 7] << 24); // 24 means offset 24-bits left
    return (BigInt(val) << 32n) + BigInt(this.calculationLE(offset, 4)); // 32 means offset 32-bits left
  }

  writeBigUInt64BE(value: bigint, offset: number = 0): number {
    typeErrorCheck(value, ['bigint'], 'value');
    typeErrorCheck(offset, ['number'], 'offset');
    this.checkOffsetRange(offset, eightBytes);
    // 2n : 64n : 1n : The range of 64-bit BigUInt value is from 0 to the 64st power of 2 minus 1
    rangeErrorCheck(value, 'value', 0, 2n ** 64n - 1n, '0', '2n ** 64n - 1n');

    return this.convertToBig64BE(value, offset);
  }

  readBigUInt64BE(offset: number = 0): bigint {
    typeErrorCheck(offset, ['number'], 'offset');
    this.checkOffsetRange(offset, eightBytes);
    const hi = this.calculationBE(offset, 4); // 4 means offset 4 bytes

    const lo = this.calculationBE(offset + 4, 4); // 4 means offset 4 bytes
    return (BigInt(hi) << 32n) + BigInt(lo); // 32 means offset 32-bits left
  }

  writeBigUInt64LE(value: bigint, offset: number = 0): number {
    typeErrorCheck(value, ['bigint'], 'value');
    typeErrorCheck(offset, ['number'], 'offset');
    this.checkOffsetRange(offset, eightBytes);
    rangeErrorCheck(value, 'value', 0, 2n ** 64n - 1n, '0', '2n ** 64n - 1n'); // 2n : 64n : 1n : The range of BigUInt

    return this.convertToBig64LE(value, offset);
  }

  readBigUInt64LE(offset: number = 0): bigint {
    typeErrorCheck(offset, ['number'], 'offset');
    this.checkOffsetRange(offset, eightBytes);
    const lo = this.calculationLE(offset, 4);
    const hi = this.calculationLE(offset + 4, 4);
    return BigInt(lo) + (BigInt(hi) << 32n); // 32 means offset 32-bits left
  }

  writeInt8(value: number, offset: number = 0): number {
    typeErrorCheck(value, ['number'], 'value');
    typeErrorCheck(offset, ['number'], 'offset');
    this.checkOffsetRange(offset, oneByte);
    rangeErrorCheck(value, 'value', - (Math.pow(2, 7)), (Math.pow(2, 7) - 1)); // 2 : 7 : The range of 8-bit Int value
    value = +value;

    this[offset] = value;
    return offset + 1;
  }

  readInt8(offset: number = 0): number {
    typeErrorCheck(offset, ['number'], 'offset');
    this.checkOffsetRange(offset, oneByte);
    const val = this[offset];

    return val | (val & Math.pow(2, 7)) * 0x1fffffe;
  }

  writeInt16BE(value: number, offset: number = 0): number {
    typeErrorCheck(value, ['number'], 'value');
    typeErrorCheck(offset, ['number'], 'offset');
    this.checkOffsetRange(offset, twoBytes);
    rangeErrorCheck(value, 'value', - (Math.pow(2, 15)), (Math.pow(2, 15) - 1)); // 2 : 15 : The range of 16-bit
    value = +value;
    this[offset++] = (value >>> 8); // 8 : calculation offset
    this[offset++] = value;
    return offset;
  }

  readInt16BE(offset: number = 0): number {
    typeErrorCheck(offset, ['number'], 'offset');
    this.checkOffsetRange(offset, twoBytes);
    const val = this.calculationBE(offset, 2); // 2 : calculation offset
    return val | (val & Math.pow(2, 15)) * 0x1fffe; // 2 : 15 : 0x1fffe : The number of 2 bytes changes to 4 bytes
  }

  writeInt16LE(value: number, offset: number = 0): number {
    typeErrorCheck(value, ['number'], 'value');
    typeErrorCheck(offset, ['number'], 'offset');
    this.checkOffsetRange(offset, twoBytes);
    rangeErrorCheck(value, 'value', -(Math.pow(2, 15)), (Math.pow(2, 15) - 1)); // 2 : 15 : The range of 16-bit
    value = +value;
    this[offset++] = value;
    this[offset++] = (value >>> 8);
    return offset;
  }

  readInt16LE(offset: number = 0): number {
    typeErrorCheck(offset, ['number'], 'offset');
    this.checkOffsetRange(offset, twoBytes);
    const val = this[offset] + this[offset + 1] * Math.pow(2, 8); // 8 : means offset 8 bits
    return val | (val & Math.pow(2, 15)) * 0x1fffe; // 2 : 15 : 0x1fffe : The number of 2 bytes changes to 4 bytes
  }

  readUInt16LE(offset: number = 0): number {
    typeErrorCheck(offset, ['number'], 'offset');
    this.checkOffsetRange(offset, twoBytes);
    return this[offset] + this[offset + 1] * Math.pow(2, 8); // 8 means offset 8 bits
  }

  writeUInt8(value: number, offset: number = 0): number {
    typeErrorCheck(value, ['number'], 'value');
    typeErrorCheck(offset, ['number'], 'offset');
    this.checkOffsetRange(offset, oneByte);
    rangeErrorCheck(value, 'value', 0, 255); // 255 : The range of 8-bit UInt value is from 0 to the 8st power of 2 minus 1
    value = +value;

    this[offset] = value;
    return offset + 1;
  }

  readUInt8(offset: number = 0): number {
    typeErrorCheck(offset, ['number'], 'offset');
    this.checkOffsetRange(offset, oneByte);
    const val = this[offset];
    return val;
  }

  writeIntBE(value: number, offset: number, byteLength: number): number | undefined {
    typeErrorCheck(value, ['number'], 'value');
    typeErrorCheck(offset, ['number'], 'offset');
    typeErrorCheck(byteLength, ['number'], 'byteLength');
    return this.writeData(value, offset, byteLength, Style.intBE);
  }

  private writeUInt24BE(value: number, offset: number = 0): number {
    this.checkOffsetRange(offset, threeBytes);
    // 2 : 24 : The range of 24-bit UInt value is from 0 to the 24st power of 2 minus 1
    rangeErrorCheck(value, 'value', 0, 2 ** 24 - 1, '0', '2**24 - 1');
    value = +value;
    for (let i: number = 2; i > 0; i--) {
      this[offset + i] = value;
      value = value >>> 8; // 8 means offset 8 bits
    }
    this[offset] = value;
    return offset + 3; // 3 means offset 3 bytes
  }

  private writeUInt40BE(value: number, offset: number = 0): number {
    this.checkOffsetRange(offset, fiveBytes);
    // 2 : 40 : The range of 40-bit UInt value is from 0 to the 40st power of 2 minus 1
    rangeErrorCheck(value, 'value', 0, 2 ** 40 - 1, '0', '2**40 - 1');
    value = +value;
    this[offset++] = Math.floor(value * Math.pow(2, -32)); // -32 means offset 32 bits to left
    for (let i: number = 3; i > 0; i--) {
      this[offset + i] = value;
      value = value >>> 8; // 8 means offset 8 bits
    }
    this[offset] = value;
    return offset + 4; // 4 means offset 4 bytes
  }

  private writeUInt48BE(value: number, offset: number = 0): number {
    this.checkOffsetRange(offset, sixBytes);
    // 2 : 48 : The range of 48-bit UInt value is from 0 to the 48st power of 2 minus 1
    rangeErrorCheck(value, 'value', 0, 2 ** 48 - 1, '0', '2**48 - 1');
    value = +value;
    const newVal = Math.floor(value * Math.pow(2, -32)); // -32 means offset 32 bits to left
    this[offset++] = (newVal >>> 8);
    this[offset++] = newVal;
    for (let i: number = 3; i > 0; i--) {
      this[offset + i] = value;
      value = value >>> 8; // 8 means offset 8 bits
    }
    this[offset] = value;
    return offset + 4; // 4 means offset 4 bytes
  }

  readIntBE(offset: number, byteLength: number): number | undefined {
    typeErrorCheck(offset, ['number'], 'offset');
    typeErrorCheck(byteLength, ['number'], 'byteLength');
    return this.readData(offset, byteLength, Style.intBE);
  }

  private readInt48BE(offset: number = 0): number {
    this.checkOffsetRange(offset, sixBytes);
    const val = this.calculationBE(offset, 2);
    // 2 : 15 : 0x1fffe : The number of 2 bytes changes to 4 bytes, positive high fill 0, negative high fill 1.
    return (val | (val & Math.pow(2, 15)) * 0x1fffe) * Math.pow(2, 32) + this.calculationBE(offset + 2, 4);
  }

  private readInt40BE(offset: number = 0): number {
    this.checkOffsetRange(offset, fiveBytes);
    const first = this[offset];
    const last = this[offset + 4];
    // 2 : 7 : 0x1fffffe : The number of 1 byte changes to 4 bytes, positive high fill 0, negative high fill 1.
    return (this[offset] | (this[offset] & Math.pow(2, 7)) * 0x1fffffe) * Math.pow(2, 32) + // 32 means offset 32 bits
      this.calculationBE(++offset, 4); // 4 means offset 4 bytes
  }


  private readInt24BE(offset: number = 0): number {
    this.checkOffsetRange(offset, threeBytes);
    const val = this.calculationBE(offset, 3);
    // 2 : 23 : 0x1fe : The number of 3 bytes changes to 4 bytes, positive high fill 0, negative high fill 1.
    return val | (val & Math.pow(2, 23)) * 0x1fe;
  }

  writeIntLE(value: number, offset: number, byteLength: number): number | undefined {
    typeErrorCheck(value, ['number'], 'value');
    typeErrorCheck(offset, ['number'], 'offset');
    typeErrorCheck(byteLength, ['number'], 'byteLength');
    return this.writeData(value, offset, byteLength, Style.intLE);
  }

  private writeUInt48LE(value: number, offset: number = 0): number {
    this.checkOffsetRange(offset, sixBytes);
    // 2 : 48 : The range of 48-bit UInt value is from 0 to the 48st power of 2 minus 1
    rangeErrorCheck(value, 'value', 0, 2 ** 48 - 1, '0', '2**48 - 1');
    value = +value;
    const newVal = Math.floor(value * Math.pow(2, -32)); // -32 means offset 32 bits to left
    for (let i: number = 3; i > 0; i--) {
      this[offset++] = value;
      value = value >>> 8; // 8 means offset 8 bits
    }
    this[offset++] = value;
    this[offset++] = newVal;
    this[offset++] = (newVal >>> 8); // 8 means offset 8 bits
    return offset;
  }

  private writeUInt40LE(value: number, offset: number = 0): number {
    this.checkOffsetRange(offset, fiveBytes);
    // 2 : 40 : The range of 40-bit UInt value is from 0 to the 40st power of 2 minus 1
    rangeErrorCheck(value, 'value', 0, 2 ** 40 - 1, '0', '2**40 - 1');
    value = +value;
    const newVal = value;
    for (let i: number = 3; i > 0; i--) {
      this[offset++] = value;
      value = value >>> 8; // 8 means offset 8 bits
    }
    this[offset++] = value;
    this[offset++] = Math.floor(newVal * Math.pow(2, -32)); // -32 means offset 32 bits to left
    return offset;
  }

  private writeUInt24LE(value: number, offset: number = 0): number {
    this.checkOffsetRange(offset, threeBytes);
    // 2 : 24 : The range of 24-bit UInt value is from 0 to the 24st power of 2 minus 1
    rangeErrorCheck(value, 'value', 0, 2 ** 24 - 1, '0', '2**24 - 1');
    value = +value;
    for (let i: number = 2; i > 0; i--) {
      this[offset++] = value;
      value = value >>> 8; // 8 means offset 8 bits
    }
    this[offset++] = value;
    return offset;
  }

  readIntLE(offset: number, byteLength: number): number | undefined {
    typeErrorCheck(offset, ['number'], 'offset');
    typeErrorCheck(byteLength, ['number'], 'byteLength');
    return this.readData(offset, byteLength, Style.intLE);
  }

  private readInt48LE(offset: number = 0): number {
    this.checkOffsetRange(offset, sixBytes);
    const val = this.calculationLE(offset + 4, 2);
    // 2 : 15 : 0x1fffe : The number of 2 bytes changes to 4 bytes, positive high fill 0, negative high fill 1.
    return (val | (val & Math.pow(2, 15)) * 0x1fffe) * Math.pow(2, 32) + this.calculationLE(offset, 4);
  }

  private readInt40LE(offset: number = 0): number {
    this.checkOffsetRange(offset, fiveBytes);
    // 2 : 7 : 0x1fffffe : The number of 1 byte changes to 4 bytes, positive high fill 0, negative high fill 1.
    return (this[offset + 4] | (this[offset + 4] & Math.pow(2, 7)) * 0x1fffffe) * Math.pow(2, 32) + // 32 means offset 32 bits
      this.calculationLE(offset, 4); // 4 means offset 4 bytes
  }

  private readInt24LE(offset: number = 0): number {
    this.checkOffsetRange(offset, threeBytes);
    const val = this.calculationLE(offset, 3); // 3 means get 3 bytes
    // 2 : 23 : 0x1fe : The number of 3 bytes changes to 4 bytes, positive high fill 0, negative high fill 1.
    return val | (val & Math.pow(2, 23)) * 0x1fe;
  }

  writeUIntLE(value: number, offset: number, byteLength: number): number | undefined {
    typeErrorCheck(value, ['number'], 'value');
    typeErrorCheck(offset, ['number'], 'offset');
    typeErrorCheck(byteLength, ['number'], 'byteLength');
    return this.writeData(value, offset, byteLength, Style.uintLE);
  }

  readUIntLE(offset: number, byteLength: number): number | undefined {
    typeErrorCheck(offset, ['number'], 'offset');
    typeErrorCheck(byteLength, ['number'], 'byteLength');
    return this.readData(offset, byteLength, Style.uintLE);
  }

  private readUInt48LE(offset: number = 0): number {
    this.checkOffsetRange(offset, sixBytes);
    return this.calculationLE(offset, 4) +
      (this.calculationLE(offset + 4, 2)) * Math.pow(2, 32); // 32 means offset 32 bits
  }

  private readUInt40LE(offset: number = 0): number {
    this.checkOffsetRange(offset, fiveBytes);
    return this.calculationLE(offset, 5); // 5 means get 5 bytes
  }

  private readUInt24LE(offset: number = 0): number {
    this.checkOffsetRange(offset, threeBytes);
    return this.calculationLE(offset, 3); // 3 means get 3 bytes
  }

  writeUIntBE(value: number, offset: number, byteLength: number): number | undefined {
    typeErrorCheck(value, ['number'], 'value');
    typeErrorCheck(offset, ['number'], 'offset');
    typeErrorCheck(byteLength, ['number'], 'byteLength');
    return this.writeData(value, offset, byteLength, Style.uintBE);
  }

  readUIntBE(offset: number, byteLength: number): number | undefined {
    typeErrorCheck(offset, ['number'], 'offset');
    typeErrorCheck(byteLength, ['number'], 'byteLength');
    return this.readData(offset, byteLength, Style.uintBE);
  }

  private readUInt48BE(offset: number = 0): number {
    this.checkOffsetRange(offset, sixBytes);
    return (this.calculationBE(offset, 2)) * Math.pow(2, 32) + // 32 means offset 32 bits
      this.calculationBE(offset + 2, 4); // 4 means get 4 bytes
  }

  private readUInt40BE(offset: number = 0): number {
    this.checkOffsetRange(offset, fiveBytes);
    return this.calculationBE(offset, 5); // 5 means get 5 bytes
  }

  private readUInt24BE(offset: number = 0): number {
    this.checkOffsetRange(offset, threeBytes);
    return this.calculationBE(offset, 3); // 3 means get 3 bytes
  }

  writeInt32BE(value: number, offset: number = 0): number {
    typeErrorCheck(value, ['number'], 'value');
    typeErrorCheck(offset, ['number'], 'offset');
    this.checkOffsetRange(offset, fourBytes);
    // 2 : 31 : The range of 32-bit Int value is from negative the 31st power of 2 to the 31st power of 2 minus 1
    rangeErrorCheck(value, 'value', - (Math.pow(2, 31)), (Math.pow(2, 31) - 1));

    value = +value;
    this[bufferSymbol].writeInt32BE(value, offset);
    return offset + 4; // 4 means offset 4 bytes
  }

  private checkOffsetRange(offset: number, size: number): void {
    rangeErrorCheck(offset, 'offset', 0, this.length - size);
  }

  readInt32BE(offset: number = 0): number {
    typeErrorCheck(offset, ['number'], 'offset');
    this.checkOffsetRange(offset, fourBytes);
    return this[bufferSymbol].readInt32BE(offset);
  }

  writeInt32LE(value: number, offset: number = 0): number {
    typeErrorCheck(value, ['number'], 'value');
    typeErrorCheck(offset, ['number'], 'offset');
    this.checkOffsetRange(offset, fourBytes);
    // 2 : 31 : The range of 32-bit Int value is from negative the 31st power of 2 to the 31st power of 2 minus 1
    rangeErrorCheck(value, 'value', - (Math.pow(2, 31)), (Math.pow(2, 31) - 1));

    value = +value;
    this[bufferSymbol].writeInt32LE(value, offset);
    return offset + 4; // 4 means offset 4 bytes
  }

  readInt32LE(offset: number = 0): number {
    typeErrorCheck(offset, ['number'], 'offset');
    this.checkOffsetRange(offset, fourBytes);
    return this[bufferSymbol].readInt32LE(offset);
  }

  writeUInt32BE(value: number, offset: number = 0): number {
    typeErrorCheck(value, ['number'], 'value');
    typeErrorCheck(offset, ['number'], 'offset');
    this.checkOffsetRange(offset, fourBytes);
    // 2 : 32 : The range of 32-bit UInt value is from zero to the 32st power of 2 minus 1
    rangeErrorCheck(value, 'value', 0, (Math.pow(2, 32) - 1));
    value = +value;
    this[bufferSymbol].writeUInt32BE(value, offset);
    return offset + 4; // 4 means offset 4 bytes
  }

  readUInt32BE(offset: number = 0): number {
    typeErrorCheck(offset, ['number'], 'offset');
    this.checkOffsetRange(offset, fourBytes);
    return this[bufferSymbol].readUInt32BE(offset);
  }

  writeUInt32LE(value: number, offset: number = 0): number {
    typeErrorCheck(value, ['number'], 'value');
    typeErrorCheck(offset, ['number'], 'offset');
    this.checkOffsetRange(offset, fourBytes);
    // 2 : 32 : The range of 32-bit UInt value is from zero to the 32st power of 2 minus 1
    rangeErrorCheck(value, 'value', 0, (Math.pow(2, 32) - 1));
    value = +value;
    this[bufferSymbol].writeUInt32LE(value, offset);
    return offset + 4; // 4 means offset 4 bytes
  }

  readUInt32LE(offset: number = 0): number {
    typeErrorCheck(offset, ['number'], 'offset');
    this.checkOffsetRange(offset, fourBytes);
    return this[bufferSymbol].readUInt32LE(offset);
  }

  writeDoubleBE(value: number, offset: number = 0): number {
    typeErrorCheck(value, ['number'], 'value');
    typeErrorCheck(offset, ['number'], 'offset');
    this.checkOffsetRange(offset, eightBytes);

    value = +value;
    float64Array[0] = value;
    let i: number = 7; // 7 is uInt8Float64Array->maxIndex
    while (i >= 0) {
      this[offset++] = uInt8Float64Array[i--];
    }
    return offset;
  }

  readDoubleBE(offset: number = 0): number {
    typeErrorCheck(offset, ['number'], 'offset');
    this.checkOffsetRange(offset, eightBytes);

    let i: number = 7; // 7 is uInt8Float64Array->maxIndex
    while (i >= 0) {
      uInt8Float64Array[i--] = this[offset++];
    }
    return float64Array[0];
  }

  writeDoubleLE(value: number, offset: number = 0): number {
    typeErrorCheck(value, ['number'], 'value');
    typeErrorCheck(offset, ['number'], 'offset');
    this.checkOffsetRange(offset, eightBytes);

    value = +value;
    float64Array[0] = value;
    let i: number = 0;
    while (i <= 7) { // 7 is uInt8Float64Array->maxIndex
      this[offset++] = uInt8Float64Array[i++];
    }
    return offset;
  }

  readDoubleLE(offset: number = 0): number {
    typeErrorCheck(offset, ['number'], 'offset');
    this.checkOffsetRange(offset, eightBytes);

    let i: number = 0;
    while (i <= 7) { // 7 is uInt8Float64Array->maxIndex
      uInt8Float64Array[i++] = this[offset++];
    }
    return float64Array[0];
  }

  writeFloatBE(value: number, offset: number = 0): number {
    typeErrorCheck(value, ['number'], 'value');
    typeErrorCheck(offset, ['number'], 'offset');
    this.checkOffsetRange(offset, fourBytes);

    value = +value;
    float32Array[0] = value;
    let i: number = 3; // 3 is uInt8Float32Array->maxIndex
    while (i >= 0) {
      this[offset++] = uInt8Float32Array[i--];
    }
    return offset;
  }

  readFloatBE(offset: number = 0): number {
    typeErrorCheck(offset, ['number'], 'offset');
    this.checkOffsetRange(offset, fourBytes);

    let i: number = 3; // 3 is uInt8Float32Array->maxIndex
    while (i >= 0) {
      uInt8Float32Array[i--] = this[offset++];
    }
    return float32Array[0];
  }

  writeFloatLE(value: number, offset: number = 0): number {
    typeErrorCheck(value, ['number'], 'value');
    typeErrorCheck(offset, ['number'], 'offset');
    this.checkOffsetRange(offset, fourBytes);

    value = +value;
    float32Array[0] = value;
    let i: number = 0;
    while (i <= 3) { // 3 is uInt8Float32Array->maxIndex
      this[offset++] = uInt8Float32Array[i++];
    }
    return offset;
  }

  readFloatLE(offset: number): number {
    if (offset === undefined || offset === null) {
      offset = 0;
    }
    typeErrorCheck(offset, ['number'], 'offset');
    this.checkOffsetRange(offset, fourBytes);
    let i: number = 0;
    while (i <= 3) { // 3 is uInt8Float32Array->maxIndex
      uInt8Float32Array[i++] = this[offset++];
    }
    return float32Array[0];
  }

  writeUInt16BE(value: number, offset: number = 0): number {
    typeErrorCheck(value, ['number'], 'value');
    typeErrorCheck(offset, ['number'], 'offset');
    this.checkOffsetRange(offset, twoBytes);
    // 2 : 16 : The range of 32-bit Int value is from zero to the 16st power of 2 minus 1
    rangeErrorCheck(value, 'value', 0, Math.pow(2, 16) - 1);
    value = +value;
    this[offset++] = (value >>> 8); // 8 means offset 8 bits
    this[offset++] = value;
    return offset;
  }

  readUInt16BE(offset: number = 0): number {
    typeErrorCheck(offset, ['number'], 'offset');
    this.checkOffsetRange(offset, twoBytes);
    const first = this[offset];
    const last = this[offset + 1];
    return first * Math.pow(2, 8) + last; // 8 means offset 8 bits
  }

  writeUInt16LE(value: number, offset: number = 0): number {
    typeErrorCheck(value, ['number'], 'value');
    typeErrorCheck(offset, ['number'], 'offset');
    this.checkOffsetRange(offset, twoBytes);
    // 2 : 16 : The range of 32-bit Int value is from zero to the 16st power of 2 minus 1
    rangeErrorCheck(value, 'value', 0, Math.pow(2, 16) - 1);
    value = +value;
    this[offset++] = value;
    this[offset++] = (value >>> 8); // 8 means offset 8 bits
    return offset;
  }

  compare(target: Buffer | Uint8Array, targetStart: number = 0, targetEnd: number = target.length,
    sourceStart: number = 0, sourceEnd: number = this.length): number {
    typeErrorCheck(target, ['Buffer', 'Uint8Array'], 'target');
    typeErrorCheck(targetStart, ['number'], 'targetStart');
    typeErrorCheck(targetEnd, ['number'], 'targetEnd');
    typeErrorCheck(sourceStart, ['number'], 'sourceStart');
    typeErrorCheck(sourceEnd, ['number'], 'sourceEnd');
    rangeErrorCheck(targetStart, 'targetStart', 0, UINT32MAX);
    rangeErrorCheck(targetEnd, 'targetEnd', 0, UINT32MAX);
    rangeErrorCheck(targetEnd, 'targetEnd', 0, target.length);
    rangeErrorCheck(sourceEnd, 'sourceEnd', 0, this.length);
    if (sourceStart >= sourceEnd) {
      return (targetStart >= targetEnd ? 0 : -1);
    }
    if (targetStart >= targetEnd) {
      return 1;
    }
    let length1: number = sourceEnd - sourceStart;
    let length2: number = targetEnd - targetStart;
    let length: number = length1 > length2 ? length2 : length1;
    if (target instanceof Buffer) {
      let val = this[bufferSymbol].compare(target[bufferSymbol], targetStart, sourceStart, length);
      if (val === 0) {
        if (length1 === length2) {
          return 0;
        }
        return length1 < length2 ? -1 : 1;
      } else {
        return val < 0 ? 1 : -1;
      }
    } else {
      let bufData1 = this[bufferSymbol].getBufferData();
      for (let i = 0; i < length; i++) {
        let value1 = +bufData1[i + sourceStart];
        let value2 = +target[i + targetStart];
        if (value1 === value2) {
          continue;
        }
        return value1 < value2 ? -1 : 1;
      }
      if (length1 === length2) {
        return 0;
      }
      return length1 < length2 ? -1 : 1;
    }
  }

  equals(otherBuffer: Uint8Array | Buffer): boolean {
    typeErrorCheck(otherBuffer, ['Buffer', 'Uint8Array'], 'otherBuffer');
    let res = this.compare(otherBuffer, 0, otherBuffer.length, 0, this.length);
    return res === 0 ? true : false;
  }

  subarray(start: number = 0, end: number = this.length): Buffer {
    let newBuf = Object.create(this);
    start = isNaN(start) ? 0 : Number(start);
    end = isNaN(end) ? 0 : Number(end);
    end = (end > this.length) ? this.length : end;
    if (start < 0 || end < 0 || end <= start) {
      return new Buffer(0);
    }
    newBuf[bufferSymbol] = this[bufferSymbol].subBuffer(start, end);
    newBuf[lengthSymbol] = (end - start);
    return newBuf;
  }

  copy(target: Buffer | Uint8Array, targetStart: number = 0, sourceStart: number = 0,
    sourceEnd: number = this.length): number {
    typeErrorCheck(target, ['Buffer', 'Uint8Array'], 'target');
    targetStart = isNaN(targetStart) ? 0 : Number(targetStart);
    sourceStart = isNaN(sourceStart) ? 0 : Number(sourceStart);
    sourceEnd = isNaN(sourceEnd) ? 0 : Number(sourceEnd);
    rangeLeftErrorCheck(targetStart, 'targetStart', 0);
    rangeLeftErrorCheck(sourceStart, 'sourceStart', 0);
    rangeLeftErrorCheck(sourceEnd, 'sourceEnd', 0);
    if (targetStart >= target.length) {
      return 0;
    }
    if (sourceEnd <= sourceStart || sourceStart >= this.length) {
      return 0;
    }
    if (target instanceof Buffer) {
      return this[bufferSymbol].copy(target[bufferSymbol], targetStart, sourceStart, sourceEnd);
    }
    let sLength: number = sourceEnd;
    let tLength: number = target.length;
    let length = tLength > sLength ? sLength : tLength;
    for (let i = targetStart; i < length; i++) {
      target[i] = this[i];
    }
    return length - targetStart;
  }

  toString(encoding: string = 'utf8', start: number = 0, end: number = this.length): string {
    let encodObj = getEncodingByType(encoding);
    if (!encodObj) {
      throw typeErrorForEncoding(encoding, 'encoding');
    }
    start = isNaN(start) ? 0 : (Number(start) < 0 ? 0 : Number(start));
    end = isNaN(end) ? 0 : Number(end);
    let bufLength = this.length;
    if (start >= bufLength || start > end) {
      return '';
    }
    end = end > bufLength ? bufLength : end;
    return encodObj.toString(this, start, end);
  }

  toJSON(): Object {
    if (this.length <= 0) {
      return { type: 'Buffer', data: [] };
    }
    let data = this[bufferSymbol].getBufferData();
    return { type: 'Buffer', data };
  }

  indexOf(value: string | number | Buffer | Uint8Array, byteOffset: number = 0, encoding: string = 'utf8'): number {
    typeErrorCheck(value, ['string', 'number', 'Buffer', 'Uint8Array'], 'value');
    if (typeof value === 'string') {
      if (typeof byteOffset === 'string') {
        encoding = byteOffset;
      }
      if (typeof byteOffset !== 'number') {
        byteOffset = 0;
      }
      encoding = encodingTypeErrorCheck(encoding);
      return this[bufferSymbol].indexOf(value, byteOffset, encoding, false);
    } else if (typeof value === 'number') {
      value = +value;
      if (value < 0 || value > utils.eightBits) {
        return -1;
      }
      let data = this[bufferSymbol].getBufferData();
      return data.indexOf(value, byteOffset);
    } else {
      let sourceData = this[bufferSymbol].getBufferData();
      if (value instanceof Buffer) {
        let targetData = value[bufferSymbol].getBufferData();
        return sourceData.join(',').indexOf(targetData.join(','), byteOffset);
      }
      return sourceData.join(',').indexOf(value.join(','), byteOffset);
    }
  }

  lastIndexOf(value: string | number | Buffer | Uint8Array, byteOffset: number = 0,
    encoding: string = 'utf8'): number {
    typeErrorCheck(value, ['string', 'number', 'Buffer', 'Uint8Array'], 'value');
    if (typeof value === 'string') {
      if (typeof byteOffset === 'string') {
        encoding = byteOffset;
      }
      if (typeof byteOffset !== 'number') {
        byteOffset = 0;
      }
      encoding = encodingTypeErrorCheck(encoding);
      return this[bufferSymbol].indexOf(value, byteOffset, encoding, true);
    } else if (typeof value === 'number') {
      value = +value;
      if (value < 0 || value > utils.eightBits) {
        return -1;
      }
      let data = this[bufferSymbol].getBufferData();
      return data.lastIndexOf(value, byteOffset);
    } else {
      let sourceData = this[bufferSymbol].getBufferData();
      if (value instanceof Buffer) {
        let targetData = value[bufferSymbol].getBufferData();
        return sourceData.join(',').lastIndexOf(targetData.join(','), byteOffset);
      }
      return sourceData.join(',').lastIndexOf(value.join(','), byteOffset);
    }
  }

  includes(value: string | number | Buffer | Uint8Array, byteOffset: number = 0, encoding: string = 'utf8'): boolean {
    typeErrorCheck(value, ['string', 'number', 'Buffer', 'Uint8Array'], 'value');
    encoding = encodingTypeErrorCheck(encoding);
    return this.indexOf(value, byteOffset, encoding) !== -1;
  }

  reverseBits(dealNum: number): Buffer {
    const len: number = this.length;
    const dealLen: number = dealNum;
    for (let i = 0; i < len / dealLen; i++) {
      let times: number = 0;
      let startIndex: number = dealLen * i;
      let endIndex: number = startIndex + dealLen - 1;
      while (times < dealLen / 2) {
        let tmp = this[startIndex + times];
        this[startIndex + times] = this[endIndex - times];
        this[endIndex - times] = tmp;
        times++;
      }
    }
    return this;
  }

  swap16(): Buffer {
    const len = this.length;
    const dealLen: number = 2; // Process every 2 bits
    if (len % dealLen !== 0) {
      throw bufferSizeError('16-bits');
    }
    return this.reverseBits(dealLen);
  }

  swap32(): Buffer {
    const len = this.length;
    const dealLen: number = 4; // Process every 4 bits
    if (len % dealLen !== 0) {
      throw bufferSizeError('32-bits');
    }
    return this.reverseBits(dealLen);
  }

  swap64(): Buffer {
    const len = this.length;
    const dealLen: number = 8; // Process every 8 bits
    if (len % dealLen !== 0) {
      throw bufferSizeError('64-bits');
    }
    return this.reverseBits(dealLen);
  }

  keys(): IterableIterator<number> {
    return this[bufferSymbol].getBufferData().keys();
  }

  values(): IterableIterator<number> {
    return this[bufferSymbol].getBufferData().values();
  }

  entries(): IterableIterator<[number, number]> {
    return this[bufferSymbol].getBufferData().entries();
  }

  [Symbol.iterator]() {
    return this[bufferSymbol].getBufferData().entries();
  }
}

function typeError(param: unknown, paramName: string, excludedTypes: string[]): BusinessError {
  let msg = new ErrorMessage(errorMap.typeError, paramName).setTypeInfo(excludedTypes, param).getString();
  return new BusinessError(msg, errorMap.typeError);
}

function typeErrorForEncoding(param: unknown, paramName: string): BusinessError {
  let msg = new ErrorMessage(errorMap.typeError, paramName).setEncodingTypeInfo(['BufferEncoding'], param).getString();
  return new BusinessError(msg, errorMap.typeError);
}

function typeErrorForProperty(typeName: string): BusinessError {
  let msg = new ErrorMessage(errorMap.typeErrorForProperty).setProperty(typeName).getString();
  return new BusinessError(msg, errorMap.typeErrorForProperty);
}

function typeErrorForSize(param: unknown, paramName: string, excludedTypes: string[]): BusinessError {
  let msg = new ErrorMessage(errorMap.typeError, paramName).setSizeTypeInfo(excludedTypes, param).getString();
  return new BusinessError(msg, errorMap.typeError);
}

function rangeError(paramName: string, rangeLeft: string | bigint | number, rangeRight: string | bigint | number,
  receivedValue: number | bigint): BusinessError {
  let msg =
    new ErrorMessage(errorMap.rangeError, paramName).setRangeInfo(rangeLeft, rangeRight, receivedValue).getString();
  return new BusinessError(msg, errorMap.rangeError);
}

function rangeLeftError(paramName: string, rangeLeft: number, receivedValue: number): BusinessError {
  let msg = new ErrorMessage(errorMap.rangeError, paramName).setRangeLeftInfo(rangeLeft, receivedValue).getString();
  return new BusinessError(msg, errorMap.rangeError);
}

function bufferSizeError(size: string): BusinessError {
  let msg = new ErrorMessage(errorMap.bufferSizeError).setSizeInfo(size).getString();
  return new BusinessError(msg, errorMap.bufferSizeError);
}

function typeErrorCheck(param: unknown, types: string[], paramName: string): void {
  let typeName = getTypeName(param);
  if (!types.includes(typeName)) {
    throw typeError(param, paramName, types);
  }
}

function sizeErrorCheck(param: unknown, paramName: string, types: string[],
  rangeLeft: number, rangeRight: number): void {
  let typeName = getTypeName(param);
  if (!types.includes(typeName)) {
    throw typeErrorForSize(param, paramName, types);
  }
  if (Number(param) < rangeLeft || Number(param) > rangeRight) {
    let typeString = types.join(', ');
    typeString = typeString.replace(',', ' or');
    let msg = 'The type of "' + paramName + '" must be ' + typeString +
      ' and the value cannot be negative. Received value is: ' + Number(param).toString();
    throw new BusinessError(msg, errorMap.typeError);
  }
}

function encodingTypeErrorCheck(encoding: string): string {
  const normalizedEncoding = normalizeEncoding(encoding);
  if (normalizedEncoding === undefined) {
    throw typeErrorForEncoding(encoding, 'encoding');
  }
  return normalizedEncoding;
}

function rangeErrorCheck(param: number | bigint, paramName: string, rangeLeft: bigint | number,
  rangeRight: bigint | number, rangeLeftExpr: string = '', rangeRightExpr: string = ''): void {
  let left = BigInt(rangeLeft);
  let right = BigInt(rangeRight);
  if (param < left || param > right) {
    throw rangeError(paramName, rangeLeftExpr === '' ? rangeLeft : rangeLeftExpr,
      rangeRightExpr === '' ? rangeRight : rangeRightExpr, param);
  }
}

function rangeLeftErrorCheck(param: number, paramName: string, rangeLeft: number): void {
  if (param < rangeLeft) {
    throw rangeLeftError(paramName, rangeLeft, param);
  }
}

function concat(list: Buffer[] | Uint8Array[], totalLength?: number): Buffer {
  typeErrorCheck(list, ['Array'], 'list');
  if (!(typeof totalLength === 'number' || typeof totalLength === 'undefined')) {
    throw typeError(totalLength, 'totalLength', ['number']);
  }
  if (list.length === 0) {
    return new Buffer(0);
  }
  if (!totalLength) {
    totalLength = 0;
    for (let i = 0, len = list.length; i < len; i++) {
      let buf = list[i];
      if (buf instanceof Uint8Array || buf instanceof Buffer) {
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
    } else if (buf instanceof Buffer) {
      buf.copy(buffer, offset);
      offset += buf.length;
    }
  }
  return buffer;
}

function alloc(size: number, fill?: string | Buffer | number, encoding?: string): Buffer {
  sizeErrorCheck(size, 'size', ['number'], 0, MAX_LENGTH);
  const buf = new Buffer(size);
  buf.fill(0);
  if (arguments.length === 2 && fill !== undefined && fill !== 0) {
    buf.fill(fill);
  } else if (arguments.length === 3) {
    if (encoding === undefined || encoding === null) {
      encoding = 'utf-8';
    }
    typeErrorCheck(encoding, ['string'], 'encoding');
    if (fill !== undefined && fill !== 0) {
      buf.fill(fill, undefined, undefined, encoding);
    }
  }
  return buf;
}

function allocUninitializedFromPool(size: number): Buffer {
  sizeErrorCheck(size, 'size', ['number'], 0, MAX_LENGTH);
  if (!pool) {
    createPool();
  }
  if (size < (poolSize >>> 1)) {
    if (size > (poolSize - poolOffset)) {
      createPool();
    }
    const b = new Buffer(pool, poolOffset, size);
    poolOffset += size;
    alignPool();
    return b;
  }
  return new Buffer(size);
}

function allocUninitialized(size: number): Buffer {
  sizeErrorCheck(size, 'size', ['number'], 0, MAX_LENGTH);
  const buf = new Buffer(size);
  return buf;
}

function normalizeEncoding(enc: string): string | undefined {
  enc = enc.toLowerCase();
  if (bufferEncoding.includes(enc)) {
    if (enc === 'ucs2' || enc === 'ucs-2' || enc === 'utf-16le') {
      enc = 'utf16le';
    }
    if (enc === 'utf-8') {
      enc = 'utf8';
    }
    return enc;
  } else {
    return undefined;
  }
}

function from(value: Buffer | Uint8Array | ArrayBuffer | SharedArrayBuffer | string | object | Array<number>,
  offsetOrEncoding?: number | string, length?: number): Buffer {
  if (value instanceof ArrayBuffer || value instanceof SharedArrayBuffer) {
    offsetOrEncoding = isNaN(Number(offsetOrEncoding)) ? 0 : Number(offsetOrEncoding);
    if (offsetOrEncoding < 0) {
      throw typeError(offsetOrEncoding, 'offset', ['number']);
    }
    if (!length) {
      length = value.byteLength - offsetOrEncoding;
    } else {
      length = isNaN(Number(length)) ? 0 : Number(length);
    }
    rangeErrorCheck(offsetOrEncoding, 'byteOffset', 0, value.byteLength);
    rangeErrorCheck(length, 'length', 0, value.byteLength - offsetOrEncoding);
    return new Buffer(value, offsetOrEncoding, length);
  }
  if (value instanceof Buffer) {
    return new Buffer(value);
  }
  if (value instanceof Uint8Array) {
    return new Buffer(value);
  }
  if (value instanceof Array) {
    if (!pool) {
      createPool();
    }
    let buffer = new Buffer(pool, poolOffset, value.length);
    poolOffset += value.length;
    alignPool();
    buffer[bufferSymbol].setArray(value);
    return buffer;
  }
  let encoding = '';
  if (typeof value === 'string' || typeof value[Symbol.toPrimitive] === 'function') {
    offsetOrEncoding = offsetOrEncoding ? offsetOrEncoding : 'utf8';
    if (typeof offsetOrEncoding !== 'string') {
      throw typeError(getTypeName(offsetOrEncoding), 'offsetOrEncoding', ['string']);
    } else {
      encoding = encodingTypeErrorCheck(offsetOrEncoding);
    }
  }
  if (typeof value === 'string') {
    return fromString(value, encoding);
  }
  if (typeof value === 'object' && value !== null) {
    const valueOf = value.valueOf && value.valueOf();
    if (valueOf != null &&
      valueOf !== value &&
      (typeof valueOf === 'string' || typeof valueOf === 'object')) {
      return from(valueOf, offsetOrEncoding, length);
    }
    if (typeof value[Symbol.toPrimitive] === 'function') {
      const primitive = value[Symbol.toPrimitive]('string');
      if (typeof primitive === 'string') {
        return fromString(primitive, encoding);
      }
    }
  }
  throw typeError(getTypeName(value), 'value', ['Buffer', 'ArrayBuffer', 'Array', 'Array-like']);
}

function hexStrtoNumbers(hex: string): Array<number> {
  let arr = hex.split('');
  let nums: Array<number> = [];
  for (let i = 0, len = arr.length; i < len / 2; i++) {
    let tmp = '0x' + arr[i * 2] + arr[i * 2 + 1];
    let hexNum = Number(tmp);
    if (isNaN(hexNum)) {
      if (i === 0) {
        throw new Error(`The argument 'value' is invalid. Received "${hex}"`);
      }
      break;
    }
    nums[i] = Number(tmp);
  }
  return nums;
}

function fromString(value: string, encoding: string): Buffer {
  let enc = normalizeEncoding(encoding);
  if (!enc) {
    throw typeErrorForEncoding(encoding, 'encoding');
  }
  let size = byteLength(value, enc);
  let buffer = allocUninitializedFromPool(size);
  buffer[bufferSymbol].fromString(value, enc, size);
  buffer[lengthSymbol] = buffer[bufferSymbol].getLength();
  return buffer;
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

function isBuffer(obj: Object): boolean {
  return obj instanceof Buffer;
}

function isEncoding(encoding: string): boolean {
  if (typeof encoding !== 'string' || encoding.length === 0) {
    return false;
  }
  return getEncodingByType(encoding) ? true : false;
}

function byteLength(string: string | BackingType, encoding: string = 'utf8'): number {
  if (typeof string === 'string' || isTypedArray(string) ||
    string instanceof DataView || string instanceof ArrayBuffer ||
    string instanceof SharedArrayBuffer) {
    if (string instanceof Buffer) {
      return string.length;
    } else if (typeof string === 'string') {
      if (string.length === 0) {
        return 0;
      }
      let encodRes = getEncodingByType(encoding);
      if (!encodRes) {
        return getUtf8ByteLength(string);
      }
      return encodRes.byteLength(string);
    } else {
      return string.byteLength;
    }
  } else {
    throw typeError(string, 'string', ['string', 'Buffer', 'ArrayBuffer']);
  }
}

function transcode(source: Buffer | Uint8Array, fromEnc: string, toEnc: string): Buffer {
  typeErrorCheck(source, ['Buffer', 'Uint8Array'], 'source');
  typeErrorCheck(fromEnc, ['string'], 'fromEnc');
  typeErrorCheck(toEnc, ['string'], 'toEnc');
  let from = source.toString(fromEnc);
  return fromString(from, toEnc);
}

function toUtf8(self: Buffer, start: number, end: number): string {
  return self[bufferSymbol].toUtf8(start, end);
}

function toAscii(self: Buffer, start: number, end: number): string {
  let bufData = self[bufferSymbol].getBufferData();
  let val = '';
  for (let i = start; i < end; i++) {
    val += String.fromCharCode(+bufData[i] & 0x7F); // 0x7F : get the lower 15-bits
  }
  return val;
}

function toBinary(self: Buffer, start: number, end: number): string {
  let bufData = self[bufferSymbol].getBufferData();
  let val = '';
  for (let i = start; i < end; i++) {
    val += String.fromCharCode(+bufData[i]);
  }
  return val;
}

function toHex(self: Buffer, start: number, end: number): string {
  let bufData = self[bufferSymbol].getBufferData();
  let str = '';
  for (let i = start, len = end; i < len; i++) {
    let tmpstr = Number(bufData[i]).toString(16); // 16 : 16 decimal
    tmpstr = (tmpstr.length === 1) ? `0${tmpstr}` : tmpstr;
    str += tmpstr;
  }
  return str;
}

function toUtf16LE(self: Buffer, start: number, end: number): string {
  let bufData: Array<number> = self[bufferSymbol].getBufferData();
  let val = '';
  for (let i = start; i + 1 < end; i += 2) {
    val += String.fromCodePoint((bufData[i + 1] << 8) + (bufData[i])); // 8 means offset 8 bits
  }
  return val;
}

function toBase64(self: Buffer, start: number, end: number): string {
  let str = self[bufferSymbol].toBase64(start, end);
  return str;
}

function getEncodingByType(type: string) {
  type = type.toLowerCase();
  switch (type) {
    case 'utf8':
    case 'utf-8':
      return {
        byteLength: getUtf8ByteLength,
        toString: toUtf8
      };
    case 'ucs2':
    case 'ucs-2':
      return {
        byteLength: (str: string) => str.length * 2, // 2 : 2 times of ascii
        toString: toUtf16LE
      };
    case 'ascii':
    case 'latin1':
      return {
        byteLength: (str: string) => str.length,
        toString: toAscii
      };
    case 'binary':
      return {
        byteLength: (str: string) => str.length,
        toString: toBinary
      };
    case 'utf16le':
    case 'utf-16le':
      return {
        byteLength: (str: string) => str.length * 2, // 2 : 2 times of ascii
        toString: toUtf16LE
      };
    case 'base64':
    case 'base64url':
      return {
        byteLength: getBase64ByteLength,
        toString: toBase64
      };
    case 'hex':
      return {
        byteLength: (str: string) => str.length >>> 1, // 1 : one-half
        toString: toHex
      };
    default:
      return undefined;
  }
}

function getUtf8ByteLength(str: string): number {
  return internalBuffer.utf8ByteLength(str);
}

function getBase64ByteLength(str: string): number {
  let bytes = str.length;
  for (let i = 0, len = str.length; i < len; i++) {
    if (str.charCodeAt(i) === 0x3D) { // 0x3D : ascii code represent of '='
      bytes--;
    }
    if (bytes > 1 && str.charCodeAt(i) === 0x3D) { // 0x3D : ascii code represent of '='
      bytes--;
    }
  }
  return (bytes * 3) >>> 2; // 3 : 2 : Base64 ratio: 3/4
}

function compare(buf1: Buffer | Uint8Array, buf2: Buffer | Uint8Array): 1 | 0 | -1 {
  if (!(buf1 instanceof Buffer) && !(buf1 instanceof Uint8Array)) {
    throw new BusinessError(new ErrorMessage(errorMap.typeError, 'buf1').setTypeInfo(['Buffer', 'Uint8Array'],
      getTypeName(buf1)).getString(), errorMap.typeError);
  }
  if (!(buf2 instanceof Buffer) && !(buf2 instanceof Uint8Array)) {
    throw new BusinessError(new ErrorMessage(errorMap.typeError, 'buf2').setTypeInfo(['Buffer', 'Uint8Array'],
      getTypeName(buf2)).getString(), errorMap.typeError);
  }

  let bufData1: Array<number> | Uint8Array;
  let bufData2: Array<number> | Uint8Array;
  if (buf1 instanceof Buffer) {
    bufData1 = buf1[bufferSymbol].getBufferData();
  } else {
    bufData1 = buf1;
  }
  if (buf2 instanceof Buffer) {
    bufData2 = buf2[bufferSymbol].getBufferData();
  } else {
    bufData2 = buf2;
  }
  let length1: number = bufData1.length;
  let length2: number = bufData2.length;
  let length: number = length1 > length2 ? length2 : length1;
  for (let i = 0; i < length; i++) {
    let value1 = +bufData1[i];
    let value2 = +bufData2[i];
    if (value1 > value2) {
      return 1;
    } else if (value1 < value2) {
      return -1;
    }
  }
  if (length1 > length2) {
    return 1;
  } else if (length1 < length2) {
    return -1;
  }
  return 0;
}

export default {
  Buffer,
  Blob,
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
