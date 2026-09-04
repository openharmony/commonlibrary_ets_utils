# AGENTS.md — commonlibrary_ets_utils

> OpenHarmony 公共基础库子系统 · 语言工具集部件（SystemCapability.Utils.Lang）

---

## 项目定位

本仓库对应 OpenHarmony 源码树 `commonlibrary/ets_utils`，为整个应用生态提供标准 JavaScript/TypeScript 语言级 API，涵盖二进制处理、URI/URL、XML、并发、系统工具、集合容器、流框架等能力。

- **子系统**：commonlibrary
- **部件名**：ets_utils
- **SysCap**：SystemCapability.Utils.Lang
- **ROM**：1400KB | **RAM**：~4096KB

---

## 代码地图

本仓库按功能域划分为 4 大模块目录，优先按这些目录定位问题：

```
commonlibrary_ets_utils/
├── base_sdk/              # BusinessError 定义 + Kit 注册与静态/动态分发
├── js_api_module/         # 标准 API：buffer, fastbuffer, uri, url, convertxml, xml
├── js_concurrent_module/  # 并发：taskpool, worker, utils(locks/ASON/条件变量)
├── js_sys_module/         # 系统：console, process, timer, dfx
├── js_util_module/        # 工具：util, container, collections, json, stream
├── platform/              # 跨平台适配（OHOS/Android/iOS/Windows/Linux/Mac）→ 详见 code_map
└── tools/                 # 公共 NAPI 工具、HILOG 宏、ErrorHelper → 详见 code_map
```

### 任务到路径映射

| 任务类型 | 先看这里 | 原因 |
|----------|----------|------|
| Buffer / Blob / 二进制 | `js_api_module/buffer/` | Buffer 和 Blob 完整实现 |
| 高性能二进制 | `js_api_module/fastbuffer/` | 运行时内置实现桥接 |
| URI（RFC 3986） | `js_api_module/uri/` | URI 解析与操作 |
| URL（WHATWG） | `js_api_module/url/` | URL 标准 + URLSearchParams |
| XML 转 JS 对象 | `js_api_module/convertxml/` | XML 反序列化（libxml2） |
| XML 解析/序列化 | `js_api_module/xml/` | XmlPullParser/XmlSerializer |
| TaskPool 并行 | `js_concurrent_module/taskpool/` | 线程池任务引擎 |
| Worker 多线程 | `js_concurrent_module/worker/` | Web Worker 实现 |
| 并发工具（锁/ASON/条件变量） | `js_concurrent_module/utils/` | 并发同步原语 |
| Console 日志 | `js_sys_module/console/` | Web Console API |
| Process 进程操作 | `js_sys_module/process/` | PID/kill/环境变量/子进程 |
| Timer 定时器 | `js_sys_module/timer/` | setTimeout/setInterval |
| TextEncoder/Base64/UUID | `js_util_module/util/` | 编解码与工具函数 |
| 集合容器（15种） | `js_util_module/container/` | ArrayList/HashMap/TreeMap 等 |
| Sendable 集合 | `js_util_module/collections/` | 并发安全集合（NAPI 桥接层） |
| JSON 解析/序列化 | `js_util_module/json/` | 增强版 JSON（BigInt、循环引用检测、has/remove） |
| 流框架 | `js_util_module/stream/` | Readable/Writable/Duplex/Transform |

详细代码地图：`docs/knowledge/code_map/ets-utils-code-map.md`

---

## 知识索引

稳定背景知识放在 `docs/knowledge/`。改动前按场景读取对应文件：

| 场景 | 先读 |
|------|------|
| 二进制数据处理 | `js_api_module/buffer/CLAUDE.md` → `js_api_module/fastbuffer/CLAUDE.md` |
| URI/URL 解析 | `js_api_module/uri/CLAUDE.md` → `js_api_module/url/CLAUDE.md` |
| XML 相关 | `js_api_module/xml/CLAUDE.md` → `js_api_module/convertxml/CLAUDE.md` |
| 并发编程 | `js_concurrent_module/taskpool/CLAUDE.md` → `js_concurrent_module/worker/CLAUDE.md` |
| 日志与定时器 | `js_sys_module/console/CLAUDE.md` → `js_sys_module/timer/CLAUDE.md` |
| 进程管理 | `js_sys_module/process/CLAUDE.md` |
| 诊断维护（DFX/HiTrace/HiCollie） | `js_sys_module/dfx/` |
| 编解码工具 | `js_util_module/util/CLAUDE.md` |
| 集合容器 | `js_util_module/container/CLAUDE.md` |
| Sendable 集合 | `js_util_module/collections/CLAUDE.md` |
| JSON 解析/序列化（BigInt、循环引用、has/remove） | `js_util_module/json/CLAUDE.md` |
| 流框架 | `js_util_module/stream/CLAUDE.md` |

### 词汇触发路由

| 术语 | 指向 |
|------|------|
| Buffer / Blob | `js_api_module/buffer/` |
| FastBuffer | `js_api_module/fastbuffer/` |
| URI | `js_api_module/uri/` |
| URL / URLSearchParams | `js_api_module/url/` |
| XmlPullParser / XmlSerializer | `js_api_module/xml/` |
| ConvertXML | `js_api_module/convertxml/` |
| TaskPool / TaskGroup | `js_concurrent_module/taskpool/` |
| Worker | `js_concurrent_module/worker/` |
| Sendable / ASON / locks | `js_concurrent_module/utils/` |
| Console | `js_sys_module/console/` |
| Timer (setTimeout/setInterval) | `js_sys_module/timer/` |
| Process / ChildProcess | `js_sys_module/process/` |
| DFX / HiTrace / HiCollie | `js_sys_module/dfx/` |
| TextEncoder / Base64 / UUID / LRUCache / promisify | `js_util_module/util/` |
| ArrayList / HashMap / TreeMap / Vector / Queue | `js_util_module/container/` |
| SendableArray / SendableMap / SendableSet | `js_util_module/collections/` |
| JSON.parse / JSON.stringify / parseBigInt / has / remove | `js_util_module/json/` |
| Readable / Writable / Duplex / Transform | `js_util_module/stream/` |
| BusinessError | `base_sdk/ets/@ohos.base.ets` |
| ArkPrivate | `js_api_module/fastbuffer/`、`js_util_module/container/` |

详细路由规则：`docs/knowledge/routing/ets-utils-routing.md`

### 编辑前报告（强制）

改动代码前，必须在回复中声明：

1. **任务类别**（引用上表场景编号或术语）
2. **已读取的知识文档**（CLAUDE.md / docs/knowledge/ 文档路径）
3. **已识别的约束条件**（引用禁止事项编号或架构不变量，并简要说明约束内容，例如："禁止事项 #6——不可绕过 Buffer 边界检查"）

未完成以上声明，不得开始编辑代码。

---

## 项目约束

### 禁止事项

1. **禁止** 删除失败的测试用例来使用例"通过"
2. **禁止** 未明确用户请求时提交代码
3. **禁止** 对已有公共 API 做不兼容变更
4. **禁止** 绕过 BusinessError 错误码规范
5. **禁止** Buffer 模块中引入网络 I/O 或文件系统 I/O
6. **禁止** 绕过 Buffer 边界检查（bounds checking）
7. **禁止** 绕过 libxml2 使用自定义 XML 解析器
8. **禁止** 在 XmlSerializer 中使用非 UTF-8 编码
9. **禁止** 在 `js_util_module/collections/` 中实现集合逻辑——它是纯 NAPI 桥接层
10. **禁止** 在 JSON 模块中实现异步解析/序列化——JSON 操作必须保持同步
11. **禁止** 在 JSON 模块中自行实现 JSON 解析器——必须基于运行时内置的 `JSON.parseBigInt`/`JSON.stringifyBigInt`
12. **禁止** 跨 Worker 直接共享可变状态
13. **禁止** 修改 Console HILOG domain（0x3D00）和默认 tag（"JSAPP"）
14. **禁止** 在 Process 模块中未经权限检查调用 kill/exit
15. **禁止** 在 NAPI 模块中使用原始裸指针管理内存——必须使用智能指针或 NAPI 引用

### 需确认后再修改

以下高风险路径的修改需经用户确认后方可进行：

- **Inner Kit 头文件**（`sys_timer.h`、`console.h`/`log.h`、`worker.h`）签名变更 → 确认下游消费方已评估
- **base_sdk/transfer/** Kit 注册映射变更 → 确认 Kit 注册链路无断裂
- **js_concurrent_module/common/helper/** → 确认 TaskPool + Worker 均已验证
- **platform/** 平台适配宏变更 → 确认所有目标平台编译通过
- **构建产物**（`.abc`、gen_obj `.o`/`.c`）→ 修改源码后重新生成，禁止直接编辑产物
- **第三方依赖版本升级**（libxml2、openssl、ICU）→ 确认许可证兼容性及全量回归测试

### Agent 常见错误模式

| 模式 | 风险 | 正确做法 |
|------|------|---------|
| 添加 BUILD.gn 未声明的依赖 | 编译通过但模块耦合度增加 | 新增依赖须同步更新 BUILD.gn deps |
| 在 TS 层调用 NAPI 内部函数 | 分层违规 | TS 层只通过 Public API 调用，NAPI 绑定在 C++ 层 |
| 直接编辑 .abc/.o 构建产物 | 源码与产物不一致 | 始终修改 .ts/.cpp 源码并重新构建 |
| 幻觉不存在的 NAPI 辅助函数 | 编译失败或运行时崩溃 | 调用 NAPI 函数前确认其存在于头文件中 |

### 架构不变量

1. 每个子模块是独立可编译的构建单元
2. TS 层定义 Public API，C++ 层通过 NAPI 实现高性能路径
3. container/fastbuffer 优先使用 `ArkPrivate.Load()` 获取运行时内置实现
4. Inner Kit 头文件（`sys_timer.h`、`console.h`/`log.h`、`worker.h`）签名不得做不兼容变更
5. 平台相关代码统一在 `platform/` 目录，主逻辑不得直接调用平台 API

### 常见失败模式

| 模式 | 典型表现 | 正确做法 |
|------|---------|---------|
| 跨线程共享非 Sendable 对象 | 运行时崩溃 | 使用 Sendable 对象或结构化克隆 |
| Buffer 越界读写 | 10200001/10200013 错误 | 读写前检查 offset + length |
| XML 非UTF-8 编码 | 10200066 错误 | 统一使用 UTF-8 |
| ConvertXML 大文档 OOM | 内存溢出 | 使用 `largeConvertToJSObject()` |
| Worker 线程泄漏 | 线程不释放 | 确保 terminate() 被调用 |
| 异步锁死锁 | 永久等待 | 遵循 DAG 锁获取顺序 |
| Stream 背压未处理 | 内存溢出 | 检查 doRead 返回值，尊重 drain 事件 |
| JSON 循环引用序列化 | BusinessError "circular Reference" | 序列化前检测循环引用，或使用 reviver 过滤 |
| JSON BigInt 模式误用 | 大数精度丢失 | 需要大整数时使用 `PARSE_AS_BIGINT` 或 `ALWAYS_PARSE_AS_BIGINT` |
| JSON has/remove 传入数组 | BusinessError 401 | has/remove 只接受纯对象，不接受数组 |

详细专家经验：`docs/knowledge/expert/ets-utils-expert.md`

---

## 构建和验证

所有构建命令从 OpenHarmony 源码根目录执行。

### 单模块构建

```sh
./build.sh --product-name rk3568 --build-target <module>
# 例：./build.sh --product-name rk3568 --build-target buffer --ccache
```

### 单元测试

```sh
./build.sh --product-name rk3568 --build-target test_<module>_unittest
hdc file send ./out/rk3568/tests/unittest/ets_utils/<path>/test_<module>_unittest /data/local/tmp/
hdc shell "cd /data/local/tmp && chmod 777 test_<module>_unittest && ./test_<module>_unittest"
```

### 最小验证顺序

1. 编译验证 → 2. 单元测试 → 3. API 兼容性检查 → 4. 逐项确认 Done 定义

### Done 定义

任务完成必须满足：

1. ✅ 所有修改的文件无新增编译错误
2. ✅ 相关模块构建成功
3. ✅ 相关测试用例通过
4. ✅ 未引入公共 API 不兼容变更
5. ✅ 未违反任何禁止事项
6. ✅ NAPI 原生代码的内存管理正确

### API 兼容性检查

- 检查修改的 .ts 文件中 export 的函数/类/接口签名是否变更
- 如有签名变更，确认已走 API 废弃流程
- 检查方法参数数量、类型、返回值是否与已有 API 一致
- SDK API 检查：`./build.sh --product-name rk3568 --build-target sdk_test`
- NAPI 接口一致性：对比 `@ohos.*.d.ts` 声明与 NAPI 注册函数列表

### 最终回复格式

任务完成时，回复必须包含：

1. **修改文件列表**（含完整路径）
2. **执行的构建/测试命令及结果**
3. **Done 定义各项的逐项确认**（✅/❌）

### 无构建环境时

如果无法运行构建/测试，必须：

1. 语法检查修改文件（`tsc --noEmit` 或 `clang -fsyntax-only`）
2. 在回复中明确标注"未验证构建"
3. 列出需要人工验证的项

### 静态分析

- C++ 代码已通过构建系统启用 CFI 和分支保护（pac_ret）
- TypeScript 代码通过 tsconfig.json 检查
- 如需额外检查：`./build.sh --product-name rk3568 --build-target ets_utils --gn-args="is_clang_check=true"`

详细验证方法：`docs/knowledge/verify/ets-utils-verify.md`
