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

/* 本文件仅在 musl_extended_function 使能时注册，验证 factory 回退行为 */
#include "passwd_factory_test_common.h"

static void passwd_query_first_ok_001(void)
{
    static const char *const test = "passwd_query_first_ok_001";
    struct outcome o;

    MAKE_SANDBOX(test, FIX_BOTH);
    o = Run(root, OP_PWUID, NULL, NORMAL_UID);
    if (Prepare(test, &o)) {
        CheckUser(test, &o, NORMAL_UID, "normal");
    }
    o = Run(root, OP_PWNAM, "normal", 0);
    if (Prepare(test, &o)) {
        CheckUser(test, &o, NORMAL_UID, "normal");
    }
    o = Run(root, OP_PWUIDR, NULL, NORMAL_UID);
    if (Prepare(test, &o)) {
        CheckRc(test, &o, 0);
        CheckUser(test, &o, NORMAL_UID, "normal");
    }
    o = Run(root, OP_PWNAMR, "normal", 0);
    if (Prepare(test, &o)) {
        CheckRc(test, &o, 0);
        CheckUser(test, &o, NORMAL_UID, "normal");
    }
    o = Run(root, OP_PWNAM, "fallback", 0);
    if (Prepare(test, &o)) {
        CheckMissing(test, &o, 0);
    }
    SandboxDestroy(root);
}

static void passwd_query_fallback_002(void)
{
    static const char *const test = "passwd_query_fallback_002";
    struct outcome o;

    MAKE_SANDBOX(test, FIX_SECOND_ONLY);
    o = Run(root, OP_PWUID, NULL, FALLBACK_UID);
    if (Prepare(test, &o)) {
        CheckUser(test, &o, FALLBACK_UID, "fallback");
    }
    o = Run(root, OP_PWNAM, "fallback", 0);
    if (Prepare(test, &o)) {
        CheckUser(test, &o, FALLBACK_UID, "fallback");
    }
    o = Run(root, OP_PWUIDR, NULL, FALLBACK_UID);
    if (Prepare(test, &o)) {
        CheckRc(test, &o, 0);
        CheckUser(test, &o, FALLBACK_UID, "fallback");
    }
    o = Run(root, OP_PWNAMR, "fallback", 0);
    if (Prepare(test, &o)) {
        CheckRc(test, &o, 0);
        CheckUser(test, &o, FALLBACK_UID, "fallback");
    }
    o = Run(root, OP_PWUID, NULL, NORMAL_UID);
    if (Prepare(test, &o)) {
        CheckMissing(test, &o, 0);
    }
    SandboxDestroy(root);
}

static void passwd_query_both_fail_003(void)
{
    static const char *const test = "passwd_query_both_fail_003";
    struct outcome o;

    MAKE_SANDBOX(test, FIX_NONE);
    o = Run(root, OP_PWUID, NULL, NORMAL_UID);
    if (Prepare(test, &o)) {
        CheckMissing(test, &o, ENOENT);
    }
    o = Run(root, OP_PWNAM, "normal", 0);
    if (Prepare(test, &o)) {
        CheckMissing(test, &o, ENOENT);
    }
    o = Run(root, OP_PWUIDR, NULL, NORMAL_UID);
    if (Prepare(test, &o)) {
        CheckMissing(test, &o, ENOENT);
        CheckRc(test, &o, ENOENT);
    }
    o = Run(root, OP_PWNAMR, "normal", 0);
    if (Prepare(test, &o)) {
        CheckMissing(test, &o, 0);
        CheckRc(test, &o, ENOENT);
    }
    SandboxDestroy(root);
}

static void passwd_query_errno_restore_004(void)
{
    static const char *const test = "passwd_query_errno_restore_004";
    struct outcome o;

    MAKE_SANDBOX(test, FIX_FIRST_LOOP);
    o = Run(root, OP_PWUID, NULL, NORMAL_UID);
    if (Prepare(test, &o)) {
        CheckMissing(test, &o, ELOOP);
    }
    o = Run(root, OP_PWUIDR, NULL, NORMAL_UID);
    if (Prepare(test, &o)) {
        CheckMissing(test, &o, ELOOP);
        CheckRc(test, &o, ELOOP);
    }
    SandboxDestroy(root);
}

static void passwd_iter_first_ok_005(void)
{
    static const char *const test = "passwd_iter_first_ok_005";
    struct outcome o;

    MAKE_SANDBOX(test, FIX_BOTH);
    o = Run(root, OP_PWENT, NULL, 0);
    if (Prepare(test, &o)) {
        CheckCount(test, &o, 1);
    }
    o = Run(root, OP_PWENTR, NULL, 0);
    if (Prepare(test, &o)) {
        CheckCount(test, &o, 1);
    }
    SandboxDestroy(root);
}

static void passwd_iter_fallback_006(void)
{
    static const char *const test = "passwd_iter_fallback_006";
    struct outcome o;

    MAKE_SANDBOX(test, FIX_SECOND_ONLY);
    o = Run(root, OP_PWENT, NULL, 0);
    if (Prepare(test, &o)) {
        CheckCount(test, &o, 2);
    }
    o = Run(root, OP_PWENTR, NULL, 0);
    if (Prepare(test, &o)) {
        CheckCount(test, &o, 2);
    }
    SandboxDestroy(root);
}

static void passwd_iter_both_fail_007(void)
{
    static const char *const test = "passwd_iter_both_fail_007";
    struct outcome o;

    MAKE_SANDBOX(test, FIX_NONE);
    o = Run(root, OP_PWENT, NULL, 0);
    if (Prepare(test, &o)) {
        CheckCount(test, &o, 0);
        if (o.err != ENOENT) {
            t_error("[%s] expected errno %d, got %d\n", test, ENOENT, o.err);
        }
    }
    o = Run(root, OP_PWENTR, NULL, 0);
    if (Prepare(test, &o)) {
        CheckCount(test, &o, 0);
        CheckRc(test, &o, ENOENT);
    }
    SandboxDestroy(root);
}

static void passwd_iter_errno_restore_008(void)
{
    static const char *const test = "passwd_iter_errno_restore_008";
    struct outcome o;

    MAKE_SANDBOX(test, FIX_FIRST_LOOP);
    o = Run(root, OP_PWENT, NULL, 0);
    if (Prepare(test, &o)) {
        CheckCount(test, &o, 0);
        if (o.err != ELOOP) {
            t_error("[%s] expected errno %d, got %d\n", test, ELOOP, o.err);
        }
    }
    SandboxDestroy(root);
}

static void group_query_first_ok_009(void)
{
    static const char *const test = "group_query_first_ok_009";
    struct outcome o;

    MAKE_SANDBOX(test, FIX_BOTH);
    o = Run(root, OP_GRGID, NULL, NORMAL_GID);
    if (Prepare(test, &o)) {
        CheckGroup(test, &o, NORMAL_GID, "gnormal");
    }
    o = Run(root, OP_GRNAM, "gnormal", 0);
    if (Prepare(test, &o)) {
        CheckGroup(test, &o, NORMAL_GID, "gnormal");
    }
    o = Run(root, OP_GRGIDR, NULL, NORMAL_GID);
    if (Prepare(test, &o)) {
        CheckRc(test, &o, 0);
        CheckGroup(test, &o, NORMAL_GID, "gnormal");
    }
    o = Run(root, OP_GRNAMR, "gnormal", 0);
    if (Prepare(test, &o)) {
        CheckRc(test, &o, 0);
        CheckGroup(test, &o, NORMAL_GID, "gnormal");
    }
    o = Run(root, OP_GRNAM, "gfallback", 0);
    if (Prepare(test, &o)) {
        CheckMissing(test, &o, 0);
    }
    SandboxDestroy(root);
}

static void group_query_fallback_010(void)
{
    static const char *const test = "group_query_fallback_010";
    struct outcome o;

    MAKE_SANDBOX(test, FIX_SECOND_ONLY);
    o = Run(root, OP_GRGID, NULL, FALLBACK_GID);
    if (Prepare(test, &o)) {
        CheckGroup(test, &o, FALLBACK_GID, "gfallback");
    }
    o = Run(root, OP_GRNAM, "gfallback", 0);
    if (Prepare(test, &o)) {
        CheckGroup(test, &o, FALLBACK_GID, "gfallback");
    }
    o = Run(root, OP_GRGIDR, NULL, FALLBACK_GID);
    if (Prepare(test, &o)) {
        CheckRc(test, &o, 0);
        CheckGroup(test, &o, FALLBACK_GID, "gfallback");
    }
    o = Run(root, OP_GRNAMR, "gfallback", 0);
    if (Prepare(test, &o)) {
        CheckRc(test, &o, 0);
        CheckGroup(test, &o, FALLBACK_GID, "gfallback");
    }
    o = Run(root, OP_GRGID, NULL, NORMAL_GID);
    if (Prepare(test, &o)) {
        CheckMissing(test, &o, 0);
    }
    SandboxDestroy(root);
}

static void group_query_both_fail_011(void)
{
    static const char *const test = "group_query_both_fail_011";
    struct outcome o;

    MAKE_SANDBOX(test, FIX_NONE);
    o = Run(root, OP_GRGID, NULL, NORMAL_GID);
    if (Prepare(test, &o)) {
        CheckMissing(test, &o, ENOENT);
    }
    o = Run(root, OP_GRNAM, "gnormal", 0);
    if (Prepare(test, &o)) {
        CheckMissing(test, &o, ENOENT);
    }
    o = Run(root, OP_GRGIDR, NULL, NORMAL_GID);
    if (Prepare(test, &o)) {
        CheckMissing(test, &o, 0);
        CheckRc(test, &o, ENOENT);
    }
    SandboxDestroy(root);
}

static void group_query_errno_restore_012(void)
{
    static const char *const test = "group_query_errno_restore_012";
    struct outcome o;

    MAKE_SANDBOX(test, FIX_FIRST_LOOP);
    o = Run(root, OP_GRGID, NULL, NORMAL_GID);
    if (Prepare(test, &o)) {
        CheckMissing(test, &o, ELOOP);
    }
    o = Run(root, OP_GRGIDR, NULL, NORMAL_GID);
    if (Prepare(test, &o)) {
        CheckMissing(test, &o, ELOOP);
        CheckRc(test, &o, ELOOP);
    }
    SandboxDestroy(root);
}

static void group_iter_first_ok_013(void)
{
    static const char *const test = "group_iter_first_ok_013";
    struct outcome o;

    MAKE_SANDBOX(test, FIX_BOTH);
    o = Run(root, OP_GRENT, NULL, 0);
    if (Prepare(test, &o)) {
        CheckCount(test, &o, 1);
    }
    o = Run(root, OP_GRENTR, NULL, 0);
    if (Prepare(test, &o)) {
        CheckCount(test, &o, 1);
    }
    SandboxDestroy(root);
}

static void group_iter_fallback_014(void)
{
    static const char *const test = "group_iter_fallback_014";
    struct outcome o;

    MAKE_SANDBOX(test, FIX_SECOND_ONLY);
    o = Run(root, OP_GRENT, NULL, 0);
    if (Prepare(test, &o)) {
        CheckCount(test, &o, 1);
    }
    o = Run(root, OP_GRENTR, NULL, 0);
    if (Prepare(test, &o)) {
        CheckCount(test, &o, 1);
    }
    SandboxDestroy(root);
}

static void group_iter_both_fail_015(void)
{
    static const char *const test = "group_iter_both_fail_015";
    struct outcome o;

    MAKE_SANDBOX(test, FIX_NONE);
    o = Run(root, OP_GRENT, NULL, 0);
    if (Prepare(test, &o)) {
        CheckCount(test, &o, 0);
        if (o.err != ENOENT) {
            t_error("[%s] expected errno %d, got %d\n", test, ENOENT, o.err);
        }
    }
    o = Run(root, OP_GRENTR, NULL, 0);
    if (Prepare(test, &o)) {
        CheckCount(test, &o, 0);
        CheckRc(test, &o, ENOENT);
    }
    SandboxDestroy(root);
}

static void group_iter_errno_restore_016(void)
{
    static const char *const test = "group_iter_errno_restore_016";
    struct outcome o;

    MAKE_SANDBOX(test, FIX_FIRST_LOOP);
    o = Run(root, OP_GRENT, NULL, 0);
    if (Prepare(test, &o)) {
        CheckCount(test, &o, 0);
        if (o.err != ELOOP) {
            t_error("[%s] expected errno %d, got %d\n", test, ELOOP, o.err);
        }
    }
    SandboxDestroy(root);
}

static void grouplist_first_ok_017(void)
{
    static const char *const test = "grouplist_first_ok_017";
    struct outcome o;

    MAKE_SANDBOX(test, FIX_BOTH);
    o = Run(root, OP_GROUPLIST, "guser", BASE_GID);
    if (Prepare(test, &o)) {
        CheckList(test, &o, 2, 2, BASE_GID, NORMAL_GID);
    }
    SandboxDestroy(root);
}

static void grouplist_fallback_018(void)
{
    static const char *const test = "grouplist_fallback_018";
    struct outcome o;

    MAKE_SANDBOX(test, FIX_SECOND_ONLY);
    o = Run(root, OP_GROUPLIST, "guser", BASE_GID);
    if (Prepare(test, &o)) {
        CheckList(test, &o, 2, 2, BASE_GID, FALLBACK_GID);
    }
    SandboxDestroy(root);
}

static void grouplist_both_fail_019(void)
{
    static const char *const test = "grouplist_both_fail_019";
    struct outcome o;

    MAKE_SANDBOX(test, FIX_NONE);
    o = Run(root, OP_GROUPLIST, "guser", BASE_GID);
    if (Prepare(test, &o)) {
        CheckList(test, &o, 1, 1, BASE_GID, 0);
    }
    SandboxDestroy(root);
}

static void grouplist_errno_restore_020(void)
{
    static const char *const test = "grouplist_errno_restore_020";
    struct outcome o;

    MAKE_SANDBOX(test, FIX_FIRST_LOOP);
    o = Run(root, OP_GROUPLIST, "guser", BASE_GID);
    if (Prepare(test, &o)) {
        if (o.rc != -1) {
            t_error("[%s] expected rc -1, got %d\n", test, o.rc);
        }
        if (o.err != ELOOP) {
            t_error("[%s] expected errno %d, got %d\n", test, ELOOP, o.err);
        }
    }
    SandboxDestroy(root);
}

static void fallback_stability_021(void)
{
    static const char *const test = "fallback_stability_021";
    struct outcome o;

    MAKE_SANDBOX(test, FIX_SECOND_ONLY);
    o = Run(root, OP_STRESS_FALLBACK, "fallback", FALLBACK_UID);
    if (Prepare(test, &o)) {
        CheckCount(test, &o, STRESS_ROUNDS);
    }
    SandboxDestroy(root);
}

static void normal_stability_022(void)
{
    static const char *const test = "normal_stability_022";
    struct outcome o;

    MAKE_SANDBOX(test, FIX_BOTH);
    o = Run(root, OP_STRESS_FIRST, "normal", NORMAL_UID);
    if (Prepare(test, &o)) {
        CheckCount(test, &o, STRESS_ROUNDS);
    }
    SandboxDestroy(root);
}

int main(void)
{
    passwd_query_first_ok_001();
    passwd_query_fallback_002();
    passwd_query_both_fail_003();
    passwd_query_errno_restore_004();
    passwd_iter_first_ok_005();
    passwd_iter_fallback_006();
    passwd_iter_both_fail_007();
    passwd_iter_errno_restore_008();
    group_query_first_ok_009();
    group_query_fallback_010();
    group_query_both_fail_011();
    group_query_errno_restore_012();
    group_iter_first_ok_013();
    group_iter_fallback_014();
    group_iter_both_fail_015();
    group_iter_errno_restore_016();
    grouplist_first_ok_017();
    grouplist_fallback_018();
    grouplist_both_fail_019();
    grouplist_errno_restore_020();
    fallback_stability_021();
    normal_stability_022();

    if (g_env_unsupported) {
        (void)printf("WARNING: chroot is not permitted, fallback cases were skipped\n");
    }
    return t_status;
}
