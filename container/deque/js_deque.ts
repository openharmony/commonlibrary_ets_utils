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
let flag = false;
let fastDeque = undefined;
let arkPritvate = globalThis["ArkPrivate"] || undefined;
if (arkPritvate !== undefined) {
  fastDeque = arkPritvate.Load(arkPritvate.Deque);
} else {
  flag = true;
}
if (flag || fastDeque == undefined) {
  class HandlerDeque<T> {
    get(obj: Deque<T>, prop: any): T {
      if (typeof prop === "symbol") {
        return obj[prop];
      }
      var index = Number.parseInt(prop);
      if (Number.isInteger(index)) {
        if (index < 0) {
          throw new Error("Deque: get out-of-bounds");
        }
      }
      return obj[prop];
    }
    set(obj: Deque<T>, prop: any, value: T): boolean {
      if (prop === "_front" || prop === "_capacity" || prop === "_rear") {
        obj[prop] = value;
        return true;
      }
      var index = Number(prop);
      if (Number.isInteger(index)) {
        if (index < 0) {
          throw new Error("Deque: set out-of-bounds");
        } else {
          obj[index] = value;
          return true;
        }
      }
      return false;
    }
    has(obj: Deque<T>, prop: any) {
      return obj.has(prop);
    }
    ownKeys(obj: Deque<T>) {
      let keys = [];
      for (let i = 0; i < obj.length; i++) {
        keys.push(i.toString());
      }
      return keys;
    }
    defineProperty(obj: Deque<T>, prop: any, desc: any) {
      return true;
    }
    getOwnPropertyDescriptor(obj: Deque<T>, prop: any) {
      var index = Number.parseInt(prop);
      if (Number.isInteger(index)) {
        if (index < 0 || index >= obj.length) {
          throw new Error("Deque: getOwnPropertyDescriptor out-of-bounds");
        }
        return Object.getOwnPropertyDescriptor(obj, prop);
      }
      return;
    }
    setPrototypeOf(obj: any, prop: any): any {
      throw new Error("Can setPrototype on Deque Object");  
    }
  }
  interface IterableIterator<T> {
    next: () => {
      value: T;
      done: boolean;
    };
  }
  class Deque<T> {
    private _front: number;
    private _capacity: number;
    private _rear: number;
    constructor() {
      this._front = 0;
      this._capacity = 8;
      this._rear = 0;
      return new Proxy(this, new HandlerDeque());
    }
    get length(){
      let result = (this._rear - this._front + this._capacity) % this._capacity;
      return result;
    }
    insertFront(element: T): void {
      if (this.isFull()) {
        this.increaseCapacity();
      }
      this._front = (this._front - 1 + this._capacity) % this._capacity;
      this[this._front] = element;
    }
    insertEnd(element: T): void {
      if (this.isFull()) {
        this.increaseCapacity();
      }
      this[this._rear] = element;
      this._rear = (this._rear + 1) % (this._capacity + 1);
    }
    getFirst(): T {
      return this[this._front];
    }
    getLast(): T {
      return this[this._rear - 1];
    }
    has(element: T): boolean {
      let result = false;
      this.forEach(function (value, index) {
        if (value == element) {
          result = true;
        }
      });
      return result;
    }
    popFirst(): T {
      let result = this[this._front];
      this._front = (this._front + 1) % (this._capacity + 1);
      return result;
    }
    popLast(): T {
      let result = this[this._rear - 1];
      this._rear = (this._rear + this._capacity) % (this._capacity + 1);
      return result;
    }
    forEach(callbackfn: (value: T, index?: number, deque?: Deque<T>) => void,
      thisArg?: Object): void {
      let k = 0;
      let i = this._front;
      while (true) {
        callbackfn.call(thisArg, this[i], k, this);
        i = (i + 1) % this._capacity;
        k++;
        if (i === this._rear) {
          break;
        }
      }
    }
    private increaseCapacity(): void {
      let count = 0;
      let arr = [];
      let length = this.length;
      while (true) {
        arr[count++] = this[this._front];
        this._front = (this._front + 1) % this._capacity;
        if (this._front === this._rear) {
          break;
        }
      }
      for (let i = 0; i < length; i++) {
        this[i] = arr[i];
      }
      this._capacity = 2 * this._capacity;
      this._front = 0;
      this._rear = length;
    }
    private isFull(): boolean {
      return (this._rear + 1) % this._capacity === this._front;
    }
    [Symbol.iterator](): IterableIterator<T> {
      let deque = this;
      let count = deque._front;
      return {
        next: function () {
          var done = count == deque._rear;
          var value = !done ? deque[count] : undefined;
          count = (count + 1) % deque._capacity;
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
export default {
  Deque: fastDeque,
};
