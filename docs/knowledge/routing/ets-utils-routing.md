# ets_utils 知识路由

## 场景触发路由

改动前按场景读取对应文件：

| 场景 | 先读 | 补充 |
|------|------|------|
| 二进制数据处理（Buffer/Blob/FastBuffer） | `js_api_module/buffer/CLAUDE.md` | `js_api_module/fastbuffer/CLAUDE.md` |
| URI/URL 解析 | `js_api_module/uri/CLAUDE.md` | `js_api_module/url/CLAUDE.md` |
| XML 相关（解析/序列化/转换） | `js_api_module/xml/CLAUDE.md` | `js_api_module/convertxml/CLAUDE.md` |
| 并发编程（TaskPool/Worker/锁） | `js_concurrent_module/taskpool/CLAUDE.md` | `js_concurrent_module/worker/CLAUDE.md`、`js_concurrent_module/utils/CLAUDE.md` |
| 日志与调试（Console/Timer） | `js_sys_module/console/CLAUDE.md` | `js_sys_module/timer/CLAUDE.md` |
| 进程管理 | `js_sys_module/process/CLAUDE.md` | — |
| 编解码（TextEncoder/Base64/UUID） | `js_util_module/util/CLAUDE.md` | — |
| 集合容器 | `js_util_module/container/CLAUDE.md` | — |
| Sendable 集合（并发安全） | `js_util_module/collections/CLAUDE.md` | — |
| JSON 处理（BigInt、循环引用、has/remove） | `js_util_module/json/CLAUDE.md` | — |
| 流式处理（Readable/Writable） | `js_util_module/stream/CLAUDE.md` | `js_util_module/util/CLAUDE.md`（TextEncoder/StringDecoder 依赖） |
| 错误类型定义 / Kit 注册 | `base_sdk/ets/@ohos.base.ets` | `base_sdk/transfer/` |
| 跨平台适配 | `platform/` | — |

---

## 词汇触发路由

| 术语/缩写 | 含义 | 指向 |
|-----------|------|------|
| **Buffer** | 二进制数据缓冲区 | `js_api_module/buffer/` |
| **Blob** | 二进制大对象 | `js_api_module/buffer/` |
| **FastBuffer** | 高性能缓冲区 | `js_api_module/fastbuffer/` |
| **URI** | RFC 3986 统一资源标识符 | `js_api_module/uri/` |
| **URL** | WHATWG 统一资源定位符 | `js_api_module/url/` |
| **URLSearchParams** | URL 查询参数 | `js_api_module/url/` |
| **XmlPullParser** | XML 事件驱动解析器 | `js_api_module/xml/` |
| **XmlSerializer** | XML 序列化器 | `js_api_module/xml/` |
| **ConvertXML** | XML 转 JS 对象 | `js_api_module/convertxml/` |
| **TaskPool** | 线程池并行任务 | `js_concurrent_module/taskpool/` |
| **Worker** | Web Worker 多线程 | `js_concurrent_module/worker/` |
| **TaskGroup** | 任务组 | `js_concurrent_module/taskpool/` |
| **SequenceRunner** | 顺序执行器 | `js_concurrent_module/taskpool/` |
| **AsyncRunner** | 异步执行器 | `js_concurrent_module/taskpool/` |
| **Sendable** | 可跨线程传递的共享对象 | `js_util_module/collections/`、`js_concurrent_module/utils/` |
| **ASON** | ArkTS Sendable JSON | `js_concurrent_module/utils/` |
| **locks** | 异步锁（AsyncLock） | `js_concurrent_module/utils/locks/` |
| **ConditionVariable** | 条件变量 | `js_concurrent_module/utils/condition/` |
| **Console** | Web Console 日志 | `js_sys_module/console/` |
| **Timer** | setTimeout/setInterval | `js_sys_module/timer/` |
| **Process** | 系统进程 API | `js_sys_module/process/` |
| **TextEncoder / TextDecoder** | 文本编解码 | `js_util_module/util/` |
| **Base64** | Base64 编解码 | `js_util_module/util/` |
| **LRUCache** | 最近最少使用缓存 | `js_util_module/util/` |
| **RationalNumber** | 有理数 | `js_util_module/util/` |
| **Scope** | 范围判断 | `js_util_module/util/` |
| **Aspect** | AOP 切面 | `js_util_module/util/` |
| **promisify / callbackWrapper** | 异步风格转换 | `js_util_module/util/` |
| **ArrayList / HashMap / TreeMap** | 集合容器 | `js_util_module/container/` |
| **SendableArray / SendableMap / SendableSet** | Sendable 集合 | `js_util_module/collections/` |
| **JSON.parse / JSON.stringify** | JSON 解析/序列化（增强版） | `js_util_module/json/` |
| **parseBigInt / stringifyBigInt** | BigInt 模式 JSON 解析/序列化 | `js_util_module/json/` |
| **BigIntMode** | BigInt 解析模式（DEFAULT/PARSE_AS_BIGINT/ALWAYS_PARSE_AS_BIGINT） | `js_util_module/json/` |
| **JSON.has / JSON.remove** | JSON 对象键检查与删除 | `js_util_module/json/` |
| **circular reference** | 循环引用检测（JSON.stringify） | `js_util_module/json/` |
| **Readable / Writable / Duplex / Transform** | 流类型 | `js_util_module/stream/` |
| **BusinessError** | 业务错误基类 | `base_sdk/ets/@ohos.base.ets` |
| **NAPI** | Node-API 原生绑定 | `tools/` + 各模块 native 代码 |
| **libxml2** | XML 解析 C 库 | `js_api_module/xml/`、`js_api_module/convertxml/` |
| **ffrt** | 华为函数式运行时 | `js_concurrent_module/taskpool/`（依赖） |
| **ArkPrivate** | 运行时内部 API 加载器 | `js_api_module/fastbuffer/`、`js_util_module/container/` |

---

## 路径触发路由

当看到以下路径模式时，应加载的知识：

| 路径模式 | 加载 |
|---------|------|
| `js_api_module/*/src/js_*.ts` | 对应模块的 CLAUDE.md（Public API 定义） |
| `js_api_module/*/native_module_*.cpp` | 对应模块的 CLAUDE.md（NAPI 原生绑定） |
| `js_concurrent_module/common/helper/` | 并发模块共享辅助（error_helper.h 含 15KB 错误码定义） |
| `js_concurrent_module/utils/locks/` | 死锁检测与异步锁实现 |
| `js_sys_module/timer/sys_timer.h` | Inner Kit 公共头文件 |
| `js_sys_module/console/console.h` | Inner Kit 公共头文件（HILOG domain 0x3D00） |
| `js_concurrent_module/worker/worker.h` | Inner Kit 公共头文件 |
| `platform/*/process_helper.cpp` | 平台特定进程实现 |
| `platform/*/util_helper.cpp` | 平台特定编解码实现 |
| `base_sdk/transfer/kitRegister/` | Kit 注册回调映射 |
| `js_util_module/json/src/json_js.ts` | JSON 模块 Public API（parse/stringify/has/remove 定义） |
| `js_util_module/json/native_module_json.cpp` | JSON NAPI 模块注册 |
| `tools/ets_error.h` | 全局 BusinessError 构造工具，修改需确认所有模块错误码格式兼容 |
| `tools/hilog/` | HILOG 宏定义，修改需确认 Console 模块 domain/tag 约束 |
| `tools/api_helper/` | ApiHelper 版本判断与平台适配，修改需确认跨模块版本逻辑一致性 |
