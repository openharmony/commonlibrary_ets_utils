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
let fastList = undefined;
let arkPritvate = globalThis["ArkPrivate"] || undefined;
if (arkPritvate !== undefined) {
  fastList = arkPritvate.Load(arkPritvate.List);
} else {
  flag = true;
}
if (flag || fastList == undefined) {
  class HandlerList<T> {
    get(obj: List<T>, prop: any) {
      if (typeof prop === "symbol") {
        return obj[prop];
      }
      var index = Number.parseInt(prop);
      if (Number.isInteger(index)) {
        if (index < 0 || index >= obj.length) {
          throw new Error("List: get out-of-bounds");
        }
      }
      return obj[prop];
    }
    set(obj: List<T>, prop: any, value: T) {
      if (
        prop === "_length" ||
        prop === "_capacity" ||
        prop === "_head" ||
        prop == "next") {
        obj[prop] = value;
        return true;
      }
      var index = Number.parseInt(prop);
      if (Number.isInteger(index)) {
        if (index < 0 || index >= obj.length) {
          throw new Error("List: set out-of-bounds");
        } else {
          obj[index] = value;
          return true;
        }
      }
      return false;
    }
    deleteProperty(obj: List<T>, prop: any) {
      var index = Number.parseInt(prop);
      if (Number.isInteger(index)) {
        if (index < 0 || index >= obj.length) {
          throw new Error("List: deleteProperty out-of-bounds");
        }
        obj.removeByIndex(index);
        return true;
      }
      return false;
    }
    has(obj: List<T>, prop: any) {
      return obj.has(prop);
    }
    ownKeys(obj: List<T>) {
      var keys = [];
      for (var i = 0; i < obj.length; i++) {
        keys.push(i.toString());
      }
      return keys;
    }
    defineProperty(obj: List<T>, prop: any, desc: any) {
      return true;
    }
    getOwnPropertyDescriptor(obj: List<T>, prop: any) {
      var index = Number.parseInt(prop);
      if (Number.isInteger(index)) {
        if (index < 0 || index >= obj.length) {
          throw new Error("List: getOwnPropertyDescriptor out-of-bounds");
        }
        return Object.getOwnPropertyDescriptor(obj, prop);
      }
      return;
    }
    setPrototypeOf(obj: any, prop: any): any {
      throw new Error("Can setPrototype on List Object");  
    }
  }
  interface IterableIterator<T> {
    next: () => {
      value: T;
      done: boolean;
    };
  }
  class NodeObj<T> {
    /* If 'any' is changed to 'T' here, an error will be reported:
       Type 'unknown' cannot be assigned to type 'T'. */
    element: any;
    next?: NodeObj<T> | null;
    constructor(element: any, next?: NodeObj<T> | null) {
      this.element = element;
      this.next = next;
    }
  }
  class LinkIterator<T> {
    /* If 'any' is changed to 'T' here, an error will be reported:
       Property 'next' does not exist on type 'T' */
    private linkNode: any;
    constructor(linkNode: any) {
      this.linkNode = linkNode;
    }
    hasNext(): boolean {
      if (this.linkNode.next !== null) {
        return true;
      }
      return false;
    }
    next(): NodeObj<T> {
      this.linkNode = this.linkNode.next;
      return this.linkNode;
    }
  }
  class List<T> {
    /* If 'any' is changed to 'NodeObj<T> | null ' here, an error will be reported:
       Object may be 'null' */
    private _head: any;
    private _length: number;
    private _capacity: number;
    constructor(_head?: NodeObj<T>) {
      this._head = _head || null;
      this._length = 0;
      this._capacity = 10;
      return new Proxy(this, new HandlerList());
    }
    get length() {
      return this._length;
    }
    private getNode(index: number): NodeObj<T> {
      let current = this._head;
      for (let i = 0; i < index; i++) {
        current = current["next"];
      }
      return current;
    }
    get(index: number): T {
      let current = this._head;
      for (let i = 0; i < index; i++) {
        current = current["next"];
      }
      return current.element;
    }
    add(element: T): boolean {
      if (this._length === 0) {
        let head = this._head;
        this._head = new NodeObj(element, head);
      } else {
        let prevNode = this.getNode(this._length - 1);
        prevNode.next = new NodeObj(element, prevNode["next"]);
      }
      this._length++;
      return true;
    }
    clear(): void {
      this._head = null;
      this._length = 0;
    }
    has(element: T): boolean {
      if (this._head.element === element) {
        return true;
      }
      const linkIterator = new LinkIterator<T>(this._head);
      while (linkIterator.hasNext()) {
        const newNode = linkIterator.next();
        if (newNode.element === element) {
          return true;
        }
      }
      return false;
    }
    equal(obj: Object): boolean {
      if (obj === this) {
        return true;
      }
      if (!(obj instanceof List)) {
        return false;
      } else {
        let e1 = new LinkIterator<T>(this._head);
        let e2 = new LinkIterator<T>(obj._head);
        while (e1.hasNext() && e2.hasNext()) {
          const newNode1 = e1.next();
          const newNode2 = e2.next();
          if (newNode1.element !== newNode2.element) {
            return false;
          }
        }
        return !(e1.hasNext() || e2.hasNext());
      }
      return true;
    }
    getIndexOf(element: T): number {
      for (let i = 0; i < this._length; i++) {
        const curNode = this.getNode(i);
        if (curNode.element === element) {
          return i;
        }
      }
      return -1;
    }
    getLastIndexOf(element: T): number {
      for (let i = this._length - 1; i >= 0; i--) {
        const curNode = this.getNode(i);
        if (curNode.element === element) {
          return i;
        }
      }
      return -1;
    }
    removeByIndex(index: number): T {
      let oldNode = this._head;
      if (index === 0) {
        oldNode = this._head;
        this._head = oldNode && oldNode.next;
      } else {
        let prevNode = this.getNode(index - 1);
        oldNode = prevNode.next;
        prevNode.next = oldNode.next;
      }
      this._length--;
      return oldNode && oldNode.element;
    }
    remove(element: T): boolean {
      if (this.has(element)) {
        let index = this.getIndexOf(element);
        this.removeByIndex(index);
        return true;
      }
      return false;
    }
    replaceAllElements(callbackfn: (value: T, index?: number, list?: List<T>) => T,
      thisArg?: Object): void {
      let index = 0;
      const linkIterator = new LinkIterator<T>(this._head);
      if (this._length > 0) {
        const linkIterator = new LinkIterator<T>(this._head);
        this.getNode(index).element = callbackfn.call(thisArg, this._head.element, index, this);
      }
      while (linkIterator.hasNext()) {
        const newNode = linkIterator.next();
        this.getNode(++index).element = callbackfn.call(thisArg, newNode.element, index, this);
      }
    }
    getFirst(): T {
      let newNode = this.getNode(0);
      let element = newNode.element;
      return element;
    }
    getLast(): T {
      let newNode = this.getNode(this._length - 1);
      let element = newNode.element;
      return element;
    }
    insert(element: T, index: number): void {
      let newNode = new NodeObj(element);
      if (index >= 0 && index < this._length) {
        if (index === 0) {
          const current = this._head;
          newNode.next = current;
          this._head = newNode;
        } else {
          const prevNode = this.getNode(index - 1);
          newNode.next = prevNode.next;
          prevNode.next = newNode;
        }
        this._length++;
      }
    }
    set(index: number, element: T): T {
      const current = this.getNode(index);
      current.element = element;
      return current && current.element;
    }
    sort(comparator: (firstValue: T, secondValue: T) => number): void {
      let isSort = true;
      for (let i = 0; i < this._length; i++) {
        for (let j = 0; j < this._length - 1 - i; j++) {
          if (comparator(this.getNode(j).element, this.getNode(j + 1).element) > 0) {
            isSort = false;
            let temp = this.getNode(j).element;
            this.getNode(j).element = this.getNode(j + 1).element;
            this.getNode(j + 1).element = temp;
          }
        }
        if (isSort) {
          break;
        }
      }
    }
    getSubList(fromIndex: number, toIndex: number): List<T> {
      if (toIndex <= fromIndex) {
        throw new Error("toIndex cannot be less than or equal to fromIndex");
      }
      toIndex = toIndex > this._length ? this._length - 1 : toIndex;
      let list = new List<T>();
      for (let i = fromIndex; i < toIndex; i++) {
        let element = this.getNode(i).element;
        list.add(element);
        if (element === null) {
          break;
        }
      }
      return list;
    }
    convertToArray(): Array<T> {
      let arr: Array<T> = [];
      let index = 0;
      if (this._length <= 0) {
        return arr;
      }
      arr[index] = this._head.element;
      const linkIterator = new LinkIterator<T>(this._head);
      while (linkIterator.hasNext()) {
        const newNode = linkIterator.next();
        arr[++index] = newNode.element;
      }
      return arr;
    }
    isEmpty(): boolean {
      return this._length == 0;
    }
    forEach(callbackfn: (value: T, index?: number, list?: List<T>) => void,
      thisArg?: Object): void {
      let index = 0;
      const linkIterator = new LinkIterator<T>(this._head);
      if (this._length > 0) {
        const linkIterator = new LinkIterator<T>(this._head);
        callbackfn.call(thisArg, this._head.element, index, this);
      }
      while (linkIterator.hasNext()) {
        const newNode = linkIterator.next();
        callbackfn.call(thisArg, newNode.element, ++index, this);
      }
    }
    [Symbol.iterator](): IterableIterator<T> {
      let count = 0;
      let list = this
      return {
        next: function () {
          var done = count >= list._length;
          var value = !done ? list.getNode(count++).element : undefined;
          return {
            done: done,
            value: value,
          };
        },
      };
    }
  }
  Object.freeze(List);
  fastList = List;
}
export default {
  List: fastList,
};
