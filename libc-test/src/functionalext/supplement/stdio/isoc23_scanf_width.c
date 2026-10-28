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
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wchar.h>
#include "isoc23_scanf_test.h"

#define SCAN_API_COUNT 6
#define GUARD_SIZE 16

static int scan_vstring(const char *text, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int ret = __isoc23_vsscanf(text, fmt, ap);
    va_end(ap);
    return ret;
}

static int scan_vfile(FILE *fp, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int ret = __isoc23_vfscanf(fp, fmt, ap);
    va_end(ap);
    return ret;
}

static int scan_stdin(FILE *fp, const char *fmt, void *value, void *count)
{
    int saved = dup(STDIN_FILENO);
    if (saved < 0) {
        t_error("cannot save stdin\n");
        close_scan_file(fp);
        return EOF;
    }
    if (dup2(fileno(fp), STDIN_FILENO) < 0) {
        t_error("cannot redirect stdin\n");
        close_scan_fd(saved);
        close_scan_file(fp);
        return EOF;
    }
    if (fseek(stdin, 0, SEEK_SET)) {
        t_error("cannot reset stdin buffer\n");
    }
    clearerr(stdin);
    errno = 0;
    int ret = __isoc23_scanf(fmt, value, count);
    int error = errno;
    if (fflush(stdin)) {
        t_error("cannot discard stdin lookahead\n");
    }
    if (dup2(saved, STDIN_FILENO) < 0) {
        t_error("cannot restore stdin\n");
    }
    clearerr(stdin);
    close_scan_fd(saved);
    errno = error;
    close_scan_file(fp);
    errno = error;
    return ret;
}

static int scan_file(int api, const char *text, const char *fmt, void *value, void *count)
{
    FILE *fp = open_scan_file();
    if (!fp) {
        return EOF;
    }
    if (fputs(text, fp) < 0 || fflush(fp)) {
        t_error("cannot write input\n");
        close_scan_file(fp);
        return EOF;
    }
    if (fseek(fp, 0, SEEK_SET) != 0) {
        t_error("cannot rewind input\n");
        close_scan_file(fp);
        return EOF;
    }
    /* Keep setup operations from affecting the errno checked by the caller. */
    errno = 0;
    int ret;
    if (api == 3) {
        ret = __isoc23_fscanf(fp, fmt, value, count);
    } else if (api == 4) {
        ret = scan_vfile(fp, fmt, value, count);
    } else {
        return scan_stdin(fp, fmt, value, count);
    }
    int error = errno;
    close_scan_file(fp);
    errno = error;
    return ret;
}

/* Run the same cases through all six exported C23 scanf entry points. */
static int scan(int api, const char *text, const char *fmt, void *value, void *count)
{
    if (api == 0) {
        return __isoc23_sscanf(text, fmt, value, count);
    }
    if (api == 1) {
        return scan_vstring(text, fmt, value, count);
    }
    if (api == 2) {
        wchar_t wtext[256], wfmt[256];
        if (mbstowcs(wtext, text, 256) >= 256 || mbstowcs(wfmt, fmt, 256) >= 256) {
            t_error("cannot convert test text or format\n");
            return EOF;
        }
        return __isoc23_swscanf(wtext, wfmt, value, count);
    }

    return scan_file(api, text, fmt, value, count);
}

/* Compare the entire object, including padding and guard bytes. */
static void check_scan(const char *text, const char *fmt, int want_ret, int want_count,
    void *actual, const void *expected, size_t size, void *value)
{
    for (int api = 0; api < SCAN_API_COUNT; api++) {
        memset(actual, 0xa5, size);
        int count = -1;
        int ret = scan(api, text, fmt, value, &count);
        if (ret != want_ret || count != want_count || memcmp(actual, expected, size) != 0) {
            t_error("api=%d format=%s input=%s: ret=%d count=%d or wrong store size/value\n",
                api, fmt, text, ret, count);
        }
    }
}

#define CHECK_SCAN(type, text, fmt, want, want_ret, want_count) do { \
    struct { unsigned char before[GUARD_SIZE]; type value; unsigned char after[GUARD_SIZE]; } actual, expected; \
    memset(&expected, 0xa5, sizeof expected); \
    expected.value = (type)(want); \
    check_scan((text), (fmt), (want_ret), (want_count), \
        &actual, &expected, sizeof actual, &actual.value); \
} while (0)

static int check_limit_text(int length, size_t size, const char *description)
{
    if (length < 0 || (size_t)length >= size) {
        t_error("cannot format %s\n", description);
        return 0;
    }
    return 1;
}

#define CHECK_LIMIT(type, value, output_fmt, scan_fmt, cast_type, description) do { \
    char text[32]; \
    int length = snprintf(text, sizeof text, (output_fmt), (cast_type)(value)); \
    if (check_limit_text(length, sizeof text, (description)) == 0) { \
        return; \
    } \
    CHECK_SCAN(type, text, (scan_fmt), (value), 1, (int)strlen(text)); \
} while (0)

#define CHECK_BASES(stype, utype, mod) do { \
    CHECK_SCAN(stype, "-0b101", "%" mod "i%n", -5, 1, 6); \
    CHECK_SCAN(utype, "0b101", "%" mod "b%n", 5, 1, 5); \
    CHECK_SCAN(utype, "125", "%" mod "o%n", 85, 1, 3); \
    CHECK_SCAN(utype, "0x5a", "%" mod "x%n", 90, 1, 4); \
    CHECK_SCAN(utype, "0X5A", "%" mod "X%n", 90, 1, 4); \
    CHECK_SCAN(stype, "A", "%*c%" mod "n%n", 1, 0, 1); \
} while (0)

#define DEFINE_TYPE_CHECK(name, stype, utype, mod, minval, maxval, umaxval) \
static void (name)(void) \
{ \
    CHECK_LIMIT(stype, (minval), "%jd", "%" mod "d%n", intmax_t, "signed minimum"); \
    CHECK_LIMIT(stype, (maxval), "%jd", "%" mod "d%n", intmax_t, "signed maximum"); \
    CHECK_LIMIT(utype, (umaxval), "%ju", "%" mod "u%n", uintmax_t, "unsigned maximum"); \
    CHECK_BASES(stype, utype, mod); \
}

DEFINE_TYPE_CHECK(check_int8_t, int8_t, uint8_t, "w8", INT8_MIN, INT8_MAX, UINT8_MAX)
DEFINE_TYPE_CHECK(check_int16_t, int16_t, uint16_t, "w16", INT16_MIN, INT16_MAX, UINT16_MAX)
DEFINE_TYPE_CHECK(check_int32_t, int32_t, uint32_t, "w32", INT32_MIN, INT32_MAX, UINT32_MAX)
DEFINE_TYPE_CHECK(check_int64_t, int64_t, uint64_t, "w64", INT64_MIN, INT64_MAX, UINT64_MAX)
DEFINE_TYPE_CHECK(check_int_least8_t, int_least8_t, uint_least8_t, "w8",
    INT_LEAST8_MIN, INT_LEAST8_MAX, UINT_LEAST8_MAX)
DEFINE_TYPE_CHECK(check_int_least16_t, int_least16_t, uint_least16_t, "w16",
    INT_LEAST16_MIN, INT_LEAST16_MAX, UINT_LEAST16_MAX)
DEFINE_TYPE_CHECK(check_int_least32_t, int_least32_t, uint_least32_t, "w32",
    INT_LEAST32_MIN, INT_LEAST32_MAX, UINT_LEAST32_MAX)
DEFINE_TYPE_CHECK(check_int_least64_t, int_least64_t, uint_least64_t, "w64",
    INT_LEAST64_MIN, INT_LEAST64_MAX, UINT_LEAST64_MAX)
DEFINE_TYPE_CHECK(check_int_fast8_t, int_fast8_t, uint_fast8_t, "wf8",
    INT_FAST8_MIN, INT_FAST8_MAX, UINT_FAST8_MAX)
DEFINE_TYPE_CHECK(check_int_fast16_t, int_fast16_t, uint_fast16_t, "wf16",
    INT_FAST16_MIN, INT_FAST16_MAX, UINT_FAST16_MAX)
DEFINE_TYPE_CHECK(check_int_fast32_t, int_fast32_t, uint_fast32_t, "wf32",
    INT_FAST32_MIN, INT_FAST32_MAX, UINT_FAST32_MAX)
DEFINE_TYPE_CHECK(check_int_fast64_t, int_fast64_t, uint_fast64_t, "wf64",
    INT_FAST64_MIN, INT_FAST64_MAX, UINT_FAST64_MAX)

/**
 * @tc.name      : isoc23_scanf_width_0100
 * @tc.desc      : Verify wN and wfN integer conversions, value limits, and destination sizes.
 * @tc.level     : Level 0
 */
static void isoc23_scanf_width_0100(void)
{
    check_int8_t();
    check_int16_t();
    check_int32_t();
    check_int64_t();
    check_int_least8_t();
    check_int_least16_t();
    check_int_least32_t();
    check_int_least64_t();
    check_int_fast8_t();
    check_int_fast16_t();
    check_int_fast32_t();
    check_int_fast64_t();
}

/**
 * @tc.name      : isoc23_scanf_width_0200
 * @tc.desc      : Verify C23 length modifiers with field widths, suppression, positional arguments, and counts.
 * @tc.level     : Level 0
 */
static void isoc23_scanf_width_0200(void)
{
    CHECK_SCAN(int16_t, "12345", "%3w16d%n", 123, 1, 3);
    CHECK_SCAN(int_fast16_t, "19 23", "%*w64d %wf16d%n", 23, 1, 5);
    CHECK_SCAN(int32_t, "42", "%1$w32d%2$n", 42, 1, 2);
    CHECK_SCAN(int32_t, "A", "%w32n%n", 0, 0, 0);
    CHECK_SCAN(int64_t, "  -0B101", "%w64i%n", -5, 1, 8);
}

static void check_failed_input(int api, int count)
{
    int32_t value = -1;
    int ret = scan(api, "abc", "%w32d%n", &value, &count);
    if (ret != 0 || value != -1 || count != -1) {
        t_error("api=%d: matching failure changed destination\n", api);
    }
    ret = scan(api, "", "%w32d%n", &value, &count);
    if (ret != EOF || value != -1 || count != -1) {
        t_error("api=%d: input failure changed destination\n", api);
    }
    errno = 0;
    ret = scan(api, "", "%w24d", &value, &count);
    if (ret != 0 || errno != EINVAL || value != -1 || count != -1) {
        t_error("api=%d: unsupported width with empty input, ret=%d errno=%d\n", api, ret, errno);
    }
}

static void check_failure_after_assignment(int api)
{
    int32_t value = -1;
    int count = -1;
    errno = 0;
    int ret = scan(api, "42 7", "%w32d %w24d", &value, &count);
    if (ret != 1 || value != 42 || count != -1 || errno != EINVAL) {
        t_error("api=%d: unsupported width after assignment, ret=%d errno=%d\n", api, ret, errno);
    }
    value = -1;
    errno = 0;
    ret = scan(api, "42 7", "%w32d %2147483648w32d", &value, &count);
    if (ret != 1 || value != 42 || count != -1 || errno != EINVAL) {
        t_error("api=%d: oversized field width after assignment, ret=%d errno=%d\n", api, ret, errno);
    }
    check_failed_input(api, count);
}

/**
 * @tc.name      : isoc23_scanf_width_0300
 * @tc.desc      : Verify invalid length modifiers, oversized field widths, and failed conversions report errors.
 * @tc.level     : Level 0
 */
static void isoc23_scanf_width_0300(void)
{
    static const char *bad[] = {
        "%w0d", "%w1d", "%w24d", "%w128d", "%w08d", "%wd", "%wfd",
        "%wf0d", "%wf24d", "%wf016d", "%w999999999999999999999999999999d",
        "%wf999999999999999999999999999999d", "%w32", "%wf", "%w",
        "%w32f", "%w32s", "%w32p", "%w32[0-9]", "%w24n",
        "%2147483648w32d", "%999999999999999999999999999999c",
    };
    for (int api = 0; api < SCAN_API_COUNT; api++) {
        for (size_t i = 0; i < sizeof bad / sizeof *bad; i++) {
            unsigned char value[32], expected[32];
            memset(value, 0xa5, sizeof value);
            memcpy(expected, value, sizeof value);
            int count = -1;
            errno = 0;
            int ret = scan(api, "42", bad[i], value, &count);
            if (ret != 0 || errno != EINVAL || count != -1 || memcmp(value, expected, sizeof value)) {
                t_error("api=%d format=%s: ret=%d errno=%d or changed destination\n",
                    api, bad[i], ret, errno);
            }
        }
        check_failure_after_assignment(api);
    }
}

/**
 * @tc.name      : isoc23_scanf_width_0400
 * @tc.desc      : Verify legacy sscanf and swscanf continue to reject C23 length modifiers.
 * @tc.level     : Level 0
 */
static void isoc23_scanf_width_0400(void)
{
    int32_t value = -1;
    /* Older compilers do not recognize the new formats on built-in functions. */
    int (*read_string)(const char *, const char *, ...) = sscanf;
    if (read_string("42", "%w32d", &value) != EOF || value != -1) {
        t_error("legacy sscanf gained a C23 length modifier\n");
    }
    if (swscanf(L"42", L"%w32d", &value) != EOF || value != -1) {
        t_error("legacy swscanf gained a C23 length modifier\n");
    }
}

int main(void)
{
    /* stdin is reused with different file descriptors in scan(). */
    if (setvbuf(stdin, NULL, _IONBF, 0)) {
        t_error("cannot disable stdin buffering\n");
        return t_status;
    }
    isoc23_scanf_width_0100();
    isoc23_scanf_width_0200();
    isoc23_scanf_width_0300();
    isoc23_scanf_width_0400();
    return t_status;
}
