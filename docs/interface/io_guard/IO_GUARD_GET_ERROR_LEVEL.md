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

## IO_GUARD_GET_ERROR_LEVEL &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; OHOS Programmer's Manual

#### **NAME**

io_guard_get_error_level - query the process-wide io_guard error policy.

#### **SYNOPSIS**

```c
#include "io_guard.h"

enum io_guard_error_level io_guard_get_error_level(void);
```

#### **DESCRIPTION**

Returns the current io_guard error level without changing the policy or tokens.
See io_guard_set_error_level() for the policy values and mismatch behavior.

The queried policy is shared by all integrated fd-output, non-anonymous
mapping, and close paths, not only write()/io_guard_write(). See io_guard_write() for the covered interfaces,
shared validation points, and coverage limitations.

#### **PARAMETERS**

None.

#### **RETURN VALUE**

Returns **IO_GUARD_DISABLED**, **IO_GUARD_WARN**,
**IO_GUARD_BLOCK**, or **IO_GUARD_FATAL**.

Returns **IO_GUARD_DISABLED** in a vfork() child. The default is **IO_GUARD_WARN**;
OpenHarmony dynamic-loader startup may override it from **musl.debug.io_guard**
as described in io_guard_set_error_level(). A fork() child starts with
**IO_GUARD_DISABLED**.

#### **ERRORS**

No errors are reported. errno is not changed.

#### **ATTRIBUTES**

| Interface | Attribute | Value |
| --------- | --------- | ----- |
| io_guard_get_error_level() | Thread safety | MT-safe |
| | Signal safety | Not guaranteed |

#### **HISTORY**

       -- 2026

#### **NOTES**

This is a system-side inner API declared in `interfaces/inner_api/io_guard.h`,
not a public NDK header.

Functional support requires libc to be built with
**musl_enable_io_guard** enabled. When disabled, the io_guard API symbols are
not linked into libc; there are no fallback implementations. Callers must
exclude these calls when building against a libc without io_guard support.
The header exposes the io_guard types, constants, and function declarations only
when **IO_GUARD_ENABLE** is defined. GN callers should depend on
`//third_party/musl:io_guard_inner`, which propagates the include path and defines
the macro only when io_guard support is enabled in the build. Defining the
macro in a caller alone does not add support to a libc built without io_guard.

This query reports runtime policy only. It cannot be used to probe a build
without io_guard support because the query symbol itself is absent.

The result is a snapshot. Another thread can change the process-wide policy
after this call returns. DISABLED does not imply that registered tokens have
been cleared.

A BLOCK or FATAL result is not proof that every way of writing the fd is
intercepted. It reports the selected policy, not per-interface coverage or fd
ownership. Buffered stdio and asynchronous output can be checked later than
this query, using the policy in effect at their actual validation point.

#### **CONFORMING TO**

OpenHarmony extension; not specified by POSIX.

#### **EXAMPLES**

```c
#include <stdio.h>
#include "io_guard.h"

int main(void)
{
    enum io_guard_error_level level = io_guard_get_error_level();
    printf("io_guard error level: %d\n", (int)level);
    return 0;
}
```

#### **COLOPHON**

This page is part of the C library user-space interface documentation.
Information about the project can be found at (https://gitcode.com/openharmony/third_party_musl/blob/master/docs/).
