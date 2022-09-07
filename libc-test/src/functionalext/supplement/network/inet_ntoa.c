/*
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

#include <arpa/inet.h>
#include <ctype.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include "functionalext.h"

typedef void (*TEST_FUN)();

/**
 * @tc.name      : inet_ntoa_0100
 * @tc.desc      : Validation converts a network binary number to an IP address string
 * @tc.level     : Level 0
 */
void inet_ntoa_0100()
{
    char buff[] = "127.0.0.1";
    struct in_addr in;
    char *result;
    inet_aton(buff, &in);
    result = inet_ntoa(in);
    EXPECT_STREQ("inet_ntoa_0100", result, buff);
}

/**
 * @tc.name      : inet_ntoa_0200
 * @tc.desc      : Validation cannot convert a network binary number to an IP address string
 *                 (the in argument is invalid)
 * @tc.level     : Level 2
 */
void inet_ntoa_0200()
{
    struct in_addr in;
    in.s_addr = 0;
    char *result;
    result = inet_ntoa(in);
    EXPECT_STREQ("inet_ntoa_0200", result, "0.0.0.0");
}

TEST_FUN G_Fun_Array[] = {
    inet_ntoa_0100,
    inet_ntoa_0200,
};

int main()
{
    int num = sizeof(G_Fun_Array) / sizeof(TEST_FUN);
    for (int pos = 0; pos < num; ++pos) {
        G_Fun_Array[pos]();
    }

    return t_status;
}