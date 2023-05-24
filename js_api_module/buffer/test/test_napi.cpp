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
#include "native_module_buffer.h"
#include "securec.h"
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
const int ARGS_NUMBER_TWO = 2;
const int ARGS_NUMBER_THREE = 3;
const int ARG_NUMBER_FOUR = 4;
void FillZero(OHOS::buffer::Buffer *buf, size_t size)
{
    for (size_t i = 0; i < size; i++) {
        buf->Set(i, 0);
    }
}

/**
 * @tc.name: ConstructorTest001
 * @tc.desc: Buffer Constructor.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, ConstructorTest001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(10);
    ASSERT_EQ(buf->GetLength(), 10);
}

/**
 * @tc.name: ConstructorTest002
 * @tc.desc: Buffer Constructor.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, ConstructorTest002, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf1 = new OHOS::buffer::Buffer();
    buf1->Init(10);
    OHOS::buffer::Buffer *buf2 = new OHOS::buffer::Buffer();
    buf2->Init(buf1);
    ASSERT_EQ(buf2->GetLength(), 10);
}

/**
 * @tc.name: ConstructorTest003
 * @tc.desc: Buffer Constructor.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
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

/**
 * @tc.name: ConstructorTest004
 * @tc.desc: Buffer Constructor.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, ConstructorTest004, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf2 = new OHOS::buffer::Buffer();
    uint8_t data[4] = {1, 2, 3, 4};
    buf2->Init(data, 0, 4);
    ASSERT_EQ(buf2->GetLength(), 4);
    ASSERT_EQ(buf2->GetByteOffset(), 0);
}

/**
 * @tc.name: ConstructorTest005
 * @tc.desc: Buffer Constructor.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, ConstructorTest005, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(1);
    ASSERT_EQ(buf->GetLength(), 1);
    uint32_t res = buf->Copy(nullptr, 0, 0, 0);
    ASSERT_EQ(res, 0);
    int result = buf->Compare(nullptr, 0, 0, 0);
    ASSERT_EQ(result, 0);
    buf->ReadBytes(nullptr, 0, 0);
    buf->FillString("abc", 1, 0, "utf16le");
    buf->FillString("abc", 1, 0, "binary");
    buf->FillString("abc", 1, 0, "sos");
    std::vector<uint8_t> array;
    buf->FillNumber(array, 1, 0);
    OHOS::buffer::Buffer *buffer = new OHOS::buffer::Buffer();
    buf->FillBuffer(buffer, 1, 0);
    buf->FillBuffer(nullptr, 1, 0);
    buf->SetArray(array, 0);
    result = buf->LastIndexOf(nullptr, 0, 0);
    ASSERT_EQ(result, -1);
    result = buf->IndexOf(nullptr, 0, 0);
    ASSERT_EQ(result, -1);
    delete buf;
    buf = nullptr;
    delete buffer;
    buffer = nullptr;
}

/**
 * @tc.name: DestructorTest001
 * @tc.desc: Buffer Destructor.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, DestructorTest001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    uint8_t data[4] = {1, 2, 3, 4};
    buf->Init(data, 0, 4);
    delete buf;
}

/**
 * @tc.name: GetLengthTest001
 * @tc.desc: Get buffer Length.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, GetLengthTest001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    ASSERT_EQ(buf->GetLength(), 0);
}

/**
 * @tc.name: GetLengthTest002
 * @tc.desc: Get buffer Length.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, GetLengthTest002, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(6);
    ASSERT_EQ(buf->GetLength(), 6);
}

/**
 * @tc.name: SetLengthTest001
 * @tc.desc: Set buffer Length.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, SetLengthTest001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(6);
    buf->SetLength(7);
    ASSERT_EQ(buf->GetLength(), 7);
}

/**
 * @tc.name: GetByteOffsetTest001
 * @tc.desc: Get buffer byteOffset.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, GetByteOffsetTest001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *poolBuffer = new OHOS::buffer::Buffer();
    poolBuffer->Init(1024 * 8);
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(poolBuffer, 2, 5);
    ASSERT_EQ(buf->GetByteOffset(), 2);
}

/**
 * @tc.name: GetAndSetTest001
 * @tc.desc: Get And Set method.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, GetAndSetTest001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(3);
    buf->Set(0, 1);
    int32_t value = buf->Get(0);
    ASSERT_EQ(value, 1);
}

/**
 * @tc.name: GetAndSetTest002
 * @tc.desc: Get And Set method.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
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

/**
 * @tc.name: WriteInt32BEAndReadInt32BETest001
 * @tc.desc: Writes value to buf at the specified offset as big-endian. The value must be a valid signed 32-bit integer
 *           Reads a signed, big-endian 32-bit integer from buf at the specified offset.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, WriteInt32BEAndReadInt32BETest001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(4);
    FillZero(buf, 4);
    buf->WriteInt32BE(0x12345678, 0);
    int32_t res = buf->ReadInt32BE(0);
    ASSERT_EQ(res, 0x12345678);
}

/**
 * @tc.name: WriteInt32BEAndReadInt32BETest002
 * @tc.desc: Writes value to buf at the specified offset as big-endian. The value must be a valid signed 32-bit integer
 *           Reads a signed, big-endian 32-bit integer from buf at the specified offset.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, WriteInt32BEAndReadInt32BETest002, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(5);
    FillZero(buf, 5);
    buf->WriteInt32BE(0x12345678, 1);
    int32_t res = buf->ReadInt32BE(1);
    ASSERT_EQ(res, 0x12345678);
}

/**
 * @tc.name: WriteInt32BEAndReadInt32BETest003
 * @tc.desc: Writes value to buf at the specified offset as big-endian. The value must be a valid signed 32-bit integer
 *           Reads a signed, big-endian 32-bit integer from buf at the specified offset.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, WriteInt32BEAndReadInt32BETest003, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(5);
    FillZero(buf, 5);
    buf->WriteInt32BE(0x12345678, 1);
    int32_t res = buf->ReadInt32BE(1);
    ASSERT_EQ(res, 0x12345678);
}

/**
 * @tc.name: WriteInt32LEAndReadInt32LETest001
 * @tc.desc: Writes value to buf at the specified offset as little-endian.
 *           The value must be a valid signed 32-bit integer.
 *           Reads a signed, little-endian 32-bit integer from buf at the specified offset.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
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

/**
 * @tc.name: WriteInt32LEAndReadInt32LETest002
 * @tc.desc: Writes value to buf at the specified offset as little-endian.
 *           The value must be a valid signed 32-bit integer.
 *           Reads a signed, little-endian 32-bit integer from buf at the specified offset.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, WriteInt32LEAndReadInt32LETest002, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(5);
    FillZero(buf, 5);
    buf->WriteInt32LE(0x12345678, 1);
    int32_t res = buf->ReadInt32LE(0);
    ASSERT_EQ(res, 0x34567800);
}

/**
 * @tc.name: WriteInt32LEAndReadInt32LETest003
 * @tc.desc: Writes value to buf at the specified offset as little-endian.
 *           The value must be a valid signed 32-bit integer.
 *           Reads a signed, little-endian 32-bit integer from buf at the specified offset.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, WriteInt32LEAndReadInt32LETest003, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(5);
    FillZero(buf, 5);
    buf->WriteInt32LE(0x12345678, 1);
    int32_t res = buf->ReadInt32LE(1);
    ASSERT_EQ(res, 0x12345678);
}

/**
 * @tc.name: WriteUInt32BEAndReadUInt32BETest001
 * @tc.desc: Writes value to buf at the specified offset as big-endian.
 *           The value must be a valid unsigned 32-bit integer.
 *           Reads an unsigned, big-endian 32-bit integer from buf at the specified offset.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, WriteUInt32BEAndReadUInt32BETest001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(4);
    FillZero(buf, 4);
    buf->WriteUInt32BE(0x12345678, 0);
    int32_t res = buf->ReadUInt32BE(0);
    ASSERT_EQ(res, 0x12345678);
}

/**
 * @tc.name: WriteUInt32BEAndReadUInt32BETest002
 * @tc.desc: Writes value to buf at the specified offset as big-endian.
 *           The value must be a valid unsigned 32-bit integer.
 *           Reads an unsigned, big-endian 32-bit integer from buf at the specified offset.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, WriteUInt32BEAndReadUInt32BETest002, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(5);
    FillZero(buf, 5);
    buf->WriteUInt32BE(0x12345678, 1);
    int32_t res = buf->ReadUInt32BE(0);
    ASSERT_EQ(res, 0x123456);
}

/**
 * @tc.name: WriteUInt32BEAndReadUInt32BETest003
 * @tc.desc: Writes value to buf at the specified offset as big-endian.
 *           The value must be a valid unsigned 32-bit integer.
 *           Reads an unsigned, big-endian 32-bit integer from buf at the specified offset.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, WriteUInt32BEAndReadUInt32BETest003, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(5);
    FillZero(buf, 5);
    buf->WriteUInt32BE(0x12345678, 1);
    int32_t res = buf->ReadUInt32BE(1);
    ASSERT_EQ(res, 0x12345678);
}

/**
 * @tc.name: WriteUInt32LEAndReadUInt32LETest001
 * @tc.desc: Writes value to buf at the specified offset as little-endian.
 *           The value must be a valid unsigned 32-bit integer.
 *           Reads an unsigned, little-endian 32-bit integer from buf at the specified offset.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, WriteUInt32LEAndReadUInt32LETest001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(4);
    FillZero(buf, 4);
    buf->WriteUInt32LE(0x12345678, 0);
    int32_t res = buf->ReadUInt32LE(0);
    ASSERT_EQ(res, 0x12345678);
}

/**
 * @tc.name: WriteUInt32LEAndReadUInt32LETest002
 * @tc.desc: Writes value to buf at the specified offset as little-endian.
 *           The value must be a valid unsigned 32-bit integer.
 *           Reads an unsigned, little-endian 32-bit integer from buf at the specified offset.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, WriteUInt32LEAndReadUInt32LETest002, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(5);
    FillZero(buf, 5);
    buf->WriteUInt32LE(0x12345678, 1);
    int32_t res = buf->ReadUInt32LE(0);
    ASSERT_EQ(res, 0x34567800);
}

/**
 * @tc.name: WriteUInt32LEAndReadUInt32LETest003
 * @tc.desc: Writes value to buf at the specified offset as little-endian.
 *           The value must be a valid unsigned 32-bit integer.
 *           Reads an unsigned, little-endian 32-bit integer from buf at the specified offset.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, WriteUInt32LEAndReadUInt32LETest003, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(5);
    FillZero(buf, 5);
    buf->WriteUInt32LE(0x12345678, 1);
    int32_t res = buf->ReadUInt32LE(1);
    ASSERT_EQ(res, 0x12345678);
}

/**
 * @tc.name: ReadBytesTest001
 * @tc.desc: Read value from buffer.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
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

/**
 * @tc.name: WriteStringTest001
 * @tc.desc: Write string to buffer.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, WriteStringTest001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(10);
    std::string str = "1234567890";
    unsigned int size = buf->WriteString(str, 10);
    ASSERT_EQ(size, 10);
}

/**
 * @tc.name: WriteStringTest002
 * @tc.desc: Write string to buffer.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
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

/**
 * @tc.name: WriteStringTest003
 * @tc.desc: Write string to buffer.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
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

/**
 * @tc.name: WriteStringTest004
 * @tc.desc: Write string to buffer.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
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

/**
 * @tc.name: WriteStringTest005
 * @tc.desc: Write string to buffer.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
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

/**
 * @tc.name: WriteStringTest006
 * @tc.desc: Write string to buffer.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
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

/**
 * @tc.name: SubBufferTest001
 * @tc.desc: Returns a new Buffer that references the same memory as the original,
 *           but offset and cropped by the start and end indices.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
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

/**
 * @tc.name: SubBufferTest002
 * @tc.desc: Returns a new Buffer that references the same memory as the original,
 *           but offset and cropped by the start and end indices.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
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

/**
 * @tc.name: CopyTest001
 * @tc.desc: Copies data from a region of buf to a region in target,
 *           even if the target memory region overlaps with buf.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
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

/**
 * @tc.name: CompareTest001
 * @tc.desc: Compares buf with target and returns a number indicating whether buf comes before, after,
 *           or is the same as target in sort order. Comparison is based on the actual sequence of bytes in each Buffer
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
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

/**
 * @tc.name: IndexOfTest001
 * @tc.desc: The index of the first occurrence of value in buf.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, IndexOfTest001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(20);
    buf->WriteString("this is a string", 16);
    int index = buf->IndexOf("is", 0, 2);
    ASSERT_EQ(index, 2);
}

/**
 * @tc.name: IndexOfTest002
 * @tc.desc: The index of the first occurrence of value in buf.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, IndexOfTest002, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(7);
    buf->WriteString("3363333", 7);
    int index = buf->IndexOf("36", 0, 2);
    ASSERT_EQ(index, 1);
}

/**
 * @tc.name: IndexOfTest003
 * @tc.desc: The index of the first occurrence of value in buf.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, IndexOfTest003, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(12);
    buf->WriteString("322362326233", 12);
    int index = buf->IndexOf("2623", 0, 4);
    ASSERT_EQ(index, 7);
}

/**
 * @tc.name: LastIndexOfTest001
 * @tc.desc: The index of the last occurrence of value in buf.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, LastIndexOfTest001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(20);
    buf->WriteString("this is a string", 16);
    int index = buf->LastIndexOf("is", 0, 2);
    ASSERT_EQ(index, 5);
}

/**
 * @tc.name: LastIndexOfTest002
 * @tc.desc: The index of the last occurrence of value in buf.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, LastIndexOfTest002, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(7);
    buf->WriteString("3363333", 7);
    int index = buf->LastIndexOf("36", 0, 2);
    ASSERT_EQ(index, 1);
}

/**
 * @tc.name: LastIndexOfTest003
 * @tc.desc: The index of the last occurrence of value in buf.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, LastIndexOfTest003, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(11);
    buf->WriteString("32236326233", 11);
    int index = buf->LastIndexOf("236", 0, 3);
    ASSERT_EQ(index, 2);
}

/**
 * @tc.name: LastIndexOfTest004
 * @tc.desc: The index of the last occurrence of value in buf.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, LastIndexOfTest004, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(12);
    buf->WriteString("322362326233", 12);
    int index = buf->LastIndexOf("2236", 0, 4);
    ASSERT_EQ(index, 1);
}

/**
 * @tc.name: LastIndexOfTest005
 * @tc.desc: The index of the last occurrence of value in buf.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, LastIndexOfTest005, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(12);
    buf->WriteString("322362326233", 12);
    int index = buf->LastIndexOf("136", 0, 3);
    ASSERT_EQ(index, -1);
}


/**
 * @tc.name: ToBase64Test001
 * @tc.desc: Convert the contents of the buffer into a string in Base64 format.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, ToBase64Test001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(20);
    buf->WriteString("this is a string", 16);
    std::string base64Str = buf->ToBase64(0, 16);
    ASSERT_STREQ(base64Str.c_str(), "dGhpcyBpcyBhIHN0cmluZw==");
}

/**
 * @tc.name: ToBase64Test002
 * @tc.desc: Convert the contents of the buffer into a string in Base64 format.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, ToBase64Test002, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(30);
    buf->WriteString("this is a big string", 20);
    std::string base64Str = buf->ToBase64(0, 20);
    ASSERT_STREQ(base64Str.c_str(), "dGhpcyBpcyBhIGJpZyBzdHJpbmc=");
}

/**
 * @tc.name: GetEncodingTypeTest001
 * @tc.desc: Get encoding type.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, GetEncodingTypeTest001, testing::ext::TestSize.Level0)
{
    std::map <std::string, int> _typeMap =
    {
        {"hex", OHOS::buffer::HEX},
        {"base64url", OHOS::buffer::BASE64URL},
        {"ascii", OHOS::buffer::ASCII},
        {"base64", OHOS::buffer::BASE64},
        {"latin1", OHOS::buffer::LATIN1},
        {"binary", OHOS::buffer::BINARY},
        {"utf16le", OHOS::buffer::UTF16LE},
        {"utf8", OHOS::buffer::UTF8},
    };

    for (auto item =_typeMap.begin(); item != _typeMap.end(); item++)
    {
        std::string type = item->first;
        OHOS::buffer::EncodingType et = OHOS::buffer::Buffer::GetEncodingType(type);
        ASSERT_EQ(et, item->second);
    }
}

/**
 * @tc.name: SetArrayTest001
 * @tc.desc: Put the contents of the array into the buffer.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
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

/**
 * @tc.name: FillBufferTest001
 * @tc.desc: Fill the buffer with the buffer object
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
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

/**
 * @tc.name: FillNumberTest001
 * @tc.desc: Fill the buffer with the number
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
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

/**
 * @tc.name: FillStringTest001
 * @tc.desc: Fill the buffer with the string
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
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

/**
 * @tc.name: FillStringTest002
 * @tc.desc: Fill the buffer with the string
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, FillStringTest002, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(20);
    std::string value = "扡摣";
    unsigned int offset = 0;
    unsigned int end = 10;
    std::string encoding = "utf16le";
    buf->FillString(value, offset, end, encoding);
    uint8_t data[20] = {0};
    buf->ReadBytes(data, offset, end);
    ASSERT_STREQ(reinterpret_cast<char*>(data), "abcdabcdab");
}

/**
 * @tc.name: FillStringTest003
 * @tc.desc: Fill the buffer with the string
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, FillStringTest003, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    buf->Init(20);
    std::string value = "YWJjZA";
    unsigned int offset = 0;
    unsigned int end = 10;
    std::string encoding = "base64";
    buf->FillString(value, offset, end, encoding);
    uint8_t data[20] = {0};
    buf->ReadBytes(data, offset, end);
    ASSERT_STREQ(reinterpret_cast<char*>(data), "abcdabcdab");
}

/**
 * @tc.name: BlobConstructorTest001
 * @tc.desc: Blob Constructor
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, BlobConstructorTest001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Blob *blob = new OHOS::buffer::Blob();
    uint8_t data[4] = {1, 2, 3, 4};
    blob->Init(data, 4);
    ASSERT_EQ(blob->GetLength(), 4);
}

/**
 * @tc.name: BlobConstructorTest002
 * @tc.desc: Blob Constructor
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, BlobConstructorTest002, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Blob *blob = new OHOS::buffer::Blob();
    uint8_t data[4] = {1, 2, 3, 4};
    blob->Init(data, 4);

    OHOS::buffer::Blob *blob2 = new OHOS::buffer::Blob();
    blob2->Init(blob, 0);

    ASSERT_EQ(blob2->GetLength(), 4);
}

/**
 * @tc.name: BlobConstructorTest003
 * @tc.desc: Blob Constructor
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, BlobConstructorTest003, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Blob *blob = new OHOS::buffer::Blob();
    uint8_t data[4] = {1, 2, 3, 4};
    blob->Init(data, 4);

    OHOS::buffer::Blob *blob2 = new OHOS::buffer::Blob();
    blob2->Init(blob, 1, 4);

    ASSERT_EQ(blob2->GetLength(), 3);
}

/**
 * @tc.name: BlobConstructorTest004
 * @tc.desc: Blob Constructor
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, BlobConstructorTest004, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Blob *blob = new OHOS::buffer::Blob();
    uint8_t data[ARG_NUMBER_FOUR] = {1, 2, 3, 4};
    OHOS::buffer::Blob *blob1 = new OHOS::buffer::Blob();
    blob->Init(blob1, ARGS_NUMBER_THREE, ARGS_NUMBER_TWO);
    blob->Init(blob1, ARGS_NUMBER_THREE, -1);
    blob->Init(nullptr, ARGS_NUMBER_TWO, ARGS_NUMBER_THREE);
    blob->Init(data, ARG_NUMBER_FOUR);
    ASSERT_EQ(blob->GetLength(), ARG_NUMBER_FOUR);
    delete blob;
    blob = nullptr;
    delete blob1;
    blob1 = nullptr;
}

/**
 * @tc.name: BlobDestructorTest001
 * @tc.desc: Blob Destructor.
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, BlobDestructorTest001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Blob *blob = new OHOS::buffer::Blob();
    uint8_t data[4] = {1, 2, 3, 4};
    blob->Init(data, 4);
    delete blob;
}

/**
 * @tc.name: BlobGetByteTest001
 * @tc.desc: Get a byte in blob
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, BlobGetByteTest001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Blob *blob = new OHOS::buffer::Blob();
    uint8_t data[4] = {1, 2, 3, 4};
    blob->Init(data, 4);

    uint8_t byte = blob->GetByte(2);

    ASSERT_EQ(byte, 3);
}

/**
 * @tc.name: BlobGetRawTest001
 * @tc.desc: Get the raw in blob
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, BlobGetRawTest001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Blob *blob = new OHOS::buffer::Blob();
    uint8_t data[4] = {1, 2, 3, 4};
    blob->Init(data, 4);

    uint8_t *raw = blob->GetRaw();

    ASSERT_TRUE(raw != nullptr);
}

/**
 * @tc.name: BlobGetLengthTest001
 * @tc.desc: Get the length in blob
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, BlobGetLengthTest001, testing::ext::TestSize.Level0)
{
    OHOS::buffer::Blob *blob = new OHOS::buffer::Blob();
    uint8_t data[4] = {1, 2, 3, 4};
    blob->Init(data, 4);

    unsigned int len = blob->GetLength();

    ASSERT_EQ(len, 4);
}

/**
 * @tc.name: BlobGetLengthTest001
 * @tc.desc: Read blob object bytes
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
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

/**
 * @tc.name: Utf8ToUtf16BETest001
 * @tc.desc: convert utf8 bytes to utf16 bytes
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, Utf8ToUtf16BETest001, testing::ext::TestSize.Level0)
{
    std::string str8 = "";
    // one byte
    str8.append(1, 0x41);
    // two bytes
    str8.append(1, 0xC3);
    str8.append(1, 0x84);
    // three bytes
    str8.append(1, 0xE5);
    str8.append(1, 0x88);
    str8.append(1, 0x98);
    // four bytes
    str8.append(1, 0xf0);
    str8.append(1, 0x9f);
    str8.append(1, 0x90);
    str8.append(1, 0x85);

    // another four bytes
    str8.append(1, 0xf0);
    str8.append(1, 0x8f);
    str8.append(1, 0x90);
    str8.append(1, 0x85);

    bool isOk = false;
    std::u16string str16 = OHOS::buffer::Utf8ToUtf16BE(str8, &isOk);
    
    char16_t results[] = {0x41, 0xc4, 0x5218, 0xd83d, 0xdc05, 0xf405};
    for (int i = 0; i < 6; i++) {
        ASSERT_EQ(results[i], str16[i]);
    }
}

/**
 * @tc.name: HexDecodeTest001
 * @tc.desc: decode a hex string
 * @tc.type: FUNC
 * @tc.require:issueI5J5Z3
 */
HWTEST_F(NativeEngineTest, HexDecodeTest001, testing::ext::TestSize.Level0)
{
    std::string ret = OHOS::buffer::HexDecode("313g");
    ASSERT_EQ(ret, "1");
}

/**
 * @tc.name: TestInit
 * @tc.desc: test init function
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TestInit, testing::ext::TestSize.Level0)
{
    napi_value exports = nullptr;
    napi_env env = (napi_env)engine_;
    napi_create_object(env, &exports);
    exports = OHOS::buffer::Init(env, exports);
    napi_value buf;
    napi_get_named_property(env, exports, "utf8StringToNumbers", &buf);
    std::string input = "1";
    napi_value src = nullptr;
    napi_create_string_utf8(env, input.c_str(), input.size(), &src);
    napi_value funcResultValue = nullptr;
    napi_value recv = nullptr;
    napi_get_undefined(env, &recv);
    napi_call_function(env, recv, buf, 1, &src, &funcResultValue);
    napi_valuetype valuetype;
    napi_typeof(env, funcResultValue, &valuetype);
    ASSERT_EQ(valuetype, napi_object);
}

/**
 * @tc.name: TestUtf8ByteLength
 * @tc.desc: test TestUtf8ByteLength function
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TestUtf8ByteLength, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value funcValue = nullptr;
    napi_create_function(env, "testFunc", NAPI_AUTO_LENGTH, OHOS::buffer::Utf8ByteLength, nullptr, &funcValue);
    std::string input = "a";
    napi_value src = nullptr;
    napi_create_string_utf8(env, input.c_str(), input.size(), &src);
    napi_value funcResultValue = nullptr;
    napi_value recv = nullptr;
    napi_get_undefined(env, &recv);
    napi_call_function(env, recv, funcValue, 1, &src, &funcResultValue);
    uint32_t res = 0;
    napi_get_value_uint32(env, funcResultValue, &res);
    ASSERT_EQ(res, 1);
}

/**
 * @tc.name: TestFromString
 * @tc.desc: test TestFromString function
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TestFromString, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value funcValue = nullptr;
    napi_value funcResultValue = nullptr;
    napi_value recv = nullptr;
    napi_create_object(env, &recv);
    OHOS::buffer::BufferInit(env, recv);

    napi_value bufferClass = nullptr;
    napi_get_named_property(env, recv, "Buffer", &bufferClass);

    int32_t typeEncode = 0;
    napi_value argInstance1 = nullptr;
    napi_create_int32(env, typeEncode, &argInstance1);
    int32_t length = 1;
    napi_value argInstance2 = nullptr;
    napi_create_uint32(env, length, &argInstance2);
    napi_value args[ARGS_NUMBER_TWO] = {argInstance1, argInstance2};
    napi_value instance;
    napi_new_instance(env, bufferClass, ARGS_NUMBER_TWO, args, &instance);
    napi_get_named_property(env, instance, "fromString", &funcValue);

    std::string arg1 = "a";
    napi_value src = nullptr;
    napi_create_string_utf8(env, arg1.c_str(), arg1.size(), &src);
    napi_value type = nullptr;
    std::string arg2 = "hex";
    napi_create_string_utf8(env, arg2.c_str(), arg2.size(), &type);
    uint32_t arg3 = 1;
    napi_value srcLength = nullptr;
    napi_create_uint32(env, arg3, &srcLength);
    napi_value args1[ARGS_NUMBER_THREE] = {src, type, srcLength};
    napi_call_function(env, instance, funcValue, ARGS_NUMBER_THREE, args1, &funcResultValue);

    napi_valuetype valuetype;
    napi_typeof(env, funcResultValue, &valuetype);
    ASSERT_EQ(valuetype, napi_object);

    arg2 = "ascii";
    napi_create_string_utf8(env, arg2.c_str(), arg2.size(), &type);
    args1[1] = type;
    napi_call_function(env, instance, funcValue, ARGS_NUMBER_THREE, args1, &funcResultValue);
    napi_typeof(env, funcResultValue, &valuetype);
    ASSERT_EQ(valuetype, napi_object);

    arg2 = "base64";
    napi_create_string_utf8(env, arg2.c_str(), arg2.size(), &type);
    args1[1] = type;
    napi_call_function(env, instance, funcValue, ARGS_NUMBER_THREE, args1, &funcResultValue);
    napi_typeof(env, funcResultValue, &valuetype);
    ASSERT_EQ(valuetype, napi_object);

    arg2 = "utf16le";
    napi_create_string_utf8(env, arg2.c_str(), arg2.size(), &type);
    args1[1] = type;
    napi_call_function(env, instance, funcValue, ARGS_NUMBER_THREE, args1, &funcResultValue);
    napi_typeof(env, funcResultValue, &valuetype);
    ASSERT_EQ(valuetype, napi_object);

    arg2 = "utf8";
    napi_create_string_utf8(env, arg2.c_str(), arg2.size(), &type);
    args1[1] = type;
    napi_call_function(env, instance, funcValue, ARGS_NUMBER_THREE, args1, &funcResultValue);
    napi_typeof(env, funcResultValue, &valuetype);
    ASSERT_EQ(valuetype, napi_object);
}

/**
 * @tc.name: TestBlobConstructor001
 * @tc.desc: test TestBlobConstructor function
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TestBlobConstructor001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value recv = nullptr;
    napi_create_object(env, &recv);
    OHOS::buffer::BlobInit(env, recv);

    napi_value bufferClass = nullptr;
    napi_get_named_property(env, recv, "Blob", &bufferClass);

    size_t byteLength = ARGS_NUMBER_TWO;
    void* data = nullptr;
    napi_value resultBuff = nullptr;
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    unsigned char arr[ARGS_NUMBER_TWO] = {0x61, 0x62};
    memcpy_s(data, sizeof(arr), reinterpret_cast<void*>(arr), sizeof(arr));
    napi_value callArg1 = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &callArg1);

    napi_value instance;
    napi_new_instance(env, bufferClass, 1, &callArg1, &instance);

    int32_t type = 0;
    napi_value arg1 = nullptr;
    napi_create_int32(env, type, &arg1);
    napi_value args1[ARGS_NUMBER_TWO] = {instance, arg1};
    napi_value instance2;
    napi_new_instance(env, bufferClass, ARGS_NUMBER_TWO, args1, &instance2);

    int32_t length = 1;
    napi_value arg2 = nullptr;
    napi_create_uint32(env, length, &arg2);
    napi_value args2[ARGS_NUMBER_THREE] = {instance, arg1, arg2};
    napi_value instance3;
    napi_new_instance(env, bufferClass, ARGS_NUMBER_THREE, args2, &instance3);

    napi_valuetype valuetype;
    napi_typeof(env, instance3, &valuetype);
    ASSERT_EQ(valuetype, napi_object);
}

/**
 * @tc.name: TestGetBufferWrapValue
 * @tc.desc: test TestGetBufferWrapValue function
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, GetBufferWrapValue, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    OHOS::buffer::Buffer *buf = new OHOS::buffer::Buffer();
    napi_value result = OHOS::buffer::GetBufferWrapValue(env, nullptr, buf);
    ASSERT_TRUE(result == nullptr);
}

/**
 * @tc.name: TestWriteInt32BE
 * @tc.desc: test WriteInt32BE function
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TestWriteInt32BE, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value funcValue = nullptr;
    napi_value funcResultValue = nullptr;
    napi_value recv = nullptr;
    napi_create_object(env, &recv);
    OHOS::buffer::BufferInit(env, recv);

    napi_value bufferClass = nullptr;
    napi_get_named_property(env, recv, "Buffer", &bufferClass);

    int32_t type = 0;
    napi_value arg1 = nullptr;
    napi_create_int32(env, type, &arg1);
    int32_t length = 1;
    napi_value arg2 = nullptr;
    napi_create_uint32(env, length, &arg2);
    napi_value args[ARGS_NUMBER_TWO] = {arg1, arg2};
    napi_value instance;
    napi_new_instance(env, bufferClass, ARGS_NUMBER_TWO, args, &instance);
    napi_get_named_property(env, instance, "witeInt32BE", &funcValue);

    napi_call_function(env, instance, funcValue, ARGS_NUMBER_TWO, args, &funcResultValue);

    napi_valuetype valuetype;
    napi_typeof(env, funcResultValue, &valuetype);
    ASSERT_EQ(valuetype, napi_undefined);
}

/**
 * @tc.name: TestReadInt32BE
 * @tc.desc: test ReadInt32BE function
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TestReadInt32BE, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value funcValue = nullptr;
    napi_value funcResultValue = nullptr;
    napi_value recv = nullptr;
    napi_create_object(env, &recv);
    OHOS::buffer::BufferInit(env, recv);

    napi_value bufferClass = nullptr;
    napi_get_named_property(env, recv, "Buffer", &bufferClass);

    int32_t type = 0;
    napi_value arg1 = nullptr;
    napi_create_int32(env, type, &arg1);
    int32_t length = 1;
    napi_value arg2 = nullptr;
    napi_create_uint32(env, length, &arg2);
    napi_value args[ARGS_NUMBER_TWO] = {arg1, arg2};
    napi_value instance;
    napi_new_instance(env, bufferClass, ARGS_NUMBER_TWO, args, &instance);
    napi_get_named_property(env, instance, "readInt32BE", &funcValue);

    uint32_t offset = 0;
    napi_value arg = nullptr;
    napi_create_uint32(env, offset, &arg);
    napi_call_function(env, instance, funcValue, 1, &arg, &funcResultValue);

    napi_valuetype valuetype;
    napi_typeof(env, funcResultValue, &valuetype);
    ASSERT_EQ(valuetype, napi_number);
}

/**
 * @tc.name: TestSetArray
 * @tc.desc: test SetArray function
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TestSetArray, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value funcValue = nullptr;
    napi_create_function(env, "testFunc", NAPI_AUTO_LENGTH, OHOS::buffer::SetArray, nullptr, &funcValue);

    napi_value funcResultValue = nullptr;
    napi_value recv = nullptr;
    napi_create_object(env, &recv);
    OHOS::buffer::BufferInit(env, recv);

    napi_value bufferClass = nullptr;
    napi_get_named_property(env, recv, "Buffer", &bufferClass);

    int32_t type = 0;
    napi_value arg1 = nullptr;
    napi_create_int32(env, type, &arg1);
    int32_t length = 1;
    napi_value arg2 = nullptr;
    napi_create_uint32(env, length, &arg2);
    napi_value args[ARGS_NUMBER_TWO] = {arg1, arg2};
    napi_value instance;
    napi_new_instance(env, bufferClass, ARGS_NUMBER_TWO, args, &instance);
    napi_get_named_property(env, instance, "setArray", &funcValue);

    napi_value resultBuff = nullptr;
    napi_create_array(env, &resultBuff);
    napi_call_function(env, instance, funcValue, 1, &resultBuff, &funcResultValue);
    napi_valuetype valuetype;
    napi_typeof(env, funcResultValue, &valuetype);
    ASSERT_EQ(valuetype, napi_undefined);
}
/**
 * @tc.name: TestGetLength
 * @tc.desc: test GetLength function
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TestGetLength, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value funcValue = nullptr;
    napi_value funcResultValue = nullptr;
    napi_value recv = nullptr;
    napi_create_object(env, &recv);
    OHOS::buffer::BufferInit(env, recv);

    napi_value bufferClass = nullptr;
    napi_get_named_property(env, recv, "Buffer", &bufferClass);

    int32_t type = 0;
    napi_value arg1 = nullptr;
    napi_create_int32(env, type, &arg1);
    int32_t length = 1;
    napi_value arg2 = nullptr;
    napi_create_uint32(env, length, &arg2);
    napi_value args[ARGS_NUMBER_TWO] = {arg1, arg2};
    napi_value instance;
    napi_new_instance(env, bufferClass, ARGS_NUMBER_TWO, args, &instance);
    napi_get_named_property(env, instance, "getLength", &funcValue);
    napi_call_function(env, instance, funcValue, 0, nullptr, &funcResultValue);

    int32_t res = 0;
    napi_get_value_int32(env, funcResultValue, &res);
    ASSERT_EQ(res, 1);
}

/**
 * @tc.name: TestGetByteOffset
 * @tc.desc: test GetByteOffset function
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TestGetByteOffset, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value funcValue = nullptr;
    napi_value funcResultValue = nullptr;
    napi_value recv = nullptr;
    napi_create_object(env, &recv);
    OHOS::buffer::BufferInit(env, recv);

    napi_value bufferClass = nullptr;
    napi_get_named_property(env, recv, "Buffer", &bufferClass);

    int32_t type = 0;
    napi_value arg1 = nullptr;
    napi_create_int32(env, type, &arg1);
    int32_t length = 1;
    napi_value arg2 = nullptr;
    napi_create_uint32(env, length, &arg2);
    napi_value args[ARGS_NUMBER_TWO] = {arg1, arg2};
    napi_value instance;
    napi_new_instance(env, bufferClass, ARGS_NUMBER_TWO, args, &instance);
    napi_get_named_property(env, instance, "getByteOffset", &funcValue);
    napi_call_function(env, instance, funcValue, 0, nullptr, &funcResultValue);

    int32_t res = 0;
    napi_get_value_int32(env, funcResultValue, &res);
    ASSERT_EQ(res, 0);
}

/**
 * @tc.name: TestWriteString
 * @tc.desc: test WriteString function
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TestWriteString, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value funcValue = nullptr;
    napi_value funcResultValue = nullptr;
    napi_value recv = nullptr;
    napi_create_object(env, &recv);
    OHOS::buffer::BufferInit(env, recv);

    napi_value bufferClass = nullptr;
    napi_get_named_property(env, recv, "Buffer", &bufferClass);

    int32_t type = 0;
    napi_value arg1 = nullptr;
    napi_create_int32(env, type, &arg1);
    int32_t length = 1;
    napi_value arg2 = nullptr;
    napi_create_uint32(env, length, &arg2);
    napi_value args[ARGS_NUMBER_TWO] = {arg1, arg2};
    napi_value instance;
    napi_new_instance(env, bufferClass, ARGS_NUMBER_TWO, args, &instance);
    napi_get_named_property(env, instance, "writeString", &funcValue);

    std::string src = "a";
    napi_value callArg1 = nullptr;
    napi_create_string_utf8(env, src.c_str(), src.size(), &callArg1);

    uint32_t offset = 0;
    napi_value callArg2 = nullptr;
    napi_create_uint32(env, offset, &callArg2);

    uint32_t srcLength = 1;
    napi_value callArg3 = nullptr;
    napi_create_uint32(env, srcLength, &callArg3);

    napi_value callArg4 = nullptr;
    std::string encodeType = "utf8";
    napi_create_string_utf8(env, encodeType.c_str(), encodeType.size(), &callArg4);
    napi_value args1[ARG_NUMBER_FOUR] = {callArg1, callArg2, callArg3, callArg4};
    napi_call_function(env, instance, funcValue, ARG_NUMBER_FOUR, args1, &funcResultValue);

    int32_t res = 0;
    napi_get_value_int32(env, funcResultValue, &res);
    ASSERT_EQ(res, 1);
}

/**
 * @tc.name: TestFillString
 * @tc.desc: test FillString function
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TestFillString, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value funcValue = nullptr;
    napi_value funcResultValue = nullptr;
    napi_value recv = nullptr;
    napi_create_object(env, &recv);
    OHOS::buffer::BufferInit(env, recv);

    napi_value bufferClass = nullptr;
    napi_get_named_property(env, recv, "Buffer", &bufferClass);

    int32_t type = 0;
    napi_value arg1 = nullptr;
    napi_create_int32(env, type, &arg1);
    int32_t length = 1;
    napi_value arg2 = nullptr;
    napi_create_uint32(env, length, &arg2);
    napi_value args[ARGS_NUMBER_TWO] = {arg1, arg2};
    napi_value instance;
    napi_new_instance(env, bufferClass, ARGS_NUMBER_TWO, args, &instance);
    napi_get_named_property(env, instance, "fillString", &funcValue);

    std::string src = "a";
    napi_value callArg1 = nullptr;
    napi_create_string_utf8(env, src.c_str(), src.size(), &callArg1);

    uint32_t offset = 0;
    napi_value callArg2 = nullptr;
    napi_create_uint32(env, offset, &callArg2);

    uint32_t srcLength = 1;
    napi_value callArg3 = nullptr;
    napi_create_uint32(env, srcLength, &callArg3);

    napi_value callArg4 = nullptr;
    std::string encodeType = "utf8";
    napi_create_string_utf8(env, encodeType.c_str(), encodeType.size(), &callArg4);
    napi_value args1[ARG_NUMBER_FOUR] = {callArg1, callArg2, callArg3, callArg4};
    napi_call_function(env, instance, funcValue, ARG_NUMBER_FOUR, args1, &funcResultValue);

    napi_valuetype valuetype;
    napi_typeof(env, funcResultValue, &valuetype);
    ASSERT_EQ(valuetype, napi_undefined);
}

/**
 * @tc.name: TestFillNumbers
 * @tc.desc: test FillNumbers function
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TestFillNumbers, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value funcValue = nullptr;
    napi_value funcResultValue = nullptr;
    napi_value recv = nullptr;
    napi_create_object(env, &recv);
    OHOS::buffer::BufferInit(env, recv);

    napi_value bufferClass = nullptr;
    napi_get_named_property(env, recv, "Buffer", &bufferClass);

    int32_t type = 0;
    napi_value arg1 = nullptr;
    napi_create_int32(env, type, &arg1);
    int32_t length = 1;
    napi_value arg2 = nullptr;
    napi_create_uint32(env, length, &arg2);
    napi_value args[ARGS_NUMBER_TWO] = {arg1, arg2};
    napi_value instance;
    napi_new_instance(env, bufferClass, ARGS_NUMBER_TWO, args, &instance);
    napi_get_named_property(env, instance, "fillNumbers", &funcValue);

    size_t byteLength = ARGS_NUMBER_TWO;
    void* data = nullptr;
    napi_value resultBuff = nullptr;
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    unsigned char arr[ARGS_NUMBER_TWO] = {0x61, 0x62};
    memcpy_s(data, sizeof(arr), reinterpret_cast<void*>(arr), sizeof(arr));
    napi_value callArg1 = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &callArg1);

    uint32_t offset = 1;
    napi_value callArg2 = nullptr;
    napi_create_uint32(env, offset, &callArg2);

    uint32_t srcLength = 0;
    napi_value callArg3 = nullptr;
    napi_create_uint32(env, srcLength, &callArg3);

    napi_value args1[ARGS_NUMBER_THREE] = {callArg1, callArg2, callArg3};
    napi_call_function(env, instance, funcValue, ARGS_NUMBER_THREE, args1, &funcResultValue);

    napi_valuetype valuetype;
    napi_typeof(env, funcResultValue, &valuetype);
    ASSERT_EQ(valuetype, napi_undefined);
}

/**
 * @tc.name: TestFillBuffer
 * @tc.desc: test FillBuffer function
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TestFillBuffer, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value funcValue = nullptr;
    napi_value funcResultValue = nullptr;
    napi_value recv = nullptr;
    napi_create_object(env, &recv);
    OHOS::buffer::BufferInit(env, recv);

    napi_value bufferClass = nullptr;
    napi_get_named_property(env, recv, "Buffer", &bufferClass);

    int32_t type = 0;
    napi_value arg1 = nullptr;
    napi_create_int32(env, type, &arg1);
    int32_t length = 1;
    napi_value arg2 = nullptr;
    napi_create_uint32(env, length, &arg2);
    napi_value args[ARGS_NUMBER_TWO] = {arg1, arg2};
    napi_value instance;
    napi_new_instance(env, bufferClass, ARGS_NUMBER_TWO, args, &instance);
    napi_get_named_property(env, instance, "fillBuffer", &funcValue);

    napi_value callArg1;
    napi_new_instance(env, bufferClass, ARGS_NUMBER_TWO, args, &callArg1);

    uint32_t offset = 1;
    napi_value callArg2 = nullptr;
    napi_create_uint32(env, offset, &callArg2);

    uint32_t srcLength = 0;
    napi_value callArg3 = nullptr;
    napi_create_uint32(env, srcLength, &callArg3);

    napi_value args1[ARGS_NUMBER_THREE] = {callArg1, callArg2, callArg3};
    napi_call_function(env, instance, funcValue, ARGS_NUMBER_THREE, args1, &funcResultValue);

    napi_valuetype valuetype;
    napi_typeof(env, funcResultValue, &valuetype);
    ASSERT_EQ(valuetype, napi_undefined);
}

/**
 * @tc.name: TestGetBufferData
 * @tc.desc: test GetBufferData function
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TestGetBufferData, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value funcValue = nullptr;
    napi_value funcResultValue = nullptr;
    napi_value recv = nullptr;
    napi_create_object(env, &recv);
    OHOS::buffer::BufferInit(env, recv);

    napi_value bufferClass = nullptr;
    napi_get_named_property(env, recv, "Buffer", &bufferClass);

    int32_t type = 0;
    napi_value arg1 = nullptr;
    napi_create_int32(env, type, &arg1);
    int32_t length = 1;
    napi_value arg2 = nullptr;
    napi_create_uint32(env, length, &arg2);
    napi_value args[ARGS_NUMBER_TWO] = {arg1, arg2};
    napi_value instance;
    napi_new_instance(env, bufferClass, ARGS_NUMBER_TWO, args, &instance);
    napi_get_named_property(env, instance, "getBufferData", &funcValue);

    napi_call_function(env, instance, funcValue, 0, nullptr, &funcResultValue);

    napi_valuetype valuetype;
    napi_typeof(env, funcResultValue, &valuetype);
    ASSERT_EQ(valuetype, napi_object);
}

/**
 * @tc.name: TestGet
 * @tc.desc: test Get function
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TestGet, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value funcValue = nullptr;
    napi_value funcResultValue = nullptr;
    napi_value recv = nullptr;
    napi_create_object(env, &recv);
    OHOS::buffer::BufferInit(env, recv);

    napi_value bufferClass = nullptr;
    napi_get_named_property(env, recv, "Buffer", &bufferClass);

    int32_t type = 0;
    napi_value arg1 = nullptr;
    napi_create_int32(env, type, &arg1);
    int32_t length = 1;
    napi_value arg2 = nullptr;
    napi_create_uint32(env, length, &arg2);
    napi_value args[ARGS_NUMBER_TWO] = {arg1, arg2};
    napi_value instance;
    napi_new_instance(env, bufferClass, ARGS_NUMBER_TWO, args, &instance);
    napi_get_named_property(env, instance, "set", &funcValue);
    napi_call_function(env, instance, funcValue, ARGS_NUMBER_TWO, args, &funcResultValue);
    napi_get_named_property(env, instance, "get", &funcValue);
    napi_call_function(env, instance, funcValue, 1, &arg1, &funcResultValue);

    int32_t res = 0;
    napi_get_value_int32(env, funcResultValue, &res);
    ASSERT_EQ(res, 1);
}

/**
 * @tc.name: TestWriteInt32LE
 * @tc.desc: test WriteInt32LE function
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TestWriteInt32LE, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value funcValue = nullptr;
    napi_value funcResultValue = nullptr;
    napi_value recv = nullptr;
    napi_create_object(env, &recv);
    OHOS::buffer::BufferInit(env, recv);

    napi_value bufferClass = nullptr;
    napi_get_named_property(env, recv, "Buffer", &bufferClass);

    int32_t type = 0;
    napi_value arg1 = nullptr;
    napi_create_int32(env, type, &arg1);
    int32_t length = 1;
    napi_value arg2 = nullptr;
    napi_create_uint32(env, length, &arg2);
    napi_value args[ARGS_NUMBER_TWO] = {arg1, arg2};
    napi_value instance;
    napi_new_instance(env, bufferClass, ARGS_NUMBER_TWO, args, &instance);
    napi_get_named_property(env, instance, "writeInt32LE", &funcValue);

    napi_call_function(env, instance, funcValue, ARGS_NUMBER_TWO, args, &funcResultValue);

    napi_valuetype valuetype;
    napi_typeof(env, funcResultValue, &valuetype);
    ASSERT_EQ(valuetype, napi_undefined);
}

/**
 * @tc.name: TestReadInt32LE
 * @tc.desc: test ReadInt32LE function
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TestReadInt32LE, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value funcValue = nullptr;
    napi_value funcResultValue = nullptr;
    napi_value recv = nullptr;
    napi_create_object(env, &recv);
    OHOS::buffer::BufferInit(env, recv);

    napi_value bufferClass = nullptr;
    napi_get_named_property(env, recv, "Buffer", &bufferClass);

    int32_t type = 0;
    napi_value arg1 = nullptr;
    napi_create_int32(env, type, &arg1);
    int32_t length = 1;
    napi_value arg2 = nullptr;
    napi_create_uint32(env, length, &arg2);
    napi_value args[ARGS_NUMBER_TWO] = {arg1, arg2};
    napi_value instance;
    napi_new_instance(env, bufferClass, ARGS_NUMBER_TWO, args, &instance);
    napi_get_named_property(env, instance, "readInt32LE", &funcValue);

    napi_call_function(env, instance, funcValue, 1, &arg1, &funcResultValue);

    int32_t res = 0;
    napi_get_value_int32(env, funcResultValue, &res);
    ASSERT_EQ(res, 0);
}

/**
 * @tc.name: TestWriteUInt32BE
 * @tc.desc: test WriteUInt32BE function
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TestWriteUInt32BE, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value funcValue = nullptr;
    napi_value funcResultValue = nullptr;
    napi_value recv = nullptr;
    napi_create_object(env, &recv);
    OHOS::buffer::BufferInit(env, recv);

    napi_value bufferClass = nullptr;
    napi_get_named_property(env, recv, "Buffer", &bufferClass);

    int32_t type = 0;
    napi_value arg1 = nullptr;
    napi_create_int32(env, type, &arg1);
    int32_t length = 1;
    napi_value arg2 = nullptr;
    napi_create_uint32(env, length, &arg2);
    napi_value args[ARGS_NUMBER_TWO] = {arg1, arg2};
    napi_value instance;
    napi_new_instance(env, bufferClass, ARGS_NUMBER_TWO, args, &instance);
    napi_get_named_property(env, instance, "writeUInt32BE", &funcValue);

    napi_call_function(env, instance, funcValue, ARGS_NUMBER_TWO, args, &funcResultValue);

    napi_valuetype valuetype;
    napi_typeof(env, funcResultValue, &valuetype);
    ASSERT_EQ(valuetype, napi_undefined);
}

/**
 * @tc.name: TestReadUInt32BE
 * @tc.desc: test ReadUInt32BE function
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TestReadUInt32BE, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value funcValue = nullptr;
    napi_value funcResultValue = nullptr;
    napi_value recv = nullptr;
    napi_create_object(env, &recv);
    OHOS::buffer::BufferInit(env, recv);

    napi_value bufferClass = nullptr;
    napi_get_named_property(env, recv, "Buffer", &bufferClass);

    int32_t type = 0;
    napi_value arg1 = nullptr;
    napi_create_int32(env, type, &arg1);
    int32_t length = 1;
    napi_value arg2 = nullptr;
    napi_create_uint32(env, length, &arg2);
    napi_value args[ARGS_NUMBER_TWO] = {arg1, arg2};
    napi_value instance;
    napi_new_instance(env, bufferClass, ARGS_NUMBER_TWO, args, &instance);
    napi_get_named_property(env, instance, "readUInt32BE", &funcValue);

    napi_call_function(env, instance, funcValue, 1, &arg1, &funcResultValue);

    int32_t res = 0;
    napi_get_value_int32(env, funcResultValue, &res);
    ASSERT_EQ(res, 0);
}

/**
 * @tc.name: TestWriteUInt32LE
 * @tc.desc: test WriteUInt32LE function
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TestWriteUInt32LE, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value funcValue = nullptr;
    napi_value funcResultValue = nullptr;
    napi_value recv = nullptr;
    napi_create_object(env, &recv);
    OHOS::buffer::BufferInit(env, recv);

    napi_value bufferClass = nullptr;
    napi_get_named_property(env, recv, "Buffer", &bufferClass);

    int32_t type = 0;
    napi_value arg1 = nullptr;
    napi_create_int32(env, type, &arg1);
    int32_t length = 1;
    napi_value arg2 = nullptr;
    napi_create_uint32(env, length, &arg2);
    napi_value args[ARGS_NUMBER_TWO] = {arg1, arg2};
    napi_value instance;
    napi_new_instance(env, bufferClass, ARGS_NUMBER_TWO, args, &instance);
    napi_get_named_property(env, instance, "writeUInt32LE", &funcValue);

    napi_call_function(env, instance, funcValue, ARGS_NUMBER_TWO, args, &funcResultValue);

    napi_valuetype valuetype;
    napi_typeof(env, funcResultValue, &valuetype);
    ASSERT_EQ(valuetype, napi_undefined);
}

/**
 * @tc.name: TestReadUInt32LE
 * @tc.desc: test ReadUInt32LE function
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TestReadUInt32LE, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value funcValue = nullptr;
    napi_value funcResultValue = nullptr;
    napi_value recv = nullptr;
    napi_create_object(env, &recv);
    OHOS::buffer::BufferInit(env, recv);

    napi_value bufferClass = nullptr;
    napi_get_named_property(env, recv, "Buffer", &bufferClass);

    int32_t type = 0;
    napi_value arg1 = nullptr;
    napi_create_int32(env, type, &arg1);
    int32_t length = 1;
    napi_value arg2 = nullptr;
    napi_create_uint32(env, length, &arg2);
    napi_value args[ARGS_NUMBER_TWO] = {arg1, arg2};
    napi_value instance;
    napi_new_instance(env, bufferClass, ARGS_NUMBER_TWO, args, &instance);
    napi_get_named_property(env, instance, "readUInt32LE", &funcValue);

    napi_call_function(env, instance, funcValue, 1, &arg1, &funcResultValue);

    int32_t res = 0;
    napi_get_value_int32(env, funcResultValue, &res);
    ASSERT_EQ(res, 0);
}

/**
 * @tc.name: TestSubBuffer
 * @tc.desc: test SubBuffer function
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TestSubBuffer, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value funcValue = nullptr;
    napi_value funcResultValue = nullptr;
    napi_value recv = nullptr;
    napi_create_object(env, &recv);
    OHOS::buffer::BufferInit(env, recv);

    napi_value bufferClass = nullptr;
    napi_get_named_property(env, recv, "Buffer", &bufferClass);

    int32_t type = 0;
    napi_value arg1 = nullptr;
    napi_create_int32(env, type, &arg1);
    int32_t length = 1;
    napi_value arg2 = nullptr;
    napi_create_uint32(env, length, &arg2);
    napi_value args[ARGS_NUMBER_TWO] = {arg1, arg2};
    napi_value instance;
    napi_new_instance(env, bufferClass, ARGS_NUMBER_TWO, args, &instance);
    napi_get_named_property(env, instance, "subBuffer", &funcValue);

    napi_call_function(env, instance, funcValue, ARGS_NUMBER_TWO, args, &funcResultValue);

    napi_valuetype valuetype;
    napi_typeof(env, funcResultValue, &valuetype);
    ASSERT_EQ(valuetype, napi_object);
}

/**
 * @tc.name: TestCopy
 * @tc.desc: test Copy function
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TestCopy, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value funcValue = nullptr;
    napi_value funcResultValue = nullptr;
    napi_value recv = nullptr;
    napi_create_object(env, &recv);
    OHOS::buffer::BufferInit(env, recv);

    napi_value bufferClass = nullptr;
    napi_get_named_property(env, recv, "Buffer", &bufferClass);

    int32_t type = 0;
    napi_value arg1 = nullptr;
    napi_create_int32(env, type, &arg1);
    int32_t length = 1;
    napi_value arg2 = nullptr;
    napi_create_uint32(env, length, &arg2);
    napi_value args[ARGS_NUMBER_TWO] = {arg1, arg2};
    napi_value instance;
    napi_new_instance(env, bufferClass, ARGS_NUMBER_TWO, args, &instance);
    napi_get_named_property(env, instance, "copy", &funcValue);

    napi_value argCall1;
    napi_new_instance(env, bufferClass, ARGS_NUMBER_TWO, args, &argCall1);
    napi_value args1[ARG_NUMBER_FOUR] = {argCall1, arg1, arg1, arg2};
    napi_call_function(env, instance, funcValue, ARG_NUMBER_FOUR, args1, &funcResultValue);

    int32_t res = 0;
    napi_get_value_int32(env, funcResultValue, &res);
    ASSERT_EQ(res, 1);
}

/**
 * @tc.name: TestCompare
 * @tc.desc: test Compare function
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TestCompare, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value funcValue = nullptr;
    napi_value funcResultValue = nullptr;
    napi_value recv = nullptr;
    napi_create_object(env, &recv);
    OHOS::buffer::BufferInit(env, recv);

    napi_value bufferClass = nullptr;
    napi_get_named_property(env, recv, "Buffer", &bufferClass);

    int32_t type = 0;
    napi_value arg1 = nullptr;
    napi_create_int32(env, type, &arg1);
    int32_t length = 1;
    napi_value arg2 = nullptr;
    napi_create_uint32(env, length, &arg2);
    napi_value args[ARGS_NUMBER_TWO] = {arg1, arg2};
    napi_value instance;
    napi_new_instance(env, bufferClass, ARGS_NUMBER_TWO, args, &instance);
    napi_get_named_property(env, instance, "compare", &funcValue);

    napi_value argCall1;
    napi_new_instance(env, bufferClass, ARGS_NUMBER_TWO, args, &argCall1);
    napi_value args1[ARG_NUMBER_FOUR] = {argCall1, arg1, arg1, arg2};
    napi_call_function(env, instance, funcValue, ARG_NUMBER_FOUR, args1, &funcResultValue);

    int32_t res = 0;
    napi_get_value_int32(env, funcResultValue, &res);
    ASSERT_EQ(res, 0);
}

/**
 * @tc.name: TestToUtf8
 * @tc.desc: test ToUtf8 function
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TestToUtf8, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value funcValue = nullptr;
    napi_value funcResultValue = nullptr;
    napi_value recv = nullptr;
    napi_create_object(env, &recv);
    OHOS::buffer::BufferInit(env, recv);

    napi_value bufferClass = nullptr;
    napi_get_named_property(env, recv, "Buffer", &bufferClass);

    int32_t type = 0;
    napi_value arg1 = nullptr;
    napi_create_int32(env, type, &arg1);
    int32_t length = 1;
    napi_value arg2 = nullptr;
    napi_create_uint32(env, length, &arg2);
    napi_value args[ARGS_NUMBER_TWO] = {arg1, arg2};
    napi_value instance;
    napi_new_instance(env, bufferClass, ARGS_NUMBER_TWO, args, &instance);
    napi_get_named_property(env, instance, "toUtf8", &funcValue);

    napi_call_function(env, instance, funcValue, ARGS_NUMBER_TWO, args, &funcResultValue);

    napi_valuetype valuetype;
    napi_typeof(env, funcResultValue, &valuetype);
    ASSERT_EQ(valuetype, napi_string);
}

/**
 * @tc.name: TestToBase64
 * @tc.desc: test ToBase64 function
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TestToBase64, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value funcValue = nullptr;
    napi_value funcResultValue = nullptr;
    napi_value recv = nullptr;
    napi_create_object(env, &recv);
    OHOS::buffer::BufferInit(env, recv);

    napi_value bufferClass = nullptr;
    napi_get_named_property(env, recv, "Buffer", &bufferClass);

    int32_t type = 0;
    napi_value arg1 = nullptr;
    napi_create_int32(env, type, &arg1);
    int32_t length = 1;
    napi_value arg2 = nullptr;
    napi_create_uint32(env, length, &arg2);
    napi_value args[ARGS_NUMBER_TWO] = {arg1, arg2};
    napi_value instance;
    napi_new_instance(env, bufferClass, ARGS_NUMBER_TWO, args, &instance);
    napi_get_named_property(env, instance, "toBase64", &funcValue);

    napi_call_function(env, instance, funcValue, ARGS_NUMBER_TWO, args, &funcResultValue);

    napi_valuetype valuetype;
    napi_typeof(env, funcResultValue, &valuetype);
    ASSERT_EQ(valuetype, napi_string);
}

/**
 * @tc.name: TestIndexOf
 * @tc.desc: test IndexOf function
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TestIndexOf, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value funcValue = nullptr;
    napi_value funcResultValue = nullptr;
    napi_value recv = nullptr;
    napi_create_object(env, &recv);
    OHOS::buffer::BufferInit(env, recv);

    napi_value bufferClass = nullptr;
    napi_get_named_property(env, recv, "Buffer", &bufferClass);

    int32_t type = 0;
    napi_value arg1 = nullptr;
    napi_create_int32(env, type, &arg1);
    int32_t length = 1;
    napi_value arg2 = nullptr;
    napi_create_uint32(env, length, &arg2);
    napi_value args[ARGS_NUMBER_TWO] = {arg1, arg2};
    napi_value instance;
    napi_new_instance(env, bufferClass, ARGS_NUMBER_TWO, args, &instance);
    napi_get_named_property(env, instance, "indexOf", &funcValue);

    std::string src = "a";
    napi_value argCall1 = nullptr;
    napi_create_string_utf8(env, src.c_str(), src.size(), &argCall1);
    uint32_t offset = 1;
    napi_value argCall2 = nullptr;
    napi_create_uint32(env, offset, &argCall2);
    napi_value argCall3 = nullptr;
    std::string encodeType = "utf8";
    napi_create_string_utf8(env, encodeType.c_str(), encodeType.size(), &argCall3);
    uint32_t arg3 = 1;
    napi_value srcLength = nullptr;
    napi_create_uint32(env, arg3, &srcLength);
    napi_value argCall4 = nullptr;
    bool isReverse = false;
    napi_get_boolean(env, isReverse, &argCall4);
    napi_value args1[ARG_NUMBER_FOUR] = {argCall1, argCall2, argCall3, argCall4};

    napi_call_function(env, instance, funcValue, ARG_NUMBER_FOUR, args1, &funcResultValue);
    int32_t res = 0;
    napi_get_value_int32(env, funcResultValue, &res);
    ASSERT_EQ(res, -1);

    encodeType = "ascii";
    napi_create_string_utf8(env, encodeType.c_str(), encodeType.size(), &argCall3);
    args1[ARGS_NUMBER_TWO] = argCall3;
    napi_call_function(env, instance, funcValue, ARG_NUMBER_FOUR, args1, &funcResultValue);
    napi_get_value_int32(env, funcResultValue, &res);
    ASSERT_EQ(res, -1);

    encodeType = "utf16le";
    napi_create_string_utf8(env, encodeType.c_str(), encodeType.size(), &argCall3);
    args1[ARGS_NUMBER_TWO] = argCall3;
    napi_call_function(env, instance, funcValue, ARG_NUMBER_FOUR, args1, &funcResultValue);
    napi_get_value_int32(env, funcResultValue, &res);
    ASSERT_EQ(res, -1);

    encodeType = "base64";
    napi_create_string_utf8(env, encodeType.c_str(), encodeType.size(), &argCall3);
    args1[ARGS_NUMBER_TWO] = argCall3;
    napi_call_function(env, instance, funcValue, ARG_NUMBER_FOUR, args1, &funcResultValue);
    napi_get_value_int32(env, funcResultValue, &res);
    ASSERT_EQ(res, -1);

    isReverse = true;
    napi_get_boolean(env, isReverse, &argCall4);
    args1[ARGS_NUMBER_THREE] = argCall4;
    encodeType = "hex";
    napi_create_string_utf8(env, encodeType.c_str(), encodeType.size(), &argCall3);
    args1[ARGS_NUMBER_TWO] = argCall3;
    napi_call_function(env, instance, funcValue, ARG_NUMBER_FOUR, args1, &funcResultValue);
    napi_get_value_int32(env, funcResultValue, &res);
    ASSERT_EQ(res, -1);
}

/**
 * @tc.name: TestGetBytes
 * @tc.desc: test GetBytes function
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TestGetBytes, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value funcValue = nullptr;
    napi_value funcResultValue = nullptr;
    napi_value recv = nullptr;
    napi_create_object(env, &recv);
    OHOS::buffer::BlobInit(env, recv);

    napi_value bufferClass = nullptr;
    napi_get_named_property(env, recv, "Blob", &bufferClass);

    size_t byteLength = ARGS_NUMBER_TWO;
    void* data = nullptr;
    napi_value resultBuff = nullptr;
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    unsigned char arr[ARGS_NUMBER_TWO] = {0x61, 0x62};
    memcpy_s(data, sizeof(arr), reinterpret_cast<void*>(arr), sizeof(arr));
    napi_value callArg1 = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &callArg1);
    napi_value instance;
    napi_new_instance(env, bufferClass, 1, &callArg1, &instance);
    napi_get_named_property(env, instance, "getBytes", &funcValue);

    napi_call_function(env, instance, funcValue, 0, nullptr, &funcResultValue);

    napi_valuetype valuetype;
    napi_typeof(env, funcResultValue, &valuetype);
    ASSERT_EQ(valuetype, napi_object);
}

/**
 * @tc.name: TestArrayBufferAsync
 * @tc.desc: test ArrayBufferAsync function
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, TestArrayBufferAsync, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value funcValue = nullptr;
    napi_value funcResultValue = nullptr;
    napi_value recv = nullptr;
    napi_create_object(env, &recv);
    OHOS::buffer::BlobInit(env, recv);

    napi_value bufferClass = nullptr;
    napi_get_named_property(env, recv, "Blob", &bufferClass);

    size_t byteLength = ARGS_NUMBER_TWO;
    void* data = nullptr;
    napi_value resultBuff = nullptr;
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    unsigned char arr[ARGS_NUMBER_TWO] = {0x61, 0x62};
    memcpy_s(data, sizeof(arr), reinterpret_cast<void*>(arr), sizeof(arr));
    napi_value callArg1 = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &callArg1);
    napi_value instance;
    napi_new_instance(env, bufferClass, 1, &callArg1, &instance);
    napi_get_named_property(env, instance, "arraybuffer", &funcValue);

    napi_call_function(env, instance, funcValue, 0, nullptr, &funcResultValue);

    napi_valuetype valuetype;
    napi_typeof(env, funcResultValue, &valuetype);
    ASSERT_EQ(valuetype, napi_object);
}

/**
 * @tc.name: Testtext
 * @tc.desc: test text function
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, Testtext, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value funcValue = nullptr;
    napi_value funcResultValue = nullptr;
    napi_value recv = nullptr;
    napi_create_object(env, &recv);
    OHOS::buffer::BlobInit(env, recv);

    napi_value bufferClass = nullptr;
    napi_get_named_property(env, recv, "Blob", &bufferClass);

    size_t byteLength = ARGS_NUMBER_TWO;
    void* data = nullptr;
    napi_value resultBuff = nullptr;
    napi_create_arraybuffer(env, byteLength, &data, &resultBuff);
    unsigned char arr[ARGS_NUMBER_TWO] = {0x61, 0x62};
    memcpy_s(data, sizeof(arr), reinterpret_cast<void*>(arr), sizeof(arr));
    napi_value callArg1 = nullptr;
    napi_create_typedarray(env, napi_int8_array, byteLength, resultBuff, 0, &callArg1);
    napi_value instance;
    napi_new_instance(env, bufferClass, 1, &callArg1, &instance);
    napi_get_named_property(env, instance, "text", &funcValue);

    napi_call_function(env, instance, funcValue, 0, nullptr, &funcResultValue);

    napi_valuetype valuetype;
    napi_typeof(env, funcResultValue, &valuetype);
    ASSERT_EQ(valuetype, napi_object);
}