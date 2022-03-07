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

#ifndef COMPILERUNTIME_JS_API_URI_H
#define COMPILERUNTIME_JS_API_URI_H

#include <bitset>
#include <cstdlib>
#include <regex>
#include <string>
#include <vector>
#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS::Uri {
    constexpr int MAX_BIT_SIZE = 128;
    struct UriData {
        int port = -1;
        std::string scheme = "";
        std::string userInfo = "";
        std::string host = "";
        std::string query = "";
        std::string fragment = "";
        std::string path = "";
        std::string authority = "";
        std::string SchemeSpecificPart = "";
    };

    class Uri {
    public:
        Uri(napi_env env, const std::string input);
        virtual ~Uri() {}

        bool Equals(const Uri other) const;
        bool IsAbsolute() const;

        std::string IsFailed() const;
        std::string ToString() const;
        std::string Normalize() const;
        std::string GetScheme() const;
        std::string GetAuthority() const;
        std::string GetSsp() const;
        std::string GetUserinfo() const;
        std::string GetHost() const;
        std::string GetPort() const;
        std::string GetPath() const;
        std::string GetQuery() const;
        std::string GetFragment() const;
    private:
        void PreliminaryWork() const;
        void AnalysisUri();
        void SpecialPath();
        void AnalysisFragment(size_t pos);
        void AnalysisQuery(size_t pos);
        void AnalysisScheme(size_t pos);
        void AnalysisHostAndPath();
        void AnalysisPath(size_t pos);
        void AnalysisUserInfo(size_t pos);
        void AnalysisIPV6();

        bool CheckCharacter(std::string data, std::bitset<MAX_BIT_SIZE> rule, bool flag) const;
        bool AnalysisPort(size_t pos);
        bool AnalysisIPV4();

        std::string Split(std::string path) const;
    private:
        UriData uriData_;
        std::string data_;
        std::string inputUri_;
        std::string errStr_;
        napi_env env_ = nullptr;
    };
} // namespace
#endif /* COMPILERUNTIME_JS_API_URI_H */