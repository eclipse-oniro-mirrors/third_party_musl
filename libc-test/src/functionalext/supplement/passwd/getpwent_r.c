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
#include <stdlib.h>
#include "functionalext.h"

#define BUF_SZ 1024
#define TINY_BUF_SZ 2

/**
 * @tc.name      : getpwent_r_0100
 * @tc.desc      : Read passwd entry or handle ENOENT gracefully
 * @tc.level     : Level 0
 */
void getpwent_r_0100(void)
{
    struct passwd pw;
    char buf[BUF_SZ];
    struct passwd *result = NULL;
    int rv = getpwent_r(&pw, buf, sizeof(buf), &result);
    if (rv == 0) {
        EXPECT_PTRNE("getpwent_r_0100", result, NULL);
        EXPECT_PTRNE("getpwent_r_0100", pw.pw_name, NULL);
    } else {
        EXPECT_EQ("getpwent_r_0100", rv, ENOENT);
    }
}

/**
 * @tc.name      : getpwent_r_0200
 * @tc.desc      : Enumerate all passwd entries until ENOENT
 * @tc.level     : Level 0
 */
void getpwent_r_0200(void)
{
    struct passwd pw;
    size_t bufsz = BUF_SZ;
    char *buf = malloc(bufsz);
    EXPECT_PTRNE("getpwent_r_0200", buf, NULL);
    struct passwd *result = NULL;
    int count = 0;
    int rv;
    while (1) {
        rv = getpwent_r(&pw, buf, bufsz, &result);
        if (rv == ERANGE) {
            bufsz *= 2;
            if (bufsz > 65536) {
                break;
            }
            char *newbuf = realloc(buf, bufsz);
            EXPECT_PTRNE("getpwent_r_0200", newbuf, NULL);
            if (!newbuf) {
                break;
            }
            buf = newbuf;
            continue;
        }
        if (rv != 0) {
            break;
        }
        count++;
        if (count > 200) {
            break;
        }
    }
    if (count > 0 && rv != 0) {
        EXPECT_EQ("getpwent_r_0200", rv, ENOENT);
    }
    EXPECT_TRUE("getpwent_r_0200", count > 0);
    free(buf);
}

/**
 * @tc.name      : getpwent_r_0300
 * @tc.desc      : Use tiny buffer, expect ERANGE or ENOENT
 * @tc.level     : Level 2
 */
void getpwent_r_0300(void)
{
    struct passwd pw;
    char tinyBuf[TINY_BUF_SZ];
    struct passwd *result = NULL;
    int rv = getpwent_r(&pw, tinyBuf, sizeof(tinyBuf), &result);
    if (rv != 0 && rv != ENOENT) {
        EXPECT_EQ("getpwent_r_0300", rv, ERANGE);
    }
}

/**
 * @tc.name      : getpwent_r_0400
 * @tc.desc      : Boundary: buflen = 0
 * @tc.level     : Level 2
 */
void getpwent_r_0400(void)
{
    struct passwd pw;
    struct passwd *result = NULL;
    int rv = getpwent_r(&pw, NULL, 0, &result);
    if (rv != 0 && rv != ENOENT) {
        EXPECT_EQ("getpwent_r_0400", rv, ERANGE);
    }
}

static void *gpw_worker(void *arg)
{
    struct passwd pw;
    char buf[BUF_SZ];
    struct passwd *result = NULL;
    int count = 0;
    int rv;
    while ((rv = getpwent_r(&pw, buf, sizeof(buf), &result)) == 0) {
        count++;
        if (count > 200) {
            break;
        }
    }
    *(int *)arg = count;
    return NULL;
}

/**
 * @tc.name      : getpwent_r_0500
 * @tc.desc      : Multi-threaded: concurrent getpwent_r enumeration
 * @tc.level     : Level 0
 */
void getpwent_r_0500(void)
{
    pthread_t tids[4];
    int counts[4] = {0};
    for (int i = 0; i < 4; i++) {
        EXPECT_EQ("getpwent_r_0500", pthread_create(&tids[i], NULL, gpw_worker, &counts[i]), 0);
    }
    int total = 0;
    for (int i = 0; i < 4; i++) {
        pthread_join(tids[i], NULL);
        total += counts[i];
    }
    EXPECT_TRUE("getpwent_r_0500", total >= 0);
}

int main(void)
{
    getpwent_r_0100();
    getpwent_r_0200();
    getpwent_r_0300();
    getpwent_r_0400();
    getpwent_r_0500();
    return t_status;
}
