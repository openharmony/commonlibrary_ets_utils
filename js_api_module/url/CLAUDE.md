# AGENTS
**Name**: URL
**Purpose**: URL is a URL parsing and manipulation module for ArkTS ecosystem that provides JavaScript APIs for URL handling. It implements standard Web URL APIs with features including:
- URL parsing and construction (protocol, hostname, port, path, query, fragment)
- URLSearchParams API for query parameter management
- URLParams API alternative parameter handling
- Percent encoding and decoding functions
- IPv4 and IPv6 address support
- Multiple protocol support (HTTP, HTTPS, FTP, WS, WSS, FILE, etc.)
- Component extraction and modification
- Standard-compliant URL validation
**Primary Language**: TypeScript with C++ native implementation for performance-critical operations, using N-API bindings for JavaScript interoperability.

## Directory Structure
```text
url/
├── BUILD.gn                                      # Main build configuration
├── tsconfig.json                                 # TypeScript compiler configuration
├── native_module_url.h / native_module_url.cpp   # NAPI module entry point
├── js_url.h / js_url.cpp                         # Native URL implementation
├── url_helper.h                                  # URL helper functions
├── src/
│   └── js_url.ts                                 # TypeScript API implementation
└── test/                                         # Unit tests subdirectory
    ├── BUILD.gn
    ├── test.h                                     # Test header file
    ├── test_ark.cpp                               # Ark runtime tests
    └── test_napi.cpp                              # NAPI tests
```

## Building
The component uses GN build system with BUILD.gn configuration, run the following command:
```bash
# Build the url module (within build system)
./build.sh --product-name <product> --build-target url
```

## Test Suite
To execute code tests, run the following command:
```bash
# Build and generate an executable file
./build.sh --product-name <product> --build-target test_url_unittest
# Send the executable file to the device using hdc
hdc shell send ./out/<product>/tests/unittest/ets_utils/ets_utils/jsapi/url/napi/test_url_unittest /data/local/tmp
# Run the executable file
hdc shell
cd /data/local/tmp/
chmod 777 ./test_url_unittest
./test_url_unittest
```

## Dependency
### Package Dependencies
| Dependency | Purpose | Platform |
|------------|---------|----------|
| `napi:ace_napi` | N-API bindings for JavaScript engine | All |
| `icu:shared_icuuc` | Unicode internationalization components | All |
| `hilog:libhilog` | System logging | All |
| `bounds_checking_function:libsec_shared` | Memory safety functions | All |
| `c_utils:utils` | Common utilities library | Non-standard system |

### External Directory Interactions
- `../../js_sys_module/` - JavaScript system modules for shared utilities
- `../../` - Parent ets_utils module for shared configurations (ets_utils_config.gni)

## Boundaries
### Allowed Operations
- ✅ Add new URL component parsing methods following Web standards
- ✅ Extend protocol support for new URL schemes
- ✅ Improve encoding/decoding performance
- ✅ Add new query parameter manipulation features
- ✅ Extend IPv6 address handling capabilities
- ✅ Improve URL validation and error messages
- ✅ Add new utility functions for URL operations
- ✅ Extend Unicode and internationalization support

### Prohibited Operations
- ❌ Do NOT implement network I/O operations within this module
- ❌ Do NOT add HTTP client/server functionality
- ❌ Do NOT implement browser-specific URL features without standards compliance
- ❌ Do NOT modify shared utility files without coordinating with other modules
- ❌ Do NOT break compatibility with existing URL API contracts (WHATWG URL Standard)
- ❌ Do NOT use platform-specific code without proper build guards
- ❌ Do NOT add protocol-specific validation beyond URL standards
- ❌ Do NOT bypass SecureC library for string operations
