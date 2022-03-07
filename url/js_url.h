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

#ifndef COMPILERUNTIME_JS_API_URL_H
#define COMPILERUNTIME_JS_API_URL_H

#include <algorithm>
#include <bitset>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>
#include "napi/native_api.h"
#include "napi/native_node_api.h"
namespace OHOS::Url {
    enum class BitsetStatusFlag {
        BIT0 = 0, // 0:Bit 0 Set to true,The URL analysis failed
        BIT1 = 1, // 1:Bit 1 Set to true,The protocol is the default protocol
        BIT2 = 2, // 2:Bit 2 Set to true,The URL has username
        BIT3 = 3, // 3:Bit 3 Set to true,The URL has password
        BIT4 = 4, // 4:Bit 4 Set to true,The URL has hostname
        BIT5 = 5, // 5:Bit 5 Set to true,The URL Port is the specially
        BIT6 = 6, // 6:Bit 6 Set to true,The URL has pathname
        BIT7 = 7, // 7:Bit 7 Set to true,The URL has query
        BIT8 = 8, // 8:Bit 8 Set to true,The URL has fragment
        BIT9 = 9, // 9:Bit 9 Set to true,The URL Can not be base
        BIT10 = 10, // 10:Bit 10 Set to true,The host is IPV6
        BIT_STATUS_11 = 11 // 11:Each bit of a BIT represents a different parsing state.
    };

    struct UrlData {
        int port = -1;
        std::vector<std::string> path;
        std::string password = "";
        std::string scheme = "";
        std::string query = "";
        std::string username = "";
        std::string fragment = "";
        std::string host = "";
    };

    class URL {
    public:
        URL(napi_env env, const std::string& input);
        URL(napi_env env, const std::string& input, const std::string& base);
        URL(napi_env env, const std::string& input, const URL& base);

        napi_value GetHostname() const;
        void SetHostname(const std::string& input);
        void SetUsername(const std::string& input);
        void SetPassword(const std::string& input);
        void SetScheme(const std::string& input);
        void SetFragment(const std::string& input);
        void SetSearch(const std::string& input);
        void SetHost(const std::string& input);
        void SetPort(const std::string& input);
        void SetHref(const std::string& input);
        void SetPath(const std::string& input);

        napi_value GetSearch() const;
        napi_value GetUsername() const;
        napi_value GetPassword() const;
        napi_value GetFragment() const;
        napi_value GetScheme() const;
        napi_value GetPath() const;
        napi_value GetPort() const;
        napi_value GetOnOrOff() const;
        napi_value GetIsIpv6() const;
        napi_value GetHost() const;

        virtual ~URL() {}
    private:
        UrlData urlData_;
        std::bitset<static_cast<size_t>(BitsetStatusFlag::BIT_STATUS_11)> flags_;
        // bitset<11>:Similar to bool array, each bit status represents the real-time status of current URL parsing
        napi_env env_ = nullptr;
    };

    class URLSearchParams {
    public:
        explicit URLSearchParams(napi_env env);
        virtual ~URLSearchParams() {}
        napi_value IsHas(napi_value  name) const;
        napi_value Get(napi_value buffer);
        napi_value GetAll(napi_value buffer);
        void Append(napi_value buffer, napi_value temp);
        void Delete(napi_value buffer);
        napi_value Entries() const;
        void Set(napi_value name, napi_value value);
        void Sort();
        napi_value ToString();
        napi_value IterByKeys();
        napi_value IterByValues();
        void SetArray(std::vector<std::string> input);
        napi_value GetArray() const;
        std::vector<std::string> StringParmas(std::string Stringpar);
    private:
        std::string ToUSVString(std::string inputStr);
        void HandleIllegalChar(std::wstring& inputStr, std::wstring::const_iterator it);
        std::vector<std::string> searchParams;
        napi_env env;
    };
} // namespace
#endif /* COMPILERUNTIME_JS_API_URL_H */
