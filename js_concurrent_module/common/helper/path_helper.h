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

#ifndef JS_CONCURRENT_MODULE_COMMON_HELPER_PATH_HELPER_H
#define JS_CONCURRENT_MODULE_COMMON_HELPER_PATH_HELPER_H

#include <vector>
#include <string>

#include "native_engine/native_engine.h"
#include "tools/log.h"

namespace Commonlibrary::Concurrent::Common::Helper {
class PathHelper {
public:
    static constexpr char PREFIX_BUNDLE[] = "@bundle:";
    static constexpr char DOUBLE_POINT_TAG[] = "..";
    static constexpr char NAME_SPACE_TAG = '@';
    static constexpr char POINT_TAG[] = ".";
    static constexpr char SLASH_TAG = '/';
    static constexpr char PAG_TAG[] = "pkg_modules";
    static constexpr char EXT_NAME_ETS[] = ".ets";
    static constexpr char EXT_NAME_TS[] = ".ts";
    static constexpr char EXT_NAME_JS[] = ".js";
    static constexpr char NORMALIZED_OHMURL_TAG = '&';
    static constexpr size_t NORMALIZED_OHMURL_ARGS_NUM = 5;

    static bool CheckWorkerPath(napi_env env, std::string script, bool isHar, bool isRelativePath)
    {
        std::string ohmurl = "";
        std::string moduleName = "";
        std::string bundleName = "";
        if (script.find(PAG_TAG) == 0 || script.find(NAME_SPACE_TAG) != std::string::npos) {
            HILOG_INFO("worker:: the HAR path cannot be verified");
            return true;
        }
        size_t prev = script.find_first_of(SLASH_TAG);
        while (prev == 0 && script != "") {
            script = script.substr(1);
            prev = script.find_first_of(SLASH_TAG);
        }
        if (isRelativePath) {
            bundleName = script.substr(0, prev);
            std::string temp = script.substr(prev + 1);
            prev = temp.find_first_of(SLASH_TAG);
            moduleName = temp.substr(0, prev);
            ohmurl = PREFIX_BUNDLE + script;
        } else {
            moduleName = script.substr(0, prev);
            bundleName = reinterpret_cast<NativeEngine*>(env)->GetBundleName();
            prev = script.find_last_of(POINT_TAG);
            script = script.substr(0, prev);
            ohmurl = PREFIX_BUNDLE + bundleName + SLASH_TAG + script;
        }
        return reinterpret_cast<NativeEngine*>(env)->IsExecuteModuleInAbcFile(bundleName, moduleName, ohmurl);
    }

    static void ConcatFileNameForWorker(napi_env env, std::string &script, std::string &fileName, bool &isRelativePath)
    {
        std::string moduleName;
        if (script.find_first_of(POINT_TAG) == 0) {
            isRelativePath = true;
        }
        reinterpret_cast<NativeEngine*>(env)->GetCurrentModuleInfo(moduleName, fileName, isRelativePath);
        if (isRelativePath) {
            // if input is relative path, need to concat new recordName.
            std::string recordName = moduleName;
            size_t pos = moduleName.rfind(SLASH_TAG);
            if (pos != std::string::npos) {
                moduleName = moduleName.substr(0, pos + 1); // from spcific file to dir
            }
            script = moduleName + script;
            script = NormalizePath(script); // remove ../ and .ets

            if (recordName.at(0) == NORMALIZED_OHMURL_TAG) {
                script.append(1, NORMALIZED_OHMURL_TAG);
                std::vector<std::string> normalizedRes = SplitNormalizedRecordName(recordName);
                script += normalizedRes[NORMALIZED_OHMURL_ARGS_NUM - 1];
            }
        } else {
            script = moduleName + script;
        }
    }

    static std::string NormalizePath(const std::string &entryPoint)
    {
        std::string res;
        size_t prev = 0;
        size_t curr = entryPoint.find(SLASH_TAG);
        std::vector<std::string> elems;
        // eliminate parent directory path
        while (curr != std::string::npos) {
            if (curr > prev) {
                std::string elem = entryPoint.substr(prev, curr - prev);
                if (elem == DOUBLE_POINT_TAG && entryPoint.at(curr) == SLASH_TAG
                    && !elems.empty()) { // looking for xxx/../
                    elems.pop_back();
                } else if (elem != POINT_TAG && elem != DOUBLE_POINT_TAG) { // remove ./
                    elems.push_back(elem);
                }
            }
            prev = curr + 1;
            curr = entryPoint.find(SLASH_TAG, prev);
        }
        if (prev != entryPoint.size()) {
            elems.push_back(entryPoint.substr(prev));
        }
        for (auto e : elems) {
            if (res.size() == 0 && entryPoint.at(0) != SLASH_TAG) {
                res.append(e);
                continue;
            }
            res.append(1, SLASH_TAG).append(e);
        }
        // remore suffix
        size_t pos = res.rfind(POINT_TAG);
        if (pos != std::string::npos) {
            std::string suffix = res.substr(pos);
            if (suffix == EXT_NAME_ETS || suffix == EXT_NAME_TS || suffix == EXT_NAME_JS) {
                res.erase(pos, suffix.length());
            }
        }
        return res;
    }

    /*
    *  Split the recordName of the new ohmurl rule
    *  recordName: &moduleName&bundleName&importPath&version
    */
    static std::vector<std::string> SplitNormalizedRecordName(const std::string &recordName)
    {
        std::vector<std::string> res(NORMALIZED_OHMURL_ARGS_NUM);
        int index = NORMALIZED_OHMURL_ARGS_NUM - 1;
        std::string temp;
        int endIndex = recordName.size() - 1;
        for (int i = endIndex; i >= 0; i--) {
            char element = recordName[i];
            if (element == NORMALIZED_OHMURL_TAG) {
                res[index] = temp;
                index--;
                temp = "";
                continue;
            }
            temp = element + temp;
        }
        if (temp.size()) {
            res[index] = temp;
        }
        return res;
    }
};
} // namespace Commonlibrary::Concurrent::Common::Helper
#endif // JS_CONCURRENT_MODULE_COMMON_HELPER_OBJECT_HELPER_H
