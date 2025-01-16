/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef COMMONLIBRARY_ETS_UTILS_TOOLS_ETS_ERROR_H
#define COMMONLIBRARY_ETS_UTILS_TOOLS_ETS_ERROR_H

#include <string>

#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS::Tools {
class ErrorHelper {
public:
    ErrorHelper() = default;
    ~ErrorHelper() = default;

    static napi_value ThrowError(napi_env env, int32_t errCode, const char* errMessage)
    {
        napi_value errorValue = nullptr;
        // err-name
        std::string errName = "BusinessError";
        napi_value name = nullptr;
        napi_create_string_utf8(env, errName.c_str(), NAPI_AUTO_LENGTH, &name);
        // err-code
        napi_value code = nullptr;
        napi_create_int32(env, errCode, &code);
        // err-message
        napi_value msg = nullptr;
        napi_create_string_utf8(env, errMessage, NAPI_AUTO_LENGTH, &msg);

        napi_create_error(env, nullptr, msg, &errorValue);
        napi_set_named_property(env, errorValue, "code", code);
        napi_set_named_property(env, errorValue, "name", name);

        napi_throw(env, errorValue);
        return nullptr;
    }

    static napi_value CreateError(napi_env env, int32_t errCode, const char* errMessage)
    {
        napi_value errorValue = nullptr;
        // err-name
        std::string errName = "BusinessError";
        napi_value name = nullptr;
        napi_create_string_utf8(env, errName.c_str(), NAPI_AUTO_LENGTH, &name);
        // err-code
        napi_value code = nullptr;
        napi_create_int32(env, errCode, &code);
        // err-message
        napi_value msg = nullptr;
        napi_create_string_utf8(env, errMessage, NAPI_AUTO_LENGTH, &msg);

        napi_create_error(env, nullptr, msg, &errorValue);
        napi_set_named_property(env, errorValue, "code", code);
        napi_set_named_property(env, errorValue, "name", name);

        return errorValue;
    }
};
} // namespace OHOS::Tools
#endif /* COMMONLIBRARY_ETS_UTILS_TOOLS_ETS_ERROR_H */
