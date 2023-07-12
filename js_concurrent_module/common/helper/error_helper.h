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

#ifndef JS_CONCURRENT_MODULE_COMMON_HELPER_ERROR_HELPER_H
#define JS_CONCURRENT_MODULE_COMMON_HELPER_ERROR_HELPER_H

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "native_engine/native_engine.h"

namespace Commonlibrary::Concurrent::Common::Helper {
class ErrorHelper {
public:
    ErrorHelper() = default;
    ~ErrorHelper() = default;

    static napi_value NewError(napi_env env, int32_t errCode, const char* errMessage = nullptr)
    {
        std::string errTitle = "";
        napi_value concurrentError = nullptr;

        napi_value code = nullptr;
        napi_create_uint32(env, errCode, &code);

        napi_value name = nullptr;
        std::string errName = "BusinessError";
        if (errCode == ERR_WORKER_INITIALIZATION) {
            errTitle = "Worker initialization failure, ";
        } else if (errCode == ERR_WORKER_NOT_RUNNING) {
            errTitle = "Worker instance is not running, ";
        } else if (errCode == ERR_WORKER_UNSUPPORTED) {
            errTitle = "The invoked API is not supported in workers, ";
        } else if (errCode == ERR_WORKER_SERIALIZATION) {
            errTitle = "An exception occurred during serialization, ";
        } else if (errCode == ERR_WORKER_INVALID_FILEPATH) {
            errTitle = "The worker file path is invalid path, ";
        } else if (errCode == ERR_NOT_CONCURRENT_FUNCTION) {
            errTitle = "The function is not mark as concurrent, ";
        } else if (errCode == ERR_CANCEL_NONEXIST_TASK) {
            errTitle = "The task does not exist when it is canceled";
        } else if (errCode == ERR_CANCEL_NONEXIST_TASK_GROUP) {
            errTitle = "The task group does not exist when it is canceled";
        } else if (errCode == ERR_CANCEL_RUNNING_TASK) {
            errTitle = "The task is executing when it is canceled";
        }
        napi_create_string_utf8(env, errName.c_str(), NAPI_AUTO_LENGTH, &name);
        napi_value msg = nullptr;
        if (errMessage == nullptr) {
            napi_create_string_utf8(env, errTitle.c_str(), NAPI_AUTO_LENGTH, &msg);
        } else {
            napi_create_string_utf8(env, (errTitle + std::string(errMessage)).c_str(), NAPI_AUTO_LENGTH, &msg);
        }

        napi_create_error(env, nullptr, msg, &concurrentError);
        napi_set_named_property(env, concurrentError, "code", code);
        napi_set_named_property(env, concurrentError, "name", name);
        return concurrentError;
    }

    static void ThrowError(napi_env env, int32_t errCode, const char* errMessage = nullptr)
    {
        napi_value concurrentError = NewError(env, errCode, errMessage);
        napi_throw(env, concurrentError);
    }

    static const int32_t TYPE_ERROR = 401; // 401 : the parameter type is incorrect
    static const int32_t ERR_WORKER_INITIALIZATION = 10200003; // 10200003 : worker initialization failure
    static const int32_t ERR_WORKER_NOT_RUNNING = 10200004; // 10200004 : worker instance is not running
    static const int32_t ERR_WORKER_UNSUPPORTED = 10200005; // 10200005 : the invoked API is not supported in worker
    static const int32_t ERR_WORKER_SERIALIZATION = 10200006; // 10200006 : serialize an uncaught exception failed
    static const int32_t ERR_WORKER_INVALID_FILEPATH = 10200007; // 10200007 : the worker file path is invalid path
    static const int32_t ERR_NOT_CONCURRENT_FUNCTION = 10200014; // 10200014 : the function is not mark as concurrent
    static const int32_t ERR_CANCEL_NONEXIST_TASK = 10200015; // 10200015 : the task does not exist when it is canceled
    static const int32_t ERR_CANCEL_RUNNING_TASK = 10200016; // 10200016 : the task is executing when it is canceled
    static const int32_t ERR_CANCEL_NONEXIST_TASK_GROUP = 10200018; // 10200018 : cancel nonexist task group
};
} // namespace Commonlibrary::Concurrent::Common::Helper
#endif // JS_CONCURRENT_MODULE_COMMON_HELPER_ERROR_HELPER_H