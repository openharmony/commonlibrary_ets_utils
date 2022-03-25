/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef PROCESS_JS_PROCESS_H_
#define PROCESS_JS_PROCESS_H_

#include <cstring>
#include <map>
#include <sys/time.h>

#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS::Js_sys_module::Process {
    using ClearRefCallback = void (*)(napi_env env);
    enum class PromiseRejectionEvent : uint32_t { REJECT = 0, HANDLE };
    class Process {
    public:
        /**
         * Create process object
         *
         * @param env NAPI environment parameters.
         */
        explicit Process(napi_env env);

        /**
         * Process destructor.
         */
        virtual ~Process() {}

        /**
         * Get process uid.
         */
        napi_value GetUid() const;

        /**
         * Get the user ID of the process.
         */
        napi_value GetGid() const;

        /**
         * Get the effective user identity of the process.
         */
        napi_value GetEUid() const;

        /**
         * Get the effective group ID of the process.
         */
        napi_value GetEGid() const;

        /**
         * Get an array with supplementary group ids.
         */
        napi_value GetGroups() const;

        /**
         * Get the pid of the current process.
         */
        napi_value GetPid() const;

        /**
         * Get the pid of the parent process of the current process.
         */
        napi_value GetPpid() const;

        /**
         * Change the current working directory of the process.
         *
         * @param args The parameter is the path.
         */
        void Chdir(napi_value args) const;

        /**
         * Get the number of seconds the current system has been running.
         */
        napi_value Uptime() const;

        /**
         * Send a signal to the specified process and end the specified process.
         *
         * @param signal The parameter is the signal sent.
         * @param proid The parameter is the id of the process.
         */
        napi_value Kill(napi_value signal, napi_value proid);

        /**
         * Causes the process to exit immediately and generate a core file.
         */
        void Abort() const;

        /**
         * Store user-triggered events.
         *
         * @param str The parameter is type of storage event.
         * @param function The parameter is callback event.
         */
        void On(napi_value str, napi_value function);

        /**
         * Delete user-stored events.
         *
         * @param str The parameter is the type of delete event.
         */
        napi_value Off(napi_value str);

        /**
         * Terminate the program.
         *
         * @param number The parameter is the exit code of the process.
         */
        void Exit(napi_value number) const;

        /**
         * Use this method to get the working directory of the process.
         */
        napi_value Cwd() const;

        /**
         * Get the tid of the current process.
         */
        napi_value GetTid() const;

        /**
         * Determines whether the process is isolated.
         */
        napi_value IsIsolatedProcess() const;

        /**
         * Determine whether the uid belongs to the application.
         *
         * @param uid The parameter is the uid of the application.
         */
        napi_value IsAppUid(napi_value uid) const;

        /**
         * Determine whether the operating environment is 64-bit.
         */
        napi_value Is64Bit() const;

        /**
         * Get process uid by process name.
         *
         * @param name The parameter is the process name.
         */
        napi_value GetUidForName(napi_value name) const;

        /**
         * Get thread priority based on specified tid.
         *
         * @param tid The parameter is the specified thread tid.
         */
        napi_value GetThreadPriority(napi_value tid) const;

        /**
         * Get the real-time elapsed time from system startup to process startup.
         */
        napi_value GetStartRealtime() const;

        /**
         * Get the CPU time from the process startup to the current time.
         */
        napi_value GetPastCputime() const;

        /**
         * Get system configuration information.
         *
         * @param name The parameter is the name of the specified system configuration parameter.
         */
        napi_value GetSystemConfig(napi_value name) const;

        /**
         * Use this method to get the value corresponding to the environment variable.
         *
         * @param name The parameter is the environment variable name.
         */
        napi_value GetEnvironmentVar(napi_value name) const;

        /**
         * Set reject callback.
         */
        napi_value SetRejectionCallback() const;

        /**
         * Clear references to callbacks.
         *
         * @param env The parameter is NAPI environment variables.
         */
        static void ClearReference(napi_env env);

    private:
        int ConvertTime(time_t tvsec, int64_t tvnsec) const;

    private:
        napi_env env_ { nullptr };
        int FIRST_APPLICATION_UID = 10000;
        int LAST_APPLICATION_UID = 19999;
    };
} // namespace OHOS::Js_sys_module::Process
#endif // PROCESS_JS_PROCESS_H_