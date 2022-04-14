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
declare function requireNapi(s: string): any;
interface ArkPrivate {
  TreeSet: number;
  Load(key: number): Object;
}
let flag: boolean = false;
let fastTreeSet: Object = undefined;
let arkPritvate: ArkPrivate = globalThis['ArkPrivate'] || undefined;
if (arkPritvate !== undefined) {
  fastTreeSet = arkPritvate.Load(arkPritvate.TreeSet);
} else {
  flag = true;
}
if (flag || fastTreeSet === undefined) {
  const RBTreeAbility = requireNapi('util.struct');
  interface IterableIterator<T> {
    next: () => {
      value: T | undefined;
      done: boolean;
    };
  }
  class HandlerTreeSet<T> {
    set(target: TreeSet<T>, p: any, value: any): boolean {
      if (p in target) {
        target[p] = value;
        return true;
      }
      return false;
    }
    defineProperty(): boolean {
      throw new Error(`Can't define Property on TreeSet Object`);
    }
    deleteProperty(): boolean {
      throw new Error(`Can't delete Property on TreeSet Object`);
    }
    setPrototypeOf(): boolean {
      throw new Error(`Can't set Prototype on TreeSet Object`);
    }
  }
  class TreeSet<T> {
    private constitute: any;
    constructor(comparator?: (firstValue: T, secondValue: T) => boolean) {
      this.constitute = new RBTreeAbility.RBTreeClass(comparator);
      return new Proxy(this, new HandlerTreeSet());
    }
    get length(): number {
      return this.constitute.memberNumber;
    }
    isEmpty(): boolean {
      return this.constitute.isEmpty();
    }
    has(value: T): boolean {
      return this.constitute.getNode(value) !== undefined;
    }
    add(value: T): boolean {
      this.constitute.addNode(value);
      return true;
    }
    remove(value: T): boolean {
      let result: T = undefined;
      result = this.constitute.removeNode(value);
      return result !== undefined;
    }
    clear() {
      this.constitute.clearTree();
    }
    getFirstValue(): T {
      let tempNode: any = undefined;
      tempNode = this.constitute.firstNode();
      if (tempNode === undefined) {
        return tempNode;
      }
      return tempNode.key;
    }
    getLastValue(): T {
      let tempNode: any = undefined;
      tempNode = this.constitute.lastNode();
      if (tempNode === undefined) {
        return tempNode;
      }
      return tempNode.key;
    }
    getLowerValue(key: T): T {
      let tempNode: any = undefined;
      tempNode = this.constitute.getNode(key);
      if (tempNode === undefined) {
        return tempNode;
      }
      if (tempNode.left !== undefined) {
        return tempNode.left.key;
      }
      let node: any = tempNode;
      while (node.parent !== undefined) {
        if (node.parent.right === node) {
          return node.parent.key;
        }
        node = node.parent; // node.parent.left === node is true;
      }
      return undefined;
    }
    getHigherValue(key: T): T {
      let tempNode: any = undefined;
      tempNode = this.constitute.getNode(key);
      if (tempNode === undefined) {
        return tempNode;
      }
      if (tempNode.right !== undefined) {
        return tempNode.right.key;
      }
      let node: any = tempNode;
      while (node.parent !== undefined) {
        if (node.parent.left === node) {
          return node.parent.key;
        }
        node = node.parent; // node.parent.right === node is true;
      }
      undefined;
    }
    popFirst(): T {
      let firstNode: any = undefined;
      firstNode = this.constitute.firstNode();
      if (firstNode === undefined) {
        return firstNode;
      }
      let value: T = firstNode.value;
      this.constitute.removeNodeProcess(firstNode);
      return value;
    }
    popLast(): T {
      let lastNode: any = undefined;
      lastNode = this.constitute.lastNode();
      if (lastNode === undefined) {
        return lastNode;
      }
      let value: T = lastNode.value;
      this.constitute.removeNodeProcess(lastNode);
      return value;
    }
    values(): IterableIterator<T> {
      let data: any = this.constitute;
      let count: number = 0;
      return {
        next: function () {
          let done: boolean = false;
          let value: T = undefined;
          done = count >= data.memberNumber;
          value = done ? undefined : data.keyValueArray[count].value as T;
          count++;
          return {
            done: done,
            value: value,
          };
        },
      };
    }
    forEach(callbackfn: (value?: T, key?: T, set?: TreeSet<T>) => void,
      thisArg?: Object): void {
      let data: any = this.constitute;
      let tagetArray: Array<any> = data.keyValueArray;
      for (let i: number = 0; i < data.memberNumber; i++) {
        callbackfn.call(thisArg, tagetArray[i].value as T, tagetArray[i].key);
      }
    }
    entries(): IterableIterator<[T, T]> {
      let data: any = this.constitute;
      let count: number = 0;
      return {
        next: function () {
          let done: boolean = false;
          let value: [T, T] = undefined;
          done = count >= data.memberNumber;
          value = done ? undefined : data.keyValueArray[count].entry();
          count++;
          return {
            done: done,
            value: value,
          };
        },
      };
    }
    [Symbol.iterator](): IterableIterator<T> {
      return this.values();
    }
  }
  Object.freeze(TreeSet);
  fastTreeSet = TreeSet;
}
export default fastTreeSet;
