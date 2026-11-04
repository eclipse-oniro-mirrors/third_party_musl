## SETAPPUID        OHOS Programmer's Manual

#### **NAME**

       setappuid - set the application user identifier.

#### **SYNOPSIS**

```c
#include <unistd.h>

int setappuid(uid_t uid);
```

#### **DESCRIPTION**

The setappuid() function sets the application user identifier (app uid) of the calling process to the value specified by uid.

When musl_extended_function support is enabled, setappuid() applies the app uid change to all threads of the process. When musl_extended_function support is disabled, setappuid() is equivalent to setuid(uid) and sets the user id of the calling process.

#### **RETURN VALUE**

On success, setappuid() returns 0.

When musl_extended_function is disabled, or when it is enabled but the change fails on all threads (so no thread applied the change), setappuid() falls back to setuid(uid) and returns its result (0 on success, -1 on error with errno set).

When musl_extended_function is enabled and a partial thread failure occurs (some threads updated the app uid successfully, others failed), setappuid() does not return: the process is terminated with SIGKILL to avoid an inconsistent app-uid state across threads.

#### **ERRORS**

**EPERM**: The calling process does not have the required privileges. This is propagated from the setuid() fallback path when musl_extended_function is disabled or when the change fails on all threads.

**EINVAL**: The uid argument is invalid. This is propagated from the setuid() fallback path.

When musl_extended_function is enabled and setappuid() succeeds, it does not set errno. When a partial thread failure triggers SIGKILL, the process is terminated before any errno is reported.

#### ATTRIBUTES

| Interface   | Attribute     | Value          |
| ----------- | ------------- | -------------- |
| setappuid() | Thread safety | MT-Safe        |
|             | Signal safety | Not Safe       |

#### HISTORY

-- 2026

#### **NOTES**

This feature is designed specifically for when musl_extended_function is true. When musl_extended_function is disabled, setappuid() is equivalent to setuid(uid).

#### **CONFORMING TO**

This is a platform-specific extension and is not part of any POSIX standard.

#### EXAMPLES

```c
#include <unistd.h>
#include <stdio.h>

int main(void)
{
    /* Set the app uid to 180 (requires appropriate privileges). */
    int ret = setappuid(180);
    if (ret == 0) {
        printf("app uid is now: %u\n", getappuid());
    } else {
        printf("setappuid failed, ret = %d\n", ret);
    }

    return 0;
}
```

#### COLOPHON

This page is part of the C library user-space interface documentation.
Information about the project can be found at (https://gitcode.com/openharmony/third_party_musl/blob/master/docs/).
