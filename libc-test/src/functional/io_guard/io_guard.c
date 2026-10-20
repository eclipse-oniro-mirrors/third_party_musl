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
#include <limits.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include "io_guard.h"
#include "test.h"

#define CHECK(x) do { \
    if (!(x)) { \
        t_error("%s: FAIL line %d: %s errno=%d\n", case_label, __LINE__, #x, errno); \
        exit(1); \
    } \
} while (0)
#define TOKEN_A UINT64_C(0x123456780001)
#define TOKEN_B UINT64_C(0x123456780002)
static const uint64_t tokens[] = { 0, TOKEN_A, TOKEN_B, UINT64_MAX };
static const char *token_names[] = { "0", "A", "B", "MAX" };
static const char *levels[] = { "DISABLED", "WARN", "BLOCK", "FATAL" };
/* Only new API contracts; ordinary interception paths run in separate io_guard_<path> binaries. */
static const char *operations[] = {
    "write", "pwrite", "mmap/file", "mmap/anonymous"
};
/* Indices into tokens: matching, mismatching, unowned, zero, invalid maximum. */
static const struct {
    int stored;
    int supplied;
} api_token_cases[] = {
    { 1, 1 }, { 1, 2 }, { 0, 1 }, { 1, 0 }, { 1, 3 }
};
static size_t page_size;
static int failed;
static char case_label[160] = "io_guard setup";

static void set_exchange_case(char group, int id, int level, const char *fd_state,
    const char *stored, const char *expected, const char *replacement)
{
    (void)snprintf(case_label, sizeof case_label,
        "[%c%03d] exchange level=%s fd=%s stored=%s expected=%s new=%s",
        group, id, levels[level], fd_state, stored, expected, replacement);
}

static void set_process_case(int id)
{
    static const char *descriptions[] = {
        "fork child get_error_level -> DISABLED",
        "fork child exchange_token A -> A",
        "vfork child get_error_level -> DISABLED",
        "vfork child set_error_level WARN -> DISABLED/ENOTSUP",
        "vfork child set_error_level -1 -> DISABLED/EINVAL",
        "vfork child exchange_token A -> 0 -> -1/ENOTSUP",
        "parent get_error_level -> BLOCK",
        "parent exchange_token A -> 0"
    };
    (void)snprintf(case_label, sizeof case_label, "[C%03d] %s", id, descriptions[id - 1]);
}

static int new_file(void)
{
    char path[] = "io_guard_api_XXXXXX";
    int fd = mkstemp(path);
    CHECK(fd >= 0);
    CHECK(unlink(path) == 0);
    return fd;
}

static int wait_child(pid_t child)
{
    int status;
    pid_t waited;
    do {
        waited = waitpid(child, &status, 0);
    } while (waited < 0 && errno == EINTR);
    CHECK(waited == child);
    return status;
}

static long call_api(int op, int fd, uint64_t token)
{
    if (op == 0) {
        return io_guard_write(fd, "x", 1, token);
    }
    if (op == 1) {
        return io_guard_pwrite(fd, "x", 1, 1, token);
    }
    int flags = MAP_SHARED;
    if (op == 3) {
        flags = MAP_PRIVATE | MAP_ANONYMOUS;
    }
    void *p = io_guard_mmap(NULL, page_size, PROT_READ | PROT_WRITE, flags, fd, 0, token);
    if (p == MAP_FAILED) {
        return -1;
    }
    CHECK(((char *)p)[0] == 0);
    ((char *)p)[0] = 'x';
    if (op == 2) {
        CHECK(msync(p, page_size, MS_SYNC) == 0);
    }
    CHECK(munmap(p, page_size) == 0);
    return 0;
}

static void run_api_case(int op, int level, int token_case)
{
    int stored = api_token_cases[token_case].stored;
    int supplied = api_token_cases[token_case].supplied;
    /* TEST_MATRIX.md G001-G080: operation, then error level, then token case. */
    int case_id = (op * 4 + level) * 5 + token_case + 1;
    (void)snprintf(case_label, sizeof case_label,
        "[G%03d] io_guard_%s level=%s stored=%s supplied=%s",
        case_id, operations[op], levels[level], token_names[stored], token_names[supplied]);
    int fd = new_file();
    if (op >= 2) {
        CHECK(ftruncate(fd, (off_t)page_size) == 0);
    }
    int invalid = supplied == 0 || supplied == 3;
    int mismatch = !invalid && op != 3 && stored != supplied;
    int fatal = mismatch && level == IO_GUARD_FATAL;
    int blocked = mismatch && level == IO_GUARD_BLOCK;
    pid_t child = fork();
    CHECK(child >= 0);
    if (child == 0) {
        alarm(5);
        CHECK(io_guard_exchange_token(fd, 0, tokens[stored]) == 0);
        io_guard_set_error_level(level);
        errno = 0;
        long result = call_api(op, fd, tokens[supplied]);
        int error = errno;
        CHECK(!fatal);
        if (invalid || blocked) {
            CHECK(result == -1 && error == (invalid ? EINVAL : EPERM));
        } else {
            CHECK(result == (op < 2 ? 1 : 0));
        }
        CHECK(io_guard_exchange_token(fd, tokens[stored], 0) == 0);
        _exit(0);
    }
    int status = wait_child(child);
    int ok = fatal ? WIFSIGNALED(status) && WTERMSIG(status) == SIGABRT :
        WIFEXITED(status) && WEXITSTATUS(status) == 0;
    if (ok) {
        int allowed = !invalid && !blocked && !fatal;
        struct stat st;
        char byte = 0;
        CHECK(fstat(fd, &st) == 0);
        if (op >= 2) {
            CHECK(st.st_size == (off_t)page_size);
            CHECK(pread(fd, &byte, 1, 0) == 1);
            CHECK(byte == (allowed && op == 2 ? 'x' : 0));
        } else {
            CHECK(st.st_size == (allowed ? op + 1 : 0));
            CHECK(lseek(fd, 0, SEEK_CUR) == (allowed && op == 0 ? 1 : 0));
            if (allowed) {
                CHECK(pread(fd, &byte, 1, op) == 1 && byte == 'x');
            }
        }
    } else {
        t_error("%s: FAIL unexpected child status=%d\n", case_label, status);
        ++failed;
    }
    CHECK(close(fd) == 0);
}

static void test_exchange(void)
{
    for (int level = IO_GUARD_DISABLED; level <= IO_GUARD_FATAL; ++level) {
        set_exchange_case('X', level * 2 + 1, level, "open", "A", "A", "B");
        int fd = new_file();
        io_guard_set_error_level(level);
        for (int expected = 1; expected <= 2; ++expected) {
            set_exchange_case('X', level * 2 + expected, level, "open", "A", token_names[expected], "B");
            CHECK(io_guard_exchange_token(fd, 0, TOKEN_A) == 0);
            errno = 0;
            int result = io_guard_exchange_token(fd, tokens[expected], TOKEN_B);
            int error = errno;
            if (expected == 1) {
                CHECK(result == 0);
                CHECK(io_guard_exchange_token(fd, TOKEN_B, 0) == 0);
            } else {
                CHECK(result == -1 && error == EPERM);
                CHECK(io_guard_exchange_token(fd, TOKEN_A, 0) == 0);
            }
        }
        /* Invalid token arguments are separate from the match/mismatch cases. */
        set_exchange_case('T', level * 2 + 1, level, "open", "A", "MAX", "B");
        CHECK(io_guard_exchange_token(fd, 0, TOKEN_A) == 0);
        CHECK(io_guard_exchange_token(fd, UINT64_MAX, TOKEN_B) == -1 && errno == EINVAL);
        CHECK(io_guard_exchange_token(fd, TOKEN_A, TOKEN_A) == 0);
        set_exchange_case('T', level * 2 + 2, level, "open", "A", "A", "MAX");
        CHECK(io_guard_exchange_token(fd, TOKEN_A, UINT64_MAX) == -1 && errno == EINVAL);
        CHECK(io_guard_exchange_token(fd, TOKEN_A, 0) == 0);
        /* Invalid descriptor checks, including token validation precedence. */
        struct rlimit rl;
        CHECK(getrlimit(RLIMIT_NOFILE, &rl) == 0);
        rlim_t max_fd = (rl.rlim_max == RLIM_INFINITY) ? 32768 : rl.rlim_max;
        if (max_fd < 127) max_fd = 127;
        const int invalid_fds[] = {
            -1, (max_fd < INT_MAX) ? (int)max_fd + 1 : -1
        };
        const char *invalid_fd_names[] = { "-1", "RLIMIT_NOFILE+1" };
        for (unsigned i = 0; i < sizeof invalid_fds / sizeof invalid_fds[0]; ++i) {
            int first_id = level * 11 + i * 3 + 1;
            set_exchange_case('E', first_id, level, invalid_fd_names[i], "none", "0", "A");
            CHECK(io_guard_exchange_token(invalid_fds[i], 0, TOKEN_A) == -1 && errno == EBADF);
            set_exchange_case('E', first_id + 1, level, invalid_fd_names[i], "none", "0", "0");
            CHECK(io_guard_exchange_token(invalid_fds[i], 0, 0) == -1 && errno == EBADF);
            set_exchange_case('E', first_id + 2, level, invalid_fd_names[i], "none", "MAX", "0");
            CHECK(io_guard_exchange_token(invalid_fds[i], UINT64_MAX, 0) == -1 && errno == EINVAL);
        }
        set_exchange_case('E', level * 11 + 7, level, "closed", "0", "0", "A");
        CHECK(close(fd) == 0);
        CHECK(io_guard_exchange_token(fd, 0, TOKEN_A) == -1 && errno == EBADF);
        set_exchange_case('E', level * 11 + 8, level, "closed", "0", "0", "0");
        CHECK(io_guard_exchange_token(fd, 0, 0) == 0);
        set_exchange_case('E', level * 11 + 9, level, "closed", "0", "A", "0");
        CHECK(io_guard_exchange_token(fd, TOKEN_A, 0) == -1 && errno == EPERM);
        /* Public exchange contract permits clearing a stale token on a closed fd. */
        set_exchange_case('E', level * 11 + 10, level, "closed/stale-token", "A", "B", "0");
        fd = new_file();
        CHECK(io_guard_exchange_token(fd, 0, TOKEN_A) == 0);
        io_guard_set_error_level(IO_GUARD_DISABLED);
        CHECK(close(fd) == 0);
        io_guard_set_error_level(level);
        CHECK(io_guard_exchange_token(fd, TOKEN_B, 0) == -1 && errno == EPERM);
        set_exchange_case('E', level * 11 + 11, level, "closed/stale-token", "A", "A", "0");
        CHECK(io_guard_exchange_token(fd, TOKEN_A, 0) == 0);
    }
    io_guard_set_error_level(IO_GUARD_DISABLED);
}

/* These are API process-context contracts, not fork/vfork stress tests. */
static void test_process_contract(void)
{
    (void)snprintf(case_label, sizeof case_label, "[C001-C002] fork setup/wait");
    int fd = new_file();
    CHECK(io_guard_exchange_token(fd, 0, TOKEN_A) == 0);
    io_guard_set_error_level(IO_GUARD_BLOCK);
    pid_t child = fork();
    CHECK(child >= 0);
    if (child == 0) {
        set_process_case(1);
        CHECK(io_guard_get_error_level() == IO_GUARD_DISABLED);
        set_process_case(2);
        CHECK(io_guard_exchange_token(fd, TOKEN_A, TOKEN_A) == 0);
        _exit(0);
    }
    int status = wait_child(child);
    CHECK(WIFEXITED(status) && WEXITSTATUS(status) == 0);
    (void)snprintf(case_label, sizeof case_label, "[C003-C006] vfork setup/wait");
    child = vfork();
    if (child == 0) {
        if (io_guard_get_error_level() != IO_GUARD_DISABLED) {
            _exit(1);
        }
        errno = 0;
        if (io_guard_set_error_level(IO_GUARD_WARN) != IO_GUARD_DISABLED || errno != ENOTSUP) {
            _exit(2);
        }
        errno = 0;
        if (io_guard_set_error_level(-1) != IO_GUARD_DISABLED || errno != EINVAL) {
            _exit(3);
        }
        errno = 0;
        if (io_guard_exchange_token(fd, TOKEN_A, 0) != -1 || errno != ENOTSUP) {
            _exit(4);
        }
        _exit(0);
    }
    CHECK(child > 0);
    status = wait_child(child);
    /* Keep stdio and label updates out of the shared vfork child context. */
    CHECK(WIFEXITED(status) && WEXITSTATUS(status) <= 4);
    if (WEXITSTATUS(status) != 0) {
        set_process_case(WEXITSTATUS(status) + 2);
        t_error("%s: FAIL child exit=%d\n", case_label, WEXITSTATUS(status));
        exit(1);
    }
    set_process_case(7);
    CHECK(io_guard_get_error_level() == IO_GUARD_BLOCK);
    set_process_case(8);
    CHECK(io_guard_exchange_token(fd, TOKEN_A, 0) == 0);
    CHECK(close(fd) == 0);
    io_guard_set_error_level(IO_GUARD_DISABLED);
}

int main(void)
{
    struct rlimit core = {0, 0};
    CHECK(setrlimit(RLIMIT_CORE, &core) == 0);
    long page = sysconf(_SC_PAGESIZE);
    CHECK(page > 0);
    page_size = (size_t)page;
    (void)snprintf(case_label, sizeof case_label, "[L001] io_guard_set_error_level/get_error_level");
    io_guard_set_error_level(IO_GUARD_DISABLED);
    (void)io_guard_get_error_level();
    for (unsigned op = 0; op < sizeof operations / sizeof operations[0]; ++op) {
        for (int level = IO_GUARD_DISABLED; level <= IO_GUARD_FATAL; ++level) {
            for (unsigned i = 0; i < sizeof api_token_cases / sizeof api_token_cases[0]; ++i) {
                run_api_case(op, level, i);
            }
        }
    }
    test_exchange();
    test_process_contract();
    return failed || t_status;
}
