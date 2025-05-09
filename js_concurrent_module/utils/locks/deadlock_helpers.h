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

#ifndef JS_CONCURRENT_MODULE_UTILS_LOCKS_DEADLOCK_HELPERS_H
#define JS_CONCURRENT_MODULE_UTILS_LOCKS_DEADLOCK_HELPERS_H

#include "common.h"
#include "graph.h"
#include "async_lock_manager.h"

namespace Commonlibrary::Concurrent::LocksModule {

using LockGraph = Graph<tid_t, AsyncLockDependency>;
using DeadlockInfo = LockGraph::Path;

std::string CreateDeadlockWarningMessage(DeadlockInfo &&deadlock);
std::string CreateFullLockInfosMessage(tid_t targetTid, std::vector<AsyncLockDependency> &&dependencies,
                                       DeadlockInfo &&deadlock);
DeadlockInfo CheckDeadlocks(const std::vector<AsyncLockDependency> &dependencies);

}  // namespace Commonlibrary::Concurrent::LocksModule

#endif