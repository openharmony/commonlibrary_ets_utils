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
let fastStack = undefined;
let arkPritvate = globalThis["ArkPrivate"] || undefined;
if (arkPritvate !== undefined) {
  fastStack = arkPritvate.Load(arkPritvate.Stack);
} else {
  flag = true;
}
if (flag || fastStack == undefined) {
  class HandlerStack<T> {
    get(obj: Stack<T>, prop: any) {
      if (typeof prop === "symbol") {
        return obj[prop];
      }
      var index = Number.parseInt(prop);
      if (Number.isInteger(index)) {
        let length = obj.length;
        if (index < 0 || index >= length) {
          throw new Error("Stack: get out-of-bounds");
        }
      }
      return obj[prop];
    }
    set(obj: Stack<T>, prop: any, value: T) {
      if (prop === "_length" || prop === "_capacity") {
        obj[prop] = value;
        return true;
      }
      var index = Number.parseInt(prop);
      if (Number.isInteger(index)) {
        let length = obj.length;
        if (index < 0 || index >= length) {
          throw new Error("Stack: set out-of-bounds");
        } else {
          obj[index] = value;
          return true;
        }
      }
      return false;
    }
    ownKeys(obj: Stack<T>) {
      var keys = [];
      let length = obj.length;
      for (var i = 0; i < length; i++) {
        keys.push(i.toString());
      }
      return keys;
    }
    defineProperty(obj: Stack<T>, prop: any, desc: any) {
      return true;
    }
    getOwnPropertyDescriptor(obj: Stack<T>, prop: any) {
      var index = Number.parseInt(prop);
      if (Number.isInteger(index)) {
        let length = obj.length;
        if (index < 0 || index >= length) {
          throw new Error("Stack: getOwnPropertyDescriptor out-of-bounds");
        }
        return Object.getOwnPropertyDescriptor(obj, prop);
      }
      return;
    }
    setPrototypeOf(obj: any, prop: any): any {
      throw new Error("Can setPrototype on Stack Object");  
    }
  }
  interface IterableIterator<T> {
    next: () => {
      value: T;
      done: boolean;
    };
  }
  class Stack<T> {
    private _length: number = 0;
    private _capacity: number = 10;
    constructor() {
      return new Proxy(this, new HandlerStack());
    }
    get length() {
      return this._length;
    }
    push(item: T): T {
      if (this.isFull()) {
        this.increaseCapacity();
      }
      this[this._length++] = item;
      return item;
    }
    pop(): T {
      let result = this[this.length - 1];
      this._length--;
      return result;
    }
    peek(): T {
      return this[this.length - 1];
    }
    locate(element: T): number {
      for (let i = 0; i < this.length; i++) {
        if (this[i] === element) {
          return i;
        }
      }
      return -1;
    }
    isEmpty(): boolean {
      return this._length == 0;
    }
    forEach(callbackfn: (value: T, index?: number, stack?: Stack<T>) => void,
      thisArg?: Object): void {
      for (let i = 0; i < this.length; i++) {
        callbackfn.call(thisArg, this[i], i, this);
      }
    }
    private isFull(): boolean {
      return this._length === this._capacity;
    }
    private increaseCapacity(): void {
      this._capacity = 1.5 * this._capacity;
    }
    [Symbol.iterator](): IterableIterator<T> {
      let count = 0;
      let stack = this;
      return {
        next: function () {
          var done = count >= stack._length;
          var value = !done ? stack[count++] : undefined;
          return {
            done: done,
            value: value,
          };
        },
      };
    }
  }
  Object.freeze(Stack);
  fastStack = Stack;
}
export default {
  Stack: fastStack,
};
