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
#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "test.h"

#define ARRAY_SIZE(a) (sizeof (a) / sizeof (a)[0])

static const unsigned seeds[] = { 0, 1, 42, 0x7fffffff, 0x80000000, UINT_MAX };
static const size_t sizes[] = { 8, 31, 32, 63, 64, 127, 128, 255, 256, 257, 512 };
static const size_t tier_sizes[] = { 8, 32, 64, 128, 256 };
static const int degrees[] = { 0, 7, 15, 31, 63 };
static const int separations[] = { 0, 3, 1, 3, 1 };

/* Hashes of 1000 musl random() outputs, using seed 1 for seed 0. */
static const uint32_t hashes[5][6] = {
	{ 0x66a99cbd, 0x66a99cbd, 0xa0bce7a5, 0xbe089d0d, 0x6043e12d, 0xbe089d0d },
	{ 0x63070794, 0x63070794, 0xfca14b1c, 0xb0eb9866, 0xb14d773d, 0x264fa974 },
	{ 0x6aa832b3, 0x6aa832b3, 0x41eb57fb, 0x17c014f0, 0x45f2fd4a, 0xd1aa4bbe },
	{ 0x95975461, 0x95975461, 0x8a692427, 0xb83a3ecc, 0xcf1f4427, 0xab3d3e66 },
	{ 0x1b4b7ce8, 0x1b4b7ce8, 0xc8bec9fe, 0x04fbda24, 0x23ac3aae, 0x1f718000 },
};

union state_storage {
	uint32_t align;
	unsigned char bytes[520];
};

static uint32_t sequence(struct random_data *d)
{
	uint32_t h = 2166136261U;
	int32_t x;

	for (int i = 0; i < 1000; i++) {
		if (random_r(d, &x) != 0 || x < 0) {
			return 0;
		}
		h = (h ^ x) * 16777619U;
	}
	return h;
}

static void check_layout(const struct random_data *d, const unsigned char *state, int type, size_t len)
{
	if (d->rand_type != type || d->rand_deg != degrees[type] ||
	    d->rand_sep != separations[type] ||
	    (const char *)d->state != (const char *)state + 4 ||
	    (const char *)d->end_ptr != (const char *)state + 4 + 4 * degrees[type]) {
		t_error("state layout: size %zu\n", len);
	}
}

static void check_canaries(const union state_storage *storage, size_t start, size_t used, size_t len)
{
	for (size_t i = 0; i < sizeof storage->bytes; i++) {
		if (i >= start && i < start + used) {
			continue;
		}
		if (storage->bytes[i] != 0xa5) {
			t_error("buffer overwrite: size %zu offset %zu byte %zu\n", len, start - 4, i);
			return;
		}
	}
}

static void check_sequence_case(size_t n, size_t s, int offset)
{
	union state_storage storage;
	struct random_data d = {0};
	size_t len = sizes[n];
	int type = (n < 8) ? n / 2 : 4;
	size_t used = (type != 0) ? 4 * (degrees[type] + 1) : 8;
	unsigned char *state = storage.bytes + 4 + offset;

	memset(&storage, 0xa5, sizeof storage);
	errno = EDOM;
	if (initstate_r(seeds[s], (char *)state, len, &d) != 0) {
		t_error("initstate_r failed: size %zu seed %u\n", len, seeds[s]);
		return;
	}
	check_layout(&d, state, type, len);
	if (sequence(&d) != hashes[type][s] || errno != EDOM) {
		t_error("sequence: size %zu seed %u offset %d\n", len, seeds[s], offset);
	}
	if (srandom_r(seeds[s], &d) != 0 || sequence(&d) != hashes[type][s] || errno != EDOM) {
		t_error("reseed: size %zu seed %u offset %d\n", len, seeds[s], offset);
	}
	check_canaries(&storage, 4 + offset, used, len);
}

static void check_sequences(void)
{
	for (size_t n = 0; n < ARRAY_SIZE(sizes); n++) {
		for (int offset = 0; offset < 4; offset++) {
			for (size_t s = 0; s < ARRAY_SIZE(seeds); s++) {
				check_sequence_case(n, s, offset);
			}
		}
	}
}

static void check_equal_sequences(struct random_data *d, struct random_data *ref,
	int count, const char *description)
{
	int32_t x, y;

	for (int i = 0; i < count; i++) {
		if (random_r(d, &x) != 0 || random_r(ref, &y) != 0 || x != y) {
			t_error("%s: step %d\n", description, i);
			return;
		}
	}
}

/* AC-2.4: compare reseeding with a fresh context in the same size tier. */
static void check_reseed_case(size_t n, size_t s)
{
	int32_t a[128], b[64], x;
	struct random_data d = {0}, fresh;
	int type = (n < 8) ? n / 2 : 4;

	/* Only state needs initialization before the first call. */
	fresh.state = NULL;
	if (initstate_r(17, (char *)a, sizes[n], &d) != 0 ||
	    initstate_r(seeds[s], (char *)b, tier_sizes[type], &fresh) != 0) {
		t_error("reseed setup\n");
		return;
	}
	for (int i = 0; i < 137; i++) {
		if (random_r(&d, &x) != 0) {
			t_error("advance before reseed\n");
			return;
		}
	}
	if (srandom_r(seeds[s], &d) != 0 || d.state != a + 1 || d.rand_type != type) {
		t_error("reseed changed generator: size %zu seed %u\n", sizes[n], seeds[s]);
		return;
	}
	check_equal_sequences(&d, &fresh, 1000, "fresh/reseed mismatch");
}

static void check_reseed(void)
{
	for (size_t n = 0; n < ARRAY_SIZE(sizes); n++) {
		for (size_t s = 0; s < ARRAY_SIZE(seeds); s++) {
			check_reseed_case(n, s);
		}
	}
}

static void check_init_errors(void)
{
	struct random_data d = {0};
	int32_t state[32];

	for (size_t n = 0; n < 8; n++) {
		errno = 0;
		if (initstate_r(1, (char *)state, n, &d) != -1 || errno != EINVAL || d.state != NULL) {
			t_error("short state: %zu\n", n);
		}
	}
	errno = 0;
	if (initstate_r(1, (char *)state, sizeof state, NULL) != -1 || errno != EINVAL) {
		t_error("initstate_r null context\n");
	}
	errno = 0;
	if (initstate_r(1, NULL, sizeof state, &d) != -1 || errno != EINVAL || d.state != NULL) {
		t_error("initstate_r null buffer\n");
	}
}

static void check_random_errors(void)
{
	struct random_data d = {0};
	int32_t state[32], x = -1;

	errno = 0;
	if (random_r(NULL, &x) != -1 || errno != EINVAL || x != -1) {
		t_error("random_r null context\n");
	}
	errno = 0;
	if (random_r(&d, &x) != -1 || errno != EINVAL || x != -1) {
		t_error("random_r uninitialized context\n");
	}
	if (initstate_r(1, (char *)state, sizeof state, &d) != 0) {
		t_error("random error test initialization\n");
		return;
	}
	errno = 0;
	if (random_r(&d, NULL) != -1 || errno != EINVAL) {
		t_error("random_r null result\n");
	}
}

static void check_seed_errors(void)
{
	static const int invalid_types[] = { -1, 5 };
	struct random_data d = {0};
	int32_t state[32];

	errno = EDOM;
	if (srandom_r(1, &d) != -1 || errno != EDOM || d.state != NULL) {
		t_error("srandom_r uninitialized context\n");
	}
	errno = EDOM;
	if (srandom_r(1, NULL) != -1 || errno != EDOM) {
		t_error("srandom_r null context\n");
	}
	if (initstate_r(1, (char *)state, sizeof state, &d) != 0) {
		t_error("seed error test initialization\n");
		return;
	}
	for (size_t i = 0; i < ARRAY_SIZE(invalid_types); i++) {
		d.rand_type = invalid_types[i];
		errno = EDOM;
		if (srandom_r(1, &d) != -1 || errno != EDOM) {
			t_error("srandom_r invalid type\n");
		}
	}
}

static void check_errors(void)
{
	check_init_errors();
	check_random_errors();
	check_seed_errors();
}

static int32_t saved_header(const struct random_data *d)
{
	if (d->rand_type == 0) {
		return 0;
	}
	return d->rand_type + 5 * (d->rptr - d->state);
}

static void check_failed_reinit(struct random_data *d, int32_t *a, int32_t *b, size_t len)
{
	int type = d->rand_type;
	int32_t saved = saved_header(d);

	errno = 0;
	if (initstate_r(42, (char *)b, 7, d) != -1 || errno != EINVAL || a[0] != saved) {
		t_error("failed reinit must save old position: type %d\n", type);
	}
	if (d->state != a + 1 || d->rand_type != type) {
		t_error("failed reinit changed context\n");
	}
	errno = 0;
	if (initstate_r(42, NULL, len, d) != -1 || errno != EINVAL) {
		t_error("reinit null buffer\n");
	}
	errno = 0;
	if (random_r(d, NULL) != -1 || errno != EINVAL) {
		t_error("null result on active generator\n");
	}
}

static void check_reinit_buffers(struct random_data *d, int32_t *a, int32_t *b, size_t len)
{
	int type = d->rand_type;
	int32_t saved = saved_header(d);

	if (initstate_r(42, (char *)b, tier_sizes[4], d) != 0 || a[0] != saved ||
	    b[0] != 4 || sequence(d) != hashes[4][2]) {
		t_error("reinit new buffer: type %d\n", type);
	}
	if (initstate_r(1, (char *)b, tier_sizes[4], d) != 0 || sequence(d) != hashes[4][1]) {
		t_error("reinit same buffer\n");
	}
	if (initstate_r(1, (char *)a, len, d) != 0 || sequence(d) != hashes[type][1] ||
	    srandom_r(42, d) != 0 || sequence(d) != hashes[type][2]) {
		t_error("reinit smaller buffer: type %d\n", type);
	}
}

static void check_reinit_type(int type)
{
	int32_t a[64], b[64], reference[64];
	struct random_data d = {0}, ref = {0};
	size_t len = tier_sizes[type];

	if (initstate_r(1, (char *)a, len, &d) != 0 ||
	    initstate_r(1, (char *)reference, len, &ref) != 0) {
		t_error("reinit setup\n");
		return;
	}
	check_equal_sequences(&d, &ref, 17, "reinit advance");
	check_failed_reinit(&d, a, b, len);
	check_equal_sequences(&d, &ref, 1000, "failed calls changed sequence");
	check_reinit_buffers(&d, a, b, len);
}

static void check_reinit(void)
{
	for (int type = 0; type < 5; type++) {
		check_reinit_type(type);
	}
}

/* SPEC stability baseline: musl's global generator with seed 0 mapped to 1. */
static void check_global_seed(struct random_data *d, int32_t *local, size_t len, size_t s)
{
	unsigned seed = seeds[s];
	int32_t x;

	srandom((seed != 0) ? seed : 1);
	if (initstate_r(seed, (char *)local, len, d) != 0) {
		t_error("local baseline setup\n");
		return;
	}
	for (int i = 0; i < 100000; i++) {
		if (i != 0 && i % 10000 == 0) {
			seed = seeds[(s + i / 10000) % ARRAY_SIZE(seeds)];
			srandom((seed != 0) ? seed : 1);
			if (srandom_r(seed, d) != 0) {
				t_error("baseline reseed\n");
			}
		}
		long expected = random();
		if (random_r(d, &x) != 0 || x < 0 || x != expected) {
			t_error("musl baseline mismatch: size %zu seed %u step %d\n", len, seed, i);
			return;
		}
	}
}

static void check_global_sequence(void)
{
	int32_t global[128], local[128];

	for (size_t n = 0; n < ARRAY_SIZE(sizes); n++) {
		struct random_data d = {0};
		char *old = initstate(1, (char *)global, sizes[n]);
		if (old == NULL) {
			t_error("global baseline setup\n");
			continue;
		}
		for (size_t s = 0; s < ARRAY_SIZE(seeds); s++) {
			check_global_seed(&d, local, sizes[n], s);
		}
		/* Do not leave global state pointing at this function's stack. */
		setstate(old);
	}
}

static void check_global(void)
{
	struct random_data d = {0};
	int32_t state[32];
	long expected[16];

	srandom(42);
	for (int i = 0; i < 16; i++) {
		expected[i] = random();
	}
	srandom(42);
	if (initstate_r(1, (char *)state, sizeof state, &d) != 0) {
		t_error("global isolation setup\n");
		return;
	}
	for (int i = 0; i < 16; i++) {
		if (srandom_r(1, &d) != 0 || sequence(&d) != hashes[3][1]) {
			t_error("global random changed reentrant state\n");
		}
		if (random() != expected[i]) {
			t_error("reentrant random changed global state\n");
		}
	}
}

struct job {
	int type;
	int failed;
};

static void *worker(void *arg)
{
	struct job *job = arg;
	struct random_data d = {0};
	int32_t state[64];
	size_t len = tier_sizes[job->type];

	if (initstate_r(1, (char *)state, len, &d) != 0) {
		job->failed = 1;
		return NULL;
	}
	for (int i = 0; i < 120; i++) {
		if (srandom_r(seeds[i % 6], &d) != 0 || sequence(&d) != hashes[job->type][i % 6]) {
			job->failed = 1;
		}
	}
	return NULL;
}

static void check_threads(void)
{
	pthread_t threads[5];
	struct job jobs[5];
	int n;

	for (n = 0; n < 5; n++) {
		jobs[n] = (struct job){ n, 0 };
		if (pthread_create(&threads[n], NULL, worker, &jobs[n]) != 0) {
			t_error("pthread_create\n");
			break;
		}
	}
	for (int i = 0; i < n; i++) {
		if (pthread_join(threads[i], NULL) != 0 || jobs[i].failed != 0) {
			t_error("independent thread state: type %d\n", i);
		}
	}
}

int main(void)
{
	check_sequences();
	check_reseed();
	check_errors();
	check_reinit();
	check_global_sequence();
	check_global();
	check_threads();
	return t_status;
}
