/**
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef _GWP_ASAN_H
#define _GWP_ASAN_H
#ifdef USE_GWP_ASAN

#include <dlfcn.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define GWP_ASAN_LOG_TAG "gwp_asan"
#define GWP_ASAN_LOG_DIR "/data/local/tmp/"

typedef void (*printf_t)(const char *fmt, ...);
typedef void (*printf_backtrace_t)(uintptr_t*, size_t, printf_t);
typedef size_t (*segv_backtrace_t)(uintptr_t*, size_t, void*);
typedef size_t (*backtrace_t)(size_t*, size_t);

// Note that "compiler-rt/lib/gwp_asan/gwp_asan_c_interface.cpp" in the llvm code need to be modified synchronously.
typedef struct gwp_asan_option {
    bool help;
    bool enable;
    bool install_fork_handlers;
    bool install_signal_handlers;
    int sample_rate;
    int max_simultaneous_allocations;
    backtrace_t backtrace;
    printf_t gwp_asan_printf;
    printf_backtrace_t printf_backtrace;
    segv_backtrace_t segv_backtrace;
    int min_sample_size;
    const char *white_list_path;
    bool gwp_asan_recoverable;
} gwp_asan_option;

typedef struct unwind_info {
    size_t fp;
    size_t lr;
} unwind_info;

size_t libc_gwp_asan_unwind_fast(size_t *frame_buf, size_t max_record_stack);
size_t libc_gwp_asan_unwind_segv(size_t *frame_buf, size_t max_record_stack, void *signal_context);

/*
 * Custom unwind backend for GWP-ASan.
 *
 * Contract:
 *   - Appends frames at frame_buf[num_frames .. max_record_stack-1] and
 *     returns the total frame count, which MUST NOT exceed max_record_stack.
 *   - Frames already present in frame_buf[0 .. num_frames-1] must be kept.
 *   - stack_start/stack_end/current_frame_addr delimit the current stack.
 *
 * The callback may be invoked from a SIGSEGV handler (crash reporting path),
 * so it MUST be async-signal-safe.
 *
 * Registration is process-global: the callback takes effect for all threads
 * once registered. A later registration replaces the earlier one (last wins).
 * Pass NULL to libc_gwp_asan_set_unwind_callbacks to unregister and fall back
 * to the frame-pointer unwinder. Same-thread reentry does not call the
 * callback again and uses the frame-pointer unwinder.
 */
#define GWP_ASAN_UNWIND_ABI_VERSION 1

typedef size_t (*gwp_asan_unwind_callback)(
    size_t *frame_buf, size_t num_frames, size_t max_record_stack,
    size_t current_frame_addr, size_t stack_start, size_t stack_end);

typedef struct gwp_asan_unwind_callbacks {
    uint32_t struct_size;
    uint32_t abi_version;
    gwp_asan_unwind_callback unwind;
} gwp_asan_unwind_callbacks;

enum {
    GWP_ASAN_UNWIND_REGISTER_OK = 0,
    GWP_ASAN_UNWIND_REGISTER_INVALID_ARGUMENT = 1,
    GWP_ASAN_UNWIND_REGISTER_UNSUPPORTED_ABI = 2
};

int libc_gwp_asan_set_unwind_callbacks(const gwp_asan_unwind_callbacks *callbacks);
size_t libc_gwp_asan_collect_allocations_by_time_range(uint64_t timespan, uintptr_t *buffer,
                                                       size_t max_count, size_t depth);

bool libc_gwp_asan_has_free_mem();
bool libc_gwp_asan_ptr_is_mine(void *addr);
bool may_init_gwp_asan(bool force_init);

#endif
#endif
