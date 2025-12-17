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

#ifndef HISPEED_STRING_PLUGIN_H
#define HISPEED_STRING_PLUGIN_H

#if defined(__aarch64__) || defined(_M_ARM64)
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*HSDPlugin_Base64Encode)(const unsigned char *,
    size_t, unsigned char *, size_t, size_t *, size_t*, uint64_t);
typedef int (*HSDPlugin_Base64Decode)(const unsigned char *,
    size_t, unsigned char *, size_t, size_t *, size_t*, uint64_t);

extern HSDPlugin_Base64Encode g_base64EncodeFunc;
extern HSDPlugin_Base64Decode g_base64DecodeFunc;

void Base64LoadHispeedPlugin();
void Base64UnloadHispeedPlugin();

#ifdef __cplusplus
}
#endif

#endif
#endif /* HISPEED_STRING_PLUGIN_H */