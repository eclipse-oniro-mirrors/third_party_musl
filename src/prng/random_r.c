/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#define MAX_TYPES 5
#define BREAK_0 8
#define BREAK_1 32
#define BREAK_2 64
#define BREAK_3 128
#define BREAK_4 256

/* State buffers may be unaligned. All word accesses use memcpy. */
static uint32_t load(const void *p)
{
	uint32_t x;

	memcpy(&x, p, sizeof x);
	return x;
}

static void store(void *p, uint32_t x)
{
	memcpy(p, &x, sizeof x);
}

static int32_t next(struct random_data *buf)
{
	uint32_t x;

	if (buf->rand_type == 0) {
		x = (1103515245U * load(buf->state) + 12345) & 0x7fffffff;
		store(buf->state, x);
		return x;
	}
	x = load(buf->fptr) + load(buf->rptr);
	store(buf->fptr, x);
	if (++buf->fptr == buf->end_ptr) {
		buf->fptr = buf->state;
	}
	if (++buf->rptr == buf->end_ptr) {
		buf->rptr = buf->state;
	}
	return x >> 1;
}

static void seed(unsigned s, struct random_data *buf)
{
	uint64_t r = (s != 0) ? s : 1;
	uint32_t x = r;
	char *state = (char *)buf->state;

	if (buf->rand_type == 0) {
		store(state, x);
		return;
	}

	/* Use musl's seeding algorithm, with zero treated as one. */
	for (int i = 0; i < buf->rand_deg; i++) {
		r = 6364136223846793005ULL * r + 1;
		x = r >> 32;
		if (i == 0) {
			x |= 1;
		}
		store(state + i * sizeof x, x);
	}
	buf->rptr = buf->state;
	buf->fptr = buf->state + buf->rand_sep;
}

int random_r(struct random_data *restrict buf, int32_t *restrict result)
{
	if (buf == NULL || result == NULL || buf->state == NULL) {
		errno = EINVAL;
		return -1;
	}
	*result = next(buf);
	return 0;
}

int srandom_r(unsigned s, struct random_data *buf)
{
	/* Unlike initstate_r and random_r, failure does not set errno. */
	if (buf == NULL || buf->state == NULL || (unsigned)buf->rand_type >= MAX_TYPES) {
		return -1;
	}
	seed(s, buf);
	return 0;
}

int initstate_r(unsigned s, char *restrict state, size_t size,
	struct random_data *restrict buf)
{
	static const unsigned char degree[MAX_TYPES] = { 0, 7, 15, 31, 63 };
	static const unsigned char separation[MAX_TYPES] = { 0, 3, 1, 3, 1 };
	int type;
	int32_t saved;

	if (buf == NULL || state == NULL) {
		errno = EINVAL;
		return -1;
	}
	if (buf->state != NULL) {
		/* Save type and rear position before checking the new size. */
		saved = buf->rand_type;
		if (saved != 0) {
			saved += MAX_TYPES * (buf->rptr - buf->state);
		}
		store((char *)buf->state - sizeof saved, saved);
	}
	if (size < BREAK_0) {
		errno = EINVAL;
		return -1;
	}
	if (size < BREAK_1) {
		type = 0;
	} else if (size < BREAK_2) {
		type = 1;
	} else if (size < BREAK_3) {
		type = 2;
	} else if (size < BREAK_4) {
		type = 3;
	} else {
		type = 4;
	}

	buf->rand_type = type;
	buf->rand_deg = degree[type];
	buf->rand_sep = separation[type];
	buf->state = (int32_t *)(state + sizeof saved);
	buf->end_ptr = buf->state + buf->rand_deg;
	seed(s, buf);
	/* The header uses type + 5*rear, not global random's n/i/j format. */
	saved = type;
	store(state, saved);
	return 0;
}
