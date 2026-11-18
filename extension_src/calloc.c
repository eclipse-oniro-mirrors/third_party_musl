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
 * @tc.name      : calloc_0100
 * @tc.desc      : Allocate nmemb*size array, memory is zero-initialized
 * @tc.level     : Level 0
 */
void calloc_0100(void)
{
    char *mem = (char *)calloc(10, 100);
    EXPECT_TRUE("calloc_0100", mem != NULL);
    if (mem == NULL) {
        return;
    }
    for (int i = 0; i < 1000; i++) {
        if (mem[i] != 0) {
            EXPECT_EQ("calloc_0100", mem[i], 0);
            break;
        }
    }
    mem[999] = 'C';
    EXPECT_EQ("calloc_0100", mem[999], 'C');
    free(mem);
}

/**
 * @tc.name      : calloc_0200
 * @tc.desc      : Allocate a single byte element
 * @tc.level     : Level 0
 */
void calloc_0200(void)
{
    char *mem = (char *)calloc(1, 1);
    EXPECT_TRUE("calloc_0200", mem != NULL);
    if (mem == NULL) {
        return;
    }
    EXPECT_EQ("calloc_0200", mem[0], 0);
    free(mem);
}

/**
 * @tc.name      : calloc_0300
 * @tc.desc      : Large allocation (1MB) is zero-initialized
 * @tc.level     : Level 1
 */
void calloc_0300(void)
{
    size_t nmemb = 1024;
    size_t size = 1024;
    char *mem = (char *)calloc(nmemb, size);
    EXPECT_TRUE("calloc_0300", mem != NULL);
    if (mem == NULL) {
        return;
    }
    EXPECT_EQ("calloc_0300", mem[0], 0);
    EXPECT_EQ("calloc_0300", mem[nmemb * size / 2], 0);
    EXPECT_EQ("calloc_0300", mem[nmemb * size - 1], 0);
    free(mem);
}

/**
 * @tc.name      : calloc_0400
 * @tc.desc      : Two calloc results are independent blocks
 * @tc.level     : Level 1
 */
void calloc_0400(void)
{
    char *mem1 = (char *)calloc(32, 8);
    char *mem2 = (char *)calloc(32, 8);
    EXPECT_TRUE("calloc_0400", mem1 != NULL && mem2 != NULL);
    EXPECT_PTRNE("calloc_0400", mem1, mem2);
    mem1[0] = 7;
    EXPECT_EQ("calloc_0400", mem2[0], 0);
    free(mem1);
    free(mem2);
}

/**
 * @tc.name      : calloc_0500
 * @tc.desc      : nmemb*size overflows SIZE_MAX, should fail with ENOMEM
 * @tc.level     : Level 2
 */
void calloc_0500(void)
{
    errno = 0;
    char *mem = (char *)calloc(SIZE_MAX / 2, 3);
    EXPECT_PTREQ("calloc_0500", mem, NULL);
    EXPECT_EQ("calloc_0500", errno, ENOMEM);
}

/**
 * @tc.name      : calloc_0600
 * @tc.desc      : Allocate with zero nmemb/size, shall not crash
 * @tc.level     : Level 2
 */
void calloc_0600(void)
{
    /* The standard allows either NULL or a unique pointer */
    char *mem1 = (char *)calloc(0, 10);
    char *mem2 = (char *)calloc(10, 0);
    char *mem3 = (char *)calloc(0, 0);
    free(mem1);
    free(mem2);
    free(mem3);
}

int main(void)
{
    calloc_0100();
    calloc_0200();
    calloc_0300();
    calloc_0400();
    calloc_0500();
    calloc_0600();

    return t_status;
}
