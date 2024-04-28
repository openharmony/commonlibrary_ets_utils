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
let arkPritvate: ArkPrivate = globalThis.ArkPrivate || undefined;
if (arkPritvate !== undefined) {
  fastTreeMap = arkPritvate.Load(arkPritvate.TreeMap);
} else {
  flag = true;
}
if (flag || fastTreeMap === undefined) {
  let treeAbility = requireNapi('util.struct');
  const errorUtil = treeAbility.errorUtil;
  interface IterableIterator<T> {
    next: () => {
      value: T | undefined;
      done: boolean;
    };
  }
  class HandlerTreeMap<K, V> {
    set(target: TreeMap<K, V>, p: string, value: string): boolean {
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
      errorUtil.checkNewTargetIsNullError('TreeMap', !new.target);
      if (comparator) {
        errorUtil.checkTypeError('comparator', 'callable', comparator);
      }
      this.constitute = new treeAbility.TreeClass(comparator);
      return new Proxy(this, new HandlerTreeMap());
    }
    get length(): number {
      return this.constitute.memberNumber;
    }
    isEmpty(): boolean {
      errorUtil.checkBindError('isEmpty', TreeMap, this);
      return this.constitute.memberNumber === 0;
    }
    hasKey(key: K): boolean {
      errorUtil.checkBindError('hasKey', TreeMap, this);
      return this.constitute.getNode(key) !== undefined;
    }
    hasValue(value: V): boolean {
      errorUtil.checkBindError('hasValue', TreeMap, this);
      return this.constitute.findNode(value) !== undefined;
    }
    get(key: K): V {
      errorUtil.checkBindError('get', TreeMap, this);
      if (this.constitute.getNode(key) === undefined) {
        return undefined;
      }
      return this.constitute.getNode(key).value;
    }
    getFirstKey(): K {
      errorUtil.checkBindError('getFirstKey', TreeMap, this);
      if (this.constitute.firstNode() === undefined) {
        return undefined;
      }
      return this.constitute.firstNode().key;
    }
    getLastKey(): K {
      errorUtil.checkBindError('getLastKey', TreeMap, this);
      if (this.constitute.lastNode() === undefined) {
        return undefined;
      }
      return this.constitute.lastNode().key;
    }
    setAll(map: TreeMap<K, V>): void {
      errorUtil.checkBindError('setAll', TreeMap, this);
      errorUtil.checkTypeError('map', 'TreeMap', map);
      this.constitute.setAll(map.constitute);
    }
    set(key: K, value: V): Object {
      errorUtil.checkBindError('set', TreeMap, this);
      return this.constitute.addNode(key, value);
    }
    remove(key: K): V {
      errorUtil.checkBindError('remove', TreeMap, this);
      return this.constitute.removeNode(key);
    }
    clear(): void {
      errorUtil.checkBindError('clear', TreeMap, this);
      this.constitute.clearTree();
    }
    getLowerKey(key: K): K {
      errorUtil.checkBindError('getLowerKey', TreeMap, this);
      let result: K | undefined = undefined;
      if (this.constitute.getNode(key) === undefined) {
        return undefined;
      }
      if (this.constitute.getNode(key).left !== undefined) {
        return  this.constitute.getNode(key).left.key;
      }
      let node =  this.constitute.getNode(key);
      while (node.parent !== undefined) {
        if (node.parent.right === node) {
          return node.parent.key;
        }
        node = node.parent;
      }
      return result;
    }
    getHigherKey(key: K): K {
      errorUtil.checkBindError('getHigherKey', TreeMap, this);
      let result: K | undefined = undefined;
      if (this.constitute.getNode(key) === undefined) {
        return undefined;
      }
      if (this.constitute.getNode(key).right !== undefined) {
        return this.constitute.getNode(key).right.key;
      }
      let node = this.constitute.getNode(key);
      while (node.parent !== undefined) {
        if (node.parent.left === node) {
          return node.parent.key;
        }
        node = node.parent;
      }
      return result;
    }
    keys(): IterableIterator<K> {
      errorUtil.checkBindError('keys', TreeMap, this);
      let count: number = 0;
      return {
        next: function (): { done: boolean, value: K } {
          let done: boolean = false;
          let value: K = undefined;
          done = count >= this.constitute.memberNumber;
          value = done ? undefined : this.constitute.keyValueArray[count].key;
          count++;
          return {
            done: done,
            value: value,
          };
        },
      };
    }
    values(): IterableIterator<V> {
      errorUtil.checkBindError('values', TreeMap, this);
      let count: number = 0;
      return {
        next: function (): { done: boolean, value: V } {
          let done: boolean = false;
          let value: V = undefined;
          done = count >= this.constitute.memberNumber;
          value = done ? undefined : this.constitute.keyValueArray[count].value;
          count++;
          return {
            done: done,
            value: value,
          };
        },
      };
    }
    replace(key: K, newValue: V): boolean {
      errorUtil.checkBindError('replace', TreeMap, this);
      if (this.constitute.getNode(key) === undefined) {
        return false;
      }
      this.constitute.getNode(key).value = newValue;
      return true;
    }
    forEach(callbackfn: (value?: V, key?: K, map?: TreeMap<K, V>) => void,
      thisArg?: Object): void {
      errorUtil.checkBindError('forEach', TreeMap, this);
      errorUtil.checkTypeError('callbackfn', 'callable', callbackfn);
      for (let i: number = 0; i < this.constitute.memberNumber; i++) {
        callbackfn.call(thisArg, this.constitute.keyValueArray[i].value as V, this.constitute.keyValueArray[i].key);
      }
    }
    entries(): IterableIterator<[K, V]> {
      errorUtil.checkBindError('entries', TreeMap, this);
      let count: number = 0;
      return {
        next: function (): { done: boolean, value: [K, V] } {
          let done: boolean = false;
          let value: [K, V] = undefined;
          done = count >= this.constitute.memberNumber;
          value = done ? undefined : this.constitute.keyValueArray[count].entry();
          count++;
          return {
            done: done,
            value: value,
          };
        },
      };
    }
    [Symbol.iterator](): IterableIterator<[K, V]> {
      errorUtil.checkBindError('Symbol.iterator', TreeMap, this);
      return this.entries();
    }
  }
  Object.freeze(TreeMap);
  fastTreeMap = TreeMap;
}
export default fastTreeMap;
