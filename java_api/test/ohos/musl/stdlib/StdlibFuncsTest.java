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

package ohos.musl.stdlib;

import ohos.musl.Assert;

/**
 * {@link StdlibFuncs}（stdlib.h：abs/labs/atoi）的测试套件。
 */
public final class StdlibFuncsTest {

    private static int failures = 0;

    private StdlibFuncsTest() {
    }

    private static void check(Runnable r) {
        try {
            r.run();
        } catch (AssertionError e) {
            failures++;
            System.err.println("  FAIL " + e.getMessage());
        }
    }

    private static void testAbs() {
        Assert.assertEquals(0, StdlibFuncs.abs(0), "abs(0)");
        Assert.assertEquals(42, StdlibFuncs.abs(42), "abs(42)");
        Assert.assertEquals(42, StdlibFuncs.abs(-42), "abs(-42)");
        Assert.assertEquals(1, StdlibFuncs.abs(-1), "abs(-1)");
        Assert.assertEquals(Integer.MAX_VALUE, StdlibFuncs.abs(Integer.MAX_VALUE), "abs(MAX_VALUE)");
        Assert.assertEquals(Integer.MIN_VALUE, StdlibFuncs.abs(Integer.MIN_VALUE), "abs(MIN_VALUE)");
    }

    private static void testLabs() {
        Assert.assertEquals(0L, StdlibFuncs.labs(0L), "labs(0)");
        Assert.assertEquals(42L, StdlibFuncs.labs(42L), "labs(42)");
        Assert.assertEquals(42L, StdlibFuncs.labs(-42L), "labs(-42)");
        Assert.assertEquals(Long.MAX_VALUE, StdlibFuncs.labs(Long.MAX_VALUE), "labs(MAX_VALUE)");
        Assert.assertEquals(Long.MIN_VALUE, StdlibFuncs.labs(Long.MIN_VALUE), "labs(MIN_VALUE)");
    }

    private static void testAtoi() {
        Assert.assertEquals(0, StdlibFuncs.atoi("0"), "atoi(\"0\")");
        Assert.assertEquals(42, StdlibFuncs.atoi("42"), "atoi(\"42\")");
        Assert.assertEquals(-42, StdlibFuncs.atoi("-42"), "atoi(\"-42\")");
        Assert.assertEquals(42, StdlibFuncs.atoi("+42"), "atoi(\"+42\")");
        Assert.assertEquals(42, StdlibFuncs.atoi("  42"), "atoi leading space");
        Assert.assertEquals(-42, StdlibFuncs.atoi("  -42"), "atoi space and sign");
        Assert.assertEquals(42, StdlibFuncs.atoi("42abc"), "atoi trailing junk");
        Assert.assertEquals(0, StdlibFuncs.atoi("abc"), "atoi no digits");
        Assert.assertEquals(0, StdlibFuncs.atoi(""), "atoi empty");
        Assert.assertEquals(0, StdlibFuncs.atoi("+"), "atoi only plus");
        Assert.assertEquals(0, StdlibFuncs.atoi("-"), "atoi only minus");
        Assert.assertEquals(Integer.MAX_VALUE, StdlibFuncs.atoi("2147483647"), "atoi INT_MAX");
        Assert.assertEquals(Integer.MIN_VALUE, StdlibFuncs.atoi("-2147483648"), "atoi INT_MIN");
        Assert.assertEquals(123, StdlibFuncs.atoi("\t\n 123"), "atoi mixed whitespace");
    }

    private static void testAtol() {
        Assert.assertEquals(0L, StdlibFuncs.atol("0"), "atol(\"0\")");
        Assert.assertEquals(42L, StdlibFuncs.atol("42"), "atol(\"42\")");
        Assert.assertEquals(-42L, StdlibFuncs.atol("-42"), "atol(\"-42\")");
        Assert.assertEquals(1234567890123L, StdlibFuncs.atol("1234567890123"), "atol long");
        Assert.assertEquals(Long.MAX_VALUE, StdlibFuncs.atol("9223372036854775807"), "atol LONG_MAX");
        Assert.assertEquals(Long.MIN_VALUE, StdlibFuncs.atol("-9223372036854775808"), "atol LONG_MIN");
        Assert.assertEquals(0L, StdlibFuncs.atol("abc"), "atol no digits");
    }

    private static void testAtoll() {
        Assert.assertEquals(42L, StdlibFuncs.atoll("42"), "atoll(\"42\")");
        Assert.assertEquals(-42L, StdlibFuncs.atoll("  -42xyz"), "atoll sign and junk");
    }

    private static void testLlabs() {
        Assert.assertEquals(0L, StdlibFuncs.llabs(0L), "llabs(0)");
        Assert.assertEquals(42L, StdlibFuncs.llabs(-42L), "llabs(-42)");
        Assert.assertEquals(Long.MAX_VALUE, StdlibFuncs.llabs(Long.MAX_VALUE), "llabs(MAX)");
        Assert.assertEquals(Long.MIN_VALUE, StdlibFuncs.llabs(Long.MIN_VALUE), "llabs(MIN)");
    }

    private static void testAtof() {
        Assert.assertEquals(0.0, StdlibFuncs.atof("0"), 1e-12, "atof(\"0\")");
        Assert.assertEquals(3.14, StdlibFuncs.atof("3.14"), 1e-12, "atof(\"3.14\")");
        Assert.assertEquals(3.14, StdlibFuncs.atof("  3.14"), 1e-12, "atof space");
        Assert.assertEquals(3.14, StdlibFuncs.atof("3.14abc"), 1e-12, "atof junk");
        Assert.assertEquals(-250.0, StdlibFuncs.atof("-2.5e2"), 1e-12, "atof exp");
        Assert.assertEquals(0.5, StdlibFuncs.atof(".5"), 1e-12, "atof .5");
        Assert.assertEquals(1.0, StdlibFuncs.atof("1."), 1e-12, "atof 1.");
        Assert.assertEquals(1.0, StdlibFuncs.atof("1e"), 1e-12, "atof trailing e");
        Assert.assertEquals(0.0, StdlibFuncs.atof(""), 1e-12, "atof empty");
        Assert.assertEquals(0.0, StdlibFuncs.atof("abc"), 1e-12, "atof no digits");
        Assert.assertEquals(Double.POSITIVE_INFINITY, StdlibFuncs.atof("inf"), 1e-12, "atof inf");
        Assert.assertEquals(Double.NEGATIVE_INFINITY, StdlibFuncs.atof("-infinity"), 1e-12, "atof -infinity");
        Assert.assertIsNaN(StdlibFuncs.atof("nan"), "atof nan");
        Assert.assertIsNaN(StdlibFuncs.atof("NaN"), "atof NaN");
        Assert.assertEquals(8.0, StdlibFuncs.atof("0x1p3"), 1e-12, "atof hex");
    }

    private static void testStrtod() {
        // 十进制
        Assert.assertEquals(0.0, StdlibFuncs.strtod("0"), 1e-12, "strtod 0");
        Assert.assertEquals(3.14, StdlibFuncs.strtod("3.14"), 1e-12, "strtod 3.14");
        Assert.assertEquals(-250.0, StdlibFuncs.strtod("  -2.5e2"), 1e-12, "strtod sign exp");
        Assert.assertEquals(1.0, StdlibFuncs.strtod("1e"), 1e-12, "strtod trailing e");
        Assert.assertEquals(0.5, StdlibFuncs.strtod(".5"), 1e-12, "strtod .5");
        Assert.assertEquals(1.0, StdlibFuncs.strtod("1."), 1e-12, "strtod 1.");
        Assert.assertEquals(0.0, StdlibFuncs.strtod(""), 1e-12, "strtod empty");
        Assert.assertEquals(0.0, StdlibFuncs.strtod("abc"), 1e-12, "strtod no digit");
        Assert.assertEquals(1.5, StdlibFuncs.strtod("1.5xyz"), 1e-12, "strtod trailing junk");
        Assert.assertEquals(Double.POSITIVE_INFINITY, StdlibFuncs.strtod("1e9999"), 1e-12, "strtod overflow");
        Assert.assertEquals(-1.5e-300, StdlibFuncs.strtod("-1.5e-300"), 1e-310, "strtod denormal-ish");

        // inf / nan
        Assert.assertEquals(Double.POSITIVE_INFINITY, StdlibFuncs.strtod("inf"), 1e-12, "strtod inf");
        Assert.assertEquals(Double.NEGATIVE_INFINITY, StdlibFuncs.strtod("-INFINITY"), 1e-12, "strtod -INFINITY");
        Assert.assertIsNaN(StdlibFuncs.strtod("nan"), "strtod nan");
        Assert.assertIsNaN(StdlibFuncs.strtod("NaN(123)"), "strtod nan payload");

        // 十六进制
        Assert.assertEquals(8.0, StdlibFuncs.strtod("0x1p3"), 1e-12, "strtod 0x1p3");
        Assert.assertEquals(3.0, StdlibFuncs.strtod("0x1.8p1"), 1e-12, "strtod 0x1.8p1");
        Assert.assertEquals(1.5, StdlibFuncs.strtod("0x1.8"), 1e-12, "strtod 0x1.8");
        Assert.assertEquals(1.0, StdlibFuncs.strtod("0x.8p1"), 1e-12, "strtod 0x.8p1");
        Assert.assertEquals(-2.0, StdlibFuncs.strtod("-0x2"), 1e-12, "strtod -0x2");
        Assert.assertEquals(0.0, StdlibFuncs.strtod("0x"), 1e-12, "strtod 0x only");
        Assert.assertEquals(1.0, StdlibFuncs.strtod("0x1p"), 1e-12, "strtod hex trailing p");
        Assert.assertEquals(0.5, StdlibFuncs.strtod("0x1p-1"), 1e-12, "strtod hex negative exp");
    }

    private static void testStrtol() {
        Assert.assertEquals(0L, StdlibFuncs.strtol("0", 10), "strtol 0");
        Assert.assertEquals(42L, StdlibFuncs.strtol("42", 10), "strtol 42");
        Assert.assertEquals(-42L, StdlibFuncs.strtol("-42", 10), "strtol -42");
        Assert.assertEquals(42L, StdlibFuncs.strtol("  42", 10), "strtol space");
        Assert.assertEquals(42L, StdlibFuncs.strtol("42abc", 10), "strtol junk");

        Assert.assertEquals(26L, StdlibFuncs.strtol("0x1A", 0), "strtol auto hex");
        Assert.assertEquals(26L, StdlibFuncs.strtol("0X1a", 16), "strtol base16 hex");
        Assert.assertEquals(8L, StdlibFuncs.strtol("010", 0), "strtol auto octal");
        Assert.assertEquals(42L, StdlibFuncs.strtol("42", 0), "strtol auto dec");
        Assert.assertEquals(5L, StdlibFuncs.strtol("101", 2), "strtol base2");
        Assert.assertEquals(1295L, StdlibFuncs.strtol("zz", 36), "strtol base36");
        Assert.assertEquals(-16L, StdlibFuncs.strtol("  -0x10", 0), "strtol neg hex");
        Assert.assertEquals(16L, StdlibFuncs.strtol("+0x10", 0), "strtol pos hex");

        Assert.assertEquals(Long.MAX_VALUE, StdlibFuncs.strtol("9223372036854775807", 10), "strtol LONG_MAX");
        Assert.assertEquals(Long.MIN_VALUE, StdlibFuncs.strtol("-9223372036854775808", 10), "strtol LONG_MIN");
        Assert.assertEquals(Long.MAX_VALUE, StdlibFuncs.strtol("9223372036854775808", 10), "strtol overflow pos");
        Assert.assertEquals(Long.MIN_VALUE, StdlibFuncs.strtol("-9223372036854775809", 10), "strtol overflow neg");

        Assert.assertEquals(0L, StdlibFuncs.strtol("", 10), "strtol empty");
        Assert.assertEquals(0L, StdlibFuncs.strtol("abc", 10), "strtol no digit");
        Assert.assertEquals(0L, StdlibFuncs.strtol("42", 1), "strtol bad base 1");
        Assert.assertEquals(0L, StdlibFuncs.strtol("42", 37), "strtol bad base 37");
        Assert.assertEquals(0L, StdlibFuncs.strtol("0x", 0), "strtol 0x only");
    }

    private static void testQsort() {
        Integer[] empty = {};
        StdlibFuncs.qsort(empty, Integer::compareTo);
        Assert.assertTrue(empty.length == 0, "qsort empty");

        Integer[] single = { 5 };
        StdlibFuncs.qsort(single, Integer::compareTo);
        Assert.assertEquals(5, single[0], "qsort single");

        assertSortedEqual(new Integer[] { 3, 1, 4, 1, 5, 9, 2, 6 }, "qsort basic");
        assertSortedEqual(new Integer[] { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 }, "qsort reverse");
        assertSortedEqual(new Integer[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }, "qsort sorted");
        assertSortedEqual(new Integer[] { 2, 2, 2, 2, 2 }, "qsort all equal");
        assertSortedEqual(new Integer[] { -5, 0, 5, -5, 0 }, "qsort negatives");

        // 大量随机（以 JDK Arrays.sort 为 oracle 校验有序且元素集合不变）
        java.util.Random rnd = new java.util.Random(20261119);
        for (int t = 0; t < 500; t++) {
            int len = rnd.nextInt(64);
            Integer[] arr = new Integer[len];
            for (int i = 0; i < len; i++) {
                arr[i] = rnd.nextInt(200) - 100;
            }
            assertSortedEqual(arr, "qsort random len=" + len);
        }

        // 字符串（字典序）
        String[] s = { "banana", "apple", "cherry", "Apple", "grape" };
        String[] copy = s.clone();
        java.util.Arrays.sort(copy, String::compareTo);
        StdlibFuncs.qsort(s, String::compareTo);
        for (int i = 0; i < s.length; i++) {
            Assert.assertTrue(s[i].equals(copy[i]), "qsort string mismatch at " + i);
        }
    }

    private static void assertSortedEqual(Integer[] input, String msg) {
        Integer[] expect = input.clone();
        java.util.Arrays.sort(expect);
        StdlibFuncs.qsort(input, Integer::compareTo);
        for (int i = 0; i < input.length; i++) {
            Assert.assertEquals(expect[i], input[i], msg + " mismatch at " + i);
        }
    }

    public static int run() {
        failures = 0;
        System.out.println("== StdlibFuncsTest ==");
        check(StdlibFuncsTest::testAbs);
        check(StdlibFuncsTest::testLabs);
        check(StdlibFuncsTest::testAtoi);
        check(StdlibFuncsTest::testAtol);
        check(StdlibFuncsTest::testAtoll);
        check(StdlibFuncsTest::testLlabs);
        check(StdlibFuncsTest::testAtof);
        check(StdlibFuncsTest::testStrtod);
        check(StdlibFuncsTest::testStrtol);
        check(StdlibFuncsTest::testQsort);
        System.out.println("  " + (failures == 0 ? "PASS" : failures + " FAILURE(S)"));
        return failures;
    }
}