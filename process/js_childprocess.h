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

#ifndef PROCESS_JS_CHILDPROCESS_H_
#define PROCESS_JS_CHILDPROCESS_H_

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
        int64_t maxBuffSize;
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
        /**
         * Create child process object.
         */
        explicit ChildProcess() {}

        /**
         * Close the target process.
         */
        void Close();

        /**
         * Send a signal to process.
         *
         * @param env NAPI environment parameters.
         * @param signal Number or string represents the signal sent.
         */
        void Kill(napi_env env, const napi_value signo);

        /**
         * Wait for the child process to finish running, and return a promise object
         * whose value is the exit code of the child process.
         *
         * @param env NAPI environment parameters.
         */
        napi_value Wait(napi_env env);

        /**
         * Get the standard output of the child process.
         *
         * @param env NAPI environment parameters.
         */
        napi_value GetOutput(napi_env env) const;

        /**
         * Get the standard error output of the child process.
         *
         * @param env NAPI environment parameters.
         */
        napi_value GetErrorOutput(napi_env env) const;

        /**
         * Get kill status.
         *
         * @param env NAPI environment parameters.
         */
        napi_value GetKilled(napi_env env) const;

        /**
         * Get the specific pid value.
         *
         * @param env NAPI environment parameters.
         */
        napi_value Getpid(napi_env env) const;

        /**
         * Get the parent process ID.
         *
         * @param env NAPI environment parameters.
         */
        napi_value Getppid(napi_env env) const;

        /**
         * Get exit status.
         *
         * @param env NAPI environment parameters.
         */
        napi_value GetExitCode(napi_env env) const;

        /**
         * Initialization option information.
         *
         * @param env NAPI environment parameters.
         * @param options Option parameter.
         */
        void InitOptionsInfo(napi_env env, napi_value options);

        /**
         * Start a subprocess to execute shell commands.
         *
         * @param env NAPI environment parameters.
         * @param command Command parameters.
         */
        void Spawn(napi_env env, napi_value command);

        /**
         * ChildProcess destructor.
         */
        virtual ~ChildProcess();

    private:
        static void ReadStdOut(napi_env env, void* data);
        static void EndStdOut(napi_env env, napi_status status, void* buffer);
        static void ReadStdErr(napi_env env, void* data);
        static void EndStdErr(napi_env env, napi_status status, void* buffer);
        static void TimeoutListener(napi_env env, void* data);
        std::string RequireStrValue(napi_env env, const napi_value strValue);
        int GetValidSignal(napi_env env, const napi_value signo);
        void CreateWorker(napi_env env);

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
} // namespace OHOS::Js_sys_module::Process
#endif // PROCESS_JS_CHILDPROCESS_H_
