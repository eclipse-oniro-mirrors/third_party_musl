## GETSERVENT_R &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; OHOS Programmer's Manual



#### **NAME**

​       getservent_r - enumerate service database entries (reentrant).

#### **SYNOPSIS**

```c
#include <netdb.h>

int getservent_r(struct servent *se, char *buf, size_t buflen,
                 struct servent **result);
```

#### **DESCRIPTION**

​The getservent_r() function sequentially enumerates the service database, returning one entry per call. Each successful call fills the caller-provided struct servent pointed to by se with the next service entry.

The service database is read from the file /etc/services at runtime. On the first call, or when the internal file pointer is not yet open, the function opens /etc/services for reading. Each line in the file represents a service entry containing the service name, port number with protocol, and optional alias names.

The caller must supply a buffer buf of buflen bytes. The function copies the service name, protocol, and alias pointer array into this buffer, and the fields of se are set to point into buf. The result pointer *result is set to se on success.

The internal file cursor and related state are protected by an internal lock (serv_lock) so that concurrent calls are serialized.

The setservent() and endservent() functions share the same internal state (sd and serv_lock) as getservent_r(). setservent() rewinds or reopens the file, and endservent() closes the file and frees the internal data structure. Calling setservent() resets the enumeration position for getservent_r().

Lines beginning with '#' and blank lines are skipped during enumeration. Lines that cannot be parsed are also skipped.

This function is the reentrant counterpart of getservent(). While it does not store the result in static storage, the file cursor and related state are shared static state protected by a lock.

#### **RETURN VALUE**

​On success, getservent_r() returns 0 and sets *result to point to se.

When all entries have been enumerated or the services file cannot be opened, it returns ENOENT and sets *result to NULL.

When the supplied buffer is too small to hold the result, it returns ERANGE and sets *result to NULL; the caller should retry with a larger buffer.

When memory allocation for the internal data structure fails, the function returns ENOMEM and sets *result to NULL.

#### **ERRORS**

**ENOENT**: All service entries have been enumerated, or /etc/services does not exist on the device.

**ERANGE**: The supplied buffer buf is too small to hold the result. Increase buflen and retry.

**ENOMEM**: Memory allocation for the internal data structure failed.

#### ATTRIBUTES

| Interface | Attribute     | Value          |
| --------- | ------------- | -------------- |
| getservent_r() | Thread safety | MT-Safe (serialized) |
|             | Signal safety | Not Safe       |

Although the function is reentrant in terms of result storage, the file cursor is shared internal state. Concurrent calls are serialized via an internal lock.

#### EXAMPLES

```c
#include <netdb.h>
#include <stdio.h>

int main(void)
{
    struct servent se;
    struct servent *result;
    char buf[1024];
    int rv;

    while ((rv = getservent_r(&se, buf, sizeof(buf), &result)) == 0 && result != NULL) {
        printf("service: %s, port: %d, proto: %s\n",
               result->s_name, ntohs(result->s_port),
               result->s_proto);
    }
    return 0;
}
```

#### COLOPHON

​This page is part of the C library user-space interface documentation.
Information about the project can be found at (https://gitcode.com/openharmony/third_party_musl/blob/master/docs/).
