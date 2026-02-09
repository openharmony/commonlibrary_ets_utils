# AGENTS
**Name**: Timer
**Purpose**: Timer is an asynchronous timing module for ArkTS ecosystem that provides JavaScript timer APIs. It bridges JavaScript timing operations with native system timers using libuv for event-driven execution. It provides features including:
- One-time execution timers (setTimeout)
- Repeating interval timers (setInterval)
- Timer cancellation (clearTimeout, clearInterval)
- Thread-safe callback management with mutex protection
- Environment-based timer isolation
- Memory-safe timer lifecycle management
- Optional HiTrace performance monitoring
- Ace framework container scope support
**Primary Language**: C++17 with N-API bindings for JavaScript interoperability.

## Directory Structure
```text
timer/
├── BUILD.gn                                      # Main build configuration
├── sys_timer.h / sys_timer.cpp                   # Timer class implementation
└── test/                                         # Unit tests subdirectory
    ├── BUILD.gn
    ├── test.h                                     # Test base class
    ├── test_ark.cpp                               # Test main setup
    ├── test_timer.h / test_timer.cpp              # Timer functionality tests
```

## Building
The component uses GN build system with BUILD.gn configuration, run the following command:
```bash
# Build the timer module (within build system)
./build.sh --product-name <product> --build-target timer
```

## Test Suite
To execute code tests, run the following command:
```bash
# Build and generate an executable file
./build.sh --product-name <product> --build-target test_timer_unittest
# Send the executable file to the device using hdc
hdc shell send ./out/<product>/tests/unittest/ets_utils/ets_utils/jssys/timer/napi/test_timer_unittest /data/local/tmp/
# Run the executable file
hdc shell

cd /data/local/tmp/
chmod 777 ./test_timer_unittest
./test_timer_unittest
```

## Dependency
### Package Dependencies
| Dependency | Purpose | Platform |
|------------|---------|----------|
| `napi:ace_napi` | N-API bindings for JavaScript engine | All |
| `hilog:libhilog` | System logging | All |
| `hitrace:hitrace_meter` | Performance tracing (optional) | OHOS (non-arkui_x) |
| `ace_engine:ace_container_scope` | Ace framework container scope | OHOS (optional) |

### External Directory Interactions
- `../../js_concurrent_module/common/helper/napi_helper.h` - Shared NAPI helper utilities for concurrent modules
- `../../` - Parent ets_utils module for shared configurations (ets_utils_config.gni)
- `native_engine` - Native JavaScript engine interface

## Boundaries
### Allowed Operations
- ✅ Add new timer types (e.g., setImmediate functionality)
- ✅ Extend timer configuration options (e.g., priority, precision)
- ✅ Improve timer performance and accuracy
- ✅ Extend performance monitoring capabilities
- ✅ Improve error handling and validation
- ✅ Add new platform-specific implementations
- ✅ Optimize memory usage and cleanup
- ✅ Extend container scope integration

### Prohibited Operations
- ❌ Do NOT modify shared helper files in `../common/helper/` without coordinating with other modules
- ❌ Do NOT bypass libuv event loop for timer operations
- ❌ Do NOT add blocking operations in timer callbacks
- ❌ Do NOT use platform-specific code without proper build guards
- ❌ Do NOT break compatibility with existing Timer API contracts
- ❌ Do NOT add file I/O operations within timer callbacks
- ❌ Do NOT add network operations within timer callbacks
- ❌ Do NOT modify the libuv handle directly without understanding lifecycle
