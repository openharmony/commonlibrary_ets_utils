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
  TreeMap: number;
  Load(key: number): Object;
}
let flag: boolean = false;
let fastTreeMap: Object = undefined;
let arkPritvate: ArkPrivate = globalThis['ArkPrivate'] || undefined;
if (arkPritvate !== undefined) {
  fastTreeMap = arkPritvate.Load(arkPritvate.TreeMap);
} else {
  flag = true;
}
if (flag || fastTreeMap === undefined) {
  let RBTreeAbility = requireNapi('util.struct');
  const ErrorUtil = RBTreeAbility.ErrorUtil;
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
    defineProperty(): boolean {
      throw new Error(`Can't define Property on TreeMap Object`);
    }
    deleteProperty(): boolean {
      throw new Error(`Can't delete Property on TreeMap Object`);
    }
    setPrototypeOf(): boolean {
      throw new Error(`Can't set Prototype on TreeMap Object`);
    }
  }
  class TreeMap<K, V> {
    private constitute: any;
    constructor(comparator?: (firstValue: K, secondValue: K) => boolean) {
      ErrorUtil.checkNewTargetIsNullError("TreeMap", !new.target);
      if (comparator) {
        ErrorUtil.checkTypeError("comparator", "callable", comparator);
      }
      this.constitute = new RBTreeAbility.RBTreeClass(comparator);
      return new Proxy(this, new HandlerTreeMap());
    }
    get length(): number {
      return this.constitute.memberNumber;
    }
    isEmpty(): boolean {
      ErrorUtil.checkBindError("isEmpty", TreeMap, this);
      return this.constitute.memberNumber === 0;
    }
    hasKey(key: K): boolean {
      ErrorUtil.checkBindError("hasKey", TreeMap, this);
      return this.constitute.getNode(key) !== undefined;
    }
    hasValue(value: V): boolean {
      ErrorUtil.checkBindError("hasValue", TreeMap, this);
      return this.constitute.findNode(value) !== undefined;
    }
    get(key: K): V {
      ErrorUtil.checkBindError("get", TreeMap, this);
      let tempNode: any = undefined;
      tempNode = this.constitute.getNode(key);
      if (tempNode === undefined) {
        return tempNode;
      }
      return tempNode.value;
    }
    getFirstKey(): K {
      ErrorUtil.checkBindError("getFirstKey", TreeMap, this);
      let tempNode: any = undefined;
      tempNode = this.constitute.firstNode();
      if (tempNode === undefined) {
        return tempNode;
      }
      return tempNode.key;
    }
    getLastKey(): K {
      ErrorUtil.checkBindError("getLastKey", TreeMap, this);
      let tempNode: any = undefined;
      tempNode = this.constitute.lastNode();
      if (tempNode === undefined)
        return tempNode;
      return tempNode.key;
    }
    setAll(map: TreeMap<K, V>) {
      ErrorUtil.checkBindError("setAll", TreeMap, this);
      ErrorUtil.checkTypeError("map", "TreeMap", map);
      this.constitute.setAll(map.constitute);
    }
    set(key: K, value: V): Object {
      ErrorUtil.checkBindError("set", TreeMap, this);
      return this.constitute.addNode(key, value);
    }
    remove(key: K): V {
      ErrorUtil.checkBindError("remove", TreeMap, this);
      return this.constitute.removeNode(key);
    }
    clear() {
      ErrorUtil.checkBindError("clear", TreeMap, this);
      this.constitute.clearTree();
    }
    getLowerKey(key: K): K {
      ErrorUtil.checkBindError("getLowerKey", TreeMap, this);
      let result: K | undefined = undefined;
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
        node = node.parent;
      }
      return result;
    }
    getHigherKey(key: K): K {
      ErrorUtil.checkBindError("getHigherKey", TreeMap, this);
      let result: K | undefined = undefined;
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
        node = node.parent;
      }
      return result;
    }
    keys(): IterableIterator<K> {
      ErrorUtil.checkBindError("keys", TreeMap, this);
      let data: any = this.constitute;
      let count: number = 0;
      return {
        next: function () {
          let done: boolean = false;
          let value: K = undefined;
          done = count >= data.memberNumber;
          value = done ? undefined : data.keyValueArray[count].key;
          count++;
          return {
            done: done,
            value: value,
          };
        },
      };
    }
    values(): IterableIterator<V> {
      ErrorUtil.checkBindError("values", TreeMap, this);
      let data: any = this.constitute;
      let count: number = 0;
      return {
        next: function () {
          let done: boolean = false;
          let value: V = undefined;
          done = count >= data.memberNumber;
          value = done ? undefined : data.keyValueArray[count].value;
          count++;
          return {
            done: done,
            value: value,
          };
        },
      };
    }
    replace(key: K, newValue: V): boolean {
      ErrorUtil.checkBindError("replace", TreeMap, this);
      let targetNode: any = this.constitute.getNode(key);
      if (targetNode === undefined) {
        return false;
      }
      targetNode.value = newValue;
      return true;
    }
    forEach(callbackfn: (value?: V, key?: K, map?: TreeMap<K, V>) => void,
      thisArg?: Object): void {
      ErrorUtil.checkBindError("forEach", TreeMap, this);
      ErrorUtil.checkTypeError("callbackfn", "callable", callbackfn);
      let data: any = this.constitute;
      let tagetArray: Array<any> = [];
      tagetArray = data.keyValueArray;
      for (let i: number = 0; i < data.memberNumber; i++) {
        callbackfn.call(thisArg, tagetArray[i].value as V, tagetArray[i].key);
      }
    }
    entries(): IterableIterator<[K, V]> {
      ErrorUtil.checkBindError("entries", TreeMap, this);
      let data: any = this.constitute;
      let count: number = 0;
      return {
        next: function () {
          let done: boolean = false;
          let value: [K, V] = undefined;
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
    [Symbol.iterator](): IterableIterator<[K, V]> {
      ErrorUtil.checkBindError("Symbol.iterator", TreeMap, this);
      return this.entries();
    }
  }
  Object.freeze(TreeMap);
  fastTreeMap = TreeMap;
}
export default fastTreeMap;
