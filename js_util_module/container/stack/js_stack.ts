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
  Stack: number;
  Load(key: number): Object;
}
let flag: boolean = false;
let fastStack: Object = undefined;
let arkPritvate: ArkPrivate = globalThis['ArkPrivate'] || undefined;
if (arkPritvate !== undefined) {
  fastStack = arkPritvate.Load(arkPritvate.Stack);
} else {
  flag = true;
}
declare function requireNapi(s: string): any;
if (flag || fastStack === undefined) {
  const { errorUtil } = requireNapi('util.struct');
  class HandlerStack<T> {
    private isOutBounds(obj: Stack<T>, prop: string): void {
      let index: number = Number.parseInt(prop);
      if (Number.isInteger(index)) {
        errorUtil.checkRangeError('index', index, 0, obj.length - 1);
      }
    }
    get(obj: Stack<T>, prop: string): T {
      if (typeof prop === 'symbol') {
        return obj[prop];
      }
      this.isOutBounds(obj, prop);
      return obj[prop];
    }
    set(obj: Stack<T>, prop: any, value: T): boolean {
      if (prop === 'elementNum' || prop === 'capacity') {
        obj[prop] = value;
        return true;
      }
      this.isOutBounds(obj, prop);
      let index: number = Number.parseInt(prop);
      if (index >= 0 && index < obj.length && Number.isInteger(index)) {
        obj[index] = value;
        return true;
      }
      return false;
    }
    ownKeys(obj: Stack<T>): Array<string> {
      let keys: string[] = [];
      let length: number = obj.length;
      for (let i: number = 0; i < length; i++) {
        keys.push(i.toString());
      }
      return keys;
    }
    defineProperty(): boolean {
      return true;
    }
    getOwnPropertyDescriptor(obj: Stack<T>, prop: string): Object {
      this.isOutBounds(obj, prop);
      let index: number = Number.parseInt(prop);
      if (index >= 0 && index < obj.length && Number.isInteger(index)) {
        return Object.getOwnPropertyDescriptor(obj, prop);
      }
      return Object;
    }
    setPrototypeOf(): T {
      throw new Error(`Can't setPrototype on Stack Object`);
    }
  }
  interface IterableIterator<T> {
    next: () => {
      value: T;
      done: boolean;
    };
  }
  class Stack<T> {
    private elementNum: number = 0;
    private capacity: number = 10; // 10 : means number
    constructor() {
      errorUtil.checkNewTargetIsNullError('Stack', !new.target);
      return new Proxy(this, new HandlerStack());
    }
    get length(): number {
      return this.elementNum;
    }
    push(item: T): T {
      errorUtil.checkBindError('push', Stack, this);
      if (this.isFull()) {
        this.increaseCapacity();
      }
      this[this.elementNum++] = item;
      return item;
    }
    pop(): T {
      errorUtil.checkBindError('pop', Stack, this);
      if (this.isEmpty()) {
        return undefined;
      }
      let result: T = undefined;
      result = this[this.length - 1];
      this.elementNum--;
      return result;
    }
    peek(): T {
      errorUtil.checkBindError('peek', Stack, this);
      if (this.isEmpty()) {
        return undefined;
      }
      return this[this.length - 1];
    }
    locate(element: T): number {
      errorUtil.checkBindError('locate', Stack, this);
      for (let i: number = 0; i < this.length; i++) {
        if (this[i] === element) {
          return i;
        }
      }
      return -1;
    }
    isEmpty(): boolean {
      errorUtil.checkBindError('isEmpty', Stack, this);
      return this.elementNum === 0;
    }
    forEach(callbackfn: (value: T, index?: number, stack?: Stack<T>) => void,
      thisArg?: Object): void {
      errorUtil.checkBindError('forEach', Stack, this);
      errorUtil.checkTypeError('callbackfn', 'callable', callbackfn);
      for (let i: number = 0; i < this.length; i++) {
        callbackfn.call(thisArg, this[i], i, this);
      }
    }
    private isFull(): boolean {
      return this.elementNum === this.capacity;
    }
    private increaseCapacity(): void {
      this.capacity = 1.5 * this.capacity; // 1.5 : means number
    }
    [Symbol.iterator](): IterableIterator<T> {
      errorUtil.checkBindError('Symbol.iterator', Stack, this);
      let count: number = 0;
      let stack: Stack<T> = this;
      return {
        next: function (): { done: boolean, value: T } {
          let done: boolean = false;
          let value: T = undefined;
          done = count >= stack.elementNum;
          value = done ? undefined : stack[count++];
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
export default fastStack;
