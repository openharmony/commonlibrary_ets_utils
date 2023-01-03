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

#ifndef JS_TASKPOOL_MODULE_TASKPOOL_ERROR_HELPER_H_
#define JS_TASKPOOL_MODULE_TASKPOOL_ERROR_HELPER_H_

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "native_engine/native_engine.h"

class ErrorHelper {
public:
    ErrorHelper() = default;
    ~ErrorHelper() = default;

    static void ThrowError(napi_env env, int32_t errCode, const char* errMessage)
    {
        std::string errTitle = "";
        napi_value workerError = nullptr;

        napi_value code = nullptr;
        napi_create_uint32(env, errCode, &code);

        napi_value name = nullptr;
        std::string errName = "BusinessError";
        napi_create_string_utf8(env, errName.c_str(), NAPI_AUTO_LENGTH, &name);
        napi_value msg = nullptr;
        napi_create_string_utf8(env, (errTitle + std::string(errMessage)).c_str(), NAPI_AUTO_LENGTH, &msg);

        napi_create_error(env, nullptr, msg, &workerError);
        napi_set_named_property(env, workerError, "code", code);
        napi_set_named_property(env, workerError, "name", name);
        napi_throw(env, workerError);
    }

    static const int32_t TYPE_ERROR = 401;
    static const int32_t NOTEXIST_ERROR = 10200015;
    static const int32_t RUNNING_ERROR = 10200016;
};
#endif // JS_TASKPOOL_MODULE_TASKPOOL_TASK_H_