## STRPTIME_L &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; OHOS Programmer's Manual

#### **NAME**

​      strptime_l - convert a string to a broken-down time, using a locale.

#### **SYNOPSIS**

```c
#include <time.h>

char *strptime_l(const char *restrict s, const char *restrict format,
                 struct tm *restrict tm, locale_t loc);
```

#### **DESCRIPTION**

​      The strptime_l() function is the locale-aware version of strptime(). It converts the character string s to a broken-down time structure tm, according to the format string format, using the locale loc for the locale-dependent format specifiers (%a, %A, %b, %B, %c, %h, %p, %r, %x, %X). The %h specifier is a synonym for %b. The %E and %O modifiers are not supported by this implementation.

​      The format string contains the same conversion specifications as documented for strptime(). Field values that are set by the conversion are stored into tm; fields that are not specified are left unchanged. In particular, this implementation does not reset the tm structure before parsing.

​      Calling strptime_l() with loc equal to the current locale is equivalent to calling strptime(). Passing NULL for s, format, or tm results in undefined behavior.

#### **RETURN VALUE**

​      On success, strptime_l() returns a pointer to the first character in s that was not consumed by the conversion. On failure (when the input does not match the format), it returns NULL and does not set errno.

#### **ERRORS**

​      No errors are defined.

#### ATTRIBUTES

| Interface | Attribute     | Value          |
| --------- | ------------- | -------------- |
| strptime_l() | Thread safety | MT-Safe locale |
|              | Async-signal-safety | Not Safe |

#### HISTORY

​      -- 2026

#### NOTES

​      This function is declared when `_GNU_SOURCE` or `_BSD_SOURCE` is defined.

​      loc must be a valid locale handle returned by newlocale(), or one of the predefined locale handles. Passing LC_GLOBAL_LOCALE is not supported and results in undefined behavior; use strptime() for parsing with the process-global locale.

#### CONFORMING TO

​      This function is a GNU extension. It is not part of POSIX or the C standard.

#### EXAMPLES

```c
#define _GNU_SOURCE
#include <time.h>
#include <locale.h>
#include <stdio.h>

int main(void)
{
    struct tm tm = {0};
    locale_t loc = newlocale(LC_ALL_MASK, "C", NULL);
    if (loc == NULL)
        return 1;

    char *r = strptime_l("2026-10-28", "%Y-%m-%d", &tm, loc);
    if (r != NULL)
        printf("%d-%02d-%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);

    freelocale(loc);
    return 0;
}
```

#### COLOPHON

​      This page is part of the C library user-space interface documentation.
​      Information about the project can be found at (https://gitcode.com/openharmony/third_party_musl/blob/master/docs/).