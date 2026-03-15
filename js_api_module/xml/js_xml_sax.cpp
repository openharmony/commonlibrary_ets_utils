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

#include "js_xml_sax.h"
#include "securec.h"
#include <memory>

namespace OHOS::xml {

// SAXCallbackRefs destructor implementation
SAXCallbackRefs::~SAXCallbackRefs()
{
    if (env != nullptr) {
        if (startDocumentRef != nullptr) {
            napi_delete_reference(env, startDocumentRef);
            startDocumentRef = nullptr;
        }
        if (endDocumentRef != nullptr) {
            napi_delete_reference(env, endDocumentRef);
            endDocumentRef = nullptr;
        }
        if (startElementRef != nullptr) {
            napi_delete_reference(env, startElementRef);
            startElementRef = nullptr;
        }
        if (endElementRef != nullptr) {
            napi_delete_reference(env, endElementRef);
            endElementRef = nullptr;
        }
        if (charactersRef != nullptr) {
            napi_delete_reference(env, charactersRef);
            charactersRef = nullptr;
        }
    }
}

XmlSAXParser::XmlSAXParser(napi_env env)
    : parserCtxt_(nullptr), callbacks_(nullptr), isInitialized_(false)
{
    asyncHandle = new uv_async_t;
    asyncHandle->data = this;
    uv_loop_t* loop = nullptr;
    napi_get_uv_event_loop(env, &loop);
    uv_async_init(loop, asyncHandle, AsyncCallback);
    env_ = env;
}

XmlSAXParser::~XmlSAXParser()
{
    if (asyncHandle != nullptr) {
        if (!uv_is_closing(reinterpret_cast<uv_handle_t*>(asyncHandle))) {
            uv_close(reinterpret_cast<uv_handle_t*>(asyncHandle), CleanupWorkDataOnClose);
        }
    }
    Cleanup();
    napi_delete_reference(env_, ref_);
}

void XmlSAXParser::Cleanup()
{
    if (parserCtxt_ != nullptr) {
        xmlFreeParserCtxt(parserCtxt_);
        parserCtxt_ = nullptr;
    }

    if (callbacks_ != nullptr) {
        delete callbacks_;
        callbacks_ = nullptr;
    }

    std::queue<AsyncParseWorkData*> tmpQueue;
    {
        std::lock_guard<std::mutex> lock(dataQueueMutex_);
        std::swap(workDataQueue_, tmpQueue);
    }

    while (!tmpQueue.empty()) {
        AsyncParseWorkData* workData = tmpQueue.front();
        tmpQueue.pop();
        if (workData != nullptr) {
            delete workData;
        }
    }

    isInitialized_ = false;
    if (refCount_ > 0) {
        napi_reference_unref(env_, ref_, &refCount_);
    }
}

bool XmlSAXParser::InitParserContext()
{
    if (isInitialized_) {
        return true;
    }

    // Create SAX2 handler structure with namespace support
    static xmlSAXHandler saxHandler = {0};
    saxHandler.initialized = XML_SAX2_MAGIC;
    saxHandler.startDocument = StartDocumentCallback;
    saxHandler.endDocument = EndDocumentCallback;
    saxHandler.startElementNs = StartElementNsCallback;
    saxHandler.endElementNs = EndElementNsCallback;
    saxHandler.characters = CharactersCallback;

    // Create push parser context
    parserCtxt_ = xmlCreatePushParserCtxt(&saxHandler, this, nullptr, 0, nullptr);
    if (parserCtxt_ == nullptr) {
        error_ = "Failed to create XML parser context";
        HILOG_ERROR("XmlSAXParser: %{public}s", error_.c_str());
        return false;
    }

    isInitialized_ = true;
    if (refCount_ == 0) {
        napi_reference_ref(env_, ref_, &refCount_);
    }
    return true;
}

std::string XmlSAXParser::GetError() const
{
    return error_;
}

void XmlSAXParser::ExtractCallback(napi_env env, napi_value handler,
                                   const char* propertyName, napi_ref& callbackRef)
{
    napi_value callback = nullptr;
    napi_status status = napi_get_named_property(env, handler, propertyName, &callback);
    if (status != napi_ok) {
        callbackRef = nullptr;
        return;
    }
    napi_create_reference(env, callback, 1, &callbackRef);
}

bool XmlSAXParser::ExtractCallbacks(napi_env env, napi_value handler, SAXCallbackRefs& callbacks)
{
    if (handler == nullptr) {
        error_ = "Handler object is null";
        return false;
    }

    callbacks.env = env;
    ExtractCallback(env, handler, "startDocument", callbacks.startDocumentRef);
    ExtractCallback(env, handler, "endDocument", callbacks.endDocumentRef);
    ExtractCallback(env, handler, "startElement", callbacks.startElementRef);
    ExtractCallback(env, handler, "endElement", callbacks.endElementRef);
    ExtractCallback(env, handler, "characters", callbacks.charactersRef);

    return true;
}

napi_value XmlSAXParser::Parse(napi_env env, napi_value handler,
                               const std::string& chunk, bool isFinal)
{
    napi_value promise = nullptr;
    napi_deferred deferred = nullptr;
    napi_create_promise(env, &deferred, &promise);

    if (callbacks_ == nullptr) {
        callbacks_ = new SAXCallbackRefs();
        if (!ExtractCallbacks(env, handler, *callbacks_)) {
            delete callbacks_;
            callbacks_ = nullptr;
            HILOG_ERROR("XmlSAXParser::Parse: Failed to extract callbacks: %{public}s", error_.c_str());
            napi_value error = nullptr;
            napi_create_string_utf8(env, error_.c_str(), error_.length(), &error);
            napi_reject_deferred(env, deferred, error);
            return promise;
        }
    }

    if (!InitParserContext()) {
        HILOG_ERROR("XmlSAXParser::Parse: Failed to initialize parser");
        napi_value error = nullptr;
        napi_create_string_utf8(env, error_.c_str(), error_.length(), &error);
        napi_reject_deferred(env, deferred, error);
        Cleanup();
        return promise;
    }

    ChunkData chunkData;
    chunkData.chunk = chunk;
    chunkData.isFinal = isFinal;
    chunkData.deferred = deferred;
    ProcessChunkQueue(chunkData);
    
    return promise;
}

std::map<std::string, std::string> XmlSAXParser::ConvertSAX2Attributes(const xmlChar** attrs, int attributeSize)
{
    std::map<std::string, std::string> attrMap;

    // SAX2 attribute format: each attribute takes 5 elements:
    // [0] localname, [1] prefix, [2] namespace URI, [3] value start, [4] value end
    for (int i = 0; i < attributeSize; i++) {
        int offset = i * XML_SAX2_ATTR_ELEMENT_COUNT;
        const xmlChar* localname = attrs[offset + XML_SAX2_ATTR_LOCALNAME_OFFSET];
        const xmlChar* prefix = attrs[offset + XML_SAX2_ATTR_PREFIX_OFFSET];
        const xmlChar* valueStart = attrs[offset + XML_SAX2_ATTR_VALUE_START_OFFSET];
        const xmlChar* valueEnd = attrs[offset + XML_SAX2_ATTR_VALUE_END_OFFSET];
        
        if (localname != nullptr && valueStart != nullptr && valueEnd != nullptr) {
            // Build the full attribute name (with prefix if present)
            std::string attrName;
            const char* localnameStr = reinterpret_cast<const char*>(localname);

            if (prefix != nullptr && *prefix != 0) {
                const char* prefixStr = reinterpret_cast<const char*>(prefix);
                attrName = prefixStr;
                attrName += ":";
                attrName += localnameStr;
            } else {
                attrName = localnameStr;
            }

            // Extract value using start and end pointers
            const char* valueStartStr = reinterpret_cast<const char*>(valueStart);
            const char* valueEndStr = reinterpret_cast<const char*>(valueEnd);
            std::string value(valueStartStr, valueEndStr - valueStartStr);
            
            attrMap[attrName] = value;
        }
    }

    return attrMap;
}

napi_value XmlSAXParser::CreateAttributesMap(napi_env env, const std::map<std::string, std::string>& attrMap)
{
    napi_value attributesMap = nullptr;
    napi_value global = nullptr;
    napi_value mapConstructor = nullptr;
    napi_get_global(env, &global);
    napi_get_named_property(env, global, "Map", &mapConstructor);
    napi_new_instance(env, mapConstructor, 0, nullptr, &attributesMap);

    napi_value setMethod = nullptr;
    napi_get_named_property(env, attributesMap, "set", &setMethod);

    for (const auto& [key, value] : attrMap) {
        napi_value keyStr = nullptr;
        napi_value valueStr = nullptr;
        napi_create_string_utf8(env, key.c_str(), key.length(), &keyStr);
        napi_create_string_utf8(env, value.c_str(), value.length(), &valueStr);

        napi_value setArgs[2] = {keyStr, valueStr};
        napi_value setResult = nullptr;
        napi_call_function(env, attributesMap, setMethod, XML_NAPI_ATTR_TWO, setArgs, &setResult);
    }

    return attributesMap;
}

void XmlSAXParser::ProcessChunkQueue(ChunkData chunkData)
{
    AsyncParseWorkData* workData = new AsyncParseWorkData();
    workData->parser = this;
    workData->chunk = chunkData.chunk;
    workData->isFinal = chunkData.isFinal;
    workData->asyncWork = nullptr;
    workData->deferred = chunkData.deferred;

    uv_loop_t* loop = nullptr;
    napi_get_uv_event_loop(callbacks_->env, &loop);

    {
        std::lock_guard<std::mutex> lock(dataQueueMutex_);
        workDataQueue_.push(workData);
    }

    napi_value resourceName = nullptr;
    napi_create_string_utf8(callbacks_->env, "XmlSAXParser::Parse", NAPI_AUTO_LENGTH, &resourceName);
    
    napi_create_async_work(callbacks_->env, nullptr, resourceName, ExecuteParse, CompleteParse,
                           workData, &workData->asyncWork);
    
    napi_queue_async_work_with_qos(callbacks_->env, workData->asyncWork, napi_qos_user_initiated);
}

void XmlSAXParser::ExecuteParse(napi_env env, void* data)
{
    XmlSAXParser* parser = static_cast<AsyncParseWorkData*>(data)->parser;
    bool expected = false;
    if (!parser->isExecuting_.compare_exchange_strong(expected, true)) {
        return;
    }
    std::queue<AsyncParseWorkData*> tmpQueue;
    {
        std::lock_guard<std::mutex> lock(parser->dataQueueMutex_);
        std::swap(parser->workDataQueue_, tmpQueue);
    }
    while (!tmpQueue.empty()) {
        AsyncParseWorkData* workData = tmpQueue.front();
        tmpQueue.pop();
        int parseResult = xmlParseChunk(parser->parserCtxt_, workData->chunk.c_str(),
                                        static_cast<int>(workData->chunk.length()),
                                        workData->isFinal ? 1 : 0);
        if (parseResult != 0) {
            workData->error = "XML parsing failed";
            if (parser->parserCtxt_->lastError.message != nullptr) {
                workData->error += ": ";
                workData->error += parser->parserCtxt_->lastError.message;
            }
            HILOG_ERROR("XmlSAXParser::ExecuteParse: %{public}s", workData->error.c_str());
        }
    }
    parser->isExecuting_ = false;
    bool needExecuteAgain = false;
    {
        std::lock_guard<std::mutex> lock(parser->dataQueueMutex_);
        needExecuteAgain = !parser->workDataQueue_.empty();
    }
    if (needExecuteAgain) {
        ExecuteParse(env, data);
    }
}

bool XmlSAXParser::HandleParseError(napi_env env, AsyncParseWorkData* workData)
{
    if (workData->error.empty()) {
        return false;
    }

    napi_value error = nullptr;
    napi_create_string_utf8(env, workData->error.c_str(), workData->error.length(), &error);
    napi_reject_deferred(env, workData->deferred, error);
    return true;
}

bool XmlSAXParser::HandleParseSuccess(napi_env env, AsyncParseWorkData* workData)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    napi_resolve_deferred(env, workData->deferred, result);
    return workData->isFinal;
}

void XmlSAXParser::CleanupWorkDataOnClose(uv_handle_t* handle)
{
    if (handle != nullptr) {
        delete handle;
    }
}

void XmlSAXParser::CompleteParse(napi_env env, napi_status status, void* data)
{
    AsyncParseWorkData* workData = static_cast<AsyncParseWorkData*>(data);
    XmlSAXParser* parser = workData->parser;

    bool hasError = parser->HandleParseError(env, workData);
    bool isFinal = false;
    if (!hasError) {
        isFinal = parser->HandleParseSuccess(env, workData);
    }

    napi_delete_async_work(env, workData->asyncWork);
}

void XmlSAXParser::AsyncCallback(uv_async_t* handle)
{
    XmlSAXParser* parser = static_cast<XmlSAXParser*>(handle->data);
    parser->ProcessCallbackQueue();
}

void XmlSAXParser::QueueCallback(const SAXCallbackData& callbackData)
{
    {
        std::lock_guard<std::mutex> lock(callbackQueueMutex_);
        callbackQueue_.push(callbackData);
    }
    uv_async_send(asyncHandle);
}

void XmlSAXParser::ProcessCallbackQueue()
{
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env_, &scope);

    std::queue<SAXCallbackData> tmpQueue;
    {
        std::lock_guard<std::mutex> lock(callbackQueueMutex_);
        std::swap(callbackQueue_, tmpQueue);
    }

    while (!tmpQueue.empty()) {
        SAXCallbackData callbackData = tmpQueue.front();
        tmpQueue.pop();
        CallJSCallbackFromData(env_, callbackData);
    }

    napi_close_handle_scope(env_, scope);
}

napi_value XmlSAXParser::CreateStringValue(napi_env env, const std::string& str)
{
    napi_value value = nullptr;
    if (!str.empty()) {
        napi_create_string_utf8(env, str.c_str(), str.length(), &value);
    } else {
        napi_get_undefined(env, &value);
    }
    return value;
}

void XmlSAXParser::PrepareElementArgs(napi_env env, const SAXCallbackData& callbackData,
                                      std::vector<napi_value>& args, bool includeAttributes)
{
    napi_value nameValue = CreateStringValue(env, callbackData.name);
    napi_value uriValue = CreateStringValue(env, callbackData.namespaceURI);
    napi_value qnameValue = CreateStringValue(env, callbackData.qname);
    
    args.push_back(nameValue);
    args.push_back(uriValue);
    args.push_back(qnameValue);
    
    if (includeAttributes) {
        napi_value attributesMap = CreateAttributesMap(env, callbackData.attributes);
        args.push_back(attributesMap);
    }
}

void XmlSAXParser::CallStartDocumentCallback(napi_env env)
{
    napi_value callback = nullptr;
    napi_get_reference_value(env, callbacks_->startDocumentRef, &callback);
    
    napi_value global = nullptr;
    napi_get_global(env, &global);
    
    napi_value result = nullptr;
    napi_call_function(env, global, callback, 0, nullptr, &result);
}

void XmlSAXParser::CallEndDocumentCallback(napi_env env)
{
    napi_value callback = nullptr;
    napi_get_reference_value(env, callbacks_->endDocumentRef, &callback);
    
    napi_value global = nullptr;
    napi_get_global(env, &global);
    
    napi_value result = nullptr;
    napi_call_function(env, global, callback, 0, nullptr, &result);

    Cleanup();
}

void XmlSAXParser::CallStartElementCallback(napi_env env, const SAXCallbackData& callbackData)
{
    std::vector<napi_value> args;
    PrepareElementArgs(env, callbackData, args, true);
    
    napi_value callback = nullptr;
    napi_get_reference_value(env, callbacks_->startElementRef, &callback);
    
    napi_value global = nullptr;
    napi_get_global(env, &global);
    
    napi_value result = nullptr;
    napi_call_function(env, global, callback, args.size(), args.data(), &result);
}

void XmlSAXParser::CallEndElementCallback(napi_env env, const SAXCallbackData& callbackData)
{
    std::vector<napi_value> args;
    PrepareElementArgs(env, callbackData, args, false);
    
    napi_value callback = nullptr;
    napi_get_reference_value(env, callbacks_->endElementRef, &callback);
    
    napi_value global = nullptr;
    napi_get_global(env, &global);
    
    napi_value result = nullptr;
    napi_call_function(env, global, callback, args.size(), args.data(), &result);
}

void XmlSAXParser::CallCharactersCallback(napi_env env, const SAXCallbackData& callbackData)
{
    napi_value contentValue = CreateStringValue(env, callbackData.content);
    
    napi_value callback = nullptr;
    napi_get_reference_value(env, callbacks_->charactersRef, &callback);
    
    napi_value global = nullptr;
    napi_get_global(env, &global);
    
    napi_value args[1] = {contentValue};
    napi_value result = nullptr;
    napi_call_function(env, global, callback, 1, args, &result);
}

void XmlSAXParser::CallJSCallbackFromData(napi_env env, const SAXCallbackData& callbackData)
{
    switch (callbackData.type) {
        case SAXCallbackType::START_DOCUMENT:
            if (callbacks_->startDocumentRef != nullptr) {
                CallStartDocumentCallback(env);
            }
            break;
            
        case SAXCallbackType::END_DOCUMENT:
            if (callbacks_->endDocumentRef != nullptr) {
                CallEndDocumentCallback(env);
            }
            break;
            
        case SAXCallbackType::START_ELEMENT:
            if (callbacks_->startElementRef != nullptr) {
                CallStartElementCallback(env, callbackData);
            }
            break;
            
        case SAXCallbackType::END_ELEMENT:
            if (callbacks_->endElementRef != nullptr) {
                CallEndElementCallback(env, callbackData);
            }
            break;
            
        case SAXCallbackType::CHARACTERS:
            if (callbacks_->charactersRef != nullptr) {
                CallCharactersCallback(env, callbackData);
            }
            break;
    }
}

void XmlSAXParser::StartDocumentCallback(void* userData)
{
    auto* parser = static_cast<XmlSAXParser*>(userData);
    SAXCallbackData callbackData;
    callbackData.type = SAXCallbackType::START_DOCUMENT;
    parser->QueueCallback(callbackData);
}

void XmlSAXParser::EndDocumentCallback(void* userData)
{
    auto* parser = static_cast<XmlSAXParser*>(userData);
    SAXCallbackData callbackData;
    callbackData.type = SAXCallbackType::END_DOCUMENT;
    parser->QueueCallback(callbackData);
}

void XmlSAXParser::StartElementNsCallback(void* userData, const xmlChar* localname,
                                          const xmlChar* prefix, const xmlChar* URI,
                                          int namespaceSize, const xmlChar** namespaces,
                                          int attributeSize, int defaultedSize,
                                          const xmlChar** attributes)
{
    auto* parser = static_cast<XmlSAXParser*>(userData);
    
    SAXCallbackData callbackData;
    callbackData.type = SAXCallbackType::START_ELEMENT;
    
    if (localname != nullptr) {
        callbackData.name = reinterpret_cast<const char*>(localname);
    }
    
    if (URI != nullptr) {
        callbackData.namespaceURI = reinterpret_cast<const char*>(URI);
    }
    
    if (prefix != nullptr && *prefix != 0) {
        std::string prefixStr = reinterpret_cast<const char*>(prefix);
        std::string localnameStr = reinterpret_cast<const char*>(localname);
        callbackData.qname = prefixStr + ":" + localnameStr;
    }
    
    if (attributes != nullptr && attributeSize > 0) {
        callbackData.attributes = parser->ConvertSAX2Attributes(attributes, attributeSize);
    }
    
    parser->QueueCallback(callbackData);
}

void XmlSAXParser::EndElementNsCallback(void* userData, const xmlChar* localname,
                                        const xmlChar* prefix, const xmlChar* URI)
{
    auto* parser = static_cast<XmlSAXParser*>(userData);
    
    SAXCallbackData callbackData;
    callbackData.type = SAXCallbackType::END_ELEMENT;
    
    if (localname != nullptr) {
        callbackData.name = reinterpret_cast<const char*>(localname);
    }
    
    if (URI != nullptr) {
        callbackData.namespaceURI = reinterpret_cast<const char*>(URI);
    }
    
    if (prefix != nullptr && *prefix != 0) {
        std::string prefixStr = reinterpret_cast<const char*>(prefix);
        std::string localnameStr = reinterpret_cast<const char*>(localname);
        callbackData.qname = prefixStr + ":" + localnameStr;
    }
    
    parser->QueueCallback(callbackData);
}

void XmlSAXParser::CharactersCallback(void* userData, const xmlChar* ch, int len)
{
    auto* parser = static_cast<XmlSAXParser*>(userData);
    
    SAXCallbackData callbackData;
    callbackData.type = SAXCallbackType::CHARACTERS;
    
    if (ch != nullptr && len > 0) {
        callbackData.content = std::string(reinterpret_cast<const char*>(ch), len);
    }
    
    parser->QueueCallback(callbackData);
}

} // namespace OHOS::xml
