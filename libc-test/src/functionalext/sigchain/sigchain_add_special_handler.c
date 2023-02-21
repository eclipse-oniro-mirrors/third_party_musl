/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <sigchain.h>
#include <wchar.h>
#include <stdlib.h>
#include "test.h"
#include "functionalext.h"
#include "sigchain_util.h"

static int g_count = 0;
/**
 * @brief the special handler
 */
static bool sigchain_special_sigabrt_handler1(int signo, siginfo_t *siginfo, void *ucontext_raw)
{
    g_count++;
    EXPECT_EQ("sigchain_add_special_handler_001", signo, SIGABRT);
    return false;
}

/**
 * @brief the special handler
 */
static bool sigchain_special_sigabrt_handler2(int signo, siginfo_t *siginfo, void *ucontext_raw)
{
    g_count++;
    EXPECT_EQ("sigchain_add_special_handler_001", signo, SIGABRT);
    return true;
}

/**
 * @tc.name      : sigchain_add_special_handler_001
 * @tc.desc      : The signal are not registered with the kernel, call add_special_signal_handler to  add
 *                 two special handlers
 * @tc.level     : Level 0
 */
static void sigchain_add_special_handler_001()
{
    struct signal_chain_action sigabrt = {
        .sca_sigaction = sigchain_special_sigabrt_handler1,
        .sca_mask = {},
        .sca_flags = 0,
    };
    add_special_signal_handler(SIGABRT, &sigabrt);

    struct signal_chain_action sigabrt1 = {
        .sca_sigaction = sigchain_special_sigabrt_handler2,
        .sca_mask = {},
        .sca_flags = SIGCHAIN_ALLOW_NORETURN,
    };
    add_special_signal_handler(SIGABRT, &sigabrt1);

    if (get_sigchain_mask_enable()) {
        sigset_t set = {0};
        int signo[SIGCHIAN_TEST_SIGNAL_NUM_1] = {SIGABRT};
        SIGCHAIN_TEST_SET_MASK(set, "sigchain_add_special_handler_001", signo, SIGCHIAN_TEST_SIGNAL_NUM_1);
    }

    raise(SIGABRT);
    EXPECT_EQ("sigchain_add_special_handler_001", g_count, SIGCHIAN_TEST_SIGNAL_NUM_2);
}

/**
 * @brief the special handler
 */
static bool sigchain_special_sighup_handler1(int signo, siginfo_t *siginfo, void *ucontext_raw)
{
    g_count++;
    EXPECT_EQ("sigchain_add_special_handler_002", signo, SIGHUP);
    return false;
}

/**
 * @brief the special handler
 */
static bool sigchain_special_sighup_handler2(int signo, siginfo_t *siginfo, void *ucontext_raw)
{
    g_count++;
    EXPECT_EQ("sigchain_add_special_handler_002", signo, SIGHUP);
    return false;
}

/**
 * @brief the signal handler
 */
static void signal_sighup_handler(int signo)
{
    g_count++;
    EXPECT_EQ("sigchain_add_special_handler_002", signo, SIGHUP);
}

/**
 * @tc.name      : sigchain_add_special_handler_002
 * @tc.desc      : The signal are registered with the kernel(Using signal), call
 *                 add_special_signal_handler to add two special handlers
 * @tc.level     : Level 0
 */
static void sigchain_add_special_handler_002()
{
    signal(SIGHUP, signal_sighup_handler);

    struct signal_chain_action sighup = {
        .sca_sigaction = sigchain_special_sighup_handler1,
        .sca_mask = {},
        .sca_flags = 0,
    };
    add_special_signal_handler(SIGHUP, &sighup);

    struct signal_chain_action sighup1 = {
        .sca_sigaction = sigchain_special_sighup_handler2,
        .sca_mask = {},
        .sca_flags = 0,
    };
    add_special_signal_handler(SIGHUP, &sighup1);

    if (get_sigchain_mask_enable()) {
        sigset_t set = {0};
        int signo[SIGCHIAN_TEST_SIGNAL_NUM_1] = {SIGHUP};
        SIGCHAIN_TEST_SET_MASK(set, "sigchain_add_special_handler_002", signo, SIGCHIAN_TEST_SIGNAL_NUM_1);
    }
    raise(SIGHUP);
    EXPECT_EQ("sigchain_add_special_handler_002", g_count, SIGCHIAN_TEST_SIGNAL_NUM_5);
}

/**
 * @brief the special handler
 */
static bool sigchain_special_sigsegv_handler1(int signo, siginfo_t *siginfo, void *ucontext_raw)
{
    g_count++;
    EXPECT_EQ("sigchain_add_special_handler_003", signo, SIGSEGV);
    return false;
}

/**
 * @brief the special handler
 */
static bool sigchain_special_sigsegv_handler2(int signo, siginfo_t *siginfo, void *ucontext_raw)
{
    g_count++;
    EXPECT_EQ("sigchain_add_special_handler_003", signo, SIGSEGV);
    return false;
}

/**
 * @brief the signal handler
 */
static void signal_sigsegv_sigaction(int signo)
{
    g_count++;
    EXPECT_EQ("sigchain_add_special_handler_003", signo, SIGSEGV);
}

/**
 * @tc.name      : sigchain_add_special_handler_003
 * @tc.desc      : the signal that are registered with the kernel(Using sigaction), call
 *                 add_special_signal_handler to add two special handlers
 * @tc.level     : Level 0
 */
static void sigchain_add_special_handler_003()
{
    struct sigaction sigac = {
        .sa_handler = signal_sigsegv_sigaction,
    };
    sigaction(SIGSEGV, &sigac, NULL);

    struct signal_chain_action sigsegv = {
        .sca_sigaction = sigchain_special_sigsegv_handler1,
        .sca_mask = {},
        .sca_flags = 0,
    };
    add_special_signal_handler(SIGSEGV, &sigsegv);

    struct signal_chain_action sigsegv2 = {
        .sca_sigaction = sigchain_special_sigsegv_handler2,
        .sca_mask = {},
        .sca_flags = 0,
    };
    add_special_signal_handler(SIGSEGV, &sigsegv2);

    if (get_sigchain_mask_enable()) {
        sigset_t set = {0};
        int signo[SIGCHIAN_TEST_SIGNAL_NUM_1] = {SIGSEGV};
        SIGCHAIN_TEST_SET_MASK(set, "sigchain_add_special_handler_003", signo, SIGCHIAN_TEST_SIGNAL_NUM_1);
    }
    raise(SIGSEGV);
    EXPECT_EQ("sigchain_add_special_handler_003", g_count, SIGCHIAN_TEST_SIGNAL_NUM_8);
}

/**
 * @brief the special handler
 */
static bool sigchain_special_sigterm_handler(int signo, siginfo_t *siginfo, void *ucontext_raw)
{
    g_count++;
    EXPECT_EQ("sigchain_add_special_handler_004", signo, SIGTERM);
    return true;
}

/**
 * @tc.name      : sigchain_add_special_handler_004
 * @tc.desc      : the signal is not registered with the kernel, call add_special_signal_handler to add
 *                 a special handler.
 * @tc.level     : Level 0
 */
static void sigchain_add_special_handler_004()
{
    struct signal_chain_action sigterm = {
        .sca_sigaction = sigchain_special_sigterm_handler,
        .sca_mask = {},
        .sca_flags = 0,
    };
    add_special_signal_handler(SIGTERM, &sigterm);

    if (get_sigchain_mask_enable()) {
        sigset_t set = {0};
        int signo[SIGCHIAN_TEST_SIGNAL_NUM_1] = {SIGTERM};
        SIGCHAIN_TEST_SET_MASK(set, "sigchain_add_special_handler_004", signo, SIGCHIAN_TEST_SIGNAL_NUM_1);
    }

    raise(SIGTERM);
    EXPECT_EQ("sigchain_add_special_handler_004", g_count, SIGCHIAN_TEST_SIGNAL_NUM_9);
}

/**
 * @brief the special handler
 */
static bool sigchain_special_64_handler(int signo, siginfo_t *siginfo, void *ucontext_raw)
{
    EXPECT_EQ("sigchain_add_special_handler_005", signo, SIGCHAIN_SIGNAL_64);
    g_count++;
    return false;
}

/**
 * @brief the signal handler
 */
static void signal_64_handler(int signo)
{
    g_count++;
    EXPECT_EQ("sigchain_add_special_handler_005", signo, SIGCHAIN_SIGNAL_64);
}

/**
 * @tc.name      : sigchain_add_special_handler_005
 * @tc.desc      : the signal is registered with the kernel(Using signal), call add_special_signal_handler to add
 *                 a special handler.
 * @tc.level     : Level 0
 */
static void sigchain_add_special_handler_005()
{
    signal(SIGCHAIN_SIGNAL_64, signal_64_handler);

    struct signal_chain_action sighup = {
        .sca_sigaction = sigchain_special_64_handler,
        .sca_mask = {},
        .sca_flags = 0,
    };
    add_special_signal_handler(SIGCHAIN_SIGNAL_64, &sighup);

    if (get_sigchain_mask_enable()) {
        sigset_t set = {0};
        int signo[SIGCHIAN_TEST_SIGNAL_NUM_1] = {SIGCHAIN_SIGNAL_64};
        SIGCHAIN_TEST_SET_MASK(set, "sigchain_add_special_handler_005", signo, SIGCHIAN_TEST_SIGNAL_NUM_1);
    }
    raise(SIGCHAIN_SIGNAL_64);
    EXPECT_EQ("sigchain_add_special_handler_005", g_count, SIGCHIAN_TEST_SIGNAL_NUM_11);
}

/**
 * @brief the special handler
 */
static bool sigchain_special_37_handler(int signo, siginfo_t *siginfo, void *ucontext_raw)
{
    g_count++;
    EXPECT_EQ("sigchain_add_special_handler_006", signo, SIGCHAIN_SIGNAL_37);
    return false;
}

/**
 * @brief the signal handler
 */
static void signal_37_sigaction(int signo)
{
    g_count++;
    EXPECT_EQ("sigchain_add_special_handler_006", signo, SIGCHAIN_SIGNAL_37);
}

/**
 * @tc.name      : sigchain_add_special_handler_006
 * @tc.desc      : the signal is registered with the kernel(Using sigaction), call add_special_signal_handler
 *                 to add a special handler.
 * @tc.level     : Level 0
 */
static void sigchain_add_special_handler_006()
{
    struct sigaction sigac = {
        .sa_handler = signal_37_sigaction,
    };
    sigaction(SIGCHAIN_SIGNAL_37, &sigac, NULL);

    struct signal_chain_action sig37 = {
        .sca_sigaction = sigchain_special_37_handler,
        .sca_mask = {},
        .sca_flags = 0,
    };
    add_special_signal_handler(SIGCHAIN_SIGNAL_37, &sig37);

    if (get_sigchain_mask_enable()) {
        sigset_t set = {0};
        int signo[SIGCHIAN_TEST_SIGNAL_NUM_1] = {SIGCHAIN_SIGNAL_37};
        SIGCHAIN_TEST_SET_MASK(set, "sigchain_add_special_handler_006", signo, SIGCHIAN_TEST_SIGNAL_NUM_1);
    }

    raise(SIGCHAIN_SIGNAL_37);
    EXPECT_EQ("sigchain_add_special_handler_006", g_count, SIGCHIAN_TEST_SIGNAL_NUM_13);
}

/**
 * @brief the special handler
 */
static bool sigchain_special_43_handler(int signo, siginfo_t *siginfo, void *ucontext_raw)
{
    g_count++;
    EXPECT_EQ("sigchain_add_special_handler_007", signo, SIGCHAIN_SIGNAL_43);
    return true;
}

/**
 * @brief the signal handler
 */
static void signal_43_sigaction(int signo)
{
    g_count++;
    EXPECT_FALSE("sigchain_intercept_sigprocmask_003", true);
}

/**
 * @tc.name      : sigchain_add_special_handler_007
 * @tc.desc      : Multiple signal are registered with the kernel(Using sigaction), sigchain_special_43_handler return
 *                 true.
 * @tc.level     : Level 0
 */
static void sigchain_add_special_handler_007()
{
    struct sigaction sigac = {
        .sa_handler = signal_43_sigaction,
    };
    sigaction(SIGCHAIN_SIGNAL_43, &sigac, NULL);

    struct signal_chain_action sig43 = {
        .sca_sigaction = sigchain_special_43_handler,
        .sca_mask = {},
        .sca_flags = 0,
    };
    add_special_signal_handler(SIGCHAIN_SIGNAL_43, &sig43);

    if (get_sigchain_mask_enable()) {
        sigset_t set = {0};
        int signo[SIGCHIAN_TEST_SIGNAL_NUM_1] = {SIGCHAIN_SIGNAL_43};
        SIGCHAIN_TEST_SET_MASK(set, "sigchain_add_special_handler_007", signo, SIGCHIAN_TEST_SIGNAL_NUM_1);
    }

    raise(SIGCHAIN_SIGNAL_43);
    EXPECT_EQ("sigchain_add_special_handler_007", g_count, SIGCHIAN_TEST_SIGNAL_NUM_14);
}

int main(void)
{
    sigchain_add_special_handler_001();
    sigchain_add_special_handler_002();
    sigchain_add_special_handler_003();
    sigchain_add_special_handler_004();
    sigchain_add_special_handler_005();
    sigchain_add_special_handler_006();
    sigchain_add_special_handler_007();
    return t_status;
}