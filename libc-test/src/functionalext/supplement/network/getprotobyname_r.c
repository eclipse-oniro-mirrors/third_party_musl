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
 * @tc.name      : getprotobyname_r_0100
 * @tc.desc      : Look up tcp protocol by name, verify result
 * @tc.level     : Level 0
 */
void getprotobyname_r_0100(void)
{
    struct protoent pe;
    char buf[BUF_SZ];
    struct protoent *result = NULL;
    int rv = getprotobyname_r("tcp", &pe, buf, sizeof(buf), &result);
    EXPECT_EQ("getprotobyname_r_0100", rv, 0);
    EXPECT_PTRNE("getprotobyname_r_0100", result, NULL);
    EXPECT_EQ("getprotobyname_r_0100", pe.p_proto, TCP_PROTO_NUM);
    EXPECT_STREQ("getprotobyname_r_0100", pe.p_name, "tcp");
}

/**
 * @tc.name      : getprotobyname_r_0200
 * @tc.desc      : Look up udp protocol by name
 * @tc.level     : Level 0
 */
void getprotobyname_r_0200(void)
{
    struct protoent pe;
    char buf[BUF_SZ];
    struct protoent *result = NULL;
    int rv = getprotobyname_r("udp", &pe, buf, sizeof(buf), &result);
    EXPECT_EQ("getprotobyname_r_0200", rv, 0);
    EXPECT_EQ("getprotobyname_r_0200", pe.p_proto, 17);
}

/**
 * @tc.name      : getprotobyname_r_0300
 * @tc.desc      : Pass NULL name, expect EINVAL
 * @tc.level     : Level 2
 */
void getprotobyname_r_0300(void)
{
    struct protoent pe;
    char buf[BUF_SZ];
    struct protoent *result = NULL;
    int rv = getprotobyname_r(NULL, &pe, buf, sizeof(buf), &result);
    EXPECT_EQ("getprotobyname_r_0300", rv, EINVAL);
    EXPECT_PTREQ("getprotobyname_r_0300", result, NULL);
}

/**
 * @tc.name      : getprotobyname_r_0400
 * @tc.desc      : Look up non-existent protocol, expect ENOENT
 * @tc.level     : Level 2
 */
void getprotobyname_r_0400(void)
{
    struct protoent pe;
    char buf[BUF_SZ];
    struct protoent *result = NULL;
    int rv = getprotobyname_r("nosuchproto", &pe, buf, sizeof(buf), &result);
    EXPECT_EQ("getprotobyname_r_0400", rv, ENOENT);
}

/**
 * @tc.name      : getprotobyname_r_0500
 * @tc.desc      : Use a tiny buffer, expect ERANGE
 * @tc.level     : Level 2
 */
void getprotobyname_r_0500(void)
{
    struct protoent pe;
    char tinyBuf[TINY_BUF_SZ];
    struct protoent *result = NULL;
    int rv = getprotobyname_r("tcp", &pe, tinyBuf, sizeof(tinyBuf), &result);
    EXPECT_EQ("getprotobyname_r_0500", rv, ERANGE);
}

/**
 * @tc.name      : getprotobyname_r_0600
 * @tc.desc      : Boundary: look up first entry "ip" and last entry "raw"
 * @tc.level     : Level 0
 */
void getprotobyname_r_0600(void)
{
    struct protoent pe;
    char buf[BUF_SZ];
    struct protoent *result = NULL;
    int rv = getprotobyname_r("ip", &pe, buf, sizeof(buf), &result);
    EXPECT_EQ("getprotobyname_r_0600_ip", rv, 0);
    EXPECT_EQ("getprotobyname_r_0600_ip", pe.p_proto, 0);

    rv = getprotobyname_r("raw", &pe, buf, sizeof(buf), &result);
    EXPECT_EQ("getprotobyname_r_0600_raw", rv, 0);
    EXPECT_EQ("getprotobyname_r_0600_raw", pe.p_proto, 255);
}

/**
 * @tc.name      : getprotobyname_r_0700
 * @tc.desc      : Empty string name returns ENOENT
 * @tc.level     : Level 2
 */
void getprotobyname_r_0700(void)
{
    struct protoent pe;
    char buf[BUF_SZ];
    struct protoent *result = NULL;
    int rv = getprotobyname_r("", &pe, buf, sizeof(buf), &result);
    EXPECT_EQ("getprotobyname_r_0700", rv, ENOENT);
}

/**
 * @tc.name      : getprotobyname_r_0800
 * @tc.desc      : Verify p_aliases array is properly populated
 * @tc.level     : Level 0
 */
void getprotobyname_r_0800(void)
{
    struct protoent pe;
    char buf[BUF_SZ];
    struct protoent *result = NULL;
    int rv = getprotobyname_r("tcp", &pe, buf, sizeof(buf), &result);
    EXPECT_EQ("getprotobyname_r_0800", rv, 0);
    EXPECT_PTRNE("getprotobyname_r_0800", pe.p_aliases, NULL);
    EXPECT_PTREQ("getprotobyname_r_0800", pe.p_aliases[0], NULL);
}

static void *gpbn_worker(void *arg)
{
    struct protoent pe;
    char buf[BUF_SZ];
    struct protoent *result = NULL;
    int rv = getprotobyname_r("tcp", &pe, buf, sizeof(buf), &result);
    *(int *)arg = rv;
    return NULL;
}

/**
 * @tc.name      : getprotobyname_r_0900
 * @tc.desc      : Multi-threaded: concurrent getprotobyname_r calls
 * @tc.level     : Level 0
 */
void getprotobyname_r_0900(void)
{
    pthread_t tids[4];
    int results[4] = {0};
    for (int i = 0; i < 4; i++) {
        EXPECT_EQ("getprotobyname_r_0900", pthread_create(&tids[i], NULL, gpbn_worker, &results[i]), 0);
    }
    for (int i = 0; i < 4; i++) {
        pthread_join(tids[i], NULL);
        EXPECT_EQ("getprotobyname_r_0900", results[i], 0);
    }
}

int main(void)
{
    getprotobyname_r_0100();
    getprotobyname_r_0200();
    getprotobyname_r_0300();
    getprotobyname_r_0400();
    getprotobyname_r_0500();
    getprotobyname_r_0600();
    getprotobyname_r_0700();
    getprotobyname_r_0800();
    getprotobyname_r_0900();
    return t_status;
}
