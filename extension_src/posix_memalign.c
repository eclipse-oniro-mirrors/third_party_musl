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

#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include "functionalext.h"

/**
 * @tc.name      : posix_memalign_0100
 * @tc.desc      : Allocate a 16-byte aligned block
 * @tc.level     : Level 0
 */
void posix_memalign_0100(void)
{
    char *mem = NULL;
    int ret = posix_memalign((void **)&mem, 16, 100);
    EXPECT_EQ("posix_memalign_0100", ret, 0);
    EXPECT_TRUE("posix_memalign_0100", mem != NULL);
    if (mem == NULL) {
        return;
    }
    EXPECT_EQ("posix_memalign_0100", (int)((uintptr_t)mem % 16), 0);
    mem[0] = 'P';
    EXPECT_EQ("posix_memalign_0100", mem[0], 'P');
    free(mem);
}

/**
 * @tc.name      : posix_memalign_0200
 * @tc.desc      : Allocate a 64-byte aligned block
 * @tc.level     : Level 0
 */
void posix_memalign_0200(void)
{
    char *mem = NULL;
    int ret = posix_memalign((void **)&mem, 64, 512);
    EXPECT_EQ("posix_memalign_0200", ret, 0);
    EXPECT_TRUE("posix_memalign_0200", mem != NULL);
    if (mem == NULL) {
        return;
    }
    EXPECT_EQ("posix_memalign_0200", (int)((uintptr_t)mem % 64), 0);
    free(mem);
}

/**
 * @tc.name      : posix_memalign_0300
 * @tc.desc      : Allocate a page (4096-byte) aligned block
 * @tc.level     : Level 1
 */
void posix_memalign_0300(void)
{
    char *mem = NULL;
    int ret = posix_memalign((void **)&mem, 4096, 8192);
    EXPECT_EQ("posix_memalign_0300", ret, 0);
    EXPECT_TRUE("posix_memalign_0300", mem != NULL);
    if (mem == NULL) {
        return;
    }
    EXPECT_EQ("posix_memalign_0300", (int)((uintptr_t)mem % 4096), 0);
    mem[8191] = 'Q';
    EXPECT_EQ("posix_memalign_0300", mem[8191], 'Q');
    free(mem);
}

/**
 * @tc.name      : posix_memalign_0400
 * @tc.desc      : Alignment equal to sizeof(void*) is the minimum allowed
 * @tc.level     : Level 1
 */
void posix_memalign_0400(void)
{
    char *mem = NULL;
    int ret = posix_memalign((void **)&mem, 8, 32);
    EXPECT_EQ("posix_memalign_0400", ret, 0);
    EXPECT_TRUE("posix_memalign_0400", mem != NULL);
    if (mem != NULL) {
        free(mem);
    }
}

/**
 * @tc.name      : posix_memalign_0500
 * @tc.desc      : Allocate with zero length, shall succeed or fail cleanly
 * @tc.level     : Level 2
 */
void posix_memalign_0500(void)
{
    char *mem = NULL;
    int ret = posix_memalign((void **)&mem, 16, 0);
    EXPECT_EQ("posix_memalign_0500", ret, 0);
    if (ret == 0 && mem != NULL) {
        EXPECT_EQ("posix_memalign_0500", (int)((uintptr_t)mem % 16), 0);
        free(mem);
    }
}

/**
 * @tc.name      : posix_memalign_0600
 * @tc.desc      : Alignment is not a power of two, should return EINVAL
 * @tc.level     : Level 2
 */
void posix_memalign_0600(void)
{
    char *mem = NULL;
    int ret = posix_memalign((void **)&mem, 24, 100);
    EXPECT_EQ("posix_memalign_0600", ret, EINVAL);
}

/**
 * @tc.name      : posix_memalign_0700
 * @tc.desc      : Alignment is smaller than sizeof(void*), should return EINVAL
 * @tc.level     : Level 2
 */
void posix_memalign_0700(void)
{
    char *mem = NULL;
    int ret = posix_memalign((void **)&mem, 2, 100);
    EXPECT_EQ("posix_memalign_0700", ret, EINVAL);
}

/**
 * @tc.name      : posix_memalign_0800
 * @tc.desc      : Length too large for allocation, should return ENOMEM
 * @tc.level     : Level 2
 */
void posix_memalign_0800(void)
{
    char *mem = NULL;
    int ret = posix_memalign((void **)&mem, 4096, SIZE_MAX - 100);
    EXPECT_EQ("posix_memalign_0800", ret, ENOMEM);
}

int main(void)
{
    posix_memalign_0100();
    posix_memalign_0200();
    posix_memalign_0300();
    posix_memalign_0400();
    posix_memalign_0500();
    posix_memalign_0600();
    posix_memalign_0700();
    posix_memalign_0800();

    return t_status;
}
