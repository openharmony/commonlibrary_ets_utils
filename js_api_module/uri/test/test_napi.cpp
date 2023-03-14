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
#include "utils/log.h"
#include "js_uri.h"

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
    ASSERT_STREQ(uri.GetUserinfo().c_str(), "null");
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
    ASSERT_STREQ(uri.GetUserinfo().c_str(), "null");
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
    ASSERT_STREQ(uri.GetUserinfo().c_str(), "null");
    ASSERT_STREQ(uri.GetHost().c_str(), "null");
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
    ASSERT_STREQ(uri.GetUserinfo().c_str(), "null");
    ASSERT_STREQ(uri.GetHost().c_str(), "null");
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
    ASSERT_STREQ(uri.GetUserinfo().c_str(), "null");
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
    ASSERT_STREQ(uri.GetAuthority().c_str(), "null");
    ASSERT_STREQ(uri.GetSsp().c_str(), "/username:password@www.baidu.com:99/path/pathquery");
    ASSERT_STREQ(uri.GetUserinfo().c_str(), "null");
    ASSERT_STREQ(uri.GetHost().c_str(), "null");
    ASSERT_STREQ(uri.GetPort().c_str(), "-1");
    ASSERT_STREQ(uri.GetPath().c_str(), "/username:password@www.baidu.com:99/path/path");
    ASSERT_STREQ(uri.GetQuery().c_str(), "query");
    ASSERT_STREQ(uri.GetFragment().c_str(), "fagment");
}

HWTEST_F(NativeEngineTest, ConstructorTest021, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http:/&username:password@[1080::8:800:200C:417A]:99/path/66path1?query#fagment");
    ASSERT_STREQ(uri.GetScheme().c_str(), "http");
    ASSERT_STREQ(uri.GetAuthority().c_str(), "null");
    ASSERT_STREQ(uri.GetSsp().c_str(), "/&username:password@[1080::8:800:200C:417A]:99/path/66path1query");
    ASSERT_STREQ(uri.GetUserinfo().c_str(), "null");
    ASSERT_STREQ(uri.GetHost().c_str(), "null");
    ASSERT_STREQ(uri.GetPort().c_str(), "-1");
    ASSERT_STREQ(uri.GetPath().c_str(), "/&username:password@[1080::8:800:200C:417A]:99/path/66path1");
    ASSERT_STREQ(uri.GetQuery().c_str(), "query");
    ASSERT_STREQ(uri.GetFragment().c_str(), "fagment");
}

HWTEST_F(NativeEngineTest, ConstructorTest022, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http:/[username:password@[::FFFF:129.144.52.38]:99/path/path?query#fagment");
    ASSERT_STREQ(uri.GetScheme().c_str(), "http");
    ASSERT_STREQ(uri.GetAuthority().c_str(), "null");
    ASSERT_STREQ(uri.GetSsp().c_str(), "/[username:password@[::FFFF:129.144.52.38]:99/path/pathquery");
    ASSERT_STREQ(uri.GetUserinfo().c_str(), "null");
    ASSERT_STREQ(uri.GetHost().c_str(), "null");
    ASSERT_STREQ(uri.GetPort().c_str(), "-1");
    ASSERT_STREQ(uri.GetPath().c_str(), "/[username:password@[::FFFF:129.144.52.38]:99/path/path");
    ASSERT_STREQ(uri.GetQuery().c_str(), "query");
    ASSERT_STREQ(uri.GetFragment().c_str(), "fagment");
}

HWTEST_F(NativeEngineTest, ConstructorTest023, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http:username:password@[1080::8:800:200C:417A]:99/path/66path1?query#fagment");
    ASSERT_STREQ(uri.GetScheme().c_str(), "http");
    ASSERT_STREQ(uri.GetAuthority().c_str(), "null");
    ASSERT_STREQ(uri.GetSsp().c_str(), "username:password@[1080::8:800:200C:417A]:99/path/66path1query");
    ASSERT_STREQ(uri.GetUserinfo().c_str(), "null");
    ASSERT_STREQ(uri.GetHost().c_str(), "null");
    ASSERT_STREQ(uri.GetPort().c_str(), "-1");
    ASSERT_STREQ(uri.GetPath().c_str(), "null");
    ASSERT_STREQ(uri.GetQuery().c_str(), "null");
    ASSERT_STREQ(uri.GetFragment().c_str(), "fagment");
}

HWTEST_F(NativeEngineTest, ConstructorTest024, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http:^$username:password@[::192.9.5.5]:99/path/path?query#fagment");
    ASSERT_STREQ(uri.GetScheme().c_str(), "http");
    ASSERT_STREQ(uri.GetAuthority().c_str(), "null");
    ASSERT_STREQ(uri.GetSsp().c_str(), "^$username:password@[::192.9.5.5]:99/path/pathquery");
    ASSERT_STREQ(uri.GetUserinfo().c_str(), "null");
    ASSERT_STREQ(uri.GetHost().c_str(), "null");
    ASSERT_STREQ(uri.GetPort().c_str(), "-1");
    ASSERT_STREQ(uri.GetPath().c_str(), "null");
    ASSERT_STREQ(uri.GetQuery().c_str(), "null");
    ASSERT_STREQ(uri.GetFragment().c_str(), "fagment");
}

HWTEST_F(NativeEngineTest, ConstructorTest025, testing::ext::TestSize.Level0)
{
    OHOS::Uri::Uri uri("http:[?]username:password@[fe80::0001:0000]:99/path/path?query#fagment");
    ASSERT_STREQ(uri.GetScheme().c_str(), "http");
    ASSERT_STREQ(uri.GetAuthority().c_str(), "null");
    ASSERT_STREQ(uri.GetSsp().c_str(), "[]username:password@[fe80::0001:0000]:99/path/path?query");
    ASSERT_STREQ(uri.GetUserinfo().c_str(), "null");
    ASSERT_STREQ(uri.GetHost().c_str(), "null");
    ASSERT_STREQ(uri.GetPort().c_str(), "-1");
    ASSERT_STREQ(uri.GetPath().c_str(), "null");
    ASSERT_STREQ(uri.GetQuery().c_str(), "null");
    ASSERT_STREQ(uri.GetFragment().c_str(), "fagment");
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
