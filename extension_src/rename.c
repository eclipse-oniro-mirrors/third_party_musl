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
 * @tc.name      : rename_0100
 * @tc.desc      : Rename a file, old name is gone and new name has the content
 * @tc.level     : Level 0
 */
void rename_0100(void)
{
    const char *oldpath = "/data/rename_test_0100_old.txt";
    const char *newpath = "/data/rename_test_0100_new.txt";
    FILE *fp = fopen(oldpath, "w");
    EXPECT_TRUE("rename_0100", fp != NULL);
    if (fp == NULL) {
        return;
    }
    fputs("rename_data", fp);
    fclose(fp);

    EXPECT_EQ("rename_0100", rename(oldpath, newpath), 0);

    struct stat st;
    errno = 0;
    EXPECT_EQ("rename_0100", stat(oldpath, &st), -1);
    EXPECT_EQ("rename_0100", errno, ENOENT);
    EXPECT_EQ("rename_0100", access(newpath, F_OK), 0);

    unlink(newpath);
}

/**
 * @tc.name      : rename_0200
 * @tc.desc      : Rename a directory
 * @tc.level     : Level 0
 */
void rename_0200(void)
{
    const char *oldpath = "/data/rename_test_0200_old";
    const char *newpath = "/data/rename_test_0200_new";

    EXPECT_EQ("rename_0200", mkdir(oldpath, 0777), 0);
    EXPECT_EQ("rename_0200", rename(oldpath, newpath), 0);

    struct stat st;
    EXPECT_EQ("rename_0200", stat(newpath, &st), 0);
    EXPECT_TRUE("rename_0200", S_ISDIR(st.st_mode));

    EXPECT_EQ("rename_0200", rmdir(newpath), 0);
}

/**
 * @tc.name      : rename_0300
 * @tc.desc      : Rename onto an existing file, newpath is atomically replaced
 * @tc.level     : Level 1
 */
void rename_0300(void)
{
    const char *oldpath = "/data/rename_test_0300_old.txt";
    const char *newpath = "/data/rename_test_0300_new.txt";

    FILE *fp = fopen(oldpath, "w");
    EXPECT_TRUE("rename_0300", fp != NULL);
    if (fp == NULL) {
        return;
    }
    fputs("old_content", fp);
    fclose(fp);

    fp = fopen(newpath, "w");
    if (fp == NULL) {
        unlink(oldpath);
        return;
    }
    fputs("new_content_to_be_replaced", fp);
    fclose(fp);

    EXPECT_EQ("rename_0300", rename(oldpath, newpath), 0);

    fp = fopen(newpath, "r");
    EXPECT_TRUE("rename_0300", fp != NULL);
    if (fp == NULL) {
        return;
    }
    char buf[32] = {0};
    fread(buf, 1, sizeof(buf) - 1, fp);
    fclose(fp);
    EXPECT_EQ("rename_0300", strcmp(buf, "old_content"), 0);

    unlink(newpath);
}

/**
 * @tc.name      : rename_0400
 * @tc.desc      : Rename a nonexistent file, should fail with ENOENT
 * @tc.level     : Level 1
 */
void rename_0400(void)
{
    errno = 0;
    int ret = rename("/data/rename_test_0400_nonexistent.txt", "/data/rename_test_0400_dst.txt");
    EXPECT_EQ("rename_0400", ret, -1);
    EXPECT_EQ("rename_0400", errno, ENOENT);
}

/**
 * @tc.name      : rename_0500
 * @tc.desc      : Rename a file onto an existing directory, should fail with EISDIR
 * @tc.level     : Level 2
 */
void rename_0500(void)
{
    const char *file = "/data/rename_test_0500_file.txt";
    const char *dir = "/data/rename_test_0500_dir";

    FILE *fp = fopen(file, "w");
    EXPECT_TRUE("rename_0500", fp != NULL);
    if (fp == NULL) {
        return;
    }
    fclose(fp);

    EXPECT_EQ("rename_0500", mkdir(dir, 0777), 0);

    errno = 0;
    EXPECT_EQ("rename_0500", rename(file, dir), -1);
    EXPECT_EQ("rename_0500", errno, EISDIR);

    unlink(file);
    rmdir(dir);
}

/**
 * @tc.name      : rename_0600
 * @tc.desc      : Rename a directory onto an existing non-empty directory, should fail with ENOTEMPTY
 * @tc.level     : Level 2
 */
void rename_0600(void)
{
    const char *srcDir = "/data/rename_test_0600_src";
    const char *dstDir = "/data/rename_test_0600_dst";
    const char *dstFile = "/data/rename_test_0600_dst/inner.txt";

    EXPECT_EQ("rename_0600", mkdir(srcDir, 0777), 0);
    EXPECT_EQ("rename_0600", mkdir(dstDir, 0777), 0);

    FILE *fp = fopen(dstFile, "w");
    EXPECT_TRUE("rename_0600", fp != NULL);
    if (fp == NULL) {
        rmdir(srcDir);
        rmdir(dstDir);
        return;
    }
    fclose(fp);

    errno = 0;
    EXPECT_EQ("rename_0600", rename(srcDir, dstDir), -1);
    EXPECT_EQ("rename_0600", errno, ENOTEMPTY);

    unlink(dstFile);
    rmdir(srcDir);
    rmdir(dstDir);
}

int main(void)
{
    rename_0100();
    rename_0200();
    rename_0300();
    rename_0400();
    rename_0500();
    rename_0600();

    return t_status;
}
