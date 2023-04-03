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
  List: number;
  Load(key: number): Object;
}
let flag: boolean = false;
let fastList: Object = undefined;
let arkPritvate: ArkPrivate = globalThis['ArkPrivate'] || undefined;
if (arkPritvate !== undefined) {
  fastList = arkPritvate.Load(arkPritvate.List);
} else {
  flag = true;
}
declare function requireNapi(s: string): any;
if (flag || fastList === undefined) {
  const { errorUtil } = requireNapi('util.struct');
  class HandlerList<T> {
    get(obj: List<T>, prop: string): T {
      if (typeof prop === 'symbol') {
        return obj[prop];
      }
      let index: number = Number.parseInt(prop);
      if (Number.isInteger(index)) {
        errorUtil.checkRangeError('index', index, 0, obj.length - 1);
        return obj.get(index);
      }
      return obj[prop];
    }
    set(obj: List<T>, prop: string, value: T): boolean {
      if (prop === 'elementNum' ||
        prop === 'capacity' ||
        prop === 'head' ||
        prop === 'next') {
        obj[prop] = value;
        return true;
      }
      let index: number = Number.parseInt(prop);
      if (Number.isInteger(index)) {
        errorUtil.checkRangeError('index', index, 0, obj.length);
        obj.set(index, value);
        return true;
      }
      return false;
    }
    deleteProperty(obj: List<T>, prop: string): boolean {
      let index: number = Number.parseInt(prop);
      if (Number.isInteger(index)) {
        errorUtil.checkRangeError('index', index, 0, obj.length - 1);
        obj.removeByIndex(index);
        return true;
      }
      return false;
    }
    has(obj: List<T>, prop: T): boolean {
      return obj.has(prop);
    }
    ownKeys(obj: List<T>): Array<string> {
      let keys: Array<string> = [];
      for (let i: number = 0; i < obj.length; i++) {
        keys.push(i.toString());
      }
      return keys;
    }
    defineProperty(): boolean {
      return true;
    }
    getOwnPropertyDescriptor(obj: List<T>, prop: string): Object {
      let index: number = Number.parseInt(prop);
      if (Number.isInteger(index)) {
        errorUtil.checkRangeError('index', index, 0, obj.length - 1);
        return Object.getOwnPropertyDescriptor(obj, prop);
      }
      return Object;
    }
    setPrototypeOf(): T {
      throw new Error(`Can't setPrototype on List Object`);
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
    constructor(element: T, next?: NodeObj<T>) {
      this.element = element;
      this.next = next;
    }
  }
  class List<T> {
    private head: NodeObj<T>;
    private elementNum: number;
    private capacity: number;
    constructor() {
      errorUtil.checkNewTargetIsNullError('List', !new.target);
      this.head = undefined;
      this.elementNum = 0;
      this.capacity = 10;
      return new Proxy(this, new HandlerList());
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
      errorUtil.checkBindError('get', List, this);
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
      errorUtil.checkBindError('add', List, this);
      let node: NodeObj<T> = new NodeObj(element);
      if (this.head === undefined) {
        this.head = node;
      } else {
        let current: NodeObj<T> = this.head;
        while (current.next !== undefined) {
          current = current.next;
        }
        current.next = node;
      }
      this.elementNum++;
      return true;
    }
    clear(): void {
      errorUtil.checkBindError('clear', List, this);
      this.head = undefined;
      this.elementNum = 0;
    }
    has(element: T): boolean {
      errorUtil.checkBindError('has', List, this);
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
    equal(obj: Object): boolean {
      errorUtil.checkBindError('equal', List, this);
      if (obj === this) {
        return true;
      }
      if (!(obj instanceof List)) {
        return false;
      } else {
        let e1: NodeObj<T> = this.head;
        let e2: NodeObj<T> = obj.head;
        if (e1 !== undefined && e2 !== undefined) {
          while (e1.next !== undefined && e2.next !== undefined) {
            e1 = e1.next;
            e2 = e2.next;
            if (e1.element !== e2.element) {
              return false;
            }
          }
          return !(e1.next !== undefined || e2.next !== undefined);
        } else if (e1 !== undefined && e2 === undefined) {
          return false;
        } else if (e1 === undefined && e2 !== undefined) {
          return false;
        } else {
          return true;
        }
      }
    }
    getIndexOf(element: T): number {
      errorUtil.checkBindError('getIndexOf', List, this);
      for (let i: number = 0; i < this.elementNum; i++) {
        let curNode: NodeObj<T> = undefined;
        curNode = this.getNode(i);
        if (curNode !== undefined && curNode.element === element) {
          return i;
        }
      }
      return -1;
    }
    getLastIndexOf(element: T): number {
      errorUtil.checkBindError('getLastIndexOf', List, this);
      for (let i: number = this.elementNum - 1; i >= 0; i--) {
        let curNode: NodeObj<T> = undefined;
        curNode = this.getNode(i);
        if (curNode !== undefined && curNode.element === element) {
          return i;
        }
      }
      return -1;
    }
    removeByIndex(index: number): T {
      errorUtil.checkBindError('removeByIndex', List, this);
      errorUtil.checkTypeError('index', 'Integer', index);
      errorUtil.checkRangeError('index', index, 0, this.elementNum - 1);
      let oldNode: NodeObj<T> = this.head;
      if (index === 0) {
        oldNode = this.head;
        this.head = oldNode && oldNode.next;
      } else {
        let prevNode: NodeObj<T> = undefined;
        prevNode = this.getNode(index - 1);
        oldNode = prevNode.next;
        prevNode.next = oldNode.next;
      }
      this.elementNum--;
      return oldNode && oldNode.element;
    }
    remove(element: T): boolean {
      errorUtil.checkBindError('remove', List, this);
      if (this.has(element)) {
        let index: number = 0;
        index = this.getIndexOf(element);
        this.removeByIndex(index);
        return true;
      }
      return false;
    }
    replaceAllElements(callbackfn: (value: T, index?: number, list?: List<T>) => T,
      thisArg?: Object): void {
      errorUtil.checkBindError('replaceAllElements', List, this);
      errorUtil.checkTypeError('callbackfn', 'callable', callbackfn);
      let index: number = 0;
      if (this.head !== undefined) {
        let current: NodeObj<T> = this.head;
        if (this.elementNum > 0) {
          this.getNode(index).element = callbackfn.call(thisArg, this.head.element, index, this);
        }
        while (current.next !== undefined) {
          current = current.next;
          this.getNode(++index).element = callbackfn.call(thisArg, current.element, index, this);
        }
      }
    }
    getFirst(): T {
      errorUtil.checkBindError('getFirst', List, this);
      if (this.isEmpty()) {
        return undefined;
      }
      let element: T = this.head.element;
      return element;
    }
    getLast(): T {
      errorUtil.checkBindError('getLast', List, this);
      if (this.isEmpty()) {
        return undefined;
      }
      let newNode: NodeObj<T> = undefined;
      newNode = this.getNode(this.elementNum - 1);
      let element: T = newNode.element;
      return element;
    }
    insert(element: T, index: number): void {
      errorUtil.checkBindError('insert', List, this);
      errorUtil.checkTypeError('index', 'Integer', index);
      errorUtil.checkRangeError('index', index, 0, this.elementNum);
      let newNode: NodeObj<T> = undefined;
      newNode = new NodeObj(element);
      if (index === 0) {
        let current: NodeObj<T> = this.head;
        newNode.next = current;
        this.head = newNode;
      } else {
        let prevNode: NodeObj<T> = undefined;
        prevNode = this.getNode(index - 1);
        newNode.next = prevNode.next;
        prevNode.next = newNode;
      }
      this.elementNum++;
    }
    set(index: number, element: T): T {
      errorUtil.checkBindError('set', List, this);
      errorUtil.checkTypeError('index', 'Integer', index);
      errorUtil.checkRangeError('index', index, 0, this.length - 1);
      let current: NodeObj<T> = undefined;
      current = this.getNode(index);
      current.element = element;
      return current.element;
    }
    sort(comparator: (firstValue: T, secondValue: T) => number): void {
      errorUtil.checkBindError('sort', List, this);
      errorUtil.checkTypeError('comparator', 'callable', comparator);
      let isSort: boolean = true;
      for (let i: number = 0; i < this.elementNum; i++) {
        for (let j: number = 0; j < this.elementNum - 1 - i; j++) {
          if (
            comparator(this.getNode(j).element, this.getNode(j + 1).element) > 0
          ) {
            isSort = false;
            let temp: T = undefined;
            temp = this.getNode(j).element;
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
      errorUtil.checkBindError('getSubList', List, this);
      errorUtil.checkTypeError('fromIndex', 'Integer', fromIndex);
      errorUtil.checkTypeError('toIndex', 'Integer', toIndex);
      errorUtil.checkRangeError('fromIndex', fromIndex, 0,
        (toIndex > this.elementNum) ? this.elementNum - 1 : toIndex - 1);
      errorUtil.checkRangeError('toIndex', toIndex, 0, this.elementNum);
      let list: List<T> = new List<T>();
      for (let i: number = fromIndex; i < toIndex; i++) {
        let element: T = undefined;
        element = this.getNode(i).element;
        list.add(element);
        if (element === undefined) {
          break;
        }
      }
      return list;
    }
    convertToArray(): Array<T> {
      errorUtil.checkBindError('convertToArray', List, this);
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
    isEmpty(): boolean {
      errorUtil.checkBindError('isEmpty', List, this);
      return this.elementNum === 0;
    }
    forEach(callbackfn: (value: T, index?: number, list?: List<T>) => void,
      thisArg?: Object): void {
      errorUtil.checkBindError('forEach', List, this);
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
      errorUtil.checkBindError('Symbol.iterator', List, this);
      let count: number = 0;
      let list: List<T> = this;
      return {
        next: function (): { done: boolean, value: T } {
          let done: boolean = false;
          let value: T = undefined;
          done = count >= list.elementNum;
          value = done ? undefined : list.getNode(count++).element;
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
export default fastList;
