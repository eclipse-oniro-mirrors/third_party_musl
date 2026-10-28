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

#ifndef ISOC23_SCANF_TEST_H
#define ISOC23_SCANF_TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "test.h"

static inline void close_scan_fd(int fd)
{
    if (close(fd) != 0) {
        t_error("close failed\n");
    }
}

static inline void close_scan_file(FILE *fp)
{
    if (fclose(fp) != 0) {
        t_error("fclose failed\n");
    }
}

static int open_scan_fd(void)
{
#ifdef __OHOS__
    char path[] = "/data/local/tmp/isoc23-scanf-XXXXXX";
#else
    char path[] = "/tmp/isoc23-scanf-XXXXXX";
#endif
    int fd = mkstemp(path);
    if (fd < 0) {
        t_error("mkstemp failed\n");
        return -1;
    }
    if (unlink(path) != 0) {
        t_error("unlink failed\n");
        close_scan_fd(fd);
        return -1;
    }
    return fd;
}

static inline FILE *scan_fdopen(int fd)
{
    FILE *fp = fdopen(fd, "w+");
    if (!fp) {
        t_error("fdopen failed\n");
        close_scan_fd(fd);
    }
    return fp;
}

/* Keep a seekable, initially unoriented stream without a persistent pathname. */
static inline FILE *open_scan_file(void)
{
    int fd = open_scan_fd();
    return fd < 0 ? NULL : scan_fdopen(fd);
}

#endif
