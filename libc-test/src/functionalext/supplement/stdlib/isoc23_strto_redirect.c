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

#include <inttypes.h>
#include <locale.h>
#include <stdlib.h>
#include <wchar.h>
#include "test.h"

static void check_conversion_result(const char *name, int passed, const char *kind)
{
    if (passed == 0) {
        t_error("%s %s did not select the expected entry point\n", name, kind);
    }
}

#define CHECK_CONVERSION(fn, text, type) do { \
    type *end; \
    __typeof__(&(fn)) convert = (fn); \
    const type *input = (text); \
    check_conversion_result(#fn, (fn)(input, &end, 0) == (TEST_C23 ? 5 : 0) && \
        end - input == (TEST_C23 ? 5 : 1), "call"); \
    check_conversion_result(#fn, convert(input, &end, 2) == (TEST_C23 ? 5 : 0) && \
        end - input == (TEST_C23 ? 5 : 1), "function pointer"); \
} while (0)

static void check_conversions(void)
{
    CHECK_CONVERSION(strtol, "0b101", char);
    CHECK_CONVERSION(strtoul, "0b101", char);
    CHECK_CONVERSION(strtoll, "0b101", char);
    CHECK_CONVERSION(strtoull, "0b101", char);
    CHECK_CONVERSION(strtoimax, "0b101", char);
    CHECK_CONVERSION(strtoumax, "0b101", char);
    CHECK_CONVERSION(wcstol, L"0b101", wchar_t);
    CHECK_CONVERSION(wcstoul, L"0b101", wchar_t);
    CHECK_CONVERSION(wcstoll, L"0b101", wchar_t);
    CHECK_CONVERSION(wcstoull, L"0b101", wchar_t);
}

#if defined(_GNU_SOURCE) && (TEST_C23 || defined(MUSL_EXTERNAL_FUNCTION))
static void check_locale_conversions(void)
{
    locale_t loc = newlocale(LC_ALL_MASK, "C", NULL);
    if (!loc) {
        t_error("newlocale failed\n");
        return;
    }
    const char *text = "0b101";
    char *end;
    long long (*convert_signed)(const char *, char **, int, locale_t) = strtoll_l;
    unsigned long long (*convert_unsigned)(const char *, char **, int, locale_t) = strtoull_l;
    if (strtoll_l(text, &end, 0, loc) != (TEST_C23 ? 5 : 0) ||
        end != text + (TEST_C23 ? 5 : 1)) {
        t_error("strtoll_l did not select the expected entry point\n");
    }
    if (strtoull_l(text, &end, 2, loc) != (TEST_C23 ? 5 : 0) ||
        end != text + (TEST_C23 ? 5 : 1)) {
        t_error("strtoull_l did not select the expected entry point\n");
    }
    if (convert_signed(text, &end, 0, loc) != (TEST_C23 ? 5 : 0) ||
        end != text + (TEST_C23 ? 5 : 1)) {
        t_error("strtoll_l function pointer selected the wrong entry point\n");
    }
    if (convert_unsigned(text, &end, 2, loc) != (TEST_C23 ? 5 : 0) ||
        end != text + (TEST_C23 ? 5 : 1)) {
        t_error("strtoull_l function pointer selected the wrong entry point\n");
    }
    freelocale(loc);
}
#endif

int main(void)
{
    check_conversions();
#if defined(_GNU_SOURCE) && (TEST_C23 || defined(MUSL_EXTERNAL_FUNCTION))
    check_locale_conversions();
#endif
    return t_status;
}
