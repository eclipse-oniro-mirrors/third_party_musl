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

#include <arpa/inet.h>
#include <ctype.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include "functionalext.h"

typedef void (*TEST_FUN)();
#define BUF_SIZE (100)
#define PORT 2288
#define WAIT() pthread_barrier_wait(&g_barrier)
static const char *g_cliMsg = "Hello, this is client";
static const char *g_localHost = "127.0.0.1";
static pthread_barrier_t g_barrier;

void *SampleServerTask()
{
    int *ret = (int *)malloc(sizeof(int));
    int rets = -1;
    int sListen = socket(AF_INET, SOCK_STREAM, 0);
    if (sListen == -1) {
        *ret = -1;
        return ret;
    }
    int flag = 1;
    rets = setsockopt(sListen, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int));
    struct sockaddr_in srvAddr = {0};
    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = inet_addr(g_localHost);
    srvAddr.sin_port = htons(PORT);
    rets = bind(sListen, (struct sockaddr *)&srvAddr, sizeof(srvAddr));
    if (rets != 0) {
        close(sListen);
        *ret = -1;
        return ret;
    }
    rets = listen(sListen, 2);
    if (rets != 0) {
        close(sListen);
        *ret = -1;
        return ret;
    }
    WAIT();
    struct sockaddr_in clnAddr = {0};
    socklen_t clnAddrLen = sizeof(clnAddr);
    int sClient = accept(sListen, (struct sockaddr *)&clnAddr, &clnAddrLen);
    static char buf[BUF_SIZE + 1] = {0};
    memset(buf, '\0', BUF_SIZE);
    rets = recv(sClient, buf, sizeof(buf), 0);
    close(sClient);
    close(sListen);
    return ret;
}

void *SampleClientTask()
{
    int clnFd = socket(AF_INET, SOCK_STREAM, 0);
    WAIT();
    struct sockaddr_in srvAddr = {0};
    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = inet_addr(g_localHost);
    srvAddr.sin_port = htons(PORT);
    int ret = connect(clnFd, (struct sockaddr *)&srvAddr, sizeof(srvAddr));
    EXPECT_EQ("send_0100", 0, ret);
    static char buf[BUF_SIZE + 1] = {0};
    memset(buf, '\0', BUF_SIZE);
    strcpy(buf, g_cliMsg);
    int sendRet = send(clnFd, buf, sizeof(buf), 0);
    EXPECT_EQ("send_0100", sendRet, sizeof(buf));
    close(clnFd);
    return NULL;
}

void *SampleServerNullTask()
{
    int *ret = (int *)malloc(sizeof(int));
    int rets = 0;
    int sListen = socket(AF_INET, SOCK_STREAM, 0);
    if (sListen == -1) {
        *ret = -1;
        return ret;
    }
    int flag = 1;
    rets = setsockopt(sListen, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int));
    struct sockaddr_in srvAddr = {0};
    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = inet_addr(g_localHost);
    srvAddr.sin_port = htons(PORT);
    rets = bind(sListen, (struct sockaddr *)&srvAddr, sizeof(srvAddr));
    if (rets != 0) {
        close(sListen);
        *ret = -1;
        return ret;
    }
    rets = listen(sListen, 2);
    if (rets != 0) {
        close(sListen);
        *ret = -1;
        return ret;
    }
    WAIT();
    struct sockaddr_in clnAddr = {0};
    socklen_t clnAddrLen = sizeof(clnAddr);
    accept(sListen, (struct sockaddr *)&clnAddr, &clnAddrLen);
    close(sListen);
    return ret;
}

void *SampleClientNullTask()
{
    int clnFd = socket(AF_INET, SOCK_STREAM, 0);
    WAIT();
    struct sockaddr_in srvAddr = {0};
    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = inet_addr(g_localHost);
    srvAddr.sin_port = htons(PORT);
    int ret = connect(clnFd, (struct sockaddr *)&srvAddr, sizeof(srvAddr));
    EXPECT_EQ("send_0200", 0, ret);
    static char buf[BUF_SIZE + 1] = {0};
    memset(buf, '\0', BUF_SIZE);
    strcpy(buf, g_cliMsg);
    int result = send(clnFd, NULL, sizeof(buf), 0);
    EXPECT_EQ("send_0200", result, -1);
    return NULL;
}

/*
 * @tc.name      : send_0100
 * @tc.desc      : Verify that the client sent successfully
 * @tc.level     : Level 0
 */
void send_0100(void)
{
    pthread_t srv;
    pthread_t cli;
    int ret = pthread_barrier_init(&g_barrier, 0, 2);
    EXPECT_EQ("send_0100", 0, ret);
    ret = pthread_create(&srv, NULL, SampleServerTask, NULL);
    EXPECT_EQ("send_0100", 0, ret);
    ret = pthread_create(&cli, NULL, SampleClientTask, NULL);
    EXPECT_EQ("send_0100", 0, ret);
    ret = pthread_join(cli, NULL);
    EXPECT_EQ("send_0100", 0, ret);
    ret = pthread_join(srv, NULL);
    EXPECT_EQ("send_0100", 0, ret);
    ret = pthread_barrier_destroy(&g_barrier);
    EXPECT_EQ("send_0100", 0, ret);
}

/*
 * @tc.name      : send_0200
 * @tc.desc      : Verify that client send failed when parameter is invalid
 * @tc.level     : Level 2
 */
void send_0200(void)
{
    pthread_t srv;
    pthread_t cli;
    int ret = pthread_barrier_init(&g_barrier, 0, 2);
    EXPECT_EQ("send_0200", 0, ret);
    ret = pthread_create(&srv, NULL, SampleServerNullTask, NULL);
    EXPECT_EQ("send_0200", 0, ret);
    ret = pthread_create(&cli, NULL, SampleClientNullTask, NULL);
    EXPECT_EQ("send_0200", 0, ret);
    ret = pthread_join(cli, NULL);
    EXPECT_EQ("send_0200", 0, ret);
    ret = pthread_join(srv, NULL);
    EXPECT_EQ("send_0200", 0, ret);
    ret = pthread_barrier_destroy(&g_barrier);
    EXPECT_EQ("send_0200", 0, ret);
}

TEST_FUN G_Fun_Array[] = {send_0100, send_0200};
int main()
{
    int num = sizeof(G_Fun_Array) / sizeof(TEST_FUN);
    for (int pos = 0; pos < num; ++pos) {
        G_Fun_Array[pos]();
    }

    return t_status;
}