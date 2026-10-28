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

#include <errno.h>
#include <inttypes.h>
#include <locale.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wchar.h>
#include "isoc23_scanf_test.h"

static int scan_vstring(const char *s, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int ret = __isoc23_vsscanf(s, fmt, ap);
    va_end(ap);
    return ret;
}

static int scan_vfile(FILE *f, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int ret = __isoc23_vfscanf(f, fmt, ap);
    va_end(ap);
    return ret;
}

static int next_char;
static long position;

static int redirect_stdin(FILE *f)
{
    int saved = dup(STDIN_FILENO);
    if (saved < 0) {
        t_error("cannot save stdin\n");
        return -1;
    }
    if (dup2(fileno(f), STDIN_FILENO) < 0) {
        t_error("cannot redirect stdin\n");
        close_scan_fd(saved);
        return -1;
    }
    if (fseek(stdin, 0, SEEK_SET)) {
        t_error("cannot reset stdin buffer\n");
    }
    clearerr(stdin);
    return saved;
}

static void restore_stdin(int saved)
{
    if (fflush(stdin)) {
        t_error("cannot discard stdin lookahead\n");
    }
    if (dup2(saved, STDIN_FILENO) < 0) {
        t_error("cannot restore stdin\n");
    }
    clearerr(stdin);
    close_scan_fd(saved);
}

static int scan_file(int api, const char *s, const char *fmt, void *a, void *b, void *c, int error)
{
    FILE *f = open_scan_file();
    if (!f) {
        return EOF;
    }
    if (fputs(s, f) < 0 || fseek(f, 0, SEEK_SET)) {
        t_error("cannot prepare input\n");
        close_scan_file(f);
        return EOF;
    }
    int saved = -1;
    if (api == 4) {
        saved = redirect_stdin(f);
        if (saved < 0) {
            close_scan_file(f);
            return EOF;
        }
    }
    errno = error;
    int ret = api == 2 ? __isoc23_fscanf(f, fmt, a, b, c) :
        api == 3 ? scan_vfile(f, fmt, a, b, c) : __isoc23_scanf(fmt, a, b, c);
    error = errno;
    FILE *input = api == 4 ? stdin : f;
    position = ftell(input);
    next_char = fgetc(input);
    if (api == 4) {
        restore_stdin(saved);
    }
    close_scan_file(f);
    errno = error;
    return ret;
}

static int scan(int api, const char *s, const char *fmt, void *a, void *b, void *c)
{
    int error = errno;
    next_char = -2;
    position = -2;
    if (api == 0) {
        return __isoc23_sscanf(s, fmt, a, b, c);
    }
    if (api == 1) {
        return scan_vstring(s, fmt, a, b, c);
    }
    if (api == 5) {
        wchar_t ws[256], wf[128];
        size_t i;
        for (i = 0; (ws[i] = (unsigned char)s[i]); i++) {
        }
        for (i = 0; (wf[i] = (unsigned char)fmt[i]); i++) {
        }
        return __isoc23_swscanf(ws, wf, a, b, c);
    }

    return scan_file(api, s, fmt, a, b, c, error);
}

static void check_result(int api, int passed, int line, const char *expression)
{
    if (passed == 0) {
        t_error("api=%d line=%d: %s\n", api, line, expression);
    }
}

#define CHECK(api, c) check_result((api), (c), __LINE__, #c)

/**
 * @tc.name      : isoc23_scanf_compat_0100
 * @tc.desc      : Verify C23 integer prefix consumption, field widths, input positions, and errno preservation.
 * @tc.level     : Level 0
 */
static void isoc23_scanf_compat_0100(int api)
{
    static const struct {
        const char *text, *fmt;
        int ret, value, consumed;
    } cases[] = {
        {"0x", "%i%n", 0, 99, 2},
        {"0XG", "%i%n", 0, 99, 2},
        {"-0x!", "%i%n", 0, 99, 3},
        {"0x123", "%2i%n", 0, 99, 2},
        {"0x1", "%i%n", 1, 1, 3},
        {"0x1", "%1i%n", 1, 0, 1},
        {"0b", "%i%n", 0, 99, 2},
        {"-0B2", "%i%n", 0, 99, 3},
        {"0b101", "%2i%n", 0, 99, 2},
        {"0b101!", "%i%n", 1, 5, 5},
        {"abc", "%i%n", 0, 99, 0},
    };
    for (size_t i = 0; i < sizeof cases / sizeof *cases; i++) {
        int value = 99, n = -1;
        errno = EDOM;
        int ret = scan(api, cases[i].text, cases[i].fmt, &value, &n, 0);
        CHECK(api, ret == cases[i].ret && value == cases[i].value && errno == EDOM);
        CHECK(api, n == (ret == 1 ? cases[i].consumed : -1));
        if (api >= 2 && api <= 4) {
            int tail = (unsigned char)cases[i].text[cases[i].consumed];
            CHECK(api, position == cases[i].consumed && next_char == (tail ? tail : EOF));
        }
    }
    int first = 99, second = 99, n = -1;
    CHECK(api, scan(api, "7 0x", "%d%i%n", &first, &second, &n) == 1);
    CHECK(api, first == 7 && second == 99 && n == -1);
    CHECK(api, scan(api, "0b2", "%*i%n", &n, 0, 0) == 0 && n == -1);
}

static const struct {
    const char *text, *fmt;
    int ret, consumed;
    float value;
} float_cases[] = {
    {"100er", "%f%n", 0, 4, 42},
    {"1e+", "%f%n", 0, 3, 42},
    {"1e-!", "%f%n", 0, 3, 42},
    {"1e", "%f%n", 0, 2, 42},
    {"1e+2", "%2f%n", 0, 2, 42},
    {"1e+2", "%3f%n", 0, 3, 42},
    {"1e+2!", "%f%n", 1, 4, 100},
    {"0x1p+!", "%f%n", 0, 5, 42},
    {"0x1p-", "%f%n", 0, 5, 42},
    {"0x1p", "%f%n", 0, 4, 42},
    {"0x1p+2", "%4f%n", 0, 4, 42},
    {"0x1p+2", "%5f%n", 0, 5, 42},
    {"0x1p+2!", "%f%n", 1, 6, 4},
    {"nan(foo)!", "%f%n", 1, 8, NAN},
    {"nan(123)!", "%f%n", 1, 8, NAN},
    {"NAN()!", "%f%n", 1, 5, NAN},
    {"nan(a_B9)!", "%f%n", 1, 9, NAN},
    {"nan(!", "%f%n", 0, 5, 42},
    {"nan(", "%f%n", 0, 4, 42},
    {"nan(123", "%f%n", 0, 7, 42},
    {"nan( )!", "%f%n", 0, 5, 42},
    {"nan(12.3)!", "%f%n", 0, 7, 42},
    {"nan()!", "%3f%n", 1, 3, NAN},
    {"nan()!", "%4f%n", 0, 4, 42},
    {"nan()!", "%5f%n", 1, 5, NAN},
    {"nan(123)!", "%7f%n", 0, 7, 42},
    {"nan(123)!", "%8f%n", 1, 8, NAN},
    {"nan", "%f%n", 1, 3, NAN},
    {"nanX", "%f%n", 1, 3, NAN},
    {"100er", "%3f%n", 1, 3, 100},
};

static void check_float_cases(int api)
{
    for (size_t i = 0; i < sizeof float_cases / sizeof *float_cases; i++) {
        float value = 42;
        int n = -1;
        errno = EDOM;
        int ret = scan(api, float_cases[i].text, float_cases[i].fmt, &value, &n, 0);
        CHECK(api, ret == float_cases[i].ret && errno == EDOM);
        CHECK(api, n == (ret == 1 ? float_cases[i].consumed : -1));
        CHECK(api, isnan(float_cases[i].value) ? isnan(value) : value == float_cases[i].value);
        if (api >= 2 && api <= 4) {
            int tail = (unsigned char)float_cases[i].text[float_cases[i].consumed];
            CHECK(api, position == float_cases[i].consumed && next_char == (tail ? tail : EOF));
        }
    }
}

struct float_destinations {
    double d;
    long double ld;
    float value;
    int n;
};

static void check_float_exponents(int api, struct float_destinations *state)
{
    CHECK(api, scan(api, "1e+", "%lf%n", &state->d, &state->n, 0) == 0 && state->d == 42 && state->n == -1);
    CHECK(api, scan(api, "0x1p+", "%Lf%n", &state->ld, &state->n, 0) == 0 && state->ld == 42 && state->n == -1);
    state->value = 42;
    errno = EDOM;
    CHECK(api, scan(api, "infi!", "%f%n", &state->value, &state->n, 0) == 0 && state->value == 42 &&
        state->n == -1 && errno == EDOM);
}

static void check_signed_nan_float(int api, struct float_destinations *state)
{
    CHECK(api, scan(api, "-nan(payload)", "%f%n", &state->value, &state->n, 0) == 1 && isnan(state->value) &&
        signbit(state->value) && state->n == 13);
}

static void check_signed_nan(int api, struct float_destinations *state)
{
    check_signed_nan_float(api, state);
    state->n = -1;
    CHECK(api, scan(api, "-NAN()", "%lf%n", &state->d, &state->n, 0) == 1 && isnan(state->d) &&
        signbit(state->d) && state->n == 6);
    state->n = -1;
    CHECK(api, scan(api, "-nan", "%Lf%n", &state->ld, &state->n, 0) == 1 && isnan(state->ld) &&
        signbit(state->ld) && state->n == 4);
}

static void check_float_suppression(int api, struct float_destinations *state)
{
    state->n = -1;
    int first = 99;
    state->value = 42;
    CHECK(api, scan(api, "7 1e+", "%d%f%n", &first, &state->value, &state->n) == 1);
    CHECK(api, first == 7 && state->value == 42 && state->n == -1);
    CHECK(api, scan(api, "nan(123)!", "%*f%n", &state->n, 0, 0) == 0 && state->n == 8);
    state->n = -1;
    CHECK(api, scan(api, "nan( )!", "%*f%n", &state->n, 0, 0) == 0 && state->n == -1);
}

/**
 * @tc.name      : isoc23_scanf_compat_0200
 * @tc.desc      : Verify C23 floating point parsing of incomplete exponents, signed NaNs, and invalid infinity input.
 * @tc.level     : Level 0
 */
static void isoc23_scanf_compat_0200(int api)
{
    check_float_cases(api);
    struct float_destinations state = {42, 42, 42, -1};
    check_float_exponents(api, &state);
    check_signed_nan(api, &state);
    check_float_suppression(api, &state);
}

/**
 * @tc.name      : isoc23_scanf_compat_0300
 * @tc.desc      : Verify partial character reads, assignment suppression, and empty input in C23 scans.
 * @tc.level     : Level 0
 */
static void isoc23_scanf_compat_0300(int api)
{
    char s[4] = {'?', '?', '?', '?'};
    wchar_t ws[4] = {'?', '?', '?', '?'};
    int n = -1;
    CHECK(api, scan(api, "A", "%3c%n", s, &n, 0) == 1 && n == 1);
    CHECK(api, s[0] == 'A' && s[1] == '?' && s[2] == '?' && s[3] == '?');
    n = -1;
    CHECK(api, scan(api, "A", "%3lc%n", ws, &n, 0) == 1 && n == 1);
    CHECK(api, ws[0] == 'A' && ws[1] == '?' && ws[2] == '?' && ws[3] == '?');
    n = -1;
    CHECK(api, scan(api, "A", "%*3c%n", &n, 0, 0) == 0 && n == 1);
    n = -1;
    CHECK(api, scan(api, "A", "%*3lc%n", &n, 0, 0) == 0 && n == 1);
    n = -1;
    CHECK(api, scan(api, "", "%3c%n", s, &n, 0) == EOF && n == -1);
    CHECK(api, s[0] == 'A' && s[1] == '?');
}

static void check_null_pointer_variants(int api)
{
    const char *text[] = {"(nil)!", "(NIL)", " (NiL)X"};
    const int count[] = {5, 5, 6};
    const int tail[] = {'!', EOF, 'X'};
    for (size_t i = 0; i < sizeof text / sizeof *text; i++) {
        void *p = &p;
        int n = -1;
        errno = EDOM;
        CHECK(api, scan(api, text[i], "%p%n", &p, &n, 0) == 1 && p == 0 && n == count[i] && errno == EDOM);
        if (api >= 2 && api <= 4) {
            CHECK(api, position == count[i] && next_char == tail[i]);
        }
    }
}

/**
 * @tc.name      : isoc23_scanf_compat_0400
 * @tc.desc      : Verify null pointer text, field widths, suppression, and input positions in C23 scans.
 * @tc.level     : Level 0
 */
static void isoc23_scanf_compat_0400(int api)
{
    check_null_pointer_variants(api);
    void *p = &p;
    int n = -1;
    CHECK(api, scan(api, "(nil)", "%4p%n", &p, &n, 0) == 0 && p == &p && n == -1);
    if (api >= 2 && api <= 4) {
        CHECK(api, position == 0 && next_char == '(');
    }
    n = -1;
    CHECK(api, scan(api, "(nix)", "%p%n", &p, &n, 0) == 0 && p == &p && n == -1);
    if (api >= 2 && api <= 4) {
        CHECK(api, position == 3 && next_char == 'x');
    }
    n = -1;
    CHECK(api, scan(api, "(nil)", "%*p%n", &n, 0, 0) == 0 && n == 5);
}

static void check_binary_guard(int api, const unsigned char *guard, size_t size)
{
    for (size_t i = 0; i < size; i++) {
        CHECK(api, guard[i] == 0xa5);
    }
}

#define CHECK_BINARY(api, type, macro) do { \
    struct { type value; unsigned char guard[16]; } object; \
    memset(&object, 0xa5, sizeof object); \
    int n = -1; \
    CHECK((api), scan((api), "0b101!", "%" macro "%n", &object.value, &n, 0) == 1); \
    CHECK(api, object.value == 5 && n == 5); \
    check_binary_guard((api), object.guard, sizeof object.guard); \
} while (0)

/**
 * @tc.name      : isoc23_scanf_compat_0500
 * @tc.desc      : Verify SCNb format macros store binary values without overwriting destination guard bytes.
 * @tc.level     : Level 0
 */
static void isoc23_scanf_compat_0500(int api)
{
    CHECK_BINARY(api, uint8_t, SCNb8);
    CHECK_BINARY(api, uint16_t, SCNb16);
    CHECK_BINARY(api, uint32_t, SCNb32);
    CHECK_BINARY(api, uint64_t, SCNb64);
    CHECK_BINARY(api, uint_least8_t, SCNbLEAST8);
    CHECK_BINARY(api, uint_least16_t, SCNbLEAST16);
    CHECK_BINARY(api, uint_least32_t, SCNbLEAST32);
    CHECK_BINARY(api, uint_least64_t, SCNbLEAST64);
    CHECK_BINARY(api, uint_fast8_t, SCNbFAST8);
    CHECK_BINARY(api, uint_fast16_t, SCNbFAST16);
    CHECK_BINARY(api, uint_fast32_t, SCNbFAST32);
    CHECK_BINARY(api, uint_fast64_t, SCNbFAST64);
    CHECK_BINARY(api, uintmax_t, SCNbMAX);
    CHECK_BINARY(api, uintptr_t, SCNbPTR);
}

#ifndef __GLIBC__
static void check_legacy_strto(void)
{
    int api = -1;
    char *end;
    CHECK(api, strtod("1e+", &end) == 1 && !strcmp(end, "e+"));
    CHECK(api, isnan(strtod("nan(foo)!", &end)) && !strcmp(end, "!"));
    CHECK(api, strtod("0x1p+", &end) == 1 && !strcmp(end, "p+"));
    CHECK(api, isnan(strtold("nan( )!", &end)) && !strcmp(end, "( )!"));
    CHECK(api, __isoc23_strtol("0b2", &end, 0) == 0 && !strcmp(end, "b2"));
    CHECK(api, __isoc23_strtol("0xG", &end, 0) == 0 && !strcmp(end, "xG"));
}

static void check_wide_and_strto_bounds(void)
{
    int api = -1;
    char *end;
    wchar_t *wend;
    CHECK(api, wcstod(L"1e+", &wend) == 1 && !wcscmp(wend, L"e+"));
    errno = 0;
    CHECK(api, __isoc23_strtol("abc", &end, 0) == 0 && errno == EINVAL && !strcmp(end, "abc"));
    struct { char s[4]; unsigned char guard[8]; } z;
    memset(&z, '?', sizeof z);
    CHECK(api, __isoc23_swscanf(L"abc!", L"%3s", z.s) == 1 && !strcmp(z.s, "abc") && z.guard[0] == '?');
    memset(&z, '?', sizeof z);
    CHECK(api, __isoc23_swscanf(L"abc!", L"%3[abc]", z.s) == 1 && !strcmp(z.s, "abc") && z.guard[0] == '?');
}

static void check_legacy_numbers(int *out_value, int *out_n, float *out_f)
{
    int api = -1, value = 99, n = -1;
    CHECK(api, sscanf("0x", "%i%n", &value, &n) == 0 && value == 99 && n == -1);
    float f = 42;
    /* Legacy OH ARM/AArch64 parsefloat accepts these; the generic path fails. */
    int ret = sscanf("1e+", "%f%n", &f, &n);
    CHECK(api, (ret == 0 && f == 42 && n == -1) || (ret == 1 && f == 1 && n == 3));
    f = 42;
    n = -1;
    ret = sscanf("0x1p+", "%f%n", &f, &n);
    CHECK(api, (ret == 0 && f == 42 && n == -1) || (ret == 1 && f == 1 && n == 5));
    CHECK(api, sscanf("1.25", "%f", &f) == 1 && f == 1.25f);
    CHECK(api, sscanf("nan(foo)", "%f%n", &f, &n) == 1 && isnan(f) && n == 8);
    *out_value = value;
    *out_n = n;
    *out_f = f;
}

/**
 * @tc.name      : isoc23_scanf_compat_0600
 * @tc.desc      : Verify legacy scan and strto behavior, C23 strtol errors, and wide string destination bounds.
 * @tc.level     : Level 0
 */
static void isoc23_scanf_compat_0600(void)
{
    int api = -1, value, n;
    float f;
    check_legacy_numbers(&value, &n, &f);
    char s[4] = {'?', '?', '?', '?'};
    n = -1;
    CHECK(api, sscanf("A", "%3c%n", s, &n) == 0 && n == -1);
    void *p = &p;
    CHECK(api, sscanf("(nil)", "%p", &p) == 0 && p == &p);
    errno = 0;
    CHECK(api, sscanf("abc", "%i", &value) == 0 && errno == EINVAL);
    wchar_t ws[4] = {'?', '?', '?', '?'};
    n = -1;
    CHECK(api, swscanf(L"A", L"%3lc%n", ws, &n) == 0 && n == -1);
    f = 42;
    CHECK(api, swscanf(L"1.25", L"%f", &f) == 1 && f == 1.25f);
    check_legacy_strto();
    check_wide_and_strto_bounds();
}
#endif

int main(void)
{
    if (!setlocale(LC_ALL, "C")) {
        t_error("C locale unavailable\n");
        return t_status;
    }
    for (int api = 0; api < 6; api++) {
        isoc23_scanf_compat_0100(api);
        isoc23_scanf_compat_0200(api);
        isoc23_scanf_compat_0300(api);
        isoc23_scanf_compat_0400(api);
        isoc23_scanf_compat_0500(api);
    }
#ifndef __GLIBC__
    isoc23_scanf_compat_0600();
#endif
    return t_status;
}
