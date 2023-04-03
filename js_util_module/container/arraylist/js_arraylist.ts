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
interface ArkPrivate {
  ArrayList: number;
  Load(key: number): Object;
}
let flag: boolean = false;
let fastArrayList: Object = undefined;
let arkPritvate: ArkPrivate = globalThis['ArkPrivate'] || undefined;
if (arkPritvate !== undefined) {
  fastArrayList = arkPritvate.Load(arkPritvate.ArrayList);
} else {
  flag = true;
}
declare function requireNapi(s: string): any;
if (flag || fastArrayList === undefined) {
  const { errorUtil } = requireNapi('util.struct');
  class HandlerArrayList<T> {
    private isOutBounds(obj: ArrayList<T>, prop: string): void {
      let index: number = Number.parseInt(prop);
      if (Number.isInteger(index)) {
        errorUtil.checkRangeError('index', index, 0, obj.length - 1);
      }
    }
    get(obj: ArrayList<T>, prop: string): T {
      if (typeof prop === 'symbol') {
        return obj[prop];
      }
      this.isOutBounds(obj, prop);
      return obj[prop];
    }
    set(obj: ArrayList<T>, prop: any, value: T): boolean {
      if (prop === 'elementNum' || prop === 'capacity') {
        obj[prop] = value;
        return true;
      }
      let index: number = Number.parseInt(prop);
      if (Number.isInteger(index)) {
        errorUtil.checkRangeError('index', index, 0, obj.length);
        obj[index] = value;
        return true;
      }
      return false;
    }
    deleteProperty(obj: ArrayList<T>, prop: string): boolean {
      this.isOutBounds(obj, prop);
      let index: number = Number.parseInt(prop);
      if (index >= 0 && index < obj.length && Number.isInteger(index)) {
        obj.removeByIndex(index);
        return true;
      }
      return false;
    }
    has(obj: ArrayList<T>, prop: T): boolean {
      return obj.has(prop);
    }
    ownKeys(obj: ArrayList<T>): Array<string> {
      let keys: Array<string> = [];
      for (let i: number = 0; i < obj.length; i++) {
        keys.push(i.toString());
      }
      return keys;
    }
    defineProperty(): boolean {
      return true;
    }
    getOwnPropertyDescriptor(obj: ArrayList<T>, prop: string): Object {
      this.isOutBounds(obj, prop);
      let index: number = Number.parseInt(prop);
      if (index >= 0 && index < obj.length && Number.isInteger(index)) {
        return Object.getOwnPropertyDescriptor(obj, prop);
      }
      return Object;
    }
    setPrototypeOf(): T {
      throw new Error(`Can't setPrototype on ArrayList Object`);
    }
  }
  interface IterableIterator<T> {
    next: () => {
      value: T;
      done: boolean;
    };
  }
  class ArrayList<T> {
    private elementNum: number = 0;
    private capacity: number = 10; // 10 : means number
    constructor() {
      errorUtil.checkNewTargetIsNullError('ArrayList', !new.target);
      return new Proxy(this, new HandlerArrayList());
    }
    get length(): number {
      return this.elementNum;
    }
    add(element: T): boolean {
      errorUtil.checkBindError('add', ArrayList, this);
      if (this.isFull()) {
        this.resize();
      }
      this[this.elementNum++] = element;
      return true;
    }
    insert(element: T, index: number): void {
      errorUtil.checkBindError('insert', ArrayList, this);
      errorUtil.checkTypeError('index', 'Integer', index);
      errorUtil.checkRangeError('index', index, 0, this.elementNum);
      if (this.isFull()) {
        this.resize();
      }
      for (let i: number = this.elementNum; i > index; i--) {
        this[i] = this[i - 1];
      }
      this[index] = element;
      this.elementNum++;
    }
    has(element: T): boolean {
      errorUtil.checkBindError('has', ArrayList, this);
      for (let i: number = 0; i < this.elementNum; i++) {
        if (this[i] === element) {
          return true;
        }
      }
      return false;
    }
    getIndexOf(element: T): number {
      errorUtil.checkBindError('getIndexOf', ArrayList, this);
      for (let i: number = 0; i < this.elementNum; i++) {
        if (element === this[i]) {
          return i;
        }
      }
      return -1;
    }
    removeByIndex(index: number): T {
      errorUtil.checkBindError('removeByIndex', ArrayList, this);
      errorUtil.checkTypeError('index', 'Integer', index);
      errorUtil.checkRangeError('index', index, 0, this.elementNum - 1);
      let result: T = this[index];
      for (let i: number = index; i < this.elementNum - 1; i++) {
        this[i] = this[i + 1];
      }
      this.elementNum--;
      return result;
    }
    remove(element: T): boolean {
      errorUtil.checkBindError('remove', ArrayList, this);
      if (this.has(element)) {
        let index: number = this.getIndexOf(element);
        for (let i: number = index; i < this.elementNum - 1; i++) {
          this[i] = this[i + 1];
        }
        this.elementNum--;
        return true;
      }
      return false;
    }
    getLastIndexOf(element: T): number {
      errorUtil.checkBindError('getLastIndexOf', ArrayList, this);
      for (let i: number = this.elementNum - 1; i >= 0; i--) {
        if (element === this[i]) {
          return i;
        }
      }
      return -1;
    }
    removeByRange(fromIndex: number, toIndex: number): void {
      errorUtil.checkBindError('removeByRange', ArrayList, this);
      errorUtil.checkTypeError('fromIndex', 'Integer', fromIndex);
      errorUtil.checkTypeError('toIndex', 'Integer', toIndex);
      errorUtil.checkRangeError('fromIndex', fromIndex, 0,
        (toIndex > this.elementNum) ? this.elementNum - 1 : toIndex - 1);
      errorUtil.checkRangeError('toIndex', toIndex, 0, this.elementNum);
      let i: number = fromIndex;
      for (let j: number = toIndex; j < this.elementNum; j++) {
        this[i] = this[j];
        i++;
      }
      this.elementNum -= toIndex - fromIndex;
    }
    replaceAllElements(callbackfn: (value: T, index?: number, arrList?: ArrayList<T>) => T,
      thisArg?: Object): void {
      errorUtil.checkBindError('replaceAllElements', ArrayList, this);
      errorUtil.checkTypeError('callbackfn', 'callable', callbackfn);
      for (let i: number = 0; i < this.elementNum; i++) {
        this[i] = callbackfn.call(thisArg, this[i], i, this);
      }
    }
    forEach(callbackfn: (value: T, index?: number, arrList?: ArrayList<T>) => void,
      thisArg?: Object): void {
      errorUtil.checkBindError('forEach', ArrayList, this);
      errorUtil.checkTypeError('callbackfn', 'callable', callbackfn);
      for (let i: number = 0; i < this.elementNum; i++) {
        callbackfn.call(thisArg, this[i], i, this);
      }
    }
    sort(comparator?: (firstValue: T, secondValue: T) => number): void {
      errorUtil.checkBindError('sort', ArrayList, this);
      let isSort: boolean = true;
      if (comparator) {
        errorUtil.checkTypeError('comparator', 'callable', comparator);
        for (let i: number = 0; i < this.elementNum; i++) {
          for (let j: number = 0; j < this.elementNum - 1 - i; j++) {
            if (comparator(this[j], this[j + 1]) > 0) {
              isSort = false;
              let temp: T = this[j];
              this[j] = this[j + 1];
              this[j + 1] = temp;
            }
          }
        }
      } else {
        for (let i: number = 0; i < this.length - 1; i++) {
          for (let j: number = 0; j < this.elementNum - 1 - i; j++) {
            if (this.asciSort(this[j], this[j + 1])) {
              isSort = false;
              let temp: T = this[j];
              this[j] = this[j + 1];
              this[j + 1] = temp;
            }
          }
          if (isSort) {
            break;
          }
        }
      }
    }
    private asciSort(curElement: string, nextElement: string): boolean {
      if ((Object.prototype.toString.call(curElement) === '[object String]' ||
        Object.prototype.toString.call(curElement) === '[object Number]') &&
        (Object.prototype.toString.call(nextElement) === '[object String]' ||
          Object.prototype.toString.call(nextElement) === '[object Number]')) {
        curElement = curElement.toString();
        nextElement = nextElement.toString();
        if (curElement > nextElement) {
          return true;
        }
        return false;
      }
      return false;
    }
    subArrayList(fromIndex: number, toIndex: number): ArrayList<T> {
      errorUtil.checkBindError('subArrayList', ArrayList, this);
      errorUtil.checkTypeError('fromIndex', 'Integer', fromIndex);
      errorUtil.checkTypeError('toIndex', 'Integer', toIndex);
      errorUtil.checkRangeError('fromIndex', fromIndex, 0,
        (toIndex > this.elementNum) ? this.elementNum - 1 : toIndex - 1);
      errorUtil.checkRangeError('toIndex', toIndex, 0, this.elementNum);
      let arraylist: ArrayList<T> = new ArrayList<T>();
      for (let i: number = fromIndex; i < toIndex; i++) {
        arraylist.add(this[i]);
      }
      return arraylist;
    }
    clear(): void {
      errorUtil.checkBindError('clear', ArrayList, this);
      this.elementNum = 0;
    }
    clone(): ArrayList<T> {
      errorUtil.checkBindError('clone', ArrayList, this);
      let clone: ArrayList<T> = new ArrayList<T>();
      for (let i: number = 0; i < this.elementNum; i++) {
        clone.add(this[i]);
      }
      return clone;
    }
    getCapacity(): number {
      errorUtil.checkBindError('getCapacity', ArrayList, this);
      return this.capacity;
    }
    convertToArray(): Array<T> {
      errorUtil.checkBindError('convertToArray', ArrayList, this);
      let arr: Array<T> = [];
      for (let i: number = 0; i < this.elementNum; i++) {
        arr[i] = this[i];
      }
      return arr;
    }
    private isFull(): boolean {
      return this.elementNum === this.capacity;
    }
    private resize(): void {
      this.capacity = 1.5 * this.capacity; // 1.5 : means number
    }
    isEmpty(): boolean {
      errorUtil.checkBindError('isEmpty', ArrayList, this);
      return this.elementNum === 0;
    }
    increaseCapacityTo(newCapacity: number): void {
      errorUtil.checkBindError('increaseCapacityTo', ArrayList, this);
      errorUtil.checkTypeError('newCapacity', 'Integer', newCapacity);
      if (newCapacity >= this.elementNum) {
        this.capacity = newCapacity;
      }
    }
    trimToCurrentLength(): void {
      errorUtil.checkBindError('trimToCurrentLength', ArrayList, this);
      this.capacity = this.elementNum;
    }
    [Symbol.iterator](): IterableIterator<T> {
      errorUtil.checkBindError('Symbol.iterator', ArrayList, this);
      let count: number = 0;
      let arraylist: ArrayList<T> = this;
      return {
        next: function (): { done: boolean, value: T } {
          let done: boolean = false;
          let value: T = undefined;
          done = count >= arraylist.elementNum;
          value = done ? undefined : arraylist[count++];
          return {
            done: done,
            value: value,
          };
        },
      };
    }
  }
  Object.freeze(ArrayList);
  fastArrayList = ArrayList;
}
export default fastArrayList;
