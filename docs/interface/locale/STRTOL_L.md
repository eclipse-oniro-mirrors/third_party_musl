## STRTOL_L        OHOS Programmer's Manual

#### **NAME**

strtol_l - convert a string to a long integer; the locale argument is ignored.

#### **SYNOPSIS**

```c
#define _GNU_SOURCE
#include <stdlib.h>
#include <locale.h>

long strtol_l(const char *restrict nptr, char **restrict endptr,
    int base, locale_t loc);
```

#### **DESCRIPTION**

strtol_l() converts the initial part of nptr to a long value. Leading white space and one optional '+' or '-' sign are accepted. base must be 0 or an integer from 2 through 36. The loc argument is accepted but does not affect parsing; locale-specific digit forms and grouping are not recognized.

With base 0, a 0x or 0X prefix followed by a hexadecimal digit selects base 16, another leading zero selects base 8, and other input is read in base 10. An explicit base of 16 also accepts a hexadecimal prefix when a hexadecimal digit follows it. Unlike the C23 strtol() entry point, strtol_l() does not recognize 0b or 0B as a binary prefix. For example, strtol_l("0b101", &endptr, 0, loc) returns 0 and leaves endptr pointing to 'b'.

Conversion stops at the first character that is not a digit in the selected base. If endptr is not NULL, *endptr receives a pointer to that character. If no conversion is performed, *endptr receives nptr. Passing a null nptr or a string without a terminating null byte has undefined behavior.

#### **RETURN VALUE**

On success, strtol_l() returns the converted long value. If the signed mathematical result is outside the long range, it returns LONG_MAX or LONG_MIN, as appropriate, and sets errno to ERANGE.

If no conversion is possible, it returns 0 and sets errno to EINVAL. A leading zero followed by an unrecognized prefix letter, such as "0b" or "0x", is converted as the digit zero without setting EINVAL.

#### **ERRORS**

**EINVAL**: base is neither 0 nor an integer from 2 through 36, or the input has no initial digit.

**ERANGE**: The signed mathematical result is outside the long range.

Successful conversions leave errno unchanged. To distinguish an error from a valid zero or range-limit result, set errno to 0 before the call.

#### ATTRIBUTES

| Interface | Attribute | Value |
| --- | --- | --- |
| strtol_l() | Thread safety | MT-Safe |
| | Signal safety | Not Safe |

Concurrent calls use separate parsing state and thread-local errno. Callers must synchronize writes to shared input or endptr storage.

#### HISTORY

2026

#### NOTES

The declaration requires _GNU_SOURCE. This implementation calls the legacy strtol() ABI entry point and ignores loc. Enabling C23 redirection in application headers does not change strtol_l() parsing behavior.

After all C23 standard library interfaces have been implemented, OpenHarmony musl plans to enable `_ISOC23_SOURCE` as the common C23 feature-selection entry point. The current release uses `_OHOS_ISOC23_CONVERSION_SOURCE` for this conversion batch.

#### CONFORMING TO

strtol_l() is a locale-specific extension and is not an ISO C function.

#### EXAMPLES

```c
#define _GNU_SOURCE
#include <errno.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    /*
     * Expected output:
     * hex: value=42 rest=tail
     * binary: value=0 rest=b101
     */
    locale_t loc = newlocale(LC_ALL_MASK, "C", NULL);
    if (loc == NULL) {
        perror("newlocale");
        return 1;
    }
    char *hex_end = NULL;
    char *binary_end = NULL;
    errno = 0;
    long hex_value = strtol_l("0x2atail", &hex_end, 0, loc);
    int ok = errno == 0 && hex_value == 42 && *hex_end == 't';

    errno = 0;
    long binary_value = strtol_l("0b101", &binary_end, 0, loc);
    ok = ok && errno == 0 && binary_value == 0 && *binary_end == 'b';
    freelocale(loc);
    if (!ok) {
        return 1;
    }
    printf("hex: value=%ld rest=%s\n", hex_value, hex_end);
    printf("binary: value=%ld rest=%s\n", binary_value, binary_end);
    return 0;
}
```

#### COLOPHON

This page is part of the C library user-space interface documentation.
Information about the project can be found at (https://gitcode.com/openharmony/third_party_musl/blob/master/docs/).
