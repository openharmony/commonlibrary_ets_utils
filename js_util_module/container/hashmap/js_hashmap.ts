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
  HashMap: number;
  Load(key: number): Object;
}
let flag: boolean = false;
let fastHashMap: Object = undefined;
let arkPritvate: ArkPrivate = globalThis['ArkPrivate'] || undefined;
if (arkPritvate !== undefined) {
  fastHashMap = arkPritvate.Load(arkPritvate.HashMap);
} else {
  flag = true;
}
if (flag || fastHashMap === undefined) {
  let HashMapAbility: any = requireNapi('util.struct');
  const ErrorUtil = HashMapAbility.ErrorUtil;
  interface IterableIterator<T> {
    next: () => {
      value: T | undefined;
      done: boolean;
    };
  }
  class HandlerHashMap<K, V> {
    set(target: HashMap<K, V>, p: any, value: any): boolean {
      if (p in target) {
        target[p] = value;
        return true;
      }
      return false;
    }
    defineProperty(): boolean {
      throw new Error(`Can't define Property on HashMap Object`);
    }
    deleteProperty(): boolean {
      throw new Error(`Can't delete Property on HashMap Object`);
    }
    setPrototypeOf(): boolean {
      throw new Error(`Can't set Prototype on HashMap Object`);
    }
  }
  class HashMap<K, V> extends HashMapAbility.DictionaryClass<K, V> {
    constructor() {
      ErrorUtil.checkNewTargetIsNullError("HashMap", !new.target);
      super();
      return new Proxy(this, new HandlerHashMap());
    }
    get length(): number {
      return this.memberNumber;
    }
    isEmpty(): boolean {
      ErrorUtil.checkBindError("isEmpty", HashMap, this);
      return this.memberNumber === 0;
    }
    hasKey(key: K): boolean {
      ErrorUtil.checkBindError("hasKey", HashMap, this);
      return super.hasKey(key);
    }
    hasValue(value: V): boolean {
      ErrorUtil.checkBindError("hasValue", HashMap, this);
      return super.Values().indexOf(value) > -1;
    }
    get(key: K): V {
      ErrorUtil.checkBindError("get", HashMap, this);
      return this.getValueByKey(key);
    }
    setAll(map: HashMap<K, V>): void {
      ErrorUtil.checkBindError("setAll", HashMap, this);
      ErrorUtil.checkTypeError("map", "HashMap", map);
      let memebers: Array<any> = [];
      memebers = map.keyValueArray;
      for (let i: number = 0; i < memebers.length; i++) {
        this.put(memebers[i].key, memebers[i].value);
      }
    }
    set(key: K, value: V): Object {
      ErrorUtil.checkBindError("set", HashMap, this);
      return super.put(key, value);
    }
    remove(key: K): V {
      ErrorUtil.checkBindError("remove", HashMap, this);
      let result: V = this.removeMember(key);
      return result;
    }
    clear(): void {
      ErrorUtil.checkBindError("clear", HashMap, this);
      super.clear();
    }
    keys(): IterableIterator<K> {
      ErrorUtil.checkBindError("keys", HashMap, this);
      let data: HashMap<K, V> = this;
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
      ErrorUtil.checkBindError("values", HashMap, this);
      let data: HashMap<K, V> = this;
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
      ErrorUtil.checkBindError("replace", HashMap, this);
      return super.replaceMember(key, newValue);
    }
    forEach(callbackfn: (value?: V, key?: K, map?: HashMap<K, V>) => void,
      thisArg?: Object): void {
      ErrorUtil.checkBindError("forEach", HashMap, this);
      ErrorUtil.checkTypeError("callbackfn", "callable", callbackfn);
      let tagetArray: Array<any> = [];
      tagetArray = this.keyValueArray;
      for (let i: number = 0; i < tagetArray.length; i++) {
        callbackfn.call(thisArg, tagetArray[i].value, tagetArray[i].key, this);
      }
    }
    entries(): IterableIterator<[K, V]> {
      ErrorUtil.checkBindError("entries", HashMap, this);
      let data: HashMap<K, V> = this;
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
      ErrorUtil.checkBindError("Symbol.iterator", HashMap, this);
      return this.entries();
    }
  }
  Object.freeze(HashMap);
  fastHashMap = HashMap;
}
export default fastHashMap;
