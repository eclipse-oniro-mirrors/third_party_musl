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

#include <fcntl.h>
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include "functionalext.h"

/**
 * @tc.name      : getgrnam_r_0100
 * @tc.desc      : Verify that the specified group data can be obtained from the group file (parameters are valid)
 * @tc.level     : Level 0
 */
void getgrnam_r_0100(void)
{
    system("ps -eo command,gid | grep -E \"GID|getgrnam_r\" > "
    "/data/tests/libc-test/src/functionalext/supplement/passwd/pss.txt");
    char abc[256] = {0};
    bool successflag = false;
    FILE *fptr = fopen("/data/tests/libc-test/src/functionalext/supplement/passwd/pss.txt", "r");
    if (fptr) {
        while (!feof(fptr)) {
            fread(abc, sizeof(abc), 1, fptr);
            char num[8] = {0};
            int index = 0;
            for (int i = 0; i < (int)strlen(abc); i++) {
                if (abc[i] >= '0' && abc[i] <= '9') {
                    num[index++] += abc[i];
                }
            }
            num[index] = '\0';
            gid_t intgid = atoi(num);
            int data;
            short int lp;
            struct group grp;
            struct group *grpptr = &grp;
            struct group *tempGrpPtr;
            char grpbuffer[200];
            int grplinelen = sizeof(grpbuffer);
            data = getgrnam_r("root", grpptr, grpbuffer, grplinelen, &tempGrpPtr);
            if (data == intgid) {
                successflag = true;
            }
        }
    }
    EXPECT_TRUE("getgrgid_r_0100", successflag);
    fclose(fptr);
    remove("/data/tests/libc-test/src/functionalext/supplement/passwd/pss.txt");
}

/**
 * @tc.name      : getgrnam_r_0200
 * @tc.desc      : Verify that the specified group data can be obtained from the group file (parameter invalid)
 * @tc.level     : Level 2
 */
void getgrnam_r_0200(void)
{
    int data;
    struct group grp;
    struct group *grpptr = &grp;
    struct group *tempGrpPtr;
    char grpbuffer[200];
    int grplinelen = sizeof(grpbuffer);
    data = getgrnam_r("root", grpptr, grpbuffer, -1, &tempGrpPtr);
    EXPECT_EQ("getgrnam_r_0200", data, 0);
}

int main()
{
    getgrnam_r_0100();
    getgrnam_r_0200();
    return t_status;
}