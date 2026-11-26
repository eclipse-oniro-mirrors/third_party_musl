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

package ohos.musl.wctype;

import ohos.musl.ctype.CtypeFuncs;

/**
 * Java 版本的 {@code <wctype.h>} 纯函数接口（对齐 musl {@code src/ctype/} 的宽字符版本）。
 *
 * <p>C 的 {@code wint_t} 为码点，Java 以 {@code int} 表示；本实现按 C locale（ASCII）
 * 语义映射到 {@link CtypeFuncs}，与 {@code wchar_t}（32 位）/ Java {@code char}（UTF-16）
 * 的差异说明见 README。分类谓词 C 返回 0 / 非 0，此处映射为 {@code boolean}。</p>
 */
public final class WctypeFuncs {

    private WctypeFuncs() {
        // 工具类，禁止实例化。
    }

    /**
     * {@code wint_t towlower(wint_t c)}：将宽字符转为小写（ASCII 折叠），委托
     * {@link CtypeFuncs#tolower(int)}。
     *
     * @param c 输入码点
     * @return 小写码点（非字母原样返回）
     */
    public static int towlower(int c) {
        return CtypeFuncs.tolower(c);
    }

    /**
     * {@code wint_t towupper(wint_t c)}：将宽字符转为大写（ASCII 折叠），委托
     * {@link CtypeFuncs#toupper(int)}。
     *
     * @param c 输入码点
     * @return 大写码点（非字母原样返回）
     */
    public static int towupper(int c) {
        return CtypeFuncs.toupper(c);
    }

    /**
     * {@code int iswdigit(wint_t c)}：是否为十进制数字。
     *
     * @param c 输入码点
     * @return 是数字返回 {@code true}
     */
    public static boolean iswdigit(int c) {
        return CtypeFuncs.isdigit(c);
    }

    /**
     * {@code int iswxdigit(wint_t c)}：是否为十六进制数字。
     *
     * @param c 输入码点
     * @return 是十六进制数字返回 {@code true}
     */
    public static boolean iswxdigit(int c) {
        return CtypeFuncs.isxdigit(c);
    }

    /**
     * {@code int iswalpha(wint_t c)}：是否为字母。
     *
     * @param c 输入码点
     * @return 是字母返回 {@code true}
     */
    public static boolean iswalpha(int c) {
        return CtypeFuncs.isalpha(c);
    }

    /**
     * {@code int iswalnum(wint_t c)}：是否为字母或数字。
     *
     * @param c 输入码点
     * @return 是字母或数字返回 {@code true}
     */
    public static boolean iswalnum(int c) {
        return CtypeFuncs.isalnum(c);
    }

    /**
     * {@code int iswupper(wint_t c)}：是否为大写字母。
     *
     * @param c 输入码点
     * @return 是大写字母返回 {@code true}
     */
    public static boolean iswupper(int c) {
        return CtypeFuncs.isupper(c);
    }

    /**
     * {@code int iswlower(wint_t c)}：是否为小写字母。
     *
     * @param c 输入码点
     * @return 是小写字母返回 {@code true}
     */
    public static boolean iswlower(int c) {
        return CtypeFuncs.islower(c);
    }

    /**
     * {@code int iswspace(wint_t c)}：是否为空白字符。
     *
     * @param c 输入码点
     * @return 是空白返回 {@code true}
     */
    public static boolean iswspace(int c) {
        return CtypeFuncs.isspace(c);
    }

    /**
     * {@code int iswblank(wint_t c)}：是否为空格或制表符。
     *
     * @param c 输入码点
     * @return 是空白（空格/制表）返回 {@code true}
     */
    public static boolean iswblank(int c) {
        return CtypeFuncs.isblank(c);
    }

    /**
     * {@code int iswgraph(wint_t c)}：是否为可打印且非空白的字符。
     *
     * @param c 输入码点
     * @return 是可见字符返回 {@code true}
     */
    public static boolean iswgraph(int c) {
        return CtypeFuncs.isgraph(c);
    }

    /**
     * {@code int iswprint(wint_t c)}：是否为可打印字符（含空格）。
     *
     * @param c 输入码点
     * @return 是可打印字符返回 {@code true}
     */
    public static boolean iswprint(int c) {
        return CtypeFuncs.isprint(c);
    }

    /**
     * {@code int iswpunct(wint_t c)}：是否为标点/符号。
     *
     * @param c 输入码点
     * @return 是标点返回 {@code true}
     */
    public static boolean iswpunct(int c) {
        return CtypeFuncs.ispunct(c);
    }

    /**
     * {@code int iswcntrl(wint_t c)}：是否为控制字符。
     *
     * @param c 输入码点
     * @return 是控制字符返回 {@code true}
     */
    public static boolean iswcntrl(int c) {
        return CtypeFuncs.iscntrl(c);
    }

    /**
     * {@code int iswascii(wint_t c)}：是否为 ASCII 码点（0x00..0x7F），实现为区间判断。
     *
     * @param c 输入码点
     * @return 是 ASCII 返回 {@code true}
     */
    public static boolean iswascii(int c) {
        return c >= 0 && c <= 0x7f;
    }

    // ------------------------------------------------------------------
    // 类别 / 映射描述符：wctype / iswctype / wctrans / towctrans
    // ------------------------------------------------------------------

    /** {@link #wctype(String)} 类别描述符：字母或数字。 */
    public static final int WC_ALNUM = 1;
    /** 类别描述符：字母。 */
    public static final int WC_ALPHA = 2;
    /** 类别描述符：空白（空格/制表）。 */
    public static final int WC_BLANK = 3;
    /** 类别描述符：控制字符。 */
    public static final int WC_CNTRL = 4;
    /** 类别描述符：十进制数字。 */
    public static final int WC_DIGIT = 5;
    /** 类别描述符：可见字符。 */
    public static final int WC_GRAPH = 6;
    /** 类别描述符：小写字母。 */
    public static final int WC_LOWER = 7;
    /** 类别描述符：可打印字符。 */
    public static final int WC_PRINT = 8;
    /** 类别描述符：标点/符号。 */
    public static final int WC_PUNCT = 9;
    /** 类别描述符：空白字符。 */
    public static final int WC_SPACE = 10;
    /** 类别描述符：大写字母。 */
    public static final int WC_UPPER = 11;
    /** 类别描述符：十六进制数字。 */
    public static final int WC_XDIGIT = 12;

    /** {@link #wctrans(String)} 映射描述符：转小写。 */
    public static final int WC_TOLOWER = 1;
    /** 映射描述符：转大写。 */
    public static final int WC_TOUPPER = 2;

    /**
     * {@code wctype_t wctype(const char *name)}：返回类别描述符。此处以类别编码常量
     * （{@link #WC_ALNUM} 等）代替不透明句柄，未知名称返回 0。
     *
     * @param name 类别名（如 {@code "digit"}）
     * @return 类别描述符，未知返回 0
     */
    public static int wctype(String name) {
        switch (name) {
            case "alnum":
                return WC_ALNUM;
            case "alpha":
                return WC_ALPHA;
            case "blank":
                return WC_BLANK;
            case "cntrl":
                return WC_CNTRL;
            case "digit":
                return WC_DIGIT;
            case "graph":
                return WC_GRAPH;
            case "lower":
                return WC_LOWER;
            case "print":
                return WC_PRINT;
            case "punct":
                return WC_PUNCT;
            case "space":
                return WC_SPACE;
            case "upper":
                return WC_UPPER;
            case "xdigit":
                return WC_XDIGIT;
            default:
                return 0;
        }
    }

    /**
     * {@code int iswctype(wint_t c, wctype_t desc)}：按 {@link #wctype(String)} 返回的
     * 类别描述符判断；未知描述符返回 {@code false}。
     *
     * @param c 输入码点
     * @param desc 类别描述符
     * @return 属于该类别返回 {@code true}
     */
    public static boolean iswctype(int c, int desc) {
        switch (desc) {
            case WC_ALNUM:
                return iswalnum(c);
            case WC_ALPHA:
                return iswalpha(c);
            case WC_BLANK:
                return iswblank(c);
            case WC_CNTRL:
                return iswcntrl(c);
            case WC_DIGIT:
                return iswdigit(c);
            case WC_GRAPH:
                return iswgraph(c);
            case WC_LOWER:
                return iswlower(c);
            case WC_PRINT:
                return iswprint(c);
            case WC_PUNCT:
                return iswpunct(c);
            case WC_SPACE:
                return iswspace(c);
            case WC_UPPER:
                return iswupper(c);
            case WC_XDIGIT:
                return iswxdigit(c);
            default:
                return false;
        }
    }

    /**
     * {@code wctrans_t wctrans(const char *name)}：返回映射描述符（以编码常量代替不透明
     * 句柄），支持 {@code "tolower"} / {@code "toupper"}，未知名称返回 0。
     *
     * @param name 映射名
     * @return 映射描述符，未知返回 0
     */
    public static int wctrans(String name) {
        if ("tolower".equals(name)) {
            return WC_TOLOWER;
        }
        if ("toupper".equals(name)) {
            return WC_TOUPPER;
        }
        return 0;
    }

    /**
     * {@code wint_t towctrans(wint_t c, wctrans_t desc)}：按描述符转换大小写；未知描述符
     * 原样返回。
     *
     * @param c 输入码点
     * @param desc 映射描述符
     * @return 转换后的码点
     */
    public static int towctrans(int c, int desc) {
        if (desc == WC_TOLOWER) {
            return towlower(c);
        }
        if (desc == WC_TOUPPER) {
            return towupper(c);
        }
        return c;
    }
}
