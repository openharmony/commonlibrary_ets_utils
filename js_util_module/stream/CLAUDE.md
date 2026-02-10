# Stream Component Documentation

## Name
stream

## Purpose
The stream component provides a comprehensive stream processing framework for ArkTS/JavaScript applications. It implements four core stream types - Readable, Writable, Duplex, and Transform - enabling efficient data flow control, backpressure management, and event-driven data processing. The component supports text encoding/decoding, pipe operations for stream chaining, and flow control mechanisms like pause/resume, making it suitable for handling large datasets and real-time data streams.

## Primary Language
- **TypeScript** (Primary implementation language for stream classes)
- **C++** (Native module registration and NAPI bindings)

## Directory Structure
```
stream/
├── BUILD.gn                          # Build configuration
├── tsconfig.json                     # TypeScript configuration
├── stream_gen_obj.gni               # GN template for object generation
├── native_module_stream.cpp           # Native module registration
└── src/
    └── stream_js.ts                  # Stream implementation
```

### File Descriptions
- **BUILD.gn**: GN build configuration defining compilation targets and dependencies
- **tsconfig.json**: TypeScript compiler configuration
- **stream_gen_obj.gni**: GN template for generating object files from JS and ABC code
- **native_module_stream.cpp**: C++ file for NAPI module registration and embedded JS/ABC code
- **src/stream_js.ts**: TypeScript implementation of Readable, Writable, Duplex, and Transform stream classes

## Building

### Build System
The component uses GN (Generate Ninja) build system with the following build targets:

1. **build_ts_js (action)**: Compiles TypeScript to JavaScript
   - Script: `$util_module/build_ts_js.py`
   - Input: `src/stream_js.ts`
   - Output: `stream_js.js`

2. **gen_stream_abc (es2abc_gen_abc)**: Compiles JavaScript to ABC bytecode
   - Input: `stream_js.js`
   - Output: `stream.abc`
   - Extra args: `--module`

3. **stream_js (gen_obj)**: Generates object file from JavaScript code
   - Output: `stream.o` or `stream.c` (platform-dependent)

4. **stream_abc (gen_obj)**: Generates object file from ABC bytecode
   - Output: `stream_abc.o` or `stream_abc.c` (platform-dependent)

5. **stream (ohos_shared_library)**: Main shared library target
   - Install directory: `module/util`
   - Module name: `stream` (@ohos.util.stream)
   - No explicit security features configured

6. **stream_static (ohos_source_set)**: Static source set containing implementation
   - Sources: `native_module_stream.cpp`
   - Include dirs: `include`

7. **stream_packages (group)**: Aggregates all stream packages

### Build Process Flow
```
stream_js.ts → stream_js.js → stream.o/stream.c → shared library
                         ↓
                    stream.abc → stream_abc.o/stream_abc.c → shared library
```

### Build Dependencies
**Standard OpenHarmony:**
- `hilog:libhilog` - Logging framework
- `napi:ace_napi` - Node-API bindings

**ArkUI X (cross-platform):**
- `plugins_root/libs/napi:napi_${target_os}` - Platform-specific NAPI

### Compiler Configuration
- C++ Standard: Default (not explicitly specified in config)
- Security: No explicit security features configured
- Platform defines: ANDROID_PLATFORM, IOS_PLATFORM (for ArkUI X)

### Build Commands
```bash
# Build stream module
./build.sh --product-name <product_name> --build-target stream

# Build stream packages group
./build.sh --product-name <product_name> --build-target stream_packages
```

## Test Suite

### Current Status
No dedicated test directory exists for stream component within `stream/` directory.

### Integration Testing
Tests for stream functionality are integrated as part of broader ets_utils component test suite. According to bundle.json configuration, component tests may be included in:
- Parent module test suites
- Application-level integration tests
- ArkTS runtime testing framework

### Recommended Test Areas
When extending this component, ensure tests cover:
- Readable stream: read(), push(), pause(), resume(), pipe(), unpipe()
- Writable stream: write(), end(), cork(), uncork(), setDefaultEncoding()
- Duplex stream: Combined read/write operations
- Transform stream: doTransform(), doFlush() implementations
- Event handling: on(), off() for all event types
- Flow control: pause/resume, high watermark, drain events
- Pipe operations: Connecting Readable to Writable streams
- Encoding/decoding: Various encoding types (UTF-8, ASCII, GBK, etc.)
- Error handling: BusinessError with various error codes
- Backpressure: writableNeedDrain, writableLength, writableHighWatermark
- Callback handling: Single execution, multiple callback errors
- End-of-stream handling: end events, closed state

## Dependency

### Internal Dependencies
- **Subsystem**: `commonlibrary`
- **Component**: `ets_utils`
- **Module**: Part of `js_util_module` alongside `collections`, `container`, `json`, and `util`

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

3. **es2abc Compiler**
   - Purpose: Compile JavaScript to ArkTS bytecode
   - Usage: Convert .js files to .abc for runtime execution
   - Integration: Part of build toolchain

4. **util Module**
   - Purpose: Provides TextEncoder and StringDecoder utilities
   - Usage: Required via `requireNapi('util')`
   - Integration: Used for encoding/decoding stream data

#### Platform-Specific Dependencies (ArkUI X)
- **Platform-specific NAPI**: Android/iOS NAPI implementations

### Runtime Dependencies
- **ArkTS Runtime**: Must support standard stream-like API patterns
- **Global Object**: Access to global scope for requireNapi utility
- **util Module**: Must provide TextEncoder and StringDecoder
- **Event Loop**: Relies on Promise.resolve() for async operation scheduling
- **setTimeout/setInterval**: Used for async callback execution

### External Directory Interactions
1. **Build System**: Interacts with GN build configuration and es2abc compiler
2. **Runtime Core**: Integrates with ArkTS runtime's object model
3. **Framework Integration**: Installed to `module/util` directory
4. **util Module**: Requires TextEncoder and StringDecoder from util module
5. **util_module**: Uses shared build_ts_js.py script from parent directory

## Boundaries

### What Can Be Done Within This Component

#### ✅ Allowed Operations
1. **Stream Implementation**
   - Implement Readable stream with data reading capabilities
   - Implement Writable stream with data writing capabilities
   - Implement Duplex stream combining read and write
   - Implement Transform stream for data transformation
   - Support event-driven architecture with EventEmitter

2. **Flow Control**
   - Implement pause/resume for Readable streams
   - Implement backpressure with high watermark
   - Implement drain events for Writable streams
   - Manage buffer sizes and flow state
   - Implement cork/uncork for write buffering

3. **Data Operations**
   - Read data chunks from Readable streams
   - Write data chunks to Writable streams
   - Transform data in Transform streams
   - Push data into Readable buffers
   - Handle encoding/decoding conversions

4. **Event Handling**
   - Implement on() for event listener registration
   - Implement off() for event listener removal
   - Support multiple event types (data, end, error, close, drain, etc.)
   - Emit events with proper data flow

5. **Pipe Operations**
   - Implement pipe() to connect Readable to Writable
   - Implement unpipe() to disconnect streams
   - Handle automatic flow control in piped streams
   - Manage multiple pipe destinations

6. **Encoding Support**
   - Support multiple encodings (UTF-8, ASCII, GBK, ISO-8859, etc.)
   - Use TextEncoder for string to binary conversion
   - Use StringDecoder for binary to string conversion
   - Validate encoding types

7. **Error Handling**
   - Implement BusinessError class with error codes
   - Handle stream-specific errors (write after end, multiple callbacks, etc.)
   - Emit error events to listeners
   - Throw errors when appropriate

8. **Buffer Management**
   - Manage internal buffers for both read and write streams
   - Implement high watermark for backpressure
   - Track buffer lengths
   - Implement buffer flushing logic

9. **Lifecycle Management**
   - Implement doInitialize() for stream initialization
   - Implement doRead() for data production
   - Implement doWrite() for data consumption
   - Implement doWritev() for batch writes
   - Implement doTransform() for data transformation
   - Implement doFlush() for final data flush

10. **Build Configuration**
    - Configure TypeScript compilation settings
    - Set up JS to ABC bytecode compilation
    - Manage platform-specific dependencies
    - Use GN templates for object generation

11. **Module Registration**
    - Implement NAPI module registration
    - Embed JS and ABC code in native module
    - Export stream classes as default export
    - Handle module initialization

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
   - Do NOT access hardware interfaces
   - Do NOT modify system settings
   - System access handled by system capability modules

5. **Database Operations**
   - Do NOT implement database connections
   - Do NOT perform SQL queries
   - Do NOT manage database transactions
   - Database operations handled by dedicated database modules

6. **Compression/Encryption**
   - Do NOT implement compression algorithms (gzip, zlib, etc.)
   - Do NOT implement encryption/decryption
   - Such operations should be in separate specialized modules

7. **Thread Pool Management**
   - Do NOT manage thread lifecycle or concurrency
   - Do NOT implement thread synchronization primitives
   - Thread management handled by concurrent modules (taskpool, worker)
   - Use Promise.resolve() and setTimeout() for async, not threads

8. **Cross-Module Dependencies**
   - Do NOT create dependencies on other utility modules (except util for TextEncoder/Decoder)
   - Maintain module isolation and independence
   - Only use built-in JavaScript/ArkTS capabilities

9. **Breaking API Changes**
   - Do NOT change existing method signatures without deprecation
   - Do NOT remove public APIs without migration path
   - Maintain backward compatibility with Node.js-like stream API

10. **Global State Pollution**
    - Do NOT pollute global namespace beyond module exports
    - Do NOT create global variables or singletons
    - Keep state encapsulated within stream instances

11. **Custom Memory Management**
    - Do NOT implement custom allocators or memory pools
    - Do NOT manually manage memory (use ArkTS garbage collector)
    - Memory management handled by runtime

12. **Security Bypass**
    - Do NOT disable or bypass security features
    - Do NOT expose internal implementation details
    - Do NOT compromise stream processing security
    - Maintain proper input validation and sanitization

13. **Async/Await in Synchronous Methods**
    - Do NOT use async/await in synchronous stream methods
    - Use Promise.resolve() and setTimeout() for async operations
    - Maintain synchronous API surface with async callbacks

### Architectural Constraints
1. **Event-Driven**: All stream operations are event-driven through EventEmitter
2. **Node.js Compatibility**: Maintain compatibility with Node.js stream API patterns
3. **Abstract Base Classes**: Readable, Writable, Duplex, Transform are abstract base classes
4. **Template Method Pattern**: Subclasses implement doRead(), doWrite(), doTransform(), doFlush()
5. **Backpressure Management**: Use high watermark and drain events for flow control
6. **Encoding Support**: Must support multiple text encodings via TextEncoder/Decoder
7. **Callback-Based**: Use callback pattern for async operations, not promises
8. **No Direct I/O**: Streams are abstract data flow mechanisms, not I/O implementations
9. **Buffer Management**: Internal buffering with high watermark for performance
10. **Error Propagation**: Errors propagate through event system and thrown exceptions

### Modification Guidelines
When extending this component:
1. Follow Node.js stream API patterns for compatibility
2. Implement abstract methods (doRead, doWrite, doTransform, doFlush) in subclasses
3. Use EventEmitter for all event handling
4. Maintain proper backpressure management with high watermark
5. Support pause/resume flow control for Readable streams
6. Implement cork/uncork for Writable stream buffering
7. Validate all input parameters and throw BusinessError appropriately
8. Use consistent error codes (401 for parameter errors, 102xxxxx for stream-specific errors)
9. Test pipe operations between different stream types
10. Ensure proper cleanup in error conditions and stream end
11. Use Promise.resolve() and setTimeout() for async operation scheduling
12. Test on all supported platforms (standard OpenHarmony, ArkUI X)
13. Document all event types and their data formats
14. Consider performance implications for large data chunks
15. Handle encoding conversions properly for international character sets
