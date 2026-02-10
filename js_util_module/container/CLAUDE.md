# Container Component Documentation

## Name
container

## Purpose
The container component provides a comprehensive set of linear and non-linear data structure collections for ArkTS/JavaScript applications. It implements efficient, type-safe collection classes including ArrayList, LinkedList, Vector, Stack, Queue, Deque, HashMap, TreeMap, HashSet, TreeSet, PlainArray, LightweightMap, LightweightSet, and Struct. These collections offer developers optimized data storage and manipulation capabilities with various performance characteristics and use cases, ranging from dynamic arrays to tree-based maps and sets.

## Primary Language
- **TypeScript** (Primary implementation language for collection logic)
- **C++** (Native module registration and NAPI bindings)

## Directory Structure
```
container/
├── BUILD.gn                          # Build configuration
├── tsconfig.json                     # TypeScript configuration
├── build_ts_js.py                    # TypeScript to JavaScript compiler script
├── arraylist/
│   ├── js_arraylist.ts              # ArrayList implementation
│   └── native_module_arraylist.cpp  # ArrayList native module registration
├── deque/
│   ├── js_deque.ts
│   └── native_module_deque.cpp
├── hashmap/
│   ├── js_hashmap.ts
│   └── native_module_hashmap.cpp
├── hashset/
│   ├── js_hashset.ts
│   └── native_module_hashset.cpp
├── lightweghtmap/
│   ├── js_lightweightmap.ts
│   └── native_module_lightweightmap.cpp
├── lightweightset/
│   ├── js_lightweightset.ts
│   └── native_module_lightweightset.cpp
├── linkedlist/
│   ├── js_linkedlist.ts
│   └── native_module_linkedlist.cpp
├── list/
│   ├── js_list.ts
│   └── native_module_list.cpp
├── plainarray/
│   ├── js_plainarray.ts
│   └── native_module_plainarray.cpp
├── queue/
│   ├── js_queue.ts
│   └── native_module_queue.cpp
├── stack/
│   ├── js_stack.ts
│   └── native_module_stack.cpp
├── struct/
│   ├── js_struct.ts                 # Base class for dictionary structures
│   └── native_module_struct.cpp
├── treemap/
│   ├── js_treemap.ts
│   └── native_module_treemap.cpp
├── treeset/
│   ├── js_treeset.ts
│   └── native_module_treeset.cpp
└── vector/
    ├── js_vector.ts
    └── native_module_vector.cpp
```

### File Descriptions
- **BUILD.gn**: Main build configuration defining the template for all collection modules
- **tsconfig.json**: TypeScript compiler configuration
- **build_ts_js.py**: Python script to compile TypeScript files to JavaScript
- **js_*.ts**: TypeScript implementation files containing collection class logic
- **native_module_*.cpp**: C++ files for NAPI module registration and embedded JS/ABC code

## Building

### Build System
The component uses GN (Generate Ninja) build system with a template-based approach for all collections:

1. **build_js_ts (action)**: Compiles TypeScript to JavaScript
   - Script: `build_ts_js.py`
   - Outputs: `js_*.js` files for all 15 collection types

2. **container_lib (template)**: Template for each collection library
   - Generates object files from JS code
   - Compiles JS to ABC bytecode using es2abc
   - Creates static source set with native module
   - Builds shared library for final output

3. **container_packages (group)**: Aggregates all collection packages

### Build Process Flow
```
.ts file → .js file → .o/.c object file → shared library
              ↓
         .abc bytecode → .o/.c object file → shared library
```

### Build Targets
Each of the 15 collections builds independently:
- arraylist, deque, queue, vector, linkedlist, list, stack, struct
- treemap, treeset, hashmap, hashset
- lightweightmap, lightweightset, plainarray

### Build Dependencies
**Standard OpenHarmony:**
- `hilog:libhilog` - Logging framework
- `napi:ace_napi` - Node-API bindings

**ArkUI X (cross-platform):**
- `plugins_root/libs/icu:icu_${target_os}` - ICU library
- `plugins_root/libs/napi:napi_${target_os}` - Platform-specific NAPI
- `plugins_root/libs/securec:sec_${target_os}` - Secure C library

### Compiler Configuration
- C++ Standard: C++17
- Warnings: `-Wno-deprecated-declarations`
- Security: PAC_RET branch protection, CFI sanitization

### Build Commands
```bash
# Build all container modules
./build.sh --product-name <product_name> --build-target container

# Build specific container (e.g., ArrayList)
./build.sh --product-name <product_name> --build-target arraylist

# Build container packages group
./build.sh --product-name <product_name> --build-target container_packages
```

## Test Suite

### Current Status
No dedicated test directory exists for the container component within the `container/` directory.

### Integration Testing
Tests for container functionality are integrated as part of the broader ets_utils component test suite. According to bundle.json, tests may be included in:
- Parent module test suites
- Application-level integration tests
- ArkTS runtime testing framework

### Recommended Test Areas
When extending this component, ensure tests cover:
- Collection creation and initialization
- CRUD operations (add, insert, remove, get, set)
- Edge cases (empty collections, boundary conditions)
- Type safety and error handling
- Performance characteristics (time complexity)
- Memory management and capacity handling
- Iterator and traversal functionality
- Comparison and sorting operations
- Serialization/deserialization (if applicable)

## Dependency

### Internal Dependencies
- **Subsystem**: `commonlibrary`
- **Component**: `ets_utils`
- **Module**: Part of `js_util_module` alongside `collections`, `util`, `json`, and `stream`

### External Dependencies

#### Core Dependencies
1. **NAPI (ace_napi)**
   - Purpose: Provides Node-API interface for native module registration
   - Usage: Module initialization, property definition, JS/ABC code embedding
   - Version: Compatible with OpenHarmony NAPI specification

2. **HiLog (libhilog)**
   - Purpose: System logging framework
   - Usage: Error logging and debugging (through struct module)
   - Integration: Used via errorUtil from `util.struct`

3. **es2abc Compiler**
   - Purpose: Compile JavaScript to ArkTS bytecode
   - Usage: Convert .js files to .abc for runtime execution
   - Integration: Part of build toolchain

#### Platform-Specific Dependencies (ArkUI X)
- **ICU**: Internationalization components for Unicode support
- **Platform-specific NAPI**: Android/iOS NAPI implementations
- **SecureC**: Secure C library replacement for memory safety

### Runtime Dependencies
- **ArkTS Runtime**: Must support standard JavaScript collections API
- **ArkPrivate API**: Optional fast path through `globalThis.ArkPrivate.Load()`
- **Global Object**: Access to global scope for fallback implementations
- **util.struct Module**: Base dictionary class used by HashMap/HashSet

### External Directory Interactions
1. **Build System**: Interacts with GN build configuration and es2abc compiler
2. **Runtime Core**: Collections integrate with ArkTS runtime's object model
3. **Framework Integration**: Installed to `module/util` directory
4. **util.struct**: HashMap/HashSet extend DictionaryClass from struct module
5. **ArkPrivate API**: Attempts to load optimized implementations when available

## Boundaries

### What Can Be Done Within This Component

#### ✅ Allowed Operations
1. **Collection Implementation**
   - Implement collection classes with full CRUD operations
   - Define collection-specific algorithms (sorting, searching, traversal)
   - Implement capacity management and dynamic resizing
   - Create iterator protocols for traversal
   - Implement type-safe generic interfaces

2. **Supported Collection Types**
   - **Linear Collections**: ArrayList, LinkedList, Vector, Stack, Queue, Deque, List
   - **Map Collections**: HashMap, TreeMap, LightweightMap
   - **Set Collections**: HashSet, TreeSet, LightweightSet
   - **Special Collections**: PlainArray, Struct (base class)

3. **Error Handling**
   - Validate input parameters and types
   - Check range and boundary conditions
   - Throw appropriate error messages
   - Bind context validation for method calls
   - New target validation for constructors

4. **Proxy Integration**
   - Use Proxy for property interception (as in ArrayList HandlerArrayList)
   - Implement custom get/set/deleteProperty traps
   - Define property descriptors and ownKeys behavior
   - Control prototype modification attempts

5. **Build Configuration**
   - Configure TypeScript compilation settings
   - Set up JS to ABC bytecode compilation
   - Manage platform-specific dependencies
   - Configure security features (CFI, PAC_RET)

6. **Module Registration**
   - Implement NAPI module registration for each collection
   - Embed JS and ABC code in native modules
   - Export collection classes as default exports
   - Handle fallback implementations when ArkPrivate unavailable

7. **Utility Functions**
   - Implement comparison and sorting algorithms
   - Create conversion methods (toArray, toString, etc.)
   - Implement capacity management (resize, trim, increaseCapacity)
   - Provide clone and copy functionality

### What Cannot Be Done Within This Component

#### ❌ Prohibited Operations
1. **Thread Pool Management**
   - Do NOT manage thread lifecycle or concurrency
   - Do NOT implement thread synchronization primitives
   - Thread management handled by concurrent modules (taskpool, worker)

2. **Network or I/O Operations**
   - Do NOT perform network requests or HTTP calls
   - Do NOT access file system directly
   - Do NOT implement database connections
   - I/O operations handled by dedicated I/O modules

3. **UI or Rendering**
   - Do NOT create or manipulate UI components
   - Do NOT handle rendering or graphics
   - UI/Rendering handled by ACE/ArkUI framework

4. **System API Calls**
   - Do NOT directly call system-level APIs
   - Do NOT access hardware interfaces
   - Do NOT modify system settings
   - System access handled by system capability modules

5. **Cross-Module Dependencies**
   - Do NOT create dependencies on collections from other modules
   - Maintain module isolation and independence
   - Only depend on util.struct for base dictionary functionality

6. **Breaking API Changes**
   - Do NOT change existing method signatures without deprecation
   - Do NOT remove public APIs without migration path
   - Maintain backward compatibility

7. **Global State Pollution**
   - Do NOT pollute global namespace beyond necessary exports
   - Do NOT create global variables or singletons
   - Keep state encapsulated within collection instances

8. **Custom Memory Management**
   - Do NOT implement custom allocators or memory pools
   - Do NOT manually manage memory (use ArkTS garbage collector)
   - Memory management handled by runtime

9. **Security Bypass**
   - Do NOT disable or bypass security features
   - Do NOT expose internal implementation details
   - Maintain proper encapsulation and access control

### Architectural Constraints
1. **Module Independence**: Each collection module is independently buildable and deployable
2. **Optional Optimization**: Use ArkPrivate fast path when available, fallback to TS implementation
3. **Type Safety**: Maintain TypeScript type safety throughout implementation
4. **Performance**: Optimize for typical use cases while maintaining flexibility
5. **Compatibility**: Support both standard OpenHarmony and ArkUI X platforms
6. **Embedding**: JS and ABC code embedded in native modules for distribution
7. **Proxy Usage**: Leverage ES6 Proxy for property interception where appropriate
8. **Error Consistency**: Use consistent error checking patterns across all collections

### Modification Guidelines
When extending this component:
1. Follow existing patterns for new collection types
2. Implement comprehensive error checking using errorUtil patterns
3. Use TypeScript generics for type-safe implementations
4. Consider using Proxy for custom property behavior
5. Provide both JS fallback and potential ArkPrivate optimization paths
6. Maintain consistent API naming and behavior across collections
7. Update BUILD.gn template if adding new collection types
8. Ensure compatibility with util.struct base class where applicable
9. Test on all supported platforms (standard OpenHarmony, ArkUI X)
10. Document performance characteristics and use case recommendations
