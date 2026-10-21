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
#include <pwd.h>
#include <stdio.h>
#include <errno.h>
#include "pwf.h"

#define FIX(x) (pw->pw_##x = pw->pw_##x-line+buf)

int fgetpwent_r(FILE *f, struct passwd *pw, char *buf, size_t size,
	struct passwd **res)
{
	char *line = 0;
	size_t len = 0;
	struct passwd *tmp;
	int rv = 0;

	rv = __getpwent_a(f, pw, &line, &len, &tmp);
	if (!tmp) {
		*res = 0;
		if (rv == 0) {
			rv = ENOENT;
		}
		goto done;
	}
	if (size < len) {
		*res = 0;
		rv = ERANGE;
		goto done;
	}
	memcpy(buf, line, len);
	FIX(name);
	FIX(passwd);
	FIX(gecos);
	FIX(dir);
	FIX(shell);
	*res = pw;
done:
	free(line);
	if (rv) {
		errno = rv;
	}
	return rv;
}
