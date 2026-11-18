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
 * Java 版本的 {@code <math.h>} 纯函数接口。
 *
 * <p>Java 标准库 {@link Math} 已提供 IEEE-754 语义一致的等价实现，此处做薄封装以保持
 * C 命名一致性。</p>
 */
public final class MathFuncs {

    private MathFuncs() {
        // 工具类，禁止实例化。
    }

    /**
     * {@code double sqrt(double x)}：平方根。负数返回 NaN、{@code +inf} 返回 +inf、
     * {@code -0.0} 返回 -0.0，与 musl IEEE-754 实现一致。
     *
     * @param x 输入值
     * @return {@code x} 的平方根
     */
    public static double sqrt(double x) {
        return Math.sqrt(x);
    }

    /**
     * {@code double pow(double x, double y)}：幂运算，语义与 musl {@code pow} 一致
     * （含 NaN、无穷、0^0 等特殊值）。
     *
     * @param x 底数
     * @param y 指数
     * @return {@code x} 的 {@code y} 次幂
     */
    public static double pow(double x, double y) {
        return Math.pow(x, y);
    }

    /**
     * {@code double floor(double x)}：向下取整，返回不大于 {@code x} 的最大整数值
     * （仍为 {@code double}）。
     *
     * @param x 输入值
     * @return {@code x} 向下取整
     */
    public static double floor(double x) {
        return Math.floor(x);
    }

    /**
     * {@code double fabs(double x)}：绝对值，等价于清除符号位。{@code -0.0} 返回
     * {@code +0.0}，NaN 返回 NaN，与 musl 位运算实现一致。
     *
     * @param x 输入值
     * @return {@code x} 的绝对值
     */
    public static double fabs(double x) {
        return Math.abs(x);
    }
}