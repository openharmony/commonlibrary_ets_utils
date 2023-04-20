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

#ifndef URL_JS_URL_H_
#define URL_JS_URL_H_

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
        BIT_STATUS_11 = 11, // 11:Each bit of a BIT represents a different parsing state.
        BIT_ASCII_32 = 32, // 32:32-bit previously invalid control characters in ascii
        BIT_ASCII_127 = 127, // 127:127 bits in ascii are DEL characters
        MAX_BIT_SIZE = 128 // 128:ascii max range
    };

    struct UrlData {
        int port = -1;
        bool isSpecialPath = false;
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
        /**
         * URI constructor, which is used to instantiate a URI object.
         *
         * @param input Constructs a URI by parsing a given string.
         */
        explicit URL(const std::string& input);

        /**
         * URI constructor, which is used to instantiate a URI object.
         *
         * @param input Constructs a URI by parsing a given string.
         * @param base The input parameter is a character string.
         */
        URL(const std::string& input, const std::string& base);

        /**
         * URI constructor, which is used to instantiate a URI object.
         *
         * @param input Constructs a URI by parsing a given string.
         * @param base The input parameter is the URL object.
         */
        URL(const std::string& input, const URL& base);

        /**
         *
         * @param env NAPI environment parameters.
         * Gets the host name portion of the URL��not include the port.
         */
        napi_value GetHostname(napi_env env) const;

        /**
         * Sets the host name portion of the URL��not include the port.
         *
         * @param input Constructs a URI by parsing a given string.
         */
        void SetHostname(const std::string& input);

        /**
         * Sets the username name portion of the URL��not include the port.
         *
         * @param input Constructs a URI by parsing a given string.
         */
        void SetUsername(const std::string& input);

        /**
         * Sets the password portion of the URL��not include the port.
         *
         * @param input Constructs a URI by parsing a given string.
         */
        void SetPassword(const std::string& input);

        /**
         * Sets the scheme portion of the URL��not include the port.
         *
         * @param input Constructs a URI by parsing a given string.
         */
        void SetScheme(const std::string& input);

        /**
         * Sets the fragment portion of the URL��not include the port.
         *
         * @param input Constructs a URI by parsing a given string.
         */
        void SetFragment(const std::string& input);

        /**
         * Sets the search portion of the URL��not include the port.
         *
         * @param input Constructs a URI by parsing a given string.
         */
        void SetSearch(const std::string& input);

        /**
         * Sets the host portion of the URL��not include the port.
         *
         * @param input Constructs a URI by parsing a given string.
         */
        void SetHost(const std::string& input);

        /**
         * Sets the port portion of the URL��not include the port.
         *
         * @param input Constructs a URI by parsing a given string.
         */
        void SetPort(const std::string& input);

        /**
         * Sets the href portion of the URL��not include the port.
         *
         * @param input Constructs a URI by parsing a given string.
         */
        void SetHref(const std::string& input);

        /**
         * Sets the path portion of the URL��not include the port.
         *
         * @param input Constructs a URI by parsing a given string.
         */
        void SetPath(const std::string& input);

        /**
         * Gets the search portion of the URL��not include the port.
         *
         * @param env NAPI environment parameters.
         */
        napi_value GetSearch(napi_env env) const;

        /**
         * Gets the username portion of the URL��not include the port.
         *
         * @param env NAPI environment parameters.
         */
        napi_value GetUsername(napi_env env) const;

        /**
         * Gets the password portion of the URL��not include the port.
         *
         * @param env NAPI environment parameters.
         */
        napi_value GetPassword(napi_env env) const;

        /**
         * Gets the fragment portion of the URL��not include the port.
         *
         * @param env NAPI environment parameters.
         */
        napi_value GetFragment(napi_env env) const;

        /**
         * Gets the scheme portion of the URL��not include the port.
         *
         * @param env NAPI environment parameters.
         */
        napi_value GetScheme(napi_env env) const;

        /**
         * Gets the path portion of the URL��not include the port.
         *
         * @param env NAPI environment parameters.
         */
        napi_value GetPath(napi_env env) const;

        /**
         * Gets the port portion of the URL��not include the port.
         *
         * @param env NAPI environment parameters.
         */
        napi_value GetPort(napi_env env) const;

        /**
         * Judge whether it's on or off.
         *
         * @param env NAPI environment parameters.
         */
        napi_value GetOnOrOff(napi_env env) const;

        /**
         * Judge whether it's Ipv6.
         *
         * @param env NAPI environment parameters.
         */
        napi_value GetIsIpv6(napi_env env) const;

        /**
         * Gets the host name portion of the URL��not include the port.
         *
         * @param env NAPI environment parameters.
         */
        napi_value GetHost(napi_env env) const;

        /**
         * The destructor of the url
         */
        virtual ~URL() {}

    private:
        UrlData urlData_;
        std::bitset<static_cast<size_t>(BitsetStatusFlag::BIT_STATUS_11)> flags_;
        // bitset<11>:Similar to bool array, each bit status represents the real-time status of current URL parsing
    };

    class URLSearchParams {
    public:
        /**
         * A parameterized constructor used to create an URLSearchParams instance.
         */
        explicit URLSearchParams() {}

        /**
         * Virtual destructor of URLSearchParams
         */
        virtual ~URLSearchParams() {}

        /**
         * Returns a Boolean that indicates whether a parameter with the specified name exists.
         *
         * @param env NAPI environment parameters.
         * @param name Specifies the name of a key-value pair.
         */
        napi_value IsHas(napi_env env, napi_value  name) const;

        /**
         * Returns the first value associated to the given search parameter.
         *
         * @param env NAPI environment parameters.
         * @param buffer Returns the first value associated to the given search parameter.
         */
        napi_value Get(napi_env env, napi_value buffer);

        /**
         * Returns all key-value pairs associated with a given search parameter as an array.
         *
         * @param env NAPI environment parameters.
         * @param buffer Specifies the name of a key value.
         */
        napi_value GetAll(napi_env env, napi_value buffer);

        /**
         * Appends a specified key/value pair as a new search parameter.
         *
         * @param env NAPI environment parameters.
         * @param buffer Key name of the search parameter to be inserted.
         * @param temp Values of search parameters to be inserted.
         */
        void Append(napi_env env, napi_value buffer, napi_value temp);

        /**
         * Deletes the given search parameter and its associated value,from the list of all search parameters.
         *
         * @param env NAPI environment parameters.
         * @param buffer Name of the key-value pair to be deleted.
         */
        void Delete(napi_env env, napi_value buffer);

        /**
         * Returns an ES6 iterator. Each item of the iterator is a JavaScript Array.
         *
         * @param env NAPI environment parameters.
         */
        napi_value Entries(napi_env env) const;

        /**
         * Sets the value associated with a given search parameter to the
         * given value. If there were several matching values, this method
         * deletes the others. If the search parameter doesn't exist, this
         * method creates it.
         *
         * @param env NAPI environment parameters.
         * @param name Key name of the parameter to be set.
         * @param value Indicates the parameter value to be set.
         */
        void Set(napi_env env, napi_value name, napi_value value);

        /**
         * Sort all key/value pairs contained in this object in place and return undefined.
         */
        void Sort();

        /**
         * Returns an iterator allowing to go through all keys contained in this object.
         *
         * @param env NAPI environment parameters.
         */
        napi_value IterByKeys(napi_env env);

        /**
         * Returns an iterator allowing to go through all values contained in this object.
         *
         * @param env NAPI environment parameters.
         */
        napi_value IterByValues(napi_env env);

        /**
         * Sets the string array of searchParams.
         *
         * @param env NAPI environment parameters.
         * @param input String array.
         */
        void SetArray(napi_env env, std::vector<std::string> input);

        /**
         * Gets the string array of searchParams.
         *
         * @param env NAPI environment parameters.
         */
        napi_value GetArray(napi_env env) const;

        /**
         * This function will decode the string and put the parsed key and value values into the
         * vector container of urlsearchparams class according to the rules.
         *
         * @param env NAPI environment parameters.
         * @param Stringpar The input parameter of urlsearchparams is string.
         */
        std::vector<std::string> StringParmas(napi_env env, std::string Stringpar);

    private:
        std::string ToUSVString(std::string inputStr);
        void HandleIllegalChar(std::wstring& inputStr, std::wstring::const_iterator it);
        std::vector<std::string> searchParams;
    };
} // namespace OHOS::Url
#endif // URL_JS_URL_H_
