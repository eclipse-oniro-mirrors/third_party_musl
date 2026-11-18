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
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "functionalext.h"

/**
 * @tc.name      : open_0100
 * @tc.desc      : Create a new file with O_CREAT and open it read/write
 * @tc.level     : Level 0
 */
void open_0100(void)
{
    const char *path = "/data/open_test_0100.txt";
    int fd = open(path, O_CREAT | O_RDWR, 0666);
    EXPECT_TRUE("open_0100", fd >= 0);
    if (fd < 0) {
        return;
    }
    EXPECT_EQ("open_0100", write(fd, "hello", 5), 5);
    EXPECT_EQ("open_0100", close(fd), 0);
    EXPECT_EQ("open_0100", unlink(path), 0);
}

/**
 * @tc.name      : open_0200
 * @tc.desc      : Open an existing file with O_CREAT but without O_EXCL
 * @tc.level     : Level 0
 */
void open_0200(void)
{
    const char *path = "/data/open_test_0200.txt";
    int fd = open(path, O_CREAT | O_RDWR, 0666);
    EXPECT_TRUE("open_0200", fd >= 0);
    if (fd < 0) {
        return;
    }
    close(fd);

    /* opening again with O_CREAT alone must succeed */
    int fd2 = open(path, O_CREAT | O_RDWR, 0666);
    EXPECT_TRUE("open_0200", fd2 >= 0);
    if (fd2 >= 0) {
        close(fd2);
    }
    unlink(path);
}

/**
 * @tc.name      : open_0300
 * @tc.desc      : O_CREAT|O_EXCL on an existing file, should fail with EEXIST
 * @tc.level     : Level 1
 */
void open_0300(void)
{
    const char *path = "/data/open_test_0300.txt";
    int fd = open(path, O_CREAT | O_RDWR, 0666);
    EXPECT_TRUE("open_0300", fd >= 0);
    if (fd < 0) {
        return;
    }
    close(fd);

    errno = 0;
    int fd2 = open(path, O_CREAT | O_EXCL | O_RDWR, 0666);
    EXPECT_EQ("open_0300", fd2, -1);
    EXPECT_EQ("open_0300", errno, EEXIST);
    unlink(path);
}

/**
 * @tc.name      : open_0400
 * @tc.desc      : Write to an O_RDONLY descriptor, should fail with EBADF
 * @tc.level     : Level 1
 */
void open_0400(void)
{
    const char *path = "/data/open_test_0400.txt";
    int fd = open(path, O_CREAT | O_RDONLY, 0666);
    EXPECT_TRUE("open_0400", fd >= 0);
    if (fd < 0) {
        return;
    }
    errno = 0;
    EXPECT_EQ("open_0400", write(fd, "x", 1), -1);
    EXPECT_EQ("open_0400", errno, EBADF);
    close(fd);
    unlink(path);
}

/**
 * @tc.name      : open_0500
 * @tc.desc      : O_TRUNC truncates an existing file to zero length
 * @tc.level     : Level 1
 */
void open_0500(void)
{
    const char *path = "/data/open_test_0500.txt";
    int fd = open(path, O_CREAT | O_RDWR, 0666);
    EXPECT_TRUE("open_0500", fd >= 0);
    if (fd < 0) {
        return;
    }
    EXPECT_EQ("open_0500", write(fd, "hello world", 11), 11);
    close(fd);

    int fd2 = open(path, O_RDWR | O_TRUNC);
    EXPECT_TRUE("open_0500", fd2 >= 0);
    if (fd2 < 0) {
        unlink(path);
        return;
    }
    struct stat st;
    EXPECT_EQ("open_0500", fstat(fd2, &st), 0);
    EXPECT_EQ("open_0500", (int)st.st_size, 0);
    close(fd2);
    unlink(path);
}

/**
 * @tc.name      : open_0600
 * @tc.desc      : Open a nonexistent file without O_CREAT, should fail with ENOENT
 * @tc.level     : Level 2
 */
void open_0600(void)
{
    errno = 0;
    int fd = open("/data/open_test_0600_nonexistent.txt", O_RDWR);
    EXPECT_EQ("open_0600", fd, -1);
    EXPECT_EQ("open_0600", errno, ENOENT);
}

/**
 * @tc.name      : open_0700
 * @tc.desc      : O_DIRECTORY on a regular file, should fail with ENOTDIR
 * @tc.level     : Level 2
 */
void open_0700(void)
{
    const char *path = "/data/open_test_0700.txt";
    int fd = open(path, O_CREAT | O_RDWR, 0666);
    EXPECT_TRUE("open_0700", fd >= 0);
    if (fd < 0) {
        return;
    }
    close(fd);

    errno = 0;
    int fd2 = open(path, O_RDONLY | O_DIRECTORY);
    EXPECT_EQ("open_0700", fd2, -1);
    EXPECT_EQ("open_0700", errno, ENOTDIR);
    unlink(path);
}

/**
 * @tc.name      : open_0800
 * @tc.desc      : Pathname component exceeds NAME_MAX, should fail with ENAMETOOLONG
 * @tc.level     : Level 2
 */
void open_0800(void)
{
    char name[300];
    memset(name, 'a', sizeof(name) - 1);
    name[sizeof(name) - 1] = '\0';

    errno = 0;
    int fd = open(name, O_CREAT | O_RDWR, 0666);
    EXPECT_EQ("open_0800", fd, -1);
    EXPECT_EQ("open_0800", errno, ENAMETOOLONG);
}

int main(void)
{
    open_0100();
    open_0200();
    open_0300();
    open_0400();
    open_0500();
    open_0600();
    open_0700();
    open_0800();

    return t_status;
}
