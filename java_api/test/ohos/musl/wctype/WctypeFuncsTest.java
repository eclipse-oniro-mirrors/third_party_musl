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

package ohos.musl.wctype;

import ohos.musl.Assert;

/**
 * {@link WctypeFuncs}（wctype.h：towlower/towupper/isw*）的测试套件。
 */
public final class WctypeFuncsTest {

    private static int failures = 0;

    private WctypeFuncsTest() {
    }

    private static void check(Runnable r) {
        try {
            r.run();
        } catch (AssertionError e) {
            failures++;
            System.err.println("  FAIL " + e.getMessage());
        }
    }

    private static void testTow() {
        Assert.assertEquals('a', WctypeFuncs.towlower('A'), "towlower('A')");
        Assert.assertEquals('z', WctypeFuncs.towlower('Z'), "towlower('Z')");
        Assert.assertEquals('a', WctypeFuncs.towlower('a'), "towlower('a') unchanged");
        Assert.assertEquals('7', WctypeFuncs.towlower('7'), "towlower non-letter");
        Assert.assertEquals('A', WctypeFuncs.towupper('a'), "towupper('a')");
        Assert.assertEquals('Z', WctypeFuncs.towupper('z'), "towupper('z')");
        Assert.assertEquals('.', WctypeFuncs.towupper('.'), "towupper non-letter");
    }

    private static void testIsdigit() {
        Assert.assertTrue(WctypeFuncs.iswdigit('0'), "iswdigit('0')");
        Assert.assertTrue(WctypeFuncs.iswdigit('9'), "iswdigit('9')");
        Assert.assertTrue(!WctypeFuncs.iswdigit('a'), "iswdigit('a') false");
        Assert.assertTrue(!WctypeFuncs.iswdigit(' '), "iswdigit(' ') false");
        Assert.assertTrue(WctypeFuncs.iswxdigit('f'), "iswxdigit('f')");
        Assert.assertTrue(WctypeFuncs.iswxdigit('A'), "iswxdigit('A')");
        Assert.assertTrue(!WctypeFuncs.iswxdigit('g'), "iswxdigit('g') false");
    }

    private static void testIsalpha() {
        Assert.assertTrue(WctypeFuncs.iswalpha('a'), "iswalpha('a')");
        Assert.assertTrue(WctypeFuncs.iswalpha('Z'), "iswalpha('Z')");
        Assert.assertTrue(!WctypeFuncs.iswalpha('1'), "iswalpha('1') false");
        Assert.assertTrue(WctypeFuncs.iswalnum('a'), "iswalnum('a')");
        Assert.assertTrue(WctypeFuncs.iswalnum('9'), "iswalnum('9')");
        Assert.assertTrue(!WctypeFuncs.iswalnum(' '), "iswalnum(' ') false");
    }

    private static void testIsUpperLower() {
        Assert.assertTrue(WctypeFuncs.iswupper('A'), "iswupper('A')");
        Assert.assertTrue(!WctypeFuncs.iswupper('a'), "iswupper('a') false");
        Assert.assertTrue(WctypeFuncs.iswlower('a'), "iswlower('a')");
        Assert.assertTrue(!WctypeFuncs.iswlower('A'), "iswlower('A') false");
    }

    private static void testIsSpace() {
        Assert.assertTrue(WctypeFuncs.iswspace(' '), "iswspace(' ')");
        Assert.assertTrue(WctypeFuncs.iswspace('\t'), "iswspace('\\t')");
        Assert.assertTrue(WctypeFuncs.iswspace('\n'), "iswspace('\\n')");
        Assert.assertTrue(!WctypeFuncs.iswspace('a'), "iswspace('a') false");
        Assert.assertTrue(WctypeFuncs.iswblank(' '), "iswblank(' ')");
        Assert.assertTrue(WctypeFuncs.iswblank('\t'), "iswblank('\\t')");
        Assert.assertTrue(!WctypeFuncs.iswblank('\n'), "iswblank('\\n') false");
    }

    private static void testIsGraphPrint() {
        Assert.assertTrue(WctypeFuncs.iswgraph('!'), "iswgraph('!')");
        Assert.assertTrue(!WctypeFuncs.iswgraph(' '), "iswgraph(' ') false");
        Assert.assertTrue(WctypeFuncs.iswprint(' '), "iswprint(' ')");
        Assert.assertTrue(WctypeFuncs.iswprint('a'), "iswprint('a')");
        Assert.assertTrue(!WctypeFuncs.iswprint('\0'), "iswprint('\\0') false");
    }

    private static void testIsPunctCntrl() {
        Assert.assertTrue(WctypeFuncs.iswpunct('.'), "iswpunct('.')");
        Assert.assertTrue(!WctypeFuncs.iswpunct('a'), "iswpunct('a') false");
        Assert.assertTrue(WctypeFuncs.iswcntrl('\0'), "iswcntrl('\\0')");
        Assert.assertTrue(WctypeFuncs.iswcntrl('\t'), "iswcntrl('\\t')");
        Assert.assertTrue(!WctypeFuncs.iswcntrl('a'), "iswcntrl('a') false");
    }

    private static void testIsAscii() {
        Assert.assertTrue(WctypeFuncs.iswascii(0), "iswascii(0)");
        Assert.assertTrue(WctypeFuncs.iswascii(0x7f), "iswascii(0x7f)");
        Assert.assertTrue(!WctypeFuncs.iswascii(0x80), "iswascii(0x80) false");
        Assert.assertTrue(!WctypeFuncs.iswascii(-1), "iswascii(-1) false");
    }

    private static void testWctypeIsWctype() {
        Assert.assertEquals(WctypeFuncs.WC_DIGIT, WctypeFuncs.wctype("digit"), "wctype digit");
        Assert.assertEquals(WctypeFuncs.WC_ALPHA, WctypeFuncs.wctype("alpha"), "wctype alpha");
        Assert.assertEquals(WctypeFuncs.WC_UPPER, WctypeFuncs.wctype("upper"), "wctype upper");
        Assert.assertEquals(0, WctypeFuncs.wctype("bogus"), "wctype unknown");

        Assert.assertTrue(WctypeFuncs.iswctype('7', WctypeFuncs.wctype("digit")),
                "iswctype('7', digit)");
        Assert.assertTrue(!WctypeFuncs.iswctype('a', WctypeFuncs.wctype("digit")),
                "iswctype('a', digit) false");
        Assert.assertTrue(WctypeFuncs.iswctype('A', WctypeFuncs.wctype("upper")),
                "iswctype('A', upper)");
        Assert.assertTrue(WctypeFuncs.iswctype('f', WctypeFuncs.wctype("xdigit")),
                "iswctype('f', xdigit)");
        Assert.assertTrue(!WctypeFuncs.iswctype('7', 0), "iswctype('7', 0) false");
    }

    private static void testWctransTowctrans() {
        Assert.assertEquals(WctypeFuncs.WC_TOLOWER, WctypeFuncs.wctrans("tolower"), "wctrans tolower");
        Assert.assertEquals(WctypeFuncs.WC_TOUPPER, WctypeFuncs.wctrans("toupper"), "wctrans toupper");
        Assert.assertEquals(0, WctypeFuncs.wctrans("bogus"), "wctrans unknown");
        Assert.assertEquals('a', WctypeFuncs.towctrans('A', WctypeFuncs.wctrans("tolower")),
                "towctrans tolower");
        Assert.assertEquals('A', WctypeFuncs.towctrans('a', WctypeFuncs.wctrans("toupper")),
                "towctrans toupper");
        Assert.assertEquals('a', WctypeFuncs.towctrans('a', 0), "towctrans unknown unchanged");
    }

    public static int run() {
        failures = 0;
        System.out.println("== WctypeFuncsTest ==");
        check(WctypeFuncsTest::testTow);
        check(WctypeFuncsTest::testIsdigit);
        check(WctypeFuncsTest::testIsalpha);
        check(WctypeFuncsTest::testIsUpperLower);
        check(WctypeFuncsTest::testIsSpace);
        check(WctypeFuncsTest::testIsGraphPrint);
        check(WctypeFuncsTest::testIsPunctCntrl);
        check(WctypeFuncsTest::testIsAscii);
        check(WctypeFuncsTest::testWctypeIsWctype);
        check(WctypeFuncsTest::testWctransTowctrans);
        System.out.println("  " + (failures == 0 ? "PASS" : failures + " FAILURE(S)"));
        return failures;
    }
}
