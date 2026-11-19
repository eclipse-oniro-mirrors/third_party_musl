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

package ohos.musl.ctype;

import ohos.musl.Assert;

/**
 * {@link CtypeFuncs} 的测试套件。期望值对照 musl 默认（非 ICU）路径的 ctype 宏/函数。
 */
public final class CtypeFuncsTest {

    private static int failures = 0;

    private CtypeFuncsTest() {
    }

    private static void check(Runnable r) {
        try {
            r.run();
        } catch (AssertionError e) {
            failures++;
            System.err.println("  FAIL " + e.getMessage());
        }
    }

    private static void testIsdigit() {
        Assert.assertTrue(CtypeFuncs.isdigit('0'), "isdigit('0')");
        Assert.assertTrue(CtypeFuncs.isdigit('9'), "isdigit('9')");
        Assert.assertTrue(CtypeFuncs.isdigit('5'), "isdigit('5')");
        Assert.assertTrue(!CtypeFuncs.isdigit('A'), "isdigit('A') false");
        Assert.assertTrue(!CtypeFuncs.isdigit('/'), "isdigit('/') false");
        Assert.assertTrue(!CtypeFuncs.isdigit(':'), "isdigit(':') false");
        Assert.assertTrue(!CtypeFuncs.isdigit(-1), "isdigit(EOF) false");
        Assert.assertTrue(!CtypeFuncs.isdigit(0), "isdigit(0) false");
        Assert.assertTrue(!CtypeFuncs.isdigit(255), "isdigit(255) false");
    }

    private static void testIsxdigit() {
        Assert.assertTrue(CtypeFuncs.isxdigit('0'), "isxdigit('0')");
        Assert.assertTrue(CtypeFuncs.isxdigit('9'), "isxdigit('9')");
        Assert.assertTrue(CtypeFuncs.isxdigit('a'), "isxdigit('a')");
        Assert.assertTrue(CtypeFuncs.isxdigit('f'), "isxdigit('f')");
        Assert.assertTrue(CtypeFuncs.isxdigit('A'), "isxdigit('A')");
        Assert.assertTrue(CtypeFuncs.isxdigit('F'), "isxdigit('F')");
        Assert.assertTrue(!CtypeFuncs.isxdigit('g'), "isxdigit('g') false");
        Assert.assertTrue(!CtypeFuncs.isxdigit('G'), "isxdigit('G') false");
        Assert.assertTrue(!CtypeFuncs.isxdigit(':'), "isxdigit(':') false");
    }

    private static void testIsalpha() {
        Assert.assertTrue(CtypeFuncs.isalpha('A'), "isalpha('A')");
        Assert.assertTrue(CtypeFuncs.isalpha('Z'), "isalpha('Z')");
        Assert.assertTrue(CtypeFuncs.isalpha('a'), "isalpha('a')");
        Assert.assertTrue(CtypeFuncs.isalpha('z'), "isalpha('z')");
        Assert.assertTrue(!CtypeFuncs.isalpha('0'), "isalpha('0') false");
        Assert.assertTrue(!CtypeFuncs.isalpha('@'), "isalpha('@') false");
        Assert.assertTrue(!CtypeFuncs.isalpha('['), "isalpha('[') false");
    }

    private static void testIsalnum() {
        Assert.assertTrue(CtypeFuncs.isalnum('A'), "isalnum('A')");
        Assert.assertTrue(CtypeFuncs.isalnum('0'), "isalnum('0')");
        Assert.assertTrue(CtypeFuncs.isalnum('z'), "isalnum('z')");
        Assert.assertTrue(!CtypeFuncs.isalnum(' '), "isalnum(' ') false");
        Assert.assertTrue(!CtypeFuncs.isalnum('!'), "isalnum('!') false");
    }

    private static void testIsupper() {
        Assert.assertTrue(CtypeFuncs.isupper('A'), "isupper('A')");
        Assert.assertTrue(CtypeFuncs.isupper('Z'), "isupper('Z')");
        Assert.assertTrue(!CtypeFuncs.isupper('a'), "isupper('a') false");
        Assert.assertTrue(!CtypeFuncs.isupper('0'), "isupper('0') false");
        Assert.assertTrue(!CtypeFuncs.isupper('@'), "isupper('@') false");
        Assert.assertTrue(!CtypeFuncs.isupper('['), "isupper('[') false");
    }

    private static void testIslower() {
        Assert.assertTrue(CtypeFuncs.islower('a'), "islower('a')");
        Assert.assertTrue(CtypeFuncs.islower('z'), "islower('z')");
        Assert.assertTrue(!CtypeFuncs.islower('A'), "islower('A') false");
        Assert.assertTrue(!CtypeFuncs.islower('0'), "islower('0') false");
        Assert.assertTrue(!CtypeFuncs.islower('`'), "islower('`') false");
        Assert.assertTrue(!CtypeFuncs.islower('{'), "islower('{') false");
    }

    private static void testIsspace() {
        Assert.assertTrue(CtypeFuncs.isspace(' '), "isspace(' ')");
        Assert.assertTrue(CtypeFuncs.isspace('\t'), "isspace('\\t')");
        Assert.assertTrue(CtypeFuncs.isspace('\n'), "isspace('\\n')");
        Assert.assertTrue(CtypeFuncs.isspace('\r'), "isspace('\\r')");
        Assert.assertTrue(CtypeFuncs.isspace('\f'), "isspace('\\f')");
        Assert.assertTrue(CtypeFuncs.isspace(11), "isspace(\\v)");
        Assert.assertTrue(!CtypeFuncs.isspace('a'), "isspace('a') false");
        Assert.assertTrue(!CtypeFuncs.isspace('0'), "isspace('0') false");
        Assert.assertTrue(!CtypeFuncs.isspace(8), "isspace(\\b) false");
        Assert.assertTrue(!CtypeFuncs.isspace(14), "isspace(0x0E) false");
    }

    private static void testIsblank() {
        Assert.assertTrue(CtypeFuncs.isblank(' '), "isblank(' ')");
        Assert.assertTrue(CtypeFuncs.isblank('\t'), "isblank('\\t')");
        Assert.assertTrue(!CtypeFuncs.isblank('\n'), "isblank('\\n') false");
        Assert.assertTrue(!CtypeFuncs.isblank('a'), "isblank('a') false");
    }

    private static void testTolower() {
        Assert.assertEquals('a', CtypeFuncs.tolower('A'), "tolower('A')");
        Assert.assertEquals('z', CtypeFuncs.tolower('Z'), "tolower('Z')");
        Assert.assertEquals('m', CtypeFuncs.tolower('M'), "tolower('M')");
        Assert.assertEquals('a', CtypeFuncs.tolower('a'), "tolower('a') unchanged");
        Assert.assertEquals('0', CtypeFuncs.tolower('0'), "tolower('0') unchanged");
        Assert.assertEquals('@', CtypeFuncs.tolower('@'), "tolower('@') unchanged");
        Assert.assertEquals('[', CtypeFuncs.tolower('['), "tolower('[') unchanged");
    }

    private static void testToupper() {
        Assert.assertEquals('A', CtypeFuncs.toupper('a'), "toupper('a')");
        Assert.assertEquals('Z', CtypeFuncs.toupper('z'), "toupper('z')");
        Assert.assertEquals('M', CtypeFuncs.toupper('m'), "toupper('m')");
        Assert.assertEquals('A', CtypeFuncs.toupper('A'), "toupper('A') unchanged");
        Assert.assertEquals('0', CtypeFuncs.toupper('0'), "toupper('0') unchanged");
        Assert.assertEquals('`', CtypeFuncs.toupper('`'), "toupper('`') unchanged");
        Assert.assertEquals('{', CtypeFuncs.toupper('{'), "toupper('{') unchanged");
    }

    private static void testIsgraph() {
        Assert.assertTrue(CtypeFuncs.isgraph('a'), "isgraph('a')");
        Assert.assertTrue(CtypeFuncs.isgraph('1'), "isgraph('1')");
        Assert.assertTrue(CtypeFuncs.isgraph('!'), "isgraph('!')");
        Assert.assertTrue(!CtypeFuncs.isgraph(' '), "isgraph(' ') false");
        Assert.assertTrue(!CtypeFuncs.isgraph('\t'), "isgraph('\\t') false");
        Assert.assertTrue(!CtypeFuncs.isgraph(0x7F), "isgraph(DEL) false");
    }

    private static void testIsprint() {
        Assert.assertTrue(CtypeFuncs.isprint('a'), "isprint('a')");
        Assert.assertTrue(CtypeFuncs.isprint(' '), "isprint(' ')");
        Assert.assertTrue(CtypeFuncs.isprint(0x7E), "isprint('~')");
        Assert.assertTrue(!CtypeFuncs.isprint('\t'), "isprint('\\t') false");
        Assert.assertTrue(!CtypeFuncs.isprint(0x7F), "isprint(DEL) false");
    }

    private static void testIspunct() {
        Assert.assertTrue(CtypeFuncs.ispunct('!'), "ispunct('!')");
        Assert.assertTrue(CtypeFuncs.ispunct('.'), "ispunct('.')");
        Assert.assertTrue(!CtypeFuncs.ispunct('a'), "ispunct('a') false");
        Assert.assertTrue(!CtypeFuncs.ispunct('1'), "ispunct('1') false");
        Assert.assertTrue(!CtypeFuncs.ispunct(' '), "ispunct(' ') false");
    }

    private static void testIscntrl() {
        Assert.assertTrue(CtypeFuncs.iscntrl(0), "iscntrl(0)");
        Assert.assertTrue(CtypeFuncs.iscntrl('\t'), "iscntrl('\\t')");
        Assert.assertTrue(CtypeFuncs.iscntrl(0x1F), "iscntrl(0x1F)");
        Assert.assertTrue(CtypeFuncs.iscntrl(0x7F), "iscntrl(DEL)");
        Assert.assertTrue(!CtypeFuncs.iscntrl(' '), "iscntrl(' ') false");
        Assert.assertTrue(!CtypeFuncs.iscntrl('a'), "iscntrl('a') false");
    }

    private static void testIsascii() {
        Assert.assertTrue(CtypeFuncs.isascii(0), "isascii(0)");
        Assert.assertTrue(CtypeFuncs.isascii('a'), "isascii('a')");
        Assert.assertTrue(CtypeFuncs.isascii(0x7F), "isascii(0x7F)");
        Assert.assertTrue(!CtypeFuncs.isascii(0x80), "isascii(0x80) false");
        Assert.assertTrue(!CtypeFuncs.isascii(-1), "isascii(-1) false");
        Assert.assertTrue(!CtypeFuncs.isascii(255), "isascii(255) false");
    }

    private static void testToascii() {
        Assert.assertEquals('a', CtypeFuncs.toascii('a'), "toascii('a')");
        Assert.assertEquals(0x69, CtypeFuncs.toascii(0xE9), "toascii(0xE9) -> 0x69");
        Assert.assertEquals(0x41, CtypeFuncs.toascii(0x41), "toascii(0x41)");
        Assert.assertEquals(0, CtypeFuncs.toascii(0x80), "toascii(0x80) -> 0");
    }

    public static int run() {
        failures = 0;
        System.out.println("== CtypeFuncsTest ==");
        check(CtypeFuncsTest::testIsdigit);
        check(CtypeFuncsTest::testIsxdigit);
        check(CtypeFuncsTest::testIsalpha);
        check(CtypeFuncsTest::testIsalnum);
        check(CtypeFuncsTest::testIsupper);
        check(CtypeFuncsTest::testIslower);
        check(CtypeFuncsTest::testIsspace);
        check(CtypeFuncsTest::testIsblank);
        check(CtypeFuncsTest::testTolower);
        check(CtypeFuncsTest::testToupper);
        check(CtypeFuncsTest::testIsgraph);
        check(CtypeFuncsTest::testIsprint);
        check(CtypeFuncsTest::testIspunct);
        check(CtypeFuncsTest::testIscntrl);
        check(CtypeFuncsTest::testIsascii);
        check(CtypeFuncsTest::testToascii);
        System.out.println("  " + (failures == 0 ? "PASS" : failures + " FAILURE(S)"));
        return failures;
    }
}