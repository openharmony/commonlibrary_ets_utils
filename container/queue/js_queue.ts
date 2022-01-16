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
let fastQueue = undefined;
let arkPritvate = globalThis["ArkPrivate"] || undefined;
if (arkPritvate !== undefined) {
  fastQueue = arkPritvate.Load(arkPritvate.Queue);
} else {
  flag = true;
}
if (flag || fastQueue == undefined) {
  class HandlerQueue<T> {
    get(obj: Queue<T>, prop: any): T {
      if (typeof prop === "symbol") {
        return obj[prop];
      }
      var index = Number.parseInt(prop);
      if (Number.isInteger(index)) {
        if (index < 0 || index > obj.length) {
          throw new Error("Queue: get out-of-bounds");
        }
      }
      return obj[prop];
    }
    set(obj: Queue<T>, prop: any, value: T): boolean {
      if (prop === "_front" || prop === "_capacity" || prop === "_rear") {
        obj[prop] = value;
        return true;
      }
      var index = Number(prop);
      if (Number.isInteger(index)) {
        if (index < 0 || index > obj.length) {
          throw new Error("Queue: set out-of-bounds");
        } else {
          obj[index] = value;
          return true;
        }
      }
      return false;
    }
    ownKeys(obj: Queue<T>) {
      var keys = [];
      for (var i = 0; i < obj.length; i++) {
        keys.push(i.toString());
      }
      return keys;
    }
    defineProperty(obj: Queue<T>, prop: any, desc: any) {
      return true;
    }
    getOwnPropertyDescriptor(obj: Queue<T>, prop: any) {
      var index = Number.parseInt(prop);
      if (Number.isInteger(index)) {
        if (index < 0 || index >= obj.length) {
          throw new Error("Queue: getOwnPropertyDescriptor out-of-bounds");
        }
        return Object.getOwnPropertyDescriptor(obj, prop);
      }
      return;
    }
    setPrototypeOf(obj: any, prop: any): any {
      throw new Error("Can setPrototype on Queue Object");  
    }
  }
  interface IterableIterator<T> {
    next: () => {
      value: T;
      done: boolean;
    };
  }
  class Queue<T> {
    private _front: number;
    private _capacity: number;
    private _rear: number;
    constructor() {
      this._front = 0;
      this._capacity = 8;
      this._rear = 0;
      return new Proxy(this, new HandlerQueue());
    }
    get length(){
      return this._rear - this._front;
    }
    add(element: T): boolean {
      if (this.isFull()) {
        this.increaseCapacity();
      }
      this[this._rear] = element;
      this._rear = (this._rear + 1) % (this._capacity + 1);
      return true;
    }
    getFirst(): T {
      return this[this._front];
    }
    pop(): T {
      let result = this[this._front];
      this._front = (this._front + 1) % (this._capacity + 1);
      return result;
    }
    forEach(callbackfn: (value: T, index?: number, queue?: Queue<T>) => void,
      thisArg?: Object): void {
      let k = 0;
      let i = this._front;
      while (true) {
        callbackfn.call(thisArg,this[i], k,this);
        i = (i + 1) % this._capacity;
        k++;
        if (i === this._rear) {
          break;
        }
      }
    }
    private isFull(): boolean {
      return this.length === this._capacity;
    }
    [Symbol.iterator](): IterableIterator<T> {
      let count = this._front;
      let queue = this;
      return {
        next: function () {
          var done = count == queue._rear;
          var value = !done ? queue[count] : undefined;
          count = (count + 1) % queue._capacity;
          return {
            done: done,
            value: value,
          };
        },
      };
    }
    private increaseCapacity(): void {
      this._capacity = 2 * this._capacity;
    }
  }
  Object.freeze(Queue);
  fastQueue = Queue;
}
export default {
  Queue: fastQueue,
};
