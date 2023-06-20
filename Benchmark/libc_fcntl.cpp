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

#include <benchmark/benchmark.h>
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "unistd.h"
#include "stdio.h"
#include "util.h"

using namespace std;

#ifdef ONO_CURRENT_INTERFACE
static void Bm_function_Fcntl_getfl(benchmark::State &state)
{
    int fd = open("/etc/passwd", O_RDONLY);
    if (fd == -1) {
        perror("open fcntl_getfl");
        exit(EXIT_FAILURE);
    }
    for (auto _ : state) {
        int ret = fcntl(fd, F_GETFL);
        if (ret < 0) {
            perror("fcntl_getfl proc");
            exit(EXIT_FAILURE);
        }
        benchmark::DoNotOptimize(ret);
    }
    close(fd);
    state.SetItemsProcessed(state.iterations());
}

static void Bm_function_Fcntl_setfl(benchmark::State &state)
{
    int flags = fcntl(STDIN_FILENO, F_GETFL);
    if (flags < 0) {
        perror("fcntl_setfl F_GETFL");
        exit(EXIT_FAILURE);
    }
    flags |= O_NONBLOCK;
    for (auto _ : state) {
        int ret = fcntl(STDIN_FILENO, F_SETFL, flags);
        if (ret < 0) {
            perror("fcntl_setfl proc");
            exit(EXIT_FAILURE);
        }
        benchmark::DoNotOptimize(ret);
    }
    state.SetItemsProcessed(state.iterations());
}

static void Bm_function_Fcntl_setlkw(benchmark::State &state)
{
    int fd = open("/dev/zero", O_RDWR);
    if (fd == -1) {
        perror("open fcntl_setlkw");
        exit(EXIT_FAILURE);
    }
    struct flock f_lock;
    for (auto _ : state) {
        state.PauseTiming();
        f_lock.l_type = F_WRLCK;
        f_lock.l_whence = SEEK_SET;
        f_lock.l_start = 0;
        f_lock.l_len = 0;
        if (fcntl(fd, F_SETLKW, &f_lock) < 0) {
            perror("fcntl_setlkw F_WRLCK");
            exit(EXIT_FAILURE);
        }
        f_lock.l_type = F_UNLCK;
        state.ResumeTiming();
        int ret = fcntl(fd, F_SETLKW, &f_lock);
        if (ret < 0) {
            perror("fcntl_setlkw proc");
            exit(EXIT_FAILURE);
        }
        benchmark::DoNotOptimize(ret);
    }
    close(fd);
    state.SetItemsProcessed(state.iterations());
}

static void Bm_function_Fcntl_dupfd(benchmark::State &state)
{
    int fd = open("/etc/passwd", O_RDONLY);
    if (fd == -1) {
        perror("open fcntl_dupfd");
        exit(EXIT_FAILURE);
    }
    for (auto _ : state) {
        int dupfd = fcntl(fd, F_DUPFD, 0);
        if (dupfd < 0) {
            perror("fcntl_dupfd proc");
            exit(EXIT_FAILURE);
        }
        benchmark::DoNotOptimize(dupfd);
        state.PauseTiming();
        close(dupfd);
        state.ResumeTiming();
    }
    close(fd);
    state.SetItemsProcessed(state.iterations());
}

static void Bm_function_Fcntl_setlk(benchmark::State &state)
{
    int fd = open("/dev/zero", O_RDWR);
    if (fd == -1) {
        perror("open fcntl_setlk");
        exit(EXIT_FAILURE);
    }
    struct flock f_lock;
    for (auto _ : state) {
        state.PauseTiming();
        f_lock.l_type = F_WRLCK;
        f_lock.l_whence = SEEK_SET;
        f_lock.l_start = 0;
        f_lock.l_len = 0;
        if (fcntl(fd, F_SETLK, &f_lock) < 0) {
            perror("fcntl_setlk F_WRLCK");
            exit(EXIT_FAILURE);
        }
        f_lock.l_type = F_UNLCK;
        state.ResumeTiming();
        int ret = fcntl(fd, F_SETLK, &f_lock);
        if (ret < 0) {
            perror("fcntl_setlk proc");
            exit(EXIT_FAILURE);
        }
        benchmark::DoNotOptimize(ret);
    }
    close(fd);
    state.SetBytesProcessed(state.iterations());
}

static void Bm_function_Fcntl_getlk(benchmark::State &state)
{
    int fd = open("/dev/zero", O_RDWR);
    if (fd == -1) {
        perror("open fcntl_getlk");
        exit(EXIT_FAILURE);
    }
    struct flock f_lock;
    for (auto _ : state) {
        state.PauseTiming();
        f_lock.l_type = F_RDLCK;
        f_lock.l_whence = SEEK_SET;
        f_lock.l_start = 0;
        f_lock.l_len = 0;
        state.ResumeTiming();
        int ret = fcntl(fd, F_GETLK, &f_lock);
        if (ret < 0) {
            perror("fcntl_getlk proc");
            exit(EXIT_FAILURE);
        }
        benchmark::DoNotOptimize(ret);
    }
    close(fd);
    state.SetBytesProcessed(state.iterations());
}

static void Bm_function_Fcntl_getfd(benchmark::State &state)
{
    int fd = open("/dev/zero", O_RDWR);
    if (fd == -1) {
        perror("open fcntl_getfd");
        exit(EXIT_FAILURE);
    }
    for (auto _ : state) {
        int ret = fcntl(fd, F_GETFD);
        if (ret < 0) {
            perror("fcntl_getfd proc");
            exit(EXIT_FAILURE);
        }
        benchmark::DoNotOptimize(ret);
    }
    close(fd);
    state.SetBytesProcessed(state.iterations());
}

static void Bm_function_Fcntl_setfd(benchmark::State &state)
{
    int fd = open("/dev/zero", O_RDWR);
    if (fd == -1) {
        perror("open fcntl_setfd");
        exit(EXIT_FAILURE);
    }
    for (auto _ : state) {
        state.PauseTiming();
        int flags = fcntl(fd, F_GETFD);
        if (flags < 0) {
            perror("fcntl_setfd F_GETFD");
            exit(EXIT_FAILURE);
        }
        flags |= FD_CLOEXEC;
        state.ResumeTiming();
        int ret = fcntl(fd, F_SETFD, flags);
        if (ret < 0) {
            perror("fcntl_setfd proc");
            exit(EXIT_FAILURE);
        }
        benchmark::DoNotOptimize(ret);
    }
    close(fd);
    state.SetBytesProcessed(state.iterations());
}
#endif

static void Bm_function_Open_rdonly(benchmark::State &state)
{
    const char *filename = "/proc/self/cmdline";
    for (auto _ : state) {
        int fd = open(filename, O_RDONLY);
        if (fd == -1) {
            perror("open_rdonly proc");
            exit(EXIT_FAILURE);
        }
        benchmark::DoNotOptimize(fd);
        state.PauseTiming();
        close(fd);
        state.ResumeTiming();
    }
    state.SetItemsProcessed(state.iterations());
}

static void Bm_function_Open_rdwr(benchmark::State &state)
{
    const char *filename = "/dev/zero";
    for (auto _ : state) {
        int fd = open(filename, O_RDWR);
        if (fd == -1) {
            perror("open_rdwr proc");
            exit(EXIT_FAILURE);
        }
        benchmark::DoNotOptimize(fd);
        state.PauseTiming();
        close(fd);
        state.ResumeTiming();
    }
    state.SetItemsProcessed(state.iterations());
}

static void Bm_function_Open_creat_rdwr(benchmark::State &state)
{
    const char *filename = "/data/log/hiview/sys_event_logger/event.db";
    for (auto _ : state) {
        int fd = open(filename, O_RDWR | O_CREAT);
        if (fd == -1) {
            perror("open_creat_rdwr proc");
            exit(EXIT_FAILURE);
        }
        benchmark::DoNotOptimize(fd);
        state.PauseTiming();
        close(fd);
        state.ResumeTiming();
    }
    state.SetItemsProcessed(state.iterations());
}

#ifdef ONO_CURRENT_INTERFACE
MUSL_BENCHMARK(Bm_function_Fcntl_getfl);
MUSL_BENCHMARK(Bm_function_Fcntl_setfl);
MUSL_BENCHMARK(Bm_function_Fcntl_setlkw);
MUSL_BENCHMARK(Bm_function_Fcntl_dupfd);
MUSL_BENCHMARK(Bm_function_Fcntl_setlk);
MUSL_BENCHMARK(Bm_function_Fcntl_getlk);
MUSL_BENCHMARK(Bm_function_Fcntl_getfd);
MUSL_BENCHMARK(Bm_function_Fcntl_setfd);
#endif

MUSL_BENCHMARK(Bm_function_Open_rdonly);
MUSL_BENCHMARK(Bm_function_Open_rdwr);
MUSL_BENCHMARK(Bm_function_Open_creat_rdwr);