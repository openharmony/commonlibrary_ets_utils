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

#if !defined(__ARKUI_CROSS__)
#include "hitrace_meter.h"

#define HITRACE_HELPER_METER_NAME(msg) HITRACE_METER_NAME(HITRACE_TAG_COMMONLIBRARY, msg)
#define HITRACE_HELPER_START_TRACE(msg) StartTrace(HITRACE_TAG_COMMONLIBRARY, msg)
#define HITRACE_HELPER_FINISH_TRACE FinishTrace(HITRACE_TAG_COMMONLIBRARY)
#define HITRACE_HELPER_COUNT_TRACE(msg, count) CountTrace(HITRACE_TAG_COMMONLIBRARY, msg, count)
#else
#define HITRACE_HELPER_METER_NAME(msg)
#define HITRACE_HELPER_START_TRACE(msg)
#define HITRACE_HELPER_FINISH_TRACE
#define HITRACE_HELPER_COUNT_TRACE(msg, count)
#endif