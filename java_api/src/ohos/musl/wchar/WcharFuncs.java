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
}
