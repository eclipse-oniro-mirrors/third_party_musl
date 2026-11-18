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

package ohos.musl;

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

    public static int run() {
        failures = 0;
        System.out.println("== StdlibFuncsTest ==");
        check(StdlibFuncsTest::testAbs);
        check(StdlibFuncsTest::testLabs);
        check(StdlibFuncsTest::testAtoi);
        System.out.println("  " + (failures == 0 ? "PASS" : failures + " FAILURE(S)"));
        return failures;
    }
}