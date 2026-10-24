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

#include "pwf.h"
#include "lock.h"

static FILE *f;
static char *line, **mem;
static struct group gr;
static int grent_lock[1];

#define FIX(x) (gr->gr_##x = gr->gr_##x-rline+buf)

void setgrent()
{
	if (f) fclose(f);
	f = 0;
}

weak_alias(setgrent, endgrent);

struct group *getgrent()
{
	struct group *res;
	size_t size=0, nmem=0;
	if (!f) f = fopen("/etc/group", "rbe");
	if (!f) return 0;
	__getgrent_a(f, &gr, &line, &size, &mem, &nmem, &res);
	return res;
}

struct group *getgrgid(gid_t gid)
{
	struct group *res;
	size_t size=0, nmem=0;
	__getgr_a(0, gid, &gr, &line, &size, &mem, &nmem, &res);
	return res;
}

struct group *getgrnam(const char *name)
{
	struct group *res;
	size_t size=0, nmem=0;
	__getgr_a(name, 0, &gr, &line, &size, &mem, &nmem, &res);
	return res;
}

static int getgrent_fill_buf(struct group *gr, char *buf, size_t size,
	char *rline, size_t len, char **rmem, size_t nmem)
{
	size_t i;

	if (size < len + (nmem+1)*sizeof(char *) + 32) {
		return ERANGE;
	}

	buf += (16-(uintptr_t)buf)%16;
	gr->gr_mem = (void *)buf;
	buf += (nmem+1)*sizeof(char *);
	memcpy(buf, rline, len);
	FIX(name);
	FIX(passwd);
	for (i=0; rmem[i]; i++) {
		gr->gr_mem[i] = rmem[i]-rline+buf;
	}
	gr->gr_mem[i] = 0;
	return 0;
}

int getgrent_r(struct group *gr, char *buf, size_t size, struct group **res)
{
	char *rline = 0;
	size_t len = 0;
	char **rmem = 0;
	size_t nmem = 0;
	struct group *tmp;
	int rv = 0;

	LOCK(grent_lock);

	if (!f) {
		f = fopen("/etc/group", "rbe");
		if (!f) {
			rv = ENOENT;
			*res = 0;
			goto done;
		}
	}

	long orig_offset = ftell(f);
	rv = __getgrent_a(f, gr, &rline, &len, &rmem, &nmem, &tmp);
	if (!tmp) {
		*res = 0;
		if (rv == 0) {
			rv = ENOENT;
		}
		goto done;
	}
	rv = getgrent_fill_buf(gr, buf, size, rline, len, rmem, nmem);
	if (rv == ERANGE) {
		*res = 0;
		free(rmem);
		free(rline);
		if (orig_offset < 0 || fseek(f, orig_offset, SEEK_SET) != 0) {
			rv = ESPIPE;
		}
		goto done;
	}
	*res = gr;
	free(rmem);
	free(rline);
done:
	UNLOCK(grent_lock);
	if (rv) {
		errno = rv;
	}
	return rv;
}
