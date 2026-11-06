## POSIX_SPAWN_FILE_ACTIONS_ADDCLOSEFROM_NP &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; OHOS Programmer's Manual

#### **NAME**

​      posix_spawn_file_actions_addclosefrom_np - add closefrom action to spawn file actions object.

#### **SYNOPSIS**

```c
#include <spawn.h>

int posix_spawn_file_actions_addclosefrom_np(posix_spawn_file_actions_t *file_actions, int from);
```

#### **DESCRIPTION**

​      The posix_spawn_file_actions_addclosefrom_np() function adds a "closefrom" action to the object referenced by file_actions. When a new process is spawned using this file actions object, all open file descriptors greater than or equal to from are closed in the child process after the clone but before the new program is executed via exec.

​      This function is a batch version of posix_spawn_file_actions_addclose(): instead of registering one close action per descriptor, it registers a single action that closes the whole range. It is a GNU extension (hence the `_np` suffix) and is not part of POSIX.

​      file_actions must have been initialized with posix_spawn_file_actions_init(). The action is executed in registration order within the child; a descriptor number closed by this action may be reused by a subsequently registered addopen() action. Passing NULL for file_actions results in undefined behavior.

​      The implementation first attempts to use the kernel close_range system call over the range [from, UINT_MAX]. If the kernel does not support close_range, it falls back to closing each descriptor individually via the close system call, bounded by getdtablesize().

#### **RETURN VALUE**

​      On success, posix_spawn_file_actions_addclosefrom_np() returns 0.

​      On error, it returns one of the following error numbers (the error number is returned directly, not -1 with errno, consistent with the posix_spawn_file_actions_add* family):

​      **EBADF**　from is negative.

​      **ENOMEM**　insufficient memory to record the action.

#### **ERRORS**

​      **EBADF**　from is negative.

​      **ENOMEM**　insufficient memory to record the action.

#### ATTRIBUTES

| Interface | Attribute     | Value          |
| --------- | ------------- | -------------- |
| posix_spawn_file_actions_addclosefrom_np() | Thread safety | MT-Safe |
|             | Async-signal-safety | Not Safe |

#### HISTORY

​      -- 2026

#### NOTES

​      This function is declared when `_BSD_SOURCE` or `_GNU_SOURCE` is defined (same as posix_spawn_file_actions_addchdir_np / addfchdir_np).

#### CONFORMING TO

​      This function is a GNU extension. It is not part of POSIX or the C standard.

#### EXAMPLES

```c
#define _GNU_SOURCE
#include <spawn.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

extern char **environ;

int main(void)
{
    posix_spawn_file_actions_t fa;
    pid_t pid;
    int status;
    char *argv[] = { "myprogram", NULL };

    if (posix_spawn_file_actions_init(&fa) != 0)
        return 1;

    /* Close every file descriptor from 3 upward in the child. */
    int rc = posix_spawn_file_actions_addclosefrom_np(&fa, 3);
    if (rc != 0) {
        printf("addclosefrom_np failed: %d\n", rc);
        posix_spawn_file_actions_destroy(&fa);
        return 1;
    }

    rc = posix_spawn(&pid, "myprogram", &fa, NULL, argv, environ);
    if (rc != 0) {
        printf("posix_spawn failed: %d\n", rc);
        posix_spawn_file_actions_destroy(&fa);
        return 1;
    }

    waitpid(pid, &status, 0);
    posix_spawn_file_actions_destroy(&fa);
    return 0;
}
```

#### COLOPHON

​      This page is part of the C library user-space interface documentation.
​      Information about the project can be found at (https://gitcode.com/openharmony/third_party_musl/blob/master/docs/).