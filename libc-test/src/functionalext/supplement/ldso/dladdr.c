/**
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

#include "functionalext.h"

void test1()
{}

/**
 * @tc.name      : dladdr_0100
 * @tc.desc      : Each parameter is valid, you can get the module, name and address where a function is located.
 * @tc.level     : Level 0
 */
void dladdr_0100(void)
{
    Dl_info info;
    info.dli_fname = 0;
    int num = 0;
    int ret = dladdr((void *)test1, &info);
    EXPECT_EQ("dladdr_0100", ret, 1);
    char *name = "dladdr";
    if (strstr(info.dli_fname, name)) {
        num = 1;
    } else {
        num = 2;
    }
    EXPECT_EQ("dladdr_0100", num, 1);
}

/**
 * @tc.name      : dladdr_0200
 * @tc.desc      : Each parameter is invalid, you can get the module, name and address where a function is located.
 * @tc.level     : Level 2
 */
void dladdr_0200(void)
{
    Dl_info info;
    info.dli_fname = 0;
    int num = 0;
    int ret = dladdr(NULL, &info);
    EXPECT_EQ("dladdr_0100", ret, 0);
    if (info.dli_fname == NULL) {
        num = 1;
    } else {
        num = 2;
    }
    EXPECT_EQ("dladdr_0200", num, 1);
}

int main()
{
    dladdr_0100();
    dladdr_0200();

    return t_status;
}
