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

#ifndef BUFFER_CONVERTER_H
#define BUFFER_CONVERTER_H

#include <string>

#include "tools/log.h"

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

constexpr uint32_t LOWER_EIGHT_BITS_MASK = 0x00FF;
constexpr uint8_t HIGER_4_BITS_MASK = 0xF0;
constexpr uint8_t FOUR_BYTES_STYLE = 0xF0;
constexpr uint8_t THREE_BYTES_STYLE = 0xE0;
constexpr uint8_t TWO_BYTES_STYLE1 = 0xD0;
constexpr uint8_t TWO_BYTES_STYLE2 = 0xC0;
constexpr uint32_t LOWER_10_BITS_MASK = 0x03FFU;
constexpr uint32_t LOWER_8_BITS_MASK = 0x00FFU;
constexpr uint8_t LOWER_6_BITS_MASK = 0x3FU;
constexpr uint8_t LOWER_5_BITS_MASK = 0x1FU;
constexpr uint8_t LOWER_4_BITS_MASK = 0x0FU;
constexpr uint8_t LOWER_3_BITS_MASK = 0x07U;
constexpr uint8_t LOWER_2_BITS_MASK = 0x03U;
constexpr uint8_t MIDDLE_4_BITS_MASK = 0x3CU;
constexpr uint32_t HIGH_AGENT_MASK = 0xD800U;
constexpr uint32_t LOW_AGENT_MASK = 0xDC00U;
constexpr uint32_t UTF8_VALID_BITS = 6;
constexpr uint32_t UTF8_ONE_BYTE_MAX = 0x007F;
constexpr uint32_t UTF8_ONE_BYTE_SCALE = UTF8_ONE_BYTE_MAX + 1;
constexpr uint32_t UTF8_TWO_BYTES_MAX = 0x07FF;
constexpr uint32_t HIGH_AGENT_RANGE_FROM = 0xD800;
constexpr uint32_t HIGH_AGENT_RANGE_TO = 0xDBFF;
constexpr uint32_t LOW_AGENT_RANGE_FROM = 0xDC00;
constexpr uint8_t UTF8_TWO_BYTES_HEAD_BYTE_MASK = 0xC0;
constexpr uint8_t UTF8_TAIL_BYTE_MASK = 0x80;
constexpr uint8_t UTF8_THREE_BYTES_HEAD_BYTE_MASK = 0xE0;
constexpr uint8_t UTF8_FOUR_BYTES_HEAD_BYTE_MASK = 0xF0;
constexpr uint32_t UTF16_SPECIAL_VALUE = 0x10000;
const std::string BASE64_TABLE = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const std::string BASE64URL_TABLE = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

/**
* IsOneByte - checks whether a charactor in a utf8 string is a one byte coding or not
* @u8Char: a uint8_t char
* Returns: if the highest bit of u8Char is 0, return true, else ,return false;
*/
bool IsOneByte(uint8_t u8Char);
bool IsBase64Char(unsigned char c);

std::u16string Utf8ToUtf16BE(const std::string &u8Str, bool *ok = nullptr);
std::string Utf16BEToANSI(const std::wstring &wstr);
std::u16string Utf16BEToLE(const std::u16string &wstr);
std::string Utf8ToUtf16BEToANSI(const std::string &str);
std::string Base64Encode(const unsigned char *src, size_t len, EncodingType type);
std::string Base64Decode(std::string const& encodedStr, EncodingType type);
std::string HexDecode(const std::string &hexStr);
int FindLastIndex(uint8_t *source, uint8_t *target, int soulen, int tarlen);
int FindIndex(uint8_t* source, uint8_t* target, int soulen, int tarlen);
int GetGoodSuffixLengthByLastChar(uint8_t *pat, int patIndex, int patLen);
int GetGoodSuffixLengthByFirstChar(uint8_t *pat, int patIndex, int tarlen);
int GetBadCharLengthInReverseOrder(uint8_t *pat, char singleChar, int patIndex);
int GetBadCharLengthInSequence(uint8_t *pat, char singleChar, int patIndex, int tarlen);
} // namespace OHOS::Buffer
#endif // BUFFER_CONVERTER_H
