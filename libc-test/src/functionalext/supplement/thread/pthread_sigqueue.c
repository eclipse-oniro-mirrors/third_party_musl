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

#include <pthread.h>
#include <signal.h>
#include "functionalext.h"

#define SIG_MAGIC_VAL 0x5678

static volatile sig_atomic_t g_sigCaught = 0;
static volatile sig_atomic_t g_sigPayload = 0;

static void sigusr1_hdl(int sig, siginfo_t *si, void *ctx)
{
    (void)sig;
    (void)ctx;
    g_sigCaught = 1;
    if (si) {
        g_sigPayload = (sig_atomic_t)si->si_value.sival_int;
    }
}

void pthread_sigqueue_0100(void)
{
    struct sigaction sa = {0};
    sa.sa_sigaction = sigusr1_hdl;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGUSR1, &sa, NULL);

    g_sigCaught = 0;
    g_sigPayload = 0;
    union sigval sv = {0};
    sv.sival_int = SIG_MAGIC_VAL;
    int rv = pthread_sigqueue(pthread_self(), SIGUSR1, sv);
    EXPECT_EQ("pthread_sigqueue_0100", rv, 0);
    EXPECT_EQ("pthread_sigqueue_0100", (int)g_sigCaught, 1);
    EXPECT_EQ("pthread_sigqueue_0100", (int)g_sigPayload, SIG_MAGIC_VAL);
}

void pthread_sigqueue_0200(void)
{
    union sigval sv = {0};
    sv.sival_int = 0;
    int rv = pthread_sigqueue(pthread_self(), 0, sv);
    EXPECT_EQ("pthread_sigqueue_0200", rv, 0);
}

static volatile int g_threadReady = 0;
static void *worker_fn(void *arg)
{
    pthread_t *mainTid = (pthread_t *)arg;
    g_threadReady = 1;
    union sigval sv = {0};
    sv.sival_int = SIG_MAGIC_VAL;
    pthread_sigqueue(*mainTid, SIGUSR1, sv);
    return NULL;
}

void pthread_sigqueue_0300(void)
{
    struct sigaction sa = {0};
    sa.sa_sigaction = sigusr1_hdl;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGUSR1, &sa, NULL);

    g_sigCaught = 0;
    g_sigPayload = 0;
    g_threadReady = 0;
    pthread_t mainTid = pthread_self();
    pthread_t worker;
    int createRet = pthread_create(&worker, NULL, worker_fn, &mainTid);
    EXPECT_EQ("pthread_sigqueue_0300", createRet, 0);
    pthread_join(worker, NULL);
    EXPECT_EQ("pthread_sigqueue_0300", (int)g_sigCaught, 1);
    EXPECT_EQ("pthread_sigqueue_0300", (int)g_sigPayload, SIG_MAGIC_VAL);
}

void pthread_sigqueue_0400(void)
{
    struct sigaction sa = {0};
    sa.sa_sigaction = sigusr1_hdl;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGUSR1, &sa, NULL);

    g_sigCaught = 0;
    g_sigPayload = 0;
    union sigval sv = {0};
    sv.sival_int = 0xABCD;
    int rv = pthread_sigqueue(pthread_self(), SIGUSR1, sv);
    EXPECT_EQ("pthread_sigqueue_0400", rv, 0);
    EXPECT_EQ("pthread_sigqueue_0400", (int)g_sigCaught, 1);
    EXPECT_EQ("pthread_sigqueue_0400", (int)g_sigPayload, 0xABCD);
}

static void *sigqueue_target(void *arg)
{
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR2);
    pthread_sigmask(SIG_BLOCK, &set, NULL);
    *(volatile int *)arg = 1;
    sleep(1);
    return NULL;
}

void pthread_sigqueue_0500(void)
{
    volatile int ready = 0;
    pthread_t tid;
    int createRet = pthread_create(&tid, NULL,
        (void *(*)(void *))sigqueue_target, (void *)&ready);
    EXPECT_EQ("pthread_sigqueue_0500", createRet, 0);
    while (!ready) {
        usleep(10000);
    }
    union sigval sv = {0};
    sv.sival_int = 0;
    int rv = pthread_sigqueue(tid, SIGUSR2, sv);
    EXPECT_EQ("pthread_sigqueue_0500", rv, 0);
    pthread_join(tid, NULL);
}

static pthread_t g_targetThread;
static volatile int g_targetReady2 = 0;

static void *target_fn(void *arg)
{
    (void)arg;
    g_targetReady2 = 1;
    sleep(2);
    return NULL;
}

static void *sender_fn(void *arg)
{
    int val = *(int *)arg;
    union sigval sv = {0};
    sv.sival_int = val;
    pthread_sigqueue(g_targetThread, SIGUSR1, sv);
    return NULL;
}

void pthread_sigqueue_0600(void)
{
    struct sigaction sa = {0};
    sa.sa_sigaction = sigusr1_hdl;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGUSR1, &sa, NULL);

    g_sigCaught = 0;
    g_targetReady2 = 0;
    EXPECT_EQ("pthread_sigqueue_0600", pthread_create(&g_targetThread, NULL, target_fn, NULL), 0);
    while (!g_targetReady2) {
        usleep(10000);
    }

    pthread_t senders[4];
    int vals[4] = {0x1111, 0x2222, 0x3333, 0x4444};
    for (int i = 0; i < 4; i++) {
        EXPECT_EQ("pthread_sigqueue_0600", pthread_create(&senders[i], NULL, sender_fn, &vals[i]), 0);
    }
    for (int i = 0; i < 4; i++) {
        pthread_join(senders[i], NULL);
    }
    pthread_join(g_targetThread, NULL);
    EXPECT_TRUE("pthread_sigqueue_0600", g_sigCaught == 1);
}

int main(void)
{
    pthread_sigqueue_0100();
    pthread_sigqueue_0200();
    pthread_sigqueue_0300();
    pthread_sigqueue_0400();
    pthread_sigqueue_0500();
    pthread_sigqueue_0600();
    return t_status;
}
