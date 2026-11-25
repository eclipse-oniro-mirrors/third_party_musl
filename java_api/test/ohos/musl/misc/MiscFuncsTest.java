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

package ohos.musl.misc;

import ohos.musl.Assert;

/**
 * {@link MiscFuncs}（strings.h：ffs）的测试套件。
 */
public final class MiscFuncsTest {

    private static int failures = 0;

    private MiscFuncsTest() {
    }

    private static void check(Runnable r) {
        try {
            r.run();
        } catch (AssertionError e) {
            failures++;
            System.err.println("  FAIL " + e.getMessage());
        }
    }

    private static void testFfs() {
        Assert.assertEquals(0, MiscFuncs.ffs(0), "ffs(0)");
        Assert.assertEquals(1, MiscFuncs.ffs(1), "ffs(1)");
        Assert.assertEquals(2, MiscFuncs.ffs(2), "ffs(2)");
        Assert.assertEquals(3, MiscFuncs.ffs(4), "ffs(4)");
        Assert.assertEquals(4, MiscFuncs.ffs(8), "ffs(8)");
        Assert.assertEquals(5, MiscFuncs.ffs(0x10), "ffs(0x10)");
        Assert.assertEquals(32, MiscFuncs.ffs(0x80000000), "ffs(0x80000000)");
    }

    private static void testFfsl() {
        Assert.assertEquals(0, MiscFuncs.ffsl(0L), "ffsl(0)");
        Assert.assertEquals(1, MiscFuncs.ffsl(1L), "ffsl(1)");
        Assert.assertEquals(2, MiscFuncs.ffsl(2L), "ffsl(2)");
        Assert.assertEquals(3, MiscFuncs.ffsl(4L), "ffsl(4)");
        Assert.assertEquals(5, MiscFuncs.ffsl(0x10L), "ffsl(0x10)");
        Assert.assertEquals(64, MiscFuncs.ffsl(Long.MIN_VALUE), "ffsl(Long.MIN_VALUE)");
        Assert.assertEquals(0, MiscFuncs.ffsll(0L), "ffsll(0)");
        Assert.assertEquals(1, MiscFuncs.ffsll(3L), "ffsll(3)");
        Assert.assertEquals(64, MiscFuncs.ffsll(Long.MIN_VALUE), "ffsll(Long.MIN_VALUE)");
    }

    public static int run() {
        failures = 0;
        System.out.println("== MiscFuncsTest ==");
        check(MiscFuncsTest::testFfs);
        check(MiscFuncsTest::testFfsl);
        System.out.println("  " + (failures == 0 ? "PASS" : failures + " FAILURE(S)"));
        return failures;
    }
}