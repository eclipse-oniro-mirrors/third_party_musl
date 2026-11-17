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
 * @tc.name      : mkdir_0100
 * @tc.desc      : Create a directory, it exists and is a directory
 * @tc.level     : Level 0
 */
void mkdir_0100(void)
{
    const char *path = "/data/mkdir_test_0100";
    EXPECT_EQ("mkdir_0100", mkdir(path, 0777), 0);

    struct stat st;
    EXPECT_EQ("mkdir_0100", stat(path, &st), 0);
    EXPECT_TRUE("mkdir_0100", S_ISDIR(st.st_mode));

    EXPECT_EQ("mkdir_0100", rmdir(path), 0);
}

/**
 * @tc.name      : mkdir_0200
 * @tc.desc      : Create a directory with a trailing slash
 * @tc.level     : Level 1
 */
void mkdir_0200(void)
{
    const char *path = "/data/mkdir_test_0200/";
    EXPECT_EQ("mkdir_0200", mkdir(path, 0777), 0);

    struct stat st;
    EXPECT_EQ("mkdir_0200", stat("/data/mkdir_test_0200", &st), 0);
    EXPECT_TRUE("mkdir_0200", S_ISDIR(st.st_mode));

    EXPECT_EQ("mkdir_0200", rmdir("/data/mkdir_test_0200"), 0);
}

/**
 * @tc.name      : mkdir_0300
 * @tc.desc      : Create a nested directory under an existing parent
 * @tc.level     : Level 1
 */
void mkdir_0300(void)
{
    EXPECT_EQ("mkdir_0300", mkdir("/data/mkdir_test_0300", 0777), 0);
    EXPECT_EQ("mkdir_0300", mkdir("/data/mkdir_test_0300/child", 0777), 0);

    struct stat st;
    EXPECT_EQ("mkdir_0300", stat("/data/mkdir_test_0300/child", &st), 0);
    EXPECT_TRUE("mkdir_0300", S_ISDIR(st.st_mode));

    EXPECT_EQ("mkdir_0300", rmdir("/data/mkdir_test_0300/child"), 0);
    EXPECT_EQ("mkdir_0300", rmdir("/data/mkdir_test_0300"), 0);
}

/**
 * @tc.name      : mkdir_0400
 * @tc.desc      : Create a directory that already exists, should fail with EEXIST
 * @tc.level     : Level 2
 */
void mkdir_0400(void)
{
    const char *path = "/data/mkdir_test_0400";
    EXPECT_EQ("mkdir_0400", mkdir(path, 0777), 0);

    errno = 0;
    EXPECT_EQ("mkdir_0400", mkdir(path, 0777), -1);
    EXPECT_EQ("mkdir_0400", errno, EEXIST);

    EXPECT_EQ("mkdir_0400", rmdir(path), 0);
}

/**
 * @tc.name      : mkdir_0500
 * @tc.desc      : Create a directory under a nonexistent parent, should fail with ENOENT
 * @tc.level     : Level 2
 */
void mkdir_0500(void)
{
    errno = 0;
    int ret = mkdir("/data/mkdir_test_0500_no_such_parent/child", 0777);
    EXPECT_EQ("mkdir_0500", ret, -1);
    EXPECT_EQ("mkdir_0500", errno, ENOENT);
}

/**
 * @tc.name      : mkdir_0600
 * @tc.desc      : Create a directory under a path component that is a file, should fail with ENOTDIR
 * @tc.level     : Level 2
 */
void mkdir_0600(void)
{
    const char *file = "/data/mkdir_test_0600_file";
    const char *dir = "/data/mkdir_test_0600_file/subdir";

    FILE *fp = fopen(file, "w");
    EXPECT_TRUE("mkdir_0600", fp != NULL);
    if (fp == NULL) {
        return;
    }
    fclose(fp);

    errno = 0;
    EXPECT_EQ("mkdir_0600", mkdir(dir, 0777), -1);
    EXPECT_EQ("mkdir_0600", errno, ENOTDIR);

    unlink(file);
}

/**
 * @tc.name      : mkdir_0700
 * @tc.desc      : Directory name exceeds NAME_MAX, should fail with ENAMETOOLONG
 * @tc.level     : Level 2
 */
void mkdir_0700(void)
{
    char name[300];
    memset(name, 'b', sizeof(name) - 1);
    name[sizeof(name) - 1] = '\0';

    char path[320];
    snprintf(path, sizeof(path), "/data/%s", name);

    errno = 0;
    EXPECT_EQ("mkdir_0700", mkdir(path, 0777), -1);
    EXPECT_EQ("mkdir_0700", errno, ENAMETOOLONG);
}

int main(void)
{
    mkdir_0100();
    mkdir_0200();
    mkdir_0300();
    mkdir_0400();
    mkdir_0500();
    mkdir_0600();
    mkdir_0700();

    return t_status;
}
