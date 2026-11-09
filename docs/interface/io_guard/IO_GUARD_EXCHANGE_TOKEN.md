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

## IO_GUARD_EXCHANGE_TOKEN &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; OHOS Programmer's Manual

#### **NAME**

io_guard_exchange_token - compare and replace a file descriptor token.

#### **SYNOPSIS**

```c
#include "io_guard.h"

int io_guard_exchange_token(int fd, uint64_t expected_token, uint64_t new_token);
```

#### **DESCRIPTION**

Atomically compares the token associated with **fd** with **expected_token**.
Only a matching token is replaced with **new_token**. A mismatch leaves the
token unchanged, regardless of the configured error level.

Token **0** means unowned; **UINT64_MAX** is reserved. The supported transitions are:

| Transition | Purpose |
| ---------- | ------- |
| 0 -> nonzero token | Register after open() |
| old token -> new token | Transfer ownership |
| nonzero token -> 0 | Release before close() |

This is the only token-changing interface. Checked output interfaces,
io_guard_write(), io_guard_pwrite(), io_guard_mmap(), and close() only validate tokens; none releases or changes
a token. See io_guard_write() for the covered output paths.

#### **PARAMETERS**

**fd**: Descriptor number within the fdsan fd table range: 0 through the
**RLIMIT_NOFILE** hard limit (32768 when the limit is infinite). It must refer
to an open descriptor when **new_token** is nonzero.

**expected_token**: Expected current token, including 0 for an unowned descriptor.

**new_token**: Replacement token, including 0 to release ownership.

Neither token argument may be **UINT64_MAX**.

#### **RETURN VALUE**

On success, returns 0. On failure, returns -1 and sets errno.

#### **ERRORS**

**EBADF**: Descriptor number is negative or outside the fdsan fd table range,
or **new_token** is nonzero and the descriptor is not open.

**EINVAL**: Either token argument is **UINT64_MAX**.

**EPERM**: The current token does not match **expected_token**. This remains a
returned error even under **IO_GUARD_FATAL**; it does not abort the process.

**ENOTSUP**: The caller is a vfork() child.

Resource allocation failures may also be reported, such as **ENOMEM**.

#### **ATTRIBUTES**

| Interface | Attribute | Value |
| --------- | --------- | ----- |
| io_guard_exchange_token() | Thread safety | MT-safe |
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

Callers must synchronize writes, token transfers, close(), and descriptor reuse.
Atomic token replacement does not reserve the underlying kernel descriptor.

Registering a nonzero token does not authorize ordinary output from the same
thread or owner. Integrated pwrite/writev, stdio, socket, transfer, and other
ordinary output paths expect token 0. io_guard_write(), io_guard_pwrite(), and
io_guard_mmap() accept the owner's nonzero token for their covered operations. See io_guard_write() for the validation-point table, failure
behavior, and the pwritev2() validation paths.

Before registering a token, flush any pending ordinary FILE output where
possible. Before transferring or releasing ownership, synchronize in-flight
writes and asynchronous requests. To resume ordinary output or flush/close a
stream, stop guarded users and successfully exchange the token to 0 first.
Do not assume that buffering data through fwrite() has already performed the
fd check: that check may occur only during a later flush.

Release ownership successfully before closing. A close() allowed by WARN or
DISABLED leaves the token intact, and open() does not reset it; a reused descriptor
number may therefore retain an old token. Releasing a stale token after close()
is permitted only while exclusively controlling that descriptor number's lifetime.

io_guard tokens and fdsan tags are independent and can coexist. Release the
io_guard token before closing through the existing fdsan owner's close operation.

#### **CONFORMING TO**

OpenHarmony extension; not specified by POSIX.

#### **EXAMPLES**

```c
#include <fcntl.h>
#include <unistd.h>
#include "io_guard.h"

int main(void)
{
    int fd = open("/dev/null", O_WRONLY);
    if (fd < 0) {
        return 1;
    }
    /* On ownership errors, do not blindly close a possibly reused fd. */
    if (io_guard_exchange_token(fd, 0, 123) < 0) {
        return 1;
    }
    if (io_guard_exchange_token(fd, 123, 456) < 0) {
        return 1;
    }
    if (io_guard_exchange_token(fd, 456, 0) < 0) {
        return 1;
    }
    return close(fd) < 0;
}
```

#### **COLOPHON**

This page is part of the C library user-space interface documentation.
Information about the project can be found at (https://gitcode.com/openharmony/third_party_musl/blob/master/docs/).
