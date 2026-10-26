## __LIBC_SINGLE_THREADED &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; OHOS Programmer's Manual



#### **NAME**

​       __libc_single_threaded - indicates whether the process is single-threaded.

#### **SYNOPSIS**

```c
#include <sys/single_threaded.h>

char __libc_single_threaded;
```

#### **DESCRIPTION**

​The __libc_single_threaded variable is a global flag that indicates whether the process is currently running in single-threaded mode.

The variable is initialized to 1 at process start, meaning the process is single-threaded. When the first thread is created via pthread_create(), the C library sets __libc_single_threaded to 0 to indicate that the process has become multi-threaded. This is a plain char assignment without atomic instructions or memory barriers. Once flipped to 0, the value never reverts to 1 for the remainder of the process lifetime, even if all additional threads later exit.

Applications may read this variable at runtime to perform single-threaded optimizations: for example, skipping locking on data structures that are only accessed from a single thread while the process remains single-threaded. Since the transition is one-way (1 to 0), a value of 1 read at any point guarantees that no additional threads have been or are being created; however, once the value is observed as 0, the process must be treated as multi-threaded for the rest of its lifetime.

#### **RETURN VALUE**

​Not applicable; __libc_single_threaded is a global variable of type char.

#### **ERRORS**

​Not applicable.

#### EXAMPLES

```c
#include <sys/single_threaded.h>
#include <stdio.h>

static int counter = 0;

void increment(void)
{
    if (__libc_single_threaded) {
        /* Fast path: no locking needed while single-threaded. */
        counter++;
    } else {
        /* Slow path: must lock once multi-threaded. */
        /* ... lock and increment ... */
        counter++;
    }
}

int main(void)
{
    printf("Single-threaded at start: %d\n", __libc_single_threaded);
    increment();
    return 0;
}
```

#### COLOPHON

​This page is part of the C library user-space interface documentation.
Information about the project can be found at (https://gitcode.com/openharmony/third_party_musl/blob/master/docs/).
