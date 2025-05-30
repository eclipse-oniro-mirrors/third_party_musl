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

#include <ctype.h>
#include <stdio.h>
#include <wchar.h>
#include <wctype.h>
#include <locale.h>

#define XDIGIT_WINT_COUNT 704
#define UNICODE_SIZE 1114111
#define MAX_VALUE 4294967295

#include "functionalext.h"
#include "test.h"

/**
 * @tc.name      : iswxdigit_0100
 * @tc.desc      : Use the iswxdigit method to determine whether the incoming wide character is a hexadecimal number
 * @tc.level     : Level 0
 */
void iswxdigit_0100(void)
{
    wchar_t ch = L'A';
    int result = iswxdigit(ch);
    if (result == 0) {
        t_error("%s iswxdigit get result is %d error", __func__, result);
    }
}

/**
 * @tc.name      : iswxdigit_0200
 * @tc.desc      : Incoming digit test iswxdigit return value
 * @tc.level     : Level 1
 */
void iswxdigit_0200(void)
{
    wchar_t ch = L'1';
    int result = iswxdigit(ch);
    if (result == 0) {
        t_error("%s iswxdigit get result is %d error", __func__, result);
    }
}

/**
 * @tc.name      : iswxdigit_0300
 * @tc.desc      : Pass in letters over F to test iswxdigit return value
 * @tc.level     : Level 1
 */
void iswxdigit_0300(void)
{
    wchar_t ch = L'G';
    int result = iswxdigit(ch);
    if (result != 0) {
        t_error("%s iswxdigit get result is %d error", __func__, result);
    }
}

/**
 * @tc.name      : iswxdigit_l_0100
 * @tc.desc      : Whether the characters in a wide string is hexadecimal digit character
 * @tc.level     : Level 0
 */
void iswxdigit_l_0100(void)
{
    const wchar_t *str = L"0123456789AaBbCcDdEeFf";
    const wchar_t *p = str;
    while (*p++ && *p != '\0') {
        int ret = iswxdigit_l(*p, NULL);
        EXPECT_EQ("iswxdigit_l_0100", ret, ONREXPECT);
    }
}

/**
 * @tc.name      : iswxdigit_l_0101
 * @tc.desc      : Whether the characters in a wide string is hexadecimal digit character
 * @tc.level     : Level 0
 */
void iswxdigit_l_0101(void)
{
    locale_t m_locale = newlocale(LC_CTYPE_MASK, "en_US.UTF-8", NULL);
    const wchar_t *str = L"0123456789AaBbCcDdEeFf";
    const wchar_t *p = str;
    while (*p++ && *p != '\0') {
        int ret = iswxdigit_l(*p, m_locale);
        EXPECT_EQ("iswxdigit_l_0101", ret, ONREXPECT);
    }
    freelocale(m_locale);
}

/**
 * @tc.name      : iswxdigit_l_0102
 * @tc.desc      : Whether the characters in a wide string is hexadecimal digit character
 * @tc.level     : Level 0
 */
void iswxdigit_l_0102(void)
{
    locale_t m_locale = newlocale(LC_CTYPE_MASK, "zh_CN", NULL);
    const wchar_t *str = L"0123456789AaBbCcDdEeFf";
    const wchar_t *p = str;
    while (*p++ && *p != '\0') {
        int ret = iswxdigit_l(*p, m_locale);
        EXPECT_EQ("iswxdigit_l_0102", ret, ONREXPECT);
    }
    freelocale(m_locale);
}

/**
 * @tc.name      : iswxdigit_l_0200
 * @tc.desc      : Whether the characters in a wide string is non-hexadecimal digit character
 * @tc.level     : Level 2
 */
void iswxdigit_l_0200(void)
{
    const wchar_t *str = L"!@hHiIjJZz";
    const wchar_t *p = str;
    while (*p++ && *p != '\0') {
        int ret = iswxdigit_l(*p, NULL);
        EXPECT_EQ("iswxdigit_l_0200", ret, CMPFLAG);
    }
}

/**
 * @tc.name      : iswxdigit_l_0201
 * @tc.desc      : Whether the characters in a wide string is non-hexadecimal digit character
 * @tc.level     : Level 2
 */
void iswxdigit_l_0201(void)
{
    locale_t m_locale = newlocale(LC_CTYPE_MASK, "en_US.UTF-8", NULL);
    const wchar_t *str = L"!@hHiIjJZz";
    const wchar_t *p = str;
    while (*p++ && *p != '\0') {
        int ret = iswxdigit_l(*p, m_locale);
        EXPECT_EQ("iswxdigit_l_0201", ret, CMPFLAG);
    }
    freelocale(m_locale);
}

/**
 * @tc.name      : iswxdigit_l_0202
 * @tc.desc      : Whether the characters in a wide string is non-hexadecimal digit character
 * @tc.level     : Level 2
 */
void iswxdigit_l_0202(void)
{
    locale_t m_locale = newlocale(LC_CTYPE_MASK, "zh_CN", NULL);
    const wchar_t *str = L"!@hHiIjJZz";
    const wchar_t *p = str;
    while (*p++ && *p != '\0') {
        int ret = iswxdigit_l(*p, m_locale);
        EXPECT_EQ("iswxdigit_l_0202", ret, CMPFLAG);
    }
    freelocale(m_locale);
}

/**
 * @tc.name      : iswxdigit_l_0300
 * @tc.desc      : Whether the characters in a wide string is non-hexadecimal digit character
 * @tc.level     : Level 2
 */
void iswxdigit_l_0300(void)
{
    locale_t m_locale = newlocale(LC_CTYPE_MASK, "en_US.UTF-8", NULL);
    int total = 0;
    for (int i = 0; i < UNICODE_SIZE; i++) {
        int ret = iswxdigit_l(i, m_locale);
        if (ret) {
            total++;
        }
    }
    freelocale(m_locale);
    EXPECT_EQ("iswxdigit_l_0300", total, XDIGIT_WINT_COUNT);
}

/**
 * @tc.name      : iswxdigit_l_0400
 * @tc.desc      : Whether the characters in a wide string is non-hexadecimal digit character
 * @tc.level     : Level 2
 */
void iswxdigit_l_0400(void)
{
    locale_t m_locale = newlocale(LC_CTYPE_MASK, "zh_CN", NULL);
    int total = 0;
    for (int i = 0; i < UNICODE_SIZE; i++) {
        int ret = iswxdigit_l(i, m_locale);
        if (ret) {
            total++;
        }
    }
    freelocale(m_locale);
    EXPECT_EQ("iswxdigit_l_0400", total, XDIGIT_WINT_COUNT);
}

/**
 * @tc.name      : iswxdigit_l_0500
 * @tc.desc      : Whether a character in a the maximum value of int is not a letter
 * @tc.level     : Level 1
 */
 void iswxdigit_l_0500(void)
 {
    locale_t m_locale = newlocale(LC_CTYPE_MASK, "zh_CN", NULL);
    int ret = iswxdigit_l(MAX_VALUE, m_locale);
    freelocale(m_locale);
    EXPECT_EQ("iswxdigit_l_0500", ret, CMPFLAG);
 }

/**
* @tc.name      : iswxdigit_l_0600
* @tc.desc      : Whether a character in a the maximum value of int is not a letter
* @tc.level     : Level 1
*/
void iswxdigit_l_0600(void)
{
    locale_t m_locale = newlocale(LC_CTYPE_MASK, "en_US.UTF-8", NULL);
    int ret = iswxdigit_l(MAX_VALUE, m_locale);
    freelocale(m_locale);
    EXPECT_EQ("iswxdigit_l_0600", ret, CMPFLAG);
}

int main(int argc, char *argv[])
{
    set_wctype_icu_enable();
    iswxdigit_0100();
    iswxdigit_0200();
    iswxdigit_0300();
    iswxdigit_l_0100();
    iswxdigit_l_0101();
    iswxdigit_l_0102();
    iswxdigit_l_0200();
    iswxdigit_l_0201();
    iswxdigit_l_0202();
    iswxdigit_l_0300();
    iswxdigit_l_0400();
    iswxdigit_l_0500();
    iswxdigit_l_0600();

    return t_status;
}