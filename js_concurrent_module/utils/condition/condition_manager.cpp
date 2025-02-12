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

#include "condition_manager.h"
#include "condition_variable.h"
#include "helper/error_helper.h"

namespace Commonlibrary::Concurrent::Condition {

static thread_local napi_ref conditionClassRef = nullptr;

napi_value ConditionManager::Init(napi_env env, napi_value exports)
{
    napi_value locks;
    bool hasLocks {false};
    napi_has_named_property(env, exports, "locks", &hasLocks);
    if (hasLocks) {
        napi_get_named_property(env, exports, "locks", &locks);
    } else {
        napi_create_object(env, &locks);
        napi_set_named_property(env, exports, "locks", locks);
    }

    napi_property_descriptor props[] = {
        DECLARE_NAPI_INSTANCE_OBJECT_PROPERTY("wait"),      DECLARE_NAPI_INSTANCE_OBJECT_PROPERTY("waitFor"),
        DECLARE_NAPI_INSTANCE_OBJECT_PROPERTY("notifyAll"), DECLARE_NAPI_INSTANCE_OBJECT_PROPERTY("notifyOne"),
        DECLARE_NAPI_STATIC_FUNCTION("request", Request),
    };
    napi_value ConditionVariableClass;
    napi_define_sendable_class(env, "ConditionVariable", NAPI_AUTO_LENGTH, Constructor, nullptr,
                               sizeof(props) / sizeof(props[0]), props, nullptr, &ConditionVariableClass);
    napi_create_reference(env, ConditionVariableClass, 1, &conditionClassRef);
    napi_set_named_property(env, locks, "ConditionVariable", ConditionVariableClass);

    return exports;
}

napi_value ConditionManager::Wait(napi_env env, napi_callback_info cbinfo)
{
    napi_value thisVar;
    napi_get_cb_info(env, cbinfo, nullptr, nullptr, &thisVar, nullptr);

    ConditionVariable *cond;
    napi_unwrap_sendable(env, thisVar, reinterpret_cast<void **>(&cond));
    napi_deferred deferred;
    napi_value promise;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));

    cond->AddTask(env, deferred);

    return promise;
}

napi_value ConditionManager::WaitFor(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = 1;
    napi_value args;
    napi_value thisVar;
    napi_get_cb_info(env, cbinfo, &argc, &args, &thisVar, nullptr);

    if (argc != 1) {
        Common::Helper::ErrorHelper::ThrowError(env, Common::Helper::ErrorHelper::TYPE_ERROR,
                                                "Invalid number of arguments");
        return nullptr;
    }

    int32_t timeout {0};
    napi_status status = napi_get_value_int32(env, args, &timeout);
    if (status != napi_ok) {
        Common::Helper::ErrorHelper::ThrowError(env, Common::Helper::ErrorHelper::TYPE_ERROR,
                                                "Invalid argument type. Expected number");
        return nullptr;
    }

    ConditionVariable *cond;
    napi_unwrap_sendable(env, thisVar, reinterpret_cast<void **>(&cond));
    napi_deferred deferred;
    napi_value promise;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));

    if (timeout < 0) {
        timeout = 0;
    }
    cond->AddTask(env, deferred, timeout);

    return promise;
}

napi_value ConditionManager::NotifyAll(napi_env env, napi_callback_info cbinfo)
{
    napi_value thisVar;
    napi_get_cb_info(env, cbinfo, nullptr, nullptr, &thisVar, nullptr);

    ConditionVariable *cond;
    napi_unwrap_sendable(env, thisVar, reinterpret_cast<void **>(&cond));

    cond->FinishTask(true);
    return nullptr;
}

napi_value ConditionManager::NotifyOne(napi_env env, napi_callback_info cbinfo)
{
    napi_value thisVar;
    napi_get_cb_info(env, cbinfo, nullptr, nullptr, &thisVar, nullptr);

    ConditionVariable *cond;
    napi_unwrap_sendable(env, thisVar, reinterpret_cast<void **>(&cond));

    cond->FinishTask();
    return nullptr;
}

napi_value ConditionManager::Constructor(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = Common::Helper::NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    NAPI_ASSERT(env, argc == 0 || argc == 1, "AsyncLock::Constructor: the number of params must be zero or one");
    auto args = std::make_unique<napi_value[]>(argc);
    napi_value thisVar;
    napi_get_cb_info(env, cbinfo, &argc, args.get(), &thisVar, nullptr);

    ConditionVariable *cond = nullptr;
    if (argc == 1) {
        napi_valuetype type;
        NAPI_CALL(env, napi_typeof(env, args[0], &type));
        if (type != napi_string) {
            Common::Helper::ErrorHelper::ThrowError(env, Common::Helper::ErrorHelper::TYPE_ERROR,
                                                    "Request:: param must be string");
            return nullptr;
        }

        std::string condName = Common::Helper::NapiHelper::GetString(env, args[0]);
        cond = ConditionVariable::FindOrCreateCondition(condName);
    } else {
        cond = new ConditionVariable();
    }

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION_WITH_DATA("wait", Wait, thisVar),
        DECLARE_NAPI_FUNCTION_WITH_DATA("waitFor", WaitFor, thisVar),
        DECLARE_NAPI_FUNCTION_WITH_DATA("notifyAll", NotifyAll, thisVar),
        DECLARE_NAPI_FUNCTION_WITH_DATA("notifyOne", NotifyOne, thisVar),
    };

    NAPI_CALL(env, napi_define_properties(env, thisVar, sizeof(properties) / sizeof(properties[0]), properties));
    napi_wrap_sendable(env, thisVar, reinterpret_cast<void *>(cond), ConditionManager::Destructor, nullptr);
    return thisVar;
}

napi_value ConditionManager::Request(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = Common::Helper::NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    NAPI_ASSERT(env, argc == 1, "Request:: the number of params must be one");
    auto args = std::make_unique<napi_value[]>(argc);
    NAPI_CALL(env, napi_get_cb_info(env, cbinfo, &argc, args.get(), nullptr, nullptr));
    napi_value conditionClass;
    NAPI_CALL(env, napi_get_reference_value(env, conditionClassRef, &conditionClass));
    napi_value instance;
    NAPI_CALL(env, napi_new_instance(env, conditionClass, argc, args.get(), &instance));

    return instance;
}

void ConditionManager::Destructor(napi_env env, void *nativeObject, void *finalizeHint)
{
    auto cond = reinterpret_cast<ConditionVariable *>(nativeObject);
    cond->TryRemoveCondition();
}

}  // namespace Commonlibrary::Concurrent::Condition
