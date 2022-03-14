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

#include "js_childprocess.h"

#include <map>
#include <vector>

#include <csignal>
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>

#include "securec.h"
#include "utils/log.h"

namespace OHOS::Js_sys_module::Process {
    constexpr int MAXSIZE = 1024;
    constexpr int TIME_EXCHANGE = 1000;
    std::map<std::string, int> g_signalsMap = {
        {"SIGHUP", 1},
        {"SIGINT", 2},
        {"SIGQUIT", 3},
        {"SIGILL", 4},
        {"SIGTRAP", 5},
        {"SIGABRT", 6},
        {"SIGBUS", 7},
        {"SIGFPE", 8},
        {"SIGKILL", 9},
        {"SIGUSR1", 10},
        {"SIGSEGV", 11},
        {"SIGUSR2", 12},
        {"SIGPIPE", 13},
        {"SIGALRM", 14},
        {"SIGTERM", 15},
        {"SIGSTKFLT", 16},
        {"SIGCHLD", 17},
        {"SIGCONT", 18},
        {"SIGSTOP", 19},
        {"SIGTSTP", 20},
        {"SIGTTIN", 21},
        {"SIGTTOU", 22},
        {"SIGURG", 23},
        {"SIGXCPU", 24},
        {"SIGXFSZ", 25},
        {"SIGVTALRM", 26},
        {"SIGPROF", 27},
        {"SIGWINCH", 28},
        {"SIGIO", 29},
        {"SIGPWR", 30},
        {"SIGSYS", 31}
    };

    ChildProcess::ChildProcess(napi_env env) : env_(env) {}

    void ChildProcess::Spawn(napi_value command)
    {
        int ret = pipe(stdOutFd_);
        if (ret < 0) {
            HILOG_ERROR("pipe1 failed %{public}d", errno);
            return;
        }
        ret = pipe(stdErrFd_);
        if (ret < 0) {
            HILOG_ERROR("pipe2 failed %{public}d", errno);
            return;
        }
        std::string strCommnd = RequireStrValue(command);
        pid_t pid = fork();
        if (!pid) {
            close(stdErrFd_[0]);
            close(stdOutFd_[0]);
            dup2(stdOutFd_[1], 1);
            dup2(stdErrFd_[1], 2); // 2:The value of parameter
            if (execl("/bin/sh", "sh", "-c", strCommnd.c_str(), nullptr) == -1) {
                HILOG_ERROR("execl command failed");
                exit(127); // 127:The parameter value
            }
        } else if (pid > 0) {
            optionsInfo_->pid = pid;
            ppid_ = getpid();
            CreateWorker();
            napi_value resourceName = nullptr;
            napi_create_string_utf8(env_, "TimeoutListener", strlen("TimeoutListener"), &resourceName);
            napi_create_async_work(
                env_, nullptr, resourceName, TimeoutListener,
                [](napi_env env, napi_status status, void* data) {
                    OptionsInfo* optionsInfo = reinterpret_cast<OptionsInfo*>(data);
                    napi_delete_async_work(env, optionsInfo->worker);
                    delete optionsInfo;
                },
                reinterpret_cast<void*>(optionsInfo_), &optionsInfo_->worker);
            napi_queue_async_work(env_, optionsInfo_->worker);
            close(stdErrFd_[1]);
            close(stdOutFd_[1]);
        } else {
            HILOG_ERROR("child process create failed");
        }
    }

    napi_value ChildProcess::Wait()
    {
        napi_value promise = nullptr;
        auto waitInfo = new WaitInfo;
        napi_create_promise(env_, &(waitInfo->deferred), &promise);

        if (isWait_) {
            int32_t status;
            isWait_ = false;
            waitpid(optionsInfo_->pid, &status, 0);
            exitCode_ = status;
        }
        isNeedRun_ = false;
        napi_value result = nullptr;
        napi_create_int32(env_, static_cast<int8_t>(exitCode_), &result);
        napi_resolve_deferred(env_, waitInfo->deferred, result);
        delete waitInfo;
        waitInfo = nullptr;

        return promise;
    }

    napi_value ChildProcess::GetOutput() const
    {
        NAPI_CALL(env_, napi_create_promise(env_, &stdOutInfo_->deferred, &stdOutInfo_->promise));
        void* data = nullptr;
        napi_value arrayBuffer = nullptr;
        size_t bufferSize = stdOutInfo_->stdData.size() + 1;
        NAPI_CALL(env_, napi_create_arraybuffer(env_, bufferSize, &data, &arrayBuffer));
        if (memcpy_s(data, bufferSize, reinterpret_cast<const void*>(stdOutInfo_->stdData.c_str()),
            stdOutInfo_->stdData.size()) != EOK) {
            HILOG_ERROR("getOutput memcpy_s failed");
            NAPI_CALL(env_, napi_delete_async_work(env_, stdOutInfo_->worker));
            napi_value res = nullptr;
            NAPI_CALL(env_, napi_get_undefined(env_, &res));
            return res;
        }

        napi_value result = nullptr;
        NAPI_CALL(env_, napi_create_typedarray(env_, napi_uint8_array, bufferSize, arrayBuffer, 0, &result));
        NAPI_CALL(env_, napi_resolve_deferred(env_, stdOutInfo_->deferred, result));
        return stdOutInfo_->promise;
    }

    napi_value ChildProcess::GetErrorOutput() const
    {
        NAPI_CALL(env_, napi_create_promise(env_, &stdErrInfo_->deferred, &stdErrInfo_->promise));
        void* data = nullptr;
        napi_value arrayBuffer = nullptr;
        size_t bufferSize = stdErrInfo_->stdData.size() + 1;
        NAPI_CALL(env_, napi_create_arraybuffer(env_, bufferSize, &data, &arrayBuffer));
        if (memcpy_s(data, bufferSize, reinterpret_cast<const void*>(stdErrInfo_->stdData.c_str()),
            stdErrInfo_->stdData.size()) != EOK) {
            HILOG_ERROR("getErrOutput memcpy_s failed");
            NAPI_CALL(env_, napi_delete_async_work(env_, stdErrInfo_->worker));
            napi_value res = nullptr;
            NAPI_CALL(env_, napi_get_undefined(env_, &res));
            return res;
        }

        napi_value result = nullptr;
        NAPI_CALL(env_, napi_create_typedarray(env_, napi_uint8_array, bufferSize, arrayBuffer, 0, &result));
        NAPI_CALL(env_, napi_resolve_deferred(env_, stdErrInfo_->deferred, result));
        return stdErrInfo_->promise;
    }

    napi_value ChildProcess::GetKilled() const
    {
        napi_value result = nullptr;
        NAPI_CALL(env_, napi_get_boolean(env_, killed_, &result));

        return result;
    }

    napi_value ChildProcess::Getpid() const
    {
        napi_value result = nullptr;
        NAPI_CALL(env_, napi_create_int32(env_, optionsInfo_->pid, &result));

        return result;
    }

    napi_value ChildProcess::Getppid() const
    {
        napi_value result = nullptr;
        NAPI_CALL(env_, napi_create_int32(env_, ppid_, &result));

        return result;
    }

    napi_value ChildProcess::GetExitCode() const
    {
        napi_value result = nullptr;
        NAPI_CALL(env_, napi_create_int32(env_, static_cast<int8_t>(exitCode_), &result));

        return result;
    }

    void ChildProcess::CreateWorker()
    {
        // getstdout
        napi_value resourceName = nullptr;
        stdOutInfo_ = new StdInfo();
        stdOutInfo_->isNeedRun = &isNeedRun_;
        stdOutInfo_->fd = stdOutFd_[0];
        stdOutInfo_->pid = optionsInfo_->pid;
        stdOutInfo_->maxBuffSize = optionsInfo_->maxBuffer;
        napi_create_string_utf8(env_, "ReadStdOut", NAPI_AUTO_LENGTH, &resourceName);
        napi_create_async_work(env_, nullptr, resourceName, ReadStdOut, EndStdOut,
                               reinterpret_cast<void*>(stdOutInfo_), &stdOutInfo_->worker);
        napi_queue_async_work(env_, stdOutInfo_->worker);

        // getstderr
        stdErrInfo_ = new StdInfo();
        stdErrInfo_->isNeedRun = &isNeedRun_;
        stdErrInfo_->fd = stdErrFd_[0];
        stdErrInfo_->pid = optionsInfo_->pid;
        stdErrInfo_->maxBuffSize = optionsInfo_->maxBuffer;
        napi_create_string_utf8(env_, "ReadStdErr", NAPI_AUTO_LENGTH, &resourceName);
        napi_create_async_work(env_, nullptr, resourceName, ReadStdErr, EndStdErr,
                               reinterpret_cast<void*>(stdErrInfo_), &stdErrInfo_->worker);
        napi_queue_async_work(env_, stdErrInfo_->worker);
    }

    void ChildProcess::ReadStdOut(napi_env env, void* data)
    {
        auto stdOutInfo = reinterpret_cast<StdInfo*>(data);
        char childStdout[MAXSIZE] = {0};
        if (stdOutInfo->isNeedRun == nullptr) {
            return;
        }
        while (*(stdOutInfo->isNeedRun)) {
            auto readSize = read(stdOutInfo->fd, childStdout, sizeof(childStdout) - 1);
            if (readSize >= 0) {
                stdOutInfo->stdData += childStdout;
            }
            if (stdOutInfo->stdData.size() > static_cast<size_t>(stdOutInfo->maxBuffSize) && *(stdOutInfo->isNeedRun)) {
                if (!kill(stdOutInfo->pid, SIGKILL)) {
                    *(stdOutInfo->isNeedRun) = false;
                    stdOutInfo->stdData = stdOutInfo->stdData.substr(0, stdOutInfo->maxBuffSize);
                } else {
                    HILOG_ERROR("stdOut maxBuff kill signal failed");
                }
            }
            if (memset_s(childStdout, sizeof(childStdout), '\0', MAXSIZE) != EOK) {
                HILOG_ERROR("getOutput memset_s failed");
                return;
            }
        }
    }

    void ChildProcess::EndStdOut(napi_env env, napi_status status, void* buffer)
    {
        auto stdOutInfo = reinterpret_cast<StdInfo*>(buffer);
        napi_delete_async_work(env, stdOutInfo->worker);
        delete stdOutInfo;
    }

    void ChildProcess::ReadStdErr(napi_env env, void* data)
    {
        auto stdErrInfo = reinterpret_cast<StdInfo*>(data);
        char childStderr[MAXSIZE] = {0};
        if (stdErrInfo->isNeedRun == nullptr) {
            return;
        }
        while (*(stdErrInfo->isNeedRun)) {
            auto readSize = read(stdErrInfo->fd, childStderr, sizeof(childStderr) - 1);
            if (readSize >= 0) {
                stdErrInfo->stdData += childStderr;
            }
            if (stdErrInfo->stdData.size() > static_cast<size_t>(stdErrInfo->maxBuffSize) && *(stdErrInfo->isNeedRun)) {
                if (!kill(stdErrInfo->pid, SIGKILL)) {
                    *(stdErrInfo->isNeedRun) = false;
                    stdErrInfo->stdData = stdErrInfo->stdData.substr(0, stdErrInfo->maxBuffSize);
                } else {
                    HILOG_ERROR("stdErr maxBuff kill signal failed");
                }
            }
            if (memset_s(childStderr, sizeof(childStderr), '\0', MAXSIZE) != EOK) {
                HILOG_ERROR("getOutput memset_s failed");
                return;
            }
        }
    }

    void ChildProcess::EndStdErr(napi_env env, napi_status status, void* buffer)
    {
        auto stdErrInfo = reinterpret_cast<StdInfo*>(buffer);
        napi_delete_async_work(env, stdErrInfo->worker);
        delete stdErrInfo;
    }

    int ChildProcess::GetValidSignal(const napi_value signo)
    {
        int32_t sig = 0;
        napi_valuetype valuetype = napi_undefined;
        napi_typeof(env_, signo, &valuetype);
        if (valuetype == napi_valuetype::napi_number) {
            napi_get_value_int32(env_, signo, &sig);
            return sig;
        } else if (valuetype == napi_valuetype::napi_string) {
            std::string buffer = RequireStrValue(signo);
            auto iter = g_signalsMap.find(buffer);
            if (iter != g_signalsMap.end()) {
                sig = iter->second;
                return sig;
            } else {
                return g_signalsMap["SIGTERM"];
            }
        } else {
            return g_signalsMap["SIGTERM"];
        }
    }

    void ChildProcess::Kill(const napi_value signo)
    {
        int signal = GetValidSignal(signo);
        std::vector<int32_t> signalType = {SIGINT, SIGQUIT, SIGKILL, SIGTERM};
        if (!kill(optionsInfo_->pid, signal)) {
            auto res = std::find(signalType.begin(), signalType.end(), static_cast<int32_t>(signal));
            (res != signalType.end()) ? isNeedRun_ = false : 0;
            killed_ = true;
        } else {
            HILOG_ERROR("kill signal failed");
        }
    }

    void ChildProcess::Close()
    {
        int32_t status = 0;
        if (isWait_ && !(waitpid(optionsInfo_->pid, &status, WNOHANG)) && isNeedRun_) {
            if (!kill(optionsInfo_->pid, SIGKILL)) {
                waitpid(optionsInfo_->pid, &status, 0);
                isWait_ = false;
                exitCode_ = status;
                isNeedRun_ = false;
            } else {
                HILOG_ERROR("close kill SIGKILL signal failed");
            }
        }
    }

    void ChildProcess::TimeoutListener(napi_env env, void* data)
    {
        std::vector<int32_t> signalType = {SIGINT, SIGQUIT, SIGKILL, SIGTERM};
        auto temp = reinterpret_cast<OptionsInfo*>(data);
        int32_t timeout = temp->timeout * TIME_EXCHANGE;
        if (timeout > 0) {
            usleep(static_cast<size_t>(timeout));
            if (*(temp->isNeedRun)) {
                if (!kill(temp->pid, temp->killSignal)) {
                    auto res = std::find(signalType.begin(), signalType.end(), temp->killSignal);
                    (res != signalType.end()) ? *(temp->isNeedRun) = false : 0;
                } else {
                    HILOG_ERROR("timeout kill signal failed");
                }
            }
        }
    }

    void ChildProcess::InitOptionsInfo(napi_value options)
    {
        std::vector<std::string> keyStr = {"timeout", "killSignal", "maxBuffer"};
        optionsInfo_ = new OptionsInfo();
        size_t size = keyStr.size();
        for (size_t i = 0; i < size; i++) {
            napi_status status = napi_ok;
            napi_value property = nullptr;
            napi_get_named_property(env_, options, keyStr[i].c_str(), &property);
            switch (i) {
                case 0:
                    status = napi_get_value_int32(env_, property, &optionsInfo_->timeout);
                    if (status != napi_ok) {
                        optionsInfo_->timeout = 0;
                    }
                    break;
                case 1:
                    optionsInfo_->killSignal = GetValidSignal(property);
                    break;
                case 2: // 2:The parameter value
                    status = napi_get_value_int64(env_, property, &optionsInfo_->maxBuffer);
                    if (status != napi_ok) {
                        optionsInfo_->maxBuffer = static_cast<int64_t>(MAXSIZE) * static_cast<int64_t>(MAXSIZE);
                    }
                    break;
                default:
                    break;
            }
        }
        optionsInfo_->isNeedRun = &isNeedRun_;
    }

    std::string ChildProcess::RequireStrValue(const napi_value strValue)
    {
        size_t bufferSize = 0;
        if (napi_get_value_string_utf8(env_, strValue, nullptr, 0, &bufferSize) != napi_ok) {
            HILOG_ERROR("can not get strValue size");
            return nullptr;
        }
        std::string result = "";
        result.reserve(bufferSize + 1);
        result.resize(bufferSize);
        if (napi_get_value_string_utf8(env_, strValue, result.data(), bufferSize + 1, &bufferSize) != napi_ok) {
            HILOG_ERROR("can not get strValue value");
            return nullptr;
        }
        return result;
    }

    ChildProcess::~ChildProcess()
    {
        close(stdOutFd_[0]);
        close(stdErrFd_[0]);
        if (isWait_) {
            int32_t status = 0;
            waitpid(optionsInfo_->pid, &status, 0);
        }
        isNeedRun_ = false;
    }
} // namespace