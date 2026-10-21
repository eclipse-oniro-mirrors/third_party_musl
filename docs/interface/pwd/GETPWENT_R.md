## GETPWENT_R &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; OHOS Programmer's Manual



#### **NAME**

​       getpwent_r - enumerate passwd database entries (reentrant).

#### **SYNOPSIS**

```c
#include <pwd.h>

int getpwent_r(struct passwd *pw, char *buf, size_t size, struct passwd **res);
```

#### **DESCRIPTION**

​The getpwent_r() function sequentially enumerates the passwd database, returning one entry per call. Each successful call fills the caller-provided struct passwd pointed to by pw with the next passwd entry.

The passwd database is read from the file /etc/passwd at runtime. On the first call, or when the internal file pointer is not yet open, the function opens /etc/passwd for reading. Each line in the file represents a user account entry containing the username, password placeholder, UID, GID, GECOS field, home directory, and login shell.

The caller must supply a buffer buf of size bytes. The function copies the parsed line content into this buffer, and the string fields of pw (pw_name, pw_passwd, pw_gecos, pw_dir, pw_shell) are adjusted to point into buf. The result pointer *res is set to pw on success.

The function shares an internal file cursor (f) with the non-reentrant getpwent() function. This cursor is protected by an internal lock (pwent_lock) so that concurrent calls are serialized.

The setpwent() function (defined in getpwent.c) closes the shared file pointer, causing subsequent getpwent_r() calls to reopen the file from the beginning. endpwent() is a weak alias of setpwent() and has the same effect.

When the end of the file is reached, subsequent calls return ENOENT and set *res to NULL.

If /etc/passwd cannot be opened, the function returns ENOENT and sets *res to NULL.

This function is the reentrant counterpart of getpwent(). While it does not store the result in static storage, the file cursor is shared static state protected by a lock.

#### **RETURN VALUE**

​On success, getpwent_r() returns 0 and sets *res to point to pw.

When the end of the passwd file is reached, it returns ENOENT and sets *res to NULL.

When the supplied buffer is too small to hold the result, it returns ERANGE and sets *res to NULL; the caller should retry with a larger buffer.

#### **ERRORS**

**ENOENT**: The end of /etc/passwd has been reached, or /etc/passwd cannot be opened.

**ERANGE**: The supplied buffer buf is too small to hold the result. Increase size and retry.

#### ATTRIBUTES

| Interface | Attribute     | Value          |
| --------- | ------------- | -------------- |
| getpwent_r() | Thread safety | MT-Safe (serialized) |
|             | Signal safety | Not Safe       |

Although the function is reentrant in terms of result storage, the file cursor is shared internal state. Concurrent calls are serialized via an internal lock.

#### EXAMPLES

```c
#include <pwd.h>
#include <stdio.h>

int main(void)
{
    struct passwd pw;
    struct passwd *res;
    char buf[1024];
    int rv;

    while ((rv = getpwent_r(&pw, buf, sizeof(buf), &res)) == 0) {
        if (res)
            printf("user: %s, uid: %d, home: %s\n",
                   res->pw_name, res->pw_uid, res->pw_dir);
    }
    return 0;
}
```

#### COLOPHON

​This page is part of the C library user-space interface documentation.
Information about the project can be found at (https://gitcode.com/openharmony/third_party_musl/blob/master/docs/).
