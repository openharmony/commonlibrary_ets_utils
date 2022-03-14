# js_worker_module

### 简介

worker能够让js拥有多线程的能力，通过postMessage完成worker线程与宿主线程通信。

### 接口说明
接口实现详见：js_worker_module/jsapi/worker

#### Worker对象描述

宿主线程用于与worker线程通信的Object对象。

##### 接口

1. 

- 接口名

|constructor(scriptURL:string, options? WorkerOptions) | 构造函数 |
|---|---|

- 使用示例

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
```

2. 

- 接口名

| postMessage(message:Object, options?:PostMessageOptions)  | 向worker线程发送消息  |
|---|---|
| postMessage(message:Object, transfer:ArrayBuffer[])  | 向worker线程发送消息  |

- 使用示例

```
// 示例一
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.postMessage("hello world");
 
// 示例二
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
var buffer = new ArrayBuffer(8);
worker.postMessage(buffer, [buffer]);
```

3. 

- 接口名

| on(type:string, listener:EventListener)  | 向worker添加一个事件监听  |
|---|---|

- 使用示例

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.on("alert", (e)=>{
     console.log("worker on...");
})
```

4. 

- 接口名

| once(type:string, listener:EventListener)  | 向worker添加一个事件监听, 事件监听只执行一次便自动删除  |
|---|---|

- 使用示例

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.once("alert", (e)=>{
    console.log("worker on...");
})
```

5. 

- 接口名

| off(type:string, listener?:EventListener)  | 删除worker的事件监听  |
|---|---|

- 使用示例

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.off("alert");
```

6. 

- 接口名

| terminate()  | 关闭worker线程，终止worker发送消息  |
|---|---|

- 使用示例

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.terminate();
```

7. 

- 接口名

| removeEventListener(type:string, listener?:EventListener)  | 删除worker的事件监听  |
|---|---|

- 使用示例

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.removeEventListener("alert");
```

8. 

- 接口名

| dispatchEvent(event: Event)  | 分发定义在worker的事件  |
|---|---|

- 使用示例

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.dispatchEvent({type:"alert"});
```

9. 

- 接口名

| removeAllListener()  | 删除worker的所有事件监听  |
|---|---|

- 使用示例

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.removeAllListener();
```

##### 属性

1. 

- 属性名

| onexit?:(code:number)=>void  | worker退出时被调用的事件处理程序，处理程序在宿主线程中执行  |
|---|---|

- 使用示例

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.onexit = function(e) {
    console.log("onexit...");
}
```

2. 

- 属性名

| onerror?:(ev:ErrorEvent)=>void  | worker在执行过程中发生异常被调用的事件处理程序，处理程序在宿主线程中执行  |
|---|---|

- 使用示例

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.onerror = function(e) {
    console.log("onerror...");
}
```

3. 

- 属性名

| onmessage?:(ev:MessageEvent)=>void  | 宿主线程收到来自其创建的worker通过parentPort.postMessage接口发送的消息时被调用的事件处理程序， 处理程序在宿主线程中执行 |
|---|---|

- 使用示例

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.onmessage = function(e) {
    console.log("onmessage...");
}
```

4. 

- 属性名

| onmessageerror?:(event:MessageEvent)=>void  | worker对象接收到一条无法序列化的消息时被调用的事件处理程序， 处理程序在宿主线程中执行 |
|---|---|

- 使用示例

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.onmessageerror = function(e) {
    console.log("onmessageerror...");
}
```

#### parentPort对象描述

worker线程用于与宿主线程通信的Object对象。

##### 接口

1. 

- 接口名

| postMessage(message:Object, options?:PostMessageOptions)  | 向宿主线程发送消息 |
|---|---|
| postMessage(message:Object, transfer:ArrayBuffer[])  | 向宿主线程发送消息  |

- 使用示例

```
// main.js
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.postMessage("hello world");

// worker.js
import worker from "@ohos.worker"
const parentPort = worker.parentPort;
parentPort.onmessage = function(e) {
    parentPort.postMessage("hello world from worker.js");
}
```

2. 

- 接口名

| close()  | 关闭worker线程，终止worker接收消息  |
|---|---|

- 使用示例

```
// main.js
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.postMessage("hello world");

// worker.js
import worker from "@ohos.worker"
const parentPort = worker.parentPort;
parentPort.onmessage = function(e) {
    parentPort.close();
}
```

##### 属性

1. 

- 属性名

| onmessage?:(event:MessageEvent)=>void  | 宿主线程收到来自其创建的worker通过worker.postMessage接口发送的消息时被调用的事件处理程序，处理程序在worker线程中执行  |
|---|---|

- 使用示例

```
// main.js
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.postMessage("hello world");

// worker.js
import worker from "@ohos.worker"
const parentPort = worker.parentPort;
parentPort.onmessage = function(e) {
    console.log("receive main.js message");
}
```

2. 

- 属性名

| onerror?:(ev: ErrorEvent)=>void  | worker在执行过程中发生异常被调用的事件处理程序，处理程序在worker线程中执行  |
|---|---|

- 使用示例

```
// main.js
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.postMessage("hello world");

// worker.js
import worker from "@ohos.worker"
const parentPort = worker.parentPort;
parentPort.onerror = function(e) {
    console.log("onerror...");
}

```

3. 

- 属性名

| onmessageerror?:(event: MessageEvent)=>void  | worker对象接收到一条无法被反序列化的消息时被调用的事件处理程序， 处理程序在worker线程中执行  |
|---|---|

- 使用示例

```
// main.js
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.postMessage("hello world");

// worker.js
import worker from "@ohos.worker"
const parentPort = worker.parentPort;
parentPort.onmessageerror = function(e) {
    console.log("onmessageerror...");
}
```

### 涉及仓

- ace_ace_engine(foundation/ace/ace_engine-readme_zh.md)
- ace_napi(foundation/ace/napi-readme_zh.md)

## 相关仓
[js_worker_module 子系统](base/compileruntime/js_worker_module-readme_zh.md)


### 许可证

Worker在[Mozilla许可证](https://www.mozilla.org/en-US/MPL/)下可用，说明文档详见[说明文档](https://gitee.com/openharmony/js_worker_module/blob/master/mozilla_docs.txt)。有关完整的许可证文本，有关完整的许可证文本，请参见[许可证](https://gitee.com/openharmony/js_worker_module/blob/master/LICENSE)