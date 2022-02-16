/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef BASE_COMPILERUNTIME_JS_SYS_MODULE_PROCESS_CLASS_H
#define BASE_COMPILERUNTIME_JS_SYS_MODULE_PROCESS_CLASS_H

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
        explicit Process(napi_env env);
        virtual ~Process() {}
        napi_value GetUid() const;
        napi_value GetGid() const;
        napi_value GetEUid() const;
        napi_value GetEGid() const;
        napi_value GetGroups() const;
        napi_value GetPid() const;
        napi_value GetPpid() const;
        void Chdir(napi_value args) const;
        napi_value Uptime() const;
        napi_value Kill(napi_value proid, napi_value signal);
        void Abort() const;
        void On(napi_value str, napi_value function);
        napi_value Off(napi_value str);
        void Exit(napi_value number) const;
        napi_value Cwd() const;
        napi_value GetTid() const;
        napi_value IsIsolatedProcess() const;
        napi_value IsAppUid(napi_value uid) const;
        napi_value Is64Bit() const;
        napi_value GetUidForName(napi_value name) const;
        napi_value GetThreadPriority(napi_value tid) const;
        napi_value GetStartRealtime() const;
        napi_value GetPastCputime() const;
        napi_value GetSystemConfig(napi_value name) const;
        napi_value GetEnvironmentVar(napi_value name) const;
        napi_value SetRejectionCallback() const;

        static void ClearReference(napi_env env);
    private:
        int ConvertTime(time_t tvsec, long tvnsec) const;
    private:
        napi_env env_ { nullptr };
        int FIRST_APPLICATION_UID = 10000;
        int LAST_APPLICATION_UID = 19999;
    };
}
#endif
