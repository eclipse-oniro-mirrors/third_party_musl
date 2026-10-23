## FGETPWENT_R &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; OHOS Programmer's Manual



#### **NAME**

​       fgetpwent_r - read a passwd entry from a stream (reentrant).

#### **SYNOPSIS**

```c
#include <pwd.h>
#include <stdio.h>

int fgetpwent_r(FILE *f, struct passwd *pw, char *buf, size_t size,
                struct passwd **res);
```

#### **DESCRIPTION**

​The fgetpwent_r() function reads the next passwd entry from the caller-provided stream f and stores the result in the caller-provided struct passwd pointed to by pw.

Unlike getpwent_r(), this function does not use the shared system passwd database cursor. Instead, it reads directly from the FILE stream supplied by the caller, allowing the caller to control which file is being read and at what position. This makes the function independent of the internal /etc/passwd cursor state and suitable for reading passwd-format data from arbitrary sources.

Each line in the stream is expected to follow the passwd file format, containing the username, password placeholder, UID, GID, GECOS field, home directory, and login shell, separated by colons.

The caller must supply a buffer buf of size bytes. The function copies the parsed line content into this buffer, and the string fields of pw (pw_name, pw_passwd, pw_gecos, pw_dir, pw_shell) are adjusted to point into buf. The result pointer *res is set to pw on success.

When the end of the stream is reached, the function returns ENOENT and sets *res to NULL.

This function does not use any internal locks or shared cursor state. The caller is responsible for ensuring that the stream is not accessed concurrently by other threads.

#### **RETURN VALUE**

​On success, fgetpwent_r() returns 0 and sets *res to point to pw.

When the end of the stream is reached, it returns ENOENT and sets *res to NULL.

When the supplied buffer is too small to hold the result, it returns ERANGE and sets *res to NULL. The entry has already been consumed; retrying will read the next entry, not the same one.

#### **ERRORS**

**ENOENT**: The end of the stream has been reached.

**ERANGE**: The supplied buffer buf is too small to hold the result. The entry has been consumed and cannot be retried.

#### ATTRIBUTES

| Interface | Attribute     | Value          |
| --------- | ------------- | -------------- |
| fgetpwent_r() | Thread safety | MT-Safe (provided the caller does not share the FILE stream concurrently) |
|             | Signal safety | Not Safe       |

The function itself does not use shared internal state. Thread safety depends on the caller not concurrently accessing the same FILE stream from multiple threads without external synchronization.

#### EXAMPLES

```c
#include <pwd.h>
#include <stdio.h>

int main(void)
{
    FILE *f = fopen("/etc/passwd", "r");
    if (!f) return 1;

    struct passwd pw;
    struct passwd *res;
    char buf[1024];
    int rv;

    while ((rv = fgetpwent_r(f, &pw, buf, sizeof(buf), &res)) == 0) {
        if (res)
            printf("user: %s, uid: %d, shell: %s\n",
                   res->pw_name, res->pw_uid, res->pw_shell);
    }
    fclose(f);
    return 0;
}
```

#### COLOPHON

​This page is part of the C library user-space interface documentation.
Information about the project can be found at (https://gitcode.com/openharmony/third_party_musl/blob/master/docs/).
