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
 * @tc.name      : realloc_0100
 * @tc.desc      : Realloc with NULL pointer behaves like malloc
 * @tc.level     : Level 0
 */
void realloc_0100(void)
{
    char *mem = (char *)realloc(NULL, 100);
    EXPECT_TRUE("realloc_0100", mem != NULL);
    if (mem == NULL) {
        return;
    }
    memset(mem, 'R', 100);
    EXPECT_EQ("realloc_0100", mem[99], 'R');
    free(mem);
}

/**
 * @tc.name      : realloc_0200
 * @tc.desc      : Grow a block, old contents are preserved
 * @tc.level     : Level 0
 */
void realloc_0200(void)
{
    const char data[] = "0123456789";
    char *mem = (char *)malloc(10);
    EXPECT_TRUE("realloc_0200", mem != NULL);
    if (mem == NULL) {
        return;
    }
    memcpy(mem, data, 10);

    char *newMem = (char *)realloc(mem, 1000);
    EXPECT_TRUE("realloc_0200", newMem != NULL);
    if (newMem == NULL) {
        free(mem);
        return;
    }
    EXPECT_EQ("realloc_0200", memcmp(newMem, data, 10), 0);
    free(newMem);
}

/**
 * @tc.name      : realloc_0300
 * @tc.desc      : Shrink a block, prefix contents are preserved
 * @tc.level     : Level 1
 */
void realloc_0300(void)
{
    char *mem = (char *)malloc(200);
    EXPECT_TRUE("realloc_0300", mem != NULL);
    if (mem == NULL) {
        return;
    }
    memset(mem, 'S', 200);

    char *newMem = (char *)realloc(mem, 20);
    EXPECT_TRUE("realloc_0300", newMem != NULL);
    if (newMem == NULL) {
        free(mem);
        return;
    }
    for (int i = 0; i < 20; i++) {
        if (newMem[i] != 'S') {
            EXPECT_EQ("realloc_0300", newMem[i], 'S');
            break;
        }
    }
    free(newMem);
}

/**
 * @tc.name      : realloc_0400
 * @tc.desc      : Repeated realloc growth, data survives each step
 * @tc.level     : Level 1
 */
void realloc_0400(void)
{
    char *mem = (char *)malloc(16);
    EXPECT_TRUE("realloc_0400", mem != NULL);
    if (mem == NULL) {
        return;
    }
    memset(mem, 'G', 16);

    for (int size = 32; size <= 4096; size *= 2) {
        char *newMem = (char *)realloc(mem, size);
        EXPECT_TRUE("realloc_0400", newMem != NULL);
        if (newMem == NULL) {
            free(mem);
            return;
        }
        mem = newMem;
        EXPECT_EQ("realloc_0400", mem[0], 'G');
        EXPECT_EQ("realloc_0400", mem[15], 'G');
        mem[size - 1] = 'G';
    }
    free(mem);
}

/**
 * @tc.name      : realloc_0500
 * @tc.desc      : Grow a block to a large size (1MB)
 * @tc.level     : Level 1
 */
void realloc_0500(void)
{
    const char data[] = "realloc_large_test";
    char *mem = (char *)malloc(32);
    EXPECT_TRUE("realloc_0500", mem != NULL);
    if (mem == NULL) {
        return;
    }
    memcpy(mem, data, sizeof(data));

    char *newMem = (char *)realloc(mem, 1024 * 1024);
    EXPECT_TRUE("realloc_0500", newMem != NULL);
    if (newMem == NULL) {
        free(mem);
        return;
    }
    EXPECT_EQ("realloc_0500", memcmp(newMem, data, sizeof(data)), 0);
    memset(newMem, 'L', 1024 * 1024);
    EXPECT_EQ("realloc_0500", newMem[1024 * 1024 - 1], 'L');
    free(newMem);
}

/**
 * @tc.name      : realloc_0600
 * @tc.desc      : Realloc to zero size, the block may be freed or shrunk
 * @tc.level     : Level 2
 */
void realloc_0600(void)
{
    char *mem = (char *)malloc(64);
    EXPECT_TRUE("realloc_0600", mem != NULL);
    if (mem == NULL) {
        return;
    }
    /* The standard leaves the result implementation-defined:
     * either NULL (block freed) or a shrinked block. Both are valid,
     * the pointer must simply not be reused without a new assignment. */
    char *newMem = (char *)realloc(mem, 0);
    if (newMem == NULL) {
        /* block was freed by realloc */
    } else {
        free(newMem);
    }
}

/**
 * @tc.name      : realloc_0700
 * @tc.desc      : Realloc NULL pointer with SIZE_MAX, should fail with ENOMEM
 * @tc.level     : Level 2
 */
void realloc_0700(void)
{
    errno = 0;
    char *mem = (char *)realloc(NULL, SIZE_MAX);
    EXPECT_PTREQ("realloc_0700", mem, NULL);
    EXPECT_EQ("realloc_0700", errno, ENOMEM);
}

int main(void)
{
    realloc_0100();
    realloc_0200();
    realloc_0300();
    realloc_0400();
    realloc_0500();
    realloc_0600();
    realloc_0700();

    return t_status;
}
