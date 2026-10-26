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
#include <sys/wait.h>
#include <unistd.h>
#include "functionalext.h"

extern char __libc_single_threaded;

/**
 * @tc.name      : libc_single_threaded_0100
 * @tc.desc      : Verify __libc_single_threaded is 1 before any thread is created
 * @tc.level     : Level 0
 */
void libc_single_threaded_0100(void)
{
    EXPECT_EQ("libc_single_threaded_0100", (int)__libc_single_threaded, 1);
}

static void *trivial_worker(void *arg)
{
    (void)arg;
    return NULL;
}

/**
 * @tc.name      : libc_single_threaded_0200
 * @tc.desc      : Verify __libc_single_threaded becomes 0 after pthread_create
 * @tc.level     : Level 0
 */
void libc_single_threaded_0200(void)
{
    pthread_t tid;
    int createRet = pthread_create(&tid, NULL, trivial_worker, NULL);
    EXPECT_EQ("libc_single_threaded_0200", createRet, 0);
    EXPECT_EQ("libc_single_threaded_0200", pthread_join(tid, NULL), 0);
    EXPECT_EQ("libc_single_threaded_0200", (int)__libc_single_threaded, 0);
}

/**
 * @tc.name      : libc_single_threaded_0300
 * @tc.desc      : Verify __libc_single_threaded stays 0 after thread exit
 * @tc.level     : Level 0
 */
void libc_single_threaded_0300(void)
{
    EXPECT_EQ("libc_single_threaded_0300", (int)__libc_single_threaded, 0);
}

/**
 * @tc.name      : libc_single_threaded_0400
 * @tc.desc      : Verify value stays 0 after multiple threads created and joined
 * @tc.level     : Level 0
 */
void libc_single_threaded_0400(void)
{
    pthread_t tids[4];
    for (int i = 0; i < 4; i++) {
        EXPECT_EQ("libc_single_threaded_0400", pthread_create(&tids[i], NULL, trivial_worker, NULL), 0);
    }
    for (int i = 0; i < 4; i++) {
        EXPECT_EQ("libc_single_threaded_0400", pthread_join(tids[i], NULL), 0);
    }
    EXPECT_EQ("libc_single_threaded_0400", (int)__libc_single_threaded, 0);
}

static void *reader_worker(void *arg)
{
    *(int *)arg = (int)__libc_single_threaded;
    return NULL;
}

/**
 * @tc.name      : libc_single_threaded_0500
 * @tc.desc      : Multi-threaded: read __libc_single_threaded from a worker thread
 * @tc.level     : Level 0
 */
void libc_single_threaded_0500(void)
{
    int threadVal = -1;
    pthread_t tid;
    EXPECT_EQ("libc_single_threaded_0500", pthread_create(&tid, NULL, reader_worker, &threadVal), 0);
    EXPECT_EQ("libc_single_threaded_0500", pthread_join(tid, NULL), 0);
    EXPECT_EQ("libc_single_threaded_0500", threadVal, 0);
}

/**
 * @tc.name      : libc_single_threaded_0600
 * @tc.desc      : Fork: child process should see __libc_single_threaded == 0 (inherited)
 * @tc.level     : Level 2
 */
void libc_single_threaded_0600(void)
{
    pid_t child = fork();
    EXPECT_TRUE("libc_single_threaded_0600", child >= 0);
    if (child == 0) {
        int val = (int)__libc_single_threaded;
        _exit(val);
    }
    int status;
    waitpid(child, &status, 0);
    EXPECT_TRUE("libc_single_threaded_0600", WIFEXITED(status));
    EXPECT_EQ("libc_single_threaded_0600", WEXITSTATUS(status), 0);
}

int main(void)
{
    libc_single_threaded_0100();
    libc_single_threaded_0200();
    libc_single_threaded_0300();
    libc_single_threaded_0400();
    libc_single_threaded_0500();
    libc_single_threaded_0600();
    return t_status;
}
