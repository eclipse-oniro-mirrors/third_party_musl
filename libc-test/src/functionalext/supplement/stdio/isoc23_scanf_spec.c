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

/* SPEC: design-docs PR 1735, US-06 (AC-06.1 through AC-06.6).
 * No conversion opt-in macro: explicit C23 declarations must remain usable. */
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <errno.h>
#include <stdarg.h>
#include <sys/wait.h>
#include <wchar.h>
#include "isoc23_scanf_test.h"

static int scan_vstring(const char *text, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    int ret = __isoc23_vsscanf(text, format, ap);
    va_end(ap);
    return ret;
}

static int scan_vfile(FILE *fp, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    int ret = __isoc23_vfscanf(fp, format, ap);
    va_end(ap);
    return ret;
}

/* AC-06.2: %b has an unsigned destination, with or without a binary prefix. */
static void isoc23_scanf_spec_0100(void)
{
    FILE *fp = open_scan_file();
    if (!fp) {
        return;
    }
    if (fputs("1010 0B1010", fp) < 0 || fseek(fp, 0, SEEK_SET)) {
        t_error("cannot prepare binary input\n");
        close_scan_file(fp);
        return;
    }
    unsigned int value = 99;
    int count = -1;
    errno = EDOM;
    int ret = __isoc23_fscanf(fp, "%b%n", &value, &count);
    if (ret != 1 || value != 10 || count != 4 || errno != EDOM || ferror(fp)) {
        t_error("AC-06.2: fscanf binary value, count or errno mismatch\n");
    }
    value = 99;
    count = -1;
    errno = EDOM;
    ret = scan_vfile(fp, "%b%n", &value, &count);
    if (ret != 1 || value != 10 || count != 7 || errno != EDOM || ferror(fp)) {
        t_error("AC-06.2: vfscanf binary value, count or errno mismatch\n");
    }
    close_scan_file(fp);
}

/* AC-06.3/06.4, BR-03: va_list, ordinary string and wide string wrappers. */
static void isoc23_scanf_spec_0200(void)
{
    int a = -7, b = -9;
    errno = EDOM;
    int ret = scan_vstring("42 99", "%d %d", &a, &b);
    if (ret != 2 || a != 42 || b != 99 || errno != EDOM) {
        t_error("AC-06.3: vsscanf assignments or errno mismatch\n");
    }
    a = -7;
    b = -9;
    errno = EDOM;
    ret = __isoc23_sscanf("42 99", "%d %d", &a, &b);
    if (ret != 2 || a != 42 || b != 99 || errno != EDOM) {
        t_error("AC-06.3: sscanf assignments or errno mismatch\n");
    }
    a = -7;
    errno = EDOM;
    ret = __isoc23_swscanf(L"42", L"%d", &a);
    if (ret != 1 || a != 42 || errno != EDOM) {
        t_error("AC-06.4: swscanf assignment or errno mismatch\n");
    }
}

struct stdin_case {
    const char *ac;
    const char *text;
    const char *format;
    int ret;
    int first;
    int eof;
};

static void check_stdin_case(const struct stdin_case *test)
{
    FILE *fp = open_scan_file();
    if (!fp) {
        return;
    }
    if (fputs(test->text, fp) < 0 || fseek(fp, 0, SEEK_SET)) {
        t_error("%s: cannot prepare stdin input\n", test->ac);
        close_scan_file(fp);
        return;
    }
    /* Each child gets fresh stdin state. Unread bytes from a mismatch cannot
     * contaminate another case or the parent's stdin buffer/EOF indicators. */
    pid_t child = fork();
    if (child < 0) {
        t_error("%s: fork failed\n", test->ac);
    } else if (child == 0) {
        if (dup2(fileno(fp), STDIN_FILENO) < 0) {
            t_error("%s: cannot redirect stdin\n", test->ac);
            _Exit(1);
        }
        clearerr(stdin);
        int a = -7, b = -9;
        errno = EDOM;
        int ret = __isoc23_scanf(test->format, &a, &b);
        int saved_errno = errno;
        if (ret != test->ret || a != test->first || b != -9 ||
            !!feof(stdin) != test->eof || ferror(stdin) || saved_errno != EDOM) {
            t_error("%s: ret=%d a=%d b=%d eof=%d error=%d errno=%d\n",
                test->ac, ret, a, b, !!feof(stdin), !!ferror(stdin), saved_errno);
        }
        _Exit(t_status ? 1 : 0);
    } else {
        int status;
        pid_t waited;
        do {
            waited = waitpid(child, &status, 0);
        } while (waited < 0 && errno == EINTR);
        if (waited != child || !WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            t_error("%s: stdin child failed\n", test->ac);
        }
    }
    close_scan_file(fp);
}

/* AC-06.1/06.5/06.6 and EB-12/13/14: return value AND destination/stream state. */
static void isoc23_scanf_spec_0300(void)
{
    const struct stdin_case cases[] = {
        {"AC-06.1", "42", "%d", 1, 42, 1},
        {"AC-06.5 EOF", "", "%d", EOF, -7, 1},
        {"AC-06.5 mismatch", "abc", "%d", 0, -7, 0},
        {"AC-06.6", "42", "%d %d", 1, 42, 1},
    };
    for (size_t i = 0; i < sizeof cases / sizeof *cases; i++) {
        check_stdin_case(&cases[i]);
    }
}

int main(void)
{
    isoc23_scanf_spec_0100();
    isoc23_scanf_spec_0200();
    isoc23_scanf_spec_0300();
    return t_status;
}
