# AGENTS
**Name**: Buffer
**Purpose**: Buffer is a binary data handling module for ArkTS ecosystem that provides Node.js-compatible Buffer and Blob APIs. It enables applications to work with raw binary data with features including:
- Buffer class for reading/writing various data types (Int8, Uint16, Float32, etc.)
- Blob class for immutable binary data objects
- Multiple encoding support (UTF-8, UTF-16LE, ASCII, Base64, Hex, Latin1, Binary)
- Binary to string conversions and vice versa
- Memory pooling optimization for performance
- Direct memory manipulation with bounds checking
- Both little-endian and big-endian support
- Comprehensive error handling with custom error types
**Primary Language**: TypeScript with C++ native implementation for performance-critical operations, using N-API bindings for JavaScript interoperability.

## Directory Structure
```text
buffer/
├── BUILD.gn                                      # Main build configuration
├── tsconfig.json                                 # TypeScript compiler configuration
├── native_module_buffer.cpp                      # NAPI module entry point
├── js_buffer.h / js_buffer.cpp                   # Native Buffer implementation
├── js_blob.h / js_blob.cpp                       # Native Blob implementation
├── converter.h / converter.cpp                   # Encoding conversion utilities
├── src/
│   └── js_buffer.ts                             # TypeScript API implementation
└── test/                                         # Unit tests subdirectory
    ├── BUILD.gn
    ├── test.h                                     # Test base class
    ├── test_ark.cpp                               # Ark runtime tests
    └── test_napi.cpp                              # NAPI tests
```

## Building
The component uses GN build system with BUILD.gn configuration, run the following command:
```bash
# Build the buffer module (within build system)
./build.sh --product-name <product> --build-target buffer
```

## Test Suite
To execute code tests, run the following command:
```bash
# Build and generate an executable file
./build.sh --product-name <product> --build-target test_buffer_unittest
# Send the executable file to device using hdc
hdc shell send ./out/<product>/tests/unittest/ets_utils/ets_utils/jsapi/buffer/napi/test_buffer_unittest /data/local/tmp/
# Run the executable file
hdc shell
cd /data/local/tmp/
chmod 777 ./test_buffer_unittest
./test_buffer_unittest
```

## Dependency
### Package Dependencies
| Dependency | Purpose | Platform |
|------------|---------|----------|
| `napi:ace_napi` | N-API bindings for JavaScript engine | All |
| `hilog:libhilog` | System logging | All |
| `icu:shared_icuuc` | Unicode internationalization components | All |
| `bounds_checking_function:libsec_shared` | Memory safety functions | All |

### External Directory Interactions
- `../../js_sys_module/` - JavaScript system modules for shared utilities
- `../../` - Parent ets_utils module for shared configurations (ets_utils_config.gni)
- Self-contained module with minimal cross-module dependencies

## Boundaries
### Allowed Operations
- ✅ Add new data type read/write methods following Node.js Buffer API
- ✅ Extend encoding support for new character sets
- ✅ Improve memory pooling and allocation performance
- ✅ Extend error handling with new error types
- ✅ Add new utility functions for data manipulation
- ✅ Improve bounds checking and memory safety
- ✅ Extend Blob functionality
- ✅ Optimize encoding/decoding performance

### Prohibited Operations
- ❌ Do NOT implement network I/O operations within this module
- ❌ Do NOT modify shared utility files without coordinating with other modules
- ❌ Do NOT break compatibility with existing Buffer API contracts (Node.js standard)
- ❌ Do NOT bypass bounds checking for memory operations
- ❌ Do NOT use platform-specific code without proper build guards
- ❌ Do NOT implement file system operations directly in Buffer/Blob
- ❌ Do NOT expose raw memory pointers to JavaScript without safety checks
- ❌ Do NOT modify memory pool behavior without understanding impact on performance
