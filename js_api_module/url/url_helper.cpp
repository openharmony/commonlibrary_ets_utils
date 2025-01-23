/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "url_helper.h"

#include <chrono>
#include <vector>

#include "tools/log.h"

namespace OHOS::Url {
std::string EncodePercentEncoding(const std::string_view inputString, const uint16_t codeMap[])
{
    auto firstToEncode = std::find_if(inputString.begin(), inputString.end(), [codeMap](char current) {
        uint8_t currentValue = static_cast<uint8_t>(current);
        return NeedEncode(codeMap, currentValue);
    });
    if (firstToEncode == inputString.end()) {
        return std::string(inputString);
    }
    std::string encodeString;
    encodeString.reserve(inputString.length() * PERCENT_ENCODING_LENGTH);
    encodeString.append(inputString.data(), firstToEncode - inputString.begin());
    for (auto index = firstToEncode; index != inputString.end(); ++index) {
        uint8_t currentValue = static_cast<uint8_t>(*index);
        if (NeedEncode(codeMap, currentValue)) {
            encodeString.push_back('%');
            encodeString.push_back(HEX_CHAR_MAP[currentValue >> SHIFT_SIZE]);
            encodeString.push_back(HEX_CHAR_MAP[currentValue & 0x0F]);
        } else {
            encodeString.push_back(*index);
        }
    }
    return encodeString;
}

// 110XXXXX 10XXXXXXX
// 1110XXXX 10XXXXXXX 10XXXXXXX
// 11110XXX 10XXXXXXX 10XXXXXXX 10XXXXXXXX
bool CheckUTF8Enble(const std::string_view inputString, size_t i, char currentChar, int count, char* chars)
{
    if (count <= 0) {
        return false;
    }
    int index = count - 1;
    chars[index] = currentChar;
    char charactor;
    while (i < inputString.size() && index > 0) {
        if (PercentCharDecodable(inputString, i, charactor)) {
            // 6, 0x2, subChar need begin with 10XXXXXX
            if (((charactor >> 6) ^ 0x2) != 0) {
                return false;
            }
            i += (HEX_PAIR_LENGTH + 1);
            index--;
            chars[index] = charactor;
        } else {
            return false;
        }
    }
    return index == 0;
}

std::string DecodePercentEncoding(const std::string_view inputString)
{
    if (inputString.find("%") == std::string_view::npos) {
        return std::string(inputString);
    }
    std::string result;
    result.reserve(inputString.size());
    size_t i = 0;
    char chars[CHARS_LENGTH];
    char ch;
    while (i < inputString.size()) {
        // convert to decimal characters and append to the result
        if (PercentCharDecodable(inputString, i, ch)) {
            auto length = GetCharLength(ch);
            if (CheckUTF8Enble(inputString, i + HEX_PAIR_LENGTH + 1, ch, length, chars)) {
                AppendChars(result, i, length, chars);
            } else {
                result += inputString.substr(i, (HEX_PAIR_LENGTH + 1));
                i += (HEX_PAIR_LENGTH + 1);
            }
        } else {
            // normal character
            result += inputString[i];
            ++i;
        }
    }
    return result;
}

void StringAnalyzing(std::string_view inputString, std::vector<KeyValue>& results)
{
    if (!inputString.empty() && ('?' == inputString.front() || '&' == inputString.front())) {
        inputString.remove_prefix(1);
    }
    auto segmentProcess = [](const std::string_view current, std::vector<KeyValue>& res) {
        if (current.empty()) {
            res.emplace_back("", "");
            return;
        }
        auto equalIndex = current.find('=');
        std::string name;
        std::string value;
        std::string decodeName;
        std::string decodeValue;
        if (equalIndex == std::string_view::npos) {
            name = std::string(current);
        } else {
            name = std::string(current.substr(0, equalIndex));
            value = std::string(current.substr(equalIndex + 1));
        }
        std::replace(name.begin(), name.end(), '+', ' ');
        decodeName = DecodePercentEncoding(name);
        if (!value.empty()) {
            std::replace(value.begin(), value.end(), '+', ' ');
            decodeValue = DecodePercentEncoding(value);
        }
        res.emplace_back(decodeName, decodeValue);
    };
    while (!inputString.empty()) {
        auto segmentIndex = inputString.find('&');
        segmentProcess(inputString.substr(0, segmentIndex), results);
        inputString.remove_prefix(segmentIndex + 1);
        if (segmentIndex == std::string_view::npos) {
            break;
        }
    }
}
} // namespace OHOS::Url