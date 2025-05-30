/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <stdio.h>
#include <ctype.h>
#include <locale.h>
#include <wchar.h>
#include <wctype.h>
#include "functionalext.h"
#include "test.h"

#define CNTRL_WINT_COUNT 237
#define UNICODE_SIZE 1114111
#define MAX_VALUE 4294967295

void iswcntrl_test(wchar_t ch, int want, char *func_name)
{
    int result = iswcntrl(ch);
    if (result != want) {
        t_error("%s iswcntrl get result is %d are not %d\n", func_name, result, want);
    }
}

/**
 * @tc.name      : iswcntrl_l_0100
 * @tc.desc      : Whether a character in a wide string is a control character
 * @tc.level     : Level 0
 */
void iswcntrl_l_0100(void)
{
    const wchar_t *str = L"\b";
    const wchar_t *p = str;
    while (*p++ && *p != '\0') {
        int ret = iswcntrl_l(*p, NULL);
        EXPECT_EQ("iswcntrl_l_0100", ret, ONREXPECT);
    }
}

/**
 * @tc.name      : iswcntrl_l_0101
 * @tc.desc      : Whether a character in a wide string is a control character
 * @tc.level     : Level 0
 */
void iswcntrl_l_0101(void)
{
    locale_t m_locale = newlocale(LC_CTYPE_MASK, "en_US.UTF-8", NULL);
    const wchar_t *str = L"\b";
    const wchar_t *p = str;
    while (*p++ && *p != '\0') {
        int ret = iswcntrl_l(*p, m_locale);
        EXPECT_EQ("iswcntrl_l_0101", ret, ONREXPECT);
    }
    freelocale(m_locale);
}

/**
 * @tc.name      : iswcntrl_l_0102
 * @tc.desc      : Whether a character in a wide string is a control character
 * @tc.level     : Level 0
 */
void iswcntrl_l_0102(void)
{
    locale_t m_locale = newlocale(LC_CTYPE_MASK, "zh_CN", NULL);
    const wchar_t *str = L"\b";
    const wchar_t *p = str;
    while (*p++ && *p != '\0') {
        int ret = iswcntrl_l(*p, m_locale);
        EXPECT_EQ("iswcntrl_l_0102", ret, ONREXPECT);
    }
    freelocale(m_locale);
}

/**
 * @tc.name      : iswcntrl_l_0200
 * @tc.desc      : Whether a character in a wide string is a non-control character
 * @tc.level     : Level 2
 */
void iswcntrl_l_0200(void)
{
    const wchar_t *str = L"2!~*3Ad";
    const wchar_t *p = str;
    while (*p++ && *p != '\0') {
        int ret = iswcntrl_l(*p, NULL);
        EXPECT_EQ("iswcntrl_l_0200", ret, CMPFLAG);
    }
}

/**
 * @tc.name      : iswcntrl_l_0201
 * @tc.desc      : Whether a character in a wide string is a non-control character
 * @tc.level     : Level 2
 */
void iswcntrl_l_0201(void)
{
    locale_t m_locale = newlocale(LC_CTYPE_MASK, "en_US.UTF-8", NULL);
    const wchar_t *str = L"2!~*3Ad";
    const wchar_t *p = str;
    while (*p++ && *p != '\0') {
        int ret = iswcntrl_l(*p, m_locale);
        EXPECT_EQ("iswcntrl_l_0201", ret, CMPFLAG);
    }
    freelocale(m_locale);
}

/**
 * @tc.name      : iswcntrl_l_0202
 * @tc.desc      : Whether a character in a wide string is a non-control character
 * @tc.level     : Level 2
 */
void iswcntrl_l_0202(void)
{
    locale_t m_locale = newlocale(LC_CTYPE_MASK, "zh_CN", NULL);
    const wchar_t *str = L"2!~*3Ad";
    const wchar_t *p = str;
    while (*p++ && *p != '\0') {
        int ret = iswcntrl_l(*p, m_locale);
        EXPECT_EQ("iswcntrl_l_0202", ret, CMPFLAG);
    }
    freelocale(m_locale);
}

/**
 * @tc.name      : iswcntrl_l_0300
 * @tc.desc      : Whether a character in a wide string is a non-control character
 * @tc.level     : Level 2
 */
void iswcntrl_l_0300(void)
{
    locale_t m_locale = newlocale(LC_CTYPE_MASK, "en_US.UTF-8", NULL);
    int total = 0;
    for (int i = 0; i < UNICODE_SIZE; i++) {
        int ret = iswcntrl_l(i, m_locale);
        if (ret) {
            total++;
        }
    }
    freelocale(m_locale);
    EXPECT_EQ("iswcntrl_l_0300", total, CNTRL_WINT_COUNT);
}

/**
 * @tc.name      : iswcntrl_l_0400
 * @tc.desc      : Whether a character in a wide string is a non-control character
 * @tc.level     : Level 2
 */
void iswcntrl_l_0400(void)
{
    locale_t m_locale = newlocale(LC_CTYPE_MASK, "zh_CN", NULL);
    int total = 0;
    for (int i = 0; i < UNICODE_SIZE; i++) {
        int ret = iswcntrl_l(i, m_locale);
        if (ret) {
            total++;
        }
    }
    freelocale(m_locale);
    EXPECT_EQ("iswcntrl_l_0400", total, CNTRL_WINT_COUNT);
}

/**
 * @tc.name      : iswcntrl_l_0500
 * @tc.desc      : Whether a character in a the maximum value of int is not a letter
 * @tc.level     : Level 1
 */
 void iswcntrl_l_0500(void)
 {
    locale_t m_locale = newlocale(LC_CTYPE_MASK, "zh_CN", NULL);
    int ret = iswcntrl_l(MAX_VALUE, m_locale);
    freelocale(m_locale);
    EXPECT_EQ("iswcntrl_l_0500", ret, CMPFLAG);
 }

/**
* @tc.name      : iswcntrl_l_0600
* @tc.desc      : Whether a character in a the maximum value of int is not a letter
* @tc.level     : Level 1
*/
void iswcntrl_l_0600(void)
{
    locale_t m_locale = newlocale(LC_CTYPE_MASK, "en_US.UTF-8", NULL);
    int ret = iswcntrl_l(MAX_VALUE, m_locale);
    freelocale(m_locale);
    EXPECT_EQ("iswcntrl_l_0600", ret, CMPFLAG);
}

int main(int argc, char *argv[])
{
    set_wctype_icu_enable();
    char *ret = setlocale(LC_ALL, "en_US.utf8");
    if (!ret) {
        printf("\n");
    }
    /**
     * @tc.name      : iswcntrl_0100
     * @tc.desc      : Test if a wide character is a control character
     * @tc.level     : Level 0
     */
    iswcntrl_test(L'\n', 1, "iswcntrl_0100");
    /**
     * @tc.name      : iswcntrl_0200
     * @tc.desc      : Test whether the wide character is a control character, the incoming character is A
     * @tc.level     : Level 1
     */
    iswcntrl_test(L'A', 0, "iswcntrl_0200");
    /**
     * @tc.name      : iswcntrl_0300
     * @tc.desc      : Test whether the wide character is a control character, the incoming character is \u2028
     * @tc.level     : Level 1
     */
    iswcntrl_test(L'\u2028', 1, "iswcntrl_0300");
    /**
     * @tc.name      : iswcntrl_0400
     * @tc.desc      : Test whether the wide character is a control character, the incoming character is \ufff9
     * @tc.level     : Level 1
     */
    iswcntrl_test(L'\ufff9', 1, "iswcntrl_0400");

    iswcntrl_l_0100();
    iswcntrl_l_0101();
    iswcntrl_l_0102();
    iswcntrl_l_0200();
    iswcntrl_l_0201();
    iswcntrl_l_0202();
    iswcntrl_l_0300();
    iswcntrl_l_0400();
    iswcntrl_l_0500();
    iswcntrl_l_0600();
    return t_status;
}