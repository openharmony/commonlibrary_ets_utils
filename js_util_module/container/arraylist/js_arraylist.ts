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
  const {ErrorUtil} = requireNapi('util.struct');
  class HandlerArrayList<T> {
    private isOutBounds(obj: ArrayList<T>, prop: any): void {
      let index: number = Number.parseInt(prop);
      if (Number.isInteger(index)) {
        ErrorUtil.checkRangeError("index", index, 0, obj.length - 1);
      }
    }
    get(obj: ArrayList<T>, prop: any): T {
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
        ErrorUtil.checkRangeError("index", index, 0, obj.length);
        obj[index] = value;
        return true;
      }
      return false;
    }
    deleteProperty(obj: ArrayList<T>, prop: any): boolean {
      this.isOutBounds(obj, prop);
      let index: number = Number.parseInt(prop);
      if (index >= 0 && index < obj.length && Number.isInteger(index)) {
        obj.removeByIndex(index);
        return true;
      }
      return false;
    }
    has(obj: ArrayList<T>, prop: any): boolean {
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
    getOwnPropertyDescriptor(obj: ArrayList<T>, prop: any): Object {
      this.isOutBounds(obj, prop);
      let index: number = Number.parseInt(prop);
      if (index >= 0 && index < obj.length && Number.isInteger(index)) {
        return Object.getOwnPropertyDescriptor(obj, prop);
      }
      return;
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
    private capacity: number = 10;
    constructor() {
      ErrorUtil.checkNewTargetIsNullError("ArrayList", !new.target);
      return new Proxy(this, new HandlerArrayList());
    }
    get length(): number {
      return this.elementNum;
    }
    add(element: T): boolean {
      ErrorUtil.checkBindError("add", ArrayList, this);
      if (this.isFull()) {
        this.resize();
      }
      this[this.elementNum++] = element;
      return true;
    }
    insert(element: T, index: number): void {
      ErrorUtil.checkBindError("insert", ArrayList, this);
      ErrorUtil.checkTypeError("index", "Integer", index);
      ErrorUtil.checkRangeError("index", index, 0, this.elementNum);
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
      ErrorUtil.checkBindError("has", ArrayList, this);
      for (let i: number = 0; i < this.elementNum; i++) {
        if (this[i] === element) {
          return true;
        }
      }
      return false;
    }
    getIndexOf(element: T): number {
      ErrorUtil.checkBindError("getIndexOf", ArrayList, this);
      for (let i: number = 0; i < this.elementNum; i++) {
        if (element === this[i]) {
          return i;
        }
      }
      return -1;
    }
    removeByIndex(index: number): T {
      ErrorUtil.checkBindError("removeByIndex", ArrayList, this);
      ErrorUtil.checkTypeError("index", "Integer", index);
      ErrorUtil.checkRangeError("index", index, 0, this.elementNum - 1);
      let result: T = this[index];
      for (let i: number = index; i < this.elementNum - 1; i++) {
        this[i] = this[i + 1];
      }
      this.elementNum--;
      return result;
    }
    remove(element: T): boolean {
      ErrorUtil.checkBindError("remove", ArrayList, this);
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
      ErrorUtil.checkBindError("getLastIndexOf", ArrayList, this);
      for (let i: number = this.elementNum - 1; i >= 0; i--) {
        if (element === this[i]) {
          return i;
        }
      }
      return -1;
    }
    removeByRange(fromIndex: number, toIndex: number): void {
      ErrorUtil.checkBindError("removeByRange", ArrayList, this);
      ErrorUtil.checkTypeError("fromIndex", "Integer", fromIndex);
      ErrorUtil.checkTypeError("toIndex", "Integer", toIndex);
      ErrorUtil.checkRangeError("fromIndex", fromIndex, 0,
                                (toIndex > this.elementNum) ? this.elementNum - 1 : toIndex - 1);
      ErrorUtil.checkRangeError("toIndex", toIndex, 0, this.elementNum);
      let i: number = fromIndex;
      for (let j: number = toIndex; j < this.elementNum; j++) {
        this[i] = this[j];
        i++;
      }
      this.elementNum -= toIndex - fromIndex;
    }
    replaceAllElements(callbackfn: (value: T, index?: number, arrList?: ArrayList<T>) => T,
      thisArg?: Object): void {
      ErrorUtil.checkBindError("replaceAllElements", ArrayList, this);
      ErrorUtil.checkTypeError("callbackfn", "callable", callbackfn);
      for (let i: number = 0; i < this.elementNum; i++) {
        this[i] = callbackfn.call(thisArg, this[i], i, this);
      }
    }
    forEach(callbackfn: (value: T, index?: number, arrList?: ArrayList<T>) => void,
      thisArg?: Object): void {
      ErrorUtil.checkBindError("forEach", ArrayList, this);
      ErrorUtil.checkTypeError("callbackfn", "callable", callbackfn);
      for (let i: number = 0; i < this.elementNum; i++) {
        callbackfn.call(thisArg, this[i], i, this);
      }
    }
    sort(comparator?: (firstValue: T, secondValue: T) => number): void {
      ErrorUtil.checkBindError("sort", ArrayList, this);
      let isSort: boolean = true;
      if (comparator) {
        ErrorUtil.checkTypeError("comparator", "callable", comparator);
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
    private asciSort(curElement: any, nextElement: any): boolean {
      if ((Object.prototype.toString.call(curElement) === '[object String]' ||
        Object.prototype.toString.call(curElement) === '[object Number]') &&
        (Object.prototype.toString.call(nextElement) === '[object String]' ||
        Object.prototype.toString.call(nextElement) === '[object Number]')) {
        curElement = curElement.toString();
        nextElement = nextElement.toString();
        if (curElement > nextElement) {
          return true;
        }
        return false
      }
      return false;
    }
    subArrayList(fromIndex: number, toIndex: number): ArrayList<T> {
      ErrorUtil.checkBindError("subArrayList", ArrayList, this);
      ErrorUtil.checkTypeError("fromIndex", "Integer", fromIndex);
      ErrorUtil.checkTypeError("toIndex", "Integer", toIndex);
      ErrorUtil.checkRangeError("fromIndex", fromIndex, 0,
                                (toIndex > this.elementNum) ? this.elementNum - 1 : toIndex - 1);
      ErrorUtil.checkRangeError("toIndex", toIndex, 0, this.elementNum);
      let arraylist: ArrayList<T> = new ArrayList<T>();
      for (let i: number = fromIndex; i < toIndex; i++) {
        arraylist.add(this[i]);
      }
      return arraylist;
    }
    clear(): void {
      ErrorUtil.checkBindError("clear", ArrayList, this);
      this.elementNum = 0;
    }
    clone(): ArrayList<T> {
      ErrorUtil.checkBindError("clone", ArrayList, this);
      let clone: ArrayList<T> = new ArrayList<T>();
      for (let i: number = 0; i < this.elementNum; i++) {
        clone.add(this[i]);
      }
      return clone;
    }
    getCapacity(): number {
      ErrorUtil.checkBindError("getCapacity", ArrayList, this);
      return this.capacity;
    }
    convertToArray(): Array<T> {
      ErrorUtil.checkBindError("convertToArray", ArrayList, this);
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
      this.capacity = 1.5 * this.capacity;
    }
    isEmpty(): boolean {
      ErrorUtil.checkBindError("isEmpty", ArrayList, this);
      return this.elementNum === 0;
    }
    increaseCapacityTo(newCapacity: number): void {
      ErrorUtil.checkBindError("increaseCapacityTo", ArrayList, this);
      ErrorUtil.checkTypeError("newCapacity", "Integer", newCapacity);
      if (newCapacity >= this.elementNum) {
        this.capacity = newCapacity;
      }
    }
    trimToCurrentLength(): void {
      ErrorUtil.checkBindError("trimToCurrentLength", ArrayList, this);
      this.capacity = this.elementNum;
    }
    [Symbol.iterator](): IterableIterator<T> {
      ErrorUtil.checkBindError("Symbol.iterator", ArrayList, this);
      let count: number = 0;
      let arraylist: ArrayList<T> = this;
      return {
        next: function () {
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

