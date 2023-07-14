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

#ifndef JS_CONCURRENT_MODULE_COMMON_UTILS_H
#define JS_CONCURRENT_MODULE_COMMON_UTILS_H

namespace Commonlibrary::Concurrent::Common {
// for task priority and worker priority
enum Priority {
    HIGH = 0,
    MEDIUM,
    LOW,
    NUMBER = 3,
    DEFAULT = MEDIUM,
};
} // namespace Commonlibrary::Concurrent::Common
#endif // JS_CONCURRENT_MODULE_COMMON_UTILS_H