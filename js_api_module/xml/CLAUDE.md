# XML Component Documentation

## Name
xml

## Purpose
The xml component provides XML (Extensible Markup Language) parsing and serialization capabilities for ArkTS/JavaScript applications. It offers two main classes: XmlSerializer for creating XML documents by writing elements, attributes, and other XML constructs, and XmlPullParser for parsing existing XML documents and extracting information from them. The component supports UTF-8 encoding, namespaces, comments, CDATA sections, document type declarations, and various XML entity types. It serves as a fundamental utility for XML data processing within the OpenHarmony ecosystem.

## Primary Language
- **TypeScript** (Primary implementation for API interfaces and wrapper classes)
- **C++** (Native XML parsing and serialization implementation)
- **C** (Minimal usage for compatibility)

## Directory Structure
```
xml/
├── BUILD.gn                          # Build configuration
├── tsconfig.json                     # TypeScript configuration
├── js_xml.cpp                       # XML serialization and parsing implementation
├── js_xml.h                         # XML class definitions (XmlSerializer, XmlPullParser)
├── js_xml_dynamic.cpp               # Dynamic XML serializer implementation
├── js_xml_dynamic.h                 # Dynamic XML serializer class definitions
├── native_module_xml.cpp            # Native module registration and exports
├── native_module_xml.h              # NAPI module definitions
├── src/
│   └── js_xml.ts                    # TypeScript wrapper classes and interfaces
└── test/
    ├── BUILD.gn                      # Test build configuration
    ├── test_ark.cpp                  # ArkTS-related tests
    ├── test_xml.cpp                  # Core XML functionality tests
    ├── test_xml.h                    # Test helper functions and utilities
    └── test.h                        # Test infrastructure with Ark runtime environment setup
```

### File Descriptions
- **BUILD.gn**: GN build configuration defining compilation targets and dependencies
- **tsconfig.json**: TypeScript compiler configuration
- **js_xml.cpp/h**: Core XML implementation containing XmlSerializer and XmlPullParser classes with full XML parsing and serialization logic
- **js_xml_dynamic.cpp/h**: Dynamic XML serializer implementation with dynamic buffer management
- **native_module_xml.cpp/h**: NAPI module registration and export definitions
- **src/js_xml.ts**: TypeScript wrapper classes (XmlSerializer, XmlPullParser, XmlDynamicSerializer) with parameter validation and error handling
- **test/**: Unit tests for all XML functionality

## Building

### Build System
The component uses GN (Generate Ninja) build system with the following build targets:

1. **build_ts_js (action)**: Compiles TypeScript to JavaScript
   - Script: `$ets_util_path/js_api_module/build_ts_js.py`
   - Input: `src/js_xml.ts`
   - Output: `js_xml.js`

2. **gen_xml_abc (es2abc_gen_abc)**: Compiles JavaScript to ABC bytecode
   - Input: `js_xml.js`
   - Output: `xml.abc`
   - Extra args: `--module`

3. **xml_js (gen_obj)**: Generates object file from JavaScript code
   - Input: `js_xml.js`
   - Output: `xml.o` or `xml.c` (platform-dependent)

4. **xml_abc (gen_obj)**: Generates object file from ABC bytecode
   - Input: `xml.abc`
   - Output: `xml_abc.o` or `xml_abc.c` (platform-dependent)

5. **xml (ohos_shared_library)**: Main shared library target
   - Install directory: `module`
   - Module name: `xml` (@ohos.xml)
   - Security features: Branch protector (pac_ret), CFI, CFI cross-DSO

6. **xml_static (ohos_source_set)**: Static source set containing implementation
   - Sources: js_xml.cpp, js_xml_dynamic.cpp, native_module_xml.cpp
   - Include dirs: include, ets_util_path

7. **xml_packages (group)**: Aggregates all xml packages

### Build Process Flow
```
js_xml.ts → js_xml.js → xml.o/xml.c → shared library
                   ↓
              xml.abc → xml_abc.o/xml_abc.c → shared library
```

### Build Dependencies
**Standard OpenHarmony:**
- `hilog:libhilog` - Logging framework
- `napi:ace_napi` - Node-API bindings
- `icu:shared_icuuc` - Internationalization components
- `bounds_checking_function:libsec_shared` - Security functions
- `c_utils:utils` - Common utilities (non-standard systems only)

**ArkUI X (cross-platform):**
- `$plugins_root/libs/napi:napi_${target_os}` - Platform-specific NAPI
- `$plugins_root/libs/icu:icu_${target_os}` - Platform-specific ICU
- `$plugins_root/libs/securec:sec_${target_os}` - Secure C library

### Compiler Configuration
- C++ Standard: Default (uses platform-specific standards)
- Security: Branch protector (pac_ret), CFI enabled, CFI cross-DSO
- Platform defines: `ANDROID_PLATFORM` (for Android)
- Optimization: -g3 for debug builds in tests

### Build Commands
```bash
# Build xml module
./build.sh --product-name <product_name> --build-target xml

# Build xml packages group
./build.sh --product-name <product_name> --build-target xml_packages

# Build tests
./build.sh --product-name <product_name> --build-target test_xml_unittest
```

## Test Suite

### Test Structure
The component has a dedicated test directory with comprehensive unit tests:

1. **test_xml.cpp**: Tests for core XML functionality
   - XmlSerializer operations
   - XmlPullParser parsing
   - Namespace handling
   - Entity resolution
   - Tag parsing
   - Attribute handling
   - Comment and CDATA processing

2. **test_ark.cpp**: ArkTS-specific tests
   - TypeScript interoperability
   - ABC bytecode integration
   - Native bridge functionality

### Test Coverage Areas

#### XmlSerializer
- XML declaration setting
- Element start/end tag writing
- Attribute setting
- Namespace declaration
- Comment writing
- CDATA section writing
- Text content writing
- Document type declaration
- Empty element writing
- Error handling and validation

#### XmlPullParser
- Document parsing with callbacks
- Event type detection (START_DOCUMENT, END_DOCUMENT, START_TAG, END_TAG, TEXT, etc.)
- Element name retrieval
- Attribute parsing and counting
- Namespace resolution
- Text content extraction
- Line and column number tracking
- Depth tracking
- Whitespace detection
- Empty element detection
- Entity resolution (built-in entities: lt, gt, amp, apos, quot)
- Document type declaration parsing
- Comment parsing
- CDATA section parsing
- Processing instruction parsing
- Relaxed parsing mode
- Custom callback functions for tokens, tags, and attributes

#### XmlDynamicSerializer
- Dynamic buffer management
- Output retrieval as ArrayBuffer
- All XmlSerializer operations supported

#### Error Handling
- Buffer overflow detection
- Invalid XML format handling
- Encoding validation
- Parameter validation
- Namespace consistency checking
- Entity resolution failures
- Malformed XML recovery (relaxed mode)

## Dependency

### Internal Dependencies
- **Subsystem**: `commonlibrary`
- **Component**: `ets_utils`
- **Module**: Part of `js_api_module`
- **Shared Build Scripts**: Uses `$ets_util_path/js_api_module/build_ts_js.py` from parent directory

### External Dependencies

#### Core Dependencies
1. **NAPI (ace_napi)**
   - Purpose: Provides Node-API interface for native module registration
   - Usage: Module initialization, JS/ABC code embedding, parameter validation
   - Version: Compatible with OpenHarmony NAPI specification

2. **HiLog (libhilog)**
   - Purpose: System logging framework
   - Usage: Error logging and debugging
   - Integration: Used for logging error conditions and debug information

3. **ICU (shared_icuuc)**
   - Purpose: International Components for Unicode
   - Usage: Character encoding support (UTF-8)
   - Integration: Essential for proper text encoding handling

4. **SecureC (libsec_shared)**
   - Purpose: Secure C library functions
   - Usage: Safe string operations
   - Integration: Memory safety and bounds checking

5. **c_utils (utils)**
   - Purpose: Common C utilities
   - Usage: Utility functions and helpers
   - Integration: Shared utility functions across OpenHarmony (non-standard systems)

#### Platform-Specific Dependencies (ArkUI X)
- **Platform-specific NAPI**: Android/iOS NAPI implementations
- **Platform-specific ICU**: ICU libraries for Android/iOS
- **Platform-specific SecureC**: Secure C libraries for Android/iOS

### Runtime Dependencies
- **ArkTS Runtime**: Must support standard JavaScript/ArkTS APIs
- **Global Object**: Access to global scope for requireInternal utility
- **ArrayBuffer/DataView**: For XmlSerializer output buffer
- **Object**: For parsing options and callbacks

### External Directory Interactions
1. **Build System**: Interacts with GN build configuration and es2abc compiler
2. **Runtime Core**: Integrates with ArkTS runtime's object model
3. **Framework Integration**: Installed to `module` directory
4. **ets_util_path**: Uses shared build scripts and includes from parent directory
5. **Plugin System**: Integrates with platform-specific libraries for ArkUI X

## Boundaries

### What Can Be Done Within This Component

#### ✅ Allowed Operations
1. **XML Serialization (XmlSerializer)**
   - Write XML declaration with version and encoding
   - Write element start and end tags
   - Set element attributes
   - Write element content (text)
   - Write CDATA sections
   - Write comments
   - Write document type declarations
   - Write empty elements
   - Set namespaces with prefixes
   - Manage buffer output to ArrayBuffer/DataView
   - Validate XML syntax during writing
   - Escape special characters (<, >, &, ', ")

2. **XML Parsing (XmlPullParser)**
   - Parse XML documents from strings
   - Detect and report event types (START_DOCUMENT, END_DOCUMENT, START_TAG, END_TAG, TEXT, CDSECT, COMMENT, DOCDECL, INSTRUCTION, ENTITY_REFERENCE, WHITESPACE)
   - Extract element names
   - Parse and count attributes
   - Retrieve attribute names and values
   - Resolve namespaces and prefixes
   - Extract text content
   - Track parsing position (line and column numbers)
   - Track element nesting depth
   - Detect empty elements
   - Detect whitespace-only text
   - Resolve built-in XML entities (lt, gt, amp, apos, quot)
   - Parse document type declarations
   - Parse comments
   - Parse CDATA sections
   - Parse processing instructions
   - Parse internal and external DTD subsets
   - Parse element declarations
   - Parse attribute list declarations
   - Parse entity declarations
   - Parse notation declarations

3. **Dynamic XML Serialization (XmlDynamicSerializer)**
   - Dynamically manage buffer allocation
   - Retrieve output as ArrayBuffer
   - Support all XmlSerializer operations

4. **Event-Based Parsing**
   - Register custom callback functions for parsing events
   - Handle token events with custom logic
   - Handle tag events with custom logic
   - Handle attributes with custom logic

5. **Error Handling**
   - Validate input parameters
   - Detect buffer overflow conditions
   - Report XML syntax errors
   - Handle malformed XML in relaxed mode
   - Throw BusinessError with appropriate error codes

6. **Character Encoding**
   - Support UTF-8 encoding
   - Handle character escaping
   - Validate encoding format

7. **Namespace Management**
   - Declare namespaces
   - Resolve namespace URIs from prefixes
   - Track namespace scopes
   - Handle default namespaces

8. **Build Configuration**
   - Configure TypeScript compilation
   - Set up JS to ABC bytecode compilation
   - Manage platform-specific dependencies
   - Configure security features (CFI, branch protector)

9. **Native Module Registration**
   - Implement NAPI module registration
   - Embed JS and ABC code
   - Export XmlSerializer, XmlPullParser, XmlDynamicSerializer classes
   - Export EventType enum

### What Cannot Be Done Within This Component

#### ❌ Prohibited Operations
1. **Network I/O**
   - Do NOT implement network sockets or HTTP clients
   - Do NOT perform network requests to fetch XML
   - Do NOT handle network protocols
   - Network I/O handled by dedicated network modules

2. **File System I/O**
   - Do NOT implement file reading or writing
   - Do NOT access file system directly to load/save XML
   - File I/O handled by dedicated I/O modules

3. **XML Schema Validation**
   - Do NOT implement XSD (XML Schema Definition) validation
   - Do NOT implement DTD validation beyond parsing
   - Do NOT validate against Relax NG or other schema languages
   - Schema validation handled by dedicated validation modules

5. **XPath/XQuery Support**
   - Do NOT implement XPath query language
   - Do NOT implement XQuery
   - Do NOT provide advanced XML querying capabilities
   - These features handled by specialized XML processing modules

6. **XSLT Transformation**
   - Do NOT implement XSLT stylesheet processing
   - Do NOT transform XML documents using XSLT
   - XSLT transformation handled by specialized modules

7. **DOM/SAX Parser Implementation**
   - Do NOT implement full DOM (Document Object Model) parser
   - Do NOT implement SAX (Simple API for XML) parser
   - This component provides pull parser (XmlPullParser) only

8. **Advanced XML Features**
   - Do NOT implement XML signature processing
   - Do NOT implement XML encryption
   - Do NOT implement SOAP protocol support
   - Do NOT implement XML-RPC
   - Advanced XML features handled by specialized modules

9. **Character Encoding Beyond UTF-8**
   - Do NOT implement other character encodings (GBK, Big5, Shift_JIS, etc.)
   - Only UTF-8 is supported
   - Other encodings handled by text encoding modules

10. **System API Calls**
    - Do NOT directly call system-level APIs
    - Do NOT access hardware interfaces
    - Do NOT modify system settings
    - System access handled by system capability modules

11. **Cross-Module Dependencies**
    - Do NOT create dependencies on other utility modules
    - Maintain module isolation and independence
    - Only use built-in JavaScript/ArkTS capabilities
    - Only depend on explicitly listed external libraries

12. **Breaking API Changes**
    - Do NOT change existing method signatures without deprecation
    - Do NOT remove public APIs without migration path
    - Maintain backward compatibility

13. **Global State Pollution**
    - Do NOT pollute global namespace beyond module exports
    - Do NOT create global variables or singletons (except module exports)
    - Keep state encapsulated within class instances

14. **Custom Memory Management**
    - Do NOT implement custom allocators or memory pools
    - Do NOT manually manage memory (use new/delete and garbage collector)
    - Memory management handled by runtime
    - Use SecureC functions for safe operations

15. **Security Bypass**
    - Do NOT disable or bypass security features
    - Do NOT expose internal implementation details
    - Do NOT compromise XML processing security
    - Maintain proper input validation and sanitization

16. **Performance-Critical Operations**
    - Do NOT implement performance-critical XML processing
    - For large XML files, consider streaming or specialized modules
    - This component is designed for general-purpose XML processing

### Architectural Constraints
1. **Module Independence**: XML module should be standalone and not depend on other API modules
2. **Type Safety**: Use TypeScript for type safety where possible
3. **Error Handling**: Use BusinessError with error codes (401 for parameter errors, 10200064 for empty parameters, 10200066 for encoding errors)
4. **Performance**: Optimize parsing for typical XML document sizes
5. **Security**: Use SecureC for C operations, enable CFI and branch protector
6. **Platform Support**: Support both standard OpenHarmony and ArkUI X platforms
7. **Encoding Support**: Only UTF-8 encoding is supported
8. **Validation**: Always validate input parameters
9. **Documentation**: Maintain clear API documentation
10. **Memory Management**: Use ArrayBuffer/DataView for serialization output, manage buffer boundaries carefully

### Modification Guidelines
When extending this component:
1. Follow TypeScript best practices and type safety
2. Validate all input parameters and throw BusinessError appropriately
3. Use consistent error codes (401 for parameter errors, 10200064 for empty parameters, 10200066 for encoding errors)
4. Test all functionality with unit tests
5. Use SecureC functions for C operations
6. Support both synchronous and asynchronous operations where appropriate
7. Maintain backward compatibility with existing APIs
8. Test on all supported platforms (standard OpenHarmony, ArkUI X Android/iOS)
9. Consider performance implications for frequently called functions
10. Use native code only for performance-critical operations
11. Document all public APIs clearly
12. Handle edge cases and error conditions gracefully
13. Use ICU for text encoding operations (UTF-8 only)
14. Keep module dependencies minimal and explicit
15. Implement proper buffer overflow protection in serialization
16. Support entity resolution according to XML specification
17. Maintain proper namespace scoping during parsing
