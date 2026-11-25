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
 * {@link MathFuncs}（math.h：sqrt/pow/floor/fabs/fmod/remainder/round/trunc/fmin/fmax/copysign/log2 等）的测试套件。
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

    private static void testTrig() {
        Assert.assertEquals(0.0, MathFuncs.sin(0.0), EPS, "sin(0)");
        Assert.assertEquals(1.0, MathFuncs.sin(Math.PI / 2), EPS, "sin(pi/2)");
        Assert.assertEquals(1.0, MathFuncs.cos(0.0), EPS, "cos(0)");
        Assert.assertEquals(0.0, MathFuncs.tan(0.0), EPS, "tan(0)");
        Assert.assertEquals(Math.PI / 2, MathFuncs.asin(1.0), EPS, "asin(1)");
        Assert.assertEquals(0.0, MathFuncs.acos(1.0), EPS, "acos(1)");
        Assert.assertEquals(0.0, MathFuncs.atan(0.0), EPS, "atan(0)");
        Assert.assertEquals(Math.PI / 4, MathFuncs.atan(1.0), EPS, "atan(1)");
        Assert.assertEquals(Math.PI / 4, MathFuncs.atan2(1.0, 1.0), EPS, "atan2(1,1)");
        Assert.assertEquals(0.0, MathFuncs.atan2(0.0, 1.0), EPS, "atan2(0,1)");
    }

    private static void testExpLog() {
        Assert.assertEquals(1.0, MathFuncs.exp(0.0), EPS, "exp(0)");
        Assert.assertEquals(Math.E, MathFuncs.exp(1.0), EPS, "exp(1)");
        Assert.assertEquals(0.0, MathFuncs.log(1.0), EPS, "log(1)");
        Assert.assertEquals(1.0, MathFuncs.log(Math.E), EPS, "log(e)");
        Assert.assertEquals(0.0, MathFuncs.log10(1.0), EPS, "log10(1)");
        Assert.assertEquals(3.0, MathFuncs.log10(1000.0), EPS, "log10(1000)");
        Assert.assertEquals(0.0, MathFuncs.sinh(0.0), EPS, "sinh(0)");
    }

    private static void testCeilCbrtHypot() {
        Assert.assertEquals(3.0, MathFuncs.ceil(2.1), EPS, "ceil(2.1)");
        Assert.assertEquals(-2.0, MathFuncs.ceil(-2.9), EPS, "ceil(-2.9)");
        Assert.assertEquals(2.0, MathFuncs.ceil(2.0), EPS, "ceil(2.0)");
        Assert.assertEquals(3.0, MathFuncs.cbrt(27.0), EPS, "cbrt(27)");
        Assert.assertEquals(-3.0, MathFuncs.cbrt(-27.0), EPS, "cbrt(-27)");
        Assert.assertEquals(5.0, MathFuncs.hypot(3.0, 4.0), EPS, "hypot(3,4)");
    }

    private static void testFmod() {
        Assert.assertEquals(1.0, MathFuncs.fmod(5.0, 2.0), EPS, "fmod(5,2)");
        Assert.assertEquals(-1.0, MathFuncs.fmod(-5.0, 2.0), EPS, "fmod(-5,2)");
        Assert.assertEquals(1.0, MathFuncs.fmod(5.0, -2.0), EPS, "fmod(5,-2)");
        Assert.assertEquals(-1.0, MathFuncs.fmod(-5.0, -2.0), EPS, "fmod(-5,-2)");
        Assert.assertEquals(1.5, MathFuncs.fmod(5.5, 2.0), EPS, "fmod(5.5,2)");
        Assert.assertEquals(2.0, MathFuncs.fmod(7.0, 2.5), EPS, "fmod(7,2.5)");
        Assert.assertEquals(0.0, MathFuncs.fmod(0.0, 3.0), EPS, "fmod(0,3)");
        Assert.assertIsNaN(MathFuncs.fmod(5.0, 0.0), "fmod(x,0) -> NaN");
        Assert.assertEquals(5.0, MathFuncs.fmod(5.0, Double.POSITIVE_INFINITY), EPS, "fmod(x,inf)");
        Assert.assertIsNaN(MathFuncs.fmod(Double.POSITIVE_INFINITY, 2.0), "fmod(inf,x) -> NaN");
        double negZero = MathFuncs.fmod(-0.0, 3.0);
        Assert.assertTrue(1.0 / negZero == Double.NEGATIVE_INFINITY, "fmod(-0,3) -> -0.0");
    }

    private static void testRemainder() {
        Assert.assertEquals(1.0, MathFuncs.remainder(5.0, 2.0), EPS, "remainder(5,2)");
        Assert.assertEquals(1.0, MathFuncs.remainder(5.0, 4.0), EPS, "remainder(5,4)");
        Assert.assertEquals(-1.0, MathFuncs.remainder(5.0, 6.0), EPS, "remainder(5,6)");
        Assert.assertEquals(-1.0, MathFuncs.remainder(-5.0, 2.0), EPS, "remainder(-5,2)");
        Assert.assertEquals(0.0, MathFuncs.remainder(4.0, 2.0), EPS, "remainder(4,2)");
        // 商为半值时平局取偶：3/2 = 1.5 -> n=2
        Assert.assertEquals(-1.0, MathFuncs.remainder(3.0, 2.0), EPS, "remainder(3,2) ties-to-even");
        // 2/4 = 0.5 -> n=0（偶数）
        Assert.assertEquals(2.0, MathFuncs.remainder(2.0, 4.0), EPS, "remainder(2,4) ties-to-even");
        Assert.assertIsNaN(MathFuncs.remainder(5.0, 0.0), "remainder(x,0) -> NaN");
        Assert.assertEquals(5.0, MathFuncs.remainder(5.0, Double.POSITIVE_INFINITY), EPS, "remainder(x,inf)");
    }

    private static void testRound() {
        Assert.assertEquals(3.0, MathFuncs.round(3.2), EPS, "round(3.2)");
        Assert.assertEquals(3.0, MathFuncs.round(2.5), EPS, "round(2.5) half up");
        Assert.assertEquals(4.0, MathFuncs.round(3.5), EPS, "round(3.5) half up");
        Assert.assertEquals(-3.0, MathFuncs.round(-2.5), EPS, "round(-2.5) half away");
        Assert.assertEquals(-3.0, MathFuncs.round(-3.2), EPS, "round(-3.2)");
        Assert.assertEquals(-2.0, MathFuncs.round(-2.4), EPS, "round(-2.4)");
        Assert.assertEquals(0.0, MathFuncs.round(0.0), EPS, "round(0)");
        Assert.assertEquals(1.0, MathFuncs.round(0.5), EPS, "round(0.5) half up");
        Assert.assertEquals(-1.0, MathFuncs.round(-0.5), EPS, "round(-0.5) half away");
        Assert.assertEquals(1.0e300, MathFuncs.round(1.0e300), EPS, "round big");
        Assert.assertTrue(Double.isInfinite(MathFuncs.round(Double.POSITIVE_INFINITY)), "round(inf)");
        Assert.assertIsNaN(MathFuncs.round(Double.NaN), "round(NaN)");
    }

    private static void testTrunc() {
        Assert.assertEquals(3.0, MathFuncs.trunc(3.7), EPS, "trunc(3.7)");
        Assert.assertEquals(-3.0, MathFuncs.trunc(-3.7), EPS, "trunc(-3.7)");
        Assert.assertEquals(0.0, MathFuncs.trunc(0.0), EPS, "trunc(0)");
        Assert.assertEquals(0.0, MathFuncs.trunc(0.5), EPS, "trunc(0.5)");
        Assert.assertEquals(3.0, MathFuncs.trunc(3.0), EPS, "trunc(3.0)");
        double negZero = MathFuncs.trunc(-0.5);
        Assert.assertTrue(1.0 / negZero == Double.NEGATIVE_INFINITY, "trunc(-0.5) -> -0.0");
        Assert.assertTrue(Double.isInfinite(MathFuncs.trunc(Double.POSITIVE_INFINITY)), "trunc(inf)");
        Assert.assertIsNaN(MathFuncs.trunc(Double.NaN), "trunc(NaN)");
    }

    private static void testFminFmax() {
        Assert.assertEquals(2.0, MathFuncs.fmin(2.0, 5.0), EPS, "fmin(2,5)");
        Assert.assertEquals(-3.0, MathFuncs.fmin(2.0, -3.0), EPS, "fmin(2,-3)");
        Assert.assertEquals(5.0, MathFuncs.fmax(2.0, 5.0), EPS, "fmax(2,5)");
        Assert.assertEquals(2.0, MathFuncs.fmax(-3.0, 2.0), EPS, "fmax(-3,2)");
        // NaN 语义：任一为 NaN 返回另一值
        Assert.assertEquals(5.0, MathFuncs.fmin(Double.NaN, 5.0), EPS, "fmin(NaN,5)");
        Assert.assertEquals(5.0, MathFuncs.fmin(5.0, Double.NaN), EPS, "fmin(5,NaN)");
        Assert.assertEquals(5.0, MathFuncs.fmax(Double.NaN, 5.0), EPS, "fmax(NaN,5)");
        Assert.assertEquals(5.0, MathFuncs.fmax(5.0, Double.NaN), EPS, "fmax(5,NaN)");
        // ±0：fmin 取 -0，fmax 取 +0
        double negZero = MathFuncs.fmin(0.0, -0.0);
        Assert.assertTrue(1.0 / negZero == Double.NEGATIVE_INFINITY, "fmin(+0,-0) -> -0.0");
        double maxZero = MathFuncs.fmax(-0.0, 0.0);
        Assert.assertTrue(1.0 / maxZero == Double.POSITIVE_INFINITY, "fmax(-0,+0) -> +0.0");
        Assert.assertEquals(Double.NEGATIVE_INFINITY, MathFuncs.fmin(Double.NEGATIVE_INFINITY, 5.0),
                EPS, "fmin(-inf,5)");
        Assert.assertEquals(Double.POSITIVE_INFINITY, MathFuncs.fmax(Double.POSITIVE_INFINITY, 5.0),
                EPS, "fmax(inf,5)");
    }

    private static void testCopysign() {
        Assert.assertEquals(3.0, MathFuncs.copysign(3.0, 2.0), EPS, "copysign(3,2)");
        Assert.assertEquals(-3.0, MathFuncs.copysign(3.0, -2.0), EPS, "copysign(3,-2)");
        Assert.assertEquals(-3.0, MathFuncs.copysign(-3.0, -2.0), EPS, "copysign(-3,-2)");
        Assert.assertEquals(3.0, MathFuncs.copysign(-3.0, 2.0), EPS, "copysign(-3,2)");
        Assert.assertEquals(-3.0, MathFuncs.copysign(3.0, -0.0), EPS, "copysign(3,-0)");
        Assert.assertEquals(3.0, MathFuncs.copysign(3.0, Double.NaN), EPS, "copysign(3,NaN)");
        double negZero = MathFuncs.copysign(0.0, -1.0);
        Assert.assertTrue(1.0 / negZero == Double.NEGATIVE_INFINITY, "copysign(+0,-1) -> -0.0");
        double posZero = MathFuncs.copysign(-0.0, 1.0);
        Assert.assertTrue(1.0 / posZero == Double.POSITIVE_INFINITY, "copysign(-0,1) -> +0.0");
    }

    private static void testLog2() {
        Assert.assertEquals(0.0, MathFuncs.log2(1.0), EPS, "log2(1)");
        Assert.assertEquals(1.0, MathFuncs.log2(2.0), EPS, "log2(2)");
        Assert.assertEquals(3.0, MathFuncs.log2(8.0), EPS, "log2(8)");
        Assert.assertEquals(-1.0, MathFuncs.log2(0.5), EPS, "log2(0.5)");
        Assert.assertEquals(10.0, MathFuncs.log2(1024.0), EPS, "log2(1024)");
        Assert.assertEquals(Double.NEGATIVE_INFINITY, MathFuncs.log2(0.0), EPS, "log2(0) -> -inf");
        Assert.assertEquals(Double.POSITIVE_INFINITY, MathFuncs.log2(Double.POSITIVE_INFINITY),
                EPS, "log2(inf) -> +inf");
        Assert.assertIsNaN(MathFuncs.log2(-1.0), "log2(-1) -> NaN");
    }

    private static void testExpM1Log1pExp2() {
        Assert.assertEquals(0.0, MathFuncs.expm1(0.0), EPS, "expm1(0)");
        Assert.assertEquals(Math.E - 1.0, MathFuncs.expm1(1.0), EPS, "expm1(1)");
        Assert.assertEquals(0.0, MathFuncs.log1p(0.0), EPS, "log1p(0)");
        Assert.assertEquals(1.0, MathFuncs.log1p(Math.E - 1.0), EPS, "log1p(e-1)");
        Assert.assertEquals(Double.NEGATIVE_INFINITY, MathFuncs.log1p(-1.0), EPS, "log1p(-1) -> -inf");
        Assert.assertIsNaN(MathFuncs.log1p(-2.0), "log1p(-2) -> NaN");
        Assert.assertEquals(1.0, MathFuncs.exp2(0.0), EPS, "exp2(0)");
        Assert.assertEquals(2.0, MathFuncs.exp2(1.0), EPS, "exp2(1)");
        Assert.assertEquals(1024.0, MathFuncs.exp2(10.0), EPS, "exp2(10)");
        Assert.assertEquals(0.5, MathFuncs.exp2(-1.0), EPS, "exp2(-1)");
    }

    private static void testFma() {
        Assert.assertEquals(10.0, MathFuncs.fma(2.0, 3.0, 4.0), EPS, "fma(2,3,4)");
        Assert.assertEquals(7.0, MathFuncs.fma(2.0, 3.0, 1.0), EPS, "fma(2,3,1)");
        Assert.assertEquals(-1.5, MathFuncs.fma(-1.0, 2.0, 0.5), EPS, "fma(-1,2,0.5)");
        Assert.assertEquals(0.0, MathFuncs.fma(0.0, 1.0, 0.0), EPS, "fma(0,1,0)");
    }

    private static void testNextAfter() {
        Assert.assertEquals(Math.nextUp(1.0), MathFuncs.nextafter(1.0, 2.0), 0.0, "nextafter(1,2)");
        Assert.assertEquals(Math.nextDown(1.0), MathFuncs.nextafter(1.0, 0.0), 0.0, "nextafter(1,0)");
        Assert.assertEquals(Double.MIN_VALUE, MathFuncs.nextafter(0.0, 1.0), 0.0, "nextafter(0,+1)");
        Assert.assertEquals(-Double.MIN_VALUE, MathFuncs.nextafter(0.0, -1.0), 0.0, "nextafter(0,-1)");
        Assert.assertEquals(1.0, MathFuncs.nextafter(1.0, 1.0), 0.0, "nextafter(x,x)");
        Assert.assertEquals(Double.POSITIVE_INFINITY, MathFuncs.nextUp(Double.POSITIVE_INFINITY),
                0.0, "nextUp(inf)");
        Assert.assertEquals(Double.MIN_VALUE, MathFuncs.nextUp(0.0), 0.0, "nextUp(0)");
        Assert.assertEquals(-Double.MIN_VALUE, MathFuncs.nextDown(0.0), 0.0, "nextDown(0)");
        Assert.assertEquals(0.9999999999999999, MathFuncs.nextDown(1.0), 0.0, "nextDown(1)");
    }

    private static void testScalbn() {
        Assert.assertEquals(1.0, MathFuncs.scalbn(1.0, 0), EPS, "scalbn(1,0)");
        Assert.assertEquals(1024.0, MathFuncs.scalbn(1.0, 10), EPS, "scalbn(1,10)");
        Assert.assertEquals(3.0, MathFuncs.scalbn(1.5, 1), EPS, "scalbn(1.5,1)");
        Assert.assertEquals(0.5, MathFuncs.scalbn(1.0, -1), EPS, "scalbn(1,-1)");
    }

    private static void testIlogbLogb() {
        Assert.assertEquals(Integer.MIN_VALUE, MathFuncs.ilogb(0.0), "ilogb(0)");
        Assert.assertEquals(Integer.MAX_VALUE, MathFuncs.ilogb(Double.NaN), "ilogb(NaN)");
        Assert.assertEquals(Integer.MAX_VALUE, MathFuncs.ilogb(Double.POSITIVE_INFINITY), "ilogb(inf)");
        Assert.assertEquals(0, MathFuncs.ilogb(1.0), "ilogb(1)");
        Assert.assertEquals(1, MathFuncs.ilogb(2.0), "ilogb(2)");
        Assert.assertEquals(3, MathFuncs.ilogb(8.0), "ilogb(8)");
        Assert.assertEquals(-1, MathFuncs.ilogb(0.5), "ilogb(0.5)");
        Assert.assertEquals(-1022, MathFuncs.ilogb(Double.MIN_NORMAL), "ilogb(MIN_NORMAL)");
        Assert.assertEquals(-1074, MathFuncs.ilogb(Double.MIN_VALUE), "ilogb(MIN_VALUE)");

        Assert.assertEquals(0.0, MathFuncs.logb(1.0), EPS, "logb(1)");
        Assert.assertEquals(3.0, MathFuncs.logb(8.0), EPS, "logb(8)");
        Assert.assertEquals(3.0, MathFuncs.logb(-8.0), EPS, "logb(-8)");
        Assert.assertEquals(-1.0, MathFuncs.logb(0.5), EPS, "logb(0.5)");
        Assert.assertEquals(-1074.0, MathFuncs.logb(Double.MIN_VALUE), EPS, "logb(MIN_VALUE)");
        Assert.assertEquals(Double.NEGATIVE_INFINITY, MathFuncs.logb(0.0), EPS, "logb(0) -> -inf");
        Assert.assertEquals(Double.POSITIVE_INFINITY, MathFuncs.logb(Double.POSITIVE_INFINITY),
                EPS, "logb(inf) -> +inf");
        Assert.assertIsNaN(MathFuncs.logb(Double.NaN), "logb(NaN) -> NaN");
    }

    private static void testFdim() {
        Assert.assertEquals(1.0, MathFuncs.fdim(3.0, 2.0), EPS, "fdim(3,2)");
        Assert.assertEquals(0.0, MathFuncs.fdim(2.0, 3.0), EPS, "fdim(2,3)");
        Assert.assertEquals(0.0, MathFuncs.fdim(3.0, 3.0), EPS, "fdim(3,3)");
        Assert.assertEquals(0.0, MathFuncs.fdim(-3.0, -2.0), EPS, "fdim(-3,-2)");
        Assert.assertEquals(1.0, MathFuncs.fdim(-2.0, -3.0), EPS, "fdim(-2,-3)");
        Assert.assertEquals(Double.POSITIVE_INFINITY, MathFuncs.fdim(Double.POSITIVE_INFINITY, 2.0),
                EPS, "fdim(inf,2)");
        Assert.assertEquals(0.0, MathFuncs.fdim(2.0, Double.POSITIVE_INFINITY), EPS, "fdim(2,inf)");
        Assert.assertIsNaN(MathFuncs.fdim(Double.NaN, 2.0), "fdim(NaN,2) -> NaN");
        Assert.assertIsNaN(MathFuncs.fdim(2.0, Double.NaN), "fdim(2,NaN) -> NaN");
    }

    private static void testRintRoundL() {
        Assert.assertEquals(2.0, MathFuncs.rint(2.5), EPS, "rint(2.5) ties-to-even");
        Assert.assertEquals(4.0, MathFuncs.rint(3.5), EPS, "rint(3.5) ties-to-even");
        Assert.assertEquals(2.0, MathFuncs.rint(2.4), EPS, "rint(2.4)");
        Assert.assertEquals(-2.0, MathFuncs.rint(-2.5), EPS, "rint(-2.5) ties-to-even");

        Assert.assertEquals(3L, MathFuncs.lround(2.5), "lround(2.5)");
        Assert.assertEquals(-3L, MathFuncs.lround(-2.5), "lround(-2.5)");
        Assert.assertEquals(2L, MathFuncs.lround(2.4), "lround(2.4)");
        Assert.assertEquals(3L, MathFuncs.llround(2.6), "llround(2.6)");

        Assert.assertEquals(2L, MathFuncs.lrint(2.5), "lrint(2.5) ties-to-even");
        Assert.assertEquals(4L, MathFuncs.lrint(3.5), "lrint(3.5) ties-to-even");
        Assert.assertEquals(-2L, MathFuncs.lrint(-2.5), "lrint(-2.5) ties-to-even");
        Assert.assertEquals(4L, MathFuncs.llrint(3.5), "llrint(3.5)");
    }

    private static void testFrexpLdexpModf() {
        MathFuncs.Frexp f = MathFuncs.frexp(1.0);
        Assert.assertEquals(0.5, f.mantissa, EPS, "frexp(1).mantissa");
        Assert.assertEquals(1, f.exp, "frexp(1).exp");
        f = MathFuncs.frexp(8.0);
        Assert.assertEquals(0.5, f.mantissa, EPS, "frexp(8).mantissa");
        Assert.assertEquals(4, f.exp, "frexp(8).exp");
        f = MathFuncs.frexp(0.5);
        Assert.assertEquals(0.5, f.mantissa, EPS, "frexp(0.5).mantissa");
        Assert.assertEquals(0, f.exp, "frexp(0.5).exp");
        f = MathFuncs.frexp(-8.0);
        Assert.assertEquals(-0.5, f.mantissa, EPS, "frexp(-8).mantissa");
        Assert.assertEquals(4, f.exp, "frexp(-8).exp");
        f = MathFuncs.frexp(0.0);
        Assert.assertEquals(0.0, f.mantissa, EPS, "frexp(0).mantissa");
        Assert.assertEquals(0, f.exp, "frexp(0).exp");
        f = MathFuncs.frexp(Double.POSITIVE_INFINITY);
        Assert.assertTrue(Double.isInfinite(f.mantissa), "frexp(inf).mantissa");
        Assert.assertEquals(0, f.exp, "frexp(inf).exp");
        Assert.assertIsNaN(MathFuncs.frexp(Double.NaN).mantissa, "frexp(NaN).mantissa");

        Assert.assertEquals(1.0, MathFuncs.ldexp(0.5, 1), EPS, "ldexp(0.5,1)");
        Assert.assertEquals(1024.0, MathFuncs.ldexp(1.0, 10), EPS, "ldexp(1,10)");
        Assert.assertEquals(1.5, MathFuncs.ldexp(3.0, -1), EPS, "ldexp(3,-1)");

        MathFuncs.Modf m = MathFuncs.modf(3.7);
        Assert.assertEquals(3.0, m.integral, EPS, "modf(3.7).integral");
        Assert.assertEquals(0.7, m.frac, 1e-12, "modf(3.7).frac");
        m = MathFuncs.modf(-3.7);
        Assert.assertEquals(-3.0, m.integral, EPS, "modf(-3.7).integral");
        Assert.assertEquals(-0.7, m.frac, 1e-12, "modf(-3.7).frac");
        m = MathFuncs.modf(0.0);
        Assert.assertEquals(0.0, m.integral, EPS, "modf(0).integral");
        Assert.assertEquals(0.0, m.frac, EPS, "modf(0).frac");
        m = MathFuncs.modf(Double.POSITIVE_INFINITY);
        Assert.assertTrue(Double.isInfinite(m.integral), "modf(inf).integral");
        Assert.assertTrue(Double.isInfinite(m.frac), "modf(inf).frac");
    }

    private static void testPredicates() {
        Assert.assertTrue(MathFuncs.isnan(Double.NaN), "isnan(NaN)");
        Assert.assertTrue(!MathFuncs.isnan(1.0), "isnan(1.0) false");
        Assert.assertTrue(MathFuncs.isinf(Double.POSITIVE_INFINITY), "isinf(+inf)");
        Assert.assertTrue(MathFuncs.isinf(Double.NEGATIVE_INFINITY), "isinf(-inf)");
        Assert.assertTrue(!MathFuncs.isinf(1.0), "isinf(1.0) false");
        Assert.assertTrue(MathFuncs.isfinite(1.0), "isfinite(1.0)");
        Assert.assertTrue(!MathFuncs.isfinite(Double.POSITIVE_INFINITY), "isfinite(inf) false");
        Assert.assertTrue(!MathFuncs.isfinite(Double.NaN), "isfinite(NaN) false");
        Assert.assertTrue(MathFuncs.isnormal(1.0), "isnormal(1.0)");
        Assert.assertTrue(MathFuncs.isnormal(Double.MIN_NORMAL), "isnormal(MIN_NORMAL)");
        Assert.assertTrue(!MathFuncs.isnormal(0.0), "isnormal(0) false");
        Assert.assertTrue(!MathFuncs.isnormal(Double.MIN_VALUE), "isnormal(MIN_VALUE) false");
        Assert.assertTrue(!MathFuncs.isnormal(Double.POSITIVE_INFINITY), "isnormal(inf) false");
        Assert.assertTrue(!MathFuncs.isnormal(Double.NaN), "isnormal(NaN) false");
        Assert.assertTrue(MathFuncs.signbit(-1.0), "signbit(-1)");
        Assert.assertTrue(MathFuncs.signbit(-0.0), "signbit(-0)");
        Assert.assertTrue(MathFuncs.signbit(Double.NEGATIVE_INFINITY), "signbit(-inf)");
        Assert.assertTrue(!MathFuncs.signbit(1.0), "signbit(+1) false");
        Assert.assertTrue(!MathFuncs.signbit(0.0), "signbit(+0) false");
    }

    public static int run() {
        failures = 0;
        System.out.println("== MathFuncsTest ==");
        check(MathFuncsTest::testSqrt);
        check(MathFuncsTest::testPow);
        check(MathFuncsTest::testFloor);
        check(MathFuncsTest::testFabs);
        check(MathFuncsTest::testTrig);
        check(MathFuncsTest::testExpLog);
        check(MathFuncsTest::testCeilCbrtHypot);
        check(MathFuncsTest::testFmod);
        check(MathFuncsTest::testRemainder);
        check(MathFuncsTest::testRound);
        check(MathFuncsTest::testTrunc);
        check(MathFuncsTest::testFminFmax);
        check(MathFuncsTest::testCopysign);
        check(MathFuncsTest::testLog2);
        check(MathFuncsTest::testExpM1Log1pExp2);
        check(MathFuncsTest::testFma);
        check(MathFuncsTest::testNextAfter);
        check(MathFuncsTest::testScalbn);
        check(MathFuncsTest::testIlogbLogb);
        check(MathFuncsTest::testFdim);
        check(MathFuncsTest::testRintRoundL);
        check(MathFuncsTest::testFrexpLdexpModf);
        check(MathFuncsTest::testPredicates);
        System.out.println("  " + (failures == 0 ? "PASS" : failures + " FAILURE(S)"));
        return failures;
    }
}