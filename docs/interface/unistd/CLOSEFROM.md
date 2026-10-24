## CLOSEFROM &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; OHOS Programmer's Manual



#### **NAME**

​       closefrom - close all open file descriptors starting from a given value.

#### **SYNOPSIS**

```c
#include <unistd.h>

void closefrom(int lowfd);
```

#### **DESCRIPTION**

​The closefrom() function closes all open file descriptors greater than or equal to lowfd.

If lowfd is negative, it is treated as zero.

The function first attempts to use the kernel close_range system call, passing the range [lowfd, UINT_MAX] with no flags. If the kernel supports close_range and the call succeeds, the function returns immediately.

If the kernel does not support the close_range system call, the function falls back to a loop that closes each descriptor individually via the close system call. The loop is bounded by the process file descriptor table size obtained from getdtablesize().

#### **RETURN VALUE**

​The closefrom() function does not return a value.

#### **ERRORS**

#### ATTRIBUTES

| Interface | Attribute     | Value          |
| --------- | ------------- | -------------- |
| closefrom() | Thread safety | MT-Safe |
|             | Async-signal-safety | AS-Safe |

#### EXAMPLES

```c
#include <unistd.h>
#include <stdio.h>

int main(void)
{
    /* Close all file descriptors from 3 upward. */
    closefrom(3);
    return 0;
}
```

#### COLOPHON

​This page is part of the C library user-space interface documentation.
Information about the project can be found at (https://gitcode.com/openharmony/third_party_musl/blob/master/docs/).
