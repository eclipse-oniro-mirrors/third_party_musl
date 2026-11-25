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

package ohos.musl.inttypes;

import ohos.musl.Assert;
import ohos.musl.stdlib.StdlibFuncs;

/**
 * {@link InttypesFuncs}（inttypes.h：imaxabs/imaxdiv/strtoimax/strtoumax）的测试套件。
 */
public final class InttypesFuncsTest {

    private static int failures = 0;

    private InttypesFuncsTest() {
    }

    private static void check(Runnable r) {
        try {
            r.run();
        } catch (AssertionError e) {
            failures++;
            System.err.println("  FAIL " + e.getMessage());
        }
    }

    private static void testImax() {
        Assert.assertEquals(42L, InttypesFuncs.imaxabs(-42L), "imaxabs(-42)");
        Assert.assertEquals(42L, InttypesFuncs.imaxabs(42L), "imaxabs(42)");
        Assert.assertEquals(Long.MAX_VALUE, InttypesFuncs.imaxabs(Long.MAX_VALUE), "imaxabs(MAX)");
        Assert.assertEquals(Long.MIN_VALUE, InttypesFuncs.imaxabs(Long.MIN_VALUE), "imaxabs(MIN)");

        StdlibFuncs.Ldiv d = InttypesFuncs.imaxdiv(-5L, 2);
        Assert.assertEquals(-2L, d.quot, "imaxdiv(-5,2).quot");
        Assert.assertEquals(-1L, d.rem, "imaxdiv(-5,2).rem");
        d = InttypesFuncs.imaxdiv(7L, 3);
        Assert.assertEquals(2L, d.quot, "imaxdiv(7,3).quot");
        Assert.assertEquals(1L, d.rem, "imaxdiv(7,3).rem");
    }

    private static void testStrtoimax() {
        Assert.assertEquals(42L, InttypesFuncs.strtoimax("42", 10), "strtoimax 42");
        Assert.assertEquals(-42L, InttypesFuncs.strtoimax("-42", 10), "strtoimax -42");
        Assert.assertEquals(26L, InttypesFuncs.strtoimax("0x1A", 0), "strtoimax hex");
        Assert.assertEquals(8L, InttypesFuncs.strtoimax("010", 0), "strtoimax octal");
        Assert.assertEquals(Long.MAX_VALUE, InttypesFuncs.strtoimax("9223372036854775808", 10),
                "strtoimax overflow");
        Assert.assertEquals(0L, InttypesFuncs.strtoimax("abc", 10), "strtoimax no digit");
    }

    private static void testStrtoumax() {
        Assert.assertEquals(42L, InttypesFuncs.strtoumax("42", 10), "strtoumax 42");
        Assert.assertEquals(255L, InttypesFuncs.strtoumax("0xFF", 0), "strtoumax hex");
        Assert.assertEquals(-1L, InttypesFuncs.strtoumax("18446744073709551616", 10),
                "strtoumax overflow -> UINTMAX_MAX");
        Assert.assertEquals(-42L, InttypesFuncs.strtoumax("-42", 10), "strtoumax -42 wrap");
        Assert.assertEquals(0L, InttypesFuncs.strtoumax("", 10), "strtoumax empty");
    }

    public static int run() {
        failures = 0;
        System.out.println("== InttypesFuncsTest ==");
        check(InttypesFuncsTest::testImax);
        check(InttypesFuncsTest::testStrtoimax);
        check(InttypesFuncsTest::testStrtoumax);
        System.out.println("  " + (failures == 0 ? "PASS" : failures + " FAILURE(S)"));
        return failures;
    }
}
