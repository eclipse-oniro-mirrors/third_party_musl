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

## IO_GUARD_PWRITE &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; OHOS Programmer's Manual

#### **NAME**

io_guard_pwrite - write at an offset using an expected io_guard token.

#### **SYNOPSIS**

```c
#include "io_guard.h"

ssize_t io_guard_pwrite(int fd, const void *buf, size_t count,
                       off_t offset, uint64_t token);
```

#### **DESCRIPTION**

Checks the supplied token and performs a positional write without reentering
pwrite()'s expected-zero check. The current file offset and registered token
are not modified. Positional I/O behavior, including append-mode behavior,
is otherwise inherited from the underlying pwrite operation.

A matching token allows the operation. On mismatch, DISABLED allows without
reporting, WARN reports through HiLog and allows, BLOCK returns -1/EPERM,
and FATAL reports and aborts. Token 0 and UINT64_MAX are rejected with EINVAL
under every error level, including DISABLED.

#### **PARAMETERS**

**fd**: Destination file descriptor.

**buf**: Buffer containing the bytes to write.

**count**: Requested byte count.

**offset**: File offset in bytes, with the same requirements as pwrite().

**token**: Expected registered nonzero token; UINT64_MAX is reserved.

#### **RETURN VALUE**

Returns the number of bytes written, possibly fewer than count. On failure,
returns -1 and sets errno. A FATAL mismatch does not return.

#### **ERRORS**

**EINVAL**: Invalid token; also possible for an invalid underlying offset.

**EPERM**: Token mismatch under IO_GUARD_BLOCK.

Other errors are inherited from pwrite(). Token validation precedes the syscall.

#### **ATTRIBUTES**

| Interface | Attribute | Value |
| --------- | --------- | ----- |
| io_guard_pwrite() | Thread safety | MT-safe |
| | Signal safety | Not guaranteed |

#### **HISTORY**

       -- 2026

#### **NOTES**

System-side inner API, not a public NDK interface. Requires musl_enable_io_guard;
the declaration is visible only with IO_GUARD_ENABLE. GN callers can depend on
`//third_party/musl:io_guard_inner`. Disabled builds provide no symbol or stub.

Callers must synchronize fd lifetime, token transfer, and descriptor reuse.
Token validation does not pin a kernel descriptor or provide atomicity with
concurrent close(). Ordinary pwrite()/pwrite64() still require token 0.

#### **CONFORMING TO**

OpenHarmony extension; not specified by POSIX.

#### **SEE ALSO**

io_guard_write(), io_guard_mmap(), io_guard_exchange_token(),
io_guard_set_error_level(), pwrite()
