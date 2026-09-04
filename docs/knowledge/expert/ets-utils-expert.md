# ets_utils 专家经验

## 禁止事项

### 全局禁止

- **禁止** 删除失败的测试用例来使用例"通过"
- **禁止** 未明确用户请求时提交代码
- **禁止** 对已有公共 API 做不兼容变更（API 废弃需走正式流程）
- **禁止** 绕过 BusinessError 错误码规范，直接抛出非标准错误
- **禁止** 在 NAPI 模块中使用原始裸指针管理内存，必须使用智能指针或 NAPI 引用

### Buffer / FastBuffer 禁止

- **禁止** 绕过边界检查（bounds checking），即使性能优化也不能跳过
- **禁止** 暴露原始内存指针到 JS 层
- **禁止** Buffer 模块中引入网络 I/O 或文件系统 I/O
- **禁止** 修改 Buffer API 的已有签名（Node.js 兼容性要求）

### XML 禁止

- **禁止** 绕过 libxml2 使用自定义 XML 解析器
- **禁止** 添加 XPath / XQuery / XSLT / Schema 验证支持
- **禁止** 在 XmlSerializer 中使用非 UTF-8 编码
- **禁止** ConvertXML 模块中实现 JS 对象到 XML 的序列化
- **禁止** 在 XML 模块中引入网络或文件 I/O

### 并发模块禁止

- **禁止** 在 TaskPool 主线程回调中执行阻塞操作
- **禁止** Worker 线程中直接操作 UI（Worker 运行在独立上下文）
- **禁止** 跨 Worker 直接共享可变状态（必须通过消息传递或 Sendable 对象）
- **禁止** 修改 locks 模块的死锁检测图算法，除非充分理解 DAG 检测逻辑

### Collections 桥接层禁止

- **禁止** 在 `js_util_module/collections/` 中实现集合逻辑——它是纯 NAPI 桥接层，所有集合实现位于 ArkTS 运行时
- **禁止** 添加自定义 Sendable 集合类型

### JSON 禁止

- **禁止** 在 JSON 模块中实现异步解析/序列化——所有 JSON 操作必须保持同步
- **禁止** 自行实现 JSON 解析器——必须基于运行时内置的 `JSON.parseBigInt` 和 `JSON.stringifyBigInt`
- **禁止** 在 `has()` 或 `remove()` 中接受数组参数——这两个方法只适用于纯 JSON 对象
- **禁止** 在 JSON 模块中引入跨模块依赖——保持模块独立性
- **禁止** 在 JSON 模块中执行网络 I/O 或文件系统 I/O

### Console 禁止

- **禁止** 修改 HILOG domain（0x3D00）和默认 tag（"JSAPP"）

### Process 禁止

- **禁止** 在非特权进程中调用 kill/exit 而不做权限检查
- **禁止** 修改子进程（ChildProcess）的安全边界

---

## 需确认后再修改

以下高风险路径的修改需经用户确认后方可进行：

- **Inner Kit 头文件**（`sys_timer.h`、`console.h`/`log.h`、`worker.h`）签名变更 → 确认下游消费方已评估
- **base_sdk/transfer/** Kit 注册映射变更 → 确认 Kit 注册链路无断裂
- **js_concurrent_module/common/helper/** → 确认 TaskPool + Worker 均已验证
- **platform/** 平台适配宏变更 → 确认所有目标平台编译通过
- **构建产物**（`.abc`、gen_obj `.o`/`.c`）→ 修改源码后重新生成，禁止直接编辑产物

## Agent 常见错误模式

| 模式 | 风险 | 正确做法 |
|------|------|---------|
| 添加 BUILD.gn 未声明的依赖 | 编译通过但模块耦合度增加 | 新增依赖须同步更新 BUILD.gn deps |
| 在 TS 层调用 NAPI 内部函数 | 分层违规 | TS 层只通过 Public API 调用，NAPI 绑定在 C++ 层 |
| 直接编辑 .abc/.o 构建产物 | 源码与产物不一致 | 始终修改 .ts/.cpp 源码并重新构建 |
| 幻觉不存在的 NAPI 辅助函数 | 编译失败或运行时崩溃 | 调用 NAPI 函数前确认其存在于头文件中 |

---

## 架构不变量

1. **模块独立性**：每个子模块（buffer, uri, url, xml, etc.）是独立可编译的构建单元，修改一个模块不应影响其它模块的编译
2. **NAPI 桥接分层**：TypeScript 层定义 Public API，C++ 层通过 NAPI 实现高性能路径，两层之间不交叉调用
3. **ArkPrivate 快速路径**：container 和 fastbuffer 模块优先使用 `ArkPrivate.Load()` 获取运行时内置实现，仅在没有内置实现时回退到 TS 实现
4. **Inner Kit 稳定性**：`sys_timer.h`、`console.h`/`log.h`、`worker.h` 是对外暴露的 C/C++ 头文件，其签名不得做不兼容变更
5. **跨平台隔离**：平台相关代码统一放在 `platform/` 目录，通过编译宏（`OHOS_PLATFORM`、`ANDROID_PLATFORM` 等）条件编译，主逻辑代码不得直接调用平台 API
6. **base_sdk 启动顺序**：`base_sdk.abc` 和 `base_transfer.abc` 是系统启动 ABC，Kit 注册映射表变更需确认不影响启动链路
7. **JSON 包装器模式**：JSON 模块是对运行时内置 `JSON.parseBigInt`/`JSON.stringifyBigInt` 的增强包装，不是独立实现；核心解析/序列化能力由运行时提供，本模块仅增加参数校验、BigInt 模式切换、循环引用检测和 has/remove 工具方法

---

## 常见失败模式

| 模式 | 涉及模块 | 典型表现 | 正确做法 |
|------|---------|---------|---------|
| 跨线程共享非 Sendable 对象 | TaskPool / Worker | 运行时崩溃或数据不一致 | 使用 Sendable 对象或结构化克隆传递 |
| Buffer 越界读写 | buffer / fastbuffer | 10200001 / 10200013 错误码 | 所有读写操作前检查 offset + length |
| XML 编码非 UTF-8 | xml | 10200066 错误码 | 统一使用 UTF-8 编码 |
| ConvertXML 大文档内存溢出 | convertxml | OOM | 使用 `largeConvertToJSObject()` 而非 `convertToJSObject()` |
| Worker 线程泄漏 | worker | 线程不释放 | 确保 terminate() 被调用，处理 TERMINATING 状态 |
| 异步锁死锁 | locks | 永久等待 | 遵循 DAG 锁获取顺序，使用死锁检测 |
| Timer 回调在错误线程 | timer | 回调不执行 | Timer 绑定到创建时的 libuv event loop |
| Base64 编解码模式混用 | util | 数据损坏 | 区分 BASIC / MIME / URL_SAFE 模式 |
| Stream 背压未处理 | stream | 内存溢出 | Readable 需检查 doRead 返回值，Writable 需尊重 drain 事件 |
| FastBuffer 依赖运行时版本 | fastbuffer | `ArkPrivate.Load` 失败 | 确认运行时已内置 FastBuffer 实现 |
| JSON 循环引用序列化 | json | BusinessError "circular Reference" | 序列化前检测循环引用，或使用 reviver 过滤 |
| JSON BigInt 精度丢失 | json | 大整数解析为 Number 精度丢失 | 使用 `PARSE_AS_BIGINT` 或 `ALWAYS_PARSE_AS_BIGINT` 模式 |
| JSON has/remove 传入数组 | json | BusinessError 401 "must be json object" | has/remove 只接受纯对象，不接受数组 |
| JSON parse 传入非字符串 | json | BusinessError 401 | 确保第一个参数为 string 类型 |

---

## 关键约束

### 错误码规范

| 错误码 | 含义 | 使用模块 |
|--------|------|---------|
| 401 | 参数错误 | 所有模块 |
| 10200001 | Buffer 越界 | buffer, fastbuffer |
| 10200002 | 语法错误 | util |
| 10200009 | Buffer 分配失败 | buffer, fastbuffer |
| 10200013 | Buffer 越界 | buffer, fastbuffer |
| 10200035 | Stream 不可读 | stream |
| 10200036 | Stream 不可写 | stream |
| 10200037 | Stream 不支持操作 | stream |
| 10200064 | XML 空参数 | xml |
| 10200066 | XML 编码错误 | xml |
| 10200068 | FastBuffer 无效操作 | fastbuffer |

### 依赖关键约束

| 依赖 | 约束 |
|------|------|
| `libxml2` | XML 和 ConvertXML 模块共用，版本升级需同时验证两个模块 |
| `icu` | 多个模块使用 `shared_icuuc`，ICU 版本升级需全量回归 |
| `openssl` | 仅 util 模块使用 `libcrypto_shared`（UUID 生成），不要在其它模块引入 openssl |
| `ffrt` | TaskPool 依赖 ffrt 调度，ffrt 行为变更可能影响 TaskPool 的任务执行顺序 |
| `runtime_core` | TaskPool/Worker 依赖 Ark 运行时（ani、libarkruntime），需与运行时版本保持一致 |

### 构建特征开关

- `ets_utils_stacksize_low_enable`：控制是否启用低栈大小模式（适用于资源受限设备）

---

## 性能注意事项

1. **TaskPool/Worker**：不要在高频回调中创建/销毁 TaskPool 任务或 Worker 线程，复用已有实例
2. **Buffer**：避免频繁创建小 Buffer，利用 `Buffer.alloc()` 池化机制
3. **Container**：HashMap/HashSet 的 `DictionaryClass` 继承自 util.struct，修改 struct 会影响所有容器
4. **Console**：高频日志路径（debug/info）避免格式化大对象，%j 占位符会触发 JSON.stringify
5. **JSON**：
   - BigInt 模式（`PARSE_AS_BIGINT`/`ALWAYS_PARSE_AS_BIGINT`）比默认模式慢，仅在需要时启用
   - `stringify()` 对含循环引用的对象会抛出 BusinessError，序列化前应确保对象无环或使用 reviver 过滤
   - `has()` 和 `remove()` 内部使用 `Object.prototype.hasOwnProperty.call()`，不会遍历原型链
   - JSON 模块安装到 `module/util` 目录（与 util 模块共享安装路径），模块名为 `util.json`
6. **Stream**：Transform 流的 doTransform() 必须是同步或返回 Promise，阻塞会导致背压
