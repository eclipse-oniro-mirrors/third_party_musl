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

#include <fcntl.h>
#include <unistd.h>
#include "functionalext.h"

/**
 * @tc.name      : acct_0100
 * @tc.desc      : Verify that the recording function is enabled (parameters are valid)
 * @tc.level     : Level 0
 */
void acct_0100(void)
{
    errno = 0;
    const char *filePath = "/data/tests/libc-test/src/functionalext/supplement/unistd/accttest.txt";
    int fd = open(filePath, O_RDWR | O_CREAT, 0777);
    if (fd < 0) {
        t_error("%s open failed\n", __func__);
    }
    int result = acct(filePath);
    if (result != 0) {
        t_error("%s result is %d, except zero\n", __func__, result);
    }
}

/**
 * @tc.name      : acct_0200
 * @tc.desc      : Verify logging is disabled (parameter is NULL)
 * @tc.level     : Level 2
 */
void acct_0200(void)
{
    int result = acct(NULL);
    EXPECT_EQ("acct_0200", result, -1);
}

/**
 * @tc.name      : acct_0300
 * @tc.desc      : Verify logging is disabled (parameter is file does not exist)
 * @tc.level     : Level 2
 */
void acct_0300(void)
{
    const char *filePath = "/data/tests/libc-test/src/functionalext/supplement/unistd/accttest.txt";
    FILE *fptr = fopen(filePath, "w");
    fclose(fptr);
    int result = acct(filePath);
    EXPECT_EQ("acct_0300", result, -1);
    remove(filePath);
}

int main(void)
{
    // acct_0100();
    acct_0200();
    acct_0300();
    return t_status;
}