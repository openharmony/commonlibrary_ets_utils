# ets_utils 代码地图

## 仓库定位

本仓库对应 OpenHarmony `commonlibrary/ets_utils`，是 **公共基础库子系统的语言工具集**，为整个 OpenHarmony 应用生态提供标准 JavaScript/TypeScript 语言级 API。

- **子系统**：commonlibrary
- **部件名**：ets_utils
- **SysCap**：SystemCapability.Utils.Lang
- **ROM**：1400KB | **RAM**：~4096KB
- **适用系统类型**：standard

---

## 顶层目录职责

```
commonlibrary_ets_utils/
├── base_sdk/              # 基础SDK：BusinessError 定义、Kit 注册与静态/动态分发
│   ├── ets/               #   @ohos.base.ets — BusinessError<T> 基类
│   └── transfer/          #   @ohos.transfer.ets — Kit 级方法分发 + 22 个 Kit 注册文件（另有 registerMain.ets）
├── js_api_module/         # 标准 API 模块（6个子模块）
│   ├── buffer/            #   Buffer / Blob — 二进制数据处理
│   ├── fastbuffer/        #   FastBuffer — 高性能二进制操作
│   ├── uri/               #   URI — RFC 3986 URI 解析与操作
│   ├── url/               #   URL / URLSearchParams — WHATWG URL 标准
│   ├── convertxml/        #   ConvertXML — XML 转 JS 对象
│   └── xml/               #   XmlPullParser / XmlSerializer — XML 解析与序列化
├── js_concurrent_module/  # 并发模块（3个子模块 + 共享工具）
│   ├── taskpool/          #   TaskPool — 线程池并行任务执行
│   ├── worker/            #   Worker — Web Worker API 多线程
│   ├── utils/             #   并发工具：locks, ASON, ConditionVariable, SendableLruCache
│   └── common/helper/     #   共享辅助：NAPI 工具、并发辅助、异步栈、错误码
├── js_sys_module/         # 系统模块（4个子模块）
│   ├── console/           #   Console — Web Console API 日志
│   ├── process/           #   Process — 系统进程操作
│   ├── timer/             #   Timer — setTimeout / setInterval
│   └── dfx/               #   DFX — 诊断维护
├── js_util_module/        # 工具模块（5个子模块）
│   ├── util/              #   TextEncoder/Decoder, Base64, UUID, LRUCache, RationalNumber, Scope, Aspect, promisify
│   ├── container/         #   15 种集合容器（ArrayList, HashMap, TreeMap, etc.）
│   ├── collections/       #   Sendable 集合（SendableArray/Set/Map, TypedArrays）— NAPI 桥接层
│   ├── json/              #   JSON 增强：parse(BigInt 三模式)、stringify(循环引用检测)、has()、remove()
│   └── stream/            #   流框架（Readable/Writable/Duplex/Transform）
├── platform/              # 跨平台适配层（4 个平台子目录 + 公共头文件；Windows/Linux/Mac 等 ArkUI-X 平台经 default/ + WINDOWS_PLATFORM 等平台宏支持）
│   ├── ohos/              #   OpenHarmony 平台适配（process_helper、qos_helper、util_helper）
│   ├── android/           #   Android 平台适配（jni_helper）
│   ├── ios/               #   iOS 平台适配（process_helper）
│   ├── default/           #   默认实现（jni/process/qos/util_helper），其余平台复用
│   └── *.h                #   公共头文件（jni_helper.h、process_helper.h、qos_helper.h、util_helper.h、utils.h）
├── tools/                 # 公共工具
│   ├── ets_error.h        #   全局 BusinessError 构造工具
│   ├── log.h              #   HILOG 宏定义（domain/tag 声明）
│   └── common_helper.h    #   ApiHelper 版本判断与公共 NAPI 辅助
└── bundle.json            # 部件元数据
```

---

## 任务到路径映射

| 任务类型 | 先看这里 | 原因 |
|----------|----------|------|
| **Buffer / 二进制数据处理** | `js_api_module/buffer/` | Buffer 和 Blob 的完整实现 |
| **高性能二进制操作** | `js_api_module/fastbuffer/` | FastBuffer，运行时内部实现桥接 |
| **URI 解析与操作** | `js_api_module/uri/` | RFC 3986 URI 类 |
| **URL 解析（WHATWG）** | `js_api_module/url/` | WHATWG URL 标准 + URLSearchParams |
| **XML 转 JS 对象** | `js_api_module/convertxml/` | XML 反序列化 |
| **XML 解析/序列化** | `js_api_module/xml/` | XmlPullParser / XmlSerializer |
| **TaskPool 并行任务** | `js_concurrent_module/taskpool/` | 线程池任务执行引擎 |
| **Worker 多线程** | `js_concurrent_module/worker/` | Web Worker 实现 |
| **并发工具（锁/ASON/条件变量）** | `js_concurrent_module/utils/` | 并发同步原语 |
| **Console 日志** | `js_sys_module/console/` | Web Console API |
| **Process 进程操作** | `js_sys_module/process/` | PID/kill/环境变量/子进程 |
| **Timer 定时器** | `js_sys_module/timer/` | setTimeout/setInterval |
| **TextEncoder/Decoder, Base64** | `js_util_module/util/` | 编解码与工具函数 |
| **集合容器** | `js_util_module/container/` | 15 种数据结构 |
| **Sendable 集合** | `js_util_module/collections/` | 并发安全集合桥接层 |
| **JSON 解析/序列化** | `js_util_module/json/` | 增强版 JSON：parse(BigInt 三模式)、stringify(循环引用检测)、has()、remove() |
| **流框架** | `js_util_module/stream/` | Readable/Writable/Duplex/Transform |
| **BusinessError / Kit 注册** | `base_sdk/` | 全局错误基类与 Kit 分发机制 |
| **跨平台适配** | `platform/` | 平台特定实现 |
| **公共 NAPI 工具/错误码** | `tools/` | HILOG 宏、版本判断、错误抛出 |

---

## 高风险路径

| 路径 | 风险说明 |
|------|---------|
| `js_concurrent_module/taskpool/` | 并发执行核心，线程安全敏感，修改可能引起竞态条件 |
| `js_concurrent_module/worker/` | 多线程消息传递，生命周期管理复杂 |
| `js_concurrent_module/utils/locks/` | 死锁检测与异步锁，逻辑复杂度高 |
| `js_api_module/buffer/` | 内存操作，越界访问风险高 |
| `js_api_module/fastbuffer/` | 依赖运行时内部实现，修改需谨慎 |
| `js_sys_module/process/` | 进程级操作（kill/exit），安全敏感 |
| `base_sdk/transfer/` | Kit 注册与分发，影响全局启动链路 |
| `js_api_module/xml/` + `js_api_module/convertxml/` | 依赖 libxml2，内存安全需注意 |
| `js_util_module/util/`（hispeed_string_plugin.c） | hispeed_string_plugin C 代码，性能敏感 |

---

## 高频变更路径

以下目录提交活动频繁，修改时需特别注意影响范围：

| 路径 | 说明 |
|------|------|
| `js_util_module/util/src/` | 最大的单模块 TS 实现目录，API 方法数最多，变更频率高 |
| `js_concurrent_module/common/helper/` | TaskPool + Worker 共享辅助工具，改动同时影响两个模块 |
| `js_api_module/buffer/` | Buffer/Blob 高频使用，边界检查和内存安全敏感 |
| `js_util_module/json/` | JSON 增强功能（BigInt/循环引用/has/remove），API 调整频繁 |

---

## Inner Kit 公共 C/C++ 头文件

| Inner Kit | 头文件 | 位置 |
|-----------|--------|------|
| timer | `sys_timer.h` | `js_sys_module/timer/` |
| console | `console.h`, `log.h` | `js_sys_module/console/` |
| worker | `worker.h` | `js_concurrent_module/worker/` |

---

## 构建产物流向

```
.ts 源码 ──[build_ts_js.py]──> .js ──[es2abc]──> .abc ──[gen_obj]──> .o ──> 共享库
                                                    │
                                              .js ──[gen_obj]──> .o ──> 共享库
```

- `base_sdk/` 编译产物：`base_sdk.abc` 和 `base_transfer.abc` → 安装到 `/system/framework/`（启动 ABC）
- `collections/` 安装目录：`module/arkts`
- `json/`, `stream/` 安装目录：`module/util`
- 其他模块安装目录：各模块默认路径
