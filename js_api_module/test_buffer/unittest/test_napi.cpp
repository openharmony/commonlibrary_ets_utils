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

#include "test.h"

#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include "converter.h"
#include "js_blob.h"
#include "js_buffer.h"
#include "utils/log.h"


#define ASSERT_CHECK_CALL(call)   \
    {                             \
        ASSERT_EQ(call, napi_ok); \
    }

#define ASSERT_CHECK_VALUE_TYPE(env, value, type)               \
    {                                                           \
        napi_valuetype valueType = napi_undefined;              \
        ASSERT_TRUE(value != nullptr);                          \
        ASSERT_CHECK_CALL(napi_typeof(env, value, &valueType)); \
        ASSERT_EQ(valueType, type);                             \
    }

void FillZero(OHOS::buffer::Buffer *buf, size_t size)
{
    for (size_t i = 0; i < size; i++) {
        buf->Set(i, 0);
    }
}

HWTEST_F(NativeEngineTest, ConstructorTest001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(10);
    ASSERT_EQ(buf->GetLength(), 10);
}

HWTEST_F(NativeEngineTest, ConstructorTest002, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf1 = new OHOS::buffer::Buffer();
    buf1->Init(10);
    OHOS::buffer::Buffer *buf2 = new OHOS::buffer::Buffer();
    buf2->Init(buf1);
    ASSERT_EQ(buf2->GetLength(), 10);
}

HWTEST_F(NativeEngineTest, ConstructorTest003, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *poolBuffer = new OHOS::buffer::Buffer();
    poolBuffer->Init(1024 * 8);
    OHOS::buffer::Buffer *buf2 = new OHOS::buffer::Buffer();
    buf2->Init(poolBuffer, 0, 5);
    ASSERT_EQ(buf2->GetLength(), 5);
    ASSERT_EQ(buf2->GetByteOffset(), 0);

    OHOS::buffer::Buffer *buf3 = new OHOS::buffer::Buffer();
    buf3->Init(poolBuffer, 5, 6);
    ASSERT_EQ(buf3->GetLength(), 6);
    ASSERT_EQ(buf3->GetByteOffset(), 5);
}

HWTEST_F(NativeEngineTest, ConstructorTest004, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf2 = new OHOS::buffer::Buffer();
    uint8_t data[4] = {1, 2, 3, 4};
    buf2->Init(data, 0, 4);
    ASSERT_EQ(buf2->GetLength(), 4);
    ASSERT_EQ(buf2->GetByteOffset(), 0);
}

HWTEST_F(NativeEngineTest, GetLengthTest001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    ASSERT_EQ(buf->GetLength(), 0);
}

HWTEST_F(NativeEngineTest, GetLengthTest002, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(6);
    ASSERT_EQ(buf->GetLength(), 6);
}

HWTEST_F(NativeEngineTest, SetLengthTest001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(6);
    buf->SetLength(7);
    ASSERT_EQ(buf->GetLength(), 7);
}

HWTEST_F(NativeEngineTest, GetByteOffsetTest001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *poolBuffer = new OHOS::buffer::Buffer();
    poolBuffer->Init(1024 * 8);
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(poolBuffer, 2, 5);
    ASSERT_EQ(buf->GetByteOffset(), 2);
}

HWTEST_F(NativeEngineTest, GetAndSetTest001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(3);
    buf->Set(0, 1);
    int32_t value = buf->Get(0);
    ASSERT_EQ(value, 1);
}

HWTEST_F(NativeEngineTest, GetAndSetTest002, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(3);
    buf->Set(0, 1);
    buf->Set(1, 2);
    buf->Set(2, 3);
    int32_t value = buf->Get(2);
    ASSERT_EQ(value, 3);
}

HWTEST_F(NativeEngineTest, WriteInt32BEAndReadInt32BETest001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(4);
    FillZero(buf, 4);
    buf->WriteInt32BE(0x12345678, 0);
    int32_t res = buf->ReadInt32BE(0);
    ASSERT_EQ(res, 0x12345678);
}

HWTEST_F(NativeEngineTest, WriteInt32BEAndReadInt32BETest002, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(5);
    FillZero(buf, 5);
    buf->WriteInt32BE(0x12345678, 1);
    int32_t res = buf->ReadInt32BE(1);
    ASSERT_EQ(res, 0x12345678);
}

HWTEST_F(NativeEngineTest, WriteInt32BEAndReadInt32BETest003, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(5);
    FillZero(buf, 5);
    buf->WriteInt32BE(0x12345678, 1);
    int32_t res = buf->ReadInt32BE(1);
    ASSERT_EQ(res, 0x12345678);
}

HWTEST_F(NativeEngineTest, WriteInt32LEAndReadInt32LETest001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(4);
    FillZero(buf, 4);
    buf->WriteInt32LE(0x12345678, 0);
    int32_t res = buf->ReadInt32LE(0);
    ASSERT_EQ(res, 0x12345678);
    res = buf->ReadInt32BE(0);
    ASSERT_EQ(res, 0x78563412);
}

HWTEST_F(NativeEngineTest, WriteInt32LEAndReadInt32LETest002, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(5);
    FillZero(buf, 5);
    buf->WriteInt32LE(0x12345678, 1);
    int32_t res = buf->ReadInt32LE(0);
    ASSERT_EQ(res, 0x34567800);
}

HWTEST_F(NativeEngineTest, WriteInt32LEAndReadInt32LETest003, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(5);
    FillZero(buf, 5);
    buf->WriteInt32LE(0x12345678, 1);
    int32_t res = buf->ReadInt32LE(1);
    ASSERT_EQ(res, 0x12345678);
}

HWTEST_F(NativeEngineTest, WriteUInt32BEAndReadUInt32BETest001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(4);
    FillZero(buf, 4);
    buf->WriteUInt32BE(0x12345678, 0);
    int32_t res = buf->ReadUInt32BE(0);
    ASSERT_EQ(res, 0x12345678);
}

HWTEST_F(NativeEngineTest, WriteUInt32BEAndReadUInt32BETest002, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(5);
    FillZero(buf, 5);
    buf->WriteUInt32BE(0x12345678, 1);
    int32_t res = buf->ReadUInt32BE(0);
    ASSERT_EQ(res, 0x123456);
}

HWTEST_F(NativeEngineTest, WriteUInt32BEAndReadUInt32BETest003, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(5);
    FillZero(buf, 5);
    buf->WriteUInt32BE(0x12345678, 1);
    int32_t res = buf->ReadUInt32BE(1);
    ASSERT_EQ(res, 0x12345678);
}

HWTEST_F(NativeEngineTest, WriteUInt32LEAndReadUInt32LETest001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(4);
    FillZero(buf, 4);
    buf->WriteUInt32LE(0x12345678, 0);
    int32_t res = buf->ReadUInt32LE(0);
    ASSERT_EQ(res, 0x12345678);
}

HWTEST_F(NativeEngineTest, WriteUInt32LEAndReadUInt32LETest002, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(5);
    FillZero(buf, 5);
    buf->WriteUInt32LE(0x12345678, 1);
    int32_t res = buf->ReadUInt32LE(0);
    ASSERT_EQ(res, 0x34567800);
}

HWTEST_F(NativeEngineTest, WriteUInt32LEAndReadUInt32LETest003, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(5);
    FillZero(buf, 5);
    buf->WriteUInt32LE(0x12345678, 1);
    int32_t res = buf->ReadUInt32LE(1);
    ASSERT_EQ(res, 0x12345678);
}

HWTEST_F(NativeEngineTest, ReadBytesTest001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(4);
    FillZero(buf, 4);
    buf->WriteUInt32BE(0x12345678, 0);
    uint32_t length = buf->GetLength();
    uint8_t data[length];
    buf->ReadBytes(data, 0, length);
    uint8_t res[4] = {0x12, 0x34, 0x56, 0x78};
    for (size_t i = 0; i < length; i++) {
        ASSERT_EQ(data[i], res[i]);
    }
}

HWTEST_F(NativeEngineTest, WriteStringTest001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(10);
    std::string str = "1234567890";
    unsigned int size = buf->WriteString(str, 10);
    ASSERT_EQ(size, 10);
}

HWTEST_F(NativeEngineTest, WriteStringTest002, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(9);
    std::string str = "123456789";
    unsigned int size = buf->WriteString(str, 9);
    uint8_t data[size];
    buf->ReadBytes(data, 0, size);
    uint8_t value = 49;
    for (size_t i = 0; i < size; i++) {
        ASSERT_EQ(data[i], value);
        value++;
    }
}

HWTEST_F(NativeEngineTest, WriteStringTest003, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(9);
    std::string str = "123456789";
    unsigned int size = buf->WriteString(str, 0, 9);
    uint8_t data[size];
    buf->ReadBytes(data, 0, size);
    uint8_t value = 49;
    for (size_t i = 0; i < size; i++) {
        ASSERT_EQ(data[i], value);
        value++;
    }
}

HWTEST_F(NativeEngineTest, WriteStringTest004, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(9);
    std::string str = "123456789";
    unsigned int size = buf->WriteString(str, 2, 7);
    uint8_t data[size];
    buf->ReadBytes(data, 0, size);
    uint8_t value = 49;
    for (size_t i = 2; i < size; i++) {
        ASSERT_EQ(data[i], value);
        value++;
    }
}

HWTEST_F(NativeEngineTest, WriteStringTest005, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(9);
    std::string str = "123456789";
    unsigned int size = buf->WriteString(str, 0, 9, "utf8");
    uint8_t data[size];
    buf->ReadBytes(data, 0, size);
    uint8_t value = 49;
    for (size_t i = 0; i < size; i++) {
        ASSERT_EQ(data[i], value);
        value++;
    }
}

HWTEST_F(NativeEngineTest, WriteStringTest006, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(16);
    FillZero(buf, 16);
    std::string str = "12345678";
    unsigned int size = buf->WriteString(str, 0, 16, "utf16le");
    ASSERT_EQ(size, 16);
    uint8_t data[size];
    buf->ReadBytes(data, 0, size);
    uint8_t value = 49;
    for (size_t i = 0; i < size; i++) {
        if (i % 2 == 0) {
            ASSERT_EQ(data[i], value);
            value++;
        } else {
            ASSERT_EQ(data[i], 0);
        }
    }
}

HWTEST_F(NativeEngineTest, SubBufferTest001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf1 = new OHOS::buffer::Buffer();
    buf1->Init(10);
    FillZero(buf1, 10);
    std::string str = "1234567890";
    buf1->WriteString(str, 0, 10);
    OHOS::buffer::Buffer *buf2 = buf1->SubBuffer(0, 10);
    ASSERT_EQ(buf2->GetLength(), 10);
    uint8_t data[11];
    buf2->ReadBytes(data, 0, 10);
    data[10] = 0;
    ASSERT_STREQ(reinterpret_cast<char*>(data), str.c_str());
}

HWTEST_F(NativeEngineTest, SubBufferTest002, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf1 = new OHOS::buffer::Buffer();
    buf1->Init(10);
    FillZero(buf1, 10);
    std::string str = "1234567890";
    buf1->WriteString(str, 0, 10);
    OHOS::buffer::Buffer *buf2 = buf1->SubBuffer(2, 10);
    ASSERT_EQ(buf2->GetLength(), 8);
    uint8_t data[9];
    buf2->ReadBytes(data, 0, 8);
    data[8] = 0;
    ASSERT_STREQ(reinterpret_cast<char*>(data), "34567890");
}

HWTEST_F(NativeEngineTest, CopyTest001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buffer = new OHOS::buffer::Buffer();
    buffer->Init(20);

    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(20);
    buf->WriteString("this is a string", 16);

    unsigned int tOffset = 1;
    unsigned int sOffset = 0;
    unsigned int tEnd = 16;
    unsigned int sEnd = 16;
    buf->Copy(buffer, tOffset, sOffset, sEnd);
    uint8_t data[20] = {0};
    buffer->ReadBytes(data, tOffset, tEnd);
    ASSERT_STREQ(reinterpret_cast<char*>(data), "this is a string");

}

HWTEST_F(NativeEngineTest, CompareTest001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buffer = new OHOS::buffer::Buffer();
    buffer->Init(20);
    buffer->WriteString("this is a string", 16);

    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(20);
    buf->WriteString("this is a string", 1, 16);

    int result = buf->Compare(buffer, 0, 1, 16);
    ASSERT_EQ(result, 0);
}

HWTEST_F(NativeEngineTest, IndexOfTest001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(20);
    buf->WriteString("this is a string", 16);
    int index = buf->IndexOf("is", 0);
    ASSERT_EQ(index, 2);
}

HWTEST_F(NativeEngineTest, LastIndexOfTest001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(20);
    buf->WriteString("this is a string", 16);
    int index = buf->LastIndexOf("is", 0, 2);
    ASSERT_EQ(index, 5);
}

HWTEST_F(NativeEngineTest, ToBase64Test001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(20);
    buf->WriteString("this is a string", 16);
    std::string base64Str = buf->ToBase64(0, 16);
    ASSERT_STREQ(base64Str.c_str(), "dGhpcyBpcyBhIHN0cmluZw==");
}

HWTEST_F(NativeEngineTest, GetEncodingTypeTest001, testing::ext::TestSize.Level0)
{
    std::string type = "base64";
    OHOS::buffer::EncodingType et = OHOS::buffer::Buffer::GetEncodingType(type);
    ASSERT_EQ(et, OHOS::buffer::BASE64);
}

HWTEST_F(NativeEngineTest, SetArrayTest001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buffer = new OHOS::buffer::Buffer();
    buffer->Init(20);
    std::vector<uint8_t> numbers;
    for (int i = 0; i < 10; i++) {
        numbers.push_back(i);
    }
    buffer->SetArray(numbers);
    unsigned int offset = 0;
    unsigned int end = 10;
    uint8_t data[20] = {0};
    buffer->ReadBytes(data, offset, end);
    for (int j = 0; j < 10; j++) {
        ASSERT_EQ(data[j], j);
    }
}

HWTEST_F(NativeEngineTest, FillBufferTest001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buffer = new OHOS::buffer::Buffer();
    buffer->Init(10);
    std::vector<uint8_t> numbers;
    for (int i = 0; i < 10; i++) {
        numbers.push_back(i);
    }
    buffer->SetArray(numbers);
    unsigned int offset = 0;
    unsigned int end = 10;
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(20);
    buf->FillBuffer(buffer, offset, end);
    uint8_t data[20] = {0};
    buf->ReadBytes(data, offset, end);
    for (int j = 0; j < 10; j++) {
        ASSERT_EQ(data[j], j);
    }
}

HWTEST_F(NativeEngineTest, FillNumberTest001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(20);
    std::vector<uint8_t> numbers;
    for (int i = 0; i < 10; i++) {
        numbers.push_back(i);
    }
    unsigned int offset = 0;
    unsigned int end = 10;
    buf->FillNumber(numbers, offset, end);
    uint8_t data[20] = {0};
    buf->ReadBytes(data, offset, end);
    for (int j = 0; j < 10; j++) {
        ASSERT_EQ(data[j], j);
    }
}

HWTEST_F(NativeEngineTest, FillStringTest001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(20);
    std::string value = "abcd";
    unsigned int offset = 0;
    unsigned int end = 10;
    std::string encoding = "ascii";
    buf->FillString(value, offset, end, encoding);
    uint8_t data[20] = {0};
    buf->ReadBytes(data, offset, end);
    ASSERT_STREQ(reinterpret_cast<char*>(data), "abcdabcdab");
}

HWTEST_F(NativeEngineTest, BlobConstructorTest001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Blob *blob = new OHOS::buffer::Blob();
    uint8_t data[4] = {1, 2, 3, 4};
    blob->Init(data, 4);
    ASSERT_EQ(blob->GetLength(), 4);
}

HWTEST_F(NativeEngineTest, BlobConstructorTest002, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Blob *blob = new OHOS::buffer::Blob();
    uint8_t data[4] = {1, 2, 3, 4};
    blob->Init(data, 4);

    OHOS::buffer::Blob *blob2 = new OHOS::buffer::Blob();
    blob2->Init(blob, 0);

    ASSERT_EQ(blob2->GetLength(), 4);
}

HWTEST_F(NativeEngineTest, BlobConstructorTest003, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Blob *blob = new OHOS::buffer::Blob();
    uint8_t data[4] = {1, 2, 3, 4};
    blob->Init(data, 4);

    OHOS::buffer::Blob *blob2 = new OHOS::buffer::Blob();
    blob2->Init(blob, 1, 4);

    ASSERT_EQ(blob2->GetLength(), 3);
}

HWTEST_F(NativeEngineTest, BlobGetByteTest001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Blob *blob = new OHOS::buffer::Blob();
    uint8_t data[4] = {1, 2, 3, 4};
    blob->Init(data, 4);

    uint8_t byte = blob->GetByte(2);

    ASSERT_EQ(byte, 3);
}

HWTEST_F(NativeEngineTest, BlobGetRawTest001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Blob *blob = new OHOS::buffer::Blob();
    uint8_t data[4] = {1, 2, 3, 4};
    blob->Init(data, 4);

    uint8_t *raw = blob->GetRaw();

    ASSERT_TRUE(raw != nullptr);
}

HWTEST_F(NativeEngineTest, BlobGetLengthTest001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Blob *blob = new OHOS::buffer::Blob();
    uint8_t data[4] = {1, 2, 3, 4};
    blob->Init(data, 4);

    unsigned int len = blob->GetLength();

    ASSERT_EQ(len, 4);
}

HWTEST_F(NativeEngineTest, BlobReadBytesTest001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Blob *blob = new OHOS::buffer::Blob();
    uint8_t data[10] = {0};
    for (int i = 0; i < 10; i++) {
        data[i] = i;
    }
    blob->Init(data, 10);

    uint8_t dat[10] = {0};
    blob->ReadBytes(dat, 10);

    for (int i = 0; i < 10; i++) {
        ASSERT_EQ(dat[i], i);
    }
}