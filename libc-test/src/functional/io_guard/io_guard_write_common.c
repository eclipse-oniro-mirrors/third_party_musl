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

#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <unistd.h>
#include "io_guard.h"
#include "io_guard_write_common.h"

#define TOKEN_A UINT64_C(0x123456780001)
#define TOKEN_B UINT64_C(0x123456780002)
#define SKIP 77

/* Only direct check sites exported by libc.map.txt, plus branches needed to
 * reach or bypass that site.
 * sendmmsg is direct on 32-bit and delegates to sendmsg on 64-bit.
 * stdio's private write_fd helper is reached through flush/direct output. */
static const char *names[] = {
    "write", "pwrite", "writev", "pwritev", "pwritev2/offset", "pwritev2/current", "pwritev2/flags",
    "close", "sendto", "sendmsg", "sendmmsg", "sendmmsg/empty", "sendfile", "splice", "tee",
    "copy_file_range", "dup", "dup2",
    "stdio/flush", "stdio/direct", "mmap/shared/read", "mmap/shared/write", "mmap/shared/none",
    "mmap/private/read", "mmap/private/write", "mmap/private/none", "mmap/anonymous",
    "dup3", "ftruncate", "fcntl/dupfd", "fcntl/dupfd_cloexec", "posix_fallocate",
    "fallocate"
};
static const char *levels[] = { "DISABLED", "WARN", "BLOCK", "FATAL" };
static const uint64_t tokens[] = { 0, TOKEN_A, TOKEN_B };
static const char *token_names[] = { "0", "A", "B" };
static int failed;
static char case_label[128] = "io_guard output setup";

void io_guard_write_check_failed(const char *file, int line, const char *condition)
{
    t_printf("%s:%d: %s: FAIL line %d: %s errno=%d\n", file, line, case_label, line, condition, errno);
    exit(1);
}

static int new_file(void)
{
    char path[] = "io_guard_matrix_XXXXXX";
    int fd = mkstemp(path);
    CHECK(fd >= 0);
    CHECK(unlink(path) == 0);
    return fd;
}

static int is_mmap_op(enum operation op)
{
    return op >= MMAP_SHARED_READ && op <= MMAP_ANON_CASE;
}

static void prepare(enum operation op, struct fixture *f)
{
    *f = (struct fixture){ .fd = -1, .input = -1, .peer = -1, .extra = -1, .byte = 'x' };
    long page = sysconf(_SC_PAGESIZE);
    CHECK(page > 0);
    f->page = (size_t)page;
    int pair[2];
    if (op >= SENDTO && op <= SENDMMSG_EMPTY) {
        CHECK(socketpair(AF_UNIX, SOCK_DGRAM | SOCK_NONBLOCK, 0, pair) == 0);
        f->fd = pair[0];
        f->peer = pair[1];
    } else if (op == SPLICE || op == TEE) {
        CHECK(pipe2(pair, O_NONBLOCK) == 0);
        f->peer = pair[0];
        f->fd = pair[1];
        CHECK(pipe2(pair, O_NONBLOCK) == 0);
        f->input = pair[0];
        f->extra = pair[1];
        CHECK(write(f->extra, "x", 1) == 1);
    } else {
        f->fd = new_file();
        if (op == SENDFILE || op == COPY_FILE_RANGE) {
            f->input = new_file();
            CHECK(write(f->input, "x", 1) == 1);
            CHECK(lseek(f->input, 0, SEEK_SET) == 0);
        }
        if (op == DUP2 || op == DUP3) {
            f->extra = new_file();
        }
        if (op == FTRUNCATE) {
            CHECK(write(f->fd, "x", 1) == 1);
            CHECK(lseek(f->fd, 0, SEEK_SET) == 0);
        }
        if (is_mmap_op(op)) {
            CHECK(ftruncate(f->fd, (off_t)f->page) == 0);
        }
    }
}

static int bypass(enum operation op)
{
    return op == SENDMMSG_EMPTY || op == MMAP_ANON_CASE;
}

static int unsupported(enum operation op, int error)
{
    if (op == PWRITEV2_FLAGS) {
        return error == ENOSYS || error == EOPNOTSUPP || error == EINVAL;
    }
    if (op == COPY_FILE_RANGE) {
        return error == ENOSYS || error == EOPNOTSUPP || error == EXDEV;
    }
    if (op == SPLICE || op == TEE || op == SENDMMSG) {
        return error == ENOSYS;
    }
    if (op == FALLOCATE) {
        return error == ENOSYS || error == EOPNOTSUPP || error == EINVAL;
    }
    return 0;
}

long io_guard_test_mmap(struct fixture *f, int prot, int flags)
{
    void *p = mmap(NULL, f->page, prot, flags, f->fd, 0);
    if (p == MAP_FAILED) {
        return -1;
    }
    if ((flags & MAP_SHARED) && (prot & PROT_WRITE)) {
        ((char *)p)[0] = 'x';
        CHECK(msync(p, f->page, MS_SYNC) == 0);
    } else if (prot & PROT_READ) {
        CHECK(((char *)p)[0] == 0);
    }
    CHECK(munmap(p, f->page) == 0);
    return 0;
}

static void child_case(enum operation op, struct fixture *f, int level, uint64_t token,
    io_guard_invoke_fn invoke)
{
    alarm(5);
    io_guard_set_error_level(IO_GUARD_DISABLED);
    if (op == STDIO_FLUSH || op == STDIO_DIRECT) {
        f->stream = fdopen(f->fd, "w");
        CHECK(f->stream != NULL);
        CHECK(setvbuf(f->stream, f->buffer, op == STDIO_FLUSH ? _IOFBF : _IONBF, sizeof f->buffer) == 0);
        if (op == STDIO_FLUSH) {
            CHECK(fwrite("x", 1, 1, f->stream) == 1);
            struct stat st;
            CHECK(fstat(f->fd, &st) == 0 && st.st_size == 0);
        }
    }
    CHECK(io_guard_exchange_token(f->fd, 0, token) == 0);
    /* Transfer source ownership must not be mistaken for destination ownership. */
    if (f->input >= 0) {
        CHECK(io_guard_exchange_token(f->input, 0, TOKEN_B) == 0);
    }
    io_guard_set_error_level(level);
    errno = 0;
    long result = invoke(f);
    int error = errno;
    int mismatch = token != 0 && !bypass(op);
    if (mismatch && level == IO_GUARD_FATAL) {
        CHECK(0); /* Returning is a failure, regardless of the syscall result. */
    }
    if (mismatch && level == IO_GUARD_BLOCK) {
        if (op == POSIX_FALLOCATE) {
            CHECK(result == EPERM);
        } else {
            if (result != -1 || error != EPERM) {
                t_error("%s: expect result=-1 errno=%d (EPERM)\n", case_label, EPERM);
                t_error("%s: actual result=%ld errno=%d\n", case_label, result, error);
                errno = error;
            }
            CHECK(result == -1 && error == EPERM);
        }
        CHECK(fcntl(f->fd, F_GETFD) >= 0);
        if (f->stream) {
            CHECK(ferror(f->stream));
        }
    } else {
        if (op == POSIX_FALLOCATE) {
            if (result == ENOSYS || result == EOPNOTSUPP || result == EINVAL) {
                CHECK(io_guard_exchange_token(f->fd, token, 0) == 0);
                _exit(SKIP);
            }
            CHECK(result == 0);
        } else {
            if (result == -1 && unsupported(op, error)) {
                CHECK(io_guard_exchange_token(f->fd, token, 0) == 0);
                _exit(SKIP);
            }
            long expected = (op == CLOSE || op == STDIO_FLUSH ||
                op == SENDMMSG_EMPTY || op == DUP || op == DUP2 ||
                op == DUP3 || op == FTRUNCATE || op == FCNTL_DUPFD ||
                op == FCNTL_DUPFD_CLOEXEC || op == FALLOCATE ||
                is_mmap_op(op)) ? 0 : 1;
            CHECK(result == expected);
            if (op == CLOSE) {
                CHECK(fcntl(f->fd, F_GETFD) == -1 && errno == EBADF);
            }
        }
    }
    /* Includes successful close: validation must never clear the stored token. */
    CHECK(io_guard_exchange_token(f->fd, token, 0) == 0);
    if (f->input >= 0) {
        CHECK(io_guard_exchange_token(f->input, TOKEN_B, 0) == 0);
    }
    if (f->stream) {
        clearerr(f->stream);
        CHECK(fclose(f->stream) == 0);
    }
    _exit(0);
}

static void verify_effect(enum operation op, struct fixture *f, int allowed)
{
    char byte = 0;
    if (op == CLOSE || op == DUP || op == DUP2 || op == DUP3 ||
        op == FCNTL_DUPFD || op == FCNTL_DUPFD_CLOEXEC) {
        return;
    }
    if (op >= SENDTO && op <= SENDMMSG_EMPTY) {
        ssize_t n = recv(f->peer, &byte, 1, 0);
        if (allowed && op != SENDMMSG_EMPTY) {
            CHECK(n == 1 && byte == 'x');
        } else {
            CHECK(n == -1 && errno == EAGAIN);
        }
    } else if (op == SPLICE || op == TEE) {
        ssize_t n = read(f->peer, &byte, 1);
        if (allowed) {
            CHECK(n == 1 && byte == 'x');
        } else {
            CHECK(n == -1 && errno == EAGAIN);
        }
    } else if (op == FTRUNCATE) {
        struct stat st;
        CHECK(fstat(f->fd, &st) == 0);
        CHECK(st.st_size == (allowed ? 0 : 1));
    } else if (op == POSIX_FALLOCATE || op == FALLOCATE) {
        struct stat st;
        CHECK(fstat(f->fd, &st) == 0);
        CHECK(st.st_size == (allowed ? 1 : 0));
    } else {
        int positioned = op == PWRITE || op == PWRITEV || op == PWRITEV2_OFFSET || op == PWRITEV2_FLAGS;
        struct stat st;
        CHECK(fstat(f->fd, &st) == 0);
        if (is_mmap_op(op)) {
            CHECK(st.st_size == (off_t)f->page);
            CHECK(pread(f->fd, &byte, 1, 0) == 1);
            CHECK(byte == (allowed && op == MMAP_SHARED_WRITE ? 'x' : 0));
        } else {
            CHECK(st.st_size == (allowed ? (positioned ? 2 : 1) : 0));
            CHECK(lseek(f->fd, 0, SEEK_CUR) == (allowed && !positioned ? 1 : 0));
            if (allowed) {
                CHECK(pread(f->fd, &byte, 1, positioned ? 1 : 0) == 1 && byte == 'x');
            }
        }
    }
}

static void run_case(enum operation op, int level, int tag, io_guard_invoke_fn invoke)
{
    /* Stable D001-D396 IDs: operation, then error level, then token. */
    int case_id = (op * 4 + level) * 3 + tag + 1;
    (void)snprintf(case_label, sizeof case_label, "[D%03d] %s level=%s token=%s",
        case_id, names[op], levels[level], token_names[tag]);
    struct fixture f;
    prepare(op, &f);
    pid_t child = fork();
    CHECK(child >= 0);
    if (child == 0) {
        child_case(op, &f, level, tokens[tag], invoke);
    }
    int status;
    pid_t waited;
    do {
        waited = waitpid(child, &status, 0);
    } while (waited < 0 && errno == EINTR);
    CHECK(waited == child);
    int mismatch = tokens[tag] != 0 && !bypass(op);
    int fatal = mismatch && level == IO_GUARD_FATAL;
    int blocked = mismatch && level == IO_GUARD_BLOCK;
    int ok = fatal ? WIFSIGNALED(status) && WTERMSIG(status) == SIGABRT :
        WIFEXITED(status) && WEXITSTATUS(status) == 0;
    int skipped = !fatal && !blocked && WIFEXITED(status) && WEXITSTATUS(status) == SKIP;
    if (ok) {
        verify_effect(op, &f, !fatal && !blocked);
    } else if (!skipped) {
        t_error("%s: FAIL unexpected child status=%d\n", case_label, status);
        ++failed;
    }
    CHECK(close(f.fd) == 0);
    int others[] = { f.input, f.peer, f.extra };
    for (unsigned i = 0; i < sizeof others / sizeof others[0]; ++i) {
        if (others[i] >= 0) {
            CHECK(close(others[i]) == 0);
        }
    }
}

int io_guard_run_write_test(enum operation op, io_guard_invoke_fn invoke)
{
    (void)snprintf(case_label, sizeof case_label, "%s setup", names[op]);
    struct rlimit core = {0, 0};
    CHECK(setrlimit(RLIMIT_CORE, &core) == 0);
    io_guard_set_error_level(IO_GUARD_DISABLED);
    for (int level = IO_GUARD_DISABLED; level <= IO_GUARD_FATAL; ++level) {
        for (int tag = 0; tag < 3; ++tag) {
            run_case(op, level, tag, invoke);
        }
    }
    return failed || t_status;
}
