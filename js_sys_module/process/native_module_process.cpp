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

#include <cassert>
#include <vector>
#include <grp.h>
#include <sys/types.h>
#include <unistd.h>
#include "js_childprocess.h"
#include "js_process.h"
#include "securec.h"
#include "utils/log.h"

namespace OHOS::Js_sys_module::Process {
    static napi_value DealType(napi_env env, napi_value args[], size_t argsSize)
    {
        if (args[0] != nullptr) {
            napi_valuetype valueType = napi_undefined;
            napi_typeof(env, args[0], &valueType);
            NAPI_ASSERT(env, valueType == napi_string, "Wrong argument typr. String expected");
        } else {
            HILOG_ERROR("command is null");
            napi_throw_error(env, "", "command is empty");
            return nullptr;
        }

        std::vector<std::string> keyStr = {"timeout", "killSignal", "maxBuffer"};

        if (args[1] == nullptr) {
            return nullptr;
        }
        size_t size = keyStr.size();
        for (size_t i = 0; i < size; i++) {
            napi_valuetype propertyType = napi_undefined;
            napi_value property = nullptr;
            napi_get_named_property(env, args[1], keyStr[i].c_str(), &property);
            switch (i) {
                case 0:
                    {
                        napi_typeof(env, property, &propertyType);
                        NAPI_ASSERT(env, propertyType == napi_number || propertyType == napi_undefined ||
                                    propertyType == napi_null, "Wrong timeout argument typr. Number expected");
                        int timeout = 0;
                        napi_get_value_int32(env, property, &timeout);
                        if (timeout < 0) {
                            NAPI_CALL(env, napi_throw_error(env, "", "options timeout is lessthen zero"));
                            return nullptr;
                        }
                        break;
                    }
                case 1:
                    napi_typeof(env, property, &propertyType);
                    NAPI_ASSERT(env, propertyType == napi_string || propertyType == napi_number
                                || propertyType == napi_undefined || propertyType == napi_null,
                                "Wrong KillSignal argument typr. String or number expected");
                    break;
                case 2: // 2:The parameter value
                    napi_typeof(env, property, &propertyType);
                    NAPI_ASSERT(env, propertyType == napi_number || propertyType == napi_undefined ||
                                propertyType == napi_null, "Wrong maxBuffer argument typr. Number expected");
                    break;
                default:
                    break;
            }
        }
        return nullptr;
    }

    static napi_value ChildProcessConstructor(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        void* data = nullptr;
        size_t argc = 2; // 2:The number of parameters is 2
        napi_value args[2] = { nullptr }; // 2:The number of parameters is 2
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, &data));

        DealType(env, args, argc);
        auto objectInfo = new ChildProcess();

        objectInfo->InitOptionsInfo(env, args[1]);

        objectInfo->Spawn(env, args[0]);

        NAPI_CALL(env, napi_wrap(
            env, thisVar, objectInfo,
            [](napi_env env, void* data, void* hint) {
                auto objectResult = reinterpret_cast<ChildProcess*>(data);
                if (objectResult != nullptr) {
                    delete objectResult;
                    objectResult = nullptr;
                }
            },
            nullptr, nullptr));

        return thisVar;
    }

    static napi_value Wait(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));

        ChildProcess* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object)));
        napi_value result = object->Wait(env);

        return result;
    }

    static napi_value GetOutput(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));

        ChildProcess* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object)));
        napi_value result = object->GetOutput(env);

        return result;
    }

    static napi_value Close(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));

        ChildProcess* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object)));
        object->Close();

        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &result));
        return result;
    }

    static napi_value GetErrorOutput(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));

        ChildProcess* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object)));

        napi_value result = object->GetErrorOutput(env);

        return result;
    }

    static napi_value Kill(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t requireArgc = 1;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));

        NAPI_ASSERT(env, argc >= requireArgc, "Wrong number of arguments");

        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, args, &valuetype));
        if ((valuetype != napi_valuetype::napi_number) && (valuetype != napi_valuetype::napi_string)) {
            napi_throw_error(env, nullptr, "The parameter type is incorrect");
            return nullptr;
        }

        ChildProcess* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object)));
        object->Kill(env, args);

        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &result));
        return result;
    }

    static napi_value GetKilled(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));

        ChildProcess* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object)));
        napi_value result = object->GetKilled(env);

        return result;
    }

    static napi_value Getpid(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));

        ChildProcess* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object)));
        napi_value result = object->Getpid(env);

        return result;
    }

    static napi_value Getppid(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));

        ChildProcess* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object)));
        napi_value result = object->Getppid(env);

        return result;
    }

    static napi_value GetExitCode(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));

        ChildProcess* object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&object)));
        napi_value result = object->GetExitCode(env);

        return result;
    }

    static napi_value RunCommand(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 2; // 2:The number of parameters is 2
        napi_value args[2] = { nullptr }; // 2:The number of parameters is 2
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));

        const char* childProcessClassName = "ChildProcess";
        napi_value childProcessClass = nullptr;
        napi_property_descriptor childProcessDesc[] = {
            DECLARE_NAPI_FUNCTION("close", Close),
            DECLARE_NAPI_FUNCTION("kill", Kill),
            DECLARE_NAPI_FUNCTION("getOutput", GetOutput),
            DECLARE_NAPI_FUNCTION("getErrorOutput", GetErrorOutput),
            DECLARE_NAPI_FUNCTION("wait", Wait),
            DECLARE_NAPI_GETTER("killed", GetKilled),
            DECLARE_NAPI_GETTER("pid", Getpid),
            DECLARE_NAPI_GETTER("ppid", Getppid),
            DECLARE_NAPI_GETTER("exitCode", GetExitCode),
        };

        NAPI_CALL(env, napi_define_class(env, childProcessClassName, strlen(childProcessClassName),
                                         ChildProcessConstructor, nullptr,
                                         sizeof(childProcessDesc) / sizeof(childProcessDesc[0]), childProcessDesc,
                                         &childProcessClass));

        napi_value result = nullptr;
        NAPI_CALL(env, napi_new_instance(env, childProcessClass, argc, args, &result));

        return result;
    }

    static napi_value GetUid(napi_env env, [[maybe_unused]] napi_callback_info info)
    {
        Process object;
        return object.GetUid(env);
    }

    static napi_value GetGid(napi_env env, [[maybe_unused]] napi_callback_info info)
    {
        Process object;
        return object.GetGid(env);
    }

    static napi_value GetEUid(napi_env env, [[maybe_unused]] napi_callback_info info)
    {
        Process object;
        return object.GetEUid(env);
    }

    static napi_value GetEGid(napi_env env, [[maybe_unused]] napi_callback_info info)
    {
        Process object;
        return object.GetEGid(env);
    }

    static napi_value GetGroups(napi_env env, [[maybe_unused]] napi_callback_info info)
    {
        Process object;
        return object.GetGroups(env);
    }

    static napi_value GetPid(napi_env env, [[maybe_unused]] napi_callback_info info)
    {
        Process object;
        return object.GetPid(env);
    }

    static napi_value GetPpid(napi_env env, [[maybe_unused]] napi_callback_info info)
    {
        Process object;
        return object.GetPpid(env);
    }

    static napi_value Chdir(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t requireArgc = 1;
        size_t argc = 1;
        napi_value args = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc >= requireArgc, "Wrong nuamber of arguments");
        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, args, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected");
        Process object;
        object.Chdir(env, args);
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &result));
        return result;
    }

    static napi_value Abort(napi_env env, [[maybe_unused]] napi_callback_info info)
    {
        Process object;
        object.Abort();
        napi_value res = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &res));
        return res;
    }

    static napi_value Cwd(napi_env env, [[maybe_unused]] napi_callback_info info)
    {
        Process object;
        return object.Cwd(env);
    }

    static napi_value Exit(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 1;
        napi_value args = nullptr;
        napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr);
        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, args, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type.number error");
        Process object;
        object.Exit(env, args);
        napi_value res = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &res));
        return res;
    }
    static napi_value On(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        bool flag = true;
        napi_value result = nullptr;
        size_t requireArgc = 2; // 2:The number of parameters is 2
        size_t argc = 2; // 2:The number of parameters is 2
        napi_value args[2] = { nullptr };
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, nullptr));
        NAPI_ASSERT(env, argc >= requireArgc, "Wrong number of arguments");
        napi_valuetype valuetype0;
        NAPI_CALL(env, napi_typeof(env, args[0], &valuetype0));
        if (valuetype0 != napi_valuetype::napi_string) {
            flag = false;
            NAPI_CALL(env, napi_get_boolean(env, flag, &result));
            return result;
        }
        napi_valuetype valuetype1;
        NAPI_CALL(env, napi_typeof(env, args[1], &valuetype1));
        Process object;
        object.On(env, args[0], args[1]);
        NAPI_CALL(env, napi_get_boolean(env, flag, &result));
        return result;
    }

    static napi_value Off(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 1;
        napi_value args = nullptr;
        napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr);
        Process object;
        napi_value result = object.Off(env, args);
        return result;
    }

    static napi_value Uptime(napi_env env, [[maybe_unused]] napi_callback_info info)
    {
        Process object;
        return object.Uptime(env);
    }

    static napi_value KillSig(napi_env env, napi_callback_info info)
    {
        size_t argc = 2; // 2:The number of parameters is 2
        napi_value argv[2] = {0}; // 2:The number of parameters is 2
        napi_value thisVar = nullptr;
        void* data = nullptr;
        napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
        Process object;
        napi_value result = nullptr;
        result = object.Kill(env, argv[0], argv[1]);
        return result;
    }
    static napi_value GetTid(napi_env env, [[maybe_unused]] napi_callback_info info)
    {
        Process object;
        return object.GetTid(env);
    }

    static napi_value IsIsolatedProcess(napi_env env, [[maybe_unused]] napi_callback_info info)
    {
        Process object;
        return object.IsIsolatedProcess(env);
    }

    static napi_value IsAppUid(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 1;
        napi_value args = nullptr;
        napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr);
        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, args, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type.String error");
        Process object;
        return object.IsAppUid(env, args);
    }

    static napi_value Is64Bit(napi_env env, [[maybe_unused]] napi_callback_info info)
    {
        Process object;
        return object.Is64Bit(env);
    }

    static napi_value GetUidForName(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 1;
        napi_value args = nullptr;
        napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr);
        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, args, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type.String error");
        Process object;
        return object.GetUidForName(env, args);
    }

    static napi_value GetThreadPriority(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 1;
        napi_value args = nullptr;
        napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr);
        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, args, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type.String error");
        Process object;
        return object.GetThreadPriority(env, args);
    }

    static napi_value GetStartRealtime(napi_env env, [[maybe_unused]] napi_callback_info info)
    {
        Process object;
        return object.GetStartRealtime(env);
    }

    static napi_value GetPastCputime(napi_env env, [[maybe_unused]] napi_callback_info info)
    {
        Process object;
        return object.GetPastCputime(env);
    }

    static napi_value GetSystemConfig(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 1;
        napi_value args = nullptr;
        napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr);
        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, args, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type.String error");
        Process object;
        return object.GetSystemConfig(env, args);
    }

    static napi_value GetEnvironmentVar(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        size_t argc = 1;
        napi_value args = nullptr;
        napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr);
        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, args, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type.String error");
        Process object;
        return object.GetEnvironmentVar(env, args);
    }

    static napi_value ThrowError(napi_env env, const char* errMessage)
    {
        napi_value processError = nullptr;
        napi_value code = nullptr;
        uint32_t errCode = 401; // 401:The code parameter of this error is 401
        napi_create_uint32(env, errCode, &code);
        napi_value name = nullptr;
        std::string errName = "BuisnessError";
        napi_value msg = nullptr;
        napi_create_string_utf8(env, errMessage, NAPI_AUTO_LENGTH, &msg);
        napi_create_string_utf8(env, errName.c_str(), NAPI_AUTO_LENGTH, &name);
        napi_create_error(env, nullptr, msg, &processError);
        napi_set_named_property(env, processError, "code", code);
        napi_set_named_property(env, processError, "name", name);
        napi_throw(env, processError);
        napi_value res = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &res));
        return res;
    }

    static napi_value GetValueFromInfo(napi_env env, napi_callback_info info, napi_value &thisVar)
    {
        size_t argc = 1;
        napi_value args = nullptr;
        napi_get_cb_info(env, info, &argc, &args, &thisVar, nullptr);
        return args;
    }

    static napi_value KillSigOfProcess(napi_env env, napi_callback_info info)
    {
        size_t argc = 2; // 2:The number of parameters is 2
        napi_value argv[2] = {0}; // 2:The number of parameters is 2
        napi_value thisVar = nullptr;
        void* data = nullptr;
        napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
        napi_valuetype valuetype0;
        NAPI_CALL(env, napi_typeof(env, argv[0], &valuetype0));
        napi_valuetype valuetype1;
        NAPI_CALL(env, napi_typeof(env, argv[1], &valuetype1));
        if (valuetype0 != napi_number || valuetype1 != napi_number) {
            return ThrowError(env, "The type of v must be number.");
        }
        ProcessManager *object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        return object->Kill(env, argv[0], argv[1]);
    }
    static napi_value ExitOfProcess(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value args = GetValueFromInfo(env, info, thisVar);
        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, args, &valuetype));
        if (valuetype != napi_number) {
            return ThrowError(env, "The type of v must be number.");
        }
        ProcessManager *object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        object->Exit(env, args);
        napi_value res = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &res));
        return res;
    }
    static napi_value GetSystemConfigOfProcess(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value args = GetValueFromInfo(env, info, thisVar);
        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, args, &valuetype));
        if (valuetype != napi_number) {
            return ThrowError(env, "The type of v must be number.");
        }
        ProcessManager *object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        return object->GetSystemConfig(env, args);
    }

    static napi_value GetThreadPriorityOfProcess(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value args = GetValueFromInfo(env, info, thisVar);
        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, args, &valuetype));
        if (valuetype != napi_number) {
            return ThrowError(env, "The type of v must be number.");
        }
        ProcessManager *object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        return object->GetThreadPriority(env, args);
    }

    static napi_value GetUidForNameOfProcess(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value args = GetValueFromInfo(env, info, thisVar);
        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, args, &valuetype));
        if (valuetype != napi_string) {
            return ThrowError(env, "The type of v must be string.");
        }
        ProcessManager *object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        return object->GetUidForName(env, args);
    }

    static napi_value IsAppUidOfProcess(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value args = GetValueFromInfo(env, info, thisVar);
        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, args, &valuetype));
        if (valuetype != napi_number) {
            return ThrowError(env, "The type of v must be number.");
        }
        ProcessManager *object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        return object->IsAppUid(env, args);
    }

    static napi_value GetEnvironmentVarOfProcess(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value args = GetValueFromInfo(env, info, thisVar);
        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, args, &valuetype));
        if (valuetype != napi_string) {
            return ThrowError(env, "The type of v must be string.");
        }
        ProcessManager *object = nullptr;
        NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&object));
        return object->GetEnvironmentVar(env, args);
    }

    static napi_value ProcessManagerConstructor(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        void *data = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, &data));
        auto objectInfo = new ProcessManager();
        napi_wrap(
            env, thisVar, objectInfo,
            [](napi_env environment, void *data, void *hint) {
                auto objInfo = reinterpret_cast<ProcessManager*>(data);
                if (objInfo != nullptr) {
                    delete objInfo;
                }
            },
            nullptr, nullptr);
        return thisVar;
    }

    static napi_value ProcessInit(napi_env env, napi_value exports)
    {
        const char *procssClassName = "ProcessManager";
        napi_value processClass = nullptr;
        napi_property_descriptor processDesc[] = {
            DECLARE_NAPI_FUNCTION("kill", KillSigOfProcess),
            DECLARE_NAPI_FUNCTION("exit", ExitOfProcess),
            DECLARE_NAPI_FUNCTION("isAppUid", IsAppUidOfProcess),
            DECLARE_NAPI_FUNCTION("getUidForName", GetUidForNameOfProcess),
            DECLARE_NAPI_FUNCTION("getThreadPriority", GetThreadPriorityOfProcess),
            DECLARE_NAPI_FUNCTION("getSystemConfig", GetSystemConfigOfProcess),
            DECLARE_NAPI_FUNCTION("getEnvironmentVar", GetEnvironmentVarOfProcess),
        };
        NAPI_CALL(env, napi_define_class(env, procssClassName, strlen(procssClassName), ProcessManagerConstructor,
                                         nullptr, sizeof(processDesc) / sizeof(processDesc[0]),
                                         processDesc, &processClass));
        napi_property_descriptor desc[] = {
            DECLARE_NAPI_PROPERTY("ProcessManager", processClass)
        };
        napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
        return exports;
    }
    static napi_value Init(napi_env env, napi_value exports)
    {
        Process object;
        object.SetRejectionCallback(env);
        napi_property_descriptor desc[] = {
            DECLARE_NAPI_FUNCTION("runCmd", RunCommand),
            DECLARE_NAPI_GETTER("uid", GetUid),
            DECLARE_NAPI_GETTER("gid", GetGid),
            DECLARE_NAPI_GETTER("euid", GetEUid),
            DECLARE_NAPI_GETTER("egid", GetEGid),
            DECLARE_NAPI_GETTER("groups", GetGroups),
            DECLARE_NAPI_GETTER("pid", GetPid),
            DECLARE_NAPI_GETTER("ppid", GetPpid),
            DECLARE_NAPI_FUNCTION("uptime", Uptime),
            DECLARE_NAPI_FUNCTION("kill", KillSig),
            DECLARE_NAPI_FUNCTION("chdir", Chdir),
            DECLARE_NAPI_FUNCTION("abort", Abort),
            DECLARE_NAPI_FUNCTION("cwd", Cwd),
            DECLARE_NAPI_FUNCTION("on", On),
            DECLARE_NAPI_FUNCTION("off", Off),
            DECLARE_NAPI_FUNCTION("exit", Exit),
            DECLARE_NAPI_GETTER("tid", GetTid),
            DECLARE_NAPI_FUNCTION("getStartRealtime", GetStartRealtime),
            DECLARE_NAPI_FUNCTION("getPastCpuTime",  GetPastCputime),
            DECLARE_NAPI_FUNCTION("isIsolatedProcess", IsIsolatedProcess),
            DECLARE_NAPI_FUNCTION("is64Bit", Is64Bit),
            DECLARE_NAPI_FUNCTION("isAppUid", IsAppUid),
            DECLARE_NAPI_FUNCTION("getUidForName", GetUidForName),
            DECLARE_NAPI_FUNCTION("getThreadPriority", GetThreadPriority),
            DECLARE_NAPI_FUNCTION("getSystemConfig", GetSystemConfig),
            DECLARE_NAPI_FUNCTION("getEnvironmentVar", GetEnvironmentVar),
        };
        NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
        ProcessInit(env, exports);
        napi_value obj = nullptr;
        NAPI_CALL(env, napi_create_object(env, &obj));

        NAPI_CALL(env, napi_wrap(
            env, obj, reinterpret_cast<void*>(Process::ClearReference),
            [](napi_env env, void* data, void* hint) {
                if (data != nullptr) {
                    ClearRefCallback clearParameters = reinterpret_cast<ClearRefCallback>(data);
                    clearParameters(env);
                }
            },
            nullptr, nullptr));
        NAPI_CALL(env, napi_set_named_property(env, exports, "obj", obj));

        return exports;
    }

    static napi_module processModule = {
        .nm_version = 1,
        .nm_flags = 0,
        .nm_filename = nullptr,
        .nm_register_func = Init,
        .nm_modname = "process",
        .nm_priv = reinterpret_cast<void*>(0),
        .reserved = { 0 },
    };

    extern "C" __attribute__ ((constructor)) void RegisterModule()
    {
        napi_module_register(&processModule);
    }
} // namespace OHOS::Js_sys_module::Process
