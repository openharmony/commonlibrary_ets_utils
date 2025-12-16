/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#if defined(__aarch64__) || defined(_M_ARM64)
#include "hispeed_string_plugin.h"

static const char *HISPEED_STRING_SO_PATH = "/system/lib64/libhispeed_string.so";
static void *hispeedStringHandle = NULL;
HSDPlugin_Base64Encode g_base64EncodeFunc = NULL; // HMS_Str_Base64Encode
HSDPlugin_Base64Decode g_base64DecodeFunc = NULL; // HMS_Str_Base64Decode

void Base64LoadHispeedPlugin()
{
    hispeedStringHandle = dlopen(HISPEED_STRING_SO_PATH, RTLD_LAZY);
    if (hispeedStringHandle == NULL) {
        return;
    }
    g_base64EncodeFunc = (HSDPlugin_Base64Encode)dlsym(hispeedStringHandle, "HMS_Str_Base64Encode");
    g_base64DecodeFunc = (HSDPlugin_Base64Decode)dlsym(hispeedStringHandle, "HMS_Str_Base64Decode");
}

void Base64UnloadHispeedPlugin()
{
    if (hispeedStringHandle != NULL) {
        dlclose(hispeedStringHandle);
        hispeedStringHandle = NULL;
        g_base64EncodeFunc = NULL;
        g_base64DecodeFunc = NULL;
    }
}
#endif