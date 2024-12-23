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
                errTitle = "Worker initialization failed, ";
                break;
            case ERR_WORKER_NOT_RUNNING:
                errTitle = "The Worker instance is not running, ";
                break;
            case ERR_WORKER_UNSUPPORTED:
                errTitle = "The called API is not supported in the worker thread, ";
                break;
            case ERR_WORKER_SERIALIZATION:
                errTitle = "An exception occurred during serialization, ";
                break;
            case ERR_WORKER_INVALID_FILEPATH:
                errTitle = "The worker file path is invalid, ";
                break;
            case ERR_NOT_CONCURRENT_FUNCTION:
                errTitle = "The function is not marked as concurrent, ";
                break;
            case ERR_CANCEL_NONEXIST_TASK:
                errTitle = "The task to cancel does not exist, ";
                break;
            case ERR_CANCEL_NONEXIST_TASK_GROUP:
                errTitle = "The task group to cancel does not exist, ";
                break;
            case ERR_CANCEL_RUNNING_TASK:
                errTitle = "The task to cancel is being executed";
                break;
            case ERR_TRIGGER_NONEXIST_EVENT:
                errTitle = "The globalCallObject is not registered";
                break;
            case ERR_CALL_METHOD_ON_BINDING_OBJ:
                errTitle = "The method to be called is not callable or is an async method or a generator.";
                break;
            case ERR_GLOBAL_CALL_TIMEOUT:
                errTitle = "Waiting for a global call timed out";
                break;
            case ERR_NOT_IN_TASKPOOL_THREAD:
                errTitle = "The function is not called in the TaskPool thread";
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
                errTitle = "";
                break;
            case ERR_IN_BOTH_CLONE_AND_TRANSFER:
                errTitle = "An ArrayBuffer cannot be set as both a transfer list and a clone list";
                break;
            case ERR_NO_SUCH_ASYNCLOCK:
                errTitle = "The lock does not exist";
                break;
            case ERR_ASYNCLOCK_TIMEOUT:
                errTitle = "lockAsync timed out";
                break;
            case TYPE_ERROR:
                errTitle = "Parameter error. The input parameters are invalid, ";
                break;
            case ERR_REGISTRATION_OF_LISTENERS:
                errTitle = "The executed task does not support the registration of listeners.";
                break;
            case ERR_TASK_EXECUTE_PERIODICALLY:
                errTitle = "The concurrent task has been executed and cannot be executed periodically.";
                break;
            case ERR_TASK_EXECUTE_AGAIN:
                errTitle = "The periodic task cannot be executed again, ";
                break;
            case ERR_TASK_HAVE_DEPENDENCY:
                errTitle = "The periodic task cannot have a dependency.";
                break;
            case ERR_ASYNCRUNNER_TASK_DISCARDED:
                errTitle = "The asyncRunner task is discarded.";
                break;
            case ERR_ASYNCRUNNER_TASK_CANCELED:
                errTitle = "The asyncRunner task has been canceled.";
                break;
            case ERR_ASYNCRUNNER_TASK_HAVE_DEPENDENCY:
                errTitle = "The task has been executed by AsyncRunner.";
                break;
            case ERR_TASK_CANNOT_EXECUTED:
                errTitle = "The task cannot be executed by two APIs, ";
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
        std::string rawStack = NapiHelper::GetString(env, stack);
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
    static const int32_t ERR_WORKER_INITIALIZATION = 10200003; // 10200003 : Worker initialization failed
    static const int32_t ERR_WORKER_NOT_RUNNING = 10200004; // 10200004 : The Worker instance is not running
    // 10200005 : The called API is not supported in the worker thread
    static const int32_t ERR_WORKER_UNSUPPORTED = 10200005;
    static const int32_t ERR_WORKER_SERIALIZATION = 10200006; // 10200006 : serialize an uncaught exception failed
    static const int32_t ERR_WORKER_INVALID_FILEPATH = 10200007; // 10200007 : The worker file path is invalid
    static const int32_t ERR_NOT_CONCURRENT_FUNCTION = 10200014; // 10200014 : The function is not marked as concurrent
    static const int32_t ERR_CANCEL_NONEXIST_TASK = 10200015; // 10200015 : The task to cancel does not exist
    static const int32_t ERR_CANCEL_RUNNING_TASK = 10200016; // 10200016 : The task to cancel is being executed
    // 10200018 : The task group to cancel does not exist
    static const int32_t ERR_CANCEL_NONEXIST_TASK_GROUP = 10200018;
    static const int32_t ERR_TRIGGER_NONEXIST_EVENT = 10200019; // 10200019 : The triggered event does not exist
    // 10200020 : The called method is not callable or async or generator
    static const int32_t ERR_CALL_METHOD_ON_BINDING_OBJ = 10200020;
    static const int32_t ERR_GLOBAL_CALL_TIMEOUT = 10200021; // 10200021 : Waiting for a global call timed out
    // 10200022 : The function is not called in the TaskPool thread
    static const int32_t ERR_NOT_IN_TASKPOOL_THREAD = 10200022;
    // 10200023 : The function is not called in the concurrent function
    static const int32_t ERR_NOT_IN_CONCURRENT_FUNCTION = 10200023;
    static const int32_t ERR_NOT_REGISTERED = 10200024; // 10200024 : The callback is not registered on the host side
    // 10200025 : dependent task not allowed.
    static const int32_t ERR_ADD_DEPENDENT_TASK_TO_SEQRUNNER = 10200025;
    static const int32_t ERR_CIRCULAR_DEPENDENCY = 10200026; // 10200026 : There is a circular dependency
    static const int32_t ERR_INEXISTENT_DEPENDENCY = 10200027; // 10200027: The dependency does not exist
    static const int32_t ERR_DELAY_TIME_ERROR = 10200028; // 10200028 : The delayTime is less than zero
    // 10200029 : An ArrayBuffer cannot be set as both a transfer list and a clone list
    static const int32_t ERR_IN_BOTH_CLONE_AND_TRANSFER = 10200029;
    static const int32_t ERR_NO_SUCH_ASYNCLOCK = 10200030;
    // 10200031 : lockAsync funtion timeout exceeded
    static const int32_t ERR_ASYNCLOCK_TIMEOUT = 10200031;
    // 10200034 : The executed task does not support the registration of listeners
    static const int32_t ERR_REGISTRATION_OF_LISTENERS = 10200034;
    // 10200050 : The concurrent task has been executed and cannot be executed periodically
    static const int32_t ERR_TASK_EXECUTE_PERIODICALLY = 10200050;
    // 10200051 : The periodic task cannot be executed again
    static const int32_t ERR_TASK_EXECUTE_AGAIN = 10200051;
    // 10200052 : The periodic task cannot have a dependency
    static const int32_t ERR_TASK_HAVE_DEPENDENCY = 10200052;
    // 10200054 : The asyncRunner task discarded
    static const int32_t ERR_ASYNCRUNNER_TASK_DISCARDED = 10200054;
    // 10200055 : The asyncRunner task has been canceled
    static const int32_t ERR_ASYNCRUNNER_TASK_CANCELED = 10200055;
    // 10200056 : The task has been executed by AsyncRunner
    static const int32_t ERR_ASYNCRUNNER_TASK_HAVE_DEPENDENCY = 10200056;
    // 10200056 : The task cannot be executed by two APIs
    static const int32_t ERR_TASK_CANNOT_EXECUTED = 10200057;
};
} // namespace Commonlibrary::Concurrent::Common::Helper
#endif // JS_CONCURRENT_MODULE_COMMON_HELPER_ERROR_HELPER_H