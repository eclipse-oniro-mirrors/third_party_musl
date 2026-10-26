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
#include <stdlib.h>
#include <unistd.h>

int group_member(gid_t gid)
{
	int n = getgroups(0, NULL);
	if (n <= 0) {
		return 0;
	}

	gid_t list[n];
	n = getgroups(n, list);
	if (n < 0) {
		return 0;
	}

	while (n > 0) {
		if (list[--n] == gid) {
			return 1;
		}
	}
	return 0;
}
