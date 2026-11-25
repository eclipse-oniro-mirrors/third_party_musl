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

    private static void testStrchrnul() {
        Assert.assertEquals(2, StringFuncs.strchrnul("hello", 'l'), "strchrnul found");
        Assert.assertEquals(5, StringFuncs.strchrnul("hello", 'z'), "strchrnul not found -> len");
        Assert.assertEquals(5, StringFuncs.strchrnul("hello", 0), "strchrnul nul -> len");
        Assert.assertEquals(0, StringFuncs.strchrnul("", 'z'), "strchrnul empty -> 0");
    }

    private static void testMemchr() {
        Assert.assertEquals(2, StringFuncs.memchr(new byte[] {1, 2, 3, 4}, 3, 4), "memchr found");
        Assert.assertEquals(-1, StringFuncs.memchr(new byte[] {1, 2, 3}, 5, 3), "memchr not found");
        Assert.assertEquals(0, StringFuncs.memchr(new byte[] {1, 2, 3}, 1, 1), "memchr first");
        Assert.assertEquals(-1, StringFuncs.memchr(new byte[0], 'a', 0), "memchr empty");
        // 无符号字节：0xFF(-1) 可匹配 0xFF
        Assert.assertEquals(0, StringFuncs.memchr(new byte[] {(byte) 0xFF}, 0xFF, 1), "memchr unsigned");
    }

    private static void testMemcpy() {
        byte[] dst = {9, 9, 9, 9};
        StringFuncs.memcpy(dst, new byte[] {1, 2, 3}, 3);
        Assert.assertEquals(1, dst[0], "memcpy d0");
        Assert.assertEquals(2, dst[1], "memcpy d1");
        Assert.assertEquals(3, dst[2], "memcpy d2");
        Assert.assertEquals(9, dst[3], "memcpy d3 untouched");
    }

    private static void testMemmove() {
        byte[] dst = {0, 0, 0};
        StringFuncs.memmove(dst, new byte[] {5, 6, 7}, 3);
        Assert.assertEquals(5, dst[0], "memmove d0");
        Assert.assertEquals(6, dst[1], "memmove d1");
        Assert.assertEquals(7, dst[2], "memmove d2");
    }

    private static void testMemset() {
        byte[] dst = {1, 2, 3, 4};
        StringFuncs.memset(dst, 0x61, 3);
        Assert.assertEquals(0x61, dst[0], "memset d0");
        Assert.assertEquals(0x61, dst[1], "memset d1");
        Assert.assertEquals(0x61, dst[2], "memset d2");
        Assert.assertEquals(4, dst[3], "memset d3 untouched");
    }

    private static void testMemmem() {
        // 空 needle / needle 长于 haystack
        Assert.assertEquals(0, StringFuncs.memmem(new byte[] {1, 2, 3}, 3, new byte[] {}, 0),
                "memmem empty needle");
        Assert.assertEquals(-1, StringFuncs.memmem(new byte[] {1, 2}, 2, new byte[] {1, 2, 3}, 3),
                "memmem needle longer");

        // nl == 1
        Assert.assertEquals(2, StringFuncs.memmem(new byte[] {1, 2, 3, 4}, 4, new byte[] {3}, 1),
                "memmem len1 found");
        Assert.assertEquals(-1, StringFuncs.memmem(new byte[] {1, 2, 3}, 3, new byte[] {9}, 1),
                "memmem len1 not found");

        // nl == 2
        Assert.assertEquals(1, StringFuncs.memmem(new byte[] {1, 2, 3, 4}, 4, new byte[] {2, 3}, 2),
                "memmem len2");
        Assert.assertEquals(0, StringFuncs.memmem(new byte[] {1, 2, 1, 2}, 4, new byte[] {1, 2}, 2),
                "memmem len2 first");
        Assert.assertEquals(2, StringFuncs.memmem(new byte[] {1, 1, 1, 2}, 4, new byte[] {1, 2}, 2),
                "memmem len2 repeated");
        Assert.assertEquals(-1, StringFuncs.memmem(new byte[] {1, 2, 3}, 3, new byte[] {3, 4}, 2),
                "memmem len2 not found");

        // nl == 3
        Assert.assertEquals(0, StringFuncs.memmem(new byte[] {1, 2, 3, 4}, 4, new byte[] {1, 2, 3}, 3),
                "memmem len3");
        Assert.assertEquals(1, StringFuncs.memmem(new byte[] {9, 1, 2, 3}, 4, new byte[] {1, 2, 3}, 3),
                "memmem len3 pos1");

        // nl == 4
        Assert.assertEquals(2, StringFuncs.memmem(new byte[] {9, 9, 1, 2, 3, 4}, 6,
                new byte[] {1, 2, 3, 4}, 4), "memmem len4");

        // nl >= 5（Two-Way）
        Assert.assertEquals(0, StringFuncs.memmem(new byte[] {1, 2, 3, 4, 5, 1, 2, 3, 4, 5, 6}, 11,
                new byte[] {1, 2, 3, 4, 5}, 5), "memmem twoway first");
        Assert.assertEquals(3, StringFuncs.memmem(new byte[] {9, 9, 9, 1, 2, 3, 4, 5, 9}, 9,
                new byte[] {1, 2, 3, 4, 5}, 5), "memmem twoway pos3");
        Assert.assertEquals(-1, StringFuncs.memmem(new byte[] {1, 2, 3, 4, 9}, 5,
                new byte[] {1, 2, 3, 4, 5}, 5), "memmem twoway not found");
        Assert.assertEquals(2, StringFuncs.memmem(new byte[] {9, 9, 1, 2, 1, 2, 1, 9, 9}, 9,
                new byte[] {1, 2, 1, 2, 1}, 5), "memmem periodic");

        // 随机，以暴力搜索为 oracle
        java.util.Random rnd = new java.util.Random(20261119);
        for (int t = 0; t < 500; t++) {
            int hl = rnd.nextInt(40);
            int nl = rnd.nextInt(12);
            byte[] hh = new byte[hl];
            byte[] nn = new byte[nl];
            rnd.nextBytes(hh);
            rnd.nextBytes(nn);
            int expected = bruteMemmem(hh, hl, nn, nl);
            int actual = StringFuncs.memmem(hh, hl, nn, nl);
            Assert.assertEquals(expected, actual, "memmem random hl=" + hl + " nl=" + nl);
        }
    }

    private static int bruteMemmem(byte[] h, int hl, byte[] n, int nl) {
        if (nl == 0) {
            return 0;
        }
        outer:
        for (int i = 0; i + nl <= hl; i++) {
            for (int j = 0; j < nl; j++) {
                if (h[i + j] != n[j]) {
                    continue outer;
                }
            }
            return i;
        }
        return -1;
    }

    private static void testMemrchr() {
        byte[] a = {1, 2, 3, 2, 1};
        Assert.assertEquals(3, StringFuncs.memrchr(a, 2, 5), "memrchr last 2");
        Assert.assertEquals(4, StringFuncs.memrchr(a, 1, 5), "memrchr last 1");
        Assert.assertEquals(0, StringFuncs.memrchr(a, 1, 1), "memrchr range only first");
        Assert.assertEquals(-1, StringFuncs.memrchr(a, 1, 0), "memrchr n=0");
        Assert.assertEquals(-1, StringFuncs.memrchr(a, 9, 5), "memrchr absent");
        // 无符号字节：0xFF 与 -1 视为同一字节
        Assert.assertEquals(1, StringFuncs.memrchr(new byte[] {0, (byte) 0xFF, 0}, -1, 3),
                "memrchr unsigned byte");
        Assert.assertEquals(1, StringFuncs.memrchr(new byte[] {5, 5, 5}, 5, 2), "memrchr bounded n");
    }

    private static void testRawmemchr() {
        byte[] a = {1, 2, 3, 2, 1};
        Assert.assertEquals(0, StringFuncs.rawmemchr(a, 1), "rawmemchr first");
        Assert.assertEquals(1, StringFuncs.rawmemchr(a, 2), "rawmemchr 2");
        Assert.assertEquals(2, StringFuncs.rawmemchr(a, 3), "rawmemchr 3");
        Assert.assertEquals(-1, StringFuncs.rawmemchr(a, 9), "rawmemchr absent");
        Assert.assertEquals(-1, StringFuncs.rawmemchr(new byte[0], 1), "rawmemchr empty");
        Assert.assertEquals(1, StringFuncs.rawmemchr(new byte[] {0, (byte) 0xFF}, 255),
                "rawmemchr unsigned byte");
    }

    private static void testStrCasestr() {
        Assert.assertEquals(6, StringFuncs.strcasestr("Hello World", "world"), "strcasestr mixed case");
        Assert.assertEquals(0, StringFuncs.strcasestr("Hello", "HELLO"), "strcasestr all upper");
        Assert.assertEquals(2, StringFuncs.strcasestr("abcabc", "CA"), "strcasestr repeat");
        Assert.assertEquals(0, StringFuncs.strcasestr("abc", ""), "strcasestr empty needle");
        Assert.assertEquals(-1, StringFuncs.strcasestr("abc", "x"), "strcasestr absent");
        Assert.assertEquals(0, StringFuncs.strcasestr("", ""), "strcasestr both empty");
        Assert.assertEquals(-1, StringFuncs.strcasestr("", "a"), "strcasestr empty haystack");
    }

    private static void testStrCasestrRandom() {
        java.util.Random rnd = new java.util.Random(20261120);
        for (int t = 0; t < 500; t++) {
            int hl = rnd.nextInt(30);
            StringBuilder h = new StringBuilder();
            for (int i = 0; i < hl; i++) {
                char c = (char) ('a' + rnd.nextInt(26));
                h.append(rnd.nextBoolean() ? Character.toUpperCase(c) : c);
            }
            int nl = rnd.nextInt(6);
            StringBuilder n = new StringBuilder();
            for (int i = 0; i < nl; i++) {
                n.append((char) ('a' + rnd.nextInt(26)));
            }
            String hs = h.toString();
            String ns = n.toString();
            int expected = hs.toLowerCase().indexOf(ns);
            int actual = StringFuncs.strcasestr(hs, ns);
            Assert.assertEquals(expected, actual, "strcasestr random");
        }
    }

    private static void testStrverscmp() {
        Assert.assertEquals(0, StringFuncs.strverscmp("foo", "foo"), "strverscmp equal");
        Assert.assertTrue(StringFuncs.strverscmp("foo.1", "foo.9") < 0, "strverscmp foo.1 < foo.9");
        Assert.assertTrue(StringFuncs.strverscmp("foo.9", "foo.1") > 0, "strverscmp foo.9 > foo.1");
        Assert.assertTrue(StringFuncs.strverscmp("file10", "file2") > 0, "strverscmp file10 > file2");
        Assert.assertTrue(StringFuncs.strverscmp("file2", "file10") < 0, "strverscmp file2 < file10");
        Assert.assertTrue(StringFuncs.strverscmp("abc", "abd") < 0, "strverscmp plain");
        Assert.assertTrue(StringFuncs.strverscmp("file01", "file02") < 0, "strverscmp leading zero");
        Assert.assertTrue(StringFuncs.strverscmp("a10b", "a9b") > 0, "strverscmp mid digit run");
    }

    private static void testStpcpy() {
        char[] buf = new char[10];
        Assert.assertEquals(3, StringFuncs.stpcpy(buf, "abc"), "stpcpy return len");
        Assert.assertEquals('a', buf[0], "stpcpy buf[0]");
        Assert.assertEquals('c', buf[2], "stpcpy buf[2]");

        Assert.assertEquals(3, StringFuncs.stpncpy(buf, "abcde", 3), "stpncpy truncate return");
        Assert.assertEquals('c', buf[2], "stpncpy truncate buf[2]");

        Assert.assertEquals(2, StringFuncs.stpncpy(buf, "ab", 5), "stpncpy pad return");
        Assert.assertEquals('a', buf[0], "stpncpy pad buf[0]");
        Assert.assertEquals(0, buf[2], "stpncpy pad buf[2] zero");
        Assert.assertEquals(0, buf[4], "stpncpy pad buf[4] zero");

        Assert.assertEquals(5, StringFuncs.stpncpy(buf, "abcde", 5), "stpncpy exact n");
        Assert.assertEquals(0, StringFuncs.stpncpy(buf, "abc", 0), "stpncpy n=0");
    }

    private static void testMemccpy() {
        byte[] dst = new byte[8];
        Assert.assertEquals(3, StringFuncs.memccpy(dst, new byte[] {1, 2, 3, 4}, 3, 8), "memccpy found");
        Assert.assertEquals(3, dst[2], "memccpy dst[2]");
        Assert.assertEquals(0, dst[3], "memccpy dst[3] untouched");
        Assert.assertEquals(-1, StringFuncs.memccpy(dst, new byte[] {1, 2}, 9, 2), "memccpy not found");
        Assert.assertEquals(1, dst[0], "memccpy copied 1");
        Assert.assertEquals(2, dst[1], "memccpy copied 2");
        Assert.assertEquals(-1, StringFuncs.memccpy(dst, new byte[] {1, 2}, 3, 0), "memccpy n=0");
        Assert.assertEquals(1, StringFuncs.memccpy(dst, new byte[] {7, 8}, 7, 2), "memccpy c at first");
    }

    private static void testMemccpyRandom() {
        java.util.Random rnd = new java.util.Random(20261124);
        for (int t = 0; t < 300; t++) {
            int n = rnd.nextInt(16);
            byte[] src = new byte[n];
            rnd.nextBytes(src);
            int c = rnd.nextInt(256);
            byte[] dst = new byte[n];
            int expected = -1;
            for (int i = 0; i < n; i++) {
                dst[i] = src[i];
                if ((src[i] & 0xFF) == c) {
                    expected = i + 1;
                    break;
                }
            }
            byte[] check = new byte[n];
            int actual = StringFuncs.memccpy(check, src, c, n);
            Assert.assertEquals(expected, actual, "memccpy random");
            Assert.assertTrue(java.util.Arrays.equals(check, dst), "memccpy random content");
        }
    }

    private static void testStrcpyCat() {
        char[] dst = new char[10];
        Assert.assertEquals(3, StringFuncs.strcpy(dst, "abc"), "strcpy return");
        Assert.assertEquals('a', dst[0], "strcpy dst[0]");
        Assert.assertEquals(0, dst[3], "strcpy terminator");

        Assert.assertEquals(3, StringFuncs.strncpy(dst, "abcde", 3), "strncpy truncate");
        Assert.assertEquals(2, StringFuncs.strncpy(dst, "ab", 5), "strncpy pad return");
        Assert.assertEquals(0, dst[2], "strncpy pad zero");

        Assert.assertEquals(4, StringFuncs.strcat(dst, "xy"), "strcat return");
        Assert.assertEquals('x', dst[2], "strcat dst[2]");
        Assert.assertEquals(0, dst[4], "strcat terminator");

        Assert.assertEquals(6, StringFuncs.strncat(dst, "zzz", 2), "strncat return");
        Assert.assertEquals('z', dst[4], "strncat dst[4]");
        Assert.assertEquals(0, dst[6], "strncat terminator");
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
        check(StringFuncsTest::testStrchrnul);
        check(StringFuncsTest::testMemchr);
        check(StringFuncsTest::testMemcpy);
        check(StringFuncsTest::testMemmove);
        check(StringFuncsTest::testMemset);
        check(StringFuncsTest::testMemrchr);
        check(StringFuncsTest::testRawmemchr);
        check(StringFuncsTest::testMemmem);
        check(StringFuncsTest::testStrCasestr);
        check(StringFuncsTest::testStrCasestrRandom);
        check(StringFuncsTest::testStrverscmp);
        check(StringFuncsTest::testStpcpy);
        check(StringFuncsTest::testMemccpy);
        check(StringFuncsTest::testMemccpyRandom);
        check(StringFuncsTest::testStrcpyCat);
        System.out.println("  " + (failures == 0 ? "PASS" : failures + " FAILURE(S)"));
        return failures;
    }
}