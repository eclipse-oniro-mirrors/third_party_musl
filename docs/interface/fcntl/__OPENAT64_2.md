## __OPENAT64_2 &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; OHOS Programmer's Manual



#### **NAME**

​       __openat64_2 - fortify-checked openat64 with compile-time argument validation.

#### **SYNOPSIS**

```c
#include <fcntl.h>

int __openat64_2(int fd, const char *pathname, int flags);
```

#### **DESCRIPTION**

​The __openat64_2() function is the FORTIFY runtime entry point for the three-argument form of openat64(). It is defined in the same source file as __openat64_chk() (src/fortify/linux/fortify.c) and shares the same implementation logic. It is invoked by the compiler-generated fortify inline when openat64() is called with three arguments (fd, pathname, flags) and without a mode argument.

The function checks whether the flags argument includes O_CREAT or O_TMPFILE. If either flag is present, a mode argument is required by the underlying openat64() system call. In that case, __openat64_2() calls __fortify_error() with a diagnostic message indicating that too few arguments were supplied, and the process is terminated via abort(). The error message is also stored as the process fatal message for post-mortem diagnostics.

If the flags argument does not require a mode (neither O_CREAT nor O_TMPFILE is set), the function proceeds to call the real openat64() system call with a mode of 0 and returns its result. The O_LARGEFILE flag is forced on 32-bit architectures before the call.

This function is part of the FORTIFY source hardening mechanism and is not intended to be called directly by application code. The compiler inserts calls to it automatically when <fcntl.h> is included with fortify support enabled.

The function is declared in <fcntl.h> and available when _LARGEFILE64_SOURCE or _GNU_SOURCE is defined.

#### **RETURN VALUE**

​On success, __openat64_2() returns the new file descriptor.

If the flags require a mode argument (O_CREAT or O_TMPFILE is set), the function does not return; the process is aborted.

On failure from the underlying openat64() call, it returns -1 and sets errno to indicate the error.

#### **ERRORS**

​The function terminates the process (via abort()) when O_CREAT or O_TMPFILE is present in flags but no mode argument was supplied. The diagnostic is reported through __fortify_error().

Errors from the underlying openat64() system call are propagated as-is.

#### ATTRIBUTES

| Interface | Attribute     | Value          |
| --------- | ------------- | -------------- |
| __openat64_2() | Thread safety | MT-Safe |
|             | Async-signal-safety | AS-Safe (unless __fortify_error is triggered) |

When __fortify_error() is triggered, the function calls abort(), which is not async-signal-safe.

#### EXAMPLES

```c
#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main(void)
{
    /*
     * The compiler rewrites this two-argument openat64() call into
     * __openat64_2(dirfd, path, flags). Since O_CREAT is not set,
     * no mode argument is required and the call succeeds.
     */
    int fd = openat64(AT_FDCWD, "/dev/null", O_RDONLY);
    if (fd < 0) {
        perror("openat64");
        return 1;
    }
    printf("Opened fd: %d\n", fd);
    close(fd);
    return 0;
}
```

#### COLOPHON

​This page is part of the C library user-space interface documentation.
Information about the project can be found at (https://gitcode.com/openharmony/third_party_musl/blob/master/docs/).
