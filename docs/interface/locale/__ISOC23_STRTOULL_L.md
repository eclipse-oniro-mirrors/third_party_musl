## __ISOC23_STRTOULL_L        OHOS Programmer's Manual

#### **NAME**

__isoc23_strtoull_l - convert a string to an unsigned long long integer with C23 binary-prefix support.

#### **SYNOPSIS**

```c
#define _GNU_SOURCE
#include <stdlib.h>
#include <locale.h>

unsigned long long __isoc23_strtoull_l(const char *restrict nptr,
    char **restrict endptr, int base, locale_t loc);
```

#### **DESCRIPTION**

__isoc23_strtoull_l() converts the initial part of nptr to an unsigned long long value. Leading white space and one optional '+' or '-' sign are accepted. base must be 0 or an integer from 2 through 36.

With base 0, a 0x or 0X prefix followed by a hexadecimal digit selects base 16, a 0b or 0B prefix followed by a binary digit selects base 2, and another leading zero selects base 8; otherwise the base is 10. Explicit base 16 accepts the hexadecimal prefix and explicit base 2 accepts the binary prefix, in each case only when a digit follows the prefix. Other bases follow the usual integer-conversion rules.

Conversion stops at the first character that is not a digit in the selected base. If endptr is not NULL, *endptr receives a pointer to that character. When no conversion is performed, *endptr receives nptr.

The loc argument is ignored: this implementation delegates to __isoc23_strtoull(). Passing a locale object returned by newlocale(), LC_GLOBAL_LOCALE, or NULL produces the same result, endptr and errno as the corresponding non-locale function. The call does not change the thread locale or take ownership of a locale object. Acceptance of NULL and LC_GLOBAL_LOCALE here is an OpenHarmony-specific behavior; it is not a portable rule for other libraries' *_l functions.

**Portability:** `loc` cannot select per-call character classification or numeric syntax in this implementation. Locale-specific digit forms and grouping are not recognized. For example, with base 10, `"1,000"` converts to `1` and leaves `*endptr` pointing to the comma when `endptr` is not NULL. Check the unconverted suffix if the entire input must be a number.

#### **RETURN VALUE**

On success, __isoc23_strtoull_l() returns the converted unsigned long long value. A leading minus sign is accepted: a representable magnitude is negated in unsigned arithmetic. If the magnitude is outside the unsigned long long range, the function returns ULLONG_MAX and sets errno to ERANGE.

If no conversion is possible, the function returns 0 and, when endptr is not NULL, stores nptr in *endptr. This implementation sets errno to EINVAL for an input with no initial digit. An incomplete binary prefix such as "0b" is treated as the digit zero followed by an unconverted 'b'; it does not set EINVAL.

#### **ERRORS**

**EINVAL**: base is neither 0 nor an integer from 2 through 36, or the input has no initial digit.

**ERANGE**: The magnitude of the parsed digits exceeds the return type's maximum.

Successful conversions leave errno unchanged. To distinguish an error from a valid zero or range-limit result, set errno to 0 before the call.

#### ATTRIBUTES

| Interface | Attribute | Value |
| --- | --- | --- |
| __isoc23_strtoull_l() | Thread safety | MT-Safe |
| | Signal safety | Not Safe |

Concurrent calls use separate parsing state and thread-local errno. Callers must synchronize writes to shared input or endptr storage.

#### HISTORY

2026

#### NOTES

The declaration of `__isoc23_strtoull_l()` requires `_GNU_SOURCE`. Direct calls do not require `_OHOS_ISOC23_CONVERSION_SOURCE`.

To call the new implementation through `strtoull_l()` in C, define both `_GNU_SOURCE` and `_OHOS_ISOC23_CONVERSION_SOURCE` before including any headers. The ordinary name then redirects to `__isoc23_strtoull_l()`. C++ does not enable this redirection; use the explicit `__isoc23_strtoull_l()` entry point for C23 behavior.

If the target SDK and runtime provide the ordinary `strtoull_l()` interface, calls without redirection retain its legacy parsing behavior.

Define `_OHOS_ISOC23_CONVERSION_SOURCE` before the first header include, or pass `-D_OHOS_ISOC23_CONVERSION_SOURCE=1` when compiling the application. This selects the new behavior for the ordinary function names delivered in this batch. The macro is tested for presence: defining it as `0` also enables redirection. Leave it undefined to retain ordinary calls to the legacy entry points.

Neither `_GNU_SOURCE`, `_ALL_SOURCE`, `_ISOC23_SOURCE`, `_ISOC2X_SOURCE` nor a C2x/C23 language mode enables this batch automatically. The macro selects only this batch of conversion interfaces.

After all C23 standard library interfaces have been implemented, OpenHarmony musl plans to enable `_ISOC23_SOURCE` as the common C23 feature-selection entry point. This is a future plan; use `_OHOS_ISOC23_CONVERSION_SOURCE` for the current release.

#### CONFORMING TO

strtoull_l() is a locale-specific extension; __isoc23_strtoull_l() is a musl entry point with C23 binary-prefix support, not an ISO C function.

#### EXAMPLES

```c
#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

int main(void)
{
    /*
     * Expected output:
     * value=5 rest=tail
     */
    locale_t loc = newlocale(LC_ALL_MASK, "C", NULL);
    if (loc == NULL) {
        perror("newlocale");
        return 1;
    }
    char *end = NULL;
    errno = 0;
    unsigned long long value = __isoc23_strtoull_l("0B101tail", &end, 0, loc);
    int ok = errno == 0 && value == 5 && *end == 't';
    freelocale(loc);
    if (!ok) {
        return 1;
    }
    printf("value=%llu rest=%s\n", value, end);
    return 0;
}
```

#### COLOPHON

This page is part of the C library user-space interface documentation.
Information about the project can be found at (https://gitcode.com/openharmony/third_party_musl/blob/master/docs/).
