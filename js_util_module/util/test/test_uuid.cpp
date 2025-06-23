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