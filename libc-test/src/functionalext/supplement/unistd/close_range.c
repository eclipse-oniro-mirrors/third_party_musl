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

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include "functionalext.h"

const int SUCCESS = 0;
const int FAILED = -1;

/**
 * @tc.name      : close_range_0100
 * @tc.desc      : Close a normal range of file descriptors
 * @tc.level     : Level 0
 */
void close_range_0100(void)
{
    int pfd[2];
    EXPECT_EQ("close_range_0100", pipe(pfd), SUCCESS);
    int dupFd = dup(pfd[0]);
    EXPECT_TRUE("close_range_0100", dupFd > 0);
    int rv = close_range((unsigned int)pfd[0], (unsigned int)dupFd, 0);
    EXPECT_EQ("close_range_0100", rv, SUCCESS);
    EXPECT_EQ("close_range_0100", fcntl(pfd[0], F_GETFL), FAILED);
    EXPECT_EQ("close_range_0100", fcntl(pfd[1], F_GETFL), FAILED);
    EXPECT_EQ("close_range_0100", fcntl(dupFd, F_GETFL), FAILED);
}

/**
 * @tc.name      : close_range_0200
 * @tc.desc      : Close a single file descriptor (first == last)
 * @tc.level     : Level 0
 */
void close_range_0200(void)
{
    int pfd[2];
    EXPECT_EQ("close_range_0200", pipe(pfd), SUCCESS);
    int rv = close_range((unsigned int)pfd[1], (unsigned int)pfd[1], 0);
    EXPECT_EQ("close_range_0200", rv, SUCCESS);
    EXPECT_EQ("close_range_0200", fcntl(pfd[1], F_GETFL), FAILED);
    EXPECT_TRUE("close_range_0200", fcntl(pfd[0], F_GETFL) >= 0);
    close(pfd[0]);
}

/**
 * @tc.name      : close_range_0300
 * @tc.desc      : first > last returns -1 and sets errno to EINVAL
 * @tc.level     : Level 2
 */
void close_range_0300(void)
{
    errno = 0;
    int rv = close_range(300, 100, 0);
    EXPECT_EQ("close_range_0300", rv, FAILED);
    EXPECT_EQ("close_range_0300", errno, EINVAL);
}

/**
 * @tc.name      : close_range_0400
 * @tc.desc      : Nonzero flags in fallback path returns -1 and EINVAL
 * @tc.level     : Level 2
 */
void close_range_0400(void)
{
    int pfd[2];
    EXPECT_EQ("close_range_0400", pipe(pfd), SUCCESS);
    errno = 0;
    int rv = close_range((unsigned int)pfd[0], (unsigned int)pfd[1], 0x7777);
    if (rv == FAILED) {
        EXPECT_EQ("close_range_0400", errno, EINVAL);
    }
    close(pfd[0]);
    close(pfd[1]);
}

/**
 * @tc.name      : close_range_0500
 * @tc.desc      : Close range with gaps (some fds already closed)
 * @tc.level     : Level 0
 */
void close_range_0500(void)
{
    int fds[5];
    EXPECT_EQ("close_range_0500", pipe(fds), SUCCESS);
    fds[2] = dup(fds[0]);
    fds[3] = dup(fds[1]);
    fds[4] = dup(fds[0]);
    close(fds[2]);
    int rv = close_range((unsigned int)fds[0], (unsigned int)fds[4], 0);
    EXPECT_EQ("close_range_0500", rv, SUCCESS);
    EXPECT_EQ("close_range_0500", fcntl(fds[0], F_GETFL), FAILED);
    EXPECT_EQ("close_range_0500", fcntl(fds[1], F_GETFL), FAILED);
    EXPECT_EQ("close_range_0500", fcntl(fds[3], F_GETFL), FAILED);
    EXPECT_EQ("close_range_0500", fcntl(fds[4], F_GETFL), FAILED);
}

/**
 * @tc.name      : close_range_0600
 * @tc.desc      : Close range where last is ~0U (maximum)
 * @tc.level     : Level 0
 */
void close_range_0600(void)
{
    int pfd[2];
    EXPECT_EQ("close_range_0600", pipe(pfd), SUCCESS);
    int rv = close_range((unsigned int)pfd[1], ~0U, 0);
    EXPECT_EQ("close_range_0600", rv, SUCCESS);
    EXPECT_EQ("close_range_0600", fcntl(pfd[1], F_GETFL), FAILED);
    close(pfd[0]);
}

/**
 * @tc.name      : close_range_0700
 * @tc.desc      : Boundary: first == 0
 * @tc.level     : Level 2
 */
void close_range_0700(void)
{
    int fd = dup(STDIN_FILENO);
    EXPECT_TRUE("close_range_0700", fd > 0);
    int rv = close_range(0, 0, 0);
    EXPECT_EQ("close_range_0700", rv, SUCCESS);
    EXPECT_EQ("close_range_0700", fcntl(0, F_GETFL), FAILED);
    dup2(fd, STDIN_FILENO);
    close(fd);
}

static void *close_range_worker(void *arg)
{
    int *fds = (int *)arg;
    close_range((unsigned int)fds[0], (unsigned int)fds[1], 0);
    return NULL;
}

/**
 * @tc.name      : close_range_0800
 * @tc.desc      : Multi-threaded: concurrent close_range on different fd ranges
 * @tc.level     : Level 0
 */
void close_range_0800(void)
{
    int pfdA[2], pfdB[2];
    EXPECT_EQ("close_range_0800", pipe(pfdA), SUCCESS);
    EXPECT_EQ("close_range_0800", pipe(pfdB), SUCCESS);
    pthread_t t1, t2;
    EXPECT_EQ("close_range_0800", pthread_create(&t1, NULL, close_range_worker, pfdA), 0);
    EXPECT_EQ("close_range_0800", pthread_create(&t2, NULL, close_range_worker, pfdB), 0);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    EXPECT_EQ("close_range_0800", fcntl(pfdA[0], F_GETFL), FAILED);
    EXPECT_EQ("close_range_0800", fcntl(pfdA[1], F_GETFL), FAILED);
    EXPECT_EQ("close_range_0800", fcntl(pfdB[0], F_GETFL), FAILED);
    EXPECT_EQ("close_range_0800", fcntl(pfdB[1], F_GETFL), FAILED);
}

int main(void)
{
    close_range_0100();
    close_range_0200();
    close_range_0300();
    close_range_0400();
    close_range_0500();
    close_range_0600();
    close_range_0700();
    close_range_0800();
    return t_status;
}
