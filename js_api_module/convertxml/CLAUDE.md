# AGENTS
**Name**: ConvertXML
**Purpose**: ConvertXML is an XML parsing and conversion module for ArkTS ecosystem that provides JavaScript APIs for converting XML to JavaScript objects. It bridges XML processing capabilities with JavaScript applications with features including:
- XML to JavaScript object conversion
- Multiple conversion strategies (standard, fast, large XML)
- Comprehensive XML support (elements, attributes, text, CDATA, comments)
- XML declaration and processing instruction handling
- Configurable parsing options
- Memory optimization for large XML files
- libxml2-based parsing engine
- NAPI bindings for JavaScript interoperability
**Primary Language**: TypeScript with C++ native implementation using libxml2, using N-API bindings for JavaScript interoperability.

## Directory Structure
```text
convertxml/
├── BUILD.gn                                      # Main build configuration
├── tsconfig.json                                 # TypeScript compiler configuration
├── native_module_convertxml.h / native_module_convertxml.cpp  # NAPI module entry point
├── js_convertxml.h / js_convertxml.cpp           # Native XML conversion implementation
├── src/
│   └── js_convertxml.ts                         # TypeScript API implementation
└── test/                                         # Unit tests subdirectory
    ├── BUILD.gn
    ├── test.h                                     # Test base class
    ├── test_convertxml.h / test_convertxml.cpp    # Test helper and implementation
    └── test_ark.cpp                               # Ark runtime tests
```

## Building
The component uses GN build system with BUILD.gn configuration, run the following command:
```bash
# Build the convertxml module (within build system)
./build.sh --product-name <product> --build-target convertxml
```

## Test Suite
To execute code tests, run the following command:
```bash
# Build and generate an executable file
./build.sh --product-name <product> --build-target test_convertxml_unittest
# Send the executable file to device using hdc
hdc shell send ./out/<product>/tests/unittest/ets_utils/ets_utils/jsapi/convertxml/napi/test_convertxml_unittest /data/local/tmp/
# Run the executable file
hdc shell
cd /data/local/tmp/
chmod 777 ./test_convertxml_unittest
./test_convertxml_unittest
```

## Dependency
### Package Dependencies
| Dependency | Purpose | Platform |
|------------|---------|----------|
| `napi:ace_napi` | N-API bindings for JavaScript engine | All |
| `libxml2:libxml2` | XML parsing library | All |
| `hilog:libhilog` | System logging | All |
| `icu:shared_icuuc` | Unicode internationalization components | All |
| `c_utils:utils` | Common utilities library | Non-standard system |

### External Directory Interactions
- `../../js_sys_module/` - JavaScript system modules for shared utilities
- `../xml/` - Related XML module for shared functionality
- `../../` - Parent ets_utils module for shared configurations (ets_utils_config.gni)

## Boundaries
### Allowed Operations
- ✅ Add new XML parsing options and configurations
- ✅ Extend XML feature support (new node types, attributes)
- ✅ Improve conversion performance for various XML sizes
- ✅ Add new utility functions for XML manipulation
- ✅ Extend error handling and validation
- ✅ Optimize memory usage for large XML files
- ✅ Add new conversion strategies
- ✅ Improve character encoding support

### Prohibited Operations
- ❌ Do NOT implement network I/O operations within this module
- ❌ Do NOT modify shared utility files without coordinating with other modules
- ❌ Do NOT bypass libxml2 for XML parsing (use the library as intended)
- ❌ Do NOT implement custom XML parsers
- ❌ Do NOT use platform-specific code without proper build guards
- ❌ Do NOT break compatibility with existing XML conversion API contracts
- ❌ Do NOT modify the XML schema or DTD validation behavior
- ❌ Do NOT implement XML serialization/encoding from JavaScript objects
