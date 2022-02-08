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
let fastHashMap = undefined;
let arkPritvate = globalThis["ArkPrivate"] || undefined;
if (arkPritvate !== undefined) {
  fastHashMap = arkPritvate.Load(arkPritvate.HashMap);
} else {
  flag = true;
}

if (flag || fastHashMap === undefined) {
  const HashMapAbility = requireNapi("util.struct");
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
    defineProperty(target: HashMap<K, V>, p: any): boolean {
      throw new Error("Can't define Property on HashMap Object");
    }
    deleteProperty(target: HashMap<K, V>, p: any): boolean {
      throw new Error("Can't delete Property on HashMap Object");
    }
    setPrototypeOf(target: HashMap<K, V>, p: any): boolean {
      throw new Error("Can't set Prototype on HashMap Object");
    }
  }
  class HashMap<K, V> extends HashMapAbility.DictionaryClass<K, V> {
    constructor() {
      super();
      return new Proxy(this, new HandlerHashMap());
    }
    get length() {
      return this.memberNumber;
    }
    isEmpty(): boolean {
      return this.memberNumber === 0;
    }
    hasKey(key: K): boolean {
      return super.hasKey(key);
    }
    hasValue(value: V): boolean {
      return super.Values().indexOf(value) > -1;
    }
    get(key: K): V {
      return this.getValueByKey(key);
    }
    setAll(map: HashMap<K, V>): void {
      let memebers = map.keyValueArray;
      for (let i = 0; i < memebers.length; i++) {
        this.put(memebers[i].key, memebers[i].value);
      }
    }
    set(key: K, value: V): Object {
      return super.put(key, value);
    }
    remove(key: K): V {
      let result = this.removeMember(key);
      if (result === undefined) {
        throw new Error("The removed element does not exist in this container");
      }
      return result;
    }
    clear(): void {
      super.clear();
    }
    keys(): IterableIterator<K> {
      let data = this;
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
      let data = this;
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
      return super.replaceMember(key, newValue);
    }
    forEach(callbackfn: (value?: V, key?: K, map?: HashMap<K, V>) => void,
      thisArg?: Object): void {
      let tagetArray = this.keyValueArray;
      for (let i = 0; i < tagetArray.length; i++) {
        callbackfn.call(thisArg, tagetArray[i].value, tagetArray[i].key, this);
      }
    }
    entries(): IterableIterator<[K, V]> {
      let data = this;
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
      let data = this;
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
  Object.freeze(HashMap);
  fastHashMap = HashMap;
}

export default {
  HashMap: fastHashMap,
};
