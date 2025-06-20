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

#include <stdlib.h>
#include "functionalext.h"

#define SUFFIX_LEN 5

/**
 * @tc.name      : mkstemps_0100
 * @tc.desc      : Provide the correct template, create a temporary file
 * @tc.level     : Level 0
 */
void mkstemps_0100(void)
{
    char tmpfile[] = "/data/local/tmp/mkstemps_0100_XXXXXX.dat";
    int fd = mkstemps(tmpfile, strlen(".dat"));
    EXPECT_TRUE("mkstemps_0100", fd != -1);
    if (fd != -1) {
        int cnt = write(fd, tmpfile, strlen(tmpfile));
        EXPECT_TRUE("mkstemps_0100", cnt == strlen(tmpfile));
        close(fd);
        char rmfile[128];
        int len = sprintf(rmfile, "rm %s", tmpfile);
        if (len > 0) {
            system(rmfile);
        }
    }
}

/**
 * @tc.name      : mkstemps_0200
 * @tc.desc      : Provide the correct template and no fixed suffix, create a temporary file
 * @tc.level     : Level 0
 */
void mkstemps_0200(void)
{
    char tmpfile[] = "/data/local/tmp/mkstemps_0200_XXXXXX";
    int fd = mkstemps(tmpfile, 0);
    EXPECT_TRUE("mkstemps_0200", fd != -1);
    if (fd != -1) {
        int cnt = write(fd, tmpfile, strlen(tmpfile));
        EXPECT_TRUE("mkstemps_0200", cnt == strlen(tmpfile));
        close(fd);
        char rmfile[128];
        int len = sprintf(rmfile, "rm %s", tmpfile);
        if (len > 0) {
            system(rmfile);
        }
    }
}

/**
 * @tc.name      : mkstemps_0300
 * @tc.desc      : Provide error template, create temp file
 * @tc.level     : Level 2
 */
void mkstemps_0300(void)
{
    char tmpfile[] = "/data/local/tmp/mkstemps_0300.dat";
    int fd = mkstemps(tmpfile, strlen(".dat"));
    EXPECT_TRUE("mkstemps_0300", fd == -1);
    if (fd != -1) {
        int cnt = write(fd, tmpfile, strlen(tmpfile));
        EXPECT_TRUE("mkstemps_0300", cnt == strlen(tmpfile));
        close(fd);
        char rmfile[128];
        int len = sprintf(rmfile, "rm %s", tmpfile);
        if (len > 0) {
            system(rmfile);
        }
    }
}

/**
 * @tc.name      : mkstemps_0400
 * @tc.desc      : Template with 'XXXXXX' not at the end
 * @tc.level     : Level 2
 */
void mkstemps_0400(void)
{
    char tmpfile[] = "/data/mkstemps_0400_XXXXXX_dir";
    int fd = mkstemps(tmpfile, 0);
    EXPECT_TRUE("mkstemps_0400", fd == -1);
    if (fd == -1) {
        EXPECT_EQ("mkstemps_0400", errno, EINVAL);
        EXPECT_STREQ("mkstemps_0400", tmpfile, "/data/mkstemps_0400_XXXXXX_dir");
    }
}

/**
 * @tc.name      : mkstemps_0500
 * @tc.desc      : Verify with suffix length larger than actual suffix
 * @tc.level     : Level 2
 */
void mkstemps_0500(void)
{
    char tmpfile[] = "/data/local/tmp/mkstemps_0500_XXXXXX.dat";
    int fd = mkstemps(tmpfile, strlen(".dat") + SUFFIX_LEN); // Exceeding the actual suffix length
    EXPECT_TRUE("mkstemps_0500", fd == -1);
    if (fd == -1) {
        EXPECT_EQ("mkstemps_0500", errno, EINVAL);
    }
}

int main(void)
{
    mkstemps_0100();
    mkstemps_0200();
    mkstemps_0300();
    mkstemps_0400();
    mkstemps_0500();
    return t_status;
}