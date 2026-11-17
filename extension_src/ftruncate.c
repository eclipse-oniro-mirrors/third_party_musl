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

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "functionalext.h"

/**
 * @tc.name      : ftruncate_0100
 * @tc.desc      : Truncate an open file to a larger size
 * @tc.level     : Level 0
 */
void ftruncate_0100(void)
{
    const char *path = "/data/ftruncate_test_0100.txt";
    int fd = open(path, O_CREAT | O_RDWR, 0666);
    EXPECT_TRUE("ftruncate_0100", fd >= 0);
    if (fd < 0) {
        return;
    }
    EXPECT_EQ("ftruncate_0100", write(fd, "hello", 5), 5);

    EXPECT_EQ("ftruncate_0100", ftruncate(fd, 10), 0);

    struct stat st;
    EXPECT_EQ("ftruncate_0100", fstat(fd, &st), 0);
    EXPECT_EQ("ftruncate_0100", (int)st.st_size, 10);
    close(fd);
    unlink(path);
}

/**
 * @tc.name      : ftruncate_0200
 * @tc.desc      : Bytes appended by ftruncate read as zero
 * @tc.level     : Level 1
 */
void ftruncate_0200(void)
{
    const char *path = "/data/ftruncate_test_0200.txt";
    int fd = open(path, O_CREAT | O_RDWR, 0666);
    EXPECT_TRUE("ftruncate_0200", fd >= 0);
    if (fd < 0) {
        return;
    }
    EXPECT_EQ("ftruncate_0200", write(fd, "hello", 5), 5);

    EXPECT_EQ("ftruncate_0200", ftruncate(fd, 100), 0);
    EXPECT_EQ("ftruncate_0200", lseek(fd, 50, SEEK_SET), 50);

    char buf[4] = {1, 1, 1, 1};
    EXPECT_EQ("ftruncate_0200", read(fd, buf, 4), 4);
    EXPECT_EQ("ftruncate_0200", buf[0], 0);
    EXPECT_EQ("ftruncate_0200", buf[3], 0);
    close(fd);
    unlink(path);
}

/**
 * @tc.name      : ftruncate_0300
 * @tc.desc      : Shrink a file to zero length
 * @tc.level     : Level 1
 */
void ftruncate_0300(void)
{
    const char *path = "/data/ftruncate_test_0300.txt";
    int fd = open(path, O_CREAT | O_RDWR, 0666);
    EXPECT_TRUE("ftruncate_0300", fd >= 0);
    if (fd < 0) {
        return;
    }
    EXPECT_EQ("ftruncate_0300", write(fd, "hello world", 11), 11);

    EXPECT_EQ("ftruncate_0300", ftruncate(fd, 0), 0);

    struct stat st;
    EXPECT_EQ("ftruncate_0300", fstat(fd, &st), 0);
    EXPECT_EQ("ftruncate_0300", (int)st.st_size, 0);
    close(fd);
    unlink(path);
}

/**
 * @tc.name      : ftruncate_0400
 * @tc.desc      : Truncate a read-only descriptor, should fail with EINVAL
 * @tc.level     : Level 2
 */
void ftruncate_0400(void)
{
    const char *path = "/data/ftruncate_test_0400.txt";
    int fd = open(path, O_CREAT | O_RDONLY, 0666);
    EXPECT_TRUE("ftruncate_0400", fd >= 0);
    if (fd < 0) {
        return;
    }

    errno = 0;
    EXPECT_EQ("ftruncate_0400", ftruncate(fd, 10), -1);
    EXPECT_EQ("ftruncate_0400", errno, EINVAL);
    close(fd);
    unlink(path);
}

/**
 * @tc.name      : ftruncate_0500
 * @tc.desc      : Truncate with a negative length, should fail with EINVAL
 * @tc.level     : Level 2
 */
void ftruncate_0500(void)
{
    const char *path = "/data/ftruncate_test_0500.txt";
    int fd = open(path, O_CREAT | O_RDWR, 0666);
    EXPECT_TRUE("ftruncate_0500", fd >= 0);
    if (fd < 0) {
        return;
    }

    errno = 0;
    EXPECT_EQ("ftruncate_0500", ftruncate(fd, -1), -1);
    EXPECT_EQ("ftruncate_0500", errno, EINVAL);
    close(fd);
    unlink(path);
}

/**
 * @tc.name      : ftruncate_0600
 * @tc.desc      : Truncate with an invalid file descriptor, should fail with EBADF
 * @tc.level     : Level 2
 */
void ftruncate_0600(void)
{
    errno = 0;
    EXPECT_EQ("ftruncate_0600", ftruncate(-1, 10), -1);
    EXPECT_EQ("ftruncate_0600", errno, EBADF);
}

int main(void)
{
    ftruncate_0100();
    ftruncate_0200();
    ftruncate_0300();
    ftruncate_0400();
    ftruncate_0500();
    ftruncate_0600();

    return t_status;
}
