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

package ohos.musl;

import ohos.musl.ctype.CtypeFuncs;
import ohos.musl.inttypes.InttypesFuncs;
import ohos.musl.math.MathFuncs;
import ohos.musl.misc.MiscFuncs;
import ohos.musl.stdlib.StdlibFuncs;
import ohos.musl.string.StringFuncs;
import ohos.musl.wchar.WcharFuncs;
import ohos.musl.wctype.WctypeFuncs;

/**
 * 演示 30 个 C 接口的 Java 版本（节选）。独立于现有编译流程：
 *
 * <pre>
 *   javac -d out src/ohos/musl/*.java
 *   java -cp out ohos.musl.MuslDemo
 * </pre>
 */
public final class MuslDemo {

    private MuslDemo() {
    }

    public static void main(String[] args) {
        // string.h / strings.h
        System.out.println("strlen(\"hello\")          = " + StringFuncs.strlen("hello"));
        System.out.println("strnlen(\"hello\",3)       = " + StringFuncs.strnlen("hello", 3));
        System.out.println("strcmp(\"abc\",\"abd\")     = " + StringFuncs.strcmp("abc", "abd"));
        System.out.println("strncmp(\"abc\",\"abx\",2)  = " + StringFuncs.strncmp("abc", "abx", 2));
        System.out.println("strcasecmp(\"Ab\",\"aB\")   = " + StringFuncs.strcasecmp("Ab", "aB"));
        System.out.println("strchr(\"hello\",'l')      = " + StringFuncs.strchr("hello", 'l'));
        System.out.println("strrchr(\"hello\",'l')     = " + StringFuncs.strrchr("hello", 'l'));
        System.out.println("strstr(\"abcabc\",\"ca\")   = " + StringFuncs.strstr("abcabc", "ca"));
        System.out.println("strspn(\"hello\",\"hel\")   = " + StringFuncs.strspn("hello", "hel"));
        System.out.println("strcspn(\"hello\",\"lo\")   = " + StringFuncs.strcspn("hello", "lo"));
        System.out.println("strpbrk(\"hello\",\"aeiou\")= " + StringFuncs.strpbrk("hello", "aeiou"));
        System.out.println("memcmp([1,2,3],[1,2,4],3) = "
                + StringFuncs.memcmp(new byte[] {1, 2, 3}, new byte[] {1, 2, 4}, 3));

        // stdlib.h
        System.out.println("abs(-42)                  = " + StdlibFuncs.abs(-42));
        System.out.println("labs(-42L)                = " + StdlibFuncs.labs(-42L));
        System.out.println("atoi(\"  -42abc\")         = " + StdlibFuncs.atoi("  -42abc"));
        System.out.println("div(5,2)                  = quot " + StdlibFuncs.div(5, 2).quot
                + ", rem " + StdlibFuncs.div(5, 2).rem);
        System.out.println("strtoul(\"0x1A\",0)        = " + StdlibFuncs.strtoul("0x1A", 0));
        System.out.println("bsearch(\"banana\",[...])  = " + StdlibFuncs.bsearch("banana",
                new String[] {"apple", "banana", "cherry"}, java.util.Comparator.naturalOrder()));

        // math.h
        System.out.println("sqrt(2.0)                = " + MathFuncs.sqrt(2.0));
        System.out.println("pow(2.0,10.0)            = " + MathFuncs.pow(2.0, 10.0));
        System.out.println("floor(-3.7)              = " + MathFuncs.floor(-3.7));
        System.out.println("fabs(-3.5)               = " + MathFuncs.fabs(-3.5));
        System.out.println("lround(-2.5)             = " + MathFuncs.lround(-2.5));
        System.out.println("frexp(8.0)               = m " + MathFuncs.frexp(8.0).mantissa
                + ", e " + MathFuncs.frexp(8.0).exp);
        System.out.println("signbit(-0.0)            = " + MathFuncs.signbit(-0.0));

        // wchar.h（UTF-16 码元）
        System.out.println("wcslen(\"hello\")          = " + WcharFuncs.wcslen("hello"));
        System.out.println("wcschr(\"hello\",'l')      = " + WcharFuncs.wcschr("hello", 'l'));

        // string.h GNU 扩展
        System.out.println("strverscmp(\"file10\",\"file2\") = "
                + StringFuncs.strverscmp("file10", "file2"));

        // strings.h / inttypes.h
        System.out.println("ffsl(0x10L)                = " + MiscFuncs.ffsl(0x10L));
        System.out.println("imaxdiv(-5L,2)             = quot " + InttypesFuncs.imaxdiv(-5L, 2).quot
                + ", rem " + InttypesFuncs.imaxdiv(-5L, 2).rem);

        // math.h 特殊函数 / 分类
        System.out.println("erf(1.0)                   = " + MathFuncs.erf(1.0));
        System.out.println("tgamma(5.0)                = " + MathFuncs.tgamma(5.0));
        System.out.println("fpclassify(-0.0)           = " + MathFuncs.fpclassify(-0.0));

        // wctype.h
        System.out.println("iswctype('A', wctype(\"upper\")) = "
                + WctypeFuncs.iswctype('A', WctypeFuncs.wctype("upper")));

        // ctype.h
        System.out.println("isdigit('7')             = " + CtypeFuncs.isdigit('7'));
        System.out.println("isxdigit('f')            = " + CtypeFuncs.isxdigit('f'));
        System.out.println("isspace('\\t')            = " + CtypeFuncs.isspace('\t'));
        System.out.println("tolower('G')             = " + (char) CtypeFuncs.tolower('G'));
        System.out.println("toupper('g')             = " + (char) CtypeFuncs.toupper('g'));
    }
}