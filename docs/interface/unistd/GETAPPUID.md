## GETAPPUID        OHOS Programmer's Manual

#### **NAME**

       getappuid - return the application user identifier.

#### **SYNOPSIS**

```c
#include <unistd.h>

uid_t getappuid(void);
```

#### **DESCRIPTION**

The getappuid() function returns the application user identifier (app uid) of the calling process.

When musl_extended_function support is enabled, getappuid() returns the app uid managed by the access token framework. When musl_extended_function support is disabled, getappuid() is equivalent to getuid() and returns the user id of the calling process.

getappuid() returns the value most recently set by setappuid(). If setappuid() has never been called, the return value is unspecified.

#### **RETURN VALUE**

The getappuid() function returns the application user identifier.

When musl_extended_function is disabled, or when it is enabled but the access token framework query fails, getappuid() falls back to getuid() and returns the user id of the calling process.

#### **ERRORS**

No errors are defined. getappuid() does not use errno as an error indicator: on any internal failure it silently falls back to getuid().

#### ATTRIBUTES

| Interface   | Attribute     | Value          |
| ----------- | ------------- | -------------- |
| getappuid() | Thread safety | MT-Safe        |
|             | Signal safety | Not Safe       |

#### HISTORY

-- 2026

#### **NOTES**

This feature is designed specifically for when musl_extended_function is true. When musl_extended_function is disabled, getappuid() is equivalent to getuid().

#### **CONFORMING TO**

This is a platform-specific extension and is not part of any POSIX standard.

#### EXAMPLES

```c
#include <unistd.h>
#include <stdio.h>

int main(void)
{
    uid_t appuid = getappuid();
    printf("app uid: %u\n", appuid);
    return 0;
}
```

#### COLOPHON

This page is part of the C library user-space interface documentation.
Information about the project can be found at (https://gitcode.com/openharmony/third_party_musl/blob/master/docs/).
