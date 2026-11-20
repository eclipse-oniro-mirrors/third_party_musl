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

import ohos.musl.Assert;

/**
 * {@link RegexFuncs}（fnmatch.h：fnmatch）的测试套件。期望值对照 POSIX fnmatch 及 musl
 * {@code src/regex/fnmatch.c} 语义推演。
 */
public final class RegexFuncsTest {

    private static int failures = 0;

    private RegexFuncsTest() {
    }

    private static void check(Runnable r) {
        try {
            r.run();
        } catch (AssertionError e) {
            failures++;
            System.err.println("  FAIL " + e.getMessage());
        }
    }

    private static void testBasic() {
        assertMatch("abc", "abc", 0);
        assertNoMatch("abc", "abd", 0);
        assertMatch("a*c", "abc", 0);
        assertMatch("a*c", "ac", 0);
        assertMatch("*", "", 0);
        assertMatch("*", "anything", 0);
        assertMatch("a?c", "abc", 0);
        assertNoMatch("a?c", "ac", 0);
        assertNoMatch("a?c", "abbc", 0);
        assertNoMatch("?", "", 0);
        assertMatch("?", "x", 0);
        assertMatch("a*b*c", "aXbYc", 0);
        assertNoMatch("ab", "abc", 0);
    }

    private static void testBracket() {
        assertMatch("[abc]", "b", 0);
        assertNoMatch("[abc]", "d", 0);
        assertMatch("[a-c]", "b", 0);
        assertNoMatch("[a-c]", "d", 0);
        assertMatch("[a-cx-z]", "y", 0);
        assertMatch("[!a-c]", "d", 0);
        assertNoMatch("[!a-c]", "a", 0);
        assertMatch("[^a-c]", "d", 0);
        assertNoMatch("[^a-c]", "b", 0);
        assertMatch("[]]", "]", 0);
        assertMatch("[-a]", "-", 0);
        assertMatch("[a-]", "a", 0);
        assertMatch("[a-]", "-", 0);
        assertMatch("a[.]c", "a.c", 0);
    }

    private static void testEscape() {
        assertMatch("\\*", "*", 0);
        assertNoMatch("\\*", "a", 0);
        assertMatch("a\\*b", "a*b", 0);
        assertNoMatch("a\\*b", "acb", 0);
        assertMatch("\\a", "a", 0);
        assertMatch("[\\]]", "]", 0);
    }

    private static void testPathname() {
        assertNoMatch("*.txt", "a/b.txt", RegexFuncs.FNM_PATHNAME);
        assertMatch("*/*.txt", "a/b.txt", RegexFuncs.FNM_PATHNAME);
        assertMatch("*.txt", "a.txt", RegexFuncs.FNM_PATHNAME);
        assertNoMatch("a*c", "a/c", RegexFuncs.FNM_PATHNAME);
        assertNoMatch("a?c", "a/c", RegexFuncs.FNM_PATHNAME);
        assertMatch("foo/bar", "foo/bar", RegexFuncs.FNM_PATHNAME);
        assertNoMatch("foo", "foo/bar", RegexFuncs.FNM_PATHNAME);
        assertNoMatch("foo/bar", "foo", RegexFuncs.FNM_PATHNAME);
    }

    private static void testPeriod() {
        assertNoMatch("*", ".junk", RegexFuncs.FNM_PERIOD);
        assertMatch(".*", ".junk", RegexFuncs.FNM_PERIOD);
        assertNoMatch("?x", ".x", RegexFuncs.FNM_PERIOD);
        assertMatch(".x", ".x", RegexFuncs.FNM_PERIOD);
        assertNoMatch("*.junk", ".junk", RegexFuncs.FNM_PERIOD);
        assertMatch("*", "junk", RegexFuncs.FNM_PERIOD);
    }

    private static void testCasefold() {
        assertMatch("ABC", "abc", RegexFuncs.FNM_CASEFOLD);
        assertMatch("abc", "abc", 0);
        assertNoMatch("abc", "ABC", 0);
        assertMatch("[A-C]", "b", RegexFuncs.FNM_CASEFOLD);
        assertMatch("a?c", "ABC", RegexFuncs.FNM_CASEFOLD);
        assertMatch("a*c", "AbC", RegexFuncs.FNM_CASEFOLD);
    }

    private static void testNoEscape() {
        assertMatch("\\a", "a", 0);
        assertNoMatch("\\a", "a", RegexFuncs.FNM_NOESCAPE);
        assertMatch("\\a", "\\a", RegexFuncs.FNM_NOESCAPE);
        assertMatch("a*b", "aXb", RegexFuncs.FNM_NOESCAPE);
    }

    private static void testLeadingDir() {
        assertMatch("foo", "foo/bar", RegexFuncs.FNM_LEADING_DIR);
        assertMatch("foo", "foo", RegexFuncs.FNM_LEADING_DIR);
        assertNoMatch("bar", "foo/bar", RegexFuncs.FNM_LEADING_DIR);
        assertMatch("fo*", "foo/bar", RegexFuncs.FNM_LEADING_DIR);
        assertMatch("foo", "foo/bar/baz", RegexFuncs.FNM_LEADING_DIR);
        // FNM_PATHNAME | FNM_LEADING_DIR
        assertMatch("foo", "foo/bar", RegexFuncs.FNM_PATHNAME | RegexFuncs.FNM_LEADING_DIR);
    }

    private static void assertMatch(String pattern, String str, int flags) {
        Assert.assertTrue(RegexFuncs.fnmatch(pattern, str, flags),
                "fnmatch(\"" + pattern + "\", \"" + str + "\", " + flags + ") should match");
    }

    private static void assertNoMatch(String pattern, String str, int flags) {
        Assert.assertTrue(!RegexFuncs.fnmatch(pattern, str, flags),
                "fnmatch(\"" + pattern + "\", \"" + str + "\", " + flags + ") should NOT match");
    }

    public static int run() {
        failures = 0;
        System.out.println("== RegexFuncsTest ==");
        check(RegexFuncsTest::testBasic);
        check(RegexFuncsTest::testBracket);
        check(RegexFuncsTest::testEscape);
        check(RegexFuncsTest::testPathname);
        check(RegexFuncsTest::testPeriod);
        check(RegexFuncsTest::testCasefold);
        check(RegexFuncsTest::testNoEscape);
        check(RegexFuncsTest::testLeadingDir);
        System.out.println("  " + (failures == 0 ? "PASS" : failures + " FAILURE(S)"));
        return failures;
    }
}