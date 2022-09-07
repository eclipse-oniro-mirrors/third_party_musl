/**
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <netdb.h>
#include <stdlib.h>
#include <sys/socket.h>

#include "functionalext.h"

const int GETADDRINFO_RESULT = 0;
const int FLAGS_FIELD = -1;
const int SERVICE_UNKNOEN = -2;
const int FAMILY_NOTSUPPORTED = -6;
const int SOCKTYPE_NOTSUPPORTED = -8;

/**
 * @tc.name      : getaddrinfo_0100
 * @tc.desc      : Each parameter is valid, the ai_flags of hint is AI_PASSIVE, which can resolve the IP
 *                 address of the host name.
 * @tc.level     : Level 0
 */
void getaddrinfo_0100(void)
{
    int ret = -1;
    int num = -1;
    struct addrinfo *result, hint;
    hint.ai_flags = AI_ALL;
    hint.ai_family = AF_UNSPEC;
    ret = getaddrinfo("127.0.0.1", NULL, &hint, &result);
    EXPECT_EQ("getaddrinfo_0100", ret, GETADDRINFO_RESULT);
    if (result->ai_addr != NULL) {
        num = 1;
    } else {
        num = 2;
    }
    EXPECT_EQ("getaddrinfo_0100", num, 1);
    freeaddrinfo(result);
    result = NULL;
}

/**
 * @tc.name      : getaddrinfo_0200
 * @tc.desc      : Each parameter is valid and can resolve the IP address of the host name.
 * @tc.level     : Level 0
 */
void getaddrinfo_0200(void)
{
    int ret = -1;
    int num = -1;
    struct addrinfo *result, hint;
    hint.ai_flags = AI_ALL;
    hint.ai_family = AF_UNSPEC;
    ret = getaddrinfo("127.0.0.1", NULL, &hint, &result);
    EXPECT_EQ("getaddrinfo_0200", ret, GETADDRINFO_RESULT);
    if (result->ai_addr != NULL) {
        num = 1;
    } else {
        num = 2;
    }
    EXPECT_EQ("getaddrinfo_0200", num, 1);
    freeaddrinfo(result);
    result = NULL;
}

/**
 * @tc.name      : getaddrinfo_0300
 * @tc.desc      : Each parameter is valid and can resolve the IP address of the host name
 *                 (hint.ai_flags =AI_NUMERICHOST).
 * @tc.level     : Level 0
 */
void getaddrinfo_0300(void)
{
    int ret = -1;
    int num = -1;
    struct addrinfo *result, hint;
    hint.ai_flags = AI_NUMERICHOST;
    hint.ai_family = AF_INET6;
    char buf[] = "fe80::bed5:4695:6cac:bef8";
    ret = getaddrinfo(buf, NULL, &hint, &result);
    EXPECT_EQ("getaddrinfo_0300", ret, GETADDRINFO_RESULT);
    if (result->ai_addr != NULL) {
        num = 1;
    } else {
        num = 2;
    }
    EXPECT_EQ("getaddrinfo_0300", num, 1);
    freeaddrinfo(result);
    result = NULL;
}

/**
 * @tc.name      : getaddrinfo_0400
 * @tc.desc      : Each parameter is valid and can resolve the IP address of the host name(hint =AI_V4MAPPED).
 * @tc.level     : Level 0
 */
void getaddrinfo_0400(void)
{
    int ret = -1;
    int num = -1;
    struct addrinfo *result, hint;
    hint.ai_flags = AI_V4MAPPED;
    hint.ai_family = AF_INET6;
    ret = getaddrinfo("127.0.0.1", NULL, &hint, &result);
    EXPECT_EQ("getaddrinfo_0400", ret, GETADDRINFO_RESULT);
    if (result->ai_addr != NULL) {
        num = 1;
    } else {
        num = 2;
    }
    EXPECT_EQ("getaddrinfo_0400", num, 1);
    freeaddrinfo(result);
    result = NULL;
}

/**
 * @tc.name      : getaddrinfo_0500
 * @tc.desc      : Each parameter is valid and can resolve the IP address of the host name(hint =AI_V4MAPPED).
 * @tc.level     : Level 0
 */
void getaddrinfo_0500(void)
{
    int ret = -1;
    int num = -1;
    struct addrinfo *result, hint;
    hint.ai_flags = AI_ALL;
    hint.ai_family = AF_UNSPEC;
    ret = getaddrinfo("127.0.0.1", NULL, &hint, &result);
    EXPECT_EQ("getaddrinfo_0500", ret, GETADDRINFO_RESULT);
    if (result->ai_addr != NULL) {
        num = 1;
    } else {
        num = 2;
    }
    EXPECT_EQ("getaddrinfo_0500", num, 1);
    freeaddrinfo(result);
    result = NULL;
}

/**
 * @tc.name      : getaddrinfo_0600
 * @tc.desc      : Each parameter is valid and can resolve the IP address of
 *                 the host name(hint.ai_flags =AI_ADDRCONFIG).
 * @tc.level     : Level 0
 */
void getaddrinfo_0600(void)
{
    int ret = -1;
    int num = -1;
    struct addrinfo *result, hint;
    hint.ai_flags = AI_ADDRCONFIG;
    hint.ai_family = AF_UNSPEC;
    ret = getaddrinfo("127.0.0.1", NULL, &hint, &result);
    EXPECT_EQ("getaddrinfo_0600", ret, GETADDRINFO_RESULT);
    if (result->ai_addr != NULL) {
        num = 1;
    } else {
        num = 2;
    }
    EXPECT_EQ("getaddrinfo_0600", num, 1);
    freeaddrinfo(result);
    result = NULL;
}

/**
 * @tc.name      : getaddrinfo_0700
 * @tc.desc      : Each parameter is valid and can resolve the IP address
 *                 of the host name(hint.ai_flags =AI_NUMERICSERV).
 * @tc.level     : Level 1
 */
void getaddrinfo_0700(void)
{
    int ret = -1;
    int num = -1;
    struct addrinfo *result, hint;
    hint.ai_flags = AI_NUMERICSERV;
    hint.ai_family = AF_UNSPEC;
    ret = getaddrinfo("127.0.0.1", NULL, &hint, &result);
    EXPECT_EQ("getaddrinfo_0700", ret, GETADDRINFO_RESULT);
    if (result->ai_addr != NULL) {
        num = 1;
    } else {
        num = 2;
    }
    EXPECT_EQ("getaddrinfo_0700", num, 1);
    freeaddrinfo(result);
    result = NULL;
}

/**
 * @tc.name      : getaddrinfo_0800
 * @tc.desc      : Each parameter is valid and can resolve the IP address of the host name(hint =NULL).
 * @tc.level     : Level 1
 */
void getaddrinfo_0800(void)
{
    int ret = -1;
    int num = -1;
    struct addrinfo *result;
    ret = getaddrinfo("127.0.0.1", NULL, NULL, &result);
    EXPECT_EQ("getaddrinfo_0800", ret, GETADDRINFO_RESULT);
    if (result->ai_addr != NULL) {
        num = 1;
    } else {
        num = 2;
    }
    EXPECT_EQ("getaddrinfo_0800", num, 1);
    freeaddrinfo(result);
    result = NULL;
}

/**
 * @tc.name      : getaddrinfo_0900
 * @tc.desc      : Invalid parameter, Hint’s ai_flags is invalid, can not resolve the IP address of the host name.
 * @tc.level     : Level 2
 */
void getaddrinfo_0900(void)
{
    int ret = -1;
    struct addrinfo *result, hint;
    hint.ai_flags = 0x4000;
    hint.ai_family = AF_UNSPEC;
    ret = getaddrinfo("127.0.0.1", NULL, &hint, &result);
    EXPECT_EQ("getaddrinfo_0900", ret, FLAGS_FIELD);
}

/**
 * @tc.name      : getaddrinfo_1000
 * @tc.desc      : Invalid parameter, Hint’s ai_family is invalid, can not resolve the IP address of the host name.
 * @tc.level     : Level 2
 */
void getaddrinfo_1000(void)
{
    int ret = -1;
    struct addrinfo *result, hint;
    hint.ai_flags = AI_ALL;
    hint.ai_family = PF_AX25;
    ret = getaddrinfo("127.0.0.1", NULL, &hint, &result);
    EXPECT_EQ("getaddrinfo_1000", ret, FAMILY_NOTSUPPORTED);
}

/**
 * @tc.name      : getaddrinfo_1100
 * @tc.desc      : The parameters are invalid,hint ai_flags is AI_NUMERICHOST,
 *                 host format is incorrect, can not resolve the host name Ip address.
 * @tc.level     : Level 2
 */
void getaddrinfo_1100(void)
{
    int ret = -1;
    struct addrinfo *result, hint;
    hint.ai_flags = AI_NUMERICHOST;
    hint.ai_family = AF_INET6;
    char one[300] = "fe80::bed5:4695:6cac:bef8:4695:6cac:bef8:4695:bef8:4695:6cac:bef8:4695";
    char two[] = ":fe80::bed5:4695:6cac:bef8:4695:6cac:bef8:4695:bef8:4695:6cac:bef8:4695";
    char three[] = ":fe80::bed5:4695:6cac:bef8:4695:6cac:bef8:4695:bef8:4695:6cac:bef8:4695";
    char four[] = ":fe80::bed5:4695:6cac:bef8:4695:6cac:bef8:4695:bef8:4695:6cac:bef8:4695";
    strcat(one, two);
    strcat(one, three);
    strcat(one, four);
    ret = getaddrinfo(one, NULL, &hint, &result);
    EXPECT_EQ("getaddrinfo_1100", ret, SERVICE_UNKNOEN);
}

/**
 * @tc.name      : getaddrinfo_1200
 * @tc.desc      : The parameter is invalid, host is NULL, SERV is NULL, and the IP address of the
 *                 host name can not be resolved.
 * @tc.level     : Level 2
 */
void getaddrinfo_1200(void)
{
    int ret = -1;
    struct addrinfo *result, hint;
    hint.ai_flags = AI_NUMERICHOST;
    hint.ai_family = AF_INET6;
    ret = getaddrinfo(NULL, NULL, &hint, &result);
    EXPECT_EQ("getaddrinfo_1200", ret, SERVICE_UNKNOEN);
}

/**
 * @tc.name      : getaddrinfo_1300
 * @tc.desc      : The parameter is invalid and the IP address of the host name can not be resolved.
 * @tc.level     : Level 2
 */
void getaddrinfo_1300(void)
{
    int ret = -1;
    struct addrinfo *result, hint;
    hint.ai_flags = AI_CANONNAME;
    hint.ai_family = AF_INET;
    hint.ai_protocol = IPPROTO_UDP;
    hint.ai_socktype = SOCK_STREAM;
    ret = getaddrinfo("127.0.0.1", NULL, &hint, &result);
    EXPECT_EQ("getaddrinfo_1300", ret, SOCKTYPE_NOTSUPPORTED);
}

/**
 * @tc.name      : getaddrinfo_1400
 * @tc.desc      : The parameter is invalid and the IP address of the host name can not be resolved.
 * @tc.level     : Level 2
 */
void getaddrinfo_1400(void)
{
    int ret = -1;
    struct addrinfo *result, hint;
    hint.ai_flags = AI_CANONNAME;
    hint.ai_family = AF_INET;
    hint.ai_protocol = IPPROTO_TCP;
    hint.ai_socktype = SOCK_DGRAM;
    ret = getaddrinfo("127.0.0.1", NULL, &hint, &result);
    EXPECT_EQ("getaddrinfo_1400", ret, SOCKTYPE_NOTSUPPORTED);
}

/**
 * @tc.name      : getaddrinfo_1500
 * @tc.desc      : The parameter is invalid and the IP address of the host name can not be resolved.
 * @tc.level     : Level 2
 */
void getaddrinfo_1500(void)
{
    int ret = -1;
    struct addrinfo *result, hint;
    hint.ai_flags = AI_CANONNAME;
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_RAW;
    ret = getaddrinfo("127.0.0.1", "2000", &hint, &result);
    EXPECT_EQ("getaddrinfo_1500", ret, SOCKTYPE_NOTSUPPORTED);
}

int main()
{
    getaddrinfo_0100();
    getaddrinfo_0200();
    getaddrinfo_0300();
    getaddrinfo_0400();
    getaddrinfo_0500();
    getaddrinfo_0600();
    getaddrinfo_0700();
    getaddrinfo_0800();
    getaddrinfo_0900();
    getaddrinfo_1000();
    getaddrinfo_1100();
    getaddrinfo_1200();
    getaddrinfo_1300();
    getaddrinfo_1400();
    getaddrinfo_1500();

    return t_status;
}