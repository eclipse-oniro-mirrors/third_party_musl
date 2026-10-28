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
#include <stdio.h>
#include <wchar.h>
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
 * @tc.name      : isoc23_wcstol_0100
 * @tc.desc      : Verify __isoc23_wcstol accepts lower-case binary prefix
 * @tc.level     : Level 0
 */
void isoc23_wcstol_0100(void)
{
    long result;
    wchar_t *end = NULL;
    TEST(result, __isoc23_wcstol(L"0b101", &end, 0), 5L, "%ld != %ld");
    if (wcscmp(end, L"") != 0) {
        t_error("%s end is %ls are not want empty string\n", __func__, end);
    }
}

/**
 * @tc.name      : isoc23_wcstol_0200
 * @tc.desc      : Verify __isoc23_wcstol accepts upper-case binary prefix
 * @tc.level     : Level 0
 */
void isoc23_wcstol_0200(void)
{
    long result;
    wchar_t *end = NULL;
    TEST(result, __isoc23_wcstol(L"0B101", &end, 0), 5L, "%ld != %ld");
    if (wcscmp(end, L"") != 0) {
        t_error("%s end is %ls are not want empty string\n", __func__, end);
    }
}

/**
 * @tc.name      : isoc23_wcstol_0300
 * @tc.desc      : Verify __isoc23_wcstol accepts negative binary prefix
 * @tc.level     : Level 0
 */
void isoc23_wcstol_0300(void)
{
    long result;
    wchar_t *end = NULL;
    TEST(result, __isoc23_wcstol(L"-0b11111", &end, 0), -31L, "%ld != %ld");
    if (wcscmp(end, L"") != 0) {
        t_error("%s end is %ls are not want empty string\n", __func__, end);
    }
}

/**
 * @tc.name      : isoc23_wcstol_0400
 * @tc.desc      : Verify __isoc23_wcstoul accepts binary prefix
 * @tc.level     : Level 0
 */
void isoc23_wcstol_0400(void)
{
    unsigned long result;
    wchar_t *end = NULL;
    TEST(result, __isoc23_wcstoul(L"0B101", &end, 0), 5UL, "%lu != %lu");
    if (wcscmp(end, L"") != 0) {
        t_error("%s end is %ls are not want empty string\n", __func__, end);
    }
}

/**
 * @tc.name      : isoc23_wcstol_0500
 * @tc.desc      : Verify __isoc23_wcstoll accepts binary prefix
 * @tc.level     : Level 0
 */
void isoc23_wcstol_0500(void)
{
    long long result;
    wchar_t *end = NULL;
    TEST(result, __isoc23_wcstoll(L"0b101", &end, 0), 5LL, "%lld != %lld");
    if (wcscmp(end, L"") != 0) {
        t_error("%s end is %ls are not want empty string\n", __func__, end);
    }
}

/**
 * @tc.name      : isoc23_wcstol_0600
 * @tc.desc      : Verify __isoc23_wcstoull accepts binary prefix
 * @tc.level     : Level 0
 */
void isoc23_wcstol_0600(void)
{
    unsigned long long result;
    wchar_t *end = NULL;
    TEST(result, __isoc23_wcstoull(L"0B101", &end, 0), 5ULL, "%llu != %llu");
    if (wcscmp(end, L"") != 0) {
        t_error("%s end is %ls are not want empty string\n", __func__, end);
    }
}

/**
 * @tc.name      : isoc23_wcstol_0700
 * @tc.desc      : Verify __isoc23_wcstol accepts binary prefix with explicit base 2
 * @tc.level     : Level 0
 */
void isoc23_wcstol_0700(void)
{
    long result;
    wchar_t *end = NULL;
    TEST(result, __isoc23_wcstol(L"0b101", &end, 2), 5L, "%ld != %ld");
    if (wcscmp(end, L"") != 0) {
        t_error("%s end is %ls are not want empty string\n", __func__, end);
    }
}

/**
 * @tc.name      : isoc23_wcstol_0800
 * @tc.desc      : Verify __isoc23_wcstol stops at incomplete binary prefix
 * @tc.level     : Level 1
 */
void isoc23_wcstol_0800(void)
{
    int offset;
    long result;
    wchar_t *start, *end = NULL;
    TEST(result, __isoc23_wcstol(start = L"0b", &end, 0), 0L, "%ld != %ld");
    TEST2(offset, "end - start", end - start, 1, "wrong final position %d != %d");
    if (wcscmp(end, L"b") != 0) {
        t_error("%s end is %ls are not want b\n", __func__, end);
    }
}

/**
 * @tc.name      : isoc23_wcstol_0900
 * @tc.desc      : Verify legacy wcstol does not accept binary prefix
 * @tc.level     : Level 1
 */
void isoc23_wcstol_0900(void)
{
    int offset;
    long result;
    wchar_t *start, *end = NULL;
    TEST(result, wcstol(start = L"0b101", &end, 0), 0L, "%ld != %ld");
    TEST2(offset, "end - start", end - start, 1, "wrong final position %d != %d");
    TEST2(offset, "errno", errno, 0, "%d != %d");
    if (wcscmp(end, L"b101") != 0) {
        t_error("%s end is %ls are not want b101\n", __func__, end);
    }
}

int main(int argc, char *argv[])
{
    isoc23_wcstol_0100();
    isoc23_wcstol_0200();
    isoc23_wcstol_0300();
    isoc23_wcstol_0400();
    isoc23_wcstol_0500();
    isoc23_wcstol_0600();
    isoc23_wcstol_0700();
    isoc23_wcstol_0800();
    isoc23_wcstol_0900();
    return t_status;
}
