## GETGRENT_R &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; OHOS Programmer's Manual



#### **NAME**

​       getgrent_r - enumerate group database entries (reentrant).

#### **SYNOPSIS**

```c
#include <grp.h>

int getgrent_r(struct group *gr, char *buf, size_t size, struct group **res);
```

#### **DESCRIPTION**

​The getgrent_r() function sequentially enumerates the group database, returning one entry per call. Each successful call fills the caller-provided struct group pointed to by gr with the next group entry.

The group database is read from the file /etc/group at runtime. On the first call, or when the internal file pointer is not yet open, the function opens /etc/group for reading. Each line in the file represents a group entry containing the group name, password placeholder, GID, and a comma-separated list of member usernames.

The caller must supply a buffer buf of size bytes. The function copies the parsed line content and the member pointer array into this buffer. The string fields of gr (gr_name, gr_passwd) and the member array (gr_mem) are adjusted to point into buf. The result pointer *res is set to gr on success.

The buffer must be large enough to hold both the line data and the member pointer array. The required size depends on the number of members in the group entry.

The function shares an internal file cursor (f) with the non-reentrant getgrent() function. This cursor is protected by an internal lock (grent_lock) so that concurrent calls are serialized.

The setgrent() function (defined in getgrent.c) closes the shared file pointer, causing subsequent getgrent_r() calls to reopen the file from the beginning. endgrent() is a weak alias of setgrent() and has the same effect.

When the end of the file is reached, subsequent calls return ENOENT and set *res to NULL.

If /etc/group cannot be opened, the function returns ENOENT and sets *res to NULL.

This function is the reentrant counterpart of getgrent(). While it does not store the result in static storage, the file cursor is shared static state protected by a lock.

#### **RETURN VALUE**

​On success, getgrent_r() returns 0 and sets *res to point to gr.

When the end of the group file is reached, it returns ENOENT and sets *res to NULL.

When the supplied buffer is too small to hold the result, it returns ERANGE and sets *res to NULL; the caller should retry with a larger buffer.

#### **ERRORS**

**ENOENT**: The end of /etc/group has been reached, or /etc/group cannot be opened.

**ERANGE**: The supplied buffer buf is too small to hold the result, including the line data and the member pointer array. Increase size and retry.

#### ATTRIBUTES

| Interface | Attribute     | Value          |
| --------- | ------------- | -------------- |
| getgrent_r() | Thread safety | MT-Safe (serialized) |
|             | Signal safety | Not Safe       |

Although the function is reentrant in terms of result storage, the file cursor is shared internal state. Concurrent calls are serialized via an internal lock.

#### EXAMPLES

```c
#include <grp.h>
#include <stdio.h>

int main(void)
{
    struct group gr;
    struct group *res;
    char buf[1024];
    int rv;

    while ((rv = getgrent_r(&gr, buf, sizeof(buf), &res)) == 0) {
        if (res)
            printf("group: %s, gid: %d\n",
                   res->gr_name, res->gr_gid);
    }
    return 0;
}
```

#### COLOPHON

​This page is part of the C library user-space interface documentation.
Information about the project can be found at (https://gitcode.com/openharmony/third_party_musl/blob/master/docs/).
