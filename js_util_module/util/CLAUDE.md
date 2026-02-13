# Util Component Documentation

## Name
util

## Purpose
The util component provides a comprehensive utility library for ArkTS/JavaScript applications, offering text encoding/decoding, Base64 conversion, type checking, UUID generation, caching mechanisms, rational number operations, scope management, and aspect-oriented programming utilities. It serves as a foundational utility module that supports data transformation, validation, and common programming patterns across the OpenHarmony ecosystem.

## Primary Language
- **TypeScript** (Primary implementation for utility classes and functions)
- **C++** (Native module registration and NAPI bindings)
- **C** (Performance-critical string operations)

## Directory Structure
```
util/
├── BUILD.gn                          # Build configuration
├── tsconfig.json                     # TypeScript configuration
├── js_base64.cpp/h                  # Base64 encoding/decoding implementation
├── js_stringdecoder.cpp/h            # String decoder for byte streams
├── js_textdecoder.cpp/h              # Text decoder with encoding support
├── js_textencoder.cpp/h              # Text encoder implementation
├── js_types.cpp/h                   # Type checking utilities
├── js_uuid.cpp/h                    # UUID generation and parsing
├── native_module_util.cpp/h          # Native module registration and exports
├── plugin/
│   ├── hispeed_string_plugin.c/h     # High-performance string operations
├── src/
│   └── util_js.ts                   # TypeScript utility implementations
└── test/
    ├── BUILD.gn                      # Test build configuration
    ├── test_ark.cpp                  # ArkTS-related tests
    ├── test_util.cpp                 # Core utility tests
    ├── test_uuid.cpp                 # UUID functionality tests
    └── test.h                        # Test infrastructure with Ark runtime environment setup
```

### File Descriptions
- **BUILD.gn**: GN build configuration defining compilation targets and dependencies
- **tsconfig.json**: TypeScript compiler configuration
- **js_base64.cpp/h**: Base64 encoding/decoding with sync and async operations
- **js_stringdecoder.cpp/h**: Decoder for byte streams to strings with encoding support
- **js_textdecoder.cpp/h**: Text decoder supporting multiple character encodings
- **js_textencoder.cpp/h**: Text encoder for string to UTF-8 conversion
- **js_types.cpp/h**: Type checking utilities for various JavaScript types
- **js_uuid.cpp/h**: UUID generation (random) and parsing functionality
- **native_module_util.cpp/h**: NAPI module registration and export definitions
- **plugin/hispeed_string_plugin.c/h**: Performance-optimized string operations
- **src/util_js.ts**: TypeScript implementations including LRUCache, RationalNumber, Scope, Aspect, etc.
- **test/**: Unit tests for all utility functions

## Building

### Build System
The component uses GN (Generate Ninja) build system with the following build targets:

1. **build_ts_js (action)**: Compiles TypeScript to JavaScript
   - Script: `$util_module/build_ts_js.py`
   - Input: `src/util_js.ts`
   - Output: `util_js.js`

2. **gen_util_abc (es2abc_gen_abc)**: Compiles JavaScript to ABC bytecode
   - Input: `util_js.js`
   - Output: `util.abc`
   - Extra args: `--module`

3. **util_js (gen_obj)**: Generates object file from JavaScript code
   - Output: `util.o` or `util.c` (platform-dependent)

4. **util_abc (gen_obj)**: Generates object file from ABC bytecode
   - Output: `util_abc.o` or `util_abc.c` (platform-dependent)

5. **util (ohos_shared_library)**: Main shared library target
   - Install directory: `module`
   - Module name: `util` (@ohos.util)
   - Security features: Branch protector (pac_ret), CFI, CFI cross-DSO

6. **util_static (ohos_source_set)**: Static source set containing implementation
   - Sources: All C++ implementation files
   - Include dirs: platform_root, ets_util_path, plugin directory

7. **util_packages (group)**: Aggregates all util packages

### Build Process Flow
```
util_js.ts → util_js.js → util.o/util.c → shared library
                     ↓
                util.abc → util_abc.o/util_abc.c → shared library
```

### Build Dependencies
**Standard OpenHarmony:**
- `hilog:libhilog` - Logging framework
- `napi:ace_napi` - Node-API bindings
- `icu:shared_icuuc` - Internationalization components
- `openssl:libcrypto_shared` - Cryptographic operations (UUID generation)
- `c_utils:utils` - Common utilities
- `bounds_checking_function:libsec_shared` - Security functions

**ArkUI X (cross-platform):**
- `plugins_root/libs/napi:napi_${target_os}` - Platform-specific NAPI
- `plugins_root/libs/icu:icu_${target_os}` - Platform-specific ICU
- `plugins_root/libs/securec:sec_${target_os}` - Secure C library
- `$plugins_root/interfaces/native:ace_plugin_util_${target_os}` - Plugin utilities

### Compiler Configuration
- C++ Standard: Default (uses platform-specific standards)
- Security: Branch protector (pac_ret), CFI enabled, CFI cross-DSO
- Platform defines: `__ARKUI_CROSS__`, ANDROID_PLATFORM, IOS_PLATFORM
- Optimization: -g3 for debug builds in tests

### Build Commands
```bash
# Build util module
./build.sh --product-name <product_name> --build-target util

# Build util packages group
./build.sh --product-name <product_name> --build-target util_packages

# Build tests
./build.sh --product-name <product_name> --build-target test_util_unittest
```

## Test Suite

### Test Structure
The component has a dedicated test directory with comprehensive unit tests:

1. **test_util.cpp**: Tests for core utilities
   - TextEncoder/TextDecoder functionality
   - Base64 encoding/decoding
   - String decoder operations
   - Type checking utilities
   - LRUCache operations
   - Rational number operations
   - Scope management
   - Aspect-oriented programming

2. **test_uuid.cpp**: Tests for UUID functionality
   - Random UUID generation
   - Binary UUID generation
   - UUID parsing
   - UUID validation

3. **test_ark.cpp**: ArkTS-specific tests
   - TypeScript interoperability
   - ABC bytecode integration
   - Native bridge functionality

### Test Coverage Areas

#### TextEncoder/TextDecoder
- UTF-8 encoding/decoding
- Multiple character encodings (GBK, Big5, Shift_JIS, etc.)
- Fatal and ignore BOM options
- Stream mode decoding
- Encoding format validation

#### Base64
- Synchronous encode/decode
- Asynchronous encode/decode
- BASIC, MIME, URL-SAFE variants
- String and Uint8Array conversions
- MIME format line breaks (76 characters)

#### Type Checking
- All JavaScript type checks (ArrayBuffer, TypedArray, Map, Set, etc.)
- Boxed primitives detection
- Promise, Proxy, Generator checks
- Module namespace detection

#### LRUCache/LRUCache
- Put/Get operations
- Cache eviction
- Hit rate calculation
- Capacity updates
- Clear, contains, remove operations
- Statistics (hits, misses, evictions)

#### RationalNumber
- Construction from integers
- Parse from string ("1:2" or "1/2" format)
- Comparison operations
- Arithmetic validation
- Finite, NaN, Zero checks
- Numerator/denominator access

#### Scope/ScopeHelper
- Range creation and validation
- Contains operations
- Clamp operations
- Intersection/Expansion
- Lower/upper limit access

#### Aspect
- Before/After advice
- Replace method functionality
- Static and instance method support
- Async/sync method handling

#### Additional Utilities
- printf/format string formatting
- UUID generation and parsing
- Error code to string conversion
- Hash calculation
- Stack trace retrieval
- promisify/promiseWrapper
- callbackWrapper

## Dependency

### Internal Dependencies
- **Subsystem**: `commonlibrary`
- **Component**: `ets_utils`
- **Module**: Part of `js_util_module` alongside `collections`, `container`, `json`, and `stream`
- **Shared Build Scripts**: Uses `$util_module/build_ts_js.py` from parent directory

### External Dependencies

#### Core Dependencies
1. **NAPI (ace_napi)**
   - Purpose: Provides Node-API interface for native module registration
   - Usage: Module initialization, JS/ABC code embedding, type checking
   - Version: Compatible with OpenHarmony NAPI specification

2. **HiLog (libhilog)**
   - Purpose: System logging framework
   - Usage: Error logging and debugging
   - Integration: Used for logging error conditions and debug information

3. **ICU (shared_icuuc)**
   - Purpose: International Components for Unicode
   - Usage: Character encoding/decoding, text conversion
   - Integration: Essential for TextEncoder/TextDecoder multi-encoding support

4. **OpenSSL (libcrypto_shared)**
   - Purpose: Cryptographic library
   - Usage: Random number generation for UUIDs
   - Integration: Provides cryptographically secure random values

5. **libuv (uv_static)**
   - Purpose: Asynchronous I/O library
   - Usage: Async operations in tests
   - Integration: Event loop for async Base64 operations

6. **SecureC (libsec_shared)**
   - Purpose: Secure C library functions
   - Usage: Safe string operations (memset_s, etc.)
   - Integration: Memory safety and bounds checking

7. **c_utils (utils)**
   - Purpose: Common C utilities
   - Usage: Utility functions and helpers
   - Integration: Shared utility functions across OpenHarmony

#### Platform-Specific Dependencies (ArkUI X)
- **Platform-specific NAPI**: Android/iOS NAPI implementations
- **Platform-specific ICU**: ICU libraries for Android/iOS
- **Platform-specific SecureC**: Secure C libraries for Android/iOS
- **Plugin Utilities**: ACE plugin utilities for cross-platform support

### Runtime Dependencies
- **ArkTS Runtime**: Must support standard JavaScript/ArkTS APIs
- **Global Object**: Access to global scope for requireInternal utility
- **FinalizationRegistry**: For AutoFinalizerCleaner functionality
- **Reflect API**: For Aspect method manipulation
- **Map/Set**: For LRUCache implementation
- **Uint8Array**: For Base64 and TextEncoder operations

### External Directory Interactions
1. **Build System**: Interacts with GN build configuration and es2abc compiler
2. **Runtime Core**: Integrates with ArkTS runtime's object model
3. **Framework Integration**: Installed to `module` directory
4. **Platform Helpers**: Uses platform_root for util_helper and jni_helper
5. **util_module**: Shares build_ts_js.py script with sibling modules
6. **Plugin System**: Integrates with hispeed_string_plugin for performance

## Boundaries

### What Can Be Done Within This Component

#### ✅ Allowed Operations
1. **Text Encoding/Decoding**
   - Implement TextEncoder for string to UTF-8 conversion
   - Implement TextDecoder for byte to string conversion with encoding support
   - Support multiple character encodings (UTF-8, GBK, Big5, Shift_JIS, etc.)
   - Handle BOM (Byte Order Mark) options
   - Implement stream mode for continuous decoding
   - Validate encoding formats

2. **Base64 Operations**
   - Implement synchronous Base64 encode/decode
   - Implement asynchronous Base64 encode/decode
   - Support BASIC, MIME, and URL-SAFE variants
   - Handle Uint8Array and string conversions
   - Add line breaks for MIME format (76 characters)
   - Remove line breaks for decoding

3. **Type Checking**
   - Implement type checking for all JavaScript types
   - Check for ArrayBuffer, TypedArray, DataView
   - Check for Map, Set, WeakMap, WeakSet
   - Check for Promise, Proxy, Generator
   - Check for boxed primitives (Number, String, Boolean, Symbol)
   - Check for module namespace objects

4. **String Decoder**
   - Implement byte stream to string conversion
   - Handle incomplete byte sequences
   - Support multiple encodings
   - Manage internal buffer state

5. **UUID Operations**
   - Generate random UUIDs (v4)
   - Generate binary UUIDs
   - Parse UUID strings to byte arrays
   - Validate UUID format
   - Use cryptographically secure random numbers

6. **Caching Mechanisms**
   - Implement LRUBuffer/LRUCache (Least Recently Used)
   - Support put, get, remove operations
   - Implement cache eviction policies
   - Track statistics (hits, misses, evictions)
   - Support capacity updates
   - Implement contains, isEmpty, clear operations

7. **Rational Number Operations**
   - Implement RationalNumber class
   - Parse from integer pairs
   - Parse from strings ("1:2" or "1/2" format)
   - Compare rational numbers
   - Check equality
   - Check for finite, NaN, zero values
   - Access numerator and denominator
   - Convert to floating point value
   - Calculate greatest common divisor

8. **Scope Management**
   - Implement Scope and ScopeHelper classes
   - Create ranges with lower/upper limits
   - Check if values are within range
   - Clamp values to range
   - Intersect ranges
   - Expand ranges
   - Validate limit ordering

9. **Aspect-Oriented Programming**
   - Implement addBefore advice
   - Implement addAfter advice
   - Implement replace method functionality
   - Support static and instance methods
   - Handle async and sync methods
   - Use Reflect API for method manipulation

10. **String Formatting**
    - Implement printf-style formatting
    - Support format specifiers (%d, %s, %f, %o, %O, %i, %j, %c)
    - Format integers, floats, strings, objects
    - Handle arrays and nested structures
    - Implement format string parsing

11. **Error Handling**
    - Implement error code to string conversion
    - Support uv errno conversion
    - Throw BusinessError with error codes
    - Validate parameters and throw appropriate errors

12. **Promise/Callback Conversion**
    - Implement promisify for callback-based functions
    - Implement promiseWrapper
    - Implement callbackWrapper
    - Handle success and error cases

13. **Utility Functions**
    - Implement hash calculation for objects
    - Get main thread stack trace
    - Get object property descriptors
    - Generate random values

14. **Build Configuration**
    - Configure TypeScript compilation
    - Set up JS to ABC bytecode compilation
    - Manage platform-specific dependencies
    - Configure security features (CFI, branch protector)

15. **Native Module Registration**
    - Implement NAPI module registration
    - Embed JS and ABC code
    - Export classes and functions
    - Handle module initialization

16. **Performance Optimization**
    - Implement hispeed_string_plugin for fast string ops
    - Use SecureC for safe memory operations
    - Optimize critical paths

### What Cannot Be Done Within This Component

#### ❌ Prohibited Operations
1. **Network I/O**
   - Do NOT implement network sockets or HTTP clients
   - Do NOT perform network requests or API calls
   - Do NOT handle network protocols
   - Network I/O handled by dedicated network modules

2. **File System I/O**
   - Do NOT implement file reading or writing
   - Do NOT access file system directly
   - Do NOT implement file watching
   - File I/O handled by dedicated I/O modules

3. **UI or Rendering**
   - Do NOT create or manipulate UI components
   - Do NOT handle rendering or graphics
   - UI/Rendering handled by ACE/ArkUI framework

4. **System API Calls**
   - Do NOT directly call system-level APIs
   - Do NOT access hardware interfaces (except through OpenSSL for random)
   - Do NOT modify system settings
   - System access handled by system capability modules

5. **Database Operations**
   - Do NOT implement database connections
   - Do NOT perform SQL queries
   - Do NOT manage database transactions
   - Database operations handled by dedicated database modules

6. **Advanced Cryptography**
   - Do NOT implement encryption algorithms beyond UUID random generation
   - Do NOT implement hashing algorithms (except simple object hash)
   - Do NOT implement digital signatures
   - Advanced crypto handled by dedicated crypto modules
   - Only use OpenSSL for random number generation

7. **Thread Pool Management**
   - Do NOT manage thread lifecycle or concurrency
   - Do NOT implement thread synchronization primitives
   - Thread management handled by concurrent modules (taskpool, worker)
   - Use async/promises for asynchronous operations

8. **Cross-Module Dependencies**
   - Do NOT create dependencies on other utility modules
   - Maintain module isolation and independence
   - Only use built-in JavaScript/ArkTS capabilities
   - Only depend on explicitly listed external libraries

9. **Breaking API Changes**
   - Do NOT change existing method signatures without deprecation
   - Do NOT remove public APIs without migration path
   - Maintain backward compatibility

10. **Global State Pollution**
    - Do NOT pollute global namespace beyond module exports
    - Do NOT create global variables or singletons (except module exports)
    - Keep state encapsulated within class instances

11. **Custom Memory Management**
    - Do NOT implement custom allocators or memory pools
    - Do NOT manually manage memory (use new/delete and garbage collector)
    - Memory management handled by runtime
    - Use SecureC functions for safe operations

12. **Security Bypass**
    - Do NOT disable or bypass security features
    - Do NOT expose internal implementation details
    - Do NOT compromise utility function security
    - Maintain proper input validation and sanitization

13. **Compression/Encryption**
    - Do NOT implement compression algorithms
    - Do NOT implement encryption/decryption
    - Such operations should be in separate specialized modules

14. **Regular Expressions**
    - Do NOT add complex regex-based parsing
    - Keep regex usage minimal and well-tested
    - Only use for simple validation (like UUID format)

15. **Date/Time Operations**
   - Do NOT implement date/time manipulation
   - Do NOT handle time zones
   - Date/time operations handled by built-in Date object

### Architectural Constraints
1. **Module Independence**: Util module should be standalone and not depend on other util modules
2. **Type Safety**: Use TypeScript for type safety where possible
3. **Error Handling**: Use BusinessError with error codes (401 for parameter errors, 10200002 for syntax errors)
4. **Performance**: Optimize critical paths with native code (hispeed_string_plugin)
5. **Security**: Use SecureC for C operations, enable CFI and branch protector
6. **Platform Support**: Support both standard OpenHarmony and ArkUI X platforms
7. **Encoding Support**: Must support multiple character encodings via ICU
8. **Async Operations**: Use Promise-based APIs for async operations
9. **Validation**: Always validate input parameters
10. **Documentation**: Maintain clear API documentation

### Modification Guidelines
When extending this component:
1. Follow TypeScript best practices and type safety
2. Validate all input parameters and throw BusinessError appropriately
3. Use consistent error codes (401 for parameter errors, 10200002 for syntax errors)
4. Test all functionality with unit tests
5. Use SecureC functions for C operations
6. Support both synchronous and asynchronous operations where appropriate
7. Maintain backward compatibility with existing APIs
8. Test on all supported platforms (standard OpenHarmony, ArkUI X Android/iOS)
9. Consider performance implications for frequently called functions
10. Use native code only for performance-critical operations
11. Document all public APIs clearly
12. Handle edge cases and error conditions gracefully
13. Use ICU for all text encoding/decoding operations
14. Use OpenSSL only for cryptographic random number generation
15. Keep module dependencies minimal and explicit
