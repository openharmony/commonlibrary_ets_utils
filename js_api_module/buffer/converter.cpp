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

#include "commonlibrary/ets_utils/js_api_module/buffer/converter.h"

#include <codecvt>
#include <locale>

using namespace std;

namespace OHOS::buffer {
bool IsOneByte(uint8_t u8Char)
{
    return (u8Char & 0x80) == 0;
}

u16string Utf8ToUtf16BE(const string &u8Str, bool *ok)
{
    u16string u16Str = u"";
    u16Str.reserve(u8Str.size());
    string::size_type len = u8Str.length();

    const unsigned char *data = reinterpret_cast<const unsigned char *>(u8Str.data());

    bool isOk = true;
    for (string::size_type i = 0; i < len; ++i) {
        uint8_t c1 = data[i]; // The first byte
        if (IsOneByte(c1)) { // only 1 byte represents the UNICODE code point
            u16Str.push_back(static_cast<char16_t>(c1));
            continue;
        }
        switch (c1 & HIGER_4_BITS_MASK) {
            case FOUR_BYTES_STYLE: { // 4 byte characters, from 0x10000 to 0x10FFFF
                uint8_t c2 = data[++i]; // The second byte
                uint8_t c3 = data[++i]; // The third byte
                uint8_t c4 = data[++i]; // The forth byte
                // Calculate the UNICODE code point value (3 bits lower for the first byte, 6 bits for the other)
                // 3 : shift left 3 times of UTF8_VALID_BITS
                uint32_t codePoint = ((c1 & LOWER_3_BITS_MASK) << (3 * UTF8_VALID_BITS))
                                     // 2 : shift left 2 times of UTF8_VALID_BITS
                                     | ((c2 & LOWER_6_BITS_MASK) << (2 * UTF8_VALID_BITS))
                                     | ((c3 & LOWER_6_BITS_MASK) << UTF8_VALID_BITS)
                                     | (c4 & LOWER_6_BITS_MASK);

                // In UTF-16, U+10000 to U+10FFFF represent surrogate pairs with two 16-bit units
                if (codePoint >= UTF16_SPECIAL_VALUE) {
                    codePoint -= UTF16_SPECIAL_VALUE;
                    // 10 : a half of 20 , shift right 10 bits
                    u16Str.push_back(static_cast<char16_t>((codePoint >> 10) | HIGH_AGENT_MASK));
                    u16Str.push_back(static_cast<char16_t>((codePoint & LOWER_10_BITS_MASK) | LOW_AGENT_MASK));
                } else { // In UTF-16, U+0000 to U+D7FF and U+E000 to U+FFFF are Unicode code point values
                    // U+D800 to U+DFFF are invalid characters, for simplicity,
                    // assume it does not exist (if any, not encoded)
                    u16Str.push_back(static_cast<char16_t>(codePoint));
                }
                break;
            }
            case THREE_BYTES_STYLE: { // 3 byte characters, from 0x800 to 0xFFFF
                uint8_t c2 = data[++i]; // The second byte
                uint8_t c3 = data[++i]; // The third byte
                // Calculates the UNICODE code point value
                // (4 bits lower for the first byte, 6 bits lower for the other)
                // 2 : shift left 2 times of UTF8_VALID_BITS
                uint32_t codePoint = ((c1 & LOWER_4_BITS_MASK) << (2 * UTF8_VALID_BITS))
                                    | ((c2 & LOWER_6_BITS_MASK) << UTF8_VALID_BITS)
                                    | (c3 & LOWER_6_BITS_MASK);
                u16Str.push_back(static_cast<char16_t>(codePoint));
                break;
            }
            case TWO_BYTES_STYLE1: // 2 byte characters, from 0x80 to 0x7FF
            case TWO_BYTES_STYLE2: {
                uint8_t c2 = data[++i]; // The second byte
                // Calculates the UNICODE code point value
                // (5 bits lower for the first byte, 6 bits lower for the other)
                uint32_t codePoint = ((c1 & LOWER_5_BITS_MASK) << UTF8_VALID_BITS)
                                    | (c2 & LOWER_6_BITS_MASK);
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

u16string Utf16BEToLE(const u16string &wstr)
{
    u16string str16 = u"";
    const char16_t *data = wstr.data();
    for (unsigned int i = 0; i < wstr.length(); i++) {
        char16_t wc = data[i];
        char16_t high = (wc >> 8) & 0x00FF;
        char16_t low = wc & 0x00FF;
        char16_t c16 = (low << 8) | high;
        str16.push_back(c16);
    }
    return str16;
}

string Utf16BEToANSI(const u16string &wstr)
{
    string ret = "";
    for (u16string::const_iterator it = wstr.begin(); it != wstr.end(); ++it) {
        char16_t wc = (*it);
        // get the lower bit from the UNICODE code point
        char c = static_cast<char>(wc & LOWER_8_BITS_MASK);
        ret.push_back(c);
    }
    return ret;
}

string Utf8ToUtf16BEToANSI(const string &str)
{
    u16string u16Str = Utf8ToUtf16BE(str);
    string ret = Utf16BEToANSI(u16Str);
    return ret;
}

bool IsBase64Char(unsigned char c)
{
    return (isalnum(c) || (c == '+') || (c == '/'));
}

/**
* Base64Encode - Base64 encode
* @src: Data to be encoded
* @len: Length of the data to be encoded
* Returns: Allocated buffer of outLen bytes of encoded data,
* or empty string on failure
*/
string Base64Encode(const unsigned char *src, size_t len)
{
    if (src == nullptr) {
        return string();
    }
    unsigned char *out = nullptr;
    unsigned char *pos = nullptr;
    const unsigned char *pEnd = nullptr;
    const unsigned char *pStart = nullptr;
    size_t outLen = 4 * ((len + 2) / 3); // 3-byte blocks to 4-byte

    if (outLen < len) {
        return string(); // integer overflow
    }

    string outStr = "";
    outStr.resize(outLen);
    out = reinterpret_cast<unsigned char *>(&outStr[0]);

    pEnd = src + len;
    pStart = src;
    pos = out;
    // 3 : 3 bytes is just 24 bits which is 4 times of 6 bits
    while (pEnd - pStart >= 3) {
        // 2 : add two zeros in front of the first set of 6 bits to become a new 8 binary bits
        *pos = base64Table[pStart[0] >> 2];
        // 4 : add two zeros in front of the following second set of 6 bits to become the new 8 binary bits
        *(pos + 1) = base64Table[((pStart[0] & LOWER_2_BITS_MASK) << 4) | (pStart[1] >> 4)];
        // 2 : 4 : 6 : add two zeros in front of the following third set of 6 bits to become the new 8 binary bits
        *(pos + 2) = base64Table[((pStart[1] & LOWER_4_BITS_MASK) << 2) | (pStart[2] >> 6)];
        // 2 : 3 : add two zeros in front of the following forth set of 6 bits to become the new 8 binary bits
        *(pos + 3) = base64Table[pStart[2] & LOWER_6_BITS_MASK];
        // 4 : the pointer of pos scrolls off 4 bytes to point the next 4 bytes of encoded chars
        pos += 4;
        // 3 : the pointer of pStart scrolls off 3 bytes to point the next 3 bytes of which will be encoded chars
        pStart += 3;
    }

    // process the last set of less than 3 bytes of data
    if (pEnd - pStart > 0) {
        // 2 : add two zeros in front of the first set of 6 bits to become a new 8 binary bits
        *pos = base64Table[pStart[0] >> 2];
        if (pEnd - pStart == 1) { // one byte remaining
            // 4 : paddle the last two bits of the last byte with two zeros in front of it and four zeros after it
            *(pos + 1) = base64Table[(pStart[0] & LOWER_2_BITS_MASK) << 4];
            // 2 : fill in the missing bytes with '='
            *(pos + 2) = '=';
        } else { // two bytes remaining
            // 4 : add two zeros in front of the second set of 6 bits to become the new 8 binary bits
            *(pos + 1) = base64Table[((pStart[0] & LOWER_2_BITS_MASK) << 4) | (pStart[1] >> 4)];
            // 2 : paddle the last four bits of the last byte with two zeros in front of it and two zeros after it
            *(pos + 2) = base64Table[(pStart[1] & LOWER_4_BITS_MASK) << 2];
        }
        // 3 : fill in the missing bytes with '='
        *(pos + 3) = '=';
    }

    return outStr;
}

string Base64Decode(string const& encodedStr)
{
    size_t len = encodedStr.size();
    unsigned int index = 0;
    unsigned int cursor = 0;
    unsigned char charArray4[4] = {0}; // an array to stage a group of indexes for encoded string
    unsigned char charArray3[3] = {0}; // an array to stage a set of original string
    string ret = "";

    while ((encodedStr[cursor] != '=') && IsBase64Char(encodedStr[cursor])) {
        // stage a 4-byte string to charArray4
        charArray4[index] = encodedStr[cursor];
        index++;
        cursor++;
        if (index == 4) { // 4 : after 4 chars is assigned to charArray4
            // 4 : fill data into charArray4
            for (index = 0; index < 4; index++) {
                charArray4[index] = base64Table.find(charArray4[index]) & LOWER_8_BITS_MASK;
            }
            // get the last six bits of the first byte of charArray4 and the first valid
            // 2 : 4 : two bits(except two higer bits) of the second byte, combine them to a new byte
            charArray3[0] = (charArray4[0] << 2) + ((charArray4[1] & 0x30) >> 4);
            // get the last four bits of the second byte of charArray4 and the first valid
            // 4 : 2 : four bits(except two higer bits) of the third byte, combine them to a new byte
            charArray3[1] = ((charArray4[1] & LOWER_4_BITS_MASK) << 4) + ((charArray4[2] & MIDDLE_4_BITS_MASK) >> 2);
            // get the last two bits of the third byte of charArray4 and the forth byte,
            // 2 : 3 : 6 : combine them to a new byte
            charArray3[2] = ((charArray4[2] & LOWER_2_BITS_MASK) << 6) + charArray4[3];
            // 3 : assigns the decoded string to the return value
            for (index = 0; index < 3; index++) {
                ret += charArray3[index];
            }
            index = 0;
        }
        if (cursor > len - 1) {
            break;
        }
    }

    if (index != 0) {
        // fill data into charArray4
        for (unsigned int i = 0; i < index; i++) {
            charArray4[i] = base64Table.find(charArray4[i]) & LOWER_8_BITS_MASK;
        }
        // get the last six bits of the first byte of charArray4 and the first valid
        // 2 : 4 : two bits(except two higer bits) of the second byte, combine them to a new byte
        charArray3[0] = (charArray4[0] << 2) + ((charArray4[1] & 0x30) >> 4);
        // get the last four bits of the second byte of charArray4 and the first valid
        // 4 : 2 : four bits(except two higer bits) of the third byte, combine them to a new byte
        charArray3[1] = ((charArray4[1] & LOWER_4_BITS_MASK) << 4) + ((charArray4[2] & LOWER_6_BITS_MASK) >> 2);
        // assigns the decoded string to the return value
        for (unsigned int i = 0; i < index - 1; i++) {
            ret += charArray3[i];
        }
    }

    return ret;
}

bool IsValidHex(const string &hex)
{
    bool isValid = false;
    for (unsigned int i = 0; i < hex.size(); i++) {
        char c = hex.at(i);
        // 0 ~ 9, A ~ F, a ~ f
        if ((c <= '9' && c >= '0') || (c <= 'F' && c >= 'A') || (c <= 'f' && c >= 'a')) {
            isValid = true;
        } else {
            isValid = false;
            break;
        }
    }
    return isValid;
}

string HexDecode(const string &hexStr)
{
    auto arr = hexStr.c_str();
    string nums = "";
    unsigned int arrSize = hexStr.size();

    // 2 : means a half length of hex str's size
    for (unsigned int i = 0; i < arrSize / 2; i++) {
        string hexStrTmp = "";
        int num = 0;
        // 2 : offset is i * 2
        hexStrTmp.push_back(arr[i * 2]);
        // 2 : offset is i * 2 + 1
        hexStrTmp.push_back(arr[i * 2 + 1]);
        if (!IsValidHex(hexStrTmp)) {
            break;
        }
        // 16 : the base is 16
        num = stoi(hexStrTmp, nullptr, 16);
        nums.push_back(static_cast<char>(num));
    }

    return nums;
}

// Find the position of the last character in pat from patIndex
int GetGoodSuffixLengthByLastChar(uint8_t *pat, int patIndex, int patLen)
{
    int lastIndex = patLen - 1;
    int index = -1;
    while (patIndex >= 0) {
        if (pat[patIndex] == pat[lastIndex]) {
            index = patIndex;
            break;
        } else {
            patIndex--;
        }
    }
    return lastIndex - index;
}
// Find the position of the first character in pat from patIndex
int GetGoodSuffixLengthByFirstChar(uint8_t *pat, int patIndex, int tarlen)
{
    int indexOfNextFirstChar = tarlen;
    for (int i = patIndex; i < tarlen; i++) {
        if (pat[0] == pat[i]) {
            indexOfNextFirstChar = i;
            break;
        }
    }
    return indexOfNextFirstChar;
}

// Match forward from patIndex to get the position of the singleChar in the pat
// and the length of the bad character
int GetBadCharLengthInReverseOrder(uint8_t *pat, char singleChar, int patIndex)
{
    int index = -1;
    for (int i = patIndex - 1; i >= 0; --i) {
        if (pat[i] == singleChar) {
            index = i;
            break;
        }
    }
    return patIndex - index;
}

// Get the position of character c in pat
int GetBadCharLengthInSequence(uint8_t *pat, char singleChar, int patIndex, int tarlen)
{
    int resIndex = tarlen;
    for (int i = patIndex; i < tarlen; i++) {
        if (singleChar == pat[i]) {
            resIndex = i;
            break;
        }
    }
    return resIndex;
}

int FindLastIndex(uint8_t *source, uint8_t *target, int soulen, int tarlen)
{
    if (source == nullptr || target == nullptr) {
        return -1;
    }
    if (soulen < tarlen || tarlen == 0) {
        return -1;
    }
    int i = soulen - tarlen;
    int j = 0;

    while (i >= 0) {
        if (source[i] == target[j]) {
            if (j == tarlen - 1) {
                return i - (tarlen - 1);
            }
            i++;
            j++;
        } else {
            if (j == 0) {
                int badValue = GetBadCharLengthInSequence(target, source[i], j, tarlen);
                i = i - badValue;
                j = 0;
            } else {
                int badValue = GetBadCharLengthInSequence(target, source[i], j, tarlen);
                int goodSuffix = GetGoodSuffixLengthByFirstChar(target, j, tarlen);
                int distance = badValue > goodSuffix ? badValue : goodSuffix;
                i = i - distance;
                j = 0;
            }
        }
    }
    return -1;
}

int FindIndex(uint8_t* source, uint8_t* target, int soulen, int tarlen)
{
    if (source == nullptr || target == nullptr) {
        return -1;
    }
    if (soulen < tarlen || tarlen == 0) {
        return -1;
    }
    int i = tarlen - 1;
    int j = tarlen - 1;
    while (i < soulen) {
        if (source[i] == target[j]) {
            if (j == 0) {
                return i;
            }
            i--;
            j--;
        } else {
            if (j == tarlen - 1) {
                int badValue = GetBadCharLengthInReverseOrder(target, source[i], j);
                i = i + badValue;
                j = tarlen - 1;
            } else {
                int badValue = GetBadCharLengthInReverseOrder(target, source[i], j);
                int goodSuffix = GetGoodSuffixLengthByLastChar(target, j, tarlen);
                int distance = badValue > goodSuffix ? badValue : goodSuffix;
                i = i + tarlen - 1 - j + distance;
                j = tarlen - 1;
            }
        }
    }
    return -1;
}
}
