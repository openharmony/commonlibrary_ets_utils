/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef XML_JS_XML_SAX_H
#define XML_JS_XML_SAX_H

#include <libxml/parser.h>
#include <map>
#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <uv.h>
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "native_engine/native_engine.h"
#include "tools/log.h"

namespace OHOS::xml {

class XmlSAXParser;
class XmlSAXParserTestHelper;

// Constants for XML SAX2 attribute parsing
constexpr size_t XML_SAX2_ATTR_ELEMENT_COUNT = 5;
constexpr size_t XML_SAX2_ATTR_LOCALNAME_OFFSET = 0;
constexpr size_t XML_SAX2_ATTR_PREFIX_OFFSET = 1;
constexpr size_t XML_SAX2_ATTR_URI_OFFSET = 2;
constexpr size_t XML_SAX2_ATTR_VALUE_START_OFFSET = 3;
constexpr size_t XML_SAX2_ATTR_VALUE_END_OFFSET = 4;
constexpr size_t XML_NAPI_ATTR_TWO = 2;

enum class SAXCallbackType {
    START_DOCUMENT,
    END_DOCUMENT,
    START_ELEMENT,
    END_ELEMENT,
    CHARACTERS
};

struct SAXCallbackData {
    SAXCallbackType type;
    std::string name;
    std::string namespaceURI;
    std::string qname;
    std::string content;
    std::map<std::string, std::string> attributes;
};

struct AsyncParseWorkData {
    napi_async_work asyncWork;
    napi_deferred deferred;
    XmlSAXParser* parser;
    std::string chunk;
    bool isFinal;
    std::string error;
};

struct ChunkData {
    std::string chunk;
    bool isFinal;
    napi_deferred deferred;
};

/**
 * SAX回调函数结构体，用于存储NAPI回调函数引用
 */
struct SAXCallbackRefs {
    napi_ref startDocumentRef;
    napi_ref endDocumentRef;
    napi_ref startElementRef;
    napi_ref endElementRef;
    napi_ref charactersRef;
    napi_env env;

    SAXCallbackRefs() : startDocumentRef(nullptr), endDocumentRef(nullptr),
                        startElementRef(nullptr), endElementRef(nullptr),
                        charactersRef(nullptr), env(nullptr) {}

    ~SAXCallbackRefs();
};

class XmlSAXParser {
    friend class XmlSAXParserTestHelper;
    
public:
    /**
     * Constructor for XmlSAXParser.
     *
     * @param env The NAPI environment variables.
     */
    explicit XmlSAXParser(napi_env env);

    /**
     * XmlSAXParser destructor.
     */
    ~XmlSAXParser();

    /**
     * Parse XML data chunk from stream (async version).
     *
     * @param env The NAPI environment variables.
     * @param handler The JS object containing SAX callback functions.
     * @param chunk The XML data chunk to parse.
     * @param isFinal Whether this is the final chunk.
     * @return napi_value Returns Promise on success, throws error on failure.
     */
    napi_value Parse(napi_env env, napi_value handler, const std::string& chunk, bool isFinal);

    /**
     * Extract SAX callback functions from JS handler object.
     *
     * @param env The NAPI environment variables.
     * @param handler The JS object containing SAX callback functions.
     * @param callbacks Output parameter for extracted callback references.
     * @return true if all required callbacks are present, false otherwise.
     */
    bool ExtractCallbacks(napi_env env, napi_value handler, SAXCallbackRefs& callbacks);

    /**
     * Get error message if parsing failed.
     *
     * @return Error message string.
     */
    std::string GetError() const;

    uint32_t refCount_ = 1;
    napi_ref ref_ {};

private:
    /**
     * Initialize libxml2 SAX parser context.
     */
    bool InitParserContext();

    /**
     * Cleanup parser context and resources.
     */
    void Cleanup();

    /**
     * SAX callback wrappers for libxml2 with namespace support.
     */
    static void StartDocumentCallback(void* userData);
    static void EndDocumentCallback(void* userData);
    static void StartElementNsCallback(void* userData, const xmlChar* localname,
                                       const xmlChar* prefix, const xmlChar* URI,
                                       int namespaceSize, const xmlChar** namespaces,
                                       int attributeSize, int defaultedSize,
                                       const xmlChar** attributes);
    static void EndElementNsCallback(void* userData, const xmlChar* localname,
                                     const xmlChar* prefix, const xmlChar* URI);
    static void CharactersCallback(void* userData, const xmlChar* ch, int len);

    /**
     * Convert SAX2 attributes array to map.
     * SAX2 attributes format: [localname/prefix, value, localname/prefix, value, ...]
     */
    std::map<std::string, std::string> ConvertSAX2Attributes(const xmlChar** attrs, int attributeSize);

    /**
     * Extract a callback function from JS handler object and create reference.
     *
     * @param env The NAPI environment variables.
     * @param handler The JS object containing callback functions.
     * @param propertyName The name of the callback property.
     * @param callbackRef Output parameter for the created reference.
     */
    void ExtractCallback(napi_env env, napi_value handler, const char* propertyName, napi_ref& callbackRef);

    /**
     * Create a builtin Map object and populate it with attributes.
     *
     * @param env The NAPI environment variables.
     * @param attrMap The map of attributes to populate.
     * @return napi_value Returns Map object on success, nullptr on failure.
     */
    napi_value CreateAttributesMap(napi_env env, const std::map<std::string, std::string>& attrMap);

    static void ExecuteParse(napi_env env, void* data);
    static void CompleteParse(napi_env env, napi_status status, void* data);
    static void AsyncCallback(uv_async_t* handle);
    
    void QueueCallback(const SAXCallbackData& callbackData);
    void ProcessCallbackQueue();
    void CallJSCallbackFromData(napi_env env, const SAXCallbackData& callbackData);
    
    napi_value CreateStringValue(napi_env env, const std::string& str);
    void PrepareElementArgs(napi_env env, const SAXCallbackData& callbackData,
                           std::vector<napi_value>& args, bool includeAttributes);
    void CallStartDocumentCallback(napi_env env);
    void CallEndDocumentCallback(napi_env env);
    void CallStartElementCallback(napi_env env, const SAXCallbackData& callbackData);
    void CallEndElementCallback(napi_env env, const SAXCallbackData& callbackData);
    void CallCharactersCallback(napi_env env, const SAXCallbackData& callbackData);

    bool HandleParseError(napi_env env, AsyncParseWorkData* workData);
    bool HandleParseSuccess(napi_env env, AsyncParseWorkData* workData);
    static void CleanupWorkDataOnClose(uv_handle_t* handle);
    
    void ProcessChunkQueue(ChunkData chunkData);
    static void ExecuteChunkParse(napi_env env, void* data);
    static void CompleteChunkParse(napi_env env, napi_status status, void* data);

    xmlParserCtxtPtr parserCtxt_;
    SAXCallbackRefs* callbacks_;
    std::string error_;
    napi_env env_;
    bool isInitialized_;

    uv_async_t* asyncHandle;
    std::atomic_bool isExecuting_ = false;
    std::mutex dataQueueMutex_;
    std::queue<AsyncParseWorkData*> workDataQueue_;

    std::mutex callbackQueueMutex_;
    std::queue<SAXCallbackData> callbackQueue_;

    friend class XmlSAXParserTestHelper;
};

} // namespace OHOS::xml

#endif // XML_JS_XML_SAX_H
