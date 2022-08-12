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

#ifndef BLOB_JS_BLOB_H
#define BLOB_JS_BLOB_H

#include <numeric>
#include <string>
#include <vector>

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "utils/log.h"

namespace OHOS::buffer {
class Blob {
public:
    Blob() = default;
    virtual ~Blob();
    void Init(uint8_t *blob, unsigned int length);
    void Init(Blob *blob, int start);
    void Init(Blob *blob, int start, int end);

    uint8_t GetByte(int index);
    uint8_t *GetRaw();
    unsigned int GetLength();
    void ReadBytes(uint8_t *data, int length);
private:
    uint8_t *raw_ = nullptr;
    unsigned int length_ = 0;
};
} // namespace OHOS::buffer
#endif // BLOB_JS_BLOB_H