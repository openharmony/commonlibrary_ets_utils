/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef BUFFER_JS_BUFFER_H
#define BUFFER_JS_BUFFER_H

#include <numeric>
#include <string>
#include <vector>

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "utils/log.h"

namespace OHOS::buffer {
enum EncodingType {
    ASCII = 1,
    UTF8,
    UTF16LE,
    BASE64,
    BASE64URL,
    LATIN1,
    BINARY,
    HEX
};
class Buffer {
public:
    Buffer() = default;
    virtual ~Buffer();
    void Init(uint32_t size);
    void Init(Buffer *buffer);
    void Init(Buffer *pool, unsigned int poolOffset, unsigned int length);
    void Init(uint8_t *buffer, unsigned int byteOffset, unsigned int length);

    unsigned int GetLength();
    void SetLength(unsigned int len);
    unsigned int GetByteOffset();
    int32_t Get(uint32_t index);
    void Set(uint32_t index, uint8_t value);

    void WriteInt32BE(int32_t value, uint32_t offset);
    void WriteInt32LE(int32_t value, uint32_t offset);
    void WriteUInt32BE(int32_t value, uint32_t offset);
    void WriteUInt32LE(int32_t value, uint32_t offset);

    void ReadBytes(uint8_t *data, uint32_t offset, uint32_t length);
    int32_t ReadInt32BE(uint32_t offset);
    int32_t ReadInt32LE(uint32_t offset);
    uint32_t ReadUInt32BE(uint32_t offset);
    uint32_t ReadUInt32LE(uint32_t offset);

    unsigned int WriteString(std::string value, unsigned int size);
    unsigned int WriteString(std::string value, unsigned int offset, unsigned int length);
    unsigned int WriteString(std::u16string value, unsigned int offset, unsigned int length);
    unsigned int WriteString(std::string value, unsigned int offset, unsigned int length, std::string encoding);

    Buffer *SubBuffer(uint32_t start, uint32_t end);
    uint32_t Copy(Buffer *tBuf, uint32_t tStart, uint32_t sStart, uint32_t sEnd);
    int Compare(Buffer *tBuf, uint32_t targetStart, uint32_t sourceStart, uint32_t length);
    int IndexOf(const char *data, uint32_t offset, uint32_t len);
    int LastIndexOf(const char *data, uint32_t offset, uint32_t len);
    std::string ToBase64(uint32_t start, uint32_t length);
    static EncodingType GetEncodingType(std::string type);
    void SetArray(std::vector<uint8_t> array, unsigned int offset = 0);
    void FillBuffer(Buffer *buffer, unsigned int offset, unsigned int end);
    void FillNumber(std::vector<uint8_t> numbers, unsigned int offset, unsigned int end);
    void FillString(std::string value, unsigned int offset, unsigned int end, std::string encoding);

private:
    uint8_t *GetRaw();
    bool WriteBytes(uint8_t *src, unsigned int size, uint8_t *dest);
    void WriteBE(int32_t value, uint32_t bytes);
    void WriteLE(int32_t value, uint32_t bytes);
    uint32_t ReadBE(uint32_t bytes);
    uint32_t ReadLE(uint32_t bytes);
    std::string Utf16StrToStr(std::u16string value);
    void WriteByte(uint8_t number, uint32_t offset);
    void WriteStringLoop(std::string value, unsigned int offset, unsigned int end, unsigned int length);
    void WriteStringLoop(std::u16string value, unsigned int offset, unsigned int end);
    std::string GetString(std::string value, EncodingType encodingType);

    uint8_t *raw_ = nullptr;
    uint8_t data_[4] = { 0 };
    unsigned int byteOffset_ = 0;
    unsigned int length_ = 0;
    bool needRelease_ = true;
};
} // namespace OHOS::Buffer
#endif // BUFFER_JS_BUFFER_H
