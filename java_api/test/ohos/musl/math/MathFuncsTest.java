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

package ohos.musl.math;

import ohos.musl.Assert;

/**
 * {@link MathFuncs}（math.h：sqrt/pow/floor/fabs）的测试套件。
 */
public final class MathFuncsTest {

    private static final double EPS = 1e-12;

    private static int failures = 0;

    private MathFuncsTest() {
    }

    private static void check(Runnable r) {
        try {
            r.run();
        } catch (AssertionError e) {
            failures++;
            System.err.println("  FAIL " + e.getMessage());
        }
    }

    private static void testSqrt() {
        Assert.assertEquals(0.0, MathFuncs.sqrt(0.0), EPS, "sqrt(0)");
        Assert.assertEquals(1.0, MathFuncs.sqrt(1.0), EPS, "sqrt(1)");
        Assert.assertEquals(2.0, MathFuncs.sqrt(4.0), EPS, "sqrt(4)");
        Assert.assertEquals(3.0, MathFuncs.sqrt(9.0), EPS, "sqrt(9)");
        Assert.assertEquals(1.4142135623730951, MathFuncs.sqrt(2.0), 1e-15, "sqrt(2)");
        Assert.assertIsNaN(MathFuncs.sqrt(-1.0), "sqrt(-1) -> NaN");
        Assert.assertTrue(Double.isInfinite(MathFuncs.sqrt(Double.POSITIVE_INFINITY)),
                "sqrt(+inf) -> +inf");
        double negZero = MathFuncs.sqrt(-0.0);
        Assert.assertTrue(1.0 / negZero == Double.NEGATIVE_INFINITY, "sqrt(-0.0) -> -0.0");
    }

    private static void testPow() {
        Assert.assertEquals(1024.0, MathFuncs.pow(2.0, 10.0), EPS, "pow(2,10)");
        Assert.assertEquals(1.0, MathFuncs.pow(2.0, 0.0), EPS, "pow(2,0)");
        Assert.assertEquals(1.0, MathFuncs.pow(0.0, 0.0), EPS, "pow(0,0)");
        Assert.assertEquals(0.5, MathFuncs.pow(2.0, -1.0), EPS, "pow(2,-1)");
        Assert.assertEquals(4.0, MathFuncs.pow(-2.0, 2.0), EPS, "pow(-2,2)");
        Assert.assertEquals(2.0, MathFuncs.pow(4.0, 0.5), EPS, "pow(4,0.5)");
    }

    private static void testFloor() {
        Assert.assertEquals(3.0, MathFuncs.floor(3.7), EPS, "floor(3.7)");
        Assert.assertEquals(-4.0, MathFuncs.floor(-3.7), EPS, "floor(-3.7)");
        Assert.assertEquals(0.0, MathFuncs.floor(0.0), EPS, "floor(0.0)");
        Assert.assertEquals(3.0, MathFuncs.floor(3.0), EPS, "floor(3.0)");
        Assert.assertEquals(-1.0, MathFuncs.floor(-0.5), EPS, "floor(-0.5)");
    }

    private static void testFabs() {
        Assert.assertEquals(3.5, MathFuncs.fabs(-3.5), EPS, "fabs(-3.5)");
        Assert.assertEquals(3.5, MathFuncs.fabs(3.5), EPS, "fabs(3.5)");
        Assert.assertEquals(0.0, MathFuncs.fabs(-0.0), EPS, "fabs(-0.0)");
        Assert.assertIsNaN(MathFuncs.fabs(Double.NaN), "fabs(NaN) -> NaN");
        Assert.assertTrue(Double.isInfinite(MathFuncs.fabs(Double.NEGATIVE_INFINITY)),
                "fabs(-inf) -> +inf");
    }

    public static int run() {
        failures = 0;
        System.out.println("== MathFuncsTest ==");
        check(MathFuncsTest::testSqrt);
        check(MathFuncsTest::testPow);
        check(MathFuncsTest::testFloor);
        check(MathFuncsTest::testFabs);
        System.out.println("  " + (failures == 0 ? "PASS" : failures + " FAILURE(S)"));
        return failures;
    }
}