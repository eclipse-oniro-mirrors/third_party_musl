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
     * {@code strtod(s, NULL)} 的子集。
     *
     * <p>支持前导空白、正负号、十进制整数/小数、科学计数法，以及
     * {@code inf}/{@code infinity}/{@code nan}（大小写不敏感）；无有效数字时返回 0.0。
     * 不支持 C99 十六进制浮点字面量（{@code 0x1p3}）。</p>
     *
     * @param s 待解析字符串
     * @return 解析得到的 {@code double}
     */
    public static double atof(String s) {
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
        if (i < n && matchesIgnoreCase(s, i, "infinity")) {
            return neg ? Double.NEGATIVE_INFINITY : Double.POSITIVE_INFINITY;
        }
        if (i < n && matchesIgnoreCase(s, i, "inf")) {
            return neg ? Double.NEGATIVE_INFINITY : Double.POSITIVE_INFINITY;
        }
        if (i < n && matchesIgnoreCase(s, i, "nan")) {
            return Double.NaN;
        }
        int start = i;
        boolean anyDigits = false;
        while (i < n && CtypeFuncs.isdigit(s.charAt(i))) {
            i++;
            anyDigits = true;
        }
        if (i < n && s.charAt(i) == '.') {
            i++;
            while (i < n && CtypeFuncs.isdigit(s.charAt(i))) {
                i++;
                anyDigits = true;
            }
        }
        if (!anyDigits) {
            return 0.0;
        }
        int end = i;
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
        double v = Double.parseDouble(s.substring(start, end));
        return neg ? -v : v;
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