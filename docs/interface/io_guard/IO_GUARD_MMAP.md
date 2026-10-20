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

## IO_GUARD_MMAP &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; OHOS Programmer's Manual

#### **NAME**

io_guard_mmap - create a mapping using an expected io_guard token.

#### **SYNOPSIS**

```c
#include <sys/mman.h>
#include "io_guard.h"

void *io_guard_mmap(void *addr, size_t length, int prot, int flags,
                    int fd, off_t offset, uint64_t token);
```

#### **DESCRIPTION**

Validates fd ownership for every non-anonymous mapping, regardless of prot or
whether flags specifies MAP_SHARED, MAP_SHARED_VALIDATE, or MAP_PRIVATE. In the
malloc-hook build, ordinary mmap()/mmap64() require token 0; io_guard_mmap()
instead checks the supplied token and directly performs the mapping syscall
without entering a second expected-zero check. Read-only and PROT_NONE mappings
are also checked.

Anonymous mappings (MAP_ANONYMOUS/MAP_ANON) ignore fd and do not check fd
ownership, even when a nonnegative tagged fd is supplied. For all mapping types,
token 0 and UINT64_MAX remain invalid arguments to io_guard_mmap(), even under
DISABLED.

| Error level | Token mismatch behavior for a checked mapping |
| ----------- | --------------------------------------------- |
| IO_GUARD_DISABLED | Allow without diagnostics |
| IO_GUARD_WARN | Report through HiLog and allow |
| IO_GUARD_BLOCK | Report and return MAP_FAILED/EPERM without creating the mapping |
| IO_GUARD_FATAL | Report and abort the process (SIGABRT) |

A matching token permits the underlying mapping operation, which can still
fail. No token is changed. Offset validation, length checks, MAP_FIXED handling,
architecture-specific mmap/mmap2 offset conversion, and syscall error handling
follow the same rules as the existing libc mmap implementation.

#### **PARAMETERS**

**addr**, **length**, **prot**, **flags**, **fd**, **offset**: Same meaning and
requirements as mmap(). Offset is expressed in bytes.

**token**: Expected registered nonzero token; UINT64_MAX is reserved.

#### **RETURN VALUE**

Returns the mapped address on success. On failure, returns MAP_FAILED, not NULL,
and sets errno. A FATAL mismatch does not return.

#### **ERRORS**

**EINVAL**: Invalid token, or invalid mapping arguments reported by mmap().

**EPERM**: Ownership mismatch under IO_GUARD_BLOCK; also possible from the kernel.

Other errors are inherited from mmap(). Ownership validation precedes mapping
argument validation and can therefore take precedence over underlying errors.

#### **ATTRIBUTES**

| Interface | Attribute | Value |
| --------- | --------- | ----- |
| io_guard_mmap() | Thread safety | MT-safe |
| | Signal safety | Not guaranteed |

#### **HISTORY**

       -- 2026

#### **NOTES**

System-side inner API, not a public NDK interface. Requires musl_enable_io_guard;
the declaration is visible only with IO_GUARD_ENABLE. GN callers can depend on
`//third_party/musl:io_guard_inner`. Disabled builds provide no symbol or stub.

The check is in the public mmap() entry. In the malloc-hook build it runs before
the dispatcher, so a replacement mmap callback cannot bypass validation. Builds
without malloc hooks do not check mmap()/mmap64() ownership; only io_guard_mmap()
retains validation there. io_guard_mmap() independently performs the mapping
syscall rather than invoking mmap() or the replacement callback.

Checking happens only at mapping creation. Later memory stores, msync(), and
mprotect() changes are not checked. Existing mappings are not revoked when a
token changes, the policy changes, or the fd is closed. Direct system calls are
also outside this check. This is diagnostic ownership checking, not memory
write isolation or a complete file-write security boundary.

Callers must coordinate mapping lifetime, token transfer, and close/reuse.
Releasing a token does not unmap memory. Stop access and unmap a writable mapping
before transferring ownership when subsequent writes must be prevented.

#### **CONFORMING TO**

OpenHarmony extension; not specified by POSIX.

#### **SEE ALSO**

io_guard_write(), io_guard_pwrite(), io_guard_exchange_token(),
io_guard_set_error_level(), mmap(), mprotect(), munmap(), msync()
