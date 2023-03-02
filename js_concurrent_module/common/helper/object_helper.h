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

#ifndef JS_CONCURRENT_MODULE_COMMON_HELPER_OBJECT_HELPER_H_
#define JS_CONCURRENT_MODULE_COMMON_HELPER_OBJECT_HELPER_H_

namespace Commonlibrary::Concurrent::Common::Helper {
class DereferenceHelp {
public:
    template<typename Inner, typename Outer>
    static Outer* DereferenceOf(const Inner Outer::*field, const Inner* pointer)
    {
        if (field != nullptr && pointer != nullptr) {
            auto fieldOffset = reinterpret_cast<uintptr_t>(&(static_cast<Outer*>(0)->*field));
            auto outPointer = reinterpret_cast<Outer*>(reinterpret_cast<uintptr_t>(pointer) - fieldOffset);
            return outPointer;
        }
        return nullptr;
    }
};

class CloseHelp {
public:
    template<typename T>
    static void DeletePointer(const T* value, bool isArray)
    {
        if (value == nullptr) {
            return;
        }
        if (isArray) {
            delete[] value;
        } else {
            delete value;
            value = nullptr;
        }
    }
};

template<typename T>
class ObjectScope {
public:
    ObjectScope(const T* data, bool isArray) : data_(data), isArray_(isArray) {}
    ~ObjectScope()
    {
        if (data_ == nullptr) {
            return;
        }
        if (isArray_) {
            delete[] data_;
        } else {
            delete data_;
            data_ = nullptr;
        }
    }

private:
    const T* data_;
    bool isArray_;
};
} // namespace Commonlibrary::Concurrent::Common::Helper
#endif // JS_CONCURRENT_MODULE_COMMON_HELPER_OBJECT_HELPER_H_
