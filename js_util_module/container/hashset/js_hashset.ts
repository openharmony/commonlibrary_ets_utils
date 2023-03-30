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
  HashSet: number;
  Load(key: number): Object;
}
let flag: boolean = false;
let fastHashSet: Object = undefined;
let arkPritvate: ArkPrivate = globalThis['ArkPrivate'] || undefined;
if (arkPritvate !== undefined) {
  fastHashSet = arkPritvate.Load(arkPritvate.HashSet);
} else {
  flag = true;
}
if (flag || fastHashSet === undefined) {
  let hashSetAbility: any = requireNapi('util.struct');
  const errorUtil = hashSetAbility.errorUtil;
  interface IterableIterator<T> {
    next: () => {
      value: T | undefined;
      done: boolean;
    };
  }
  class HandlerHashSet<T> {
    set(target: HashSet<T>, p: string, value: string): boolean {
      if (p in target) {
        target[p] = value;
        return true;
      }
      return false;
    }
    defineProperty(): boolean {
      throw new Error(`Can't define Property on HashSet Object`);
    }
    deleteProperty(): boolean {
      throw new Error(`Can't delete Property on HashSet Object`);
    }
    setPrototypeOf(): boolean {
      throw new Error(`Can't set Prototype on HashSet Object`);
    }
  }
  class HashSet<T> extends hashSetAbility.DictionaryClass<T, T> {
    constructor() {
      errorUtil.checkNewTargetIsNullError('HashSet', !new.target);
      super();
      return new Proxy(this, new HandlerHashSet());
    }
    get length(): number {
      return this.memberNumber;
    }
    isEmpty(): boolean {
      errorUtil.checkBindError('isEmpty', HashSet, this);
      return this.memberNumber === 0;
    }
    has(value: T): boolean {
      errorUtil.checkBindError('has', HashSet, this);
      return this.hasKey(value);
    }
    add(value: T): boolean {
      errorUtil.checkBindError('add', HashSet, this);
      if (this.has(value)) {
        return false;
      }
      return this.put(value);
    }
    remove(value: T): boolean {
      errorUtil.checkBindError('remove', HashSet, this);
      if (this.removeMember(value) !== undefined) {
        return true;
      }
      return false;
    }
    clear(): void {
      errorUtil.checkBindError('clear', HashSet, this);
      super.clear();
    }
    forEach(callbackfn: (value?: T, key?: T, set?: HashSet<T>) => void,
      thisArg?: Object): void {
      errorUtil.checkBindError('forEach', HashSet, this);
      errorUtil.checkTypeError('callbackfn', 'callable', callbackfn);
      let tagetArray: Array<HashSet<T>> = [];
      tagetArray = this.keyValueArray;
      for (let i: number = 0; i < tagetArray.length; i++) {
        callbackfn.call(thisArg, tagetArray[i].key, tagetArray[i].key, this);
      }
    }
    values(): IterableIterator<T> {
      errorUtil.checkBindError('values', HashSet, this);
      let data: HashSet<T> = this;
      let count: number = 0;
      return {
        next: function (): { done: boolean, value: T } {
          let done: boolean = false;
          let value: T = undefined;
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
    entries(): IterableIterator<[T, T]> {
      errorUtil.checkBindError('entries', HashSet, this);
      let data: HashSet<T> = this;
      let count: number = 0;
      return {
        next: function (): { done: boolean, value: [T, T] } {
          let done: boolean = false;
          let value: [T, T] = undefined;
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
    [Symbol.iterator](): IterableIterator<T> {
      errorUtil.checkBindError('Symbol.iterator', HashSet, this);
      return this.values();
    }
  }
  Object.freeze(HashSet);
  fastHashSet = HashSet;
}
export default fastHashSet;