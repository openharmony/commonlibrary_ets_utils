/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "qos_helper.h"

#include <map>

#ifdef ENABLE_QOS
#include "qos.h"
#endif
#include "tools/log.h"

namespace Commonlibrary::Platform {

using OHOS::QOS::QosLevel;

#ifdef ENABLE_QOS
static const std::map<Priority, QosLevel> WORKERPRIORITY_QOSLEVEL_MAP = {
    {Priority::LOW, OHOS::QOS::QosLevel::QOS_UTILITY},
    {Priority::DEFAULT, OHOS::QOS::QosLevel::QOS_DEFAULT},
    {Priority::HIGH, OHOS::QOS::QosLevel::QOS_USER_INITIATED}
};

int SetWorkerPriority([[maybe_unused]] Priority priority)
{
    return SetThreadQos(WORKERPRIORITY_QOSLEVEL_MAP.at(priority));
}
#else
int SetWorkerPriority(Priority priority)
{
    HILOG_DEBUG("SetWorkerPriority:: not supported currently.");
    return 0;
}
#endif
} // namespace Commonlibrary::Platform
