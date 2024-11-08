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

#ifndef JS_CONCURRENT_MODULE_UTILS_LOCKS_WEAK_WRAP_H
#define JS_CONCURRENT_MODULE_UTILS_LOCKS_WEAK_WRAP_H

#include <memory>

namespace Commonlibrary::Concurrent::LocksModule {

template <typename T>
class WeakWrap {
public:
    WeakWrap(std::weak_ptr<T> ptr) : ptr_{std::move(ptr)} {}
    std::weak_ptr<T> GetWeakPtr() const
    {
        return ptr_;
    }

private:
    std::weak_ptr<T> ptr_;
};

}  // namespace Commonlibrary::Concurrent::LocksModule

#endif
