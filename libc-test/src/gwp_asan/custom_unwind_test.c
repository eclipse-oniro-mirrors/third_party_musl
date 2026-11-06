/**
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#define _GNU_SOURCE
#include <string.h>
#if defined(__aarch64__)
#include <ucontext.h>
#endif
#include "gwp_asan.h"
#include "test.h"

static int g_calls;
static size_t g_seen_fp;
static size_t g_seen_num_frames;
static int g_saw_bounds;

static size_t zero_unwind(size_t *frame_buf, size_t num_frames, size_t max_record_stack,
                          size_t current_frame_addr, size_t stack_start, size_t stack_end)
{
    (void)frame_buf;
    (void)num_frames;
    (void)max_record_stack;
    (void)current_frame_addr;
    (void)stack_start;
    (void)stack_end;
    g_calls++;
    return 0;
}

static size_t overflow_unwind(size_t *frame_buf, size_t num_frames, size_t max_record_stack,
                              size_t current_frame_addr, size_t stack_start, size_t stack_end)
{
    (void)frame_buf;
    (void)num_frames;
    (void)current_frame_addr;
    (void)stack_start;
    (void)stack_end;
    g_calls++;
    return max_record_stack + 1;
}

static size_t marker_unwind(size_t *frame_buf, size_t num_frames, size_t max_record_stack,
                            size_t current_frame_addr, size_t stack_start, size_t stack_end)
{
    g_calls++;
    g_seen_fp = current_frame_addr;
    g_seen_num_frames = num_frames;
    g_saw_bounds = stack_start < stack_end &&
                   current_frame_addr >= stack_start &&
                   current_frame_addr < stack_end;
    if (frame_buf == NULL || num_frames >= max_record_stack) {
        return num_frames;
    }
    frame_buf[num_frames] = 0x2222;
    return num_frames + 1;
}

static size_t recursive_unwind(size_t *frame_buf, size_t num_frames, size_t max_record_stack,
                               size_t current_frame_addr, size_t stack_start, size_t stack_end)
{
    size_t nested[8];

    (void)frame_buf;
    (void)num_frames;
    (void)max_record_stack;
    (void)current_frame_addr;
    (void)stack_start;
    (void)stack_end;
    g_calls++;
    if (g_calls == 1) {
        libc_gwp_asan_unwind_fast(nested, 8);
    }
    return 0;
}

static int register_unwind(gwp_asan_unwind_callback unwind)
{
    gwp_asan_unwind_callbacks callbacks = {
        .struct_size = sizeof(callbacks),
        .abi_version = GWP_ASAN_UNWIND_ABI_VERSION,
        .unwind = unwind,
    };
    return libc_gwp_asan_set_unwind_callbacks(&callbacks);
}

static void expect_custom_return(gwp_asan_unwind_callback unwind, size_t expect_frames)
{
    size_t frame_buf[32];
    size_t frames;
    int calls;

    g_calls = 0;
    if (register_unwind(unwind) != GWP_ASAN_UNWIND_REGISTER_OK) {
        t_error("FAIL: register custom callback\n");
        return;
    }
    frames = libc_gwp_asan_unwind_fast(frame_buf, 10);
    calls = g_calls;
    libc_gwp_asan_set_unwind_callbacks(NULL);
    if (calls != 1 || frames != expect_frames) {
        t_error("FAIL: custom return calls=%d frames=%zu expect=%zu\n",
                calls, frames, expect_frames);
    }
}

int main(void)
{
    size_t frame_buf[32];
    size_t frames;
    gwp_asan_unwind_callbacks bad = {
        .struct_size = 4,
        .abi_version = GWP_ASAN_UNWIND_ABI_VERSION,
        .unwind = marker_unwind,
    };
    gwp_asan_unwind_callbacks unsupported = {
        .struct_size = sizeof(unsupported),
        .abi_version = GWP_ASAN_UNWIND_ABI_VERSION + 1,
        .unwind = marker_unwind,
    };
    gwp_asan_unwind_callbacks empty = {
        .struct_size = sizeof(empty),
        .abi_version = GWP_ASAN_UNWIND_ABI_VERSION,
        .unwind = NULL,
    };

    frames = libc_gwp_asan_unwind_fast(frame_buf, 10);
    if (frames < 3) {
        t_error("FAIL: unregistered unwind frames=%zu\n", frames);
    }

    if (libc_gwp_asan_set_unwind_callbacks(&bad) != GWP_ASAN_UNWIND_REGISTER_INVALID_ARGUMENT) {
        t_error("FAIL: small struct_size should be rejected\n");
    }
    if (libc_gwp_asan_set_unwind_callbacks(&unsupported) != GWP_ASAN_UNWIND_REGISTER_UNSUPPORTED_ABI) {
        t_error("FAIL: abi mismatch should be rejected\n");
    }
    if (libc_gwp_asan_set_unwind_callbacks(&empty) != GWP_ASAN_UNWIND_REGISTER_INVALID_ARGUMENT) {
        t_error("FAIL: empty callbacks should be rejected\n");
    }

    g_calls = 0;
    g_seen_fp = 0;
    g_seen_num_frames = (size_t)-1;
    g_saw_bounds = 0;
    if (register_unwind(marker_unwind) != GWP_ASAN_UNWIND_REGISTER_OK) {
        t_error("FAIL: register marker callback\n");
    }
    memset(frame_buf, 0, sizeof(frame_buf));
    frames = libc_gwp_asan_unwind_fast(frame_buf, 10);
    if (g_calls != 1 || g_seen_num_frames != 0 || !g_saw_bounds || g_seen_fp == 0 ||
        frames != 1 || frame_buf[0] != 0x2222) {
        t_error("FAIL: custom fast calls=%d num=%zu bounds=%d frames=%zu buf0=%zx\n",
                g_calls, g_seen_num_frames, g_saw_bounds, frames, frame_buf[0]);
    }

    g_calls = 0;
    if (libc_gwp_asan_set_unwind_callbacks(&unsupported) != GWP_ASAN_UNWIND_REGISTER_UNSUPPORTED_ABI) {
        t_error("FAIL: bad replace should keep the previous callback\n");
    }
    libc_gwp_asan_unwind_fast(frame_buf, 10);
    if (g_calls != 1) {
        t_error("FAIL: previous callback was dropped, calls=%d\n", g_calls);
    }

    expect_custom_return(zero_unwind, 0);
    expect_custom_return(overflow_unwind, 10);

    g_calls = 0;
    if (register_unwind(recursive_unwind) != GWP_ASAN_UNWIND_REGISTER_OK) {
        t_error("FAIL: register recursive callback\n");
    }
    frames = libc_gwp_asan_unwind_fast(frame_buf, 8);
    if (g_calls != 1 || frames != 0) {
        t_error("FAIL: custom unwind reentered, calls=%d frames=%zu\n", g_calls, frames);
    }

    g_calls = 0;
    libc_gwp_asan_set_unwind_callbacks(NULL);
    libc_gwp_asan_unwind_fast(frame_buf, 10);
    if (g_calls != 0) {
        t_error("FAIL: callback ran after unregister calls=%d\n", g_calls);
    }

#if defined(__aarch64__)
    {
        ucontext_t uc;

        g_calls = 0;
        g_seen_fp = 0;
        g_seen_num_frames = 0;
        if (register_unwind(marker_unwind) != GWP_ASAN_UNWIND_REGISTER_OK) {
            t_error("FAIL: register segv callback\n");
        }
        memset(&uc, 0, sizeof(uc));
        uc.uc_mcontext.pc = 0xabc0;
        uc.uc_mcontext.regs[29] = (unsigned long)__builtin_frame_address(0);
        memset(frame_buf, 0, sizeof(frame_buf));
        frames = libc_gwp_asan_unwind_segv(frame_buf, 8, &uc);
        if (g_calls != 1 || g_seen_num_frames != 1 ||
            g_seen_fp != (size_t)__builtin_frame_address(0) ||
            frames != 2 || frame_buf[0] != 0xabc0 || frame_buf[1] != 0x2222) {
            t_error("FAIL: custom segv calls=%d num=%zu frames=%zu buf0=%zx buf1=%zx\n",
                    g_calls, g_seen_num_frames, frames, frame_buf[0], frame_buf[1]);
        }
        libc_gwp_asan_set_unwind_callbacks(NULL);
    }
#endif

    return t_status;
}
