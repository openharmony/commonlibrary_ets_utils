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
const InternalBuffer = requireInternal('buffer');
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
const maxIntValue = Math.pow(2, 31);
const maxUIntValue = Math.pow(2, 32);
const one_byte: number = 1;
const two_bytes: number = 2;
const three_bytes: number = 3;
const four_bytes: number = 4;
const five_bytes: number = 5;
const six_bytes: number = 6;


type TypedArray = Int8Array | Uint8Array | Uint8ClampedArray | Int16Array | Uint16Array |
                  Int32Array | Uint32Array | Float32Array | Float64Array;
type BackingType = Buffer | TypedArray | DataView | ArrayBuffer | SharedArrayBuffer;
const float64Array: Float64Array = new Float64Array(1);
const uInt8Float64Array: Uint8Array = new Uint8Array(float64Array.buffer);
const float32Array: Float32Array = new Float32Array(1);
const uInt8Float32Array: Uint8Array = new Uint8Array(float32Array.buffer);

function createPool() {
  poolSize = initialPoolSize;
  pool = new Buffer(poolSize);
  poolOffset = 0;
}

function alignPool() {
  if (poolOffset & 0x7) {
    poolOffset |= 0x7;
    poolOffset++;
  }
}
type Options = {
  type: string,
  endings: string
}

const _log = console.log;

console.log = function (...args) {
  if (args.length === 1 &&  args[0] instanceof Buffer ) {
    let buf: Buffer = args[0];
    let bufArr:Array<number> = buf[bufferSymbol].getBufferData();
    let getStr = function(bufArr: Array<number>, len: number) {
      let str = '';
      for (let i = 0; i < len; i++) {
        let str_t = bufArr[i].toString(16); // 16: hexadecimal
        str_t = (str_t.length === 1) ? `0${str_t}` : str_t;
        str += ` ${str_t}`; 
      }
      return str;
    }
    let msg = '';
    if (bufArr.length > 50) { // 50: Maximum number of log displays
      let bufStr = getStr(bufArr, 50);
      msg = bufStr + ` ... ${bufArr.length - 50} more bytes`
    } else {
      msg = getStr(bufArr, bufArr.length);
    }
    _log.call(console, `<Buffer ${msg}>`);
  } else {
    _log(...args);
  }
}

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
        type: "",
        endings: "transparent"
      }
    }
    let type = options.type ? options.type : '';
    let endings = options.endings ? options.endings : 'transparent';
    if (endings !== 'transparent' && endings !== 'native') {
        throw new Error('invalid arg value of options.endings');
    }
    let arr: Array<number> = [];
    if (sources instanceof Array || isTypedArray(sources)) {
      for (const value of sources) {
          arr = arr.concat(this.normalizeSource(value));
      }
    }
    this._size = arr.length;
    this._type = type;
    this.blobClass = new InternalBuffer.Blob(arr);
  }

  normalizeSource(source: string | ArrayBuffer | TypedArray | DataView | Blob | Buffer): Array<number> {
    let ret: Array<number> = [];
    if (typeof source === 'string') {
      return InternalBuffer.utf8StringToNumbers(source);
    } else if(source instanceof ArrayBuffer) {
      return Array.prototype.slice.call(new Uint8Array(source));
    } else if(isTypedArray(source)) {
      let numbers = Array.prototype.slice.call(source);
      let str = "";
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
      newBlob.blobClass = new InternalBuffer.Blob(this.blobClass, start);
      return newBlob;
    }
    if (start > end) {
      return newBlob;
    }
    if ((start > 0 && end < 0) || (start < 0 && end > 0)) {
      return newBlob;
    }
    newBlob.blobClass = new InternalBuffer.Blob(this.blobClass, start, end);
    return newBlob;
  }
}

let utils = {
  EIGHT_BITS: 0xFF,
  SIXTYFOUR_BIT: 0xFFFFFFFFn,

  getLowerEight(value: number): number {
      return value & this.EIGHT_BITS
  },
  getLowerSixtyFour(value: bigint): bigint {
    return value & this.SIXTYFOUR_BIT
  }
}

enum Style {
  IntBE = 0,
  IntLE,
  UIntBE,
  UIntLE
}

class HandlerBuffer {
  get(obj: Buffer, prop: any): number | undefined {
    if (typeof prop === 'number') {
      if (prop >= obj.length) {
        return obj[prop];
      }
      return obj[bufferSymbol].get(prop);
    }
    return obj[prop];
  }
  set(obj: Buffer, prop: any, value: any): boolean {
    if (typeof prop === 'number') {
      if (prop >= obj.length) {
        return false;
      }
      value = utils.getLowerEight(value);
      obj[bufferSymbol].set(prop, value);
      return true;
    } else if (prop === lengthSymbol || prop === bufferSymbol) {
      obj[prop] = value;
      return true;
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

  public get buffer() {
    if (this._arrayBuffer) {
      return this._arrayBuffer;
    }
    let arr = this[bufferSymbol].getBufferData();
    return new Uint8Array(arr).buffer;
  }

  constructor(value: number | Buffer | Uint8Array | ArrayBuffer | Array<number> | string,
              byteOffsetOrEncoding?: number | string , length?: number) {
    if (arguments.length === 1) {
      if (typeof value === 'number') {
        this[bufferSymbol] = new InternalBuffer.Buffer(ParaType.NUMBER, value);
      } else if (value instanceof Buffer) {
        this[bufferSymbol] = new InternalBuffer.Buffer(ParaType.BUFFER, value[bufferSymbol]);
      } else if (value instanceof Uint8Array) {
        this[bufferSymbol] = new InternalBuffer.Buffer(ParaType.UINT8ARRAY, value);
      } else if (value instanceof Array) {
        this[bufferSymbol] = new InternalBuffer.Buffer(ParaType.NUMBERS, value);
      }
    } else if (arguments.length === 3 && typeof byteOffsetOrEncoding === 'number' && typeof length === 'number') {
      if (value instanceof ArrayBuffer) {
        this[bufferSymbol] = new InternalBuffer.Buffer(ParaType.ARRAYBUFFER, value, byteOffsetOrEncoding, length);
        this._arrayBuffer = value;
      } else if (value instanceof Buffer) {
        this[bufferSymbol] = new InternalBuffer.Buffer(ParaType.BUFFER, value[bufferSymbol], byteOffsetOrEncoding, length);
      }
    } else if (arguments.length === 2 && typeof value === 'string' && typeof byteOffsetOrEncoding === 'string') {
      this[bufferSymbol] = new InternalBuffer.Buffer(ParaType.STRING, value, byteOffsetOrEncoding);
    } else {
      this[bufferSymbol] = new InternalBuffer.Buffer(ParaType.NUMBER, 0);
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
    if (this.length == 0) {
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
      throwError(offset, 'number', 'offset');
    }
    if (typeof end === 'number') {
      throwError(end, 'number', 'end');
    }
    if (typeof encoding !== 'string') {
      throwError(encoding, 'string', 'encoding');
    }
    const normalizedEncoding = normalizeEncoding(encoding);
    if (normalizedEncoding === undefined) {
      throw new TypeError('Unknown encoding: ' + encoding);
    }
    if (offset <  0) {
      throw new RangeError(`The value of "offset" is out of range`);
    }
    if (end < 0) {
      throw new RangeError(`The value of "end" is out of range`);
    }
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
      nums.push(value & 0xFF);
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
    if (arguments.length === 1) {
      return this[bufferSymbol].writeString(str, 0, length, 'utf8');
    } else if (arguments.length === 2) {
      if (typeof offset === 'string') {
        encoding = offset;
        return this[bufferSymbol].writeString(str, 0, this.length, 'utf8');
      } else if (typeof offset === 'number') {
        if (offset <= 0 || offset >= this.length) {
          throw new RangeError(`The value of "offset" is out of range. It must be >= 0 && <= ${this.length}. Received ${offset}`);
        }
        return this[bufferSymbol].writeString(str, offset, length, 'utf8');
      } else {
        throw new TypeError(`The "offset" argument must be of type number`);
      }
    } else if (arguments.length === 3) {
      if (typeof offset !== 'number') {
        throw new TypeError(`The "offset" argument must be of type number`);
      }
      if (offset <= 0 || offset >= this.length) {
        throw new RangeError(`The value of "offset" is out of range.` +
          ` It must be >= 0 && <= ${this.length}. Received ${offset}`);
      }
      if (typeof length === 'number') {
        length = (length > this.length - offset) ? (this.length - offset) : length;
        return this[bufferSymbol].writeString(str, offset, length, 'utf8');
      } else if (typeof length === 'string') {
        encoding = length;
        length = this.length - offset;
        return this[bufferSymbol].writeString(str, offset, length, encoding);
      } else {
        throw new TypeError(`The "length" argument must be of type number`);
      }
    } else {
      if (typeof offset !== 'number') {
        throw new TypeError(`The "offset" argument must be of type number`);
      } else if (typeof length !== 'number') {
        throw new TypeError(`The "length" argument must be of type number`);
      } else {
        if (offset <= 0 || offset >= this.length) {
          throw new RangeError(`The value of "offset" is out of range.` +
            ` It must be >= 0 && <= ${this.length}. Received ${offset}`)
        }
        let encode = normalizeEncoding(encoding);
        if (encode === undefined) {
          throw new TypeError(`Unknown encoding: ${encoding}`)
        }
        length = (length > this.length - offset) ? (this.length - offset) : length;
        return this[bufferSymbol].writeString(str, offset, length, encoding);
      }
    }
  }

  convertToBig64BE(value: bigint, offset: number): number {
    let byteValue = Number(utils.getLowerSixtyFour(value));
    let bitNum = 8; // 8: 8 bit
    for (let i = bitNum - 1; i > 0; i--) {
      this[offset + i] = byteValue;
      if (i === 4) {
        byteValue = Number(utils.getLowerSixtyFour(value >> 32n));
      } else {
        byteValue = byteValue >> 8;
      }
    }
    this[offset] = byteValue;
    return offset + 8;
  }

  convertToBig64LE(value: bigint, offset: number): number {
    let byteValue = Number(utils.getLowerSixtyFour(value));
    let bitNum = 8; // 8: 8 bit
    for (let i = 0; i < bitNum - 1; i++) {
      this[offset++] = byteValue;
      if (i === 3) {
        byteValue = Number(utils.getLowerSixtyFour(value >> 32n));
      } else {
        byteValue = byteValue >> 8;
      }
    }
    this[offset++] = byteValue;
    return offset;
  }

  private readData(offset: number, byteLength: number, style: Style) : number | undefined {
    if (style == Style.IntBE) {
      switch (byteLength) {
        case one_byte:
          return this.readInt8(offset);
        case two_bytes:
          return this.readInt16BE(offset);
        case three_bytes:
          return this.readInt24BE(offset);
        case four_bytes:
          return this.readInt32BE(offset);
        case five_bytes:
          return this.readInt40BE(offset);
        case six_bytes:
          return this.readInt48BE(offset);
        default:
          break;
      }
    } else if (style == Style.IntLE) {
      switch (byteLength) {
        case one_byte:
          return this.readInt8(offset);
        case two_bytes:
          return this.readInt16LE(offset);
        case three_bytes:
          return this.readInt24LE(offset);
        case four_bytes:
          return this.readInt32LE(offset);
        case five_bytes:
          return this.readInt40LE(offset);
        case six_bytes:
          return this.readInt48LE(offset);
        default:
          break;
      }
    } else if (style == Style.UIntLE) {
      switch (byteLength) {
        case one_byte:
          return this.readUInt8(offset);
        case two_bytes:
          return this.readUInt16LE(offset);
        case three_bytes:
          return this.readUInt24LE(offset);
        case four_bytes:
          return this.readUInt32LE(offset);
        case five_bytes:
          return this.readUInt40LE(offset);
        case six_bytes:
          return this.readUInt48LE(offset);
        default:
          break;
      }
    } else if (style == Style.UIntBE) {
      switch (byteLength) {
        case one_byte:
          return this.readUInt8(offset);
        case two_bytes:
          return this.readUInt16BE(offset);
        case three_bytes:
          return this.readUInt24BE(offset);
        case four_bytes:
          return this.readUInt32BE(offset);
        case five_bytes:
          return this.readUInt40BE(offset);
        case six_bytes:
          return this.readUInt48BE(offset);
        default:
          break;
      }
    }
    
    throw new Error('byteLength Must satisfy `0 < byteLength <= ' + byteLength +'`');
  }
  
  private writeData(value: number, offset: number, byteLength: number, style: Style) : number | undefined {
    if (style == Style.IntBE) {
      switch (byteLength) {
        case one_byte:
          return this.writeInt8(value, offset);
        case two_bytes:
          return this.writeInt16BE(value, offset);
        case three_bytes:
          return this.writeUInt24BE(value, offset);
        case four_bytes:
          return this.writeInt32BE(value, offset);
        case five_bytes:
          return this.writeUInt40BE(value, offset);
        case six_bytes:
          return this.writeUInt48BE(value, offset);
        default:
          break;
      }
    } else if (style == Style.IntLE) {
      switch (byteLength) {
        case one_byte:
          return this.writeUInt8(value, offset);
        case two_bytes:
          return this.writeUInt16LE(value, offset);
        case three_bytes:
          return this.writeUInt24LE(value, offset);
        case four_bytes:
          return this.writeUInt32LE(value, offset);
        case five_bytes:
          return this.writeUInt40LE(value, offset);
        case six_bytes:
          return this.writeUInt48LE(value, offset);
        default:
          break;
      }
    } else if (style == Style.UIntLE) {
      switch (byteLength) {
        case one_byte:
          return this.writeUInt8(value, offset);
        case two_bytes:
          return this.writeUInt16LE(value, offset);
        case three_bytes:
          return this.writeUInt24LE(value, offset);
        case four_bytes:
          return this.writeUInt32LE(value, offset);
        case five_bytes:
          return this.writeUInt40LE(value, offset);
        case six_bytes:
          return this.writeUInt48LE(value, offset);
        default:
          break;
      }
    } else if (style == Style.UIntBE) {
      switch (byteLength) {
        case one_byte:
          return this.writeUInt8(value, offset);
        case two_bytes:
          return this.writeUInt16BE(value, offset);
        case three_bytes:
          return this.writeUInt24BE(value, offset);
        case four_bytes:
          return this.writeUInt32BE(value, offset);
        case five_bytes:
          return this.writeUInt40BE(value, offset);
        case six_bytes:
          return this.writeUInt48BE(value, offset);
        default:
          break;
      }
    }

    throw new RangeError('byteLength Must satisfy `0 < byteLength <= ' + byteLength +'`');
  }

  writeBigInt64BE(value: bigint, offset: number = 0): number {
    if (offset < 0 || offset > this.length - 8) {
      throw new RangeError('The value of "offset" is out of range');
    }
    if (value < -(2n **63n) ||value > 2n **63n) {
      throw new RangeError('The value of "value" is out of range');
    }

    return this.convertToBig64BE(value, offset);
  }

  readBigInt64BE(offset: number = 0): bigint {
    if (offset < 0 || offset > this.length - 8) {
      throw new RangeError('The value of "offset" is out of range');
    }
    // 24 : the first val for this[offset] shifts left by 3 bytes
    const val = (this[offset] << 24) + this.calculationBE(offset + 1, 3);
    // 32 : Shift left by 4 bytes
    return (BigInt(val) << 32n) +
      BigInt(this.calculationBE(offset + 4, 4));
  }

  writeBigInt64LE(value: bigint, offset: number = 0): number {
    if (offset < 0 || offset > this.length - 8) {
      throw new RangeError('The value of "offset" is out of range');
    }
    if (value < -(2n **63n) ||value > 2n **63n) {
      throw new RangeError('The value of "value" is out of range');
    }

    return this.convertToBig64LE(value, offset);
  }

  private calculationLE(offset: number, count: number): number {
    let result: number = 0;
    for (let i = 0; i < count; i++) {
      result += this[offset++] * Math.pow(2, 8 * i);
    }
    return result;
  }

  private calculationBE(offset: number, count: number): number {
    let result: number = 0;
    for (let i = count - 1; i >= 0; i--) {
      result += this[offset++] * Math.pow(2, 8 * i);
    }
    return result;
  }

  readBigInt64LE(offset: number = 0): bigint {
    if (offset < 0 || offset > this.length - 8) {
      throw new RangeError('The value of "offset" is out of range');
    }
    const val = this.calculationLE(offset + 4, 3) + (this[offset + 7] << 24); 
    return (BigInt(val) << 32n) + BigInt(this.calculationLE(offset, 4));
  }

  writeBigUInt64BE(value: bigint, offset: number = 0): number {
    if (offset < 0 || offset > this.length - 8) {
      throw new RangeError('The value of "offset" is out of range');
    }
    if (value < 0n || value >= 2n **64n) {
      throw new RangeError('The value of "value" is out of range');
    }

    return this.convertToBig64BE(value, offset);
  }

  readBigUInt64BE(offset: number = 0): bigint {
    if (offset < 0 || offset > this.length - 8) {
      throw new RangeError('The value of "offset" is out of range');
    }
    const hi = this.calculationBE(offset, 4);
  
    const lo = this.calculationBE(offset + 4, 4);
    return (BigInt(hi) << 32n) + BigInt(lo);
  }

  writeBigUInt64LE(value: bigint, offset: number = 0): number {
    if (offset < 0 || offset > this.length - 8) {
      throw new RangeError('The value of "offset" is out of range');
    }
    if (value < 0n || value >= 2n **64n) {
      throw new RangeError('The value of "value" is out of range');
    }

    return this.convertToBig64LE(value, offset);
  }

  readBigUInt64LE(offset: number = 0): bigint {
    if (offset < 0 || offset > this.length - 8) {
      throw new RangeError('The value of "offset" is out of range');
    }
    const lo = this.calculationLE(offset, 4);
    const hi = this.calculationLE(offset + 4, 4);
    return BigInt(lo) + (BigInt(hi) << 32n);
  }

  writeInt8(value: number, offset: number = 0): number {
    if (offset < 0 || offset > this.length - 1) {
      throw new RangeError('The value of "offset" is out of range');
    }
    if (value > (Math.pow(2, 7) -1) || value < -(Math.pow(2, 7))){
      throw new RangeError('The value of "value" is out of range')
    }
    value = +value;

    this[offset] = value;
    return offset + 1;
  }

  readInt8(offset: number = 0): number {
    if (offset < 0 || offset > this.length - 1) {
      throw new RangeError('The value of "offset" is out of range');
    }
    const val = this[offset];
    if (val === undefined) {
      throw new RangeError('The value of "offset" is out of range');
    }
  
    return val | (val & Math.pow(2, 7)) * 0x1fffffe;
  }

  writeInt16BE(value: number, offset: number = 0): number {
    if (offset < 0 || offset > this.length - 2) {
      throw new RangeError('The value of "offset" is out of range');
    }
    if (value < -(Math.pow(2, 15)) || value > (Math.pow(2, 15) - 1)){
      throw new RangeError('The value of "value" is out of range')
    }
    value = +value;
    this[offset++] = (value >>> 8);
    this[offset++] = value;
    return offset;
  }

  readInt16BE(offset: number = 0): number {
    if (offset < 0 || offset > this.length - 2) {
      throw new RangeError('The value of "offset" is out of range');
    }
    const val = this.calculationBE(offset, 2);
    return val | (val & Math.pow(2, 15)) * 0x1fffe;
  }

  writeInt16LE(value: number, offset: number = 0): number {
    if (offset < 0 || offset > this.length - 2) {
      throw new RangeError('The value of "offset" is out of range');
    }
    if (value < -(Math.pow(2, 15)) || value > (Math.pow(2, 15) - 1)){
      throw new RangeError('The value of "value" is out of range')
    }
    value = +value;
    this[offset++] = value;
    this[offset++] = (value >>> 8);
    return offset;
  }

  readInt16LE(offset: number = 0): number {
    if (offset < 0 || offset > this.length - 2) {
      throw new RangeError('The value of "offset" is out of range');
    }

    const val = this[offset] + this[offset + 1] * Math.pow(2, 8);
    return val | (val & Math.pow(2, 15)) * 0x1fffe;
  }

  readUInt16LE(offset: number = 0): number {
    if (offset < 0 || offset > this.length - 2) {
      throw new RangeError('The value of "offset" is out of range');
    }
  
    return this[offset] + this[offset + 1] * Math.pow(2, 8);
  }

  writeUInt8(value: number, offset: number = 0): number {
    if (offset < 0 || offset > this.length - 1) {
      throw new RangeError('The value of "offset" is out of range');
    }
    if (value < 0 || value >255) {
      throw new RangeError('The value of "value" is out of range');
    }
    value = +value;
    
    this[offset] = value;
    return offset + 1;
  }

  readUInt8(offset: number = 0): number {
    if (offset < 0 || offset > this.length - 1) {
      throw new RangeError('The value of "offset" is out of range');
    }
    const val = this[offset];
    if (val === undefined) {
      throw new RangeError('The value of "offset" is out of range');
    }
    return val;
  }

  writeIntBE(value: number, offset: number, byteLength: number): number | undefined {
    return this.writeData(value, offset, byteLength, Style.IntBE);
  }

  private writeUInt24BE(value: number, offset: number = 0) {
    if (offset < 0 || offset > this.length - 3) {
      throw new RangeError('The value of "offset" is out of range');
    }
    if (value < 0 || value > (Math.pow(2, 24) -1)) {
      throw new RangeError('The value of "value" is out of range');
    }
    value = +value;
    for(let i: number = 2; i > 0; i--) {
      this[offset + i] = value;
      value = value >>> 8;
    }
    this[offset] = value;
    return offset + 3;
  }

  private writeUInt40BE(value: number, offset: number = 0) {
    if (offset < 0 || offset > this.length - 5) {
      throw new RangeError('The value of "offset" is out of range');
    }
    if (value < 0 || value >= Math.pow(2, 40)) {
      throw new RangeError('The value of "value" is out of range');
    }
    value = +value;
    this[offset++] = Math.floor(value * Math.pow(2, -32));
    for(let i: number = 3; i > 0; i--) {
      this[offset + i] = value;
      value = value >>> 8;
    }
    this[offset] = value;
    return offset + 4;
  }

  private writeUInt48BE(value: number, offset: number = 0) {
    if (offset < 0 || offset > this.length - 6) {
      throw new RangeError('The value of "offset" is out of range');
    }
    if(value < 0 || value >= Math.pow(2, 48)){
      throw new RangeError('The value of "value" is out of range');
    }
    value = +value;
    const newVal = Math.floor(value * Math.pow(2, -32));
    this[offset++] = (newVal >>> 8);
    this[offset++] = newVal;
    for(let i: number = 3; i > 0; i--) {
      this[offset + i] = value;
      value = value >>> 8;
    }
    this[offset] = value;
    return offset + 4;
  }

  readIntBE(offset: number, byteLength: number): number | undefined {
    if (offset < 0 || offset > this.length - 1) {
      throw new RangeError('The value of "offset" is out of range');
    }

    return this.readData(offset, byteLength, Style.IntBE);
  }

  private readInt48BE(offset: number = 0) {
    if (offset < 0 || offset > this.length - 6) {
      throw new RangeError('The value of "offset" is out of range');
    }
    const val = this.calculationBE(offset, 2);
    return (val | (val & Math.pow(2, 15)) * 0x1fffe) * Math.pow(2, 32) + this.calculationBE(offset + 2, 4);
  }

  private readInt40BE(offset: number = 0) {
    if (offset < 0 || offset > this.length - 5) {
      throw new RangeError('The value of "offset" is out of range');
    }
    const first = this[offset];
    const last = this[offset + 4];
    if (first === undefined || last === undefined) {
      throw new RangeError('The value of "offset" is out of range');
    }
    return (this[offset] | (this[offset] & Math.pow(2, 7)) * 0x1fffffe) * Math.pow(2, 32) +
      this.calculationBE(++offset, 4);
  }
  

  private readInt24BE(offset: number = 0) {
    if (offset < 0 || offset > this.length - 3) {
      throw new RangeError('The value of "offset" is out of range');
    }
    const val = this.calculationBE(offset, 3);
    return val | (val & Math.pow(2, 23)) * 0x1fe;
  }

  writeIntLE(value: number, offset: number, byteLength: number): number | undefined {
    return this.writeData(value, offset, byteLength, Style.IntLE);
  }

  private writeUInt48LE(value: number, offset: number = 0) {
    if (offset < 0 || offset > this.length - 6) {
      throw new RangeError('The value of "offset" is out of range');
    }
    if (value < 0 || value >= Math.pow(2, 48)) {
      throw new RangeError('The value of "value" is out of range');
    }
    value = +value;
    const newVal = Math.floor(value * Math.pow(2, -32));
    for(let i: number = 3; i > 0; i--) {
      this[offset++] = value;
      value = value >>> 8;
    }
    this[offset++] = value;
    this[offset++] = newVal;
    this[offset++] = (newVal >>> 8);
    return offset;
  }

  private writeUInt40LE(value: number, offset: number = 0) {
    if (offset < 0 || offset > this.length - 5) {
      throw new RangeError('The value of "offset" is out of range');
    }
    if (value < 0 || value >= Math.pow(2, 40)) {
      throw new RangeError('The value of "value" is out of range');
    }
    value = +value;
    const newVal = value;
    for(let i: number = 3; i > 0; i--) {
      this[offset++] = value;
      value = value >>> 8;
    }
    this[offset++] = value;
    this[offset++] = Math.floor(newVal * Math.pow(2, -32));
    return offset;
  }

  private writeUInt24LE(value: number, offset: number = 0) {
    if (offset < 0 || offset > this.length - 3) {
      throw new RangeError('The value of "offset" is out of range');
    }
    if (value < 0 || value > (Math.pow(2, 24) - 1)) {
      throw new RangeError('The value of "value" is out of range');
    }
    value = +value;
    for(let i: number = 2; i > 0; i--) {
      this[offset++] = value;
      value = value >>> 8;
    }
    this[offset++] = value;
    return offset;
  }

  readIntLE(offset: number, byteLength: number): number | undefined {
    if (offset < 0 || offset > this.length - 1) {
      throw new RangeError('The value of "offset" is out of range');
    }

    return this.readData(offset, byteLength, Style.IntLE);
  }

  private readInt48LE(offset: number = 0) {
    if (offset < 0 || offset > this.length - 6) {
      throw new RangeError('The value of "offset" is out of range');
    }
    const val = this.calculationLE(offset + 4, 2);
    return (val | (val & Math.pow(2, 15)) * 0x1fffe) * Math.pow(2, 32) + this.calculationLE(offset, 4);
  }

  private readInt40LE(offset: number = 0) {
    if (offset < 0 || offset > this.length - 5) {
      throw new RangeError('The value of "offset" is out of range');
    }
    return (this[offset + 4] | (this[offset + 4] & Math.pow(2, 7)) * 0x1fffffe) * Math.pow(2, 32) +
      this.calculationLE(offset, 4);
  }

  private readInt24LE(offset: number = 0) {
    if (offset < 0 || offset > this.length - 3) {
      throw new RangeError('The value of "offset" is out of range');
    }
    const val = this.calculationLE(offset, 3);
    return val | (val & Math.pow(2, 23)) * 0x1fe;
  }

  writeUIntLE(value: number, offset: number, byteLength: number): number | undefined {
    return this.writeData(value, offset, byteLength, Style.UIntLE);
  }

  readUIntLE(offset: number, byteLength: number): number | undefined {
    if (offset < 0 || offset > this.length - 1) {
      throw new RangeError('The value of "offset" is out of range');
    }

    return this.readData(offset, byteLength, Style.UIntLE);
  }

  private readUInt48LE(offset: number = 0) {
    if (offset < 0 || offset > this.length - 6) {
      throw new RangeError('The value of "offset" is out of range');
    }
    return this.calculationLE(offset, 4) +
      (this.calculationLE(offset + 4, 2)) * Math.pow(2, 32);
  }

  private readUInt40LE(offset: number = 0) {
    if (offset < 0 || offset > this.length - 5) {
      throw new RangeError('The value of "offset" is out of range');
    }
    return this.calculationLE(offset, 5);
  }

  private readUInt24LE(offset: number = 0) {
    if (offset < 0 || offset > this.length - 3) {
      throw new RangeError('The value of "offset" is out of range');
    }
  
    return this.calculationLE(offset, 3);
  }

  writeUIntBE(value: number, offset: number, byteLength: number): number | undefined {
    return this.writeData(value, offset, byteLength, Style.UIntBE);
  }

  readUIntBE(offset: number, byteLength: number): number | undefined {
    if (offset < 0 || offset > this.length - 1) {
      throw new RangeError('The value of "offset" is out of range');
    }

    return this.readData(offset, byteLength, Style.UIntBE);
  }

  private readUInt48BE(offset: number = 0) {
    if (offset < 0 || offset > this.length - 6) {
      throw new RangeError('The value of "offset" is out of range');
    }
    return (this.calculationBE(offset, 2)) * Math.pow(2, 32) +
      this.calculationBE(offset + 2, 4);
  }

  private readUInt40BE(offset: number = 0) {
    if (offset < 0 || offset > this.length - 5) {
      throw new RangeError('The value of "offset" is out of range');
    }
    return this.calculationBE(offset, 5);
  }
  
  private readUInt24BE(offset: number = 0) {
    if (offset < 0 || offset > this.length - 3) {
      throw new RangeError('The value of "offset" is out of range');
    }
    return this.calculationBE(offset, 3);
  }

  checkRangeWriteInteger(value: number, offset: number, isInt: boolean): void {
    const curMaxValue = isInt ? maxIntValue : maxUIntValue;
    const curMinValue = isInt ? -maxIntValue : 0;
    if(value < curMinValue || value > (curMaxValue - 1)) {
      throw new RangeError('The value of "value" is out of range');
    }
    if (offset < 0 || offset > this.length - 4) {
      throw new RangeError(`The value of "offset" is out of range`);
    }
  }
  checkRangeWriteDouble(offset: number): void {
    if (offset < 0 || offset > this.length - 8) {
      throw new RangeError('The value of "offset" is out of range');
    }
  }
  checkRangeWriteFloat(offset: number): void {
    if (offset < 0 || offset > this.length - 4) {
      throw new RangeError('The value of "offset" is out of range');
    }
  }

  writeInt32BE(value: number, offset: number = 0): number {
    this.checkRangeWriteInteger(value, offset, true);
    
    value = +value;
    this[bufferSymbol].writeInt32BE(value, offset);
    return offset + 4;
  }

  readInt32BE(offset: number = 0): number {
    return this[bufferSymbol].readInt32BE(offset);
  }

  writeInt32LE(value: number, offset: number = 0): number {
    this.checkRangeWriteInteger(value, offset, true);

    value = +value;
    this[bufferSymbol].writeInt32LE(value, offset);
    return offset + 4;
  }

  readInt32LE(offset: number = 0) : number {
    return this[bufferSymbol].readInt32LE(offset);
  }

  writeUInt32BE(value: number, offset: number = 0): number {
    this.checkRangeWriteInteger(value, offset, false);
    value = +value;
    this[bufferSymbol].writeUInt32BE(value, offset);
    return offset + 4;
  }

  readUInt32BE(offset: number = 0): number {
    return this[bufferSymbol].readUInt32BE(offset);
  }

  writeUInt32LE(value: number, offset: number = 0): number {
    this.checkRangeWriteInteger(value, offset, false);
    value = +value;
    this[bufferSymbol].writeUInt32LE(value, offset);
    return offset + 4;
  }

  readUInt32LE(offset: number = 0): number {
    return this[bufferSymbol].readUInt32LE(offset);
  }

  writeDoubleBE(value: number, offset: number = 0): number {
    this.checkRangeWriteDouble(offset);

    value = +value;
    float64Array[0] = value;
    let i: number = 7; // 7 is uInt8Float64Array->maxIndex
    while (i >= 0) {
      this[offset++] = uInt8Float64Array[i--];
    }
    return offset;
  } 

  readDoubleBE(offset: number = 0): number {
    const first = this[offset];
    const last = this[offset + 7];
    if (first === undefined || last === undefined) {
      throw new RangeError('The value of "offset" is out of range');
    }

    let i: number = 7; // 7 is uInt8Float64Array->maxIndex
    while (i >= 0) {
      uInt8Float64Array[i--] = this[offset++];
    }
    return float64Array[0];
  }

  writeDoubleLE(value: number, offset: number = 0): number {
    this.checkRangeWriteDouble(offset);
    
    value = +value;
    float64Array[0] = value;
    let i: number = 0; 
    while (i <= 7) { // 7 is uInt8Float64Array->maxIndex
      this[offset++] = uInt8Float64Array[i++];
    }
    return offset;
  }

  readDoubleLE(offset: number = 0): number {
    const first = this[offset];
    const last = this[offset + 7];
    if (first === undefined || last === undefined) {
      throw new RangeError('The value of "offset" is out of range');
    }

    let i: number = 0; 
    while (i <= 7) { // 7 is uInt8Float64Array->maxIndex
      uInt8Float64Array[i++] = this[offset++];
    }
    return float64Array[0];
  }

  writeFloatBE(value: number, offset: number = 0): number {
    this.checkRangeWriteFloat(offset);

    value = +value;
    float32Array[0] = value;
    let i: number = 3; // 3 is uInt8Float32Array->maxIndex
    while (i >= 0) {
      this[offset++] = uInt8Float32Array[i--];
    }
    return offset;
  }

  readFloatBE(offset: number = 0): number {
    const first = this[offset];
    const last = this[offset + 3];
    if (first === undefined || last === undefined) {
      throw new RangeError('The value of "offset" is out of range');
    }

    let i: number = 3; // 3 is uInt8Float32Array->maxIndex
    while (i >= 0) {
      uInt8Float32Array[i--] = this[offset++];
    }
    return float32Array[0];
  }

  writeFloatLE(value: number, offset: number = 0): number {
    this.checkRangeWriteFloat(offset);

    value = +value;
    float32Array[0] = value;
    let i: number = 0;
    while (i <= 3) {  // 3 is uInt8Float32Array->maxIndex
      this[offset++] = uInt8Float32Array[i++];
    }
    return offset;
  }

  readFloatLE(offset: number): number { 
    const first = this[offset];
    const last = this[offset + 3];
    if (first === undefined || last === undefined) {
      throw new RangeError('The value of "offset" is out of range');
    }

    let i: number = 0;
    while (i <= 3) { // 3 is uInt8Float32Array->maxIndex
      uInt8Float32Array[i++] = this[offset++];
    }
    return float32Array[0];
  }

  writeUInt16BE(value: number, offset: number = 0): number {
    if (offset < 0 || offset > this.length - 2) {
      throw new RangeError('The value of "offset" is out of range');
    }
    if (value < 0 || value > Math.pow(2, 16)) {
      throw new RangeError('The value of "value" is out of range');
    }
    value = +value;
    this[offset++] = (value >>> 8);
    this[offset++] = value;
    return offset;
  }

  readUInt16BE(offset: number = 0) : number {
    if (offset < 0 || offset > this.length - 2) {
      throw new RangeError('The value of "offset" is out of range');
    }
    const first = this[offset];
    const last = this[offset + 1];
    if (first === undefined || last === undefined) {
      throw new RangeError('The value of "offset" is out of range');
    }
    return first * Math.pow(2, 8) + last;
  }

  writeUInt16LE(value: number, offset: number = 0): number {
    if (offset < 0 || offset > this.length - 2) {
      throw new RangeError('The value of "offset" is out of range');
    }
    if (value < 0 || value > Math.pow(2, 16)) {
      throw new RangeError('The value of "value" is out of range');
    }
    value = +value;
    this[offset++] = value;
    this[offset++] = (value >>> 8);
    return offset;
  }

  compare(target: Buffer | Uint8Array, targetStart: number = 0, targetEnd: number = target.length,
          sourceStart: number = 0, sourceEnd: number = this.length): -1 | 0 | 1 {
    if (!(target instanceof Buffer) && !(target instanceof Uint8Array)) {
      throw new Error(`The "target" argument must be an instance of Buffer or Uint8Array`);
    }
    throwError(targetStart, 'number', 'targetStart');
    throwError(targetEnd, 'number', 'targetEnd');
    throwError(sourceStart, 'number', 'sourceStart');
    throwError(sourceEnd, 'number', 'sourceEnd');
    if (sourceStart >= sourceEnd) {
      return (targetStart >= targetEnd ? 0 : -1);
    }
    if (targetStart >= targetEnd) {
      return 1;
    }
    if (targetEnd > target.length) {
      throw new RangeError(`The value of "targetEnd" is out of range. It must be >= 0 && <= ${target.length}`)
    }
    if (sourceEnd > this.length) {
      throw new RangeError(`The value of "sourceEnd" is out of range. It must be >= 0 && <= ${this.length}`)
    }
    let length1: number = sourceEnd - sourceStart; 
    let length2: number = targetEnd - targetStart;
    let length: number = length1 > length2 ? length2 : length1;
    if (target instanceof Buffer) {
      let val = this[bufferSymbol].compare(target[bufferSymbol], targetStart, sourceStart, length);
      if (val == 0) {
        if (length1 == length2) {
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
        if (value1 == value2) {
          continue;
        }
        return value1 < value2 ? -1 : 1;
      }
      if (length1 == length2) {
        return 0;
      }
      return length1 < length2 ? -1 : 1;
    }
  }

  equals(otherBuffer: Uint8Array | Buffer): boolean {
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
    if (!(target instanceof Buffer) && !(target instanceof Uint8Array)) {
      throw new Error(`The "target" argument must be an instance of Buffer or Uint8Array`);
    }
    targetStart = isNaN(targetStart) ? 0 : Number(targetStart);
    sourceStart = isNaN(sourceStart) ? 0 : Number(sourceStart);
    sourceEnd = isNaN(sourceEnd) ? 0 : Number(sourceEnd);
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

  toString(encoding: string = 'utf8',start: number = 0, end: number = this.length): string {
    let encodObj = getEncodingByType(encoding);
    if (!encodObj) {
      throw new Error(`Unknown encoding: ${encoding}`);
    }
    start = isNaN(start) ? 0 : (Number(start) < 0 ? 0 : Number(start));
    end = isNaN(end) ? 0 : Number(end);
    let bufLength = this.length;
    if (start >= bufLength || start > end) {
      return "";
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
    if (typeof value !== 'string' && typeof value !== 'number' &&
        !(value instanceof Buffer) && !(value instanceof Uint8Array)) {
      throw new Error(`The "value" argument must be one of type number ` +
       `or string or an instance of Buffer or Uint8Array`);
    }
    if (typeof value === 'string') {
      let encodObj = getEncodingByType(encoding);
      if (!encodObj) {
        throw new Error(`Unknown encoding: ${encoding}`);
      }
      return this[bufferSymbol].indexOf(value, byteOffset, encoding, false);
    } else if (typeof value === 'number') {
      value = +value;
      if (value < 0 || value > utils.EIGHT_BITS) {
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
    if (typeof value !== 'string' && typeof value !== 'number' &&
        !(value instanceof Buffer) && !(value instanceof Uint8Array)) {
      throw new Error(`The "value" argument must be one of ` +
        `type number or string or an instance of Buffer or Uint8Array`);
    }
    if (typeof value === 'string') {
      let encodObj = getEncodingByType(encoding);
      if (!encodObj) {
        throw new Error(`Unknown encoding: ${encoding}`);
      }
      return this[bufferSymbol].indexOf(value, byteOffset, encoding, true);
    } else if (typeof value === 'number') {
      value = +value;
      if (value < 0 || value > utils.EIGHT_BITS) {
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
    const dealLen: number = 2;  // Process every 2 bits
    if (len % dealLen !== 0) {
      throw new Error('Buffer size must be a multiple of 16-bits');
    }
    return this.reverseBits(dealLen);
  }

  swap32(): Buffer {
    const len = this.length;
    const dealLen: number = 4;  // Process every 4 bits
    if (len % dealLen !== 0) {
      throw new Error('Buffer size must be a multiple of 32-bits');
    }
    return this.reverseBits(dealLen);
  }

  swap64(): Buffer {
    const len = this.length;
    const dealLen: number = 8;  // Process every 8 bits
    if (len % dealLen !== 0) {
      throw new Error('Buffer size must be a multiple of 64-bits');
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

function throwError(param: unknown, type: string, msg: string) {
  if (typeof param !== type) {
    throw new Error(`"${msg}" argument must be of type ${type}`)
  }
}

function concat(list: Buffer[] | Uint8Array[], totalLength?: number): Buffer {
  if (!(list instanceof Array)) {
    throw new Error(`The "list" argument must be an instance of Array`)
  }
  if (!(typeof totalLength === 'number' || typeof totalLength === 'undefined')) {
    throw new Error(`The "length" argument must be of type number`);
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

function alloc(size: number, fill?: string | Buffer | number, encoding?: string): Buffer
{
  if (typeof size !== 'number') {
    throw new TypeError(`The "size" argument must be of type number`);
  }
  if (size < 0 || size > MAX_LENGTH) {
    throw new RangeError(`The value of "size" is out of range`);
  }
  const buf = new Buffer(size);
  if (fill !== undefined && fill !== 0) {
    buf.fill(fill);
  }
  return buf;
}

function allocUninitializedFromPool(size: number): Buffer
{
  if (typeof size !== 'number') {
    throw new TypeError(`The "size" argument must be of type number`);
  }
  if (size < 0 || size > MAX_LENGTH) {
    throw new RangeError(`The value of "size" is out of range`);
  }
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

function allocUninitialized(size: number): Buffer
{
  if (typeof size !== 'number') {
    throw new TypeError(`The "size" argument must be of type number`);
  }
  if (size < 0 || size > MAX_LENGTH) {
    throw new RangeError(`The value of "size" is out of range`);
  }
  const buf = new Buffer(size);
  return buf;
}

function normalizeEncoding(enc: string) {
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
              offsetOrEncoding?: number | string, length?: number): Buffer
{
  if (value instanceof ArrayBuffer || value instanceof SharedArrayBuffer ) {
    offsetOrEncoding = isNaN(Number(offsetOrEncoding)) ? 0 : Number(offsetOrEncoding);
    if (offsetOrEncoding < 0) {
      throw new Error(`"offset" is outside of buffer bounds`);
    }
    if (!length) {
      length = value.byteLength - offsetOrEncoding;
    } else {
      length = isNaN(Number(length)) ? 0 : Number(length);
    }
    if (offsetOrEncoding + length > value.byteLength) {
      throw new Error(`"length" is outside of buffer bounds`);
    }
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
      createPool()
    }
    let buffer = new Buffer(pool, poolOffset, value.length);
    poolOffset += value.length;
    alignPool();
    buffer[bufferSymbol].setArray(value);
    return buffer;
  }
  let encoding = "";
  if (typeof value === 'string' || typeof value[Symbol.toPrimitive] === 'function') {
    offsetOrEncoding = offsetOrEncoding ? offsetOrEncoding :"utf8";
    if (typeof offsetOrEncoding !== 'string') {
      throw new Error('offsetOrEncoding type err');
    } else {
      offsetOrEncoding = normalizeEncoding(offsetOrEncoding);
      if (!offsetOrEncoding) {
        throw new Error('unkown encoding type');
      }
      encoding = offsetOrEncoding;
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
  throw new TypeError("The first argument must be of type string or an instance of " +
    "Buffer, ArrayBuffer, or Array or an Array-like Object. Received an instance of " + typeof value);
}

function hexStrtoNumbers(hex: string): Array<number>
{
  let arr = hex.split("");
  let nums: Array<number> = [];
  for (let i = 0, len = arr.length; i < len / 2; i++) {
    let tmp = "0x" + arr[i * 2] + arr[i * 2 + 1]
    let hexNum = Number(tmp);
    if (isNaN(hexNum)) {
      if (i == 0) {
        throw new Error(`The argument 'value' is invalid. Received "${hex}"`)
      }
      break;
    }
    nums[i] = Number(tmp);
  }
  return nums;
}

function fromString(value: string, encoding: string): Buffer
{
  let enc = normalizeEncoding(encoding);
  if (!enc) {
    throw new Error('unkown encoding type');
  }
  let size = byteLength(value, enc);
  let buffer = allocUninitializedFromPool(size);
  buffer[bufferSymbol].fromString(value, enc, size);
  buffer[lengthSymbol] = buffer[bufferSymbol].getLength();
  return buffer;
}

function isTypedArray(self: unknown) : boolean{
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

function byteLength(string: string | BackingType, encoding: string = "utf8"): number {
  if (typeof string === 'string' || isTypedArray(string) ||
      string instanceof DataView || string instanceof ArrayBuffer ||
      string instanceof SharedArrayBuffer) {
    if (string instanceof Buffer) {
      return string.length;
    } else if(typeof string === 'string') {
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
    throw new TypeError(`The "string" argument must be of type string or an instance of Buffer or ArrayBuffer. Received an instance of ${typeof string}`)
  }
}

function transcode(source: Buffer | Uint8Array, fromEnc: string, toEnc: string): Buffer {
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
    val += String.fromCharCode(+bufData[i] & 0x7F);
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
  let str = "";
  for (let i = start, len = end; i < len; i++) {
    let tmpstr = Number(bufData[i]).toString(16);
    tmpstr = (tmpstr.length === 1)? `0${tmpstr}` : tmpstr;
    str += tmpstr;
  }
  return str;
}

function toUtf16LE(self: Buffer, start: number, end: number): string {
  let bufData = self[bufferSymbol].getBufferData();
  let val = '';
  for (let i = start; i + 1 < end; i += 2) {
    val += String.fromCodePoint((bufData[i + 1] << 8) + (bufData[i]))
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
      }
    case 'ucs2':
    case 'ucs-2':
      return {
        byteLength: (str: string) => str.length * 2,
        toString: toUtf16LE
      }
    case 'ascii':
    case 'latin1':
      return {
        byteLength: (str: string) => str.length,
        toString: toAscii
      }
    case 'binary':
      return {
        byteLength: (str: string) => str.length,
        toString: toBinary
      }
    case 'utf16le':
    case 'utf-16le':
      return {
        byteLength: (str: string) => str.length * 2,
        toString: toUtf16LE
      }
    case 'base64':
    case 'base64url':
      return {
        byteLength: getBase64ByteLength,
        toString: toBase64
      }
    case 'hex':
      return {
        byteLength: (str: string) => str.length >>> 1,
        toString: toHex
      }
    default:
      return undefined;
  }
}

function getUtf8ByteLength(str: string): number {
  return InternalBuffer.utf8ByteLength(str);
}

function getBase64ByteLength(str: string): number {
  let bytes = str.length;
  for (let i = 0, len = str.length; i < len; i++) {
    if (str.charCodeAt(i) === 0x3D) {
      bytes--;
    }
    if (bytes > 1 && str.charCodeAt(i) === 0x3D) {
      bytes--;
    }
  }
  return (bytes * 3) >>> 2;
}

function compare(buf1: Buffer | Uint8Array, buf2: Buffer | Uint8Array): 1 | 0 | -1 {
  if (!(buf1 instanceof Buffer) && !(buf1 instanceof Uint8Array)) {
    throw new TypeError(`The "buf1" argument must be an instance of Buffer or Uint8Array`);
  }
  if (!(buf2 instanceof Buffer) && !(buf2 instanceof Uint8Array)) {
    throw new TypeError(`The "buf2" argument must be an instance of Buffer or Uint8Array`);
  }

  let bufData1: Array<number> | Uint8Array, bufData2: Array<number> | Uint8Array;
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
}
