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
interface ArkPrivate {
  Vector: number;
  Load(key: number): Object;
}
let flag: boolean = false;
let fastVector: object = undefined;
let arkPritvate: ArkPrivate = globalThis['ArkPrivate'] || undefined;
if (arkPritvate !== undefined) {
  fastVector = arkPritvate.Load(arkPritvate.Vector);
} else {
  flag = true;
}
if (flag || fastVector === undefined) {
  class HandlerVector<T> {
    private isOutBounds(obj: Vector<T>, prop: string): void {
      let index: number = Number.parseInt(prop);
      if (Number.isInteger(index)) {
        if (index < 0 || index >= obj.length) {
          throw new RangeError('the index is out-of-bounds');
        }
      }
    }
    get(obj: Vector<T>, prop: string): T {
      if (typeof prop === 'symbol') {
        return obj[prop];
      }
      this.isOutBounds(obj, prop);
      return obj[prop];
    }
    set(obj: Vector<T>, prop: any, value: T): boolean {
      if (prop === 'elementNum' || prop === 'capacity') {
        obj[prop] = value;
        return true;
      }
      let index: number = Number.parseInt(prop);
      if (Number.isInteger(index)) {
        if (index < 0 || index > obj.length) {
          throw new RangeError('the index is out-of-bounds');
        } else {
          obj[index] = value;
          return true;
        }
      }
      return false;
    }
    deleteProperty(obj: Vector<T>, prop: string): boolean {
      this.isOutBounds(obj, prop);
      let index: number = Number.parseInt(prop);
      if (index >= 0 && index < obj.length && Number.isInteger(index)) {
        obj.removeByIndex(index);
        return true;
      }
      return false;
    }
    has(obj: Vector<T>, prop: T): boolean {
      return obj.has(prop);
    }
    ownKeys(obj: Vector<T>): Array<string> {
      let keys: string[] = [];
      for (let i: number = 0; i < obj.length; i++) {
        keys.push(i.toString());
      }
      return keys;
    }
    defineProperty(): boolean {
      return true;
    }
    getOwnPropertyDescriptor(obj: Vector<T>, prop: string): Object {
      this.isOutBounds(obj, prop);
      let index: number = Number.parseInt(prop);
      if (index >= 0 && index < obj.length && Number.isInteger(index)) {
        return Object.getOwnPropertyDescriptor(obj, prop);
      }
      return Object;
    }
    setPrototypeOf(): T {
      throw new Error(`Can't setPrototype on Vector Object`);
    }
  }
  interface IterableIterator<T> {
    next: () => {
      value: T;
      done: boolean;
    };
  }
  class Vector<T> {
    private elementNum: number = 0;
    private capacity: number = 10; // 10 : means number
    constructor() {
      return new Proxy(this, new HandlerVector());
    }
    get length(): number {
      return this.elementNum;
    }
    add(element: T): boolean {
      if (this.isFull()) {
        this.resize();
      }
      this[this.elementNum++] = element;
      return true;
    }
    insert(element: T, index: number): void {
      if (index < 0 || index >= this.elementNum) {
        throw new RangeError('the index is out-of-bounds');
      }
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
      for (let i: number = 0; i < this.elementNum; i++) {
        if (this[i] === element) {
          return true;
        }
      }
      return false;
    }
    get(index: number): T {
      return this[index];
    }
    getIndexOf(element: T): number {
      for (let i: number = 0; i < this.elementNum; i++) {
        if (element === this[i]) {
          return i;
        }
      }
      return -1;
    }
    getFirstElement(): T {
      if (this.isEmpty()) {
        return undefined;
      }
      return this[0];
    }
    set(index: number, element: T): T {
      if (index < 0 || index >= this.elementNum) {
        throw new RangeError('the index is out-of-bounds');
      }
      this[index] = element;
      return this[index];
    }
    removeByIndex(index: number): T {
      if (index < 0 || index >= this.elementNum) {
        throw new RangeError('the index is out-of-bounds');
      }
      let result: T = this[index];
      for (let i: number = index; i < this.elementNum - 1; i++) {
        this[i] = this[i + 1];
      }
      this.elementNum--;
      return result;
    }
    remove(element: T): boolean {
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
    getLastElement(): T {
      if (this.isEmpty()) {
        return undefined;
      }
      return this[this.elementNum - 1];
    }
    getLastIndexOf(element: T): number {
      for (let i: number = this.elementNum - 1; i >= 0; i--) {
        if (element === this[i]) {
          return i;
        }
      }
      return -1;
    }
    getLastIndexFrom(element: T, index: number): number {
      if (this.has(element)) {
        for (let i: number = index; i >= 0; i--) {
          if (this[i] === element) {
            return i;
          }
        }
      }
      return -1;
    }
    getIndexFrom(element: T, index: number): number {
      if (this.has(element)) {
        for (let i: number = index; i < this.elementNum; i++) {
          if (this[i] === element) {
            return i;
          }
        }
      }
      return -1;
    }
    clear(): void {
      this.elementNum = 0;
    }
    removeByRange(fromIndex: number, toIndex: number): void {
      if (fromIndex >= toIndex) {
        throw new RangeError(`the fromIndex cannot be less than or equal to toIndex`);
      }
      if (fromIndex >= this.elementNum || fromIndex < 0 || toIndex < 0) {
        throw new RangeError(`the fromIndex or the toIndex is out-of-bounds`);
      }
      toIndex = toIndex >= this.elementNum ? this.elementNum : toIndex;
      let i: number = fromIndex;
      for (let j = toIndex; j < this.elementNum; j++) {
        this[i] = this[j];
        i++;
      }
      this.elementNum -= toIndex - fromIndex;
    }
    setLength(newSize: number): void {
      if (newSize < 0) {
        throw new RangeError(`An incorrect size was set`);
      }
      this.elementNum = newSize;
    }
    replaceAllElements(callbackfn: (value: T, index?: number, vector?: Vector<T>) => T,
      thisArg?: Object): void {
      for (let i: number = 0; i < this.elementNum; i++) {
        this[i] = callbackfn.call(thisArg, this[i], i, this);
      }
    }
    forEach(callbackfn: (value: T, index?: number, vector?: Vector<T>) => void,
      thisArg?: Object): void {
      for (let i: number = 0; i < this.elementNum; i++) {
        callbackfn.call(thisArg, this[i], i, this);
      }
    }
    sort(comparator?: (firstValue: T, secondValue: T) => number): void {
      let isSort: boolean = true;
      if (comparator) {
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
        for (let i: number = 0; i < this.elementNum - 1; i++) {
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
    subVector(fromIndex: number, toIndex: number): Vector<T> {
      if (fromIndex >= toIndex) {
        throw new RangeError(`the fromIndex cannot be less than or equal to toIndex`);
      }
      if (fromIndex >= this.elementNum || fromIndex < 0 || toIndex < 0) {
        throw new RangeError(`the fromIndex or the toIndex is out-of-bounds`);
      }
      toIndex = toIndex >= this.elementNum - 1 ? this.elementNum - 1 : toIndex;
      let vector: Vector<T> = new Vector<T>();
      for (let i: number = fromIndex; i < toIndex; i++) {
        vector.add(this[i]);
      }
      return vector;
    }
    convertToArray(): Array<T> {
      let arr: Array<T> = [];
      for (let i: number = 0; i < this.elementNum; i++) {
        arr[i] = this[i];
      }
      return arr;
    }
    copyToArray(array: Array<T>): void {
      let arr: Array<T> = this.convertToArray();
      let len = array.length;
      for (let i: number = 0, arrLen = arr.length; i < arrLen; i++) {
        array[i + len] = arr[i];
      }
    }
    toString(): string {
      let str: string = '';
      str = `${this[0]}`;
      for (let i: number = 1; i < this.elementNum; i++) {
        str = `${str},${this[i]}`;
      }
      return str;
    }
    clone(): Vector<T> {
      let clone: Vector<T> = new Vector<T>();
      for (let i: number = 0; i < this.elementNum; i++) {
        clone.add(this[i]);
      }
      return clone;
    }
    getCapacity(): number {
      return this.capacity;
    }
    private isFull(): boolean {
      return this.elementNum === this.capacity;
    }
    private resize(): void {
      this.capacity = 2 * this.capacity; // 2 : means number
    }
    increaseCapacityTo(newCapacity: number): void {
      if (newCapacity >= this.elementNum) {
        this.capacity = newCapacity;
      }
    }
    trimToCurrentLength(): void {
      this.capacity = this.elementNum;
    }
    isEmpty(): boolean {
      return this.elementNum === 0;
    }
    [Symbol.iterator](): IterableIterator<T> {
      let count: number = 0;
      let vector: Vector<T> = this;
      return {
        next: function (): { done: boolean, value: T } {
          let done: boolean = false;
          let value: T = undefined;
          done = count >= vector.elementNum;
          value = done ? undefined : vector[count++];
          return {
            done: done,
            value: value,
          };
        },
      };
    }
  }
  Object.freeze(Vector);
  fastVector = Vector;
}
export default fastVector;
