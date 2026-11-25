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

package ohos.musl.wchar;

import ohos.musl.Assert;

/**
 * {@link WcharFuncs}（wchar.h）的测试套件。期望值对照 musl {@code src/wchar/*.c}
 * 语义推演（Java 以 UTF-16 码元表达宽字符）。
 */
public final class WcharFuncsTest {

    private static int failures = 0;

    private WcharFuncsTest() {
    }

    private static void check(Runnable r) {
        try {
            r.run();
        } catch (AssertionError e) {
            failures++;
            System.err.println("  FAIL " + e.getMessage());
        }
    }

    private static void testWcslen() {
        Assert.assertEquals(0, WcharFuncs.wcslen(""), "wcslen empty");
        Assert.assertEquals(5, WcharFuncs.wcslen("hello"), "wcslen hello");
        Assert.assertEquals(0, WcharFuncs.wcsnlen("hello", 0), "wcsnlen n=0");
        Assert.assertEquals(3, WcharFuncs.wcsnlen("hello", 3), "wcsnlen n<len");
        Assert.assertEquals(5, WcharFuncs.wcsnlen("hello", 10), "wcsnlen n>len");
    }

    private static void testWcscmp() {
        Assert.assertEquals(0, WcharFuncs.wcscmp("", ""), "wcscmp empty");
        Assert.assertEquals(0, WcharFuncs.wcscmp("abc", "abc"), "wcscmp equal");
        Assert.assertTrue(WcharFuncs.wcscmp("abc", "abd") < 0, "wcscmp less");
        Assert.assertTrue(WcharFuncs.wcscmp("abd", "abc") > 0, "wcscmp greater");
        Assert.assertEquals('c', WcharFuncs.wcscmp("abc", "ab"), "wcscmp prefix");
        Assert.assertEquals(0, WcharFuncs.wcsncmp("abc", "abd", 2), "wcsncmp n=2");
        Assert.assertTrue(WcharFuncs.wcsncmp("abc", "abd", 3) < 0, "wcsncmp n=3");
        Assert.assertEquals(0, WcharFuncs.wcsncmp("abc", "abd", 0), "wcsncmp n=0");
    }

    private static void testWmemcmp() {
        Assert.assertEquals(0, WcharFuncs.wmemcmp(new char[] {'a', 'b'}, new char[] {'a', 'b'}, 2),
                "wmemcmp equal");
        Assert.assertTrue(WcharFuncs.wmemcmp(new char[] {'a', 'b'}, new char[] {'a', 'c'}, 2) < 0,
                "wmemcmp less");
        Assert.assertEquals(0, WcharFuncs.wmemcmp(new char[] {'a', 'b'}, new char[] {'a', 'c'}, 1),
                "wmemcmp n=1");
    }

    private static void testWcschr() {
        Assert.assertEquals(0, WcharFuncs.wcschr("abc", 'a'), "wcschr a");
        Assert.assertEquals(2, WcharFuncs.wcschr("abca", 'c'), "wcschr c");
        Assert.assertEquals(-1, WcharFuncs.wcschr("abc", 'x'), "wcschr absent");
        Assert.assertEquals(3, WcharFuncs.wcschr("abc", 0), "wcschr NUL -> len");
        Assert.assertEquals(3, WcharFuncs.wcsrchr("abca", 'a'), "wcsrchr a");
        Assert.assertEquals(3, WcharFuncs.wcsrchr("abc", 0), "wcsrchr NUL -> len");
        Assert.assertEquals(-1, WcharFuncs.wcsrchr("abc", 'x'), "wcsrchr absent");
    }

    private static void testWcschrnul() {
        Assert.assertEquals(2, WcharFuncs.wcschrnul("abca", 'c'), "wcschrnul found");
        Assert.assertEquals(4, WcharFuncs.wcschrnul("abca", 'x'), "wcschrnul absent -> len");
        Assert.assertEquals(4, WcharFuncs.wcschrnul("abca", 0), "wcschrnul NUL -> len");
    }

    private static void testWcscasecmp() {
        Assert.assertEquals(0, WcharFuncs.wcscasecmp("AbC", "aBc"), "wcscasecmp equal");
        Assert.assertTrue(WcharFuncs.wcscasecmp("abc", "abd") < 0, "wcscasecmp less");
        Assert.assertTrue(WcharFuncs.wcscasecmp("abd", "abc") > 0, "wcscasecmp greater");
        Assert.assertTrue(WcharFuncs.wcscasecmp("a", "B") < 0, "wcscasecmp fold diff");
        Assert.assertEquals(0, WcharFuncs.wcsncasecmp("AbC", "aBd", 2), "wcsncasecmp n=2");
        Assert.assertTrue(WcharFuncs.wcsncasecmp("AbC", "aBd", 3) < 0, "wcsncasecmp n=3");
        Assert.assertEquals(0, WcharFuncs.wcsncasecmp("AbC", "aBd", 0), "wcsncasecmp n=0");
    }

    private static void testWcspbrk() {
        Assert.assertEquals(1, WcharFuncs.wcspbrk("hello", "aeiou"), "wcspbrk e");
        Assert.assertEquals(-1, WcharFuncs.wcspbrk("hello", "xyz"), "wcspbrk absent");
        Assert.assertEquals(4, WcharFuncs.wcsspn("hello", "hel"), "wcsspn");
        Assert.assertEquals(0, WcharFuncs.wcsspn("hello", ""), "wcsspn empty set");
        Assert.assertEquals(2, WcharFuncs.wcscspn("hello", "lo"), "wcscspn");
        Assert.assertEquals(5, WcharFuncs.wcscspn("hello", "xyz"), "wcscspn all");
        Assert.assertEquals(2, WcharFuncs.wcsstr("abcabc", "ca"), "wcsstr");
        Assert.assertEquals(0, WcharFuncs.wcsstr("abc", ""), "wcsstr empty");
        Assert.assertEquals(-1, WcharFuncs.wcsstr("abc", "x"), "wcsstr absent");
    }

    private static void testWmemops() {
        char[] a = {'a', 'b', 'c', 'd'};
        Assert.assertEquals(1, WcharFuncs.wmemchr(a, 'b', 4), "wmemchr b");
        Assert.assertEquals(-1, WcharFuncs.wmemchr(a, 'z', 4), "wmemchr absent");
        Assert.assertEquals(-1, WcharFuncs.wmemchr(a, 'a', 0), "wmemchr n=0");

        char[] dst = new char[4];
        WcharFuncs.wmemcpy(dst, a, 4);
        Assert.assertEquals('a', dst[0], "wmemcpy dst[0]");
        Assert.assertEquals('d', dst[3], "wmemcpy dst[3]");

        char[] mv = {'1', '2', '3', '4'};
        WcharFuncs.wmemmove(mv, mv, 4);
        Assert.assertEquals('1', mv[0], "wmemmove self");

        char[] pad = new char[4];
        WcharFuncs.wmemset(pad, 'x', 3);
        Assert.assertEquals('x', pad[0], "wmemset pad[0]");
        Assert.assertEquals('x', pad[2], "wmemset pad[2]");
        Assert.assertEquals(0, pad[3], "wmemset pad[3] untouched");
    }

    private static void testWcscpyCat() {
        char[] dst = new char[10];
        Assert.assertEquals(3, WcharFuncs.wcscpy(dst, "abc"), "wcscpy return");
        Assert.assertEquals('a', dst[0], "wcscpy dst[0]");
        Assert.assertEquals(0, dst[3], "wcscpy terminator");

        Assert.assertEquals(3, WcharFuncs.wcsncpy(dst, "abcde", 3), "wcsncpy truncate");
        Assert.assertEquals(2, WcharFuncs.wcsncpy(dst, "ab", 5), "wcsncpy pad return");
        Assert.assertEquals(0, dst[2], "wcsncpy pad zero");

        Assert.assertEquals(4, WcharFuncs.wcscat(dst, "xy"), "wcscat return");
        Assert.assertEquals('x', dst[2], "wcscat dst[2]");
        Assert.assertEquals(0, dst[4], "wcscat terminator");

        Assert.assertEquals(6, WcharFuncs.wcsncat(dst, "zzz", 2), "wcsncat return");
        Assert.assertEquals('z', dst[4], "wcsncat dst[4]");
        Assert.assertEquals(0, dst[6], "wcsncat terminator");
    }

    private static void testWcstox() {
        Assert.assertEquals(42L, WcharFuncs.wcstol("42", 10), "wcstol 42");
        Assert.assertEquals(-42L, WcharFuncs.wcstol("-42", 10), "wcstol -42");
        Assert.assertEquals(26L, WcharFuncs.wcstol("0x1A", 0), "wcstol hex");
        Assert.assertEquals(42L, WcharFuncs.wcstoul("42", 10), "wcstoul 42");
        Assert.assertEquals(-1L, WcharFuncs.wcstoul("18446744073709551616", 10), "wcstoul overflow");
        Assert.assertEquals(-42L, WcharFuncs.wcstoll("-42", 10), "wcstoll -42");
        Assert.assertEquals(-42L, WcharFuncs.wcstoull("-42", 10), "wcstoull wrap");
        Assert.assertEquals(3.14, WcharFuncs.wcstod("3.14"), 1e-12, "wcstod 3.14");
        Assert.assertEquals(Double.POSITIVE_INFINITY, WcharFuncs.wcstod("inf"), 1e-12, "wcstod inf");
        Assert.assertEquals(3.14f, WcharFuncs.wcstof("3.14"), 1e-5f, "wcstof 3.14");
        Assert.assertEquals(8.0f, WcharFuncs.wcstof("0x1p3"), 1e-6f, "wcstof hex");
        Assert.assertEquals(3.14, WcharFuncs.wcstold("3.14"), 1e-12, "wcstold 3.14");
        Assert.assertEquals(-250.0, WcharFuncs.wcstold("-2.5e2"), 1e-12, "wcstold exp");
    }

    private static void testBtowcWctob() {
        Assert.assertEquals('a', WcharFuncs.btowc('a'), "btowc('a')");
        Assert.assertEquals(0xFF, WcharFuncs.btowc(0xFF), "btowc(0xFF)");
        Assert.assertEquals(-1, WcharFuncs.btowc(-1), "btowc(EOF)");
        Assert.assertEquals(-1, WcharFuncs.btowc(0x100), "btowc(0x100)");
        Assert.assertEquals('a', WcharFuncs.wctob('a'), "wctob('a')");
        Assert.assertEquals(0x7F, WcharFuncs.wctob(0x7F), "wctob(0x7F)");
        Assert.assertEquals(-1, WcharFuncs.wctob(0x100), "wctob(0x100) -> EOF");
        Assert.assertEquals(-1, WcharFuncs.wctob(-1), "wctob(-1) -> EOF");
    }

    public static int run() {
        failures = 0;
        System.out.println("== WcharFuncsTest ==");
        check(WcharFuncsTest::testWcslen);
        check(WcharFuncsTest::testWcscmp);
        check(WcharFuncsTest::testWmemcmp);
        check(WcharFuncsTest::testWcschr);
        check(WcharFuncsTest::testWcspbrk);
        check(WcharFuncsTest::testWcschrnul);
        check(WcharFuncsTest::testWcscasecmp);
        check(WcharFuncsTest::testWmemops);
        check(WcharFuncsTest::testWcscpyCat);
        check(WcharFuncsTest::testWcstox);
        check(WcharFuncsTest::testBtowcWctob);
        System.out.println("  " + (failures == 0 ? "PASS" : failures + " FAILURE(S)"));
        return failures;
    }
}
