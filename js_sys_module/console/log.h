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

#ifndef JCONSOLE_LOG_H
#define JCONSOLE_LOG_H

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

#define HILOG_FATAL(fmt, ...) ((void)HILOG_IMPL(LOG_APP, LOG_FATAL, LOG_DOMAIN, LOG_TAG, fmt, ##__VA_ARGS__))
#define HILOG_ERROR(fmt, ...) ((void)HILOG_IMPL(LOG_APP, LOG_ERROR, LOG_DOMAIN, LOG_TAG, fmt, ##__VA_ARGS__))
#define HILOG_WARN(fmt, ...) ((void)HILOG_IMPL(LOG_APP, LOG_WARN, LOG_DOMAIN, LOG_TAG, fmt, ##__VA_ARGS__))
#define HILOG_INFO(fmt, ...) ((void)HILOG_IMPL(LOG_APP, LOG_INFO, LOG_DOMAIN, LOG_TAG, fmt, ##__VA_ARGS__))
#define HILOG_DEBUG(fmt, ...) ((void)HILOG_IMPL(LOG_APP, LOG_DEBUG, LOG_DOMAIN, LOG_TAG, fmt, ##__VA_ARGS__))

#endif /* JCONSOLE_LOG_H */
