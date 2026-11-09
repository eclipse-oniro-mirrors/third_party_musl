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

## IO_GUARD_SET_ERROR_LEVEL &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; OHOS Programmer's Manual

#### **NAME**

io_guard_set_error_level - set the process-wide io_guard error policy.

#### **SYNOPSIS**

```c
#include "io_guard.h"

enum io_guard_error_level {
    IO_GUARD_DISABLED = 0,
    IO_GUARD_WARN = 1,
    IO_GUARD_BLOCK = 2,
    IO_GUARD_FATAL = 3
};

enum io_guard_error_level io_guard_set_error_level(enum io_guard_error_level level);
```

#### **DESCRIPTION**

Atomically replaces the process-wide token-checking policy. The default is
**IO_GUARD_WARN** when support is enabled.

During OpenHarmony dynamic-loader startup, the internal initializer reads
**musl.debug.io_guard** after the parameter client has been initialized:

| Parameter value | Initial policy |
| --------------- | -------------- |
| disabled | IO_GUARD_DISABLED |
| warn | IO_GUARD_WARN |
| block | IO_GUARD_BLOCK |
| fatal | IO_GUARD_FATAL |

Values are case-sensitive. A missing, unreadable, empty, or unrecognized value,
a parameter-handle creation failure, or a build without **OHOS_ENABLE_PARAMETER**
uses **IO_GUARD_WARN**. The parameter is read only at startup; changes are not
automatically applied to a running process. Subsequent calls to
io_guard_set_error_level() override the startup policy. Startup paths that do not
run this dynamic-loader initializer retain the static default **IO_GUARD_WARN**.
The initializer does not change fd tokens or the fork-child disablement behavior.

| Level | Effect of a token mismatch in checked fd output, close(), or io_guard_write() |
| ----- | -------------------------------------------------------------------- |
| IO_GUARD_DISABLED | Allow without diagnostics |
| IO_GUARD_WARN | Report through HiLog and immediately allow; no BLOCK/FATAL handling |
| IO_GUARD_BLOCK | Report and reject the checked operation with EPERM; the caller-facing result follows the interface contract |
| IO_GUARD_FATAL | Report and abort the process (SIGABRT) |

Ordinary checked fd-output paths, non-anonymous mappings, and close()
expect token 0. io_guard_write(), io_guard_pwrite(), and io_guard_mmap() check
the supplied valid nonzero token for their covered operations. All these paths only validate,
including close(). The policy applies to all integrated ordinary output paths,
not just write(): positional/vectored output, standard FILE output and flushing,
socket sends, fd transfers, and message-queue sends. Asynchronous requests use
the policy at execution time through write()/pwrite(). See io_guard_write() for
the complete validation-point table, including all pwritev2() output branches.
mq_close() also requires token 0. Empty sendmmsg() batches skip io_guard validation.

For checked mappings, BLOCK returns MAP_FAILED/EPERM at mapping creation;
existing mappings and later memory stores are unaffected. For direct writes,
BLOCK returns -1/EPERM. stdio reports its stream error and
normal failure return; asynchronous writes report EPERM through aio_error().
Batched or buffered operations may have already completed earlier output;
rejection is not a rollback. FATAL terminates the process rather than returning
an I/O error. No report is issued for a matching token or DISABLED checking.

Changing the level does not clear or modify tokens. io_guard_exchange_token() continues
to require an exact expected-token match under every policy.

#### **PARAMETERS**

**level**: One of the four values of **enum io_guard_error_level**.

#### **RETURN VALUE**

On success, returns the previous error level, not a 0/-1 status code.

For an invalid level, returns the current level and sets errno to **EINVAL**;
the policy remains unchanged. When a vfork() child supplies a valid level,
returns **IO_GUARD_DISABLED** and sets
errno to **ENOTSUP**, without changing the policy.

Success does not clear errno. Set errno to 0 before calling when checking for
errors; the returned level alone does not indicate success or failure.

#### **ERRORS**

**EINVAL**: Invalid level when io_guard support is enabled.

**ENOTSUP**: A vfork() child attempts to change the policy with a valid level.

#### **ATTRIBUTES**

| Interface | Attribute | Value |
| --------- | --------- | ----- |
| io_guard_set_error_level() | Thread safety | MT-safe |
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

The policy affects all threads. It is not a per-thread bypass and should be
configured under application-level coordination. The level returned by the
setter is the previous global policy, not a policy scoped to one fd or thread.
A level change does not retroactively check completed writes or flush buffers.
Buffered output and asynchronous requests observe the policy used when their
actual write path checks the token.

After fork(), the child starts with checking DISABLED but retains inherited
tokens. Re-enabling checking uses those tokens again; the parent's policy and
tokens are unaffected. A vfork() child cannot change the shared policy.

Disabling checking does not make it safe to close a tagged descriptor without
first releasing its token: close() leaves the token behind.

#### **CONFORMING TO**

OpenHarmony extension; not specified by POSIX.

#### **EXAMPLES**

```c
#include <errno.h>
#include "io_guard.h"

int main(void)
{
    errno = 0;
    enum io_guard_error_level previous = io_guard_set_error_level(IO_GUARD_BLOCK);
    if (errno != 0) {
        return 1;
    }
    (void)previous; /* Previous policy, not a success code. */
    return io_guard_get_error_level() != IO_GUARD_BLOCK;
}
```

#### **COLOPHON**

This page is part of the C library user-space interface documentation.
Information about the project can be found at (https://gitcode.com/openharmony/third_party_musl/blob/master/docs/).
