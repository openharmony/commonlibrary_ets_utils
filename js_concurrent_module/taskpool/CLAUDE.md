# AGENTS
**Name**: TaskPool
**Purpose**: TaskPool is a concurrent task execution module for arkcompiler runtime that implements task scheduling and execution management. It provides a thread pool-based parallel execution environment for ArkTS code with features including:
- Task execution with priority levels (HIGH, MEDIUM, LOW, IDLE)
- Task group management for batch task execution
- Delayed and periodic task execution
- Task dependency management
- Load balancing and dynamic thread pool scaling
- Task cancellation and timeout handling
- Task result information tracking
- Sequence runner for ordered task execution
- Async runner for asynchronous task execution
- Worker thread lifecycle management
- Task duration statistics and monitoring
- System event reporting for diagnostics
**Primary Language**: C++ with N-API bindings for ArkTS interoperability.

## Directory Structure
```text
taskpool/
├── BUILD.gn                                      # Main build configuration
├── native_module_taskpool.cpp                    # NAPI module registration entry point
├── taskpool.h / taskpool.cpp                     # Main TaskPool API class
├── task_manager.h / task_manager.cpp             # Task manager for task lifecycle and scheduling
├── task.h / task.cpp                             # Task implementation
├── task_group.h / task_group.cpp                 # Task group implementation
├── task_group_manager.h / task_group_manager.cpp # Task group manager
├── worker.h / worker.cpp                         # Worker thread implementation
├── thread.h / thread.cpp                         # Thread abstraction
├── task_queue.h / task_queue.cpp                 # Task queue for scheduling
├── task_runner.h / task_runner.cpp                # Task runner base class
├── base_runner.h / base_runner.cpp               # Base runner implementation
├── base_runner_manager.h / base_runner_manager.cpp # Base runner manager
├── sequence_runner.h / sequence_runner.cpp       # Sequence runner for ordered execution
├── sequence_runner_manager.h / sequence_runner_manager.cpp # Sequence runner manager
├── async_runner.h / async_runner.cpp             # Async runner for async execution
├── async_runner_manager.h / async_runner_manager.cpp # Async runner manager
├── log_manager.h / log_manager.cpp               # Logging management
├── dfx_hisys_event.h / dfx_hisys_event.cpp      # System event reporting
├── AGENTS.md                                     # This documentation file
└── test/                                         # Unit tests subdirectory
    ├── BUILD.gn
    ├── test.h / test.cpp
    ├── test_taskpool.cpp                         # TaskPool-specific tests
    └── test_ark.cpp                              # ArkTS runtime tests
```

## Building
The component uses GN build system with BUILD.gn configuration, run the following command:
```bash
# Build the taskpool module (within OpenHarmony build system)
./build.sh --product-name <product> --build-target taskpool

# Build static version
./build.sh --product-name <product> --build-target taskpool_static
```

## Test Suite
To execute the code tests, run the following command:
```bash
# Build and generate an executable file
./build.sh --product-name <product> --build-target test_taskpool_unittest
# Send the executable file to the device using hdc
hdc shell send ./out/<product>/tests/unittest/ets_utils/ets_utils/js_concurrent_module/taskpool/test_taskpool_unittest /data/local/tmp/
# Run the executable file
hdc shell
cd /data/local/tmp/
chmod 777 ./test_taskpool_unittest
./test_taskpool_unittest
```

## Dependency
### Package Dependencies
| Dependency | Purpose | Platform |
|------------|---------|----------|
| `napi:ace_napi` | N-API bindings for JavaScript engine | All |
| `bounds_checking_function:libsec_shared` | Security bounds checking | All |
| `hilog:libhilog` | System logging | All (non-Linux host) |
| `innerkits:libhilog_linux` | System logging (Linux host) | Linux host |
| `hitrace:hitrace_meter` | Performance tracing | All |
| `c_utils:utils` | Common utilities | OHOS (standard) |
| `eventhandler:libeventhandler` | Event handling | OHOS (standard) |
| `ffrt:libffrt` | Flexible Function Runtime | OHOS (standard) |
| `hisysevent:libhisysevent` | System event reporting | OHOS (standard) |
| `runtime_core:ani` | ARK Native Interface | OHOS (standard) |
| `runtime_core:libarkruntime` | ARK runtime library | OHOS (standard) |
| `init:libbegetutil` | Init utilities | OHOS (standard) |
| `bundle_framework:appexecfwk_base` | Bundle framework base | OHOS (standard) |
| `bundle_framework:appexecfwk_core` | Bundle framework core | OHOS (standard) |
| `ipc:ipc_core` | IPC core | OHOS (standard) |
| `samgr:samgr_proxy` | System ability manager proxy | OHOS (standard) |
| `qos_manager:qos` | Quality of Service management | OHOS (standard, optional) |

### External Directory Interactions
- `../common/helper/` - Shared helper utilities for concurrent modules (napi_helper.cpp, concurrent_helper.cpp, async_stack_helper.cpp, hybrid_concurrent_helper.cpp)
- `../../js_sys_module/timer/` - Timer module dependency for time-related operations
- `../../` - Parent ets_utils module for shared configurations (ets_utils_config.gni)
- `$platform_root/` - Platform-specific implementations (qos_helper.cpp, process_helper.cpp)

## Default Configuration
- Default thread pool size: Determined by ConcurrentHelper::GetMaxThreads()
- Default task priority: Priority::DEFAULT (MEDIUM)
- Task timeout: Configurable per task
- Load balance interval: Configurable via system properties
- FFRT support: Enabled for system apps by default, can be disabled via flags

## Boundaries
### Allowed Operations
- ✅ Add new task types by extending the Task class
- ✅ Implement new runner types by extending TaskRunner
- ✅ Add new N-API methods to expose ArkTS APIs
- ✅ Modify task queue implementation for performance optimization
- ✅ Add new priority levels for task scheduling
- ✅ Extend task dependency management features
- ✅ Add new diagnostic/telemetry events
- ✅ Improve load balancing and thread pool scaling algorithms
- ✅ Enhance task cancellation and timeout handling
- ✅ Add new task execution modes
- ✅ Extend task group management features
- ✅ Improve error handling and exception propagation

### Prohibited Operations
- ❌ Do NOT modify shared helper files in `../common/helper/` without coordinating with other modules
- ❌ Do NOT bypass the task lifecycle state machine
- ❌ Do NOT use raw thread APIs - always use the provided Thread/Worker abstractions
- ❌ Do NOT store sensitive data in task objects without proper security measures
- ❌ Do NOT break compatibility with existing TaskPool ArkTS API contracts
- ❌ Do NOT modify worker thread management without understanding the load balancing requirements
- ❌ Do NOT disable task priority scheduling without proper justification
- ❌ Do NOT remove or modify task dependency validation logic
