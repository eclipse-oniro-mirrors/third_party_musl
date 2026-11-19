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

package ohos.musl.ctype;

/**
 * Java 版本的 ctype.h 纯函数接口。
 *
 * <p>仅转换 musl 默认（非 ICU locale）路径下的字符分类与大小写转换函数。这些函数为
 * 纯函数：给定 {@code int}，返回确定结果，不依赖任何全局 locale 状态。</p>
 *
 * <p>返回值映射：C 中分类函数返回 0 / 非 0 的 {@code int}，Java 侧映射为
 * {@code boolean}；{@code tolower} / {@code toupper} 仍返回 {@code int}（字符码值）。</p>
 */
public final class CtypeFuncs {

    private CtypeFuncs() {
        // 工具类，禁止实例化。
    }

    // ------------------------------------------------------------------
    // 数字 / 字母
    // ------------------------------------------------------------------

    /**
     * {@code int isdigit(int c)}：等价于 {@code (unsigned)c - '0' < 10}，判断是否为
     * {@code '0'..'9'}。
     *
     * @param c 待判断字符码值
     * @return 数字字符返回 {@code true}
     */
    public static boolean isdigit(int c) {
        return c >= '0' && c <= '9';
    }

    /**
     * {@code int isxdigit(int c)}：判断是否为十六进制数字 {@code 0-9 a-f A-F}。
     *
     * <p>musl 实现 {@code isdigit(c) || ((unsigned)c|32)-'a' < 6}。</p>
     *
     * @param c 待判断字符码值
     * @return 十六进制数字返回 {@code true}
     */
    public static boolean isxdigit(int c) {
        return isdigit(c) || ((c | 0x20) >= 'a' && (c | 0x20) <= 'f');
    }

    /**
     * {@code int isalpha(int c)}：判断是否为字母 {@code 'A'..'Z'} 或 {@code 'a'..'z'}。
     *
     * <p>musl 实现 {@code ((unsigned)c|32)-'a' < 26}。</p>
     *
     * @param c 待判断字符码值
     * @return 字母返回 {@code true}
     */
    public static boolean isalpha(int c) {
        return ((c | 0x20) >= 'a') && ((c | 0x20) <= 'z');
    }

    /**
     * {@code int isalnum(int c)}：判断是否为字母或数字（{@code isalpha || isdigit}）。
     *
     * @param c 待判断字符码值
     * @return 字母或数字返回 {@code true}
     */
    public static boolean isalnum(int c) {
        return isalpha(c) || isdigit(c);
    }

    /**
     * {@code int isupper(int c)}：等价于 {@code (unsigned)c - 'A' < 26}，判断是否为
     * {@code 'A'..'Z'}。
     *
     * @param c 待判断字符码值
     * @return 大写字母返回 {@code true}
     */
    public static boolean isupper(int c) {
        return c >= 'A' && c <= 'Z';
    }

    /**
     * {@code int islower(int c)}：等价于 {@code (unsigned)c - 'a' < 26}，判断是否为
     * {@code 'a'..'z'}。
     *
     * @param c 待判断字符码值
     * @return 小写字母返回 {@code true}
     */
    public static boolean islower(int c) {
        return c >= 'a' && c <= 'z';
    }

    // ------------------------------------------------------------------
    // 空白字符
    // ------------------------------------------------------------------

    /**
     * {@code int isspace(int c)}：判断是否为空白字符（空格、{@code \t \n \v \f \r}）。
     *
     * <p>musl 实现 {@code c == ' ' || (unsigned)c - '\t' < 5}。</p>
     *
     * @param c 待判断字符码值
     * @return 空白字符返回 {@code true}
     */
    public static boolean isspace(int c) {
        return c == ' ' || (c >= '\t' && c <= '\r');
    }

    /**
     * {@code int isblank(int c)}：判断是否为空格或水平制表符 {@code \t}。
     *
     * @param c 待判断字符码值
     * @return 空格或制表符返回 {@code true}
     */
    public static boolean isblank(int c) {
        return c == ' ' || c == '\t';
    }

    // ------------------------------------------------------------------
    // 大小写转换
    // ------------------------------------------------------------------

    /**
     * {@code int tolower(int c)}：若为大写字母 {@code 'A'..'Z'} 则返回 {@code c | 0x20}
     * （小写形式），否则原样返回。
     *
     * @param c 待转换字符码值
     * @return 小写后的字符码值
     */
    public static int tolower(int c) {
        if (c >= 'A' && c <= 'Z') {
            return c | 0x20;
        }
        return c;
    }

    /**
     * {@code int toupper(int c)}：若为小写字母 {@code 'a'..'z'} 则返回 {@code c & 0x5f}
     * （大写形式），否则原样返回。
     *
     * @param c 待转换字符码值
     * @return 大写后的字符码值
     */
    public static int toupper(int c) {
        if (c >= 'a' && c <= 'z') {
            return c & 0x5f;
        }
        return c;
    }
}