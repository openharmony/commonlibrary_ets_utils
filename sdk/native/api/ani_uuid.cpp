/**
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ani_uuid.h"
#include "tools/log.h"
#include <cstdint>
#include <cstdlib>
#include <array>
#include <sstream>
#include <securec.h>
#include <sys/types.h>
#include <random>

namespace OHOS::ETSUtil {

constexpr int UUID_LEN = 37;
constexpr uint32_t NULL_FOUR_HIGH_BITS_IN_16 = 0x0FFF;
constexpr uint32_t RFC4122_UUID_VERSION_MARKER = 0x4000;
constexpr uint32_t NULL_TWO_HIGH_BITS_IN_16 = 0x3FFF;
constexpr uint32_t RFC4122_UUID_RESERVED_BITS = 0x8000;

template <typename S>
S GenRandUint()
{
    static auto device = std::random_device();
    static auto randomGenerator = std::mt19937(device());
    static auto range = std::uniform_int_distribution<S>();

    return range(randomGenerator);
}

ani_object ThrowError(ani_env *env, std::string message)
{
    ani_string errString;
    env->String_NewUTF8(message.c_str(), message.size(), &errString);
    static const char *className = "L@ohos/util/util/BusinessError;";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        HILOG_ERROR("Not found %s", className);
        return nullptr;
    }

    ani_method errorCtor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", "Lstd/core/String;:V", &errorCtor)) {
        HILOG_ERROR("Class_FindMethod <ctor> Failed");
        return nullptr;
    }

    ani_object errorObj;
    if (ANI_OK != env->Object_New(cls, errorCtor, &errorObj, errString)) {
        HILOG_ERROR("Object_New Array Faild");
    }
    return errorObj;
}

std::string GenUuid4(ani_env *env)
{
    std::array<char, UUID_LEN> uuidStr = {0};
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
    int n = snprintf_s(
        uuidStr.begin(), UUID_LEN, UUID_LEN - 1, "%08x-%04x-%04x-%04x-%012x", GenRandUint<uint32_t>(),
        GenRandUint<uint16_t>(), (GenRandUint<uint16_t>() & NULL_FOUR_HIGH_BITS_IN_16) | RFC4122_UUID_VERSION_MARKER,
        (GenRandUint<uint16_t>() & NULL_TWO_HIGH_BITS_IN_16) | RFC4122_UUID_RESERVED_BITS, GenRandUint<uint64_t>());
    if ((n < 0) || (n > static_cast<int>(UUID_LEN))) {
        env->ThrowError(static_cast<ani_error>(ThrowError(env, "GenerateRandomUUID failed")));
        return std::string();
    }
    std::stringstream res;
    res << uuidStr.data();

    return res.str();
}

std::string ETSApiUtilHelperGenerateRandomUUID(ani_env *env, bool entropyCache)
{
    static std::string lastGeneratedUUID;
    if (entropyCache != true || lastGeneratedUUID.empty()) {
        lastGeneratedUUID = GenUuid4(env);
    }
    return lastGeneratedUUID;
}
}