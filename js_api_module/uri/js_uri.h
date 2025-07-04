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

#ifndef URI_JS_URI_H
#define URI_JS_URI_H

#include <bitset>
#include <cstdlib>
#include <regex>
#include <string>
#include <string_view>
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
        /**
         * URI constructor, which is used to instantiate a URI object.
         *
         * @param input Constructs a URI by parsing a given string.
         */
        explicit Uri(const std::string input);

        /**
         * The destructor of the Uri.
         */
        ~Uri() {}

        /**
         * Tests whether this URI is equivalent to other URI objects.
         *
         * @param other URI object to be compared
         */
        bool Equals(const Uri other) const;

        /**
         * Indicates whether this URI is an absolute URI.
         */
        bool IsAbsolute() const;

        /**
         * Determine whether parsing failed.
         */
        std::string IsFailed() const;

        /**
         * Returns the serialized URI as a string.
         */
        std::string ToString() const;

        /**
         * Indicates whether this URI is an relative URI.
         */
        bool IsRelative() const;

        /**
         * Indicates whether this URI is an opaque URI.
         */
        bool IsOpaque() const;

        /**
         * Indicates whether this URI is an hierarchical URI.
         */
        bool IsHierarchical() const;

        /**
         * Add key and value to Uri's query
         */
        std::string AddQueryValue(const std::string key, const std::string value) const;

        /**
         * Add pathSegment to Uri's segment
         */
        std::string AddSegment(const std::string pathSegment) const;

        /**
         * Gets the LastSegment of the URI.
         */
        std::string GetLastSegment() const;

        /**
         * Normalize the path of this URI.
         */
        std::string Normalize() const;

        /**
         * Gets the protocol part of the URI.
         */
        std::string GetScheme() const;

        /**
         * Gets the decoding permission component part of this URI.
         */
        std::string GetAuthority() const;

        /**
         * Gets the decoding scheme-specific part of the URI.
         */
        std::string GetSsp() const;

        /**
         * Obtains the user information part of the URI.
         */
        std::string GetUserinfo() const;

        /**
         * Gets the hostname portion of the URI without a port.
         */
        std::string GetHost() const;

        /**
         * Gets the hostname portion of the URI without a port.
         */
        std::string GetPort() const;

        /**
         * Gets the path portion of the URI.
         */
        std::string GetPath() const;

        /**
         * Gets the query portion of the URI.
         */
        std::string GetQuery() const;

        /**
         * Gets the fragment part of the URI.
         */
        std::string GetFragment() const;

        /**
         * Clear Uri's query
         */
        std::string ClearQuery() const;

        /**
         * Sets the protocol part of the URI.
         */
        void SetScheme(const std::string_view Scheme);

        /**
         * Sets the user information part of the URI.
         */
        void SetUserInfo(const std::string userInfo);

        /**
         * Sets the path portion of the URI.
         */
        void SetPath(const std::string pathStr);

        /**
         * Sets the query portion of the URI.
         */
        void SetQuery(const std::string queryStr);

        /**
         * Sets the fragment part of the URI.
         */
        void SetFragment(const std::string fragmentStr);

        /**
         * Sets the permission component part of this URI.
         */
        void SetAuthority(const std::string authorityStr);

        /**
         * Sets the decoding scheme-specific part of the URI.
         */
        void SetSsp(const std::string sspStr);
    private:
        void PreliminaryWork() const;
        void AnalysisUri();
        void SpecialPath();
        void AnalysisFragment(size_t pos);
        void AnalysisQuery(size_t pos);
        void AnalysisScheme(size_t pos);
        void AnalysisHostAndPath();
        void AnalysisHost(bool isLawfulProt);
        void AnalysisPath(size_t pos);
        void AnalysisUserInfo(size_t pos);
        void AnalysisIPV6();
        void AssignSchemeSpecificPart();

        bool CheckCharacter(std::string data, std::bitset<MAX_BIT_SIZE> rule, bool flag) const;
        bool AnalysisPort(size_t pos);
        bool AnalysisIPV4();

        std::string UpdateToString() const;
        void UpdateAuthority();
        void UpdateSsp();
        std::string Split(const std::string &path) const;
        std::string BuildUriString(const std::string str, const std::string param) const;

    private:
        UriData uriData_;
        std::string data_ {};
        std::string inputUri_ {};
        std::string errStr_ {};
    };
} // namespace OHOS::Uri
#endif // URI_JS_URI_H
