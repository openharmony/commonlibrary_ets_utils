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

#include "test.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "tools/log.h"
#include "js_uri.h"
#include "native_module_uri.h"

#define ASSERT_CHECK_CALL(call)   \
    {                             \
        ASSERT_EQ(call, napi_ok); \
    }

#define ASSERT_CHECK_VALUE_TYPE(env, value, type)               \
    {                                                           \
        napi_valuetype valueType = napi_undefined;              \
        ASSERT_TRUE(value != nullptr);                          \
        ASSERT_CHECK_CALL(napi_typeof(env, value, &valueType)); \
        ASSERT_EQ(valueType, type);                             \
    }

HWTEST_F(NativeEngineTest, ConstructorTest001, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http://username:password@www.baidu.com:99/path/path?query#fagment");
    ASSERT_STREQ(uri.GetScheme().c_str(), "http");
    ASSERT_STREQ(uri.GetAuthority().c_str(), "username:password@www.baidu.com:99");
    ASSERT_STREQ(uri.GetSsp().c_str(), "//username:password@www.baidu.com:99/path/path?query");
    ASSERT_STREQ(uri.GetUserinfo().c_str(), "username:password");
    ASSERT_STREQ(uri.GetHost().c_str(), "www.baidu.com");
    ASSERT_STREQ(uri.GetPort().c_str(), "99");
    ASSERT_STREQ(uri.GetPath().c_str(), "/path/path");
    ASSERT_STREQ(uri.GetQuery().c_str(), "query");
    ASSERT_STREQ(uri.GetFragment().c_str(), "fagment");
}

HWTEST_F(NativeEngineTest, ConstructorTest002, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http://username:password@[1080::8:800:200C:417A]:99/path/66path1?query#fagment");
    ASSERT_STREQ(uri.GetScheme().c_str(), "http");
    ASSERT_STREQ(uri.GetAuthority().c_str(), "username:password@[1080::8:800:200C:417A]:99");
    ASSERT_STREQ(uri.GetSsp().c_str(), "//username:password@[1080::8:800:200C:417A]:99/path/66path1?query");
    ASSERT_STREQ(uri.GetUserinfo().c_str(), "username:password");
    ASSERT_STREQ(uri.GetHost().c_str(), "[1080::8:800:200C:417A]");
    ASSERT_STREQ(uri.GetPort().c_str(), "99");
    ASSERT_STREQ(uri.GetPath().c_str(), "/path/66path1");
    ASSERT_STREQ(uri.GetQuery().c_str(), "query");
    ASSERT_STREQ(uri.GetFragment().c_str(), "fagment");
}

HWTEST_F(NativeEngineTest, ConstructorTest003, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http://username:password@[::]:88/path/path66?foooo#gaogao");
    ASSERT_STREQ(uri.GetScheme().c_str(), "http");
    ASSERT_STREQ(uri.GetAuthority().c_str(), "username:password@[::]:88");
    ASSERT_STREQ(uri.GetSsp().c_str(), "//username:password@[::]:88/path/path66?foooo");
    ASSERT_STREQ(uri.GetUserinfo().c_str(), "username:password");
    ASSERT_STREQ(uri.GetHost().c_str(), "[::]");
    ASSERT_STREQ(uri.GetPort().c_str(), "88");
    ASSERT_STREQ(uri.GetPath().c_str(), "/path/path66");
    ASSERT_STREQ(uri.GetQuery().c_str(), "foooo");
    ASSERT_STREQ(uri.GetFragment().c_str(), "gaogao");
}

HWTEST_F(NativeEngineTest, ConstructorTest004, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http://username:password@[1:0:0:1:2:1:2:1]:99/path/66path1?query#fagment");
    ASSERT_STREQ(uri.GetScheme().c_str(), "http");
    ASSERT_STREQ(uri.GetAuthority().c_str(), "username:password@[1:0:0:1:2:1:2:1]:99");
    ASSERT_STREQ(uri.GetSsp().c_str(), "//username:password@[1:0:0:1:2:1:2:1]:99/path/66path1?query");
    ASSERT_STREQ(uri.GetUserinfo().c_str(), "username:password");
    ASSERT_STREQ(uri.GetHost().c_str(), "[1:0:0:1:2:1:2:1]");
    ASSERT_STREQ(uri.GetPort().c_str(), "99");
    ASSERT_STREQ(uri.GetPath().c_str(), "/path/66path1");
    ASSERT_STREQ(uri.GetQuery().c_str(), "query");
    ASSERT_STREQ(uri.GetFragment().c_str(), "fagment");
}

HWTEST_F(NativeEngineTest, ConstructorTest005, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http://username:password@[::FFFF:129.144.52.38]:99/path/path?query#fagment");
    ASSERT_STREQ(uri.GetScheme().c_str(), "http");
    ASSERT_STREQ(uri.GetAuthority().c_str(), "username:password@[::FFFF:129.144.52.38]:99");
    ASSERT_STREQ(uri.GetSsp().c_str(), "//username:password@[::FFFF:129.144.52.38]:99/path/path?query");
    ASSERT_STREQ(uri.GetUserinfo().c_str(), "username:password");
    ASSERT_STREQ(uri.GetHost().c_str(), "[::FFFF:129.144.52.38]");
    ASSERT_STREQ(uri.GetPort().c_str(), "99");
    ASSERT_STREQ(uri.GetPath().c_str(), "/path/path");
    ASSERT_STREQ(uri.GetQuery().c_str(), "query");
    ASSERT_STREQ(uri.GetFragment().c_str(), "fagment");
}

HWTEST_F(NativeEngineTest, ConstructorTest006, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http://username:password@[::192.9.5.5]:99/path/path?query#fagment");
    ASSERT_STREQ(uri.GetScheme().c_str(), "http");
    ASSERT_STREQ(uri.GetAuthority().c_str(), "username:password@[::192.9.5.5]:99");
    ASSERT_STREQ(uri.GetSsp().c_str(), "//username:password@[::192.9.5.5]:99/path/path?query");
    ASSERT_STREQ(uri.GetUserinfo().c_str(), "username:password");
    ASSERT_STREQ(uri.GetHost().c_str(), "[::192.9.5.5]");
    ASSERT_STREQ(uri.GetPort().c_str(), "99");
    ASSERT_STREQ(uri.GetPath().c_str(), "/path/path");
    ASSERT_STREQ(uri.GetQuery().c_str(), "query");
    ASSERT_STREQ(uri.GetFragment().c_str(), "fagment");
}

HWTEST_F(NativeEngineTest, ConstructorTest007, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http://username:password@[22::22:2:2%ss]:99/path/path?query#fagment");
    ASSERT_STREQ(uri.GetScheme().c_str(), "http");
    ASSERT_STREQ(uri.GetAuthority().c_str(), "username:password@[22::22:2:2%ss]:99");
    ASSERT_STREQ(uri.GetSsp().c_str(), "//username:password@[22::22:2:2%ss]:99/path/path?query");
    ASSERT_STREQ(uri.GetUserinfo().c_str(), "username:password");
    ASSERT_STREQ(uri.GetHost().c_str(), "[22::22:2:2%ss]");
    ASSERT_STREQ(uri.GetPort().c_str(), "99");
    ASSERT_STREQ(uri.GetPath().c_str(), "/path/path");
    ASSERT_STREQ(uri.GetQuery().c_str(), "query");
    ASSERT_STREQ(uri.GetFragment().c_str(), "fagment");
}

HWTEST_F(NativeEngineTest, ConstructorTest008, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http://username:password@[fe80:0000:0001:0000:0440:44ff:1233:5678]"
                        ":99/path/path?query#fagment");
    ASSERT_STREQ(uri.GetScheme().c_str(), "http");
    ASSERT_STREQ(uri.GetAuthority().c_str(), "username:password@[fe80:0000:0001:0000:0440:44ff:1233:5678]:99");
    ASSERT_STREQ(uri.GetSsp().c_str(), "//username:password@[fe80:0000:0001:0000:0440:44ff:1233:5678]"
                ":99/path/path?query");
    ASSERT_STREQ(uri.GetUserinfo().c_str(), "username:password");
    ASSERT_STREQ(uri.GetHost().c_str(), "[fe80:0000:0001:0000:0440:44ff:1233:5678]");
    ASSERT_STREQ(uri.GetPort().c_str(), "99");
    ASSERT_STREQ(uri.GetPath().c_str(), "/path/path");
    ASSERT_STREQ(uri.GetQuery().c_str(), "query");
    ASSERT_STREQ(uri.GetFragment().c_str(), "fagment");
}

HWTEST_F(NativeEngineTest, ConstructorTest009, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http://username:password@[fe80::0001:0000]:99/path/path?query#fagment");
    ASSERT_STREQ(uri.GetScheme().c_str(), "http");
    ASSERT_STREQ(uri.GetAuthority().c_str(), "username:password@[fe80::0001:0000]:99");
    ASSERT_STREQ(uri.GetSsp().c_str(), "//username:password@[fe80::0001:0000]:99/path/path?query");
    ASSERT_STREQ(uri.GetUserinfo().c_str(), "username:password");
    ASSERT_STREQ(uri.GetHost().c_str(), "[fe80::0001:0000]");
    ASSERT_STREQ(uri.GetPort().c_str(), "99");
    ASSERT_STREQ(uri.GetPath().c_str(), "/path/path");
    ASSERT_STREQ(uri.GetQuery().c_str(), "query");
    ASSERT_STREQ(uri.GetFragment().c_str(), "fagment");
}

HWTEST_F(NativeEngineTest, ConstructorTest010, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http://username:password@www.baidu.com:99/path/path?query#fagment");
    ASSERT_STREQ(uri.GetScheme().c_str(), "http");
    ASSERT_STREQ(uri.GetAuthority().c_str(), "username:password@www.baidu.com:99");
    ASSERT_STREQ(uri.GetSsp().c_str(), "//username:password@www.baidu.com:99/path/path?query");
    ASSERT_STREQ(uri.GetUserinfo().c_str(), "username:password");
    ASSERT_STREQ(uri.GetHost().c_str(), "www.baidu.com");
    ASSERT_STREQ(uri.GetPort().c_str(), "99");
    ASSERT_STREQ(uri.GetPath().c_str(), "/path/path");
    ASSERT_STREQ(uri.GetQuery().c_str(), "query");
    ASSERT_STREQ(uri.GetFragment().c_str(), "fagment");
}

HWTEST_F(NativeEngineTest, ConstructorTest011, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http://username:password@199.98.55.44:99/path/path?query#fagment");
    ASSERT_STREQ(uri.GetScheme().c_str(), "http");
    ASSERT_STREQ(uri.GetAuthority().c_str(), "username:password@199.98.55.44:99");
    ASSERT_STREQ(uri.GetSsp().c_str(), "//username:password@199.98.55.44:99/path/path?query");
    ASSERT_STREQ(uri.GetUserinfo().c_str(), "username:password");
    ASSERT_STREQ(uri.GetHost().c_str(), "199.98.55.44");
    ASSERT_STREQ(uri.GetPort().c_str(), "99");
    ASSERT_STREQ(uri.GetPath().c_str(), "/path/path");
    ASSERT_STREQ(uri.GetQuery().c_str(), "query");
    ASSERT_STREQ(uri.GetFragment().c_str(), "fagment");
}

HWTEST_F(NativeEngineTest, ConstructorTest012, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http://16.9.5.4:99/path/path?query#fagment");
    ASSERT_STREQ(uri.GetScheme().c_str(), "http");
    ASSERT_STREQ(uri.GetAuthority().c_str(), "16.9.5.4:99");
    ASSERT_STREQ(uri.GetSsp().c_str(), "//16.9.5.4:99/path/path?query");
    ASSERT_STREQ(uri.GetUserinfo().c_str(), "");
    ASSERT_STREQ(uri.GetHost().c_str(), "16.9.5.4");
    ASSERT_STREQ(uri.GetPort().c_str(), "99");
    ASSERT_STREQ(uri.GetPath().c_str(), "/path/path");
    ASSERT_STREQ(uri.GetQuery().c_str(), "query");
    ASSERT_STREQ(uri.GetFragment().c_str(), "fagment");
}

HWTEST_F(NativeEngineTest, ConstructorTest013, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http://[::168:169:333]:99/path/path?query#fagment");
    ASSERT_STREQ(uri.GetScheme().c_str(), "http");
    ASSERT_STREQ(uri.GetAuthority().c_str(), "[::168:169:333]:99");
    ASSERT_STREQ(uri.GetSsp().c_str(), "//[::168:169:333]:99/path/path?query");
    ASSERT_STREQ(uri.GetUserinfo().c_str(), "");
    ASSERT_STREQ(uri.GetHost().c_str(), "[::168:169:333]");
    ASSERT_STREQ(uri.GetPort().c_str(), "99");
    ASSERT_STREQ(uri.GetPath().c_str(), "/path/path");
    ASSERT_STREQ(uri.GetQuery().c_str(), "query");
    ASSERT_STREQ(uri.GetFragment().c_str(), "fagment");
}

HWTEST_F(NativeEngineTest, ConstructorTest014, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http://user@49.10hh8.54.12:80/path/path?query#qwer");
    ASSERT_STREQ(uri.GetScheme().c_str(), "http");
    ASSERT_STREQ(uri.GetAuthority().c_str(), "user@49.10hh8.54.12:80");
    ASSERT_STREQ(uri.GetSsp().c_str(), "//user@49.10hh8.54.12:80/path/path?query");
    ASSERT_STREQ(uri.GetUserinfo().c_str(), "");
    ASSERT_STREQ(uri.GetHost().c_str(), "");
    ASSERT_STREQ(uri.GetPort().c_str(), "-1");
    ASSERT_STREQ(uri.GetPath().c_str(), "/path/path");
    ASSERT_STREQ(uri.GetQuery().c_str(), "query");
    ASSERT_STREQ(uri.GetFragment().c_str(), "qwer");
}

HWTEST_F(NativeEngineTest, ConstructorTest015, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http://user@www.baidu.com/path/path?query#qwer");
    ASSERT_STREQ(uri.GetScheme().c_str(), "http");
    ASSERT_STREQ(uri.GetAuthority().c_str(), "user@www.baidu.com");
    ASSERT_STREQ(uri.GetSsp().c_str(), "//user@www.baidu.com/path/path?query");
    ASSERT_STREQ(uri.GetUserinfo().c_str(), "user");
    ASSERT_STREQ(uri.GetHost().c_str(), "www.baidu.com");
    ASSERT_STREQ(uri.GetPort().c_str(), "-1");
    ASSERT_STREQ(uri.GetPath().c_str(), "/path/path");
    ASSERT_STREQ(uri.GetQuery().c_str(), "query");
    ASSERT_STREQ(uri.GetFragment().c_str(), "qwer");
}

HWTEST_F(NativeEngineTest, ConstructorTest016, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("ftp://user@www.1hw.1com:77/path/path?query#qwer");
    ASSERT_STREQ(uri.GetScheme().c_str(), "ftp");
    ASSERT_STREQ(uri.GetAuthority().c_str(), "user@www.1hw.1com:77");
    ASSERT_STREQ(uri.GetSsp().c_str(), "//user@www.1hw.1com:77/path/path?query");
    ASSERT_STREQ(uri.GetUserinfo().c_str(), "");
    ASSERT_STREQ(uri.GetHost().c_str(), "");
    ASSERT_STREQ(uri.GetPort().c_str(), "-1");
    ASSERT_STREQ(uri.GetPath().c_str(), "/path/path");
    ASSERT_STREQ(uri.GetQuery().c_str(), "query");
    ASSERT_STREQ(uri.GetFragment().c_str(), "qwer");
}

HWTEST_F(NativeEngineTest, ConstructorTest017, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http://user@hosthost/path/path?query#qwer");
    ASSERT_STREQ(uri.GetScheme().c_str(), "http");
    ASSERT_STREQ(uri.GetAuthority().c_str(), "user@hosthost");
    ASSERT_STREQ(uri.GetSsp().c_str(), "//user@hosthost/path/path?query");
    ASSERT_STREQ(uri.GetUserinfo().c_str(), "user");
    ASSERT_STREQ(uri.GetHost().c_str(), "hosthost");
    ASSERT_STREQ(uri.GetPort().c_str(), "-1");
    ASSERT_STREQ(uri.GetPath().c_str(), "/path/path");
    ASSERT_STREQ(uri.GetQuery().c_str(), "query");
    ASSERT_STREQ(uri.GetFragment().c_str(), "qwer");
}

HWTEST_F(NativeEngineTest, ConstructorTest018, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http://user@[::]/path/path?query#qwer");
    ASSERT_STREQ(uri.GetScheme().c_str(), "http");
    ASSERT_STREQ(uri.GetAuthority().c_str(), "user@[::]");
    ASSERT_STREQ(uri.GetSsp().c_str(), "//user@[::]/path/path?query");
    ASSERT_STREQ(uri.GetUserinfo().c_str(), "user");
    ASSERT_STREQ(uri.GetHost().c_str(), "[::]");
    ASSERT_STREQ(uri.GetPort().c_str(), "-1");
    ASSERT_STREQ(uri.GetPath().c_str(), "/path/path");
    ASSERT_STREQ(uri.GetQuery().c_str(), "query");
    ASSERT_STREQ(uri.GetFragment().c_str(), "qwer");
}

HWTEST_F(NativeEngineTest, ConstructorTest019, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http://[::192:0:5]/path/path?query#qwer");
    ASSERT_STREQ(uri.GetScheme().c_str(), "http");
    ASSERT_STREQ(uri.GetAuthority().c_str(), "[::192:0:5]");
    ASSERT_STREQ(uri.GetSsp().c_str(), "//[::192:0:5]/path/path?query");
    ASSERT_STREQ(uri.GetUserinfo().c_str(), "");
    ASSERT_STREQ(uri.GetHost().c_str(), "[::192:0:5]");
    ASSERT_STREQ(uri.GetPort().c_str(), "-1");
    ASSERT_STREQ(uri.GetPath().c_str(), "/path/path");
    ASSERT_STREQ(uri.GetQuery().c_str(), "query");
    ASSERT_STREQ(uri.GetFragment().c_str(), "qwer");
}

HWTEST_F(NativeEngineTest, ConstructorTest020, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http:/username:password@www.baidu.com:99/path/path?query#fagment");
    ASSERT_STREQ(uri.GetScheme().c_str(), "http");
    ASSERT_STREQ(uri.GetAuthority().c_str(), "");
    ASSERT_STREQ(uri.GetSsp().c_str(), "/username:password@www.baidu.com:99/path/path?query");
    ASSERT_STREQ(uri.GetUserinfo().c_str(), "");
    ASSERT_STREQ(uri.GetHost().c_str(), "");
    ASSERT_STREQ(uri.GetPort().c_str(), "-1");
    ASSERT_STREQ(uri.GetPath().c_str(), "/username:password@www.baidu.com:99/path/path");
    ASSERT_STREQ(uri.GetQuery().c_str(), "query");
    ASSERT_STREQ(uri.GetFragment().c_str(), "fagment");
}

HWTEST_F(NativeEngineTest, ConstructorTest021, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http:/&username:password@[1080::8:800:200C:417A]:99/path/66path1?query#fagment");
    ASSERT_STREQ(uri.GetScheme().c_str(), "http");
    ASSERT_STREQ(uri.GetAuthority().c_str(), "");
    ASSERT_STREQ(uri.GetSsp().c_str(), "/&username:password@[1080::8:800:200C:417A]:99/path/66path1?query");
    ASSERT_STREQ(uri.GetUserinfo().c_str(), "");
    ASSERT_STREQ(uri.GetHost().c_str(), "");
    ASSERT_STREQ(uri.GetPort().c_str(), "-1");
    ASSERT_STREQ(uri.GetPath().c_str(), "/&username:password@[1080::8:800:200C:417A]:99/path/66path1");
    ASSERT_STREQ(uri.GetQuery().c_str(), "query");
    ASSERT_STREQ(uri.GetFragment().c_str(), "fagment");
}

HWTEST_F(NativeEngineTest, ConstructorTest022, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http:/[username:password@[::FFFF:129.144.52.38]:99/path/path?query#fagment");
    ASSERT_STREQ(uri.GetScheme().c_str(), "http");
    ASSERT_STREQ(uri.GetAuthority().c_str(), "");
    ASSERT_STREQ(uri.GetSsp().c_str(), "/[username:password@[::FFFF:129.144.52.38]:99/path/path?query");
    ASSERT_STREQ(uri.GetUserinfo().c_str(), "");
    ASSERT_STREQ(uri.GetHost().c_str(), "");
    ASSERT_STREQ(uri.GetPort().c_str(), "-1");
    ASSERT_STREQ(uri.GetPath().c_str(), "/[username:password@[::FFFF:129.144.52.38]:99/path/path");
    ASSERT_STREQ(uri.GetQuery().c_str(), "query");
    ASSERT_STREQ(uri.GetFragment().c_str(), "fagment");
}

HWTEST_F(NativeEngineTest, ConstructorTest023, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http:username:password@[1080::8:800:200C:417A]:99/path/66path1?query#fagment");
    ASSERT_STREQ(uri.GetScheme().c_str(), "http");
    ASSERT_STREQ(uri.GetAuthority().c_str(), "");
    ASSERT_STREQ(uri.GetSsp().c_str(), "username:password@[1080::8:800:200C:417A]:99/path/66path1?query");
    ASSERT_STREQ(uri.GetUserinfo().c_str(), "");
    ASSERT_STREQ(uri.GetHost().c_str(), "");
    ASSERT_STREQ(uri.GetPort().c_str(), "-1");
    ASSERT_STREQ(uri.GetPath().c_str(), "");
    ASSERT_STREQ(uri.GetQuery().c_str(), "");
    ASSERT_STREQ(uri.GetFragment().c_str(), "fagment");
}

HWTEST_F(NativeEngineTest, ConstructorTest024, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http:^$username:password@[::192.9.5.5]:99/path/path?query#fagment");
    ASSERT_STREQ(uri.GetScheme().c_str(), "http");
    ASSERT_STREQ(uri.GetAuthority().c_str(), "");
    ASSERT_STREQ(uri.GetSsp().c_str(), "^$username:password@[::192.9.5.5]:99/path/path?query");
    ASSERT_STREQ(uri.GetUserinfo().c_str(), "");
    ASSERT_STREQ(uri.GetHost().c_str(), "");
    ASSERT_STREQ(uri.GetPort().c_str(), "-1");
    ASSERT_STREQ(uri.GetPath().c_str(), "");
    ASSERT_STREQ(uri.GetQuery().c_str(), "");
    ASSERT_STREQ(uri.GetFragment().c_str(), "fagment");
}

HWTEST_F(NativeEngineTest, ConstructorTest025, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http:[?]username:password@[fe80::0001:0000]:99/path/path?query#fagment");
    ASSERT_STREQ(uri.GetScheme().c_str(), "http");
    ASSERT_STREQ(uri.GetAuthority().c_str(), "");
    ASSERT_STREQ(uri.GetSsp().c_str(), "[?]username:password@[fe80::0001:0000]:99/path/path?query");
    ASSERT_STREQ(uri.GetUserinfo().c_str(), "");
    ASSERT_STREQ(uri.GetHost().c_str(), "");
    ASSERT_STREQ(uri.GetPort().c_str(), "-1");
    ASSERT_STREQ(uri.GetPath().c_str(), "");
    ASSERT_STREQ(uri.GetQuery().c_str(), "");
    ASSERT_STREQ(uri.GetFragment().c_str(), "fagment");
}

HWTEST_F(NativeEngineTest, ConstructorTest026, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("");
    ASSERT_STREQ(uri.GetScheme().c_str(), "");
    ASSERT_STREQ(uri.GetSsp().c_str(), "");
    ASSERT_STREQ(uri.GetFragment().c_str(), "");
    ASSERT_STREQ(uri.IsFailed().c_str(), "uri is empty");
}

HWTEST_F(NativeEngineTest, ConstructorTest027, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("#asd;");
    ASSERT_STREQ(uri.IsFailed().c_str(), "#It can't be the first");
}

HWTEST_F(NativeEngineTest, ConstructorTest028, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("?sa^d:s#asd;");
    ASSERT_STREQ(uri.IsFailed().c_str(), "Query does not conform to the rule");
}

HWTEST_F(NativeEngineTest, ConstructorTest029, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("?sad:s#a^sd;");
    ASSERT_STREQ(uri.IsFailed().c_str(), "Fragment does not conform to the rule");
}

HWTEST_F(NativeEngineTest, ConstructorTest030, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("4http:/username:password@www.baidu.com:99/path/path?query#fagment");
    ASSERT_STREQ(uri.IsFailed().c_str(), "Scheme the first character must be a letter");
}

HWTEST_F(NativeEngineTest, ConstructorTest031, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("ht*tp:/username:password@www.baidu.com:99/path/path?query#fagment");
    ASSERT_STREQ(uri.IsFailed().c_str(), "scheme does not conform to the rule");
}

HWTEST_F(NativeEngineTest, ConstructorTest032, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("/usern]ame/path/path?query#fagment");
    ASSERT_STREQ(uri.IsFailed().c_str(), "SpecialPath does not conform to the rule");
}

HWTEST_F(NativeEngineTest, ConstructorTest033, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("/username/path/path?query#fagment");
    ASSERT_STREQ(uri.IsFailed().c_str(), "");
}

HWTEST_F(NativeEngineTest, ConstructorTest034, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http:/userna^me:password@www.baidu.com:99/path/path?query#fagment");
    ASSERT_STREQ(uri.IsFailed().c_str(), "");
}

HWTEST_F(NativeEngineTest, ConstructorTest035, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http://?query#fagment");
    ASSERT_STREQ(uri.GetScheme().c_str(), "http");
}

HWTEST_F(NativeEngineTest, ConstructorTest036, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http:/username:password@www.baidu.com:99/pa^th/path?query#fagment");
    ASSERT_STREQ(uri.IsFailed().c_str(), "");
}

HWTEST_F(NativeEngineTest, ConstructorTest037, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http:/username:password@www.baidu.com:9^9/path/path?query#fagment");
    ASSERT_STREQ(uri.IsFailed().c_str(), "");
}

HWTEST_F(NativeEngineTest, ConstructorTest038, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http:/username:password@[1:0:0:1:2:1:2:1]:9^9/path/path?query#fagment");
    ASSERT_STREQ(uri.IsFailed().c_str(), "");
}

HWTEST_F(NativeEngineTest, ConstructorTest039, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http:/username:password@[1:0:0:1:2:1:2:1/path/path?query#fagment");
    ASSERT_STREQ(uri.IsFailed().c_str(), "");
}

HWTEST_F(NativeEngineTest, ConstructorTest040, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http://gg:gaogao@[1:0:0:sfvs1:2:1:2:1]:99/path1?query#fagment");
    ASSERT_STREQ(uri.IsFailed().c_str(), "ipv6 does not conform to the rule");
}

HWTEST_F(NativeEngineTest, EqualsTest001, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http://gg:gaogao@[1:0:0:1:2:1:2:1]:99/path1?query#fagment");
    OHOS::Uri::Uri uri1 = uri;
    ASSERT_TRUE(uri.Equals(uri1));
}

HWTEST_F(NativeEngineTest, EqualsTest002, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http://gg:gaogao@[1:0:0:1:2:1:2:1]:99/path1?query#fagment");
    OHOS::Uri::Uri uri1("http://gg:gaogao@[1:0:0:1:2:1:2:1]:99/path1?query#fagment");
    ASSERT_TRUE(uri.Equals(uri1));
}

HWTEST_F(NativeEngineTest, EqualsTest003, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http://gg:gaogao@[1:0:0:1:2:1:2:1]:99/path1?query#fagment");
    OHOS::Uri::Uri uri1("http://gg:gaogao@[1:0:0:1:2:1:2:1]:99/path1?query#fagment123");
    ASSERT_FALSE(uri.Equals(uri1));
}

HWTEST_F(NativeEngineTest, EqualsTest004, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http://username:password@www.baidu.com:99/path/path?query#fagment");
    OHOS::Uri::Uri uri1("http://username:password@www.baidu.com:29/path/path?query#fagment");
    ASSERT_FALSE(uri.Equals(uri1));
}

HWTEST_F(NativeEngineTest, EqualsTest005, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http://username:password@www.baidu.com:99/path/path?query#fagment");
    OHOS::Uri::Uri uri1("http://user1name:password@www.baidu.com:99/path/path?query#fagment");
    ASSERT_FALSE(uri.Equals(uri1));
}

HWTEST_F(NativeEngineTest, EqualsTest006, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http://username:password@www.baidu.com:99/path/path?query#fagment");
    OHOS::Uri::Uri uri1("http://username:password@w2ww.baidu.com:99/path/path?query#fagment");
    ASSERT_FALSE(uri.Equals(uri1));
}

HWTEST_F(NativeEngineTest, EqualsTest007, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http://username:password@www.baidu.com:99/path/path?query#fagment");
    OHOS::Uri::Uri uri1("http://username:password@www.baidu.com:99/pa4th/path?query#fagment");
    ASSERT_FALSE(uri.Equals(uri1));
}

HWTEST_F(NativeEngineTest, EqualsTest008, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http://username:password@www.baidu.com:99/path/path?qu4ery#fagment");
    OHOS::Uri::Uri uri1("http://username:password@www.baidu.com:99/path/path?query#fagment");
    ASSERT_FALSE(uri.Equals(uri1));
}

HWTEST_F(NativeEngineTest, EqualsTest009, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http://username:password@www.baidu.com:99/path/path?query#fagment");
    OHOS::Uri::Uri uri1("h4ttp://username:password@www.baidu.com:99/path/path?query#fagment");
    ASSERT_FALSE(uri.Equals(uri1));
}

HWTEST_F(NativeEngineTest, NormalizeTest001, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http://user@[1:0:0:1:2:1:2:1]:99/path/66./../././mm/.././path1?query#fagment");
    std::string normalize = uri.Normalize();
    ASSERT_STREQ(normalize.c_str(), "http://user@[1:0:0:1:2:1:2:1]:99/path/path1?query#fagment");
}

HWTEST_F(NativeEngineTest, NormalizeTest002, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http://gg:gaogao@[1:0:0:1:2:1:2:1]:99/../../path?query#fagment");
    std::string normalize = uri.Normalize();
    ASSERT_STREQ(normalize.c_str(), "http://gg:gaogao@[1:0:0:1:2:1:2:1]:99/../../path?query#fagment");
}

HWTEST_F(NativeEngineTest, NormalizeTest003, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http://gg:gaogao@[1:0:0:1:2:1:2:1]:99/../../path/.././../aa/bb/cc?query#fagment");
    std::string normalize = uri.Normalize();
    ASSERT_STREQ(normalize.c_str(), "http://gg:gaogao@[1:0:0:1:2:1:2:1]:99/../../../aa/bb/cc?query#fagment");
}

HWTEST_F(NativeEngineTest, NormalizeTest004, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http://gg:gaogao@[1:0:0:1:2:1:2:1]:99?query#fagment");
    std::string normalize = uri.Normalize();
    ASSERT_STREQ(normalize.c_str(), "http://gg:gaogao@[1:0:0:1:2:1:2:1]:99?query#fagment");
}

HWTEST_F(NativeEngineTest, ToStringTest001, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http://gg:gaogao@[1:0:0:1:2:1:2:1]:99/aa/bb/cc?query#fagment");
    ASSERT_STREQ(uri.ToString().c_str(), "http://gg:gaogao@[1:0:0:1:2:1:2:1]:99/aa/bb/cc?query#fagment");
}

HWTEST_F(NativeEngineTest, ToStringTest002, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("htt1p://gg:gaogao@[::192.9.5.5]:99/path/66path1?query#fagment");
    ASSERT_STREQ(uri.ToString().c_str(), "htt1p://gg:gaogao@[::192.9.5.5]:99/path/66path1?query#fagment");
}

HWTEST_F(NativeEngineTest, ToStringTest003, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("ftp://username:www.baidu.com/path?query#fagment");
    ASSERT_STREQ(uri.ToString().c_str(), "ftp://username:www.baidu.com/path?query#fagment");
}

HWTEST_F(NativeEngineTest, IsAbsoluteTest001, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("f/tp://username:password@www.baidu.com:88/path?query#fagment");
    bool res = uri.IsAbsolute();
    ASSERT_FALSE(res);
}

HWTEST_F(NativeEngineTest, IsAbsoluteTest002, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("ftp://username:password@www.baidu.com:88/path?query#fagment");
    bool res = uri.IsAbsolute();
    ASSERT_TRUE(res);
}

HWTEST_F(NativeEngineTest, IsAbsoluteTest003, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("htt/p://username:password@www.baidu.com:88/path?query#fagment");
    bool res = uri.IsAbsolute();
    ASSERT_FALSE(res);
}

HWTEST_F(NativeEngineTest, IsRelativeTest001, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("https://www.example.com/aaa");
    bool res = uri.IsRelative();
    ASSERT_FALSE(res);
}

HWTEST_F(NativeEngineTest, IsRelativeTest002, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("/bbb");
    bool res = uri.IsRelative();
    ASSERT_TRUE(res);
}

HWTEST_F(NativeEngineTest, IsOpaqueTest001, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("aaa:user@example.com");
    bool res = uri.IsOpaque();
    ASSERT_TRUE(res);
}

HWTEST_F(NativeEngineTest, IsOpaqueTest002, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("content://com.example/bbb");
    bool res = uri.IsOpaque();
    ASSERT_FALSE(res);
}

HWTEST_F(NativeEngineTest, IsHierarchicalTest001, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("https://www.example.com/path/to/resource");
    bool res = uri.IsHierarchical();
    ASSERT_TRUE(res);
}

HWTEST_F(NativeEngineTest, IsHierarchicalTest002, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("/path/to/resource");
    bool res = uri.IsHierarchical();
    ASSERT_TRUE(res);
}

HWTEST_F(NativeEngineTest, IsHierarchicalTest003, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("tel:123456789");
    bool res = uri.IsHierarchical();
    ASSERT_FALSE(res);
}

HWTEST_F(NativeEngineTest, AddQueryValueTest001, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("https://username:password@host:8080/file?aaa=1#myfragment");
    std::string temp = uri.AddQueryValue("bbb", "2");
    ASSERT_STREQ(temp.c_str(), "https://username:password@host:8080/file?aaa=1&bbb=2#myfragment");
}

HWTEST_F(NativeEngineTest, AddQueryValueTest002, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("mao:user@example.com");
    std::string temp = uri.AddQueryValue("bb", "cc");
    ASSERT_STREQ(temp.c_str(), "mao:?bb=cc");
}

HWTEST_F(NativeEngineTest, ClearQueryTest001, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("https://username:password@host:8080/file?aaa=1#myfragment");
    std::string temp = uri.ClearQuery();
    ASSERT_STREQ(temp.c_str(), "https://username:password@host:8080/file#myfragment");
}

HWTEST_F(NativeEngineTest, GetSegmentTest001, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("https://username:password@host:8080/file?aaa=1#myfragment");
    std::vector<std::string> temp = uri.GetSegment();
    ASSERT_EQ(temp.size(), 1);
}

HWTEST_F(NativeEngineTest, AddSegmentTest001, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("https://username:password@host:8080/file?aaa=1#myfragment");
    std::string temp = uri.AddSegment("segment");
    ASSERT_STREQ(temp.c_str(), "https://username:password@host:8080/file/segment?aaa=1#myfragment");
}

HWTEST_F(NativeEngineTest, AddSegmentTest002, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("mao:user@example.com");
    std::string temp = uri.AddSegment("aaa");
    ASSERT_STREQ(temp.c_str(), "mao:/aaa");
}

HWTEST_F(NativeEngineTest, GetTest001, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("https://username:password@host:8080");
    ASSERT_STREQ(uri.GetPath().c_str(), "");
    ASSERT_STREQ(uri.GetQuery().c_str(), "");
    ASSERT_STREQ(uri.GetFragment().c_str(), "");
}

std::string GetStringUtf8(napi_env env, napi_value str)
{
    std::string buffer = "";
    size_t bufferSize = 0;
    if (napi_get_value_string_utf8(env, str, nullptr, 0, &bufferSize) != napi_ok) {
        HILOG_ERROR("can not get src size");
        return buffer;
    }
    buffer.resize(bufferSize);
    if (napi_get_value_string_utf8(env, str, buffer.data(), bufferSize + 1, &bufferSize) != napi_ok) {
        HILOG_ERROR("can not get src value");
        return buffer;
    }
    return buffer;
}

napi_value StrToNapiValue(napi_env env, const std::string &result)
{
    napi_value output = nullptr;
    napi_create_string_utf8(env, result.c_str(), result.size(), &output);
    return output;
}

std::vector<std::string> GetArray(napi_env env, const napi_value tempStr)
{
    std::vector<std::string> strVec;
    napi_value napiStr = nullptr;
    uint32_t length = 0;
    size_t strLength = 0;
    napi_get_array_length(env, tempStr, &length);
    for (size_t i = 0; i < length; i++) {
        napi_get_element(env, tempStr, i, &napiStr);
        if (napi_get_value_string_utf8(env, napiStr, nullptr, 0, &strLength) != napi_ok) {
            HILOG_ERROR("can not get napiStr size");
            return strVec;
        }
        if (strLength > 0) {
            std::string itemStr = "";
            itemStr.resize(strLength);
            if (napi_get_value_string_utf8(env, napiStr, itemStr.data(), strLength + 1, &strLength) != napi_ok) {
                HILOG_ERROR("can not get napiStr size");
                return strVec;
            }
            strVec.push_back(itemStr);
        } else {
            strVec.push_back("");
        }
    }
    return strVec;
}

HWTEST_F(NativeEngineTest, ModuleTest001, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value exports = nullptr;
    napi_create_object(env, &exports);
    OHOS::Uri::UriInit(env, exports);
    napi_value uriClass = nullptr;
    napi_value constructorArgs[1] =  { 0 };
    std::string input = "http://username:password@www.baidu.com:99/path/path?query#fagment";
    napi_create_string_utf8(env, input.c_str(), input.size(), &constructorArgs[0]);
    napi_status status = napi_get_named_property(env, exports, "Uri", &uriClass);
    napi_value instance = nullptr;
    status = napi_new_instance(env, uriClass, 1, constructorArgs, &instance);
    napi_value getTemp = nullptr;
    napi_get_named_property(env, instance, "scheme", &getTemp);
    std::string res = GetStringUtf8(env, getTemp);
    ASSERT_STREQ(res.c_str(), "http");

    napi_get_named_property(env, instance, "authority", &getTemp);
    res = GetStringUtf8(env, getTemp);
    ASSERT_STREQ(res.c_str(), "username:password@www.baidu.com:99");

    napi_get_named_property(env, instance, "ssp", &getTemp);
    res = GetStringUtf8(env, getTemp);
    ASSERT_STREQ(res.c_str(), "//username:password@www.baidu.com:99/path/path?query");

    napi_get_named_property(env, instance, "userInfo", &getTemp);
    res = GetStringUtf8(env, getTemp);
    ASSERT_STREQ(res.c_str(), "username:password");

    napi_get_named_property(env, instance, "host", &getTemp);
    res = GetStringUtf8(env, getTemp);
    ASSERT_STREQ(res.c_str(), "www.baidu.com");

    napi_get_named_property(env, instance, "port", &getTemp);
    res = GetStringUtf8(env, getTemp);
    ASSERT_STREQ(res.c_str(), "99");

    napi_get_named_property(env, instance, "path", &getTemp);
    res = GetStringUtf8(env, getTemp);
    ASSERT_STREQ(res.c_str(), "/path/path");

    napi_get_named_property(env, instance, "query", &getTemp);
    res = GetStringUtf8(env, getTemp);
    ASSERT_STREQ(res.c_str(), "query");

    napi_get_named_property(env, instance, "fragment", &getTemp);
    res = GetStringUtf8(env, getTemp);
    ASSERT_STREQ(res.c_str(), "fagment");

    napi_get_named_property(env, instance, "isFailed", &getTemp);
    res = GetStringUtf8(env, getTemp);
    ASSERT_STREQ(res.c_str(), "");
}

HWTEST_F(NativeEngineTest, ModuleTest002, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value exports = nullptr;
    napi_create_object(env, &exports);
    OHOS::Uri::UriInit(env, exports);
    napi_value uriClass = nullptr;
    napi_value constructorArgs[1] =  { 0 };
    std::string input = "http://username:password@www.baidu.com:99/path/path?query#fagment";
    napi_create_string_utf8(env, input.c_str(), input.size(), &constructorArgs[0]);
    napi_get_named_property(env, exports, "Uri", &uriClass);
    napi_value instance = nullptr;
    napi_new_instance(env, uriClass, 1, constructorArgs, &instance);
    napi_value tempFn = nullptr;
    napi_get_named_property(env, instance, "checkIsAbsolute", &tempFn);
    napi_value result = nullptr;
    napi_call_function(env, instance, tempFn, 0, nullptr, &result);
    ASSERT_TRUE(result);

    napi_get_named_property(env, instance, "toString", &tempFn);
    napi_call_function(env, instance, tempFn, 0, nullptr, &result);
    std::string res = GetStringUtf8(env, result);
    ASSERT_STREQ(res.c_str(), "http://username:password@www.baidu.com:99/path/path?query#fagment");

    napi_get_named_property(env, instance, "normalize", &tempFn);
    napi_call_function(env, instance, tempFn, 0, nullptr, &result);
    res = GetStringUtf8(env, result);
    ASSERT_STREQ(res.c_str(), "http://username:password@www.baidu.com:99/path/path?query#fagment");

    napi_value equalsFn = nullptr;
    napi_get_named_property(env, instance, "equals", &equalsFn);
    napi_value constructorArgs1[1] = { 0 };
    std::string input2 = "http://username:password@www.baidu.com:88/path?query1#fagment";
    napi_create_string_utf8(env, input2.c_str(), input2.size(), &constructorArgs1[0]);
    napi_value otherInstance = nullptr;
    napi_new_instance(env, uriClass, 1, constructorArgs1, &otherInstance);
    napi_value args[1] = { otherInstance };
    napi_value result1 = nullptr;
    napi_call_function(env, instance, equalsFn, 1, args, &result1);
    bool res1 = true;
    napi_get_value_bool(env, result1, &res1);
    ASSERT_FALSE(res1);
}

HWTEST_F(NativeEngineTest, ModuleTest003, testing::ext::TestSize.Level0)
{
    napi_env env = (napi_env)engine_;
    napi_value exports = nullptr;
    napi_create_object(env, &exports);
    OHOS::Uri::UriInit(env, exports);
    napi_value uriClass = nullptr;
    napi_value constructorArgs[1] =  { 0 };
    std::string input = "http://name:word@www.uritest.com:99/path/abc?query#fagment";
    napi_create_string_utf8(env, input.c_str(), input.size(), &constructorArgs[0]);
    napi_get_named_property(env, exports, "Uri", &uriClass);
    napi_value instance = nullptr;
    napi_new_instance(env, uriClass, 1, constructorArgs, &instance);
    napi_value tempFn = nullptr;

    napi_get_named_property(env, instance, "checkIsRelative", &tempFn);
    napi_value result = nullptr;
    napi_call_function(env, instance, tempFn, 0, nullptr, &result);
    bool flag = true;
    napi_get_value_bool(env, result, &flag);
    ASSERT_FALSE(flag);

    napi_get_named_property(env, instance, "checkIsOpaque", &tempFn);
    napi_call_function(env, instance, tempFn, 0, nullptr, &result);
    napi_get_value_bool(env, result, &flag);
    ASSERT_FALSE(flag);

    napi_get_named_property(env, instance, "checkIsHierarchical", &tempFn);
    napi_call_function(env, instance, tempFn, 0, nullptr, &result);
    napi_get_value_bool(env, result, &flag);
    ASSERT_TRUE(flag);

    napi_value key = StrToNapiValue(env, "aaa");
    napi_value value = StrToNapiValue(env, "bbb");
    napi_value keyArgs[] = { key, value };
    napi_get_named_property(env, instance, "addQueryValue", &tempFn);
    napi_call_function(env, instance, tempFn, 2, keyArgs, &result);
    std::string res = GetStringUtf8(env, result);
    ASSERT_STREQ(res.c_str(), "http://name:word@www.uritest.com:99/path/abc?query&aaa=bbb#fagment");

    napi_get_named_property(env, instance, "getSegment", &tempFn);
    napi_call_function(env, instance, tempFn, 0, nullptr, &result);
    std::vector<std::string> temp = GetArray(env, result);
    ASSERT_STREQ(temp[0].c_str(), "path");
    ASSERT_STREQ(temp[1].c_str(), "abc");

    napi_value segment = StrToNapiValue(env, "aaa");
    napi_value segargs[] = { segment };
    napi_get_named_property(env, instance, "addSegment", &tempFn);
    napi_call_function(env, instance, tempFn, 1, segargs, &result);
    res = GetStringUtf8(env, result);
    ASSERT_STREQ(res.c_str(), "http://name:word@www.uritest.com:99/path/abc/aaa?query#fagment");

    napi_get_named_property(env, instance, "clearQuery", &tempFn);
    napi_call_function(env, instance, tempFn, 0, nullptr, &result);
    res = GetStringUtf8(env, result);
    ASSERT_STREQ(res.c_str(), "http://name:word@www.uritest.com:99/path/abc#fagment");
}