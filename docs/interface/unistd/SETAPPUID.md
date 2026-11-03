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

When musl_extended_function is disabled, setappuid() returns the result of setuid(uid).

#### **ERRORS**

#### ATTRIBUTES

| Interface   | Attribute     | Value          |
| ----------- | ------------- | -------------- |
| setappuid() | Thread safety | MT-Safe        |
|             | Signal safety | Not Safe       |

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
