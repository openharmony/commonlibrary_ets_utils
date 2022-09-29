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
         */
        explicit Process() {}

        /**
         * Process destructor.
         */
        virtual ~Process() {}

        /**
         * Get process uid.
         *
         * @param env The parameter is NAPI environment variables.
         */
        napi_value GetUid(napi_env env) const;

        /**
         * Get the user ID of the process.
         *
         * @param env The parameter is NAPI environment variables.
         */
        napi_value GetGid(napi_env env) const;

        /**
         * Get the effective user identity of the process.
         *
         * @param env The parameter is NAPI environment variables.
         */
        napi_value GetEUid(napi_env env) const;

        /**
         * Get the effective group ID of the process.
         *
         * @param env The parameter is NAPI environment variables.
         */
        napi_value GetEGid(napi_env env) const;

        /**
         * Get an array with supplementary group ids.
         *
         * @param env The parameter is NAPI environment variables.s
         */
        napi_value GetGroups(napi_env env) const;

        /**
         * Get the pid of the current process.
         *
         * @param env The parameter is NAPI environment variables.
         */
        napi_value GetPid(napi_env env) const;

        /**
         * Get the pid of the parent process of the current process.
         *
         * @param env The parameter is NAPI environment variables.
         */
        napi_value GetPpid(napi_env env) const;

        /**
         * Change the current working directory of the process.
         *
         * @param env The parameter is NAPI environment variables.
         * @param args The parameter is the path.
         */
        void Chdir(napi_env env, napi_value args) const;

        /**
         * Get the number of seconds the current system has been running.
         *
         * @param env The parameter is NAPI environment variables.
         */
        napi_value Uptime(napi_env env) const;

        /**
         * Send a signal to the specified process and end the specified process.
         *
         * @param env The parameter is NAPI environment variables.
         * @param signal The parameter is the signal sent.
         * @param proid The parameter is the id of the process.
         */
        napi_value Kill(napi_env env, napi_value signal, napi_value proid);

        /**
         * Causes the process to exit immediately and generate a core file.
         */
        void Abort() const;

        /**
         * Store user-triggered events.
         *
         * @param env The parameter is NAPI environment variables.
         * @param str The parameter is type of storage event.
         * @param function The parameter is callback event.
         */
        void On(napi_env env, napi_value str, napi_value function);

        /**
         * Delete user-stored events.
         *
         * @param env The parameter is NAPI environment variables.
         * @param str The parameter is the type of delete event.
         */
        napi_value Off(napi_env env, napi_value str);

        /**
         * Terminate the program.
         *
         * @param env The parameter is NAPI environment variables.
         * @param number The parameter is the exit code of the process.
         */
        void Exit(napi_env env, napi_value number) const;

        /**
         * Use this method to get the working directory of the process.
         *
         * @param env The parameter is NAPI environment variables.
         */
        napi_value Cwd(napi_env env) const;

        /**
         * Get the tid of the current process.
         *
         * @param env The parameter is NAPI environment variables.
         */
        napi_value GetTid(napi_env env) const;

        /**
         * Determines whether the process is isolated.
         *
         * @param env The parameter is NAPI environment variables.
         */
        napi_value IsIsolatedProcess(napi_env env) const;

        /**
         * Determine whether the uid belongs to the application.
         *
         * @param env The parameter is NAPI environment variables.
         * @param uid The parameter is the uid of the application.
         */
        napi_value IsAppUid(napi_env env, napi_value uid) const;

        /**
         * Determine whether the operating environment is 64-bit.
         *
         * @param env The parameter is NAPI environment variables.
         */
        napi_value Is64Bit(napi_env env) const;

        /**
         * Get process uid by process name.
         *
         * @param env The parameter is NAPI environment variables.
         * @param name The parameter is the process name.
         */
        napi_value GetUidForName(napi_env env, napi_value name) const;

        /**
         * Get thread priority based on specified tid.
         *
         * @param env The parameter is NAPI environment variables.
         * @param tid The parameter is the specified thread tid.
         */
        napi_value GetThreadPriority(napi_env env, napi_value tid) const;

        /**
         * Get the real-time elapsed time from system startup to process startup.
         */
        napi_value GetStartRealtime(napi_env env) const;

        /**
         * Get the CPU time from the process startup to the current time.
         *
         * @param env The parameter is NAPI environment variables.
         */
        napi_value GetPastCputime(napi_env env) const;

        /**
         * Get system configuration information.
         *
         * @param env The parameter is NAPI environment variables.
         * @param name The parameter is the name of the specified system configuration parameter.
         */
        napi_value GetSystemConfig(napi_env env, napi_value name) const;

        /**
         * Use this method to get the value corresponding to the environment variable.
         *
         * @param env The parameter is NAPI environment variables.
         * @param name The parameter is the environment variable name.
         */
        napi_value GetEnvironmentVar(napi_env env, napi_value name) const;

        /**
         * Set reject callback.
         *
         * @param env The parameter is NAPI environment variables.
         */
        napi_value SetRejectionCallback(napi_env env) const;

        /**
         * Clear references to callbacks.
         *
         * @param env The parameter is NAPI environment variables.
         */
        static void ClearReference(napi_env env);

    private:
        int ConvertTime(time_t tvsec, int64_t tvnsec) const;

    private:
        int FIRST_APPLICATION_UID = 10000;
        int LAST_APPLICATION_UID = 19999;
    };
    class ProcessManager {
    public:
        /**
         * Create process object.
         */
        explicit ProcessManager() {}

        /**
         * Process destructor.
         */
        virtual ~ProcessManager() {}

        /**
         * Determine whether the uid belongs to the application.
         *
         * @param env The parameter is NAPI environment variables.
         * @param uid The parameter is the uid of the application.
         */
        napi_value IsAppUid(napi_env env, napi_value uid) const;

        /**
         * Get process uid by process name.
         *
         * @param env The parameter is NAPI environment variables.
         * @param name The parameter is the process name.
         */
        napi_value GetUidForName(napi_env env, napi_value name) const;

        /**
         * Get thread priority based on specified tid.
         *
         * @param env The parameter is NAPI environment variables.
         * @param tid The parameter is the specified thread tid.
         */
        napi_value GetThreadPriority(napi_env env, napi_value tid) const;

        /**
         * Get system configuration information.
         *
         * @param env The parameter is NAPI environment variables.
         * @param name The parameter is the name of the specified system configuration parameter.
         */
        napi_value GetSystemConfig(napi_env env, napi_value name) const;

        /**
         * Use this method to get the value corresponding to the environment variable.
         *
         * @param env The parameter is NAPI environment variables.
         * @param name The parameter is the environment variable name.
         */
        napi_value GetEnvironmentVar(napi_env env, napi_value name) const;

        /**
         * Terminate the program.
         *
         * @param env The parameter is NAPI environment variables.
         * @param number The parameter is the exit code of the process.
         */
        void Exit(napi_env env, napi_value number) const;

        /**
         * Send a signal to the specified process and end the specified process.
         *
         * @param env The parameter is NAPI environment variables.
         * @param signal The parameter is the signal sent.
         * @param proid The parameter is the id of the process.
         */
        napi_value Kill(napi_env env, napi_value signal, napi_value proid);
    private:
        int FIRST_APPLICATION_UID = 10000;
        int LAST_APPLICATION_UID = 19999;
    };
} // namespace OHOS::Js_sys_module::Process
#endif // PROCESS_JS_PROCESS_H_