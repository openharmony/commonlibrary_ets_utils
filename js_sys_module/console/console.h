/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#ifndef CONSOLE_CONSOLE_H
#define CONSOLE_CONSOLE_H

#include <map>
#include <string>
#include <thread>

#include "commonlibrary/ets_utils/js_concurrent_module/common/helper/napi_helper.h"
#include "commonlibrary/ets_utils/js_concurrent_module/common/helper/object_helper.h"
#include "commonlibrary/ets_utils/js_concurrent_module/common/helper/error_helper.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS::JsSysModule {

enum class LogLevel : uint32_t {
    DEBUG = 0,
    INFO,
    WARN,
    ERROR,
    FATAL,
};

class Console {
public:
    Console() = default;
    ~Console() = default;
    static void InitConsoleModule(napi_env env);
    friend class ConsoleTest;

private:
    template<LogLevel LEVEL>
    static napi_value ConsoleLog(napi_env env, napi_callback_info info);
    static napi_value Count(napi_env env, napi_callback_info info);
    static napi_value CountReset(napi_env env, napi_callback_info info);
    static napi_value Dir(napi_env env, napi_callback_info info);
    static napi_value Group(napi_env env, napi_callback_info info);
    static napi_value GroupEnd(napi_env env, napi_callback_info info);
    static napi_value Table(napi_env env, napi_callback_info info);
    static napi_value Time(napi_env env, napi_callback_info info);
    static napi_value TimeLog(napi_env env, napi_callback_info info);
    static napi_value TimeEnd(napi_env env, napi_callback_info info);
    static napi_value Trace(napi_env env, napi_callback_info info);
    static napi_value TraceHybridStack(napi_env env, napi_callback_info info);
    static napi_value Assert(napi_env env, napi_callback_info info);

    static void LogPrint(LogLevel level, const char* content);
    static std::string ParseLogContent(const std::vector<std::string>& params);
    static std::string MakeLogContent(napi_env env, napi_callback_info info, size_t& argc,
                                      size_t startIdx, bool format = true);
    static std::string GetTimerOrCounterName(napi_env env, napi_callback_info info, size_t argc);
    static void PrintTime(std::string timerName, double time, const std::string& log);
    static void GraphTable(napi_env env, napi_value head, napi_value columns, const size_t& length);
    static std::string RenderHead(napi_env env, napi_value Head, std::vector<size_t> columnWidths);
    static void PrintRows(napi_env env, napi_value Rows, std::vector<size_t> columnWidths, size_t indexNum);
    static std::string StringRepeat(size_t number, const std::string& tableChars);
    static std::string ArrayJoin(std::vector<std::string> rowDivider, const std::string& tableChars);
    static std::string GetStringAndStringWidth(napi_env env, napi_value element, size_t& stringLen);

    static thread_local std::map<std::string, int64_t> timerMap;
    static thread_local std::map<std::string, uint32_t> counterMap;
    static thread_local std::string groupIndent;
};
} // namespace Commonlibrary::JsSysModule
#endif // CONSOLE_CONSOLE_H