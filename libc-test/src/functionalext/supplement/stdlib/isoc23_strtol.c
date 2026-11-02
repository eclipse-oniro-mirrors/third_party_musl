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

/* Test legacy entry points alongside explicit C23 calls. */
#include <features.h>
#undef _OHOS_ISOC23_CONVERSION_SOURCE

#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test.h"

#define TEST(r, f, x, m) \
	do { \
		errno = 0; \
		(r) = (f); \
		if ((r) != (x)) { \
			t_error("%s failed (" m ")\n", #f, r, x); \
		} \
	} while (0)

#define TEST2(r, s, f, x, m) \
	do { \
		(r) = (f); \
		if ((r) != (x)) { \
			t_error("%s failed (" m ")\n", s, r, x); \
		} \
	} while (0)

/**
 * @tc.name      : isoc23_strtol_0100
 * @tc.desc      : Verify __isoc23_strtol accepts lower-case binary prefix
 * @tc.level     : Level 0
 */
void isoc23_strtol_0100(void)
{
    long result;
    char *end = NULL;
    TEST(result, __isoc23_strtol("0b101", &end, 0), 5L, "%ld != %ld");
    if (strcmp(end, "") != 0) {
        t_error("%s end is %s are not want empty string\n", __func__, end);
    }
}

/**
 * @tc.name      : isoc23_strtol_0200
 * @tc.desc      : Verify __isoc23_strtol accepts upper-case binary prefix
 * @tc.level     : Level 0
 */
void isoc23_strtol_0200(void)
{
    long result;
    char *end = NULL;
    TEST(result, __isoc23_strtol("0B101", &end, 0), 5L, "%ld != %ld");
    if (strcmp(end, "") != 0) {
        t_error("%s end is %s are not want empty string\n", __func__, end);
    }
}

/**
 * @tc.name      : isoc23_strtol_0300
 * @tc.desc      : Verify __isoc23_strtol accepts negative binary prefix
 * @tc.level     : Level 0
 */
void isoc23_strtol_0300(void)
{
    long result;
    char *end = NULL;
    TEST(result, __isoc23_strtol("-0b11111", &end, 0), -31L, "%ld != %ld");
    if (strcmp(end, "") != 0) {
        t_error("%s end is %s are not want empty string\n", __func__, end);
    }
}

/**
 * @tc.name      : isoc23_strtol_0400
 * @tc.desc      : Verify __isoc23_strtoul accepts binary prefix
 * @tc.level     : Level 0
 */
void isoc23_strtol_0400(void)
{
    unsigned long result;
    char *end = NULL;
    TEST(result, __isoc23_strtoul("0B101", &end, 0), 5UL, "%lu != %lu");
    if (strcmp(end, "") != 0) {
        t_error("%s end is %s are not want empty string\n", __func__, end);
    }
}

/**
 * @tc.name      : isoc23_strtol_0500
 * @tc.desc      : Verify __isoc23_strtoll accepts binary prefix
 * @tc.level     : Level 0
 */
void isoc23_strtol_0500(void)
{
    long long result;
    char *end = NULL;
    TEST(result, __isoc23_strtoll("0b101", &end, 0), 5LL, "%lld != %lld");
    if (strcmp(end, "") != 0) {
        t_error("%s end is %s are not want empty string\n", __func__, end);
    }
}

/**
 * @tc.name      : isoc23_strtol_0600
 * @tc.desc      : Verify __isoc23_strtoull accepts binary prefix
 * @tc.level     : Level 0
 */
void isoc23_strtol_0600(void)
{
    unsigned long long result;
    char *end = NULL;
    TEST(result, __isoc23_strtoull("0B101", &end, 0), 5ULL, "%llu != %llu");
    if (strcmp(end, "") != 0) {
        t_error("%s end is %s are not want empty string\n", __func__, end);
    }
}

/**
 * @tc.name      : isoc23_strtol_0700
 * @tc.desc      : Verify __isoc23_strtoimax accepts binary prefix
 * @tc.level     : Level 0
 */
void isoc23_strtol_0700(void)
{
    intmax_t result;
    char *end = NULL;
    TEST(result, __isoc23_strtoimax("-0b11111", &end, 0), (intmax_t)-31, "%" PRIdMAX " != %" PRIdMAX);
    if (strcmp(end, "") != 0) {
        t_error("%s end is %s are not want empty string\n", __func__, end);
    }
}

/**
 * @tc.name      : isoc23_strtol_0800
 * @tc.desc      : Verify __isoc23_strtoumax accepts binary prefix
 * @tc.level     : Level 0
 */
void isoc23_strtol_0800(void)
{
    uintmax_t result;
    char *end = NULL;
    TEST(result, __isoc23_strtoumax("0b101", &end, 0), (uintmax_t)5, "%" PRIuMAX " != %" PRIuMAX);
    if (strcmp(end, "") != 0) {
        t_error("%s end is %s are not want empty string\n", __func__, end);
    }
}

/**
 * @tc.name      : isoc23_strtol_0900
 * @tc.desc      : Verify __isoc23_strtol accepts binary prefix with explicit base 2
 * @tc.level     : Level 0
 */
void isoc23_strtol_0900(void)
{
    long result;
    char *end = NULL;
    TEST(result, __isoc23_strtol("0b101", &end, 2), 5L, "%ld != %ld");
    if (strcmp(end, "") != 0) {
        t_error("%s end is %s are not want empty string\n", __func__, end);
    }
}

/**
 * @tc.name      : isoc23_strtol_1000
 * @tc.desc      : Verify __isoc23_strtol stops at incomplete binary prefix
 * @tc.level     : Level 1
 */
void isoc23_strtol_1000(void)
{
    int offset;
    long result;
    char *start, *end = NULL;
    TEST(result, __isoc23_strtol(start = "0b", &end, 0), 0L, "%ld != %ld");
    TEST2(offset, "end - start", end - start, 1, "wrong final position %d != %d");
    if (strcmp(end, "b") != 0) {
        t_error("%s end is %s are not want b\n", __func__, end);
    }
}

/**
 * @tc.name      : isoc23_strtol_1100
 * @tc.desc      : Verify legacy strtol does not accept binary prefix
 * @tc.level     : Level 1
 */
void isoc23_strtol_1100(void)
{
    int offset;
    long result;
    char *start, *end = NULL;
    TEST(result, strtol(start = "0b101", &end, 0), 0L, "%ld != %ld");
    TEST2(offset, "end - start", end - start, 1, "wrong final position %d != %d");
    TEST2(offset, "errno", errno, 0, "%d != %d");
    if (strcmp(end, "b101") != 0) {
        t_error("%s end is %s are not want b101\n", __func__, end);
    }
}

/* SPEC US-05, AC-05.1 and BR-03: success must preserve a nonzero errno. */
static void isoc23_strtol_1200(void)
{
#define CHECK_DECIMAL(fn) do { \
    const char *text = "123abc"; \
    char *end = NULL; \
    errno = EDOM; \
    if (fn(text, &end, 10) != 123 || end != text + 3 || errno != EDOM) { \
        t_error("%s: decimal value, endptr or errno mismatch\n", #fn); \
    } \
} while (0)
    CHECK_DECIMAL(__isoc23_strtol);
    CHECK_DECIMAL(__isoc23_strtoul);
    CHECK_DECIMAL(__isoc23_strtoll);
    CHECK_DECIMAL(__isoc23_strtoull);
    CHECK_DECIMAL(__isoc23_strtoimax);
    CHECK_DECIMAL(__isoc23_strtoumax);
#undef CHECK_DECIMAL
}

/* AC-05.2/05.3, EB-01/02: the same inputs overflow both ILP32 and LP64 long. */
static void isoc23_strtol_1300(void)
{
    errno = EDOM;
    if (__isoc23_strtol("99999999999999999999", NULL, 10) != LONG_MAX || errno != ERANGE) {
        t_error("positive long overflow must return LONG_MAX and ERANGE\n");
    }
    errno = EDOM;
    if (__isoc23_strtol("-99999999999999999999", NULL, 10) != LONG_MIN || errno != ERANGE) {
        t_error("negative long overflow must return LONG_MIN and ERANGE\n");
    }
}

/* AC-05.4/05.5/05.8: unsigned negation, hexadecimal and optional endptr. */
static void isoc23_strtol_1400(void)
{
    errno = EDOM;
    if (__isoc23_strtoull("-1", NULL, 10) != ULLONG_MAX || errno != EDOM) {
        t_error("unsigned negation must preserve errno\n");
    }
    const char *text = "0xFF";
    char *end = NULL;
    errno = EDOM;
    if (__isoc23_strtoimax(text, &end, 16) != 255 || end != text + 4 || errno != EDOM) {
        t_error("hexadecimal intmax conversion failed\n");
    }
    errno = EDOM;
    if (__isoc23_strtoumax(text, &end, 16) != 255 || end != text + 4 || errno != EDOM) {
        t_error("hexadecimal uintmax conversion failed\n");
    }
    errno = EDOM;
    if (__isoc23_strtol("123", NULL, 10) != 123 || errno != EDOM) {
        t_error("null endptr conversion failed\n");
    }
}

/* AC-05.9, BR-02: explicit C23 and legacy symbols retain distinct behavior. */
static void isoc23_strtol_1500(void)
{
    const char *inputs[] = {"0b1010", "0B1010"};
    const int bases[] = {0, 2};
    for (size_t i = 0; i < sizeof inputs / sizeof *inputs; i++) {
        for (size_t j = 0; j < sizeof bases / sizeof *bases; j++) {
            const char *text = inputs[i];
            char *end = NULL;
            errno = EDOM;
            if (__isoc23_strtol(text, &end, bases[j]) != 10 || end != text + 6 || errno != EDOM) {
                t_error("C23 binary prefix case %zu/%zu failed\n", i, j);
            }
            errno = EDOM;
            if (strtol(text, &end, bases[j]) != 0 || end != text + 1 || errno != EDOM) {
                t_error("legacy binary prefix case %zu/%zu changed\n", i, j);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    isoc23_strtol_0100();
    isoc23_strtol_0200();
    isoc23_strtol_0300();
    isoc23_strtol_0400();
    isoc23_strtol_0500();
    isoc23_strtol_0600();
    isoc23_strtol_0700();
    isoc23_strtol_0800();
    isoc23_strtol_0900();
    isoc23_strtol_1000();
    isoc23_strtol_1100();
    isoc23_strtol_1200();
    isoc23_strtol_1300();
    isoc23_strtol_1400();
    isoc23_strtol_1500();
    return t_status;
}
