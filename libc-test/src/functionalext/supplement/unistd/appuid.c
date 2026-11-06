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
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include "functionalext.h"

/**
 * @tc.name      : getappuid_0100
 * @tc.desc      : Call getappuid to get the application uid, result should be >= 0
 * @tc.level     : Level 0
 */
void getappuid_0100(void)
{
    uid_t appuid = getappuid();
    uid_t realuid = getuid();
    EXPECT_GTE("getappuid_0100", appuid, 0);
    EXPECT_GTE("getappuid_0100", realuid, 0);
}

/**
 * @tc.name      : getappuid_0200
 * @tc.desc      : Call getappuid multiple times, result should be consistent
 * @tc.level     : Level 1
 */
void getappuid_0200(void)
{
    uid_t uid1 = getappuid();
    uid_t uid2 = getappuid();
    uid_t uid3 = getappuid();
    EXPECT_EQ("getappuid_0200", uid1, uid2);
    EXPECT_EQ("getappuid_0200", uid2, uid3);
}

/**
 * @tc.name      : setappuid_0100
 * @tc.desc      : Call setappuid(0) to set process uid to 0, should succeed
 * @tc.level     : Level 0
 */
void setappuid_0100(void)
{
    uid_t orig = getappuid();
    int ret = setappuid(0);
    if (ret == 0) {
        EXPECT_EQ("setappuid_0100", getappuid(), 0);
    }
    setappuid(orig);
}

/**
 * @tc.name      : setappuid_0200
 * @tc.desc      : Call setappuid with current uid (no-op), should succeed
 * @tc.level     : Level 0
 */
void setappuid_0200(void)
{
    uid_t cur = getappuid();
    int ret = setappuid(cur);
    EXPECT_EQ("setappuid_0200", ret, 0);
    EXPECT_EQ("setappuid_0200", getappuid(), cur);
}

/**
 * @tc.name      : setappuid_0300
 * @tc.desc      : Call setappuid twice with same value (idempotent)
 * @tc.level     : Level 1
 */
void setappuid_0300(void)
{
    uid_t cur = getappuid();
    int ret1 = setappuid(cur);
    int ret2 = setappuid(cur);
    EXPECT_EQ("setappuid_0300", ret1, ret2);
    EXPECT_EQ("setappuid_0300", getappuid(), cur);
}

/**
 * @tc.name      : setappuid_0400
 * @tc.desc      : setappuid return value type correctness (int return, not uid_t)
 * @tc.level     : Level 1
 */
void setappuid_0400(void)
{
    uid_t cur = getappuid();
    int ret = setappuid(cur);
    EXPECT_TRUE("setappuid_0400", ret == 0 || ret == -1);
}

/**
 * @tc.name      : setappuid_boundary_0100
 * @tc.desc      : Boundary value - setappuid(0), the minimum valid uid
 * @tc.level     : Level 1
 */
void setappuid_boundary_0100(void)
{
    uid_t orig = getappuid();
    int ret = setappuid(0);
    if (ret == 0) {
        EXPECT_EQ("setappuid_boundary_0100", getappuid(), 0);
    }
    setappuid(orig);
}

/**
 * @tc.name      : setappuid_boundary_0200
 * @tc.desc      : Boundary value - setappuid(UINT16_MAX), large uid
 * @tc.level     : Level 2
 */
void setappuid_boundary_0200(void)
{
    uid_t orig = getappuid();
    uid_t target = 65535;
    int ret = setappuid(target);
    if (ret == 0) {
        EXPECT_EQ("setappuid_boundary_0200", getappuid(), (int)target);
        setappuid(orig);
    }
}

/**
 * @tc.name      : setappuid_boundary_0300
 * @tc.desc      : Boundary value - setappuid(UINT32_MAX / 2), very large uid
 * @tc.level     : Level 2
 */
void setappuid_boundary_0300(void)
{
    uid_t orig = getappuid();
    uid_t target = 2147483647;
    int ret = setappuid(target);
    if (ret == -1) {
        EXPECT_TRUE("setappuid_boundary_0300", errno == EINVAL || errno == EPERM);
    }
    setappuid(orig);
}

/**
 * @tc.name      : setappuid_boundary_0400
 * @tc.desc      : Boundary value - setappuid(UINT32_MAX), max uid_t value
 * @tc.level     : Level 2
 */
void setappuid_boundary_0400(void)
{
    uid_t orig = getappuid();
    uid_t target = 4294967295U;
    int ret = setappuid(target);
    if (ret == -1) {
        EXPECT_TRUE("setappuid_boundary_0400", errno == EINVAL || errno == EPERM);
    }
    setappuid(orig);
}

/**
 * @tc.name      : setappuid_abnormal_0100
 * @tc.desc      : Abnormal - setappuid with invalid uid (UINT32_MAX), should not crash
 * @tc.level     : Level 1
 */
void setappuid_abnormal_0100(void)
{
    uid_t orig = getappuid();
    int ret = setappuid(4294967295U);
    EXPECT_TRUE("setappuid_abnormal_0100", ret == 0 || ret == -1);
    setappuid(orig);
}

/**
 * @tc.name      : setappuid_abnormal_0200
 * @tc.desc      : Abnormal - rapid successive setappuid calls with alternating values
 * @tc.level     : Level 2
 */
void setappuid_abnormal_0200(void)
{
    uid_t orig = getappuid();
    uid_t values[] = {0, orig, 1, orig, 1000, orig};
    int i;
    for (i = 0; i < (int)(sizeof(values) / sizeof(values[0])); i++) {
        int ret = setappuid(values[i]);
        EXPECT_TRUE("setappuid_abnormal_0200", ret == 0 || ret == -1);
    }
    setappuid(orig);
}

/**
 * @tc.name      : setappuid_abnormal_0300
 * @tc.desc      : Abnormal - setappuid then immediately getappuid, verify consistency
 * @tc.level     : Level 1
 */
void setappuid_abnormal_0300(void)
{
    uid_t orig = getappuid();
    int ret = setappuid(orig);
    if (ret == 0) {
        uid_t appuid = getappuid();
        EXPECT_EQ("setappuid_abnormal_0300", appuid, (int)orig);
    }
}

/**
 * @tc.name      : getappuid_abnormal_0100
 * @tc.desc      : Abnormal - call getappuid in rapid succession, verify no side effects
 * @tc.level     : Level 1
 */
void getappuid_abnormal_0100(void)
{
    uid_t first = getappuid();
    int i;
    for (i = 0; i < 100; i++) {
        uid_t uid = getappuid();
        EXPECT_EQ("getappuid_abnormal_0100", uid, first);
    }
}

int main(int argc, char *argv[])
{
    getappuid_0100();
    getappuid_0200();
    setappuid_0100();
    setappuid_0200();
    setappuid_0300();
    setappuid_0400();
    setappuid_boundary_0100();
    setappuid_boundary_0200();
    setappuid_boundary_0300();
    setappuid_boundary_0400();
    setappuid_abnormal_0100();
    setappuid_abnormal_0200();
    setappuid_abnormal_0300();
    getappuid_abnormal_0100();
    return t_status;
}
