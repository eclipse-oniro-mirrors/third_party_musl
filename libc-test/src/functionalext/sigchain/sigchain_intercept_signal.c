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
#include <signal.h>
#include <wchar.h>
#include <stdlib.h>
#include "test.h"
#include "functionalext.h"
#include "sigchain_util.h"

static int g_count = 0;
/**
 * @brief the signal handler
 */
static void signal_handler1(int signo)
{
    g_count++;
    EXPECT_EQ("sigchain_intercept_signal_001", signo, SIGHUP);
}

/**
 * @tc.name      : sigchain_add_special_handler_025
 * @tc.desc      : The signals are not registered with the special handler, test the influence of sigchain on signal
 * @tc.level     : Level 0
 */
static void sigchain_intercept_signal_001()
{
    signal(SIGHUP, signal_handler1);
    raise(SIGHUP);
    EXPECT_EQ("sigchain_intercept_signal_001", g_count, SIGCHIAN_TEST_SIGNAL_NUM_1);
}

/**
 * @brief the special handler
 */
static bool sigchain_special_handler1(int signo, siginfo_t *siginfo, void *ucontext_raw)
{
    g_count++;
    EXPECT_EQ("sigchain_intercept_signal_002", signo, SIGSEGV);
    return false;
}

/**
 * @brief the signal handler
 */
static void signal_handler2(int signo)
{
    g_count++;
    EXPECT_EQ("sigchain_intercept_signal_002", signo, SIGSEGV);
}

/**
 * @tc.name      : sigchain_intercept_signal_002
 * @tc.desc      : The signals are registered with the special handler, test the influence of sigchain on signal
 * @tc.level     : Level 0
 */
static void sigchain_intercept_signal_002()
{
    struct signal_chain_action sigsegv = {
        .sca_sigaction = sigchain_special_handler1,
        .sca_mask = {},
        .sca_flags = 0,
    };
    add_special_signal_handler(SIGSEGV, &sigsegv);

    signal(SIGSEGV, signal_handler2);

    if (get_sigchain_mask_enable()) {
        sigset_t set = {0};
        int signo[SIGCHIAN_TEST_SIGNAL_NUM_1] = {SIGSEGV};
        SIGCHAIN_TEST_SET_MASK(set, "sigchain_intercept_signal_002", signo, SIGCHIAN_TEST_SIGNAL_NUM_1);
    }
    raise(SIGSEGV);
    EXPECT_EQ("sigchain_intercept_signal_002", g_count, SIGCHIAN_TEST_SIGNAL_NUM_3);
}

/**
 * @brief the special handler
 */
static bool sigchain_special_handler3(int signo, siginfo_t *siginfo, void *ucontext_raw)
{
    g_count++;
    EXPECT_EQ("sigchain_intercept_signal_003", signo, SIGURG);
    return false;
}

/**
 * @brief the signal handler
 */
static void signal_handler3(int signo)
{
    g_count++;
    EXPECT_EQ("sigchain_intercept_signal_003", signo, SIGURG);
}


/**
 * @tc.name      : sigchain_intercept_signal_003
 * @tc.desc      : The signals are registered with the special handler, and remove the special handler.
 *                 Test the influence of sigchain on signal
 * @tc.level     : Level 0
 */
static void sigchain_intercept_signal_003()
{
    struct signal_chain_action sigurg = {
        .sca_sigaction = sigchain_special_handler3,
        .sca_mask = {},
        .sca_flags = 0,
    };
    add_special_signal_handler(SIGURG, &sigurg);

    signal(SIGURG, signal_handler3);

    remove_special_signal_handler(SIGURG, sigchain_special_handler3);

    if (get_sigchain_mask_enable()) {
        sigset_t set = {0};
        int signo[SIGCHIAN_TEST_SIGNAL_NUM_1] = {SIGURG};
        SIGCHAIN_TEST_SET_MASK(set, "sigchain_intercept_sigaction_002", signo, SIGCHIAN_TEST_SIGNAL_NUM_1);
    }
    raise(SIGURG);
    EXPECT_EQ("sigchain_intercept_signal_003", g_count, SIGCHIAN_TEST_SIGNAL_NUM_4);
}
int main(void)
{
    sigchain_intercept_signal_001();
    sigchain_intercept_signal_002();
    sigchain_intercept_signal_003();
    return t_status;
}