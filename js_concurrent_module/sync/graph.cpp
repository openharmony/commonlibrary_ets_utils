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

#include <iostream>
#include <sstream>


#include "graph.h"

namespace Commonlibrary::Concurrent::LocksModule {

template<class G>
std::ostream &operator<<(std::ostream &stream, typename G::VColor c)
{
    switch (c) {
        case G::VColor::WHITE:
            stream << "W";
            break;
        case G::VColor::BLACK:
            stream << "B";
            break;
        case G::VColor::GREY:
            stream << "G";
            break;
        default:
            stream << "?";
            break;
    }
    return stream;
}

}  // namespace Commonlibrary::Concurrent::LocksModule