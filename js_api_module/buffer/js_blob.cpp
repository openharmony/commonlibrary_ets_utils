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

#include "commonlibrary/ets_utils/js_api_module/buffer/js_blob.h"
#include "securec.h"
using namespace std;

namespace OHOS::buffer {
void Blob::Init(uint8_t *blob, unsigned int length)
{
    if (blob != nullptr) {
        this->raw_ = reinterpret_cast<uint8_t *>(malloc(length));
        if (raw_ == nullptr) {
            HILOG_FATAL("Blob constructor malloc failed");
        } else {
            this->length_ = length;
            if (memcpy_s(raw_, length, blob, length) != EOK) {
                HILOG_FATAL("Blob constructor(length) memcpy_s failed");
            }
        }
    }
}

void Blob::Init(Blob *blob, int start)
{
    if (blob != nullptr) {
        this->raw_ = reinterpret_cast<uint8_t *>(malloc(blob->length_));
        if (raw_ == nullptr) {
            HILOG_FATAL("Blob constructor malloc failed");
        } else {
            this->length_ = blob->length_;
            if (memcpy_s(raw_, blob->length_ - start, blob->raw_ + start, blob->length_ - start) != EOK) {
                HILOG_FATAL("Blob constructor(start) memcpy_s failed");
            }
        }
    }
}

void Blob::Init(Blob *blob, int start, int end)
{
    if (start > end) {
        return;
    }
    if ((start > 0 && end < 0) || (start < 0 && end > 0)) {
        return;
    }
    if (blob == nullptr) {
        return;
    }
    unsigned int length = static_cast<unsigned int>(end - start);
    length = blob->length_ > length ? length : blob->length_;
    this->raw_ = reinterpret_cast<uint8_t *>(malloc(length));
    if (raw_ == nullptr) {
        HILOG_FATAL("Blob constructor malloc failed");
    } else {
        this->length_ = length;
        if (start >= 0) {
            if (memcpy_s(this->raw_, length, blob->raw_ + start, length) != EOK) {
                HILOG_FATAL("Blob constructor(start >= 0, end) memcpy_s failed");
            }
        } else {
            if (memcpy_s(raw_, length, blob->raw_ + blob->length_ + start, length) != EOK) {
                HILOG_FATAL("Blob constructor(start, end) memcpy_s failed");
            }
        }
    }
}

Blob::~Blob()
{
    if (raw_ != nullptr) {
        free(raw_);
        raw_ = nullptr;
    }
}

uint8_t Blob::GetByte(int index)
{
    return *(this->raw_ + index);
}

uint8_t *Blob::GetRaw()
{
    return this->raw_;
}

unsigned int Blob::GetLength()
{
    return this->length_;
}

void Blob::ReadBytes(uint8_t *data, int length)
{
    if (memcpy_s(data, length, raw_, length) != EOK) {
        HILOG_FATAL("read bytes from blob error");
    }
}
} // namespace OHOS::buffer