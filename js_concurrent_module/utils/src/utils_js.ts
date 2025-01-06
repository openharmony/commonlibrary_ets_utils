/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
const helpUtil = requireInternal('arkts.utils');
let locks = helpUtil.locks;
let ASON = helpUtil.ASON;
let ConditionVariable = helpUtil.ConditionVariable;
let isSendable = helpUtil.isSendable;

const typeErrorCode = 401;
export class BusinessError extends Error {
  code: number;
  constructor(errorcode: number, msg: string) {
    super(msg);
    this.name = 'BusinessError';
    this.code = errorcode;
  }
}

class SendableLruCache {
  // the following cache is a sendable instance of sendable map
  private cache: SendableMap<Object, Object>;
  // Default current size
  private maxSize: number = 64;
  // Default maximum size
  private maxNumber: number = 2147483647;
  private putCount: number = 0;
  private createCount: number = 0;
  private evictionCount: number = 0;
  private hitCount: number = 0;
  private missCount: number = 0;
  public length: number = 0;

  public constructor(capacity?: number) {
    'use sendable';
    if (capacity !== undefined && capacity !== null) {
      if (capacity <= 0 || capacity % 1 !== 0 || capacity > this.maxNumber) {
        let error = new BusinessError(typeErrorCode, `Parameter error. The type of ${capacity} must be small integer`);
        throw error;
      }
      this.maxSize = capacity;
    }
    this.cache = new SendableMap();
  }

  private changeCapacity(newCapacity: number) {
    while (this.cache.size > newCapacity) {
      this.cache.delete(this.cache.keys().next().value);
      this.evictionCount++;
      this.afterRemoval(true, this.cache.keys(), this.cache.values(), null);
    }
  }

  protected afterRemoval(isEvict: boolean, key: Object | undefined | null, value: Object | undefined | null,
    newValue: Object | undefined | null): void {
  }

  protected createDefault(key: Object): Object | undefined {
    if (typeof (key as Object) === 'undefined') {
      let error = new BusinessError(typeErrorCode, `Parameter error. The type of ${key} must be Object`);
      throw error;
    }
    return undefined;
  }

  public updateCapacity(newCapacity: number): void {
    if (typeof newCapacity !== 'number') {
      let error = new BusinessError(typeErrorCode, `Parameter error. The type of ${newCapacity} must be number`);
      throw error;
    }
    if (newCapacity <= 0 || newCapacity % 1 !== 0 || newCapacity > this.maxNumber) {
      let error = new BusinessError(typeErrorCode, `Parameter error. The type of ${newCapacity} must be small integer`);
      throw error;
    }
    if (this.cache.size > newCapacity) {
      this.changeCapacity(newCapacity);
    }
    this.length = this.cache.size;
    this.maxSize = newCapacity;
  }

  public get(key: Object): Object | undefined {
    if (typeof (key as Object) === 'undefined' || key === null) {
      let error = new BusinessError(typeErrorCode, `Parameter error. The type of ${key} must be Object`);
      throw error;
    }
    let value: Object;
    if (this.cache.has(key)) {
      value = this.cache.get(key);
      this.hitCount++;
      this.cache.delete(key);
      this.cache.set(key, value);
      return value;
    }
    this.missCount++;
    let createValue: Object | undefined = this.createDefault(key);
    if (createValue === undefined) {
      return undefined;
    } else {
      value = this.put(key, createValue);
      this.createCount++;
      if (value !== undefined) {
        this.put(key, value);
        this.afterRemoval(false, key, createValue, value);
        return value;
      }
      return createValue;
    }
  }

  public put(key: Object, value: Object): Object | undefined {
    if (typeof (key as Object) === 'undefined') {
      let error = new BusinessError(typeErrorCode, `Parameter error. The type of ${key} must be Object`);
      throw error;
    }
    if (typeof (value as Object) === 'undefined') {
      let error = new BusinessError(typeErrorCode, `Parameter error. The type of ${value} must be Object`);
      throw error;
    }
    if (key === null || value === null) {
      let error = new BusinessError(typeErrorCode, `Parameter error. The type of key and value must be Object`);
      throw error;
    }
    let former: Object | undefined = undefined;
    this.putCount++;
    if (this.cache.has(key)) {
      former = this.cache.get(key);
      this.cache.delete(key);
      this.afterRemoval(false, key, former, value);
    } else if (this.cache.size >= this.maxSize) {
      this.afterRemoval(true, this.cache.keys().next().value, this.cache.values().next().value, null);
      this.cache.delete(this.cache.keys().next().value);
      this.evictionCount++;
    }
    this.cache.set(key, value);
    this.length = this.cache.size;
    former = this.cache.get(key);
    return former;
  }

  public remove(key: Object): Object | undefined {
    if (typeof (key as Object) === 'undefined' || key === null) {
      let error = new BusinessError(typeErrorCode, `Parameter error. The type of ${key} must be Object`);
      throw error;
    }
    let former: Object = undefined;
    if (this.cache.has(key)) {
      former = this.cache.get(key);
      this.cache.delete(key);
      if (former !== null) {
        this.afterRemoval(false, key, former, null);
        this.length = this.cache.size;
      }
    } else {
      this.length = this.cache.size;
    }
    return former;
  }

  public contains(key: Object): boolean {
    if (typeof (key as Object) === 'undefined') {
      let error = new BusinessError(typeErrorCode, `Parameter error. The type of ${key} must be Object`);
      throw error;
    }
    let flag: boolean = false;
    if (this.cache.has(key)) {
      flag = true;
      this.hitCount++;
      let value: Object = this.cache.get(key);
      this.cache.delete(key);
      this.cache.set(key, value);
      this.length = this.cache.size;
    } else {
      this.missCount++;
    }
    return flag;
  }

  public getCreateCount(): number {
    return this.createCount;
  }

  public getMissCount(): number {
    return this.missCount;
  }

  public getRemoveCount(): number {
    return this.evictionCount;
  }

  public getMatchCount(): number {
    return this.hitCount;
  }

  public getPutCount(): number {
    return this.putCount;
  }

  public getCapacity(): number {
    return this.maxSize;
  }

  public clear(): void {
    this.afterRemoval(false, this.cache.keys(), this.cache.values(), null);
    this.cache.clear();
    this.length = this.cache.size;
  }

  public isEmpty(): boolean {
    return this.cache.size === 0;
  }

  public toString(): string {
    let peek: number = 0;
    let hitRate: number = 0;
    let str: string = '';
    peek = this.hitCount + this.missCount;
    if (peek !== 0) {
      // The value is 100 times larger
      hitRate = 100 * this.hitCount / peek;
    }
    str = 'SendableLruCache[ maxSize = ' + this.maxSize + ', hits = ' + this.hitCount +
      ', misses = ' + this.missCount + ', hitRate = ' + hitRate + '% ]';
    return str;
  }

  public values(): Object[] {
    let arr: Array<Object> = [];
    for (let value of this.cache.values()) {
      arr.push(value);
    }
    return arr;
  }

  public keys(): Object[] {
    let arr: Array<Object> = [];
    for (let key of this.cache.keys()) {
      arr.push(key);
    }
    return arr;
  }

  public entries(): IterableIterator<[Object, Object]> {
    return this.cache.entries();
  }

  public [Symbol.iterator](): IterableIterator<[Object, Object]> {
    return this.cache.entries();
  }
}

export default {
  SendableLruCache: SendableLruCache,
  locks: locks,
  ASON: ASON,
  ConditionVariable: ConditionVariable,
  isSendable: isSendable,
};
