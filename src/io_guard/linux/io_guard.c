/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifdef IO_GUARD_ENABLE
#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <unistd.h>
#include "io_guard_impl.h"
#include "libc.h"
#include "pthread_impl.h"
#include "stdatomic_impl.h"
#include "syscall.h"

#include "musl_log.h"

#ifdef OHOS_ENABLE_PARAMETER
#include <string.h>
#include "sys_param.h"
#endif

#define IO_GUARD_TABLE_SIZE (128)
#define IO_GUARD_ALIGN_UP(x, a) (((x) + (a) - 1) & ~((a) - 1))

struct io_guard_entry {
    _Atomic(uint64_t) token;
};

struct io_guard_overflow {
    size_t len;
    struct io_guard_entry entries[];
};

/* Tokens are stored the same way as fdsan: 128 inline entries plus one
 * lazily mmap'd table covering the RLIMIT_NOFILE hard limit (32768 when
 * the limit is infinite). The token itself stays a separate structure. */
static struct io_guard_entry io_guard_entries[IO_GUARD_TABLE_SIZE];
static _Atomic(struct io_guard_overflow *) io_guard_overflow;
static atomic_int policy = IO_GUARD_WARN;
/* Suppress nested diagnostics, never the nested token check or its policy. */
static _Thread_local volatile sig_atomic_t reporting;

/* Same fd upper limit as fdsan: the RLIMIT_NOFILE hard limit, with an
 * infinite limit treated as 32768. */
static rlim_t fd_limit(void)
{
    struct rlimit rlim = { .rlim_max = 32768 };
    getrlimit(RLIMIT_NOFILE, &rlim);
    return rlim.rlim_max == RLIM_INFINITY ? 32768 : rlim.rlim_max;
}

static _Atomic(uint64_t) *entry(int fd)
{
    if (fd < 0) {
        return NULL;
    }
    if ((size_t)fd < IO_GUARD_TABLE_SIZE) {
        return &io_guard_entries[fd].token;
    }
    struct io_guard_overflow *local =
        atomic_load_explicit(&io_guard_overflow, memory_order_acquire);
    if (!local) {
        rlim_t max = fd_limit();
        if ((rlim_t)(size_t)fd > max) {
            return NULL;
        }
        /* Cover indices [0, max - IO_GUARD_TABLE_SIZE] so that fd == max,
         * which the bounds check below accepts, has a slot. */
        size_t required_count = (size_t)max - IO_GUARD_TABLE_SIZE + 1;
        size_t required_size = sizeof(struct io_guard_overflow) +
                               required_count * sizeof(struct io_guard_entry);
        size_t aligned_size = IO_GUARD_ALIGN_UP(required_size, PAGE_SIZE);
        size_t aligned_count =
            (aligned_size - sizeof(struct io_guard_overflow)) / sizeof(struct io_guard_entry);
        void *allocation =
            mmap(NULL, aligned_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (allocation == MAP_FAILED) {
            return NULL;
        }
        struct io_guard_overflow *fresh = allocation;
        fresh->len = aligned_count;
        struct io_guard_overflow *expected = NULL;
        if (atomic_compare_exchange_strong_explicit(&io_guard_overflow, &expected, fresh,
                                                    memory_order_acq_rel, memory_order_acquire)) {
            local = fresh;
        } else {
            munmap(fresh, aligned_size);
            local = expected;
        }
    }
    size_t offset = (size_t)fd - IO_GUARD_TABLE_SIZE;
    if (local->len <= offset) {
        return NULL;
    }
    return &local->entries[offset].token;
}

/* Mirror the table bounds without allocating: fd numbers are limited by the
 * same RLIMIT_NOFILE rule that sizes the overflow table. */
static int fd_in_range(int fd)
{
    if (fd < 0) {
        return 0;
    }
    if ((size_t)fd < IO_GUARD_TABLE_SIZE) {
        return 1;
    }
    struct io_guard_overflow *local =
        atomic_load_explicit(&io_guard_overflow, memory_order_acquire);
    if (local) {
        return (size_t)(fd - IO_GUARD_TABLE_SIZE) < local->len;
    }
    return (rlim_t)(size_t)fd <= fd_limit();
}

/* Read-only lookup: never grows the overflow table. Entries outside the
 * currently stored range read as 0. */
uint64_t __io_guard_get_token(int fd)
{
    if (fd < 0) {
        return 0;
    }
    if ((size_t)fd < IO_GUARD_TABLE_SIZE) {
        return atomic_load_explicit(&io_guard_entries[fd].token, memory_order_acquire);
    }
    struct io_guard_overflow *local =
        atomic_load_explicit(&io_guard_overflow, memory_order_acquire);
    if (!local) {
        return 0;
    }
    size_t offset = (size_t)fd - IO_GUARD_TABLE_SIZE;
    if (offset >= local->len) {
        return 0;
    }
    return atomic_load_explicit(&local->entries[offset].token, memory_order_acquire);
}

/* Logging must not change the caller's errno or the mismatch policy. */
static void report(int fd, uint64_t expected, uint64_t actual)
{
    if (reporting) {
        return;
    }
    int saved = errno;
    reporting = 1;
    (void)HiLogAdapterPrint(MUSL_LOG_TYPE, LOG_ERROR, MUSL_LOG_DOMAIN, "MUSL-IO-GUARD",
        "io_guard: fd=%{public}d expected=0x%{public}016llx actual=0x%{public}016llx",
        fd, (unsigned long long)expected, (unsigned long long)actual);
    reporting = 0;
    errno = saved;
}

enum io_guard_error_level io_guard_get_error_level(void)
{
    if (__pthread_self()->by_vfork) {
        return IO_GUARD_DISABLED;
    }
    return atomic_load_explicit(&policy, memory_order_relaxed);
}

enum io_guard_error_level io_guard_set_error_level(enum io_guard_error_level level)
{
    if ((unsigned)level > IO_GUARD_FATAL) {
        errno = EINVAL;
        return io_guard_get_error_level();
    }
    if (__pthread_self()->by_vfork) {
        errno = ENOTSUP;
        return IO_GUARD_DISABLED;
    }
    return atomic_exchange_explicit(&policy, level, memory_order_relaxed);
}

/* Read the startup policy once, after the parameter client is initialized. */
void __init_io_guard(void)
{
    enum io_guard_error_level level = IO_GUARD_WARN;
#ifdef OHOS_ENABLE_PARAMETER
    CachedHandle handle = CachedParameterCreate("musl.debug.io_guard", "warn");
    if (handle != NULL) {
        const char *value = CachedParameterGet(handle);
        if (value != NULL) {
            if (strcmp(value, "disabled") == 0) {
                level = IO_GUARD_DISABLED;
            } else if (strcmp(value, "warn") == 0) {
                level = IO_GUARD_WARN;
            } else if (strcmp(value, "block") == 0) {
                level = IO_GUARD_BLOCK;
            } else if (strcmp(value, "fatal") == 0) {
                level = IO_GUARD_FATAL;
            }
        }
        CachedParameterDestroy(handle);
    }
#endif
    io_guard_set_error_level(level);
}

int __io_guard_check(int fd, uint64_t expected)
{
    int level = io_guard_get_error_level();
    if (level == IO_GUARD_DISABLED) {
        return 0;
    }
    uint64_t actual = __io_guard_get_token(fd);
    if (actual == expected) {
        return 0;
    }
    report(fd, expected, actual);
    if (level == IO_GUARD_WARN) {
        return 0;
    }
    if (level == IO_GUARD_FATAL) {
        abort();
    }
    if (level == IO_GUARD_BLOCK) {
        errno = EPERM;
        return -1;
    }
    return 0;
}

int io_guard_exchange_token(int fd, uint64_t expected, uint64_t replacement)
{
    if (__pthread_self()->by_vfork) {
        errno = ENOTSUP;
        return -1;
    }
    if (expected == UINT64_MAX || replacement == UINT64_MAX) {
        errno = EINVAL;
        return -1;
    }
    if (!fd_in_range(fd)) {
        errno = EBADF;
        return -1;
    }
    if (replacement && __syscall(SYS_fcntl, fd, F_GETFD) < 0) {
        errno = EBADF;
        return -1;
    }
    _Atomic(uint64_t) *p = entry(fd);
    if (!p) {
        /* After the range check, a NULL entry means the fdsan-style
         * overflow table could not be allocated. */
        if (replacement) {
            errno = ENOMEM;
            return -1;
        }
        if (!expected) {
            return 0;
        }
        errno = EPERM;
        return -1;
    }
    /* io_guard owns only its token table; fdsan tags are independent. */
    int ok = atomic_compare_exchange_strong_explicit(p, &expected, replacement,
                                                     memory_order_acq_rel, memory_order_acquire);
    if (!ok) {
        errno = EPERM;
        return -1;
    }
    return 0;
}

ssize_t io_guard_write(int fd, const void *buf, size_t count, uint64_t token)
{
    if (!token || token == UINT64_MAX) {
        errno = EINVAL;
        return -1;
    }
    if (__io_guard_check(fd, token) < 0) {
        return -1;
    }
    return syscall_cp(SYS_write, fd, buf, count);
}

ssize_t io_guard_pwrite(int fd, const void *buf, size_t count, off_t offset, uint64_t token)
{
    if (!token || token == UINT64_MAX) {
        errno = EINVAL;
        return -1;
    }
    if (__io_guard_check(fd, token) < 0) {
        return -1;
    }
    return syscall_cp(SYS_pwrite, fd, buf, count, __SYSCALL_LL_PRW(offset));
}

/* Keep mapping validation and syscall conversion consistent with __mmap. */
void *io_guard_mmap(void *start, size_t len, int prot, int flags, int fd,
                    off_t off, uint64_t token)
{
    if (!token || token == UINT64_MAX) {
        errno = EINVAL;
        return MAP_FAILED;
    }
    if (!(flags & MAP_ANON) && __io_guard_check(fd, token) < 0) {
        return MAP_FAILED;
    }
    const uint64_t off_mask = (-0x2000ULL << (8 * sizeof(syscall_arg_t) - 1)) |
                              (SYSCALL_MMAP2_UNIT - 1);
    if (off & off_mask) {
        errno = EINVAL;
        return MAP_FAILED;
    }
    if (len >= PTRDIFF_MAX) {
        errno = ENOMEM;
        return MAP_FAILED;
    }
    if (len == 0) {
        errno = EINVAL;
        return MAP_FAILED;
    }
    if (flags & MAP_FIXED) {
        __vm_wait();
    }
    long ret;
#ifdef SYS_mmap2
    ret = __syscall(SYS_mmap2, start, len, prot, flags, fd, off / SYSCALL_MMAP2_UNIT);
#else
    ret = __syscall(SYS_mmap, start, len, prot, flags, fd, off);
#endif
    /* Match libc's correction of anonymous mmap EPERM to ENOMEM. */
    if (ret == -EPERM && !start && (flags & MAP_ANON) && !(flags & MAP_FIXED)) {
        ret = -ENOMEM;
    }
    return (void *)__syscall_ret(ret);
}

#endif /* IO_GUARD_ENABLE */
