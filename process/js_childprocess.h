/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef BASE_COMPILERUNTIME_JS_SYS_MODULE_CHILD_PROCESS_CLASS_H
#define BASE_COMPILERUNTIME_JS_SYS_MODULE_CHILD_PROCESS_CLASS_H

#include <string>
#include <sys/types.h>

#include "napi/native_api.h"
#include "napi/native_node_api.h"
namespace OHOS::Js_sys_module::Process {
    struct WaitInfo {
        napi_async_work worker = nullptr;
        napi_deferred deferred = nullptr;
        int status = 0;
    };

    struct StdInfo {
        napi_async_work worker = nullptr;
        napi_deferred deferred = nullptr;
        napi_value promise = nullptr;
        std::string stdData = "";
        bool *isNeedRun = nullptr;
        size_t maxBuffSize;
        int fd = 0;
        int pid = 0;
    };

    struct OptionsInfo {
        napi_async_work worker = nullptr;
        bool *isNeedRun = nullptr;
        int32_t timeout = 0;
        int32_t killSignal = 0;
        int64_t maxBuffer = 0;
        pid_t pid = 0;
    };

    class ChildProcess {
    public:
        explicit ChildProcess(napi_env env);
        void Close();
        void Kill(const napi_value signo);
        napi_value Wait();
        napi_value GetOutput() const;
        napi_value GetErrorOutput() const;
        napi_value GetKilled() const;
        napi_value Getpid() const;
        napi_value Getppid() const;
        napi_value GetExitCode() const;
        void InitOptionsInfo(napi_value options);
        void Spawn(napi_value command);
        virtual ~ChildProcess();

    private:
        static void ReadStdOut(napi_env env, void* data);
        static void EndStdOut(napi_env env, napi_status status, void* buffer);
        static void ReadStdErr(napi_env env, void* data);
        static void EndStdErr(napi_env env, napi_status status, void* buffer);
        static void TimeoutListener(napi_env env, void* data);
        std::string RequireStrValue(const napi_value strValue);
        int GetValidSignal(const napi_value signo);
        void CreateWorker();

        napi_env env_ = nullptr;
        OptionsInfo* optionsInfo_ = nullptr;
        StdInfo* stdOutInfo_ = nullptr;
        StdInfo* stdErrInfo_ = nullptr;

        int exitCode_ = 0;
        int stdOutFd_[2] = {0};
        int stdErrFd_[2] = {0};
        int ppid_ = 0;

        bool isNeedRun_ = true;
        bool killed_ = false;
        bool isWait_ = true;
    };
} // namespace
#endif