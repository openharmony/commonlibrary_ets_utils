# AGENTS
**Name**: Process
**Purpose**: Process is a process management module for ArkTS ecosystem that provides JavaScript APIs for system process operations. It enables applications to access process information, spawn child processes, and control process execution with features including:
- Process ID and parent process information (PID, PPID, TID)
- User and group management (UID, GID, effective IDs, groups)
- Process control operations (kill, exit, abort, chdir)
- System information access (uptime, CPU time, system config)
- Environment variable operations
- Child process spawning and management
- Standard output/error handling for spawned processes
- Process status monitoring (exit code, killed status)
- Thread priority management
**Primary Language**: C++ with N-API bindings for JavaScript interoperability.

## Directory Structure
```text
process/
├── BUILD.gn                                      # Main build configuration
├── js_process.h / js_process.cpp                 # Core process management implementation
├── js_childprocess.h / js_childprocess.cpp       # Child process spawning implementation
└── native_module_process.cpp                     # NAPI module registration entry point
```

## Building
The component uses GN build system with BUILD.gn configuration, run the following command:
```bash
# Build the process module (within build system)
./build.sh --product-name <product> --build-target process
```

## Dependency
### Package Dependencies
| Dependency | Purpose | Platform |
|------------|---------|----------|
| `napi:ace_napi` | N-API bindings for JavaScript engine | All |
| `hilog:libhilog` | System logging | All |
| `bounds_checking_function:libsec_shared` | Security bounds checking | All |
| `icu:shared_icuuc` | Unicode internationalization components | All |
| `c_utils:utils` | Common utilities library | Non-standard system |

### External Directory Interactions
- `../../js_concurrent_module/common/helper/` - Shared helper utilities for concurrent modules
- `../../` - Parent ets_utils module for shared configurations (ets_utils_config.gni)
- `../../tools/log.h` - Logging framework wrapper

## Boundaries
### Allowed Operations
- ✅ Add new process information retrieval methods following POSIX standards
- ✅ Extend child process spawning capabilities
- ✅ Add new signal handling options
- ✅ Improve process status monitoring
- ✅ Extend environment variable management
- ✅ Add new platform-specific implementations
- ✅ Improve error handling and validation
- ✅ Extend thread priority management
- ✅ Optimize performance for process operations

### Prohibited Operations
- ❌ Do NOT modify shared helper files in `../common/helper/` without coordinating with other modules
- ❌ Do NOT bypass security boundaries or execute privileged operations without proper authorization
- ❌ Do NOT use platform-specific code without proper build guards
- ❌ Do NOT execute arbitrary system commands without proper validation
- ❌ Do NOT break compatibility with existing Process API contracts
- ❌ Do NOT add file I/O operations that bypass standard process management
- ❌ Do NOT kill critical system processes
- ❌ Do NOT modify process isolation mechanisms
- ❌ Do NOT expose sensitive system information without proper access control
