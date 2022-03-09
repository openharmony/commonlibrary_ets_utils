# js_sys_module Subsystems/Components

-   [Introduction](#Introduction)
-   [Directory](#Directory)
-   [Description](#Description)
    -   [Interface description](#Interface description)
    -   [Interface instructions](#Interface instructions)

-   [Related warehouse](#Related warehouse])

## Introduction
Process is mainly used to obtain the relevant ID of the process, obtain and modify the working directory of the process, exit and close the process. The childprocess object can be used to create a new process. The main process can obtain the standard input and output of the child process, send signals and close the child process. 
## Directory

```
base/compileruntime/js_sys_module/
├── Class:PROCESS                   # class of PROCESS
├── Uid                             # attribute of Uid
├── Gid                             # attribute of Gid
├── EUid                            # attribute of EUid
├── EGid                            # attribute of EGid
├── Groups                          # attribute of Groups
├── Pid                             # attribute of Pid
├── Ppid                            # attribute of Ppid
├── chdir()                         # method of chdir
├── uptime()                        # method of uptime
├── kill()                          # method of kill
├── abort()                         # method of abort
├── on()                            # method of on
├── tid                             # method of tid
├── getStartRealtime()              # method of getStartRealtime
├── getAvailableCores()             # method of getAvailableCores
├── getPastCputime()                # method of getPastCputime
├── isIsolatedProcess()             # method of isIsolatedProcess
├── is64Bit()                       # method of is64Bit
├── isAppUid()                      # method of isAppUid
├── getUidForName()                 # method of getUidForName
├── getThreadPriority()             # method of getThreadPriority
├── getSystemConfig()               # method of getSystemConfig
├── getEnvironmentVar()             # method of getEnvironmentVar
├── exit()                          # method of exit
├── cwd()                           # method of cwd
├── off()                           # method of off
├── runCmd()                        # method of runCmd
└─── Class:CHILDPROCESS             # class of CHILDPROCESS
    ├── close()                     # method of close
    ├── kill()                      # method of kill
    ├── getOutput()                 # method of getOutput
    ├── getErrorOutput()            # method of getErrorOutput
    ├── wait()                      # method of wait
    ├── killed                      # attribute of killed
    ├── pid                         # attribute of pid
    ├── ppid                        # attribute of ppid
    └── exitCode                    # attribute of exitCode
```

## Description

### Interface description
| Interface name | description |
| -------- | -------- |
| const uid :number | returns the digital user ID of the process. |
| const gid :number | returns the numeric group ID of the process. |
| const euid :number | returns the numeric valid user identity of the process. |
| const egid :number | returns the numeric valid group ID of the node.js process. |
| const groups :number[] |  returns an array with supplementary group ID. |
| const pid :number | returns the PID of the process. |
| const ppid :number |  returns the PID of the parent process of the current process. |
| chdir(dir:string) :void | change the current working directory of the node.js process. |
| uptime() :number |  returns the number of seconds the current system has been running. |
| Kill(pid:number, signal:number) :boolean | send the signal to the identified process PID, and true means the sending is successful. |
| abort() :void | cause the node.js process to exit immediately and generate a core file. |
| on(type:string ,listener:EventListener) :void | used to store events triggered by users. |
| exit(code:number):void | cause the node.js process to exit immediately. |
| cwd():string |  returns the current working directory of the node.js process. |
| off(type: string): boolean | clear the events stored by the user. True means the clearing is successful. |
| runCmd(command: string, options?: { timeout : number, killSignal : number \| string, maxBuffer : number }): ChildProcess |through runcmd, you can fork a new process to run a shell and return the childprocess object. The first parameter command refers to the shell to be run, and the second parameter options refers to some running parameters of the child process. These parameters mainly refer to timeout, killsignal and maxbuffer. If timeout is set, the child process will send a signal killsignal after timeout is exceeded. Maxbuffer is used to limit the maximum stdout and stderr sizes that can be received. |
| wait()： Promise\<number> | is used to wait for the child process to run and return the promise object, whose value is the exit code of the child process. |
| getOutput(): Promise\<Uint8Array> |  used to get the standard output of the child process. |
| getErrorOutput(): Promise\<Uint8Array> | used to get the standard error output of the child process. |
| const tid:number | Returns the TID of the process. |
| getStartRealtime() :number | Gets the real time elapsed (in milliseconds) from system startup to process startup. |
| getAvailableCores() :number[] | Gets the CPU kernel available to the current process on the multi-core device. |
| getPastCputime() :number | Gets the CPU time (in milliseconds) from the start of the process to the current time. |
| isIsolatedProcess(): boolean | Check if the process is quarantined. |
| is64Bit(): boolean | Check whether the process is running in a 64 bit environment. |
| isAppUid(v:number): boolean | Checks whether the specified uid belongs to a specific application. |
| getUidForName(v:string): number | Obtain the user group ID to which the user belongs according to the user name |
| getThreadPriority(v:number): number | Gets the thread priority based on the specified TID. |
| getSystemConfig(name:number): number | Gets the configuration of the system according to the specified system configuration name. |
| getEnvironmentVar(name:string): string | Obtain the corresponding value according to the name of the environment variable. |
| close(): void | used to close the running child process. |
| kill(signal: number \| string): void |  used to send signals to child processes. |
| readonly killed: boolean | indicates whether the signal is sent successfully, and true indicates that the signal is sent successfully. |
| readonly exitCode: number | indicates the exit code of the child process. |
| readonly pid: number | represents the child process ID. |
| readonly ppid: number | represents the main process ID. |

### Interface instructions

Example of using interface：
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
## Related warehouse
[js_sys_module](base/compileruntime/js_sys_module/readme.md)

### License

SYS is available under [Mozilla license](https://www.mozilla.org/en-US/MPL/), and the documentation is detailed in [documentation](https://gitee.com/openharmony/js_sys_module/blob/master/mozilla_docs.txt). See [LICENSE](https://gitee.com/openharmony/js_sys_module/blob/master/LICENSE) for the full license text.