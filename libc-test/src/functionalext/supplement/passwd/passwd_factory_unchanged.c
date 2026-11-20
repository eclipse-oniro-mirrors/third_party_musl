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

/* 本文件仅在 musl_extended_function 未使能时注册，验证行为与改动前一致 */
#include "passwd_factory_test_common.h"

/* 第一路径成功：正常路径行为不变，且不读取 factory 文件 */
static void passwd_query_first_ok_101(void)
{
    static const char *const test = "passwd_query_first_ok_101";
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

/* 第一路径失败、factory 存在：不得回退，按第一路径错误失败 */
static void passwd_query_no_fallback_102(void)
{
    static const char *const test = "passwd_query_no_fallback_102";
    struct outcome o;

    MAKE_SANDBOX(test, FIX_SECOND_ONLY);
    o = Run(root, OP_PWUID, NULL, FALLBACK_UID);
    if (Prepare(test, &o)) {
        CheckMissing(test, &o, ENOENT);
    }
    o = Run(root, OP_PWNAM, "fallback", 0);
    if (Prepare(test, &o)) {
        CheckMissing(test, &o, ENOENT);
    }
    o = Run(root, OP_PWUIDR, NULL, FALLBACK_UID);
    if (Prepare(test, &o)) {
        CheckMissing(test, &o, ENOENT);
        CheckRc(test, &o, ENOENT);
    }
    o = Run(root, OP_PWNAMR, "fallback", 0);
    if (Prepare(test, &o)) {
        CheckMissing(test, &o, 0);
        CheckRc(test, &o, ENOENT);
    }
    SandboxDestroy(root);
}

static void passwd_query_both_fail_103(void)
{
    static const char *const test = "passwd_query_both_fail_103";
    struct outcome o;

    MAKE_SANDBOX(test, FIX_NONE);
    o = Run(root, OP_PWUID, NULL, NORMAL_UID);
    if (Prepare(test, &o)) {
        CheckMissing(test, &o, ENOENT);
    }
    o = Run(root, OP_PWUIDR, NULL, NORMAL_UID);
    if (Prepare(test, &o)) {
        CheckMissing(test, &o, ENOENT);
        CheckRc(test, &o, ENOENT);
    }
    SandboxDestroy(root);
}

/* 第一路径以 ELOOP 失败：错误码与改动前一致 */
static void passwd_query_errno_restore_104(void)
{
    static const char *const test = "passwd_query_errno_restore_104";
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

static void passwd_iter_first_ok_105(void)
{
    static const char *const test = "passwd_iter_first_ok_105";
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

/* 第一路径失败、factory 存在：迭代不得回退 */
static void passwd_iter_no_fallback_106(void)
{
    static const char *const test = "passwd_iter_no_fallback_106";
    struct outcome o;

    MAKE_SANDBOX(test, FIX_SECOND_ONLY);
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

static void passwd_iter_both_fail_107(void)
{
    static const char *const test = "passwd_iter_both_fail_107";
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

static void passwd_iter_errno_restore_108(void)
{
    static const char *const test = "passwd_iter_errno_restore_108";
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

static void group_query_first_ok_109(void)
{
    static const char *const test = "group_query_first_ok_109";
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

static void group_query_no_fallback_110(void)
{
    static const char *const test = "group_query_no_fallback_110";
    struct outcome o;

    MAKE_SANDBOX(test, FIX_SECOND_ONLY);
    o = Run(root, OP_GRGID, NULL, FALLBACK_GID);
    if (Prepare(test, &o)) {
        CheckMissing(test, &o, ENOENT);
    }
    o = Run(root, OP_GRNAM, "gfallback", 0);
    if (Prepare(test, &o)) {
        CheckMissing(test, &o, ENOENT);
    }
    o = Run(root, OP_GRGIDR, NULL, FALLBACK_GID);
    if (Prepare(test, &o)) {
        CheckMissing(test, &o, ENOENT);
        CheckRc(test, &o, ENOENT);
    }
    o = Run(root, OP_GRNAMR, "gfallback", 0);
    if (Prepare(test, &o)) {
        CheckMissing(test, &o, 0);
        CheckRc(test, &o, ENOENT);
    }
    SandboxDestroy(root);
}

static void group_query_both_fail_111(void)
{
    static const char *const test = "group_query_both_fail_111";
    struct outcome o;

    MAKE_SANDBOX(test, FIX_NONE);
    o = Run(root, OP_GRGID, NULL, NORMAL_GID);
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

static void group_query_errno_restore_112(void)
{
    static const char *const test = "group_query_errno_restore_112";
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

static void group_iter_first_ok_113(void)
{
    static const char *const test = "group_iter_first_ok_113";
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

static void group_iter_no_fallback_114(void)
{
    static const char *const test = "group_iter_no_fallback_114";
    struct outcome o;

    MAKE_SANDBOX(test, FIX_SECOND_ONLY);
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

static void group_iter_both_fail_115(void)
{
    static const char *const test = "group_iter_both_fail_115";
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

static void group_iter_errno_restore_116(void)
{
    static const char *const test = "group_iter_errno_restore_116";
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

static void grouplist_first_ok_117(void)
{
    static const char *const test = "grouplist_first_ok_117";
    struct outcome o;

    MAKE_SANDBOX(test, FIX_BOTH);
    o = Run(root, OP_GROUPLIST, "guser", BASE_GID);
    if (Prepare(test, &o)) {
        CheckList(test, &o, 2, 2, BASE_GID, NORMAL_GID);
    }
    SandboxDestroy(root);
}

/* 第一路径失败、factory 存在：只返回基础 gid，factory 组不得参与 */
static void grouplist_no_fallback_118(void)
{
    static const char *const test = "grouplist_no_fallback_118";
    struct outcome o;

    MAKE_SANDBOX(test, FIX_SECOND_ONLY);
    o = Run(root, OP_GROUPLIST, "guser", BASE_GID);
    if (Prepare(test, &o)) {
        CheckList(test, &o, 1, 1, BASE_GID, 0);
    }
    SandboxDestroy(root);
}

static void grouplist_both_fail_119(void)
{
    static const char *const test = "grouplist_both_fail_119";
    struct outcome o;

    MAKE_SANDBOX(test, FIX_NONE);
    o = Run(root, OP_GROUPLIST, "guser", BASE_GID);
    if (Prepare(test, &o)) {
        CheckList(test, &o, 1, 1, BASE_GID, 0);
    }
    SandboxDestroy(root);
}

static void grouplist_errno_restore_120(void)
{
    static const char *const test = "grouplist_errno_restore_120";
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

/* 稳定性压测：factory 存在时 2000 轮查询/迭代持续失败，不崩溃、不泄漏 */
static void no_fallback_stability_121(void)
{
    static const char *const test = "no_fallback_stability_121";
    struct outcome o;

    MAKE_SANDBOX(test, FIX_SECOND_ONLY);
    o = Run(root, OP_STRESS_DISABLED, NULL, 0);
    if (Prepare(test, &o)) {
        CheckCount(test, &o, STRESS_ROUNDS);
    }
    SandboxDestroy(root);
}

/* 稳定性压测：第一路径可用时 2000 轮查询/迭代持续正确 */
static void normal_stability_122(void)
{
    static const char *const test = "normal_stability_122";
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
    passwd_query_first_ok_101();
    passwd_query_no_fallback_102();
    passwd_query_both_fail_103();
    passwd_query_errno_restore_104();
    passwd_iter_first_ok_105();
    passwd_iter_no_fallback_106();
    passwd_iter_both_fail_107();
    passwd_iter_errno_restore_108();
    group_query_first_ok_109();
    group_query_no_fallback_110();
    group_query_both_fail_111();
    group_query_errno_restore_112();
    group_iter_first_ok_113();
    group_iter_no_fallback_114();
    group_iter_both_fail_115();
    group_iter_errno_restore_116();
    grouplist_first_ok_117();
    grouplist_no_fallback_118();
    grouplist_both_fail_119();
    grouplist_errno_restore_120();
    no_fallback_stability_121();
    normal_stability_122();

    if (g_env_unsupported) {
        (void)printf("WARNING: chroot is not permitted, cases were skipped\n");
    }
    return t_status;
}
