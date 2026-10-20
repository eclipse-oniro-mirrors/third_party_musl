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

#define _GNU_SOURCE
#include <sys/uio.h>
#include "io_guard_write_common.h"

/* D025-D036: this path only, 12 policy/token combinations. */
static long invoke(struct fixture *f)
{
    struct iovec iov = { .iov_base = &f->byte, .iov_len = 1 };
    return writev(f->fd, &iov, 1);
}

int main(void)
{
    return io_guard_run_write_test(WRITEV, invoke);
}
