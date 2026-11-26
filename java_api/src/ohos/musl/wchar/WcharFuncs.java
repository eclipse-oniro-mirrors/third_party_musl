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

package ohos.musl.wchar;

import ohos.musl.ctype.CtypeFuncs;
import ohos.musl.stdlib.StdlibFuncs;

/**
 * Java 版本的 {@code <wchar.h>} 纯函数接口（对齐 musl {@code src/wchar/}）。
 *
 * <p>C 的 {@code wchar_t} 在 Linux 上为 32 位；Java {@code char} 为 16 位 UTF-16
 * 码元，{@link String} 为码元序列。对 ASCII 及基本多文种平面，两者结果一致；增补平面
 * 码点在 Java 中表现为代理对（两个码元），语义与 32 位 {@code wchar_t} 不同。与其它
 * 类型保持一致：指针返回值统一映射为 {@code int} 下标，未找到（NULL）映射为 -1。</p>
 */
public final class WcharFuncs {

    private WcharFuncs() {
        // 工具类，禁止实例化。
    }

    /**
     * {@code size_t wcslen(const wchar_t *s)}：宽字符串长度（不含结尾 NUL）。
     *
     * @param s 输入宽字符串
     * @return 长度（UTF-16 码元数）
     */
    public static int wcslen(String s) {
        return s.length();
    }

    /**
     * {@code size_t wcsnlen(const wchar_t *s, size_t n)}：返回 {@code min(wcslen(s), n)}。
     *
     * @param s 输入宽字符串
     * @param n 检查上限
     * @return {@code min(长度, n)}，{@code n <= 0} 时返回 0
     */
    public static int wcsnlen(String s, int n) {
        if (n <= 0) {
            return 0;
        }
        return Math.min(s.length(), n);
    }

    /**
     * {@code int wcscmp(const wchar_t *l, const wchar_t *r)}：逐码元比较，返回首个不同
     * 码元的差值（负 / 0 / 正），规则同 {@code strcmp}。
     *
     * @param l 左操作数
     * @param r 右操作数
     * @return 首个不同码元的差值
     */
    public static int wcscmp(String l, String r) {
        int i = 0;
        int ln = l.length();
        int rn = r.length();
        while (i < ln && i < rn && l.charAt(i) == r.charAt(i)) {
            i++;
        }
        int cl = i < ln ? l.charAt(i) : 0;
        int cr = i < rn ? r.charAt(i) : 0;
        return cl - cr;
    }

    /**
     * {@code int wcsncmp(const wchar_t *l, const wchar_t *r, size_t n)}：同
     * {@link #wcscmp(String, String)}，但最多比较前 {@code n} 个码元。
     *
     * @param l 左操作数
     * @param r 右操作数
     * @param n 最多比较的码元数
     * @return 首个不同码元的差值（负 / 0 / 正）
     */
    public static int wcsncmp(String l, String r, int n) {
        if (n <= 0) {
            return 0;
        }
        int i = 0;
        int ln = l.length();
        int rn = r.length();
        while (i < n && i < ln && i < rn && l.charAt(i) == r.charAt(i)) {
            i++;
        }
        if (i == n) {
            return 0;
        }
        int cl = i < ln ? l.charAt(i) : 0;
        int cr = i < rn ? r.charAt(i) : 0;
        return cl - cr;
    }

    /**
     * {@code int wmemcmp(const wchar_t *l, const wchar_t *r, size_t n)}：按码元比较两个
     * 字符数组的前 {@code n} 个元素。
     *
     * @param l 左字符数组
     * @param r 右字符数组
     * @param n 比较的元素个数（需不超过两数组长度）
     * @return 首个不同码元的差值，或 0
     */
    public static int wmemcmp(char[] l, char[] r, int n) {
        for (int i = 0; i < n; i++) {
            if (l[i] != r[i]) {
                return l[i] - r[i];
            }
        }
        return 0;
    }

    /**
     * {@code wchar_t *wcschr(const wchar_t *s, wchar_t c)}：返回 {@code c} 首次出现的
     * 下标。C 会连同结尾 NUL 一起查找，故 {@code c == 0} 返回 {@code s.length()}；
     * 未找到（NULL）返回 -1。
     *
     * @param s 输入宽字符串
     * @param c 待查找码元
     * @return 首次出现下标，未找到返回 -1
     */
    public static int wcschr(String s, int c) {
        if (c == 0) {
            return s.length();
        }
        return s.indexOf(c);
    }

    /**
     * {@code wchar_t *wcsrchr(const wchar_t *s, wchar_t c)}：返回 {@code c} 最后一次
     * 出现的下标；{@code c == 0} 返回 {@code s.length()}；未找到返回 -1。
     *
     * @param s 输入宽字符串
     * @param c 待查找码元
     * @return 最后一次出现下标，未找到返回 -1
     */
    public static int wcsrchr(String s, int c) {
        if (c == 0) {
            return s.length();
        }
        return s.lastIndexOf(c);
    }

    /**
     * {@code wchar_t *wcschrnul(const wchar_t *s, wchar_t c)}（GNU 扩展）：同
     * {@link #wcschr(String, int)}，但未找到时返回结尾 NUL 位置（即
     * {@code s.length()}），对应 C 中返回指向 NUL 的指针。
     *
     * @param s 输入宽字符串
     * @param c 待查找码元
     * @return 首次出现下标，未找到返回 {@code s.length()}
     */
    public static int wcschrnul(String s, int c) {
        if (c == 0) {
            return s.length();
        }
        int i = s.indexOf(c);
        return i >= 0 ? i : s.length();
    }

    /**
     * {@code int wcscasecmp(const wchar_t *l, const wchar_t *r)}（GNU 扩展）：忽略 ASCII
     * 大小写的宽字符串比较，返回首个折叠后不同码元的差值（负 / 0 / 正）。折叠用
     * {@link CtypeFuncs#tolower(int)}（同 {@code StringFuncs.strcasecmp} 的 ASCII 约定）。
     *
     * @param l 左操作数
     * @param r 右操作数
     * @return 首个不同（折叠后）码元的差值
     */
    public static int wcscasecmp(String l, String r) {
        int i = 0;
        int ln = l.length();
        int rn = r.length();
        while (i < ln && i < rn
                && CtypeFuncs.tolower(l.charAt(i)) == CtypeFuncs.tolower(r.charAt(i))) {
            i++;
        }
        int cl = i < ln ? CtypeFuncs.tolower(l.charAt(i)) : 0;
        int cr = i < rn ? CtypeFuncs.tolower(r.charAt(i)) : 0;
        return cl - cr;
    }

    /**
     * {@code int wcsncasecmp(const wchar_t *l, const wchar_t *r, size_t n)}（GNU 扩展）：
     * 同 {@link #wcscasecmp(String, String)}，但最多比较前 {@code n} 个码元。
     *
     * @param l 左操作数
     * @param r 右操作数
     * @param n 最多比较的码元数
     * @return 首个不同（折叠后）码元的差值
     */
    public static int wcsncasecmp(String l, String r, int n) {
        if (n <= 0) {
            return 0;
        }
        int i = 0;
        int ln = l.length();
        int rn = r.length();
        while (i < n && i < ln && i < rn
                && CtypeFuncs.tolower(l.charAt(i)) == CtypeFuncs.tolower(r.charAt(i))) {
            i++;
        }
        if (i == n) {
            return 0;
        }
        int cl = i < ln ? CtypeFuncs.tolower(l.charAt(i)) : 0;
        int cr = i < rn ? CtypeFuncs.tolower(r.charAt(i)) : 0;
        return cl - cr;
    }

    /**
     * {@code wchar_t *wcspbrk(const wchar_t *s, const wchar_t *b)}：返回 {@code s} 中
     * 首个出现在字符集 {@code b} 里的码元下标；未找到返回 -1。
     *
     * @param s 输入宽字符串
     * @param b 字符集
     * @return 首个命中码元下标，未找到返回 -1
     */
    public static int wcspbrk(String s, String b) {
        int i = wcscspn(s, b);
        return i < s.length() ? i : -1;
    }

    /**
     * {@code size_t wcsspn(const wchar_t *s, const wchar_t *c)}：{@code s} 开头由
     * 字符集 {@code c} 中码元构成的最长前缀长度；{@code c} 为空串时返回 0。
     *
     * @param s 输入宽字符串
     * @param c 字符集
     * @return 命中前缀的长度
     */
    public static int wcsspn(String s, String c) {
        if (c.isEmpty()) {
            return 0;
        }
        int i = 0;
        while (i < s.length() && c.indexOf(s.charAt(i)) >= 0) {
            i++;
        }
        return i;
    }

    /**
     * {@code size_t wcscspn(const wchar_t *s, const wchar_t *c)}：{@code s} 开头由
     * <em>不在</em>字符集 {@code c} 中码元构成的最长前缀长度。
     *
     * @param s 输入宽字符串
     * @param c 字符集
     * @return 未命中前缀的长度
     */
    public static int wcscspn(String s, String c) {
        int i = 0;
        while (i < s.length() && c.indexOf(s.charAt(i)) < 0) {
            i++;
        }
        return i;
    }

    /**
     * {@code wchar_t *wcsstr(const wchar_t *h, const wchar_t *n)}：返回子串 {@code n}
     * 在 {@code h} 中首次出现的下标；空串返回 0；未找到返回 -1。
     *
     * @param h 主串
     * @param n 子串
     * @return 首次出现下标，未找到返回 -1
     */
    public static int wcsstr(String h, String n) {
        return h.indexOf(n);
    }

    /**
     * {@code wchar_t *wmemchr(const wchar_t *src, wchar_t c, size_t n)}：在字符数组前
     * {@code n} 个元素中查找 {@code c}，返回下标；未找到返回 -1。
     *
     * @param src 源字符数组
     * @param c 待查找码元
     * @param n 查找的元素个数（需不超过数组长度）
     * @return 首次出现下标，未找到返回 -1
     */
    public static int wmemchr(char[] src, int c, int n) {
        for (int i = 0; i < n; i++) {
            if (src[i] == c) {
                return i;
            }
        }
        return -1;
    }

    /**
     * {@code wchar_t *wmemcpy(wchar_t *dst, const wchar_t *src, size_t n)}：复制前
     * {@code n} 个码元（C 中要求区间不重叠）；返回值（目标指针）在 Java 中省略为
     * {@code void}。
     *
     * @param dst 目标字符数组
     * @param src 源字符数组
     * @param n 复制的元素个数
     */
    public static void wmemcpy(char[] dst, char[] src, int n) {
        System.arraycopy(src, 0, dst, 0, n);
    }

    /**
     * {@code wchar_t *wmemmove(wchar_t *dst, const wchar_t *src, size_t n)}：同
     * {@link #wmemcpy(char[], char[], int)}，但允许区间重叠（{@code System.arraycopy}
     * 本身即重叠安全）。
     *
     * @param dst 目标字符数组
     * @param src 源字符数组
     * @param n 复制的元素个数
     */
    public static void wmemmove(char[] dst, char[] src, int n) {
        System.arraycopy(src, 0, dst, 0, n);
    }

    /**
     * {@code wchar_t *wmemset(wchar_t *dst, wchar_t c, size_t n)}：将 {@code dst} 前
     * {@code n} 个码元填充为 {@code c}。
     *
     * @param dst 目标字符数组
     * @param c 填充码元
     * @param n 填充的元素个数
     */
    public static void wmemset(char[] dst, int c, int n) {
        java.util.Arrays.fill(dst, 0, n, (char) c);
    }

    // ------------------------------------------------------------------
    // 宽字符串复制 / 拼接（char[] 缓冲）
    // ------------------------------------------------------------------

    /**
     * {@code wchar_t *wcscpy(wchar_t *dst, const wchar_t *src)}：将 {@code src} 复制到
     * {@code dst}（C 中连同结尾 NUL），返回 C 中指向 {@code dst} 的指针；Java 版返回
     * 已复制长度 {@code src.length()}。调用方需保证 {@code dst} 足够长。
     *
     * @param dst 目标字符数组
     * @param src 源宽字符串
     * @return 复制的码元数
     */
    public static int wcscpy(char[] dst, String src) {
        int n = src.length();
        for (int i = 0; i < n; i++) {
            dst[i] = src.charAt(i);
        }
        dst[n] = 0;
        return n;
    }

    /**
     * {@code wchar_t *wcsncpy(wchar_t *dst, const wchar_t *src, size_t n)}：至多复制
     * {@code n} 个码元；{@code src} 不足 {@code n} 时以 0 填充，与 C 一致。Java 版返回
     * {@code min(src.length(), n)}。
     *
     * @param dst 目标字符数组
     * @param src 源宽字符串
     * @param n 最多复制的码元数（需不超过 {@code dst} 长度）
     * @return 已写入的码元数
     */
    public static int wcsncpy(char[] dst, String src, int n) {
        int copy = Math.min(src.length(), n);
        for (int i = 0; i < copy; i++) {
            dst[i] = src.charAt(i);
        }
        for (int i = copy; i < n; i++) {
            dst[i] = 0;
        }
        return copy;
    }

    /**
     * {@code wchar_t *wcscat(wchar_t *dst, const wchar_t *src)}：将 {@code src} 追加到
     * {@code dst} 中首个 0 码元之后（C 中连同结尾 NUL），Java 版返回追加后的总长度。
     *
     * @param dst 目标字符数组（含 0 结尾的内容）
     * @param src 追加的宽字符串
     * @return 追加后的总码元数
     */
    public static int wcscat(char[] dst, String src) {
        int k = indexOfZero(dst);
        for (int i = 0; i < src.length(); i++) {
            dst[k + i] = src.charAt(i);
        }
        dst[k + src.length()] = 0;
        return k + src.length();
    }

    /**
     * {@code wchar_t *wcsncat(wchar_t *dst, const wchar_t *src, size_t n)}：将
     * {@code src} 至多 {@code n} 个码元追加到 {@code dst}，并始终写入结尾 0 码元。
     * Java 版返回追加后的总长度。
     *
     * @param dst 目标字符数组（含 0 结尾的内容）
     * @param src 追加的宽字符串
     * @param n 最多追加的码元数
     * @return 追加后的总码元数
     */
    public static int wcsncat(char[] dst, String src, int n) {
        int k = indexOfZero(dst);
        int copy = Math.min(src.length(), n);
        for (int i = 0; i < copy; i++) {
            dst[k + i] = src.charAt(i);
        }
        dst[k + copy] = 0;
        return k + copy;
    }

    private static int indexOfZero(char[] a) {
        for (int i = 0; i < a.length; i++) {
            if (a[i] == 0) {
                return i;
            }
        }
        return a.length;
    }

    // ------------------------------------------------------------------
    // 宽字符串数值解析 / 字节转换
    // ------------------------------------------------------------------

    /**
     * {@code long wcstol(const wchar_t *s, wchar_t **end, int base)}：解析宽字符串开头的
     * 整数，复用 {@link StdlibFuncs#strtol(String, int)}（省略 end 指针）。
     *
     * @param s 待解析宽字符串
     * @param base 进制（0 表示自动检测，或 2..36）
     * @return 解析得到的 {@code long}
     */
    public static long wcstol(String s, int base) {
        return StdlibFuncs.strtol(s, base);
    }

    /**
     * {@code unsigned long wcstoul(const wchar_t *s, wchar_t **end, int base)}：解析
     * 无符号整数，复用 {@link StdlibFuncs#strtoul(String, int)}（位模式约定一致）。
     *
     * @param s 待解析宽字符串
     * @param base 进制（0 表示自动检测，或 2..36）
     * @return 无符号值的 64 位位模式
     */
    public static long wcstoul(String s, int base) {
        return StdlibFuncs.strtoul(s, base);
    }

    /**
     * {@code long long wcstoll(const wchar_t *s, wchar_t **end, int base)}：解析
     * {@code long long}，复用 {@link StdlibFuncs#strtoll(String, int)}。
     *
     * @param s 待解析宽字符串
     * @param base 进制（0 表示自动检测，或 2..36）
     * @return 解析得到的 {@code long}
     */
    public static long wcstoll(String s, int base) {
        return StdlibFuncs.strtoll(s, base);
    }

    /**
     * {@code unsigned long long wcstoull(const wchar_t *s, wchar_t **end, int base)}：
     * 解析 {@code unsigned long long}，复用 {@link StdlibFuncs#strtoull(String, int)}。
     *
     * @param s 待解析宽字符串
     * @param base 进制（0 表示自动检测，或 2..36）
     * @return 无符号值的 64 位位模式
     */
    public static long wcstoull(String s, int base) {
        return StdlibFuncs.strtoull(s, base);
    }

    /**
     * {@code double wcstod(const wchar_t *s, wchar_t **end)}：解析宽字符串开头的浮点数，
     * 复用 {@link StdlibFuncs#strtod(String)}（省略 end 指针）。
     *
     * @param s 待解析宽字符串
     * @return 解析得到的 {@code double}
     */
    public static double wcstod(String s) {
        return StdlibFuncs.strtod(s);
    }

    /**
     * {@code float wcstof(const wchar_t *s, wchar_t **end)}：解析宽字符串开头的
     * {@code float}，复用 {@link StdlibFuncs#strtof(String)}。
     *
     * @param s 待解析宽字符串
     * @return 解析得到的 {@code float}
     */
    public static float wcstof(String s) {
        return StdlibFuncs.strtof(s);
    }

    /**
     * {@code long double wcstold(const wchar_t *s, wchar_t **end)}：解析宽字符串开头的
     * {@code long double}。本项目中 {@code long double} 以 {@code double} 表达（精度
     * 低于 C 的 80 位扩展精度），复用 {@link StdlibFuncs#strtod(String)}。
     *
     * @param s 待解析宽字符串
     * @return 解析得到的 {@code double}
     */
    public static double wcstold(String s) {
        return StdlibFuncs.strtod(s);
    }

    /**
     * {@code wint_t btowc(int c)}：将单字节 {@code c}（0..255）转换为宽码点；{@code c}
     * 为 EOF（-1）或超出字节范围时返回 -1（{@code WEOF} 映射）。
     *
     * @param c 输入字节（或 EOF）
     * @return 宽码点，无效输入返回 -1
     */
    public static int btowc(int c) {
        if (c < 0 || c > 0xFF) {
            return -1;
        }
        return c;
    }

    /**
     * {@code int wctob(wint_t c)}：将宽码点转换为单字节；超出字节范围（0..255）时返回
     * -1（{@code EOF} 映射）。
     *
     * @param c 输入宽码点
     * @return 单字节值，超出范围返回 -1
     */
    public static int wctob(int c) {
        if (c < 0 || c > 0xFF) {
            return -1;
        }
        return c;
    }
}
