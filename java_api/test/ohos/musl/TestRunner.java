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

import ohos.musl.ctype.CtypeFuncsTest;
import ohos.musl.inttypes.InttypesFuncsTest;
import ohos.musl.math.MathFuncsTest;
import ohos.musl.misc.MiscFuncsTest;
import ohos.musl.regex.RegexFuncsTest;
import ohos.musl.stdlib.StdlibFuncsTest;
import ohos.musl.string.StringFuncsTest;
import ohos.musl.wchar.WcharFuncsTest;
import ohos.musl.wctype.WctypeFuncsTest;

/**
 * 测试套件入口：依次运行各测试类并汇总结果，失败时以非零码退出。
 *
 * <pre>
 *   cd java_api
 *   javac -d out $(find src test -name '*.java')
 *   java -cp out ohos.musl.TestRunner
 * </pre>
 */
public final class TestRunner {

    private TestRunner() {
    }

    public static void main(String[] args) {
        int total = 0;
        total += StringFuncsTest.run();
        total += CtypeFuncsTest.run();
        total += MiscFuncsTest.run();
        total += RegexFuncsTest.run();
        total += StdlibFuncsTest.run();
        total += MathFuncsTest.run();
        total += WcharFuncsTest.run();
        total += InttypesFuncsTest.run();
        total += WctypeFuncsTest.run();

        System.out.println();
        if (total == 0) {
            System.out.println("ALL TESTS PASSED");
        } else {
            System.out.println("TOTAL FAILURES: " + total);
            System.exit(1);
        }
    }
}