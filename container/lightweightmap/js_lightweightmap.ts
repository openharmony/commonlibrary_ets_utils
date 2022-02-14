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
let fastLightWeightMap = undefined;
let arkPritvate = globalThis["ArkPrivate"] || undefined;
if (arkPritvate !== undefined) {
  fastLightWeightMap = arkPritvate.Load(arkPritvate.LightWeightMap);
} else {
  flag = true;
}

if (flag || fastLightWeightMap === undefined) {
  const LightWeightAbility = requireNapi("util.struct");
  interface IterableIterator<T> {
    next: () => {
      value: T | undefined;
      done: boolean;
    };
  }
  class HandlerLightWeightMap<K, V> {
    set(target: LightWeightMap<K, V>, p: any, value: any): boolean {
      if (p in target) {
        target[p] = value;
        return true;
      }
      return false;
    }
    defineProperty(target: LightWeightMap<K, V>, p: any): boolean {
      throw new Error("Can't define Property on LightWeightMap Object");
    }
    deleteProperty(target: LightWeightMap<K, V>, p: any): boolean {
      throw new Error("Can't delete Property on LightWeightMap Object");
    }
    setPrototypeOf(target: LightWeightMap<K, V>, p: any): boolean {
      throw new Error("Can't set Prototype on LightWeightMap Object");
    }
  }
  class LightWeightMap<K, V> extends LightWeightAbility.LightWeightClass<K, V> {
    constructor() {
      super();
      return new Proxy(this, new HandlerLightWeightMap());
    }
    get length() {
      return this.memberNumber;
    }
    hasAll(map: LightWeightMap<K, V>): boolean {
      if (map.memberNumber > this.memberNumber) return false;
      if (LightWeightAbility.isIncludeToArray(this.keyValueStringArray(), map.keyValueStringArray()) ) {
        return true;
      }
      return false;
    }
    hasKey(key: K): boolean {
      return this.members.keys.indexOf(key) > -1;
    }
    hasValue(value: V): boolean {
      return this.members.values.indexOf(value) > -1;
    }
    increaseCapacityTo(minimumCapacity: number): void {
      super.ensureCapacity(minimumCapacity);
    }
    entries(): IterableIterator<[K, V]> {
      let data = this;
      let count = 0;
      return {
        next: function () {
          var done = count >= data.memberNumber;
          var value = !done ? [data.members.keys[count], data.members.values[count]] as [K, V] : undefined;
          count++;
          return {
            done: done,
            value: value,
          };
        },
      };
    }
    get(key: K): V {
      let index = this.getIndexByKey(key);
      return this.members.values[index];
    }
    getIndexOfKey(key: K): number {
      return this.getIndexByKey(key);
    }
    getIndexOfValue(value: V): number {
      return this.members.values.indexOf(value);
    }
    isEmpty(): boolean {
      return this.memberNumber === 0;
    }
    getKeyAt(index: number): K {
      return this.members.keys[index];
    }
    keys(): IterableIterator<K> {
      let data = this;
      let count = 0;
      return {
        next: function () {
          var done = count >= data.memberNumber;
          var value = !done ? data.members.keys[count] : undefined;
          count++;
          return {
            done: done,
            value: value,
          };
        },
      };
    }
    setAll(map: LightWeightMap<K, V>): void {
      if (this.memberNumber === 0) {
        this.members.hashs = map.members.hashs.slice();
        this.members.keys = map.members.keys.slice();
        this.members.values = map.members.values.slice();
        this.memberNumber = map.memberNumber;
      } else {
        for (let i = 0; i < map.memberNumber; i++) {
          this.addmember(map.members.keys[i], map.members.values[i]);
        }
      }
    }
    set(key: K, value: V): Object {
      this.addmember(key, value);
      return this;
    }
    remove(key: K): V {
      return this.deletemember(key);
    }
    removeAt(index: number): boolean {
      if (index > this.memberNumber--) return false;
      this.members.hashs.splice(index, 1);
      this.members.values.splice(index, 1);
      this.members.keys.splice(index, 1);
      this.memberNumber--;
      return true;
    }
    clear(): void {
      if (this.memberNumber != 0 || this.capacity > 8) {
        this.members.hashs = [];
        this.members.keys = [];
        this.members.values = [];
        this.memberNumber = 0;
        this.capacity = 8;
      }
    }
    setValueAt(index: number, newValue: V): boolean {
      if (index > this.memberNumber || this.members.values[index] === undefined) return false;
      this.members.values[index] = newValue;
      return true;
    }
    forEach(callbackfn: (value?: V, key?: K, map?: LightWeightMap<K, V>) => void,
      thisArg?: Object): void {
      let data = this;
      for (let i = 0; i < data.memberNumber; i++) {
        callbackfn.call(thisArg, data.members.values[i], data.members.keys[i], data);
      }
    }
    [Symbol.iterator](): IterableIterator<[K, V]> {
      let data = this;
      let count = 0;
      return {
        next: function () {
          var done = count >= data.memberNumber;
          var value = !done ? [data.members.keys[count], data.members.values[count]] as [K, V] : undefined;
          count++;
          return {
            done: done,
            value: value,
          };
        },
      };
    }
    toString(): string {
      let result = new Array<string>();
      for (let i = 0; i < this.memberNumber; i++) {
        result.push(this.members.keys[i] + ":" + this.members.values[i]);
      }
      return result.join(",");
    }
    getValueAt(index: number): V {
      return this.members.values[index];
    }
    values(): IterableIterator<V> {
      let data = this;
      let count = 0;
      return {
        next: function () {
          var done = count >= data.memberNumber;
          var value = !done ? data.members.values[count] : undefined;
          count++;
          return {
            done: done,
            value: value,
          };
        },
      };
    }
  }
  Object.freeze(LightWeightMap);
  fastLightWeightMap = LightWeightMap;
}
export default fastLightWeightMap;
