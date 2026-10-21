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
#include <unistd.h>
#include <errno.h>
#include "syscall.h"

int close_range(unsigned int first, unsigned int last, int flags)
{
#ifdef SYS_close_range
	int ret = __syscall(SYS_close_range, first, last, flags);
	if (ret != -ENOSYS) {
		return __syscall_ret(ret);
	}
#endif
	if (first > last || flags != 0) {
		errno = EINVAL;
		return -1;
	}
	int maxfd = getdtablesize();
	unsigned int maxfd_u = (unsigned int)maxfd;
	for (unsigned int i = first; i < maxfd_u && i <= last; i++) {
		__syscall(SYS_close, i);
	}
	return 0;
}
