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

/**
 * @brief the special handler
 */
static bool sigchain_special_handler1(int signo, siginfo_t *siginfo, void *ucontext_raw)
{
    EXPECT_FALSE("sigchain_rm_special_handler_010", false);
    return false;
}

/**
 * @brief the special handler
 */
static bool sigchain_special_handler2(int signo, siginfo_t *siginfo, void *ucontext_raw)
{
    EXPECT_FALSE("sigchain_rm_special_handler_010", false);
    return false;
}

/**
 * @brief the special handler
 */
static bool sigchain_special_handler3(int signo, siginfo_t *siginfo, void *ucontext_raw)
{
    EXPECT_FALSE("sigchain_rm_special_handler_010", false);
    return false;
}

/**
 * @brief the special handler
 */
static bool sigchain_special_handler4(int signo, siginfo_t *siginfo, void *ucontext_raw)
{
    EXPECT_FALSE("sigchain_rm_special_handler_010", false);
    return false;
}

/**
 * @brief the special handler
 */
static bool sigchain_special_handler5(int signo, siginfo_t *siginfo, void *ucontext_raw)
{
    EXPECT_FALSE("sigchain_rm_special_handler_010", false);
    return false;
}

/**
 * @brief the special handler
 */
static bool sigchain_special_handler6(int signo, siginfo_t *siginfo, void *ucontext_raw)
{
    EXPECT_FALSE("sigchain_rm_special_handler_010", false);
    return false;
}

/**
 * @brief the special handler
 */
static bool sigchain_special_handler7(int signo, siginfo_t *siginfo, void *ucontext_raw)
{
    EXPECT_FALSE("sigchain_rm_special_handler_010", false);
    return false;
}

/**
 * @brief the special handler
 */
static bool sigchain_special_handler8(int signo, siginfo_t *siginfo, void *ucontext_raw)
{
    EXPECT_FALSE("sigchain_rm_special_handler_010", false);
    return false;
}

/**
 * @brief the special handler
 */
static bool sigchain_special_handler9(int signo, siginfo_t *siginfo, void *ucontext_raw)
{
    EXPECT_FALSE("sigchain_rm_special_handler_010", false);
    return false;
}

/**
 * @brief the special handler
 */
static bool sigchain_special_handler10(int signo, siginfo_t *siginfo, void *ucontext_raw)
{
    EXPECT_FALSE("sigchain_rm_special_handler_010", false);
    return false;
}

/**
 * @tc.name      : sigchain_rm_special_handler_010
 * @tc.desc      : Remove multiple special handlers for the different signals that are not registered with
 *                 the kernel in sigchain.
 * @tc.level     : Level 0
 */
static void sigchain_rm_special_handler_010()
{
    struct signal_chain_action sigsegv = {
        .sca_sigaction = sigchain_special_handler1,
        .sca_mask = {},
        .sca_flags = 0,
    };
    add_special_signal_handler(SIGHUP, &sigsegv);

    struct signal_chain_action sigsegv1 = {
        .sca_sigaction = sigchain_special_handler2,
        .sca_mask = {},
        .sca_flags = 0,
    };
    add_special_signal_handler(SIGABRT, &sigsegv1);

    struct signal_chain_action sigsegv2 = {
        .sca_sigaction = sigchain_special_handler3,
        .sca_mask = {},
        .sca_flags = 0,
    };
    add_special_signal_handler(SIGSEGV, &sigsegv2);

    struct signal_chain_action sigsegv3 = {
        .sca_sigaction = sigchain_special_handler4,
        .sca_mask = {},
        .sca_flags = 0,
    };
    add_special_signal_handler(SIGURG, &sigsegv3);

    struct signal_chain_action sigsegv4 = {
        .sca_sigaction = sigchain_special_handler5,
        .sca_mask = {},
        .sca_flags = 0,
    };
    add_special_signal_handler(SIGSYS, &sigsegv4);

    struct signal_chain_action sigsegv5 = {
        .sca_sigaction = sigchain_special_handler6,
        .sca_mask = {},
        .sca_flags = 0,
    };
    add_special_signal_handler(SIGCHAIN_SIGNAL_37, &sigsegv5);

    struct signal_chain_action sigsegv6 = {
        .sca_sigaction = sigchain_special_handler7,
        .sca_mask = {},
        .sca_flags = 0,
    };
    add_special_signal_handler(SIGCHAIN_SIGNAL_43, &sigsegv6);

    struct signal_chain_action sigsegv7 = {
        .sca_sigaction = sigchain_special_handler8,
        .sca_mask = {},
        .sca_flags = 0,
    };
    add_special_signal_handler(SIGCHAIN_SIGNAL_50, &sigsegv7);

    struct signal_chain_action sigsegv8 = {
        .sca_sigaction = sigchain_special_handler9,
        .sca_mask = {},
        .sca_flags = 0,
    };
    add_special_signal_handler(SIGCHAIN_SIGNAL_56, &sigsegv8);

    struct signal_chain_action sigsegv9 = {
        .sca_sigaction = sigchain_special_handler10,
        .sca_mask = {},
        .sca_flags = 0,
    };
    add_special_signal_handler(SIGCHAIN_SIGNAL_64, &sigsegv9);

    remove_special_signal_handler(SIGHUP, sigchain_special_handler1);
    remove_special_signal_handler(SIGABRT, sigchain_special_handler2);
    remove_special_signal_handler(SIGSEGV, sigchain_special_handler3);
    remove_special_signal_handler(SIGURG, sigchain_special_handler4);
    remove_special_signal_handler(SIGSYS, sigchain_special_handler5);
    remove_special_signal_handler(SIGCHAIN_SIGNAL_37, sigchain_special_handler6);
    remove_special_signal_handler(SIGCHAIN_SIGNAL_43, sigchain_special_handler7);
    remove_special_signal_handler(SIGCHAIN_SIGNAL_50, sigchain_special_handler8);
    remove_special_signal_handler(SIGCHAIN_SIGNAL_56, sigchain_special_handler9);
    remove_special_signal_handler(SIGCHAIN_SIGNAL_64, sigchain_special_handler10);

    raise(SIGHUP);
    raise(SIGURG);
    raise(SIGCHAIN_SIGNAL_37);
    raise(SIGCHAIN_SIGNAL_43);
    raise(SIGCHAIN_SIGNAL_50);
    raise(SIGCHAIN_SIGNAL_56);
    raise(SIGCHAIN_SIGNAL_64);
}

int main(void)
{
    sigchain_rm_special_handler_010();
    return t_status;
}