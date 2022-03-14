# js_worker_module

### Introduction

Worker enables JS to have the ability of multithreading, and completes the communication between worker thread and host thread through PostMessage.

### Interface description
For interface implementation, see: js_worker_module/jsapi/worker

#### Worker object description

The object object used by the host thread to communicate with the worker thread.

##### Interface

1. 

- name

|constructor(scriptURL:string, options? WorkerOptions) | worker constructor to Creates a worker instance |
|---|---|

- example

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
```

2. 

- name

| postMessage(message:Object, options?:PostMessageOptions)  | Sends a message to the worker thread |
|---|---|
| postMessage(message:Object, transfer:ArrayBuffer[])  | Sends a message to the worker thread |

- example

```
// example 1
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.postMessage("hello world");
 
// example 2
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
var buffer = new ArrayBuffer(8);
worker.postMessage(buffer, [buffer]);
```

3. 

- name

| on(type:string, listener:EventListener)  | Adds an event listener to the worker  |
|---|---|

- example

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.on("alert", (e)=>{
     console.log("worker on...");
})
```

4. 

- name

| once(type:string, listener:EventListener)  | Adds an event listener to the worker and removes the event listener automically after it is invoked once  |
|---|---|

- example

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.once("alert", (e)=>{
    console.log("worker on...");
})
```

5. 

- name

| off(type:string, listener?:EventListener)  | Removes an event listener to the worker  |
|---|---|

- example

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.off("alert");
```

6. 

- name

| terminate()  | Terminates the worker thread to stop the worker from receiving messages  |
|---|---|

- example

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.terminate();
```

7. 

- name

| removeEventListener(type:string, listener?:EventListener)  | Removes an event defined for the worker  |
|---|---|

- example

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.removeEventListener("alert");
```

8. 

- name

| dispatchEvent(event: Event)  | Dispatches the event defined for the worker  |
|---|---|

- example

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.dispatchEvent({type:"alert"});
```

9. 

- name

| removeAllListener()  | Removes all event listeners for the worker  |
|---|---|

- example

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.removeAllListener();
```

##### Attribute

1. 

- name

| onexit?:(code:number)=>void  | The onexit attribute of the worker specifies the event handler to be called when the worker exits. The handler is executed in the host thread  |
|---|---|

- example

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.onexit = function(e) {
    console.log("onexit...");
}
```

2. 

- name

| onerror?:(ev:ErrorEvent)=>void  |  The onerror attribute of the worker specifies the event handler to be called when an exception occurs during worker execution. The event handler is executed in the host thread |
|---|---|

- example

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.onerror = function(e) {
    console.log("onerror...");
}
```

3. 

- name

| onmessage?:(ev:MessageEvent)=>void  | The onmessage attribute of the worker specifies the event handler to be called then the host thread receives a  message created by itself and sent by the worker through the parentPort.postMessage. The event handler is executed in the host thread  |
|---|---|

- example

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.onmessage = function(e) {
    console.log("onmessage...");
}
```

4. 

- name

| onmessageerror?:(event:MessageEvent)=>void  | The onmessage attribute of the worker specifies the event handler when the worker receives a message that   cannot be serialized. The event handler is executed in the host thread |
|---|---|

- example

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.onmessageerror = function(e) {
    console.log("onmessageerror...");
}
```

#### parentPort object description

Object of the worker thread used to communicate with the host thread

##### Interface

1. 

- name

| postMessage(message:Object, options?:PostMessageOptions)  | Send a message to host thread  |
|---|---|
| postMessage(message:Object, transfer:ArrayBuffer[])  | Send a message to host thread |

- example

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

- name

| close()  | Close the worker thread to stop the worker from receiving messages  |
|---|---|

- example

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

##### Attribute

1. 

- name

| onmessage?:(event:MessageEvent)=>void  | The onmessage attribute of parentPort specifies the event handler to be called then the worker thread receives a message sent by the host thread through worker postMessage. The event handler is executed in the worker thread  |
|---|---|

- example

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

- name

| onerror?:(ev: ErrorEvent)=>void  | The onerror attribute of parentPort specifies the event handler to be called when an exception occurs during worker execution. The event handler is executed in the worker thread  |
|---|---|

- example

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

- name

| onmessageerror?:(event: MessageEvent)=>void  | The onmessage attribute of parentPort specifies the event handler to be called then the worker receives a message that cannot be deserialized. The event handler is executed in the worker thread.  |
|---|---|

- example

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

### Repositories Involved

- ace_ace_engine(foundation/ace/ace_engine-readme.md)
- ace_napi(foundation/ace/napi-readme.md)

## Related warehouse
[js_worker_module Subsystem](base/compileruntime/js_worker_module-readme.md)

### License

Worker is available under [Mozilla license](https://www.mozilla.org/en-US/MPL/), and the documentation is detailed in [documentation](https://gitee.com/openharmony/js_worker_module/blob/master/mozilla_docs.txt). See [LICENSE](https://gitee.com/openharmony/js_worker_module/blob/master/LICENSE) for the full license text.