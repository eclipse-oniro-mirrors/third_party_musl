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

#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "test.h"

/**
 * @tc.name      : fchdir_0100
 * @tc.desc      : Change the current working directory of the calling process to the directory pointed to fd
 * @tc.level     : Level 0
 */
void fchdir_0100(void)
{
    char buf[30];

    int fd = open("/data", O_RDONLY);
    if (fd < 0) {
        t_error("%s open failed\n", __func__);
    }

    int result = fchdir(fd);
    if (result) {
        t_error("%s fchdir failed, result is %d\n", __func__, result);
    }

    memset(buf, 0, sizeof(buf));

    char *ret = getcwd(buf, sizeof(buf));
    if (!ret) {
        t_error("%s getcwd failed\n", __func__);
    }

    result = strcmp("/data", buf);
    if (result) {
        t_error("%s fchdir failed, buf is %s\n", __func__, buf);
    }

    close(fd);
}

int main(int argc, char *argv[])
{
    fchdir_0100();
    return t_status;
}
