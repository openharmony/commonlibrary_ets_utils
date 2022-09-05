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

#include "commonlibrary/ets_utils/js_util_module/util/js_uuid.h"
#include "utils/log.h"

#include "napi/native_api.h"
#include "napi/native_node_api.h"

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


/* @tc.name: GetStringUUIDTest001
 * @tc.desc: Test Generate a random RFC 4122 version 4 UUID.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, GetStringUUIDTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    std::string uuid = OHOS::Util::GetStringUUID(env, true);
    ASSERT_EQ(uuid.length(), 36);
}

/* @tc.name: GetBinaryUUIDTest002
 * @tc.desc: Test Generate a random RFC 4122 version 4 UUID.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, GetBinaryUUIDTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value arr = OHOS::Util::GetBinaryUUID(env, true);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}

/* @tc.name: DoParseUUIDTest003
 * @tc.desc: Parse a UUID from the string standard representation as described in the RFC 4122 version 4.
 * @tc.type: FUNC
 */
HWTEST_F(NativeEngineTest, DoParseUUIDTest003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value src = nullptr;
    napi_create_string_utf8(env, "84bdf796-66cc-4655-9b89-d6218d100f9c", NAPI_AUTO_LENGTH, &src);
    napi_value arr = OHOS::Util::DoParseUUID(env, src);
    napi_typedarray_type type = napi_int8_array;
    size_t byteOffset = 0;
    size_t length = 0;
    void* resultData = nullptr;
    napi_value resultBuffer = nullptr;
    napi_get_typedarray_info(env, arr, &type, &length, &resultData, &resultBuffer, &byteOffset);
    ASSERT_EQ(length, 16);
}
