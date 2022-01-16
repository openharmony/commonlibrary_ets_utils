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
function hashCode(element: any): number {
  let str = String(element);
  let hash = 0;
  if (hash === 0 && str.length > 0) {
    for (let i = 0; i < str.length; i++) {
      let char = str.charCodeAt(i);
      hash = ((hash << 5) - hash) + char;
      hash = hash & hash;
    }
  }
  return hash;
}

function insert<T>(a: Array<T>, index: number, value: T) {
  for (let i = a.length; i > index; i--) {
    a[i] = a[i - 1];
  }
  a[index] = value;
}

enum ComparResult {
  LESS_THAN = -1,
  BIGGER_THAN = 1,
  EQUALS = 0
}

function compareToString(string1: String, string2: String) {
  let len1 = string1.length;
  let len2 = string2.length;
  let lim = (len1 > len2 ? len2 : len1);
  let k = 0;
  while (k < lim) {
    if (string1.charCodeAt(k) === string2.charCodeAt(k)) {
      k++;
      if (k === lim) {
        return len1 > len2 ? ComparResult.BIGGER_THAN : ComparResult.LESS_THAN;
      }
      continue;
    }
    return string1.charCodeAt(k) > string2.charCodeAt(k) ? ComparResult.BIGGER_THAN : ComparResult.LESS_THAN;
  }
  throw new Error("this function run error");
}

function currencyCompare(a: any, b: any, compareFn?: Function): number {
  if (a === b) return ComparResult.EQUALS;
  if (compareFn != undefined) {
    return compareFn(a, b) ? ComparResult.BIGGER_THAN : ComparResult.LESS_THAN;
  }
  if (typeof a === "number" && typeof b === "number") {
    if (a > b) {
      return ComparResult.BIGGER_THAN;
    } else {
      return ComparResult.LESS_THAN;
    }
  } else if (typeof a === "string" && typeof b === "string") {
    return compareToString(a, b);
  } else if (typeof a === "string" && typeof b === "number") {
    return ComparResult.BIGGER_THAN;
  } else if (typeof a === "number" && typeof b === "string") {
    return ComparResult.LESS_THAN;
  } else if (a instanceof Pair && b instanceof Pair) {
    return currencyCompare(a.key, b.key);
  }
  throw new Error("This form of comparison is not supported");
}

function isIncludeToArray(array1: Array<any>, array2: Array<any>): boolean {
  let newlist = array1.filter((val) => {
    return array2.indexOf(val) > -1;
  })
  if (newlist.length == array2.length) return true;
  return false;
}

class Pair<K, V>{
  key: K;
  value: V;
  constructor(key: K, value: V = key as unknown as V) {
    this.key = key;
    this.value = value;
  }

  entry(): [K, V] {
    return [this.key, this.value];
  }

  toString() {
    return `[#${this.key}: ${this.value}]`;
  }
}

class PlainArrayMembers<T> {
  keys: Array<number>;
  values: Array<T>;
  constructor() {
    this.keys = [];
    this.values = [];
  }
}
class PlainArrayClass<T> {
  protected memberNumber: number;
  protected members: PlainArrayMembers<T>;
  constructor() {
    this.memberNumber = 0;
    this.members = new PlainArrayMembers<T>();
  }

  protected addmember(key: number, value: T) {
    let index = this.binarySearchAtPlain(key);
    if (index > 0) {
      this.members.keys[index] = key;
      this.members.values[index] = value;
    } else {
      index = ~index;
      insert(this.members.keys, index, key);
      insert(this.members.values, index, value);
      this.memberNumber++;
    }
  }

  protected deletemember(index: number, safeSize: number = 1): T {
    this.memberNumber -= safeSize;
    this.members.keys.splice(index, safeSize);
    let removeValue = this.members.values.splice(index, safeSize)[0];
    return removeValue;
  }

  protected binarySearchAtPlain(key: number, startIndex: number = 0, endIndex: number = this.memberNumber): number {
    let low = startIndex;
    let high = endIndex - 1;
    while (low <= high) {
      let mid = (low + high) >>> 1;
      let midVal = this.members.keys[mid];
      if (midVal < key) {
        low = mid + 1;
      } else {
        if (midVal <= key) {
          return mid;
        }
        high = mid - 1;
      }
    }
    return -(low + 1);
  }
}

class LightWeightMembers<K, V> {
  hashs: Array<number>;
  keys: Array<K>;
  values: Array<V>;
  constructor() {
    this.hashs = [];
    this.keys = [];
    this.values = [];
  }
}
class LightWeightClass<K, V> {
  protected memberNumber: number;
  protected members: LightWeightMembers<K, V>;
  protected capacity: number = 8;
  constructor() {
    this.memberNumber = 0;
    this.members = new LightWeightMembers<K, V>();
  }

  protected addmember(key: K, value: V = key as unknown as V) {
    let hash = hashCode(key);
    let index = this.binarySearchAtLightWeight(hash);
    if (index > 0) {
      this.members.keys[index] = key;
      this.members.values[index] = value;
    } else {
      index = ~index;
      insert(this.members.hashs, index, hash);
      insert(this.members.keys, index, key);
      insert(this.members.values, index, value);
      this.memberNumber++;
    }
    if (this.capacity < this.memberNumber) this.ensureCapacity(1);
  }

  protected getIndexByKey(key: K): number {
    let hash = hashCode(key);
    let index = this.binarySearchAtLightWeight(hash);
    if (index < 0 || index >= this.memberNumber) return -1;
    return index;
  }

  protected deletemember(key: K): V {
    let index = this.getIndexByKey(key);
    if (index < 0)
      throw new Error("don't find the key in lightweight");
    this.memberNumber--;
    this.members.hashs.splice(index, 1);
    this.members.keys.splice(index, 1);
    return this.members.values.splice(index, 1)[0];
  }

  protected ensureCapacity(addCapacity: number = 1) {
    let tempCapacity = this.capacity + addCapacity;
    while (this.capacity < tempCapacity) {
      this.capacity = 2 * this.capacity;
    }
  }

  protected getIndex(key: K): number {
    let hash = hashCode(key);
    let index = this.binarySearchAtLightWeight(hash);
    if (index < 0) index = ~index;
    return index;
  }

  protected keyArray(): Array<K> {
    let resultArray: Array<K> = [];
    for (let i = 0; i < this.memberNumber; i++) {
      resultArray.push(this.members[i].key);
    }
    return resultArray;
  }

  protected binarySearchAtLightWeight(hash: number, startIndex: number = 0, endIndex: number = this.memberNumber): number {
    let low = startIndex;
    let high = endIndex - 1;
    while (low <= high) {
      let mid = (low + high) >>> 1;
      let midVal = this.members.hashs[mid];
      if (midVal < hash) {
        low = mid + 1;
      } else {
        if (midVal <= hash) {
          return mid;
        }
        high = mid - 1;
      }
    }
    return -(low + 1);
  }
}

type RBTreeNodeColor = "black" | "red";
const BLACK = "black";
const RED = "red";
class RBTreeNode<K, V> extends Pair<K, V>{
  color: RBTreeNodeColor;
  left: RBTreeNode<K, V> | null;
  right: RBTreeNode<K, V> | null;
  parent: RBTreeNode<K, V> | null;
  constructor(key: K,
    value?: V,
    color: RBTreeNodeColor = RED,
    parent: RBTreeNode<K, V> | null = null,
    left: RBTreeNode<K, V> | null = null,
    right: RBTreeNode<K, V> | null = null) {
    super(key, value);
    this.color = color;
    this.left = left;
    this.right = right;
    this.parent = parent;
  }
}
class RBTreeClass<K, V> {
  private _root: RBTreeNode<K, V> | null;
  public memberNumber: number;
  private _isChange: boolean;
  private _treeArray: Array<RBTreeNode<K, V>>;
  constructor(root: RBTreeNode<K, V> | null = null) {
    this._root = root;
    this.memberNumber = 0;
    this._isChange = true;
    this._treeArray = [];
  }

  get keyValueArray() {
    let result = this.recordByMinToMax();
    return result;
  }

  addNode(key: K, value: V = key as unknown as V): RBTreeClass<K, V> {
    if (this._root === null) {
      this._root = new RBTreeNode<K, V>(key, value)
      this.setColor(this._root, BLACK);
      this.memberNumber++;
      this._isChange = true;
    } else {
      this.addProcess(key, value)
    }
    return this;
  }

  addProcess(key: K, value: V): RBTreeClass<K, V> {
    let leafNode: RBTreeNode<K, V> | null = this._root;
    let parentNode: RBTreeNode<K, V> = this._root as RBTreeNode<K, V>;
    let comp: number = 0;
    while (leafNode !== null) {
      parentNode = leafNode;
      comp = currencyCompare(leafNode.key, key);
      if (comp === 0) {
        leafNode.value = value;
        return this;
      } else if (comp < 0) {
        leafNode = leafNode.right;
      } else {
        leafNode = leafNode.left;
      }
    }
    leafNode = new RBTreeNode<K, V>(key, value)
    leafNode.parent = parentNode;
    if (comp < 0) {
      parentNode.right = leafNode;
    } else {
      parentNode.left = leafNode;
    }
    this.insertRebalance(leafNode);
    this.memberNumber++;
    this._isChange = true;
    return this;
  }

  removeNode(key: K): V | null {
    const removeNode = this.getNode(key);
    if (removeNode === null) {
      return null;
    } else {
      let result = removeNode.value;
      this.removeNodeProcess(removeNode);
      return result;
    }
  }

  removeNodeProcess(removeNode: RBTreeNode<K, V>) {
    if (removeNode.left !== null && removeNode.right !== null) {
      let successor = removeNode.right;
      while (successor.left !== null) {
        successor = successor.left;
      }
      removeNode = successor;
    }
    let replacementNode = (removeNode.right === null ? removeNode.left : removeNode.right);
    if (replacementNode !== null) {
      replacementNode.parent = removeNode.parent;
      if (removeNode.parent === null) {
        this._root = replacementNode;
      } else if (removeNode === removeNode.parent.right) {
        removeNode.parent.right = replacementNode;
      } else if (removeNode === removeNode.parent.left) {
        removeNode.parent.left = replacementNode;
      }
      if (this.getColor(removeNode) === BLACK) {
        this.deleteRebalance(replacementNode)
      }
    } else if (removeNode.parent === null) {
      // removeNode.right = null; removeNode.left = null
      this._root = null;
    } else {
      if (this.getColor(removeNode) === BLACK) {
        this.deleteRebalance(removeNode)
      }
      if (removeNode === removeNode.parent.left) {
        removeNode.parent.left = null;
      } else if (removeNode === removeNode.parent.right) {
        removeNode.parent.right = null;
      }
    }
    this.memberNumber--;
    this._isChange = true;
  }

  getNode(key: K): RBTreeNode<K, V> | null {
    if (this._root === null)
      return null;
    let removeNode: RBTreeNode<K, V> | null = this._root;
    while (removeNode !== null && removeNode.key !== key) {
      removeNode = removeNode.key > key ? removeNode.left : removeNode.right;
    }
    return removeNode;
  }

  findNode(value: V): RBTreeNode<K, V> | null {
    let tempNode: RBTreeNode<K, V> | null = null;
    this.recordByMinToMax();
    for (let i = 0; i < this.memberNumber; i++) {
      if (this._treeArray[i].value === value) tempNode = this._treeArray[i];
      break;
    }
    return tempNode;
  }

  firstNode(): RBTreeNode<K, V> | null {
    let tempNode: RBTreeNode<K, V> | null = this._root;
    while (tempNode !== null && tempNode.left !== null) {
      tempNode = tempNode.left;
    }
    return tempNode;
  }

  lastNode(): RBTreeNode<K, V> | null {
    let tempNode: RBTreeNode<K, V> | null = this._root;
    while (tempNode !== null && tempNode.right !== null) {
      tempNode = tempNode.right;
    }
    return tempNode;
  }

  isEmpty(): boolean {
    return this._root === null;
  }

  setAll(map: RBTreeClass<K, V>) {
    this.recordByMinToMax();
    for (let i = 0; i < this.memberNumber; i++) {
      map.addNode(this._treeArray[i].key, this._treeArray[i].value);
    }
  }

  clearTree() {
    this._root = null;
    this.memberNumber = 0;
  }

  private recordByMinToMax(): Array<RBTreeNode<K, V>> {
    if (!this._isChange) return this._treeArray;
    let stack = [];
    this._treeArray = [];
    let node = this._root;
    while (node != null || stack.length) {
      while (node != null) {
        stack.push(node);
        node = node.left;
      }
      let tempNode = stack.pop();
      if (tempNode === undefined || tempNode === null)
        throw new Error("this function run error");
      node = tempNode;
      this._treeArray.push(node);
      node = node.right;
    }
    this._isChange = false;
    this.memberNumber = this._treeArray.length;
    return this._treeArray;
  }

  private lRotate(datumPointNode: RBTreeNode<K, V>): RBTreeClass<K, V> {
    let newTopNode = datumPointNode.right;
    if (newTopNode === null)
      throw new Error("[rotate right error]: the right child node of the base node === null")
    datumPointNode.right = newTopNode.left;
    datumPointNode.right !== null ? datumPointNode.right.parent = datumPointNode : "";
    newTopNode.parent = datumPointNode.parent;
    if (datumPointNode.parent === null) {
      this._root = newTopNode;
    } else if (datumPointNode.parent.left === datumPointNode) {
      datumPointNode.parent.left = newTopNode;
    } else if (datumPointNode.parent.right === datumPointNode) {
      datumPointNode.parent.right = newTopNode;
    }
    datumPointNode.parent = newTopNode;
    newTopNode.left = datumPointNode;
    return this;
  }

  private rRotate(datumPointNode: RBTreeNode<K, V>): RBTreeClass<K, V> {
    const newTopNode = datumPointNode.left;
    if (newTopNode === null) {
      throw new Error("[rotate right error]: the left child node of the base node === null")
    }
    datumPointNode.left = newTopNode.right;
    datumPointNode.left !== null ? datumPointNode.left.parent = datumPointNode : "";
    newTopNode.parent = datumPointNode.parent
    if (datumPointNode.parent === null) {
      this._root = newTopNode;
    } else if (datumPointNode === datumPointNode.parent.left) {
      datumPointNode.parent.left = newTopNode;
    } else if (datumPointNode === datumPointNode.parent.right) {
      datumPointNode.parent.right = newTopNode;
    }
    datumPointNode.parent = newTopNode;
    newTopNode.right = datumPointNode;
    return this;
  }

  private insertRebalance(fixNode: RBTreeNode<K, V>): RBTreeClass<K, V> {
    let parentNode = fixNode.parent;
    while (this.getColor(parentNode) === RED &&
      parentNode !== null &&
      parentNode.parent !== null) {
      let grandpaNode = parentNode && parentNode.parent;
      if (parentNode === grandpaNode.left &&
        this.getColor(grandpaNode.right) === BLACK &&
        fixNode === parentNode.left) {
        this
          .setColor(parentNode, BLACK)
          .setColor(grandpaNode, RED)
          .rRotate(grandpaNode)
        break;
      } else if (parentNode === grandpaNode.left &&
        this.getColor(grandpaNode.right) === BLACK &&
        fixNode === parentNode.right) {
        this
          .setColor(fixNode, BLACK)
          .setColor(grandpaNode, RED)
          .lRotate(parentNode)
          .rRotate(grandpaNode)
        break;
      } else if (parentNode === grandpaNode.right &&
        this.getColor(grandpaNode.left) === BLACK &&
        fixNode === parentNode.left) {
        this
          .setColor(fixNode, BLACK)
          .setColor(grandpaNode, RED)
          .rRotate(parentNode)
          .lRotate(grandpaNode)
        break;
      } else if (parentNode === grandpaNode.right &&
        this.getColor(grandpaNode.left) === BLACK &&
        fixNode === parentNode.right) {
        this
          .setColor(parentNode, BLACK)
          .setColor(grandpaNode, RED)
          .lRotate(grandpaNode)
        break;
      } else if ((parentNode === grandpaNode.right && this.getColor(grandpaNode.left) === RED) ||
        (parentNode === grandpaNode.left && this.getColor(grandpaNode.right) === RED)) {
        this
          .setColor(parentNode, BLACK)
          .setColor(parentNode === grandpaNode.left ? grandpaNode.right : grandpaNode.left, BLACK)
          .setColor(grandpaNode, RED)
        fixNode = grandpaNode;
        parentNode = fixNode.parent;
      } else {
        throw new Error("Exceptions after adding")
      }
    }
    this._root ? this._root.color = BLACK : "";
    return this;
  }

  private deleteRebalance(fixNode: RBTreeNode<K, V>) {
    while (this.getColor(fixNode) === BLACK && fixNode !== this._root && fixNode.parent) {
      let sibling: RBTreeNode<K, V> | null;
      if (fixNode === fixNode.parent.left) {
        sibling = fixNode.parent.right;
        if (this.getColor(sibling) === RED) {
          this
            .setColor(fixNode.parent, RED)
            .setColor(sibling, BLACK)
            .lRotate(fixNode.parent)
          sibling = fixNode.parent.right;
        }
        if (sibling === null) {
          throw new Error('Error sibling node is null')
        }
        if (this.getColor(sibling.left) === BLACK && this.getColor(sibling.right) === BLACK) {
          this.setColor(sibling, RED)
          fixNode = fixNode.parent
        } else if (this.getColor(sibling.left) === RED && this.getColor(sibling.right) === BLACK) {
          this
            .setColor(sibling, RED)
            .setColor(sibling.left, BLACK)
            .rRotate(sibling);
          sibling = fixNode.parent.right
          if (sibling === null) {
            throw new Error('Error sibling node is empty')
          }
          this
            .setColor(sibling, fixNode.parent.color)
            .setColor(fixNode.parent, BLACK)
            .setColor(sibling.right, BLACK)
            .lRotate(fixNode.parent);
          break;
        } else if (this.getColor(sibling.right) === RED) {
          this
            .setColor(sibling, fixNode.parent.color)
            .setColor(fixNode.parent, BLACK)
            .setColor(sibling.right, BLACK)
            .lRotate(fixNode.parent);
          break;
        } else {
          throw new Error("Adjust the error after the error is deleted")
        }
      } else {
        sibling = fixNode.parent.left;
        if (this.getColor(sibling) === RED) {
          this
            .setColor(sibling, BLACK)
            .setColor(fixNode.parent, RED)
            .rRotate(fixNode.parent);
          sibling = fixNode.parent.left;
        }
        if (sibling === null) {
          throw new Error('Error sibling node is null')
        }
        if (this.getColor(sibling.left) === BLACK && this.getColor(sibling.right) === BLACK) {
          this
            .setColor(sibling, RED)
          fixNode = fixNode.parent;
        } else if (this.getColor(sibling.left) === BLACK && this.getColor(sibling.right) === RED) {
          this
            .setColor(sibling, RED)
            .setColor(sibling.right, BLACK)
            .lRotate(sibling);
          sibling = fixNode.parent.left;
          if (sibling === null) {
            throw new Error('Adjust the error after the error is deleted')
          }
          this
            .setColor(sibling, fixNode.parent.color)
            .setColor(fixNode.parent, BLACK)
            .setColor(sibling.left, BLACK)
            .rRotate(fixNode.parent);
          break;
        } else if (this.getColor(sibling.left) === RED) {
          this
            .setColor(sibling, fixNode.parent.color)
            .setColor(fixNode.parent, BLACK)
            .setColor(sibling.left, BLACK)
            .rRotate(fixNode.parent);
          break;
        } else {
          throw new Error("Adjust the error after the error is deleted")
        }
      }
    }
    this.setColor(fixNode, BLACK)
  }

  private getColor(node: RBTreeNode<K, V> | null): RBTreeNodeColor {
    return node === null ? BLACK : node.color;
  }

  private setColor(node: RBTreeNode<K, V> | null, color: RBTreeNodeColor): RBTreeClass<K, V> {
    if (node === null) {
      throw new Error("Wrong color setting")
    } else {
      node.color = color
    }
    return this;
  }
}

const MAX_CAPACITY = 1 << 30;
const LOADER_FACTOR = 0.75;
class DictionaryClass<K, V>  {
  private _tableLink: { [hashIndex: number]: LinkedList<Pair<K, V>> | RBTreeClass<K, V> };
  protected memberNumber: number;
  private _isChange: boolean;
  private _memberArray: Array<Pair<K, V>>;
  private _capacity: number;
  constructor() {
    this._tableLink = {};
    this.memberNumber = 0;
    this._isChange = true;
    this._memberArray = [];
    this._capacity = 16;
  }

  get keyValueArray() {
    let result = this.keyValues();
    return result;
  }

  protected getHashIndex(key: K): number {
    let h;
    let hash = ((key === null) ? 0 : ((h = hashCode(key)) ^ (h >>> 16)));
    this.expandCapacity();
    let n = this.power(this._capacity);
    return (n - 1) & hash;
  }

  private power(size: number) {
    let n = 1;
    let temp = size;
    while (temp >>> 1 != 1) {
      n++;
      temp = temp >>> 1;
    }
    return n;
  }

  private keyValues(): Pair<K, V>[] {
    if (!this._isChange) return this._memberArray;
    this._memberArray = [];
    const keys = Object.keys(this._tableLink).map((item) => parseInt(item));
    for (let i = 0; i < keys.length; i++) {
      const members = this._tableLink[keys[i]];
      if (members instanceof RBTreeClass) {
        let tempArray = members.keyValueArray;
        for (let i = 0; i < members.memberNumber; i++) {
          this._memberArray.push(new Pair(tempArray[i].key, tempArray[i].value));
        }
      } else {
        if (members != null && !members.isEmpty()) {
          let current = members.getHead();
          while (current != null) {
            this._memberArray.push(current.element);
            current = current.next;
          }
        }
      }
    }
    this.memberNumber = this._memberArray.length;
    let valuePairs = this._memberArray;
    return valuePairs;
  }

  protected expandCapacity() {
    while (this._capacity < this.memberNumber / LOADER_FACTOR && this._capacity < MAX_CAPACITY) {
      this._capacity = 2 * this._capacity;
    }
  }

  protected put(key: K, value: V = key as unknown as V): boolean {
    if (key != null && value != null) {
      this._isChange = true;
      this.memberNumber++;
      const position = this.getHashIndex(key);
      let members = this._tableLink[position];
      if (members instanceof LinkedList && members.count >= 8) {
        let newElement = new RBTreeClass<K, V>();
        let current = members.getHead();
        while (current != null || current != undefined) {
          if (!(current.element instanceof Pair))
            throw new Error("this hashtable member save error");
          newElement.addNode(current.element.key, current.element.value);
          current = current.next;
        }
        newElement.addNode(key, value);
        this._tableLink[position] = newElement;
        return true;
      } else if (members instanceof RBTreeClass) {
        members.addNode(key, value);
        this._tableLink[position] = members;
        return true;
      } else {
        if (this._tableLink[position] == null) {
          members = new LinkedList<Pair<K, V>>();
        }
        if (!this.replaceMember(key, value)) {
          members.push(new Pair(key, value));
          this._tableLink[position] = members;
        }
        return true;
      }
    }
    return false;
  }

  protected replaceMember(key: K, value: V = key as unknown as V): boolean {
    const position = this.getHashIndex(key);
    const members = this._tableLink[position] as LinkedList<Pair<K, V>>;
    if (members === null || members === undefined) return false;
    let current = members.getHead();
    while (current != null || current != undefined) {
      if (current.element.key === key) {
        current.element.value = value;
        return true;
      }
      current = current.next;
    }
    return false;
  }

  protected getValueByKey(key: K): V | undefined {
    const position = this.getHashIndex(key);
    const members = this._tableLink[position];
    if (members instanceof RBTreeClass) {
      let resultNode = members.getNode(key);
      if (resultNode === null) return undefined;
      return resultNode.value;
    } else {
      if (members != null && !members.isEmpty()) {
        members as LinkedList<Pair<K, V>>;
        let current = members.getHead();
        while (current != null) {
          if (current.element.key === key) {
            return current.element.value;
          }
          current = current.next;
        }
      }
    }
    return undefined;
  }

  protected removeMember(key: K): V | null {
    const position = this.getHashIndex(key);
    const members = this._tableLink[position];
    if (members instanceof RBTreeClass) {
      let result = members.removeNode(key);
      if (result != null) {
        this._isChange = true;
        this.memberNumber--;
        return result;
      }
    } else {
      if (members != null && !members.isEmpty()) {
        let current = members.getHead();
        while (current != null) {
          if (current.element.key === key) {
            const result = current.element.value;
            members.remove(current.element);
            if (members.isEmpty()) {
              delete this._tableLink[position];
            }
            this._isChange = true;
            this.memberNumber--;
            return result;
          }
          current = current.next;
        }
      }
    }
    return null;
  }

  protected clear() {
    this._tableLink = {};
    this.memberNumber = 0;
    this._isChange = true;
  }

  protected hasKey(key: K): boolean {
    const position = this.getHashIndex(key);
    const members = this._tableLink[position];
    if (members === null || members === undefined) return false;
    if (members instanceof RBTreeClass) {
      return members.getNode(key) !== null;
    }
    let current = members.getHead();
    while (current != null && current != undefined) {
      if (current.element.key === key) {
        return true;
      }
      current = current.next;
    }
    return false;
  }

  protected setAll(map: DictionaryClass<K, V>): void {
    let memebers = this.keyValues();
    for (let i = 0; i < memebers.length; i++) {
      map.put(memebers[i].key, memebers[i].value);
    }
  }

  protected Values(): V[] {
    const values = [];
    const valuePairs = this.keyValues();
    for (let i = 0; i < valuePairs.length; i++) {
      values.push(valuePairs[i].value);
    }
    return values;
  }
}

class Node<T>{
  element: T;
  next: Node<T> | null;
  constructor(element: T, next: Node<T> | null = null) {
    this.element = element;
    this.next = next;
  }
}
class LinkedList<T> {
  public count: number;
  protected next: Node<T> | null;
  protected head: Node<T> | null;
  constructor() {
    this.count = 0;
    this.next = null;
    this.head = null;
  }

  push(element: T) {
    const node = new Node(element);
    let current;
    if (this.head == null) {
      this.head = node;
    } else {
      current = this.head;
      while (current.next != null) {
        current = current.next;
      }
      current.next = node;
    }
    this.count++;
  }

  removeAt(index: number) {
    if (index >= 0 && index < this.count) {
      let current = this.head;
      if (index === 0 && current != null) {
        this.head = current.next;
      } else {
        const previous = this.getElementAt(index--);
        if (previous !== null) {
          current = previous.next;
          previous.next = (current === null ? null : current.next);
        }
      }
      if (current !== null) {
        this.count--;
        return current.element;
      }
    }
    return undefined;
  }

  getElementAt(index: number) {
    if (index > 0 && index < this.count) {
      let current = this.head;
      for (let i = 0; i < index && current != null; i++) {
        current = current.next;
      }
      return current;
    }
    return null;
  }

  insert(element: T, index: number) {
    if (index >= 0 && index <= this.count) {
      const node = new Node(element);
      if (index === 0) {
        node.next = this.head;
        this.head = node;
      } else {
        const previous = this.getElementAt(index--);
        if (previous === null)
          throw new Error("data storage error");
        node.next = previous.next;
        previous.next = node;
      }
      this.count++;
      return true;
    }
    return false;
  }

  indexOf(element: T) {
    let current = this.head;
    for (let i = 0; i < this.count && current != null; i++) {
      if (currencyCompare(element, current.element)) {
        return i;
      }
      current = current.next;
    }
    return -1;
  }

  remove(element: T) {
    this.removeAt(this.indexOf(element));
  }

  clear() {
    this.head = null;
    this.count = 0;
  }

  isEmpty() {
    return this.count === 0;
  }

  getHead() {
    return this.head;
  }

  toString() {
    if (this.head == null) {
      return "";
    }
    let objString = `${this.head.element}`;
    let current = this.head.next;
    for (let i = 1; i < this.count && current != null; i++) {
      objString = `${objString}, ${current.element}`;
      current = current.next;
    }
    return objString;
  }
}


export default {
  isIncludeToArray,
  LightWeightClass,
  PlainArrayClass,
  RBTreeClass,
  DictionaryClass
}

