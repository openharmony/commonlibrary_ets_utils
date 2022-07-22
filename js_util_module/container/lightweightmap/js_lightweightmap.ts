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
  LightWeightMap: number;
  Load(key: number): Object;
}
let flag: boolean = false;
let fastLightWeightMap: Object = undefined;
let arkPritvate: ArkPrivate = globalThis['ArkPrivate'] || undefined;
if (arkPritvate !== undefined) {
  fastLightWeightMap = arkPritvate.Load(arkPritvate.LightWeightMap);
} else {
  flag = true;
}
if (flag || fastLightWeightMap === undefined) {
  const LightWeightAbility = requireNapi('util.struct');
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
    defineProperty(): boolean {
      throw new Error(`Can't define Property on LightWeightMap Object`);
    }
    deleteProperty(): boolean {
      throw new Error(`Can't delete Property on LightWeightMap Object`);
    }
    setPrototypeOf(): boolean {
      throw new Error(`Can't set Prototype on LightWeightMap Object`);
    }
  }
  class LightWeightMap<K, V> extends LightWeightAbility.LightWeightClass<K, V> {
    constructor() {
      super();
      return new Proxy(this, new HandlerLightWeightMap());
    }
    get length(): number {
      return this.memberNumber;
    }
    hasAll(map: LightWeightMap<K, V>): boolean {
      if (!(map instanceof LightWeightMap)) {
        throw new TypeError('map is not JSAPILightWeightMap');
      }
      if (map.memberNumber > this.memberNumber) {
        return false;
      }
      if (LightWeightAbility.isIncludeToArray(this.keyValueStringArray(), map.keyValueStringArray())) {
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
      if (typeof minimumCapacity !== 'number') {
        throw new TypeError('the size is not integer');
      }
      super.ensureCapacity(minimumCapacity);
    }
    entries(): IterableIterator<[K, V]> {
      let data: LightWeightMap<K, V> = this;
      let count: number = 0;
      return {
        next: function () {
          let done: boolean = false;
          let value: [K, V] = undefined;
          done = count >= data.memberNumber;
          value = done ? undefined : [data.members.keys[count], data.members.values[count]] as [K, V];
          count++;
          return {
            done: done,
            value: value,
          };
        },
      };
    }
    get(key: K): V {
      let index: number = 0;
      index = this.getIndexByKey(key);
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
      if (typeof index !== 'number') {
        throw new TypeError('the index is not integer');
      }
      return this.members.keys[index];
    }
    keys(): IterableIterator<K> {
      let data: LightWeightMap<K, V> = this;
      let count: number = 0;
      return {
        next: function () {
          let done: boolean = false;
          let value: K = undefined;
          done = count >= data.memberNumber;
          value = done ? undefined : data.members.keys[count];
          count++;
          return {
            done: done,
            value: value,
          };
        },
      };
    }
    setAll(map: LightWeightMap<K, V>): void {
      if (!(map instanceof LightWeightMap)) {
        throw new TypeError('Incoming object is not JSAPILightWeightMap');
      }
      if (this.memberNumber === 0) {
        this.members.hashs = map.members.hashs.slice();
        this.members.keys = map.members.keys.slice();
        this.members.values = map.members.values.slice();
        this.memberNumber = map.memberNumber;
      } else {
        for (let i: number = 0; i < map.memberNumber; i++) {
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
      if (index > this.memberNumber--) {
        return false;
      }
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
      if (index > this.memberNumber || this.members.values[index] === undefined) {
        return false;
      }
      this.members.values[index] = newValue;
      return true;
    }
    forEach(callbackfn: (value?: V, key?: K, map?: LightWeightMap<K, V>) => void,
      thisArg?: Object): void {
      let data: LightWeightMap<K, V> = this;
      for (let i: number = 0; i < data.memberNumber; i++) {
        callbackfn.call(thisArg, data.members.values[i], data.members.keys[i], data);
      }
    }
    [Symbol.iterator](): IterableIterator<[K, V]> {
      return this.entries();
    }
    toString(): string {
      let result: string[] = [];
      for (let i: number = 0; i < this.memberNumber; i++) {
        result.push(this.members.keys[i] + ':' + this.members.values[i]);
      }
      return result.join(',');
    }
    getValueAt(index: number): V {
      return this.members.values[index];
    }
    values(): IterableIterator<V> {
      let data: LightWeightMap<K, V> = this;
      let count: number = 0;
      return {
        next: function () {
          let done: boolean = false;
          let value: V = undefined;
          done = count >= data.memberNumber;
          value = done ? undefined : data.members.values[count];
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
