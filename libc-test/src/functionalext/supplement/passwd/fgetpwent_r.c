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
#include <pwd.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include "functionalext.h"

#define BUF_SZ 512
#define TINY_BUF_SZ 2

/**
 * @tc.name      : fgetpwent_r_0100
 * @tc.desc      : Read a passwd entry from a temp file, verify all fields
 * @tc.level     : Level 0
 */
void fgetpwent_r_0100(void)
{
    const char *testFile = "fgetpwent_r_test.txt";
    const char *passwdLine = "testuser:x:1000:1000:Test User:/home/testuser:/bin/sh\n";
    FILE *fp = fopen(testFile, "w");
    EXPECT_PTRNE("fgetpwent_r_0100", fp, NULL);
    (void)fprintf(fp, "%s", passwdLine);
    (void)fclose(fp);

    fp = fopen(testFile, "r");
    EXPECT_PTRNE("fgetpwent_r_0100", fp, NULL);
    struct passwd pw;
    char buf[BUF_SZ];
    struct passwd *result = NULL;
    int rv = fgetpwent_r(fp, &pw, buf, sizeof(buf), &result);
    EXPECT_EQ("fgetpwent_r_0100", rv, 0);
    EXPECT_PTRNE("fgetpwent_r_0100", result, NULL);
    EXPECT_STREQ("fgetpwent_r_0100", pw.pw_name, "testuser");
    EXPECT_STREQ("fgetpwent_r_0100", pw.pw_passwd, "x");
    EXPECT_EQ("fgetpwent_r_0100", pw.pw_uid, 1000);
    EXPECT_EQ("fgetpwent_r_0100", pw.pw_gid, 1000);
    EXPECT_STREQ("fgetpwent_r_0100", pw.pw_gecos, "Test User");
    EXPECT_STREQ("fgetpwent_r_0100", pw.pw_dir, "/home/testuser");
    EXPECT_STREQ("fgetpwent_r_0100", pw.pw_shell, "/bin/sh");
    (void)fclose(fp);
    (void)remove(testFile);
}

/**
 * @tc.name      : fgetpwent_r_0200
 * @tc.desc      : Use tiny buffer, expect ERANGE
 * @tc.level     : Level 2
 */
void fgetpwent_r_0200(void)
{
    const char *testFile = "fgetpwent_r_test2.txt";
    const char *passwdLine = "testuser:x:1000:1000:Test User:/home/testuser:/bin/sh\n";
    FILE *fp = fopen(testFile, "w");
    EXPECT_PTRNE("fgetpwent_r_0200", fp, NULL);
    (void)fprintf(fp, "%s", passwdLine);
    (void)fclose(fp);

    fp = fopen(testFile, "r");
    EXPECT_PTRNE("fgetpwent_r_0200", fp, NULL);
    struct passwd pw;
    char tinyBuf[TINY_BUF_SZ];
    struct passwd *result = NULL;
    int rv = fgetpwent_r(fp, &pw, tinyBuf, sizeof(tinyBuf), &result);
    EXPECT_EQ("fgetpwent_r_0200", rv, ERANGE);
    (void)fclose(fp);
    (void)remove(testFile);
}

/**
 * @tc.name      : fgetpwent_r_0300
 * @tc.desc      : Read from an empty file, expect ENOENT
 * @tc.level     : Level 2
 */
void fgetpwent_r_0300(void)
{
    const char *testFile = "fgetpwent_r_test3.txt";
    FILE *fp = fopen(testFile, "w");
    EXPECT_PTRNE("fgetpwent_r_0300", fp, NULL);
    (void)fclose(fp);

    fp = fopen(testFile, "r");
    EXPECT_PTRNE("fgetpwent_r_0300", fp, NULL);
    struct passwd pw;
    char buf[BUF_SZ];
    struct passwd *result = NULL;
    int rv = fgetpwent_r(fp, &pw, buf, sizeof(buf), &result);
    EXPECT_EQ("fgetpwent_r_0300", rv, ENOENT);
    (void)fclose(fp);
    (void)remove(testFile);
}

/**
 * @tc.name      : fgetpwent_r_0400
 * @tc.desc      : Read multiple entries from a multi-line file
 * @tc.level     : Level 0
 */
void fgetpwent_r_0400(void)
{
    const char *testFile = "fgetpwent_r_test4.txt";
    const char *content = "user1:x:1001:1001:User One:/home/user1:/bin/sh\n"
                          "user2:x:1002:1002:User Two:/home/user2:/bin/bash\n";
    FILE *fp = fopen(testFile, "w");
    EXPECT_PTRNE("fgetpwent_r_0400", fp, NULL);
    (void)fprintf(fp, "%s", content);
    (void)fclose(fp);

    fp = fopen(testFile, "r");
    EXPECT_PTRNE("fgetpwent_r_0400", fp, NULL);
    struct passwd pw;
    char buf[BUF_SZ];
    struct passwd *result = NULL;

    int rv = fgetpwent_r(fp, &pw, buf, sizeof(buf), &result);
    EXPECT_EQ("fgetpwent_r_0400_first", rv, 0);
    EXPECT_STREQ("fgetpwent_r_0400_first", pw.pw_name, "user1");
    EXPECT_EQ("fgetpwent_r_0400_first", pw.pw_uid, 1001);

    rv = fgetpwent_r(fp, &pw, buf, sizeof(buf), &result);
    EXPECT_EQ("fgetpwent_r_0400_second", rv, 0);
    EXPECT_STREQ("fgetpwent_r_0400_second", pw.pw_name, "user2");
    EXPECT_EQ("fgetpwent_r_0400_second", pw.pw_uid, 1002);

    rv = fgetpwent_r(fp, &pw, buf, sizeof(buf), &result);
    EXPECT_EQ("fgetpwent_r_0400_eof", rv, ENOENT);
    (void)fclose(fp);
    (void)remove(testFile);
}

/**
 * @tc.name      : fgetpwent_r_0500
 * @tc.desc      : Line without trailing newline (last line)
 * @tc.level     : Level 0
 */
void fgetpwent_r_0500(void)
{
    const char *testFile = "fgetpwent_r_test5.txt";
    const char *content = "nouser:x:2000:2000:No Newline:/home/nouser:/bin/sh";
    FILE *fp = fopen(testFile, "w");
    EXPECT_PTRNE("fgetpwent_r_0500", fp, NULL);
    (void)fprintf(fp, "%s", content);
    (void)fclose(fp);

    fp = fopen(testFile, "r");
    EXPECT_PTRNE("fgetpwent_r_0500", fp, NULL);
    struct passwd pw;
    char buf[BUF_SZ];
    struct passwd *result = NULL;
    int rv = fgetpwent_r(fp, &pw, buf, sizeof(buf), &result);
    EXPECT_EQ("fgetpwent_r_0500", rv, 0);
    EXPECT_STREQ("fgetpwent_r_0500", pw.pw_name, "nouser");
    (void)fclose(fp);
    (void)remove(testFile);
}

/**
 * @tc.name      : fgetpwent_r_0600
 * @tc.desc      : Line with empty fields (empty passwd/gecos)
 * @tc.level     : Level 0
 */
void fgetpwent_r_0600(void)
{
    const char *testFile = "fgetpwent_r_test6.txt";
    const char *content = "emptyuser::3000:3000::/:/bin/sh\n";
    FILE *fp = fopen(testFile, "w");
    EXPECT_PTRNE("fgetpwent_r_0600", fp, NULL);
    (void)fprintf(fp, "%s", content);
    (void)fclose(fp);

    fp = fopen(testFile, "r");
    EXPECT_PTRNE("fgetpwent_r_0600", fp, NULL);
    struct passwd pw;
    char buf[BUF_SZ];
    struct passwd *result = NULL;
    int rv = fgetpwent_r(fp, &pw, buf, sizeof(buf), &result);
    EXPECT_EQ("fgetpwent_r_0600", rv, 0);
    EXPECT_STREQ("fgetpwent_r_0600", pw.pw_name, "emptyuser");
    EXPECT_EQ("fgetpwent_r_0600", pw.pw_uid, 3000);
    (void)fclose(fp);
    (void)remove(testFile);
}

static void *fpw_worker(void *arg)
{
    const char *testFile = "fgetpwent_r_mt.txt";
    FILE *fp = fopen(testFile, "r");
    if (!fp) {
        return NULL;
    }
    struct passwd pw;
    char buf[BUF_SZ];
    struct passwd *result = NULL;
    int count = 0;
    while (fgetpwent_r(fp, &pw, buf, sizeof(buf), &result) == 0) {
        count++;
        if (count > 100) {
            break;
        }
    }
    (void)fclose(fp);
    *(int *)arg = count;
    return NULL;
}

/**
 * @tc.name      : fgetpwent_r_0700
 * @tc.desc      : Multi-threaded: each thread reads from its own FILE* on same file
 * @tc.level     : Level 0
 */
void fgetpwent_r_0700(void)
{
    const char *testFile = "fgetpwent_r_mt.txt";
    FILE *fp = fopen(testFile, "w");
    EXPECT_PTRNE("fgetpwent_r_0700", fp, NULL);
    for (int i = 0; i < 10; i++) {
        (void)fprintf(fp, "user%d:x:%d:%d:User %d:/home/user%d:/bin/sh\n", i, i, i, i, i);
    }
    (void)fclose(fp);

    pthread_t tids[4];
    int counts[4] = {0};
    for (int i = 0; i < 4; i++) {
        EXPECT_EQ("fgetpwent_r_0700", pthread_create(&tids[i], NULL, fpw_worker, &counts[i]), 0);
    }
    for (int i = 0; i < 4; i++) {
        EXPECT_EQ("fgetpwent_r_0700", pthread_join(tids[i], NULL), 0);
        EXPECT_EQ("fgetpwent_r_0700", counts[i], 10);
    }
    (void)remove(testFile);
}

int main(void)
{
    fgetpwent_r_0100();
    fgetpwent_r_0200();
    fgetpwent_r_0300();
    fgetpwent_r_0400();
    fgetpwent_r_0500();
    fgetpwent_r_0600();
    fgetpwent_r_0700();
    return t_status;
}
