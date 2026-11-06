## INITSTATE_R        OHOS Programmer's Manual

#### **NAME**

initstate_r - initialize a pseudo-random generator in caller-owned memory.

#### **SYNOPSIS**

```c
#include <stdlib.h>

int initstate_r(unsigned seed, char *restrict state, size_t size,
    struct random_data *restrict buf);
```

#### **DESCRIPTION**

The initstate_r() function initializes the buffer pointed to by state using seed and records pointers into that buffer in buf. The context is an input/output object. Before the first call, set buf->state to NULL or initialize the entire context to zero. After a successful call, buf can be used by random_r() and srandom_r(). A seed of 0 is treated as 1.

size is the available buffer capacity in bytes, including a four-byte metadata header. The generator is selected as follows:

| Buffer size in bytes | Generator | 32-bit state words, excluding header |
| --- | --- | --- |
| 8–31 | Linear congruential | 1 |
| 32–63 | Additive feedback | 7 |
| 64–127 | Additive feedback | 15 |
| 128–255 | Additive feedback | 31 |
| 256 or more | Additive feedback | 63 |

Increasing capacity within a tier does not change the generator or its period. Capacity above 256 bytes does not select another tier. Byte-aligned buffers are supported. The context and buffer must not overlap.

The function allocates no memory. The selected buffer must remain valid while the context uses it. On reinitialization, the previous context and buffer must still be valid: the function saves the old position in the old buffer's header before initializing the supplied buffer. Reusing the same buffer is supported. After a successful switch to a different buffer, the old buffer may be released.

#### **RETURN VALUE**

On success, initstate_r() returns 0 and leaves errno unchanged. On failure, it returns -1 and sets errno to indicate the error.

#### **ERRORS**

**EINVAL**: buf or state is NULL, or size is less than eight bytes.

With a valid existing context and a non-NULL new buffer, a too-small size fails after saving the old position in the old header. The context and random sequence remain usable. Arbitrary invalid pointers and corrupted contexts are outside the interface contract.

#### ATTRIBUTES

| Interface | Attribute | Value |
| --- | --- | --- |
| initstate_r() | Thread safety | MT-Safe race:buf |
| | Signal safety | Not Safe |

Independent contexts with independent buffers may be initialized concurrently. Callers must synchronize access when sharing either object. Copying a context does not copy its backing buffer.

#### HISTORY

2026

#### NOTES

To make the declaration visible from `<stdlib.h>`, define `_GNU_SOURCE`, `_BSD_SOURCE`, or `_DEFAULT_SOURCE` before the first header include.

The interface is available on all supported device types and does not require `MUSL_EXTERNAL_FUNCTION`.

Generation and seeding use musl's algorithms, with seed 0 normalized to 1. For the same capacity tier, the sequence matches musl's global initstate()/random() sequence when the global seed is `(seed != 0) ? seed : 1`. Additive sequences do not reproduce glibc's sequences.

The public structure and buffer metadata format follow glibc. Buffers cannot be exchanged with musl's global initstate()/setstate() interfaces. This addition does not provide setstate_r().

#### CONFORMING TO

initstate_r() is a GNU extension and is not specified by ISO C or POSIX.

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
     */
    struct random_data buf = {0};
    char state[128];
    int32_t value;

    if (initstate_r(1, state, sizeof state, &buf) != 0) {
        return 1;
    }
    if (random_r(&buf, &value) != 0) {
        return 1;
    }
    printf("%ld\n", (long)value);
    return 0;
}
```

#### COLOPHON

This page is part of the C library user-space interface documentation.
Information about the project can be found at (https://gitcode.com/openharmony/third_party_musl/blob/master/docs/).
