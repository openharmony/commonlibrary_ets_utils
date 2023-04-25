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

#include "js_url.h"
#include <regex>
#include <sstream>
#include "securec.h"
#include "unicode/stringpiece.h"
#include "unicode/unistr.h"
#include "utils/log.h"
namespace OHOS::Url {
    std::map<std::string, int> g_head = {
        {"ftp:", 21}, {"file:", -1}, {"gopher:", 70}, {"http:", 80},
        {"https:", 443}, {"ws:", 80}, {"wss:", 443}
    };

    std::vector<std::string> g_doubleSegment = {
        "..", ".%2e", ".%2E", "%2e.", "%2E.",
        "%2e%2e", "%2E%2E", "%2e%2E", "%2E%2e"
    };

    std::vector<std::string> g_singlesegment = { ".", "%2e", "%2E" };
    std::vector<std::string> g_specialSymbols = {
        "@", "%40", "#", "%23", "=", "%3D", ":", "%3A",
        "/", "%2F", ";", "%3B", "?", "%3F"
    };
    std::vector<char> g_specialcharacter = {
        '\0', '\t', '\n', '\r', ' ', '#', '%', '/', ':', '?',
        '@', '[', '\\', ']'
    };

    std::bitset<static_cast<size_t>(BitsetStatusFlag::MAX_BIT_SIZE)> g_specialCharForBit;

    void PreliminaryWork()
    {
        std::vector<char> g_specialSymbolsTmp = {'#', '%', '/', ':', '?', '@', '[', '\\', ']', '<', '>', '^', '|'};
        size_t invalidCharLength = static_cast<size_t>(BitsetStatusFlag::BIT_ASCII_32);
        for (size_t i = 0; i <= invalidCharLength; ++i) {
            g_specialCharForBit.set(i);
        }
        size_t len = g_specialSymbolsTmp.size();
        for (size_t i = 0; i < len; ++i) {
            g_specialCharForBit.set(g_specialSymbolsTmp[i]);
        }
        g_specialCharForBit.set(static_cast<size_t>(BitsetStatusFlag::BIT_ASCII_127));
    }

    bool CheckCharacter(std::string data, std::bitset<static_cast<size_t>(BitsetStatusFlag::MAX_BIT_SIZE)> rule)
    {
        size_t dataLen = data.size();
        for (size_t i = 0; i < dataLen; ++i) {
            if (static_cast<int>(data[i]) >= 0 &&
                static_cast<int>(data[i]) < static_cast<int>(BitsetStatusFlag::MAX_BIT_SIZE)) {
                bool IsIllegal = rule.test(data[i]);
                if (IsIllegal) {
                    return false;
                }
            }
        }
        return true;
    }

    void ReplaceSpecialSymbols(std::string& input, std::string& oldstr, std::string& newstr)
    {
        size_t oldlen = oldstr.size();
        while (true) {
            size_t pos = 0;
            if ((pos = input.find(oldstr)) != std::string::npos) {
                input.replace(pos, oldlen, newstr);
                continue;
            }
                break;
        }
    }

    template<typename T>
    bool IsASCIITabOrNewline(const T ch)
    {
        return (ch == '\t' || ch == '\n' || ch == '\r');
    }

    template<typename T>
    bool IsHexDigit(const T ch)
    {
        if (isdigit(ch) || (ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f')) {
            return true;
        }
        return false;
    }

    std::string DecodeSpecialChars(std::string input)
    {
        std::string temp = input;
        size_t len = temp.size();
        if (input.empty()) {
            return temp;
        }
        size_t pos = temp.find("%");
        while (pos != std::string::npos && pos < len - 2) { // 2:end subscript backspace
            if (IsHexDigit(temp[pos + 1]) && IsHexDigit(temp[pos + 2])) { // 2:Determine the second character after %
                std::string subStr = temp.substr(pos + 1, 2); // 2:Truncate the last two digits of the %
                int octNum = 0;
                if (sscanf_s(subStr.c_str(), "%x", &octNum) == -1) {
                    HILOG_ERROR("sscanf_s is falie");
                    return temp;
                }
                std::string convertedChar(1, static_cast<char>(octNum));
                temp.replace(pos, 3, convertedChar); // 3:Replace the percent character with the corresponding char
                len = len - 2; // 2:After the replacement, the length of the string is reduced by two
            }
            pos = temp.find("%", pos + 1);
        }
        return temp;
    }

    void DeleteC0OrSpace(std::string& str)
    {
        if (str.empty()) {
            return;
        }
        size_t i = 0;
        size_t strlen = str.size();
        while (i < strlen) {
            if (str[i] >= '\0' && str[i] <= ' ') {
                i++;
                continue;
            }
                break;
        }
        str = str.substr(i);
        strlen = str.size();
        for (i = strlen - 1; i != 0; i--) {
            if (str[i] >= '\0' && str[i] <= ' ') {
                str.pop_back();
                continue;
            }
                break;
        }
    }

    void DeleteTabOrNewline(std::string& str1)
    {
        for (auto item = str1.begin(); item != str1.end();) {
            if (IsASCIITabOrNewline(*item)) {
                item = str1.erase(item);
            } else {
                ++item;
            }
        }
    }

    bool IsSpecial(std::string scheme)
    {
        auto temp = g_head.count(scheme);
        if (temp > 0) {
            return true;
        }
        return false;
    }

    bool AnalysisScheme(std::string& input, std::string& scheme,
        std::bitset<static_cast<size_t>(BitsetStatusFlag::BIT_STATUS_11)>& flags)
    {
        if (!isalpha(input[0])) {
            flags.set(static_cast<size_t>(BitsetStatusFlag::BIT0));
            return false;
        } else {
            size_t strlen = input.size();
            for (size_t i = 0; i < strlen - 1; ++i) {
                if ((isalnum(input[i]) || input[i] == '+' || input[i] == '-' || input[i] == '.') &&
                    isupper(input[i])) {
                        input[i] = static_cast<size_t>(tolower(input[i]));
                }
                if (!isalnum(input[i]) && input[i] != '+' && input[i] != '-' && input[i] != '.') {
                    flags.set(static_cast<size_t>(BitsetStatusFlag::BIT0));
                    // 0:Bit 0 Set to true,The URL analysis failed
                    return false;
                }
            }
            scheme = input;
            if (IsSpecial(scheme)) {
                flags.set(static_cast<size_t>(BitsetStatusFlag::BIT1));
            }
            return true;
        }
    }

    void AnalysisFragment(const std::string& input, std::string& fragment,
        std::bitset<static_cast<size_t>(BitsetStatusFlag::BIT_STATUS_11)>& flags)
    {
        fragment = input;
        flags.set(static_cast<size_t>(BitsetStatusFlag::BIT8));
    }

    void AnalysisQuery(const std::string& input, std::string& query,
        std::bitset<static_cast<size_t>(BitsetStatusFlag::BIT_STATUS_11)>& flags)
    {
        query = input;
        flags.set(static_cast<size_t>(BitsetStatusFlag::BIT7));
    }
    void AnalysisUsernameAndPasswd(std::string& input, std::string& username, std::string& password,
        std::bitset<static_cast<size_t>(BitsetStatusFlag::BIT_STATUS_11)>& flags)
    {
        int pos = static_cast<int>(input.size()) - 1;
        for (; pos >= 0; pos--) {
            if (input[pos] == '@') {
                break;
            }
        }
        std::string userAndPasswd = input.substr(0, pos);
        input = input.substr(pos + 1);
        if (userAndPasswd.empty()) {
            return;
        }
        if (userAndPasswd.find('@') != std::string::npos) {
            while (true) {
                size_t posTmp = 0;
                if ((posTmp = userAndPasswd.find('@')) != std::string::npos) {
                    userAndPasswd = userAndPasswd.replace(posTmp, 1, "%40");
                } else {
                    break;
                }
            }
        }
        if (userAndPasswd.find(':') != std::string::npos) {
            size_t position = userAndPasswd.find(':');
            std::string user = userAndPasswd.substr(0, position);
            std::string keyWord = userAndPasswd.substr(position + 1);
            if (!user.empty()) {
                username = user;
                flags.set(static_cast<size_t>(BitsetStatusFlag::BIT2));
            }
            if (!keyWord.empty()) {
                password = keyWord;
                flags.set(static_cast<size_t>(BitsetStatusFlag::BIT3));
            }
        } else {
            username = userAndPasswd;
            flags.set(static_cast<size_t>(BitsetStatusFlag::BIT2));
        }
    }

    void AnalysisPath(std::string& input, std::vector<std::string>& path,
        std::bitset<static_cast<size_t>(BitsetStatusFlag::BIT_STATUS_11)>& flags, bool isSpecial)
    {
        std::vector<std::string> temp;
        size_t pos = 0;
        while (((pos = input.find('/')) != std::string::npos) ||
            ((pos = input.find('\\')) != std::string::npos && isSpecial)) {
            temp.push_back(input.substr(0, pos));
            input = input.substr(pos + 1);
        }
        temp.push_back(input);
        size_t length = temp.size();
        for (size_t it = 0; it < length; ++it) {
            auto result = find(g_doubleSegment.begin(), g_doubleSegment.end(), temp[it]);
            if (result != g_doubleSegment.end()) {
                if (path.empty() && it == length - 1) {
                    path.emplace_back("");
                    flags.set(static_cast<size_t>(BitsetStatusFlag::BIT6));
                }
                if (path.empty()) {
                    continue;
                }
                path.pop_back();
                if (it == length - 1) {
                    path.emplace_back("");
                    flags.set(static_cast<size_t>(BitsetStatusFlag::BIT6));
                }
                continue;
            }
            result = find(g_singlesegment.begin(), g_singlesegment.end(), temp[it]);
            if (result != g_singlesegment.end() && it == length - 1) {
                path.emplace_back("");
                flags.set(static_cast<size_t>(BitsetStatusFlag::BIT6));
                continue;
            }
            if (result == g_singlesegment.end()) {
                path.push_back(temp[it]);
                flags.set(static_cast<size_t>(BitsetStatusFlag::BIT6));
            }
        }
    }

    void AnalysisPort(std::string input, UrlData& urlinfo,
        std::bitset<static_cast<size_t>(BitsetStatusFlag::BIT_STATUS_11)>& flags)
    {
        for (auto i : input) {
            if (!isdigit(i)) {
                flags.set(static_cast<size_t>(BitsetStatusFlag::BIT0));
                return;
            }
        }
        int it = stoi(input);
        const int maxPort = 65535; // 65535:Maximum port number
        if (it > maxPort) {
            flags.set(static_cast<size_t>(BitsetStatusFlag::BIT0));
            return;
        }
        flags.set(static_cast<size_t>(BitsetStatusFlag::BIT5));
        for (auto i : g_head) {
            if (i.first == urlinfo.scheme && i.second == it) {
                urlinfo.port = -1;
                flags.set(static_cast<size_t>(BitsetStatusFlag::BIT5), 0);
                return;
            }
        }
        urlinfo.port = it;
    }

    void AnalysisOpaqueHost(std::string input, std::string& host,
        std::bitset<static_cast<size_t>(BitsetStatusFlag::BIT_STATUS_11)>& flags)
    {
        size_t strlen = input.size();
        for (size_t i = 0; i < strlen; ++i) {
            char ch = input[i];
            auto result = find(g_specialcharacter.begin(), g_specialcharacter.end(), ch);
            if (ch != '%' && (result != g_specialcharacter.end())) {
                flags.set(static_cast<size_t>(BitsetStatusFlag::BIT0));
                return;
            }
        }
        host = input;
        flags.set(static_cast<size_t>(BitsetStatusFlag::BIT4));
    }

    std::string DealIpv4(std::string str)
    {
        std::vector<std::string> temp;
        size_t pos = str.rfind(":");
        size_t index = pos;
        size_t left = pos + 1;
        char hexVal[3] = { 0 };
        std::string val = "";
        while ((pos = str.find(".", left)) != std::string::npos) {
            val = str.substr(left, pos - left);
            if (sprintf_s(hexVal, sizeof(hexVal), "%02x", stoi(val)) == -1) {
                HILOG_ERROR("sprintf_s is falie");
                return val;
            }

            temp.push_back(hexVal);
            left = pos + 1;
            }
        val = str.substr(left);
        if (sprintf_s(hexVal, sizeof(hexVal), "%02x", stoi(val)) == -1) {
            HILOG_ERROR("sprintf_s is falie");
            return val;
        }
        temp.push_back(hexVal);
        std::string res = str.substr(0, index);
        res = res + ":" + temp[0] + temp[1] + ":" + temp[2] + temp[3]; // 2:subscript 3:subscript
        return res;
    }

    void FormatIpv6(std::string& str)
    {
        size_t pos = str.find("::");
        size_t index = pos;
        if (pos != std::string::npos) {
            size_t left = 0;
            int count = 0;
            while ((pos = str.find(":", left)) != std::string::npos) {
                count++;
                left = pos + 1;
            }
            int size = 7 - (count - 2); // 7:point number 2:Continuous colon number
            std::string temp = "";
            for (int i = 0; i < size - 1; ++i) {
                temp += ":0";
            }
            temp += ":";
            str.replace(index, 2, temp); // 2:jump"::"
            if (index == 0) {
                str = "0" + str;
            }
        }
    }

    void RemoveLeadingZeros(std::vector<std::string> &ipv6)
    {
        size_t len = ipv6.size();
        for (size_t i = 0; i < len; ++i) {
            size_t strLen = ipv6[i].size();
            size_t count = 0;
            size_t j = 0;
            for (j = 0; j < strLen; ++j) {
                if (ipv6[i][j] != '0') {
                    break;
        }
                    count++;
                }
            if (count == strLen) {
                ipv6[i] = "0";
            } else if (count != 0) {
                ipv6[i] = ipv6[i].substr(j);
                }
            }
        }

    std::string ZeroCompression(std::vector<std::string> &ipv6)
    {
        size_t maxIndex = 0;
        size_t maxSize = 0;
        size_t index = 0;
        size_t size = 0;
        bool isNeedZeroCompression = false;
        size_t len = ipv6.size();
        for (size_t i = 0; i < len; ++i) {
            index = i;
            size = 0;
            while (i < len && ipv6[i] == "0") {
                isNeedZeroCompression = true;
                size++;
                i++;
                }
            if (maxSize < size) {
                maxSize = size;
                maxIndex = index;
            }
            }
        std::string res = "";
        size_t ipv6Len = ipv6.size();
        for (size_t i = 0; i < ipv6Len; ++i) {
            if (isNeedZeroCompression && i == maxIndex) {
                if (maxIndex == 0) {
                    res += "::";
        } else {
                    res += ":";
        }
                i += maxSize - 1;
                continue;
    }
            res += ipv6[i];
            i != (ipv6Len - 1) ? res += ":" : "";
        }
        return res;
    }

    void ToLower(std::string &str)
    {
        size_t strLen = str.size();
        for (size_t i = 0; i < strLen; ++i) {
            if (isupper(str[i])) {
                str[i] = static_cast<size_t>(tolower(str[i]));
        }
    }
    }

    std::string Compress(std::string str)
    {
        std::vector<std::string> temp;
        size_t pos = 0;
        size_t left = 0;
        while ((pos = str.find(":", left)) != std::string::npos) {
            temp.push_back(str.substr(left, pos - left));
            left = pos + 1;
        }
        temp.push_back(str.substr(left));
        RemoveLeadingZeros(temp);
        std::string res = ZeroCompression(temp);
        ToLower(res);
        return res;
        }

    void IPv6Host(std::string& input, std::string& host,
        std::bitset<static_cast<size_t>(BitsetStatusFlag::BIT_STATUS_11)>& flags)
    {
        std::regex ipv6("(::|(:((:[0-9A-Fa-f]{1,4}){1,7}))|(([0-9A-Fa-f]{1,4}:){7}([0-9A-Fa-f]{1,4}|:))|"
                        "(([0-9A-Fa-f]{1,4}:){1}(((:[0-9A-Fa-f]{1,4}){1,6})|:))|(([0-9A-Fa-f]{1,4}:){2}"
                        "(((:[0-9A-Fa-f]{1,4}){1,5})|:))|(([0-9A-Fa-f]{1,4}:){3}(((:[0-9A-Fa-f]{1,4}){1,4})"
                        "|:))|(([0-9A-Fa-f]{1,4}:){4}(((:[0-9A-Fa-f]{1,4}){1,3})|:))|(([0-9A-Fa-f]{1,4}:){5}"
                        "(((:[0-9A-Fa-f]{1,4}){1,2})|:))|(([0-9A-Fa-f]{1,4}:){6}(:[0-9A-Fa-f]{1,4}|:))|"
                        "(((:(:[0-9A-Fa-f]{1,4}){0,5}:)|(([0-9A-Fa-f]{1,4}:){1}(:[0-9A-Fa-f]{1,4}){0,4}:)"
                        "|(([0-9A-Fa-f]{1,4}:){2}(:[0-9A-Fa-f]{1,4}){0,3}:)|(([0-9A-Fa-f]{1,4}:){3}"
                        "(:[0-9A-Fa-f]{1,4}){0,2}:)|(([0-9A-Fa-f]{1,4}:){4}(:[0-9A-Fa-f]{1,4})?:)|"
                        "(([0-9A-Fa-f]{1,4}:){5}:)|(([0-9A-Fa-f]{1,4}:){6}))((25[0-5]|2[0-4]\\d|1\\d{2}|"
                        "[1-9]\\d|\\d)\\.){3}(25[0-5]|2[0-4]\\d|1\\d{2}|[1-9]\\d|\\d)))(%[a-zA-Z0-9._]+)?");
        if (!std::regex_match(input, ipv6)) {
                flags.set(static_cast<size_t>(BitsetStatusFlag::BIT0));
                return;
            }
        size_t pos = 0;
        pos = input.find('.');
        if (pos != std::string::npos) {
            input = DealIpv4(input);
        }
        FormatIpv6(input);
        input = Compress(input);
        host = "[" + input + "]";
        flags.set(static_cast<size_t>(BitsetStatusFlag::BIT4));
        flags.set(static_cast<size_t>(BitsetStatusFlag::BIT10));
    }

    bool IsRadix(std::string num, std::string radix)
    {
        size_t len = num.size();
        for (size_t i = 0; i < len; ++i) {
            if (radix.find(num[i]) == std::string::npos) {
                return false;
                }
            }
        return true;
        }

    bool IsNumber(std::string num, int &radix)
    {
        size_t len = num.size();
        if (len > 2 && num[0] == '0' && (num[1] == 'x' || num[1] == 'X')) { // 2:hex head length
            radix = 16; // 16:hex
            return IsRadix(num.substr(2), "0123456789abcdefABCDEF"); // 2:jump 0x
        } else if (len > 1 && num[0] == '0') {
            radix = 8; // 8:octal
            return IsRadix(num.substr(1), "01234567");
        } else if (IsRadix(num, "0123456789")) {
            radix = 10; // 10:decimal
            return true;
    }

        return false;
        }

    std::string BinaryConversion(std::string num, int radix)
    {
        int val = 0;
        if (radix == 16) { // 16:hex
            if (sscanf_s(num.c_str(), "%x", &val) == -1) {
                HILOG_ERROR("sscanf_s is falie");
                return num;
        }
            return std::to_string(val);
        } else if (radix == 8) { // 8:octal
            if (sscanf_s(num.c_str(), "%o", &val) == -1) {
                HILOG_ERROR("sscanf_s is falie");
                return num;
            }
            return std::to_string(val);
        } else {
            return num;
        }
    }

    bool RemovalIpv4(std::vector<std::string> &temp, std::string str)
    {
        size_t pos = 0;
        size_t left = 0;
        while ((pos = str.find(".", left)) != std::string::npos) {
            temp.push_back(str.substr(left, pos - left));
            left = pos + 1;
            }
        temp.push_back(str.substr(left));
        size_t tmpLen = temp.size();
        std::vector<std::string> res;
        for (size_t i = 0; i < tmpLen; ++i) {
            int radix = 0;
            if (IsNumber(temp[i], radix)) {
                res.push_back(BinaryConversion(temp[i], radix));
            } else {
                return false;
            }
        }
        temp = res;
        return true;
    }

    int IsFormatIpv4(std::vector<std::string> nums)
    {
        size_t len = nums.size();
        for (size_t i = 0; i < len; ++i) {
            if (stoi(nums[i]) > 255) { // 255:ipv4 max value
                return i;
            }
        }
        return -1;
    }

    std::string SplitNum(std::string num, size_t &number)
    {
        int val = stoi(num);
        std::vector<std::string> nums;
        std::string res = "";
        while (val > 0) {
            int numConver = val % 256; // 256:ipv4 max value
            nums.push_back(std::to_string(numConver));
            val /= 256; // 256:ipv4 max value
            }
        for (int i = static_cast<int>(nums.size()) - 1; i >= 0; --i) {
            res += nums[i] + ".";
        }
        number = nums.size();
        return res.substr(0, res.size() - 1);
    }

    void FormatIpv4(std::vector<std::string> nums, std::string& host,
        std::bitset<static_cast<size_t>(BitsetStatusFlag::BIT_STATUS_11)> &flags)
    {
        size_t len = nums.size();
        int index = IsFormatIpv4(nums);
        std::string res = "";
        if (index == -1) {
            for (size_t i = 0; i < len - 1; ++i) {
                res += nums[i] + ".";
                }
            for (size_t i = 0; i < 4 - len; ++i) { // 4:ipv4 max size
                res += "0.";
            }
            res += nums[len - 1];
            host = res;
            flags.set(static_cast<size_t>(BitsetStatusFlag::BIT4));
        } else if (index == static_cast<int>(len - 1)) {
            for (size_t i = 0; i < len - 1; ++i) {
                res += nums[i] + ".";
        }
            size_t number = 0;
            std::string temp = SplitNum(nums[index], number);
            if (number + (len - 1) > 4) { // 4:ipv4 max size
                flags.set(static_cast<size_t>(BitsetStatusFlag::BIT0));
                return;
        }
            for (size_t i = 0; i < 4 - (len - 1 + number); ++i) { // 4:ipv4 max size
                temp = "0." + temp;
        }
            host = res + temp;
            flags.set(static_cast<size_t>(BitsetStatusFlag::BIT4));
        } else {
            flags.set(static_cast<size_t>(BitsetStatusFlag::BIT0));
            return;
            }
            }

    void AnalyseIPv4(const std::string& input, std::string& host,
        std::bitset<static_cast<size_t>(BitsetStatusFlag::BIT_STATUS_11)>& flags)
    {
        bool isipv4 = false;
        std::vector<std::string> temp;
        isipv4 = RemovalIpv4(temp, input);
        size_t tempLen = temp.size();
        std::string res = "";
        for (size_t i = 0; i < tempLen; ++i) {
            res += temp[i];
            if (i != tempLen - 1) {
                res += ".";
            }
        }
        if (isipv4) {
            if (tempLen > 4) { // 4:ipv4 max size
                ToLower(res);
                host = res;
                flags.set(static_cast<size_t>(BitsetStatusFlag::BIT4));
            } else if (tempLen == 4) { // 4:ipv4 max size
                if (IsFormatIpv4(temp) == -1) {
                    host = res;
                    flags.set(static_cast<size_t>(BitsetStatusFlag::BIT4));
            } else {
                    flags.set(static_cast<size_t>(BitsetStatusFlag::BIT0));
                }
            } else {
                FormatIpv4(temp, host, flags);
            }
        } else {
            ToLower(res);
            host = res;
        flags.set(static_cast<size_t>(BitsetStatusFlag::BIT4));
        }
    }
    void AnalysisHost(std::string& input, std::string& host,
        std::bitset<static_cast<size_t>(BitsetStatusFlag::BIT_STATUS_11)>& flags, bool special)
    {
        if (input.empty()) {
            flags.set(static_cast<size_t>(BitsetStatusFlag::BIT0));
            return;
        }
        if (input[0] == '[') {
            if ((input[input.length() - 1]) == ']') {
                size_t  b = input.length();
                input = input.substr(1, b - 2); // 2:Truncating Strings
                IPv6Host(input, host, flags);
                return;
            } else {
                flags.set(static_cast<size_t>(BitsetStatusFlag::BIT0));
                return;
            }
        }
        if (!special) {
            AnalysisOpaqueHost(input, host, flags);
            return;
        }
        std::string decodeInput = DecodeSpecialChars(input);
        if (!CheckCharacter(decodeInput, g_specialCharForBit)) {
            flags.set(static_cast<size_t>(BitsetStatusFlag::BIT0));
            return;
        }
        AnalyseIPv4(decodeInput, host, flags);
    }
    bool ISFileNohost(const std::string& input)
    {
        if ((isalpha(input[0]) && (input[1] == ':' || input[1] == '|'))) {
            return true;
        }
        return false;
    }
    void AnalysisFilePath(std::string& input, UrlData& urlinfo,
        std::bitset<static_cast<size_t>(BitsetStatusFlag::BIT_STATUS_11)>& flags)
    {
        std::vector<std::string> temp;
        size_t pos = 0;
        while (((pos = input.find('/')) != std::string::npos) || ((pos = input.find('\\')) != std::string::npos)) {
            temp.push_back(input.substr(0, pos));
            input = input.substr(pos + 1);
        }
        temp.push_back(input);
        size_t length = temp.size();
        for (size_t i = 0; i < length; ++i) {
            auto a = find(g_doubleSegment.begin(), g_doubleSegment.end(), temp[i]);
            if (a != g_doubleSegment.end()) {
                if ((urlinfo.path.size() == 1) && ISFileNohost(urlinfo.path[0]) &&
                    urlinfo.path[0].size() == 2) { // 2:The interception length is 2
                    urlinfo.path[0][1] = ':';
                } else if (!urlinfo.path.empty()) {
                    urlinfo.path.pop_back();
                }
                if (i == temp.size() - 1) {
                    urlinfo.path.push_back("");
                }
                continue;
            }
            a = find(g_singlesegment.begin(), g_singlesegment.end(), temp[i]);
            if (a != g_singlesegment.end()) {
                if (i == temp.size() - 1) {
                    urlinfo.path.push_back("");
                }
                continue;
            }
            urlinfo.path.push_back(temp[i]);
            flags.set(static_cast<size_t>(BitsetStatusFlag::BIT6));
        }
        std::string it = urlinfo.path[0];
        if (isalpha(it[0]) && (it[1] == ':' || it[1] == '|')) {
            if (it.size() == 2) { // 2:The length is 2
                it[1] = ':';
                flags.set(static_cast<size_t>(BitsetStatusFlag::BIT4), 0);
                urlinfo.host.clear();
            }
        }
    }

    void AnalysisSpecialFile(std::string& temp, size_t pos, UrlData& urlinfo,
        std::bitset<static_cast<size_t>(BitsetStatusFlag::BIT_STATUS_11)>& flags)
    {
        std::string strHost = temp.substr(0, pos);
        std::string strPath = temp.substr(pos + 1);
        bool special = true;
        if (!ISFileNohost(strHost)) {
            AnalysisHost(strHost, urlinfo.host, flags, special);
        } else if (!ISFileNohost(strHost) && flags.test(static_cast<size_t>(BitsetStatusFlag::BIT0))) {
            return;
        }
        if (!ISFileNohost(strHost)) {
            AnalysisFilePath(strPath, urlinfo, flags);
        } else {
            AnalysisFilePath(temp, urlinfo, flags);
        }
    }
    void AnalysisFile(std::string& input, UrlData& urlinfo,
        std::bitset<static_cast<size_t>(BitsetStatusFlag::BIT_STATUS_11)>& flags)
    {
        bool special = true;
        if ((input[0] == '/' || input[0] == '\\') && (input[1] == '/' || input[1] == '\\')) {
            std::string temp = input.substr(2); // 2:Intercept from 2 subscripts
            size_t pos = 0;
            if ((((pos = temp.find('/')) != std::string::npos) ||
                ((pos = temp.find('\\')) != std::string::npos)) && pos == 0) {
                temp = temp.substr(1);
                AnalysisFilePath(temp, urlinfo, flags);
            } else if ((((pos = temp.find('/')) != std::string::npos) ||
                ((pos = temp.find('\\')) != std::string::npos)) && pos != 0) {
                AnalysisSpecialFile(temp, pos, urlinfo, flags);
            } else {
                if (!temp.empty() && flags.test(static_cast<size_t>(BitsetStatusFlag::BIT0))) {
                    AnalysisHost(temp, urlinfo.host, flags, special);
                } else if (!temp.empty() && !flags.test(static_cast<size_t>(BitsetStatusFlag::BIT0))) {
                    AnalysisHost(temp, urlinfo.host, flags, special);
                    return;
                }
            }
        } else {
            if (input[0] == '/' || input[0] == '\\') {
                input = input.substr(1);
            }
            AnalysisFilePath(input, urlinfo, flags);
        }
    }

    void AnalysisFilescheme(const std::string& input, UrlData& urlinfo,
        std::bitset<static_cast<size_t>(BitsetStatusFlag::BIT_STATUS_11)>& flags)
    {
        std::string strPath = urlinfo.scheme + input;
        urlinfo.scheme = "file:";
        flags.set(static_cast<size_t>(BitsetStatusFlag::BIT1));
        AnalysisFilePath(strPath, urlinfo, flags);
    }

    void AnalyInfoPath(std::bitset<static_cast<size_t>(BitsetStatusFlag::BIT_STATUS_11)> &flags,
        UrlData& urlinfo, const std::string& input)
    {
        flags.set(static_cast<size_t>(BitsetStatusFlag::BIT9));
        if (urlinfo.path.empty()) {
            urlinfo.path.emplace_back("");
        }
        urlinfo.path[0] = input;
        flags.set(static_cast<size_t>(BitsetStatusFlag::BIT6));
        return;
    }

    void AnalyHostPath(std::string &strHost, std::bitset<static_cast<size_t>(BitsetStatusFlag::BIT_STATUS_11)>& flags,
        UrlData& urlinfo)
    {
        size_t pos = 0;
        if (strHost[strHost.size() - 1] != ']' && (pos = strHost.find_last_of(':')) != std::string::npos) {
            std::string port = strHost.substr(pos + 1);
            strHost = strHost.substr(0, pos);
            AnalysisPort(port, urlinfo, flags);
            if (flags.test(static_cast<size_t>(BitsetStatusFlag::BIT0))) {
                return;
            }
        }
    }
    void AnalyStrHost(std::string &strHost, UrlData& urlinfo,
        std::bitset<static_cast<size_t>(BitsetStatusFlag::BIT_STATUS_11)> &flags)
    {
        if (strHost.find('@') != std::string::npos) {
            AnalysisUsernameAndPasswd(strHost, urlinfo.username, urlinfo.password, flags);
        }
        if (strHost.empty()) {
            flags.set(static_cast<size_t>(BitsetStatusFlag::BIT0));
            return;
        }
    }

    void AnalysisNoDefaultProtocol(std::string& input, UrlData& urlinfo,
        std::bitset<static_cast<size_t>(BitsetStatusFlag::BIT_STATUS_11)>& flags)
    {
        if (urlinfo.scheme.size() == 2) { // 2:The length is 2
            AnalysisFilescheme(input, urlinfo, flags);
            return;
        }
        if (input[0] == '/' && input[1] == '/') {
            std::string hostandpath = input.substr(2); // 2:Intercept from 2 subscripts
            if (hostandpath.empty()) {
                return;
            }
            size_t i = 0;
            bool special = false;
            std::string strHost = "";
            if (hostandpath.find('/') != std::string::npos) {
                i = hostandpath.find('/');
                strHost = hostandpath.substr(0, i);
                std::string strPath = hostandpath.substr(i + 1);
                if (strHost.find('@') != std::string::npos) {
                    AnalysisUsernameAndPasswd(strHost, urlinfo.username, urlinfo.password, flags);
                }
                if (strHost.empty()) {
                    flags.set(static_cast<size_t>(BitsetStatusFlag::BIT0));
                    return;
                }
                size_t pos = 0;
                if (strHost[strHost.size() - 1] != ']' && (pos = strHost.find_last_of(':')) != std::string::npos) {
                    std::string port = strHost.substr(pos + 1);
                    strHost = strHost.substr(0, pos);
                    AnalysisPort(port, urlinfo, flags);
                }
                if (strHost[strHost.size() - 1] != ']' && (pos = strHost.find_last_of(':')) != std::string::npos &&
                    flags.test(static_cast<size_t>(BitsetStatusFlag::BIT0))) {
                    return;
                }
                AnalysisHost(strHost, urlinfo.host, flags, special);
                AnalysisPath(strPath, urlinfo.path, flags, special);
            } else {
                strHost = hostandpath;
                AnalyStrHost(strHost, urlinfo, flags);
                AnalyHostPath(strHost, flags, urlinfo);
                AnalysisHost(strHost, urlinfo.host, flags, special);
            }
        } else if (input[1] == '/') {
            std::string strOfPath = input.substr(1);
            AnalysisPath(strOfPath, urlinfo.path, flags, false);
        } else {
            AnalyInfoPath(flags, urlinfo, input);
        }
    }

    void AnalysisOnlyHost(const std::string& input, UrlData& urlinfo,
        std::bitset<static_cast<size_t>(BitsetStatusFlag::BIT_STATUS_11)>& flags, size_t pos)
    {
        std::string strHost = input;
        if (strHost.find('@') != std::string::npos) {
            AnalysisUsernameAndPasswd(strHost, urlinfo.username, urlinfo.password, flags);
        }
        if (strHost.empty()) {
            flags.set(static_cast<size_t>(BitsetStatusFlag::BIT0));
            return;
        }
        if (strHost[strHost.size() - 1] != ']') {
            if ((pos = strHost.find_last_of(':')) != std::string::npos) {
                std::string port = strHost.substr(pos + 1);
                strHost = strHost.substr(0, pos);
                AnalysisPort(port, urlinfo, flags);
            }
            if ((pos = strHost.find_last_of(':')) != std::string::npos &&
                flags.test(static_cast<size_t>(BitsetStatusFlag::BIT0))) {
                return;
            }
        }
        AnalysisHost(strHost, urlinfo.host, flags, true);
    }
    void JudgePos(size_t &pos, const size_t &length, const std::string& input)
    {
        for (pos = 0; pos < length; pos++) {
            if (input[pos] == '/' || input[pos] == '\\') {
                break;
            }
        }
    }
    void AnalysisHostAndPath(std::string& input, UrlData& urlinfo,
        std::bitset<static_cast<size_t>(BitsetStatusFlag::BIT_STATUS_11)>& flags)
    {
        if (flags.test(static_cast<size_t>(BitsetStatusFlag::BIT1))) {
            size_t pos = 0;
            bool special = true;
            size_t inputLen = input.size();
            while (pos < inputLen) {
                if (input[pos] == '/' || input[pos] == '\\') {
                    pos++;
                    continue;
                }
                    break;
            }
            input = input.substr(pos);
            if (input.size() == 0) {
                flags.set(static_cast<size_t>(BitsetStatusFlag::BIT0));
                return;
            } else if ((input.find('/') != std::string::npos || input.find('\\') != std::string::npos)) {
                size_t length = input.size();
                JudgePos(pos, length, input);
                std::string strHost = input.substr(0, pos);
                std::string strPath = input.substr(pos + 1);
                if (strHost.find('@') != std::string::npos) {
                    AnalysisUsernameAndPasswd(strHost, urlinfo.username, urlinfo.password, flags);
                }
                if (strHost.empty()) {
                    flags.set(static_cast<size_t>(BitsetStatusFlag::BIT0));
                    return;
                }
                if (strHost[strHost.size() - 1] != ']' && (pos = strHost.find_last_of(':')) != std::string::npos) {
                    std::string port = strHost.substr(pos + 1);
                    strHost = strHost.substr(0, pos);
                    AnalysisPort(port, urlinfo, flags);
                }
                if (strHost[strHost.size() - 1] != ']' && (pos = strHost.find_last_of(':')) != std::string::npos &&
                    flags.test(static_cast<size_t>(BitsetStatusFlag::BIT0))) {
                    return;
                }
                AnalysisHost(strHost, urlinfo.host, flags, special);
                AnalysisPath(strPath, urlinfo.path, flags, special);
            } else if (input.size() != 0 && input.find('/') == std::string::npos &&
                input.find('\\') == std::string::npos) {
                AnalysisOnlyHost(input, urlinfo, flags, pos);
            }
        } else {
            size_t inputLen = input.size();
            if (inputLen > 0) {
                urlinfo.isSpecialPath = input[0] != '/' ? true : false;
            }
            AnalysisNoDefaultProtocol(input, urlinfo, flags);
        }
    }

    void AnalysisInput(std::string& input, UrlData& urlData,
        std::bitset<static_cast<size_t>(BitsetStatusFlag::BIT_STATUS_11)>& flags)
    {
        size_t pos = 0;
        if (input.find('#') != std::string::npos) {
            pos = input.find('#');
            std::string fragment = input.substr(pos);
            AnalysisFragment(fragment, urlData.fragment, flags);
            input = input.substr(0, pos);
        }
        if (input.find('?') != std::string::npos) {
            pos = input.find('?');
            std::string query = input.substr(pos);
            AnalysisQuery(query, urlData.query, flags);
            input = input.substr(0, pos);
        }
        bool special = (flags.test(static_cast<size_t>(BitsetStatusFlag::BIT1)) ? true : false);
        AnalysisPath(input, urlData.path, flags, special);
    }

    void BaseInfoToUrl(const UrlData& baseInfo,
        const std::bitset<static_cast<size_t>(BitsetStatusFlag::BIT_STATUS_11)>& baseflags, UrlData& urlData,
        std::bitset<static_cast<size_t>(BitsetStatusFlag::BIT_STATUS_11)>& flags, bool inputIsEmpty)
    {
        urlData.scheme = baseInfo.scheme;
        flags.set(static_cast<size_t>(BitsetStatusFlag::BIT1),
            baseflags.test(static_cast<size_t>(BitsetStatusFlag::BIT1)));
        urlData.host = baseInfo.host;
        flags.set(static_cast<size_t>(BitsetStatusFlag::BIT4));
        urlData.username = baseInfo.username;
        flags.set(static_cast<size_t>(BitsetStatusFlag::BIT2),
            baseflags.test(static_cast<size_t>(BitsetStatusFlag::BIT2)));
        urlData.password = baseInfo.password;
        flags.set(static_cast<size_t>(BitsetStatusFlag::BIT3),
            baseflags.test(static_cast<size_t>(BitsetStatusFlag::BIT3)));
        urlData.port = baseInfo.port;
        flags.set(static_cast<size_t>(BitsetStatusFlag::BIT5),
            baseflags.test(static_cast<size_t>(BitsetStatusFlag::BIT5)));
        if (inputIsEmpty) {
            urlData.path = baseInfo.path;
            flags.set(static_cast<size_t>(BitsetStatusFlag::BIT6),
                baseflags.test(static_cast<size_t>(BitsetStatusFlag::BIT6)));
            urlData.query = baseInfo.query;
            flags.set(static_cast<size_t>(BitsetStatusFlag::BIT7),
                baseflags.test(static_cast<size_t>(BitsetStatusFlag::BIT7)));
            urlData.fragment = baseInfo.fragment;
            flags.set(static_cast<size_t>(BitsetStatusFlag::BIT8),
                baseflags.test(static_cast<size_t>(BitsetStatusFlag::BIT8)));
        }
        flags.set(static_cast<size_t>(BitsetStatusFlag::BIT9),
            baseflags.test(static_cast<size_t>(BitsetStatusFlag::BIT9)));
        flags.set(static_cast<size_t>(BitsetStatusFlag::BIT10),
            baseflags.test(static_cast<size_t>(BitsetStatusFlag::BIT10)));
    }

    void ShorteningPath(UrlData& baseData, bool isFile)
    {
        if (baseData.path.empty()) {
            return;
        }
        if ((baseData.path.size() == 1) && isFile &&
            isalpha(baseData.path[0][0]) && (baseData.path[0][1] == ':')) {
            return;
        }
        baseData.path.pop_back();
    }
    void InitOnlyInput(std::string& input, UrlData& urlData,
        std::bitset<static_cast<size_t>(BitsetStatusFlag::BIT_STATUS_11)>& flags)
    {
        if (input.empty()) {
            flags.set(static_cast<size_t>(BitsetStatusFlag::BIT0));
            return;
        }
        if (input.find(':') != std::string::npos) {
            size_t pos = input.find(':');
            pos++;
            std::string scheme = input.substr(0, pos);
            if (!AnalysisScheme(scheme, urlData.scheme, flags)) {
                return;
            }
            if (input.find('#') != std::string::npos) {
                size_t posTmp = input.find('#');
                std::string fragment = input.substr(posTmp);
                AnalysisFragment(fragment, urlData.fragment, flags);
                input = input.substr(0, posTmp);
            }
            if (input.find('?') != std::string::npos) {
                size_t position = input.find('?');
                std::string query = input.substr(position);
                AnalysisQuery(query, urlData.query, flags);
                input = input.substr(0, position);
            }
            std::string str = input.substr(pos);
            if (urlData.scheme == "file:") {
                AnalysisFile(str, urlData, flags);
            } else {
                AnalysisHostAndPath(str, urlData, flags);
            }
        } else {
            flags.set(static_cast<size_t>(BitsetStatusFlag::BIT0));
            return;
        }
    }
    void ToolHasBase(std::string input, std::string &strInput, UrlData &urlData,
        std::bitset<static_cast<size_t>(BitsetStatusFlag::BIT_STATUS_11)> &flags)
    {
        if (!input.empty() && input[0] == '/') {
            strInput = input.substr(1);
            AnalysisInput(strInput, urlData, flags);
        } else if (!input.empty() && input[0] != '/') {
            AnalysisInput(strInput, urlData, flags);
        }
    }

    URL::URL(const std::string& input)
    {
        std::string str = input;
        PreliminaryWork();
        DeleteC0OrSpace(str);
        DeleteTabOrNewline(str);
        InitOnlyInput(str, urlData_, flags_);
    }

    void DelCont(std::string strBase, std::string &strInput, UrlData &baseInfo,
        std::bitset<static_cast<size_t>(BitsetStatusFlag::BIT_STATUS_11)> &baseflags)
    {
        DeleteC0OrSpace(strBase);
        DeleteTabOrNewline(strBase);
        DeleteC0OrSpace(strInput);
        DeleteTabOrNewline(strInput);
        InitOnlyInput(strBase, baseInfo, baseflags);
    }

    URL::URL(const std::string& input, const std::string& base)
    {
        UrlData baseInfo;
        std::bitset<static_cast<size_t>(BitsetStatusFlag::BIT_STATUS_11)> baseflags;
        std::string strBase = base;
        std::string strInput = input;
        if (strBase.empty()) {
            baseflags.set(static_cast<size_t>(BitsetStatusFlag::BIT0));
        }
        DelCont(strBase, strInput, baseInfo, baseflags);
        if (baseflags.test(static_cast<size_t>(BitsetStatusFlag::BIT0))) {
            flags_.set(static_cast<size_t>(BitsetStatusFlag::BIT0));
            return;
        } else if (!baseflags.test(static_cast<size_t>(BitsetStatusFlag::BIT0))) {
            InitOnlyInput(strInput, urlData_, flags_);
            if (!flags_.test(static_cast<size_t>(BitsetStatusFlag::BIT0))) {
                return;
            }
            if ((input[0] == '/') && (input[1] == '/' || (input[1] == '\\' &&
                baseflags.test(static_cast<size_t>(BitsetStatusFlag::BIT1))))) {
                std::string newInput = baseInfo.scheme + input;
                flags_.set(static_cast<size_t>(BitsetStatusFlag::BIT0), 0);
                InitOnlyInput(newInput, urlData_, flags_);
                return;
            }
            if (!baseflags.test(static_cast<size_t>(BitsetStatusFlag::BIT9))) {
                flags_.set(static_cast<size_t>(BitsetStatusFlag::BIT0), 0);
                BaseInfoToUrl(baseInfo, baseflags, urlData_, flags_, input.empty());
                ToolHasBase(input, strInput, urlData_, flags_);
                if (!input.empty() && input[0] != '/' && urlData_.path.empty()) {
                    urlData_.path = baseInfo.path;
                    flags_.set(static_cast<size_t>(BitsetStatusFlag::BIT6),
                        baseflags.test(static_cast<size_t>(BitsetStatusFlag::BIT6)));
                }
                if (!input.empty() && input[0] != '/' && !urlData_.path.empty()) {
                    bool isFile = ((urlData_.scheme == "file:") ? true : false);
                    ShorteningPath(baseInfo, isFile);
                    baseInfo.path.insert(baseInfo.path.end(), urlData_.path.begin(), urlData_.path.end());
                    urlData_.path = baseInfo.path;
                    flags_.set(static_cast<size_t>(BitsetStatusFlag::BIT6));
                }
            } else if (baseflags.test(static_cast<size_t>(BitsetStatusFlag::BIT9))) {
                flags_.set(static_cast<size_t>(BitsetStatusFlag::BIT0));
                return;
            }
        }
    }

    URL::URL(const std::string& input, const URL& base)
    {
        std::string strInput = input;
        UrlData baseInfo = base.urlData_;
        std::bitset<static_cast<size_t>(BitsetStatusFlag::BIT_STATUS_11)> baseflags = base.flags_;
        DeleteC0OrSpace(strInput);
        DeleteTabOrNewline(strInput);
        InitOnlyInput(strInput, urlData_, flags_);
        if (!flags_.test(static_cast<size_t>(BitsetStatusFlag::BIT0))) {
            return;
        }
        if ((input[0] == '/') && (input[1] == '/' || (input[1] == '\\' &&
            baseflags.test(static_cast<size_t>(BitsetStatusFlag::BIT1))))) {
            std::string newInput = baseInfo.scheme + input;
            flags_.set(static_cast<size_t>(BitsetStatusFlag::BIT0), 0);
            InitOnlyInput(newInput, urlData_, flags_);
            return;
        }
        if (!baseflags.test(static_cast<size_t>(BitsetStatusFlag::BIT9))) {
            flags_.set(static_cast<size_t>(BitsetStatusFlag::BIT0), 0);
            BaseInfoToUrl(baseInfo, baseflags, urlData_, flags_, input.empty());
            if (!input.empty() && input[0] == '/') {
                strInput = input.substr(1);
                AnalysisInput(strInput, urlData_, flags_);
            }
            if (!input.empty() && input[0] != '/') {
                AnalysisInput(strInput, urlData_, flags_);
                if (urlData_.path.empty()) {
                    urlData_.path = baseInfo.path;
                    flags_.set(static_cast<size_t>(BitsetStatusFlag::BIT6),
                        baseflags.test(static_cast<size_t>(BitsetStatusFlag::BIT6)));
                } else {
                    bool isFile = ((urlData_.scheme == "file:") ? true : false);
                    ShorteningPath(baseInfo, isFile);
                    baseInfo.path.insert(baseInfo.path.end(), urlData_.path.begin(), urlData_.path.end());
                    urlData_.path = baseInfo.path;
                    flags_.set(static_cast<size_t>(BitsetStatusFlag::BIT6));
                }
            }
        } else if (baseflags.test(static_cast<size_t>(BitsetStatusFlag::BIT9))) {
            flags_.set(static_cast<size_t>(BitsetStatusFlag::BIT0));
            return;
        }
    }

    napi_value URL::GetHostname(napi_env env) const
    {
        napi_value result;
        std::string temp = "";
        if (flags_.test(static_cast<size_t>(BitsetStatusFlag::BIT4))) {
            temp = urlData_.host;
        }
        NAPI_CALL(env, napi_create_string_utf8(env, temp.c_str(), temp.size(), &result));
        return result;
    }

    napi_value URL::GetSearch(napi_env env) const
    {
        napi_value result;
        std::string temp = "";
        if (flags_.test(static_cast<size_t>(BitsetStatusFlag::BIT7)) && !(urlData_.query.size() == 1)) {
            temp = urlData_.query;
        }
        NAPI_CALL(env, napi_create_string_utf8(env, temp.c_str(), temp.size(), &result));
        return result;
    }

    napi_value URL::GetUsername(napi_env env) const
    {
        napi_value result;
        std::string temp = "";
        if (flags_.test(static_cast<size_t>(BitsetStatusFlag::BIT2))) {
            temp = urlData_.username;
        }
        NAPI_CALL(env, napi_create_string_utf8(env, temp.c_str(), temp.size(), &result));
        return result;
    }

    napi_value URL::GetPassword(napi_env env) const
    {
        napi_value result;
        std::string temp = "";
        if (flags_.test(static_cast<size_t>(BitsetStatusFlag::BIT3))) {
            temp = urlData_.password;
        }
        NAPI_CALL(env, napi_create_string_utf8(env, temp.c_str(), temp.size(), &result));
        return result;
    }

    napi_value URL::GetFragment(napi_env env) const
    {
        napi_value result;
        std::string temp = "";
        if (flags_.test(static_cast<size_t>(BitsetStatusFlag::BIT8)) && !(urlData_.fragment.size() == 1)) {
            temp = urlData_.fragment;
        }
        NAPI_CALL(env, napi_create_string_utf8(env, temp.c_str(), temp.size(), &result));
        return result;
    }

    napi_value URL::GetScheme(napi_env env) const
    {
        napi_value result;
        std::string temp = "";
        if (!urlData_.scheme.empty()) {
            temp = urlData_.scheme;
        }
        NAPI_CALL(env, napi_create_string_utf8(env, temp.c_str(), temp.size(), &result));
        return result;
    }

    napi_value URL::GetPath(napi_env env) const
    {
        napi_value result;
        std::string temp = "/";
        if (urlData_.isSpecialPath) {
            temp = "";
        }
        if (flags_.test(static_cast<size_t>(BitsetStatusFlag::BIT6))) {
            size_t length = urlData_.path.size();
            for (size_t i = 0; i < length; i++) {
                if (i < length - 1) {
                    temp += urlData_.path[i] + "/";
                } else {
                    temp += urlData_.path[i];
                }
            }
        } else {
            bool special = IsSpecial(urlData_.scheme);
            if (!special) {
                temp = "";
            }
        }
        NAPI_CALL(env, napi_create_string_utf8(env, temp.c_str(), temp.size(), &result));
        return result;
    }


    napi_value URL::GetPort(napi_env env) const
    {
        napi_value result;
        std::string temp = "";
        if (flags_.test(static_cast<size_t>(BitsetStatusFlag::BIT5))) {
            temp = std::to_string(urlData_.port);
        }
        NAPI_CALL(env, napi_create_string_utf8(env, temp.c_str(), temp.size(), &result));
        return result;
    }

    napi_value URL::GetHost(napi_env env) const
    {
        napi_value result;
        std::string temp = urlData_.host;
        if (flags_.test(static_cast<size_t>(BitsetStatusFlag::BIT5))) {
            temp += ":";
            temp += std::to_string(urlData_.port);
        }
        NAPI_CALL(env, napi_create_string_utf8(env, temp.c_str(), temp.size(), &result));
        return result;
    }

    napi_value URL::GetOnOrOff(napi_env env) const
    {
        napi_value result;
        if (flags_.test(static_cast<size_t>(BitsetStatusFlag::BIT0))) {
            bool flag = false;
            NAPI_CALL(env, napi_get_boolean(env, flag, &result));
        } else {
            bool flag = true;
            NAPI_CALL(env, napi_get_boolean(env, flag, &result));
        }
        return result;
    }

    napi_value URL::GetIsIpv6(napi_env env) const
    {
        napi_value result;
        if (flags_.test(static_cast<size_t>(BitsetStatusFlag::BIT10))) {
            NAPI_CALL(env, napi_get_boolean(env, true, &result));
        } else {
            NAPI_CALL(env, napi_get_boolean(env, false, &result));
        }
        return result;
    }

    void URL::SetHostname(const std::string& input)
    {
        if (flags_.test(static_cast<size_t>(BitsetStatusFlag::BIT9))) {
            return;
        }
        std::string strHost = input;
        size_t length = strHost.size();
        for (size_t pos = 0; pos < length; pos++) {
            if ((strHost[pos] == ':') || (strHost[pos] == '?') || (strHost[pos] == '#') ||
                (strHost[pos] == '/') || (strHost[pos] == '\\')) {
                strHost = strHost.substr(0, pos);
                break;
            }
        }
        if (strHost.size() == 0) {
            return;
        }
        bool special = IsSpecial(urlData_.scheme);
        std::bitset<static_cast<size_t>(BitsetStatusFlag::BIT_STATUS_11)> thisFlags;
        std::string thisHostname = "";
        AnalysisHost(strHost, thisHostname, thisFlags, special);
        if (thisFlags.test(static_cast<size_t>(BitsetStatusFlag::BIT4))) {
            if ((urlData_.scheme == "file:") && (thisHostname == "localhost")) {
                thisHostname = "";
            }
            urlData_.host = thisHostname;
            flags_.set(static_cast<size_t>(BitsetStatusFlag::BIT4));
        }
    }

    void URL::SetHref(const std::string& input)
    {
        std::string str = input;
        DeleteC0OrSpace(str);
        DeleteTabOrNewline(str);
        UrlData thisNewUrl;
        std::bitset<static_cast<size_t>(BitsetStatusFlag::BIT_STATUS_11)> thisNewFlags;
        InitOnlyInput(str, thisNewUrl, thisNewFlags);
        if (!thisNewFlags.test(static_cast<size_t>(BitsetStatusFlag::BIT0))) {
            urlData_ = thisNewUrl;
            flags_ = thisNewFlags;
        }
    }

    void URL::SetPath(const std::string& input)
    {
        std::string strPath = input;
        if (flags_.test(static_cast<size_t>(BitsetStatusFlag::BIT9)) || strPath.empty()) {
            return;
        }
        std::string oldstr = "%3A";
        std::string newstr = ":";
        ReplaceSpecialSymbols(strPath, oldstr, newstr);
        bool special = IsSpecial(urlData_.scheme);
        if (urlData_.scheme == "file:") {
            UrlData thisFileDate;
            std::bitset<static_cast<size_t>(BitsetStatusFlag::BIT_STATUS_11)> thisFileFlag;
            if ((strPath[0] == '/') || (strPath[0] == '\\' &&
                flags_.test(static_cast<size_t>(BitsetStatusFlag::BIT1)))) {
                strPath = strPath.substr(1);
            }
            AnalysisFilePath(strPath, thisFileDate, thisFileFlag);
            if (thisFileFlag.test(static_cast<size_t>(BitsetStatusFlag::BIT6))) {
                urlData_.path = thisFileDate.path;
                flags_.set(static_cast<size_t>(BitsetStatusFlag::BIT6));
            }
        } else {
            std::vector<std::string> thisPath;
            std::bitset<static_cast<size_t>(BitsetStatusFlag::BIT_STATUS_11)> thisFlags;
            if ((strPath[0] == '/') || (strPath[0] == '\\' &&
                flags_.test(static_cast<size_t>(BitsetStatusFlag::BIT1)))) {
                strPath = strPath.substr(1);
            }
            AnalysisPath(strPath, thisPath, thisFlags, special);
            if (thisFlags.test(static_cast<size_t>(BitsetStatusFlag::BIT6))) {
                urlData_.path = thisPath;
                flags_.set(static_cast<size_t>(BitsetStatusFlag::BIT6));
            }
        }
    }

    void SplitString(const std::string& input, std::string& strHost, std::string& port)
    {
        size_t strlen = input.size();
        for (size_t pos = 0; pos < strlen; pos++) {
            if ((input[pos] == ':') || (input[pos] == '?') || (input[pos] == '#') ||
                (input[pos] == '/') || (input[pos] == '\\')) {
                strHost = input.substr(0, pos);
                if (input[pos] == ':') {
                    pos++;
                    port = input.substr(pos);
                }
                break;
            }
        }
    }

    void URL::SetHost(const std::string& input)
    {
        if (input.empty() || flags_.test(static_cast<size_t>(BitsetStatusFlag::BIT9))) {
            return;
        }
        std::string strHost = input;
        std::string port = "";
        SplitString(input, strHost, port);
        if (strHost.size() == 0) {
            return;
        }
        bool special = IsSpecial(urlData_.scheme);
        std::bitset<static_cast<size_t>(BitsetStatusFlag::BIT_STATUS_11)> hostnameflags;
        std::string thisHostname = "";
        AnalysisHost(strHost, thisHostname, hostnameflags, special);
        if (hostnameflags.test(static_cast<size_t>(BitsetStatusFlag::BIT4))) {
            if ((urlData_.scheme == "file:") && (thisHostname == "localhost")) {
                thisHostname = "";
            }
            urlData_.host = thisHostname;
            flags_.set(static_cast<size_t>(BitsetStatusFlag::BIT4));
        } else {
            return;
        }
        if (port.size() > 0) {
            size_t strlen = port.size();
            for (size_t pos = 0; pos < strlen; pos++) {
                if ((port[pos] == '?') || (port[pos] == '#') || (port[pos] == '/') || (port[pos] == '\\')) {
                    port = port.substr(0, pos);
                    break;
                }
            }
            if (port.size() > 0) {
                std::bitset<static_cast<size_t>(BitsetStatusFlag::BIT_STATUS_11)> thisFlags;
                UrlData thisport;
                AnalysisPort(port, thisport, thisFlags);
                if (thisFlags.test(static_cast<size_t>(BitsetStatusFlag::BIT5))) {
                    flags_.set(static_cast<size_t>(BitsetStatusFlag::BIT5));
                    urlData_.port = thisport.port;
                }
            }
        }
    }

    void URL::SetPort(const std::string& input)
    {
        std::string port = input;
        size_t portlen = port.size();
        for (size_t pos = 0; pos < portlen; pos++) {
            if ((port[pos] == '?') || (port[pos] == '#') || (port[pos] == '/') || (port[pos] == '\\')) {
                port = port.substr(0, pos);
                break;
            }
        }
        if (port.size() > 0) {
            std::bitset<static_cast<size_t>(BitsetStatusFlag::BIT_STATUS_11)> thisFlags;
            UrlData thisport;
            AnalysisPort(port, thisport, thisFlags);
            if (thisFlags.test(static_cast<size_t>(BitsetStatusFlag::BIT5))) {
                flags_.set(static_cast<size_t>(BitsetStatusFlag::BIT5));
                urlData_.port = thisport.port;
            }
        }
    }

    void URL::SetSearch(const std::string& input)
    {
        std::string temp;
        if (input.size() == 0) {
            urlData_.query = "";
            flags_.set(static_cast<size_t>(BitsetStatusFlag::BIT7), 0);
        } else {
            if (input[0] != '?') {
                temp = "?";
                temp += input;
            } else {
                temp = input;
            }
            std::string oldstr = "#";
            std::string newstr = "%23";
            ReplaceSpecialSymbols(temp, oldstr, newstr);
            AnalysisQuery(temp, urlData_.query, flags_);
        }
    }

    void URL::SetFragment(const std::string& input)
    {
        std::string temp;
        if (input.size() == 0) {
            urlData_.fragment = "";
            flags_.set(static_cast<size_t>(BitsetStatusFlag::BIT8), 0);
        } else {
            if (input[0] != '#') {
                temp = "#";
                temp += input;
            } else {
                temp = input;
            }
            AnalysisFragment(temp, urlData_.fragment, flags_);
        }
    }

    void URL::SetScheme(const std::string& input)
    {
        std::string strInput = input;
        bool special = IsSpecial(urlData_.scheme);
        bool inputIsSpecial = IsSpecial(input);
        if ((special != inputIsSpecial) || ((input == "file") &&
            (flags_.test(static_cast<size_t>(BitsetStatusFlag::BIT2)) ||
            flags_.test(static_cast<size_t>(BitsetStatusFlag::BIT3)) ||
            flags_.test(static_cast<size_t>(BitsetStatusFlag::BIT5))))) {
            return;
        }
        std::string thisScheme = "";
        std::bitset<static_cast<size_t>(BitsetStatusFlag::BIT_STATUS_11)> thisFlags;
        if (AnalysisScheme(strInput, thisScheme, thisFlags)) {
            if (thisFlags.test(static_cast<size_t>(BitsetStatusFlag::BIT1))) {
                flags_.set(static_cast<size_t>(BitsetStatusFlag::BIT1));
            }
            urlData_.scheme = thisScheme;
        }
    }

    void URL::SetUsername(const std::string& input)
    {
        if (input.size() == 0) {
            urlData_.username = "";
            flags_.set(static_cast<size_t>(BitsetStatusFlag::BIT2), 0);
        } else {
                std::string usname = input;
            size_t len = g_specialSymbols.size() - 2; // 2:Maximum position of subscript
            for (size_t i = 0; i <= len; i += 2) { // 2:Shift subscript right 2
            ReplaceSpecialSymbols(usname, g_specialSymbols[i], g_specialSymbols[i + 1]);
            }
                urlData_.username = usname;
                flags_.set(static_cast<size_t>(BitsetStatusFlag::BIT2));
        }
    }

    void URL::SetPassword(const std::string& input)
    {
        if (input.size() == 0) {
            urlData_.password = "";
            flags_.set(static_cast<size_t>(BitsetStatusFlag::BIT3), 0);
        } else {
                std::string keyWord = input;
            size_t len = g_specialSymbols.size() - 2; // 2:Maximum position of subscript
            for (size_t i = 0; i <= len; i += 2) { // 2:Shift subscript right 2
            ReplaceSpecialSymbols(keyWord, g_specialSymbols[i], g_specialSymbols[i + 1]);
            }
                urlData_.password = keyWord;
                flags_.set(static_cast<size_t>(BitsetStatusFlag::BIT3));
        }
    }

    void URLSearchParams::HandleIllegalChar(std::wstring& inputStr, std::wstring::const_iterator it)
    {
        std::wstring::iterator iter = inputStr.begin();
        advance(iter, std::distance<std::wstring::const_iterator>(iter, it));
        while (iter != inputStr.end()) {
            char16_t ch = *iter;
            if (!((ch & 0xF800) == 0xD800)) {
                ++iter;
                continue;
            } else if ((ch & 0x400) != 0 || iter == inputStr.end() - 1) {
                *iter = 0xFFFD;
            } else {
                char16_t dh = *(iter + 1);
                if ((dh & 0xFC00) == 0xDC00) {
                    ++iter;
                } else {
                    *iter = 0xFFFD;
                }
            }
            ++iter;
        }
    }
    std::string URLSearchParams::ToUSVString(std::string inputStr)
    {
        std::wstring winput(inputStr.length(), L' ');
        std::copy(inputStr.begin(), inputStr.end(), winput.begin());
        std::wregex wexpr(L"(?:[^\\uD800-\\uDBFF]|^)[\\uDC00-\\uDFFF]|[\\uD800-\\uDBFF](?![\\uDC00-\\uDFFF])");
        std::wsmatch result;
        std::wstring::const_iterator iterStart = winput.begin();
        std::wstring::const_iterator iterEnd = winput.end();
        if (!regex_search(iterStart, iterEnd, result, wexpr)) {
            return inputStr;
        }
        HandleIllegalChar(winput, result[0].first);
        size_t inputLen = wcslen(winput.c_str());
        char *rePtr = nullptr;
        std::string reStr = "";
        size_t reSize = wcstombs(rePtr, winput.c_str(), 0) + 1;
        if (reSize > 0) {
            rePtr = new char[reSize];
            if (memset_s(rePtr, reSize, 0, reSize) != EOK) {
                HILOG_ERROR("ToUSVString memset_s failed");
                delete[] rePtr;
                return reStr;
            } else {
                wcstombs(rePtr, winput.c_str(), inputLen);
                reStr = rePtr;
            }
        }
        delete[] rePtr;
        return reStr;
    }
    napi_value URLSearchParams::Get(napi_env env, napi_value buffer)
    {
        std::string name = "";
        size_t nameSize = 0;
        if (napi_get_value_string_utf8(env, buffer, nullptr, 0, &nameSize) != napi_ok) {
            HILOG_ERROR("can not get buffer size");
            return nullptr;
        }
        name.reserve(nameSize);
        name.resize(nameSize);
        if (napi_get_value_string_utf8(env, buffer, name.data(), nameSize + 1, &nameSize) != napi_ok) {
            HILOG_ERROR("can not get buffer value");
            return nullptr;
        }
        std::string temp = name;
        std::string sname = ToUSVString(temp);
        napi_value result = nullptr;
        if (searchParams.size() == 0) {
            return result;
        }
        size_t size = searchParams.size() - 1;
        for (size_t i = 0; i < size; i += 2) { // 2:Searching for the number and number of keys and values
            if (searchParams[i] == sname) {
                napi_create_string_utf8(env, searchParams[i + 1].c_str(), searchParams[i + 1].length(), &result);
                return result;
            }
        }
        return result;
    }
    napi_value URLSearchParams::GetAll(napi_env env, napi_value buffer)
    {
        std::string name = "";
        size_t nameSize = 0;
        if (napi_get_value_string_utf8(env, buffer, nullptr, 0, &nameSize) != napi_ok) {
            HILOG_ERROR("can not get buffer size");
            return nullptr;
        }
        name.reserve(nameSize);
        name.resize(nameSize);
        if (napi_get_value_string_utf8(env, buffer, name.data(), nameSize + 1, &nameSize) != napi_ok) {
            HILOG_ERROR("can not get buffer value");
            return nullptr;
        }
        std::string sname = ToUSVString(name);

        napi_value result = nullptr;
        napi_value napiStr = nullptr;
        NAPI_CALL(env, napi_create_array(env, &result));
        size_t flag = 0;
        if (searchParams.size() == 0) {
            return result;
        }
        size_t size = searchParams.size() - 1;
        for (size_t i = 0; i < size; i += 2) { // 2:Searching for the number and number of keys and values
            if (searchParams[i] == sname) {
                napi_create_string_utf8(env, searchParams[i + 1].c_str(), searchParams[i + 1].length(), &napiStr);
                NAPI_CALL(env, napi_set_element(env, result, flag, napiStr));
                flag++;
            }
        }
        return result;
    }
    void URLSearchParams::Append(napi_env env, napi_value buffer, napi_value temp)
    {
        std::string name = "";
        size_t nameSize = 0;
        if (napi_get_value_string_utf8(env, buffer, nullptr, 0, &nameSize) != napi_ok) {
            HILOG_ERROR("can not get buffer size");
            return;
        }
        name.reserve(nameSize);
        name.resize(nameSize);
        if (napi_get_value_string_utf8(env, buffer, name.data(), nameSize + 1, &nameSize) != napi_ok) {
            HILOG_ERROR("can not get buffer value");
            return;
        }
        std::string tempName = name;
        std::string value = "";
        size_t valueSize = 0;
        if (napi_get_value_string_utf8(env, temp, nullptr, 0, &valueSize) != napi_ok) {
            HILOG_ERROR("can not get temp size");
            return;
        }
        value.reserve(valueSize);
        value.resize(valueSize);
        if (napi_get_value_string_utf8(env, temp, value.data(), valueSize + 1, &valueSize) != napi_ok) {
            HILOG_ERROR("can not get temp value");
            return;
        }
        std::string tempValue = value;
        searchParams.push_back(tempName);
        searchParams.push_back(tempValue);
    }
    void URLSearchParams::Delete(napi_env env, napi_value buffer)
    {
        std::string name = "";
        size_t nameSize = 0;
        if (napi_get_value_string_utf8(env, buffer, nullptr, 0, &nameSize) != napi_ok) {
            HILOG_ERROR("can not get buffer size");
            return;
        }
        name.reserve(nameSize);
        name.resize(nameSize);
        if (napi_get_value_string_utf8(env, buffer, name.data(), nameSize + 1, &nameSize) != napi_ok) {
            HILOG_ERROR("can not get buffer value");
            return;
        }
        std::string sname = ToUSVString(name);
        for (auto iter = searchParams.begin(); iter != searchParams.end();) {
            if (*iter == sname) {
                iter = searchParams.erase(iter, iter + 2); // 2:Searching for the number and number of keys and values
            } else {
                iter += 2; // 2:Searching for the number and number of keys and values
            }
        }
    }
    napi_value URLSearchParams::Entries(napi_env env) const
    {
        napi_value resend = nullptr;
        napi_value firNapiStr = nullptr;
        napi_value secNapiStr = nullptr;
        napi_create_array(env, &resend);
        if (searchParams.size() == 0) {
            return resend;
        }
        size_t size = searchParams.size() - 1;
        for (size_t i = 0; i < size; i += 2) { // 2:Searching for the number and number of keys and values
            napi_value result = nullptr;
            napi_create_array(env, &result);
            napi_create_string_utf8(env, searchParams[i].c_str(), searchParams[i].length(), &firNapiStr);
            napi_create_string_utf8(env, searchParams[i + 1].c_str(), searchParams[i + 1].length(), &secNapiStr);
            napi_set_element(env, result, 0, firNapiStr);
            napi_set_element(env, result, 1, secNapiStr);
            napi_set_element(env, resend, i / 2, result); // 2:Find the number of keys
        }
        return resend;
    }

    napi_value URLSearchParams::IsHas(napi_env env, napi_value name) const
    {
        size_t bufferSize = 0;
        if (napi_get_value_string_utf8(env, name, nullptr, 0, &bufferSize) != napi_ok) {
            HILOG_ERROR("can not get name size");
            return nullptr;
        }
        std::string buf = "";
        buf.resize(bufferSize);
        if (napi_get_value_string_utf8(env, name, buf.data(), bufferSize + 1, &bufferSize) != napi_ok) {
            HILOG_ERROR("can not get name value");
            return nullptr;
        }
        bool flag = false;
        napi_value result = nullptr;
        size_t lenStr = searchParams.size();
        for (size_t i = 0; i != lenStr; i += 2) { // 2:Searching for the number and number of keys and values
            if (searchParams[i] == buf) {
                flag = true;
                napi_get_boolean(env, flag, &result);
                return result;
            }
        }
        napi_get_boolean(env, flag, &result);
        return result;
    }
    void URLSearchParams::Set(napi_env env, napi_value name, napi_value value)
    {
        std::string buffer = "";
        size_t bufferSize = 0;
        if (napi_get_value_string_utf8(env, name, nullptr, 0, &bufferSize) != napi_ok) {
            HILOG_ERROR("can not get name size");
            return;
        }
        buffer.reserve(bufferSize);
        buffer.resize(bufferSize);
        if (napi_get_value_string_utf8(env, name, buffer.data(), bufferSize + 1, &bufferSize) != napi_ok) {
            HILOG_ERROR("can not get name value");
            return;
        }
        std::string cppName = buffer;
        std::string temp = "";
        size_t tempSize = 0;
        if (napi_get_value_string_utf8(env, value, nullptr, 0, &tempSize) != napi_ok) {
            HILOG_ERROR("can not get value size");
            return;
        }
        temp.reserve(tempSize);
        temp.resize(tempSize);
        if (napi_get_value_string_utf8(env, value, temp.data(), tempSize + 1, &tempSize) != napi_ok) {
            HILOG_ERROR("can not get value value");
            return;
        }
        std::string cppValue = temp;
        bool flag = false;
        for (auto it = searchParams.begin(); it < (searchParams.end() - 1);) {
            if (*it == cppName) {
                if (!flag) {
                    *(it + 1) = cppValue;
                    flag = true;
                    it += 2; // 2:Searching for the number and number of keys and values
                } else {
                    it = searchParams.erase(it, it + 2); // 2:Searching for the number and number of keys and values
                }
            } else {
                it += 2; // 2:Searching for the number and number of keys and values
            }
        }
        if (!flag) {
            searchParams.push_back(cppName);
            searchParams.push_back(cppValue);
        }
    }
    void URLSearchParams::Sort()
    {
        size_t len = searchParams.size();
        if (len <= 2 && (len % 2 != 0)) { // 2: Iterate over key-value pairs
            return;
        }
        size_t i = 0;
        for (; i < len - 2; i += 2) { // 2:Iterate over key-value pairs
            size_t j = i + 2; // 2:Iterate over key-value pairs
            for (; j < len; j += 2) { // 2:Iterate over key-value pairs
                bool tmp = (searchParams[i] > searchParams[j]);
                if (tmp) {
                    const std::string curKey = searchParams[i];
                    const std::string curVal = searchParams[i + 1];
                    searchParams[i] = searchParams[j];
                    searchParams[i + 1] = searchParams[j + 1];
                    searchParams[j] = curKey;
                    searchParams[j + 1] = curVal;
                }
            }
        }
    }
    napi_value URLSearchParams::IterByKeys(napi_env env)
    {
        std::vector<std::string> toKeys;
        napi_value result = nullptr;
        napi_value napiStr = nullptr;
        napi_create_array(env, &result);
        size_t stepSize = 2; // 2:Searching for the number and number of keys and values
        size_t lenStr = searchParams.size();
        if (lenStr % 2 == 0) { // 2:Get the number of values
            for (auto it = searchParams.begin(); it != searchParams.end(); it += stepSize) {
                toKeys.push_back(*it);
            }
            size_t lenToKeys = toKeys.size();
            for (size_t i = 0; i < lenToKeys; i++) {
                napi_create_string_utf8(env, toKeys[i].c_str(), toKeys[i].length(), &napiStr);
                napi_set_element(env, result, i, napiStr);
            }
        }
        return result;
    }
    napi_value URLSearchParams::IterByValues(napi_env env)
    {
        std::vector<std::string> toKeys;
        napi_value result = nullptr;
        napi_value napiStr = nullptr;
        napi_create_array(env, &result);
        size_t stepSize = 2; // 2:Searching for the number and number of keys and values
        size_t lenStr = searchParams.size();
        if (lenStr % 2 == 0) { // 2:Get the number of values
            for (auto it = searchParams.begin();
                it != searchParams.end();
                it += stepSize) {
                toKeys.push_back(*(it + 1));
            }
            size_t lenToKeys = toKeys.size();
            for (size_t i = 0; i < lenToKeys; i++) {
                napi_create_string_utf8(env, toKeys[i].c_str(), toKeys[i].length(), &napiStr);
                napi_set_element(env, result, i, napiStr);
            }
        }
        return result;
    }
    void URLSearchParams::SetArray(napi_env env, const std::vector<std::string> vec)
    {
        searchParams = vec;
    }
    napi_value URLSearchParams::GetArray(napi_env env) const
    {
        napi_value arr = nullptr;
        napi_create_array(env, &arr);
        size_t length = searchParams.size();
        for (size_t i = 0; i < length; i++) {
            napi_value result = nullptr;
            napi_create_string_utf8(env, searchParams[i].c_str(), searchParams[i].size(), &result);
            napi_set_element(env, arr, i, result);
        }
        return arr;
    }
} // namespace OHOS::Url
