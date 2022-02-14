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
let fastLightWeightSet = undefined;
let arkPritvate = globalThis["ArkPrivate"] || undefined;
if (arkPritvate !== undefined) {
  fastLightWeightSet = arkPritvate.Load(arkPritvate.LightWeightSet);
} else {
  flag = true;
}

if (flag || fastLightWeightSet === undefined) {
  const LightWeightAbility = requireNapi("util.struct");
  interface IterableIterator<T> {
    next: () => {
      value: T | undefined;
      done: boolean;
    };
  }
  class HandlerLightWeightSet<T> {
    set(target: LightWeightSet<T>, p: any, value: any): boolean {
      if (p in target) {
        target[p] = value;
        return true;
      }
      return false;
    }
    defineProperty(target: LightWeightSet<T>, p: any): boolean {
      throw new Error("Can't define Property on LightWeightSet Object");
    }
    deleteProperty(target: LightWeightSet<T>, p: any): boolean {
      throw new Error("Can't delete Property on LightWeightSet Object");
    }
    setPrototypeOf(target: LightWeightSet<T>, p: any): boolean {
      throw new Error("Can't set Prototype on LightWeightSet Object");
    }
  }
  class LightWeightSet<T> extends LightWeightAbility.LightWeightClass<T, T> {
    constructor() {
      super();
      return new Proxy(this, new HandlerLightWeightSet());
    }
    get length() {
      return this.memberNumber;
    }
    add(obj: T): boolean {
      if (this.members.keys.indexOf(obj) > 0) return false;
      this.addmember(obj);
      return true;
    }
    addAll(set: LightWeightSet<T>): boolean {
      let change = false;
      if (set.memberNumber == 0) {
        change = false;
      } else {
        for (let i = 0; i < set.memberNumber; i++) {
          change = this.add(set.members.keys[i]) || change;
        }
      } 
      return change;
    }
    hasAll(set: LightWeightSet<T>): boolean {
      if (set.memberNumber > this.memberNumber) return false;
      if (LightWeightAbility.isIncludeToArray(this.members.keys, set.members.keys)) {
        return true;
      }
      return false;
    }
    has(key: T): boolean {
      return this.members.keys.indexOf(key) > -1;
    }
    equal(obj: Object): boolean {
      if (this.memberNumber === 0) return false;
      if (JSON.stringify(obj) === JSON.stringify(this.members.keys)) return true;
      return false;
    }
    increaseCapacityTo(minimumCapacity: number): void {
      super.ensureCapacity(minimumCapacity);
    }
    getIndexOf(key: T): number {
      return super.getIndexByKey(key);
    }
    isEmpty(): boolean {
      return this.memberNumber === 0;
    }
    remove(key: T): T {
      return super.deletemember(key);
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
    forEach(callbackfn: (value?: T, key?: T, set?: LightWeightSet<T>) => void,
      thisArg?: Object): void {
      let data = this;
      for (let i = 0; i < data.memberNumber; i++) {
        callbackfn.call(thisArg, data.members.keys[i], data.members.keys[i], data);
      }
    }
    [Symbol.iterator](): IterableIterator<T> {
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
    toString(): string {
      return this.members.keys.join(",");
    }
    toArray(): Array<T> {
      return this.members.keys.slice();
    }
    getValueAt(index: number): T {
      return this.members.keys[index];
    }
    values(): IterableIterator<T> {
      return this.members.keys.values() as IterableIterator<T>;
    }
    entries(): IterableIterator<[T, T]> {
      let data = this;
      let count = 0;
      return {
        next: function () {
          var done = count >= data.memberNumber;
          var tempValue = data.members.keys[count];
          var value = !done ? ([tempValue, tempValue] as [T, T]) : undefined;
          count++;
          return {
            done: done,
            value: value,
          };
        },
      };
    }
  }
  Object.freeze(LightWeightSet);
  fastLightWeightSet = LightWeightSet;
}
export default fastLightWeightSet;
