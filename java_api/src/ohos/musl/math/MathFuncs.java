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
     * {@code double log2(double x)}：以 2 为底的对数。实现为
     * {@code Math.log(x) / Math.log(2)}（近似，与 musl 的精确舍入有微小差异）。
     * {@code 0} 返回 -inf、负数返回 NaN、{@code +inf} 返回 +inf。
     *
     * @param x 输入值
     * @return {@code x} 以 2 为底的对数
     */
    public static double log2(double x) {
        return Math.log(x) / Math.log(2.0);
    }

    /**
     * {@code double expm1(double x)}：{@code exp(x) - 1}，对接近 0 的 {@code x} 保持
     * 精度，委托 {@link Math#expm1(double)}。
     *
     * @param x 指数
     * @return {@code exp(x) - 1}
     */
    public static double expm1(double x) {
        return Math.expm1(x);
    }

    /**
     * {@code double log1p(double x)}：{@code log(1 + x)}，对接近 0 的 {@code x} 保持
     * 精度，委托 {@link Math#log1p(double)}。{@code x < -1} 返回 NaN、{@code x == -1}
     * 返回 -inf。
     *
     * @param x 输入值
     * @return {@code log(1 + x)}
     */
    public static double log1p(double x) {
        return Math.log1p(x);
    }

    /**
     * {@code double exp2(double x)}：2 的 {@code x} 次幂，实现为 {@code pow(2.0, x)}。
     *
     * @param x 指数
     * @return {@code 2^x}
     */
    public static double exp2(double x) {
        return Math.pow(2.0, x);
    }

    /**
     * {@code double sinh(double x)}：双曲正弦，委托 {@link Math#sinh(double)}。
     */
    public static double sinh(double x) {
        return Math.sinh(x);
    }

    // ------------------------------------------------------------------
    // 取余 / 极值 / 符号
    // ------------------------------------------------------------------

    /**
     * {@code double fmod(double x, double y)}：浮点取余，结果符号随被除数 {@code x}。
     * Java 的 {@code %} 运算符即 C99 {@code fmod} 语义：{@code y == 0} 返回 NaN、
     * {@code y} 为无穷时返回 {@code x}、{@code x} 为无穷时返回 NaN。
     *
     * @param x 被除数
     * @param y 除数
     * @return {@code x} mod {@code y}（符号随 {@code x}）
     */
    public static double fmod(double x, double y) {
        return x % y;
    }

    /**
     * {@code double remainder(double x, double y)}：IEEE 754 余数（结果绝对值不超过
     * {@code |y|/2}，商向最近整数取整、平局取偶），委托
     * {@link Math#IEEEremainder(double, double)}。
     *
     * @param x 被除数
     * @param y 除数
     * @return IEEE 754 余数
     */
    public static double remainder(double x, double y) {
        return Math.IEEEremainder(x, y);
    }

    /**
     * {@code double fmin(double x, double y)}：两值中的较小者。与
     * {@link Math#min(double, double)} 不同，任一参数为 NaN 时返回另一个值；
     * {@code fmin(+0.0, -0.0)} 返回 {@code -0.0}。
     *
     * @param x 输入值
     * @param y 输入值
     * @return 较小者
     */
    public static double fmin(double x, double y) {
        if (Double.isNaN(x)) {
            return y;
        }
        if (Double.isNaN(y)) {
            return x;
        }
        return Math.min(x, y);
    }

    /**
     * {@code double fmax(double x, double y)}：两值中的较大者。任一参数为 NaN 时返回
     * 另一个值；{@code fmax(-0.0, +0.0)} 返回 {@code +0.0}。
     *
     * @param x 输入值
     * @param y 输入值
     * @return 较大者
     */
    public static double fmax(double x, double y) {
        if (Double.isNaN(x)) {
            return y;
        }
        if (Double.isNaN(y)) {
            return x;
        }
        return Math.max(x, y);
    }

    /**
     * {@code double copysign(double x, double y)}：返回绝对值取自 {@code x}、符号取自
     * {@code y} 的值，委托 {@link Math#copySign(double, double)}。注意 Java 对 NaN 的
     * 符号位约定为正（musl 为位操作，行为取决于 NaN 载荷）。
     *
     * @param x 提供绝对值的值
     * @param y 提供符号的值
     * @return 符号重设后的 {@code x}
     */
    public static double copysign(double x, double y) {
        return Math.copySign(x, y);
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
     * {@code double round(double x)}：四舍五入到最近的整数，平局时远离零（C99
     * half-away-from-zero）。不能直接用 {@link Math#round(double)}（Java 对负的半值
     * 向 {@code +inf} 舍入），这里以 {@code copySign(floor(|x| + 0.5), x)} 复刻。
     *
     * @param x 输入值
     * @return 最接近 {@code x} 的整数（平局远离零）
     */
    public static double round(double x) {
        return Math.copySign(Math.floor(Math.abs(x) + 0.5), x);
    }

    /**
     * {@code double trunc(double x)}：向零取整。实现为负数取 {@link Math#ceil(double)}、
     * 非负数取 {@link Math#floor(double)}，以保留 {@code -0.0} 等符号语义。
     *
     * @param x 输入值
     * @return 向零截断后的整数值
     */
    public static double trunc(double x) {
        return x < 0 ? Math.ceil(x) : Math.floor(x);
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

    // ------------------------------------------------------------------
    // 舍入到整数类型：rint / lrint / llrint / lround / llround
    // ------------------------------------------------------------------

    /**
     * {@code double rint(double x)}：按当前舍入模式舍入到整数（默认 IEEE 754
     * round-to-nearest-even），委托 {@link Math#rint(double)}。与
     * {@link #round(double)} 不同：{@code rint(2.5) == 2.0}（平局取偶）。
     *
     * @param x 输入值
     * @return 舍入后的整数值（仍为 {@code double}）
     */
    public static double rint(double x) {
        return Math.rint(x);
    }

    /**
     * {@code long lrint(double x)}：按 {@link #rint(double)} 舍入后截断为
     * {@code long}。结果超出 {@code long} 范围时 C 为未定义行为，Java 为截断回绕。
     *
     * @param x 输入值
     * @return 舍入后的 {@code long}
     */
    public static long lrint(double x) {
        return (long) Math.rint(x);
    }

    /**
     * {@code long long llrint(double x)}：{@code long long} 版本，C 的
     * {@code long long} 与本项目目标 {@code long} 同为 64 位，故复用
     * {@link #lrint(double)}。
     *
     * @param x 输入值
     * @return 舍入后的 {@code long}
     */
    public static long llrint(double x) {
        return lrint(x);
    }

    /**
     * {@code long lround(double x)}：按 {@link #round(double)}（平局远离零）舍入后
     * 截断为 {@code long}。结果超出 {@code long} 范围时 C 为未定义行为，Java 为截断
     * 回绕。
     *
     * @param x 输入值
     * @return 舍入后的 {@code long}
     */
    public static long lround(double x) {
        return (long) round(x);
    }

    /**
     * {@code long long llround(double x)}：{@code long long} 版本，复用
     * {@link #lround(double)}。
     *
     * @param x 输入值
     * @return 舍入后的 {@code long}
     */
    public static long llround(double x) {
        return lround(x);
    }

    // ------------------------------------------------------------------
    // 分解与缩放：frexp / ldexp / modf / scalbn
    // ------------------------------------------------------------------

    /**
     * Java 版 {@code frexp} 的返回值（C 通过出参指针返回尾数与指数）。
     */
    public static final class Frexp {
        /** 尾数 {@code m}，满足 {@code |m| ∈ [0.5, 1)}（0/inf/NaN 时原样返回）。 */
        public final double mantissa;
        /** 指数 {@code e}，满足 {@code x == m * 2^e}。 */
        public final int exp;

        private Frexp(double mantissa, int exp) {
            this.mantissa = mantissa;
            this.exp = exp;
        }
    }

    /**
     * {@code double frexp(double x, int *exp)}：将 {@code x} 分解为 {@code m * 2^e}，
     * 其中 {@code |m| ∈ [0.5, 1)}；Java 版以返回 {@link Frexp} 代替出参。特殊值：
     * 0/±0、inf、NaN 原样返回且 {@code e == 0}。
     *
     * @param x 输入值
     * @return 尾数与指数
     */
    public static Frexp frexp(double x) {
        if (x == 0.0 || Double.isNaN(x) || Double.isInfinite(x)) {
            return new Frexp(x, 0);
        }
        int e = Math.getExponent(x) + 1;
        return new Frexp(Math.scalb(x, -e), e);
    }

    /**
     * {@code double ldexp(double x, int e)}：{@code x * 2^e}，委托
     * {@link Math#scalb(double, int)}。
     *
     * @param x 尾数
     * @param e 二进制指数
     * @return {@code x * 2^e}
     */
    public static double ldexp(double x, int e) {
        return Math.scalb(x, e);
    }

    /**
     * Java 版 {@code modf} 的返回值（C 通过出参指针返回小数与整数部分）。
     */
    public static final class Modf {
        /** 小数部分（符号与 {@code x} 一致）。 */
        public final double frac;
        /** 整数部分（向零截断）。 */
        public final double integral;

        private Modf(double frac, double integral) {
            this.frac = frac;
            this.integral = integral;
        }
    }

    /**
     * {@code double modf(double x, double *iptr)}：将 {@code x} 拆为小数部分与向零
     * 截断的整数部分，Java 版以返回 {@link Modf} 代替出参。特殊值：0/±0、inf、NaN
     * 的整数部分与其自身相同、小数部分为同号 0。
     *
     * @param x 输入值
     * @return 小数部分与整数部分
     */
    public static Modf modf(double x) {
        if (x == 0.0 || Double.isNaN(x) || Double.isInfinite(x)) {
            return new Modf(x, x);
        }
        double integral = x < 0 ? Math.ceil(x) : Math.floor(x);
        return new Modf(x - integral, integral);
    }

    /**
     * {@code double scalbn(double x, int n)}：{@code x * FLT_RADIX^n}（二进制
     * {@code FLT_RADIX == 2}），委托 {@link Math#scalb(double, int)}。
     *
     * @param x 尾数
     * @param n 指数
     * @return {@code x * 2^n}
     */
    public static double scalbn(double x, int n) {
        return Math.scalb(x, n);
    }

    // ------------------------------------------------------------------
    // 指数取整 / 相邻值：ilogb / logb / nextafter / nextUp / nextDown
    // ------------------------------------------------------------------

    /**
     * {@code int ilogb(double x)}：返回 {@code x} 的无偏二进制指数（即
     * {@code floor(log2(|x|))}）。特殊值对齐 C：{@code x == 0} 返回
     * {@link Integer#MIN_VALUE}（{@code FP_ILOGB0}）；NaN/inf 返回
     * {@link Integer#MAX_VALUE}（{@code FP_ILOGBNAN}）；次正规数按其真实指数返回。
     *
     * @param x 输入值
     * @return 无偏指数
     */
    public static int ilogb(double x) {
        if (x == 0.0) {
            return Integer.MIN_VALUE;
        }
        if (Double.isNaN(x) || Double.isInfinite(x)) {
            return Integer.MAX_VALUE;
        }
        if (Math.getExponent(x) == Double.MIN_EXPONENT - 1) {
            // 次正规数：x = mantissa * 2^-1074，最高置位位在 52 位 mantissa 中的位置
            // 为 63 - numberOfLeadingZeros(mantissa)，故指数 = 63 - nLZ - 1074
            long mantissa = Double.doubleToLongBits(Math.abs(x)) & ((1L << 52) - 1);
            return -1011 - Long.numberOfLeadingZeros(mantissa);
        }
        return Math.getExponent(x);
    }

    /**
     * {@code double logb(double x)}：{@code floor(log2(|x|))}，返回 {@code double}。
     * 特殊值：{@code x == 0} 返回 -inf、NaN 返回 NaN、inf 返回 +inf；次正规数按其
     * 真实指数返回。
     *
     * @param x 输入值
     * @return 二进制指数
     */
    public static double logb(double x) {
        if (x == 0.0) {
            return Double.NEGATIVE_INFINITY;
        }
        if (Double.isNaN(x)) {
            return Double.NaN;
        }
        if (Double.isInfinite(x)) {
            return Double.POSITIVE_INFINITY;
        }
        return ilogb(x);
    }

    /**
     * {@code double nextafter(double x, double y)}：返回向 {@code y} 方向、紧邻
     * {@code x} 的下一个可表示 {@code double}，委托 {@link Math#nextAfter(double, double)}。
     *
     * @param x 起始值
     * @param y 目标方向
     * @return {@code x} 与 {@code y} 之间的相邻值
     */
    public static double nextafter(double x, double y) {
        return Math.nextAfter(x, y);
    }

    /**
     * {@code double nextUp(double x)}：返回大于 {@code x} 的最小可表示 {@code double}，
     * 委托 {@link Math#nextUp(double)}。
     *
     * @param x 输入值
     * @return 紧邻 {@code x} 的上一个值
     */
    public static double nextUp(double x) {
        return Math.nextUp(x);
    }

    /**
     * {@code double nextDown(double x)}：返回小于 {@code x} 的最大可表示 {@code double}，
     * 委托 {@link Math#nextDown(double)}。
     *
     * @param x 输入值
     * @return 紧邻 {@code x} 的下一个值
     */
    public static double nextDown(double x) {
        return Math.nextDown(x);
    }

    // ------------------------------------------------------------------
    // 其他：fdim / fma
    // ------------------------------------------------------------------

    /**
     * {@code double fdim(double x, double y)}：正差，即 {@code x > y ? x - y : +0.0}。
     * 任一参数为 NaN 时返回 NaN。
     *
     * @param x 输入值
     * @param y 输入值
     * @return 正差
     */
    public static double fdim(double x, double y) {
        if (Double.isNaN(x) || Double.isNaN(y)) {
            return Double.NaN;
        }
        return x > y ? x - y : 0.0;
    }

    /**
     * {@code double fma(double x, double y, double z)}：{@code x*y + z} 的融合乘加
     * （单次舍入），委托 {@link Math#fma(double, double, double)}（JDK 9+）。
     *
     * @param x 乘数
     * @param y 乘数
     * @param z 加数
     * @return {@code x*y + z}（一次舍入）
     */
    public static double fma(double x, double y, double z) {
        return Math.fma(x, y, z);
    }

    // ------------------------------------------------------------------
    // 分类谓词：isnan / isinf / isfinite / isnormal / signbit
    // ------------------------------------------------------------------

    /**
     * {@code int isnan(double x)}：是否为 NaN，映射为 {@code boolean}。
     *
     * @param x 输入值
     * @return 是 NaN 返回 {@code true}
     */
    public static boolean isnan(double x) {
        return Double.isNaN(x);
    }

    /**
     * {@code int isinf(double x)}：是否为正负无穷，映射为 {@code boolean}。
     *
     * @param x 输入值
     * @return 是无穷返回 {@code true}
     */
    public static boolean isinf(double x) {
        return Double.isInfinite(x);
    }

    /**
     * {@code int isfinite(double x)}：是否为有限值（非 NaN、非无穷）。
     *
     * @param x 输入值
     * @return 有限返回 {@code true}
     */
    public static boolean isfinite(double x) {
        return !Double.isNaN(x) && !Double.isInfinite(x);
    }

    /**
     * {@code int isnormal(double x)}：是否为正规数（非 0、非次正规、非 inf、非 NaN）。
     *
     * @param x 输入值
     * @return 正规返回 {@code true}
     */
    public static boolean isnormal(double x) {
        if (Double.isNaN(x) || Double.isInfinite(x) || x == 0.0) {
            return false;
        }
        return Math.getExponent(x) != Double.MIN_EXPONENT - 1;
    }

    /**
     * {@code int signbit(double x)}：符号位是否为 1（含 {@code -0.0} 与负 NaN 载荷），
     * 映射为 {@code boolean}。以符号位判断实现，而非与 0 比较。
     *
     * @param x 输入值
     * @return 符号位为 1 返回 {@code true}
     */
    public static boolean signbit(double x) {
        return (Double.doubleToLongBits(x) & Long.MIN_VALUE) != 0;
    }

    // ------------------------------------------------------------------
    // C99 比较宏：isgreater / isgreaterequal / isless / islessequal /
    // islessgreater / isunordered / fpclassify
    // ------------------------------------------------------------------

    /** {@code fpclassify} 的返回值：NaN。 */
    public static final int FP_NAN = 0;
    /** {@code fpclassify} 的返回值：无穷。 */
    public static final int FP_INFINITE = 1;
    /** {@code fpclassify} 的返回值：零。 */
    public static final int FP_ZERO = 2;
    /** {@code fpclassify} 的返回值：次正规数。 */
    public static final int FP_SUBNORMAL = 3;
    /** {@code fpclassify} 的返回值：正规数。 */
    public static final int FP_NORMAL = 4;

    /**
     * {@code int isgreater(x, y)}（math.h 宏）：等价于 {@code x > y}（任一为 NaN 时为
     * 假），映射为 {@code boolean}。
     */
    public static boolean isgreater(double x, double y) {
        return x > y;
    }

    /**
     * {@code int isgreaterequal(x, y)}（math.h 宏）：等价于 {@code x >= y}（任一为 NaN
     * 时为假），映射为 {@code boolean}。
     */
    public static boolean isgreaterequal(double x, double y) {
        return x >= y;
    }

    /**
     * {@code int isless(x, y)}（math.h 宏）：等价于 {@code x < y}（任一为 NaN 时为假），
     * 映射为 {@code boolean}。
     */
    public static boolean isless(double x, double y) {
        return x < y;
    }

    /**
     * {@code int islessequal(x, y)}（math.h 宏）：等价于 {@code x <= y}（任一为 NaN 时
     * 为假），映射为 {@code boolean}。
     */
    public static boolean islessequal(double x, double y) {
        return x <= y;
    }

    /**
     * {@code int islessgreater(x, y)}（math.h 宏）：{@code x < y || x > y}（任一为 NaN
     * 时为假），映射为 {@code boolean}。
     */
    public static boolean islessgreater(double x, double y) {
        return x < y || x > y;
    }

    /**
     * {@code int isunordered(x, y)}（math.h 宏）：任一参数为 NaN 即为真，映射为
     * {@code boolean}。
     */
    public static boolean isunordered(double x, double y) {
        return Double.isNaN(x) || Double.isNaN(y);
    }

    /**
     * {@code int fpclassify(x)}（math.h 宏）：返回 {@code x} 的类别，取值为
     * {@link #FP_NAN} / {@link #FP_INFINITE} / {@link #FP_ZERO} /
     * {@link #FP_SUBNORMAL} / {@link #FP_NORMAL} 之一。
     *
     * @param x 输入值
     * @return 分类常量
     */
    public static int fpclassify(double x) {
        if (Double.isNaN(x)) {
            return FP_NAN;
        }
        if (Double.isInfinite(x)) {
            return FP_INFINITE;
        }
        if (x == 0.0) {
            return FP_ZERO;
        }
        if (Math.getExponent(x) == Double.MIN_EXPONENT - 1) {
            return FP_SUBNORMAL;
        }
        return FP_NORMAL;
    }

    // ------------------------------------------------------------------
    // scalbln / nan / remquo / exp10
    // ------------------------------------------------------------------

    /**
     * {@code double scalbln(double x, long n)}：{@code x * 2^n}，指数为 {@code long}，
     * 委托 {@link Math#scalb(double, int)}（先钳制到 {@code int} 范围）。
     *
     * @param x 尾数
     * @param n 二进制指数
     * @return {@code x * 2^n}
     */
    public static double scalbln(double x, long n) {
        int e = (int) Math.max(Integer.MIN_VALUE, Math.min(Integer.MAX_VALUE, n));
        return Math.scalb(x, e);
    }

    /**
     * {@code double nan(const char *s)}：返回安静的 NaN（忽略字符串载荷，恒返回
     * {@link Double#NaN}）。
     *
     * @param s 载荷字符串（本实现忽略）
     * @return NaN
     */
    public static double nan(String s) {
        return Double.NaN;
    }

    /**
     * Java 版 {@code remquo} 的返回（C 通过出参指针返回商）。{@code quo} 为商的低 3 位
     * （符号取商），{@code rem} 为 IEEE 754 余数。
     */
    public static final class Remquo {
        /** IEEE 754 余数。 */
        public final double rem;
        /** 商的低 3 位（含符号）。 */
        public final int quo;

        private Remquo(double rem, int quo) {
            this.rem = rem;
            this.quo = quo;
        }
    }

    /**
     * {@code double remquo(double x, double y, int *quo)}：返回 IEEE 754 余数（同
     * {@link #remainder(double, double)}），并计算商的低 3 位（含符号），Java 版以返回
     * {@link Remquo} 代替出参。
     *
     * @param x 被除数
     * @param y 除数
     * @return 余数与商的低 3 位
     */
    public static Remquo remquo(double x, double y) {
        double rem = Math.IEEEremainder(x, y);
        int quo = ((int) Math.round((x - rem) / y)) & 7;
        return new Remquo(rem, quo);
    }

    /**
     * {@code double exp10(double x)}（GNU 扩展）：10 的 {@code x} 次幂，实现为
     * {@code pow(10.0, x)}。
     *
     * @param x 指数
     * @return {@code 10^x}
     */
    public static double exp10(double x) {
        return Math.pow(10.0, x);
    }

    // ------------------------------------------------------------------
    // float 变体（委托 double 实现后收窄，两次舍入与 C 的单次舍入有极微小差异）
    // ------------------------------------------------------------------

    /** {@code float sqrtf(float x)}：平方根（float）。 */
    public static float sqrtf(float x) {
        return (float) Math.sqrt(x);
    }

    /** {@code float powf(float x, float y)}：幂运算（float）。 */
    public static float powf(float x, float y) {
        return (float) Math.pow(x, y);
    }

    /** {@code float fabsf(float x)}：绝对值（float）。 */
    public static float fabsf(float x) {
        return Math.abs(x);
    }

    /** {@code float floorf(float x)}：向下取整（float）。 */
    public static float floorf(float x) {
        return (float) Math.floor(x);
    }

    /** {@code float ceilf(float x)}：向上取整（float）。 */
    public static float ceilf(float x) {
        return (float) Math.ceil(x);
    }

    /** {@code float fmodf(float x, float y)}：浮点取余（float），符号随被除数。 */
    public static float fmodf(float x, float y) {
        return x % y;
    }

    /** {@code float fminf(float x, float y)}：较小者（float），任一 NaN 返回另一值。 */
    public static float fminf(float x, float y) {
        return (float) fmin(x, y);
    }

    /** {@code float fmaxf(float x, float y)}：较大者（float），任一 NaN 返回另一值。 */
    public static float fmaxf(float x, float y) {
        return (float) fmax(x, y);
    }

    /** {@code float copysignf(float x, float y)}：符号重设（float）。 */
    public static float copysignf(float x, float y) {
        return Math.copySign(x, y);
    }

    /** {@code float hypotf(float x, float y)}：斜边长度（float）。 */
    public static float hypotf(float x, float y) {
        return (float) Math.hypot(x, y);
    }

    /** {@code float cbrtf(float x)}：立方根（float）。 */
    public static float cbrtf(float x) {
        return (float) Math.cbrt(x);
    }

    /** {@code float roundf(float x)}：四舍五入（float，平局远离零）。 */
    public static float roundf(float x) {
        return (float) round(x);
    }

    /** {@code float truncf(float x)}：向零取整（float）。 */
    public static float truncf(float x) {
        return (float) trunc(x);
    }

    /** {@code float expf(float x)}：自然指数（float）。 */
    public static float expf(float x) {
        return (float) Math.exp(x);
    }

    /** {@code float exp2f(float x)}：2 的 {@code x} 次幂（float）。 */
    public static float exp2f(float x) {
        return (float) exp2(x);
    }

    /** {@code float expm1f(float x)}：{@code exp(x)-1}（float）。 */
    public static float expm1f(float x) {
        return (float) Math.expm1(x);
    }

    /** {@code float logf(float x)}：自然对数（float）。 */
    public static float logf(float x) {
        return (float) Math.log(x);
    }

    /** {@code float log2f(float x)}：以 2 为底对数（float）。 */
    public static float log2f(float x) {
        return (float) log2(x);
    }

    /** {@code float log10f(float x)}：常用对数（float）。 */
    public static float log10f(float x) {
        return (float) Math.log10(x);
    }

    /** {@code float log1pf(float x)}：{@code log(1+x)}（float）。 */
    public static float log1pf(float x) {
        return (float) Math.log1p(x);
    }

    /** {@code float sinf(float x)}：正弦（float）。 */
    public static float sinf(float x) {
        return (float) Math.sin(x);
    }

    /** {@code float cosf(float x)}：余弦（float）。 */
    public static float cosf(float x) {
        return (float) Math.cos(x);
    }

    /** {@code float tanf(float x)}：正切（float）。 */
    public static float tanf(float x) {
        return (float) Math.tan(x);
    }

    /** {@code float sinhf(float x)}：双曲正弦（float）。 */
    public static float sinhf(float x) {
        return (float) Math.sinh(x);
    }

    /** {@code float coshf(float x)}：双曲余弦（float）。 */
    public static float coshf(float x) {
        return (float) Math.cosh(x);
    }

    /** {@code float tanhf(float x)}：双曲正切（float）。 */
    public static float tanhf(float x) {
        return (float) Math.tanh(x);
    }

    /** {@code float asinf(float x)}：反正弦（float）。 */
    public static float asinf(float x) {
        return (float) Math.asin(x);
    }

    /** {@code float acosf(float x)}：反余弦（float）。 */
    public static float acosf(float x) {
        return (float) Math.acos(x);
    }

    /** {@code float atanf(float x)}：反正切（float）。 */
    public static float atanf(float x) {
        return (float) Math.atan(x);
    }

    /** {@code float atan2f(float y, float x)}：两参数反正切（float）。 */
    public static float atan2f(float y, float x) {
        return (float) Math.atan2(y, x);
    }

    /** {@code float rintf(float x)}：按当前舍入模式取整（float，平局取偶）。 */
    public static float rintf(float x) {
        return (float) Math.rint(x);
    }

    /** {@code float fdimf(float x, float y)}：正差（float）。 */
    public static float fdimf(float x, float y) {
        return (float) fdim(x, y);
    }

    /** {@code float nextafterf(float x, float y)}：相邻 float 值。 */
    public static float nextafterf(float x, float y) {
        return Math.nextAfter(x, y);
    }

    /** {@code float remainderf(float x, float y)}：IEEE 754 余数（float）。 */
    public static float remainderf(float x, float y) {
        return (float) Math.IEEEremainder(x, y);
    }

    /** {@code float fmaf(float x, float y, float z)}：融合乘加（float）。 */
    public static float fmaf(float x, float y, float z) {
        return (float) Math.fma(x, y, z);
    }

    /** {@code float scalbnf(float x, int n)}：{@code x * 2^n}（float）。 */
    public static float scalbnf(float x, int n) {
        return Math.scalb(x, n);
    }

    /** {@code float ldexpf(float x, int e)}：{@code x * 2^e}（float）。 */
    public static float ldexpf(float x, int e) {
        return Math.scalb(x, e);
    }

    // ------------------------------------------------------------------
    // 误差函数 / 伽马函数（数值实现）
    // ------------------------------------------------------------------

    /**
     * {@code double erf(double x)}：误差函数，采用 Abramowitz &amp; Stegun 7.1.26 近似
     * （绝对误差约 1.5e-7）。{@code ±0} 保持符号、{@code ±inf} 返回 {@code ±1}、NaN
     * 返回 NaN。
     *
     * @param x 输入值
     * @return {@code erf(x)}
     */
    public static double erf(double x) {
        if (Double.isNaN(x)) {
            return Double.NaN;
        }
        if (x == 0.0) {
            return x;
        }
        double ax = Math.abs(x);
        double t = 1.0 / (1.0 + 0.3275911 * ax);
        double poly = t * (0.254829592 + t * (-0.284496736 + t * (1.421413741
                + t * (-1.453152027 + t * 1.061405429))));
        double r = 1.0 - poly * Math.exp(-ax * ax);
        return x < 0 ? -r : r;
    }

    /**
     * {@code double erfc(double x)}：互补误差函数 {@code 1 - erf(x)}。
     *
     * @param x 输入值
     * @return {@code 1 - erf(x)}
     */
    public static double erfc(double x) {
        if (Double.isNaN(x)) {
            return Double.NaN;
        }
        return 1.0 - erf(x);
    }

    /** Lanczos 近似系数（g = 7，n = 9）。 */
    private static final double[] LANCZOS = {
        0.99999999999980993, 676.5203681218851, -1259.1392167224028,
        771.32342877765313, -176.61502916214059, 12.507343278686905,
        -0.13857109526572012, 9.9843695780195716e-6, 1.5056327351493116e-7
    };

    /**
     * {@code double tgamma(double x)}：伽马函数。正数用 Lanczos 近似（g = 7，n = 9），
     * 负数经反射公式 {@code Γ(x) = π / (sin(πx) · Γ(1-x))}；负整数极点返回 NaN、
     * {@code ±0} 按符号返回 {@code ±inf}。
     *
     * @param x 输入值
     * @return {@code Γ(x)}
     */
    public static double tgamma(double x) {
        if (Double.isNaN(x)) {
            return Double.NaN;
        }
        if (Double.isInfinite(x)) {
            return x > 0 ? Double.POSITIVE_INFINITY : Double.NaN;
        }
        if (x == 0.0) {
            return 1.0 / x < 0 ? Double.NEGATIVE_INFINITY : Double.POSITIVE_INFINITY;
        }
        if (x < 0.0) {
            if (x == Math.floor(x)) {
                return Double.NaN;
            }
            return Math.PI / (Math.sin(Math.PI * x) * tgamma(1.0 - x));
        }
        return lanczosGamma(x);
    }

    private static double lanczosGamma(double x) {
        double z = x - 1.0;
        double a = LANCZOS[0];
        for (int i = 1; i < LANCZOS.length; i++) {
            a += LANCZOS[i] / (z + i);
        }
        double t = z + 7.5;
        return Math.sqrt(2.0 * Math.PI) * Math.pow(t, z + 0.5) * Math.exp(-t) * a;
    }

    /**
     * {@code double lgamma(double x)}：伽马函数绝对值的自然对数。正数用 Lanczos 求值后
     * 取对数，负数用反射公式，非正极点（0、负整数）返回 {@code +inf}。
     *
     * @param x 输入值
     * @return {@code log|Γ(x)|}
     */
    public static double lgamma(double x) {
        if (Double.isNaN(x)) {
            return Double.NaN;
        }
        if (x == Double.POSITIVE_INFINITY) {
            return Double.POSITIVE_INFINITY;
        }
        if (x <= 0.0) {
            if (x == Math.floor(x)) {
                return Double.POSITIVE_INFINITY;
            }
            return Math.log(Math.PI / Math.abs(Math.sin(Math.PI * x))) - lgamma(1.0 - x);
        }
        return Math.log(lanczosGamma(x));
    }

    /** {@code float erff(float x)}：误差函数（float）。 */
    public static float erff(float x) {
        return (float) erf(x);
    }

    /** {@code float erfcf(float x)}：互补误差函数（float）。 */
    public static float erfcf(float x) {
        return (float) erfc(x);
    }

    /** {@code float tgammaf(float x)}：伽马函数（float）。 */
    public static float tgammaf(float x) {
        return (float) tgamma(x);
    }

    /** {@code float lgammaf(float x)}：伽马函数绝对值的对数（float）。 */
    public static float lgammaf(float x) {
        return (float) lgamma(x);
    }

    // ------------------------------------------------------------------
    // 更多 double / float 接口
    // ------------------------------------------------------------------

    /**
     * {@code double nearbyint(double x)}：按当前舍入模式取整（默认平局取偶），同
     * {@link #rint(double)}，委托 {@link Math#rint(double)}。
     *
     * @param x 输入值
     * @return 取整后的 {@code double}
     */
    public static double nearbyint(double x) {
        return Math.rint(x);
    }

    /** {@code float nearbyintf(float x)}：按当前舍入模式取整（float）。 */
    public static float nearbyintf(float x) {
        return (float) Math.rint(x);
    }

    /** {@code int ilogbf(float x)}：无偏二进制指数（float），特殊值约定同
     * {@link #ilogb(double)}。 */
    public static int ilogbf(float x) {
        return ilogb(x);
    }

    /** {@code float logbf(float x)}：二进制指数（float）。 */
    public static float logbf(float x) {
        return (float) logb(x);
    }

    /** {@code float scalblnf(float x, long n)}：{@code x * 2^n}（float，long 指数）。 */
    public static float scalblnf(float x, long n) {
        return (float) scalbln(x, n);
    }

    /** {@code float exp10f(float x)}：10 的 {@code x} 次幂（float）。 */
    public static float exp10f(float x) {
        return (float) exp10(x);
    }

    /** {@code float nanf(const char *s)}：安静 NaN（float）。 */
    public static float nanf(String s) {
        return Float.NaN;
    }

    /** {@code float nextupf(float x)}：紧邻 {@code x} 的上一个 float 值。 */
    public static float nextupf(float x) {
        return Math.nextUp(x);
    }

    /** {@code float nextdownf(float x)}：紧邻 {@code x} 的下一个 float 值。 */
    public static float nextdownf(float x) {
        return Math.nextDown(x);
    }

    /** {@code float frexpf(float x, int *exp)}：分解尾数与指数（float），返回
     * {@link Frexp}。 */
    public static Frexp frexpf(float x) {
        Frexp f = frexp(x);
        return new Frexp((float) f.mantissa, f.exp);
    }

    /** {@code float modff(float x, float *iptr)}：拆分小数与整数部分（float），返回
     * {@link Modf}。 */
    public static Modf modff(float x) {
        Modf m = modf(x);
        return new Modf((float) m.frac, (float) m.integral);
    }

    /** {@code float remquof(float x, float y, int *quo)}：IEEE 754 余数与商的低 3 位
     * （float），返回 {@link Remquo}。 */
    public static Remquo remquof(float x, float y) {
        Remquo r = remquo(x, y);
        return new Remquo((float) r.rem, r.quo);
    }
}