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
  const lightWeightAbility = requireNapi('util.struct');
  const errorUtil = lightWeightAbility.errorUtil;
  interface IterableIterator<T> {
    next: () => {
      value: T | undefined;
      done: boolean;
    };
  }
  class HandlerLightWeightMap<K, V> {
    set(target: LightWeightMap<K, V>, p: string, value: string): boolean {
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
  class LightWeightMap<K, V> extends lightWeightAbility.LightWeightClass<K, V> {
    constructor() {
      errorUtil.checkNewTargetIsNullError('LightWeightMap', !new.target);
      super();
      return new Proxy(this, new HandlerLightWeightMap());
    }
    get length(): number {
      return this.memberNumber;
    }
    hasAll(map: LightWeightMap<K, V>): boolean {
      errorUtil.checkBindError('hasAll', LightWeightMap, this);
      errorUtil.checkTypeError('map', 'LightWeightMap', map);
      if (!(map instanceof LightWeightMap)) {
        throw new TypeError('map is not JSAPILightWeightMap');
      }
      if (map.memberNumber > this.memberNumber) {
        return false;
      }
      if (lightWeightAbility.isIncludeToArray(this.keyValueStringArray(), map.keyValueStringArray())) {
        return true;
      }
      return false;
    }
    hasKey(key: K): boolean {
      errorUtil.checkBindError('hasKey', LightWeightMap, this);
      return this.members.keys.indexOf(key) > -1;
    }
    hasValue(value: V): boolean {
      errorUtil.checkBindError('hasValue', LightWeightMap, this);
      return this.members.values.indexOf(value) > -1;
    }
    increaseCapacityTo(minimumCapacity: number): void {
      errorUtil.checkBindError('increaseCapacityTo', LightWeightMap, this);
      errorUtil.checkTypeError('minimumCapacity', 'Integer', minimumCapacity);
      super.ensureCapacity(minimumCapacity);
    }
    entries(): IterableIterator<[K, V]> {
      errorUtil.checkBindError('entries', LightWeightMap, this);
      let data: LightWeightMap<K, V> = this;
      let count: number = 0;
      return {
        next: function (): { done: boolean, value: [K, V] } {
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
      errorUtil.checkBindError('get', LightWeightMap, this);
      let index: number = 0;
      index = this.getIndexByKey(key);
      return this.members.values[index];
    }
    getIndexOfKey(key: K): number {
      errorUtil.checkBindError('getIndexOfKey', LightWeightMap, this);
      return this.getIndexByKey(key);
    }
    getIndexOfValue(value: V): number {
      errorUtil.checkBindError('getIndexOfValue', LightWeightMap, this);
      return this.members.values.indexOf(value);
    }
    isEmpty(): boolean {
      errorUtil.checkBindError('isEmpty', LightWeightMap, this);
      return this.memberNumber === 0;
    }
    getKeyAt(index: number): K {
      errorUtil.checkBindError('getKeyAt', LightWeightMap, this);
      errorUtil.checkTypeError('index', 'Integer', index);
      errorUtil.checkRangeError('index', index, 0, this.length - 1);
      return this.members.keys[index];
    }
    keys(): IterableIterator<K> {
      errorUtil.checkBindError('keys', LightWeightMap, this);
      let data: LightWeightMap<K, V> = this;
      let count: number = 0;
      return {
        next: function (): { done: boolean, value: K } {
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
      errorUtil.checkBindError('setAll', LightWeightMap, this);
      errorUtil.checkTypeError('map', 'LightWeightMap', map);
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
      errorUtil.checkBindError('set', LightWeightMap, this);
      this.addmember(key, value);
      return this;
    }
    remove(key: K): V {
      errorUtil.checkBindError('remove', LightWeightMap, this);
      return this.deletemember(key);
    }
    removeAt(index: number): boolean {
      errorUtil.checkBindError('removeAt', LightWeightMap, this);
      errorUtil.checkTypeError('index', 'Integer', index);
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
      errorUtil.checkBindError('clear', LightWeightMap, this);
      if (this.memberNumber !== 0 || this.capacity > 8) { // 8 : means number
        this.members.hashs = [];
        this.members.keys = [];
        this.members.values = [];
        this.memberNumber = 0;
        this.capacity = 8;
      }
    }
    setValueAt(index: number, newValue: V): boolean {
      errorUtil.checkBindError('setValueAt', LightWeightMap, this);
      errorUtil.checkTypeError('index', 'Integer', index);
      errorUtil.checkRangeError('index', index, 0, this.length - 1);
      if (index > this.memberNumber || this.members.values[index] === undefined) {
        return false;
      }
      this.members.values[index] = newValue;
      return true;
    }
    forEach(callbackfn: (value?: V, key?: K, map?: LightWeightMap<K, V>) => void,
      thisArg?: Object): void {
      errorUtil.checkBindError('forEach', LightWeightMap, this);
      errorUtil.checkTypeError('callbackfn', 'callable', callbackfn);
      let data: LightWeightMap<K, V> = this;
      for (let i: number = 0; i < data.memberNumber; i++) {
        callbackfn.call(thisArg, data.members.values[i], data.members.keys[i], data);
      }
    }
    [Symbol.iterator](): IterableIterator<[K, V]> {
      errorUtil.checkBindError('Symbol.iterator', LightWeightMap, this);
      return this.entries();
    }
    toString(): string {
      errorUtil.checkBindError('toString', LightWeightMap, this);
      let result: string[] = [];
      for (let i: number = 0; i < this.memberNumber; i++) {
        result.push(this.members.keys[i] + ':' + this.members.values[i]);
      }
      return result.join(',');
    }
    getValueAt(index: number): V {
      errorUtil.checkBindError('getValueAt', LightWeightMap, this);
      errorUtil.checkTypeError('index', 'Integer', index);
      errorUtil.checkRangeError('index', index, 0, this.length - 1);
      return this.members.values[index];
    }
    values(): IterableIterator<V> {
      errorUtil.checkBindError('values', LightWeightMap, this);
      let data: LightWeightMap<K, V> = this;
      let count: number = 0;
      return {
        next: function (): { done: boolean, value: V } {
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
