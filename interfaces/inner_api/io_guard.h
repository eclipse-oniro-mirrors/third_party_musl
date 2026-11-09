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

#ifndef IO_GUARD_H
#define IO_GUARD_H

#ifdef IO_GUARD_ENABLE
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#ifdef __cplusplus
extern "C" {
#endif
/* Inner API: token is an operation marker, not a security credential.
 * 0 denotes unowned; UINT64_MAX is reserved. The caller must synchronize
 * writes, token transfer, close and fd reuse. No check pins the kernel fd.
 * Ordinary fd-output paths (including stdio flushes and socket sends) expect
 * token 0. Buffered stdio is checked when bytes are written, not buffered.
 * Only exchange mutates tokens during normal operation. In particular, a
 * WARN/DISABLED close leaves its token behind: release BEFORE close.
 * A stale token may be released with the exact old token after a bypassing
 * close, but only while exclusively controlling that fd number's lifetime.
 * fork children start DISABLED but retain inherited tokens; enabling checking
 * again uses those tokens. vfork must not mutate state.
 */
enum io_guard_error_level {
    IO_GUARD_DISABLED = 0,
    IO_GUARD_WARN = 1,
    IO_GUARD_BLOCK = 2,
    IO_GUARD_FATAL = 3
};
/* Returns old level; invalid level sets EINVAL without changing it.
 * Build-time OFF: these APIs are not linked into libc; no fallback symbols
 * are provided. Runtime DISABLED retains tags. */
enum io_guard_error_level io_guard_set_error_level(enum io_guard_error_level level);
enum io_guard_error_level io_guard_get_error_level(void);
/* Strict CAS under every policy, including FATAL: mismatch is -1/EPERM.
 * 0 -> token claims, old -> new transfers, token -> 0 releases.
 * Nonzero claims require a live fd. fdsan tags are independent: this API
 * neither checks nor changes them, and both tags may coexist on one fd.
 * Valid fd range follows the fdsan fd table: [0, RLIMIT_NOFILE hard limit]
 * (32768 when the limit is infinite); negative or out of range is -1/EBADF.
 * Metadata allocation failure is reported through errno. */
int io_guard_exchange_token(int fd, uint64_t expected_token, uint64_t new_token);
/* token must be nonzero and not UINT64_MAX (-1/EINVAL). A mismatch follows
 * the process policy: allow, warn+allow, -1/EPERM, or abort. Does not mutate
 * token. Otherwise preserves write semantics, including short writes. */
ssize_t io_guard_write(int fd, const void *buf, size_t count, uint64_t token);
/* Positional write with the same token rules; does not change the file offset. */
ssize_t io_guard_pwrite(int fd, const void *buf, size_t count, off_t offset, uint64_t token);
/* Validate all non-anonymous mappings, including private and read-only ones.
 * Anonymous mappings do not check fd ownership. Invalid token arguments always fail.
 * Validation occurs at mapping creation, not on subsequent memory stores. */
void *io_guard_mmap(void *addr, size_t length, int prot, int flags, int fd,
                    off_t offset, uint64_t token);
#ifdef __cplusplus
}
#endif
#endif /* IO_GUARD_ENABLE */
#endif /* IO_GUARD_H */
