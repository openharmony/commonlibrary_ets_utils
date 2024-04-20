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

#include <ctime>
#include <iomanip>
#include <iostream>
#include <regex>
#include <sstream>
#include <vector>

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "napi_helper.h"
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
        switch (errCode) {
            case ERR_WORKER_INITIALIZATION:
                errTitle = "Worker initialization failure, ";
                break;
            case ERR_WORKER_NOT_RUNNING:
                errTitle = "Worker instance is not running, ";
                break;
            case ERR_WORKER_UNSUPPORTED:
                errTitle = "The invoked API is not supported in workers, ";
                break;
            case ERR_WORKER_SERIALIZATION:
                errTitle = "An exception occurred during serialization, ";
                break;
            case ERR_WORKER_INVALID_FILEPATH:
                errTitle = "The worker file path is invalid path, ";
                break;
            case ERR_NOT_CONCURRENT_FUNCTION:
                errTitle = "The function is not mark as concurrent, ";
                break;
            case ERR_CANCEL_NONEXIST_TASK:
                errTitle = "The task does not exist when it is canceled, ";
                break;
            case ERR_CANCEL_NONEXIST_TASK_GROUP:
                errTitle = "The task group does not exist when it is canceled, ";
                break;
            case ERR_CANCEL_RUNNING_TASK:
                errTitle = "The task is executing when it is canceled";
                break;
            case ERR_TRIGGER_NONEXIST_EVENT:
                errTitle = "The globalCallObject is not registered";
                break;
            case ERR_CALL_METHOD_ON_BINDING_OBJ:
                errTitle = "The method to be called is not callable or is an async method or a generator.";
                break;
            case ERR_GLOBAL_CALL_TIMEOUT:
                errTitle = "The global call exceeds the timeout.";
                break;
            case ERR_NOT_IN_TASKPOOL_THREAD:
                errTitle = "The function is not called in the taskpool thread";
                break;
            case ERR_NOT_IN_CONCURRENT_FUNCTION:
                errTitle = "The function is not called in the concurrent function";
                break;
            case ERR_NOT_REGISTERED:
                errTitle = "The callback is not registered on the host side";
                break;
            case ERR_CIRCULAR_DEPENDENCY:
                errTitle = "There is a circular dependency";
                break;
            case ERR_INEXISTENT_DEPENDENCY:
                errTitle = "The dependency does not exist, ";
                break;
            case ERR_DELAY_TIME_ERROR:
                errTitle = "The delayTime is less than zero";
                break;
            case ERR_IN_BOTH_CLONE_AND_TRANSFER:
                errTitle = "Can not set an arraybuffer to both transferList and cloneList";
                break;
            case ERR_NO_SUCH_ASYNCLOCK:
                errTitle = "No such lock";
                break;
            case ERR_ASYNCLOCK_TIMEOUT:
                errTitle = "lockAsync timed out";
                break;
            case TYPE_ERROR:
                errTitle = "The input parameters are invalid, ";
                break;
            case ERR_REGISTRATION_OF_LISTENERS:
                errTitle = "The executed task does not support the registration of listeners.";
                break;
            default:
                break;
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

    static std::string GetCurrentTimeStamp()
    {
        auto now = std::chrono::system_clock::now();
        std::time_t currentTimeStamp = std::chrono::system_clock::to_time_t(now);
        std::tm* timeInfo = std::localtime(&currentTimeStamp);
        std::stringstream ss;
        ss << std::put_time(timeInfo, "%Y-%m-%d %X");
        return ss.str();
    }

    static std::string GetErrorFileInfo(const std::string& input)
    {
        std::regex pattern("\\((.*?)\\)");
        std::smatch match;
        if (std::regex_search(input, match, pattern)) {
            return match[1].str();
        }
        return "";
    }

    static std::vector<std::string> SplitErrorFileInfo(const std::string& input, char delimiter, int count)
    {
        std::vector<std::string> result;
        std::string rawErrorInfo = GetErrorFileInfo(input);
        if (rawErrorInfo.empty()) {
            return result;
        }

        auto pos = rawErrorInfo.rfind(delimiter);
        while (pos != std::string::npos && count > 0) {
            result.push_back(rawErrorInfo.substr(pos + 1));
            rawErrorInfo = rawErrorInfo.substr(0, pos);
            pos = rawErrorInfo.rfind(delimiter);
            count--;
        }
        result.push_back(rawErrorInfo);
        std::reverse(result.begin(), result.end());
        return result;
    }

    static napi_value TranslateErrorEvent(napi_env env, napi_value error)
    {
        napi_value obj = NapiHelper::CreateObject(env);

        // add message
        napi_value msgValue = nullptr;
        napi_coerce_to_string(env, error, &msgValue);
        napi_set_named_property(env, obj, "message", msgValue);

        // add backtrace
        napi_value stack = NapiHelper::GetNameProperty(env, error, "stack");
        napi_set_named_property(env, obj, "backtrace", stack);

        // add timeStamp
        std::string current = GetCurrentTimeStamp();
        napi_value timeStamp = nullptr;
        napi_create_string_utf8(env, current.c_str(), NAPI_AUTO_LENGTH, &timeStamp);
        napi_set_named_property(env, obj, "timeStamp", timeStamp);
        char* stackValue = NapiHelper::GetString(env, stack);
        std::string rawStack = std::string(stackValue);
        delete[] stackValue;
        std::vector<std::string> result = SplitErrorFileInfo(rawStack, ':', 2); // 2 : the last two :
        if (result.size() == 3) { // 3 : the rawStack is divided into three parts by last two :
            // add filename
            napi_value filenameValue = nullptr;
            napi_create_string_utf8(env, result[0].c_str(), NAPI_AUTO_LENGTH, &filenameValue); // 0 : filename
            napi_set_named_property(env, obj, "filename", filenameValue);

            // add lineno
            double line = result[1].empty() ? 0 : std::strtod(result[1].c_str(), nullptr); // 1 : index of lineno
            napi_value lineno = nullptr;
            napi_create_double(env, line, &lineno);
            napi_set_named_property(env, obj, "lineno", lineno);

            // add colno
            double col = result[2].empty() ? 0 : std::strtod(result[2].c_str(), nullptr); // 2 : index of colno
            napi_value colno = nullptr;
            napi_create_double(env, col, &colno);
            napi_set_named_property(env, obj, "colno", colno);
        }

        // add type
        napi_value eventType = nullptr;
        napi_create_string_utf8(env, "ErrorEvent", NAPI_AUTO_LENGTH, &eventType);
        napi_set_named_property(env, obj, "type", eventType);

        // add error
        napi_set_named_property(env, obj, "error", error);

        return obj;
    }

    static napi_value ObjectToError(napi_env env, napi_value error)
    {
        napi_value message = NapiHelper::GetNameProperty(env, error, "message");
        napi_value backtrace = NapiHelper::GetNameProperty(env, error, "backtrace");
        napi_value businessError = nullptr;
        napi_create_error(env, nullptr, message, &businessError);
        napi_set_named_property(env, businessError, "stack", backtrace);
        napi_set_named_property(env, businessError, "name", message);
        return businessError;
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
    static const int32_t ERR_TRIGGER_NONEXIST_EVENT = 10200019; // 10200019 : The triggered event does not exist
    // 10200020 : The called method is not callable or async or generator
    static const int32_t ERR_CALL_METHOD_ON_BINDING_OBJ = 10200020;
    static const int32_t ERR_GLOBAL_CALL_TIMEOUT = 10200021; // 10200021 : Global call has exceeded the timeout
    // 10200022 : The function is not called in the taskpool thread
    static const int32_t ERR_NOT_IN_TASKPOOL_THREAD = 10200022;
    // 10200023 : The function is not called in the concurrent function
    static const int32_t ERR_NOT_IN_CONCURRENT_FUNCTION = 10200023;
    static const int32_t ERR_NOT_REGISTERED = 10200024; // 10200024 : The callback is not registered on the host side
    // 10200025 : add dependent task to SequenceRunner
    static const int32_t ERR_ADD_DEPENDENT_TASK_TO_SEQRUNNER = 10200025;
    static const int32_t ERR_CIRCULAR_DEPENDENCY = 10200026; // 10200026 : There is a circular dependency
    static const int32_t ERR_INEXISTENT_DEPENDENCY = 10200027; // 10200027: The dependency does not exist
    static const int32_t ERR_DELAY_TIME_ERROR = 10200028; // 10200028 : The delayTime is less than zero
    // 10200029 : Obj in both clone array and transfer array
    static const int32_t ERR_IN_BOTH_CLONE_AND_TRANSFER = 10200029;
    static const int32_t ERR_NO_SUCH_ASYNCLOCK = 10200030;
    // 10200031 : lockAsync funtion timeout exceeded
    static const int32_t ERR_ASYNCLOCK_TIMEOUT = 10200031;
    // 10200034 : The executed task does not support the registration of listeners
    static const int32_t ERR_REGISTRATION_OF_LISTENERS = 10200034;
};
} // namespace Commonlibrary::Concurrent::Common::Helper
#endif // JS_CONCURRENT_MODULE_COMMON_HELPER_ERROR_HELPER_H