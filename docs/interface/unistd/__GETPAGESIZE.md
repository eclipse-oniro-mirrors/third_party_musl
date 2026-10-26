## __GETPAGESIZE &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; OHOS Programmer's Manual



#### **NAME**

​       __getpagesize - return the system page size.

#### **SYNOPSIS**

```c
#include <unistd.h>

int __getpagesize(void);
```

#### **DESCRIPTION**

​The __getpagesize() function returns the system page size in bytes. It is the internal implementation counterpart to getpagesize().

​The page size is obtained via the PAGE_SIZE macro. On architectures where PAGE_SIZE is predefined by arch headers, it is a compile-time constant. Otherwise, PAGE_SIZE is defined as `libc.page_size`, a runtime value set during C library initialization.

#### **RETURN VALUE**

​The __getpagesize() function returns the system page size in bytes.

#### **ERRORS**

#### ATTRIBUTES

| Interface | Attribute     | Value          |
| --------- | ------------- | -------------- |
| __getpagesize() | Thread safety | MT-Safe |
|             | Async-signal-safety | AS-Safe |

#### EXAMPLES

```c
#include <unistd.h>
#include <stdio.h>

int main(void)
{
    int ps = __getpagesize();
    printf("Page size: %d bytes\n", ps);
    return 0;
}
```

#### COLOPHON

​This page is part of the C library user-space interface documentation.
Information about the project can be found at (https://gitcode.com/openharmony/third_party_musl/blob/master/docs/).
