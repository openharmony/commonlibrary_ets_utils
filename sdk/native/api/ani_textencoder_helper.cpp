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

#include "ani_textencoder.h"

namespace ani_helper {
constexpr uint8_t HIGER_4_BITS_MASK = 0xF0;
constexpr uint8_t FOUR_BYTES_STYLE = 0xF0;
constexpr uint8_t THREE_BYTES_STYLE = 0xE0;
constexpr uint8_t TWO_BYTES_STYLE1 = 0xD0;
constexpr uint8_t TWO_BYTES_STYLE2 = 0xC0;
constexpr uint32_t LOWER_10_BITS_MASK = 0x03FFU;
constexpr uint8_t LOWER_6_BITS_MASK = 0x3FU;
constexpr uint8_t LOWER_5_BITS_MASK = 0x1FU;
constexpr uint8_t LOWER_4_BITS_MASK = 0x0FU;
constexpr uint8_t LOWER_3_BITS_MASK = 0x07U;
constexpr uint32_t HIGH_AGENT_MASK = 0xD800U;
constexpr uint32_t LOW_AGENT_MASK = 0xDC00U;
constexpr uint32_t UTF8_VALID_BITS = 6;
constexpr uint32_t UTF16_SPECIAL_VALUE = 0x10000;

bool IsOneByte(uint8_t u8Char)
{
    return (u8Char & 0x80) == 0;
}

static bool Utf8ToUtf16LEToData(const unsigned char *data, std::u16string &u16Str,
                                size_t resultLengthLimit, size_t firstByteIndex, uint8_t c1)
{
    uint8_t c2 = data[1 + firstByteIndex]; // The second byte
    uint8_t c3 = data[2 + firstByteIndex]; // The third byte
    uint8_t c4 = data[3 + firstByteIndex]; // The forth byte
    // Calculate the UNICODE code point value (3 bits lower for the first byte, 6 bits for the other)
    // 3 : shift left 3 times of UTF8_VALID_BITS
    uint32_t codePoint = ((c1 & LOWER_3_BITS_MASK) << (3 * UTF8_VALID_BITS)) |
        // 2 : shift left 2 times of UTF8_VALID_BITS
        ((c2 & LOWER_6_BITS_MASK) << (2 * UTF8_VALID_BITS)) |
        ((c3 & LOWER_6_BITS_MASK) << UTF8_VALID_BITS) |
        (c4 & LOWER_6_BITS_MASK);
    // In UTF-16, U+10000 to U+10FFFF represent surrogate pairs with two 16-bit units
    if (codePoint >= UTF16_SPECIAL_VALUE) {
        if (u16Str.length() + 1 == resultLengthLimit) {
            return false;
        }
        codePoint -= UTF16_SPECIAL_VALUE;
        // 10 : a half of 20 , shift right 10 bits
        u16Str.push_back(static_cast<char16_t>((codePoint >> 10) | HIGH_AGENT_MASK));
        u16Str.push_back(static_cast<char16_t>((codePoint & LOWER_10_BITS_MASK) | LOW_AGENT_MASK));
    } else { // In UTF-16, U+0000 to U+D7FF and U+E000 to U+FFFF are Unicode code point values
        // U+D800 to U+DFFF are invalid characters, for simplicity,
        // assume it does not exist (if any, not encoded)
        u16Str.push_back(static_cast<char16_t>(codePoint));
    }
    return true;
}

std::u16string Utf8ToUtf16LE(std::string_view u8Str, bool *ok)
{
    std::u16string u16Str = u"";
    u16Str.reserve(u8Str.size());
    size_t len = u8Str.length();
    const unsigned char *data = reinterpret_cast<const unsigned char *>(u8Str.data());
    bool isOk = true;
    for (size_t i = 0; isOk && i < len;) {
        uint8_t c1 = data[i];
        if (IsOneByte(c1)) {
            i += 1; // 1 : Proceeds 1 byte
            u16Str.push_back(static_cast<char16_t>(c1));
            continue;
        }
        switch (c1 & HIGER_4_BITS_MASK) {
            case FOUR_BYTES_STYLE: {
                constexpr size_t threeMoreBytesToConsume = 3;
                if (i + threeMoreBytesToConsume >= len) {
                    isOk = false;
                    break;
                }
                uint8_t c2 = data[i + 1];
                uint8_t c3 = data[i + 2];
                uint8_t c4 = data[i + 3];
                i += (threeMoreBytesToConsume + 1);
                uint32_t codePoint = ((c1 & LOWER_3_BITS_MASK) << (3 * UTF8_VALID_BITS)) | // 3:multiple
                    ((c2 & LOWER_6_BITS_MASK) << (2 * UTF8_VALID_BITS)) | // 2:multiple
                    ((c3 & LOWER_6_BITS_MASK) << UTF8_VALID_BITS) | (c4 & LOWER_6_BITS_MASK);
                if (codePoint >= UTF16_SPECIAL_VALUE) {
                    codePoint -= UTF16_SPECIAL_VALUE;
                    u16Str.push_back(static_cast<char16_t>((codePoint >> 10) | HIGH_AGENT_MASK)); // 10:offset value
                    u16Str.push_back(static_cast<char16_t>((codePoint & LOWER_10_BITS_MASK) | LOW_AGENT_MASK));
                } else {
                    u16Str.push_back(static_cast<char16_t>(codePoint));
                }
                break;
            }
            case THREE_BYTES_STYLE: {
                constexpr size_t twoMoreBytesToConsume = 2;
                if (i + twoMoreBytesToConsume >= len) {
                    isOk = false;
                    break;
                }
                uint8_t c2 = data[i + 1];
                uint8_t c3 = data[i + 2];
                i += (twoMoreBytesToConsume + 1);
                uint32_t codePoint = ((c1 & LOWER_4_BITS_MASK) << (2 * UTF8_VALID_BITS)) | // 2:multiple
                    ((c2 & LOWER_6_BITS_MASK) << UTF8_VALID_BITS) | (c3 & LOWER_6_BITS_MASK);
                u16Str.push_back(static_cast<char16_t>(codePoint));
                break;
            }
            case TWO_BYTES_STYLE1:
            case TWO_BYTES_STYLE2: {
                constexpr size_t oneMoreByteToConsume = 1;
                if (i + oneMoreByteToConsume >= len) {
                    isOk = false;
                    break;
                }
                uint8_t c2 = data[i + 1];
                i += (oneMoreByteToConsume + 1);
                uint32_t codePoint = ((c1 & LOWER_5_BITS_MASK) << UTF8_VALID_BITS) | (c2 & LOWER_6_BITS_MASK);
                u16Str.push_back(static_cast<char16_t>(codePoint));
                break;
            }
            default: {
                isOk = false;
                break;
            }
        }
    }
    if (ok != nullptr) {
        *ok = isOk;
    }
    return u16Str;
}

std::u16string Utf8ToUtf16LE(std::string_view u8Str, size_t resultLengthLimit,
                             size_t *nInputCharsConsumed, bool *ok)
{
    std::u16string u16Str;
    u16Str.reserve(std::min(u8Str.length(), resultLengthLimit));
    const unsigned char *data = reinterpret_cast<const unsigned char *>(u8Str.data());
    bool isOk = true;
    bool continues = true;
    size_t inputCharCount = 0;
    size_t inputSizeBytes = u8Str.length();
    for (size_t i = 0; isOk && continues && i < inputSizeBytes;) {
        if (u16Str.length() == resultLengthLimit) {
            break;
        }
        uint8_t c1 = data[i]; // The first byte
        if (IsOneByte(c1)) { // only 1 byte represents the UNICODE code point
            i += 1; // 1 : Proceeds 1 byte
            u16Str.push_back(static_cast<char16_t>(c1));
            ++inputCharCount;
            continue;
        }
        switch (c1 & HIGER_4_BITS_MASK) {
            case FOUR_BYTES_STYLE: { // 4 byte characters, from 0x10000 to 0x10FFFF
                constexpr size_t threeMoreBytesToConsume = 3;
                // Stops if two u16 characters to be written to output while remaining space is only 1 u16 character.
                if (i + threeMoreBytesToConsume >= inputSizeBytes) {
                    isOk = false;
                    break;
                }
                continues = Utf8ToUtf16LEToData(data, u16Str, resultLengthLimit, i, c1);
                if (continues) {
                    i += (threeMoreBytesToConsume + 1);
                    inputCharCount += 2; // 2 : A four-byte character is considered 2 characters
                }
                break;
            }
            case THREE_BYTES_STYLE: { // 3 byte characters, from 0x800 to 0xFFFF
                constexpr size_t twoMoreBytesToConsume = 2;
                if (i + twoMoreBytesToConsume >= inputSizeBytes) {
                    isOk = false;
                    break;
                }
                uint8_t c2 = data[i + 1]; // The second byte
                uint8_t c3 = data[i + 2]; // The third byte
                i += (twoMoreBytesToConsume + 1);
                // Calculates the UNICODE code point value
                // (4 bits lower for the first byte, 6 bits lower for the other)
                // 2 : shift left 2 times of UTF8_VALID_BITS
                uint32_t codePoint = ((c1 & LOWER_4_BITS_MASK) << (2 * UTF8_VALID_BITS)) |
                    ((c2 & LOWER_6_BITS_MASK) << UTF8_VALID_BITS) |
                    (c3 & LOWER_6_BITS_MASK);
                u16Str.push_back(static_cast<char16_t>(codePoint));
                ++inputCharCount;
                break;
            }
            case TWO_BYTES_STYLE1: // 2 byte characters, from 0x80 to 0x7FF
            case TWO_BYTES_STYLE2: {
                constexpr size_t oneMoreByteToConsume = 1;
                if (i + oneMoreByteToConsume >= inputSizeBytes) {
                    isOk = false;
                    break;
                }
                uint8_t c2 = data[i + 1]; // The second byte
                i += (oneMoreByteToConsume + 1);
                // Calculates the UNICODE code point value
                // (5 bits lower for the first byte, 6 bits lower for the other)
                uint32_t codePoint = ((c1 & LOWER_5_BITS_MASK) << UTF8_VALID_BITS) |
                    (c2 & LOWER_6_BITS_MASK);
                u16Str.push_back(static_cast<char16_t>(codePoint));
                ++inputCharCount;
                break;
            }
            default: {
                isOk = false;
                break;
            }
        }
    }
    if (nInputCharsConsumed != nullptr) {
        *nInputCharsConsumed = inputCharCount;
    }
    if (ok != nullptr) {
        *ok = isOk;
    }
    return u16Str;
}

std::u16string Utf16LEToBE(std::u16string_view wstr)
{
    std::u16string str16 = u"";
    const char16_t *data = wstr.data();
    for (unsigned int i = 0; i < wstr.length(); i++) {
        char16_t wc = data[i];
        char16_t high = (wc >> 8) & 0x00FF; // 8 : offset value
        char16_t low = wc & 0x00FF;
        char16_t c16 = (low << 8) | high; // 8 : offset value
        str16.push_back(c16);
    }
    return str16;
}

std::string_view Utf8GetPrefix(std::string_view u8Str, size_t resultSizeBytesLimit,
                               size_t *nInputCharsConsumed, bool *ok)
{
    size_t len = u8Str.length();
    const unsigned char *data = reinterpret_cast<const unsigned char *>(u8Str.data());
    bool isOk = true;
    size_t i = 0;
    size_t inputCharCount = 0;
    for (; i < len && i < resultSizeBytesLimit;) {
        uint8_t c1 = data[i]; // The first byte
        if (IsOneByte(c1)) { // only 1 byte represents the UNICODE code point
            ++i;
            ++inputCharCount;
            continue;
        }
        bool continues = true;
        switch (c1 & HIGER_4_BITS_MASK) {
            case FOUR_BYTES_STYLE: { // 4 byte characters, from 0x10000 to 0x10FFFF
                constexpr size_t threeMoreBytesToConsume = 3;
                isOk = (i + threeMoreBytesToConsume < len);
                continues = (i + threeMoreBytesToConsume < resultSizeBytesLimit);
                if (isOk && continues) {
                    inputCharCount += 2; // 2 : 4-byte characters are considered 2 input characters.
                    i += (threeMoreBytesToConsume + 1);
                }
                break;
            }
            case THREE_BYTES_STYLE: { // 3 byte characters, from 0x800 to 0xFFFF
                constexpr size_t twoMoreBytesToConsume = 2;
                isOk = (i + twoMoreBytesToConsume < len);
                continues = (i + twoMoreBytesToConsume < resultSizeBytesLimit);
                if (isOk && continues) {
                    ++inputCharCount;
                    i += (twoMoreBytesToConsume + 1);
                }
                break;
            }
            case TWO_BYTES_STYLE1: // 2 byte characters, from 0x80 to 0x7FF
            case TWO_BYTES_STYLE2: {
                constexpr size_t oneMoreByteToConsume = 1;
                isOk = (i + oneMoreByteToConsume < len);
                continues = (i + oneMoreByteToConsume < resultSizeBytesLimit);
                if (isOk && continues) {
                    ++inputCharCount;
                    i += (oneMoreByteToConsume + 1);
                }
                break;
            }
            default: {
                isOk = false;
                break;
            }
        }
        if (!continues || !isOk) {
            break;
        }
    }
    if (nInputCharsConsumed != nullptr) {
        *nInputCharsConsumed = inputCharCount;
    }
    if (ok != nullptr) {
        *ok = isOk;
    }
    return u8Str.substr(0, i);
}
} // namespace ani_helper
