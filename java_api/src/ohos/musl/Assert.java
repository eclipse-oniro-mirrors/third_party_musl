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

/**
 * 极简无依赖断言工具，供测试套件使用（避免引入 JUnit 等第三方依赖）。
 *
 * <p>断言失败抛出 {@link AssertionError}，由各测试类的 {@code check} 包装器捕获并计数。</p>
 */
public final class Assert {

    private Assert() {
    }

    public static void assertEquals(int expected, int actual, String msg) {
        if (expected != actual) {
            throw new AssertionError(msg + " (expected " + expected + ", got " + actual + ")");
        }
    }

    public static void assertEquals(long expected, long actual, String msg) {
        if (expected != actual) {
            throw new AssertionError(msg + " (expected " + expected + ", got " + actual + ")");
        }
    }

    public static void assertTrue(boolean cond, String msg) {
        if (!cond) {
            throw new AssertionError(msg + " (expected true, got false)");
        }
    }

    public static void assertEquals(double expected, double actual, double eps, String msg) {
        if (Double.isNaN(expected)) {
            if (!Double.isNaN(actual)) {
                throw new AssertionError(msg + " (expected NaN, got " + actual + ")");
            }
        } else if (Math.abs(expected - actual) > eps) {
            throw new AssertionError(msg + " (expected " + expected + ", got " + actual + ")");
        }
    }

    public static void assertIsNaN(double actual, String msg) {
        if (!Double.isNaN(actual)) {
            throw new AssertionError(msg + " (expected NaN, got " + actual + ")");
        }
    }
}
