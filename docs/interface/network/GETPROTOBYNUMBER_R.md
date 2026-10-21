## GETPROTOBYNUMBER_R &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; OHOS Programmer's Manual



#### **NAME**

​       getprotobynumber_r - retrieve protocol entry by number (reentrant).

#### **SYNOPSIS**

```c
#include <netdb.h>

int getprotobynumber_r(int proto, struct protoent *res,
                       char *buf, size_t buflen, struct protoent **result);
```

#### **DESCRIPTION**

​The getprotobynumber_r() function looks up the protocol database entry whose protocol number matches proto and stores the result in the caller-provided struct protoent pointed to by res.

The protocol database is a compile-time hardcoded constant array embedded in the C library. It is not read from any file at runtime. The array contains well-known protocol entries such as ip, icmp, tcp, udp, and others.

The caller must supply a buffer buf of buflen bytes. The function copies the protocol name and the alias pointer array into this buffer, and the fields of res are set to point into buf. The result pointer *result is set to res on success.

This function is the reentrant counterpart of getprotobynumber(). It does not use static internal state for storing the result, making it safe for concurrent use.

#### **RETURN VALUE**

​On success, getprotobynumber_r() returns 0 and sets *result to point to res.

When the protocol number is not found, it returns ENOENT and sets *result to NULL.

When the supplied buffer is too small to hold the result, it returns ERANGE and sets *result to NULL; the caller should retry with a larger buffer.

#### **ERRORS**

**ENOENT**: The specified protocol number was not found in the protocol database.

**ERANGE**: The supplied buffer buf is too small to hold the result. Increase buflen and retry.

#### ATTRIBUTES

| Interface | Attribute     | Value          |
| --------- | ------------- | -------------- |
| getprotobynumber_r() | Thread safety | MT-Safe        |
|             | Signal safety | Not Safe       |

The function is reentrant and does not use static internal state for storing the result. It reads from a compile-time constant array which is inherently thread-safe.

#### EXAMPLES

```c
#include <netdb.h>
#include <stdio.h>
#include <errno.h>

int main(void)
{
    struct protoent pe;
    struct protoent *result;
    char buf[1024];

    int rv = getprotobynumber_r(6, &pe, buf, sizeof(buf), &result);
    if (rv == 0 && result) {
        printf("protocol: %s, number: %d\n",
               result->p_name, result->p_proto);
    } else if (rv == ENOENT) {
        printf("protocol not found\n");
    } else {
        printf("error: %d\n", rv);
    }
    return 0;
}
```

#### COLOPHON

​This page is part of the C library user-space interface documentation.
Information about the project can be found at (https://gitcode.com/openharmony/third_party_musl/blob/master/docs/).
