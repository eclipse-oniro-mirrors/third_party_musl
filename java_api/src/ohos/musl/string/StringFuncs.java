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

package ohos.musl.string;

import ohos.musl.ctype.CtypeFuncs;

/**
 * Java 版本的 {@code <string.h>} / {@code <strings.h>} 纯函数接口。
 *
 * <p>对 musl 中独立性高、无副作用、无全局状态的字符串函数进行 Java 等价实现。</p>
 *
 * <ul>
 *   <li>C 字符串以 NUL（'\0'）结尾；Java {@link String} 自带长度。{@code char *}
 *       返回统一映射为 {@code int} 下标，找不到（C 返回 NULL）映射为 {@code -1}。</li>
 *   <li>C 比较基于 {@code unsigned char}（0..255）；Java {@code char} 为 UTF-16 码元，
 *       对 ASCII/字节范围结果一致，Java 额外支持完整 Unicode。</li>
 *   <li>{@code strcasecmp} / {@code strncasecmp} 声明于 {@code <strings.h>}，musl 实现
 *       使用 ASCII 的 {@code tolower} 做大小写折叠（非 ICU 构建）。</li>
 * </ul>
 */
public final class StringFuncs {

    private StringFuncs() {
        // 工具类，禁止实例化。
    }

    // ------------------------------------------------------------------
    // strlen / strnlen
    // ------------------------------------------------------------------

    /**
     * {@code size_t strlen(const char *s)}：返回字符串长度（不含结尾 NUL）。
     *
     * @param s 输入字符串，不能为 {@code null}
     * @return 字符串长度（UTF-16 码元个数，ASCII 下等于字节数）
     */
    public static int strlen(String s) {
        return s.length();
    }

    /**
     * {@code size_t strnlen(const char *s, size_t n)}：返回 {@code min(strlen(s), n)}。
     *
     * <p>musl 实现为 {@code memchr(s, 0, n) ? p-s : n}。</p>
     *
     * @param s 输入字符串
     * @param n 检查上限
     * @return {@code min(长度, n)}，{@code n <= 0} 时返回 0
     */
    public static int strnlen(String s, int n) {
        if (n <= 0) {
            return 0;
        }
        return Math.min(s.length(), n);
    }

    // ------------------------------------------------------------------
    // strcmp / strncmp / strcasecmp / strncasecmp / memcmp
    // ------------------------------------------------------------------

    /**
     * {@code int strcmp(const char *l, const char *r)}：逐字符按字典序比较，
     * 返回首个不同字符的差值（负值表示 l &lt; r，0 相等，正值 l &gt; r）。
     *
     * @param l 左操作数
     * @param r 右操作数
     * @return 首个不同字符的差值
     */
    public static int strcmp(String l, String r) {
        int i = 0;
        int ln = l.length();
        int rn = r.length();
        while (i < ln && i < rn && l.charAt(i) == r.charAt(i)) {
            i++;
        }
        if (i == ln && i == rn) {
            return 0;
        }
        if (i == ln) {
            return -r.charAt(i);   // 0 - r[i]
        }
        if (i == rn) {
            return l.charAt(i);    // l[i] - 0
        }
        return l.charAt(i) - r.charAt(i);
    }

    /**
     * {@code int strncmp(const char *l, const char *r, size_t n)}：同
     * {@link #strcmp(String, String)}，但最多比较前 {@code n} 个字符。
     *
     * @param l 左操作数
     * @param r 右操作数
     * @param n 最多比较的字符数
     * @return 首个不同字符的差值（负数 / 0 / 正数）
     */
    public static int strncmp(String l, String r, int n) {
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
        int cl = (i < ln) ? l.charAt(i) : 0;
        int cr = (i < rn) ? r.charAt(i) : 0;
        return cl - cr;
    }

    /**
     * {@code int strcasecmp(const char *l, const char *r)}（&lt;strings.h&gt;）：忽略
     * ASCII 大小写的字典序比较，返回首个折叠后不同字符的差值。
     *
     * @param l 左操作数
     * @param r 右操作数
     * @return 首个不同（折叠后）字符的差值
     */
    public static int strcasecmp(String l, String r) {
        int i = 0;
        int ln = l.length();
        int rn = r.length();
        while (i < ln && i < rn
                && CtypeFuncs.tolower(l.charAt(i)) == CtypeFuncs.tolower(r.charAt(i))) {
            i++;
        }
        int cl = (i < ln) ? CtypeFuncs.tolower(l.charAt(i)) : 0;
        int cr = (i < rn) ? CtypeFuncs.tolower(r.charAt(i)) : 0;
        return cl - cr;
    }

    /**
     * {@code int strncasecmp(const char *l, const char *r, size_t n)}
     * （&lt;strings.h&gt;）：同 {@link #strcasecmp(String, String)}，但最多比较前
     * {@code n} 个字符。
     *
     * @param l 左操作数
     * @param r 右操作数
     * @param n 最多比较的字符数
     * @return 首个不同（折叠后）字符的差值
     */
    public static int strncasecmp(String l, String r, int n) {
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
        int cl = (i < ln) ? CtypeFuncs.tolower(l.charAt(i)) : 0;
        int cr = (i < rn) ? CtypeFuncs.tolower(r.charAt(i)) : 0;
        return cl - cr;
    }

    /**
     * {@code int memcmp(const void *l, const void *r, size_t n)}：按无符号字节比较两个
     * 字节数组的前 {@code n} 字节，返回首个不同字节的差值，全相等返回 0。
     *
     * <p>Java 中以 {@code byte[]} 对应 C 的 {@code void *}，比较时用 {@code & 0xFF}
     * 复刻 {@code unsigned char} 语义。</p>
     *
     * @param l 左字节数组
     * @param r 右字节数组
     * @param n 比较的字节数（需不超过两数组长度，否则 C 为越界未定义行为）
     * @return 首个不同字节的无符号差值，或 0
     */
    public static int memcmp(byte[] l, byte[] r, int n) {
        for (int i = 0; i < n; i++) {
            int a = l[i] & 0xFF;
            int b = r[i] & 0xFF;
            if (a != b) {
                return a - b;
            }
        }
        return 0;
    }

    // ------------------------------------------------------------------
    // 查找类：strchr / strrchr / strstr / strpbrk
    // ------------------------------------------------------------------

    /**
     * {@code char *strchr(const char *s, int c)}：返回 {@code c} 在 {@code s} 中首次
     * 出现的下标。C 将 {@code c} 截断为 {@code unsigned char}，未找到（NULL）返回 -1。
     *
     * @param s 输入字符串
     * @param c 待查找字符（仅低 8 位有效）
     * @return 首次出现的下标，未找到返回 -1
     */
    public static int strchr(String s, int c) {
        return s.indexOf(c & 0xFF);
    }

    /**
     * {@code char *strrchr(const char *s, int c)}：返回 {@code c} 在 {@code s} 中最后
     * 一次出现的下标。注意 C 实现会连同结尾 NUL 一起查找，故 {@code c == 0} 时返回
     * 字符串长度；未找到返回 -1。
     *
     * @param s 输入字符串
     * @param c 待查找字符（仅低 8 位有效）
     * @return 最后一次出现的下标，未找到返回 -1
     */
    public static int strrchr(String s, int c) {
        int ch = c & 0xFF;
        if (ch == 0) {
            return s.length();
        }
        return s.lastIndexOf(ch);
    }

    /**
     * {@code char *strstr(const char *h, const char *n)}：返回子串 {@code n} 在
     * {@code h} 中首次出现的下标；空串返回 0；未找到返回 -1。
     *
     * @param h 主串
     * @param n 子串
     * @return 首次出现的下标，未找到返回 -1
     */
    public static int strstr(String h, String n) {
        return h.indexOf(n);
    }

    /**
     * {@code char *strpbrk(const char *s, const char *b)}：返回 {@code s} 中第一个
     * 出现在字符集 {@code b} 里的字符下标；未找到（NULL）返回 -1。
     *
     * @param s 输入字符串
     * @param b 字符集
     * @return 首个命中字符的下标，未找到返回 -1
     */
    public static int strpbrk(String s, String b) {
        int i = strcspn(s, b);
        return (i < s.length()) ? i : -1;
    }

    // ------------------------------------------------------------------
    // 跨度类：strspn / strcspn
    // ------------------------------------------------------------------

    /**
     * {@code size_t strspn(const char *s, const char *c)}：返回 {@code s} 开头由
     * 字符集 {@code c} 中的字符构成的最长前缀长度；{@code c} 为空串时返回 0。
     *
     * @param s 输入字符串
     * @param c 字符集
     * @return 命中前缀的长度
     */
    public static int strspn(String s, String c) {
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
     * {@code size_t strcspn(const char *s, const char *c)}：返回 {@code s} 开头由
     * <em>不在</em>字符集 {@code c} 中的字符构成的最长前缀长度。
     *
     * @param s 输入字符串
     * @param c 字符集
     * @return 未命中前缀的长度
     */
    public static int strcspn(String s, String c) {
        int i = 0;
        while (i < s.length() && c.indexOf(s.charAt(i)) < 0) {
            i++;
        }
        return i;
    }

    /**
     * {@code char *strchrnul(const char *s, int c)}：同 {@link #strchr(String, int)}，
     * 但未找到时返回结尾 NUL 位置（即 {@code s.length()}），对应 C 中返回指向 NUL 的指针。
     *
     * @param s 输入字符串
     * @param c 待查找字符（仅低 8 位有效）
     * @return 首次出现的下标，未找到返回 {@code s.length()}
     */
    public static int strchrnul(String s, int c) {
        int i = s.indexOf(c & 0xFF);
        return i >= 0 ? i : s.length();
    }

    // ------------------------------------------------------------------
    // 内存操作：memchr / memcpy / memmove / memset
    // ------------------------------------------------------------------

    /**
     * {@code void *memchr(const void *src, int c, size_t n)}：在字节数组前 {@code n}
     * 字节中查找 {@code (unsigned char)c}，返回首次出现的下标；未找到（NULL）返回 -1。
     *
     * <p>C 的 {@code void *} 映射为 {@code byte[]}，偏移由调用方通过传入子数组表达。</p>
     *
     * @param src 源字节数组
     * @param c 待查找字节（仅低 8 位有效）
     * @param n 查找的字节数（需不超过数组长度）
     * @return 首次出现的下标，未找到返回 -1
     */
    public static int memchr(byte[] src, int c, int n) {
        int target = c & 0xFF;
        for (int i = 0; i < n; i++) {
            if ((src[i] & 0xFF) == target) {
                return i;
            }
        }
        return -1;
    }

    /**
     * {@code void *memcpy(void *dst, const void *src, size_t n)}：将 {@code src} 前
     * {@code n} 字节复制到 {@code dst}（C 中要求区间不重叠，见 {@code restrict}）。
     *
     * <p>C 返回值（目标指针）在 Java 中省略为 {@code void}。</p>
     *
     * @param dst 目标字节数组
     * @param src 源字节数组
     * @param n 复制的字节数
     */
    public static void memcpy(byte[] dst, byte[] src, int n) {
        System.arraycopy(src, 0, dst, 0, n);
    }

    /**
     * {@code void *memmove(void *dst, const void *src, size_t n)}：同
     * {@link #memcpy(byte[], byte[], int)}，但允许区间重叠（Java 的
     * {@code System.arraycopy} 本身即为重叠安全）。
     *
     * @param dst 目标字节数组
     * @param src 源字节数组
     * @param n 复制的字节数
     */
    public static void memmove(byte[] dst, byte[] src, int n) {
        System.arraycopy(src, 0, dst, 0, n);
    }

    /**
     * {@code void *memset(void *dst, int c, size_t n)}：将 {@code dst} 前 {@code n}
     * 字节填充为 {@code (unsigned char)c}。
     *
     * @param dst 目标字节数组
     * @param c 填充值（仅低 8 位有效）
     * @param n 填充的字节数
     */
    public static void memset(byte[] dst, int c, int n) {
        java.util.Arrays.fill(dst, 0, n, (byte) c);
    }
}