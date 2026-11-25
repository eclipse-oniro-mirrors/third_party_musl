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

    // ------------------------------------------------------------------
    // GNU 扩展查找：memrchr / rawmemchr
    // ------------------------------------------------------------------

    /**
     * {@code void *memrchr(const void *src, int c, size_t n)}（GNU 扩展）：在字节数组
     * 前 {@code n} 字节中从后向前查找 {@code (unsigned char)c}，返回最后一次出现的下标；
     * 未找到（C 返回 NULL）返回 -1。
     *
     * @param src 源字节数组
     * @param c 待查找字节（仅低 8 位有效）
     * @param n 查找的字节数（需不超过数组长度）
     * @return 最后一次出现的下标，未找到返回 -1
     */
    public static int memrchr(byte[] src, int c, int n) {
        int target = c & 0xFF;
        for (int i = n - 1; i >= 0; i--) {
            if ((src[i] & 0xFF) == target) {
                return i;
            }
        }
        return -1;
    }

    /**
     * {@code void *rawmemchr(const void *src, int c)}（GNU 扩展）：在字节数组中查找
     * {@code (unsigned char)c} 首次出现的下标。C 中要求字符必然存在（未找到为未定义
     * 行为），Java 版在未找到时返回 -1。
     *
     * @param src 源字节数组
     * @param c 待查找字节（仅低 8 位有效）
     * @return 首次出现的下标，未找到返回 -1
     */
    public static int rawmemchr(byte[] src, int c) {
        return memchr(src, c, src.length);
    }

    // ------------------------------------------------------------------
    // memmem（GNU 扩展，Two-Way 算法）
    // ------------------------------------------------------------------

    /**
     * {@code void *memmem(const void *h, size_t hl, const void *n, size_t nl)}
     * （&lt;string.h&gt; GNU 扩展）：在字节数组 {@code h} 前 {@code hl} 字节中查找字节
     * 序列 {@code n} 前 {@code nl} 字节的首次出现，返回下标；未找到（NULL）返回 -1。
     *
     * <p>移植 musl：空 needle 返回 0；needle 长于 haystack 返回 -1；长度 1..4 使用
     * 专用滑动窗口；更长使用 Two-Way 字符串匹配（O(n) 平均/最坏）。</p>
     *
     * @param h haystack 字节数组
     * @param hl haystack 长度（需不超过数组长度）
     * @param n needle 字节数组
     * @param nl needle 长度（需不超过数组长度）
     * @return 首次出现的下标，未找到返回 -1
     */
    public static int memmem(byte[] h, int hl, byte[] n, int nl) {
        if (nl == 0) {
            return 0;
        }
        if (hl < nl) {
            return -1;
        }
        if (nl == 1) {
            return memchr(h, n[0], hl);
        }
        if (nl == 2) {
            return twobyteMemmem(h, hl, n);
        }
        if (nl == 3) {
            return threebyteMemmem(h, hl, n);
        }
        if (nl == 4) {
            return fourbyteMemmem(h, hl, n);
        }
        return twowayMemmem(h, hl, n, nl);
    }

    private static int twobyteMemmem(byte[] h, int hl, byte[] n) {
        int nw = ((n[0] & 0xFF) << 8) | (n[1] & 0xFF);
        int hw = ((h[0] & 0xFF) << 8) | (h[1] & 0xFF);
        if (hw == nw) {
            return 0;
        }
        for (int i = 2; i < hl; i++) {
            hw = ((hw << 8) | (h[i] & 0xFF)) & 0xFFFF;
            if (hw == nw) {
                return i - 1;
            }
        }
        return -1;
    }

    private static int threebyteMemmem(byte[] h, int hl, byte[] n) {
        int nw = ((n[0] & 0xFF) << 16) | ((n[1] & 0xFF) << 8) | (n[2] & 0xFF);
        int hw = ((h[0] & 0xFF) << 16) | ((h[1] & 0xFF) << 8) | (h[2] & 0xFF);
        if (hw == nw) {
            return 0;
        }
        for (int i = 3; i < hl; i++) {
            hw = ((hw << 8) | (h[i] & 0xFF)) & 0xFFFFFF;
            if (hw == nw) {
                return i - 2;
            }
        }
        return -1;
    }

    private static int fourbyteMemmem(byte[] h, int hl, byte[] n) {
        int nw = ((n[0] & 0xFF) << 24) | ((n[1] & 0xFF) << 16) | ((n[2] & 0xFF) << 8)
                | (n[3] & 0xFF);
        int hw = ((h[0] & 0xFF) << 24) | ((h[1] & 0xFF) << 16) | ((h[2] & 0xFF) << 8)
                | (h[3] & 0xFF);
        if (hw == nw) {
            return 0;
        }
        for (int i = 4; i < hl; i++) {
            hw = (hw << 8) | (h[i] & 0xFF);
            if (hw == nw) {
                return i - 3;
            }
        }
        return -1;
    }

    private static int twowayMemmem(byte[] h, int hl, byte[] n, int l) {
        boolean[] inNeedle = new boolean[256];
        int[] shift = new int[256];
        for (int i = 0; i < l; i++) {
            int b = n[i] & 0xFF;
            inNeedle[b] = true;
            shift[b] = i + 1;
        }

        // 计算 maximal suffix（正向比较）
        int ip = -1;
        int jp = 0;
        int k = 1;
        int p = 1;
        while (jp + k < l) {
            int a = n[ip + k] & 0xFF;
            int b = n[jp + k] & 0xFF;
            if (a == b) {
                if (k == p) {
                    jp += p;
                    k = 1;
                } else {
                    k++;
                }
            } else if (a > b) {
                jp += k;
                k = 1;
                p = jp - ip;
            } else {
                ip = jp++;
                k = 1;
                p = 1;
            }
        }
        int ms = ip;
        int p0 = p;

        // 反向比较的 maximal suffix
        ip = -1;
        jp = 0;
        k = 1;
        p = 1;
        while (jp + k < l) {
            int a = n[ip + k] & 0xFF;
            int b = n[jp + k] & 0xFF;
            if (a == b) {
                if (k == p) {
                    jp += p;
                    k = 1;
                } else {
                    k++;
                }
            } else if (a < b) {
                jp += k;
                k = 1;
                p = jp - ip;
            } else {
                ip = jp++;
                k = 1;
                p = 1;
            }
        }
        if (ip + 1 > ms + 1) {
            ms = ip;
        } else {
            p = p0;
        }

        // 周期 needle？
        int mem0;
        if (!equalsRange(n, 0, n, p, ms + 1)) {
            mem0 = 0;
            p = Math.max(ms, l - ms - 1) + 1;
        } else {
            mem0 = l - p;
        }
        int mem = 0;

        int hpos = 0;
        while (true) {
            if (hl - hpos < l) {
                return -1;
            }
            int last = h[hpos + l - 1] & 0xFF;
            if (inNeedle[last]) {
                k = l - shift[last];
                if (k != 0) {
                    if (k < mem) {
                        k = mem;
                    }
                    hpos += k;
                    mem = 0;
                    continue;
                }
            } else {
                hpos += l;
                mem = 0;
                continue;
            }
            // 比较右半
            for (k = Math.max(ms + 1, mem); k < l && (n[k] & 0xFF) == (h[hpos + k] & 0xFF); k++) {
                // 空循环体
            }
            if (k < l) {
                hpos += k - ms;
                mem = 0;
                continue;
            }
            // 比较左半
            for (k = ms + 1; k > mem && (n[k - 1] & 0xFF) == (h[hpos + k - 1] & 0xFF); k--) {
                // 空循环体
            }
            if (k <= mem) {
                return hpos;
            }
            hpos += p;
            mem = mem0;
        }
    }

    private static boolean equalsRange(byte[] a, int aOff, byte[] b, int bOff, int len) {
        for (int i = 0; i < len; i++) {
            if (a[aOff + i] != b[bOff + i]) {
                return false;
            }
        }
        return true;
    }

    // ------------------------------------------------------------------
    // 其他 GNU 扩展：strcasestr / strverscmp / stpcpy / stpncpy
    // ------------------------------------------------------------------

    /**
     * {@code char *strcasestr(const char *h, const char *n)}（GNU 扩展）：忽略 ASCII
     * 大小写地在 {@code h} 中查找子串 {@code n} 的首次出现，返回下标；空 {@code n}
     * 返回 0；未找到（NULL）返回 -1。移植 musl：在每个位置调用
     * {@code strncasecmp(h+i, n, len(n))}。
     *
     * @param h 主串
     * @param n 子串
     * @return 首次出现下标，未找到返回 -1
     */
    public static int strcasestr(String h, String n) {
        int l = n.length();
        for (int i = 0; i + l <= h.length(); i++) {
            if (strncasecmp(h.substring(i, i + l), n, l) == 0) {
                return i;
            }
        }
        return -1;
    }

    /**
     * {@code int strverscmp(const char *l, const char *r)}（GNU 扩展）：自然顺序版本
     * 比较（数字段按数值而非字典序）。移植 musl {@code strverscmp}：先找最长公共前缀
     * 并跟踪其中最后一串数字是否全零；若两串在非零数字串处首次不同，则数字串更长者
     * 更大；若公共数字段全零，则数字顺序小于非数字。
     *
     * @param l0 左操作数
     * @param r0 右操作数
     * @return 负 / 0 / 正
     */
    public static int strverscmp(String l0, String r0) {
        int i = 0;
        int dp = 0;
        boolean z = true;
        while (true) {
            int a = i < l0.length() ? l0.charAt(i) : 0;
            int b = i < r0.length() ? r0.charAt(i) : 0;
            if (a != b) {
                break;
            }
            if (a == 0) {
                return 0;
            }
            if (!CtypeFuncs.isdigit(a)) {
                dp = i + 1;
                z = true;
            } else if (a != '0') {
                z = false;
            }
            i++;
        }
        int lc = i < l0.length() ? l0.charAt(i) : 0;
        int rc = i < r0.length() ? r0.charAt(i) : 0;
        int ldc = dp < l0.length() ? l0.charAt(dp) : 0;
        int rdc = dp < r0.length() ? r0.charAt(dp) : 0;
        if (ldc >= '1' && ldc <= '9' && rdc >= '1' && rdc <= '9') {
            int j = i;
            while (j < l0.length() && CtypeFuncs.isdigit(l0.charAt(j))) {
                if (j >= r0.length() || !CtypeFuncs.isdigit(r0.charAt(j))) {
                    return 1;
                }
                j++;
            }
            if (j < r0.length() && CtypeFuncs.isdigit(r0.charAt(j))) {
                return -1;
            }
        } else if (z && dp < i && (CtypeFuncs.isdigit(lc) || CtypeFuncs.isdigit(rc))) {
            return (lc - '0') - (rc - '0');
        }
        return lc - rc;
    }

    /**
     * {@code char *stpcpy(char *dst, const char *src)}：将 {@code src} 复制到字符数组
     * {@code dst}（C 中连同结尾 NUL），返回 C 中指向结尾 NUL 的指针；Java 版返回已复制
     * 长度 {@code src.length()}。调用方需保证 {@code dst} 足够长。
     *
     * @param dst 目标字符数组
     * @param src 源字符串
     * @return 复制的字符数（C 的结尾指针下标）
     */
    public static int stpcpy(char[] dst, String src) {
        int n = src.length();
        for (int i = 0; i < n; i++) {
            dst[i] = src.charAt(i);
        }
        return n;
    }

    /**
     * {@code char *stpncpy(char *dst, const char *src, size_t n)}：至多复制 {@code n}
     * 个字符；{@code src} 不足 {@code n} 时以 {@code '\0'}（char 0）填充剩余位，与 C
     * 一致。返回 C 中指向首个写入的 NUL 的指针；Java 版返回
     * {@code min(src.length(), n)}。
     *
     * @param dst 目标字符数组
     * @param src 源字符串
     * @param n 最多复制的字符数（需不超过 {@code dst} 长度）
     * @return 已写入的字符数（{@code min(src.length(), n)}）
     */
    public static int stpncpy(char[] dst, String src, int n) {
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
     * {@code void *memccpy(void *dest, const void *src, int c, size_t n)}：从
     * {@code src} 复制到 {@code dest}，直到复制满 {@code n} 字节或复制到
     * {@code (unsigned char)c}（含该字节）为止。C 返回目标中 {@code c} 之后的字节指针
     * （即下标 {@code i + 1}）；未找到（NULL）返回 -1。移植 musl {@code memccpy}。
     *
     * @param dst 目标字节数组
     * @param src 源字节数组
     * @param c 终止字节（仅低 8 位有效）
     * @param n 最多复制的字节数（需不超过数组长度）
     * @return 终止字节之后的下标，未找到返回 -1
     */
    public static int memccpy(byte[] dst, byte[] src, int c, int n) {
        int target = c & 0xFF;
        for (int i = 0; i < n; i++) {
            dst[i] = src[i];
            if ((src[i] & 0xFF) == target) {
                return i + 1;
            }
        }
        return -1;
    }

    // ------------------------------------------------------------------
    // 字符串复制 / 拼接（char[] 缓冲）：strcpy / strncpy / strcat / strncat
    // ------------------------------------------------------------------

    /**
     * {@code char *strcpy(char *dst, const char *src)}：将 {@code src} 复制到
     * {@code dst}（C 中连同结尾 NUL），返回 C 中指向 {@code dst} 的指针；Java 版返回
     * 已复制长度 {@code src.length()}。调用方需保证 {@code dst} 足够长。
     *
     * @param dst 目标字符数组
     * @param src 源字符串
     * @return 复制的字符数（C 的起始指针映射为结尾下标）
     */
    public static int strcpy(char[] dst, String src) {
        int n = src.length();
        for (int i = 0; i < n; i++) {
            dst[i] = src.charAt(i);
        }
        dst[n] = 0;
        return n;
    }

    /**
     * {@code char *strncpy(char *dst, const char *src, size_t n)}：至多复制 {@code n}
     * 个字符；{@code src} 不足 {@code n} 时以 0 填充，与 C 一致。Java 版返回
     * {@code min(src.length(), n)}。
     *
     * @param dst 目标字符数组
     * @param src 源字符串
     * @param n 最多复制的字符数（需不超过 {@code dst} 长度）
     * @return 已写入的字符数
     */
    public static int strncpy(char[] dst, String src, int n) {
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
     * {@code char *strcat(char *dst, const char *src)}：将 {@code src} 追加到
     * {@code dst} 中首个 0 字符之后（C 中连同结尾 NUL），返回 C 中指向 {@code dst} 的
     * 指针；Java 版返回追加后的总长度。
     *
     * @param dst 目标字符数组（含 0 结尾的内容）
     * @param src 追加的字符串
     * @return 追加后的总字符数
     */
    public static int strcat(char[] dst, String src) {
        int k = firstZero(dst);
        for (int i = 0; i < src.length(); i++) {
            dst[k + i] = src.charAt(i);
        }
        dst[k + src.length()] = 0;
        return k + src.length();
    }

    /**
     * {@code char *strncat(char *dst, const char *src, size_t n)}：将 {@code src} 至多
     * {@code n} 个字符追加到 {@code dst}，并始终写入结尾 0 字符。Java 版返回追加后的
     * 总长度。
     *
     * @param dst 目标字符数组（含 0 结尾的内容）
     * @param src 追加的字符串
     * @param n 最多追加的字符数
     * @return 追加后的总字符数
     */
    public static int strncat(char[] dst, String src, int n) {
        int k = firstZero(dst);
        int copy = Math.min(src.length(), n);
        for (int i = 0; i < copy; i++) {
            dst[k + i] = src.charAt(i);
        }
        dst[k + copy] = 0;
        return k + copy;
    }

    private static int firstZero(char[] a) {
        for (int i = 0; i < a.length; i++) {
            if (a[i] == 0) {
                return i;
            }
        }
        return a.length;
    }
}