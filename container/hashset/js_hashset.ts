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
let fastHashSet = undefined;
let arkPritvate = globalThis["ArkPrivate"] || undefined;
if (arkPritvate !== undefined) {
  fastHashSet = arkPritvate.Load(arkPritvate.HashSet);
} else {
  flag = true;
}

if (flag || fastHashSet === undefined) {
  const HashSetAbility = requireNapi("util.struct");
  interface IterableIterator<T> {
    next: () => {
      value: T | undefined;
      done: boolean;
    };
  }
  class HandlerHashSet<T> {
    set(target: HashSet<T>, p: any, value: any): boolean {
      if (p in target) {
        target[p] = value;
        return true;
      }
      return false;
    }
    defineProperty(target: HashSet<T>, p: any): boolean {
      throw new Error("Can't define Property on HashSet Object");
    }
    deleteProperty(target: HashSet<T>, p: any): boolean {
      throw new Error("Can't delete Property on HashSet Object");
    }
    setPrototypeOf(target: HashSet<T>, p: any): boolean {
      throw new Error("Can't set Prototype on HashSet Object");
    }
  }
  class HashSet<T> extends HashSetAbility.DictionaryClass<T, T> {
    constructor() {
      super();
      return new Proxy(this, new HandlerHashSet());
    }
    get length() {
      return this.memberNumber;
    }
    isEmpty(): boolean {
      return this.memberNumber === 0;
    }
    has(value: T): boolean {
      return this.hasKey(value);
    }
    add(value: T): boolean {
      if (this.has(value)) return false;
      return this.put(value);
    }
    remove(value: T): boolean {
      if (this.removeMember(value) !== undefined) return true;
      return false;
    }
    clear(): void {
      super.clear();
    }
    forEach(callbackfn: (value?: T, key?: T, set?: HashSet<T>) => void,
      thisArg?: Object): void {
      let tagetArray = this.keyValueArray;
      for (let i = 0; i < tagetArray.length; i++) {
        callbackfn.call(thisArg, tagetArray[i].key, tagetArray[i].key, this);
      }
    }
    values(): IterableIterator<T> {
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
    entries(): IterableIterator<[T, T]> {
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
    [Symbol.iterator](): IterableIterator<T> {
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
  }
  Object.freeze(HashSet);
  fastHashSet = HashSet;
}
export default fastHashSet;