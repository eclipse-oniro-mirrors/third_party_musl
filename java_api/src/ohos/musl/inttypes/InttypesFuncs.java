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

import ohos.musl.stdlib.StdlibFuncs;

/**
 * Java 版本的 {@code <inttypes.h>} 纯函数接口（对齐 musl {@code src/inttypes/}）。
 *
 * <p>{@code intmax_t} / {@code uintmax_t} 为 64 位；C 的 {@code long long} 与本项目
 * 目标 {@code long} 同为 64 位，故 {@code imax*} 与 {@code strto*max} 复用
 * {@code StdlibFuncs} 的等价实现。返回指针映射为下标、无符号以位模式表达等约定与其它
 * 类型一致。</p>
 */
public final class InttypesFuncs {

    private InttypesFuncs() {
        // 工具类，禁止实例化。
    }

    /**
     * {@code intmax_t imaxabs(intmax_t a)}：返回 {@code a} 的绝对值，复用
     * {@link StdlibFuncs#labs(long)}。{@code Long.MIN_VALUE} 的绝对值仍为
     * {@code Long.MIN_VALUE}（C 为未定义行为，Java 为约定的溢出回绕）。
     *
     * @param a 输入整数
     * @return {@code a} 的绝对值
     */
    public static long imaxabs(long a) {
        return StdlibFuncs.labs(a);
    }

    /**
     * {@code imaxdiv_t imaxdiv(intmax_t numer, intmax_t denom)}：整数除法，复用
     * {@link StdlibFuncs#ldiv(long, long)}，返回其 {@link StdlibFuncs.Ldiv}
     * （C 的 {@code imaxdiv_t} 结构体映射）。
     *
     * @param numer 被除数
     * @param denom 除数（不能为 0）
     * @return 商与余数
     */
    public static StdlibFuncs.Ldiv imaxdiv(long numer, long denom) {
        return StdlibFuncs.ldiv(numer, denom);
    }

    /**
     * {@code intmax_t strtoimax(const char *s, char **end, int base)}：按 {@code base}
     * 进制解析整数，复用 {@link StdlibFuncs#strtol(String, int)}（省略 end 指针）。
     *
     * @param s 待解析字符串
     * @param base 进制（0 表示自动检测，或 2..36）
     * @return 解析得到的 {@code long}
     */
    public static long strtoimax(String s, int base) {
        return StdlibFuncs.strtol(s, base);
    }

    /**
     * {@code uintmax_t strtoumax(const char *s, char **end, int base)}：按 {@code base}
     * 进制解析无符号整数，复用 {@link StdlibFuncs#strtoul(String, int)}（无符号值以
     * {@code long} 位模式表示，溢出钳制 {@code UINTMAX_MAX} 即 {@code -1L}）。
     *
     * @param s 待解析字符串
     * @param base 进制（0 表示自动检测，或 2..36）
     * @return 解析得到的无符号值的 64 位位模式
     */
    public static long strtoumax(String s, int base) {
        return StdlibFuncs.strtoul(s, base);
    }
}
