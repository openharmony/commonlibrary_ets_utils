# Collections Component Documentation

## Name
collections

## Purpose
The collections component provides NAPI (Native API) bindings for ArkTS collection types. It acts as a bridge between native C++ code and ArkTS/JavaScript runtime, exposing sendable collection classes and typed arrays for use in concurrent and multi-threaded scenarios. The component enables efficient data sharing across threads through sendable data structures.

## Primary Language
C++ (Native module implementation using NAPI)

## Directory Structure
```
collections/
├── BUILD.gn                          # Build configuration file
└── native_module_collections.cpp     # Native module source code
```

### File Descriptions
- **BUILD.gn**: GN build script defining the shared library compilation, dependencies, and installation configuration
- **native_module_collections.cpp**: C++ implementation that exports ArkTS collection constructors to the NAPI module interface

## Building

### Build System
The component uses the GN (Generate Ninja) build system with the following build targets:

1. **collections (ohos_shared_library)**: The main shared library target
   - Install directory: `module/arkts`
   - Module name: `arkts.collections` (@ohos.arkts.collections)
   - Security features: PAC_RET branch protection, CFI sanitization

2. **collections_static (ohos_source_set)**: Static source set containing the implementation
   - Include directories: `include`, `ets_util_path`
   - Conditional dependencies for ArkUI X vs standard OpenHarmony

### Build Dependencies
**Standard OpenHarmony:**
- `hilog:libhilog` - Logging framework
- `napi:ace_napi` - Node-API bindings
- `bounds_checking_function:libsec_shared` - Secure bounds checking

**ArkUI X (cross-platform):**
- ICU library
- NAPI library (platform-specific)
- SecureC library (platform-specific)

### Build Commands
```bash
# Build the collections module
./build.sh --product-name <product_name> --build-target collections
```

## Test Suite

### Current Status
No dedicated test directory exists for the collections component within the `collections/` directory.

### Integration Testing
Tests for the collections functionality are integrated as part of the broader ets_utils component test suite. According to the bundle.json configuration, component tests may be included in:
- Parent module test suites
- Integration tests with concurrent modules
- ArkTS runtime testing framework

### Recommended Test Areas
When extending this component, ensure tests cover:
- NAPI module registration and initialization
- Collection constructor exposure
- Thread-safe operations for sendable collections
- Typed array data integrity
- Cross-thread data sharing scenarios

## Dependency

### Internal Dependencies
- **Subsystem**: `commonlibrary`
- **Component**: `ets_utils`
- **Module**: Part of `js_util_module` alongside `util`, `container`, `json`, and `stream`

### External Dependencies

#### Core Dependencies
1. **NAPI (ace_napi)**
   - Purpose: Provides Node-API interface for native module registration
   - Usage: Module initialization, property definition, function exposure
   - Version: Compatible with OpenHarmony NAPI specification

2. **HiLog (libhilog)**
   - Purpose: System logging framework
   - Usage: Error logging and debugging information
   - Integration: Used through `tools/log.h` header

3. **Bounds Checking Function (libsec_shared)**
   - Purpose: Security bounds checking for memory operations
   - Usage: Memory safety in native code

#### Platform-Specific Dependencies (ArkUI X)
- **ICU**: Internationalization components
- **Platform-specific NAPI**: Android/iOS NAPI implementations
- **SecureC**: Secure C library replacement

### Runtime Dependencies
- **ArkTS Runtime**: Collection classes must be available in the ArkTS runtime environment
- **ARK_Private API**: Access to `ArkPrivate.Load` for loading BitVector
- **Global Object**: Requires access to global JavaScript runtime context

### External Directory Interactions
1. **Runtime Core**: Interacts with ArkTS runtime for collection class retrieval
2. **Framework Integration**: Installed to `module/arkts` directory for framework loading
3. **NAPI Framework**: Registers with the NAPI module system at startup

## Boundaries

### What Can Be Done Within This Component

#### ✅ Allowed Operations
1. **Export Collection Constructors**
   - Expose ArkTS collection classes to NAPI interface
   - Define module properties for collection access
   - Register module with NAPI system

2. **Supported Collection Types**
   - SendableArray
   - SendableSet
   - SendableMap
   - SendableArrayBuffer
   - Typed Arrays: SendableInt8Array, SendableUint8Array, SendableInt16Array, SendableUint16Array, SendableInt32Array, SendableUint32Array, SendableUint8ClampedArray, SendableFloat32Array
   - BitVector

3. **Module Registration**
   - Implement NAPI module initialization
   - Define export properties
   - Handle module lifecycle

4. **Error Handling**
   - Log initialization failures
   - Validate function availability
   - Report missing collection types

5. **Build Configuration**
   - Configure shared library compilation
   - Set security features (CFI, PAC_RET)
   - Manage platform-specific dependencies

### What Cannot Be Done Within This Component

#### ❌ Prohibited Operations
1. **Collection Implementation**
   - Do NOT implement collection algorithms or data structures
   - Do NOT modify collection behavior or semantics
   - All collection logic resides in ArkTS runtime

2. **Thread Pool Management**
   - Do NOT manage thread lifecycle directly
   - Do NOT implement thread synchronization primitives
   - Thread management handled by concurrent modules

3. **Custom Collections**
   - Do NOT add custom collection types not provided by ArkTS
   - Do NOT create hybrid collection implementations
   - Only expose existing ArkTS collections

4. **Direct Memory Manipulation**
   - Do NOT directly manipulate collection memory
   - Do NOT bypass NAPI interface for collection access
   - Use NAPI functions for all operations

5. **Framework Integration**
   - Do NOT modify framework loading mechanisms
   - Do NOT change module installation paths
   - Installation and loading managed by build system and framework

6. **Type System Modifications**
   - Do NOT alter type definitions or type checking
   - Do NOT implement custom type coercion
   - Type system managed by ArkTS compiler and runtime

7. **Cross-Module Dependencies**
   - Do NOT create direct dependencies on other util modules (util, container, json, stream)
   - Maintain module isolation
   - Use NAPI interface for inter-module communication

8. **API Extensions**
   - Do NOT add custom methods or properties to collections
   - Do NOT modify standard collection APIs
   - Maintain API compatibility with ArkTS specifications

### Architectural Constraints
1. **Bridge Pattern**: This component is strictly a bridge layer, not an implementation layer
2. **Read-Only Access**: Collections are retrieved from global scope, not created here
3. **Runtime Dependency**: Component is useless without ArkTS runtime providing collection classes
4. **Minimal Logic**: Keep logic minimal; only perform validation and registration
5. **No State Maintenance**: Do not maintain persistent state; state managed by ArkTS runtime

### Modification Guidelines
When extending this component:
1. Only add new collection types if they exist in ArkTS runtime
2. Follow existing pattern for exposing new constructors
3. Maintain backward compatibility with existing exports
4. Update documentation for any new collections
5. Ensure proper error handling for all new exports
6. Test on all supported platforms (standard OpenHarmony, ArkUI X)
