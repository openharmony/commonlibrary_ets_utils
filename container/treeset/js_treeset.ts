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
  const RBTreeAbility = requireNapi("struct");
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
      throw new Error("Can't TreeSet on HashMap Object");
    }
    deleteProperty(target: TreeSet<T>, p: any): boolean {
      throw new Error("Can't TreeSet on HashMap Object");
    }
    setPrototypeOf(target: TreeSet<T>, p: any): boolean {
      throw new Error("Can't TreeSet on HashMap Object");
    }
  }

  class TreeSet<T> {
    private _constitute: any;
    constructor() {
      this._constitute = new RBTreeAbility.RBTreeClass();
      return new Proxy(this, new HandlerTreeSet());
    }

    get length() {
      return this._constitute.memberNumber;
    }
    isEmpty(): boolean {
      return this._constitute.isEmpty();
    }
    has(value: T): boolean {
      return this._constitute.getNode(value) !== null;
    }
    add(value: T): boolean {
      this._constitute.addNode(value);
      return true;
    }
    remove(value: T): boolean {
      let result = this._constitute.removeNode(value);
      return result !== null;
    }
    clear() {
      this._constitute.clearTree();
    }
    getFirstValue(): T {
      let tempNode = this._constitute.firstNode();
      if (tempNode === null)
        throw new Error("don't find this key,this tree is null");
      return tempNode.key;
    }
    getLastValue(): T {
      let tempNode = this._constitute.lastNode();
      if (tempNode === null)
        throw new Error("don't find this key,this tree is null");
      return tempNode.key;
    }
    getLowerValue(key: T): T {
      let tempNode = this._constitute.getNode(key);

      if (tempNode === null)
        throw new Error("don't find this key,this node is undefine");
      if (tempNode.left !== null) return tempNode.left.key;
      let node = tempNode;
      while (node.parent !== null) {
        if (node.parent.right === node) return node.parent.key;
        node = node.parent; // node.parent.left === node is true;
      }

      throw new Error("don't find a key meet the conditions");
    }
    getHigherValue(key: T): T {
      let tempNode = this._constitute.getNode(key);

      if (tempNode === null)
        throw new Error("don't find this key,this node is undefine");
      if (tempNode.right !== null) return tempNode.right.key;
      let node = tempNode;
      while (node.parent !== null) {
        if (node.parent.left === node) return node.parent.key;
        node = node.parent; // node.parent.right === node is true;
      }

      throw new Error("don't find a key meet the conditions");
    }
    popFirst(): T {
      let firstNode = this._constitute.firstNode();
      if (firstNode === null)
        throw new Error("don't find first node,this tree is empty");
      let value = firstNode.value;
      this._constitute.removeNodeProcess(firstNode);
      return value as T;
    }
    popLast(): T {
      let lastNode = this._constitute.lastNode();
      if (lastNode === null)
        throw new Error("don't find last node,this tree is empty");
      let value = lastNode.value;
      this._constitute.removeNodeProcess(lastNode);
      return value as T;
    }
    values(): IterableIterator<T> {
      let _this = this._constitute;
      let i = 0;
      return {
        next: function () {
          var done = i >= _this.memberNumber;
          var value = !done ? _this.keyValueArray[i++].value as T : undefined;
          return {
            done: done,
            value: value,
          };
        },
      };
    }
    forEach(callbackfn: (value?: T, key?: T, set?: TreeSet<T>) => void,
      thisArg?: Object): void {
      let _this = this._constitute;
      let tagetArray = _this.keyValueArray;
      for (let i = 0; i < _this.memberNumber; i++) {
        callbackfn.call(thisArg, tagetArray[i].value as T, tagetArray[i].key);
      }
    }
    entries(): IterableIterator<[T, T]> {
      let _this = this._constitute;
      let i = 0;
      return {
        next: function () {
          var done = i >= _this.memberNumber;
          var value = !done ? _this.keyValueArray[i++].entry() : undefined;
          return {
            done: done,
            value: value,
          };
        },
      };
    }
    [Symbol.iterator](): IterableIterator<T> {
      let _this = this._constitute;
      let i = 0;
      return {
        next: function () {
          var done = i >= _this.memberNumber;
          var value = !done ? _this.keyValueArray[i++].key : undefined;
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
export default {
  TreeSet: fastTreeSet,
};
