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
declare function requireNapi(s: string): any;

let flag = false;
let fastTreeSet = undefined;
let arkPritvate = globalThis["ArkPrivate"] || undefined;
if (arkPritvate !== undefined) {
  fastTreeSet = arkPritvate.Load(arkPritvate.TreeSet);
} else {
  flag = true;
}

if (flag || fastTreeSet === undefined) {
  const RBTreeAbility = requireNapi("util.struct");
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
    defineProperty(target: TreeSet<T>, p: any): boolean {
      throw new Error("Can't define Property on TreeSet Object");
    }
    deleteProperty(target: TreeSet<T>, p: any): boolean {
      throw new Error("Can't delete Property on TreeSet Object");
    }
    setPrototypeOf(target: TreeSet<T>, p: any): boolean {
      throw new Error("Can't set Prototype on TreeSet Object");
    }
  }
  class TreeSet<T> {
    private constitute: any;
    constructor(comparator?: (firstValue: T, secondValue: T) => boolean) {
      this.constitute = new RBTreeAbility.RBTreeClass(comparator);
      return new Proxy(this, new HandlerTreeSet());
    }
    get length() {
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
      let result = this.constitute.removeNode(value);
      return result !== undefined;
    }
    clear() {
      this.constitute.clearTree();
    }
    getFirstValue(): T {
      let tempNode = this.constitute.firstNode();
      if (tempNode === undefined)
        throw new Error("don't find this key,this tree is empty");
      return tempNode.key;
    }
    getLastValue(): T {
      let tempNode = this.constitute.lastNode();
      if (tempNode === undefined)
        throw new Error("don't find this key,this tree is empty");
      return tempNode.key;
    }
    getLowerValue(key: T): T {
      let tempNode = this.constitute.getNode(key);
      if (tempNode === undefined)
        throw new Error("don't find this key,this node is undefine");
      if (tempNode.left !== undefined) return tempNode.left.key;
      let node = tempNode;
      while (node.parent !== undefined) {
        if (node.parent.right === node) return node.parent.key;
        node = node.parent; // node.parent.left === node is true;
      }
      throw new Error("don't find a key meet the conditions");
    }
    getHigherValue(key: T): T {
      let tempNode = this.constitute.getNode(key);
      if (tempNode === undefined)
        throw new Error("don't find this key,this node is undefine");
      if (tempNode.right !== undefined) return tempNode.right.key;
      let node = tempNode;
      while (node.parent !== undefined) {
        if (node.parent.left === node) return node.parent.key;
        node = node.parent; // node.parent.right === node is true;
      }
      throw new Error("don't find a key meet the conditions");
    }
    popFirst(): T {
      let firstNode = this.constitute.firstNode();
      if (firstNode === undefined)
        throw new Error("don't find first node,this tree is empty");
      let value = firstNode.value;
      this.constitute.removeNodeProcess(firstNode);
      return value as T;
    }
    popLast(): T {
      let lastNode = this.constitute.lastNode();
      if (lastNode === undefined)
        throw new Error("don't find last node,this tree is empty");
      let value = lastNode.value;
      this.constitute.removeNodeProcess(lastNode);
      return value as T;
    }
    values(): IterableIterator<T> {
      let data = this.constitute;
      let count = 0;
      return {
        next: function () {
          var done = count >= data.memberNumber;
          var value = !done ? data.keyValueArray[count].value as T : undefined;
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
      let data = this.constitute;
      let tagetArray = data.keyValueArray;
      for (let i = 0; i < data.memberNumber; i++) {
        callbackfn.call(thisArg, tagetArray[i].value as T, tagetArray[i].key);
      }
    }
    entries(): IterableIterator<[T, T]> {
      let data = this.constitute;
      let count = 0;
      return {
        next: function () {
          var done = count >= data.memberNumber;
          var value = !done ? data.keyValueArray[count].entry() : undefined;
          count++;
          return {
            done: done,
            value: value,
          };
        },
      };
    }
    [Symbol.iterator](): IterableIterator<T> {
      let data = this.constitute;
      let count = 0;
      return {
        next: function () {
          var done = count >= data.memberNumber;
          var value = !done ? data.keyValueArray[count].key : undefined;
          count++;
          return {
            done: done,
            value: value,
          };
        },
      };
    }
  }
  Object.freeze(TreeSet);
  fastTreeSet = TreeSet;
}
export default fastTreeSet;
