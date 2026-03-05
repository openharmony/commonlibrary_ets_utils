# AGENTS
**Name**: Worker
**Purpose**: Worker is a concurrent execution module for arkcompiler runtime that implements the Web Worker API. It provides thread-based parallel execution environment for ArkTS code with features including:
- Multi-threaded script execution with dedicated worker threads
- Bidirectional message passing between host and worker threads
- Event listener management (onmessage, onerror, onmessageerror, onallerrors)
- Worker priority levels (HIGH, MEDIUM, LOW, IDLE, DEADLINE, VIP)
- Global call synchronization between host and worker
- Sendable objects support for efficient data transfer
- Parent port communication for nested workers
- Container scope isolation for worker execution
- Classic and Module script modes
- Worker lifecycle management (STARTING, RUNNING, TERMINATING, TERMINATED)
**Primary Language**: C++ with N-API bindings for ArkTS interoperability.

## Directory Structure
```text
worker/
├── BUILD.gn                                      # Main build configuration
├── native_module_worker.cpp                        # NAPI module registration entry point
├── worker.h / worker.cpp                          # Main Worker API class
├── worker_runner.h / worker_runner.cpp              # Worker runner implementation
├── message_queue.h / message_queue.cpp              # Message queue for communication
├── thread.h / thread.cpp                          # Thread abstraction
├── AGENTS.md                                     # This documentation file
└── test/                                         # Unit tests subdirectory
    ├── BUILD.gn
    ├── test.h
    ├── test_worker.cpp                             # Worker-specific tests
    └── test_ark.cpp                              # ArkTS runtime tests
```

## Building
The component uses GN build system with BUILD.gn configuration, run the following command:
```bash
# Build the worker module (within OpenHarmony build system)
./build.sh --product-name <product> --build-target worker

# Build static version
./build.sh --product-name <product> --build-target worker_static
```

## Test Suite
To execute the code tests, run the following command:
```bash
# Build and generate an executable file
./build.sh --product-name <product> --build-target test_worker_unittest
# Send the executable file to the device using hdc
hdc shell send ./out/<product>/tests/unittest/ets_utils/ets_utils/js_concurrent_module/worker/test_worker_unittest /data/local/tmp/
# Run the executable file
hdc shell
cd /data/local/tmp/
chmod 777 ./test_worker_unittest
./test_worker_unittest
```

## Dependency
### Package Dependencies
| Dependency | Purpose | Platform |
|------------|---------|----------|
| `napi:ace_napi` | N-API bindings for JavaScript engine | All |
| `c_utils:utils` | Common utilities | All |
| `hilog:libhilog` | System logging | All (non-Linux host) |
| `innerkits:libhilog_linux` | System logging (Linux host) | Linux host |
| `hitrace:hitrace_meter` | Performance tracing | All |
| `eventhandler:libeventhandler` | Event handling | OHOS (standard) |
| `runtime_core:ani` | ARK Native Interface | OHOS (standard) |
| `runtime_core:libarkruntime` | ARK runtime library | OHOS (standard) |
| `init:libbegetutil` | Init utilities | OHOS (standard) |
| `qos_manager:qos` | Quality of Service management | OHOS (standard, optional) |

### External Directory Interactions
- `../common/helper/` - Shared helper utilities for concurrent modules (napi_helper.cpp, async_stack_helper.cpp, hybrid_concurrent_helper.cpp)
- `../../js_sys_module/timer/` - Timer module dependency for time-related operations
- `../../` - Parent ets_utils module for shared configurations (ets_utils_config.gni)
- `$platform_root/` - Platform-specific implementations (qos_helper.cpp, process_helper.cpp)

## Default Configuration
- Default worker priority: WorkerPriority::INVALID (can be set to HIGH, MEDIUM, LOW, IDLE, DEADLINE, VIP)
- Default script mode: CLASSIC (can be set to MODULE)
- Default listener mode: PERMANENT (can be set to ONCE)
- Worker state transitions: STARTING → RUNNING → TERMINATING → TERMINATED

## Boundaries
### Allowed Operations
- ✅ Add new worker types by extending the Worker class
- ✅ Implement new runner types by extending WorkerRunner
- ✅ Add new N-API methods to expose ArkTS APIs
- ✅ Modify message queue implementation for performance optimization
- ✅ Add new event types for worker communication
- ✅ Extend priority level management
- ✅ Add new diagnostic/telemetry events
- ✅ Extend global call synchronization features
- ✅ Improve error handling and exception propagation
- ✅ Add new listener modes
- ✅ Enhance sendable objects support

### Prohibited Operations
- ❌ Do NOT modify shared helper files in `../common/helper/` without coordinating with other modules
- ❌ Do NOT bypass the worker lifecycle state machine
- ❌ Do NOT use raw thread APIs - always use the provided Thread/WorkerRunner abstractions
- ❌ Do NOT store sensitive data in worker objects without proper security measures
- ❌ Do NOT break compatibility with existing Worker ArkTS API contracts
- ❌ Do NOT modify container scope management without understanding the isolation requirements
- ❌ Do NOT disable message queue synchronization without proper justification
