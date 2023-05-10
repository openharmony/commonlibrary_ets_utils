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

#include "commonlibrary/ets_utils/js_api_module/buffer/js_buffer.h"

#include <cmath>
#include <iostream>

#include "commonlibrary/ets_utils/js_api_module/buffer/converter.h"
#include "securec.h"

using namespace std;

namespace OHOS::buffer {
void Buffer::Init(uint32_t size)
{
    raw_ = reinterpret_cast<uint8_t *>(malloc(size));
    if (raw_ == nullptr) {
        HILOG_FATAL("Buffer constructor malloc failed");
        length_ = 0;
    } else {
        length_ = size;
    }
}

void Buffer::Init(Buffer *buffer)
{
    if (buffer != nullptr && buffer->length_ > 0) {
        this->raw_ = reinterpret_cast<uint8_t *>(malloc(buffer->length_));
        if (raw_ == nullptr) {
            HILOG_FATAL("Buffer constructor malloc failed");
        } else {
            this->length_ = buffer->length_;
            if (memcpy_s(raw_, length_, buffer->raw_ + buffer->byteOffset_, length_) != EOK) {
                HILOG_FATAL("Buffer constructor memcpy_s failed");
            }
        }
    }
}

void Buffer::Init(Buffer *pool, unsigned int poolOffset, unsigned int length)
{
    if (pool != nullptr) {
        this->raw_ = pool->GetRaw();
        this->byteOffset_ = poolOffset;
        this->length_ = length;
    }
    this->needRelease_ = false;
}

void Buffer::Init(uint8_t *buffer, unsigned int byteOffset, unsigned int length)
{
    if (buffer != nullptr) {
        this->raw_ = buffer;
        this->length_ = length;
        this->byteOffset_ = byteOffset;
    }
    this->needRelease_ = false;
}

Buffer::~Buffer()
{
    if (raw_ != nullptr && needRelease_) {
        free(raw_);
        raw_ = nullptr;
    }
}

EncodingType Buffer::GetEncodingType(string encode)
{
    if (encode == "hex") {
        return HEX;
    } else if (encode == "base64url") {
        return BASE64URL;
    } else if (encode == "ascii") {
        return ASCII;
    } else if (encode == "base64") {
        return BASE64;
    } else if (encode == "latin1") {
        return LATIN1;
    } else if (encode == "binary") {
        return BINARY;
    } else if (encode == "utf16le") {
        return UTF16LE;
    } else {
        return UTF8;
    }
}

uint8_t *Buffer::GetRaw()
{
    return raw_;
}

unsigned int Buffer::GetLength()
{
    return length_;
}

void Buffer::SetLength(unsigned int len)
{
    length_ = len;
}

unsigned int Buffer::GetByteOffset()
{
    return byteOffset_;
}

Buffer* Buffer::SubBuffer(uint32_t start, uint32_t end)
{
    Buffer *buffer = new Buffer();
    if (buffer != nullptr) {
        buffer->Init(this, this->byteOffset_ + start, (end - start));
    }
    return buffer;
}

uint32_t Buffer::Copy(Buffer *tBuf, uint32_t tStart, uint32_t sStart, uint32_t sEnd)
{
    if (tBuf == nullptr) {
        return 0; // 0 : cannot copy anything
    }
    uint8_t *dest = tBuf->raw_ + tBuf->byteOffset_ + tStart;
    uint32_t tLength = tBuf->length_ - tStart;
    uint8_t *src = this->raw_ + this->byteOffset_ + sStart;
    uint32_t sLength = sEnd - sStart;
    uint32_t len = tLength > sLength ? sLength : tLength;
    WriteBytes(src, len, dest);
    return len;
}

int Buffer::Compare(Buffer *tBuf, uint32_t targetStart, uint32_t sourceStart, uint32_t length)
{
    if (tBuf == nullptr) {
        return 0;
    }
    uint8_t *dest = tBuf->GetRaw() + tBuf->byteOffset_ + targetStart;
    uint8_t *src = this->GetRaw() + this->byteOffset_ + sourceStart;
    return memcmp(dest, src, length);
}

void Buffer::WriteBE(int32_t value, uint32_t bytes)
{
    uint32_t uValue = static_cast<uint32_t>(value);
    for (uint32_t i = bytes; i > 0; i--) {
        uint8_t va = static_cast<uint8_t>(uValue & 0x000000FF);
        data_[i - 1] = va;
        // 8 : shift right 8 bits(i.e 1 byte)
        uValue = uValue >> 8;
    }
}

void Buffer::WriteLE(int32_t value, uint32_t bytes)
{
    if (bytes == 0) {
        return;
    }
    uint32_t uValue = static_cast<uint32_t>(value);
    for (uint32_t i = 0, len = bytes - 1; i <= len; i++) {
        uint8_t va = static_cast<uint8_t>(uValue & 0x000000FF);
        data_[i] = va;
        // 8 : shift right 8 bits(i.e 1 byte)
        uValue = uValue >> 8;
    }
}

uint32_t Buffer::ReadBE(uint32_t bytes)
{
    uint32_t result = 0x0000;
    for (uint32_t i = 0; i < bytes; i++) {
        // 8 : shift left 8 bits(i.e 1 byte)
        result = result << 8;
        result |= data_[i];
    }
    return result;
}

uint32_t Buffer::ReadLE(uint32_t bytes)
{
    uint32_t result = 0x0000;
    if (bytes == 0) {
        return result;
    }
    for (uint32_t i = bytes; i > 0; i--) {
        // 8 : shift left 8 bits(i.e 1 byte)
        result = result << 8;
        result |= data_[i - 1];
    }
    return result;
}

void Buffer::WriteInt32BE(int32_t value, uint32_t offset)
{
    // 4 : 4 bytes(i.e 4 * 8 = 32 bits)
    WriteBE(value, 4);
    // 4 : write 4 bytes
    WriteBytes(data_, 4, raw_ + byteOffset_ + offset);
}

int32_t Buffer::ReadInt32BE(uint32_t offset)
{
    // 4 : 4 bytes(i.e 4 * 8 = 32 bits)
    ReadBytes(data_, offset, 4);
    // 4 : read 4 bytes
    return static_cast<int32_t>(ReadBE(4));
}

void Buffer::WriteInt32LE(int32_t value, uint32_t offset)
{
    // 4 : 4 bytes(i.e 4 * 8 = 32 bits)
    WriteLE(value, 4);
    // 4 : write 4 bytes
    WriteBytes(data_, 4, raw_ + byteOffset_ + offset);
}

int32_t Buffer::ReadInt32LE(uint32_t offset)
{
    // 4 : 4 bytes(i.e 4 * 8 = 32 bits)
    ReadBytes(data_, offset, 4);
    // 4 : read 4 bytes
    return static_cast<int32_t>(ReadLE(4));
}

void Buffer::WriteUInt32BE(int32_t value, uint32_t offset)
{
    // 4 : 4 bytes(i.e 4 * 8 = 32 bits)
    WriteBE(value, 4);
    // 4 : write 4 bytes
    WriteBytes(data_, 4, raw_ + byteOffset_ + offset);
}

uint32_t Buffer::ReadUInt32BE(uint32_t offset)
{
    // 4 : 4 bytes(i.e 4 * 8 = 32 bits)
    ReadBytes(data_, offset, 4);
    // 4 : read 4 bytes
    return ReadBE(4);
}

void Buffer::WriteUInt32LE(int32_t value, uint32_t offset)
{
    // 4 : 4 bytes(i.e 4 * 8 = 32 bits)
    WriteLE(value, 4);
    // 4 : write 4 bytes
    WriteBytes(data_, 4, raw_ + byteOffset_ + offset);
}

uint32_t Buffer::ReadUInt32LE(uint32_t offset)
{
    // 4 : 4 bytes(i.e 4 * 8 = 32 bits)
    ReadBytes(data_, offset, 4);
    // 4 : read 4 bytes
    return ReadLE(4);
}

int32_t Buffer::Get(uint32_t index)
{
    uint8_t value;
    uint32_t count = 1;
    if (memcpy_s(&value, count, raw_ + byteOffset_ + index, count) != EOK) {
        HILOG_FATAL("Buffer get memcpy_s failed");
    }
    return value;
}

void Buffer::Set(uint32_t index, uint8_t value)
{
    WriteByte(value, index);
}

void Buffer::ReadBytes(uint8_t *data, uint32_t offset, uint32_t length)
{
    if (data == nullptr) {
        return;
    }
    if (memcpy_s(data, length, raw_ + byteOffset_ + offset, length) != EOK) {
        HILOG_FATAL("Buffer ReadBytes memcpy_s failed");
    }
}

void Buffer::WriteByte(uint8_t number, uint32_t offset)
{
    WriteBytes(&number, 1, raw_ + byteOffset_ + offset);
}

unsigned int Buffer::WriteString(std::string value, unsigned int size)
{
    uint8_t *str = const_cast<uint8_t *>(reinterpret_cast<const uint8_t *>(value.data()));
    bool isWriteSuccess = WriteBytes(str, size, raw_ + byteOffset_);
    return isWriteSuccess ? size : 0; // 0: write failed
}

unsigned int Buffer::WriteString(string value, unsigned int offset, unsigned int size)
{
    const char *data = value.c_str();
    uint8_t *str = reinterpret_cast<uint8_t *>(const_cast<char *>(data));
    bool isWriteSuccess = WriteBytes(str, size, raw_ + byteOffset_ + offset);
    return isWriteSuccess ? size : 0; // 0: write failed
}

void Buffer::WriteStringLoop(string value, unsigned int offset, unsigned int end, unsigned int length)
{
    if (end - offset <= 0) {
        return;
    }
    const char *data = value.c_str();
    unsigned int loop = length > end - offset ? end - offset : length;

    uint8_t *str = reinterpret_cast<uint8_t *>(const_cast<char *>(data));
    while (offset < end) {
        if (loop + offset > end) {
            WriteBytes(str, end - offset, raw_ + byteOffset_ + offset);
        } else {
            WriteBytes(str, loop, raw_ + byteOffset_ + offset);
        }
        offset += loop;
    }
}

std::string Buffer::Utf16StrToStr(std::u16string value)
{
    string str = "";
    char16_t *data = const_cast<char16_t *>(reinterpret_cast<const char16_t *>(value.data()));
    for (unsigned int i = 0; i < value.length(); i++) {
        char16_t c = data[i];
        char high = (char)((c >> 8) & 0x00FF);
        char low = (char)(c & 0x00FF);
        str.push_back(low);
        str.push_back(high);
    }
    return str;
}

unsigned int Buffer::WriteString(std::u16string value, unsigned int offset, unsigned int length)
{
    string str = Utf16StrToStr(value);
    return this->WriteString(str, offset, length);
}

void Buffer::WriteStringLoop(std::u16string value, unsigned int offset, unsigned int end)
{
    string str = Utf16StrToStr(value);
    // 2 : utf16 is 2 times of length of ascii
    this->WriteStringLoop(str, offset, end, value.length() * 2);
}

bool Buffer::WriteBytes(uint8_t *src, unsigned int size, uint8_t *dest)
{
    if (src == nullptr || dest == nullptr) {
        return false;
    }
    if (memcpy_s(dest, size, src, size) != EOK) {
        HILOG_FATAL("Buffer WriteBytes memcpy_s failed");
        return false;
    }
    return true;
}

void Buffer::SetArray(vector<uint8_t> array, unsigned int offset)
{
    unsigned int arrLen = array.size();
    unsigned int size = arrLen <= length_ ? arrLen : length_;
    WriteBytes(array.data(), size, raw_ + byteOffset_ + offset);
}

void Buffer::FillBuffer(Buffer *buffer, unsigned int offset, unsigned int end)
{
    if (buffer == nullptr) {
        return;
    }
    if (end - offset <= 0) {
        return;
    }
    unsigned int loop = buffer->GetLength() > end - offset ? end - offset : buffer->GetLength();

    while (offset < end) {
        WriteBytes(buffer->GetRaw() + buffer->byteOffset_, loop, raw_ + byteOffset_ + offset);
        offset += loop;
    }
}

void Buffer::FillNumber(vector<uint8_t> numbers, unsigned int offset, unsigned int end)
{
    if (end - offset <= 0) {
        return;
    }
    unsigned int loop = numbers.size() > end - offset ? end - offset : numbers.size();

    while (offset < end) {
        WriteBytes(numbers.data(), loop, raw_ + byteOffset_ + offset);
        offset += loop;
    }
}

std::string Buffer::GetString(std::string value, EncodingType encodingType)
{
    string str = "";
    switch (encodingType) {
        case ASCII:
        case LATIN1:
            str = Utf8ToUtf16BEToANSI(value);
            break;
        case UTF8:
            str = value;
            break;
        case BASE64:
        case BASE64URL:
            str = Base64Decode(value);
            break;
        case BINARY:
            str = Utf8ToUtf16BEToANSI(value);
            break;
        default:
            break;
    }
    return str;
}

void Buffer::FillString(string value, unsigned int offset, unsigned int end, string encoding)
{
    EncodingType encodingType = GetEncodingType(encoding);
    if (encodingType == UTF16LE) {
        u16string u16Str = Utf8ToUtf16BE(value);
        this->WriteStringLoop(u16Str, offset, end);
    } else {
        string str = GetString(value, encodingType);
        this->WriteStringLoop(str, offset, end, str.length());
    }
}

unsigned int Buffer::WriteString(string value, unsigned int offset, unsigned int length, string encoding)
{
    EncodingType encodingType = GetEncodingType(encoding);
    unsigned int lengthWrote = 0;
    if (encodingType == UTF16LE) {
        u16string u16Str = Utf8ToUtf16BE(value);
        lengthWrote = this->WriteString(u16Str, offset, length);
    } else {
        string str = GetString(value, encodingType);
        lengthWrote = this->WriteString(str, offset, length);
    }
    return lengthWrote;
}

std::string Buffer::ToBase64(uint32_t start, uint32_t length)
{
    uint8_t data[length];
    ReadBytes(data, start, length);
    return Base64Encode(reinterpret_cast<const unsigned char*>(data), length);
}

int Buffer::IndexOf(const char *data, uint32_t offset, uint32_t len)
{
    if (data == nullptr) {
        return -1;
    }
    uint8_t sData[length_ - offset];
    ReadBytes(sData, offset, length_ - offset);
    int index = FindIndex(sData, reinterpret_cast<uint8_t *>(const_cast<char *>(data)), length_ - offset, len);
    return index == -1 ? index : (offset + index);
}

int Buffer::LastIndexOf(const char *data, uint32_t offset, uint32_t len)
{
    if (data == nullptr) {
        return -1;
    }
    uint8_t sData[length_ - offset];
    ReadBytes(sData, offset, length_ - offset);
    return FindLastIndex(sData, reinterpret_cast<uint8_t *>(const_cast<char *>(data)), length_ - offset, len);
}
} // namespace OHOS::Buffer
