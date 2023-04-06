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

#include "js_process.h"

#include <cstdlib>
#include <vector>

#include <grp.h>
#include <pthread.h>
#include <pwd.h>
#include <sched.h>
#include <unistd.h>
#include <uv.h>

#include <sys/resource.h>
#include <sys/syscall.h>

#ifdef IOS_PLATFORM
#include <sys/sysctl.h>
#else
#include <sys/sysinfo.h>
#endif

#include <sys/types.h>

#include "securec.h"
#include "utils/log.h"
namespace OHOS::JsSysModule::Process {

#ifdef IOS_PLATFORM
enum class SYSArgUnix : uint8_t {
    SC_ARG_MAX = 0,
    SC_CHILD_MAX = 1,
    SC_CLK_TCK = 2,
    SC_NGROUPS_MAX = 3,
    SC_OPEN_MAX = 4,
    SC_STREAM_MAX = 5,
    SC_TZNAME_MAX = 6,
    SC_JOB_CONTROL = 7,
    SC_SAVED_IDS = 8,
    SC_REALTIME_SIGNALS = 9,
    SC_PRIORITY_SCHEDULING = 10,
    SC_TIMERS = 11,
    SC_ASYNCHRONOUS_IO = 12,
    SC_PRIORITIZED_IO = 13,
    SC_SYNCHRONIZED_IO = 14,
    SC_FSYNC = 15,
    SC_MAPPED_FILES = 16,
    SC_MEMLOCK = 17,
    SC_MEMLOCK_RANGE = 18,
    SC_MEMORY_PROTECTION = 19,
    SC_MESSAGE_PASSING = 20,
    SC_SEMAPHORES = 21,
    SC_SHARED_MEMORY_OBJECTS = 22,
    SC_AIO_LISTIO_MAX = 23,
    SC_AIO_MAX = 24,
    SC_AIO_PRIO_DELTA_MAX = 25,
    SC_DELAYTIMER_MAX = 26,
    SC_MQ_OPEN_MAX = 27,
    SC_MQ_PRIO_MAX = 28,
    SC_VERSION = 29,
    SC_PAGESIZE = 30,  // same with _SC_PAGE_SIZE in unix
    SC_RTSIG_MAX = 31,
    SC_SEM_NSEMS_MAX = 32,
    SC_SEM_VALUE_MAX = 33,
    SC_SIGQUEUE_MAX = 34,
    SC_TIMER_MAX = 35,
    SC_BC_BASE_MAX = 36,
    SC_BC_DIM_MAX = 37,
    SC_BC_SCALE_MAX = 38,
    SC_BC_STRING_MAX = 39,
    SC_COLL_WEIGHTS_MAX = 40,
    SC_EXPR_NEST_MAX = 42,
    SC_LINE_MAX = 43,
    SC_RE_DUP_MAX = 44,
    SC_2_VERSION = 46,
    SC_2_C_BIND = 47,
    SC_2_C_DEV = 48,
    SC_2_FORT_DEV = 49,
    SC_2_FORT_RUN = 50,
    SC_2_SW_DEV = 51,
    SC_2_LOCALEDEF = 52,
    SC_IOV_MAX = 60,   //  same with _SC_UIO_MAXIOV in unix
    SC_THREADS = 67,
    SC_THREAD_SAFE_FUNCTIONS = 68,
    SC_GETGR_R_SIZE_MAX = 69,
    SC_GETPW_R_SIZE_MAX = 70,
    SC_LOGIN_NAME_MAX = 71,
    SC_TTY_NAME_MAX = 72,
    SC_THREAD_DESTRUCTOR_ITERATIONS = 73,
    SC_THREAD_KEYS_MAX = 74,
    SC_THREAD_STACK_MIN = 75,
    SC_THREAD_THREADS_MAX = 76,
    SC_THREAD_ATTR_STACKADDR = 77,
    SC_THREAD_ATTR_STACKSIZE = 78,
    SC_THREAD_PRIORITY_SCHEDULING = 79,
    SC_THREAD_PRIO_INHERIT = 80,
    SC_THREAD_PRIO_PROTECT = 81,
    SC_THREAD_PROCESS_SHARED = 82,
    SC_NPROCESSORS_CONF = 83,
    SC_NPROCESSORS_ONLN = 84,
    SC_PHYS_PAGES = 85,
    SC_AVPHYS_PAGES = 86,
    SC_ATEXIT_MAX = 87,
    SC_PASS_MAX = 88,
    SC_XOPEN_VERSION = 89,
    SC_XOPEN_XCU_VERSION = 90,
    SC_XOPEN_UNIX = 91,
    SC_XOPEN_CRYPT = 92,
    SC_XOPEN_ENH_I18N = 93,
    SC_XOPEN_SHM = 94,
    SC_2_CHAR_TERM = 95,
    SC_2_UPE = 97,
    SC_XOPEN_XPG2 = 98,
    SC_XOPEN_XPG3 = 99,
    SC_XOPEN_XPG4 = 100,
    SC_NZERO = 109,
    SC_XBS5_ILP32_OFF32 = 125,
    SC_XBS5_ILP32_OFFBIG = 126,
    SC_XBS5_LP64_OFF64 = 127,
    SC_XBS5_LPBIG_OFFBIG = 128,
    SC_XOPEN_LEGACY = 129,
    SC_XOPEN_REALTIME = 130,
    SC_XOPEN_REALTIME_THREADS = 131,
    SC_ADVISORY_INFO = 132,
    SC_BARRIERS = 133,
    SC_CLOCK_SELECTION = 137,
    SC_CPUTIME = 138,
    SC_THREAD_CPUTIME = 139,
    SC_MONOTONIC_CLOCK = 149,
    SC_READER_WRITER_LOCKS = 153,
    SC_SPIN_LOCKS = 154,
    SC_REGEXP = 155,
    SC_SHELL = 157,
    SC_SPAWN = 159,
    SC_SPORADIC_SERVER = 160,
    SC_THREAD_SPORADIC_SERVER = 161,
    SC_TIMEOUTS = 164,
    SC_TYPED_MEMORY_OBJECTS = 165,
    SC_2_PBS = 168,
    SC_2_PBS_ACCOUNTING = 169,
    SC_2_PBS_LOCATE = 170,
    SC_2_PBS_MESSAGE = 171,
    SC_2_PBS_TRACK = 172,
    SC_SYMLOOP_MAX = 173,
    SC_STREAMS = 174,
    SC_2_PBS_CHECKPOINT = 175,
    SC_V6_ILP32_OFF32 = 176,
    SC_V6_ILP32_OFFBIG = 177,
    SC_V6_LP64_OFF64 = 178,
    SC_V6_LPBIG_OFFBIG = 179,
    SC_HOST_NAME_MAX = 180,
    SC_TRACE = 181,
    SC_TRACE_EVENT_FILTER = 182,
    SC_TRACE_INHERIT = 183,
    SC_TRACE_LOG = 184,
    SC_IPV6 = 235,
    SC_RAW_SOCKETS = 236,
    SC_V7_ILP32_OFF32 = 237,
    SC_V7_ILP32_OFFBIG = 238,
    SC_V7_LP64_OFF64 = 239,
    SC_V7_LPBIG_OFFBIG = 240,
    SC_SS_REPL_MAX = 241,
    SC_TRACE_EVENT_NAME_MAX = 242,
    SC_TRACE_NAME_MAX = 243,
    SC_TRACE_SYS_MAX = 244,
    SC_TRACE_USER_EVENT_MAX = 245,
    SC_XOPEN_STREAMS = 246,
    SC_THREAD_ROBUST_PRIO_INHERIT = 247,
    SC_THREAD_ROBUST_PRIO_PROTECT = 248
};
enum class SYSArgMacOS : uint8_t {
    SC_ARG_MAX = 1,
    SC_CHILD_MAX = 2,
    SC_CLK_TCK = 3,
    SC_NGROUPS_MAX = 4,
    SC_OPEN_MAX = 5,
    SC_JOB_CONTROL = 6,
    SC_SAVED_IDS = 7,
    SC_VERSION = 8,
    SC_BC_BASE_MAX = 9,
    SC_BC_DIM_MAX = 10,
    SC_BC_SCALE_MAX = 11,
    SC_BC_STRING_MAX = 12,
    SC_COLL_WEIGHTS_MAX = 13,
    SC_EXPR_NEST_MAX = 14,
    SC_LINE_MAX = 15,
    SC_RE_DUP_MAX = 16,
    SC_2_VERSION = 17,
    SC_2_C_BIND = 18,
    SC_2_C_DEV = 19,
    SC_2_CHAR_TERM = 20,
    SC_2_FORT_DEV = 21,
    SC_2_FORT_RUN = 22,
    SC_2_LOCALEDEF = 23,
    SC_2_SW_DEV = 24,
    SC_2_UPE = 25,
    SC_STREAM_MAX = 26,
    SC_TZNAME_MAX = 27,
    SC_ASYNCHRONOUS_IO = 28,
    SC_PAGESIZE = 29,   // same with _SC_PAGE_SIZE in mac platform
    SC_MEMLOCK = 30,
    SC_MEMLOCK_RANGE = 31,
    SC_MEMORY_PROTECTION = 32,
    SC_MESSAGE_PASSING = 33,
    SC_PRIORITIZED_IO = 34,
    SC_PRIORITY_SCHEDULING = 35,
    SC_REALTIME_SIGNALS = 36,
    SC_SEMAPHORES = 37,
    SC_FSYNC = 38,
    SC_SHARED_MEMORY_OBJECTS = 39,
    SC_SYNCHRONIZED_IO = 40,
    SC_TIMERS = 41,
    SC_AIO_LISTIO_MAX = 42,
    SC_AIO_MAX = 43,
    SC_AIO_PRIO_DELTA_MAX = 44,
    SC_DELAYTIMER_MAX = 45,
    SC_MQ_OPEN_MAX = 46,
    SC_MAPPED_FILES = 47,
    SC_RTSIG_MAX = 48,
    SC_SEM_NSEMS_MAX = 49,
    SC_SEM_VALUE_MAX = 50,
    SC_SIGQUEUE_MAX = 51,
    SC_TIMER_MAX = 52,
    SC_NPROCESSORS_CONF = 57,
    SC_NPROCESSORS_ONLN = 58,
    SC_2_PBS = 59,
    SC_2_PBS_ACCOUNTING = 60,
    SC_2_PBS_CHECKPOINT = 61,
    SC_2_PBS_LOCATE = 62,
    SC_2_PBS_MESSAGE = 63,
    SC_2_PBS_TRACK = 64,
    SC_ADVISORY_INFO = 65,
    SC_BARRIERS = 66,
    SC_CLOCK_SELECTION = 67,
    SC_CPUTIME = 68,
    SC_FILE_LOCKING = 69,
    SC_GETGR_R_SIZE_MAX = 70,
    SC_GETPW_R_SIZE_MAX = 71,
    SC_HOST_NAME_MAX = 72,
    SC_LOGIN_NAME_MAX = 73,
    SC_MONOTONIC_CLOCK = 74,
    SC_MQ_PRIO_MAX = 75,
    SC_READER_WRITER_LOCKS = 76,
    SC_REGEXP = 77,
    SC_SHELL = 78,
    SC_SPAWN = 79,
    SC_SPIN_LOCKS = 80,
    SC_SPORADIC_SERVER = 81,
    SC_THREAD_ATTR_STACKADDR = 82,
    SC_THREAD_ATTR_STACKSIZE = 83,
    SC_THREAD_CPUTIME = 84,
    SC_THREAD_DESTRUCTOR_ITERATIONS = 85,
    SC_THREAD_KEYS_MAX = 86,
    SC_THREAD_PRIO_INHERIT = 87,
    SC_THREAD_PRIO_PROTECT = 88,
    SC_THREAD_PRIORITY_SCHEDULING = 89,
    SC_THREAD_PROCESS_SHARED = 90,
    SC_THREAD_SAFE_FUNCTIONS = 91,
    SC_THREAD_SPORADIC_SERVER = 92,
    SC_THREAD_STACK_MIN = 93,
    SC_THREAD_THREADS_MAX = 94,
    SC_TIMEOUTS = 95,
    SC_THREADS = 96,
    SC_TRACE = 97,
    SC_TRACE_EVENT_FILTER = 98,
    SC_TRACE_INHERIT = 99,
    SC_TRACE_LOG = 100,
    SC_TTY_NAME_MAX = 101,
    SC_TYPED_MEMORY_OBJECTS = 102,
    SC_V6_ILP32_OFF32 = 103,
    SC_V6_ILP32_OFFBIG = 104,
    SC_V6_LP64_OFF64 = 105,
    SC_V6_LPBIG_OFFBIG = 106,
    SC_IPV6 = 118,
    SC_RAW_SOCKETS = 119,
    SC_SYMLOOP_MAX = 120,
    SC_ATEXIT_MAX = 107,
    SC_IOV_MAX = 56,
    SC_XOPEN_CRYPT = 108,
    SC_XOPEN_ENH_I18N = 109,
    SC_XOPEN_LEGACY = 110,
    SC_XOPEN_REALTIME = 111,
    SC_XOPEN_REALTIME_THREADS = 112,
    SC_XOPEN_SHM = 113,
    SC_XOPEN_STREAMS = 114,
    SC_XOPEN_UNIX = 115,
    SC_XOPEN_VERSION = 116,
    SC_XOPEN_XCU_VERSION = 121,
    SC_XBS5_ILP32_OFF32 = 122,
    SC_XBS5_ILP32_OFFBIG = 123,
    SC_XBS5_LP64_OFF64 = 124,
    SC_XBS5_LPBIG_OFFBIG = 125,
    SC_SS_REPL_MAX = 126,
    SC_TRACE_EVENT_NAME_MAX = 127,
    SC_TRACE_NAME_MAX = 128,
    SC_TRACE_SYS_MAX = 129,
    SC_TRACE_USER_EVENT_MAX = 130,
    SC_PASS_MAX = 131,
    SC_PHYS_PAGES = 200
};

std::map<SYSArgUnix, SYSArgMacOS> sysconfig_map = {
    {SYSArgUnix::SC_ARG_MAX, SYSArgMacOS::SC_ARG_MAX},
    {SYSArgUnix::SC_CHILD_MAX, SYSArgMacOS::SC_CHILD_MAX},
    {SYSArgUnix::SC_CLK_TCK, SYSArgMacOS::SC_CLK_TCK},
    {SYSArgUnix::SC_NGROUPS_MAX, SYSArgMacOS::SC_NGROUPS_MAX},
    {SYSArgUnix::SC_OPEN_MAX, SYSArgMacOS::SC_OPEN_MAX},
    {SYSArgUnix::SC_STREAM_MAX, SYSArgMacOS::SC_STREAM_MAX},
    {SYSArgUnix::SC_TZNAME_MAX, SYSArgMacOS::SC_TZNAME_MAX},
    {SYSArgUnix::SC_JOB_CONTROL, SYSArgMacOS::SC_JOB_CONTROL},
    {SYSArgUnix::SC_SAVED_IDS, SYSArgMacOS::SC_SAVED_IDS},
    {SYSArgUnix::SC_REALTIME_SIGNALS, SYSArgMacOS::SC_REALTIME_SIGNALS},
    {SYSArgUnix::SC_PRIORITY_SCHEDULING, SYSArgMacOS::SC_PRIORITY_SCHEDULING},
    {SYSArgUnix::SC_TIMERS, SYSArgMacOS::SC_TIMERS},
    {SYSArgUnix::SC_ASYNCHRONOUS_IO, SYSArgMacOS::SC_ASYNCHRONOUS_IO},
    {SYSArgUnix::SC_PRIORITIZED_IO, SYSArgMacOS::SC_PRIORITIZED_IO},
    {SYSArgUnix::SC_SYNCHRONIZED_IO, SYSArgMacOS::SC_SYNCHRONIZED_IO},
    {SYSArgUnix::SC_FSYNC, SYSArgMacOS::SC_FSYNC},
    {SYSArgUnix::SC_MAPPED_FILES, SYSArgMacOS::SC_MAPPED_FILES},
    {SYSArgUnix::SC_MEMLOCK, SYSArgMacOS::SC_MEMLOCK},
    {SYSArgUnix::SC_MEMLOCK_RANGE, SYSArgMacOS::SC_MEMLOCK_RANGE},
    {SYSArgUnix::SC_MEMORY_PROTECTION, SYSArgMacOS::SC_MEMORY_PROTECTION},
    {SYSArgUnix::SC_MESSAGE_PASSING, SYSArgMacOS::SC_MESSAGE_PASSING},
    {SYSArgUnix::SC_SEMAPHORES, SYSArgMacOS::SC_SEMAPHORES},
    {SYSArgUnix::SC_SHARED_MEMORY_OBJECTS, SYSArgMacOS::SC_SHARED_MEMORY_OBJECTS},
    {SYSArgUnix::SC_AIO_LISTIO_MAX, SYSArgMacOS::SC_AIO_LISTIO_MAX},
    {SYSArgUnix::SC_AIO_MAX, SYSArgMacOS::SC_AIO_MAX},
    {SYSArgUnix::SC_AIO_PRIO_DELTA_MAX, SYSArgMacOS::SC_AIO_PRIO_DELTA_MAX},
    {SYSArgUnix::SC_DELAYTIMER_MAX, SYSArgMacOS::SC_DELAYTIMER_MAX},
    {SYSArgUnix::SC_MQ_OPEN_MAX, SYSArgMacOS::SC_MQ_OPEN_MAX},
    {SYSArgUnix::SC_MQ_PRIO_MAX, SYSArgMacOS::SC_MQ_PRIO_MAX},
    {SYSArgUnix::SC_VERSION, SYSArgMacOS::SC_VERSION},
    {SYSArgUnix::SC_PAGESIZE, SYSArgMacOS::SC_PAGESIZE},
    {SYSArgUnix::SC_RTSIG_MAX, SYSArgMacOS::SC_RTSIG_MAX},
    {SYSArgUnix::SC_SEM_NSEMS_MAX, SYSArgMacOS::SC_SEM_NSEMS_MAX},
    {SYSArgUnix::SC_SEM_VALUE_MAX, SYSArgMacOS::SC_SEM_VALUE_MAX},
    {SYSArgUnix::SC_SIGQUEUE_MAX, SYSArgMacOS::SC_SIGQUEUE_MAX},
    {SYSArgUnix::SC_TIMER_MAX, SYSArgMacOS::SC_TIMER_MAX},
    {SYSArgUnix::SC_BC_BASE_MAX, SYSArgMacOS::SC_BC_BASE_MAX},
    {SYSArgUnix::SC_BC_DIM_MAX, SYSArgMacOS::SC_BC_DIM_MAX},
    {SYSArgUnix::SC_BC_SCALE_MAX, SYSArgMacOS::SC_BC_SCALE_MAX},
    {SYSArgUnix::SC_BC_STRING_MAX, SYSArgMacOS::SC_BC_STRING_MAX},
    {SYSArgUnix::SC_COLL_WEIGHTS_MAX, SYSArgMacOS::SC_COLL_WEIGHTS_MAX},
    {SYSArgUnix::SC_EXPR_NEST_MAX, SYSArgMacOS::SC_EXPR_NEST_MAX},
    {SYSArgUnix::SC_LINE_MAX, SYSArgMacOS::SC_LINE_MAX},
    {SYSArgUnix::SC_RE_DUP_MAX, SYSArgMacOS::SC_RE_DUP_MAX},
    {SYSArgUnix::SC_2_VERSION, SYSArgMacOS::SC_2_VERSION},
    {SYSArgUnix::SC_2_C_BIND, SYSArgMacOS::SC_2_C_BIND},
    {SYSArgUnix::SC_2_C_DEV, SYSArgMacOS::SC_2_C_DEV},
    {SYSArgUnix::SC_2_FORT_DEV, SYSArgMacOS::SC_2_FORT_DEV},
    {SYSArgUnix::SC_2_FORT_RUN, SYSArgMacOS::SC_2_FORT_RUN},
    {SYSArgUnix::SC_2_SW_DEV, SYSArgMacOS::SC_2_SW_DEV},
    {SYSArgUnix::SC_2_LOCALEDEF, SYSArgMacOS::SC_2_LOCALEDEF},
    {SYSArgUnix::SC_IOV_MAX, SYSArgMacOS::SC_IOV_MAX},
    {SYSArgUnix::SC_THREADS, SYSArgMacOS::SC_THREADS},
    {SYSArgUnix::SC_THREAD_SAFE_FUNCTIONS, SYSArgMacOS::SC_THREAD_SAFE_FUNCTIONS},
    {SYSArgUnix::SC_GETGR_R_SIZE_MAX, SYSArgMacOS::SC_GETGR_R_SIZE_MAX},
    {SYSArgUnix::SC_GETPW_R_SIZE_MAX, SYSArgMacOS::SC_GETPW_R_SIZE_MAX},
    {SYSArgUnix::SC_LOGIN_NAME_MAX, SYSArgMacOS::SC_LOGIN_NAME_MAX},
    {SYSArgUnix::SC_TTY_NAME_MAX, SYSArgMacOS::SC_TTY_NAME_MAX},
    {SYSArgUnix::SC_THREAD_DESTRUCTOR_ITERATIONS, SYSArgMacOS::SC_THREAD_DESTRUCTOR_ITERATIONS},
    {SYSArgUnix::SC_THREAD_KEYS_MAX, SYSArgMacOS::SC_THREAD_KEYS_MAX},
    {SYSArgUnix::SC_THREAD_STACK_MIN, SYSArgMacOS::SC_THREAD_STACK_MIN},
    {SYSArgUnix::SC_THREAD_THREADS_MAX, SYSArgMacOS::SC_THREAD_THREADS_MAX},
    {SYSArgUnix::SC_THREAD_ATTR_STACKADDR, SYSArgMacOS::SC_THREAD_ATTR_STACKADDR},
    {SYSArgUnix::SC_THREAD_ATTR_STACKSIZE, SYSArgMacOS::SC_THREAD_ATTR_STACKSIZE},
    {SYSArgUnix::SC_THREAD_PRIORITY_SCHEDULING, SYSArgMacOS::SC_THREAD_PRIORITY_SCHEDULING},
    {SYSArgUnix::SC_THREAD_PRIO_INHERIT, SYSArgMacOS::SC_THREAD_PRIO_INHERIT},
    {SYSArgUnix::SC_THREAD_PRIO_PROTECT, SYSArgMacOS::SC_THREAD_PRIO_PROTECT},
    {SYSArgUnix::SC_THREAD_PROCESS_SHARED, SYSArgMacOS::SC_THREAD_PROCESS_SHARED},
    {SYSArgUnix::SC_NPROCESSORS_CONF, SYSArgMacOS::SC_NPROCESSORS_CONF},
    {SYSArgUnix::SC_NPROCESSORS_ONLN, SYSArgMacOS::SC_NPROCESSORS_ONLN},
    {SYSArgUnix::SC_PHYS_PAGES, SYSArgMacOS::SC_PHYS_PAGES},
    {SYSArgUnix::SC_ATEXIT_MAX, SYSArgMacOS::SC_ATEXIT_MAX},
    {SYSArgUnix::SC_PASS_MAX, SYSArgMacOS::SC_PASS_MAX},
    {SYSArgUnix::SC_XOPEN_VERSION, SYSArgMacOS::SC_XOPEN_VERSION},
    {SYSArgUnix::SC_XOPEN_XCU_VERSION, SYSArgMacOS::SC_XOPEN_XCU_VERSION},
    {SYSArgUnix::SC_XOPEN_UNIX, SYSArgMacOS::SC_XOPEN_UNIX},
    {SYSArgUnix::SC_XOPEN_CRYPT, SYSArgMacOS::SC_XOPEN_CRYPT},
    {SYSArgUnix::SC_XOPEN_ENH_I18N, SYSArgMacOS::SC_XOPEN_ENH_I18N},
    {SYSArgUnix::SC_XOPEN_SHM, SYSArgMacOS::SC_XOPEN_SHM},
    {SYSArgUnix::SC_2_CHAR_TERM, SYSArgMacOS::SC_2_CHAR_TERM},
    {SYSArgUnix::SC_2_UPE, SYSArgMacOS::SC_2_UPE},
    {SYSArgUnix::SC_XBS5_ILP32_OFF32, SYSArgMacOS::SC_XBS5_ILP32_OFF32},
    {SYSArgUnix::SC_XBS5_ILP32_OFFBIG, SYSArgMacOS::SC_XBS5_ILP32_OFFBIG},
    {SYSArgUnix::SC_XBS5_LP64_OFF64, SYSArgMacOS::SC_XBS5_LP64_OFF64},
    {SYSArgUnix::SC_XBS5_LPBIG_OFFBIG, SYSArgMacOS::SC_XBS5_LPBIG_OFFBIG},
    {SYSArgUnix::SC_XOPEN_LEGACY, SYSArgMacOS::SC_XOPEN_LEGACY},
    {SYSArgUnix::SC_XOPEN_REALTIME, SYSArgMacOS::SC_XOPEN_REALTIME},
    {SYSArgUnix::SC_XOPEN_REALTIME_THREADS, SYSArgMacOS::SC_XOPEN_REALTIME_THREADS},
    {SYSArgUnix::SC_ADVISORY_INFO, SYSArgMacOS::SC_ADVISORY_INFO},
    {SYSArgUnix::SC_BARRIERS, SYSArgMacOS::SC_BARRIERS},
    {SYSArgUnix::SC_CLOCK_SELECTION, SYSArgMacOS::SC_CLOCK_SELECTION},
    {SYSArgUnix::SC_CPUTIME, SYSArgMacOS::SC_CPUTIME},
    {SYSArgUnix::SC_THREAD_CPUTIME, SYSArgMacOS::SC_THREAD_CPUTIME},
    {SYSArgUnix::SC_MONOTONIC_CLOCK, SYSArgMacOS::SC_MONOTONIC_CLOCK},
    {SYSArgUnix::SC_READER_WRITER_LOCKS, SYSArgMacOS::SC_READER_WRITER_LOCKS},
    {SYSArgUnix::SC_SPIN_LOCKS, SYSArgMacOS::SC_SPIN_LOCKS},
    {SYSArgUnix::SC_REGEXP, SYSArgMacOS::SC_REGEXP},
    {SYSArgUnix::SC_SHELL, SYSArgMacOS::SC_SHELL},
    {SYSArgUnix::SC_SPAWN, SYSArgMacOS::SC_SPAWN},
    {SYSArgUnix::SC_SPORADIC_SERVER, SYSArgMacOS::SC_SPORADIC_SERVER},
    {SYSArgUnix::SC_THREAD_SPORADIC_SERVER, SYSArgMacOS::SC_THREAD_SPORADIC_SERVER},
    {SYSArgUnix::SC_TIMEOUTS, SYSArgMacOS::SC_TIMEOUTS},
    {SYSArgUnix::SC_TYPED_MEMORY_OBJECTS, SYSArgMacOS::SC_TYPED_MEMORY_OBJECTS},
    {SYSArgUnix::SC_2_PBS, SYSArgMacOS::SC_2_PBS},
    {SYSArgUnix::SC_2_PBS_ACCOUNTING, SYSArgMacOS::SC_2_PBS_ACCOUNTING},
    {SYSArgUnix::SC_2_PBS_LOCATE, SYSArgMacOS::SC_2_PBS_LOCATE},
    {SYSArgUnix::SC_2_PBS_MESSAGE, SYSArgMacOS::SC_2_PBS_MESSAGE},
    {SYSArgUnix::SC_2_PBS_TRACK, SYSArgMacOS::SC_2_PBS_TRACK},
    {SYSArgUnix::SC_SYMLOOP_MAX, SYSArgMacOS::SC_SYMLOOP_MAX},
    {SYSArgUnix::SC_2_PBS_CHECKPOINT, SYSArgMacOS::SC_2_PBS_CHECKPOINT},
    {SYSArgUnix::SC_V6_ILP32_OFF32, SYSArgMacOS::SC_V6_ILP32_OFF32},
    {SYSArgUnix::SC_V6_ILP32_OFFBIG, SYSArgMacOS::SC_V6_ILP32_OFFBIG},
    {SYSArgUnix::SC_V6_LP64_OFF64, SYSArgMacOS::SC_V6_LP64_OFF64},
    {SYSArgUnix::SC_V6_LPBIG_OFFBIG, SYSArgMacOS::SC_V6_LPBIG_OFFBIG},
    {SYSArgUnix::SC_HOST_NAME_MAX, SYSArgMacOS::SC_HOST_NAME_MAX},
    {SYSArgUnix::SC_TRACE, SYSArgMacOS::SC_TRACE},
    {SYSArgUnix::SC_TRACE_EVENT_FILTER, SYSArgMacOS::SC_TRACE_EVENT_FILTER},
    {SYSArgUnix::SC_TRACE_INHERIT, SYSArgMacOS::SC_TRACE_INHERIT},
    {SYSArgUnix::SC_TRACE_LOG, SYSArgMacOS::SC_TRACE_LOG},
    {SYSArgUnix::SC_IPV6, SYSArgMacOS::SC_IPV6},
    {SYSArgUnix::SC_RAW_SOCKETS, SYSArgMacOS::SC_RAW_SOCKETS},
    {SYSArgUnix::SC_SS_REPL_MAX, SYSArgMacOS::SC_SS_REPL_MAX},
    {SYSArgUnix::SC_TRACE_EVENT_NAME_MAX, SYSArgMacOS::SC_TRACE_EVENT_NAME_MAX},
    {SYSArgUnix::SC_TRACE_NAME_MAX, SYSArgMacOS::SC_TRACE_NAME_MAX},
    {SYSArgUnix::SC_TRACE_SYS_MAX, SYSArgMacOS::SC_TRACE_SYS_MAX},
    {SYSArgUnix::SC_TRACE_USER_EVENT_MAX, SYSArgMacOS::SC_TRACE_USER_EVENT_MAX},
    {SYSArgUnix::SC_XOPEN_STREAMS, SYSArgMacOS::SC_XOPEN_STREAMS},
};
#endif

    namespace {
        constexpr int NUM_OF_DATA = 4;
        constexpr int PER_USER_RANGE = 100000;
        constexpr int32_t NAPI_RETURN_ZERO = 0;
        constexpr int32_t NAPI_RETURN_ONE = 1;
    }
    thread_local std::multimap<std::string, napi_ref> eventMap;
    thread_local std::map<napi_ref, napi_ref> pendingUnHandledRejections;
    // support events
    thread_local std::string events = "UnHandleRejection";

    napi_value Process::GetUid(napi_env env) const
    {
        napi_value result = nullptr;
        auto processGetuid = static_cast<uint32_t>(getuid());
        NAPI_CALL(env, napi_create_uint32(env, processGetuid, &result));
        return result;
    }

    napi_value Process::GetGid(napi_env env) const
    {
        napi_value result = nullptr;
        auto processGetgid = static_cast<uint32_t>(getgid());
        NAPI_CALL(env, napi_create_uint32(env, processGetgid, &result));
        return result;
    }

    napi_value Process::GetEUid(napi_env env) const
    {
        napi_value result = nullptr;
        auto processGeteuid = static_cast<uint32_t>(geteuid());
        NAPI_CALL(env, napi_create_uint32(env, processGeteuid, &result));
        return result;
    }

    napi_value Process::GetEGid(napi_env env) const
    {
        napi_value result = nullptr;
        auto processGetegid = static_cast<uint32_t>(getegid());
        NAPI_CALL(env, napi_create_uint32(env, processGetegid, &result));
        return result;
    }

    napi_value Process::GetGroups(napi_env env) const
    {
        napi_value result = nullptr;
        int progroups = getgroups(0, nullptr);
        if (progroups == -1) {
            napi_throw_error(env, "-1", "getgroups initialize failed");
        }
        std::vector<gid_t> pgrous(progroups);
        progroups = getgroups(progroups, pgrous.data());
        if (progroups == -1) {
            napi_throw_error(env, "-1", "getgroups");
        }
        pgrous.resize(static_cast<size_t>(progroups));
        gid_t proegid = getegid();
        if (std::find(pgrous.begin(), pgrous.end(), proegid) == pgrous.end()) {
            pgrous.push_back(proegid);
        }
        std::vector<uint32_t> array;
        for (auto iter = pgrous.begin(); iter != pgrous.end(); iter++) {
            auto receive = static_cast<uint32_t>(*iter);
            array.push_back(receive);
        }
        NAPI_CALL(env, napi_create_array(env, &result));
        size_t len = array.size();
        for (size_t i = 0; i < len; i++) {
            napi_value numvalue = nullptr;
            NAPI_CALL(env, napi_create_uint32(env, array[i], &numvalue));
            NAPI_CALL(env, napi_set_element(env, result, i, numvalue));
        }
        return result;
    }

    napi_value Process::GetPid(napi_env env) const
    {
        napi_value result = nullptr;
        auto proPid = static_cast<int32_t>(getpid());
        napi_create_int32(env, proPid, &result);
        return result;
    }

    napi_value Process::GetPpid(napi_env env) const
    {
        napi_value result = nullptr;
        auto proPpid = static_cast<int32_t>(getppid());
        napi_create_int32(env, proPpid, &result);
        return result;
    }

    void Process::Chdir(napi_env env, napi_value args) const
    {
        size_t prolen = 0;
        if (napi_get_value_string_utf8(env, args, nullptr, 0, &prolen) != napi_ok) {
            HILOG_ERROR("can not get args size");
            return;
        }
        std::string result = "";
        result.reserve(prolen + 1);
        result.resize(prolen);
        if (napi_get_value_string_utf8(env, args, result.data(), prolen + 1, &prolen) != napi_ok) {
            HILOG_ERROR("can not get args value");
            return;
        }
        int proerr = 0;
        proerr = uv_chdir(result.c_str());
        if (proerr) {
            napi_throw_error(env, "-1", "chdir");
        }
    }

    napi_value Process::Kill(napi_env env, napi_value signal, napi_value proid)
    {
        int32_t pid = 0;
        int32_t sig = 0;
        napi_get_value_int32(env, proid, &pid);
        napi_get_value_int32(env, signal, &sig);
        uv_pid_t ownPid = uv_os_getpid();
        // 64:The maximum valid signal value is 64.
        if (sig > 64 && (!pid || pid == -1 || pid == ownPid || pid == -ownPid)) {
            napi_throw_error(env, "0", "process exit");
        }
        bool flag = false;
        int err = uv_kill(pid, sig);
        if (!err) {
            flag = true;
        }
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_boolean(env, flag, &result));
        return result;
    }

    napi_value Process::Uptime(napi_env env) const
    {
        napi_value result = nullptr;
        double runsystime = 0.0;
#ifdef IOS_PLATFORM
        struct timeval boottime;
        struct timeval now;
        struct timezone tz;
        int mib[2] = { CTL_KERN, KERN_BOOTTIME };
        size_t size = sizeof(boottime);
        gettimeofday(&now, &tz);
        double systimer = -1;
#define MIB_SIZE 2
#define MICROSECONDS_OF_SECOND 1000.0*1000.0
        if (sysctl(mib, MIB_SIZE, &boottime, &size, NULL, 0) != -1 && boottime.tv_sec != 0) {
            systimer = now.tv_sec - boottime.tv_sec;
            systimer += (double)(now.tv_usec - boottime.tv_usec) / MICROSECONDS_OF_SECOND;
        }
#else
        struct sysinfo information = {0};
        time_t systimer = 0;
        if (sysinfo(&information)) {
            napi_throw_error(env, "-1", "Failed to get sysinfo");
        }
        systimer = information.uptime;
#endif
        if (systimer > 0) {
            runsystime = static_cast<double>(systimer);
            NAPI_CALL(env, napi_create_double(env, runsystime, &result));
        } else {
            napi_throw_error(env, "-1", "Failed to get systimer");
        }
        return result;
    }

    void Process::Exit(napi_env env, napi_value number) const
    {
        int32_t result = 0;
        napi_get_value_int32(env, number, &result);
        exit(result);
    }

    napi_value Process::Cwd(napi_env env) const
    {
        napi_value result = nullptr;
        char buf[260 * NUM_OF_DATA] = { 0 }; // 260:Only numbers path String size is 260.
        size_t length = sizeof(buf);
        int err = uv_cwd(buf, &length);
        if (err) {
            napi_throw_error(env, "1", "uv_cwd");
        }
        napi_create_string_utf8(env, buf, length, &result);
        return result;
    }

    void Process::Abort() const
    {
        exit(0);
    }

    void Process::On(napi_env env, napi_value str, napi_value function)
    {
        std::string result = "";
        size_t bufferSize = 0;
        if (napi_get_value_string_utf8(env, str, nullptr, NAPI_RETURN_ZERO, &bufferSize) != napi_ok) {
            HILOG_ERROR("can not get str size");
            return;
        }
        result.reserve(bufferSize + NAPI_RETURN_ONE);
        result.resize(bufferSize);
        if (napi_get_value_string_utf8(env, str, result.data(), bufferSize + NAPI_RETURN_ONE,
                                       &bufferSize) != napi_ok) {
            HILOG_ERROR("can not get str value");
            return;
        }
        if (function == nullptr) {
            HILOG_ERROR("function is nullptr");
            return;
        }
        napi_ref myCallRef = nullptr;
        napi_status status = napi_create_reference(env, function, 1, &myCallRef);
        if (status != napi_ok) {
            HILOG_ERROR("napi_create_reference is failed");
            return;
        }
        if (!result.empty()) {
            size_t pos = events.find(result);
            if (pos == std::string::npos) {
                HILOG_ERROR("illegal event");
                return;
            }
            eventMap.insert(std::make_pair(result, myCallRef));
        }
    }

    napi_value Process::Off(napi_env env, napi_value str)
    {
        size_t bufferSize = 0;
        bool flag = false;
        if (napi_get_value_string_utf8(env, str, nullptr, 0, &bufferSize) != napi_ok) {
            HILOG_ERROR("can not get str size");
            return nullptr;
        }
        std::string result = "";
        result.reserve(bufferSize + 1);
        result.resize(bufferSize);
        if (napi_get_value_string_utf8(env, str, result.data(), bufferSize + 1, &bufferSize) != napi_ok) {
            HILOG_ERROR("can not get str value");
            return nullptr;
        }
        std::string temp = "";
        temp = result;
        auto iter = eventMap.equal_range(temp);
        while (iter.first != iter.second) {
            NAPI_CALL(env, napi_delete_reference(env, iter.first->second));
            iter.first = eventMap.erase(iter.first);
            flag = true;
        }
        napi_value convertResult = nullptr;
        NAPI_CALL(env, napi_get_boolean(env, flag, &convertResult));
        return convertResult;
    }

    napi_value Process::GetTid(napi_env env) const
    {
        napi_value result = nullptr;
#ifdef IOS_PLATFORM
        uint64_t proTid;
        pthread_threadid_np(0, &proTid);
        napi_create_int64(env, proTid, &result);
#else
        auto proTid = static_cast<int32_t>(gettid());
        napi_create_int32(env, proTid, &result);
#endif
        return result;
    }

    napi_value Process::IsIsolatedProcess(napi_env env) const
    {
        napi_value result = nullptr;
#ifdef IOS_PLATFORM
        bool flag = false;
        NAPI_CALL(env, napi_get_boolean(env, flag, &result));
#else
        bool flag = true;
        auto prouid = static_cast<int32_t>(getuid());
        auto uid = prouid % PER_USER_RANGE;
        if ((uid >= 99000 && uid <= 99999) || // 99999:Only isolateuid numbers between 99000 and 99999.
            (uid >= 9000 && uid <= 98999)) { // 98999:Only appuid numbers between 9000 and 98999.
            NAPI_CALL(env, napi_get_boolean(env, flag, &result));
            return result;
        }
        flag = false;
        NAPI_CALL(env, napi_get_boolean(env, flag, &result));
#endif
        return result;
    }

    napi_value Process::IsAppUid(napi_env env, napi_value uid) const
    {
        int32_t number = 0;
        napi_value result = nullptr;
        bool flag = true;
        napi_get_value_int32(env, uid, &number);
        if (number > 0) {
            const auto appId = number % PER_USER_RANGE;
            if (appId >= FIRST_APPLICATION_UID && appId <= LAST_APPLICATION_UID) {
                napi_get_boolean(env, flag, &result);
                return result;
            }
            flag = false;
            NAPI_CALL(env, napi_get_boolean(env, flag, &result));
            return result;
        } else {
            flag = false;
            NAPI_CALL(env, napi_get_boolean(env, flag, &result));
            return result;
        }
    }

    napi_value Process::Is64Bit(napi_env env) const
    {
        napi_value result = nullptr;
        bool flag = true;
        auto size = sizeof(char*);
        flag = (size == NUM_OF_DATA) ? false : true;
        NAPI_CALL(env, napi_get_boolean(env, flag, &result));
        return result;
    }

    napi_value Process::GetEnvironmentVar(napi_env env, napi_value name) const
    {
        napi_value convertResult = nullptr;
        char buf[260 * NUM_OF_DATA] = { 0 }; // 260:Only numbers path String size is 260.
        size_t length = sizeof(buf);
        size_t bufferSize = 0;
        if (napi_get_value_string_utf8(env, name, nullptr, 0, &bufferSize) != napi_ok) {
            HILOG_ERROR("can not get name size");
            return nullptr;
        }
        std::string result = "";
        result.reserve(bufferSize + 1);
        result.resize(bufferSize);
        if (napi_get_value_string_utf8(env, name, result.data(), bufferSize + 1, &bufferSize) != napi_ok) {
            HILOG_ERROR("can not get name value");
            return nullptr;
        }
        std::string temp = "";
        temp = result;
        auto envNum = uv_os_getenv(temp.c_str(), buf, &length);
        if (envNum == UV_ENOENT) {
            NAPI_CALL(env, napi_get_undefined(env, &convertResult));
            return convertResult;
        }
        napi_create_string_utf8(env, buf, strlen(buf), &convertResult);
        return convertResult;
    }

    napi_value Process::GetUidForName(napi_env env, napi_value name) const
    {
        napi_value convertResult = nullptr;
        size_t bufferSize = 0;
        if (napi_get_value_string_utf8(env, name, nullptr, 0, &bufferSize) != napi_ok) {
            HILOG_ERROR("can not get name size");
            return nullptr;
        }
        std::string result = "";
        result.reserve(bufferSize + 1);
        result.resize(bufferSize);
        if (napi_get_value_string_utf8(env, name, result.data(), bufferSize + 1, &bufferSize) != napi_ok) {
            HILOG_ERROR("can not get name value");
            return nullptr;
        }
        struct passwd user;
        int32_t uid = 0;
        struct passwd *bufp = nullptr;
        long bufLen = sysconf(_SC_GETPW_R_SIZE_MAX);
        if (bufLen == -1) {
            bufLen = 16384; // 16384:Should be more than enough
        }

        std::string buf;
        buf.reserve(bufLen + 1);
        buf.resize(bufLen);
        if (getpwnam_r(result.c_str(), &user, buf.data(), bufLen, &bufp) == 0 && bufp != nullptr) {
            uid = static_cast<int32_t>(bufp->pw_uid);
            napi_create_int32(env, uid, &convertResult);
            return convertResult;
        }
        napi_create_int32(env, (-1), &convertResult);
        return convertResult;
    }

    napi_value Process::GetThreadPriority(napi_env env, napi_value tid) const
    {
        errno = 0;
        napi_value result = nullptr;
#ifdef IOS_PLATFORM
        int32_t pri = getpriority(PRIO_DARWIN_THREAD, 0);   // 0: current thread
#else
        int32_t proTid = 0;
        napi_get_value_int32(env, tid, &proTid);
        int32_t pri = getpriority(PRIO_PROCESS, proTid);
#endif
        if (errno) {
            napi_throw_error(env, "-1", "Invalid tid");
        }
        napi_create_int32(env, pri, &result);
        return result;
    }

    napi_value Process::GetStartRealtime(napi_env env) const
    {
        struct timespec timespro = {0, 0};
        struct timespec timessys = {0, 0};
        napi_value result = nullptr;
        auto res = clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &timespro);
        if (res) {
            return 0;
        }
        auto res1 = clock_gettime(CLOCK_MONOTONIC, &timessys);
        if (res1) {
            return 0;
        }
        int whenpro = ConvertTime(timespro.tv_sec, timespro.tv_nsec);
        int whensys = ConvertTime(timessys.tv_sec, timessys.tv_nsec);
        auto timedif = (whensys - whenpro);
        napi_create_int32(env, timedif, &result);
        return result;
    }

    int Process::ConvertTime(time_t tvsec, int64_t tvnsec) const
    {
        return int(tvsec * 1000) + int(tvnsec / 1000000); // 98999:Only converttime numbers is 1000 and 1000000.
    }

    napi_value Process::GetPastCputime(napi_env env) const
    {
        struct timespec times = {0, 0};
        napi_value result = nullptr;
        auto res = clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &times);
        if (res) {
            return 0;
        }
        int when =  ConvertTime(times.tv_sec, times.tv_nsec);
        napi_create_int32(env, when, &result);
        return result;
    }

    napi_value Process::GetSystemConfig(napi_env env, napi_value name) const
    {
        int32_t number = 0;
        napi_value result = nullptr;
        napi_get_value_int32(env, name, &number);
#ifdef IOS_PLATFORM
        SYSArgUnix unixSysconf = static_cast<SYSArgUnix>(number);
        uint8_t index = static_cast<uint8_t>(sysconfig_map[unixSysconf]);
        auto configinfo = static_cast<int32_t>(sysconf(index));
        if (configinfo < 0) {
            HILOG_ERROR("Unable to find this system configuration in MAC platform:[unix]%d", unixSysconf);
        }
#else
        auto configinfo = static_cast<int32_t>(sysconf(number));
#endif
        napi_create_int32(env, configinfo, &result);
        return result;
    }

    napi_value UnHandle(napi_env env, napi_value promise, napi_value reason)
    {
        napi_ref promiseRef = nullptr;
        NAPI_CALL(env, napi_create_reference(env, promise, 1, &promiseRef));
        napi_ref reasonRef = nullptr;
        NAPI_CALL(env, napi_create_reference(env, reason, 1, &reasonRef));
        pendingUnHandledRejections.insert(std::make_pair(promiseRef, reasonRef));
        napi_value res = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &res));
        return res;
    }

    napi_value AddHandle(napi_env env, napi_value promise)
    {
        auto iter = pendingUnHandledRejections.begin();
        while (iter != pendingUnHandledRejections.end()) {
            napi_value prom = nullptr;
            NAPI_CALL(env, napi_get_reference_value(env, iter->first, &prom));
            bool isEquals = false;
            NAPI_CALL(env, napi_strict_equals(env, promise, prom, &isEquals));
            if (isEquals) {
                NAPI_CALL(env, napi_delete_reference(env, iter->first));
                NAPI_CALL(env, napi_delete_reference(env, iter->second));
                iter = pendingUnHandledRejections.erase(iter);
                continue;
            }
            iter++;
        }
        napi_value res = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &res));
        return res;
    }

    napi_value UnHandleRejection(napi_env env, napi_value promise, napi_value reason)
    {
        auto it = eventMap.find("UnHandleRejection");
        if (it != eventMap.end()) {
            napi_value global = nullptr;
            NAPI_CALL(env, napi_get_global(env, &global));
            size_t argc = 2; // 2 parameter size
            napi_value args[] = {reason, promise};
            auto iter = eventMap.equal_range("UnHandleRejection");
            while (iter.first != iter.second) {
                napi_value cb = nullptr;
                NAPI_CALL(env, napi_get_reference_value(env, iter.first->second, &cb));
                napi_value result = nullptr;
                NAPI_CALL(env, napi_call_function(env, global, cb, argc, args, &result));
                iter.first++;
            }
        }
        napi_value res = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &res));
        return res;
    }

    static napi_value OnUnHandleRejection(napi_env env, napi_callback_info info)
    {
        size_t argc = 3; // 3 parameter size
        napi_value argv[3] = {0}; // 3 array length
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
        int32_t event = 0;
        NAPI_CALL(env, napi_get_value_int32(env, argv[0], &event));
        if (event == static_cast<int32_t>(PromiseRejectionEvent::REJECT)) {
            UnHandle(env, argv[1], argv[2]); // 2 array index
        } else if (event == static_cast<int32_t>(PromiseRejectionEvent::HANDLE)) {
            AddHandle(env, argv[1]);
        }
        napi_value res = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &res));
        return res;
    }

    static napi_value CheckUnhandleRejections(napi_env env, napi_callback_info info)
    {
        if (!pendingUnHandledRejections.empty()) {
            auto iter = pendingUnHandledRejections.begin();
            while (iter != pendingUnHandledRejections.end()) {
                napi_value promise = nullptr;
                NAPI_CALL(env, napi_get_reference_value(env, iter->first, &promise));
                napi_value reason = nullptr;
                NAPI_CALL(env, napi_get_reference_value(env, iter->second, &reason));

                UnHandleRejection(env, promise, reason);

                NAPI_CALL(env, napi_delete_reference(env, iter->first));
                NAPI_CALL(env, napi_delete_reference(env, iter->second));
                iter = pendingUnHandledRejections.erase(iter);
            }
        }
        napi_value res = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &res));
        return res;
    }

    napi_value Process::SetRejectionCallback(napi_env env) const
    {
        napi_value cb = nullptr;
        std::string callbackName = "onUnHandleRejection";
        NAPI_CALL(env, napi_create_function(env, callbackName.c_str(), callbackName.size(), OnUnHandleRejection,
                                             nullptr, &cb));
        napi_ref unHandleRejectionCallbackRef = nullptr;
        NAPI_CALL(env, napi_create_reference(env, cb, 1, &unHandleRejectionCallbackRef));

        napi_ref checkUnhandleRejectionsRef = nullptr;
        napi_value checkcb = nullptr;
        std::string cbName = "CheckUnhandleRejections";
        NAPI_CALL(env, napi_create_function(env, cbName.c_str(), cbName.size(), CheckUnhandleRejections,
                                             nullptr, &checkcb));
        NAPI_CALL(env, napi_create_reference(env, checkcb, 1, &checkUnhandleRejectionsRef));
        napi_value res = nullptr;
        NAPI_CALL(env, napi_get_undefined(env, &res));
        return res;
    }
    void Process::ClearReference(napi_env env)
    {
        auto iter = eventMap.begin();
        while (iter != eventMap.end()) {
            napi_status status = napi_delete_reference(env, iter->second);
            if (status != napi_ok) {
                napi_throw_error(env, nullptr, "ClearReference failed");
            }
            iter++;
        }
        eventMap.clear();
    }

    napi_value ProcessManager::IsAppUid(napi_env env, napi_value uid) const
    {
        int32_t number = 0;
        napi_value result = nullptr;
        bool flag = true;
        napi_get_value_int32(env, uid, &number);
        if (number > 0) {
            const auto appId = number % PER_USER_RANGE;
            if (appId >= FIRST_APPLICATION_UID && appId <= LAST_APPLICATION_UID) {
                napi_get_boolean(env, flag, &result);
                return result;
            }
            flag = false;
            NAPI_CALL(env, napi_get_boolean(env, flag, &result));
            return result;
        } else {
            flag = false;
            NAPI_CALL(env, napi_get_boolean(env, flag, &result));
            return result;
        }
    }

    napi_value ProcessManager::GetUidForName(napi_env env, napi_value name) const
    {
        napi_value convertResult = nullptr;
        size_t bufferSize = 0;
        if (napi_get_value_string_utf8(env, name, nullptr, 0, &bufferSize) != napi_ok) {
            HILOG_ERROR("can not get name size");
            return nullptr;
        }
        std::string result = "";
        result.reserve(bufferSize + 1);
        result.resize(bufferSize);
        if (napi_get_value_string_utf8(env, name, result.data(), bufferSize + 1, &bufferSize) != napi_ok) {
            HILOG_ERROR("can not get name value");
            return nullptr;
        }
        struct passwd user;
        int32_t uid = 0;
        struct passwd *bufp = nullptr;
        long bufLen = sysconf(_SC_GETPW_R_SIZE_MAX);
        if (bufLen == -1) {
            bufLen = 16384; // 16384:Should be more than enough
        }

        std::string buf;
        buf.reserve(bufLen + 1);
        buf.resize(bufLen);
        if (getpwnam_r(result.c_str(), &user, buf.data(), bufLen, &bufp) == 0 && bufp != nullptr) {
            uid = static_cast<int32_t>(bufp->pw_uid);
            napi_create_int32(env, uid, &convertResult);
            return convertResult;
        }
        napi_create_int32(env, (-1), &convertResult);
        return convertResult;
    }

    napi_value ProcessManager::GetThreadPriority(napi_env env, napi_value tid) const
    {
        errno = 0;
        napi_value result = nullptr;
#ifdef IOS_PLATFORM
        int32_t pri = getpriority(PRIO_DARWIN_THREAD, 0);   // 0: current thread
#else
        int32_t proTid = 0;
        napi_get_value_int32(env, tid, &proTid);
        int32_t pri = getpriority(PRIO_PROCESS, proTid);
#endif
        if (errno) {
            napi_throw_error(env, "-1", "Invalid tid");
        }
        napi_create_int32(env, pri, &result);
        return result;
    }

    napi_value ProcessManager::GetSystemConfig(napi_env env, napi_value name) const
    {
        int32_t number = 0;
        napi_value result = nullptr;
        napi_get_value_int32(env, name, &number);
#ifdef IOS_PLATFORM
        SYSArgUnix unixSysconf = static_cast<SYSArgUnix>(number);
        uint8_t index = static_cast<uint8_t>(sysconfig_map[unixSysconf]);
        auto configinfo = static_cast<int32_t>(sysconf(index));
        if (configinfo < 0) {
            HILOG_ERROR("Unable to find this system configuration in MAC platform:[unix]%d", unixSysconf);
        }
#else
        auto configinfo = static_cast<int32_t>(sysconf(number));
#endif
        napi_create_int32(env, configinfo, &result);
        return result;
    }

    napi_value ProcessManager::GetEnvironmentVar(napi_env env, napi_value name) const
    {
        size_t bufferSize = 0;
        if (napi_get_value_string_utf8(env, name, nullptr, 0, &bufferSize) != napi_ok) {
            HILOG_ERROR("can not get name size");
            return nullptr;
        }
        std::string result = "";
        result.reserve(bufferSize + 1);
        result.resize(bufferSize);
        if (napi_get_value_string_utf8(env, name, result.data(), bufferSize + 1, &bufferSize) != napi_ok) {
            HILOG_ERROR("can not get name value");
            return nullptr;
        }
        std::string temp = "";
        temp = result;
        char buf[260 * NUM_OF_DATA] = { 0 }; // 260:Only numbers path String size is 260.
        size_t length = sizeof(buf);
        auto envNum = uv_os_getenv(temp.c_str(), buf, &length);
        napi_value convertResult = nullptr;
        if (envNum == UV_ENOENT) {
            NAPI_CALL(env, napi_get_undefined(env, &convertResult));
            return convertResult;
        }
        napi_create_string_utf8(env, buf, strlen(buf), &convertResult);
        return convertResult;
    }

    void ProcessManager::Exit(napi_env env, napi_value number) const
    {
        int32_t result = 0;
        napi_get_value_int32(env, number, &result);
        exit(result);
    }

    napi_value ProcessManager::Kill(napi_env env, napi_value signal, napi_value proid)
    {
        int32_t pid = 0;
        int32_t sig = 0;
        napi_get_value_int32(env, proid, &pid);
        napi_get_value_int32(env, signal, &sig);
        uv_pid_t ownPid = uv_os_getpid();
        // 64:The maximum valid signal value is 64.
        if (sig > 64 && (!pid || pid == -1 || pid == ownPid || pid == -ownPid)) {
            napi_throw_error(env, "0", "process exit");
        }
        bool flag = false;
        int err = uv_kill(pid, sig);
        if (!err) {
            flag = true;
        }
        napi_value result = nullptr;
        NAPI_CALL(env, napi_get_boolean(env, flag, &result));
        return result;
    }
} // namespace OHOS::JsSysModule::Process
