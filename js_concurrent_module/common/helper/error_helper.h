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

#ifndef JS_CONCURRENT_MODULE_COMMON_HELPER_ERROR_HELPER_H_
#define JS_CONCURRENT_MODULE_COMMON_HELPER_ERROR_HELPER_H_

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "native_engine/native_engine.h"

namespace Commonlibrary::Concurrent::Common::Helper {
class ErrorHelper {
public:
    ErrorHelper() = default;
    ~ErrorHelper() = default;

    static napi_value NewError(napi_env env, int32_t errCode, const char* errMessage)
    {
        std::string errTitle = "";
        napi_value concurrentError = nullptr;

        napi_value code = nullptr;
        napi_create_uint32(env, errCode, &code);

        napi_value name = nullptr;
        std::string errName = "BusinessError";
        if (errCode == WORKERINITIALIZATION_ERROR) {
            errTitle = "Worker initialization failure, ";
        } else if (errCode == WORKENOTRUNNING_ERROR) {
            errTitle = "Worker instance is not running, ";
        } else if (errCode == WORKERUNSUPPORTED_ERROR) {
            errTitle = "The invoked API is not supported in workers, ";
        } else if (errCode == WORKERSERIALIZATION_ERROR) {
            errTitle = "Serializing an uncaught exception failed, ";
        } else if (errCode == WORKERFILEPATH_ERROR) {
            errTitle = "The worker file path is invalid path, ";
        } else if (errCode == NOTCONCURRENTFUNCTION_ERROR) {
            errTitle = "The function is not mark as concurrent, ";
        } else if (errCode == NOTEXIST_ERROR) {
            errTitle = "The task does not exist when it is canceled, ";
        } else if (errCode == RUNNING_ERROR) {
            errTitle = "The task is executing when it is canceled, ";
        }
        napi_create_string_utf8(env, errName.c_str(), NAPI_AUTO_LENGTH, &name);
        napi_value msg = nullptr;
        napi_create_string_utf8(env, (errTitle + std::string(errMessage)).c_str(), NAPI_AUTO_LENGTH, &msg);

        napi_create_error(env, nullptr, msg, &concurrentError);
        napi_set_named_property(env, concurrentError, "code", code);
        napi_set_named_property(env, concurrentError, "name", name);
        return concurrentError;
    }

    static void ThrowError(napi_env env, int32_t errCode, const char* errMessage)
    {
        napi_value concurrentError = NewError(env, errCode, errMessage);
        napi_throw(env, concurrentError);
    }

    static const int32_t TYPE_ERROR = 401; // 401 : the parameter type is incorrect
    static const int32_t WORKERINITIALIZATION_ERROR = 10200003; // 10200003 : worker initialization failure
    static const int32_t WORKENOTRUNNING_ERROR = 10200004; // 10200004 : worker instance is not running
    static const int32_t WORKERUNSUPPORTED_ERROR = 10200005; // 10200005 : the invoked API is not supported in workers
    static const int32_t WORKERSERIALIZATION_ERROR = 10200006; // 10200006 : serialize an uncaught exception failed
    static const int32_t WORKERFILEPATH_ERROR = 10200007; // 10200007 : the worker file path is invalid path
    static const int32_t NOTCONCURRENTFUNCTION_ERROR = 10200014; // 10200014 : the function is not mark as concurrent
    static const int32_t NOTEXIST_ERROR = 10200015; // 10200015 : the task does not exist when it is canceled
    static const int32_t RUNNING_ERROR = 10200016; // 10200016 : the task is executing when it is canceled
};
} // namespace Commonlibrary::Concurrent::Common::Helper
#endif // JS_CONCURRENT_MODULE_COMMON_HELPER_ERROR_HELPER_H_