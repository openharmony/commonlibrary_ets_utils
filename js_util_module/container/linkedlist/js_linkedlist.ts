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
  LinkedList: number;
  Load(key: number): Object;
}
let flag: boolean = false;
let fastLinkedList: Object = undefined;
let arkPritvate: ArkPrivate = globalThis['ArkPrivate'] || undefined;
if (arkPritvate !== undefined) {
  fastLinkedList = arkPritvate.Load(arkPritvate.LinkedList);
} else {
  flag = true;
}
declare function requireNapi(s: string): any;
if (flag || fastLinkedList === undefined) {
  const { errorUtil } = requireNapi('util.struct');
  class HandlerLinkedList<T> {
    get(obj: LinkedList<T>, prop: string): T {
      if (typeof prop === 'symbol') {
        return obj[prop];
      }
      let index: number = Number.parseInt(prop);
      let length: number = obj.length;
      if (Number.isInteger(index)) {
        errorUtil.checkRangeError('index', index, 0, length - 1);
        return obj.get(index);
      }
      return obj[prop];
    }
    set(obj: LinkedList<T>, prop: string, value: T): boolean {
      if (prop === 'elementNum' ||
        prop === 'capacity' ||
        prop === 'head' ||
        prop === 'next' ||
        prop === 'tail') {
        obj[prop] = value;
        return true;
      }
      let index: number = Number.parseInt(prop);
      if (Number.isInteger(index)) {
        let length: number = obj.length;
        errorUtil.checkRangeError('index', index, 0, length);
        obj.set(index, value);
        return true;
      }
      return false;
    }
    deleteProperty(obj: LinkedList<T>, prop: string): boolean {
      let index: number = Number.parseInt(prop);
      if (Number.isInteger(index)) {
        let length: number = obj.length;
        errorUtil.checkRangeError('index', index, 0, length - 1);
        obj.removeByIndex(index);
        return true;
      }
      return false;
    }
    has(obj: LinkedList<T>, prop: T): boolean {
      return obj.has(prop);
    }
    ownKeys(obj: LinkedList<T>): Array<string> {
      let keys: Array<string> = [];
      let length: number = obj.length;
      for (let i = 0; i < length; i++) {
        keys.push(i.toString());
      }
      return keys;
    }
    defineProperty(): boolean {
      return true;
    }
    getOwnPropertyDescriptor(obj: LinkedList<T>, prop: string): Object {
      let index: number = Number.parseInt(prop);
      if (Number.isInteger(index)) {
        let length: number = obj.length;
        errorUtil.checkRangeError('index', index, 0, length - 1);
        return Object.getOwnPropertyDescriptor(obj, prop);
      }
      return Object;
    }
    setPrototypeOf(): T {
      throw new Error(`Can't setPrototype on LinkedList Object`);
    }
  }
  interface IterableIterator<T> {
    next: () => {
      value: T;
      done: boolean;
    };
  }
  class NodeObj<T> {
    element: T;
    next?: NodeObj<T>;
    prev?: NodeObj<T>;
    constructor(element: T, next?: NodeObj<T>, prev?: NodeObj<T>) {
      this.element = element;
      this.next = next;
      this.prev = prev;
    }
  }
  class LinkedList<T> {
    private head?: NodeObj<T>;
    private tail?: NodeObj<T>;
    private elementNum: number;
    private capacity: number;
    constructor() {
      errorUtil.checkNewTargetIsNullError('LinkedList', !new.target);
      this.head = undefined;
      this.tail = undefined;
      this.elementNum = 0;
      this.capacity = 10;
      return new Proxy(this, new HandlerLinkedList());
    }
    get length(): number {
      return this.elementNum;
    }
    private getNode(index: number): NodeObj<T> | undefined {
      if (index >= 0 && index < this.elementNum) {
        let current: NodeObj<T> = this.head;
        for (let i: number = 0; i < index; i++) {
          if (current !== undefined) {
            current = current.next;
          }
        }
        return current;
      }
      return undefined;
    }
    get(index: number): T {
      errorUtil.checkBindError('get', LinkedList, this);
      errorUtil.checkTypeError('index', 'Integer', index);
      if (index >= 0 && index < this.elementNum) {
        let current: NodeObj<T> = this.head;
        for (let i: number = 0; i < index && current !== undefined; i++) {
          current = current.next;
        }
        return current.element;
      }
      return undefined;
    }
    add(element: T): boolean {
      errorUtil.checkBindError('add', LinkedList, this);
      let node: NodeObj<T> = new NodeObj(element);
      if (this.head === undefined) {
        this.head = this.tail = node;
      } else {
        let current: NodeObj<T> = this.head;
        while (current.next !== undefined) {
          current = current.next;
        }
        this.tail = current.next = node;
      }
      this.elementNum++;
      return true;
    }
    addFirst(element: T): void {
      errorUtil.checkBindError('addFirst', LinkedList, this);
      let node: NodeObj<T> = new NodeObj(element);
      if (this.elementNum === 0) {
        this.head = this.tail = node;
      } else {
        node.next = this.head;
        this.head = node;
      }
      this.elementNum++;
    }
    removeFirst(): T {
      errorUtil.checkBindError('removeFirst', LinkedList, this);
      errorUtil.checkIsEmptyError(!this.head);
      let result: T = this.head.element;
      this.removeByIndex(0);
      return result;
    }
    removeLast(): T {
      errorUtil.checkBindError('removeLast', LinkedList, this);
      errorUtil.checkIsEmptyError(!this.tail);
      let result: T = this.tail.element;
      this.removeByIndex(this.elementNum - 1);
      return result;
    }
    clear(): void {
      errorUtil.checkBindError('clear', LinkedList, this);
      this.head = undefined;
      this.tail = undefined;
      this.elementNum = 0;
    }
    has(element: T): boolean {
      errorUtil.checkBindError('has', LinkedList, this);
      if (this.head !== undefined) {
        if (this.head.element === element) {
          return true;
        }
        let current: NodeObj<T> = this.head;
        while (current.next !== undefined) {
          current = current.next;
          if (current.element === element) {
            return true;
          }
        }
      }
      return false;
    }
    getIndexOf(element: T): number {
      errorUtil.checkBindError('getIndexOf', LinkedList, this);
      for (let i: number = 0; i < this.elementNum; i++) {
        let curNode: NodeObj<T> = this.getNode(i);
        if (curNode !== undefined && curNode.element === element) {
          return i;
        }
      }
      return -1;
    }
    getLastIndexOf(element: T): number {
      errorUtil.checkBindError('getLastIndexOf', LinkedList, this);
      for (let i: number = this.elementNum - 1; i >= 0; i--) {
        let curNode: NodeObj<T> = this.getNode(i);
        if (curNode !== undefined && curNode.element === element) {
          return i;
        }
      }
      return -1;
    }
    removeByIndex(index: number): T {
      errorUtil.checkBindError('removeByIndex', LinkedList, this);
      errorUtil.checkTypeError('index', 'Integer', index);
      errorUtil.checkRangeError('index', index, 0, this.length - 1);
      if (index >= 0 && index < this.elementNum) {
        let current: NodeObj<T> = this.head;
        if (index === 0 && current !== undefined) {
          this.head = current.next;
          this.head.prev = undefined;
          if (this.elementNum === 1) {
            this.head = this.tail = undefined;
          }
        } else if (index === this.elementNum - 1) {
          current = this.getNode(index - 1);
          if (current !== undefined) {
            this.tail = current;
            current.next = undefined;
          }
        } else {
          let prevNode: NodeObj<T> = this.getNode(index - 1);
          let nextNode: NodeObj<T> = this.getNode(index + 1);
          if (prevNode !== undefined && nextNode !== undefined) {
            prevNode.next = nextNode;
            nextNode.prev = prevNode;
          }
        }
        if (current !== undefined) {
          this.elementNum--;
          return current.element;
        }
      } else {
        throw new RangeError('the index is out-of-bounds');
      }
      return undefined;
    }
    remove(element: T): boolean {
      errorUtil.checkBindError('remove', LinkedList, this);
      if (this.isEmpty()) {
        return false;
      }
      if (this.has(element)) {
        let index: number = 0;
        index = this.getIndexOf(element);
        this.removeByIndex(index);
        return true;
      }
      return false;
    }
    removeFirstFound(element: T): boolean {
      errorUtil.checkBindError('removeFirstFound', LinkedList, this);
      errorUtil.checkIsEmptyError(!this.head);
      if (this.has(element)) {
        let index: number = 0;
        index = this.getIndexOf(element);
        this.removeByIndex(index);
        return true;
      }
      return false;
    }
    removeLastFound(element: T): boolean {
      errorUtil.checkBindError('removeLastFound', LinkedList, this);
      errorUtil.checkIsEmptyError(!this.tail);
      if (this.has(element)) {
        let index: number = 0;
        index = this.getLastIndexOf(element);
        this.removeByIndex(index);
        return true;
      }
      return false;
    }
    getFirst(): T {
      errorUtil.checkBindError('getFirst', LinkedList, this);
      if (this.head !== undefined) {
        return this.head.element;
      }
      return undefined;
    }
    getLast(): T {
      errorUtil.checkBindError('getLast', LinkedList, this);
      if (this.tail !== undefined) {
        return this.tail.element;
      }
      return undefined;
    }
    insert(index: number, element: T): void {
      errorUtil.checkBindError('insert', LinkedList, this);
      errorUtil.checkTypeError('index', 'Integer', index);
      errorUtil.checkRangeError('index', index, 0, this.length);
      if (index >= 0 && index <= this.elementNum) {
        let newNode: NodeObj<T> = new NodeObj(element);
        let current: NodeObj<T> = this.head;
        if (index === 0) {
          if (this.head === undefined) {
            this.head = this.tail = newNode;
          } else {
            newNode.next = this.head;
            this.head.prev = newNode;
            this.head = newNode;
          }
        } else if (index === this.elementNum && this.elementNum !== 0) {
          let prevNode: NodeObj<T> = this.getNode(this.elementNum - 1);
          prevNode.next = this.tail = newNode;
        } else {
          let prevNode: NodeObj<T> = this.getNode(index - 1);
          current = prevNode.next;
          newNode.next = current;
          prevNode.next = newNode;
          current.prev = newNode;
          newNode.prev = prevNode;
        }
      } else {
        throw new RangeError('the index is out-of-bounds');
      }
      this.elementNum++;
    }
    set(index: number, element: T): T {
      errorUtil.checkBindError('set', LinkedList, this);
      errorUtil.checkTypeError('index', 'Integer', index);
      errorUtil.checkRangeError('index', index, 0, this.length - 1);
      let current: NodeObj<T> = undefined;
      current = this.getNode(index);
      current.element = element;
      return current.element;
    }
    convertToArray(): Array<T> {
      errorUtil.checkBindError('convertToArray', LinkedList, this);
      let arr: Array<T> = [];
      let index: number = 0;
      if (this.elementNum <= 0) {
        return arr;
      }
      if (this.head !== undefined) {
        let current: NodeObj<T> = this.head;
        arr[index] = this.head.element;
        while (current.next !== undefined) {
          current = current.next;
          arr[++index] = current.element;
        }
      }
      return arr;
    }
    clone(): LinkedList<T> {
      errorUtil.checkBindError('clone', LinkedList, this);
      let clone: LinkedList<T> = new LinkedList<T>();
      let arr: Array<T> = this.convertToArray();
      for (let i: number = 0; i < arr.length; i++) {
        let item: T = arr[i];
        clone.add(item);
      }
      return clone;
    }
    private isEmpty(): boolean {
      return this.elementNum === 0;
    }
    forEach(callbackfn: (value: T, index?: number, linkedList?: LinkedList<T>) => void,
      thisArg?: Object): void {
      errorUtil.checkBindError('forEach', LinkedList, this);
      errorUtil.checkTypeError('callbackfn', 'callable', callbackfn);
      let index: number = 0;
      if (this.head !== undefined) {
        let current: NodeObj<T> = this.head;
        if (this.elementNum > 0) {
          callbackfn.call(thisArg, this.head.element, index, this);
        }
        while (current.next !== undefined) {
          current = current.next;
          callbackfn.call(thisArg, current.element, ++index, this);
        }
      }
    }
    [Symbol.iterator](): IterableIterator<T> {
      errorUtil.checkBindError('Symbol.iterator', LinkedList, this);
      let count: number = 0;
      let linkedlist: LinkedList<T> = this;
      return {
        next: function (): { done: boolean, value: T } {
          let done: boolean = false;
          let value: T = undefined;
          done = count >= linkedlist.elementNum;
          value = done ? undefined : linkedlist.getNode(count++).element;
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
export default fastLinkedList;
