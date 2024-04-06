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
#include "console.h"

#include <chrono>
#include <vector>
#include "log.h"

namespace OHOS::JsSysModule {
using namespace Commonlibrary::Concurrent::Common;

thread_local std::map<std::string, int64_t> Console::timerMap;
thread_local std::map<std::string, uint32_t> Console::counterMap;
thread_local std::string Console::groupIndent;
constexpr size_t GROUPINDETATIONWIDTH = 2; // 2 : indentation
constexpr uint32_t SECOND = 1000;
constexpr uint32_t MINUTE = 60 * SECOND;
constexpr uint32_t HOUR = 60 * MINUTE;

std::map<std::string, std::string> tableChars = {
    {"middleMiddle", "─"},
    {"rowMiddle", "┼"},
    {"topRight", "┐"},
    {"topLeft", "┌"},
    {"leftMiddle", "├"},
    {"topMiddle", "┬"},
    {"bottomRight", "┘"},
    {"bottomLeft", "└"},
    {"bottomMiddle", "┴"},
    {"rightMiddle", "┤"},
    {"left", "│ "},
    {"right", " │"},
    {"middle", " │ "},
};

void Console::LogPrint(LogLevel level, const char* content)
{
    switch (level) {
        case LogLevel::DEBUG:
            HILOG_DEBUG("%{public}s", content);
            break;
        case LogLevel::INFO:
            HILOG_INFO("%{public}s", content);
            break;
        case LogLevel::WARN:
            HILOG_WARN("%{public}s", content);
            break;
        case LogLevel::ERROR:
            HILOG_ERROR("%{public}s", content);
            break;
        case LogLevel::FATAL:
            HILOG_FATAL("%{public}s", content);
            break;
        default:
            HILOG_FATAL("Console::LogPrint: this branch is unreachable");
    }
}

std::string Console::ParseLogContent(const std::vector<std::string>& params)
{
    std::string ret;
    if (params.empty()) {
        return ret;
    }
    std::string formatStr = params[0];
    size_t size = params.size();
    size_t len = formatStr.size();
    size_t pos = 0;
    size_t count = 1;
    for (; pos < len; ++pos) {
        if (count >= size) {
            break;
        }
        if (formatStr[pos] == '%') {
            if (pos + 1 >= len) {
                break;
            }
            switch (formatStr[pos + 1]) {
                case 's':
                case 'j':
                case 'd':
                case 'O':
                case 'o':
                case 'i':
                case 'f':
                case 'c':
                    ret += params[count++];
                    ++pos;
                    break;
                case '%':
                    ret += formatStr[pos];
                    ++pos;
                    break;
                default:
                    ret += formatStr[pos];
                    break;
            }
        } else {
            ret += formatStr[pos];
        }
    }
    if (pos < len) {
        ret += formatStr.substr(pos, len - pos);
    }
    for (; count < size; ++count) {
        ret += " ";
        ret += params[count];
    }
    return ret;
}

std::string Console::MakeLogContent(napi_env env, napi_callback_info info, size_t& argc, size_t startIdx, bool format)
{
    std::vector<std::string> content;
    content.reserve(argc);
    // get argv
    napi_value* argv = new napi_value[argc];
    Helper::ObjectScope<napi_value> scope(argv, true);
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);

    for (size_t i = startIdx; i < argc; i++) {
        if (!Helper::NapiHelper::IsString(env, argv[i])) {
            napi_value buffer;
            napi_status status = napi_coerce_to_string(env, argv[i], &buffer);
            if (status != napi_ok) {
                HILOG_ERROR("Console log failed to convert to string object");
                continue;
            }
            argv[i] = buffer;
        }
        std::string stringValue = Helper::NapiHelper::GetPrintString(env, argv[i]);
        if (stringValue.empty()) {
            continue;
        }
        content.emplace_back(stringValue);
    }
    if (format) {
        return ParseLogContent(content);
    } else {
        std::string ret;
        size_t size = content.size();
        for (size_t i = 0; i < size; ++i) {
            ret += " ";
            ret += content[i];
        }
        return ret;
    }
}

std::string Console::StringRepeat(size_t number, const std::string& tableChars)
{
    std::string divider;
    size_t length = number;
    for (size_t i = 0; i < length; i++) {
        divider += tableChars;
    }
    return divider;
}


template<LogLevel LEVEL>
napi_value Console::ConsoleLog(napi_env env, napi_callback_info info)
{
    size_t argc = Helper::NapiHelper::GetCallbackInfoArgc(env, info);
    if (argc < 1) {
        return Helper::NapiHelper::GetUndefinedValue(env);
    }
    std::string content;
    content += groupIndent;
    content += MakeLogContent(env, info, argc, 0); // startInx = 0
    LogPrint(LEVEL, content.c_str());
    return Helper::NapiHelper::GetUndefinedValue(env);
}

std::string Console::GetTimerOrCounterName(napi_env env, napi_callback_info info, size_t argc)
{
    if (argc < 1) {
        return "default";
    }
    napi_value* argv = new napi_value[argc];
    Helper::ObjectScope<napi_value> scope(argv, true);
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (!Helper::NapiHelper::IsString(env, argv[0])) {
        napi_value buffer = nullptr;
        napi_status status = napi_coerce_to_string(env, argv[0], &buffer);
        if (status != napi_ok) {
            Helper::ErrorHelper::ThrowError(env, Helper::ErrorHelper::TYPE_ERROR,
                                            "Timer or Counter name must be String.");
            return "";
        }
        argv[0] = buffer;
    }
    std::string name = Helper::NapiHelper::GetPrintString(env, argv[0]);
    if (name.empty()) {
        Helper::ErrorHelper::ThrowError(env, Helper::ErrorHelper::TYPE_ERROR,
                                        "Timer or Counter name must be not null.");
        return "";
    }
    return name;
}

napi_value Console::Count(napi_env env, napi_callback_info info)
{
    size_t argc = Helper::NapiHelper::GetCallbackInfoArgc(env, info);
    std::string counterName = GetTimerOrCounterName(env, info, argc);
    counterMap[counterName]++;
    HILOG_INFO("%{public}s%{public}s: %{public}d",
               groupIndent.c_str(), counterName.c_str(), counterMap[counterName]);
    return Helper::NapiHelper::GetUndefinedValue(env);
}

napi_value Console::CountReset(napi_env env, napi_callback_info info)
{
    size_t argc = Helper::NapiHelper::GetCallbackInfoArgc(env, info);
    std::string counterName = GetTimerOrCounterName(env, info, argc);
    if (counterMap.find(counterName) == counterMap.end()) {
        HILOG_WARN("%{public}sCounter %{public}s doesn't exists, please check Counter Name.",
                   groupIndent.c_str(), counterName.c_str());
        return Helper::NapiHelper::GetUndefinedValue(env);
    }
    counterMap.erase(counterName);
    return Helper::NapiHelper::GetUndefinedValue(env);
}

napi_value Console::Dir(napi_env env, napi_callback_info info)
{
    size_t argc = Helper::NapiHelper::GetCallbackInfoArgc(env, info);
    if (argc < 1) {
        HILOG_INFO("%{public}sundefined", groupIndent.c_str());
        return Helper::NapiHelper::GetUndefinedValue(env);
    }
    napi_value* argv = new napi_value[argc];
    Helper::ObjectScope<napi_value> scope(argv, true);
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    std::string ctorVal = Helper::NapiHelper::GetConstructorName(env, argv[0]);
    // JSON.stringify()
    napi_value globalValue = nullptr;
    napi_get_global(env, &globalValue);
    napi_value jsonValue;
    napi_get_named_property(env, globalValue, "JSON", &jsonValue);
    napi_value stringifyValue = nullptr;
    napi_get_named_property(env, jsonValue, "stringify", &stringifyValue);
    napi_value transValue = nullptr;
    napi_call_function(env, jsonValue, stringifyValue, 1, &argv[0], &transValue);
    if (transValue == nullptr) {
        Helper::ErrorHelper::ThrowError(env, Helper::ErrorHelper::TYPE_ERROR, "Dir content must not be null.");
        return Helper::NapiHelper::GetUndefinedValue(env);
    }
    std::string content = Helper::NapiHelper::GetPrintString(env, transValue);
    bool functionFlag = Helper::NapiHelper::IsFunction(env, argv[0]);
    if (!ctorVal.empty() && !functionFlag) {
        HILOG_INFO("%{public}s%{public}s: %{public}s", groupIndent.c_str(), ctorVal.c_str(), content.c_str());
    } else if (!ctorVal.empty() && functionFlag) {
        HILOG_INFO("%{public}s[Function %{public}s]", groupIndent.c_str(), ctorVal.c_str());
    } else {
        HILOG_INFO("%{public}s%{public}s", groupIndent.c_str(), content.c_str());
    }
    return Helper::NapiHelper::GetUndefinedValue(env);
}

napi_value Console::Group(napi_env env, napi_callback_info info)
{
    size_t argc = Helper::NapiHelper::GetCallbackInfoArgc(env, info);
    if (argc > 0) {
        ConsoleLog<LogLevel::INFO>(env, info);
    }
    groupIndent += StringRepeat(GROUPINDETATIONWIDTH, " ");
    return Helper::NapiHelper::GetUndefinedValue(env);
}

napi_value Console::GroupEnd(napi_env env, napi_callback_info info)
{
    size_t length = groupIndent.size();
    if (length > GROUPINDETATIONWIDTH) {
        groupIndent = groupIndent.substr(0, length - GROUPINDETATIONWIDTH);
    }
    return Helper::NapiHelper::GetUndefinedValue(env);
}

std::string Console::ArrayJoin(std::vector<std::string> rowDivider, const std::string& tableChars)
{
    size_t size = rowDivider.size();
    if (size == 0) {
        return "no rowDivider";
    }
    std::string result = rowDivider[0];
    for (size_t i = 1; i < size; i++) {
        result += tableChars;
        result += rowDivider[i];
    }
    return result;
}

std::string Console::RenderHead(napi_env env, napi_value head, std::vector<size_t> columnWidths)
{
    std::string result = tableChars["left"];
    size_t length = columnWidths.size();
    for (size_t i = 0; i < length; i++) {
        napi_value element = nullptr;
        napi_get_element(env, head, i, &element);
        napi_value string = nullptr;
        napi_status status = napi_coerce_to_string(env, element, &string);
        if (status != napi_ok) {
            Helper::ErrorHelper::ThrowError(env, Helper::ErrorHelper::TYPE_ERROR,
                                            "Table elements can't convert to string.");
            return "";
        }
        std::string elemStr = Helper::NapiHelper::GetPrintString(env, string);
        size_t stringLen = elemStr.size();
        size_t left = (columnWidths[i] - stringLen) / 2; // 2: half
        size_t right = columnWidths[i] - stringLen - left;
        result += StringRepeat(left, " ") + elemStr + StringRepeat(right, " ");
        if (i != length - 1) {
            result += tableChars["middle"];
        }
    }
    result += tableChars["right"];
    return result;
}

std::string Console::GetStringAndStringWidth(napi_env env, napi_value element, size_t& stringLen)
{
    napi_value string = nullptr;
    napi_status status = napi_coerce_to_string(env, element, &string);
    if (status != napi_ok) {
        Helper::ErrorHelper::ThrowError(env, Helper::ErrorHelper::TYPE_ERROR,
                                        "GetStringAndStringWidth: can't convert to string.");
        return "";
    }
    std::string result = Helper::NapiHelper::GetPrintString(env, string);
    napi_valuetype valuetype;
    napi_typeof(env, element, &valuetype);
    if (valuetype != napi_undefined) {
        stringLen = result.size();  // If element type is undefined, length is 0.
    }
    return result;
}

void Console::PrintRows(napi_env env, napi_value Rows, std::vector<size_t> columnWidths, size_t indexNum)
{
    size_t length = columnWidths.size();
    for (size_t i = 0; i < indexNum; i++) {
        std::string result = tableChars["left"];
        for (size_t j = 0; j < length; j++) {
            napi_value element = nullptr;
            napi_get_element(env, Rows, j * indexNum + i, &element);
            size_t stringLen = 0;
            std::string stringVal = GetStringAndStringWidth(env, element, stringLen);
            if (stringLen > 0) {
                size_t left = (columnWidths[j] - stringLen) / 2; // 2: half
                size_t right = columnWidths[j] - stringLen - left;
                result += StringRepeat(left, " ") + stringVal + StringRepeat(right, " ");
            } else {
                result += StringRepeat(columnWidths[j], " ");
            }
            if (j != length - 1) {
                result += tableChars["middle"];
            }
        }
        result += tableChars["right"];
        HILOG_INFO("%{public}s%{public}s", groupIndent.c_str(), result.c_str());
    }
}

void Console::GraphTable(napi_env env, napi_value head, napi_value columns, const size_t& length)
{
    uint32_t columnLen = 0;
    napi_get_array_length(env, head, &columnLen);
    std::vector<size_t> columnWidths(columnLen);
    std::vector<std::string> rowDivider(columnLen);
    // get maxColumnWidths and get rowDivider(------)
    // get key string length
    for (size_t i = 0; i < columnLen; i++) {
        napi_value element = nullptr;
        napi_get_element(env, head, i, &element);
        size_t stringLen = 0;
        GetStringAndStringWidth(env, element, stringLen);
        columnWidths[i] = stringLen;
    }
    // compare key/value string and get max length
    for (size_t i = 0; i < columnLen; i++) {
        for (size_t j = 0; j < length; j++) {
            napi_value element = nullptr;
            napi_get_element(env, columns, i * length + j, &element);
            size_t stringLen = 0;
            GetStringAndStringWidth(env, element, stringLen);
            columnWidths[i] = columnWidths[i] > stringLen ? columnWidths[i] :  stringLen;
        }
        rowDivider[i] = StringRepeat(columnWidths[i] + 2, tableChars["middleMiddle"]); // 2: two space
    }
    // print head row
    std::string indexRow1 = tableChars["topLeft"] +
                            ArrayJoin(rowDivider, tableChars["topMiddle"]) +
                            tableChars["topRight"];
    std::string indexRow2 = RenderHead(env, head, columnWidths);
    std::string indexRow3 = tableChars["leftMiddle"] +
                            ArrayJoin(rowDivider, tableChars["rowMiddle"]) +
                            tableChars["rightMiddle"];
    HILOG_INFO("%{public}s%{public}s", groupIndent.c_str(), indexRow1.c_str());
    HILOG_INFO("%{public}s%{public}s", groupIndent.c_str(), indexRow2.c_str());
    HILOG_INFO("%{public}s%{public}s", groupIndent.c_str(), indexRow3.c_str());
    // print value row
    PrintRows(env, columns, columnWidths, length);
    // print end row
    std::string endRow = tableChars["bottomLeft"] +
                         ArrayJoin(rowDivider, tableChars["bottomMiddle"]) +
                         tableChars["bottomRight"];
    HILOG_INFO("%{public}s%{public}s", groupIndent.c_str(), endRow.c_str());
}

napi_value GetKeyArray(napi_env env, napi_value map)
{
    napi_value mapKeys = nullptr;
    napi_object_get_keys(env, map, &mapKeys);
    uint32_t maplen = 0;
    napi_get_array_length(env, mapKeys, &maplen);

    size_t keyLength = maplen + 1;
    napi_value outputKeysArray = nullptr;
    napi_create_array_with_length(env, keyLength, &outputKeysArray);
    // set (index) to array
    napi_value result = nullptr;
    napi_create_string_utf8(env, "(index)", NAPI_AUTO_LENGTH, &result);
    napi_set_element(env, outputKeysArray, 0, result);

    // set Keys to array
    for (size_t j = 0; j < maplen ; ++j) {
        napi_value keyNumber = nullptr;
        napi_get_element(env, mapKeys, j, &keyNumber);
        napi_set_element(env, outputKeysArray, j + 1, keyNumber); // startkeyIdx = 1
    }
    return outputKeysArray;
}

napi_value GetValueArray(napi_env env, napi_value map, const size_t& length, napi_value keyArray)
{
    napi_value mapKeys = nullptr;
    napi_object_get_keys(env, map, &mapKeys);
    uint32_t maplen = 0;
    napi_get_array_length(env, mapKeys, &maplen);
    size_t keyLength = maplen + 1;

    size_t valueLength = keyLength * length;
    napi_value outputValuesArray = nullptr;
    napi_create_array_with_length(env, valueLength, &outputValuesArray);
    // set indexKeyValue
    size_t valueIdx = 0;
    for (size_t j = 0; j < length ; ++j) {
        napi_value keyNumber = nullptr;
        napi_get_element(env, keyArray, j, &keyNumber);
        napi_set_element(env, outputValuesArray, valueIdx++, keyNumber);
    }
    for (size_t i = 0; i < maplen ; ++i) {
        napi_value keyNumber = nullptr;
        napi_get_element(env, mapKeys, i, &keyNumber);
        char* innerKey = Helper::NapiHelper::GetString(env, keyNumber);
        if (innerKey == nullptr) {
            Helper::ErrorHelper::ThrowError(env, Helper::ErrorHelper::TYPE_ERROR,
                                            "property key must not be null.");
            return Helper::NapiHelper::GetUndefinedValue(env);
        }
        napi_value valueNumber = nullptr;
        napi_get_named_property(env, map, innerKey, &valueNumber);
        Helper::CloseHelp::DeletePointer(innerKey, true);
        for (size_t j = 0; j < length ; ++j) {
            napi_value value = nullptr;
            napi_get_element(env, valueNumber, j, &value);
            napi_set_element(env, outputValuesArray, valueIdx++, value);
        }
    }
    return outputValuesArray;
}

void SetPrimitive(napi_env env, napi_value map, const size_t& length, napi_value valuesKeyArray,
                  napi_value outputKeysArray, napi_value outputValuesArray)
{
    napi_value mapKeys = nullptr;
    napi_object_get_keys(env, map, &mapKeys);
    uint32_t maplen = 0;
    napi_get_array_length(env, mapKeys, &maplen);
    napi_value result = nullptr;
    napi_create_string_utf8(env, "Values", NAPI_AUTO_LENGTH, &result);
    napi_set_element(env, outputKeysArray, maplen + 1, result);
    uint32_t valuesLen = 0;
    napi_get_array_length(env, valuesKeyArray, &valuesLen);
    size_t startVal = (maplen + 1) * length;
    for (size_t j = 0; j < length ; ++j) {
        napi_value value = nullptr;
        napi_get_element(env, valuesKeyArray, j, &value);
        napi_set_element(env, outputValuesArray, startVal + j, value);
    }
}

napi_value Console::Table(napi_env env, napi_callback_info info)
{
    size_t argc = Helper::NapiHelper::GetCallbackInfoArgc(env, info);
    if (argc < 1) {
        return Helper::NapiHelper::GetUndefinedValue(env);
    }
    napi_value* argv = new napi_value[argc];
    Helper::ObjectScope<napi_value> scope(argv, true);
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (!Helper::NapiHelper::IsObject(env, argv[0])) {
        ConsoleLog<LogLevel::INFO>(env, info);
    }
    napi_value tabularData = argv[0];
    // map/set object is incomplete, needs add.
    
    napi_value keyArray = nullptr;
    napi_object_get_keys(env, tabularData, &keyArray);
    uint32_t length = 0;
    napi_get_array_length(env, keyArray, &length);
    napi_value valuesKeyArray = nullptr;
    napi_create_array_with_length(env, length, &valuesKeyArray);

    napi_value map = nullptr;
    napi_create_object(env, &map);
    bool hasPrimitive = false;
    bool primitiveInit = false;
    napi_value keys = nullptr;
    std::map<std::string, bool> initialMap;
    for (size_t i = 0; i < length; i++) {
        // get key
        napi_value napiNumber = nullptr;
        napi_get_element(env, keyArray, i, &napiNumber);
        char* key = Helper::NapiHelper::GetString(env, napiNumber);
        if (key == nullptr) {
            Helper::ErrorHelper::ThrowError(env, Helper::ErrorHelper::TYPE_ERROR,
                                            "property key must not be null.");
            return Helper::NapiHelper::GetUndefinedValue(env);
        }
        napi_value item = nullptr;
        napi_get_named_property(env, tabularData, key, &item);
        Helper::CloseHelp::DeletePointer(key, true);
        bool isPrimitive = ((item == nullptr) ||
                            (!Helper::NapiHelper::IsObject(env, item) && !Helper::NapiHelper::IsFunction(env, item)));
        if (isPrimitive) {
            if (!primitiveInit) {
                for (size_t j = 0; j < length ; ++j) {
                    napi_value result = nullptr;
                    napi_create_string_utf8(env, "", NAPI_AUTO_LENGTH, &result);
                    napi_set_element(env, valuesKeyArray, j, result);
                }
                primitiveInit = true;
            }
            hasPrimitive = true;
            napi_set_element(env, valuesKeyArray, i, item);
        } else {
            // get inner keys
            uint32_t innerLength = 0;
            napi_object_get_keys(env, item, &keys);
            napi_get_array_length(env, keys, &innerLength);
            // set value to array
            for (size_t j = 0; j < innerLength ; ++j) {
                napi_value keyNumber = nullptr;
                napi_get_element(env, keys, j, &keyNumber);
                char* innerKey = Helper::NapiHelper::GetString(env, keyNumber);
                if (innerKey == nullptr) {
                    Helper::ErrorHelper::ThrowError(env, Helper::ErrorHelper::TYPE_ERROR,
                                                    "property key must not be null.");
                    return Helper::NapiHelper::GetUndefinedValue(env);
                }
                napi_value innerItem = nullptr;
                if (isPrimitive) {
                    napi_value result = nullptr;
                    napi_create_string_utf8(env, "", NAPI_AUTO_LENGTH, &result);
                    innerItem = result;
                } else {
                    napi_get_named_property(env, item, innerKey, &innerItem);
                }
                // re-palce(sort) key, value.
                if (initialMap.find(std::string(innerKey)) == initialMap.end()) {
                    napi_value mapArray = nullptr;
                    napi_create_array_with_length(env, length, &mapArray);
                    napi_set_element(env, mapArray, i, innerItem);
                    napi_set_named_property(env, map, innerKey, mapArray);
                    initialMap[innerKey] = true;
                } else {
                    napi_value mapArray = nullptr;
                    napi_get_named_property(env, map, innerKey, &mapArray);
                    napi_set_element(env, mapArray, i, innerItem);
                    napi_set_named_property(env, map, innerKey, mapArray);
                }
                Helper::CloseHelp::DeletePointer(innerKey, true);
            }
        }
    }
    // set outputKeysArray
    napi_value outputKeysArray = GetKeyArray(env, map);
    // set outputValuesArray
    napi_value outputValuesArray = GetValueArray(env, map, length, keyArray);
    // if has Primitive, add new colomn Values
    if (hasPrimitive) {
        SetPrimitive(env, map, length, valuesKeyArray, outputKeysArray, outputValuesArray);
    }
    GraphTable(env, outputKeysArray, outputValuesArray, length);
    return Helper::NapiHelper::GetUndefinedValue(env);
}

void Console::PrintTime(std::string timerName, double time, const std::string& log)
{
    uint32_t hours = 0;
    uint32_t minutes = 0;
    uint32_t seconds = 0;
    uint32_t ms = time;
    if (ms >= SECOND) {
        if (ms >= MINUTE) {
            if (ms >= HOUR) {
                hours = ms / HOUR;
                ms = ms - HOUR * hours;
            }
            minutes = ms / MINUTE;
            ms = ms - MINUTE * minutes;
        }
        seconds = ms / SECOND;
        ms = ms - SECOND * seconds;
    }
    if (hours != 0) {
        HILOG_INFO("%{public}s%{public}s: %{public}d:%{public}.2d:%{public}.2d.%{public}.3d(h:m:s.mm) %{public}s",
            groupIndent.c_str(), timerName.c_str(), hours, minutes, seconds, ms, log.c_str());
        return;
    }
    if (minutes != 0) {
        HILOG_INFO("%{public}s%{public}s: %{public}d:%{public}.2d.%{public}.3d(m:s.mm) %{public}s",
            groupIndent.c_str(), timerName.c_str(), minutes, seconds, ms, log.c_str());
        return;
    }
    if (seconds != 0) {
        HILOG_INFO("%{public}s%{public}s: %{public}d.%{public}.3ds %{public}s",
            groupIndent.c_str(), timerName.c_str(), seconds, ms, log.c_str());
        return;
    }
    HILOG_INFO("%{public}s%{public}s: %{public}.3fms %{public}s",
        groupIndent.c_str(), timerName.c_str(), time, log.c_str());
}

napi_value Console::Time(napi_env env, napi_callback_info info)
{
    size_t argc = Helper::NapiHelper::GetCallbackInfoArgc(env, info);
    std::string timerName = GetTimerOrCounterName(env, info, argc);
    if (timerMap.find(timerName) == timerMap.end()) {
        timerMap[timerName] = std::chrono::duration_cast<std::chrono::microseconds>
                              (std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    } else {
        HILOG_WARN("Timer %{public}s already exists,  please check Timer Name", timerName.c_str());
    }
    return Helper::NapiHelper::GetUndefinedValue(env);
}

napi_value Console::TimeLog(napi_env env, napi_callback_info info)
{
    size_t argc = Helper::NapiHelper::GetCallbackInfoArgc(env, info);
    std::string timerName = GetTimerOrCounterName(env, info, argc);
    if (timerMap.find(timerName) != timerMap.end()) {
        // get time in ms
        int64_t endTime = std::chrono::duration_cast<std::chrono::microseconds>
                          (std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        double ms = static_cast<uint64_t>(endTime - timerMap[timerName]) / 1000.0;
        std::string content = MakeLogContent(env, info, argc, 1, false); // startInx = 1, format = false;
        PrintTime(timerName, ms, content);
    } else {
        HILOG_WARN("%{public}sTimer %{public}s doesn't exists, please check Timer Name.",
                   groupIndent.c_str(), timerName.c_str());
    }
    return Helper::NapiHelper::GetUndefinedValue(env);
}

napi_value Console::TimeEnd(napi_env env, napi_callback_info info)
{
    size_t argc = Helper::NapiHelper::GetCallbackInfoArgc(env, info);
    std::string timerName = GetTimerOrCounterName(env, info, argc);
    if (timerMap.find(timerName) != timerMap.end()) {
        // get time in ms
        int64_t endTime = std::chrono::duration_cast<std::chrono::microseconds>
                          (std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        double ms = static_cast<uint64_t>(endTime - timerMap[timerName]) / 1000.0;
        PrintTime(timerName, ms, "");
        timerMap.erase(timerName);
    } else {
        HILOG_WARN("%{public}sTimer %{public}s doesn't exists, please check Timer Name.",
                   groupIndent.c_str(), timerName.c_str());
    }
    return Helper::NapiHelper::GetUndefinedValue(env);
}

napi_value Console::Trace(napi_env env, napi_callback_info info)
{
    size_t argc = Helper::NapiHelper::GetCallbackInfoArgc(env, info);
    std::string content;
    if (argc > 0) {
        content = MakeLogContent(env, info, argc, 0); // startInx = 0
    }
    HILOG_INFO("%{public}sTrace: %{public}s", groupIndent.c_str(), content.c_str());
    std::string stack;
    napi_get_stack_trace(env, stack);
    std::string tempStr = "";
    for (size_t i = 0; i < stack.length(); i++) {
        if (stack[i] == '\n') {
            HILOG_INFO("%{public}s%{public}s", groupIndent.c_str(), tempStr.c_str());
            tempStr = "";
        } else {
            tempStr += stack[i];
        }
    }
    return Helper::NapiHelper::GetUndefinedValue(env);
}

napi_value Console::TraceHybridStack(napi_env env, napi_callback_info info)
{
    std::string stack;
    napi_get_hybrid_stack_trace(env, stack);
    HILOG_INFO("%{public}sTraceHybridStack: ", groupIndent.c_str());
    std::string tempStr = "";
    for (size_t i = 0; i < stack.length(); i++) {
        if (stack[i] == '\n') {
            HILOG_INFO("%{public}s%{public}s", groupIndent.c_str(), tempStr.c_str());
            tempStr = "";
        } else {
            tempStr += stack[i];
        }
    }
    return Helper::NapiHelper::GetUndefinedValue(env);
}

napi_value Console::Assert(napi_env env, napi_callback_info info)
{
    // 1. check args
    size_t argc = Helper::NapiHelper::GetCallbackInfoArgc(env, info);
    if (argc < 1) {
        HILOG_ERROR("%{public}sAssertion failed", groupIndent.c_str());
        return Helper::NapiHelper::GetUndefinedValue(env);
    }
    napi_value* argv = new napi_value[argc];
    Helper::ObjectScope<napi_value> scope(argv, true);
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);

    if (Helper::NapiHelper::GetBooleanValue(env, argv[0])) {
        return Helper::NapiHelper::GetUndefinedValue(env);
    }
    std::string content = "Assertion failed";
    if (argc > 1) {
        content += ":";
        content += MakeLogContent(env, info, argc, 1); // startIndex = 1
    }
    HILOG_ERROR("%{public}s%{public}s", groupIndent.c_str(), content.c_str());
    return Helper::NapiHelper::GetUndefinedValue(env);
}

void Console::InitConsoleModule(napi_env env)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_DEFAULT_PROPERTY_FUNCTION("log", ConsoleLog<LogLevel::INFO>),
        DECLARE_NAPI_DEFAULT_PROPERTY_FUNCTION("debug", ConsoleLog<LogLevel::DEBUG>),
        DECLARE_NAPI_DEFAULT_PROPERTY_FUNCTION("info", ConsoleLog<LogLevel::INFO>),
        DECLARE_NAPI_DEFAULT_PROPERTY_FUNCTION("warn", ConsoleLog<LogLevel::WARN>),
        DECLARE_NAPI_DEFAULT_PROPERTY_FUNCTION("error", ConsoleLog<LogLevel::ERROR>),
        DECLARE_NAPI_DEFAULT_PROPERTY_FUNCTION("fatal", ConsoleLog<LogLevel::FATAL>),
        DECLARE_NAPI_DEFAULT_PROPERTY_FUNCTION("group", Group),
        DECLARE_NAPI_DEFAULT_PROPERTY_FUNCTION("groupCollapsed", Group),
        DECLARE_NAPI_DEFAULT_PROPERTY_FUNCTION("groupEnd", GroupEnd),
        DECLARE_NAPI_DEFAULT_PROPERTY_FUNCTION("table", Table),
        DECLARE_NAPI_DEFAULT_PROPERTY_FUNCTION("time", Time),
        DECLARE_NAPI_DEFAULT_PROPERTY_FUNCTION("timeLog", TimeLog),
        DECLARE_NAPI_DEFAULT_PROPERTY_FUNCTION("timeEnd", TimeEnd),
        DECLARE_NAPI_DEFAULT_PROPERTY_FUNCTION("trace", Trace),
        DECLARE_NAPI_DEFAULT_PROPERTY_FUNCTION("traceHybridStack", TraceHybridStack),
        DECLARE_NAPI_DEFAULT_PROPERTY_FUNCTION("assert", Assert),
        DECLARE_NAPI_DEFAULT_PROPERTY_FUNCTION("count", Count),
        DECLARE_NAPI_DEFAULT_PROPERTY_FUNCTION("countReset", CountReset),
        DECLARE_NAPI_DEFAULT_PROPERTY_FUNCTION("dir", Dir),
        DECLARE_NAPI_DEFAULT_PROPERTY_FUNCTION("dirxml", ConsoleLog<LogLevel::INFO>)
    };
    napi_value globalObj = Helper::NapiHelper::GetGlobalObject(env);
    napi_value console = nullptr;
    napi_create_object(env, &console);
    napi_define_properties(env, console, sizeof(properties) / sizeof(properties[0]), properties);
    napi_set_named_property(env, globalObj, "console", console);
}
} // namespace Commonlibrary::JsSysModule