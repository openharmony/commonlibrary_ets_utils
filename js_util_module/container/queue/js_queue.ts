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
  Queue: number;
  Load(key: number): Object;
}
let flag: boolean = false;
let fastQueue: Object = undefined;
let arkPritvate: ArkPrivate = globalThis['ArkPrivate'] || undefined;
if (arkPritvate !== undefined) {
  fastQueue = arkPritvate.Load(arkPritvate.Queue);
} else {
  flag = true;
}
declare function requireNapi(s: string): any;
if (flag || fastQueue === undefined) {
  const {ErrorUtil} = requireNapi('util.struct');
  class HandlerQueue<T> {
    private isOutBounds(obj: Queue<T>, prop: any): void {
      let index: number = Number.parseInt(prop);
      if (Number.isInteger(index)) {
        ErrorUtil.checkRangeError("index", index, 0, obj.length);
      }
    }
    get(obj: Queue<T>, prop: any): T {
      if (typeof prop === 'symbol') {
        return obj[prop];
      }
      this.isOutBounds(obj, prop);
      return obj[prop];
    }
    set(obj: Queue<T>, prop: any, value: T): boolean {
      if (prop === 'front' || prop === 'capacity' || prop === 'rear') {
        obj[prop] = value;
        return true;
      }
      this.isOutBounds(obj, prop);
      let index: number = Number(prop);
      if (index >= 0 && index <= obj.length && Number.isInteger(index)) {
        obj[index] = value;
        return true;
      }
      return false;
    }
    ownKeys(obj: Queue<T>): Array<string> {
      let keys: string[] = [];
      for (let i: number = 0; i < obj.length; i++) {
        keys.push(i.toString());
      }
      return keys;
    }
    defineProperty(): boolean {
      return true;
    }
    getOwnPropertyDescriptor(obj: Queue<T>, prop: any): Object {
      this.isOutBounds(obj, prop);
      let index: number = Number.parseInt(prop);
      if (index >= 0 && index < obj.length && Number.isInteger(index)) {
        return Object.getOwnPropertyDescriptor(obj, prop);
      }
      return
    }
    setPrototypeOf(): T {
      throw new Error(`Can't setPrototype on Queue Object`);
    }
  }
  interface IterableIterator<T> {
    next: () => {
      value: T;
      done: boolean;
    };
  }
  class Queue<T> {
    private front: number;
    private capacity: number;
    private rear: number;
    constructor() {
      ErrorUtil.checkNewTargetIsNullError("Queue", !new.target);
      this.front = 0;
      this.capacity = 8;
      this.rear = 0;
      return new Proxy(this, new HandlerQueue());
    }
    get length(): number {
      return this.rear - this.front;
    }
    add(element: T): boolean {
      ErrorUtil.checkBindError("add", Queue, this);
      if (this.isFull()) {
        this.increaseCapacity();
      }
      this[this.rear] = element;
      this.rear = (this.rear + 1) % (this.capacity + 1);
      return true;
    }
    getFirst(): T {
      ErrorUtil.checkBindError("getFirst", Queue, this);
      if (this.isEmpty()) {
        return undefined;
      }
      return this[this.front];
    }
    pop(): T {
      ErrorUtil.checkBindError("pop", Queue, this);
      if (this.isEmpty()) {
        return undefined;
      }
      let result: T = undefined;
      result = this[this.front];
      this.front = (this.front + 1) % (this.capacity + 1);
      return result;
    }
    forEach(callbackfn: (value: T, index?: number, queue?: Queue<T>) => void,
      thisArg?: Object): void {
      ErrorUtil.checkBindError("forEach", Queue, this);
      ErrorUtil.checkTypeError("callbackfn", "callable", callbackfn);
      let k: number = 0;
      let i: number = this.front;
      if (this.isEmpty()) {
        return;
      } else {
        while (true) {
          callbackfn.call(thisArg, this[i], k, this);
          i = (i + 1) % this.capacity;
          k++;
          if (i === this.rear) {
            break;
          }
        }
      }
    }
    private isFull(): boolean {
      return this.length === this.capacity;
    }
    private isEmpty(): boolean {
      return this.length === 0;
    }
    [Symbol.iterator](): IterableIterator<T> {
      ErrorUtil.checkBindError("Symbol.iterator", Queue, this);
      let count: number = this.front;
      let queue: Queue<T> = this;
      return {
        next: function () {
          let done: boolean = false;
          let value: T = undefined;
          done = count === queue.rear;
          value = done ? undefined : queue[count];
          count = (count + 1) % queue.capacity;
          return {
            done: done,
            value: value,
          };
        },
      };
    }
    private increaseCapacity(): void {
      this.capacity = 2 * this.capacity;
    }
  }
  Object.freeze(Queue);
  fastQueue = Queue;
}
export default fastQueue;
