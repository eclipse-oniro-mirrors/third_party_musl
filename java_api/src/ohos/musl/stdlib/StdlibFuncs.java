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
 * <p>仅包含无全局状态、无内存所有权、无 errno 依赖的部分：绝对值与数字字符串解析。
 * {@code atoi} 复用 {@link CtypeFuncs#isspace(int)} / {@link CtypeFuncs#isdigit(int)}。</p>
 */
public final class StdlibFuncs {

    private StdlibFuncs() {
        // 工具类，禁止实例化。
    }

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
        if (base < 0 || base == 1 || base > 36) {
            return 0L;
        }
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
        int b = base;
        if (b == 0) {
            if (i < n && s.charAt(i) == '0') {
                if (i + 1 < n && (s.charAt(i + 1) == 'x' || s.charAt(i + 1) == 'X')) {
                    b = 16;
                    i += 2;
                    if (i >= n || digitVal(s.charAt(i)) >= 16) {
                        return 0L;
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
                    return 0L;
                }
            }
        }
        BigInteger val = BigInteger.ZERO;
        BigInteger bb = BigInteger.valueOf(b);
        while (i < n) {
            int d = digitVal(s.charAt(i));
            if (d < 0 || d >= b) {
                break;
            }
            val = val.multiply(bb).add(BigInteger.valueOf(d));
            i++;
        }
        BigInteger signed = neg ? val.negate() : val;
        if (signed.compareTo(BigInteger.valueOf(Long.MAX_VALUE)) > 0) {
            return Long.MAX_VALUE;
        }
        if (signed.compareTo(BigInteger.valueOf(Long.MIN_VALUE)) < 0) {
            return Long.MIN_VALUE;
        }
        return signed.longValue();
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

    // ------------------------------------------------------------------
    // qsort（smoothsort）
    // ------------------------------------------------------------------

    /**
     * {@code void qsort(void *base, size_t nmemb, size_t size, cmp)}：对数组原地排序。
     *
     * <p>移植 musl 的 smoothsort（Leonardo 堆，最坏 O(n log n)、近似有序时接近 O(n)、
     * 原地且空间 O(1)）。C 的 {@code void *} 通用数组 + 元素大小 + 比较回调映射为
     * Java 泛型数组 {@code T[]} + {@link Comparator}。</p>
     *
     * @param a 待排序数组（原地修改）
     * @param cmp 比较器（返回值语义同 C 的 {@code cmp(a, b)}）
     */
    public static <T> void qsort(T[] a, Comparator<? super T> cmp) {
        int size = a.length;
        if (size <= 1) {
            return;
        }
        int[] lp = new int[96];
        lp[0] = 1;
        lp[1] = 1;
        int li = 2;
        for (; li < lp.length; li++) {
            int next = lp[li - 2] + lp[li - 1] + 1;
            lp[li] = next;
            if (next >= size) {
                break;
            }
        }
        long p0 = 1;
        long p1 = 0;
        int pshift = 1;
        int head = 0;
        int high = size - 1;
        while (head < high) {
            if ((p0 & 3) == 3) {
                sift(a, head, cmp, pshift, lp);
                long[] r = shr(p0, p1, 2);
                p0 = r[0];
                p1 = r[1];
                pshift += 2;
            } else {
                if (lp[pshift - 1] >= high - head) {
                    trinkle(a, head, cmp, p0, p1, pshift, 0, lp);
                } else {
                    sift(a, head, cmp, pshift, lp);
                }
                if (pshift == 1) {
                    long[] r = shl(p0, p1, 1);
                    p0 = r[0];
                    p1 = r[1];
                    pshift = 0;
                } else {
                    long[] r = shl(p0, p1, pshift - 1);
                    p0 = r[0];
                    p1 = r[1];
                    pshift = 1;
                }
            }
            p0 |= 1;
            head++;
        }
        trinkle(a, head, cmp, p0, p1, pshift, 0, lp);
        while (pshift != 1 || p0 != 1 || p1 != 0) {
            if (pshift <= 1) {
                int trail = pntz(p0, p1);
                long[] r = shr(p0, p1, trail);
                p0 = r[0];
                p1 = r[1];
                pshift += trail;
            } else {
                long[] r = shl(p0, p1, 2);
                p0 = r[0];
                p1 = r[1];
                pshift -= 2;
                p0 ^= 7;
                r = shr(p0, p1, 1);
                p0 = r[0];
                p1 = r[1];
                trinkle(a, head - lp[pshift] - 1, cmp, p0, p1, pshift + 1, 1, lp);
                r = shl(p0, p1, 1);
                p0 = r[0];
                p1 = r[1];
                p0 |= 1;
                trinkle(a, head - 1, cmp, p0, p1, pshift, 1, lp);
            }
            head--;
        }
    }

    private static <T> void sift(T[] a, int head, Comparator<? super T> cmp, int pshift, int[] lp) {
        T val = a[head];
        while (pshift > 1) {
            int rt = head - 1;
            int lf = head - 1 - lp[pshift - 2];
            if (cmp.compare(val, a[lf]) >= 0 && cmp.compare(val, a[rt]) >= 0) {
                break;
            }
            if (cmp.compare(a[lf], a[rt]) >= 0) {
                a[head] = a[lf];
                head = lf;
                pshift -= 1;
            } else {
                a[head] = a[rt];
                head = rt;
                pshift -= 2;
            }
        }
        a[head] = val;
    }

    private static <T> void trinkle(T[] a, int head, Comparator<? super T> cmp, long p0, long p1,
            int pshift, int trusty, int[] lp) {
        T val = a[head];
        while (p0 != 1 || p1 != 0) {
            int stepson = head - lp[pshift];
            if (cmp.compare(a[stepson], val) <= 0) {
                break;
            }
            if (trusty == 0 && pshift > 1) {
                int rt = head - 1;
                int lf = head - 1 - lp[pshift - 2];
                if (cmp.compare(a[rt], a[stepson]) >= 0 || cmp.compare(a[lf], a[stepson]) >= 0) {
                    break;
                }
            }
            a[head] = a[stepson];
            head = stepson;
            int trail = pntz(p0, p1);
            long[] r = shr(p0, p1, trail);
            p0 = r[0];
            p1 = r[1];
            pshift += trail;
            trusty = 0;
        }
        a[head] = val;
        if (trusty == 0) {
            sift(a, head, cmp, pshift, lp);
        }
    }

    private static int pntz(long p0, long p1) {
        if (p0 != 1) {
            return Long.numberOfTrailingZeros(p0 - 1);
        }
        if (p1 != 0) {
            return 64 + Long.numberOfTrailingZeros(p1);
        }
        return 0;
    }

    private static long[] shl(long p0, long p1, int n) {
        if (n <= 0) {
            return new long[] { p0, p1 };
        }
        if (n >= 64) {
            n -= 64;
            p1 = p0;
            p0 = 0;
            if (n == 0) {
                return new long[] { p0, p1 };
            }
        }
        p1 = (p1 << n) | (p0 >>> (64 - n));
        p0 = p0 << n;
        return new long[] { p0, p1 };
    }

    private static long[] shr(long p0, long p1, int n) {
        if (n <= 0) {
            return new long[] { p0, p1 };
        }
        if (n >= 64) {
            n -= 64;
            p0 = p1;
            p1 = 0;
            if (n == 0) {
                return new long[] { p0, p1 };
            }
        }
        p0 = (p0 >>> n) | (p1 << (64 - n));
        p1 = p1 >>> n;
        return new long[] { p0, p1 };
    }
}