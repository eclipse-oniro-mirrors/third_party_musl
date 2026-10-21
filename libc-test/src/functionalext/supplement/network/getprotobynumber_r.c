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

#define TCP_PROTO_NUM 6
#define BUF_SZ 256
#define TINY_BUF_SZ 2

/**
 * @tc.name      : getprotobynumber_r_0100
 * @tc.desc      : Look up protocol by number 6 (tcp)
 * @tc.level     : Level 0
 */
void getprotobynumber_r_0100(void)
{
    struct protoent pe;
    char buf[BUF_SZ];
    struct protoent *result = NULL;
    int rv = getprotobynumber_r(TCP_PROTO_NUM, &pe, buf, sizeof(buf), &result);
    EXPECT_EQ("getprotobynumber_r_0100", rv, 0);
    EXPECT_PTRNE("getprotobynumber_r_0100", result, NULL);
    EXPECT_STREQ("getprotobynumber_r_0100", pe.p_name, "tcp");
    EXPECT_EQ("getprotobynumber_r_0100", pe.p_proto, TCP_PROTO_NUM);
}

/**
 * @tc.name      : getprotobynumber_r_0200
 * @tc.desc      : Look up protocol by number 17 (udp)
 * @tc.level     : Level 0
 */
void getprotobynumber_r_0200(void)
{
    struct protoent pe;
    char buf[BUF_SZ];
    struct protoent *result = NULL;
    int rv = getprotobynumber_r(17, &pe, buf, sizeof(buf), &result);
    EXPECT_EQ("getprotobynumber_r_0200", rv, 0);
    EXPECT_STREQ("getprotobynumber_r_0200", pe.p_name, "udp");
}

/**
 * @tc.name      : getprotobynumber_r_0300
 * @tc.desc      : Look up non-existent protocol number, expect ENOENT
 * @tc.level     : Level 2
 */
void getprotobynumber_r_0300(void)
{
    struct protoent pe;
    char buf[BUF_SZ];
    struct protoent *result = NULL;
    int rv = getprotobynumber_r(999, &pe, buf, sizeof(buf), &result);
    EXPECT_EQ("getprotobynumber_r_0300", rv, ENOENT);
}

/**
 * @tc.name      : getprotobynumber_r_0400
 * @tc.desc      : Use tiny buffer, expect ERANGE
 * @tc.level     : Level 2
 */
void getprotobynumber_r_0400(void)
{
    struct protoent pe;
    char tinyBuf[TINY_BUF_SZ];
    struct protoent *result = NULL;
    int rv = getprotobynumber_r(TCP_PROTO_NUM, &pe, tinyBuf, sizeof(tinyBuf), &result);
    EXPECT_EQ("getprotobynumber_r_0400", rv, ERANGE);
}

/**
 * @tc.name      : getprotobynumber_r_0500
 * @tc.desc      : Boundary: proto 0 (ip, first entry) and proto 255 (raw, last entry)
 * @tc.level     : Level 0
 */
void getprotobynumber_r_0500(void)
{
    struct protoent pe;
    char buf[BUF_SZ];
    struct protoent *result = NULL;

    int rv = getprotobynumber_r(0, &pe, buf, sizeof(buf), &result);
    EXPECT_EQ("getprotobynumber_r_0500_ip", rv, 0);
    EXPECT_STREQ("getprotobynumber_r_0500_ip", pe.p_name, "ip");

    rv = getprotobynumber_r(255, &pe, buf, sizeof(buf), &result);
    EXPECT_EQ("getprotobynumber_r_0500_raw", rv, 0);
    EXPECT_STREQ("getprotobynumber_r_0500_raw", pe.p_name, "raw");
}

/**
 * @tc.name      : getprotobynumber_r_0600
 * @tc.desc      : Abnormal: negative proto number returns ENOENT
 * @tc.level     : Level 2
 */
void getprotobynumber_r_0600(void)
{
    struct protoent pe;
    char buf[BUF_SZ];
    struct protoent *result = NULL;
    int rv = getprotobynumber_r(-1, &pe, buf, sizeof(buf), &result);
    EXPECT_EQ("getprotobynumber_r_0600", rv, ENOENT);
}

/**
 * @tc.name      : getprotobynumber_r_0700
 * @tc.desc      : Verify p_aliases array populated correctly
 * @tc.level     : Level 0
 */
void getprotobynumber_r_0700(void)
{
    struct protoent pe;
    char buf[BUF_SZ];
    struct protoent *result = NULL;
    int rv = getprotobynumber_r(TCP_PROTO_NUM, &pe, buf, sizeof(buf), &result);
    EXPECT_EQ("getprotobynumber_r_0700", rv, 0);
    EXPECT_PTRNE("getprotobynumber_r_0700", pe.p_aliases, NULL);
    EXPECT_PTREQ("getprotobynumber_r_0700", pe.p_aliases[0], NULL);
}

static void *gpbn_worker(void *arg)
{
    struct protoent pe;
    char buf[BUF_SZ];
    struct protoent *result = NULL;
    *(int *)arg = getprotobynumber_r(TCP_PROTO_NUM, &pe, buf, sizeof(buf), &result);
    return NULL;
}

/**
 * @tc.name      : getprotobynumber_r_0800
 * @tc.desc      : Multi-threaded: concurrent getprotobynumber_r calls
 * @tc.level     : Level 0
 */
void getprotobynumber_r_0800(void)
{
    pthread_t tids[4];
    int results[4] = {0};
    for (int i = 0; i < 4; i++) {
        EXPECT_EQ("getprotobynumber_r_0800", pthread_create(&tids[i], NULL, gpbn_worker, &results[i]), 0);
    }
    for (int i = 0; i < 4; i++) {
        pthread_join(tids[i], NULL);
        EXPECT_EQ("getprotobynumber_r_0800", results[i], 0);
    }
}

int main(void)
{
    getprotobynumber_r_0100();
    getprotobynumber_r_0200();
    getprotobynumber_r_0300();
    getprotobynumber_r_0400();
    getprotobynumber_r_0500();
    getprotobynumber_r_0600();
    getprotobynumber_r_0700();
    getprotobynumber_r_0800();
    return t_status;
}
