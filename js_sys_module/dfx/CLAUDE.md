# AGENTS
**Name**: DFX
**Purpose**: DFX is a diagnostics and fault analysis module for ArkTS ecosystem that provides JavaScript APIs for runtime memory inspection, heap snapshot dumping, stack trace capture, and runtime statistics. It enables applications and tools to perform memory profiling and diagnostics with features including:
- Heap snapshot dumping to file (VM mode and app mode)
- Heap tracking with configurable time interval (start/stop)
- Hybrid native + JavaScript stack trace building
- Runtime statistics start/stop/print
- ArrayBuffer total size query
- Heap total and used size query
**Primary Language**: C++ with N-API bindings for JavaScript interoperability.

## Directory Structure
```text
dfx/
├── BUILD.gn                    # Main build configuration (module name: dfx, install dir: module)
└── native_module_dfx.cpp       # NAPI module registration + 10 exported functions
```

## Building
The component uses GN build system with BUILD.gn configuration, run the following command:
```bash
./build.sh --product-name <product> --build-target dfx
```

## Test Suite
No dedicated test directory exists for the dfx module. DFX functionality is verified through integration tests and manual device testing.

## Dependency
### Package Dependencies
| Dependency | Purpose | Platform |
|------------|---------|----------|
| `napi:ace_napi` | N-API bindings for JavaScript engine | All |
| `hilog:libhilog` | System logging | All |
| `icu:shared_icuuc` | Unicode internationalization components | All |
| `libuv:uv` | Event loop utilities | All |
| `node:node_header_notice` | Node-API header notice | All |
| `c_utils:utils` | Common utilities | Non-standard system |

### External Directory Interactions
- `../../tools/log.h` - HILOG logging wrapper
- `../../` - Parent ets_utils module for shared configurations

## Exported API Surface

| Function | Parameters | Returns | Description |
|----------|-----------|---------|-------------|
| `dumpHeapSnapshot` | `path: string, isVmMode: boolean` | `void` | Dump heap snapshot to file path |
| `buildNativeAndJsStackTrace` | — | `string` | Build hybrid native+JS stack trace |
| `startHeapTracking` | `timeInterval: number, isVmMode: boolean` | `boolean` | Start heap tracking |
| `stopHeapTracking` | `filePath: string` | `boolean` | Stop heap tracking, output to file |
| `printStatisticResult` | — | `void` | Print runtime statistics |
| `startRuntimeStat` | — | `void` | Start runtime statistics collection |
| `stopRuntimeStat` | — | `void` | Stop runtime statistics collection |
| `getArrayBufferSize` | — | `number` | Get ArrayBuffer total size |
| `getHeapTotalSize` | — | `number` | Get heap total size |
| `getHeapUsedSize` | — | `number` | Get heap used size |

All functions delegate to `NativeEngine` methods via `reinterpret_cast<NativeEngine*>(env)`.

## Boundaries
### Allowed Operations
- ✅ Add new diagnostic NAPI functions that delegate to `NativeEngine` methods
- ✅ Extend heap snapshot/tracking configuration options
- ✅ Add new runtime statistics metrics
- ✅ Improve error handling and parameter validation
- ✅ Add new stack trace capture modes

### Prohibited Operations
- ❌ Do NOT implement heap analysis logic in this module — all logic delegates to `NativeEngine`; this module is a thin NAPI binding layer
- ❌ Do NOT modify `NativeEngine` API signatures — changes require coordination with the ArkTS runtime team
- ❌ Do NOT break compatibility with existing DFX API contracts (`dumpHeapSnapshot`, `buildNativeAndJsStackTrace`, etc.)
- ❌ Do NOT add file system I/O beyond what `NativeEngine` provides for snapshot/tracking output
- ❌ Do NOT use platform-specific code without proper build guards
- ❌ Do NOT bypass HILOG for error reporting — all errors must be logged via `HILOG_ERROR`
- ❌ Do NOT remove the `NAPI_ASSERT` parameter count checks — they prevent runtime crashes from invalid argument counts
