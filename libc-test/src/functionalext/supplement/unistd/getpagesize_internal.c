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
#include <unistd.h>
#include "functionalext.h"

/**
 * @tc.name      : getpagesize_internal_0100
 * @tc.desc      : Verify __getpagesize returns a positive value
 * @tc.level     : Level 0
 */
void getpagesize_internal_0100(void)
{
    int psize = __getpagesize();
    EXPECT_TRUE("getpagesize_internal_0100", psize > 0);
}

/**
 * @tc.name      : getpagesize_internal_0200
 * @tc.desc      : Verify __getpagesize matches getpagesize
 * @tc.level     : Level 0
 */
void getpagesize_internal_0200(void)
{
    int internalPsize = __getpagesize();
    int publicPsize = getpagesize();
    EXPECT_EQ("getpagesize_internal_0200", internalPsize, publicPsize);
}

/**
 * @tc.name      : getpagesize_internal_0300
 * @tc.desc      : Verify __getpagesize returns a power-of-two value
 * @tc.level     : Level 0
 */
void getpagesize_internal_0300(void)
{
    int sz = __getpagesize();
    EXPECT_TRUE("getpagesize_internal_0300", sz > 0);
    int isPow2 = (sz & (sz - 1)) == 0;
    EXPECT_TRUE("getpagesize_internal_0300", isPow2);
}

/**
 * @tc.name      : getpagesize_internal_0400
 * @tc.desc      : Verify __getpagesize matches sysconf(_SC_PAGESIZE)
 * @tc.level     : Level 0
 */
void getpagesize_internal_0400(void)
{
    int psize = __getpagesize();
    long scPagesize = sysconf(_SC_PAGESIZE);
    EXPECT_EQ("getpagesize_internal_0400", (long)psize, scPagesize);
}

/**
 * @tc.name      : getpagesize_internal_0500
 * @tc.desc      : Verify value is a known valid page size
 * @tc.level     : Level 0
 */
void getpagesize_internal_0500(void)
{
    int sz = __getpagesize();
    int valid = (sz == 4096) || (sz == 8192) || (sz == 16384) || (sz == 65536);
    EXPECT_TRUE("getpagesize_internal_0500", valid);
}

static void *gps_worker(void *arg)
{
    *(int *)arg = __getpagesize();
    return NULL;
}

/**
 * @tc.name      : getpagesize_internal_0600
 * @tc.desc      : Multi-threaded: concurrent __getpagesize returns same value
 * @tc.level     : Level 0
 */
void getpagesize_internal_0600(void)
{
    int mainVal = __getpagesize();
    pthread_t tid;
    int threadVal = 0;
    EXPECT_EQ("getpagesize_internal_0600", pthread_create(&tid, NULL, gps_worker, &threadVal), 0);
    pthread_join(tid, NULL);
    EXPECT_EQ("getpagesize_internal_0600", mainVal, threadVal);
}

int main(void)
{
    getpagesize_internal_0100();
    getpagesize_internal_0200();
    getpagesize_internal_0300();
    getpagesize_internal_0400();
    getpagesize_internal_0500();
    getpagesize_internal_0600();
    return t_status;
}
