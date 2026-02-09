# AGENTS
**Name**: Console
**Purpose**: Console is a JavaScript-compatible logging module for ArkTS ecosystem that implements standard Web Console API. It provides comprehensive logging and debugging capabilities with features including:
- Multi-level logging (debug, info, warn, error, fatal)
- Formatted output with placeholders (%s, %d, %f, %j, etc.)
- Table rendering with Unicode borders and alignment
- Timer operations for performance measurement
- Counter management for tracking occurrences
- Grouping with nested indentation support
- Stack trace capture for debugging
- Assertion handling for condition checking
- Hybrid stack trace support (JavaScript + native)
**Primary Language**: C++17 with N-API bindings for JavaScript interoperability.

## Directory Structure
```text
console/
├── BUILD.gn                                      # Main build configuration
├── console.h / console.cpp                       # Console API class implementation
├── log.h                                         # HILOG logging wrapper utilities
└── test/                                         # Unit tests subdirectory
    ├── BUILD.gn
    ├── test.h / test_console.h                   # Test utility headers
    ├── test_ark.cpp                              # ArkTS runtime specific tests
    └── test_console.cpp                          # Console functionality tests
```

## Building
The component uses GN build system with BUILD.gn configuration, run the following command:
```bash
# Build the console module (within build system)
./build.sh --product-name <product> --build-target console
```

## Test Suite
To execute the code tests, run the following command:
```bash
# Build and generate an executable file
./build.sh --product-name <product> --build-target test_console_unittest
# Send the executable file to the device using hdc
hdc shell send ./out/<product>/tests/unittest/ets_utils/ets_utils/jssys/console/napi/test_console_unittest /data/local/tmp/
# Run the executable file
hdc shell
cd /data/local/tmp/
chmod 777 ./test_console_unittest
./test_console_unittest
```

## Dependency
### Package Dependencies
| Dependency | Purpose | Platform |
|------------|---------|----------|
| `napi:ace_napi` | N-API bindings for JavaScript engine | All |
| `hilog:libhilog` | System logging | All |

### External Directory Interactions
- `../../js_concurrent_module/common/helper/` - Shared helper utilities for concurrent modules (napi_helper.cpp, error_helper.h, object_helper.h, path_helper.h)
- `../../` - Parent ets_utils module for shared configurations (ets_utils_config.gni)
- `../timer/` - Related timer module for time measurement functionality

## Boundaries
### Allowed Operations
- ✅ Add new console API methods following Web Console standard
- ✅ Extend format string placeholder support
- ✅ Improve table rendering with new border styles
- ✅ Add new log levels if needed
- ✅ Optimize logging performance and memory usage
- ✅ Extend stack trace capture capabilities
- ✅ Improve assertion handling and error messages
- ✅ Add new timer measurement utilities
- ✅ Enhance formatting and display options

### Prohibited Operations
- ❌ Do NOT modify shared helper files in `../common/helper/` without coordinating with other modules
- ❌ Do NOT bypass HILOG system for direct console output
- ❌ Do NOT use platform-specific code without proper build guards
- ❌ Do NOT execute arbitrary JavaScript code within this module
- ❌ Do NOT break compatibility with existing Console API contracts
- ❌ Do NOT add file I/O operations for log persistence
- ❌ Do NOT add network operations within this module
- ❌ Do NOT modify HILOG domain (0x3D00) or tag ("JSAPP") without system-wide coordination
