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

The reentrant getservent_r() protects the internal file cursor and related state with an internal lock (serv_lock) so that concurrent calls to getservent_r() are serialized. The non-reentrant getservent(), setservent(), and endservent() do not acquire this lock and are not thread-safe.

The setservent() and endservent() functions share the same internal state (sd) as getservent_r(). setservent() rewinds or reopens the file, and endservent() closes the file and frees the internal data structure. Calling setservent() resets the enumeration position for getservent_r().

Lines beginning with '#' and blank lines are skipped during enumeration. Lines that cannot be parsed, or whose protocol is not tcp/udp, or whose port is outside the range 1-65535, are also silently skipped.

This function is the reentrant counterpart of getservent(). While it does not store the result in static storage, the file cursor and related state are shared static state. Access from getservent_r() is protected by serv_lock. The actual implementation is available only when compiled with __LITEOS_A__ defined. On other platforms, the function is a stub that returns 0 and sets *result to NULL.

#### **RETURN VALUE**

​On success, getservent_r() returns 0 and sets *result to point to se. On platforms where __LITEOS_A__ is not defined, the function is a stub that returns 0 and sets *result to NULL.

When all entries have been enumerated or the services file cannot be opened, it returns ENOENT and sets *result to NULL.

When the supplied buffer is too small to hold the result, it returns ERANGE and sets *result to NULL; the caller should retry with a larger buffer.

When memory allocation for the internal data structure fails, the function returns ENOMEM and sets *result to NULL.

#### **ERRORS**

**ENOENT**: All service entries have been enumerated, or /etc/services does not exist on the device.

**ERANGE**: The supplied buffer buf is too small to hold the result. Increase buflen and retry.

**ESPIPE**: The file position could not be saved or restored during an ERANGE retry, making it impossible to re-read the current entry.

**ENOMEM**: Memory allocation for the internal data structure failed.

#### ATTRIBUTES

| Interface | Attribute     | Value          |
| --------- | ------------- | -------------- |
| getservent_r() | Thread safety | MT-Safe (serialized, only within _r variants) |
|             | Signal safety | Not Safe       |

Although the function is reentrant in terms of result storage, the file cursor is shared internal state. Concurrent calls to getservent_r() are serialized via an internal lock. The non-reentrant getservent(), setservent(), and endservent() do not use the lock and are not thread-safe. These functions share the same static state (service data structure, file pointer). Mixing any of getservent(), setservent(), endservent() with getservent_r() is strictly prohibited, as it will cause unpredictable results due to unsynchronized access to shared state.

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
