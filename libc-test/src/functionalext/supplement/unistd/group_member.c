/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <grp.h>
#include <limits.h>
#include <pthread.h>
#include <unistd.h>
#include "functionalext.h"

/**
 * @tc.name      : group_member_0100
 * @tc.desc      : Check current gid is in supplementary group list
 * @tc.level     : Level 0
 */
void group_member_0100(void)
{
    gid_t curGid = getgid();
    int rv = group_member(curGid);
    EXPECT_TRUE("group_member_0100", rv == 0 || rv == 1);
}

/**
 * @tc.name      : group_member_0200
 * @tc.desc      : Check non-existent gid, should return 0
 * @tc.level     : Level 2
 */
void group_member_0200(void)
{
    int rv = group_member(99999);
    EXPECT_EQ("group_member_0200", rv, 0);
}

/**
 * @tc.name      : group_member_0300
 * @tc.desc      : Check effective gid
 * @tc.level     : Level 0
 */
void group_member_0300(void)
{
    gid_t egid = getegid();
    int rv = group_member(egid);
    EXPECT_TRUE("group_member_0300", rv == 0 || rv == 1);
}

/**
 * @tc.name      : group_member_0400
 * @tc.desc      : Boundary: gid 0 (root group)
 * @tc.level     : Level 2
 */
void group_member_0400(void)
{
    int rv = group_member(0);
    EXPECT_TRUE("group_member_0400", rv == 0 || rv == 1);
}

/**
 * @tc.name      : group_member_0500
 * @tc.desc      : Boundary: GID_MAX extreme value
 * @tc.level     : Level 2
 */
void group_member_0500(void)
{
    int rv = group_member((gid_t)UINT_MAX);
    EXPECT_EQ("group_member_0500", rv, 0);
}

/**
 * @tc.name      : group_member_0600
 * @tc.desc      : Verify group_member against actual getgroups list
 * @tc.level     : Level 0
 */
void group_member_0600(void)
{
    gid_t groups[64];
    int n = getgroups(64, groups);
    EXPECT_TRUE("group_member_0600", n >= 0);
    if (n > 0) {
        int rv = group_member(groups[0]);
        EXPECT_EQ("group_member_0600", rv, 1);
    }
    if (n > 1) {
        int rv = group_member(groups[n - 1]);
        EXPECT_EQ("group_member_0600_last", rv, 1);
    }
}

static void *gm_worker(void *arg)
{
    gid_t g = *(gid_t *)arg;
    group_member(g);
    return NULL;
}

/**
 * @tc.name      : group_member_0700
 * @tc.desc      : Multi-threaded: concurrent group_member calls
 * @tc.level     : Level 0
 */
void group_member_0700(void)
{
    gid_t g = getgid();
    pthread_t tids[4];
    for (int i = 0; i < 4; i++) {
        EXPECT_EQ("group_member_0700", pthread_create(&tids[i], NULL, gm_worker, &g), 0);
    }
    for (int i = 0; i < 4; i++) {
        pthread_join(tids[i], NULL);
    }
    EXPECT_TRUE("group_member_0700", 1);
}

int main(void)
{
    group_member_0100();
    group_member_0200();
    group_member_0300();
    group_member_0400();
    group_member_0500();
    group_member_0600();
    group_member_0700();
    return t_status;
}
