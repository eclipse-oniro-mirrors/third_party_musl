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

package ohos.musl.regex;

import java.util.HashSet;
import java.util.Set;

/**
 * Java 版本的 {@code <fnmatch.h>}（POSIX 文件名通配）纯函数接口。
 *
 * <p>对 musl {@code src/regex/fnmatch.c} 的 {@code fnmatch} 做 Java 等价实现。C 版以字节流
 * 处理 UTF-8 多字节序列与 locale 大小写折叠，Java 版以 {@link String}（UTF-16 码元）为
 * 字符单元，大小写折叠改用 {@link Character}（ASCII 下与 musl 的 {@code towupper} /
 * {@code towlower} 结果一致）。C 返回值 {@code 0}（匹配）/ {@code FNM_NOMATCH}（不匹配）
 * 映射为 {@code boolean}。</p>
 */
public final class RegexFuncs {

    /** {@code FNM_PATHNAME}：{@code *} / {@code ?} 不匹配路径分隔符 {@code /}。 */
    public static final int FNM_PATHNAME = 0x1;
    /** {@code FNM_NOESCAPE}：反斜杠 {@code \} 按普通字符处理。 */
    public static final int FNM_NOESCAPE = 0x2;
    /** {@code FNM_PERIOD}：前导句点（每段开头的 {@code .}）必须由 pattern 的显式句点匹配。 */
    public static final int FNM_PERIOD = 0x4;
    /** {@code FNM_LEADING_DIR}：pattern 匹配到字符串的某个 {@code /} 前缀即算匹配。 */
    public static final int FNM_LEADING_DIR = 0x8;
    /** {@code FNM_CASEFOLD}：忽略大小写比较。 */
    public static final int FNM_CASEFOLD = 0x10;

    private RegexFuncs() {
        // 工具类，禁止实例化。
    }

    /**
     * {@code int fnmatch(const char *pattern, const char *string, int flags)}：
     * 按 POSIX 通配规则判断 {@code pattern} 是否匹配 {@code string}。
     *
     * <p>支持 {@code *}（任意字符序列，可空）、{@code ?}（单个字符）、{@code [...]}
     * 字符类（含范围 {@code a-z}、取反 {@code [!...]} / {@code [^...]}、作为首字符的
     * {@code ]} 与 {@code -}）以及 {@code \} 转义。配合 {@link #FNM_PATHNAME} /
     * {@link #FNM_NOESCAPE} / {@link #FNM_PERIOD} / {@link #FNM_LEADING_DIR} /
     * {@link #FNM_CASEFOLD} 标志使用。</p>
     *
     * <p>未实现 musl 亦仅部分支持的 POSIX 字符类（{@code [[:alpha:]]}）与 collating /
     * equivalence 元素，字符类内部按字面解析。</p>
     *
     * @param pattern 通配模式
     * @param str 待匹配字符串
     * @param flags {@code FNM_*} 标志位组合
     * @return 匹配返回 {@code true}（对应 C 返回 0），否则 {@code false}
     */
    public static boolean fnmatch(String pattern, String str, int flags) {
        if ((flags & FNM_PATHNAME) != 0) {
            return matchPath(pattern, str, flags, (flags & FNM_LEADING_DIR) != 0);
        }
        if ((flags & FNM_LEADING_DIR) != 0) {
            for (int i = 0; i < str.length(); i++) {
                if (str.charAt(i) == '/' && match(pattern, str.substring(0, i), flags)) {
                    return true;
                }
            }
        }
        return match(pattern, str, flags);
    }

    /**
     * {@code FNM_PATHNAME} 的逐段匹配：将 pattern 与 str 都按 {@code /} 切分，逐段调用
     * {@link #match(String, String, int)}；段内的 {@code *} / {@code ?} 因此天然不跨越
     * {@code /}。
     */
    private static boolean matchPath(String pattern, String str, int flags, boolean leadingDir) {
        int pn = pattern.length();
        int sn = str.length();
        int pi = 0;
        int si = 0;
        for (;;) {
            int pEnd = findSegmentEnd(pattern, pi);
            int sEnd = si;
            while (sEnd < sn && str.charAt(sEnd) != '/') {
                sEnd++;
            }
            boolean patEnds = pEnd >= pn;
            boolean strEnds = sEnd >= sn;
            if (strEnds != patEnds) {
                if (!strEnds && patEnds && leadingDir) {
                    return true;
                }
                return false;
            }
            if (!match(pattern.substring(pi, pEnd), str.substring(si, sEnd), flags)) {
                return false;
            }
            if (patEnds) {
                return true;
            }
            pi = pEnd + 1;
            si = sEnd + 1;
        }
    }

    /** 找到 pattern 中下一个未转义、不在 {@code [...]} 内的 {@code /}（或结尾）。 */
    private static int findSegmentEnd(String p, int from) {
        int pn = p.length();
        boolean inBracket = false;
        int i = from;
        while (i < pn) {
            char c = p.charAt(i);
            if (c == '\\' && i + 1 < pn) {
                i += 2;
                continue;
            }
            if (c == '[') {
                inBracket = true;
            } else if (c == ']' && inBracket) {
                inBracket = false;
            } else if (c == '/' && !inBracket) {
                return i;
            }
            i++;
        }
        return pn;
    }

    /**
     * 单段（或整串）匹配：先复刻 musl {@code fnmatch_internal} 开头的
     * {@code FNM_PERIOD} 检查，再用带回溯的递归匹配 + 失败状态记忆化。
     */
    private static boolean match(String p, String s, int flags) {
        if ((flags & FNM_PERIOD) != 0
                && !s.isEmpty() && s.charAt(0) == '.'
                && (p.isEmpty() || p.charAt(0) != '.')) {
            return false;
        }
        Set<Long> memo = new HashSet<>();
        return matchRec(p, 0, s, 0, flags, memo);
    }

    private static boolean matchRec(String p, int pi, String s, int si, int flags, Set<Long> memo) {
        long key = ((long) pi << 32) | (si & 0xFFFFFFFFL);
        if (memo.contains(key)) {
            return false;
        }
        boolean r = matchInner(p, pi, s, si, flags, memo);
        if (!r) {
            memo.add(key);
        }
        return r;
    }

    private static boolean matchInner(String p, int pi, String s, int si, int flags, Set<Long> memo) {
        int pn = p.length();
        int sn = s.length();
        while (pi < pn) {
            char pc = p.charAt(pi);
            if (pc == '*') {
                while (pi < pn && p.charAt(pi) == '*') {
                    pi++;
                }
                int k = si;
                while (true) {
                    if (matchRec(p, pi, s, k, flags, memo)) {
                        return true;
                    }
                    if (k >= sn) {
                        return false;
                    }
                    k++;
                }
            }
            if (si >= sn) {
                return false;
            }
            char sc = s.charAt(si);
            if (pc == '?') {
                pi++;
                si++;
                continue;
            }
            if (pc == '[') {
                int[] r = matchBracket(p, pi, sc, flags);
                if (r[0] == 0) {
                    return false;
                }
                pi = r[1];
                si++;
                continue;
            }
            if (pc == '\\' && (flags & FNM_NOESCAPE) == 0 && pi + 1 < pn) {
                pc = p.charAt(pi + 1);
                pi += 2;
            } else {
                pi += 1;
            }
            if (!literalMatch(pc, sc, flags)) {
                return false;
            }
            si++;
        }
        return si == sn;
    }

    /**
     * 解析 {@code p.charAt(pi)} 开头的 {@code [...]} 字符类，判断 {@code sc} 是否命中。
     *
     * @return 二元组 {@code { 命中 ? 1 : 0, 消费后的下一个下标 }}
     */
    private static int[] matchBracket(String p, int pi, char sc, int flags) {
        int pn = p.length();
        int i = pi + 1;
        boolean inv = false;
        if (i < pn && (p.charAt(i) == '^' || p.charAt(i) == '!')) {
            inv = true;
            i++;
        }
        boolean matched = false;
        boolean leading = true;
        boolean closed = false;
        while (i < pn) {
            char c = p.charAt(i);
            if (c == ']' && !leading) {
                closed = true;
                i++;
                break;
            }
            char lo;
            int ni;
            if (c == '\\' && (flags & FNM_NOESCAPE) == 0 && i + 1 < pn) {
                lo = p.charAt(i + 1);
                ni = i + 2;
            } else {
                lo = c;
                ni = i + 1;
            }
            if (ni < pn && p.charAt(ni) == '-' && ni + 1 < pn && p.charAt(ni + 1) != ']') {
                int hj = ni + 1;
                char hi;
                if (p.charAt(hj) == '\\' && (flags & FNM_NOESCAPE) == 0 && hj + 1 < pn) {
                    hi = p.charAt(hj + 1);
                    i = hj + 2;
                } else {
                    hi = p.charAt(hj);
                    i = hj + 1;
                }
                if (inRange(lo, hi, sc, flags)) {
                    matched = true;
                }
            } else {
                if (literalMatch(lo, sc, flags)) {
                    matched = true;
                }
                i = ni;
            }
            leading = false;
        }
        if (!closed) {
            // 未闭合 '[' 按字面 '[' 处理（同 musl）。
            return literalMatch('[', sc, flags) ? new int[] { 1, pi + 1 } : new int[] { 0, pi + 1 };
        }
        return (matched != inv) ? new int[] { 1, i } : new int[] { 0, i };
    }

    private static boolean literalMatch(char patChar, char strChar, int flags) {
        if (patChar == strChar) {
            return true;
        }
        if ((flags & FNM_CASEFOLD) != 0) {
            return casefold(strChar) == patChar;
        }
        return false;
    }

    private static boolean inRange(char lo, char hi, char k, int flags) {
        if (lo > hi) {
            return false;
        }
        if (k >= lo && k <= hi) {
            return true;
        }
        if ((flags & FNM_CASEFOLD) != 0) {
            char f = casefold(k);
            return f >= lo && f <= hi;
        }
        return false;
    }

    /** 复刻 musl 的 {@code casefold}：先大写，若不变则小写。 */
    private static char casefold(char c) {
        char up = Character.toUpperCase(c);
        return up == c ? Character.toLowerCase(c) : up;
    }
}