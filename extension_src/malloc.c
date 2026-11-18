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
#include <string.h>
#include <errno.h>
#include "functionalext.h"

/**
 * @tc.name      : malloc_0100
 * @tc.desc      : Allocate a normal size block and read/write it
 * @tc.level     : Level 0
 */
void malloc_0100(void)
{
    char *mem = (char *)malloc(100 * sizeof(char));
    EXPECT_TRUE("malloc_0100", mem != NULL);
    if (mem == NULL) {
        return;
    }
    memset(mem, 'A', 100);
    EXPECT_EQ("malloc_0100", mem[0], 'A');
    EXPECT_EQ("malloc_0100", mem[99], 'A');
    free(mem);
}

/**
 * @tc.name      : malloc_0200
 * @tc.desc      : Allocate a single byte block
 * @tc.level     : Level 0
 */
void malloc_0200(void)
{
    char *mem = (char *)malloc(1);
    EXPECT_TRUE("malloc_0200", mem != NULL);
    if (mem == NULL) {
        return;
    }
    mem[0] = 0x5A;
    EXPECT_EQ("malloc_0200", mem[0], 0x5A);
    free(mem);
}

/**
 * @tc.name      : malloc_0300
 * @tc.desc      : Multiple allocations return distinct non-overlapping pointers
 * @tc.level     : Level 1
 */
void malloc_0300(void)
{
    char *mem1 = (char *)malloc(64);
    char *mem2 = (char *)malloc(64);
    char *mem3 = (char *)malloc(64);
    EXPECT_TRUE("malloc_0300", mem1 != NULL && mem2 != NULL && mem3 != NULL);
    EXPECT_PTRNE("malloc_0300", mem1, mem2);
    EXPECT_PTRNE("malloc_0300", mem2, mem3);
    EXPECT_PTRNE("malloc_0300", mem1, mem3);
    mem1[0] = 1;
    mem2[0] = 2;
    mem3[0] = 3;
    EXPECT_EQ("malloc_0300", mem1[0], 1);
    EXPECT_EQ("malloc_0300", mem2[0], 2);
    EXPECT_EQ("malloc_0300", mem3[0], 3);
    free(mem1);
    free(mem2);
    free(mem3);
}

/**
 * @tc.name      : malloc_0400
 * @tc.desc      : Returned pointer is suitably aligned for any type
 * @tc.level     : Level 1
 */
void malloc_0400(void)
{
    void *mem = malloc(24);
    EXPECT_TRUE("malloc_0400", mem != NULL);
    if (mem == NULL) {
        return;
    }
    EXPECT_EQ("malloc_0400", (int)((uintptr_t)mem % sizeof(void *)), 0);
    free(mem);
}

/**
 * @tc.name      : malloc_0500
 * @tc.desc      : Allocate a large block (1MB) and touch both ends
 * @tc.level     : Level 1
 */
void malloc_0500(void)
{
    size_t largeSize = 1024 * 1024;
    char *mem = (char *)malloc(largeSize);
    EXPECT_TRUE("malloc_0500", mem != NULL);
    if (mem == NULL) {
        return;
    }
    memset(mem, 'B', largeSize);
    EXPECT_EQ("malloc_0500", mem[0], 'B');
    EXPECT_EQ("malloc_0500", mem[largeSize - 1], 'B');
    free(mem);
}

/**
 * @tc.name      : malloc_0600
 * @tc.desc      : Allocate with SIZE_MAX, should fail and set errno to ENOMEM
 * @tc.level     : Level 2
 */
void malloc_0600(void)
{
    errno = 0;
    char *mem = (char *)malloc(SIZE_MAX);
    EXPECT_PTREQ("malloc_0600", mem, NULL);
    EXPECT_EQ("malloc_0600", errno, ENOMEM);
}

/**
 * @tc.name      : malloc_0700
 * @tc.desc      : Allocate zero bytes and free, free(NULL) is a valid no-op
 * @tc.level     : Level 2
 */
void malloc_0700(void)
{
    /* The standard allows either NULL or a unique pointer for size 0 */
    char *mem = (char *)malloc(0);
    free(mem);

    /* free(NULL) shall do nothing and shall not crash */
    free(NULL);
}

int main(void)
{
    malloc_0100();
    malloc_0200();
    malloc_0300();
    malloc_0400();
    malloc_0500();
    malloc_0600();
    malloc_0700();

    return t_status;
}
