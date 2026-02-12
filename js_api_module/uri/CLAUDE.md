# AGENTS
**Name**: URI
**Purpose**: URI (Uniform Resource Identifier) is a parsing and manipulation module for ArkTS runtime that implements standard URI functionality. It provides comprehensive URI operations including:
- **Core Functionality**: URI parsing and validation from string input, URI component extraction (scheme, authority, path, query, fragment), URI normalization and equality comparison, query parameter management (add, get, clear, hasKey, hasValue, setAll, remove, etc.), path segment manipulation
- **Protocol Support**: Support for both absolute and relative URIs, hierarchical and opaque URI handling, IPv4 and IPv6 host address support
- **Encoding Operations**: URL encoding/decoding for safe character handling with comprehensive character set support
- **Standard Compliance**: Implements RFC 3986 compliant URI parsing and manipulation

**Primary Language**: C++ with N-API bindings for ArkTS interoperability and TypeScript wrapper layer.

## Directory Structure
```text
uri/
├── BUILD.gn                                      # Main build configuration defining compilation targets and dependencies
├── tsconfig.json                                 # TypeScript compiler configuration (tsconfig options)
├── src/
│   ├── js_uri.ts                                 # TypeScript API wrapper exposing Uri class and public API surface
│   └── js_uri.ts exports:                     # Uri class, Uri.Escape/Hierarchy/Helper functions, parse/set/toString methods
├── js_uri.h / js_uri.cpp                         # Core URI implementation: Uri class with RFC 3986 compliant parsing, character validation rules, component extractors, normalization algorithms
├── native_module_uri.h / native_module_uri.cpp   # N-API module registration: UriConstructor binding, method exports (Normalize, Equals, IsAbsolute, etc.)
└── test/                                         # Unit tests subdirectory
    ├── BUILD.gn                                  # Test build configuration
    ├── test.h / test_napi.cpp                    # N-API binding tests
    │   └── test_ark.cpp                              # ArkTS runtime integration tests
    └── test_ark.ts exports                      # Test entry points
```

## Building
The component uses GN (Generate Ninja) build system with BUILD.gn configuration file. The build process compiles TypeScript to JavaScript, converts to ABC bytecode using es2abc compiler, and finally links with C++ native implementation into a shared library.

### Build System
The component uses GN build system configured through BUILD.gn with the following characteristics:
- **Build Tool**: GN (Generate Ninja) - Meta-build system that generates Ninja build files
- **Compiler Toolchain**: Clang/LLVM for C++ compilation
- **TypeScript Compiler**: build_ts_js.py action that invokes ArkTS tsc compiler
- **Bytecode Compiler**: es2abc compiler that converts JavaScript to ArkTS ABC bytecode
- **Integration**: Seamless integration with ArkTS runtime and N-API framework

### Build Process Flow
```text
├──────────────────────────────────────────────────────────────────────┐
│                    TypeScript Source Files                          │
│            (src/js_uri.ts)                            │
│                    ↓                                       │
│            [build_ts_js action]                        │
│                    ↓                                       │
│            JavaScript Intermediate (.js)                │
│                    ↓                                       │
│            [gen_uri_abc action]                       │
│                    ↓                                       │
│            ABC Bytecode File (.abc)                    │
│                    ↓                                       │
│            C++ Compiler (clang++)                    │
│                    ↓                                       │
│            Native Object Files (.o)                     │
│                    ↓                                       │
│            ↓                                       │
│            Linker (ld)                                │
│                    ↓                                       │
│            Target Shared Library (liburi.so/.dylib)     │
└──────────────────────────────────────────────────────────────────────┘
```

### Build Targets
BUILD.gn defines the following build targets:

#### Core Targets
- **gen_uri_abc**: Compiles JavaScript (.js) files to ABC bytecode (.abc) using es2abc compiler
  - **Purpose**: Transform TypeScript source to ArkTS-executable bytecode format
  - **Inputs**: TypeScript files from `src/` directory
  - **Outputs**: ABC bytecode file with embedded native code
  - **Dependencies**: es2abc compiler, ArkTS runtime headers

- **uri_static**: Static library containing native C++ implementation
  - **Purpose**: Compile C++ source files and link into static library
  - **Sources**: `js_uri.cpp` (native implementation), related C++ files
  - **Outputs**: `uri_static.a` / `uri_static.lib` (platform-dependent)
  - **Note**: Static library allows compile-time optimization but does not allow runtime flexibility

- **uri**: Final shared library target
  - **Purpose**: Aggregate all components into loadable shared library
  - **Sources**: Compiled C++ objects, embedded ABC bytecode
  - **Outputs**: `liburi.so` (Linux), `liburi.dylib` (macOS), `liburi.dll` (Windows)
  - **Dependencies**: uri_static (static), es2abc compiler (bytecode generation), N-API framework headers

### Build Commands
```bash
# Build URI module (from repository root)
./build.sh --product-name <product> --build-target uri

# Build specific targets
./build.sh --product-name <product> --build-target gen_uri_abc     # Generate ABC bytecode from TypeScript
./build.sh --product-name <product> --build-target uri_static     # Build static library with native implementation
./build.sh --product-name <product> --build-target uri             # Build final shared library
```

## Test Suite
Comprehensive unit test suite covering all URI functionality areas.

### Test Build
```bash
# Build and generate test executable
./build.sh --product-name <product> --build-target test_uri_unittest

# Test executable output: test_uri_unittest
# Location: ./out/<product>/tests/unittest/ets_utils/js_api_module/uri/
```

### Test Execution
```bash
# Transfer executable to device
hdc shell send ./out/<product>/tests/unittest/ets_utils/js_api_module/uri/test_uri_unittest /data/local/tmp/

# Execute on device
hdc shell
cd /data/local/tmp/
chmod 777 ./test_uri_unittest
./test_uri_unittest
```

### Test Suite Content
The test suite includes comprehensive tests organized by category:

#### 1. N-API Binding Tests (test_napi.cpp)
- **Constructor Tests**: Verify Uri object creation with string input
- **Method Binding Tests**: Ensure all exported methods (Normalize, Equals, IsAbsolute, etc.) are properly bound
- **Parameter Validation**: Type checking for all method parameters (string, number, Uri)
- **Error Handling**: Verify BusinessError exceptions with correct error codes

#### 2. ArkTS Runtime Integration Tests (test_ark.cpp)
- **Parsing Functionality**: Test URI parsing from various valid and invalid string inputs
- **Component Extraction**: Verify correct extraction of scheme, authority, path, query, fragment components
- **Query Management**: Test add, get, remove, hasKey, hasValue, setAll operations
- **Path Operations**: Test path segment manipulation (add, delete, modify)
- **Edge Cases**: Boundary conditions (empty strings, overly long inputs, special characters)
- **RFC 3986 Compliance**: Verify adherence to URI specification requirements

#### 3. Unit Test Framework
- **Framework**: Google Test (gtest/gmock) for test organization and mocking
- **Assertions**: EXPECT_EQ, EXPECT_TRUE, EXPECT_THROW for validating expected behaviors
- **Test Coverage**: Measure code coverage across all URI functionality
- **Performance**: Benchmark critical parsing paths for optimization validation

### Recommended Test Areas
When extending this component, ensure tests cover:
- **Parsing Coverage** (Priority 1)
  - Valid URI schemes: http, https, ftp, file, mailto, etc.
  - Invalid inputs: empty strings, malformed URIs, invalid characters
  - RFC 3986 compliance: character validation, encoding rules
  - Parsing failures: error messages and error codes
  - Component boundaries: maximum lengths (各自 scheme, 2048 chars for authority, etc.)

- **Component Extraction** (Priority 2)
  - Correct scheme parsing for all valid schemes
  - Authority format: username@host, user:pass@host
  - Path formats: absolute, relative, path-rootless, path-absolute, path-noscheme
  - Query string format: ?key=value&key2=value2 with proper encoding
  - Fragment extraction: identifier after # without proper validation
  - Opaque URIs: proper handling of non-hierarchical URIs

- **Query Operations** (Priority 3)
  - Add: insert query parameters, verify addition
  - Get: retrieve query values, verify existence
  - HasKey: check key existence before access
  - HasValue: check value existence
  - SetAll: bulk insert from map/object
  - Remove: delete specific or all query parameters
  - Clear: remove all query parameters
  - Parse: string to query object conversion
  - Stringify: query object to string format

- **Path Manipulation** (Priority 4)
  - Add: add path segments
  - Delete: remove path segments at indices
  - Modify: update existing path segments
  - Slice: extract sub-paths
  - Normalize: path normalization algorithms
  - Join: concatenate path segments
  - Split: break path into segments
  - Parent: resolve parent directory references

- **Normalization** (Priority 5)
  - Scheme normalization: lowercase, remove default port
  - Path normalization: remove dot segments, resolve . and ..
  - Query normalization: sort parameters, encode spaces
  - Host normalization: lowercase hostname, convert international domain names to Punycode
  - Fragment normalization: percent-encode reserved characters

- **Encoding/Decoding** (Priority 6)
  - UTF-8 encoding: convert to/from UTF-8 byte arrays
  - Base64: binary to text encoding for URI components
  - Percent encoding: encode special characters as %XX
  - Hex encoding: convert binary data to hex string
  - Character set validation: ASCII, UTF-8, GBK, Big5, ICU character sets

- **Error Handling** (Priority 7)
  - Invalid URI format: throw BusinessError(401, "parameter error")
  - Invalid scheme: throw BusinessError(401, "unsupported scheme")
  - Invalid characters: throw BusinessError(401, "illegal characters")
  - Out of range: throw BusinessError(10200001, "out of range")
  - Missing component: throw BusinessError(401, "missing required component")
  - Encoding errors: throw BusinessError(401, "encoding failure")

- **Integration** (Priority 8)
  - ArkTS runtime: test with real ArkTS VM
  - N-API framework: verify N-API binding correctness
  - Error propagation: verify error propagation through N-API layer
  - Memory management: test with actual heap allocation/deallocation

## Dependency
### Package Dependencies
| Dependency | Purpose | Platform |
|------------|---------|----------|
| `napi:ace_napi` | Node-API bindings for native module registration and export | All |
| `c_utils:utils` | Common utility functions (logging, macros, assertions) | Non-standard system |
| `hilog:libhilog` | System logging framework for error reporting and debugging | All |
| `icu:shared_icuuc` | International Components for Unicode (ICU) - character encoding, locale data | All |
| `bounds_checking_function:libsec_shared` | Security functions for bounds checking and memory safety | Test |
| `ets_runtime:libark_jsruntime` | ArkTS JavaScript runtime engine | Test |
| `libuv:uv` | Event loop utilities for async I/O operations | Test |

### External Directory Interactions
- `../../ets_utils_config.gni` - Parent module configuration for shared build settings
  - Provides common compiler flags, optimization settings, and platform definitions
- `../../build_ts_js.py` - TypeScript compilation script used by all ets_utils modules
  - Converts `.ts` files to `.js` JavaScript files using ArkTS tsc compiler
- `../../es2abc` (from es2abc_config.gni) - ABC bytecode compiler
  - Converts `.js` files to `.abc` ArkTS bytecode for runtime execution
  - Provides compiler integration and optimization flags
- `../../include/` - Public API header directory for external module interfaces
- `../../napi/` - N-API framework headers for native module development

## Boundaries
### Allowed Operations
- ✅ **Extend Uri Class** - Add new URI parsing methods by extending the Uri class with additional functionality
- ✅ **Implement N-API Bindings** - Add new N-API bindings to expose additional URI operations to ArkTS layer
- ✅ **Query Parameter Management** - Extend query parameter manipulation functions (add, get, remove, clear, hasKey, hasValue, forEach, etc.)
- ✅ **Path Operations** - Add new path manipulation methods (join, normalize, resolve, parent, etc.)
- ✅ **Encoding/Decoding Support** - Add support for additional character encodings beyond UTF-8 and Base64
- ✅ **IPv4/IPv6 Support** - Extend host address parsing to support IPv6 addresses and IPv6 zone IDs
- ✅ **Custom URI Schemes** - Add support for custom URI schemes (e.g., jar:, urn:)
- ✅ **Error Handling** - Add new error types with specific error codes for different failure scenarios
- ✅ **Performance Optimization** - Optimize character encoding/decoding operations for frequently used encodings
- ✅ **Utility Functions** - Add URI transformation utilities (escape, unescape, file URL to path, etc.)
- ✅ **Validation Enhancement** - Add stricter validation for URI components according to RFC 3986 and security requirements
- ✅ **Testing Support** - Add diagnostic hooks and profiling capabilities for URI operations
- ✅ **Documentation** - Maintain clear API documentation for all Uri methods with examples

### Prohibited Operations
- ❌ **API Breaking Changes** - Do NOT modify existing Uri class public API signatures without considering backward compatibility
  - Breaking changes include: removing methods, changing parameter types, altering return values
  - Rationale: Existing applications depend on stable API contracts; breaking changes cause widespread failures
  - Migration path: If breaking changes are necessary, provide deprecation warnings and migration guide
  - Exception: Only break API if absolutely required for security or critical bug fixes

- ❌ **RFC 3986 Violations** - Do NOT implement URI parsing logic that violates RFC 3986 specification
  - **Character Validation**: Do NOT bypass or modify character validation rules (g_ruleAlpha, g_ruleDigit, g_ruleScheme, etc.)
  - **Encoding Rules**: Do NOT use incorrect encoding rules for percent-encoding or Base64 padding
  - **Component Separation**: Do NOT ignore # (fragment) separator or ? (query) separator without proper parsing
  - **Length Limits**: Do NOT exceed maximum length limits for URI components (scheme limits, total length 2GB-8MB)
  - **Security**: Do NOT introduce security vulnerabilities through URI parsing (injection attacks, buffer overflows)
  - **Error Message Format**: Do NOT change error message format or error codes (errStr_, error numbers)
  - **Normalization Logic**: Do NOT alter URI normalization algorithms that could lead to incorrect results

- ❌ **N-API Safety Bypass** - Do NOT bypass N-API type checking and safety mechanisms
  - **Raw String Manipulation** - Do NOT use raw string manipulation without proper encoding consideration
  - **Type System Bypass** - Do NOT circumvent TypeScript type system for parameters
  - **Memory Safety** - Do NOT introduce memory leaks, buffer overflows, or use-after-free vulnerabilities
  - **Error Suppression** - Do NOT swallow or hide errors without proper handling or reporting

- ❌ **Test Suite Breaking** - Do NOT modify or remove existing test cases without updating documentation
  - **Test Coverage Reduction** - Do NOT reduce test coverage or remove critical test categories
  - **Documentation Neglect** - Do NOT fail to update CLAUDE.md when adding new features or modifying behavior

- ❌ **Global Namespace Pollution** - Do NOT add unnecessary global variables or singletons
  - Do NOT modify existing global object prototypes without approval
  - Do NOT create naming conflicts with existing modules or libraries

- ❌ **Cross-Module Dependencies** - Do NOT create dependencies on non-util modules (collections, json, stream)
  - Only depend on explicitly allowed external libraries listed in Dependency section
  - Maintain module independence and clear separation of concerns

- ❌ **Build System Bypass** - Do NOT modify BUILD.gn configuration directly without understanding implications
  - Do NOT add custom build targets that break established build patterns
  - Do NOT bypass es2abc compiler or TypeScript compilation process