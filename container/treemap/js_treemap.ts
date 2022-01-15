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
let fastTreeMap = undefined;
let arkPritvate = globalThis["ArkPrivate"] || undefined;
if (arkPritvate !== undefined) {
  fastTreeMap = arkPritvate.Load(arkPritvate.TreeMap);
} else {
  flag = true;
}
if (flag || fastTreeMap === undefined) {
  const RBTreeAbility = requireNapi("struct")
  interface IterableIterator<T> {
    next: () => {
      value: T | undefined;
      done: boolean;
    };
  }

  class HandlerTreeMap<K, V> {
    set(target: TreeMap<K, V>, p: any, value: any): boolean {
      if (p in target) {
        target[p] = value;
        return true;
      }
      return false;
    }
    defineProperty(target: TreeMap<K, V>, p: any): boolean {
      throw new Error("Can't defineProperty on TreeMap Object");
    }
    deleteProperty(target: TreeMap<K, V>, p: any): boolean {
      throw new Error("Can't deleteProperty on TreeMap Object");
    }
    setPrototypeOf(target: TreeMap<K, V>, p: any): boolean {
      throw new Error("Can't setPrototype on TreeMap Object");
    }
  }

  class TreeMap<K, V> {
    private _constitute: any;
    constructor() {
      this._constitute = new RBTreeAbility.RBTreeClass();
      return new Proxy(this, new HandlerTreeMap());
    }

    get length() {
      return this._constitute.memberNumber;
    }
    hasKey(key: K): boolean {
      return this._constitute.getNode(key) !== null;
    }
    hasValue(value: V): boolean {
      return this._constitute.findNode(value) !== null;
    }
    get(key: K): V | null {
      let tempNode = this._constitute.getNode(key);
      if (tempNode === null)
        return null;
      return tempNode.value;
    }
    getFirstKey(): K {
      let tempNode = this._constitute.firstNode();
      if (tempNode === null)
        throw new Error("don't find this key,this tree is null");
      return tempNode.key;
    }
    getLastKey(): K {
      let tempNode = this._constitute.lastNode();
      if (tempNode === null)
        throw new Error("don't find this key,this tree is null");
      return tempNode.key;
    }
    setAll(map: TreeMap<K, V>) {
      this._constitute.setAll(map._constitute);
    }
    set(key: K, value: V): Object {
      // If the user enters key other than number and string, a comparison function is required
      return this._constitute.addNode(key, value);
    }
    remove(key: K): V | null {
      return this._constitute.removeNode(key);
    }
    clear() {
      this._constitute.clearTree();
    }
    getLowerKey(key: K): K {
      let tempNode = this._constitute.getNode(key);
      if (tempNode === null)
        throw new Error("don't find this key,this node is undefine");
      if (tempNode.left !== null) return tempNode.left.key;
      let node = tempNode;
      while (node.parent !== null) {
        if (node.parent.right === node) return node.parent.key;
        node = node.parent;
      }
      throw new Error("don't find a key meet the conditions");
    }
    getHigherKey(key: K): K {
      let tempNode = this._constitute.getNode(key);

      if (tempNode === null)
        throw new Error("don't find this key,this node is undefine");
      if (tempNode.right !== null) return tempNode.right.key;
      let node = tempNode;
      while (node.parent !== null) {
        if (node.parent.left === node) return node.parent.key;
        node = node.parent;
      }

      throw new Error("don't find a key meet the conditions");
    }
    keys(): IterableIterator<K> {
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
    values(): IterableIterator<V> {
      let _this = this._constitute;
      let i = 0;
      return {
        next: function () {
          var done = i >= _this.memberNumber;
          var value = !done ? _this.keyValueArray[i++].value as V : undefined;
          return {
            done: done,
            value: value,
          };
        },
      };
    }
    replace(key: K, newValue: V): boolean {
      let targetNode = this._constitute.getNode(key);
      if (targetNode === null) return false;
      targetNode.value = newValue;
      return true;
    }
    forEach(callbackfn: (value?: V, key?: K, map?: TreeMap<K, V>) => void,
      thisArg?: Object): void {
      let _this = this._constitute;
      let tagetArray = _this.keyValueArray;
      for (let i = 0; i < _this.memberNumber; i++) {
        callbackfn.call(thisArg, tagetArray[i].value as V, tagetArray[i].key);
      }
    }
    entries(): IterableIterator<[K, V]> {
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
    [Symbol.iterator](): IterableIterator<[K, V]> {
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
  }

  Object.freeze(TreeMap);
  fastTreeMap = TreeMap;
}
export default {
  TreeMap: fastTreeMap,
};