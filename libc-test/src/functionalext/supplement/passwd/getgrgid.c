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

#include <ctype.h>
#include <fcntl.h>
#include <grp.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>
#include "functionalext.h"

/**
 * @tc.name      : mprotect_0100
 * @tc.desc      : Verify that the specified GID can be retrieved from the group file (parameter valid)
 * @tc.level     : Level 0
 */
void getgrgid_0100(void)
{
    system("ps -eo command,gid | grep -E \"GID|getgrgid\" > ps.txt");
    char abc[256] = {0};
    bool successflag = false;
    FILE *fptr = fopen("ps.txt", "r");
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
            struct group *gid;
            gid = getgrgid(intgid);
            if (gid->gr_gid == intgid) {
                successflag = true;
            }
        }
    }
    EXPECT_TRUE("getgrgid_0100", successflag);
    fclose(fptr);
    remove("ps.txt");
}

/**
 * @tc.name      : getgrgid_0200
 * @tc.desc      : Verify that data with the specified GID cannot be retrieved from the group file
 *                 (gid argument is invalid)
 * @tc.level     : Level 2
 */
void getgrgid_0200(void)
{
    struct group *data;
    data = getgrgid(-1);
    EXPECT_EQ("getgrgid_0200", data, 0);
}

int main()
{
    getgrgid_0100();
    getgrgid_0200();
    return t_status;
}