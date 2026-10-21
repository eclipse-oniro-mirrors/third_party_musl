## GROUP_MEMBER &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; OHOS Programmer's Manual



#### **NAME**

​       group_member - check whether the calling process belongs to a specified group.

#### **SYNOPSIS**

```c
#include <unistd.h>

int group_member(gid_t gid);
```

#### **DESCRIPTION**

​The group_member() function checks whether the group ID gid matches any of the supplementary group IDs of the calling process (as returned by getgroups()).

The function retrieves the supplementary group list via getgroups(). It uses a doubling-allocation strategy with alloca: starting from an initial capacity of NGROUPS_MAX (defaulting to 16 if undefined), it immediately doubles the capacity before each getgroups() call, so the first call uses NGROUPS_MAX * 2 (32 by default). This process repeats until the returned count is strictly less than the requested capacity, ensuring the complete list has been obtained.

#### **RETURN VALUE**

​The group_member() function returns 1 if the calling process is a member of the group identified by gid. Otherwise, it returns 0.

#### **ERRORS**

If getgroups() fails, the function returns 0 (indicating the process is not a member of the group). errno may be set by getgroups() but the function return value does not indicate an error.

#### ATTRIBUTES

| Interface | Attribute     | Value          |
| --------- | ------------- | -------------- |
| group_member() | Thread safety | MT-Safe |
|             | Async-signal-safety | AS-Unsafe (uses alloca) |

#### EXAMPLES

```c
#include <unistd.h>
#include <stdio.h>

int main(void)
{
    gid_t gid = 1000;

    if (group_member(gid)) {
        printf("Process is a member of group %u\n", gid);
    } else {
        printf("Process is NOT a member of group %u\n", gid);
    }
    return 0;
}
```

#### COLOPHON

​This page is part of the C library user-space interface documentation.
Information about the project can be found at (https://gitcode.com/openharmony/third_party_musl/blob/master/docs/).
