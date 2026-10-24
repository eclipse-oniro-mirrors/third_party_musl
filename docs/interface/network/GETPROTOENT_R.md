## GETPROTOENT_R &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; OHOS Programmer's Manual



#### **NAME**

​       getprotoent_r - enumerate protocol database entries (reentrant).

#### **SYNOPSIS**

```c
#include <netdb.h>

int getprotoent_r(struct protoent *res, char *buf, size_t buflen,
                  struct protoent **result);
```

#### **DESCRIPTION**

​The getprotoent_r() function sequentially enumerates the protocol database, returning one entry per call. Each successful call fills the caller-provided struct protoent pointed to by res with the next protocol entry.

The protocol database is a compile-time hardcoded constant array embedded in the C library. It is not read from any file at runtime. The array contains well-known protocol entries such as ip, icmp, tcp, udp, and others.

The caller must supply a buffer buf of buflen bytes. The function copies the protocol name and the alias pointer array into this buffer, and the fields of res are set to point into buf. The result pointer *result is set to res on success.

Enumeration uses an internal cursor idx that tracks the current position within the constant array. The reentrant getprotoent_r() protects this cursor with an internal lock (proto_lock) so that concurrent calls to getprotoent_r() are serialized. The non-reentrant getprotoent(), setprotoent(), and endprotoent() do not acquire this lock and are not thread-safe. On each successful call the cursor is advanced to the next entry.

The cursor is shared with the non-reentrant getprotoent() function via the same static variable idx. Calling setprotoent() or endprotoent() resets idx to 0, which also resets the position for getprotoent_r(). To reset the enumeration cursor, call setprotoent() or endprotoent().

When all entries have been enumerated, subsequent calls return ENOENT and set *result to NULL.

This function is the reentrant counterpart of getprotoent(). While it does not store the result in static storage, the enumeration cursor itself is shared static state. Access from getprotoent_r() is protected by proto_lock.

#### **RETURN VALUE**

​On success, getprotoent_r() returns 0 and sets *result to point to res.

When all entries have been enumerated, it returns ENOENT and sets *result to NULL.

When the supplied buffer is too small to hold the result, it returns ERANGE and sets *result to NULL; the caller should retry with a larger buffer. The cursor is not advanced in this case.

#### **ERRORS**

**ENOENT**: All protocol entries have been enumerated, or the database is empty.

**ERANGE**: The supplied buffer buf is too small to hold the result. Increase buflen and retry.

#### ATTRIBUTES

| Interface | Attribute     | Value          |
| --------- | ------------- | -------------- |
| getprotoent_r() | Thread safety | MT-Safe (serialized, only within _r variants) |
|             | Signal safety | Not Safe       |

Although the function is reentrant in terms of result storage, the enumeration cursor is shared internal state. Concurrent calls to getprotoent_r() are serialized via an internal lock, so multiple threads calling getprotoent_r() will each receive successive entries but never the same entry twice. The non-reentrant getprotoent(), setprotoent(), and endprotoent() do not use the lock and are not thread-safe. These functions share the same static cursor (idx). Mixing any of getprotoent(), setprotoent(), endprotoent() with getprotoent_r() is strictly prohibited, as it will cause unpredictable results due to unsynchronized access to shared state.

#### EXAMPLES

```c
#include <netdb.h>
#include <stdio.h>

int main(void)
{
    struct protoent pe;
    struct protoent *result;
    char buf[1024];
    int rv;

    while ((rv = getprotoent_r(&pe, buf, sizeof(buf), &result)) == 0) {
        if (result)
            printf("protocol: %s, number: %d\n",
                   result->p_name, result->p_proto);
    }
    return 0;
}
```

#### COLOPHON

​This page is part of the C library user-space interface documentation.
Information about the project can be found at (https://gitcode.com/openharmony/third_party_musl/blob/master/docs/).
