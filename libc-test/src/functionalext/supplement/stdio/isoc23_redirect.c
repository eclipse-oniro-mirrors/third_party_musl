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

/* By default, opt in to C23 conversion redirection alongside GNU extensions.
 * Override TEST_C23 to exercise other feature and language configurations. */
#ifndef TEST_C23
#define TEST_C23 1
#define _OHOS_ISOC23_CONVERSION_SOURCE 1
#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif
#endif
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdarg.h>
#include <inttypes.h>
#include <stdio.h>
#include <unistd.h>
#include <wchar.h>
#include "isoc23_scanf_test.h"

/* Format macros are available independently of scanner redirection. */
#if !defined(SCNb8) \
 || !defined(SCNb16) \
 || !defined(SCNb32) \
 || !defined(SCNb64) \
 || !defined(SCNbLEAST8) \
 || !defined(SCNbLEAST16) \
 || !defined(SCNbLEAST32) \
 || !defined(SCNbLEAST64) \
 || !defined(SCNbFAST8) \
 || !defined(SCNbFAST16) \
 || !defined(SCNbFAST32) \
 || !defined(SCNbFAST64) \
 || !defined(SCNbMAX) \
 || !defined(SCNbPTR)
#error C23 binary scanf macros are missing
#endif

static void check_scan_result(const char *name, int ret, int value, int count)
{
    if (ret != 1 || value != (TEST_C23 ? 5 : 0) || count != (TEST_C23 ? 5 : 1)) {
        t_error("%s: ret=%d value=%d count=%d\n", name, ret, value, count);
    }
}

static int scan_string(const char *text, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int ret = vsscanf(text, fmt, ap);
    va_end(ap);
    return ret;
}

static int scan_file(FILE *fp, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int ret = vfscanf(fp, fmt, ap);
    va_end(ap);
    return ret;
}

static void check_string_redirect(void)
{
    int (*read_string)(const char *, const char *, ...) = sscanf;
#if TEST_C23
    int32_t exact = 0;
    int_fast16_t fast = 0;
    if (read_string("42", "%w32d", &exact) != 1 || exact != 42) {
        t_error("sscanf length modifier was not redirected\n");
    }
    if (swscanf(L"43", L"%wf16d", &fast) != 1 || fast != 43) {
        t_error("swscanf fast length modifier was not redirected\n");
    }
#endif
    int value = -1, count = -1;
    int ret = sscanf("0b101", "%i%n", &value, &count);
    check_scan_result("sscanf", ret, value, count);
    value = count = -1;
    ret = read_string("0b101", "%i%n", &value, &count);
    check_scan_result("sscanf function pointer", ret, value, count);
    value = count = -1;
    ret = scan_string("0b101", "%i%n", &value, &count);
    check_scan_result("vsscanf", ret, value, count);
    value = count = -1;
    ret = swscanf(L"0b101", L"%i%n", &value, &count);
    check_scan_result("swscanf", ret, value, count);
}

static void check_stdin_redirect(FILE *fp)
{
    int value, count, ret;
    int saved = dup(0);
    if (saved < 0) {
        t_error("cannot save stdin\n");
    } else {
        if (dup2(fileno(fp), 0) < 0) {
            t_error("cannot redirect stdin\n");
        } else {
            clearerr(stdin);
            value = count = -1;
            ret = scanf("%i%n", &value, &count);
            check_scan_result("scanf", ret, value, count);
            if (dup2(saved, 0) < 0) {
                t_error("cannot restore stdin\n");
            }
            clearerr(stdin);
        }
        close_scan_fd(saved);
    }
}

static void check_file_redirect(void)
{
    int value, count, ret;
    FILE *fp = open_scan_file();
    if (!fp) {
        return;
    }
    if (fputs("0b101", fp) < 0 || fflush(fp)) {
        t_error("cannot write input\n");
        close_scan_file(fp);
        return;
    }
    if (fseek(fp, 0, SEEK_SET) != 0) {
        t_error("cannot rewind input\n");
        goto cleanup;
    }
    value = count = -1;
    ret = fscanf(fp, "%i%n", &value, &count);
    check_scan_result("fscanf", ret, value, count);
    if (fseek(fp, 0, SEEK_SET) != 0) {
        t_error("cannot rewind input\n");
        goto cleanup;
    }
    value = count = -1;
    ret = scan_file(fp, "%i%n", &value, &count);
    check_scan_result("vfscanf", ret, value, count);

    if (fseek(fp, 0, SEEK_SET) != 0) {
        t_error("cannot rewind input\n");
        goto cleanup;
    }
    check_stdin_redirect(fp);
cleanup:
    close_scan_file(fp);
}

/**
 * @tc.name      : isoc23_redirect_0100
 * @tc.desc      : Verify scanf redirection follows feature macros and language mode.
 * @tc.level     : Level 0
 */
static void isoc23_redirect_0100(void)
{
    check_string_redirect();
    check_file_redirect();
}

/* GNU extensions alone must not enable the extra glibc scanf behavior. */
static void isoc23_redirect_0200(void)
{
    char output[3] = {'?', '?', '?'};
    int count = -1;
    int ret = sscanf("A", "%3c%n", output, &count);
    if (ret != (TEST_C23 ? 1 : 0) || count != (TEST_C23 ? 1 : -1)) {
        t_error("sscanf short character conversion selected the wrong behavior\n");
    }

    void *pointer = output;
    count = -1;
    ret = sscanf("(nil)", "%p%n", &pointer, &count);
    if (ret != (TEST_C23 ? 1 : 0) || count != (TEST_C23 ? 5 : -1) ||
        pointer != (TEST_C23 ? 0 : (void *)output)) {
        t_error("sscanf null pointer conversion selected the wrong behavior\n");
    }
}

int main(void)
{
    isoc23_redirect_0100();
    isoc23_redirect_0200();
    return t_status;
}
