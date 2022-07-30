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

#ifndef UTIL_JS_UUID_H_
#define UTIL_JS_UUID_H_

#include <cstring>
#include <iostream>
#include <iomanip>
#include <openssl/rand.h>
#include <queue>
#include <sys/sysinfo.h>
#include <unistd.h>

#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS::Util {
constexpr const int UUID_SIZE = 16;
struct UUID {
    unsigned char elements[16] = {0};
};

enum ConvertFlags {
    HEX_ZERO_FLG = 0x0,
    HEX_ONE_FLG = 0x1,
    HEX_TWO_FLG = 0x2,
    HEX_THREE_FLG = 0x3,
    HEX_FOUR_FLG = 0x4,
    HEX_FIVE_FLG = 0x5,
    HEX_SIX_FLG = 0x6,
    HEX_SEVEN_FLG = 0x7,
    HEX_EIGHT_FLG = 0x8,
    HEX_NINE_FLG = 0x9,
    HEX_TEN_FLG = 0xA,
    HEX_ELEVEN_FLG = 0xB,
    HEX_TWELVE_FLG = 0xC,
    HEX_THIRTEEN_FLG = 0xD,
    HEX_FOURTEEN_FLG = 0xE,
    HEX_FIFTEEN_FLG = 0xF,
    HEX_SIXTEEN_FLG = 0X10,
    MAX_CACHE_MASK = 128,
};

napi_value DoParseUUID(napi_env env, napi_value src);
napi_value GetBinaryUUID(napi_env env, bool entropyCache);
std::string GetStringUUID(napi_env env, bool entropyCache);
bool GenerateUuid(unsigned char *data, int32_t size);
bool GetUUID(napi_env env, bool entropyCache, UUID &uuid);
std::string GetFormatUUID(const UUID &uuid);
unsigned char CharToHex(char in);
unsigned char ConvertBits(std::string &input);
unsigned char HexToChar(unsigned char _in);
}
#endif // UTIL_JS_UUID_H_