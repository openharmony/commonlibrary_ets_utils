# AGENTS
**Name**: FastBuffer
**Purpose**: FastBuffer is a high-performance binary data handling module for ArkTS ecosystem that provides efficient buffer operations. It enables applications to work with raw binary data with features including:
- Binary data manipulation (read/write operations for various data types)
- Multiple encoding support (UTF-8, UTF-16, ASCII, Base64, Hex, Latin1, Binary)
- Endianness conversion (Big-Endian and Little-Endian)
- Byte order swapping (16/32/64-bit)
- Memory allocation with and without initialization
- Buffer concatenation and comparison
- Subarray operations and slicing
- Custom error handling with BusinessError class
- Memory pool optimization for performance
**Primary Language**: TypeScript with C++ native module glue code, using N-API bindings for JavaScript interoperability.

## Directory Structure
```text
fastbuffer/
├── BUILD.gn                                      # Main build configuration
├── tsconfig.json                                 # TypeScript compiler configuration
├── native_module_fastbuffer.cpp                  # NAPI module entry point
└── src/
    └── js_fastbuffer.ts                         # TypeScript API implementation
```

## Building
The component uses GN build system with BUILD.gn configuration, run the following command:
```bash
# Build the fastbuffer module (within build system)
./build.sh --product-name <product> --build-target fastbuffer
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
- `../buffer/` - Related Buffer module for shared functionality
- `../../` - Parent ets_utils module for shared configurations (ets_utils_config.gni)
- Self-contained module with minimal cross-module dependencies

## Boundaries
### Allowed Operations
- ✅ Add new data type read/write methods following Buffer API patterns
- ✅ Extend encoding support for new character sets
- ✅ Improve memory pooling and allocation performance
- ✅ Extend error handling with new error types
- ✅ Add new utility functions for data manipulation
- ✅ Improve bounds checking and memory safety
- ✅ Optimize encoding/decoding performance
- ✅ Add new comparison and search operations

### Prohibited Operations
- ❌ Do NOT implement network I/O operations within this module
- ❌ Do NOT modify shared utility files without coordinating with other modules
- ❌ Do NOT break compatibility with existing FastBuffer API contracts
- ❌ Do NOT bypass bounds checking for memory operations
- ❌ Do NOT use platform-specific code without proper build guards
- ❌ Do NOT implement file system operations directly
- ❌ Do NOT expose raw memory pointers to JavaScript without safety checks
- ❌ Do NOT modify memory pool behavior without understanding impact on performance
