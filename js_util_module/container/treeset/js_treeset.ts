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
  TreeSet: number;
  Load(key: number): Object;
}
let flag: boolean = false;
let fastTreeSet: Object = undefined;
let arkPritvate: ArkPrivate = globalThis['ArkPrivate'] || undefined;
if (arkPritvate !== undefined) {
  fastTreeSet = arkPritvate.Load(arkPritvate.TreeSet);
} else {
  flag = true;
}
if (flag || fastTreeSet === undefined) {
  const treeAbility = requireNapi('util.struct');
  const errorUtil = treeAbility.errorUtil;
  interface IterableIterator<T> {
    next: () => {
      value: T | undefined;
      done: boolean;
    };
  }
  class HandlerTreeSet<T> {
    set(target: TreeSet<T>, p: string, value: string): boolean {
      if (p in target) {
        target[p] = value;
        return true;
      }
      return false;
    }
    defineProperty(): boolean {
      throw new Error(`Can't define Property on TreeSet Object`);
    }
    deleteProperty(): boolean {
      throw new Error(`Can't delete Property on TreeSet Object`);
    }
    setPrototypeOf(): boolean {
      throw new Error(`Can't set Prototype on TreeSet Object`);
    }
  }
  class TreeSet<T> {
    private constitute: any;
    constructor(comparator?: (firstValue: T, secondValue: T) => boolean) {
      errorUtil.checkNewTargetIsNullError('TreeSet', !new.target);
      if (comparator) {
        errorUtil.checkTypeError('comparator', 'callable', comparator);
      }
      this.constitute = new treeAbility.TreeClass(comparator);
      return new Proxy(this, new HandlerTreeSet());
    }
    get length(): number {
      return this.constitute.memberNumber;
    }
    isEmpty(): boolean {
      errorUtil.checkBindError('isEmpty', TreeSet, this);
      return this.constitute.isEmpty();
    }
    has(value: T): boolean {
      errorUtil.checkBindError('has', TreeSet, this);
      return this.constitute.getNode(value) !== undefined;
    }
    add(value: T): boolean {
      errorUtil.checkBindError('add', TreeSet, this);
      this.constitute.addNode(value);
      return true;
    }
    remove(value: T): boolean {
      errorUtil.checkBindError('remove', TreeSet, this);
      let result: T = undefined;
      result = this.constitute.removeNode(value);
      return result !== undefined;
    }
    clear(): void {
      errorUtil.checkBindError('clear', TreeSet, this);
      this.constitute.clearTree();
    }
    getFirstValue(): T {
      errorUtil.checkBindError('getFirstValue', TreeSet, this);
      let tempNode: any = undefined;
      tempNode = this.constitute.firstNode();
      if (tempNode === undefined) {
        return tempNode;
      }
      return tempNode.key;
    }
    getLastValue(): T {
      errorUtil.checkBindError('getLastValue', TreeSet, this);
      let tempNode: any = undefined;
      tempNode = this.constitute.lastNode();
      if (tempNode === undefined) {
        return tempNode;
      }
      return tempNode.key;
    }
    getLowerValue(key: T): T {
      errorUtil.checkBindError('getLowerValue', TreeSet, this);
      let tempNode: any = undefined;
      tempNode = this.constitute.getNode(key);
      if (tempNode === undefined) {
        return tempNode;
      }
      if (tempNode.left !== undefined) {
        return tempNode.left.key;
      }
      let node: any = tempNode;
      while (node.parent !== undefined) {
        if (node.parent.right === node) {
          return node.parent.key;
        }
        node = node.parent; // node.parent.left === node is true;
      }
      return undefined;
    }
    getHigherValue(key: T): T {
      errorUtil.checkBindError('getHigherValue', TreeSet, this);
      let tempNode: any = undefined;
      tempNode = this.constitute.getNode(key);
      if (tempNode === undefined) {
        return tempNode;
      }
      if (tempNode.right !== undefined) {
        return tempNode.right.key;
      }
      let node: any = tempNode;
      while (node.parent !== undefined) {
        if (node.parent.left === node) {
          return node.parent.key;
        }
        node = node.parent; // node.parent.right === node is true;
      }
      return undefined;
    }
    popFirst(): T {
      errorUtil.checkBindError('popFirst', TreeSet, this);
      let firstNode: any = undefined;
      firstNode = this.constitute.firstNode();
      if (firstNode === undefined) {
        return firstNode;
      }
      let value: T = firstNode.value;
      this.constitute.removeNodeProcess(firstNode);
      return value;
    }
    popLast(): T {
      errorUtil.checkBindError('popLast', TreeSet, this);
      let lastNode: any = undefined;
      lastNode = this.constitute.lastNode();
      if (lastNode === undefined) {
        return lastNode;
      }
      let value: T = lastNode.value;
      this.constitute.removeNodeProcess(lastNode);
      return value;
    }
    values(): IterableIterator<T> {
      errorUtil.checkBindError('values', TreeSet, this);
      let data: any = this.constitute;
      let count: number = 0;
      return {
        next: function (): { done: boolean, value: T } {
          let done: boolean = false;
          let value: T = undefined;
          done = count >= data.memberNumber;
          value = done ? undefined : data.keyValueArray[count].value as T;
          count++;
          return {
            done: done,
            value: value,
          };
        },
      };
    }
    forEach(callbackfn: (value?: T, key?: T, set?: TreeSet<T>) => void,
      thisArg?: Object): void {
      errorUtil.checkBindError('forEach', TreeSet, this);
      errorUtil.checkTypeError('callbackfn', 'callable', callbackfn);
      let data: any = this.constitute;
      let tagetArray: Array<any> = data.keyValueArray;
      for (let i: number = 0; i < data.memberNumber; i++) {
        callbackfn.call(thisArg, tagetArray[i].value as T, tagetArray[i].key);
      }
    }
    entries(): IterableIterator<[T, T]> {
      errorUtil.checkBindError('entries', TreeSet, this);
      let data: any = this.constitute;
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
      errorUtil.checkBindError('Symbol.iterator', TreeSet, this);
      return this.values();
    }
  }
  Object.freeze(TreeSet);
  fastTreeSet = TreeSet;
}
export default fastTreeSet;
