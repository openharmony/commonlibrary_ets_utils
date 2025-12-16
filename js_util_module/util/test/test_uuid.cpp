/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include <codecvt>

#include "commonlibrary/ets_utils/js_util_module/util/js_uuid.h"
#include "ohos/init_data.h"
#include "tools/log.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "securec.h"

/* @tc.name: DoParseUUIDTest005
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest005, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100005", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest006
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest006, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100006", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest007
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest007, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100007", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest008
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest008, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100008", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest009
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest009, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100009", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest010
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest010, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100010", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest011
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest011, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100011", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest012
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest012, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100012", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest013
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest013, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100013", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest014
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest014, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100014", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest015
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest015, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100015", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest016
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest016, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100016", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest017
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest017, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100017", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest018
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest018, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100018", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest019
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest019, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100019", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest020
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest020, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100020", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest021
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest021, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100021", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest022
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest022, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100022", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest023
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest023, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100023", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest024
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest024, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100024", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest025
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest025, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100025", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest026
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest026, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100026", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest027
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest027, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100027", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest028
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest028, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100028", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest029
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest029, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100029", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest030
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest030, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100030", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest031
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest031, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100031", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest032
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest032, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100032", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest033
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest033, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100033", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest034
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest034, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100034", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest035
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest035, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100035", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest036
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest036, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100036", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest037
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest037, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100037", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest038
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest038, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100038", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest039
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest039, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100039", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest040
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest040, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100040", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest041
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest041, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100041", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest042
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest042, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100042", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest043
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest043, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100043", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest044
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest044, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100044", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest045
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest045, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100045", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest046
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest046, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100046", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest047
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest047, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100047", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest048
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest048, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100048", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest049
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest049, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100049", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest050
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest050, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100050", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest051
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest051, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100051", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest052
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest052, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100052", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest053
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest053, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100053", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest054
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest054, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100054", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest055
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest055, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100055", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest056
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest056, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100056", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest057
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest057, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100057", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest058
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest058, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100058", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest059
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest059, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100059", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest060
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest060, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100060", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest061
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest061, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100061", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest062
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest062, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100062", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest063
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest063, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100063", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest064
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest064, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100064", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest065
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest065, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100065", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest066
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest066, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100066", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest067
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest067, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100067", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest068
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest068, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100068", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest069
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest069, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100069", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest070
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest070, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100070", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest071
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest071, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100071", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest072
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest072, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100072", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest073
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest073, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100073", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest074
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest074, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100074", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest075
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest075, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100075", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest076
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest076, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100076", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest077
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest077, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100077", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest078
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest078, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100078", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest079
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest079, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100079", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest080
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest080, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100080", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest081
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest081, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100081", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest082
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest082, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100082", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest083
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest083, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100083", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest084
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest084, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100084", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest085
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest085, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100085", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest086
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest086, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101186", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest087
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest087, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101187", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest088
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest088, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101188", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest089
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest089, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101189", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest090
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest090, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101190", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest091
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest091, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101191", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest092
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest092, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101192", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest093
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest093, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101193", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest094
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest094, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101194", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest095
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest095, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101195", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest096
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest096, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101196", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest097
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest097, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101197", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest098
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest098, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101198", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest099
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest099, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101199", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest100
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest100, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101200", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest101
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest101, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101201", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest102
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest102, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101295", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest103
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest103, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101203", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest104
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest104, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101204", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest105
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest105, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101205", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest106
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest106, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101206", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest107
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest107, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101207", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest108
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest108, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101208", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest109
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest109, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101209", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest110
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest110, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101210", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest111
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest111, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101211", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest112
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest112, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101212", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest113
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest113, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101213", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest114
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest114, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101214", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest115
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest115, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101215", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest116
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest116, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101216", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest117
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest117, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101217", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest118
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest118, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101218", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest119
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest119, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101219", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest220
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest220, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101320", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest221
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest221, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101308", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest222
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest222, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101322", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest223
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest223, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101323", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest224
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest224, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101324", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest225
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest225, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101325", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest226
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest226, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101326", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest227
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest227, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101327", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest228
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest228, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101328", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest229
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest229, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101329", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest230
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest230, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101330", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest231
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest231, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101331", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest232
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest232, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101332", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest233
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest233, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101333", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest234
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest234, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101334", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest235
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest235, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101335", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest236
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest236, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101336", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest237
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest237, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101337", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest238
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest238, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101338", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest239
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest239, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101339", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest240
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest240, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101340", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest241
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest241, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101341", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest242
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest242, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101342", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest243
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest243, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101343", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest244
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest244, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101344", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest245
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest245, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101345", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest246
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest246, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101346", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest247
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest247, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101347", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest248
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest248, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101348", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest249
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest249, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101349", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest250
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest250, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101350", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest251
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest251, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101351", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest252
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest252, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101352", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest253
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest253, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101353", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest254
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest254, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101354", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest255
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest255, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101355", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest256
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest256, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101356", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest257
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest257, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101357", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest258
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest258, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101358", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest259
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest259, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101359", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest260
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest260, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101460", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest261
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest261, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101461", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest262
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest262, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101462", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest263
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest263, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101463", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest264
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest264, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101464", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest265
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest265, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101465", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest266
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest266, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101466", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest267
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest267, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101467", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest268
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest268, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101468", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest269
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest269, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101469", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest270
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest270, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101470", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest271
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest271, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-65cc-4755-9b89-d6218d101471", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}
