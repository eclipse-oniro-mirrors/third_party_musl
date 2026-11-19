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

import ohos.musl.Assert;

/**
 * {@link StringFuncs} 的测试套件。期望值对照 musl {@code src/string/*.c} 语义推演。
 */
public final class StringFuncsTest {

    private static int failures = 0;

    private StringFuncsTest() {
    }

    private static void check(Runnable r) {
        try {
            r.run();
        } catch (AssertionError e) {
            failures++;
            System.err.println("  FAIL " + e.getMessage());
        }
    }

    private static void testStrlen() {
        Assert.assertEquals(0, StringFuncs.strlen(""), "strlen(\"\")");
        Assert.assertEquals(1, StringFuncs.strlen("a"), "strlen(\"a\")");
        Assert.assertEquals(5, StringFuncs.strlen("hello"), "strlen(\"hello\")");
        Assert.assertEquals(13, StringFuncs.strlen("hello, world!"), "strlen(\"hello, world!\")");
    }

    private static void testStrnlen() {
        Assert.assertEquals(0, StringFuncs.strnlen("hello", 0), "strnlen n=0");
        Assert.assertEquals(3, StringFuncs.strnlen("hello", 3), "strnlen n<len");
        Assert.assertEquals(5, StringFuncs.strnlen("hello", 10), "strnlen n>len");
        Assert.assertEquals(0, StringFuncs.strnlen("", 5), "strnlen empty");
    }

    private static void testStrcmp() {
        Assert.assertEquals(0, StringFuncs.strcmp("", ""), "strcmp(\"\",\"\")");
        Assert.assertEquals(0, StringFuncs.strcmp("abc", "abc"), "strcmp equal");
        Assert.assertEquals(-1, StringFuncs.strcmp("a", "b"), "strcmp(\"a\",\"b\")");
        Assert.assertEquals(1, StringFuncs.strcmp("b", "a"), "strcmp(\"b\",\"a\")");
        Assert.assertEquals(-1, StringFuncs.strcmp("abc", "abd"), "strcmp(\"abc\",\"abd\")");
        Assert.assertEquals('c', StringFuncs.strcmp("abc", "ab"), "strcmp(\"abc\",\"ab\")");
        Assert.assertEquals(-'c', StringFuncs.strcmp("ab", "abc"), "strcmp(\"ab\",\"abc\")");
        Assert.assertEquals(-'a', StringFuncs.strcmp("", "a"), "strcmp(\"\",\"a\")");
        Assert.assertEquals('a', StringFuncs.strcmp("a", ""), "strcmp(\"a\",\"\")");
        Assert.assertEquals(-32, StringFuncs.strcmp("ABC", "abc"), "strcmp(\"ABC\",\"abc\")");
    }

    private static void testStrncmp() {
        Assert.assertEquals(0, StringFuncs.strncmp("abc", "abc", 3), "strncmp equal n=3");
        Assert.assertEquals(0, StringFuncs.strncmp("abc", "abc", 5), "strncmp equal n>len");
        Assert.assertEquals(0, StringFuncs.strncmp("abc", "abc", 0), "strncmp n=0");
        Assert.assertEquals(0, StringFuncs.strncmp("abc", "abd", 2), "strncmp diff after n");
        Assert.assertEquals(-1, StringFuncs.strncmp("abc", "abd", 3), "strncmp diff at last");
        Assert.assertEquals(0, StringFuncs.strncmp("abc", "abx", 1), "strncmp n=1 equal");
        // 'c'(99) - 'x'(120) = -21
        Assert.assertEquals(-21, StringFuncs.strncmp("abc", "abx", 3), "strncmp diff");
        Assert.assertEquals(0, StringFuncs.strncmp("a", "ab", 1), "strncmp prefix n=1");
        Assert.assertEquals(-'b', StringFuncs.strncmp("a", "ab", 2), "strncmp l shorter");
        Assert.assertEquals('b', StringFuncs.strncmp("ab", "a", 2), "strncmp r shorter");
        Assert.assertEquals(-'a', StringFuncs.strncmp("", "a", 1), "strncmp empty l");
        Assert.assertEquals('a', StringFuncs.strncmp("a", "", 1), "strncmp empty r");
        Assert.assertEquals(0, StringFuncs.strncmp("", "", 5), "strncmp both empty");
    }

    private static void testStrcasecmp() {
        Assert.assertEquals(0, StringFuncs.strcasecmp("a", "A"), "strcasecmp(\"a\",\"A\")");
        Assert.assertEquals(0, StringFuncs.strcasecmp("ABC", "abc"), "strcasecmp case fold");
        Assert.assertEquals(-1, StringFuncs.strcasecmp("a", "b"), "strcasecmp(\"a\",\"b\")");
        Assert.assertEquals(-1, StringFuncs.strcasecmp("abc", "abd"), "strcasecmp(\"abc\",\"abd\")");
        Assert.assertEquals('c', StringFuncs.strcasecmp("abc", "ab"), "strcasecmp l longer");
        Assert.assertEquals(-'c', StringFuncs.strcasecmp("ab", "abc"), "strcasecmp r longer");
        Assert.assertEquals(-'a', StringFuncs.strcasecmp("", "a"), "strcasecmp empty l");
        Assert.assertEquals('a', StringFuncs.strcasecmp("a", ""), "strcasecmp empty r");
    }

    private static void testStrncasecmp() {
        Assert.assertEquals(0, StringFuncs.strncasecmp("a", "A", 1), "strncasecmp n=1");
        Assert.assertEquals(0, StringFuncs.strncasecmp("AB", "ab", 2), "strncasecmp n=2");
        Assert.assertEquals(0, StringFuncs.strncasecmp("abc", "abd", 2), "strncasecmp diff after n");
        Assert.assertEquals(-1, StringFuncs.strncasecmp("abc", "abd", 3), "strncasecmp diff");
        Assert.assertEquals(0, StringFuncs.strncasecmp("a", "ab", 1), "strncasecmp prefix n=1");
        // 0 - tolower('b') = -98
        Assert.assertEquals(-'b', StringFuncs.strncasecmp("a", "ab", 2), "strncasecmp l shorter");
        Assert.assertEquals(0, StringFuncs.strncasecmp("a", "A", 0), "strncasecmp n=0");
    }

    private static void testStrchr() {
        Assert.assertEquals(2, StringFuncs.strchr("hello", 'l'), "strchr first l");
        Assert.assertEquals(0, StringFuncs.strchr("hello", 'h'), "strchr head");
        Assert.assertEquals(4, StringFuncs.strchr("hello", 'o'), "strchr tail");
        Assert.assertEquals(1, StringFuncs.strchr("hello", 'e'), "strchr middle");
        Assert.assertEquals(-1, StringFuncs.strchr("hello", 'z'), "strchr not found");
        Assert.assertEquals(-1, StringFuncs.strchr("", 'a'), "strchr empty");
        Assert.assertEquals(0, StringFuncs.strchr("A", 321), "strchr mask to byte");
        Assert.assertEquals(-1, StringFuncs.strchr("hello", 0), "strchr nul");
    }

    private static void testStrrchr() {
        Assert.assertEquals(3, StringFuncs.strrchr("hello", 'l'), "strrchr last l");
        Assert.assertEquals(0, StringFuncs.strrchr("hello", 'h'), "strrchr head");
        Assert.assertEquals(4, StringFuncs.strrchr("hello", 'o'), "strrchr tail");
        Assert.assertEquals(-1, StringFuncs.strrchr("hello", 'z'), "strrchr not found");
        Assert.assertEquals(-1, StringFuncs.strrchr("", 'a'), "strrchr empty");
        // C 连同结尾 NUL 一起查找
        Assert.assertEquals(5, StringFuncs.strrchr("hello", 0), "strrchr nul -> len");
    }

    private static void testStrstr() {
        Assert.assertEquals(2, StringFuncs.strstr("abcabc", "ca"), "strstr basic");
        Assert.assertEquals(0, StringFuncs.strstr("hello", ""), "strstr empty needle");
        Assert.assertEquals(0, StringFuncs.strstr("hello", "hello"), "strstr whole");
        Assert.assertEquals(3, StringFuncs.strstr("hello", "lo"), "strstr tail");
        Assert.assertEquals(-1, StringFuncs.strstr("hello", "world"), "strstr not found");
        Assert.assertEquals(-1, StringFuncs.strstr("hello", "helloo"), "strstr needle too long");
        Assert.assertEquals(0, StringFuncs.strstr("", ""), "strstr both empty");
        Assert.assertEquals(-1, StringFuncs.strstr("", "a"), "strstr empty haystack");
        Assert.assertEquals(0, StringFuncs.strstr("aaa", "aa"), "strstr overlap");
    }

    private static void testStrspn() {
        Assert.assertEquals(0, StringFuncs.strspn("abc", ""), "strspn empty set");
        Assert.assertEquals(1, StringFuncs.strspn("abc", "a"), "strspn single");
        Assert.assertEquals(3, StringFuncs.strspn("aaab", "a"), "strspn repeat");
        Assert.assertEquals(4, StringFuncs.strspn("hello", "hel"), "strspn prefix");
        Assert.assertEquals(3, StringFuncs.strspn("123abc", "123456"), "strspn digits");
        Assert.assertEquals(0, StringFuncs.strspn("", "abc"), "strspn empty s");
    }

    private static void testStrcspn() {
        Assert.assertEquals(3, StringFuncs.strcspn("abc", "def"), "strcspn no hit");
        Assert.assertEquals(2, StringFuncs.strcspn("hello", "lo"), "strcspn first l");
        Assert.assertEquals(0, StringFuncs.strcspn("hello", "h"), "strcspn head hit");
        Assert.assertEquals(3, StringFuncs.strcspn("abc", ""), "strcspn empty set");
        Assert.assertEquals(5, StringFuncs.strcspn("hello", "z"), "strcspn none");
    }

    private static void testStrpbrk() {
        Assert.assertEquals(1, StringFuncs.strpbrk("hello", "aeiou"), "strpbrk vowel");
        Assert.assertEquals(2, StringFuncs.strpbrk("hello", "l"), "strpbrk l");
        Assert.assertEquals(-1, StringFuncs.strpbrk("hello", "xyz"), "strpbrk not found");
        Assert.assertEquals(-1, StringFuncs.strpbrk("hello", ""), "strpbrk empty set");
    }

    private static void testMemcmp() {
        Assert.assertEquals(0, StringFuncs.memcmp(new byte[] {1, 2, 3}, new byte[] {1, 2, 3}, 3), "memcmp equal");
        Assert.assertEquals(-1, StringFuncs.memcmp(new byte[] {1, 2, 3}, new byte[] {1, 2, 4}, 3), "memcmp diff");
        Assert.assertEquals(0, StringFuncs.memcmp(new byte[] {1, 2, 3}, new byte[] {1, 2, 9}, 2), "memcmp n prefix");
        Assert.assertEquals(0, StringFuncs.memcmp(new byte[0], new byte[0], 0), "memcmp n=0");
        // 无符号字节比较：0x80(128) - 0x00(0) = 128
        Assert.assertEquals(128, StringFuncs.memcmp(new byte[] {(byte) 0x80}, new byte[] {0}, 1), "memcmp unsigned");
        // 0xFF(255) - 0x00(0) = 255
        Assert.assertEquals(255, StringFuncs.memcmp(new byte[] {(byte) 0xFF}, new byte[] {0}, 1), "memcmp signed byte");
    }

    public static int run() {
        failures = 0;
        System.out.println("== StringFuncsTest ==");
        check(StringFuncsTest::testStrlen);
        check(StringFuncsTest::testStrnlen);
        check(StringFuncsTest::testStrcmp);
        check(StringFuncsTest::testStrncmp);
        check(StringFuncsTest::testStrcasecmp);
        check(StringFuncsTest::testStrncasecmp);
        check(StringFuncsTest::testStrchr);
        check(StringFuncsTest::testStrrchr);
        check(StringFuncsTest::testStrstr);
        check(StringFuncsTest::testStrspn);
        check(StringFuncsTest::testStrcspn);
        check(StringFuncsTest::testStrpbrk);
        check(StringFuncsTest::testMemcmp);
        System.out.println("  " + (failures == 0 ? "PASS" : failures + " FAILURE(S)"));
        return failures;
    }
}