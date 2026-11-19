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
        System.out.println("  " + (failures == 0 ? "PASS" : failures + " FAILURE(S)"));
        return failures;
    }
}