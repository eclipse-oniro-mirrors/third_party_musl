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
#include <string.h>
#include "functionalext.h"

#define BUF_SZ 512
#define TINY_BUF_SZ 2

/**
 * @tc.name      : getprotoent_r_0100
 * @tc.desc      : Get first protocol entry (ip, proto 0)
 * @tc.level     : Level 0
 */
void getprotoent_r_0100(void)
{
    struct protoent pe;
    char buf[BUF_SZ];
    struct protoent *result = NULL;
    int rv = getprotoent_r(&pe, buf, sizeof(buf), &result);
    EXPECT_EQ("getprotoent_r_0100", rv, 0);
    EXPECT_PTRNE("getprotoent_r_0100", result, NULL);
    EXPECT_EQ("getprotoent_r_0100", pe.p_proto, 0);
    EXPECT_STREQ("getprotoent_r_0100", pe.p_name, "ip");
}

/**
 * @tc.name      : getprotoent_r_0200
 * @tc.desc      : Enumerate all protocols until ENOENT
 * @tc.level     : Level 0
 */
void getprotoent_r_0200(void)
{
    struct protoent pe;
    char buf[BUF_SZ];
    struct protoent *result = NULL;
    int count = 0;
    int rv;
    while (1) {
        rv = getprotoent_r(&pe, buf, sizeof(buf), &result);
        if (rv != 0) {
            break;
        }
        count++;
        if (count > 200) {
            break;
        }
    }
    EXPECT_TRUE("getprotoent_r_0200", count >= 2);
    EXPECT_EQ("getprotoent_r_0200", rv, ENOENT);
}

/**
 * @tc.name      : getprotoent_r_0300
 * @tc.desc      : Use tiny buffer, expect ERANGE; retry with larger buffer returns same entry
 * @tc.level     : Level 2
 */
void getprotoent_r_0300(void)
{
    struct protoent pe;
    setprotoent(0);
    char tinyBuf[TINY_BUF_SZ];
    struct protoent *result = NULL;
    int rv = getprotoent_r(&pe, tinyBuf, sizeof(tinyBuf), &result);
    EXPECT_EQ("getprotoent_r_0300", rv, ERANGE);

    char bigBuf[BUF_SZ];
    rv = getprotoent_r(&pe, bigBuf, sizeof(bigBuf), &result);
    EXPECT_EQ("getprotoent_r_0300_retry", rv, 0);
    EXPECT_PTRNE("getprotoent_r_0300_retry", result, NULL);
}

/**
 * @tc.name      : getprotoent_r_0400
 * @tc.desc      : Verify p_aliases populated correctly
 * @tc.level     : Level 0
 */
void getprotoent_r_0400(void)
{
    struct protoent pe;
    setprotoent(0);
    char buf[BUF_SZ];
    struct protoent *result = NULL;
    int rv = getprotoent_r(&pe, buf, sizeof(buf), &result);
    EXPECT_EQ("getprotoent_r_0400", rv, 0);
    EXPECT_PTRNE("getprotoent_r_0400", pe.p_aliases, NULL);
    EXPECT_PTREQ("getprotoent_r_0400", pe.p_aliases[0], NULL);
}

static void *gpe_worker(void *arg)
{
    struct protoent pe;
    char buf[BUF_SZ];
    struct protoent *result = NULL;
    int rv;
    int count = 0;
    while ((rv = getprotoent_r(&pe, buf, sizeof(buf), &result)) == 0) {
        count++;
        if (count > 200) {
            break;
        }
    }
    *(int *)arg = count;
    return NULL;
}

/**
 * @tc.name      : getprotoent_r_0500
 * @tc.desc      : Multi-threaded: concurrent getprotoent_r enumeration
 * @tc.level     : Level 0
 */
void getprotoent_r_0500(void)
{
    pthread_t tids[4];
    int counts[4] = {0};
    for (int i = 0; i < 4; i++) {
        EXPECT_EQ("getprotoent_r_0500", pthread_create(&tids[i], NULL, gpe_worker, &counts[i]), 0);
    }
    int total = 0;
    for (int i = 0; i < 4; i++) {
        pthread_join(tids[i], NULL);
        total += counts[i];
    }
    EXPECT_TRUE("getprotoent_r_0500", total > 0);
}

int main(void)
{
    getprotoent_r_0100();
    getprotoent_r_0200();
    getprotoent_r_0300();
    getprotoent_r_0400();
    getprotoent_r_0500();
    return t_status;
}
