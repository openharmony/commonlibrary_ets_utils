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

#include "js_process.h"

#include <cstdlib>
#include <vector>

#include <grp.h>
#include <pthread.h>
#include <pwd.h>
#include <sched.h>
#include <unistd.h>
#include <uv.h>

#include <sys/resource.h>
#include <sys/syscall.h>
#include <sys/sysinfo.h>
#include <sys/types.h>

#include "securec.h"
#include "utils/log.h"
namespace OHOS::Js_sys_module::Process {
    namespace {
        constexpr int NUM_OF_DATA = 4;
        constexpr int PER_USER_RANGE = 100000;
        constexpr int32_t NAPI_RETURN_ZERO = 0;
        constexpr int32_t NAPI_RETURN_ONE = 1;
    }
    thread_local std::multimap<std::string, napi_ref> eventMap;
    thread_local std::map<napi_ref, napi_ref> pendingUnHandledRejections;
    // support events
    thread_local std::string events = "UnHandleRejection";

    napi_value Process::GetUid(napi_env env) const
    {
        napi_value result = nullptr;
        auto processGetuid = static_cast<uint32_t>(getuid());
        NAPI_CALL(env, napi_create_uint32(env, processGetuid, &result));
        return result;
    }

    napi_value Process::GetGid(napi_env env) const
    {
        napi_value result = nullptr;
        auto processGetgid = static_cast<uint32_t>(getgid());
        NAPI_CALL(env, napi_create_uint32(env, processGetgid, &result));
        return result;
    }

    napi_value Process::GetEUid(napi_env env) const
    {
        napi_value result = nullptr;
        auto processGeteuid = static_cast<uint32_t>(geteuid());
        NAPI_CALL(env, napi_create_uint32(env, processGeteuid, &result));
        return result;
    }

    napi_value Process::GetEGid(napi_env env) const
    {
        napi_value result = nullptr;
        auto processGetegid = static_cast<uint32_t>(getegid());
        NAPI_CALL(env, napi_create_uint32(env, processGetegid, &result));
        return result;
    }

    napi_value Process::GetGroups(napi_env env) const
    {
        napi_value result = nullptr;
        int progroups = getgroups(0, nullptr);
        if (progroups == -1) {
            napi_throw_error(env, "-1", "getgroups initialize failed");
            return nullptr;
        }
        std::vector<gid_t> pgrous(progroups);
        progroups = getgroups(progroups, pgrous.data());
        if (progroups == -1) {
            napi_throw_error(env, "-1", "getgroups");
            return nullptr;
        }
        pgrous.resize(static_cast<size_t>(progroups));
        gid_t proegid = getegid();
        if (std::find(pgrous.begin(), pgrous.end(), proegid) == pgrous.end()) {
            pgrous.push_back(proegid);
        }
        std::vector<uint32_t> array;
        for (auto iter = pgrous.begin(); iter != pgrous.end(); iter++) {
            auto receive = static_cast<uint32_t>(*iter);
            array.push_back(receive);
        }
        NAPI_CALL(env, napi_create_array(env, &result));
        size_t len = array.size();
        for (size_t i = 0; i < len; i++) {
            napi_value numvalue = nullptr;
            NAPI_CALL(env, napi_create_uint32(env, array[i], &numvalue));
            NAPI_CALL(env, napi_set_element(env, result, i, numvalue));
        }
        return result;
    }

    napi_value Process::GetPid(napi_env env) const
    {
        napi_value result = nullptr;
        auto proPid = static_cast<int32_t>(getpid());
        napi_create_int32(env, proPid, &result);
        return result;
    }

    napi_value Process::GetPpid(napi_env env) const
    {
        napi_value result = nullptr;
        auto proPpid = static_cast<int32_t>(getppid());
        napi_create_int32(env, proPpid, &result);
        return result;
    }

    void Process::Chdir(napi_env env, napi_value args) const
    {
        size_t prolen = 0;
        if (napi_get_value_string_utf8(env, args, nullptr, 0, &prolen) != napi_ok) {
            HILOG_ERROR("can not get args size");
            return;
        }
        std::string result = "";
        result.reserve(prolen + 1);
        result.resize(prolen);
        if (napi_get_value_string_utf8(env, args, result.data(), prolen + 1, &prolen) != napi_ok) {
            HILOG_ERROR("can not get args value");
            return;
        }
        int proerr = 0;
        proerr = uv_chdir(result.c_str());
        if (proerr) {
            napi_throw_error(env, "-1", "chdir");
            return;
        }
    }

    napi_value Process::Kill(napi_env env, napi_value signal, napi_value proid)
    {
        int32_t pid = 0;
        int32_t sig = 0;
        napi_get_value_int32(env, proid, &pid);
        napi_get_value_int32(env, signal, &sig);
        uv_pid_t ownPid = uv_os_getpid();
        // 64:The maximum valid signal value is 64.
        if (sig > 64 && (!pid || pid == -1 || pid == ownPid || pid == -ownPid)) {
            napi_throw_error(env, "0", "process exit");
            return nullptr;
        }
        bool flag = false;
        int err = uv_kill(pid, sig);
        if (!err) {
            flag = true;
        }
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_boolean(env, flag, &result));
        return result;
    }

    napi_value Process::Uptime(napi_env env) const
    {
        napi_value result = nullptr;
        struct sysinfo information = {0};
        time_t systimer = 0;
        double runsystime = 0.0;
        if (sysinfo(&information)) {
            napi_throw_error(env, "-1", "Failed to get sysinfo");
            return nullptr;
        }
        systimer = information.uptime;
        if (systimer > 0) {
            runsystime = static_cast<double>(systimer);
            NAPI_CALL(env, napi_create_double(env, runsystime, &result));
        } else {
            napi_throw_error(env, "-1", "Failed to get systimer");
            return nullptr;
        }
        return result;
    }

    void Process::Exit(napi_env env, napi_value number) const
    {
        int32_t result = 0;
        napi_get_value_int32(env, number, &result);
        exit(result);
    }

    napi_value Process::Cwd(napi_env env) const
    {
        napi_value result = nullptr;
        char buf[260 * NUM_OF_DATA] = { 0 }; // 260:Only numbers path String size is 260.
        size_t length = sizeof(buf);
        int err = uv_cwd(buf, &length);
        if (err) {
            napi_throw_error(env, "1", "uv_cwd");
            return nullptr;
        }
        napi_create_string_utf8(env, buf, length, &result);
        return result;
    }

    void Process::Abort() const
    {
        exit(0);
    }

    void Process::On(napi_env env, napi_value str, napi_value function)
    {
        std::string result = "";
        size_t bufferSize = 0;
        if (napi_get_value_string_utf8(env, str, nullptr, NAPI_RETURN_ZERO, &bufferSize) != napi_ok) {
            HILOG_ERROR("can not get str size");
            return;
        }
        result.reserve(bufferSize + NAPI_RETURN_ONE);
        result.resize(bufferSize);
        if (napi_get_value_string_utf8(env, str, result.data(), bufferSize + NAPI_RETURN_ONE,
                                       &bufferSize) != napi_ok) {
            HILOG_ERROR("can not get str value");
            return;
        }
        if (function == nullptr) {
            HILOG_ERROR("function is nullptr");
            return;
        }
        napi_ref myCallRef = nullptr;
        napi_status status = napi_create_reference(env, function, 1, &myCallRef);
        if (status != napi_ok) {
            HILOG_ERROR("napi_create_reference is failed");
            return;
        }
        if (!result.empty()) {
            size_t pos = events.find(result);
            if (pos == std::string::npos) {
                HILOG_ERROR("illegal event");
                return;
            }
            eventMap.insert(std::make_pair(result, myCallRef));
        }
    }

    napi_value Process::Off(napi_env env, napi_value str)
    {
        size_t bufferSize = 0;
        bool flag = false;
        if (napi_get_value_string_utf8(env, str, nullptr, 0, &bufferSize) != napi_ok) {
            HILOG_ERROR("can not get str size");
            return nullptr;
        }
        std::string result = "";
        result.reserve(bufferSize + 1);
        result.resize(bufferSize);
        if (napi_get_value_string_utf8(env, str, result.data(), bufferSize + 1, &bufferSize) != napi_ok) {
            HILOG_ERROR("can not get str value");
            return nullptr;
        }
        std::string temp = "";
        temp = result;
        auto iter = eventMap.equal_range(temp);
        while (iter.first != iter.second) {
            NAPI_CALL(env, napi_delete_reference(env, iter.first->second));
            iter.first = eventMap.erase(iter.first);
            flag = true;
        }
        napi_value convertResult = nullptr;
        NAPI_CALL(env, napi_get_boolean(env, flag, &convertResult));
        return convertResult;
    }

    napi_value Process::GetTid(napi_env env) const
    {
        napi_value result = nullptr;
        auto proTid = static_cast<int32_t>(gettid());
        napi_create_int32(env, proTid, &result);
        return result;
    }

    napi_value Process::IsIsolatedProcess(napi_env env) const
    {
        napi_value result = nullptr;
        bool flag = true;
        auto prouid = static_cast<int32_t>(getuid());
        auto uid = prouid % PER_USER_RANGE;
        if ((uid >= 99000 && uid <= 99999) || // 99999:Only isolateuid numbers between 99000 and 99999.
            (uid >= 9000 && uid <= 98999)) { // 98999:Only appuid numbers between 9000 and 98999.
            NAPI_CALL(env, napi_get_boolean(env, flag, &result));
            return result;
        }
        flag = false;
        NAPI_CALL(env, napi_get_boolean(env, flag, &result));
        return result;
    }

    napi_value Process::IsAppUid(napi_env env, napi_value uid) const
    {
        int32_t number = 0;
        napi_value result = nullptr;
        bool flag = true;
        napi_get_value_int32(env, uid, &number);
        if (number > 0) {
            const auto appId = number % PER_USER_RANGE;
            if (appId >= FIRST_APPLICATION_UID && appId <= LAST_APPLICATION_UID) {
                napi_get_boolean(env, flag, &result);
                return result;
            }
            flag = false;
            NAPI_CALL(env, napi_get_boolean(env, flag, &result));
            return result;
        } else {
            flag = false;
            NAPI_CALL(env, napi_get_boolean(env, flag, &result));
            return result;
        }
    }

    napi_value Process::Is64Bit(napi_env env) const
    {
        napi_value result = nullptr;
        bool flag = true;
        auto size = sizeof(char*);
        flag = (size == NUM_OF_DATA) ? false : true;
        NAPI_CALL(env, napi_get_boolean(env, flag, &result));
        return result;
    }

    napi_value Process::GetEnvironmentVar(napi_env env, napi_value name) const
    {
        napi_value convertResult = nullptr;
        char buf[260 * NUM_OF_DATA] = { 0 }; // 260:Only numbers path String size is 260.
        size_t length = sizeof(buf);
        size_t bufferSize = 0;
        if (napi_get_value_string_utf8(env, name, nullptr, 0, &bufferSize) != napi_ok) {
            HILOG_ERROR("can not get name size");
            return nullptr;
        }
        std::string result = "";
        result.reserve(bufferSize + 1);
        result.resize(bufferSize);
        if (napi_get_value_string_utf8(env, name, result.data(), bufferSize + 1, &bufferSize) != napi_ok) {
            HILOG_ERROR("can not get name value");
            return nullptr;
        }
        std::string temp = "";
        temp = result;
        auto envNum = uv_os_getenv(temp.c_str(), buf, &length);
        if (envNum == UV_ENOENT) {
            NAPI_CALL(env, napi_get_undefined(env, &convertResult));
            return convertResult;
        }
        napi_create_string_utf8(env, buf, strlen(buf), &convertResult);
        return convertResult;
    }

    napi_value Process::GetUidForName(napi_env env, napi_value name) const
    {
        napi_value convertResult = nullptr;
        size_t bufferSize = 0;
        if (napi_get_value_string_utf8(env, name, nullptr, 0, &bufferSize) != napi_ok) {
            HILOG_ERROR("can not get name size");
            return nullptr;
        }
        std::string result = "";
        result.reserve(bufferSize + 1);
        result.resize(bufferSize);
        if (napi_get_value_string_utf8(env, name, result.data(), bufferSize + 1, &bufferSize) != napi_ok) {
            HILOG_ERROR("can not get name value");
            return nullptr;
        }
        struct passwd user;
        int32_t uid = 0;
        struct passwd *bufp = nullptr;
        long bufLen = sysconf(_SC_GETPW_R_SIZE_MAX);
        if (bufLen == -1) {
            bufLen = 16384; // 16384:Should be more than enough
        }

        std::string buf;
        buf.reserve(bufLen + 1);
        buf.resize(bufLen);
        if (getpwnam_r(result.c_str(), &user, buf.data(), bufLen, &bufp) == 0 && bufp != nullptr) {
            uid = static_cast<int32_t>(bufp->pw_uid);
            napi_create_int32(env, uid, &convertResult);
            return convertResult;
        }
        napi_create_int32(env, (-1), &convertResult);
        return convertResult;
    }

    napi_value Process::GetThreadPriority(napi_env env, napi_value tid) const
    {
        errno = 0;
        int32_t proTid = 0;
        napi_value result = nullptr;
        napi_get_value_int32(env, tid, &proTid);
        int32_t pri = getpriority(PRIO_PROCESS, proTid);
        if (errno) {
            napi_throw_error(env, "-1", "Invalid tid");
            return nullptr;
        }
        napi_create_int32(env, pri, &result);
        return result;
    }

    napi_value Process::GetStartRealtime(napi_env env) const
    {
        struct timespec timespro = {0, 0};
        struct timespec timessys = {0, 0};
        napi_value result = nullptr;
        auto res = clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &timespro);
        if (res) {
            return 0;
        }
        auto res1 = clock_gettime(CLOCK_MONOTONIC, &timessys);
        if (res1) {
            return 0;
        }
        int whenpro = ConvertTime(timespro.tv_sec, timespro.tv_nsec);
        int whensys = ConvertTime(timessys.tv_sec, timessys.tv_nsec);
        auto timedif = (whensys - whenpro);
        napi_create_int32(env, timedif, &result);
        return result;
    }

    int Process::ConvertTime(time_t tvsec, int64_t tvnsec) const
    {
        return int(tvsec * 1000) + int(tvnsec / 1000000); // 98999:Only converttime numbers is 1000 and 1000000.
    }

    napi_value Process::GetPastCputime(napi_env env) const
    {
        struct timespec times = {0, 0};
        napi_value result = nullptr;
        auto res = clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &times);
        if (res) {
            return 0;
        }
        int when =  ConvertTime(times.tv_sec, times.tv_nsec);
        napi_create_int32(env, when, &result);
        return result;
    }

    napi_value Process::GetSystemConfig(napi_env env, napi_value name) const
    {
        int32_t number = 0;
        napi_value result = nullptr;
        napi_get_value_int32(env, name, &number);
        auto configinfo = static_cast<int32_t>(sysconf(number));
        napi_create_int32(env, configinfo, &result);
        return result;
    }

    napi_value UnHandle(napi_env env, napi_value promise, napi_value reason)
    {
        napi_ref promiseRef = nullptr;
        NAPI_CALL(env, napi_create_reference(env, promise, 1, &promiseRef));
        napi_ref reasonRef = nullptr;
        NAPI_CALL(env, napi_create_reference(env, reason, 1, &reasonRef));
        pendingUnHandledRejections.insert(std::make_pair(promiseRef, reasonRef));
        napi_value res = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &res));
        return res;
    }

    napi_value AddHandle(napi_env env, napi_value promise)
    {
        auto iter = pendingUnHandledRejections.begin();
        while (iter != pendingUnHandledRejections.end()) {
            napi_value prom = nullptr;
            NAPI_CALL(env, napi_get_reference_value(env, iter->first, &prom));
            bool isEquals = false;
            NAPI_CALL(env, napi_strict_equals(env, promise, prom, &isEquals));
            if (isEquals) {
                NAPI_CALL(env, napi_delete_reference(env, iter->first));
                NAPI_CALL(env, napi_delete_reference(env, iter->second));
                iter = pendingUnHandledRejections.erase(iter);
                continue;
            }
            iter++;
        }
        napi_value res = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &res));
        return res;
    }

    napi_value UnHandleRejection(napi_env env, napi_value promise, napi_value reason)
    {
        auto it = eventMap.find("UnHandleRejection");
        if (it != eventMap.end()) {
            napi_value global = nullptr;
            NAPI_CALL(env, napi_get_global(env, &global));
            size_t argc = 2; // 2 parameter size
            napi_value args[] = {reason, promise};
            auto iter = eventMap.equal_range("UnHandleRejection");
            while (iter.first != iter.second) {
                napi_value cb = nullptr;
                NAPI_CALL(env, napi_get_reference_value(env, iter.first->second, &cb));
                napi_value result = nullptr;
                NAPI_CALL(env, napi_call_function(env, global, cb, argc, args, &result));
                iter.first++;
            }
        }
        napi_value res = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &res));
        return res;
    }

    static napi_value OnUnHandleRejection(napi_env env, napi_callback_info info)
    {
        size_t argc = 3; // 3 parameter size
        napi_value argv[3] = {0}; // 3 array length
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
        int32_t event = 0;
        NAPI_CALL(env, napi_get_value_int32(env, argv[0], &event));
        if (event == static_cast<int32_t>(PromiseRejectionEvent::REJECT)) {
            UnHandle(env, argv[1], argv[2]); // 2 array index
        } else if (event == static_cast<int32_t>(PromiseRejectionEvent::HANDLE)) {
            AddHandle(env, argv[1]);
        }
        napi_value res = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &res));
        return res;
    }

    static napi_value CheckUnhandleRejections(napi_env env, napi_callback_info info)
    {
        if (!pendingUnHandledRejections.empty()) {
            auto iter = pendingUnHandledRejections.begin();
            while (iter != pendingUnHandledRejections.end()) {
                napi_value promise = nullptr;
                NAPI_CALL(env, napi_get_reference_value(env, iter->first, &promise));
                napi_value reason = nullptr;
                NAPI_CALL(env, napi_get_reference_value(env, iter->second, &reason));

                UnHandleRejection(env, promise, reason);

                NAPI_CALL(env, napi_delete_reference(env, iter->first));
                NAPI_CALL(env, napi_delete_reference(env, iter->second));
                iter = pendingUnHandledRejections.erase(iter);
            }
        }
        napi_value res = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &res));
        return res;
    }

    napi_value Process::SetRejectionCallback(napi_env env) const
    {
        napi_value cb = nullptr;
        std::string callbackName = "onUnHandleRejection";
        NAPI_CALL(env, napi_create_function(env, callbackName.c_str(), callbackName.size(), OnUnHandleRejection,
                                             nullptr, &cb));
        napi_ref unHandleRejectionCallbackRef = nullptr;
        NAPI_CALL(env, napi_create_reference(env, cb, 1, &unHandleRejectionCallbackRef));

        napi_ref checkUnhandleRejectionsRef = nullptr;
        napi_value checkcb = nullptr;
        std::string cbName = "CheckUnhandleRejections";
        NAPI_CALL(env, napi_create_function(env, cbName.c_str(), cbName.size(), CheckUnhandleRejections,
                                             nullptr, &checkcb));
        NAPI_CALL(env, napi_create_reference(env, checkcb, 1, &checkUnhandleRejectionsRef));
        napi_value res = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &res));
        return res;
    }
    void Process::ClearReference(napi_env env)
    {
        auto iter = eventMap.begin();
        while (iter != eventMap.end()) {
            napi_status status = napi_delete_reference(env, iter->second);
            if (status != napi_ok) {
                napi_throw_error(env, nullptr, "ClearReference failed");
                return;
            }
            iter++;
        }
        eventMap.clear();
    }

    napi_value ProcessManager::IsAppUid(napi_env env, napi_value uid) const
    {
        int32_t number = 0;
        napi_value result = nullptr;
        bool flag = true;
        napi_get_value_int32(env, uid, &number);
        if (number > 0) {
            const auto appId = number % PER_USER_RANGE;
            if (appId >= FIRST_APPLICATION_UID && appId <= LAST_APPLICATION_UID) {
                napi_get_boolean(env, flag, &result);
                return result;
            }
            flag = false;
            NAPI_CALL(env, napi_get_boolean(env, flag, &result));
            return result;
        } else {
            flag = false;
            NAPI_CALL(env, napi_get_boolean(env, flag, &result));
            return result;
        }
    }

    napi_value ProcessManager::GetUidForName(napi_env env, napi_value name) const
    {
        napi_value convertResult = nullptr;
        size_t bufferSize = 0;
        if (napi_get_value_string_utf8(env, name, nullptr, 0, &bufferSize) != napi_ok) {
            HILOG_ERROR("can not get name size");
            return nullptr;
        }
        std::string result = "";
        result.reserve(bufferSize + 1);
        result.resize(bufferSize);
        if (napi_get_value_string_utf8(env, name, result.data(), bufferSize + 1, &bufferSize) != napi_ok) {
            HILOG_ERROR("can not get name value");
            return nullptr;
        }
        struct passwd user;
        int32_t uid = 0;
        struct passwd *bufp = nullptr;
        long bufLen = sysconf(_SC_GETPW_R_SIZE_MAX);
        if (bufLen == -1) {
            bufLen = 16384; // 16384:Should be more than enough
        }

        std::string buf;
        buf.reserve(bufLen + 1);
        buf.resize(bufLen);
        if (getpwnam_r(result.c_str(), &user, buf.data(), bufLen, &bufp) == 0 && bufp != nullptr) {
            uid = static_cast<int32_t>(bufp->pw_uid);
            napi_create_int32(env, uid, &convertResult);
            return convertResult;
        }
        napi_create_int32(env, (-1), &convertResult);
        return convertResult;
    }

    napi_value ProcessManager::GetThreadPriority(napi_env env, napi_value tid) const
    {
        errno = 0;
        int32_t proTid = 0;
        napi_value result = nullptr;
        napi_get_value_int32(env, tid, &proTid);
        int32_t pri = getpriority(PRIO_PROCESS, proTid);
        if (errno) {
            napi_throw_error(env, "-1", "Invalid tid");
            return nullptr;
        }
        napi_create_int32(env, pri, &result);
        return result;
    }

    napi_value ProcessManager::GetSystemConfig(napi_env env, napi_value name) const
    {
        int32_t number = 0;
        napi_value result = nullptr;
        napi_get_value_int32(env, name, &number);
        auto configinfo = static_cast<int32_t>(sysconf(number));
        napi_create_int32(env, configinfo, &result);
        return result;
    }

    napi_value ProcessManager::GetEnvironmentVar(napi_env env, napi_value name) const
    {
        size_t bufferSize = 0;
        if (napi_get_value_string_utf8(env, name, nullptr, 0, &bufferSize) != napi_ok) {
            HILOG_ERROR("can not get name size");
            return nullptr;
        }
        std::string result = "";
        result.reserve(bufferSize + 1);
        result.resize(bufferSize);
        if (napi_get_value_string_utf8(env, name, result.data(), bufferSize + 1, &bufferSize) != napi_ok) {
            HILOG_ERROR("can not get name value");
            return nullptr;
        }
        std::string temp = "";
        temp = result;
        char buf[260 * NUM_OF_DATA] = { 0 }; // 260:Only numbers path String size is 260.
        size_t length = sizeof(buf);
        auto envNum = uv_os_getenv(temp.c_str(), buf, &length);
        napi_value convertResult = nullptr;
        if (envNum == UV_ENOENT) {
            NAPI_CALL(env, napi_get_undefined(env, &convertResult));
            return convertResult;
        }
        napi_create_string_utf8(env, buf, strlen(buf), &convertResult);
        return convertResult;
    }

    void ProcessManager::Exit(napi_env env, napi_value number) const
    {
        int32_t result = 0;
        napi_get_value_int32(env, number, &result);
        exit(result);
    }

    napi_value ProcessManager::Kill(napi_env env, napi_value signal, napi_value proid)
    {
        int32_t pid = 0;
        int32_t sig = 0;
        napi_get_value_int32(env, proid, &pid);
        napi_get_value_int32(env, signal, &sig);
        uv_pid_t ownPid = uv_os_getpid();
        // 64:The maximum valid signal value is 64.
        if (sig > 64 && (!pid || pid == -1 || pid == ownPid || pid == -ownPid)) {
            napi_throw_error(env, "0", "process exit");
            return nullptr;
        }
        bool flag = false;
        int err = uv_kill(pid, sig);
        if (!err) {
            flag = true;
        }
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_boolean(env, flag, &result));
        return result;
    }
} // namespace OHOS::Js_sys_module::Process
