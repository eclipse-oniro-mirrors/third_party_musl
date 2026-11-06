## RPMATCH &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; OHOS Programmer's Manual

#### **NAME**

​      rpmatch - determine whether the response to a yes/no question is affirmative or negative.

#### **SYNOPSIS**

```c
#include <stdlib.h>

int rpmatch(const char *response);
```

#### **DESCRIPTION**

​      The rpmatch() function checks the string response to see whether it is an affirmative or negative answer to a yes/no question, according to the current locale.

​      It matches response against the locale's YESEXPR and NOEXPR regular expressions obtained from nl_langinfo(). In the "C" locale, YESEXPR is "^[yY]" and NOEXPR is "^[nN]", so any word beginning with 'y' or 'Y' (such as "yes") is affirmative and any word beginning with 'n' or 'N' (such as "no") is negative. Other locales provide their own localized patterns.

​      The function compiles the locale's response patterns on each call. It uses no shared mutable state, so it is safe to call concurrently from multiple threads. Passing NULL for response results in undefined behavior.

#### **RETURN VALUE**

​      The rpmatch() function returns:

​      1　if response is an affirmative answer (matches YESEXPR).

​      0　if response is a negative answer (matches NOEXPR).

​      -1　if response does not match either pattern, or if the locale's YESEXPR or NOEXPR patterns could not be compiled (the answer cannot be determined).

​      This function does not set errno.

#### **ERRORS**

​      rpmatch() can fail for any of the reasons that regcomp() or regexec() can fail; the error is not available from errno or anywhere else, but indicates a failure of the regex engine. This case is indistinguishable from that of an unrecognized value of response, since both return -1.

#### ATTRIBUTES

| Interface | Attribute     | Value          |
| --------- | ------------- | -------------- |
| rpmatch()  | Thread safety | MT-Safe |
|            | Async-signal-safety | Not Safe |

#### HISTORY

​      -- 2026

#### NOTES

​      This function is declared when `_GNU_SOURCE` or `_BSD_SOURCE` is defined.

#### CONFORMING TO

​      This function is a GNU extension. It is not part of POSIX or the C standard.

#### EXAMPLES

```c
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>

int main(void)
{
    printf("%d\n", rpmatch("yes"));   /* prints 1 */
    printf("%d\n", rpmatch("n"));     /* prints 0 */
    printf("%d\n", rpmatch("maybe")); /* prints -1 */
    return 0;
}
```

#### COLOPHON

​      This page is part of the C library user-space interface documentation.
​      Information about the project can be found at (https://gitcode.com/openharmony/third_party_musl/blob/master/docs/).