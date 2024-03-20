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

#include <unistd.h>
#include <sys/types.h>

#include "async_lock_manager.h"
#include "deadlock_helpers.h"
#include "async_lock.h"
#include "helper/error_helper.h"
#include "helper/napi_helper.h"
#include "helper/object_helper.h"
#include "utils/log.h"

namespace Commonlibrary::Concurrent::LocksModule {
using namespace Commonlibrary::Concurrent::Common::Helper;

static thread_local napi_ref asyncLockClassRef = nullptr;

std::mutex AsyncLockManager::lockMutex;
std::unordered_map<std::string, AsyncLock *> AsyncLockManager::lockMap = {};
std::unordered_map<uint32_t, AsyncLock *> AsyncLockManager::anonymousLockMap = {};
std::atomic<uint32_t> AsyncLockManager::nextId = 1;

void AsyncLockManager::CollectLockDependencies(std::vector<AsyncLockDependency> &dependencies)
{
    auto lockProcessor = [&dependencies](std::string lockName, AsyncLock *lock) {
        auto holderInfos = lock->GetSatisfiedRequestInfos();
        if (holderInfos.empty()) {
            // lock should have holders to be waited, skip
            return;
        }
        auto holderTid = holderInfos[0].tid;
        dependencies.push_back(
            AsyncLockDependency {INVALID_TID, holderTid, lockName, holderInfos[0].creationStacktrace});
        for (auto &waiterInfo : lock->GetPendingRequestInfos()) {
            dependencies.push_back(
                AsyncLockDependency {waiterInfo.tid, holderTid, lockName, waiterInfo.creationStacktrace});
        }
    };
    std::unique_lock<std::mutex> guard(lockMutex);
    for (auto [name, lock] : lockMap) {
        lockProcessor(name, lock);
    }
    for (auto [id, lock] : anonymousLockMap) {
        std::string lockName = "anonymous #" + std::to_string(id);
        lockProcessor(lockName, lock);
    }
}

void AsyncLockManager::DumpLocksInfoForThread(tid_t targetTid, std::string &result)
{
    std::vector<AsyncLockDependency> deps;
    CollectLockDependencies(deps);
    auto deadlock = CheckDeadlocks(deps);
    result = CreateFullLockInfosMessage(targetTid, std::move(deps), std::move(deadlock));
}

void AsyncLockManager::CheckDeadlocksAndLogWarning()
{
    std::vector<AsyncLockDependency> deps;
    CollectLockDependencies(deps);
    auto deadlock = CheckDeadlocks(deps);
    if (!deadlock.IsEmpty()) {
        std::string warning = CreateDeadlockWarningMessage(std::move(deadlock));
        HILOG_WARN("DeadlockDetector: %{public}s", warning.c_str());
    }
}

napi_value AsyncLockManager::Init(napi_env env, napi_value exports)
{
    // AsyncLock class
    napi_value requestFunc = nullptr;
    napi_create_function(env, "request", NAPI_AUTO_LENGTH, Request, nullptr, &requestFunc);

    napi_value queryFunc = nullptr;
    napi_create_function(env, "query", NAPI_AUTO_LENGTH, Query, nullptr, &queryFunc);
    napi_value queryAllFunc = nullptr;
    napi_create_function(env, "queryAll", NAPI_AUTO_LENGTH, QueryAll, nullptr, &queryAllFunc);

    napi_property_descriptor props[] = {
        DECLARE_NAPI_PROPERTY("request", requestFunc),
        DECLARE_NAPI_PROPERTY("query", queryFunc),
        DECLARE_NAPI_PROPERTY("queryAll", queryAllFunc),
    };
    napi_value asyncLockManagerClass = nullptr;
    napi_define_sendable_class(env, "AsyncLock", NAPI_AUTO_LENGTH, Constructor, nullptr,
                               sizeof(props) / sizeof(props[0]), props, nullptr, &asyncLockManagerClass);

    // AsyncLockMode enum
    napi_value asyncLockMode = NapiHelper::CreateObject(env);
    napi_value sharedMode = NapiHelper::CreateUint32(env, LOCK_MODE_SHARED);
    napi_value exclusiveMode = NapiHelper::CreateUint32(env, LOCK_MODE_EXCLUSIVE);
    napi_property_descriptor exportMode[] = {
        DECLARE_NAPI_PROPERTY("SHARED", sharedMode),
        DECLARE_NAPI_PROPERTY("EXCLUSIVE", exclusiveMode),
    };
    napi_define_properties(env, asyncLockMode, sizeof(exportMode) / sizeof(exportMode[0]), exportMode);

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_PROPERTY("AsyncLock", asyncLockManagerClass),
        DECLARE_NAPI_PROPERTY("AsyncLockMode", asyncLockMode),
    };
    napi_define_properties(env, exports, sizeof(properties) / sizeof(properties[0]), properties);
    NAPI_CALL(env, napi_create_reference(env, asyncLockManagerClass, 1, &asyncLockClassRef));

    return exports;
}

napi_value AsyncLockManager::Constructor(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    NAPI_ASSERT(env, argc == 0, "Constructor:: the number of params must be zero");

    auto args = std::make_unique<napi_value[]>(argc);
    napi_value thisVar;
    NAPI_CALL(env, napi_get_cb_info(env, cbinfo, &argc, args.get(), &thisVar, nullptr));

    uint32_t lockId = nextId++;
    Request(lockId);

    napi_value name;
    std::ostringstream out;
    out << "anonymousLock" << lockId;
    std::string lockName = out.str();
    NAPI_CALL(env, napi_create_string_utf8(env, lockName.c_str(), NAPI_AUTO_LENGTH, &name));

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_PROPERTY("name", name),
        DECLARE_NAPI_FUNCTION_WITH_DATA("lockAsync", LockAsync, thisVar),
    };
    NAPI_CALL(env, napi_define_properties(env, thisVar, sizeof(properties) / sizeof(properties[0]), properties));

    AsyncLockIdentity *data = new AsyncLockIdentity {true, lockId};
    NAPI_CALL(env, napi_wrap(env, thisVar, data, Destructor, nullptr, nullptr));

    return thisVar;
}

napi_value AsyncLockManager::Request(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    NAPI_ASSERT(env, argc == 1, "Request:: the number of params must be one");

    auto args = std::make_unique<napi_value[]>(argc);
    NAPI_CALL(env, napi_get_cb_info(env, cbinfo, &argc, args.get(), nullptr, nullptr));
    napi_value asyncLockClass;
    NAPI_CALL(env, napi_get_reference_value(env, asyncLockClassRef, &asyncLockClass));
    napi_value instance;
    NAPI_CALL(env, napi_create_object(env, &instance));

    napi_valuetype type;
    NAPI_CALL(env, napi_typeof(env, args[0], &type));
    if (type != napi_string) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "Request:: param must be string");
        return nullptr;
    }

    std::string name = NapiHelper::GetString(env, args[0]);
    Request(name);

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_PROPERTY("name", args[0]),
        DECLARE_NAPI_FUNCTION_WITH_DATA("lockAsync", LockAsync, instance),
    };
    NAPI_CALL(env, napi_define_properties(env, instance, sizeof(properties) / sizeof(properties[0]), properties));

    AsyncLockIdentity *data = new AsyncLockIdentity {false, 0, name};
    NAPI_CALL(env, napi_wrap(env, instance, data, Destructor, nullptr, nullptr));

    return instance;
}

void AsyncLockManager::Destructor(napi_env env, void *data, [[maybe_unused]] void *hint)
{
    // NOTE: need to count references to AsyncLock, because it might be deleted even if
    // there are pending LockRequests
    AsyncLockIdentity *identity = reinterpret_cast<AsyncLockIdentity *>(data);
    std::unique_lock<std::mutex> guard(lockMutex);
    if (identity->isAnonymous) {
        delete anonymousLockMap.at(identity->id);
        anonymousLockMap.erase(identity->id);
    } else {
        delete lockMap.at(identity->name);
        lockMap.erase(identity->name);
    }
    delete identity;
}

napi_value AsyncLockManager::LockAsync(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    NAPI_ASSERT(env, 0 < argc && argc < 4U, "Invalid number of arguments");

    auto argv = std::make_unique<napi_value[]>(argc);
    napi_value thisVar;
    NAPI_CALL(env, napi_get_cb_info(env, cbinfo, &argc, argv.get(), &thisVar, nullptr));

    AsyncLockIdentity *id;
    NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void **>(&id)));

    AsyncLock *asyncLock = nullptr;
    {
        std::unique_lock<std::mutex> guard(lockMutex);
        asyncLock = FindAsyncLock(id);
    }
    if (asyncLock == nullptr) {
        // UDAV: Fix error type
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "Internal error: no such lock");
        napi_value undefined;
        napi_get_undefined(env, &undefined);
        return undefined;
    }
    LockMode mode = LOCK_MODE_EXCLUSIVE;
    LockOptions options;
    if (argc > 1 && !GetLockMode(env, argv[1], mode)) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "Invalid lock mode.");
        return nullptr;
    }
    if (argc > 2U && !GetLockOptions(env, argv[2U], options)) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "Invalid options.");
        return nullptr;
    }
    napi_ref callback;
    napi_create_reference(env, argv[0], 1, &callback);
    return asyncLock->LockAsync(env, callback, mode, options);
}

napi_value AsyncLockManager::Query(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    if (argc != 1) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "Invalid number of arguments");
        return nullptr;
    }

    // later on we can decide to cache the check result if needed
    CheckDeadlocksAndLogWarning();

    napi_value undefined;
    napi_get_undefined(env, &undefined);
    napi_value arg;
    NAPI_CALL(env, napi_get_cb_info(env, cbinfo, &argc, &arg, nullptr, nullptr));
    napi_valuetype type;
    napi_typeof(env, arg, &type);
    if (type != napi_string) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "Invalid argument type");
        return undefined;
    }

    std::string name = NapiHelper::GetString(env, arg);
    AsyncLockIdentity identity{false, 0, name};
    AsyncLock *lock = FindAsyncLock(&identity);
    if (lock == nullptr) {
        ErrorHelper::ThrowError(env, ErrorHelper::ERR_NO_SUCH_ASYNCLOCK, "No such lock");
        return undefined;
    }

    return CreateLockState(env, lock);
}

napi_value AsyncLockManager::QueryAll(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = NapiHelper::GetCallbackInfoArgc(env, cbinfo);
    if (argc != 0) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "Invalid number of arguments");
        return nullptr;
    }

    // later on we can decide to cache the check result if needed
    CheckDeadlocksAndLogWarning();
    return CreateLockStates(env, [] ([[maybe_unused]] const AsyncLockIdentity &identity) {
        return true;
    });
}

napi_value AsyncLockManager::CreateLockState(napi_env env, AsyncLock *asyncLock)
{
    napi_value undefined;
    napi_get_undefined(env, &undefined);
    napi_value result;
    if (napi_create_object(env, &result) != napi_ok) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "Cannot create an object");
        return undefined;
    }
    napi_value held;
    napi_value pending;
    if (napi_create_array(env, &held) != napi_ok) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "Cannot create an object");
        return undefined;
    }
    if (napi_create_array(env, &pending) != napi_ok) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "Cannot create an object");
        return undefined;
    }
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_PROPERTY("held", held),
        DECLARE_NAPI_PROPERTY("pending", pending),
    };
    NAPI_CALL(env, napi_define_properties(env, result, sizeof(properties) / sizeof(properties[0]), properties));

    if (asyncLock->FillLockState(env, held, pending) != napi_ok) {
        ErrorHelper::ThrowError(env, ErrorHelper::TYPE_ERROR, "Cannot create an object");
        return undefined;
    }

    return result;
}

napi_value AsyncLockManager::CreateLockStates(napi_env env,
    const std::function<bool(const AsyncLockIdentity& identity)> &pred)
{
    bool pendingException = false;
    napi_value undefined;
    napi_get_undefined(env, &undefined);
    napi_value array;
    NAPI_CALL(env, napi_create_array(env, &array));

    std::unique_lock<std::mutex> guard(lockMutex);
    uint32_t idx = 0;
    for (auto &entry : anonymousLockMap) {
        AsyncLockIdentity identity = {true, entry.first, ""};
        if (pred(identity)) {
            napi_value v = CreateLockState(env, entry.second);
            napi_is_exception_pending(env, &pendingException);
            if (pendingException) {
                return undefined;
            }
            napi_value index;
            NAPI_CALL(env, napi_create_uint32(env, idx, &index));
            NAPI_CALL(env, napi_set_property(env, array, index, v));
            ++idx;
        }
    }
    for (auto &entry : lockMap) {
        AsyncLockIdentity identity = {false, 0, entry.first};
        if (pred(identity)) {
            napi_value v = CreateLockState(env, entry.second);
            napi_is_exception_pending(env, &pendingException);
            if (pendingException) {
                return undefined;
            }
            napi_value index;
            NAPI_CALL(env, napi_create_uint32(env, idx, &index));
            NAPI_CALL(env, napi_set_property(env, array, index, v));
            ++idx;
        }
    }
    return array;
}

AsyncLock *AsyncLockManager::Request(uint32_t id)
{
    std::unique_lock<std::mutex> guard(lockMutex);
    AsyncLockIdentity identity{true, id, ""};
    AsyncLock *lock = FindAsyncLock(&identity);
    if (lock == nullptr) {
        lock = new AsyncLock(id);
        anonymousLockMap.emplace(id, lock);
    }

    return lock;
}

AsyncLock *AsyncLockManager::Request(const std::string &name)
{
    std::unique_lock<std::mutex> guard(lockMutex);
    AsyncLockIdentity identity{false, 0, name};
    AsyncLock *lock = FindAsyncLock(&identity);
    if (lock == nullptr) {
        lock = new AsyncLock(name);
        lockMap.emplace(name, lock);
    }

    return lock;
}

AsyncLock* AsyncLockManager::FindAsyncLock(AsyncLockIdentity *id)
{
    if (id->isAnonymous) {
        auto it = anonymousLockMap.find(id->id);
        if (it == anonymousLockMap.end()) {
            return nullptr;
        }
        return it->second;
    } else {
        auto it = lockMap.find(id->name);
        if (it == lockMap.end()) {
            return nullptr;
        }
        return it->second;
    }
}

bool AsyncLockManager::GetLockMode(napi_env env, napi_value val, LockMode &mode)
{
    int32_t modeNative = NapiHelper::GetUint32Value(env, val);
    if (modeNative  < LockMode::LOCK_MODE_SHARED || modeNative > LOCK_MODE_EXCLUSIVE) {
        return false;
    }
    mode = static_cast<LockMode>(modeNative);
    return true;
}

bool AsyncLockManager::GetLockOptions(napi_env env, napi_value val, LockOptions &options)
{
    napi_value isAvailable = NapiHelper::GetNameProperty(env, val, "isAvailable");
    napi_value signal = NapiHelper::GetNameProperty(env, val, "signal");
    napi_value timeout = NapiHelper::GetNameProperty(env, val, "timeout");
    if (isAvailable != nullptr) {
        options.isAvailable = NapiHelper::GetBooleanValue(env, isAvailable);
    }
    if (signal != nullptr) {
        napi_create_reference(env, signal, 1, &options.signal);
    }
    if (timeout != nullptr) {
        options.timeoutMillis = NapiHelper::GetUint32Value(env, timeout);
    }
    return true;
}

tid_t AsyncLockManager::GetCurrentTid()
{
    return static_cast<tid_t>(gettid());
}
}  // namespace Commonlibrary::Concurrent::LocksModule
