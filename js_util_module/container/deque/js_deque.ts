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
  Deque: number;
  Load(key: number): Object;
}
let flag: boolean = false;
let fastDeque: Object = undefined;
let arkPritvate: ArkPrivate = globalThis['ArkPrivate'] || undefined;
if (arkPritvate !== undefined) {
  fastDeque = arkPritvate.Load(arkPritvate.Deque);
} else {
  flag = true;
}
declare function requireNapi(s: string): any;
if (flag || fastDeque === undefined) {
  const { errorUtil } = requireNapi('util.struct');
  class HandlerDeque<T> {
    private isOutBounds(prop: string): void {
      let index: number = Number.parseInt(prop);
      if (Number.isInteger(index)) {
        errorUtil.checkRangeError('index', index, 0);
      }
    }
    get(obj: Deque<T>, prop: string): T {
      if (typeof prop === 'symbol') {
        return obj[prop];
      }
      this.isOutBounds(prop);
      return obj[prop];
    }
    set(obj: Deque<T>, prop: any, value: T): boolean {
      if (prop === 'front' || prop === 'capacity' || prop === 'rear') {
        obj[prop] = value;
        return true;
      }
      let index: number = Number(prop);
      if (Number.isInteger(index)) {
        errorUtil.checkRangeError('index', index, 0);
        obj[index] = value;
        return true;
      }
      return false;
    }
    has(obj: Deque<T>, prop: T): boolean {
      return obj.has(prop);
    }
    ownKeys(obj: Deque<T>): Array<string> {
      let keys: Array<string> = [];
      for (let i: number = 0; i < obj.length; i++) {
        keys.push(i.toString());
      }
      return keys;
    }
    defineProperty(): boolean {
      return true;
    }
    getOwnPropertyDescriptor(obj: Deque<T>, prop: string): Object {
      this.isOutBounds(prop);
      let index: number = Number.parseInt(prop);
      if (index >= 0 && Number.isInteger(index)) {
        return Object.getOwnPropertyDescriptor(obj, prop);
      }
      return Object;
    }
    setPrototypeOf(): T {
      throw new Error(`Can't setPrototype on Deque Object`);
    }
  }
  interface IterableIterator<T> {
    next: () => {
      value: T;
      done: boolean;
    };
  }
  class Deque<T> {
    private front: number;
    private capacity: number;
    private rear: number;
    constructor() {
      errorUtil.checkNewTargetIsNullError('Deque', !new.target);
      this.front = 0;
      this.capacity = 8;
      this.rear = 0;
      return new Proxy(this, new HandlerDeque());
    }
    get length(): number {
      let result: number = 0;
      result = (this.rear - this.front + this.capacity) % this.capacity;
      return result;
    }
    insertFront(element: T): void {
      errorUtil.checkBindError('insertFront', Deque, this);
      if (this.isFull()) {
        this.increaseCapacity();
      }
      this.front = (this.front - 1 + this.capacity) % this.capacity;
      this[this.front] = element;
    }
    insertEnd(element: T): void {
      errorUtil.checkBindError('insertEnd', Deque, this);
      if (this.isFull()) {
        this.increaseCapacity();
      }
      this[this.rear] = element;
      this.rear = (this.rear + 1) % (this.capacity + 1);
    }
    getFirst(): T {
      errorUtil.checkBindError('getFirst', Deque, this);
      if (this.isEmpty()) {
        return undefined;
      }
      return this[this.front];
    }
    getLast(): T {
      errorUtil.checkBindError('getLast', Deque, this);
      if (this.isEmpty()) {
        return undefined;
      }
      return this[this.rear - 1];
    }
    has(element: T): boolean {
      errorUtil.checkBindError('has', Deque, this);
      let result: boolean = false;
      this.forEach(function (value) {
        if (value === element) {
          result = true;
        }
      });
      return result;
    }
    popFirst(): T {
      errorUtil.checkBindError('popFirst', Deque, this);
      if (this.isEmpty()) {
        return undefined;
      }
      let result: T = undefined;
      result = this[this.front];
      this.front = (this.front + 1) % (this.capacity + 1);
      return result;
    }
    popLast(): T {
      errorUtil.checkBindError('popLast', Deque, this);
      if (this.isEmpty()) {
        return undefined;
      }
      let result: T = undefined;
      result = this[this.rear - 1];
      this.rear = (this.rear + this.capacity) % (this.capacity + 1);
      return result;
    }
    forEach(callbackfn: (value: T, index?: number, deque?: Deque<T>) => void,
      thisArg?: Object): void {
      errorUtil.checkBindError('forEach', Deque, this);
      errorUtil.checkTypeError('callbackfn', 'callable', callbackfn);
      let k: number = 0;
      let i: number = this.front;
      while (true) {
        callbackfn.call(thisArg, this[i], k, this);
        i = (i + 1) % this.capacity;
        k++;
        if (i === this.rear) {
          break;
        }
      }
    }
    private increaseCapacity(): void {
      let count: number = 0;
      let arr: Array<T> = [];
      let length: number = this.length;
      while (true) {
        arr[count++] = this[this.front];
        this.front = (this.front + 1) % this.capacity;
        if (this.front === this.rear) {
          break;
        }
      }
      for (let i: number = 0; i < length; i++) {
        this[i] = arr[i];
      }
      this.capacity = 2 * this.capacity; // 2 : means number
      this.front = 0;
      this.rear = length;
    }
    private isFull(): boolean {
      return (this.rear + 1) % this.capacity === this.front;
    }
    private isEmpty(): boolean {
      return this.length === 0;
    }
    [Symbol.iterator](): IterableIterator<T> {
      errorUtil.checkBindError('Symbol.iterator', Deque, this);
      let deque: Deque<T> = this;
      let count: number = deque.front;
      return {
        next: function (): { done: boolean, value: T } {
          let done: boolean = false;
          let value: T = undefined;
          done = count === deque.rear;
          value = done ? undefined : deque[count];
          count = (count + 1) % deque.capacity;
          return {
            done: done,
            value: value,
          };
        },
      };
    }
  }
  Object.freeze(Deque);
  fastDeque = Deque;
}
export default fastDeque;
