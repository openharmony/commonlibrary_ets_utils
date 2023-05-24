/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef NATIVE_MODULE_BUFFER_H
#define NATIVE_MODULE_BUFFER_H

#include "native_engine.h"

#include <string>
#include <vector>

#include "js_blob.h"
#include "js_buffer.h"

namespace OHOS::buffer {
    using namespace std;
    struct PromiseInfo {
        napi_env env = nullptr;
        napi_async_work worker = nullptr;
        napi_deferred deferred = nullptr;
        napi_value promise = nullptr;
        Blob* jsBlob = nullptr;
        napi_value arrayBuffer = nullptr;
        napi_value string = nullptr;
    };
    void FinalizeBufferCallback(napi_env env, void *finalizeData, void *finalizeHint);
    void FinalizeBlobCallback(napi_env env, void *finalizeData, void *finalizeHint);
    string GetStringUtf8(napi_env env, napi_value strValue);
    string GetStringASCII(napi_env env, napi_value strValue);
    string GetString(napi_env env, EncodingType encodingType, napi_value strValue);
    napi_value FromStringUtf8(napi_env env, napi_value thisVar, napi_value str);
    napi_value FromStringASCII(napi_env env, napi_value thisVar, napi_value str, uint32_t size);
    u16string GetStringUtf16LE(napi_env env, napi_value strValue);
    napi_value FromStringUtf16LE(napi_env env, napi_value thisVar, napi_value str);
    string GetStringBase64(napi_env env, napi_value str);
    napi_value FromStringBase64(napi_env env, napi_value thisVar, napi_value str, uint32_t size);
    string GetStringHex(napi_env env, napi_value str);
    napi_value FromStringHex(napi_env env, napi_value thisVar, napi_value str);
    napi_value FromString(napi_env env, napi_callback_info info);
    vector<uint8_t> GetArray(napi_env env, napi_value arr);
    napi_value BlobConstructor(napi_env env, napi_callback_info info);
    napi_value GetBufferWrapValue(napi_env env, napi_value thisVar, Buffer *buffer);
    napi_value BufferConstructor(napi_env env, napi_callback_info info);
    Buffer *GetValueOffsetAndBuf(napi_env env, napi_callback_info info, int32_t *pValue, uint32_t *pOffset);
    Buffer *GetOffsetAndBuf(napi_env env, napi_callback_info info, uint32_t *pOffset);
    napi_value WriteInt32BE(napi_env env, napi_callback_info info);
    napi_value ReadInt32BE(napi_env env, napi_callback_info info);
    napi_value SetArray(napi_env env, napi_callback_info info);
    napi_value GetLength(napi_env env, napi_callback_info info);
    napi_value GetByteOffset(napi_env env, napi_callback_info info);
    napi_value WriteString(napi_env env, napi_callback_info info);
    napi_value FillString(napi_env env, napi_callback_info info);
    napi_value FillNumbers(napi_env env, napi_callback_info info);
    napi_value FillBuffer(napi_env env, napi_callback_info info);
    napi_value Utf8ByteLength(napi_env env, napi_callback_info info);
    napi_value GetBufferData(napi_env env, napi_callback_info info);
    napi_value Get(napi_env env, napi_callback_info info);
    napi_value Set(napi_env env, napi_callback_info info);
    napi_value WriteInt32LE(napi_env env, napi_callback_info info);
    napi_value ReadInt32LE(napi_env env, napi_callback_info info);
    napi_value WriteUInt32BE(napi_env env, napi_callback_info info);
    napi_value ReadUInt32BE(napi_env env, napi_callback_info info);
    napi_value WriteUInt32LE(napi_env env, napi_callback_info info);
    napi_value ReadUInt32LE(napi_env env, napi_callback_info info);
    napi_value SubBuffer(napi_env env, napi_callback_info info);
    napi_value Copy(napi_env env, napi_callback_info info);
    napi_value Compare(napi_env env, napi_callback_info info);
    napi_value ToUtf8(napi_env env, napi_callback_info info);
    napi_value ToBase64(napi_env env, napi_callback_info info);
    napi_value IndexOf(napi_env env, napi_callback_info info);
    napi_value Utf8StringToNumbers(napi_env env, napi_callback_info info);
    void CopiedBlobToArrayBuffer(napi_env env, napi_status status, void *data);
    void CopiedBlobToString(napi_env env, napi_status status, void *data);
    napi_value ArrayBufferAsync(napi_env env, napi_callback_info info);
    napi_value TextAsync(napi_env env, napi_callback_info info);
    napi_value GetBytes(napi_env env, napi_callback_info info);
    napi_value BufferInit(napi_env env, napi_value exports);
    napi_value BlobInit(napi_env env, napi_value exports);
    napi_value Init(napi_env env, napi_value exports);
}
#endif // NATIVE_MODULE_BUFFER_H