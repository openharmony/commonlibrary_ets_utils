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

#ifndef JCONSOLE_LOG_H_
#define JCONSOLE_LOG_H_

#ifdef LINUX_PLATFORM
#include <cstring>
#include <cstdint>
#endif
#include <string>

#include "utils/macros.h"
#include "hilog/log.h"

#undef LOG_DOMAIN
#undef LOG_TAG
#undef HILOG_FATAL
#undef HILOG_ERROR
#undef HILOG_WARN
#undef HILOG_INFO
#undef HILOG_DEBUG

#define LOG_DOMAIN 0x3D00
#define LOG_TAG "JSAPP"


constexpr OHOS::HiviewDFX::HiLogLabel LOG_LABEL = { LOG_APP, LOG_DOMAIN, LOG_TAG };

#define HILOG_PRINT(Level, fmt, ...)     \
    (void)OHOS::HiviewDFX::HiLog::Level(LOG_LABEL, fmt, ##__VA_ARGS__)


#define HILOG_FATAL(fmt, ...) HILOG_PRINT(Fatal, fmt, ##__VA_ARGS__)
#define HILOG_ERROR(fmt, ...) HILOG_PRINT(Error, fmt, ##__VA_ARGS__)
#define HILOG_WARN(fmt, ...) HILOG_PRINT(Warn, fmt, ##__VA_ARGS__)
#define HILOG_INFO(fmt, ...) HILOG_PRINT(Info, fmt, ##__VA_ARGS__)
#define HILOG_DEBUG(fmt, ...) HILOG_PRINT(Debug, fmt, ##__VA_ARGS__)

#endif /* JCONSOLE_LOG_H_ */
