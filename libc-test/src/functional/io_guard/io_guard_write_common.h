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

#ifndef IO_GUARD_WRITE_COMMON_H
#define IO_GUARD_WRITE_COMMON_H

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "test.h"

#define CHECK(x) do { \
    if (!(x)) { \
        io_guard_write_check_failed(__FILE__, __LINE__, #x); \
    } \
} while (0)

/* Keep the operation order: it defines the stable D001-D396 case IDs. */
enum operation {
    WRITE, PWRITE, WRITEV, PWRITEV, PWRITEV2_OFFSET, PWRITEV2_CURRENT, PWRITEV2_FLAGS,
    CLOSE, SENDTO, SENDMSG, SENDMMSG, SENDMMSG_EMPTY, SENDFILE, SPLICE, TEE,
    COPY_FILE_RANGE, DUP, DUP2,
    STDIO_FLUSH, STDIO_DIRECT,
    MMAP_SHARED_READ, MMAP_SHARED_WRITE, MMAP_SHARED_NONE,
    MMAP_PRIVATE_READ, MMAP_PRIVATE_WRITE, MMAP_PRIVATE_NONE, MMAP_ANON_CASE,
    DUP3, FTRUNCATE, FCNTL_DUPFD, FCNTL_DUPFD_CLOEXEC, POSIX_FALLOCATE, FALLOCATE,
    OP_COUNT
};
struct fixture {
    int fd, input, peer, extra;
    size_t page;
    char byte;
    char buffer[128];
    FILE *stream;
};

typedef long (*io_guard_invoke_fn)(struct fixture *);

void io_guard_write_check_failed(const char *file, int line, const char *condition);
/* Run one path: four error levels, three registered tokens (12 cases). */
int io_guard_run_write_test(enum operation op, io_guard_invoke_fn invoke);
long io_guard_test_mmap(struct fixture *f, int prot, int flags);

#endif /* IO_GUARD_WRITE_COMMON_H */
