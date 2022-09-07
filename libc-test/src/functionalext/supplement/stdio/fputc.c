/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "functionalext.h"

/**
 * @tc.name       : fputc_0100
 * @tc.desc       : Verify that a character can be written to the file
 * @tc.level      : level 0.
 */
void fputc_0100(void)
{
    FILE *fptr = NULL;
    fptr = fopen("/data/tests/libc-test/src/functionalext/supplement/stdio/fputc.txt", "w");
    int Exit = fputc('a', fptr);
    EXPECT_EQ("fputc_0100", Exit, 97);
    fclose(fptr);
    fptr = NULL;
}

/**
 * @tc.name       : fputc_0200
 * @tc.desc       : Verify that a character cannot be written to the file
 * @tc.level      : level 2.
 */
void fputc_0200(void)
{
    FILE *fptr = NULL;
    fptr = fopen("/data/tests/libc-test/src/functionalext/supplement/stdio/fputc.txt", "r");
    int Exit = fputc('a', fptr);
    EXPECT_EQ("fputc_0200", Exit, EOF);
    fclose(fptr);
    remove("/data/tests/libc-test/src/functionalext/supplement/stdio/fputc.txt");
    fptr = NULL;
}

int main(void)
{
    fputc_0100();
    fputc_0200();
    return t_status;
}