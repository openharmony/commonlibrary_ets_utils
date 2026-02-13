# JSON Component Documentation

## Name
json

## Purpose
The json component provides enhanced JSON parsing and stringification utilities for ArkTS/JavaScript applications. It extends standard JSON functionality with additional features including BigInt support, circular reference detection, parameter validation, and utility methods for object manipulation (has, remove). The component offers robust JSON data interchange capabilities with improved error handling and type safety.

## Primary Language
- **TypeScript** (Primary implementation language for JSON utilities)
- **C++** (Native module registration and NAPI bindings)

## Directory Structure
```
json/
├── BUILD.gn                          # Build configuration
├── tsconfig.json                     # TypeScript configuration
├── native_module_json.cpp            # Native module registration
└── src/
    └── json_js.ts                   # JSON utility implementation
```

### File Descriptions
- **BUILD.gn**: GN build configuration defining compilation targets and dependencies
- **tsconfig.json**: TypeScript compiler configuration
- **native_module_json.cpp**: C++ file for NAPI module registration and embedded JS/ABC code
- **src/json_js.ts**: TypeScript implementation of JSON utility functions (parse, stringify, has, remove)

## Building

### Build System
The component uses GN (Generate Ninja) build system with the following build targets:

1. **build_js_ts (action)**: Compiles TypeScript to JavaScript
   - Script: `$util_module/build_ts_js.py`
   - Input: `src/json_js.ts`
   - Output: `json_js.js`

2. **gen_json_abc (es2abc_gen_abc)**: Compiles JavaScript to ABC bytecode
   - Input: `json_js.js`
   - Output: `json.abc`
   - Extra args: `--module`

3. **json_js (gen_obj)**: Generates object file from JavaScript code
   - Output: `json.o` or `json.c` (platform-dependent)

4. **json_abc (gen_obj)**: Generates object file from ABC bytecode
   - Output: `json_abc.o` or `json_abc.c` (platform-dependent)

5. **json (ohos_shared_library)**: Main shared library target
   - Install directory: `module/util`
   - Module name: `util.json` (@ohos.util.json)
   - Security features: PAC_RET branch protection, CFI sanitization

6. **json_static (ohos_source_set)**: Static source set containing implementation
   - Sources: `native_module_json.cpp`
   - Include dirs: `include`, `ets_util_path`

7. **json_packages (group)**: Aggregates all json packages

### Build Process Flow
```
json_js.ts → json_js.js → json.o/json.c → shared library
                      ↓
                 json.abc → json_abc.o/json_abc.c → shared library
```

### Build Dependencies
**Standard OpenHarmony:**
- `hilog:libhilog` - Logging framework
- `napi:ace_napi` - Node-API bindings
- `bounds_checking_function:libsec_shared` - Secure bounds checking
- `icu:shared_icuuc` - ICU library for Unicode support

**ArkUI X (cross-platform):**
- `plugins_root/libs/icu:icu_${target_os}` - ICU library
- `plugins_root/libs/napi:napi_${target_os}` - Platform-specific NAPI
- `plugins_root/libs/securec:sec_${target_os}` - Secure C library

### Compiler Configuration
- C++ Standard: Default (not explicitly specified in config)
- Security: PAC_RET branch protection, CFI sanitization
- Platform defines: ANDROID_PLATFORM, IOS_PLATFORM (for ArkUI X)

### Build Commands
```bash
# Build json module
./build.sh --product-name <product_name> --build-target json

# Build json packages group
./build.sh --product-name <product_name> --build-target json_packages
```

## Test Suite

### Current Status
No dedicated test directory exists for the json component within the `json/` directory.

### Integration Testing
Tests for json functionality are integrated as part of the broader ets_utils component test suite. According to bundle.json configuration, component tests may be included in:
- Parent module test suites
- Application-level integration tests
- ArkTS runtime testing framework

### Recommended Test Areas
When extending this component, ensure tests cover:
- JSON parsing with various data types (numbers, strings, objects, arrays, booleans, null)
- JSON stringification with different replacer functions
- BigInt parsing and stringification
- Circular reference detection and handling
- Parameter validation and error cases
- has() method for key existence checking
- remove() method for key removal
- Error messages and BusinessError handling
- Edge cases (empty strings, malformed JSON, special characters)
- Performance with large JSON payloads
- Unicode and special character handling

## Dependency

### Internal Dependencies
- **Subsystem**: `commonlibrary`
- **Component**: `ets_utils`
- **Module**: Part of `js_util_module` alongside `collections`, `container`, `util`, and `stream`

### External Dependencies

#### Core Dependencies
1. **NAPI (ace_napi)**
   - Purpose: Provides Node-API interface for native module registration
   - Usage: Module initialization, JS/ABC code embedding
   - Version: Compatible with OpenHarmony NAPI specification

2. **HiLog (libhilog)**
   - Purpose: System logging framework
   - Usage: Error logging and debugging (through `tools/log.h`)
   - Integration: Used for logging error conditions

3. **ICU (shared_icuuc)**
   - Purpose: Internationalization Components for Unicode
   - Usage: Unicode support for JSON string handling
   - Integration: Required for proper text encoding/decoding

4. **Bounds Checking Function (libsec_shared)**
   - Purpose: Security bounds checking for memory operations
   - Usage: Memory safety in native code
   - Integration: Ensures safe memory access

5. **es2abc Compiler**
   - Purpose: Compile JavaScript to ArkTS bytecode
   - Usage: Convert .js files to .abc for runtime execution
   - Integration: Part of build toolchain

#### Platform-Specific Dependencies (ArkUI X)
- **Platform-specific NAPI**: Android/iOS NAPI implementations
- **SecureC**: Secure C library replacement for memory safety

### Runtime Dependencies
- **ArkTS Runtime**: Must support standard JSON API extensions
- **Global JSON Object**: Access to built-in JSON.parseBigInt and JSON.stringifyBigInt
- **Error Handling**: BusinessError class for consistent error reporting

### External Directory Interactions
1. **Build System**: Interacts with GN build configuration and es2abc compiler
2. **Runtime Core**: Integrates with ArkTS runtime's JSON implementation
3. **Framework Integration**: Installed to `module/util` directory
4. **GlobalThis**: Access to global JSON object for extended functionality
5. **util_module**: Uses shared build_ts_js.py script from parent directory

## Boundaries

### What Can Be Done Within This Component

#### ✅ Allowed Operations
1. **JSON Parsing and Stringification**
   - Implement enhanced parse() with BigInt support
   - Implement enhanced stringify() with circular reference detection
   - Support custom replacer functions
   - Handle space formatting options
   - Validate and sanitize input parameters

2. **BigInt Support**
   - Parse JSON strings containing BigInt values
   - Stringify objects with BigInt properties
   - Handle different BigInt modes (DEFAULT, PARSE_AS_BIGINT, ALWAYS_PARSE_AS_BIGINT)
   - Ensure type safety for BigInt operations

3. **Object Manipulation Utilities**
   - Implement has() method to check key existence
   - Implement remove() method to delete properties
   - Validate object types (reject arrays for has/remove)
   - Ensure proper property access patterns

4. **Error Handling**
   - Implement BusinessError class for consistent error reporting
   - Validate parameter types (string, function, array, etc.)
   - Provide descriptive error messages
   - Handle and wrap standard JSON errors

5. **Circular Reference Detection**
   - Implement recursive object traversal
   - Detect circular references before stringification
   - Use Set for tracking visited objects
   - Prevent infinite recursion

6. **Parameter Validation**
   - Validate text parameter is string type for parse()
   - Validate replacer is function or array for stringify()
   - Validate space is string or number for stringify()
   - Validate object parameter for has()/remove()
   - Validate key parameter is non-empty string

7. **Build Configuration**
   - Configure TypeScript compilation settings
   - Set up JS to ABC bytecode compilation
   - Manage platform-specific dependencies
   - Configure security features (CFI, PAC_RET)

8. **Module Registration**
   - Implement NAPI module registration
   - Embed JS and ABC code in native module
   - Export utility functions as default export
   - Handle module initialization

### What Cannot Be Done Within This Component

#### ❌ Prohibited Operations
1. **Network or I/O Operations**
   - Do NOT perform network requests or HTTP calls
   - Do NOT read from or write to file system
   - Do NOT implement database connections
   - I/O operations handled by dedicated I/O modules

2. **UI or Rendering**
   - Do NOT create or manipulate UI components
   - Do NOT handle rendering or graphics
   - UI/Rendering handled by ACE/ArkUI framework

3. **System API Calls**
   - Do NOT directly call system-level APIs
   - Do NOT access hardware interfaces
   - Do NOT modify system settings
   - System access handled by system capability modules

4. **Custom JSON Implementations**
   - Do NOT implement custom JSON parsers from scratch
   - Do NOT bypass built-in JSON.parseBigInt and JSON.stringifyBigInt
   - Always leverage runtime JSON implementations
   - This component is a wrapper, not a replacement

5. **Thread Pool Management**
   - Do NOT manage thread lifecycle or concurrency
   - Do NOT implement thread synchronization primitives
   - Thread management handled by concurrent modules (taskpool, worker)

6. **Cross-Module Dependencies**
   - Do NOT create dependencies on other utility modules
   - Maintain module isolation and independence
   - Only use built-in JavaScript/ArkTS capabilities

7. **Breaking API Changes**
   - Do NOT change existing method signatures without deprecation
   - Do NOT remove public APIs without migration path
   - Maintain backward compatibility with standard JSON API

8. **Global State Pollution**
   - Do NOT pollute global namespace beyond module exports
   - Do NOT create global variables or singletons
   - Keep state encapsulated within function calls

9. **Custom Memory Management**
   - Do NOT implement custom allocators or memory pools
   - Do NOT manually manage memory (use ArkTS garbage collector)
   - Memory management handled by runtime

10. **Security Bypass**
    - Do NOT disable or bypass security features
    - Do NOT expose internal implementation details
    - Do NOT compromise JSON parsing security
    - Maintain proper input validation and sanitization

11. **Async Operations**
    - Do NOT implement asynchronous parsing or stringification
    - Do NOT use promises or async/await
    - JSON operations should remain synchronous
    - Async JSON handling should be done at application layer

### Architectural Constraints
1. **Wrapper Pattern**: This component wraps and enhances built-in JSON functionality
2. **Type Safety**: Maintain TypeScript type safety throughout implementation
3. **Error Consistency**: Use consistent BusinessError class for all errors (code: 401)
4. **Runtime Dependency**: Component depends on built-in JSON extensions (parseBigInt, stringifyBigInt)
5. **Synchronous Operations**: All JSON operations must be synchronous
6. **No Side Effects**: Functions should not modify input objects unexpectedly
7. **Standard Compliance**: Maintain compatibility with standard JSON specification
8. **Performance**: Optimize for typical use cases while maintaining correctness

### Modification Guidelines
When extending this component:
1. Always validate input parameters before processing
2. Use BusinessError class with code 401 for all errors
3. Leverage built-in JSON.parseBigInt and JSON.stringifyBigInt
4. Implement comprehensive error messages
5. Maintain backward compatibility with existing API
6. Test edge cases including circular references and malformed JSON
7. Ensure type safety with TypeScript generics where applicable
8. Test on all supported platforms (standard OpenHarmony, ArkUI X)
9. Document any new features or behavioral changes
10. Consider performance implications for large JSON payloads
11. Handle Unicode and special characters properly
12. Maintain consistency with standard JSON error messages
