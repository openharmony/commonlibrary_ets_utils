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
type CompFun<T> = (firstValue: T, secondValue: T) => boolean;

function hashCode(element: Object): number {
  let str: string = '';
  str = String(element);
  let hash: number = 0;
  if (hash === 0 && str.length > 0) {
    for (let i: number = 0; i < str.length; i++) {
      let char: number = 0;
      char = str.charCodeAt(i);
      hash = ((hash << 5) - hash) + char; // 5 : means number
      hash = hash & hash;
    }
  }
  return hash;
}
function insert<T>(a: Array<T>, index: number, value: T): void {
  for (let i: number = a.length; i > index; i--) {
    a[i] = a[i - 1];
  }
  a[index] = value;
}
enum ComparResult {
  LESS_THAN = -1,
  BIGGER_THAN = 1,
  EQUALS = 0
}
function compareToString(string1: String, string2: String): number {
  let len1: number = string1.length;
  let len2: number = string2.length;
  let lim: number = 0;
  lim = (len1 > len2 ? len2 : len1);
  let k: number = 0;
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
  throw new Error('this compare function run error');
}
function currencyCompare(a: string | Object, b: string | Object, compareFn?: CompFun<string | Object>): number {
  if (a === b) {
    return ComparResult.EQUALS;
  }
  if (a instanceof Pair && b instanceof Pair) {
    return currencyCompare(a.key, b.key, compareFn);
  }
  if (compareFn !== undefined) {
    return compareFn(a, b) ? ComparResult.BIGGER_THAN : ComparResult.LESS_THAN;
  }
  if (typeof a === 'number' && typeof b === 'number') {
    if (a > b) {
      return ComparResult.BIGGER_THAN;
    } else {
      return ComparResult.LESS_THAN;
    }
  } else if (typeof a === 'string' && typeof b === 'string') {
    return compareToString(a, b);
  } else if (typeof a === 'string' && typeof b === 'number') {
    return ComparResult.BIGGER_THAN;
  } else if (typeof a === 'number' && typeof b === 'string') {
    return ComparResult.LESS_THAN;
  }
  throw new Error('This form of comparison is not supported');
}
function isIncludeToArray(array1: Array<number>, array2: Array<number>): boolean {
  let newList: number[] = [];
  newList = array1.filter((val) => {
    return array2.indexOf(val) > -1;
  });
  if (newList.length === array2.length) {
    return true;
  }
  return false;
}
class Pair<K, V> {
  key: K;
  value: V;
  constructor(key: K, value: V = key as unknown as V) {
    this.key = key;
    this.value = value;
  }
  entry(): [K, V] {
    return [this.key, this.value];
  }
  toString(): string {
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
  protected addmember(key: number, value: T): void {
    let index: number = 0;
    index = this.binarySearchAtPlain(key);
    if (index >= 0) {
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
    let low: number = startIndex;
    let high: number = endIndex - 1;
    while (low <= high) {
      let mid: number = (low + high) >>> 1;
      let midVal: number = this.members.keys[mid];
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
  protected capacity: number = 8; // 8 : means number
  constructor() {
    this.memberNumber = 0;
    this.members = new LightWeightMembers<K, V>();
  }
  protected addmember(key: K, value: V = key as unknown as V): void {
    let hash: number = 0;
    hash = hashCode(key);
    let index: number = 0;
    index = this.binarySearchAtLightWeight(hash);
    if (index >= 0) {
      this.members.keys[index] = key;
      this.members.values[index] = value;
    } else {
      index = ~index;
      insert(this.members.hashs, index, hash);
      insert(this.members.keys, index, key);
      insert(this.members.values, index, value);
      this.memberNumber++;
    }
    if (this.capacity < this.memberNumber) {
      this.ensureCapacity(1);
    }
  }
  protected getIndexByKey(key: K): number {
    let hash: number = 0;
    hash = hashCode(key);
    let index: number = 0;
    index = this.binarySearchAtLightWeight(hash);
    // js ( A negative number indicates an inverted number in the array )
    if (index < 0 || index >= this.memberNumber) {
      return -1;
    }
    return index;
  }
  protected deletemember(key: K): V {
    let result: V = undefined;
    let index: number = 0;
    index = this.getIndexByKey(key);
    if (index < 0) {
      return result;
    }
    this.memberNumber--;
    this.members.hashs.splice(index, 1);
    this.members.keys.splice(index, 1);
    return this.members.values.splice(index, 1)[0];
  }
  protected ensureCapacity(addCapacity: number = 1): void {
    let tempCapacity: number = 0;
    tempCapacity = this.capacity + addCapacity;
    while (this.capacity < tempCapacity) {
      this.capacity = 2 * this.capacity; // 2 : means number
    }
  }
  protected getIndex(key: K): number {
    let hash: number = 0;
    hash = hashCode(key);
    let index: number = 0;
    index = this.binarySearchAtLightWeight(hash);
    if (index < 0) {
      index = ~index;
    }
    return index;
  }
  protected keyValueStringArray(): Array<string> {
    let resultArray: Array<string> = [];
    for (let i: number = 0; i < this.memberNumber; i++) {
      resultArray.push(JSON.stringify(this.members.keys[i]) + ':' + JSON.stringify(this.members.values[i]));
    }
    return resultArray;
  }
  protected binarySearchAtLightWeight(hash: number, startIndex: number = 0, endIndex: number = this.memberNumber): number {
    let low: number = startIndex;
    let high: number = endIndex - 1;
    while (low <= high) {
      let mid: number = 0;
      mid = (low + high) >>> 1;
      let midVal: number = 0;
      midVal = this.members.hashs[mid];
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
type RBTreeNodeColor = 0 | 1;
const BLACK = 0;
const RED = 1;
class TreeNode<K, V> extends Pair<K, V> {
  color: RBTreeNodeColor;
  left: TreeNode<K, V> | undefined;
  right: TreeNode<K, V> | undefined;
  parent: TreeNode<K, V> | undefined;
  constructor(key: K,
    value?: V,
    color: RBTreeNodeColor = RED,
    parent?: TreeNode<K, V>,
    left?: TreeNode<K, V>,
    right?: TreeNode<K, V>) {
    super(key, value);
    this.color = color;
    this.left = left;
    this.right = right;
    this.parent = parent;
  }
}
class TreeClass<K, V> {
  private root: TreeNode<K, V> | undefined;
  public memberNumber: number;
  private isChange: boolean;
  private treeNodeArray: Array<TreeNode<K, V>>;
  private compFun: CompFun<K> | undefined;
  constructor(comparator?: CompFun<K>, root?: TreeNode<K, V>) {
    this.root = root;
    this.compFun = comparator;
    this.memberNumber = 0;
    this.isChange = true;
    this.treeNodeArray = [];
  }
  get keyValueArray(): Array<TreeNode<K, V>> {
    let result: Array<TreeNode<K, V>> = [];
    result = this.recordByMinToMax();
    return result;
  }
  addNode(key: K, value: V = key as unknown as V): TreeClass<K, V> {
    if (this.root === undefined) {
      this.root = new TreeNode<K, V>(key, value);
      this.setColor(BLACK, this.root);
      this.memberNumber++;
      this.isChange = true;
    } else {
      this.addProcess(key, value);
    }
    return this;
  }
  addProcess(key: K, value: V): TreeClass<K, V> {
    let leafNode: TreeNode<K, V> | undefined = this.root;
    let parentNode: TreeNode<K, V> = this.root as TreeNode<K, V>;
    let comp: number = 0;
    while (leafNode !== undefined) {
      parentNode = leafNode;
      comp = currencyCompare(leafNode.key, key, this.compFun);
      if (comp === 0) {
        leafNode.value = value;
        return this;
      } else if (comp < 0) {
        leafNode = leafNode.right;
      } else {
        leafNode = leafNode.left;
      }
    }
    leafNode = new TreeNode<K, V>(key, value);
    leafNode.parent = parentNode;
    if (comp < 0) {
      parentNode.right = leafNode;
    } else {
      parentNode.left = leafNode;
    }
    this.insertRebalance(leafNode);
    this.memberNumber++;
    this.isChange = true;
    return this;
  }
  removeNode(key: K): V | undefined {
    let removeNode: TreeNode<K, V> | undefined = undefined;
    removeNode = this.getNode(key);
    if (removeNode === undefined) {
      return undefined;
    } else {
      let result: V = removeNode.value;
      this.removeNodeProcess(removeNode);
      return result;
    }
  }
  removeNodeProcess(removeNode: TreeNode<K, V>): void {
    if (removeNode.left !== undefined && removeNode.right !== undefined) {
      let successor: TreeNode<K, V> | undefined = removeNode.right;
      while (successor.left !== undefined) {
        successor = successor.left;
      }
      removeNode.key = successor.key;
      removeNode.value = successor.value;
      this.removeNodeProcess(successor); // only once
      return;
    } else { // one or zero child
      let child: TreeNode<K, V> | undefined = undefined;
      child = (removeNode.left === undefined ? removeNode.right : removeNode.left);
      if (removeNode.parent === undefined) { // remove is root
        if (child === undefined) {
          this.root = undefined;
        } else {
          child.parent = undefined;
          child.color = BLACK;
          this.root = child;
        }
      } else {
        if (child !== undefined) {
          // delete removeNode
          if (removeNode.parent.left === removeNode) {
            removeNode.parent.left = child;
          } else {
            removeNode.parent.right = child;
          }
          if (this.getColor(removeNode) === BLACK) {
            this.deleteRebalance(child);
          }
        } else {
          if (this.getColor(removeNode) === BLACK) {
            this.deleteRebalance(removeNode);
          }
          if (removeNode.parent.left === removeNode) {
            removeNode.parent.left = child;
          } else {
            removeNode.parent.right = child;
          }
        }
      }
      this.memberNumber--;
      this.isChange = true;
    }
  }
  getNode(key: K): TreeNode<K, V> | undefined {
    if (this.root === undefined) {
      return undefined;
    }
    let findNode: TreeNode<K, V> | undefined = this.root;
    while (findNode !== undefined && findNode.key !== key) {
      findNode = currencyCompare(findNode.key, key, this.compFun) === ComparResult.BIGGER_THAN ?
        findNode.left : findNode.right;
    }
    return findNode;
  }
  findNode(value: V): TreeNode<K, V> | undefined {
    let tempNode: TreeNode<K, V> | undefined = undefined;
    this.recordByMinToMax();
    for (let i: number = 0; i < this.memberNumber; i++) {
      if (this.treeNodeArray[i].value === value) {
        tempNode = this.treeNodeArray[i];
        break;
      }
    }
    return tempNode;
  }
  firstNode(): TreeNode<K, V> | undefined {
    let tempNode: TreeNode<K, V> | undefined = this.root;
    while (tempNode !== undefined && tempNode.left !== undefined) {
      tempNode = tempNode.left;
    }
    return tempNode;
  }
  lastNode(): TreeNode<K, V> | undefined {
    let tempNode: TreeNode<K, V> | undefined = this.root;
    while (tempNode !== undefined && tempNode.right !== undefined) {
      tempNode = tempNode.right;
    }
    return tempNode;
  }
  isEmpty(): boolean {
    return this.root === undefined;
  }
  setAll(map: TreeClass<K, V>): void {
    let tempArray: TreeNode<K, V>[] = [];
    tempArray = map.recordByMinToMax();
    for (let i: number = 0; i < map.memberNumber; i++) {
      this.addNode(tempArray[i].key, tempArray[i].value);
    }
  }
  clearTree(): void {
    this.root = undefined;
    this.memberNumber = 0;
  }
  private recordByMinToMax(): Array<TreeNode<K, V>> {
    if (!this.isChange) {
      return this.treeNodeArray;
    }
    let stack: Array<TreeNode<K, V>> = [];
    this.treeNodeArray = [];
    let node: TreeNode<K, V> | undefined = this.root;
    while (node !== undefined || stack.length) {
      while (node !== undefined) {
        stack.push(node);
        node = node.left;
      }
      let tempNode = stack.pop();
      if (tempNode === undefined) {
        throw new Error('this function run error');
      }
      node = tempNode;
      this.treeNodeArray.push(node);
      node = node.right;
    }
    this.isChange = false;
    this.memberNumber = this.treeNodeArray.length;
    return this.treeNodeArray;
  }
  private lRotate(datumPointNode: TreeNode<K, V>): TreeClass<K, V> {
    let newTopNode: TreeNode<K, V> | undefined = datumPointNode.right;
    if (newTopNode === undefined) {
      throw new Error('[rotate right error]: the right child node of the base node === undefined');
    }
    datumPointNode.right = newTopNode.left;
    datumPointNode.right !== undefined ? datumPointNode.right.parent = datumPointNode : '';
    newTopNode.parent = datumPointNode.parent;
    if (datumPointNode.parent === undefined) {
      this.root = newTopNode;
    } else if (datumPointNode.parent.left === datumPointNode) {
      datumPointNode.parent.left = newTopNode;
    } else if (datumPointNode.parent.right === datumPointNode) {
      datumPointNode.parent.right = newTopNode;
    }
    datumPointNode.parent = newTopNode;
    newTopNode.left = datumPointNode;
    return this;
  }
  private rRotate(datumPointNode: TreeNode<K, V>): TreeClass<K, V> {
    let newTopNode: TreeNode<K, V> | undefined = datumPointNode.left;
    if (newTopNode === undefined) {
      throw new Error('[rotate right error]: the left child node of the base node === undefined');
    }
    datumPointNode.left = newTopNode.right;
    datumPointNode.left !== undefined ? datumPointNode.left.parent = datumPointNode : '';
    newTopNode.parent = datumPointNode.parent;
    if (datumPointNode.parent === undefined) {
      this.root = newTopNode;
    } else if (datumPointNode === datumPointNode.parent.left) {
      datumPointNode.parent.left = newTopNode;
    } else if (datumPointNode === datumPointNode.parent.right) {
      datumPointNode.parent.right = newTopNode;
    }
    datumPointNode.parent = newTopNode;
    newTopNode.right = datumPointNode;
    return this;
  }
  private insertRebalance(fixNode: TreeNode<K, V>): TreeClass<K, V> {
    let parentNode: TreeNode<K, V> | undefined = fixNode.parent;
    while (this.getColor(parentNode) === RED &&
      parentNode !== undefined &&
      parentNode.parent !== undefined) {
      let grandpaNode = parentNode && parentNode.parent;
      if (parentNode === grandpaNode.left &&
        this.getColor(grandpaNode.right) === BLACK &&
        fixNode === parentNode.left) {
        this
          .setColor(BLACK, parentNode)
          .setColor(RED, grandpaNode)
          .rRotate(grandpaNode);
        break;
      } else if (parentNode === grandpaNode.left &&
        this.getColor(grandpaNode.right) === BLACK &&
        fixNode === parentNode.right) {
        this
          .setColor(BLACK, fixNode)
          .setColor(RED, grandpaNode)
          .lRotate(parentNode)
          .rRotate(grandpaNode);
        break;
      } else if (parentNode === grandpaNode.right &&
        this.getColor(grandpaNode.left) === BLACK &&
        fixNode === parentNode.left) {
        this
          .setColor(BLACK, fixNode)
          .setColor(RED, grandpaNode)
          .rRotate(parentNode)
          .lRotate(grandpaNode);
        break;
      } else if (parentNode === grandpaNode.right &&
        this.getColor(grandpaNode.left) === BLACK &&
        fixNode === parentNode.right) {
        this
          .setColor(BLACK, parentNode)
          .setColor(RED, grandpaNode)
          .lRotate(grandpaNode);
        break;
      } else if ((parentNode === grandpaNode.right && this.getColor(grandpaNode.left) === RED) ||
        (parentNode === grandpaNode.left && this.getColor(grandpaNode.right) === RED)) {
        this
          .setColor(BLACK, parentNode)
          .setColor(BLACK, parentNode === grandpaNode.left ? grandpaNode.right : grandpaNode.left)
          .setColor(RED, grandpaNode);
        fixNode = grandpaNode;
        parentNode = fixNode.parent;
      } else {
        throw new Error('Exceptions after adding');
      }
    }
    this.root ? this.root.color = BLACK : '';
    return this;
  }
  private deleteRebalance(fixNode: TreeNode<K, V>): void {
    while (this.getColor(fixNode) === BLACK && fixNode !== this.root && fixNode.parent) {
      let sibling: TreeNode<K, V> | undefined = undefined;
      if (fixNode === fixNode.parent.left) {
        sibling = fixNode.parent.right;
        if (this.getColor(sibling) === RED) {
          this
            .setColor(RED, fixNode.parent)
            .setColor(BLACK, sibling)
            .lRotate(fixNode.parent);
          sibling = fixNode.parent.right;
        }
        if (sibling === undefined) {
          throw new Error('Error sibling node is undefined');
        }
        if (this.getColor(sibling.left) === BLACK && this.getColor(sibling.right) === BLACK) {
          this.setColor(RED, sibling);
          fixNode = fixNode.parent;
        } else if (this.getColor(sibling.left) === RED && this.getColor(sibling.right) === BLACK) {
          this
            .setColor(RED, sibling)
            .setColor(BLACK, sibling.left)
            .rRotate(sibling);
          sibling = fixNode.parent.right;
          if (sibling === undefined) {
            throw new Error('Error sibling node is empty');
          }
          this
            .setColor(fixNode.parent.color, sibling)
            .setColor(BLACK, fixNode.parent)
            .setColor(BLACK, sibling.right)
            .lRotate(fixNode.parent);
          break;
        } else if (this.getColor(sibling.right) === RED) {
          this
            .setColor(fixNode.parent.color, sibling)
            .setColor(BLACK, fixNode.parent)
            .setColor(BLACK, sibling.right)
            .lRotate(fixNode.parent);
          break;
        } else {
          throw new Error('Adjust the error after the error is deleted');
        }
      } else {
        sibling = fixNode.parent.left;
        if (this.getColor(sibling) === RED) {
          this
            .setColor(BLACK, sibling)
            .setColor(RED, fixNode.parent)
            .rRotate(fixNode.parent);
          sibling = fixNode.parent.left;
        }
        if (sibling === undefined) {
          throw new Error('Error sibling node is undefined');
        }
        if (this.getColor(sibling.left) === BLACK && this.getColor(sibling.right) === BLACK) {
          this
            .setColor(RED, sibling);
          fixNode = fixNode.parent;
        } else if (this.getColor(sibling.left) === BLACK && this.getColor(sibling.right) === RED) {
          this
            .setColor(RED, sibling)
            .setColor(BLACK, sibling.right)
            .lRotate(sibling);
          sibling = fixNode.parent.left;
          if (sibling === undefined) {
            throw new Error('Adjust the error after the error is deleted');
          }
          this
            .setColor(fixNode.parent.color, sibling)
            .setColor(BLACK, fixNode.parent)
            .setColor(BLACK, sibling.left)
            .rRotate(fixNode.parent);
          break;
        } else if (this.getColor(sibling.left) === RED) {
          this
            .setColor(fixNode.parent.color, sibling)
            .setColor(BLACK, fixNode.parent,)
            .setColor(BLACK, sibling.left)
            .rRotate(fixNode.parent);
          break;
        } else {
          throw new Error('Adjust the error after the error is deleted');
        }
      }
    }
    this.setColor(BLACK, fixNode);
  }
  private getColor(node: TreeNode<K, V> | undefined): RBTreeNodeColor {
    return node === undefined ? BLACK : node.color;
  }
  private setColor(color: RBTreeNodeColor, node: TreeNode<K, V> | undefined): TreeClass<K, V> {
    if (node === undefined) {
      throw new Error('Wrong color setting');
    } else {
      node.color = color;
    }
    return this;
  }
}
const MAX_CAPACITY = 1 << 30; // 30 : means number
const LOADER_FACTOR = 0.75;
class DictionaryClass<K, V> {
  private tableLink: { [hashIndex: number]: LinkedList<Pair<K, V>> | TreeClass<K, V> };
  protected memberNumber: number;
  private isChange: boolean;
  private memberArray: Array<Pair<K, V>>;
  private capacity: number;
  constructor() {
    this.tableLink = {};
    this.memberNumber = 0;
    this.isChange = true;
    this.memberArray = [];
    this.capacity = 16;
  }
  get keyValueArray(): Pair<K, V>[] {
    let result: Pair<K, V>[] = [];
    result = this.keyValues();
    return result;
  }
  protected getHashIndex(key: K): number {
    let h: number = 0;
    let hash: number = 0;
    hash = ((key === null) ? 0 : ((h = hashCode(key)) ^ (h >>> 16))); // 16 : means number
    if (this.expandCapacity()) {
      this.keyValues();
      this.memberNumber = 0;
      this.tableLink = {};
      this.isChange = true;
      for (let item of this.memberArray) {
        this.put(item.key, item.value);
      }
      this.memberNumber++;
    }
    let n: number = 0;
    n = this.power(this.capacity);
    return (n - 1) & hash;
  }
  private power(size: number): number {
    let n: number = 1;
    let temp: number = size;
    while (temp >>> 1 !== 1) {
      n++;
      temp = temp >>> 1;
    }
    return n;
  }
  private keyValues(): Pair<K, V>[] {
    if (!this.isChange) {
      return this.memberArray;
    }
    this.memberArray = [];
    let keys: number[] = [];
    keys = Object.keys(this.tableLink).map((item) => parseInt(item));
    for (let i: number = 0; i < keys.length; i++) {
      let members: TreeClass<K, V> | LinkedList<Pair<K, V>> = undefined;
      members = this.tableLink[keys[i]];
      if (members instanceof TreeClass) {
        let tempArray: TreeNode<K, V>[] = members.keyValueArray;
        for (let i: number = 0; i < members.memberNumber; i++) {
          this.memberArray.push(new Pair(tempArray[i].key, tempArray[i].value));
        }
      } else {
        if (members !== undefined && !members.isEmpty()) {
          let current: Node<Pair<K, V>> = members.getHead();
          while (current !== undefined) {
            this.memberArray.push(current.element);
            current = current.next;
          }
        }
      }
    }
    this.memberNumber = this.memberArray.length;
    let valuePairs: Pair<K, V>[] = this.memberArray;
    return valuePairs;
  }
  protected expandCapacity(): boolean {
    let capacityChange: boolean = false;
    while (this.capacity < this.memberNumber / LOADER_FACTOR && this.capacity < MAX_CAPACITY) {
      this.capacity = 2 * this.capacity; // 2 : means number
      capacityChange = true;
    }
    return capacityChange;
  }
  protected put(key: K, value: V = key as unknown as V): boolean {
    this.isChange = true;
    if (!this.hasKey(key)) {
      this.memberNumber++;
    }
    let position: number = 0;
    position = this.getHashIndex(key);
    let members: LinkedList<Pair<K, V>> | TreeClass<K, V> = undefined;
    members = this.tableLink[position];
    if (members instanceof LinkedList && members.count >= 8) { // 8 : means number
      let newElement: TreeClass<K, V> = new TreeClass<K, V>();
      let current: Node<Pair<K, V>> = undefined;
      current = members.getHead();
      while (current != null || current !== undefined) {
        if (!(current.element instanceof Pair)) {
          return false;
        }
        newElement.addNode(current.element.key, current.element.value);
        current = current.next;
      }
      newElement.addNode(key, value);
      this.tableLink[position] = newElement;
      return true;
    } else if (members instanceof TreeClass) {
      members.addNode(key, value);
      this.tableLink[position] = members;
      return true;
    } else {
      if (this.tableLink[position] === undefined) {
        members = new LinkedList<Pair<K, V>>();
      }
      if (!this.replaceMember(key, value)) {
        members.push(new Pair(key, value));
        this.tableLink[position] = members;
      }
      return true;
    }
  }
  protected replaceMember(key: K, value: V = key as unknown as V): boolean {
    let position: number = 0;
    position = this.getHashIndex(key);
    let members: LinkedList<Pair<K, V>> = undefined;
    members = this.tableLink[position] as LinkedList<Pair<K, V>>;
    if (members === null || members === undefined) {
      return false;
    }
    let current: Node<Pair<K, V>> = undefined;
    current = members.getHead();
    while (current !== undefined) {
      if (current.element.key === key) {
        current.element.value = value;
        return true;
      }
      current = current.next;
    }
    return false;
  }
  protected getValueByKey(key: K): V | undefined {
    let position: number = 0;
    position = this.getHashIndex(key);
    let members: LinkedList<Pair<K, V>> | TreeClass<K, V> = undefined;
    members = this.tableLink[position];
    if (members instanceof TreeClass) {
      let resultNode: TreeNode<K, V> | undefined = undefined;
      resultNode = members.getNode(key);
      if (resultNode === undefined) {
        return undefined;
      }
      return resultNode.value;
    } else {
      if (members !== undefined && !members.isEmpty()) {
        members as LinkedList<Pair<K, V>>;
        let current: Node<Pair<K, V>> = undefined;
        current = members.getHead();
        while (current !== undefined) {
          if (current.element.key === key) {
            return current.element.value;
          }
          current = current.next;
        }
      }
    }
    return undefined;
  }
  protected removeMember(key: K): V | undefined {
    let position: number = 0;
    position = this.getHashIndex(key);
    let members: LinkedList<Pair<K, V>> | TreeClass<K, V> = undefined;
    members = this.tableLink[position];
    if (members instanceof TreeClass) {
      let result: V | undefined = members.removeNode(key);
      if (result !== undefined) {
        this.isChange = true;
        this.memberNumber--;
        return result;
      }
    } else {
      if (members !== undefined && !members.isEmpty()) {
        let current: Node<Pair<K, V>> = undefined;
        current = members.getHead();
        while (current !== undefined) {
          if (current.element.key === key) {
            let result: V | undefined = current.element.value;
            members.remove(current.element);
            if (members.isEmpty()) {
              delete this.tableLink[position];
            }
            this.isChange = true;
            this.memberNumber--;
            return result;
          }
          current = current.next;
        }
      }
    }
    return undefined;
  }
  protected clear(): void {
    this.tableLink = {};
    this.memberNumber = 0;
    this.isChange = true;
    this.capacity = 16;
  }
  protected hasKey(key: K): boolean {
    let position: number = 0;
    position = this.getHashIndex(key);
    let members: LinkedList<Pair<K, V>> | TreeClass<K, V> = undefined;
    members = this.tableLink[position];
    if (members === undefined || members === undefined) {
      return false;
    }
    if (members instanceof TreeClass) {
      return members.getNode(key) !== undefined;
    }
    let current: Node<Pair<K, V>> = undefined;
    current = members.getHead();
    while (current !== undefined && current !== undefined) {
      if (current.element.key === key) {
        return true;
      }
      current = current.next;
    }
    return false;
  }
  protected setAll(map: DictionaryClass<K, V>): void {
    let memebers: Pair<K, V>[] = [];
    memebers = map.keyValues();
    for (let i: number = 0; i < memebers.length; i++) {
      this.put(memebers[i].key, memebers[i].value);
    }
  }
  protected values(): V[] {
    let values: Array<V> = [];
    let valuePairs: Pair<K, V>[] = [];
    valuePairs = this.keyValues();
    for (let i: number = 0; i < valuePairs.length; i++) {
      values.push(valuePairs[i].value);
    }
    return values;
  }
}
class Node<T> {
  element: T;
  next: Node<T> | undefined;
  constructor(element: T, next?: Node<T>) {
    this.element = element;
    this.next = next;
  }
}
class LinkedList<T> {
  public count: number;
  protected next: Node<T> | undefined;
  protected head: Node<T> | undefined;
  constructor() {
    this.count = 0;
    this.next = undefined;
    this.head = undefined;
  }
  push(element: T): void {
    let node: Node<T> = undefined;
    node = new Node(element);
    let current: undefined | Node<T>;
    if (this.head === undefined) {
      this.head = node;
    } else {
      current = this.head;
      while (current.next !== undefined) {
        current = current.next;
      }
      current.next = node;
    }
    this.count++;
  }
  removeAt(index: number): T {
    if (index >= 0 && index < this.count) {
      let current: Node<T> = this.head;
      if (index === 0 && current !== undefined) {
        this.head = current.next;
      } else {
        let previous: Node<T> = this.getElementAt(index--);
        if (previous !== undefined) {
          current = previous.next;
          previous.next = (current === undefined ? undefined : current.next);
        }
      }
      if (current !== undefined) {
        this.count--;
        return current.element;
      }
    }
    return undefined;
  }
  getElementAt(index: number): Node<T> {
    if (index > 0 && index < this.count) {
      let current: Node<T> = this.head;
      for (let i: number = 0; i < index && current !== undefined; i++) {
        current = current.next;
      }
      return current;
    }
    return undefined;
  }
  insert(element: T, index: number): boolean {
    if (index >= 0 && index <= this.count) {
      let node: Node<T> = undefined;
      node = new Node(element);
      if (index === 0) {
        node.next = this.head;
        this.head = node;
      } else {
        let previous: Node<T> = this.getElementAt(index--);
        if (previous === undefined) {
          throw new Error('data storage error');
        }
        node.next = previous.next;
        previous.next = node;
      }
      this.count++;
      return true;
    }
    return false;
  }
  indexOf(element: T, compareFn?: CompFun<T>): number {
    let current: Node<T> = this.head;
    for (let i: number = 0; i < this.count && current !== undefined; i++) {
      if (currencyCompare(element, current.element, compareFn) === ComparResult.EQUALS) {
        return i;
      }
      current = current.next;
    }
    return -1;
  }
  remove(element: T, compareFn?: CompFun<T>): void {
    this.removeAt(this.indexOf(element, compareFn));
  }
  clear(): void {
    this.head = undefined;
    this.count = 0;
  }
  isEmpty(): boolean {
    return this.count === 0;
  }
  getHead(): Node<T> {
    return this.head;
  }
  toString(): string {
    if (this.head === undefined) {
      return '';
    }
    let objString: string = '';
    objString = `${this.head.element}`;
    let current: Node<T> = undefined;
    current = this.head.next;
    for (let i: number = 1; i < this.count && current !== undefined; i++) {
      objString = `${objString}, ${current.element}`;
      current = current.next;
    }
    return objString;
  }
}

const NEWTARGET_IS_NULL_ERROR_CODE = 10200012;
const BIND_ERROR_CODE = 10200011;
const RANGE_ERROR_CODE = 10200001;
const TYPE_ERROR_CODE = 401;
const EMPTY_ERROR_CODE = 10200010;

class BusinessError extends Error {
  code: number;
  constructor(message, code) {
    super();
    this.name = 'BusinessError';
    this.message = message;
    this.code = code;
  }
}

function checkBindError(methodName: string, className: Function, self: unknown): void {
  if (!(self instanceof className)) {
    throw new BusinessError(`The ${methodName} method cannot be bound`, BIND_ERROR_CODE);
  }
}

function checkTypeError(paramName: string, type: string, receivedValue: unknown) {
  let tmpType = '';
  if (typeof receivedValue === 'object') {
    tmpType = (receivedValue === null) ? 'Null' : receivedValue.constructor.name;
  } else {
    tmpType = typeof receivedValue;
  }
  if (tmpType === 'number' && type === 'Integer') {
    tmpType = Number.isInteger(receivedValue) ? 'Integer' : 'number';
  }
  if (tmpType === 'function' && type === 'callable') {
    tmpType = 'callable';
  }
  if (tmpType.toLocaleLowerCase() !== type.toLocaleLowerCase()) {
    type = (type === 'Integer') ? 'number' : type;
    throw new BusinessError(`The type of "${paramName}" must be ${type}. Received value is: ${receivedValue}`, TYPE_ERROR_CODE);
  }
}

function checkRangeError(paramName: string, receivedValue: unknown, min?: number, max?: number, options?: string): void {
  let range = [];
  let minFlag = true;
  let maxFlag = true;
  if (min !== undefined) {
    if (options === '!=min') {
      minFlag = (receivedValue > min);
      range.push(`> ${min}`);
    } else {
      minFlag = (receivedValue >= min);
      range.push(`>= ${min}`);
    }
  }
  if (max !== undefined) {
    max = (max < 0) ? 0 : max;
    maxFlag = (receivedValue <= max);
    range.push(`<= ${max}`);
  }
  if (!(minFlag && maxFlag)) {
    throw new BusinessError(
      `The value of "${paramName}" is out of range. It must be ${range.join(' && ')}. Received value is: ${receivedValue}`, RANGE_ERROR_CODE);
  }
}

function checkIsEmptyError(isEmpty: boolean): void {
  if (isEmpty) {
    throw new BusinessError(`Container is empty`, EMPTY_ERROR_CODE);
  }
}

function checkNewTargetIsNullError(className: string, isNull: boolean): void {
  if (isNull) {
    throw new BusinessError(`The ${className}'s constructor cannot be directly invoked`, NEWTARGET_IS_NULL_ERROR_CODE);
  }
}

let errorUtil = {
  checkBindError,
  checkTypeError,
  checkRangeError,
  checkIsEmptyError,
  checkNewTargetIsNullError
}
export default {
  isIncludeToArray,
  LightWeightClass,
  PlainArrayClass,
  TreeClass,
  DictionaryClass,
  errorUtil
};