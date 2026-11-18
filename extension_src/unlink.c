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
 * @tc.name      : unlink_0100
 * @tc.desc      : Remove a regular file, it no longer exists
 * @tc.level     : Level 0
 */
void unlink_0100(void)
{
    const char *path = "/data/unlink_test_0100.txt";
    FILE *fp = fopen(path, "w");
    EXPECT_TRUE("unlink_0100", fp != NULL);
    if (fp == NULL) {
        return;
    }
    fclose(fp);

    EXPECT_EQ("unlink_0100", unlink(path), 0);

    errno = 0;
    EXPECT_EQ("unlink_0100", access(path, F_OK), -1);
    EXPECT_EQ("unlink_0100", errno, ENOENT);
}

/**
 * @tc.name      : unlink_0200
 * @tc.desc      : Unlink an open file, name is gone but data still readable
 * @tc.level     : Level 1
 */
void unlink_0200(void)
{
    const char *path = "/data/unlink_test_0200.txt";
    int fd = open(path, O_CREAT | O_RDWR, 0666);
    EXPECT_TRUE("unlink_0200", fd >= 0);
    if (fd < 0) {
        return;
    }
    EXPECT_EQ("unlink_0200", write(fd, "hello", 5), 5);

    EXPECT_EQ("unlink_0200", unlink(path), 0);

    /* the descriptor stays valid until close */
    EXPECT_EQ("unlink_0200", lseek(fd, 0, SEEK_SET), 0);
    char buf[5] = {0};
    EXPECT_EQ("unlink_0200", read(fd, buf, 5), 5);
    EXPECT_EQ("unlink_0200", memcmp(buf, "hello", 5), 0);
    close(fd);
}

/**
 * @tc.name      : unlink_0300
 * @tc.desc      : Remove a nonexistent file, should fail with ENOENT
 * @tc.level     : Level 2
 */
void unlink_0300(void)
{
    errno = 0;
    EXPECT_EQ("unlink_0300", unlink("/data/unlink_test_0300_nonexistent.txt"), -1);
    EXPECT_EQ("unlink_0300", errno, ENOENT);
}

/**
 * @tc.name      : unlink_0400
 * @tc.desc      : Remove a directory with unlink, should fail with EISDIR
 * @tc.level     : Level 2
 */
void unlink_0400(void)
{
    const char *dir = "/data/unlink_test_0400_dir";
    EXPECT_EQ("unlink_0400", mkdir(dir, 0777), 0);

    errno = 0;
    EXPECT_EQ("unlink_0400", unlink(dir), -1);
    EXPECT_EQ("unlink_0400", errno, EISDIR);

    rmdir(dir);
}

/**
 * @tc.name      : unlink_0500
 * @tc.desc      : Remove with an empty path, should fail with ENOENT
 * @tc.level     : Level 2
 */
void unlink_0500(void)
{
    errno = 0;
    EXPECT_EQ("unlink_0500", unlink(""), -1);
    EXPECT_EQ("unlink_0500", errno, ENOENT);
}

/**
 * @tc.name      : unlink_0600
 * @tc.desc      : Remove a file in a nonexistent directory, should fail with ENOENT
 * @tc.level     : Level 2
 */
void unlink_0600(void)
{
    errno = 0;
    int ret = unlink("/data/unlink_test_0600_no_such_dir/a.txt");
    EXPECT_EQ("unlink_0600", ret, -1);
    EXPECT_EQ("unlink_0600", errno, ENOENT);
}

int main(void)
{
    unlink_0100();
    unlink_0200();
    unlink_0300();
    unlink_0400();
    unlink_0500();
    unlink_0600();

    return t_status;
}
