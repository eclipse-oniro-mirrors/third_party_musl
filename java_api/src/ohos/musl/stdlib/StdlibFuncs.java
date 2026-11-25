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

import ohos.musl.ctype.CtypeFuncs;

import java.math.BigInteger;
import java.util.Comparator;

/**
 * Java 版本的 {@code <stdlib.h>} 纯函数接口。
 *
 * <p>仅包含无全局状态、无内存所有权、无 errno 依赖的部分：绝对值、整数除法
 * （{@code div/ldiv/lldiv}）、数字字符串解析（{@code ato* / strto* / strtou*}）与
 * 二分查找（{@code bsearch}）。{@code atoi} 复用
 * {@link CtypeFuncs#isspace(int)} / {@link CtypeFuncs#isdigit(int)}。</p>
 */
public final class StdlibFuncs {

    private StdlibFuncs() {
        // 工具类，禁止实例化。
    }

    /** {@code 2^64}，用于无符号 64 位回绕。 */
    private static final BigInteger TWO_64 = BigInteger.ONE.shiftLeft(64);
    /** {@code ULONG_MAX == 2^64 - 1}。 */
    private static final BigInteger ULONG_MAX = TWO_64.subtract(BigInteger.ONE);

    /**
     * {@code int abs(int a)}：返回 {@code a} 的绝对值，等价于
     * {@code return a > 0 ? a : -a;}。
     *
     * <p>注意与 C 一样，{@code Integer.MIN_VALUE} 的绝对值仍为
     * {@code Integer.MIN_VALUE}（C 中为未定义行为，Java 为约定的溢出回绕）。</p>
     *
     * @param a 输入整数
     * @return {@code a} 的绝对值
     */
    public static int abs(int a) {
        return a > 0 ? a : -a;
    }

    /**
     * {@code long labs(long a)}：返回 {@code long} 的绝对值，等价于
     * {@code return a > 0 ? a : -a;}。
     *
     * @param a 输入长整数
     * @return {@code a} 的绝对值
     */
    public static long labs(long a) {
        return a > 0 ? a : -a;
    }

    /**
     * {@code int atoi(const char *s)}：解析字符串开头的十进制整数。
     *
     * <p>语义与 musl 一致：跳过前导空白（{@code isspace}），可选 {@code +/-} 符号，
     * 然后连续读取数字（遇首个非数字字符停止）；无有效数字时返回 0。按负数累加以
     * 复刻 C 对 {@code INT_MIN} 的处理。</p>
     *
     * @param s 待解析字符串（不能为 {@code null}）
     * @return 解析得到的 {@code int}；越界时行为与 C 的溢出等同（未定义/回绕）
     */
    public static int atoi(String s) {
        int n = 0;
        int neg = 0;
        int i = 0;
        while (i < s.length() && CtypeFuncs.isspace(s.charAt(i))) {
            i++;
        }
        if (i < s.length()) {
            if (s.charAt(i) == '-') {
                neg = 1;
                i++;
            } else if (s.charAt(i) == '+') {
                i++;
            }
        }
        while (i < s.length() && CtypeFuncs.isdigit(s.charAt(i))) {
            n = 10 * n - (s.charAt(i++) - '0');
        }
        return neg == 1 ? n : -n;
    }

    /**
     * {@code long atol(const char *s)}：解析 {@code long}，规则同 {@link #atoi(String)}。
     *
     * @param s 待解析字符串
     * @return 解析得到的 {@code long}
     */
    public static long atol(String s) {
        long n = 0;
        int neg = 0;
        int i = 0;
        while (i < s.length() && CtypeFuncs.isspace(s.charAt(i))) {
            i++;
        }
        if (i < s.length()) {
            if (s.charAt(i) == '-') {
                neg = 1;
                i++;
            } else if (s.charAt(i) == '+') {
                i++;
            }
        }
        while (i < s.length() && CtypeFuncs.isdigit(s.charAt(i))) {
            n = 10 * n - (s.charAt(i++) - '0');
        }
        return neg == 1 ? n : -n;
    }

    /**
     * {@code long long atoll(const char *s)}：解析 {@code long long}，规则同
     * {@link #atoi(String)}。C 的 {@code long long} 与本项目目标 {@code long} 同为
     * 64 位，故复用 {@link #atol(String)}。
     *
     * @param s 待解析字符串
     * @return 解析得到的 {@code long}
     */
    public static long atoll(String s) {
        return atol(s);
    }

    /**
     * {@code long long llabs(long long a)}：返回 {@code long long} 的绝对值（Java
     * {@code long}）。
     *
     * @param a 输入长整数
     * @return {@code a} 的绝对值
     */
    public static long llabs(long a) {
        return a > 0 ? a : -a;
    }

    // ------------------------------------------------------------------
    // 整数除法：div / ldiv / lldiv
    // ------------------------------------------------------------------

    /**
     * Java 版 C 的 {@code div_t} 结构体映射：{@code div_t { int quot; int rem; }}。
     *
     * <p>字段不可变，仅能通过 {@link StdlibFuncs#div(int, int)} 获得。</p>
     */
    public static final class Div {
        /** 商。 */
        public final int quot;
        /** 余数。 */
        public final int rem;

        private Div(int quot, int rem) {
            this.quot = quot;
            this.rem = rem;
        }
    }

    /**
     * Java 版 C 的 {@code ldiv_t} / {@code lldiv_t} 结构体映射（本项目中
     * {@code long} 与 {@code long long} 同为 64 位，两者共用）。字段不可变。
     */
    public static final class Ldiv {
        /** 商。 */
        public final long quot;
        /** 余数。 */
        public final long rem;

        private Ldiv(long quot, long rem) {
            this.quot = quot;
            this.rem = rem;
        }
    }

    /**
     * {@code div_t div(int numer, int denom)}：返回整数除法结果，商向零截断
     * （C99 语义），余数满足 {@code numer == denom * quot + rem} 且符号与被除数一致。
     *
     * <p>与 C 相同，{@code Integer.MIN_VALUE / -1} 为溢出（Java 为约定回绕）；
     * {@code denom == 0} 抛出 {@link ArithmeticException}（C 中为未定义行为）。</p>
     *
     * @param numer 被除数
     * @param denom 除数（不能为 0）
     * @return 商与余数
     */
    public static Div div(int numer, int denom) {
        return new Div(numer / denom, numer % denom);
    }

    /**
     * {@code ldiv_t ldiv(long numer, long denom)}：{@code long} 版本的整数除法，规则同
     * {@link #div(int, int)}。
     *
     * @param numer 被除数
     * @param denom 除数（不能为 0）
     * @return 商与余数
     */
    public static Ldiv ldiv(long numer, long denom) {
        return new Ldiv(numer / denom, numer % denom);
    }

    /**
     * {@code lldiv_t lldiv(long long numer, long long denom)}：{@code long long} 版本
     * 的整数除法。C 的 {@code long long} 与本项目目标 {@code long} 同为 64 位，故复用
     * {@link #ldiv(long, long)}。
     *
     * @param numer 被除数
     * @param denom 除数（不能为 0）
     * @return 商与余数
     */
    public static Ldiv lldiv(long numer, long denom) {
        return ldiv(numer, denom);
    }

    /**
     * {@code double atof(const char *s)}：解析 {@code double}，等价于
     * {@code strtod(s, NULL)}，直接委托 {@link #strtod(String)}。
     *
     * @param s 待解析字符串
     * @return 解析得到的 {@code double}
     */
    public static double atof(String s) {
        return strtod(s);
    }

    /**
     * {@code double strtod(const char *s, char **end)}：解析字符串开头的浮点数，
     * Java 版省略 end 指针参数，仅返回数值。
     *
     * <p>语义对齐 musl {@code __floatscan}：跳过前导空白与 {@code +/-} 符号，支持
     * {@code inf}/{@code infinity}（区分大小写，只需匹配到 {@code "inf"} 前缀）、
     * {@code nan}/{@code nan(chars)}、十进制（{@code 123 / 123. / .5 / 1.23e4}，指数
     * 后无数字时不消费指数）与 C99 十六进制浮点（{@code 0x1.8p3}，二进制指数
     * {@code p}/{@code P} 后无数字时不消费）。无有效数字返回 {@code 0.0}。</p>
     *
     * <p>十进制子串交由 {@link Double#parseDouble(String)}（正确舍入）解析；十六进制
     * 用 {@link BigInteger} 求尾数后由 {@link Math#scalb(double, int)} 缩放，精度与
     * 溢出/下溢行为对齐 IEEE-754。</p>
     *
     * @param s 待解析字符串
     * @return 解析得到的 {@code double}
     */
    public static double strtod(String s) {
        int i = 0;
        int n = s.length();
        while (i < n && CtypeFuncs.isspace(s.charAt(i))) {
            i++;
        }
        boolean neg = false;
        if (i < n && (s.charAt(i) == '+' || s.charAt(i) == '-')) {
            neg = s.charAt(i) == '-';
            i++;
        }
        if (i < n && matchesIgnoreCase(s, i, "inf")) {
            return neg ? Double.NEGATIVE_INFINITY : Double.POSITIVE_INFINITY;
        }
        if (i < n && matchesIgnoreCase(s, i, "nan")) {
            return Double.NaN;
        }
        double v;
        if (i + 1 < n && s.charAt(i) == '0' && (s.charAt(i + 1) == 'x' || s.charAt(i + 1) == 'X')) {
            v = parseHex(s, i + 2);
        } else {
            v = parseDec(s, i);
        }
        return neg ? -v : v;
    }

    /**
     * {@code float strtof(const char *s, char **end)}：解析 {@code float}，规则同
     * {@link #strtod(String)}。Java 以 {@code (float) strtod(s)} 实现（先按 double 解析
     * 再收窄，两次舍入与 C 的单次舍入存在极微小差异）。
     *
     * @param s 待解析字符串
     * @return 解析得到的 {@code float}
     */
    public static float strtof(String s) {
        return (float) strtod(s);
    }

    /**
     * {@code long strtol(const char *s, char **end, int base)}：按 {@code base} 进制
     * 解析字符串开头的整数，Java 版省略 end 指针，仅返回数值。
     *
     * <p>语义对齐 musl {@code __intscan}：跳过前导空白与符号；{@code base == 0} 时按
     * {@code 0x/0X} 前缀自动识别十六进制、按 {@code 0} 前缀识别八进制、否则十进制；
     * {@code base == 16} 额外识别 {@code 0x/0X} 前缀。溢出时返回边界值
     * （正溢出 {@link Long#MAX_VALUE}，负溢出 {@link Long#MIN_VALUE}，与 C 在
     * {@code ERANGE} 下返回 {@code LONG_MAX}/{@code LONG_MIN} 一致）。{@code base} 为
     * 非法值（负、1 或大于 36）时返回 0。不使用十进制 {@code 0b} 二进制前缀（经典
     * C99 语义）。</p>
     *
     * @param s 待解析字符串
     * @param base 进制（0 表示自动检测，或 2..36）
     * @return 解析得到的 {@code long}
     */
    public static long strtol(String s, int base) {
        IntScan scan = scanInt(s, base);
        if (!scan.any) {
            return 0L;
        }
        BigInteger val = scan.neg ? scan.mag.negate() : scan.mag;
        if (val.compareTo(BigInteger.valueOf(Long.MAX_VALUE)) > 0) {
            return Long.MAX_VALUE;
        }
        if (val.compareTo(BigInteger.valueOf(Long.MIN_VALUE)) < 0) {
            return Long.MIN_VALUE;
        }
        return val.longValue();
    }

    /**
     * {@code unsigned long strtoul(const char *s, char **end, int base)}：按
     * {@code base} 进制解析无符号整数，前缀/进制识别规则同 {@link #strtol(String, int)}。
     *
     * <p>Java 无无符号整数，返回的 {@code long} 表示该无符号值的 64 位位模式（超出
     * {@link Long#MAX_VALUE} 的值表现为负 {@code long}）。溢出时钳制到
     * {@code ULONG_MAX}（{@code 0xFFFFFFFFFFFFFFFF}，即 {@code -1L}），与 C 在
     * {@code ERANGE} 下返回 {@code ULONG_MAX} 一致；负号按无符号回绕（如
     * {@code "-1"} 得 {@code ULONG_MAX}）。</p>
     *
     * @param s 待解析字符串
     * @param base 进制（0 表示自动检测，或 2..36）
     * @return 解析得到的无符号值的 64 位位模式
     */
    public static long strtoul(String s, int base) {
        IntScan scan = scanInt(s, base);
        if (!scan.any) {
            return 0L;
        }
        BigInteger mag = scan.mag;
        if (mag.compareTo(ULONG_MAX) > 0) {
            mag = ULONG_MAX;
        }
        if (scan.neg) {
            if (mag.signum() == 0) {
                return 0L;
            }
            return TWO_64.subtract(mag).longValue();
        }
        return mag.longValue();
    }

    /**
     * {@code unsigned long long strtoull(const char *s, char **end, int base)}：
     * {@code unsigned long long} 版本的 {@link #strtoul(String, int)}。C 的
     * {@code unsigned long} 与 {@code unsigned long long} 在本项目目标下均为 64 位，
     * 故复用 {@link #strtoul(String, int)}。
     *
     * @param s 待解析字符串
     * @param base 进制（0 表示自动检测，或 2..36）
     * @return 解析得到的无符号值的 64 位位模式
     */
    public static long strtoull(String s, int base) {
        return strtoul(s, base);
    }

    /**
     * {@code long long strtoll(const char *s, char **end, int base)}：{@code long long}
     * 版本的 {@link #strtol(String, int)}。C 的 {@code long long} 与本项目目标
     * {@code long} 同为 64 位，故复用 {@link #strtol(String, int)}。
     *
     * @param s 待解析字符串
     * @param base 进制（0 表示自动检测，或 2..36）
     * @return 解析得到的 {@code long}
     */
    public static long strtoll(String s, int base) {
        return strtol(s, base);
    }

    // ------------------------------------------------------------------
    // 二分查找：bsearch
    // ------------------------------------------------------------------

    /**
     * {@code void *bsearch(const void *key, const void *base, size_t nmemb,
     * size_t size, int (*compar)(const void *, const void *))}：在已按
     * {@code compar} 升序排序的数组中查找 {@code key}，返回命中下标；未找到
     * （C 返回 {@code NULL}）返回 -1。
     *
     * <p>C 的 {@code (base, nmemb, size)} 映射为泛型数组 {@code T[]}，比较函数指针
     * 映射为 {@link Comparator}，返回指针映射为 {@code int} 下标。移植 musl 的
     * {@code bsearch} 实现（命中中点即返回，故存在重复元素时返回第一个被比较到的
     * 匹配下标，结果确定）。</p>
     *
     * @param key 待查找的键
     * @param base 已按 {@code compar} 升序排序的数组
     * @param compar 比较器
     * @param <T> 元素类型
     * @return 命中下标，未找到返回 -1
     */
    public static <T> int bsearch(T key, T[] base, Comparator<? super T> compar) {
        int lo = 0;
        int nel = base.length;
        while (nel > 0) {
            int tryIdx = lo + nel / 2;
            int sign = compar.compare(key, base[tryIdx]);
            if (sign == 0) {
                return tryIdx;
            }
            if (nel == 1) {
                break;
            }
            if (sign < 0) {
                nel /= 2;
            } else {
                lo = tryIdx;
                nel -= nel / 2;
            }
        }
        return -1;
    }

    private static double parseDec(String s, int i) {
        int n = s.length();
        int start = i;
        int end = i;
        boolean anyDigit = false;
        while (i < n && CtypeFuncs.isdigit(s.charAt(i))) {
            i++;
            anyDigit = true;
        }
        if (i < n && s.charAt(i) == '.') {
            i++;
            while (i < n && CtypeFuncs.isdigit(s.charAt(i))) {
                i++;
                anyDigit = true;
            }
        }
        if (!anyDigit) {
            return 0.0;
        }
        end = i;
        if (i < n && (s.charAt(i) == 'e' || s.charAt(i) == 'E')) {
            int j = i + 1;
            if (j < n && (s.charAt(j) == '+' || s.charAt(j) == '-')) {
                j++;
            }
            if (j < n && CtypeFuncs.isdigit(s.charAt(j))) {
                while (j < n && CtypeFuncs.isdigit(s.charAt(j))) {
                    j++;
                }
                end = j;
            }
        }
        return Double.parseDouble(s.substring(start, end));
    }

    private static double parseHex(String s, int i) {
        int n = s.length();
        int intStart = i;
        while (i < n && CtypeFuncs.isxdigit(s.charAt(i))) {
            i++;
        }
        int intEnd = i;
        boolean gotInt = intEnd > intStart;
        int fracStart = intEnd;
        if (i < n && s.charAt(i) == '.') {
            i++;
            fracStart = i;
            while (i < n && CtypeFuncs.isxdigit(s.charAt(i))) {
                i++;
            }
        }
        int fracEnd = i;
        boolean gotFrac = fracEnd > fracStart;
        if (!gotInt && !gotFrac) {
            return 0.0;
        }
        long pExp = 0;
        if (i < n && (s.charAt(i) == 'p' || s.charAt(i) == 'P')) {
            int j = i + 1;
            int sign = 1;
            if (j < n && (s.charAt(j) == '+' || s.charAt(j) == '-')) {
                if (s.charAt(j) == '-') {
                    sign = -1;
                }
                j++;
            }
            if (j < n && CtypeFuncs.isdigit(s.charAt(j))) {
                long e = 0;
                while (j < n && CtypeFuncs.isdigit(s.charAt(j))) {
                    int d = s.charAt(j) - '0';
                    if (e < 1000000) {
                        e = e * 10 + d;
                    }
                    j++;
                }
                pExp = sign * e;
            }
        }
        String mantissa = s.substring(intStart, intEnd) + s.substring(fracStart, fracEnd);
        BigInteger m = new BigInteger(mantissa, 16);
        long scale = pExp - 4L * (fracEnd - fracStart);
        if (scale > 100000) {
            return Double.POSITIVE_INFINITY;
        }
        if (scale < -100000) {
            return 0.0;
        }
        return Math.scalb(m.doubleValue(), (int) scale);
    }

    private static int digitVal(char c) {
        if (c >= '0' && c <= '9') {
            return c - '0';
        }
        if (c >= 'a' && c <= 'z') {
            return c - 'a' + 10;
        }
        if (c >= 'A' && c <= 'Z') {
            return c - 'A' + 10;
        }
        return -1;
    }

    /** {@code scanInt} 的解析结果。 */
    private static final class IntScan {
        /** 数值的绝对值（无符号大小）。 */
        final BigInteger mag;
        /** 是否带负号。 */
        final boolean neg;
        /** 是否解析到了至少一位数字。 */
        final boolean any;

        IntScan(BigInteger mag, boolean neg, boolean any) {
            this.mag = mag;
            this.neg = neg;
            this.any = any;
        }
    }

    /**
     * 整数解析公共前缀（供 {@link #strtol(String, int)} 与
     * {@link #strtoul(String, int)} 共用）：跳过空白与符号、识别进制前缀、累加数字，
     * 与 musl {@code __intscan} 的扫描部分对齐。
     */
    private static IntScan scanInt(String s, int base) {
        int i = 0;
        int n = s.length();
        while (i < n && CtypeFuncs.isspace(s.charAt(i))) {
            i++;
        }
        boolean neg = false;
        if (i < n) {
            char c = s.charAt(i);
            if (c == '-') {
                neg = true;
                i++;
            } else if (c == '+') {
                i++;
            }
        }
        if (base < 0 || base == 1 || base > 36) {
            return new IntScan(BigInteger.ZERO, false, false);
        }
        int b = base;
        if (b == 0) {
            if (i < n && s.charAt(i) == '0') {
                if (i + 1 < n && (s.charAt(i + 1) == 'x' || s.charAt(i + 1) == 'X')) {
                    b = 16;
                    i += 2;
                    if (i >= n || digitVal(s.charAt(i)) >= 16) {
                        return new IntScan(BigInteger.ZERO, false, false);
                    }
                } else {
                    b = 8;
                    i += 1;
                }
            } else {
                b = 10;
            }
        } else if (b == 16) {
            if (i + 1 < n && s.charAt(i) == '0'
                    && (s.charAt(i + 1) == 'x' || s.charAt(i + 1) == 'X')) {
                i += 2;
                if (i >= n || digitVal(s.charAt(i)) >= 16) {
                    return new IntScan(BigInteger.ZERO, false, false);
                }
            }
        }
        BigInteger mag = BigInteger.ZERO;
        BigInteger bb = BigInteger.valueOf(b);
        while (i < n) {
            int d = digitVal(s.charAt(i));
            if (d < 0 || d >= b) {
                break;
            }
            mag = mag.multiply(bb).add(BigInteger.valueOf(d));
            i++;
        }
        return new IntScan(mag, neg, true);
    }

    private static boolean matchesIgnoreCase(String s, int off, String word) {
        if (off + word.length() > s.length()) {
            return false;
        }
        for (int k = 0; k < word.length(); k++) {
            if (Character.toLowerCase(s.charAt(off + k)) != Character.toLowerCase(word.charAt(k))) {
                return false;
            }
        }
        return true;
    }
}
