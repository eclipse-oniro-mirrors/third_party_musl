<!--
Copyright (c) 2026 Huawei Device Co., Ltd.
SPDX-License-Identifier: MIT
-->

## RANDOM_R        OHOS Programmer's Manual

#### **NAME**

random_r - generate a pseudo-random number using caller-owned state.

#### **SYNOPSIS**

```c
#include <stdlib.h>

int random_r(struct random_data *restrict buf, int32_t *restrict result);
```

#### **DESCRIPTION**

The random_r() function advances the generator described by buf and writes its next value, between 0 and 2147483647 inclusive, to *result. Its return value reports success or failure; it is not the generated number. The context and its state buffer are updated in place, without using the global generator or an internal lock.

buf must first be initialized by initstate_r(). A zero-filled context alone is not a usable generator. Both the context and its backing buffer must remain valid. result must point to writable storage that does not overlap either object. srandom_r() can be used to restart the sequence.

Unlike rand_r(), this interface supports different amounts of state. The 8–31-byte tier uses a one-word linear congruential generator; larger tiers use additive feedback. The capacity tiers are described in the initstate_r() documentation.

#### **RETURN VALUE**

On success, random_r() returns 0 and leaves errno unchanged. On failure, it returns -1 and sets errno to indicate the error.

#### **ERRORS**

**EINVAL**: buf or result is NULL, or an accessible context has a NULL state member, including a zero-filled context.

These failures leave the result and generator unchanged. Arbitrary uninitialized values, dangling pointers and corrupted internal fields are outside the interface contract and need not produce an error return.

#### ATTRIBUTES

| Interface | Attribute | Value |
| --- | --- | --- |
| random_r() | Thread safety | MT-Safe race:buf |
| | Signal safety | Not Safe |

Separate contexts and buffers may be used concurrently. Callers must synchronize access when sharing a context or its buffer. Copying a context does not copy the buffer to which its members point.

#### HISTORY

2026

#### NOTES

To make the declaration visible from `<stdlib.h>`, define `_GNU_SOURCE`, `_BSD_SOURCE`, or `_DEFAULT_SOURCE` before the first header include.

The interface is available on all supported device types and does not require `MUSL_EXTERNAL_FUNCTION`.

Generation and seeding use musl's algorithms. For the same seed and capacity tier, output matches musl's global initstate()/random() sequence, except that seed 0 is treated as 1. The additive generators do not reproduce glibc's sequences.

The buffer metadata format follows glibc and differs from musl's global generator. Do not exchange buffers with global initstate()/setstate(). The two families maintain independent state.

#### CONFORMING TO

random_r() is a GNU extension and is not specified by ISO C or POSIX.

#### EXAMPLES

```c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    /*
     * Expected output:
     * 262836907
     * 2022765545
     * 1985587709
     */
    struct random_data buf = {0};
    char state[128];
    int32_t value;

    if (initstate_r(1, state, sizeof state, &buf) != 0) {
        return 1;
    }
    for (int i = 0; i < 3; i++) {
        if (random_r(&buf, &value) != 0) {
            return 1;
        }
        printf("%ld\n", (long)value);
    }
    return 0;
}
```

#### COLOPHON

This page is part of the C library user-space interface documentation.
Information about the project can be found at (https://gitcode.com/openharmony/third_party_musl/blob/master/docs/).
