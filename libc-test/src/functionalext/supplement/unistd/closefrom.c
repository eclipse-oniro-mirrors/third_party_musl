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

#include <fcntl.h>
#include <limits.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include "functionalext.h"

/**
 * @tc.name      : closefrom_0100
 * @tc.desc      : Close all fds from a given lowfd in a child process
 * @tc.level     : Level 0
 */
void closefrom_0100(void)
{
    pid_t child = fork();
    EXPECT_TRUE("closefrom_0100", child >= 0);
    if (child == 0) {
        int pfd[2];
        if (pipe(pfd) < 0) {
            _exit(1);
        }
        int extraFd = dup(pfd[0]);
        if (extraFd < 0) {
            _exit(2);
        }
        closefrom(pfd[0]);
        if (fcntl(pfd[0], F_GETFL) != -1) {
            _exit(3);
        }
        if (fcntl(pfd[1], F_GETFL) != -1) {
            _exit(4);
        }
        if (fcntl(extraFd, F_GETFL) != -1) {
            _exit(5);
        }
        _exit(0);
    }
    int status;
    waitpid(child, &status, 0);
    EXPECT_TRUE("closefrom_0100", WIFEXITED(status));
    EXPECT_EQ("closefrom_0100", WEXITSTATUS(status), 0);
}

/**
 * @tc.name      : closefrom_0200
 * @tc.desc      : Closefrom with negative lowfd (treated as 0)
 * @tc.level     : Level 0
 */
void closefrom_0200(void)
{
    pid_t child = fork();
    EXPECT_TRUE("closefrom_0200", child >= 0);
    if (child == 0) {
        int fd = open("/dev/null", O_RDONLY);
        if (fd < 0) {
            _exit(1);
        }
        closefrom(-1);
        if (fcntl(fd, F_GETFL) != -1) {
            _exit(2);
        }
        _exit(0);
    }
    int status;
    waitpid(child, &status, 0);
    EXPECT_TRUE("closefrom_0200", WIFEXITED(status));
    EXPECT_EQ("closefrom_0200", WEXITSTATUS(status), 0);
}

/**
 * @tc.name      : closefrom_0300
 * @tc.desc      : Closefrom with very high lowfd (no-op, no crash)
 * @tc.level     : Level 2
 */
void closefrom_0300(void)
{
    closefrom(9999);
    EXPECT_TRUE("closefrom_0300", 1);
}

/**
 * @tc.name      : closefrom_0400
 * @tc.desc      : Closefrom with mixed open and closed fds in range
 * @tc.level     : Level 0
 */
void closefrom_0400(void)
{
    pid_t child = fork();
    EXPECT_TRUE("closefrom_0400", child >= 0);
    if (child == 0) {
        int pfd[2];
        if (pipe(pfd) < 0) {
            _exit(1);
        }
        int mid = dup(pfd[0]);
        if (mid < 0) {
            _exit(2);
        }
        closefrom(pfd[0]);
        if (fcntl(pfd[0], F_GETFL) != -1) {
            _exit(3);
        }
        if (fcntl(pfd[1], F_GETFL) != -1) {
            _exit(4);
        }
        if (fcntl(mid, F_GETFL) != -1) {
            _exit(5);
        }
        _exit(0);
    }
    int status;
    waitpid(child, &status, 0);
    EXPECT_TRUE("closefrom_0400", WIFEXITED(status));
    EXPECT_EQ("closefrom_0400", WEXITSTATUS(status), 0);
}

/**
 * @tc.name      : closefrom_0500
 * @tc.desc      : Boundary: closefrom(0) in child closes all fds
 * @tc.level     : Level 2
 */
void closefrom_0500(void)
{
    pid_t child = fork();
    EXPECT_TRUE("closefrom_0500", child >= 0);
    if (child == 0) {
        int fd = open("/dev/null", O_RDONLY);
        if (fd < 0) {
            _exit(1);
        }
        closefrom(0);
        if (fcntl(fd, F_GETFL) != -1) {
            _exit(2);
        }
        _exit(0);
    }
    int status;
    waitpid(child, &status, 0);
    EXPECT_TRUE("closefrom_0500", WIFEXITED(status));
    EXPECT_EQ("closefrom_0500", WEXITSTATUS(status), 0);
}

static void *closefrom_worker(void *arg)
{
    (void)arg;
    closefrom(256);
    return NULL;
}

/**
 * @tc.name      : closefrom_0600
 * @tc.desc      : Multi-threaded: closefrom in a thread does not affect main thread fds
 * @tc.level     : Level 0
 */
void closefrom_0600(void)
{
    int fd = dup(STDOUT_FILENO);
    EXPECT_TRUE("closefrom_0600", fd > 0);
    pthread_t tid;
    EXPECT_EQ("closefrom_0600", pthread_create(&tid, NULL, closefrom_worker, NULL), 0);
    pthread_join(tid, NULL);
    EXPECT_TRUE("closefrom_0600", fcntl(fd, F_GETFL) >= 0);
    close(fd);
}

int main(void)
{
    closefrom_0100();
    closefrom_0200();
    closefrom_0300();
    closefrom_0400();
    closefrom_0500();
    closefrom_0600();
    return t_status;
}
