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
#include <limits.h>

#ifndef NGROUPS_MAX
#define NGROUPS_MAX 16
#endif

int group_member(gid_t gid)
{
	int n = 0;
	int cap = NGROUPS_MAX;
	gid_t *list;

	do {
		cap <<= 1;
		list = __builtin_alloca(cap * sizeof *list);
		n = getgroups(cap, list);
	} while (n == cap);

	while (n > 0) {
		if (list[--n] == gid) {
			return 1;
		}
	}
	return 0;
}
