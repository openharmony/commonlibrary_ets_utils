# AGENTS
**Name**: Utils (Locks / ASON / ConditionVariable)
**Purpose**: Utils is a concurrency synchronization module for ArkTS ecosystem that provides asynchronous lock primitives, condition variables, and ASON (ArkTS Sendable Object Notation) serialization. It enables cross-thread synchronization in TaskPool/Worker concurrent environments with features including:
- Named and anonymous async locks with shared/exclusive modes
- Lock request lifecycle with timeout, cancellation, and env cleanup hooks
- Deadlock detection via wait-for graph and cycle detection (DFS with tri-color marking)
- ConditionVariable for cross-thread wait/notify with timeout support
- ASON serialization/deserialization for Sendable objects
- `isSendable()` type guard for checking sendable objects
- Lock state query API (`query` / `queryAll`) for diagnostics
**Primary Language**: C++17 with N-API bindings for ArkTS interoperability.

## Directory Structure
```text
utils/
├── BUILD.gn                                      # Main build configuration (module name: arkts.utils)
├── native_utils_module.cpp                       # NAPI module registration entry point
├── utils.h / utils.cpp                           # Module init: registers locks + ASON + ConditionVariable + isSendable
├── src/
│   └── utils_js.ts                               # TypeScript API: Lock, ASON, ConditionVariable wrappers
├── locks/
│   ├── common.h                                  # tid_t type alias, INVALID_TID constant
│   ├── async_lock.h / async_lock.cpp             # AsyncLock class: lock state machine, pending/held lists
│   ├── async_lock_manager.h / async_lock_manager.cpp  # Lock registry, deadlock check, query API
│   ├── lock_request.h / lock_request.cpp          # LockRequest: Promise-based, timeout timer, env cleanup
│   ├── graph.h / graph.cpp                       # Generic wait-for graph with DFS cycle detection
│   ├── deadlock_helpers.h / deadlock_helpers.cpp  # Deadlock warning message generation
│   └── weak_wrap.h                               # Weak pointer wrapper utility
├── condition/
│   ├── condition_manager.h / *.cpp               # NAPI constructor + method bindings
│   ├── condition_variable.h / *.cpp              # Named condition registry, wait/notify, ref counting
│   └── condition_task.h / *.cpp                  # Wait task with timeout and deferred Promise
├── json/
│   └── json_manager.h / *.cpp                    # ASON: retrieves serialize/deserialize from runtime global
└── test/
    ├── BUILD.gn
    ├── test_ark.cpp                              # ArkTS runtime test setup
    ├── test_locks.cpp                            # Lock functionality tests
    └── test_condition.cpp                        # ConditionVariable tests
```

## Building
The component uses GN build system with BUILD.gn configuration, run the following command:
```bash
./build.sh --product-name <product> --build-target utils
```

## Test Suite
```bash
./build.sh --product-name <product> --build-target test_locks_unittest
hdc shell send ./out/<product>/tests/unittest/ets_utils/ets_utils/js_concurrent_module/utils/test_locks_unittest /data/local/tmp/
hdc shell "cd /data/local/tmp && chmod 777 ./test_locks_unittest && ./test_locks_unittest"

./build.sh --product-name <product> --build-target test_condition_unittest
hdc shell send ./out/<product>/tests/unittest/ets_utils/ets_utils/js_concurrent_module/utils/test_condition_unittest /data/local/tmp/
hdc shell "cd /data/local/tmp && chmod 777 ./test_condition_unittest && ./test_condition_unittest"
```

## Dependency
### Package Dependencies
| Dependency | Purpose | Platform |
|------------|---------|----------|
| `napi:ace_napi` | N-API bindings for JavaScript engine | All |
| `hilog:libhilog` | System logging | All (non-Linux host) |
| `innerkits:libhilog_linux` | System logging (Linux host) | Linux host |
| `hitrace:hitrace_meter` | Performance tracing | All |
| `libuv:uv` | Timer for lock request timeout | All |
| `js_sys_module/timer:timer` | Timer module dependency | OHOS (standard) |

### External Directory Interactions
- `../common/helper/` - Shared helper utilities (napi_helper.cpp)
- `../../js_sys_module/timer/` - Timer module for timeout features
- `../../` - Parent ets_utils module for shared configurations (ets_utils_config.gni)

## Key Concepts

### Lock Modes
- `LOCK_MODE_UNLOCK` — lock is free
- `LOCK_MODE_SHARED` — multiple shared-mode requests can hold simultaneously
- `LOCK_MODE_EXCLUSIVE` — only one request can hold, blocks all others

### Lock Identity
- **Named lock**: identified by string name, stored in `lockMap` (string → AsyncLock*)
- **Anonymous lock**: identified by auto-incremented uint32_t id, stored in `anonymousLockMap`
- Both use reference counting; lock is deleted when refcount reaches 0 and no pending requests exist

### Deadlock Detection
- `AsyncLockManager::CheckDeadlocksAndLogWarning()` builds a wait-for graph from all lock dependencies
- `Graph::FindCycle()` uses DFS with tri-color marking (WHITE/GREY/BLACK) to detect cycles
- When a cycle is found, `CreateDeadlockWarningMessage()` generates a human-readable warning logged via HILOG
- Deadlock detection runs on every lock request that cannot be immediately satisfied

### ConditionVariable
- Named condition variables are shared across threads via a static `condMap_` (string → ConditionVariable*)
- `wait()` returns a Promise that resolves when `notifyOne()`/`notifyAll()` is called
- `waitFor(timeout)` supports timeout-based wait
- Ref counting: condition is removed when refcount reaches 0

### ASON
- `JsonManager::Init()` retrieves `serialize`/`deserialize` functions from the ArkTS runtime global object
- ASON is a thin bridge layer — all serialization logic resides in the runtime, not in this module

## Boundaries
### Allowed Operations
- ✅ Add new lock modes by extending the `LockMode` enum and updating `CanAcquireLock()`
- ✅ Extend lock options (e.g., new fields in `LockOptions` struct)
- ✅ Add new diagnostic query methods to `AsyncLockManager`
- ✅ Extend `ConditionVariable` with new wait strategies
- ✅ Improve deadlock detection algorithm performance
- ✅ Add new ASON configuration options via runtime global

### Prohibited Operations
- ❌ Do NOT modify `../common/helper/` shared files without coordinating with taskpool and worker modules
- ❌ Do NOT bypass the lock state machine — always go through `AsyncLock::LockAsync()` / `ProcessPendingLockRequest()`
- ❌ Do NOT use raw `std::mutex` directly for lock state — use the `asyncLockMutex_` member and follow the locking order: AsyncLockManager::lockMutex → AsyncLock::asyncLockMutex_
- ❌ Do NOT implement custom deadlock detection outside `Graph::FindCycle()` — all cycle detection must use the tri-color DFS
- ❌ Do NOT store `napi_ref` or `napi_deferred` without registering an env cleanup hook (`AddEnvCleanupHook`)
- ❌ Do NOT break compatibility with existing `arkts.utils` API contracts
- ❌ Do NOT implement ASON serialization logic in this module — it is a bridge layer; all logic is in the runtime
- ❌ Do NOT modify `LockRequest` timeout behavior without updating `test_locks.cpp` timeout test cases
- ❌ Do NOT remove the `EmptyExecuteCallback` placeholder — it is required for env destruction scenarios where `napi_send_event` is unavailable

### Modification Rules
1. Before adding a new `LockMode` value, update `CanAcquireLockUnsafe()` in `async_lock.cpp` and add at least 3 test cases in `test_locks.cpp` covering: acquire, pending-then-acquire, and release-then-pending-wakes
2. Before changing the deadlock detection algorithm, verify `test_locks.cpp` deadlock test cases pass and add a new 3-thread circular wait test case
3. Before modifying `ConditionVariable::wait()` Promise resolution path, update `test_condition.cpp` with wait-notify and wait-timeout test cases
4. Before changing `LockRequest` env cleanup logic, verify that `RemoveEnvCleanupHook` is called in all early-return paths to avoid stale hooks
5. When modifying `BUILD.gn` source list (`locks_sources`), ensure both `utils` and `utils_static` targets compile
6. When modifying the TypeScript API in `src/utils_js.ts`, run `./build.sh --product-name rk3568 --build-target utils` to verify the `.ts → .js → .abc` pipeline succeeds
