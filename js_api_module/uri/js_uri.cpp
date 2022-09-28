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

#include "js_uri.h"
#include "utils/log.h"
namespace OHOS::Uri {
    std::bitset<MAX_BIT_SIZE> g_ruleAlpha;
    std::bitset<MAX_BIT_SIZE> g_ruleScheme;
    std::bitset<MAX_BIT_SIZE> g_ruleUrlc;
    std::bitset<MAX_BIT_SIZE> g_rulePath;
    std::bitset<MAX_BIT_SIZE> g_ruleUserInfo;
    std::bitset<MAX_BIT_SIZE> g_ruleDigit;
    std::bitset<MAX_BIT_SIZE> g_rulePort;
    void Uri::PreliminaryWork() const
    {
        std::string digitAggregate = "0123456789";
        for (size_t i = 0; i < digitAggregate.size(); ++i) {
            g_ruleDigit.set(digitAggregate[i]);
        }

        std::string alphasAggregate = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        for (size_t i = 0; i < alphasAggregate.size(); ++i) {
            g_ruleAlpha.set(alphasAggregate[i]);
        }

        std::string schemeAggregate = digitAggregate + alphasAggregate + "+-.";
        for (size_t i = 0; i < schemeAggregate.size(); ++i) {
            g_ruleScheme.set(schemeAggregate[i]);
        }

        std::string uricAggregate = schemeAggregate + ";/?:@&=$,[]_!~*'()%";
        for (size_t i = 0; i < uricAggregate.size(); ++i) {
            g_ruleUrlc.set(uricAggregate[i]);
        }

        std::string pathAggregate = schemeAggregate + ";/:@&=$,_!~*'()%";
        for (size_t i = 0; i < pathAggregate.size(); ++i) {
            g_rulePath.set(pathAggregate[i]);
        }

        std::string userInfoAggregate = schemeAggregate + ";:&=$,_!~*'()%";
        for (size_t i = 0; i < userInfoAggregate.size(); ++i) {
            g_ruleUserInfo.set(userInfoAggregate[i]);
        }

        std::string portAggregate = digitAggregate + alphasAggregate + ".:@-;&=+$,-_!~*'()";
        for (size_t i = 0; i < portAggregate.size(); ++i) {
            g_rulePort.set(portAggregate[i]);
        }
    }

    Uri::Uri(const std::string input)
    {
        PreliminaryWork();
        errStr_ = "";
        if (input.empty()) {
            errStr_ = "uri is empty";
            return;
        }
        inputUri_ = input;
        AnalysisUri();
    }

    void Uri::AnalysisUri()
    {
        data_ = inputUri_;
        size_t pos = data_.find('#'); // Fragment
        if (pos != std::string::npos) {
            AnalysisFragment(pos);
            if (!errStr_.empty()) {
                return;
            }
        }
        pos = data_.find('?'); // Query
        if (pos != std::string::npos) {
            AnalysisQuery(pos);
            if (!errStr_.empty()) {
                return;
            }
        }
        pos = data_.find(':'); // Scheme
        if (pos != std::string::npos) {
            AnalysisScheme(pos);
            if (!errStr_.empty()) {
                return;
            }
        } else {
            SpecialPath();
            if (!errStr_.empty()) {
                return;
            }
            uriData_.SchemeSpecificPart = data_ + "?" + uriData_.query;
            return;
        }
        pos = data_.find("//"); // userInfo path host port ipv4 or ipv6
        if (pos != std::string::npos && pos == 0) {
            uriData_.SchemeSpecificPart = data_ + "?" + uriData_.query;
            data_ = data_.substr(2); // 2:Intercept the string from the second subscript
            AnalysisHostAndPath();
            if (!errStr_.empty()) {
                return;
            }
        } else if (data_[0] == '/') {
            uriData_.path = data_;
            uriData_.SchemeSpecificPart = data_ + uriData_.query;
            data_ = "";
        } else if (!data_.empty()) {
            uriData_.SchemeSpecificPart = data_ + uriData_.query;
            uriData_.query = "";
            data_ = "";
        }
    }

    bool Uri::CheckCharacter(std::string data, std::bitset<MAX_BIT_SIZE> rule, bool flag) const
    {
        size_t dataLen = data.size();
        for (size_t i = 0; i < dataLen; ++i) {
            if (static_cast<int>(data[i]) >= 0 && static_cast<int>(data[i]) < 128) { // 128:ASCII Max Number
                bool isLegal = rule.test(data[i]);
                if (!isLegal) {
                    return false;
                }
            } else if (!flag) {
                return false;
            }
        }
        return true;
    }

    void Uri::SpecialPath()
    {
        if (!CheckCharacter(data_, g_rulePath, true)) {
            errStr_ = "SpecialPath does not conform to the rule";
            return;
        }
        uriData_.path = data_;
        data_ = "";
    }

    void Uri::AnalysisFragment(size_t pos)
    {
        if (pos == 0) {
            errStr_ = "#It can't be the first";
            return;
        }
        std::string fragment = data_.substr(pos + 1);
        if (!CheckCharacter(fragment, g_ruleUrlc, true)) {
            errStr_ = "Fragment does not conform to the rule";
            return;
        }
        uriData_.fragment = fragment;
        data_ = data_.substr(0, pos);
    }

    void Uri::AnalysisQuery(size_t pos)
    {
        std::string query = data_.substr(pos + 1);
        if (!CheckCharacter(query, g_ruleUrlc, true)) {
            errStr_ = "Query does not conform to the rule";
            return;
        }
        uriData_.query = query;
        data_ = data_.substr(0, pos);
    }

    void Uri::AnalysisScheme(size_t pos)
    {
        size_t slashPos = data_.find('/');
        if (slashPos != std::string::npos && slashPos < pos) {
            SpecialPath();
            uriData_.SchemeSpecificPart = uriData_.path + "?" + uriData_.query;
            data_ = "";
        } else {
            if (!g_ruleAlpha.test(data_[0])) {
                errStr_ = "Scheme the first character must be a letter";
                return;
            }
            std::string scheme = data_.substr(0, pos);
            if (!CheckCharacter(scheme, g_ruleScheme, false)) {
                errStr_ = "scheme does not conform to the rule";
                return;
            }
            uriData_.scheme = scheme;
            data_ = data_.substr(pos + 1);
        }
    }

    void Uri::AnalysisHostAndPath()
    {
        if (data_.empty()) {
            return;
        }
        // find path
        size_t pos = data_.find('/');
        if (pos != std::string::npos) {
            AnalysisPath(pos);
            if (!errStr_.empty()) {
                return;
            }
        }

        uriData_.authority = data_;

        // find UserInfo
        pos = data_.find('@');
        if (pos != std::string::npos) {
            AnalysisUserInfo(pos);
            if (!errStr_.empty()) {
                return;
            }
        }
        bool isLawfulProt = true;
        // find port
        pos = data_.rfind(':');
        if (pos != std::string::npos) {
            size_t pos1 = data_.rfind(']');
            if (pos1 == std::string::npos || pos > pos1) {
                isLawfulProt = AnalysisPort(pos);
            }
            if (!errStr_.empty()) {
            return;
            }
        }

        // find ipv4 or ipv6 or host
        if (data_[0] == '[') {
            if (data_[data_.size() - 1] == ']') {
                // IPV6
                if (!isLawfulProt) {
                    errStr_ = "Prot does not conform to the rule";
                    return;
                }
                AnalysisIPV6();
            } else {
                errStr_ = "IPv6 is missing a closing bracket";
                return;
            }
        } else {
            // ipv4
            if (!isLawfulProt || !AnalysisIPV4()) {
                uriData_.port = -1;
                uriData_.host = "";
                uriData_.userInfo = "";
            }
        }
    }

    void Uri::AnalysisPath(size_t pos)
    {
        std::string path = data_.substr(pos);
        if (!CheckCharacter(path, g_rulePath, true)) {
            errStr_ = "path does not conform to the rule";
            return;
        }
        uriData_.path = path;
        data_ = data_.substr(0, pos);
    }

    void Uri::AnalysisUserInfo(size_t pos)
    {
        std::string userInfo = data_.substr(0, pos);
        if (!CheckCharacter(userInfo, g_ruleUserInfo, true)) {
            errStr_ = "userInfo does not conform to the rule";
            return;
        }
        uriData_.userInfo = userInfo;
        data_ = data_.substr(pos + 1);
    }

    bool Uri::AnalysisPort(size_t pos)
    {
        std::string port = data_.substr(pos + 1);
        if (!CheckCharacter(port, g_rulePort, true)) {
            errStr_ = "port does not conform to the rule";
            return false;
        } else if (CheckCharacter(port, g_ruleDigit, false)) {
            uriData_.port = std::stoi(port);
            data_ = data_.substr(0, pos);
            return true;
        } else {
            data_ = data_.substr(0, pos);
            return false;
        }
        return false;
    }

    bool Uri::AnalysisIPV4()
    {
        std::regex ipv4("((25[0-5]|2[0-4]\\d|1\\d{2}|[1-9]\\d|\\d)\\.){3}(25[0-5]|2[0-4]\\d|1\\d{2}|[1-9]\\d|\\d)");
        std::regex hostname("(([a-zA-Z0-9]([a-zA-Z0-9\\-]*[a-zA-Z0-9])?\\.)+([a-zA-Z]([a-zA-Z0-9\\-]*[a-zA-Z0-9])?))|"
                            "([a-zA-Z0-9]([a-zA-Z0-9\\-]*[a-zA-Z0-9])?)");
        bool isIpv4 = std::regex_match(data_, ipv4);
        bool isHosName = std::regex_match(data_, hostname);
        if (!isIpv4 && !isHosName) {
            return false;
        } else {
            uriData_.host = data_;
            data_ = "";
            return true;
        }
    }

    void Uri::AnalysisIPV6()
    {
        std::string str = data_.substr(1, data_.size() - 2); // 2:Intercept the string from the second subscript
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
        if (!std::regex_match(str, ipv6)) {
            errStr_ = "ipv6 does not conform to the rule";
            return;
        }
        uriData_.host = data_;
        data_ = "";
    }

    bool Uri::Equals(const Uri other) const
    {
        if (uriData_.port != other.uriData_.port) {
            return false;
        }
        if (uriData_.scheme != other.uriData_.scheme) {
            return false;
        }
        if (uriData_.userInfo != other.uriData_.userInfo) {
            return false;
        }
        if (uriData_.host != other.uriData_.host) {
            return false;
        }
        if (uriData_.query != other.uriData_.query) {
            return false;
        }
        if (uriData_.fragment != other.uriData_.fragment) {
            return false;
        }
        if (uriData_.path != other.uriData_.path) {
            return false;
        }
        if (uriData_.authority != other.uriData_.authority) {
            return false;
        }
        if (uriData_.SchemeSpecificPart != other.uriData_.SchemeSpecificPart) {
            return false;
        }
        return true;
    }

    std::string Uri::ToString() const
    {
        return inputUri_;
    }

    bool Uri::IsAbsolute() const
    {
        return !uriData_.scheme.empty();
    }

    std::string Uri::IsFailed() const
    {
        return errStr_;
    }

    std::string Uri::Normalize() const
    {
        std::vector<std::string> temp;
        size_t pathLen = uriData_.path.size();
        if (pathLen == 0) {
            return this->inputUri_;
        }
        size_t pos = 0;
        size_t left = 0;
        while ((pos = uriData_.path.find('/', left)) != std::string::npos) {
            temp.push_back(uriData_.path.substr(left, pos - left));
            left = pos + 1;
        }
        if (left != pathLen) {
            temp.push_back(uriData_.path.substr(left));
        }
        size_t tempLen = temp.size();
        std::vector<std::string> normalizeTemp;
        for (size_t i = 0; i < tempLen; ++i) {
            if (!temp[i].empty() && !(temp[i] == ".") && !(temp[i] == "..")) {
                normalizeTemp.push_back(temp[i]);
            }
            if (temp[i] == "..") {
                if (!normalizeTemp.empty() && normalizeTemp.back() != "..") {
                    normalizeTemp.pop_back();
                } else {
                    normalizeTemp.push_back(temp[i]);
                }
            }
        }
        std::string normalizePath = "";
        tempLen = normalizeTemp.size();
        if (tempLen == 0) {
            normalizePath = "/";
        } else {
            for (size_t i = 0; i < tempLen; ++i) {
                normalizePath += "/" + normalizeTemp[i];
            }
        }
        return Split(normalizePath);
    }


    std::string Uri::Split(const std::string &path) const
    {
        std::string normalizeUri = "";
        if (!uriData_.scheme.empty()) {
            normalizeUri += uriData_.scheme + ":";
        }
        if (uriData_.path.empty()) {
            normalizeUri += uriData_.SchemeSpecificPart;
        } else {
            if (!uriData_.host.empty()) {
                normalizeUri += "//";
                if (!uriData_.userInfo.empty()) {
                    normalizeUri += uriData_.userInfo + "@";
                }
                normalizeUri += uriData_.host;
                if (uriData_.port != -1) {
                    normalizeUri += ":" + std::to_string(uriData_.port);
                }
            } else if (!uriData_.authority.empty()) {
                normalizeUri += "//" + uriData_.authority;
            }
            normalizeUri += path;
        }
        if (!uriData_.query.empty()) {
            normalizeUri += "?" + uriData_.query;
        }
        if (!uriData_.fragment.empty()) {
            normalizeUri += "#" + uriData_.fragment;
        }
        return normalizeUri;
    }


    std::string Uri::GetScheme() const
    {
        if (uriData_.scheme.empty()) {
            return "null";
        }
        return uriData_.scheme;
    }

    std::string Uri::GetAuthority() const
    {
        if (uriData_.authority.empty()) {
            return "null";
        }
        return uriData_.authority;
    }

    std::string Uri::GetSsp() const
    {
        if (uriData_.SchemeSpecificPart.empty()) {
            return "null";
        }
        return uriData_.SchemeSpecificPart;
    }

    std::string Uri::GetUserinfo() const
    {
        if (uriData_.userInfo.empty()) {
            return "null";
        }
        return uriData_.userInfo;
    }

    std::string Uri::GetHost() const
    {
        if (uriData_.host.empty()) {
            return "null";
        }
        return uriData_.host;
    }

    std::string Uri::GetPort() const
    {
        return std::to_string(uriData_.port);
    }

    std::string Uri::GetPath() const
    {
        if (uriData_.path.empty()) {
            return "null";
        }
        return uriData_.path;
    }

    std::string Uri::GetQuery() const
    {
        if (uriData_.query.empty()) {
            return "null";
        }
        return uriData_.query;
    }

    std::string Uri::GetFragment() const
    {
        if (uriData_.fragment.empty()) {
            return "null";
        }
        return uriData_.fragment;
    }
} // namespace OHOS::Uri
