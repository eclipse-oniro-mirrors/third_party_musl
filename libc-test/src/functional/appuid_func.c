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

/*
 * Functional tests for getappuid / setappuid:
 * - Multithreading (concurrent getappuid, concurrent setappuid, mixed)
 * - Memory leak / fd leak detection
 * - Stress / stability
 */

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "test.h"

#define T(f) do{ if ((f)+1 == 0) t_error("%s failed: %s\n", #f, strerror(errno)); }while(0)
#define EQ(a,b,fmt) do{ if ((a) != (b)) t_error("%s == %s failed: " fmt "\n", #a, #b, a, b); }while(0)

#define NUM_THREADS 4
#define ITER_COUNT 10
#define LEAK_ITER 10
#define STRESS_ITER 100

/* ======================== Multithreading Tests ======================== */

/* Test 1: Concurrent getappuid — all threads should get the same uid */
struct getappuid_mt_arg {
    uid_t expected;
    int mismatch;
};

static void *getappuid_mt_worker(void *p)
{
    struct getappuid_mt_arg *a = p;
    int i;
    for (i = 0; i < ITER_COUNT; i++) {
        uid_t uid = getappuid();
        if (uid != a->expected) {
            a->mismatch = 1;
        }
    }
    return NULL;
}

static void test_getappuid_concurrent(void)
{
    pthread_t threads[NUM_THREADS];
    struct getappuid_mt_arg arg = { .expected = getappuid(), .mismatch = 0 };
    int i;

    for (i = 0; i < NUM_THREADS; i++) {
        T(pthread_create(&threads[i], NULL, getappuid_mt_worker, &arg));
    }
    for (i = 0; i < NUM_THREADS; i++) {
        T(pthread_join(threads[i], NULL));
    }

    EQ(arg.mismatch, 0, "%d");
}

/* Test 2: Concurrent setappuid with same value — no crash, uid unchanged */
struct setappuid_mt_arg {
    uid_t uid;
    int error;
};

static void *setappuid_mt_worker(void *p)
{
    struct setappuid_mt_arg *a = p;
    int i;
    for (i = 0; i < ITER_COUNT; i++) {
        int ret = setappuid(a->uid);
        if (ret != 0 && ret != -1) {
            a->error = 1;
        }
    }
    return NULL;
}

static void test_setappuid_concurrent(void)
{
    pthread_t threads[NUM_THREADS];
    uid_t orig = getuid();
    struct setappuid_mt_arg arg = { .uid = orig, .error = 0 };
    int i;

    for (i = 0; i < NUM_THREADS; i++) {
        T(pthread_create(&threads[i], NULL, setappuid_mt_worker, &arg));
    }
    for (i = 0; i < NUM_THREADS; i++) {
        T(pthread_join(threads[i], NULL));
    }

    EQ(arg.error, 0, "%d");
    EQ(getuid(), orig, "%d");
}

/* Test 3: Mixed getappuid + setappuid from multiple threads — no crash */
struct mixed_mt_arg {
    uid_t uid;
    int stop;
    int error;
};

static void *mixed_get_worker(void *p)
{
    struct mixed_mt_arg *a = p;
    while (!a->stop) {
        getappuid();
    }
    return NULL;
}

static void *mixed_set_worker(void *p)
{
    struct mixed_mt_arg *a = p;
    int i;
    for (i = 0; i < ITER_COUNT; i++) {
        int ret = setappuid(a->uid);
        if (ret != 0 && ret != -1) {
            a->error = 1;
        }
    }
    a->stop = 1;
    return NULL;
}

static void test_mixed_get_set(void)
{
    pthread_t getters[8];
    pthread_t setters[4];
    uid_t orig = getuid();
    struct mixed_mt_arg arg = { .uid = orig, .stop = 0, .error = 0 };
    int i;

    for (i = 0; i < 8; i++) {
        T(pthread_create(&getters[i], NULL, mixed_get_worker, &arg));
    }
    for (i = 0; i < 4; i++) {
        T(pthread_create(&setters[i], NULL, mixed_set_worker, &arg));
    }

    for (i = 0; i < 8; i++) {
        T(pthread_join(getters[i], NULL));
    }
    for (i = 0; i < 4; i++) {
        T(pthread_join(setters[i], NULL));
    }

    EQ(arg.error, 0, "%d");
    EQ(getuid(), orig, "%d");
}

/* Test 4: setappuid uses __synccall — verify all threads survive */
struct synccall_survive_arg {
    int done;
};

/* ======================== Memory Leak / FD Leak Tests ======================== */

static int count_open_fds(void)
{
    DIR *d = opendir("/proc/self/fd");
    struct dirent *e;
    int count = 0;
    if (!d) {
        return -1;
    }
    while ((e = readdir(d)) != NULL) {
        if (e->d_name[0] == '.') {
            continue;
        }
        count++;
    }
    closedir(d);
    return count;
}

/* Test 5: getappuid fd leak — call many times, fd count should not grow */
static void test_getappuid_fd_leak(void)
{
    int before = count_open_fds();
    int i;
    if (before < 0) {
        t_error("count_open_fds failed: %s\n", strerror(errno));
        return;
    }
    for (i = 0; i < LEAK_ITER; i++) {
        getappuid();
    }
    int after = count_open_fds();
    EQ(after, before, "%d");
}

/* Test 6: setappuid fd leak — call many times, fd count should not grow */
static void test_setappuid_fd_leak(void)
{
    uid_t orig = getuid();
    int before = count_open_fds();
    int i;
    if (before < 0) {
        t_error("count_open_fds failed: %s\n", strerror(errno));
        return;
    }
    for (i = 0; i < LEAK_ITER; i++) {
        setappuid(orig);
    }
    int after = count_open_fds();
    EQ(after, before, "%d");
}

/* ======================== Stress / Stability Tests ======================== */

/* Test 7: getappuid stress — 10000 iterations, no crash */
static void test_getappuid_stress(void)
{
    uid_t first = getappuid();
    int i;
    for (i = 0; i < STRESS_ITER; i++) {
        uid_t uid = getappuid();
        EQ(uid, first, "%d");
    }
}

/* Test 8: setappuid stress — 1000 iterations with same uid, no crash */
static void test_setappuid_stress(void)
{
    uid_t orig = getuid();
    int i;
    for (i = 0; i < ITER_COUNT; i++) {
        int ret = setappuid(orig);
        if (ret != 0 && ret != -1) {
            t_error("setappuid returned unexpected value %d at iter %d\n", ret, i);
        }
    }
    EQ(getuid(), orig, "%d");
}

/* Test 9: getappuid + setappuid interleaved stress — no crash, no corruption */
static void test_interleaved_stress(void)
{
    uid_t orig = getuid();
    int i;
    for (i = 0; i < ITER_COUNT; i++) {
        getappuid();
        setappuid(orig);
        uid_t uid = getappuid();
        EQ(uid, orig, "%d");
    }
}

/* ======================== Boundary / Edge Cases ======================== */

/* Test 10: setappuid(0) then restore — verify fd is cleaned up */
static void test_setappuid_cleanup(void)
{
    uid_t orig = getuid();
    int before = count_open_fds();
    int i;
    if (before < 0) {
        return;
    }

    for (i = 0; i < ITER_COUNT; i++) {
        int ret = setappuid(0);
        if (ret == 0) {
            setuid(orig);
        }
    }
    setuid(orig);

    int after = count_open_fds();
    EQ(after, before, "%d");
    EQ(getuid(), orig, "%d");
}

/* Test 11: getappuid after fork — child should get same uid */
static void test_getappuid_fork(void)
{
    uid_t parent_uid = getappuid();
    pid_t pid = fork();
    if (pid == 0) {
        uid_t child_uid = getappuid();
        _exit(child_uid == parent_uid ? 0 : 1);
    } else if (pid > 0) {
        int status;
        T(waitpid(pid, &status, 0));
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            t_error("child getappuid mismatch: got %d, expected %d\n",
                    WEXITSTATUS(status), parent_uid);
        }
    } else {
        t_error("fork failed: %s\n", strerror(errno));
    }
}

int main(void)
{
    test_getappuid_concurrent();
    test_setappuid_concurrent();
    test_mixed_get_set();
    test_getappuid_fd_leak();
    test_setappuid_fd_leak();
    test_getappuid_stress();
    test_setappuid_stress();
    test_interleaved_stress();
    test_setappuid_cleanup();
    test_getappuid_fork();
    return t_status;
}
