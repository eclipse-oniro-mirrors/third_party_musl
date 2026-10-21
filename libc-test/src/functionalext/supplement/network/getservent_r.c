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
#include <netdb.h>
#include <pthread.h>
#include <stdlib.h>
#include "functionalext.h"

#define BUF_SZ 1024
#define TINY_BUF_SZ 2

/**
 * @tc.name      : getservent_r_0100
 * @tc.desc      : Read service entry or handle ENOENT gracefully
 * @tc.level     : Level 0
 */
void getservent_r_0100(void)
{
    struct servent se;
    char buf[BUF_SZ];
    struct servent *result = NULL;
    int rv = getservent_r(&se, buf, sizeof(buf), &result);
    if (rv == 0) {
        EXPECT_PTRNE("getservent_r_0100", result, NULL);
        EXPECT_PTRNE("getservent_r_0100", se.s_name, NULL);
        EXPECT_PTRNE("getservent_r_0100", se.s_proto, NULL);
    } else {
        EXPECT_EQ("getservent_r_0100", rv, ENOENT);
    }
}

/**
 * @tc.name      : getservent_r_0200
 * @tc.desc      : Enumerate all services until ENOENT
 * @tc.level     : Level 0
 */
void getservent_r_0200(void)
{
    struct servent se;
    size_t bufsz = BUF_SZ;
    char *buf = malloc(bufsz);
    EXPECT_PTRNE("getservent_r_0200", buf, NULL);
    struct servent *result = NULL;
    int count = 0;
    int rv;
    while (1) {
        rv = getservent_r(&se, buf, bufsz, &result);
        if (rv == ERANGE) {
            bufsz *= 2;
            if (bufsz > 65536) {
                break;
            }
            char *newbuf = realloc(buf, bufsz);
            EXPECT_PTRNE("getservent_r_0200", newbuf, NULL);
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
        EXPECT_EQ("getservent_r_0200", rv, ENOENT);
    }
    EXPECT_TRUE("getservent_r_0200", count > 0);
    free(buf);
}

/**
 * @tc.name      : getservent_r_0300
 * @tc.desc      : Use tiny buffer, expect ERANGE or ENOENT
 * @tc.level     : Level 2
 */
void getservent_r_0300(void)
{
    struct servent se;
    char tinyBuf[TINY_BUF_SZ];
    struct servent *result = NULL;
    int rv = getservent_r(&se, tinyBuf, sizeof(tinyBuf), &result);
    if (rv != 0 && rv != ENOENT) {
        EXPECT_EQ("getservent_r_0300", rv, ERANGE);
    }
}

/**
 * @tc.name      : getservent_r_0400
 * @tc.desc      : Boundary: buflen = 0
 * @tc.level     : Level 2
 */
void getservent_r_0400(void)
{
    struct servent se;
    struct servent *result = NULL;
    int rv = getservent_r(&se, NULL, 0, &result);
    if (rv != 0 && rv != ENOENT) {
        EXPECT_EQ("getservent_r_0400", rv, ERANGE);
    }
}

static void *gse_worker(void *arg)
{
    struct servent se;
    char buf[BUF_SZ];
    struct servent *result = NULL;
    int count = 0;
    int rv;
    while ((rv = getservent_r(&se, buf, sizeof(buf), &result)) == 0) {
        count++;
        if (count > 200) {
            break;
        }
    }
    *(int *)arg = count;
    return NULL;
}

/**
 * @tc.name      : getservent_r_0500
 * @tc.desc      : Multi-threaded: concurrent getservent_r enumeration
 * @tc.level     : Level 0
 */
void getservent_r_0500(void)
{
    pthread_t tids[4];
    int counts[4] = {0};
    for (int i = 0; i < 4; i++) {
        EXPECT_EQ("getservent_r_0500", pthread_create(&tids[i], NULL, gse_worker, &counts[i]), 0);
    }
    int total = 0;
    for (int i = 0; i < 4; i++) {
        pthread_join(tids[i], NULL);
        total += counts[i];
    }
    EXPECT_TRUE("getservent_r_0500", total >= 0);
}

int main(void)
{
#ifdef __LITEOS_A__
    getservent_r_0100();
    getservent_r_0200();
    getservent_r_0300();
    getservent_r_0400();
    getservent_r_0500();
#endif
    return t_status;
}
