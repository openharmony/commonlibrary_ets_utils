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

#include "commonlibrary/ets_utils/js_concurrent_module/worker/worker.h"
#include "commonlibrary/ets_utils/js_concurrent_module/worker/worker_new.h"

/*
 * module init func
 */
static napi_value Init(napi_env env, napi_value exports)
{
    exports = Commonlibrary::Concurrent::WorkerModule::NewWorker::InitWorker(env, exports);
    exports = Commonlibrary::Concurrent::WorkerModule::Worker::InitWorker(env, exports);

    return exports;
}

/*
 * module define
 */
static napi_module g_workerModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "worker",
    .nm_priv = reinterpret_cast<void*>(0),
    .reserved = { 0 },
};

/*
 * module register
 */
extern "C" __attribute__((constructor)) void Register()
{
    napi_module_register(&g_workerModule);
}