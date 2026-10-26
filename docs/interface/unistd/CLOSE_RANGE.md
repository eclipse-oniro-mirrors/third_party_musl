## CLOSE_RANGE &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; OHOS Programmer's Manual



#### **NAME**

​       close_range - close a range of file descriptors.

#### **SYNOPSIS**

```c
#include <unistd.h>

int close_range(unsigned int first, unsigned int last, int flags);
```

#### **DESCRIPTION**

​The close_range() function closes all open file descriptors in the inclusive range [first, last].

If the kernel supports the close_range system call, the function delegates to it directly.

If the kernel does not support the close_range system call, the function falls back to a loop that closes each descriptor individually via the close system call. In this fallback path, flags must be zero; a nonzero flags value is rejected with EINVAL. The loop is bounded by the process file descriptor table size obtained from getdtablesize().

#### **RETURN VALUE**

​On success, close_range() returns 0.

On failure, it returns -1 and sets errno to indicate the error.

#### **ERRORS**

**EINVAL**: first is greater than last, or flags is nonzero in the fallback path (when the kernel does not provide the close_range system call).

#### ATTRIBUTES

| Interface | Attribute     | Value          |
| --------- | ------------- | -------------- |
| close_range() | Thread safety | MT-Safe |
|             | Async-signal-safety | AS-Safe |

#### EXAMPLES

```c
#include <unistd.h>
#include <stdio.h>

int main(void)
{
    /* Close all file descriptors from 3 upward. */
    if (close_range(3, ~0U, 0) < 0) {
        perror("close_range");
        return 1;
    }
    return 0;
}
```

#### COLOPHON

​This page is part of the C library user-space interface documentation.
Information about the project can be found at (https://gitcode.com/openharmony/third_party_musl/blob/master/docs/).
