/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <stdio.h>
#include <stdio_ext.h>
#include "functionalext.h"

/**
 * @tc.name      : __freadable_0100
 * @tc.desc      : The parameters are valid, and the file can be judged to be readable.
 * @tc.level     : Level 0
 */
void __freadable_0100(void)
{
    char buf[100];
    FILE *fp = fopen("freadable.txt", "w");
    fclose(fp);
    FILE *ffp = fopen("freadable.txt", "r");
    size_t ret = __freadable(ffp);
    EXPECT_NE("__freadable_0100", ret, 0);
    fclose(ffp);
    remove("freadable.txt");
}

/**
 * @tc.name      : __freadable_0200
 * @tc.desc      : The parameters are valid, and the file can be judged to be unreadable.
 * @tc.level     : Level 1
 */
void __freadable_0200(void)
{
    FILE *fp = fopen("freadable.txt", "wb");
    size_t ret = __freadable(fp);
    EXPECT_EQ("__freadable_0200", ret, 0);
    fclose(fp);
    remove("freadable.txt");
}

int main(int argc, char *argv[])
{
    __freadable_0100();
    __freadable_0200();
    return t_status;
}