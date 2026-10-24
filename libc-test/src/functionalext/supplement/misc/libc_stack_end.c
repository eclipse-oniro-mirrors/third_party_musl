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
#include <stddef.h>
#include "functionalext.h"

extern void *__libc_stack_end;

/**
 * @tc.name      : libc_stack_end_0100
 * @tc.desc      : Verify __libc_stack_end is non-NULL after process startup
 * @tc.level     : Level 0
 */
void libc_stack_end_0100(void)
{
    EXPECT_PTRNE("libc_stack_end_0100", __libc_stack_end, NULL);
}

/**
 * @tc.name      : libc_stack_end_0200
 * @tc.desc      : Verify __libc_stack_end is consistent across multiple reads
 * @tc.level     : Level 0
 */
void libc_stack_end_0200(void)
{
    void *first = __libc_stack_end;
    void *second = __libc_stack_end;
    EXPECT_PTREQ("libc_stack_end_0200", first, second);
}

/**
 * @tc.name      : libc_stack_end_0300
 * @tc.desc      : Verify __libc_stack_end is above current stack frame
 * @tc.level     : Level 0
 */
void libc_stack_end_0300(void)
{
    int localVar = 42;
    void *stackPtr = &localVar;
    void *endPtr = __libc_stack_end;
    EXPECT_PTRNE("libc_stack_end_0300", endPtr, NULL);
    EXPECT_TRUE("libc_stack_end_0300", endPtr != stackPtr);
}

static void *stack_end_worker(void *arg)
{
    *(void **)arg = __libc_stack_end;
    return NULL;
}

/**
 * @tc.name      : libc_stack_end_0400
 * @tc.desc      : Multi-threaded: __libc_stack_end from a worker thread points to main thread stack
 * @tc.level     : Level 0
 */
void libc_stack_end_0400(void)
{
    void *mainEnd = __libc_stack_end;
    void *threadEnd = NULL;
    pthread_t tid;
    EXPECT_EQ("libc_stack_end_0400", pthread_create(&tid, NULL, stack_end_worker, &threadEnd), 0);
    EXPECT_EQ("libc_stack_end_0400", pthread_join(tid, NULL), 0);
    EXPECT_PTRNE("libc_stack_end_0400", threadEnd, NULL);
    EXPECT_PTREQ("libc_stack_end_0400", threadEnd, mainEnd);
}

int main(void)
{
    libc_stack_end_0100();
    libc_stack_end_0200();
    libc_stack_end_0300();
    libc_stack_end_0400();
    return t_status;
}
