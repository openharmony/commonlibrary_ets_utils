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
      if (prop === "front" || prop === "capacity" || prop === "rear") {
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
    private front: number;
    private capacity: number;
    private rear: number;
    constructor() {
      this.front = 0;
      this.capacity = 8;
      this.rear = 0;
      return new Proxy(this, new HandlerQueue());
    }
    get length(){
      return this.rear - this.front;
    }
    add(element: T): boolean {
      if (this.isFull()) {
        this.increaseCapacity();
      }
      this[this.rear] = element;
      this.rear = (this.rear + 1) % (this.capacity + 1);
      return true;
    }
    getFirst(): T {
      return this[this.front];
    }
    pop(): T {
      let result = this[this.front];
      this.front = (this.front + 1) % (this.capacity + 1);
      return result;
    }
    forEach(callbackfn: (value: T, index?: number, queue?: Queue<T>) => void,
      thisArg?: Object): void {
      let k = 0;
      let i = this.front;
      while (true) {
        callbackfn.call(thisArg,this[i], k,this);
        i = (i + 1) % this.capacity;
        k++;
        if (i === this.rear) {
          break;
        }
      }
    }
    private isFull(): boolean {
      return this.length === this.capacity;
    }
    [Symbol.iterator](): IterableIterator<T> {
      let count = this.front;
      let queue = this;
      return {
        next: function () {
          var done = count == queue.rear;
          var value = !done ? queue[count] : undefined;
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
