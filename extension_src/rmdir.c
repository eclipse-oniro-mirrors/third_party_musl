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
#include <unistd.h>
#include <sys/stat.h>
#include "functionalext.h"

/**
 * @tc.name      : rmdir_0100
 * @tc.desc      : Remove an empty directory, it no longer exists
 * @tc.level     : Level 0
 */
void rmdir_0100(void)
{
    const char *path = "/data/rmdir_test_0100";
    EXPECT_EQ("rmdir_0100", mkdir(path, 0777), 0);
    EXPECT_EQ("rmdir_0100", rmdir(path), 0);

    struct stat st;
    errno = 0;
    EXPECT_EQ("rmdir_0100", stat(path, &st), -1);
    EXPECT_EQ("rmdir_0100", errno, ENOENT);
}

/**
 * @tc.name      : rmdir_0200
 * @tc.desc      : Remove a directory that contains a file, should fail with ENOTEMPTY
 * @tc.level     : Level 2
 */
void rmdir_0200(void)
{
    const char *dir = "/data/rmdir_test_0200";
    const char *file = "/data/rmdir_test_0200/a.txt";

    EXPECT_EQ("rmdir_0200", mkdir(dir, 0777), 0);
    FILE *fp = fopen(file, "w");
    EXPECT_TRUE("rmdir_0200", fp != NULL);
    if (fp == NULL) {
        rmdir(dir);
        return;
    }
    fclose(fp);

    errno = 0;
    EXPECT_EQ("rmdir_0200", rmdir(dir), -1);
    EXPECT_EQ("rmdir_0200", errno, ENOTEMPTY);

    unlink(file);
    EXPECT_EQ("rmdir_0200", rmdir(dir), 0);
}

/**
 * @tc.name      : rmdir_0300
 * @tc.desc      : Remove a nonexistent directory, should fail with ENOENT
 * @tc.level     : Level 2
 */
void rmdir_0300(void)
{
    errno = 0;
    EXPECT_EQ("rmdir_0300", rmdir("/data/rmdir_test_0300_nonexistent"), -1);
    EXPECT_EQ("rmdir_0300", errno, ENOENT);
}

/**
 * @tc.name      : rmdir_0400
 * @tc.desc      : Remove a path that is a regular file, should fail with ENOTDIR
 * @tc.level     : Level 2
 */
void rmdir_0400(void)
{
    const char *file = "/data/rmdir_test_0400_file";

    FILE *fp = fopen(file, "w");
    EXPECT_TRUE("rmdir_0400", fp != NULL);
    if (fp == NULL) {
        return;
    }
    fclose(fp);

    errno = 0;
    EXPECT_EQ("rmdir_0400", rmdir(file), -1);
    EXPECT_EQ("rmdir_0400", errno, ENOTDIR);

    unlink(file);
}

/**
 * @tc.name      : rmdir_0500
 * @tc.desc      : Remove the current directory ".", should fail with EINVAL
 * @tc.level     : Level 2
 */
void rmdir_0500(void)
{
    errno = 0;
    EXPECT_EQ("rmdir_0500", rmdir("."), -1);
    EXPECT_EQ("rmdir_0500", errno, EINVAL);
}

/**
 * @tc.name      : rmdir_0600
 * @tc.desc      : Remove an empty path string, should fail with ENOENT
 * @tc.level     : Level 2
 */
void rmdir_0600(void)
{
    errno = 0;
    EXPECT_EQ("rmdir_0600", rmdir(""), -1);
    EXPECT_EQ("rmdir_0600", errno, ENOENT);
}

int main(void)
{
    rmdir_0100();
    rmdir_0200();
    rmdir_0300();
    rmdir_0400();
    rmdir_0500();
    rmdir_0600();

    return t_status;
}
