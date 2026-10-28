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
#include <locale.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wchar.h>
#include "isoc23_scanf_test.h"

enum scanner { STRING, VSTRING, STREAM, VSTREAM };
static const char *scanner_names[] = { "sscanf", "vsscanf", "fscanf", "vfscanf" };

struct scan_input {
    enum scanner scanner;
    const char *text;
    FILE *fp;
    char buffer[16];
};

static int scan_string(const char *text, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int ret = __isoc23_vsscanf(text, fmt, ap);
    va_end(ap);
    return ret;
}

static int scan_stream(FILE *fp, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int ret = __isoc23_vfscanf(fp, fmt, ap);
    va_end(ap);
    return ret;
}

#define SCAN(in, fmt, ...) \
    ((in)->scanner == STRING ? __isoc23_sscanf((in)->text, (fmt), __VA_ARGS__) : \
     (in)->scanner == VSTRING ? scan_string((in)->text, (fmt), __VA_ARGS__) : \
     (in)->scanner == STREAM ? __isoc23_fscanf((in)->fp, (fmt), __VA_ARGS__) : \
     scan_stream((in)->fp, (fmt), __VA_ARGS__))

static int scan_string_legacy(const char *text, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int ret = vsscanf(text, fmt, ap);
    va_end(ap);
    return ret;
}

static int scan_stream_legacy(FILE *fp, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int ret = vfscanf(fp, fmt, ap);
    va_end(ap);
    return ret;
}

#define SCAN_LEGACY(in, fmt, ...) \
    ((in)->scanner == STRING ? sscanf((in)->text, (fmt), __VA_ARGS__) : \
     (in)->scanner == VSTRING ? scan_string_legacy((in)->text, (fmt), __VA_ARGS__) : \
     (in)->scanner == STREAM ? fscanf((in)->fp, (fmt), __VA_ARGS__) : \
     scan_stream_legacy((in)->fp, (fmt), __VA_ARGS__))

static void close_input(struct scan_input *in)
{
    if (fclose(in->fp) != 0) {
        t_error("%s: input %s: fclose failed (errno=%d)\n",
            scanner_names[in->scanner], in->text, errno);
    }
    in->fp = NULL;
}

static int begin_scan(struct scan_input *in, enum scanner scanner, const char *text)
{
    *in = (struct scan_input){ .scanner = scanner, .text = text };
    if (scanner == STRING || scanner == VSTRING) {
        return 0;
    }
    /* Use a descriptor-backed stream so OH fclose can release it. */
    in->fp = open_scan_file();
    if (!in->fp) {
        return -1;
    }
    /* A small caller-owned buffer also exercises input refills. */
    if (scanner == STREAM && setvbuf(in->fp, in->buffer, _IOFBF, sizeof in->buffer) != 0) {
        t_error("%s: setvbuf failed\n", scanner_names[scanner]);
        close_input(in);
        return -1;
    }
    if (fputs(text, in->fp) == EOF || fseek(in->fp, 0, SEEK_SET) != 0) {
        t_error("%s: cannot prepare input (errno=%d)\n", scanner_names[scanner], errno);
        close_input(in);
        return -1;
    }
    return 0;
}

static void end_scan(struct scan_input *in, long position)
{
    if (!in->fp) {
        return;
    }
    long actual = ftell(in->fp);
    int next = fgetc(in->fp);
    int expected = in->text[position] ? (unsigned char)in->text[position] : EOF;
    if (actual != position || next != expected) {
        t_error("%s: input %s, position %ld/%ld, next %d/%d\n",
            scanner_names[in->scanner], in->text, actual, position, next, expected);
    }
    close_input(in);
}

struct binary_case {
    const char *text;
    const char *fmt;
    int binary;
    unsigned long long value;
    int ret;
    int count;
    char next;
    long position;
};

static void check_binary_case(enum scanner scanner, const struct binary_case *c)
{
    struct scan_input in;
    if (begin_scan(&in, scanner, c->text)) {
        return;
    }
    union { long long signed_value; unsigned long long unsigned_value; } value;
    void *dest;
    if (c->binary) {
        value.unsigned_value = 99;
        dest = &value.unsigned_value;
    } else {
        value.signed_value = 99;
        dest = &value.signed_value;
    }
    int count = -1;
    char next = '?';
    errno = 0;
    int ret = SCAN(&in, c->fmt, dest, &count, &next);
    unsigned long long actual = c->binary ? value.unsigned_value : (unsigned long long)value.signed_value;
    if (ret != c->ret || actual != c->value || count != c->count || next != c->next) {
        t_error("%s: input %s, format %s, ret %d/%d, value %llu/%llu, count %d/%d, next %c/%c\n",
            scanner_names[scanner], c->text, c->fmt, ret, c->ret,
            actual, c->value, count, c->count, next, c->next);
    }
    if (ret > 0 && errno != 0) {
        t_error("%s: input %s unexpectedly set errno %d\n", scanner_names[scanner], c->text, errno);
    }
    end_scan(&in, c->position);
}

/**
 * @tc.name      : isoc23_scanf_binary_0100
 * @tc.desc      : Verify binary prefixes, field widths, and input positions across narrow scan APIs.
 * @tc.level     : Level 0
 */
static void isoc23_scanf_binary_0100(enum scanner scanner)
{
    static const struct binary_case cases[] = {
        { "101Z", "%llb%n%c", 1, 5, 2, 3, 'Z', 4 },
        { "0b101Z", "%llb%n%c", 1, 5, 2, 5, 'Z', 6 },
        { "0B101", "%llb%n%c", 1, 5, 1, 5, '?', 5 },
        { " \t-0b111Z", "%llb%n%c", 1, ULLONG_MAX-6, 2, 8, 'Z', 9 },
        { "+101Z", "%llb%n%c", 1, 5, 2, 4, 'Z', 5 },
        { "0b", "%llb%n%c", 1, 99, 0, -1, '?', 2 },
        { "0B2Z", "%llb%n%c", 1, 99, 0, -1, '?', 2 },
        { "0b101", "%1llb%n%c", 1, 0, 2, 1, 'b', 2 },
        { "0b101", "%2llb%n%c", 1, 99, 0, -1, '?', 2 },
        { "-0b11", "%3llb%n%c", 1, 99, 0, -1, '?', 3 },
        { "0b101", "%3llb%n%c", 1, 1, 2, 3, '0', 4 },
        { "101", "%2llb%n%c", 1, 2, 2, 2, '1', 3 },
        { "2Z", "%llb%n%c", 1, 99, 0, -1, '?', 0 },
        { "", "%llb%n%c", 1, 99, EOF, -1, '?', 0 },
        { "0b", "%lli%n%c", 0, 99, 0, -1, '?', 2 },
        { "0B", "%lli%n%c", 0, 99, 0, -1, '?', 2 },
        { "-0b", "%lli%n%c", 0, 99, 0, -1, '?', 3 },
        { "+0B2Z", "%lli%n%c", 0, 99, 0, -1, '?', 3 },
        { "0b101", "%1lli%n%c", 0, 0, 2, 1, 'b', 2 },
        { "0b101", "%2lli%n%c", 0, 99, 0, -1, '?', 2 },
        { "-0b11", "%3lli%n%c", 0, 99, 0, -1, '?', 3 },
        { "0b101", "%3lli%n%c", 0, 1, 2, 3, '0', 4 },
        { "0B101Z", "%lli%n%c", 0, 5, 2, 5, 'Z', 6 },
        { "0b Z", "%lli%n%c", 0, 99, 0, -1, '?', 2 },
    };

    for (size_t i = 0; i < sizeof cases / sizeof *cases; i++) {
        check_binary_case(scanner, &cases[i]);
    }
}

/**
 * @tc.name      : isoc23_scanf_binary_0200
 * @tc.desc      : Verify signed integer destination sizes across narrow scan APIs.
 * @tc.level     : Level 0
 */
static void isoc23_scanf_binary_0200(enum scanner scanner)
{
    static const struct {
        const char *text;
        long long value;
        int ret;
        char next;
    } cases[] = {
        { "0b101 x", 5, 2, 'x' },
        { "0B101", 5, 1, '?' },
        { "-0b11111 y", -31, 2, 'y' },
        { "-0B11111", -31, 1, '?' },
        { "0b101", 5, 1, '?' },
    };
    static const char *formats[][2] = {
        { "%i %c", "%i" },
        { "%li %c", "%li" },
        { "%lli %c", "%lli" },
    };

    for (int size = 0; size < 3; size++) {
        for (size_t i = 0; i < sizeof cases / sizeof *cases; i++) {
            struct scan_input in;
            if (begin_scan(&in, scanner, cases[i].text)) {
                continue;
            }
            int value_i = 99;
            long value_l = 99;
            long long value_ll = 99;
            void *dest = size == 0 ? (void *)&value_i :
                size == 1 ? (void *)&value_l : (void *)&value_ll;
            char next = '?';
            const char *fmt = formats[size][cases[i].ret == 2 ? 0 : 1];
            int ret = SCAN(&in, fmt, dest, &next);
            long long value = size == 0 ? value_i : size == 1 ? value_l : value_ll;
            if (ret != cases[i].ret || value != cases[i].value || next != cases[i].next) {
                t_error("%s: signed size %d case %zu: ret %d, value %lld, next %c\n",
                    scanner_names[scanner], size, i, ret, value, next);
            }
            end_scan(&in, strlen(cases[i].text));
        }
    }
}

/**
 * @tc.name      : isoc23_scanf_binary_0300
 * @tc.desc      : Verify mixed integer and character conversions in one call.
 * @tc.level     : Level 0
 */
static void isoc23_scanf_binary_0300(enum scanner scanner)
{
    const char *text = "0b101 0B10 -0b111 z";
    struct scan_input in;
    if (begin_scan(&in, scanner, text)) {
        return;
    }
    int value_i = 0;
    long value_l = 0;
    long long value_ll = 0;
    char next = '?';
    int ret = SCAN(&in, "%i %li %lli %c", &value_i, &value_l, &value_ll, &next);
    if (ret != 4 || value_i != 5 || value_l != 2 || value_ll != -7 || next != 'z') {
        t_error("%s: mixed integer and character conversion failed\n", scanner_names[scanner]);
    }
    end_scan(&in, strlen(text));
}

/**
 * @tc.name      : isoc23_scanf_binary_0400
 * @tc.desc      : Verify binary conversions with integer length modifiers.
 * @tc.level     : Level 0
 */
static void isoc23_scanf_binary_0400(enum scanner scanner)
{
    const char *text = "0b1 10 0B11 100 101 110 111 1000Z";
    struct scan_input in;
    if (begin_scan(&in, scanner, text)) {
        return;
    }
    unsigned char hh = 0;
    unsigned short h = 0;
    unsigned int u = 0;
    unsigned long l = 0;
    unsigned long long ll = 0;
    uintmax_t j = 0;
    size_t z = 0;
    uintptr_t t = 0;
    int count = -1;
    char next = '?';
    int ret = SCAN(&in, "%hhb %hb %b %lb %llb %jb %zb %tb%n%c",
        &hh, &h, &u, &l, &ll, &j, &z, &t, &count, &next);
    if (ret != 9 || hh != 1 || h != 2 || u != 3 || l != 4 || ll != 5 || j != 6 || z != 7 || t != 8 ||
        count != (int)strlen(text)-1 || next != 'Z') {
        t_error("%s: binary length modifiers failed\n", scanner_names[scanner]);
    }
    end_scan(&in, strlen(text));
}

/**
 * @tc.name      : isoc23_scanf_binary_0500
 * @tc.desc      : Verify suppressed binary conversions and incomplete prefixes.
 * @tc.level     : Level 0
 */
static void isoc23_scanf_binary_0500(enum scanner scanner)
{
    struct scan_input in;
    const char *text = "0b101 0B11Z";
    if (begin_scan(&in, scanner, text)) {
        return;
    }
    unsigned long long value = 99;
    int count = -1;
    char next = '?';
    int ret = SCAN(&in, "%*b %llb%n%c", &value, &count, &next);
    if (ret != 2 || value != 3 || count != (int)strlen(text)-1 || next != 'Z') {
        t_error("%s: suppressed binary conversion failed\n", scanner_names[scanner]);
    }
    end_scan(&in, strlen(text));

    if (begin_scan(&in, scanner, "0b2Z")) {
        return;
    }
    count = -1;
    next = '?';
    ret = SCAN(&in, "%*2i%n%c", &count, &next);
    if (ret != 0 || count != -1 || next != '?') {
        t_error("%s: suppressed incomplete prefix failed\n", scanner_names[scanner]);
    }
    end_scan(&in, 2);
}

/**
 * @tc.name      : isoc23_scanf_binary_0600
 * @tc.desc      : Verify legacy scan symbols retain their binary prefix behavior.
 * @tc.level     : Level 0
 */
static void isoc23_scanf_binary_0600(enum scanner scanner)
{
    struct scan_input in;
    if (begin_scan(&in, scanner, "0b101")) {
        return;
    }
    int value = 99;
    int count = -1;
    char next = '?';
    int ret = SCAN_LEGACY(&in, "%i%n%c", &value, &count, &next);
    if (ret != 2 || value != 0 || count != 1 || next != 'b') {
        t_error("%s: legacy %%i binary prefix behavior changed\n", scanner_names[scanner]);
    }
    end_scan(&in, 2);

    if (begin_scan(&in, scanner, "0b101")) {
        return;
    }
    unsigned int binary = 99;
    count = -1;
    ret = SCAN_LEGACY(&in, "%b%n", &binary, &count);
    if (ret != EOF || binary != 99 || count != -1) {
        t_error("%s: legacy %%b format rejection changed\n", scanner_names[scanner]);
    }
    end_scan(&in, 0);
}

static int prepare_stdin(const char *s)
{
    int fd[2], saved = dup(0);
    if (saved < 0 || pipe(fd)) {
        if (saved >= 0) {
            close_scan_fd(saved);
        }
        t_error("cannot prepare stdin\n");
        return -1;
    }
    if (write(fd[1], s, strlen(s)) != (ssize_t)strlen(s) || dup2(fd[0], 0) < 0) {
        close_scan_fd(fd[0]);
        close_scan_fd(fd[1]);
        close_scan_fd(saved);
        t_error("cannot redirect stdin\n");
        return -1;
    }
    close_scan_fd(fd[0]);
    close_scan_fd(fd[1]);
    clearerr(stdin);
    return saved;
}

static void restore_stdin(int saved)
{
    while (getchar() != EOF) {
    }
    if (dup2(saved, 0) < 0) {
        t_error("cannot restore stdin\n");
    }
    close_scan_fd(saved);
    clearerr(stdin);
}

/**
 * @tc.name      : isoc23_scanf_binary_0700
 * @tc.desc      : Verify C23 scanf reads binary and ordinary integers from stdin.
 * @tc.level     : Level 0
 */
static void isoc23_scanf_binary_0700(void)
{
    static const struct {
        const char *text, *fmt;
        int ret, value, count;
        char next;
    } cases[] = {
        { "0b101Z", "%i%n%c", 2, 5, 5, 'Z' },
        { " \t-0B11Z", "%i%n%c", 2, -3, 7, 'Z' },
        { "0b", "%i%n%c", 0, 99, -1, '?' },
        { "+0B2Z", "%i%n%c", 0, 99, -1, '?' },
        { "0b101Z", "%1i%n%c", 2, 0, 1, 'b' },
        { "0b101Z", "%2i%n%c", 0, 99, -1, '?' },
        { "0b101Z", "%3i%n%c", 2, 1, 3, '0' },
        { "17Z", "%i%n%c", 2, 17, 2, 'Z' },
        { "077Z", "%i%n%c", 2, 63, 3, 'Z' },
        { "0x2aZ", "%i%n%c", 2, 42, 4, 'Z' },
        { "Z", "%i%n%c", 0, 99, -1, '?' },
        { "", "%i%n%c", EOF, 99, -1, '?' },
    };
    for (size_t i=0; i<sizeof cases/sizeof *cases; i++) {
        int saved = prepare_stdin(cases[i].text);
        if (saved < 0) {
            continue;
        }
        int value = 99, count = -1;
        char next = '?';
        int ret = __isoc23_scanf(cases[i].fmt, &value, &count, &next);
        if (ret != cases[i].ret || value != cases[i].value ||
            count != cases[i].count || next != cases[i].next) {
            t_error("stdin case %zu: ret %d, value %d, count %d, next %c\n",
                i, ret, value, count, next);
        }
        restore_stdin(saved);
    }
}

static int check_stdin_lengths(unsigned int *value, int *consumed, char *tail)
{
    int saved = prepare_stdin("0b1 10 0B11 100 101 110 111 1000Z");
    if (saved < 0) {
        return 0;
    }
    unsigned char hh = 0;
    unsigned short h = 0;
    unsigned int u = 0;
    unsigned long l = 0;
    unsigned long long ll = 0;
    uintmax_t j = 0;
    size_t z = 0;
    uintptr_t t = 0;
    int count = -1;
    char next = '?';
    int ret = __isoc23_scanf("%hhb %hb %b %lb %llb %jb %zb %tb%n%c",
        &hh, &h, &u, &l, &ll, &j, &z, &t, &count, &next);
    if (ret != 9 || hh != 1 || h != 2 || u != 3 || l != 4 || ll != 5 ||
        j != 6 || z != 7 || t != 8 || count != 32 || next != 'Z') {
        t_error("stdin binary length modifiers failed\n");
    }
    restore_stdin(saved);

    *value = u;
    *consumed = count;
    *tail = next;
    return 1;
}

/**
 * @tc.name      : isoc23_scanf_binary_0800
 * @tc.desc      : Verify binary length modifiers, suppression, and legacy scanf on stdin.
 * @tc.level     : Level 0
 */
static void isoc23_scanf_binary_0800(void)
{
    unsigned int u;
    int count;
    char next;
    if (check_stdin_lengths(&u, &count, &next) == 0) {
        return;
    }
    int saved = prepare_stdin("0b101 0B11Z");
    if (saved < 0) {
        return;
    }
    int ret = __isoc23_scanf("%*i %b%n%c", &u, &count, &next);
    if (ret != 2 || u != 3 || count != 10 || next != 'Z') {
        t_error("stdin suppression failed\n");
    }
    restore_stdin(saved);

    saved = prepare_stdin("0b101Z");
    if (saved < 0) {
        return;
    }
    int value = 99;
    ret = scanf("%i%n%c", &value, &count, &next);
    if (ret != 2 || value != 0 || count != 1 || next != 'b') {
        t_error("legacy stdin binary prefix behavior changed\n");
    }
    restore_stdin(saved);
}

/**
 * @tc.name      : isoc23_scanf_binary_0900
 * @tc.desc      : Verify C23 swscanf handles binary numbers and wide input.
 * @tc.level     : Level 0
 */
static void isoc23_scanf_binary_0900(void)
{
    static const struct {
        const wchar_t *text, *fmt;
        int ret;
        long long value;
        int count;
        wchar_t next;
    } cases[] = {
        { L"0b101Z", L"%lli%n%lc", 2, 5, 5, L'Z' },
        { L" \t-0B11Z", L"%lli%n%lc", 2, -3, 7, L'Z' },
        { L"0b", L"%lli%n%lc", 0, 99, -1, L'?' },
        { L"+0B2Z", L"%lli%n%lc", 0, 99, -1, L'?' },
        { L"0b101Z", L"%1lli%n%lc", 2, 0, 1, L'b' },
        { L"0b101Z", L"%2lli%n%lc", 0, 99, -1, L'?' },
        { L"0b101Z", L"%3lli%n%lc", 2, 1, 3, L'0' },
        { L"-0b11", L"%3lli%n%lc", 0, 99, -1, L'?' },
        { L"17Z", L"%lli%n%lc", 2, 17, 2, L'Z' },
        { L"077Z", L"%lli%n%lc", 2, 63, 3, L'Z' },
        { L"0x2aZ", L"%lli%n%lc", 2, 42, 4, L'Z' },
        { L"0b101\u4e2d", L"%lli%n%lc", 2, 5, 5, L'\u4e2d' },
        { L"Z", L"%lli%n%lc", 0, 99, -1, L'?' },
        { L"", L"%lli%n%lc", EOF, 99, -1, L'?' },
    };
    for (size_t i=0; i<sizeof cases/sizeof *cases; i++) {
        long long value = 99;
        int count = -1;
        wchar_t next = L'?';
        int ret = __isoc23_swscanf(cases[i].text, cases[i].fmt, &value, &count, &next);
        if (ret != cases[i].ret || value != cases[i].value ||
            count != cases[i].count || next != cases[i].next) {
            t_error("wide case %zu: ret %d, value %lld, count %d, next %lx\n",
                i, ret, value, count, (unsigned long)next);
        }
    }
}

static void check_wide_lengths(void)
{
    unsigned char hh = 0;
    unsigned short h = 0;
    unsigned int u = 0;
    unsigned long l = 0;
    unsigned long long ll = 0;
    uintmax_t j = 0;
    size_t z = 0;
    uintptr_t t = 0;
    int count = -1;
    wchar_t next = L'?';
    int ret = __isoc23_swscanf(L"0b1 10 0B11 100 101 110 111 1000Z",
        L"%hhb %hb %b %lb %llb %jb %zb %tb%n%lc",
        &hh, &h, &u, &l, &ll, &j, &z, &t, &count, &next);
    if (ret != 9 || hh != 1 || h != 2 || u != 3 || l != 4 || ll != 5 ||
        j != 6 || z != 7 || t != 8 || count != 32 || next != L'Z') {
        t_error("wide binary length modifiers failed\n");
    }
}

/**
 * @tc.name      : isoc23_scanf_binary_1000
 * @tc.desc      : Verify binary length modifiers, suppression, and legacy swscanf.
 * @tc.level     : Level 0
 */
static void isoc23_scanf_binary_1000(void)
{
    check_wide_lengths();
    unsigned int u = 99;
    int count = -1;
    wchar_t next = L'?';
    int ret = __isoc23_swscanf(L"0b2Z", L"%2b%n%lc", &u, &count, &next);
    if (ret != 0 || u != 99 || count != -1 || next != L'?') {
        t_error("wide incomplete binary conversion failed\n");
    }
    int value = 99;
    ret = __isoc23_swscanf(L"0b101 0B11Z", L"%*b %i%n%lc", &value, &count, &next);
    if (ret != 2 || value != 3 || count != 10 || next != L'Z') {
        t_error("wide suppression failed\n");
    }
    count = -1;
    next = L'?';
    ret = __isoc23_swscanf(L"0b2Z", L"%*2i%n%lc", &count, &next);
    if (ret != 0 || count != -1 || next != L'?') {
        t_error("wide suppressed incomplete prefix failed\n");
    }
    ret = swscanf(L"0b101", L"%i%n%lc", &value, &count, &next);
    if (ret != 2 || value != 0 || count != 1 || next != L'b') {
        t_error("legacy wide binary prefix behavior changed\n");
    }
    u = 99;
    count = -1;
    ret = swscanf(L"0b101", L"%b%n", &u, &count);
    if (ret != EOF || u != 99 || count != -1) {
        t_error("legacy wide %%b format rejection changed\n");
    }
}

/**
 * @tc.name      : isoc23_scanf_binary_1100
 * @tc.desc      : Verify wide characters, floating point input, and buffer boundaries.
 * @tc.level     : Level 0
 */
static void isoc23_scanf_binary_1100(void)
{
    wchar_t word[3], next = 0;
    int value = 0, count = -1;
    double d = 0;
    const wchar_t *text = L"\u4e2d\u6587 0b101 0x1.8p+1 X";
    int ret = __isoc23_swscanf(text, L"%2ls %i %lf %lc%n", word, &value, &d, &next, &count);
    if (ret != 4 || wcscmp(word, L"\u4e2d\u6587") || value != 5 || d != 3 ||
        next != L'X' || count != (int)wcslen(text)) {
        t_error("wide string/float/count conversion failed\n");
    }

    wchar_t buf[608];
    for (int n=252; n<=258; n++) {
        wmemset(buf, L' ', n);
        wcscpy(buf+n, L"0b101Z");
        ret = __isoc23_swscanf(buf, L"%i%n%lc", &value, &count, &next);
        if (ret != 2 || value != 5 || count != n+5 || next != L'Z') {
            t_error("wide buffer boundary %d failed\n", n);
        }
    }
    wmemset(buf, L'\u3000', 600);
    wcscpy(buf+600, L"0b101Z");
    ret = __isoc23_swscanf(buf, L"%i%n%lc", &value, &count, &next);
    if (ret != 2 || value != 5 || count != 605 || next != L'Z') {
        t_error("wide multibyte whitespace/refill count failed\n");
    }
}

int main(void)
{
    for (enum scanner scanner = STRING; scanner <= VSTREAM; scanner++) {
        isoc23_scanf_binary_0100(scanner);
        isoc23_scanf_binary_0200(scanner);
        isoc23_scanf_binary_0300(scanner);
        isoc23_scanf_binary_0400(scanner);
        isoc23_scanf_binary_0500(scanner);
        isoc23_scanf_binary_0600(scanner);
    }
    isoc23_scanf_binary_0700();
    isoc23_scanf_binary_0800();
    if (!setlocale(LC_ALL, "C.UTF-8")) {
        t_error("C.UTF-8 locale unavailable\n");
    } else {
        isoc23_scanf_binary_0900();
        isoc23_scanf_binary_1000();
        isoc23_scanf_binary_1100();
    }
    return t_status;
}
