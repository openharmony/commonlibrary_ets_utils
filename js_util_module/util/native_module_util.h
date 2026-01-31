 /*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef NATIVE_MODULE_UTIL_H
#define NATIVE_MODULE_UTIL_H

#include "napi/native_api.h"

namespace OHOS::Util {
napi_value TextcoderInit(napi_env env, napi_value exports);
napi_value Base64Init(napi_env env, napi_value exports);
napi_value Base64HelperInit(napi_env env, napi_value exports);
napi_value TypeofInit(napi_env env, napi_value exports);
napi_value StringDecoderInit(napi_env env, napi_value exports);
napi_value ArkTSVMInit(napi_env env, napi_value exports);
napi_value UtilInit(napi_env env, napi_value exports);
}
#endif // NATIVE_MODULE_UTIL_H