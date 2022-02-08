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
let fastLinkedList = undefined;
let arkPritvate = globalThis["ArkPrivate"] || undefined;
if (arkPritvate !== undefined) {
  fastLinkedList = arkPritvate.Load(arkPritvate.LinkedList);
} else {
  flag = true;
}
if (flag || fastLinkedList == undefined) {
  class HandlerLinkedList<T> {
    get(obj: LinkedList<T>, prop: any, receiver: any) {
      if (typeof prop === "symbol") {
        return obj[prop];
      }
      var index = Number.parseInt(prop);
      let length = obj.length;
      if (Number.isInteger(index)) {
        if (index < 0 || index >= length) {
          throw new Error("LinkedList: get out-of-bounds");
        }
        return obj.get(index);
      }
      return obj[prop];
    }
    set(obj: LinkedList<T>, prop: any, value: any) {
      if (prop === "elementNum" ||
          prop === "capacity" ||
          prop === "head" ||
          prop == "next" ||
          prop == "tail" ) {
        obj[prop] = value;
        return true;
      }
      var index = Number.parseInt(prop);
      if (Number.isInteger(index)) {
        let length = obj.length;
        if (index < 0 || index >= length) {
          throw new Error("LinkedList: set out-of-bounds");
        } else {
          obj.set(index, value);
          return true;
        }
      }
      return false;
    }
    deleteProperty(obj: LinkedList<T>, prop: any) {
      var index = Number.parseInt(prop);
      if (Number.isInteger(index)) {
        let length = obj.length;
        if (index < 0 || index >= length) {
          throw new Error("LinkedList: deleteProperty out-of-bounds");
        }
        obj.removeByIndex(index);
        return true;
      }
      return false;
    }
    has(obj: LinkedList<T>, prop: any) {
      return obj.has(prop);
    }
    ownKeys(obj: LinkedList<T>) {
      var keys = [];
      let length = obj.length;
      for (var i = 0; i < length; i++) {
        keys.push(i.toString());
      }
      return keys;
    }
    defineProperty(obj: LinkedList<T>, prop: any, desc: any) {
      return true;
    }
    getOwnPropertyDescriptor(obj: LinkedList<T>, prop: any) {
      var index = Number.parseInt(prop);
      if (Number.isInteger(index)) {
        let length = obj.length;
        if (index < 0 || index >= length) {
          throw new Error("LinkedList: getOwnPropertyDescriptor out-of-bounds");
        }
        return Object.getOwnPropertyDescriptor(obj, prop);
      }
      return;
    }
    setPrototypeOf(obj: any, prop: any): any {
      throw new Error("Can setPrototype on LinkedList Object");  
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
    prev?: NodeObj<T> | null;
    constructor(
      element: any,
      next?: NodeObj<T> | null,
      prev?: NodeObj<T> | null
    ) {
      this.element = element;
      this.next = next;
      this.prev = prev;
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
      } else {
        return false;
      }
    }
    next(): NodeObj<T> {
      this.linkNode = this.linkNode.next;
      return this.linkNode;
    }
    prev(): NodeObj<T> {
      this.linkNode = this.linkNode.prev;
      return this.linkNode;
    }
  }
  class LinkedList<T> {
    private head?: any;
    private tail?: any;
    private elementNum : number;
    private capacity: number;
    constructor(head?: NodeObj<T>, tail?: NodeObj<T>) {
      this.head = head || null;
      this.tail = tail || null;
      this.elementNum = 0;
      this.capacity = 10;
      return new Proxy(this, new HandlerLinkedList());
    }
    get length() {
      return this.elementNum;
    }
    private getNode(index: number): NodeObj<T> {
      let current = this.head;
      for (let i = 0; i < index; i++) {
        current = current["next"];
      }
      return current;
    }
    get(index: number): T {
      let current = this.head;
      for (let i = 0; i < index; i++) {
        current = current["next"];
      }
      return current.element;
    }
    add(element: T): boolean {
      if (this.elementNum === 0) {
        let head = this.head;
        let tail = this.tail;
        this.head = this.tail = new NodeObj(element, head, tail);
      } else {
        let prevNode = this.getNode(this.elementNum - 1);
        prevNode.next = new NodeObj(element, prevNode["next"], this.tail);
      }
      this.elementNum++;
      return true;
    }
    addFirst(element: T): void {
      if (!element) {
        throw new Error("element cannot be null");
      }
      let node = new NodeObj(element, this.head, this.tail);
      if (this.elementNum === 0) {
        this.head = this.tail = node;
      } else {
        node.next = this.head;
        this.head = node;
      }
      this.elementNum++;
    }
    removeFirst(): T {
      let result = this.getNode(0).element;
      this.removeByIndex(0);
      return result;
    }
    popFirst(): T {
      let result = this.getNode(0).element;
      this.removeByIndex(0);
      return result;
    }
    popLast(): T {
      let result = this.getNode(this.elementNum - 1).element;
      this.removeByIndex(this.elementNum - 1);
      return result;
    }
    removeLast(): T {
      let result = this.getNode(this.elementNum - 1).element;
      this.removeByIndex(this.elementNum - 1);
      return result;
    }
    clear(): void {
      this.head = null;
      this.tail = null;
      this.elementNum = 0;
    }
    has(element: T): boolean {
      if (this.head.element === element) {
        return true;
      }
      const linkIterator = new LinkIterator<T>(this.head);
      while (linkIterator.hasNext()) {
        const newNode = linkIterator.next();
        if (newNode.element === element) {
          return true;
        }
      }
      return false;
    }
    getIndexOf(element: T): number {
      for (let i = 0; i < this.elementNum; i++) {
        const curNode = this.getNode(i);
        if (curNode.element === element) {
          return i;
        }
      }
      return -1;
    }
    getLastIndexOf(element: T): number {
      for (let i = this.elementNum - 1; i >= 0; i--) {
        const curNode = this.getNode(i);
        if (curNode.element === element) {
          return i;
        }
      }
      return -1;
    }
    removeByIndex(index: number): T {
      if (index < 0 || index >= this.elementNum) {
        throw new Error("removeByIndex is out-of-bounds");
      }
      let current = this.head;
      if (index === 0) {
        this.head = current && current.next;
        if (this.elementNum == 1) {
          this.head = this.tail = null;
        } else {
          this.head.prev = null;
        }
      } else if (index == this.elementNum - 1) {
        current = this.getNode(index - 1);
        this.tail = current;
        current.next = null;
      } else {
        const prevNode = this.getNode(index - 1);
        const nextNode = this.getNode(index + 1);
        prevNode.next = nextNode;
        nextNode.prev = prevNode;
      }
      this.elementNum--;
      return current && current.element;
    }
    remove(element: T): boolean {
      if(this.isEmpty()){
        return false;
      }
      if (this.has(element)) {
        let index = this.getIndexOf(element);
        this.removeByIndex(index);
        return true;
      }
      return false;
    }
    removeFirstFound(element: T): boolean {
      if (this.has(element)) {
        let index = this.getIndexOf(element);
        this.removeByIndex(index);
        return true;
      } else {
        return false;
      }
    }
    removeLastFound(element: T): boolean {
      if (this.has(element)) {
        let index = this.getLastIndexOf(element);
        this.removeByIndex(index);
        return true;
      } else {
        return false;
      }
    }
    getFirst(): T {
      let newNode = this.getNode(0);
      let element = newNode.element;
      return element;
    }
    getLast(): T {
      let newNode = this.getNode(this.elementNum - 1);
      let element = newNode.element;
      return element;
    }
    insert(index: number, element: T): void {
      if (index >= 0 && index < this.elementNum) {
        let newNode = new NodeObj(element);
        let current = this.head;
        if (index === 0) {
          if (this.head === null) {
            this.head = newNode;
            this.tail = newNode;
          } else {
            newNode.next = this.head;
            this.head.prev = newNode;
            this.head = newNode;
          }
        } else {
          const prevNode = this.getNode(index - 1);
          current = prevNode.next;
          newNode.next = current;
          prevNode.next = newNode;
          current.prev = newNode;
          newNode.prev = prevNode;
        }
      } else if (index === this.elementNum) {
        let prevNode = this.getNode(this.elementNum - 1);
        prevNode.next = new NodeObj(element, prevNode["next"], this.tail);
      } else {
        throw new Error("index cannot Less than 0 and more than this length");
      }
      this.elementNum++;
    }
    set(index: number, element: T): T {
      const current = this.getNode(index);
      current.element = element;
      return current && current.element;
    }
    convertToArray(): Array<T> {
      let arr: Array<T> = [];
      let index = 0;
      if (this.elementNum <= 0) {
        return arr;
      }
      arr[index] = this.head.element;
      const linkIterator = new LinkIterator<T>(this.head);
      while (linkIterator.hasNext()) {
        const newNode = linkIterator.next();
        arr[++index] = newNode.element;
      }
      return arr;
    }
    clone(): LinkedList<T> {
      let clone = new LinkedList<T>();
      let arr = this.convertToArray();
      for (let i = 0; i < arr.length; i++) {
        let item = arr[i]
        clone.add(item);
      }
      return clone;
    }
    private isEmpty(): boolean {
      return this.elementNum == 0;
    }
    forEach(callbackfn: (value: T,index?: number,linkedlist?: LinkedList<T>) => void,
      thisArg?: Object): void {
      let index = 0;
      const linkIterator = new LinkIterator<T>(this.head);
      if (this.elementNum > 0) {
        callbackfn.call(thisArg, this.head.element, index, this);
      }
      while (linkIterator.hasNext()) {
        const newNode = linkIterator.next();
        callbackfn.call(thisArg, newNode.element, ++index, this);
      }
    }
    [Symbol.iterator](): IterableIterator<T> {
      let count = 0;
      let linkedlist = this;
      return {
        next: function () {
          var done = count >= linkedlist.elementNum;
          var value = !done ? linkedlist.getNode(count++).element : undefined;
          return {
            done: done,
            value: value,
          };
        },
      };
    }
  }
  Object.freeze(LinkedList);
  fastLinkedList = LinkedList;
}
export default {
  LinkedList: fastLinkedList,
};
