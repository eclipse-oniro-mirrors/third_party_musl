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

/* Exercise C23 POSIX behavior and preserve the legacy scanner behavior. */
#include <features.h>
#undef _OHOS_ISOC23_CONVERSION_SOURCE

#include <errno.h>
#include <limits.h>
#include <locale.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include "isoc23_scanf_test.h"

static int scan_v(const char *s, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int r = vsscanf(s, fmt, ap);
    va_end(ap);
    return r;
}

static int scan_c23_v(const char *s, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int r = __isoc23_vsscanf(s, fmt, ap);
    va_end(ap);
    return r;
}

static int scan_file(const char *s, const char *fmt, int c23, va_list ap)
{
    int saved = errno;
    FILE *f = open_scan_file();
    if (!f) {
        return EOF;
    }
    if (fputs(s, f) < 0 || fseek(f, 0, SEEK_SET)) {
        t_error("file setup failed\n");
        close_scan_file(f);
        return EOF;
    }
    errno = saved;
    int r = c23 ? __isoc23_vfscanf(f, fmt, ap) : vfscanf(f, fmt, ap);
    saved = errno;
    close_scan_file(f);
    errno = saved;
    return r;
}

static int scan_f(const char *s, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int r = scan_file(s, fmt, 0, ap);
    va_end(ap);
    return r;
}

static int scan_c23_f(const char *s, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int r = scan_file(s, fmt, 1, ap);
    va_end(ap);
    return r;
}

static int scan_wv(const wchar_t *s, const wchar_t *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int r = vswscanf(s, fmt, ap);
    va_end(ap);
    return r;
}

static int (*const narrow[])(const char *, const char *, ...) = {
    __isoc23_sscanf, scan_c23_v, scan_c23_f
};
static int (*const wide[])(const wchar_t *, const wchar_t *, ...) = {
    __isoc23_swscanf
};

static const char *width_text = "\xe4\xb8\xad" "A" "\xe6\x96\x87" "\xf0\x9f\x98\x80" "X";
static const wchar_t *width_want = L"\u4e2dA\u6587\U0001f600";
static const int width_bytes[] = {0, 3, 4, 7, 11};
static const char *width_types[] = {"c", "s", "[^X]"};

static void check_narrow_width(size_t a, size_t t, int w)
{
    wchar_t out[8];
    char fmt[32], next = 0;
    int n = -1;
    wmemset(out, L'?', 8);
    int fmt_len = snprintf(fmt, sizeof fmt, "%%%dl%s%%n%%c", w, width_types[t]);
    if (fmt_len < 0 || (size_t)fmt_len >= sizeof fmt) {
        t_error("cannot format wide conversion\n");
        return;
    }
    int r = narrow[a](width_text, fmt, out, &n, &next);
    if (r != 2 || n != width_bytes[w] || wmemcmp(out, width_want, w) ||
        (unsigned char)next != (unsigned char)width_text[width_bytes[w]] ||
        out[w] != (t ? 0 : L'?')) {
        t_error("narrow %zu %s: r=%d n=%d\n", a, fmt, r, n);
    }
    n = -1;
    fmt_len = snprintf(fmt, sizeof fmt, "%%*%dl%s%%n%%c", w, width_types[t]);
    if (fmt_len < 0 || (size_t)fmt_len >= sizeof fmt) {
        t_error("cannot format suppressed conversion\n");
        return;
    }
    r = narrow[a](width_text, fmt, &n, &next);
    if (r != 1 || n != width_bytes[w] ||
        (unsigned char)next != (unsigned char)width_text[width_bytes[w]]) {
        t_error("suppression %zu %s: r=%d n=%d\n", a, fmt, r, n);
    }
}

static void check_narrow_encoding(size_t a)
{
    wchar_t out[8];
    int n = -1;
    if (narrow[a]("\xe4\xb8\xad\xe6\x96\x87 X", "%ls%n", out, &n) != 1 ||
        wcscmp(out, L"\u4e2d\u6587") || n != 6) {
        t_error("unlimited wide output %zu\n", a);
    }
    const char *invalid[] = {"\xff", "\xe4\xb8"};
    for (size_t i = 0; i < sizeof invalid / sizeof *invalid; i++) {
        errno = 0;
        int r = narrow[a](invalid[i], "%lc", out);
        if (r != EOF || errno != EILSEQ) {
            t_error("encoding failure %zu/%zu: r=%d errno=%d\n", a, i, r, errno);
        }
    }
    errno = 0;
    n = -1;
    int r = narrow[a]("\xe4\xb8\xad\xff", "%1lc%n", out, &n);
    if (r != 1 || n != 3 || out[0] != L'\u4e2d' || errno) {
        t_error("width must stop before invalid byte %zu: r=%d n=%d errno=%d\n", a, r, n, errno);
    }
    errno = 0;
    int value = 0;
    r = narrow[a]("42 \xe4\xb8", "%d %lc", &value, out);
    if (r != 1 || value != 42 || errno != EILSEQ) {
        t_error("encoding error after assignment %zu\n", a);
    }
}

/**
 * @tc.name      : isoc23_scanf_posix_0100
 * @tc.desc      : Verify wide character field widths, byte counts, suppression, and encoding errors in narrow scans.
 * @tc.level     : Level 0
 */
static void isoc23_scanf_posix_0100(void)
{
    for (size_t a = 0; a < sizeof narrow / sizeof *narrow; a++) {
        for (size_t t = 0; t < sizeof width_types / sizeof *width_types; t++) {
            for (int w = 1; w <= 4; w++) {
                check_narrow_width(a, t, w);
            }
        }
        check_narrow_encoding(a);
    }
}

static void check_allocated_conversion(size_t a, size_t t, int len, const char *text, const char *type)
{
    wchar_t *out = NULL;
    char fmt[32], next = 0;
    int n = -1;
    int fmt_len = snprintf(fmt, sizeof fmt, "%%%dml%s%%n%%c", len, type);
    if (fmt_len < 0 || (size_t)fmt_len >= sizeof fmt) {
        t_error("cannot format allocated conversion\n");
        return;
    }
    int r = narrow[a](text, fmt, &out, &n, &next);
    if (r != 2 || n != 3*len || next != 'X' || !out) {
        t_error("allocation %zu %s\n", a, fmt);
    } else {
        for (int i = 0; i < len; i++) {
            if (out[i] != L'\u4e2d') {
                t_error("allocation contents %zu %s\n", a, fmt);
                break;
            }
        }
        if (t && out[len]) {
            t_error("allocation terminator\n");
        }
    }
    free(out);
}

/**
 * @tc.name      : isoc23_scanf_posix_0200
 * @tc.desc      : Verify POSIX allocation for wide characters, strings, and scansets across input lengths.
 * @tc.level     : Level 0
 */
static void isoc23_scanf_posix_0200(void)
{
    const int lengths[] = {1, 31, 32, 63, 4096};
    const char *types[] = {"c", "s", "[^X]"};
    for (size_t a = 0; a < sizeof narrow / sizeof *narrow; a++) {
        for (size_t j = 0; j < sizeof lengths / sizeof *lengths; j++) {
            int len = lengths[j];
            char *text = malloc(3 * len + 2);
            if (!text) {
                t_error("test allocation failed\n");
                return;
            }
            for (int i = 0; i < len; i++) {
                memcpy(text + 3*i, "\xe4\xb8\xad", 3);
            }
            strcpy(text + 3*len, "X");
            for (size_t t = 0; t < sizeof types / sizeof *types; t++) {
                check_allocated_conversion(a, t, len, text, types[t]);
            }
            free(text);
        }
    }
}

struct wide_destinations {
    wchar_t out[8];
    wchar_t next;
    int n;
    int value;
};

static void check_wide_characters(size_t a, int utf8, struct wide_destinations *state)
{
    int r = wide[a](L"\u4e2d\u6587X", L"%2lc%n%lc", state->out, &state->n, &state->next);
    if (r != 2 || state->n != 2 || wmemcmp(state->out, L"\u4e2d\u6587", 2) || state->next != L'X') {
        t_error("wide lc %zu locale %d\n", a, utf8);
    }
    r = wide[a](L"\u4e2d\u6587X", L"%1ls%n%lc", state->out, &state->n, &state->next);
    if (r != 2 || state->n != 1 || wcscmp(state->out, L"\u4e2d") || state->next != L'\u6587') {
        t_error("wide ls %zu locale %d\n", a, utf8);
    }
    r = wide[a](L"\u4e2d\u6587X", L"%l[^X]%n%lc", state->out, &state->n, &state->next);
    if (r != 2 || state->n != 2 || wcscmp(state->out, L"\u4e2d\u6587") || state->next != L'X') {
        t_error("wide scanset %zu locale %d\n", a, utf8);
    }
}

static void check_wide_numbers(size_t a, int utf8, struct wide_destinations *state)
{
    int r = wide[a](L"\u4e2d\u6587", L"\u4e2d%*lc%n", &state->n);
    if (r != 0 || state->n != 2) {
        t_error("wide literal/suppression %zu\n", a);
    }
    r = wide[a](L"42\u4e2d", L"%d%n%lc", &state->value, &state->n, &state->next);
    if (r != 2 || state->value != 42 || state->n != 2 || state->next != L'\u4e2d') {
        t_error("wide numeric tail %zu locale %d\n", a, utf8);
    }
    double d = 0;
    r = wide[a](L"3.5\u4e2d42\u6587", L"%lf%lc%d%n%lc", &d, state->out, &state->value, &state->n, &state->next);
    if (r != 4 || d != 3.5 || state->out[0] != L'\u4e2d' || state->value != 42 || state->n != 6 ||
        state->next != L'\u6587') {
        t_error("wide alternating numeric/character %zu\n", a);
    }
}

static void check_wide_encoding(size_t a, int utf8, struct wide_destinations *state)
{
    char bytes[16] = {0};
    errno = 0;
    int r = wide[a](L"\u4e2d", L"%c%n", bytes, &state->n);
    if (utf8 ? (r != 1 || state->n != 1 || memcmp(bytes, "\xe4\xb8\xad", 3)) :
               (r != EOF || errno != EILSEQ)) {
        t_error("wide-to-narrow encoding %zu locale %d\n", a, utf8);
    }
    if (utf8) {
        char *mb = NULL;
        r = wide[a](L"\U0001f600X", L"%1mc%n%lc", &mb, &state->n, &state->next);
        if (r != 2 || !mb || state->n != 1 || state->next != L'X' ||
            memcmp(mb, "\xf0\x9f\x98\x80", 4)) {
            t_error("allocated multibyte c %zu\n", a);
        }
        free(mb);
    }
    wchar_t *allocated = NULL;
    r = wide[a](L"\u4e2d\u6587X", L"%ml[^X]%n", &allocated, &state->n);
    if (r != 1 || state->n != 2 || !allocated || wcscmp(allocated, L"\u4e2d\u6587")) {
        t_error("wide allocation %zu locale %d\n", a, utf8);
    }
    free(allocated);
}

/**
 * @tc.name      : isoc23_scanf_posix_0300
 * @tc.desc      : Verify wide scans, multibyte output, allocation, and numeric buffer refills in C and UTF-8 locales.
 * @tc.level     : Level 0
 */
static void isoc23_scanf_posix_0300(int utf8)
{
    for (size_t a = 0; a < sizeof wide / sizeof *wide; a++) {
        struct wide_destinations state;
        state.next = 0;
        state.n = -1;
        state.value = 0;
        check_wide_characters(a, utf8, &state);
        check_wide_numbers(a, utf8, &state);
        check_wide_encoding(a, utf8, &state);
    }
    int32_t value = 0;
    int n = -1;
    wchar_t next = 0;
    int r = __isoc23_swscanf(L"0b101\u4e2d", L"%w32i%n%lc", &value, &n, &next);
    if (r != 2 || value != 5 || n != 5 || next != L'\u4e2d') {
        t_error("C23 binary/wN wide input in locale %d\n", utf8);
    }
    wchar_t text[1028];
    wmemset(text, L'0', 1024);
    wcscpy(text + 1024, L"42\u4e2d");
    r = __isoc23_swscanf(text, L"%w32d%n%lc", &value, &n, &next);
    if (r != 2 || value != 42 || n != 1026 || next != L'\u4e2d') {
        t_error("numeric adapter refill in locale %d\n", utf8);
    }
}

static int check_byte_stream_position(void)
{
    FILE *f = open_scan_file();
    if (!f) {
        return 0;
    }
    /* A tiny buffer forces a multibyte character across refills. */
    char buffer[2];
    if (setvbuf(f, buffer, _IOFBF, sizeof buffer) != 0 ||
        fputs("\xe4\xb8\xadX", f) < 0 || fseek(f, 0, SEEK_SET) != 0) {
        t_error("cannot prepare buffered input\n");
        close_scan_file(f);
        return 0;
    }
    wchar_t out = 0;
    int n = -1;
    int r = __isoc23_fscanf(f, "%lc%n", &out, &n);
    if (r != 1 || out != L'\u4e2d' || n != 3 || ftell(f) != 3 || fgetc(f) != 'X') {
        t_error("C23 byte stream position\n");
    }
    close_scan_file(f);
    return 1;
}

/**
 * @tc.name      : isoc23_scanf_posix_0400
 * @tc.desc      : Verify C23 byte stream positions across multibyte refills and legacy wide stream conversions.
 * @tc.level     : Level 0
 */
static void isoc23_scanf_posix_0400(void)
{
    if (check_byte_stream_position() == 0) {
        return;
    }
    FILE *f = open_scan_file();
    if (!f) {
        return;
    }
    if (fputws(L"\u4e2d42\u6587", f) < 0 || fseek(f, 0, SEEK_SET) != 0) {
        t_error("cannot prepare wide input\n");
        close_scan_file(f);
        return;
    }
    wchar_t first = 0, last = 0;
    int value = 0, n = -1;
    int r = fwscanf(f, L"%lc%d%n%lc", &first, &value, &n, &last);
    if (r != 3 || first != L'\u4e2d' || value != 42 || n != 3 || last != L'\u6587') {
        t_error("wide FILE regression\n");
    }
    close_scan_file(f);
}

static void check_legacy_narrow(void)
{
    static int (*const legacy[])(const char *, const char *, ...) = {
        sscanf, scan_v, scan_f
    };
    const char *formats[] = {"%1lc%n", "%2ls%n", "%2l[^X]%n"};
    for (size_t a = 0; a < sizeof legacy / sizeof *legacy; a++) {
        for (size_t t = 0; t < sizeof formats / sizeof *formats; t++) {
            wchar_t out[4] = {L'?', L'?', L'?', L'?'};
            int count = -1;
            errno = EDOM;
            int ret = legacy[a]("\xe4\xb8\xadX", formats[t], out, &count);
            if (ret != EOF || out[0] != L'?' || out[1] != L'?' || count != -1 || errno != EDOM) {
                t_error("legacy wide field width %zu/%zu: ret=%d count=%d errno=%d\n",
                    a, t, ret, count, errno);
            }
        }
        wchar_t out = L'?';
        int count = -1;
        errno = EDOM;
        int ret = legacy[a]("\xe4\xb8", "%3lc%n", &out, &count);
        if (ret != EOF || out != L'?' || count != -1 || errno != EDOM) {
            t_error("legacy incomplete multibyte input %zu changed\n", a);
        }
        errno = EDOM;
        ret = legacy[a]("\xe4\xb8\xadX", "%*2ls%n", &count);
        if (ret != EOF || count != -1 || errno != EDOM) {
            t_error("legacy suppressed wide field width %zu changed\n", a);
        }
    }
}

/**
 * @tc.name      : isoc23_scanf_posix_0500
 * @tc.desc      : Verify legacy scanners retain byte field widths, incomplete sequence errno, and wide input encoding.
 * @tc.level     : Level 0
 */
static void isoc23_scanf_posix_0500(void)
{
    check_legacy_narrow();
    if (!setlocale(LC_ALL, "C")) {
        t_error("C locale unavailable\n");
        return;
    }
    static int (*const legacy_wide[])(const wchar_t *, const wchar_t *, ...) = {
        swscanf, scan_wv
    };
    for (size_t a = 0; a < sizeof legacy_wide / sizeof *legacy_wide; a++) {
        wchar_t out = L'?';
        errno = 0;
        int ret = legacy_wide[a](L"\u4e2d", L"%lc", &out);
        if (ret != EOF || out != L'?' || errno != EILSEQ) {
            t_error("legacy wide string encoding %zu changed\n", a);
        }
    }
}

/**
 * @tc.name      : isoc23_scanf_posix_0600
 * @tc.desc      : Verify C23 allocated wide character conversions reject overflowing allocation sizes.
 * @tc.level     : Level 0
 */
static void isoc23_scanf_posix_0600(void)
{
    if (SIZE_MAX / sizeof(wchar_t) > INT_MAX) {
        return;
    }
    char fmt[64];
    wchar_t wfmt[64];
    int fmt_len = snprintf(fmt, sizeof fmt, "%%%zumlc%%n", SIZE_MAX / sizeof(wchar_t));
    if (fmt_len < 0 || (size_t)fmt_len >= sizeof fmt) {
        t_error("cannot format allocation overflow conversion\n");
        return;
    }
    for (size_t i = 0; (wfmt[i] = (unsigned char)fmt[i]); i++) {
    }
    for (size_t a = 0; a < sizeof narrow / sizeof *narrow; a++) {
        wchar_t *out = NULL;
        int count = -1;
        errno = 0;
        int ret = narrow[a]("A", fmt, &out, &count);
        if (ret != EOF || errno != ENOMEM || out || count != -1) {
            t_error("C23 wide allocation overflow %zu: ret=%d errno=%d count=%d\n", a, ret, errno, count);
        }
        free(out);
    }
    wchar_t *out = NULL;
    int count = -1;
    errno = 0;
    int ret = __isoc23_swscanf(L"A", wfmt, &out, &count);
    if (ret != EOF || errno != ENOMEM || out || count != -1) {
        t_error("C23 wide string allocation overflow: ret=%d errno=%d count=%d\n", ret, errno, count);
    }
    free(out);
}

int main(void)
{
    if (!setlocale(LC_ALL, "C")) {
        t_error("C locale unavailable\n");
        return t_status;
    }
    isoc23_scanf_posix_0300(0);
    if (!setlocale(LC_ALL, "C.UTF-8")) {
        t_error("C.UTF-8 locale unavailable\n");
        return t_status;
    }
    isoc23_scanf_posix_0100();
    isoc23_scanf_posix_0200();
    isoc23_scanf_posix_0300(1);
    isoc23_scanf_posix_0400();
    isoc23_scanf_posix_0500();
    isoc23_scanf_posix_0600();
    return t_status;
}
