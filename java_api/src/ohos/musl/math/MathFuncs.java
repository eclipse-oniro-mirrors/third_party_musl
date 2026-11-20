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

/**
 * Java 版本的 {@code <math.h>} 纯函数接口。
 *
 * <p>Java 标准库 {@link Math} 已提供 IEEE-754 语义一致的等价实现，此处做薄封装以保持
 * C 命名一致性。</p>
 */
public final class MathFuncs {

    private MathFuncs() {
        // 工具类，禁止实例化。
    }

    /**
     * {@code double sqrt(double x)}：平方根。负数返回 NaN、{@code +inf} 返回 +inf、
     * {@code -0.0} 返回 -0.0，与 musl IEEE-754 实现一致。
     *
     * @param x 输入值
     * @return {@code x} 的平方根
     */
    public static double sqrt(double x) {
        return Math.sqrt(x);
    }

    /**
     * {@code double pow(double x, double y)}：幂运算，语义与 musl {@code pow} 一致
     * （含 NaN、无穷、0^0 等特殊值）。
     *
     * @param x 底数
     * @param y 指数
     * @return {@code x} 的 {@code y} 次幂
     */
    public static double pow(double x, double y) {
        return Math.pow(x, y);
    }

    /**
     * {@code double floor(double x)}：向下取整，返回不大于 {@code x} 的最大整数值
     * （仍为 {@code double}）。
     *
     * @param x 输入值
     * @return {@code x} 向下取整
     */
    public static double floor(double x) {
        return Math.floor(x);
    }

    /**
     * {@code double fabs(double x)}：绝对值，等价于清除符号位。{@code -0.0} 返回
     * {@code +0.0}，NaN 返回 NaN，与 musl 位运算实现一致。
     *
     * @param x 输入值
     * @return {@code x} 的绝对值
     */
    public static double fabs(double x) {
        return Math.abs(x);
    }

    // ------------------------------------------------------------------
    // 三角函数
    // ------------------------------------------------------------------

    /**
     * {@code double sin(double x)}：正弦，委托 {@link Math#sin(double)}。
     */
    public static double sin(double x) {
        return Math.sin(x);
    }

    /**
     * {@code double cos(double x)}：余弦，委托 {@link Math#cos(double)}。
     */
    public static double cos(double x) {
        return Math.cos(x);
    }

    /**
     * {@code double tan(double x)}：正切，委托 {@link Math#tan(double)}。
     */
    public static double tan(double x) {
        return Math.tan(x);
    }

    /**
     * {@code double asin(double x)}：反正弦，委托 {@link Math#asin(double)}。
     */
    public static double asin(double x) {
        return Math.asin(x);
    }

    /**
     * {@code double acos(double x)}：反余弦，委托 {@link Math#acos(double)}。
     */
    public static double acos(double x) {
        return Math.acos(x);
    }

    /**
     * {@code double atan(double x)}：反正切，委托 {@link Math#atan(double)}。
     */
    public static double atan(double x) {
        return Math.atan(x);
    }

    /**
     * {@code double atan2(double y, double x)}：两参数反正切，委托
     * {@link Math#atan2(double, double)}（参数顺序与 C 一致为 {@code (y, x)}）。
     */
    public static double atan2(double y, double x) {
        return Math.atan2(y, x);
    }

    // ------------------------------------------------------------------
    // 指数 / 对数 / 双曲
    // ------------------------------------------------------------------

    /**
     * {@code double exp(double x)}：自然指数，委托 {@link Math#exp(double)}。
     */
    public static double exp(double x) {
        return Math.exp(x);
    }

    /**
     * {@code double log(double x)}：自然对数，委托 {@link Math#log(double)}。
     */
    public static double log(double x) {
        return Math.log(x);
    }

    /**
     * {@code double log10(double x)}：常用对数，委托 {@link Math#log10(double)}。
     */
    public static double log10(double x) {
        return Math.log10(x);
    }

    /**
     * {@code double sinh(double x)}：双曲正弦，委托 {@link Math#sinh(double)}。
     */
    public static double sinh(double x) {
        return Math.sinh(x);
    }

    // ------------------------------------------------------------------
    // 其他
    // ------------------------------------------------------------------

    /**
     * {@code double ceil(double x)}：向上取整，返回不小于 {@code x} 的最小整数值。
     */
    public static double ceil(double x) {
        return Math.ceil(x);
    }

    /**
     * {@code double cbrt(double x)}：立方根，委托 {@link Math#cbrt(double)}。
     */
    public static double cbrt(double x) {
        return Math.cbrt(x);
    }

    /**
     * {@code double hypot(double x, double y)}：{@code sqrt(x*x + y*y)}，委托
     * {@link Math#hypot(double, double)}。
     */
    public static double hypot(double x, double y) {
        return Math.hypot(x, y);
    }
}