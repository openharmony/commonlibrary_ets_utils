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
declare function requireNapi(s: string): any;
interface ArkPrivate {
  PlainArray: number;
  Load(key: number): Object;
}
let flag: boolean = false;
let fastPlainArray: Object = undefined;
let arkPritvate: ArkPrivate = globalThis['ArkPrivate'] || undefined;
if (arkPritvate !== undefined) {
  fastPlainArray = arkPritvate.Load(arkPritvate.PlainArray);
} else {
  flag = true;
}
if (flag || fastPlainArray === undefined) {
  const plainAbility = requireNapi('util.struct');
  const errorUtil = plainAbility.errorUtil;
  interface IterableIterator<T> {
    next: () => {
      value: T | undefined;
      done: boolean;
    };
  }
  class HandlerPlainArray<T> {
    set(target: PlainArray<T>, p: string, value: string): boolean {
      if (p in target) {
        target[p] = value;
        return true;
      }
      return false;
    }
    defineProperty(): boolean {
      throw new Error(`Can't define Property on PlainArray Object`);
    }
    deleteProperty(): boolean {
      throw new Error(`Can't delete Property on PlainArray Object`);
    }
    setPrototypeOf(): boolean {
      throw new Error(`Can't set Prototype on PlainArray Object`);
    }
  }
  class PlainArray<T> extends plainAbility.PlainArrayClass<T> {
    constructor() {
      errorUtil.checkNewTargetIsNullError('PlainArray', !new.target);
      super();
      return new Proxy(this, new HandlerPlainArray());
    }
    get length(): number {
      return this.memberNumber;
    }
    add(key: number, value: T): void {
      errorUtil.checkBindError('add', PlainArray, this);
      errorUtil.checkTypeError('key', 'number', key);
      this.addmember(key, value);
    }
    clear(): void {
      errorUtil.checkBindError('clear', PlainArray, this);
      if (this.memberNumber !== 0) {
        this.members.keys = [];
        this.members.values = [];
        this.memberNumber = 0;
      }
    }
    clone(): PlainArray<T> {
      errorUtil.checkBindError('clone', PlainArray, this);
      let clone: PlainArray<T> = new PlainArray<T>();
      clone.memberNumber = this.memberNumber;
      clone.members.keys = this.members.keys.slice();
      clone.members.values = this.members.values.slice();
      return clone;
    }
    has(key: number): boolean {
      errorUtil.checkBindError('has', PlainArray, this);
      errorUtil.checkTypeError('key', 'number', key);
      return this.binarySearchAtPlain(key) > -1;
    }
    get(key: number): T {
      errorUtil.checkBindError('get', PlainArray, this);
      errorUtil.checkTypeError('key', 'number', key);
      let index: number = 0;
      index = this.binarySearchAtPlain(key);
      return this.members.values[index];
    }
    getIndexOfKey(key: number): number {
      errorUtil.checkBindError('getIndexOfKey', PlainArray, this);
      errorUtil.checkTypeError('key', 'number', key);
      let result: number = 0;
      result = this.binarySearchAtPlain(key);
      return result < 0 ? -1 : result;
    }
    getIndexOfValue(value: T): number {
      errorUtil.checkBindError('getIndexOfValue', PlainArray, this);
      return this.members.values.indexOf(value);
    }
    isEmpty(): boolean {
      errorUtil.checkBindError('isEmpty', PlainArray, this);
      return this.memberNumber === 0;
    }
    getKeyAt(index: number): number {
      errorUtil.checkBindError('getKeyAt', PlainArray, this);
      errorUtil.checkTypeError('index', 'Integer', index);
      return this.members.keys[index];
    }
    remove(key: number): T {
      errorUtil.checkBindError('remove', PlainArray, this);
      errorUtil.checkTypeError('key', 'number', key);
      let result: T = undefined;
      let index: number = 0;
      index = this.binarySearchAtPlain(key);
      if (index < 0) {
        return result;
      }
      return this.deletemember(index);
    }
    removeAt(index: number): T {
      errorUtil.checkBindError('removeAt', PlainArray, this);
      errorUtil.checkTypeError('index', 'Integer', index);
      return this.deletemember(index);
    }
    removeRangeFrom(index: number, size: number): number {
      errorUtil.checkBindError('removeRangeFrom', PlainArray, this);
      errorUtil.checkTypeError('index', 'Integer', index);
      errorUtil.checkTypeError('size', 'Integer', size);
      errorUtil.checkRangeError('index', index, 0, this.memberNumber - 1);
      let safeSize: number = 0;
      safeSize = (this.memberNumber - (index + size) < 0) ? this.memberNumber - index : size;
      this.deletemember(index, safeSize);
      return safeSize;
    }
    setValueAt(index: number, value: T): void {
      errorUtil.checkBindError('setValueAt', PlainArray, this);
      errorUtil.checkTypeError('index', 'Integer', index);
      errorUtil.checkRangeError('index', index, 0, this.memberNumber - 1);
      this.members.values[index] = value;
    }
    toString(): string {
      errorUtil.checkBindError('toString', PlainArray, this);
      let result: string[] = [];
      for (let i: number = 0; i < this.memberNumber; i++) {
        result.push(this.members.keys[i] + ':' + this.members.values[i]);
      }
      return result.join(',');
    }
    getValueAt(index: number): T {
      errorUtil.checkBindError('getValueAt', PlainArray, this);
      errorUtil.checkTypeError('index', 'Integer', index);
      errorUtil.checkRangeError('index', index, 0, this.memberNumber - 1);
      return this.members.values[index];
    }
    forEach(callbackfn: (value: T, index?: number, PlainArray?: PlainArray<T>) => void,
      thisArg?: Object): void {
      errorUtil.checkBindError('forEach', PlainArray, this);
      errorUtil.checkTypeError('callbackfn', 'callable', callbackfn);
      for (let i: number = 0; i < this.memberNumber; i++) {
        callbackfn.call(thisArg, this.members.values[i], this.members.keys[i]);
      }
    }
    [Symbol.iterator](): IterableIterator<[number, T]> {
      errorUtil.checkBindError('Symbol.iterator', PlainArray, this);
      let data: PlainArray<T> = this;
      let count: number = 0;
      return {
        next: function (): { done: boolean, value: [number, T] } {
          let done: boolean = false;
          let value: [number, T] = undefined;
          done = count >= data.memberNumber;
          value = done ? undefined : [data.members.keys[count], data.members.values[count]] as [number, T];
          count++;
          return {
            done: done,
            value: value,
          };
        },
      };
    }
  }
  Object.freeze(PlainArray);
  fastPlainArray = PlainArray;
}
export default fastPlainArray;
