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

#include <sstream>

#include "deadlock_helpers.h"
#include "common.h"
#include "graph.h"

namespace Commonlibrary::Concurrent::LocksModule {

std::string CreateDeadlockWarningMessage(DeadlockInfo &&deadlock)
{
    std::stringstream s;
    auto vertexPrinter = [](tid_t tid) { return "TID " + std::to_string(tid); };
    auto edgePrinter = [](const AsyncLockDependency *edata) { return " <-- lock {" + edata->name + "} -- WAITED BY "; };
    s << "!!! DEADLOCK WARNING !!!\n"
      << LockGraph::CycleAsString(deadlock, "Possible deadlock: ", "\n", vertexPrinter, edgePrinter);
    return s.str();
}

std::string CreateFullLockInfosMessage(tid_t targetTid, std::vector<AsyncLockDependency> &&dependencies,
                                       DeadlockInfo &&deadlock)
{
    std::stringstream s;
    s << "\nThread's async locks information:\n";
    for (auto &maybeHeld : dependencies) {
        if ((maybeHeld.holderTid == targetTid) && (maybeHeld.waiterTid == INVALID_TID)) {
            s << "HELD: lock {" + maybeHeld.name + "} , taken at:\n" + maybeHeld.creationStacktrace + "\n";
        }
    }
    for (auto &maybeWaiting : dependencies) {
        if (maybeWaiting.waiterTid == targetTid) {
            s << "WAITING: lock {" + maybeWaiting.name + "} , taken at:\n" + maybeWaiting.creationStacktrace + "\n";
        }
    }
    if (!deadlock.IsEmpty()) {
        s << CreateDeadlockWarningMessage(std::move(deadlock));
    } else {
        s << "No deadlocks detected.\n";
    }
    return s.str();
}

DeadlockInfo CheckDeadlocks(const std::vector<AsyncLockDependency> &dependencies)
{
    LockGraph::AdjacencyList adjlist;
    for (auto &dep : dependencies) {
        if (dep.waiterTid == INVALID_TID) {
            // this is the "held only" edge, skip
            continue;
        }
        adjlist.push_back(std::make_tuple(dep.waiterTid, dep.holderTid, &dep));
    }
    LockGraph g(std::move(adjlist));
    return g.FindFirstCycle();
}

}  // namespace Commonlibrary::Concurrent::LocksModule