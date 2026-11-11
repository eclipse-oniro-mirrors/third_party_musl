/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdbool.h>
#include <dlfcn.h>
#include <dlfcn_ext.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "test.h"

#ifndef EXPECT_RELATIVE_PATH
#error "Set EXPECT_RELATIVE_PATH to match the libc build configuration"
#endif

#define FIXTURE "librelative_path_fixture.so"
#define COLD_FIXTURE "librelative_path_cold.so"
#define HIDDEN_FIXTURE "librelative_path_hidden.so"
#define DEFAULT_FIXTURE "/data/local/tmp/libc-test-lib/" FIXTURE
#define THREAD_COUNT 6
#define ITERATIONS 500

static char root[] = "/data/local/tmp/dlopen-relative-XXXXXX";
static char allowed[PATH_MAX], blocked[PATH_MAX];
static char library[PATH_MAX], blocked_library[PATH_MAX];
static char cold_library[PATH_MAX], hidden_library[PATH_MAX];
static char good_link[PATH_MAX], bad_link[PATH_MAX];
static bool root_created;
static Dl_namespace ns;
static void *fixture_handle;
static void (*enable_relative_path)(void);
static void *(*fixture_open)(const char *, int, int);

static int make_path(char *out, const char *base, const char *leaf)
{
    int n = snprintf(out, PATH_MAX, "%s/%s", base, leaf);
    if (n < 0 || n >= PATH_MAX) {
        t_error("test path exceeds PATH_MAX\n");
        return -1;
    }
    return 0;
}

static int copy_library(const char *src, const char *dst)
{
    int input = open(src, O_RDONLY | O_CLOEXEC);
    if (input < 0) {
        t_error("open fixture %s: %s\n", src, strerror(errno));
        return -1;
    }
    int output = open(dst, O_WRONLY | O_CREAT | O_EXCL | O_CLOEXEC, 0700);
    if (output < 0) {
        t_error("create fixture %s: %s\n", dst, strerror(errno));
        close(input);
        return -1;
    }
    int result = 0;
    char buffer[4096];
    for (;;) {
        ssize_t size = read(input, buffer, sizeof(buffer));
        if (size < 0 && errno == EINTR) {
            continue;
        }
        if (size == 0) {
            break;
        }
        if (size < 0) {
            result = -1;
            break;
        }
        for (ssize_t done = 0; done < size;) {
            ssize_t count = write(output, buffer + done, size - done);
            if (count < 0 && errno == EINTR) {
                continue;
            }
            if (count <= 0) {
                result = -1;
                break;
            }
            done += count;
        }
        if (result) {
            break;
        }
    }
    if (result) {
        t_error("copy fixture: %s\n", strerror(errno));
    }
    if (close(output)) {
        t_error("close fixture: %s\n", strerror(errno));
        result = -1;
    }
    close(input);
    return result;
}

static void cleanup(void)
{
    if (fixture_handle && dlclose(fixture_handle)) {
        t_error("close fixture handle failed\n");
    }
    if (!root_created) {
        return;
    }
    if (*good_link) {
        unlink(good_link);
    }
    if (*bad_link) {
        unlink(bad_link);
    }
    if (*library) {
        unlink(library);
    }
    if (*cold_library) {
        unlink(cold_library);
    }
    if (*hidden_library) {
        unlink(hidden_library);
    }
    if (*blocked_library) {
        unlink(blocked_library);
    }
    if (*allowed) {
        rmdir(allowed);
    }
    if (*blocked) {
        rmdir(blocked);
    }
    if (rmdir(root)) {
        t_error("remove test directory %s: %s\n", root, strerror(errno));
    }
}

static int setup(const char *source)
{
    if (!mkdtemp(root)) {
        t_error("create test directory: %s\n", strerror(errno));
        return -1;
    }
    root_created = true;
    if (make_path(allowed, root, "allowed") || make_path(blocked, root, "blocked") ||
        make_path(library, allowed, FIXTURE) || make_path(cold_library, allowed, COLD_FIXTURE) ||
        make_path(hidden_library, blocked, HIDDEN_FIXTURE) ||
        make_path(blocked_library, blocked, FIXTURE) ||
        make_path(good_link, allowed, "good.so") || make_path(bad_link, allowed, "escape.so")) {
        return -1;
    }
    if (mkdir(allowed, 0700) || mkdir(blocked, 0700)) {
        t_error("mkdir fixture directories: %s\n", strerror(errno));
        return -1;
    }
    if (copy_library(source, library) || copy_library(source, blocked_library) ||
        copy_library(source, cold_library) || copy_library(source, hidden_library)) {
        return -1;
    }
    if (symlink(FIXTURE, good_link) || symlink(blocked_library, bad_link)) {
        t_error("create symlinks: %s\n", strerror(errno));
        return -1;
    }
    /* No inheritance: rejection must not be rescued by a permissive parent. */
    dlns_init(&ns, "relative_path_test");
    if (dlns_create2(&ns, allowed, 0) || dlns_set_namespace_separated(ns.name, true)) {
        t_error("create isolated namespace: %s\n", strerror(errno));
        return -1;
    }
    fixture_handle = dlopen_ns(&ns, library, RTLD_NOW | RTLD_LOCAL);
    if (!fixture_handle) {
        t_error("load fixture by absolute path: %s\n", dlerror());
        return -1;
    }
    fixture_open = dlsym(fixture_handle, "relative_path_open");
    if (!fixture_open) {
        t_error("resolve fixture entry point: %s\n", dlerror());
        return -1;
    }
    if (chdir(root)) {
        t_error("chdir fixture directory: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

static void *open_library(int api, const char *path, int mode)
{
    const dl_extinfo info = {0};
    switch (api) {
        case 0: {
            return dlopen_ns(&ns, path, mode);
        }
        case 1: {
            return dlopen_ns_ext(&ns, path, mode, &info);
        }
        case 2: {
            return fixture_open(path, mode, 0);
        }
        default: {
            return fixture_open(path, mode, 1);
        }
    }
}

/* Return a per-call result; worker threads never write the shared t_status. */
static int verify_open(int api, const char *path, int mode, bool expected)
{
    dlerror();
    void *handle = open_library(api, path, mode);
    if (!handle) {
        const char *error = dlerror();
        return expected || !error || dlerror() != NULL;
    }
    int failed = !expected;
    if (path) {
        int (*value)(void) = dlsym(handle, "relative_path_value");
        if (!value || value() != 42) {
            failed = 1;
        }
        int (*nested)(void) = dlsym(handle, "relative_path_nested_value");
        if (!nested || nested() != 42) {
            failed = 1;
        }
        /* A nested constructor must not overwrite the outer DSO's name.
         * The DSO keeps the pathname exactly as the caller passed it. */
        if (strstr(path, COLD_FIXTURE) && value) {
            Dl_info info;
            if (!dladdr((void *)value, &info) || !info.dli_fname || strcmp(info.dli_fname, path)) {
                failed = 1;
            }
        }
    }
    if (dlclose(handle)) {
        failed = 1;
    }
    return failed;
}

#ifndef RELATIVE_PATH_CONCURRENT
static void check_path(const char *path, int mode, bool expected)
{
    for (int api = 0; api < 4; ++api) {
        if (verify_open(api, path, mode, expected)) {
            t_error("api=%d path=%s mode=%d expected=%s\n", api, path ? path : "(null)", mode,
                    expected ? "success" : "failure");
        }
    }
}

static void check_matrix_switch_off(void)
{
    char resolved[PATH_MAX];
    if (!realpath("./blocked/" FIXTURE, resolved) || !realpath("./allowed/escape.so", resolved) ||
        !realpath(hidden_library, resolved)) {
        t_error("access-control fixtures must resolve successfully: %s\n", strerror(errno));
    }

    /* M1: absolute path inside the namespace range. */
    check_path(library, RTLD_NOW, true);
    /* M3: absolute path outside the range. */
    check_path(blocked_library, RTLD_NOW, false);
    /* M5: relative path inside the range stays rejected while the switch is off. */
    check_path("./allowed/" FIXTURE, RTLD_NOW, false);
    check_path("allowed/" FIXTURE, RTLD_NOW, false);
    check_path("./allowed/good.so", RTLD_NOW, false);
    /* M7: relative path outside the range. */
    check_path("./blocked/" FIXTURE, RTLD_NOW, false);
    check_path("./allowed/escape.so", RTLD_NOW, false);
    /* M9: bare name resolved through the namespace search path. */
    check_path(FIXTURE, RTLD_NOW, true);
    /* M11: bare name that only exists outside the search path. */
    check_path(HIDDEN_FIXTURE, RTLD_NOW, false);
}

static void check_matrix_switch_on(void)
{
    /* M2: absolute path inside the namespace range. */
    check_path(library, RTLD_NOW, true);
    /* M4: absolute path outside the range. */
    check_path(blocked_library, RTLD_NOW, false);
    /* M6: relative path inside the range; the cold fixture covers first load. */
    check_path("./allowed/" COLD_FIXTURE, RTLD_NOW, EXPECT_RELATIVE_PATH);
    check_path("./allowed/" FIXTURE, RTLD_NOW, EXPECT_RELATIVE_PATH);
    check_path("allowed/" FIXTURE, RTLD_NOW, EXPECT_RELATIVE_PATH);
    check_path("./allowed/good.so", RTLD_NOW, EXPECT_RELATIVE_PATH);
    /* M8: relative path outside the range, including a link escaping it. */
    check_path("./blocked/" FIXTURE, RTLD_NOW, false);
    check_path("./allowed/escape.so", RTLD_NOW, false);
    /* M10: bare name resolved through the namespace search path. */
    check_path(FIXTURE, RTLD_NOW, true);
    /* M12: bare name that only exists outside the search path. */
    check_path(HIDDEN_FIXTURE, RTLD_NOW, false);
}

static void matrix(void)
{
    check_matrix_switch_off();
    enable_relative_path();
    check_matrix_switch_on();
}

#else
struct worker {
    int index;
    int failed;
};

static void *run_worker(void *arg)
{
    struct worker *worker = arg;
    for (int i = 0; i < ITERATIONS; ++i) {
        /* Setters race with loads from other workers, including repeated enables. */
        if (worker->index < 2 || i >= ITERATIONS / 2) {
            enable_relative_path();
        }
        bool must_succeed = EXPECT_RELATIVE_PATH && (worker->index < 2 || i >= ITERATIONS / 2);
        dlerror();
        bool cold = (worker->index + i) % 2;
        const char *path = cold ? "./allowed/" COLD_FIXTURE : "./allowed/" FIXTURE;
        /* The warm fixture was preloaded by absolute path; the cold one is
         * first loaded by this relative path and keeps it as its name. */
        const char *expected_name = cold ? path : library;
        void *handle = open_library(worker->index % 4, path, RTLD_NOW);
        if (handle) {
            int (*value)(void) = dlsym(handle, "relative_path_value");
            if (!EXPECT_RELATIVE_PATH || !value || value() != 42) {
                worker->failed = 1;
            }
            int (*nested)(void) = dlsym(handle, "relative_path_nested_value");
            if (!nested || nested() != 42) {
                worker->failed = 1;
            }
            Dl_info info;
            if (value && (!dladdr((void *)value, &info) || !info.dli_fname || strcmp(info.dli_fname, expected_name))) {
                worker->failed = 1;
            }
            if (dlclose(handle)) {
                worker->failed = 1;
            }
        } else if (must_succeed || !dlerror()) {
            worker->failed = 1;
        }
        if (verify_open(worker->index % 4, "./blocked/" FIXTURE, RTLD_NOW, false)) {
            worker->failed = 1;
        }
    }
    return NULL;
}

static void concurrent(void)
{
    pthread_t threads[THREAD_COUNT];
    struct worker workers[THREAD_COUNT] = {0};
    int count = 0;
    for (; count < THREAD_COUNT; ++count) {
        workers[count].index = count;
        int error = pthread_create(&threads[count], NULL, run_worker, &workers[count]);
        if (error) {
            t_error("pthread_create: %s\n", strerror(error));
            break;
        }
    }
    for (int i = 0; i < count; ++i) {
        int error = pthread_join(threads[i], NULL);
        if (error) {
            t_error("pthread_join: %s\n", strerror(error));
            /* Do not release fixtures while an unjoined worker may use them. */
            _exit(1);
        }
        if (workers[i].failed) {
            t_error("concurrent worker %d failed\n", i);
        }
    }
}
#endif

int main(int argc, char **argv)
{
    /* Bound loader hangs in the test. */
    alarm(60);
    const char *source = argc > 1 ? argv[1] : DEFAULT_FIXTURE;
    char source_path[PATH_MAX];
    if (!realpath(source, source_path)) {
        t_error("fixture is missing: %s: %s\n", source, strerror(errno));
        return t_status;
    }
    enable_relative_path = dlsym(RTLD_DEFAULT, "dlopen_enable_relative_path");
    if (!enable_relative_path) {
        t_error("missing dlopen_enable_relative_path: %s\n", dlerror());
        return t_status;
    }
    if (!setup(source_path)) {
#ifdef RELATIVE_PATH_CONCURRENT
        concurrent();
#else
        matrix();
#endif
    }
    cleanup();
    return t_status;
}
