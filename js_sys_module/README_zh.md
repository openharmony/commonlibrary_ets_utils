# js_sys_module子系统/组件

-   [简介](#简介)
-   [目录](#目录)
-   [说明](#说明)
    -   [接口说明](#接口说明)
    -   [使用说明](#使用说明)

-   [相关仓](#相关仓])

## 简介
进程主要用于获取进程的相关ID，获取和修改进程的工作目录，退出和关闭进程。 childprocess 对象可用于创建新进程。 主进程可以获取子进程的标准输入输出，发送信号，关闭子进程。
## 目录

```
base/compileruntime/js_sys_module/
├── Class:PROCESS                   # PROCESS类
├── Uid                             # Uid属性
├── Gid                             # Gid属性
├── EUid                            # EUid属性
├── EGid                            # EGid属性
├── Groups                          # Groups属性
├── Pid                             # Pid属性
├── Ppid                            # Ppid属性
├── chdir()                         # chdir方法
├── uptime()                        # uptime方法
├── kill()                          # kill方法
├── abort()                         # abort方法
├── on()                            # on方法
├── tid                             # tid方法
├── getStartRealtime()              # getStartRealtime方法
├── getAvailableCores()             # getAvailableCores方法
├── getPastCputime()                # getPastCputime方法
├── isIsolatedProcess()             # isIsolatedProcess方法
├── is64Bit()                       # is64Bit方法
├── isAppUid()                      # isAppUid方法
├── getUidForName()                 # getUidForName方法
├── getThreadPriority()             # getThreadPriority方法
├── getSystemConfig()               # getSystemConfig方法
├── getEnvironmentVar()             # getEnvironmentVar方法
├── exit()                          # exit方法
├── cwd()                           # cwd方法
├── off()                           # off方法
├── runCmd()                        # runCmd方法
└─── Class:CHILDPROCESS             # class of CHILDPROCESS类
    ├── close()                     # close方法
    ├── kill()                      # kill方法
    ├── getOutput()                 # getOutput方法
    ├── getErrorOutput()            # getErrorOutput方法
    ├── wait()                      # wait方法
    ├── killed                      # killed属性
    ├── pid                         # pid属性
    ├── ppid                        # ppid属性
    └── exitCode                    # exitCode属性
```

## 说明

### 接口说明
| 接口名 | 说明 |
| -------- | -------- |
| const uid :number | 返回进程的数字用户 ID。 |
| const gid :number | 返回进程的数字组 ID。 |
| const euid :number | 返回进程的数字有效用户身份。 |
| const egid :number | 返回 node.js 进程的数字有效组 ID。 |
| const groups :number[] |  返回具有补充组 ID 的数组。 |
| const pid :number | 返回进程的PID。 |
| const ppid :number |  返回当前进程的父进程的PID。 |
| chdir(dir:string) :void | 更改 node.js 进程的当前工作目录。 |
| uptime() :number |  返回当前系统已经运行的秒数。 |
| Kill(pid:number, signal:number) :boolean | 将信号发送到识别的进程PID，true表示发送成功。 |
| abort() :void | 导致 node.js 进程立即退出并生成核心文件。 |
| on(type:string ,listener:EventListener) :void | 用于存储用户触发的事件。 |
| exit(code:number):void | 导致 node.js 进程立即退出。 |
| cwd():string |  返回 node.js 进程的当前工作目录。 |
| off(type: string): boolean | 清除用户存储的事件。 True 表示清算成功。 |
| runCmd(command: string, options?: { timeout : number, killSignal : number \| string, maxBuffer : number }): ChildProcess |通过runcmd，你可以fork一个新进程来运行一个shell并返回childprocess对象。 第一个参数command指的是要运行的shell，第二个参数options指的是子进程的一些运行参数。 这些参数主要是指 timeout、killsignal 和 maxbuffer。 如果设置了timeout，则子进程会在超时后发送killsignal信号。 Maxbuffer 用于限制可以接收的最大 stdout 和 stderr 大小。 |
| wait()： Promise\<number> | 用于等待子进程运行并返回promise对象，其值为子进程的退出码。 |
| getOutput(): Promise\<Uint8Array> |  用于获取子进程的标准输出。 |
| getErrorOutput(): Promise\<Uint8Array> | 用于获取子进程的标准错误输出。 |
| const tid:number | 返回进程的 TID。 |
| getStartRealtime() :number | 获取从系统启动到进程启动所经过的实时时间（以毫秒为单位）。 |
| getAvailableCores() :number[] | 获取多核设备上当前进程可用的 CPU 内核。 |
| getPastCputime() :number | 获取从进程开始到当前时间的 CPU 时间（以毫秒为单位）。 |
| isIsolatedProcess(): boolean | 检查进程是否被隔离。 |
| is64Bit(): boolean | 检查进程是否在 64 位环境中运行。 |
| isAppUid(v:number): boolean | 检查指定的 uid 是否属于特定应用程序。 |
| getUidForName(v:string): number | 根据用户名获取用户所属的用户组ID |
| getThreadPriority(v:number): number | 根据指定的 TID 获取线程优先级。 |
| getSystemConfig(name:number): number | 根据指定的系统配置名称获取系统的配置。 |
| getEnvironmentVar(name:string): string | 根据环境变量的名称获取对应的值。 |
| close(): void | 用于关闭正在运行的子进程。 |
| kill(signal: number \| string): void |  用于向子进程发送信号。 |
| readonly killed: boolean | 表示信号是否发送成功，true表示信号发送成功。 |
| readonly exitCode: number | 表示子进程的退出代码。 |
| readonly pid: number | 表示子进程ID。 |
| readonly ppid: number | 代表主进程ID。 |

### 使用说明

各接口使用方法如下：
1.uid()
```
uid(){
    var res =  Process.uid;
}
```
2.gid()
```
gid(){
    var result = Process.gid;
}
```
3.euid()
```
euid(){
    var and = Process.euid;
}
```
4.egid()
```
egid(){
    var resb = Process.egid;
}
```
5.groups()
```
groups(){
    var answer = Process.groups;
}
```
6.pid()
```
pid(){
    var result = Process.pid;
}
```
7.ppid()
```
ppid(){
    var result = Process.ppid;
}
```
8.chdir()
```
chdir(){
    Process.chdir("123456");
}
```
9.uptime()
```
uptime(){
    var num = Process.uptime();
}
```
10.kill()
```
kill(){
    var ansu = Process.kill(5,23);
}
```
11.abort()
```
abort(){
    Process.abort();
}
```
12.on()
```
on(){
    function add(num){
        var value = num + 5;
        return value;
    }
    Process.on("add",add);
}
```
13.exit()
```
exit(){
    Process.exit(15);
}
```
14.Cwd()
```
Cwd(){
    var result = Process.cwd();
}
```
15.off()

```
off(){
    var result =  Process.off("add");
}
```
16.runCmd()
```
runCmd(){
    var child = process.runCmd('echo abc')
    // killSignal can be a number or a string
    var child = process.runCmd('echo abc;', {killSignal : 'SIGKILL'});
    var child = process.runCmd('sleep 5; echo abc;', {timeout : 1, killSignal : 9, maxBuffer : 2})
}
```
17.wait()
```
wait()
{
    var child = process.runCmd('ls')
    var status = child.wait();
    status.then(val => {
        console.log(val);
    })
}
```
18.getOutput()
```
getOutput(){
    var child = process.runCmd('echo bcd;');
    var res = child.getOutput();
    child.wait();
    res.then(val => {
        console.log(val);
    })
}
```
19.getErrorOutput()
```
getErrorOutput(){
    var child = process.runCmd('makdir 1.txt'); // execute an error command
    var res = child.getErrorOutput();
    child.wait();
    res.then(val => {
        console.log(val);
    })
}
```
20.close()
```
close(){
    var child =  process.runCmd('ls; sleep 5s;')
    var result = child.close()
}
```
21.kill()
```
kill(){
    var child =  process.runCmd('ls; sleep 5s;')
    var result = child.kill('SIGHUP');
    child.wait();
    var temp = child.killed;
}
```
22.killed
```
{
    var child = process.runCmd('ls; sleep 5;')
    child.kill(3);
    var killed_ = child.killed;
    child.wait();
}
```
23.exitCode
```
{
    var child = process.runCmd('ls; sleep 5;')
    child.kill(9);
    child.wait();
    var exitCode_ = child.exitCode;
}
```
24.pid
```
pid
{
    var child = process.runCmd('ls; sleep 5;')
    var pid_ = child.pid;
    child.wait();
}
```
25.ppid
```
ppid
{
    var child = process.runCmd('ls; sleep 5;')
    var ppid_ = child.ppid;
    child.wait();
}
```
26.tid
```
tid(){
    var ansu = Process.tid;
}
```
27.isIsolatedProcess()
```
isIsolatedProcess(){
    var ansu = Process.isIsolatedProcess()();
}
```
28.isAppUid()
```
isAppUid(){
    var ansu = Process.isAppUid(10000);
}
```
29.is64Bit()
```
is64Bit(){
    var ansu = Process.is64Bit();
}
```
30.getUidForName()
```
getUidForName(){
    var buf = "root";
    var ansu = Process.getUidForName(buf);
}
```
31.getEnvironmentVar()
```
getEnvironmentVar(){
    var ansu = Process.getEnvironmentVar('USER');
}
```
32.getAvailableCores()
```
getAvailableCores(){
    var ansu = Process.getAvailableCores();
}
```
33.getThreadPriority()
```
getThreadPriority(){
    var result = Process.getTid();
    var ansu = getThreadPriority(result);
}
```
34.getStartRealtime()
```
getStartRealtime(){
    var ansu = Process.getStartRealtime();
}
```
35.getPastCputime()
```
getPastCputime(){
    var ansu = Process.getPastCputime();
}
```
36.getSystemConfig()
```
getSystemConfig(){
    var _SC_ARG_MAX = 0;
    var ansu = Process.getSystemConfig(_SC_ARG_MAX)
}
```

## 相关仓
[js_sys_module](base/compileruntime/js_sys_module/readme.md)

### 许可证

SYS在[Mozilla许可证](https://www.mozilla.org/en-US/MPL/)下可用，说明文档详见[说明文档](https://gitee.com/openharmony/js_sys_module/blob/master/mozilla_docs.txt)。有关完整的许可证文本，有关完整的许可证文本，请参见[许可证](https://gitee.com/openharmony/js_sys_module/blob/master/LICENSE)