<!--
Copyright (c) 2026 Huawei Device Co., Ltd.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
-->

## IO_GUARD_WRITE &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; OHOS Programmer's Manual

#### **NAME**

io_guard_write - write using an expected io_guard token.

#### **SYNOPSIS**

```c
#include "io_guard.h"

ssize_t io_guard_write(int fd, const void *buf, size_t count, uint64_t token);
```

#### **DESCRIPTION**

Checks **token** against the token registered for **fd**, then applies the
process-wide error policy before writing. An unregistered descriptor has token
0 and does not match a valid nonzero token.

| Error level | Token mismatch behavior |
| ----------- | ----------------------- |
| IO_GUARD_DISABLED | Allow without diagnostics |
| IO_GUARD_WARN | Report and allow |
| IO_GUARD_BLOCK | Report and return -1/EPERM without writing |
| IO_GUARD_FATAL | Report and abort the process (SIGABRT) |

A matching token permits the underlying write operation. No token is modified.
An invalid token argument is rejected even when runtime checking is disabled.

#### **PARAMETERS**

**fd**: Destination file descriptor.

**buf**: Buffer containing the bytes to write.

**count**: Number of bytes requested, subject to the same limits as write().

**token**: Expected registered token; neither 0 nor **UINT64_MAX** is allowed.

#### **RETURN VALUE**

Returns the number of bytes actually written, which may be less than **count**.
On failure, returns -1 and sets errno. FATAL token mismatches do not return.

#### **ERRORS**

**EINVAL**: **token** is 0 or **UINT64_MAX**.

**EPERM**: Token mismatch while the policy is **IO_GUARD_BLOCK**.

Errors from the underlying write() operation may also be returned. Token
validation precedes the write operation, so a mismatch can be reported before
an underlying descriptor error.

#### **ATTRIBUTES**

| Interface | Attribute | Value |
| --------- | --------- | ----- |
| io_guard_write() | Thread safety | MT-safe |
| | Signal safety | Not guaranteed |

#### **HISTORY**

       -- 2026

#### **NOTES**

This is a system-side inner API declared in `interfaces/inner_api/io_guard.h`,
not a public NDK header. Functional support requires libc to be built with
**musl_enable_io_guard** enabled. When disabled, the io_guard API symbols are
not linked into libc; there are no fallback implementations. Callers must
exclude these calls when building against a libc without io_guard support.
The header exposes the io_guard types, constants, and function declarations only
when **IO_GUARD_ENABLE** is defined. GN callers should depend on
`//third_party/musl:io_guard_inner`, which propagates the include path and defines
the macro only when **musl_enable_io_guard** is enabled. Defining the macro in a
caller alone does not add support to a libc built without io_guard.

**Validation rules for ordinary output**

Ordinary fd-output paths and close() expect token **0**; they do not accept an
owner token argument. A nonzero registered token therefore causes a mismatch
even when the call comes from the thread that registered it. io_guard_write(),
io_guard_pwrite(), and io_guard_mmap() instead check the supplied nonzero token
for their covered operations without entering a second expected-zero check. None of these checks changes or releases a token.

When a matching token is found, the underlying operation retains its normal
return value and errors. On a mismatch, DISABLED allows the operation without a
diagnostic; WARN reports through HiLog and immediately allows the operation;
BLOCK reports and rejects the checked operation with EPERM; FATAL reports and
aborts. WARN never falls through to BLOCK or FATAL handling. Allowing an
operation does not guarantee that the underlying I/O succeeds.

**Output interfaces and validation points**

This table describes the current OpenHarmony libc paths when built with
**musl_enable_io_guard** enabled. Multiple interfaces that converge on one
checked path reuse that check rather than adding a check to every upper-level
interface. Platform availability of the individual interfaces is unchanged.
mmap()/mmap64() validation additionally requires the malloc-hook build; builds
without malloc hooks do not check these paths.

| Interfaces | Descriptor checked | Validation point and scope |
| ---------- | ------------------ | -------------------------- |
| write(), eventfd_write() | Destination fd | write(); eventfd_write() delegates to it |
| mmap(), mmap64() | File fd | At creation of every non-anonymous mapping, including private and read-only mappings; see io_guard_mmap() |
| pwrite(), pwrite64() | Destination fd | pwrite(), before positional output; pwrite64 shares the implementation |
| writev() | Destination fd | Before the vectored write |
| pwritev(), pwritev64() | Destination fd | pwritev(), before positional vectored output; the alias shares the implementation |
| pwritev2() | Destination fd | All output branches are checked; the offset -1, zero-flags path reuses writev() validation |
| fwrite()/fwrite_unlocked(), fputs(), fputc()/putc(), fprintf()/vfprintf(), printf()/vprintf(), puts(), putchar(), and related wide-character output | The fd associated with a standard file stream | Shared actual fd-write path, covering direct output and buffered flushes |
| dprintf(), vdprintf() | Supplied fd | Shared standard FILE output path |
| fflush(), fclose(), freopen(), and normal process-exit flushing | The fd of each stream being flushed | Pending output uses the same FILE write path; buffering and closing are not one atomic operation |
| send(), sendto() | Socket fd | sendto(); send() delegates to it |
| sendmsg() | Socket fd | Before the message send |
| sendmmsg() | Socket fd | The delegated sendmsg() path checks each attempted message when LONG_MAX > INT_MAX; the direct syscall path checks before the batch; empty batches skip io_guard validation on both word sizes |
| sendfile(), sendfile64() | out_fd | Before transfer; the input fd is not checked for writing |
| splice(), copy_file_range() | fd_out | Before transfer; fd_in is not checked for writing |
| tee() | dest | Before transfer; src is not checked for writing |
| mq_send(), mq_timedsend() | Message queue descriptor | mq_timedsend(); mq_send() delegates to it |
| mq_close() | Message queue descriptor | Before the original close syscall; expects token 0 and never clears it |
| aio_write(), LIO_WRITE requests submitted through lio_listio() | aio_fildes | write()/pwrite() when the asynchronous operation executes, not at submission |
| close() and wrappers reaching the same checked close path | Closing fd | Requires token 0 before closing; never releases a token itself |
| dup(), dup2(), dup3() | Source fd; for dup2()/dup3() also the replaced target fd when it differs | Before duplication, with the same expected-zero rule as close(); a guarded fd is not duplicated or replaced past the guard. freopen()'s internal __dup3() path is covered as well |
| fcntl(F_DUPFD), fcntl(F_DUPFD_CLOEXEC) | Source fd | Before duplication, with the same expected-zero rule as dup(); the fcntl64() alias shares the implementation |
| ftruncate(), ftruncate64() | File fd | Before truncation, with the same expected-zero rule as write(); ftruncate64 shares the implementation |
| posix_fallocate(), posix_fallocate64() | File fd | Before allocation, with the same expected-zero rule as write(); a BLOCK mismatch is returned as the EPERM error number, not -1/errno; posix_fallocate64 shares the implementation |
| fallocate(), fallocate64() | File fd | Before allocation, with the same expected-zero rule as write(); a BLOCK mismatch returns -1/EPERM; fallocate64 shares the implementation |

**pwritev2() validation paths**

When SYS_pwritev is available, zero flags and offset -1 delegate to writev(),
which performs the check. Zero flags with another offset are checked before
the direct pwritev syscall. The remaining path is checked before pwritev2.
The delegated path does not add a second check at the pwritev2 entry point.

**Closing and empty batches**

mq_close() validates token 0 but retains its original syscall and error handling;
it does not delegate to close() or add fdsan ownership processing. As with
close(), WARN/DISABLED may close a tagged descriptor without clearing its token.
Callers should release the token before closing.

sendmmsg() skips io_guard validation when vlen is zero because no messages are
sent. The 64-bit path returns zero; the 32-bit path retains its original syscall
and argument/error handling. A nonempty 64-bit batch retains per-message
sendmsg() checks without an extra entry check.

**Failure reporting and buffering**

For direct fd-output interfaces, BLOCK returns -1 with errno set to EPERM before
the rejected write or transfer is issued. This does not undo earlier successful
writes. In particular, a delegated sendmmsg() loop may return a positive count
if some messages were sent before a later check failed. Callers must handle
partial completion using each interface's normal contract.

For stdio, validation occurs when bytes are written to the fd, not when bytes
are merely copied into a user-space stream buffer. Thus fwrite()/fprintf() may
initially succeed and a later fflush()/fclose() may fail. A rejected actual write
sets the stream error indicator and follows the existing error path, which
clears the pending write pointers; do not assume that clearerr() and another
fflush() will recover the discarded buffered data. Check returned element or
character counts, ferror(), and flush/close results. stdio reports its usual
failure form (for example a short fwrite() result or EOF), rather than always
returning the direct-write value -1.

For asynchronous output, a submission can succeed before validation occurs.
A blocked operation is reported by aio_error() as EPERM and by aio_return() as
-1. A waiting lio_listio() call may report EIO for a failed batch; inspect the
individual requests. Synchronize completion before releasing or transferring a
token or closing/reusing the fd. Changing the process policy after submission
can affect the later execution-time check.

**Caller lifecycle requirements**

The token-bearing interfaces are io_guard_write(), io_guard_pwrite(), and
io_guard_mmap(). A token does not authorize ordinary pwrite(), non-anonymous
mmap(), stdio output, socket sends, or other checked interfaces. Under BLOCK/FATAL,
using them while the fd remains tagged is a mismatch.

Complete and synchronize all guarded writes before releasing the token through
io_guard_exchange_token(fd, token, 0). Only after a successful release should
ordinary output, pending stdio flushes, or close/fclose proceed. Prefer flushing
ordinary buffered output before initially registering a token. Keep exclusive
control of the lifecycle across these transitions; do not temporarily disable
the process-wide policy as a per-thread workaround.

close() never clears the token, including when WARN or DISABLED permits closing.
A reused fd number can retain that token because open() does not reset it.
fdsan ownership remains independent and its existing closing rules still apply.

**Diagnostics and coverage boundaries**

Diagnostics use HiLog and preserve the caller's errno. HiLog's internal socket
send uses a direct writev system call without io_guard validation, retaining
the cancellation point and EINTR retry behavior. Ordinary writev() calls still
undergo token validation. Nested diagnostics remain suppressed; other nested
I/O through checked libc paths, including HiLog's initialization-error output
to stderr, still follows the configured BLOCK/FATAL policy.

Memory streams have no fd output and are unaffected. Custom cookie streams are
covered only when their callbacks use a checked libc output path. Reading a
tagged input fd with read-like or transfer-source operations remains permitted.
However, non-anonymous mmap() checks the fd even for read-only mappings.

Mapping creation is checked as described above, but subsequent memory stores,
msync(), and mprotect() permission changes are not checked. Existing mappings
are not revoked by token changes. Direct user system calls and libc-private
child-process status pipes used by posix_spawn/faccessat are outside these application-output
checks. The generic syscall() entry point is unchanged. A libc path that uses a
write system call directly is covered only where an explicit check has been
added; using the same kernel syscall as write() does not itself imply coverage.

Tokens are cooperative operation markers, not security credentials. The check
does not reserve the kernel fd or prevent a concurrent close/reuse after
validation. Callers must synchronize fd lifetime and handle short writes and
underlying I/O errors.

#### **CONFORMING TO**

OpenHarmony extension; not specified by POSIX.

#### **EXAMPLES**

```c
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include "io_guard.h"

int main(void)
{
    errno = 0;
    (void)io_guard_set_error_level(IO_GUARD_BLOCK);
    if (errno != 0) {
        return 1;
    }

    int fd = open("/dev/null", O_WRONLY);
    if (fd < 0) {
        return 1;
    }
    uint64_t token = 123;
    if (io_guard_exchange_token(fd, 0, token) < 0) {
        return 1;
    }

    const char data[] = "hello\n";
    size_t done = 0;
    int failed = 0;
    while (done < sizeof(data) - 1) {
        ssize_t n = io_guard_write(fd, data + done, sizeof(data) - 1 - done, token);
        if (n < 0 && errno == EINTR) {
            continue;
        }
        if (n <= 0) {
            failed = 1;
            break;
        }
        done += (size_t)n;
    }
    if (io_guard_exchange_token(fd, token, 0) < 0) {
        return 1;
    }
    if (close(fd) < 0) {
        return 1;
    }
    return failed;
}
```

#### **COLOPHON**

This page is part of the C library user-space interface documentation.
Information about the project can be found at (https://gitcode.com/openharmony/third_party_musl/blob/master/docs/).
