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

#include "commonlibrary/ets_utils/js_util_module/util/js_uuid.h"

#include "securec.h"
#include "utils/log.h"

#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS::Util {
static thread_local std::queue<UUID> g_uuidCache;

unsigned char CharToHex(char in)
{
    unsigned char res = 0;  // 0: initialization
    switch (in) {
        case '0': res = HEX_ZERO_FLG; break;
        case '1': res = HEX_ONE_FLG; break;
        case '2': res = HEX_TWO_FLG; break;
        case '3': res = HEX_THREE_FLG; break;
        case '4': res = HEX_FOUR_FLG; break;
        case '5': res = HEX_FIVE_FLG; break;
        case '6': res = HEX_SIX_FLG; break;
        case '7': res = HEX_SEVEN_FLG; break;
        case '8': res = HEX_EIGHT_FLG; break;
        case '9': res = HEX_NINE_FLG; break;
        case 'a': res = HEX_TEN_FLG; break;
        case 'b': res = HEX_ELEVEN_FLG; break;
        case 'c': res = HEX_TWELVE_FLG; break;
        case 'd': res = HEX_THIRTEEN_FLG; break;
        case 'e': res = HEX_FOURTEEN_FLG; break;
        case 'f': res = HEX_FIFTEEN_FLG; break;
        case 'A': res = HEX_TEN_FLG; break;
        case 'B': res = HEX_ELEVEN_FLG; break;
        case 'C': res = HEX_TWELVE_FLG; break;
        case 'D': res = HEX_THIRTEEN_FLG; break;
        case 'E': res = HEX_FOURTEEN_FLG; break;
        case 'F': res = HEX_FIFTEEN_FLG; break;
        default : res = HEX_ZERO_FLG;
    }
    return res;
}

unsigned char HexToChar(unsigned char in)
{
    unsigned char res = '0';
    switch (in) {
        case HEX_ZERO_FLG: res = '0'; break;
        case HEX_ONE_FLG: res = '1'; break;
        case HEX_TWO_FLG: res = '2'; break;
        case HEX_THREE_FLG: res = '3'; break;
        case HEX_FOUR_FLG: res = '4'; break;
        case HEX_FIVE_FLG: res = '5'; break;
        case HEX_SIX_FLG: res = '6'; break;
        case HEX_SEVEN_FLG: res = '7'; break;
        case HEX_EIGHT_FLG: res = '8'; break;
        case HEX_NINE_FLG: res = '9'; break;
        case HEX_TEN_FLG: res = 'a'; break;
        case HEX_ELEVEN_FLG: res = 'b'; break;
        case HEX_TWELVE_FLG: res = 'c'; break;
        case HEX_THIRTEEN_FLG: res = 'd'; break;
        case HEX_FOURTEEN_FLG: res = 'e'; break;
        case HEX_FIFTEEN_FLG: res = 'f'; break;
        default : res = 'x';
    }
    return res;
}

unsigned char ConvertBits(std::string &input)
{
    unsigned char temp = 0; // 0: initialization
    if (input[0] == '-') {
        input.erase(0, 1);
    }
    temp = CharToHex(input[0]);
    temp *= HEX_SIXTEEN_FLG;
    input.erase(0, 1);
    temp += CharToHex(input[0]);
    input.erase(0, 1);
    return temp;
}

bool GenerateUuid(unsigned char *data, int32_t size)
{
    unsigned char buf[UUID_SIZE] = { 0 };  // 0: initialization
    if (memcpy_s(data, size, buf, size) != EOK) {
        return false;
    }
    int32_t len = 0;
    while (len < size) {
        len += RAND_priv_bytes(data, size - len);
    }
    data[HEX_SIX_FLG] = (data[HEX_SIX_FLG] & 0x0F) | 0x40; // 0x0F,0x40 Operate the mark
    int m = 0x8;    // Upper of numerical range
    int n = 0xb;    // down of numerical range
    int r = static_cast<int>(data[HEX_EIGHT_FLG]);
    unsigned char num = static_cast<unsigned char>(r % (n - m + 1) + m);
    data[HEX_EIGHT_FLG] = (data[HEX_EIGHT_FLG] & 0x0F) | (num << 4);  // 0x0F,4 Operate the mark
    return true;
}

bool GetUUID(napi_env env, bool entropyCache, UUID &uuid)
{
    uint32_t size = g_uuidCache.size();
    if ((entropyCache == true) && (size != 0)) {
        uuid = g_uuidCache.front();
        g_uuidCache.pop();
    } else {
        if (size > MAX_CACHE_MASK) {
            for (uint32_t i = 0; i < size; i++) {
                g_uuidCache.pop();
            }
        }
        bool res = GenerateUuid(uuid.elements, sizeof(uuid.elements));
        if (!res) {
            napi_throw_error(env, "-1", "uuid generate failed");
            return false;
        }
        g_uuidCache.push(uuid);
        res = GenerateUuid(uuid.elements, sizeof(uuid.elements));
        if (!res) {
            napi_throw_error(env, "-1", "uuid generate failed");
            return false;
        }
    }
    return true;
}

std::string GetStringUUID(napi_env env, bool entropyCache)
{
    UUID uuid;
    std::string uuidString = "";
    if (!GetUUID(env, entropyCache, uuid)) {
        uuidString = '0';
    } else {
        uuidString = GetFormatUUID(uuid);
    }
    return uuidString;
}

std::string GetFormatUUID(const UUID &uuid)
{
    std::string format = "";
    for (size_t i = 0; i < sizeof(uuid.elements); i++) {
        unsigned char value = uuid.elements[i];
        if (i >= HEX_FOUR_FLG && i % 2 == 0 && i <= HEX_TEN_FLG) {  // 2: step value
            format += "-";
        }
        format += HexToChar(value >> HEX_FOUR_FLG);
        unsigned char high = value & 0xF0;  // Operate the mark
        if (high == 0) {
            format += HexToChar(value);
        } else {
            format += HexToChar(value % (value & high));
        }
    }
    return format;
}

napi_value GetBinaryUUID(napi_env env, bool entropyCache)
{
    UUID uuid;
    GetUUID(env, entropyCache, uuid);
    void *data = nullptr;
    napi_value arrayBuffer = nullptr;
    size_t bufferSize = sizeof(uuid.elements);
    napi_create_arraybuffer(env, bufferSize, &data, &arrayBuffer);
    if (memcpy_s(data, bufferSize, uuid.elements, bufferSize) != EOK) {
        HILOG_ERROR("get uuid memcpy_s failed");
        return nullptr;
    }
    napi_value result = nullptr;
    napi_create_typedarray(env, napi_uint8_array, bufferSize, arrayBuffer, 0, &result);
    return result;
}

napi_value DoParseUUID(napi_env env, napi_value src)
{
    UUID uuid;
    std::string buffer = "";
    size_t bufferSize = 0;  // 0: initialization
    napi_status status = napi_ok;
    status = napi_get_value_string_utf8(env, src, nullptr, 0, &bufferSize);
    if (status != napi_ok) {
        HILOG_ERROR("can not get src size");
        return nullptr;
    }
    buffer.resize(bufferSize);
    status = napi_get_value_string_utf8(env, src, buffer.data(), bufferSize + 1, &bufferSize);
    if (status != napi_ok) {
        HILOG_ERROR("can not get src value");
        return nullptr;
    }
    void *data = reinterpret_cast<void*>(uuid.elements);
    napi_value arrayBuffer = nullptr;
    size_t outLen = sizeof(uuid.elements);
    napi_create_arraybuffer(env, outLen, &data, &arrayBuffer);
    unsigned char *count = static_cast<unsigned char*>(data);
    while (!buffer.empty()) {
        *count = ConvertBits(buffer);
        count++;
    }
    napi_value result = nullptr;
    napi_create_typedarray(env, napi_uint8_array, outLen, arrayBuffer, 0, &result);
    return result;
}
}