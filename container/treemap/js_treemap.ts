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
  const RBTreeAbility = requireNapi("util.struct")
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
      throw new Error("Can't define Property on TreeMap Object");
    }
    deleteProperty(target: TreeMap<K, V>, p: any): boolean {
      throw new Error("Can't delete Property on TreeMap Object");
    }
    setPrototypeOf(target: TreeMap<K, V>, p: any): boolean {
      throw new Error("Can't set Prototype on TreeMap Object");
    }
  }
  class TreeMap<K, V> {
    private constitute: any;
    constructor(comparator?: (firstValue: K, secondValue: K) => boolean) {
      this.constitute = new RBTreeAbility.RBTreeClass(comparator);
      return new Proxy(this, new HandlerTreeMap());
    }
    get length() {
      return this.constitute.memberNumber;
    }
    isEmpty() {
      return this.constitute.memberNumber === 0;
    }
    hasKey(key: K): boolean {
      return this.constitute.getNode(key) !== undefined;
    }
    hasValue(value: V): boolean {
      return this.constitute.findNode(value) !== undefined;
    }
    get(key: K): V {
      let tempNode = this.constitute.getNode(key);
      if (tempNode === undefined)
        throw new Error("The node of this key does not exist in the tree");
      return tempNode.value;
    }
    getFirstKey(): K {
      let tempNode = this.constitute.firstNode();
      if (tempNode === undefined)
        throw new Error("don't find this key,this tree is empty");
      return tempNode.key;
    }
    getLastKey(): K {
      let tempNode = this.constitute.lastNode();
      if (tempNode === undefined)
        throw new Error("don't find this key,this tree is empty");
      return tempNode.key;
    }
    setAll(map: TreeMap<K, V>) {
      this.constitute.setAll(map.constitute);
    }
    set(key: K, value: V): Object {
      return this.constitute.addNode(key, value);
    }
    remove(key: K): V {
      return this.constitute.removeNode(key);
    }
    clear() {
      this.constitute.clearTree();
    }
    getLowerKey(key: K): K {
      let tempNode = this.constitute.getNode(key);
      if (tempNode === undefined)
        throw new Error("don't find this key,this node is undefine");
      if (tempNode.left !== undefined) return tempNode.left.key;
      let node = tempNode;
      while (node.parent !== undefined) {
        if (node.parent.right === node) return node.parent.key;
        node = node.parent;
      }
      throw new Error("don't find a key meet the conditions");
    }
    getHigherKey(key: K): K {
      let tempNode = this.constitute.getNode(key);
      if (tempNode === undefined)
        throw new Error("don't find this key,this node is undefine");
      if (tempNode.right !== undefined) return tempNode.right.key;
      let node = tempNode;
      while (node.parent !== undefined) {
        if (node.parent.left === node) return node.parent.key;
        node = node.parent;
      }
      throw new Error("don't find a key meet the conditions");
    }
    keys(): IterableIterator<K> {
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
    values(): IterableIterator<V> {
      let data = this.constitute;
      let count = 0;
      return {
        next: function () {
          var done = count >= data.memberNumber;
          var value = !done ? data.keyValueArray[count].value : undefined;
          count++;
          return {
            done: done,
            value: value,
          };
        },
      };
    }
    replace(key: K, newValue: V): boolean {
      let targetNode = this.constitute.getNode(key);
      if (targetNode === undefined) return false;
      targetNode.value = newValue;
      return true;
    }
    forEach(callbackfn: (value?: V, key?: K, map?: TreeMap<K, V>) => void,
      thisArg?: Object): void {
      let data = this.constitute;
      let tagetArray = data.keyValueArray;
      for (let i = 0; i < data.memberNumber; i++) {
        callbackfn.call(thisArg, tagetArray[i].value as V, tagetArray[i].key);
      }
    }
    entries(): IterableIterator<[K, V]> {
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
    [Symbol.iterator](): IterableIterator<[K, V]> {
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
  }
  Object.freeze(TreeMap);
  fastTreeMap = TreeMap;
}
export default fastTreeMap;
