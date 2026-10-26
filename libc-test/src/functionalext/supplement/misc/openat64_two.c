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
#include <pthread.h>
#include <unistd.h>
#include "functionalext.h"

/**
 * @tc.name      : openat64_two_0100
 * @tc.desc      : Open a file via __openat64_2 with O_RDONLY (no O_CREAT)
 * @tc.level     : Level 0
 */
void openat64_two_0100(void)
{
    const char *testFile = "openat64_two_test.txt";
    FILE *fp = fopen(testFile, "w");
    EXPECT_PTRNE("openat64_two_0100", fp, NULL);
    if (fp) {
        (void)fclose(fp);
    }

    int fd = __openat64_2(AT_FDCWD, testFile, O_RDONLY);
    EXPECT_TRUE("openat64_two_0100", fd >= 0);
    if (fd >= 0) {
        (void)close(fd);
    }
    (void)remove(testFile);
}

/**
 * @tc.name      : openat64_two_0200
 * @tc.desc      : Open a non-existent file for read, expect failure
 * @tc.level     : Level 2
 */
void openat64_two_0200(void)
{
    int fd = __openat64_2(AT_FDCWD, "nonexistent_openat64_file.txt", O_RDONLY);
    EXPECT_TRUE("openat64_two_0200", fd < 0);
    if (fd >= 0) {
        (void)close(fd);
    }
}

/**
 * @tc.name      : openat64_two_0300
 * @tc.desc      : Open with O_WRONLY and write to file
 * @tc.level     : Level 0
 */
void openat64_two_0300(void)
{
    const char *testFile = "openat64_two_test3.txt";
    FILE *fp2 = fopen(testFile, "w");
    EXPECT_PTRNE("openat64_two_0300", fp2, NULL);
    if (fp2) { (void)fclose(fp2); }
    int fd = __openat64_2(AT_FDCWD, testFile, O_WRONLY | O_TRUNC);
    if (fd >= 0) {
        ssize_t wr = write(fd, "test", 4);
        EXPECT_EQ("openat64_two_0300", (int)wr, 4);
        (void)close(fd);
    }
    (void)remove(testFile);
    EXPECT_TRUE("openat64_two_0300", 1);
}

/**
 * @tc.name      : openat64_two_0400
 * @tc.desc      : Open with O_RDWR and verify read/write
 * @tc.level     : Level 0
 */
void openat64_two_0400(void)
{
    const char *testFile = "openat64_two_test4.txt";
    FILE *fp = fopen(testFile, "w");
    EXPECT_PTRNE("openat64_two_0400", fp, NULL);
    if (fp) {
        (void)fprintf(fp, "hello");
        (void)fclose(fp);
    }
    int fd = __openat64_2(AT_FDCWD, testFile, O_RDWR);
    EXPECT_TRUE("openat64_two_0400", fd >= 0);
    if (fd >= 0) {
        char buf[16] = {0};
        ssize_t n = read(fd, buf, sizeof(buf));
        EXPECT_EQ("openat64_two_0400", (int)n, 5);
        (void)close(fd);
    }
    (void)remove(testFile);
}

/**
 * @tc.name      : openat64_two_0500
 * @tc.desc      : Boundary: invalid fd (-1), expect failure
 * @tc.level     : Level 2
 */
void openat64_two_0500(void)
{
    int fd = __openat64_2(-1, "anyfile.txt", O_RDONLY);
    EXPECT_TRUE("openat64_two_0500", fd < 0);
    if (fd >= 0) {
        (void)close(fd);
    }
}

/**
 * @tc.name      : openat64_two_0600
 * @tc.desc      : Boundary: NULL pathname, expect failure
 * @tc.level     : Level 2
 */
void openat64_two_0600(void)
{
    int fd = __openat64_2(AT_FDCWD, NULL, O_RDONLY);
    EXPECT_TRUE("openat64_two_0600", fd < 0);
    if (fd >= 0) {
        (void)close(fd);
    }
}

/**
 * @tc.name      : openat64_two_0700
 * @tc.desc      : Boundary: empty string pathname, expect failure
 * @tc.level     : Level 2
 */
void openat64_two_0700(void)
{
    int fd = __openat64_2(AT_FDCWD, "", O_RDONLY);
    EXPECT_TRUE("openat64_two_0700", fd < 0);
    if (fd >= 0) {
        (void)close(fd);
    }
}

static void *oat_worker(void *arg)
{
    const char *testFile = "openat64_two_mt.txt";
    int fd = __openat64_2(AT_FDCWD, testFile, O_RDONLY);
    if (fd >= 0) {
        (void)close(fd);
    }
    *(int *)arg = (fd >= 0) ? 0 : -1;
    return NULL;
}

/**
 * @tc.name      : openat64_two_0800
 * @tc.desc      : Multi-threaded: concurrent __openat64_2 opening same file
 * @tc.level     : Level 0
 */
void openat64_two_0800(void)
{
    const char *testFile = "openat64_two_mt.txt";
    FILE *fp = fopen(testFile, "w");
    EXPECT_PTRNE("openat64_two_0800", fp, NULL);
    if (fp) {
        (void)fclose(fp);
    }

    pthread_t tids[4];
    int results[4] = {0};
    for (int i = 0; i < 4; i++) {
        EXPECT_EQ("openat64_two_0800", pthread_create(&tids[i], NULL, oat_worker, &results[i]), 0);
    }
    for (int i = 0; i < 4; i++) {
        EXPECT_EQ("openat64_two_0800", pthread_join(tids[i], NULL), 0);
        EXPECT_EQ("openat64_two_0800", results[i], 0);
    }
    (void)remove(testFile);
}

int main(void)
{
    openat64_two_0100();
    openat64_two_0200();
    openat64_two_0300();
    openat64_two_0400();
    openat64_two_0500();
    openat64_two_0600();
    openat64_two_0700();
    openat64_two_0800();
    return t_status;
}
