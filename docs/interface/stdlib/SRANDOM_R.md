## SRANDOM_R        OHOS Programmer's Manual

#### **NAME**

srandom_r - restart an initialized pseudo-random generator with a seed.

#### **SYNOPSIS**

```c
#include <stdlib.h>

int srandom_r(unsigned seed, struct random_data *buf);
```

#### **DESCRIPTION**

The srandom_r() function reseeds a context previously initialized by initstate_r(). It rewrites the generator state and resets its position, preserving its buffer and capacity tier. Subsequent calls to random_r() start at the beginning of the new sequence.

For the same seed and capacity tier, reseeding produces the same sequence as initializing a fresh context. The previously consumed sequence and the old seed do not affect the restarted sequence. Seed 0 is equivalent to seed 1.

This function neither allocates storage nor initializes an empty context. The context and its state buffer must remain valid throughout use. It does not alter the global random() generator or any independent context.

#### **RETURN VALUE**

On success, srandom_r() returns 0. On failure, it returns -1. Both successful and failed calls leave errno unchanged.

#### **ERRORS**

This function does not set errno.

A NULL context or a generator type outside 0 through 4 causes srandom_r() to return -1. This implementation also rejects an accessible context with a NULL state member, including a zero-filled context. Arbitrary invalid pointers and corrupted state fields are outside the interface contract; these checks do not validate every field.

#### ATTRIBUTES

| Interface | Attribute | Value |
| --- | --- | --- |
| srandom_r() | Thread safety | MT-Safe race:buf |
| | Signal safety | Not Safe |

Separate contexts and buffers may be reseeded concurrently. Callers must synchronize access to a shared context or buffer, including access through random_r() and initstate_r().

#### HISTORY

2026

#### NOTES

To make the declaration visible from `<stdlib.h>`, define `_GNU_SOURCE`, `_BSD_SOURCE`, or `_DEFAULT_SOURCE` before the first header include.

The interface is available on all supported device types and does not require `MUSL_EXTERNAL_FUNCTION`.

Seeding uses musl's algorithm with the glibc convention that seed 0 means 1. For a matching capacity tier, compare against musl's global srandom() using `(seed != 0) ? seed : 1`. Reproducing glibc's additive output sequence is not part of this interface's contract.

The error return convention follows glibc 2.42: a failure does not set errno. The context and buffer formats are described in the initstate_r() documentation.

#### CONFORMING TO

srandom_r() is a GNU extension and is not specified by ISO C or POSIX.

#### EXAMPLES

```c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    /*
     * Expected output:
     * 262836907 262836907
     */
    struct random_data buf = {0};
    char state[128];
    int32_t first, next, repeated;

    if (initstate_r(1, state, sizeof state, &buf) != 0) {
        return 1;
    }
    if (random_r(&buf, &first) != 0 || random_r(&buf, &next) != 0) {
        return 1;
    }
    if (srandom_r(1, &buf) != 0 || random_r(&buf, &repeated) != 0) {
        return 1;
    }
    printf("%ld %ld\n", (long)first, (long)repeated);
    return first != repeated;
}
```

#### COLOPHON

This page is part of the C library user-space interface documentation.
Information about the project can be found at (https://gitcode.com/openharmony/third_party_musl/blob/master/docs/).
