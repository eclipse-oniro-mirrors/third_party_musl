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

/**
 * Java 版本的 {@code <strings.h>} 位操作纯函数接口。
 *
 * <p>对应 musl {@code src/misc/} 目录下、无法归入 string/ctype/stdlib/math 的零散函数。</p>
 */
public final class MiscFuncs {

    private MiscFuncs() {
        // 工具类，禁止实例化。
    }

    /**
     * {@code int ffs(int i)}（strings.h）：返回 {@code i} 的最低有效置位位的位置
     * （从 1 开始编号）；若 {@code i == 0} 则返回 0。
     *
     * <p>musl 实现 {@code i ? a_ctz_l(i) + 1 : 0}，等价于
     * {@code Integer.numberOfTrailingZeros(i) + 1}。</p>
     *
     * @param i 输入整数
     * @return 最低置位位的位置（1..32），无置位位返回 0
     */
    public static int ffs(int i) {
        return i != 0 ? Integer.numberOfTrailingZeros(i) + 1 : 0;
    }
}
