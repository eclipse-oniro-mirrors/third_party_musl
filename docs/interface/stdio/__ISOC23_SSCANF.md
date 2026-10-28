## __ISOC23_SSCANF &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; OHOS Programmer's Manual

#### **NAME**

__isoc23_sscanf - read formatted input from a byte string with C23 scanning extensions.

#### **SYNOPSIS**

```c
#include <stdarg.h>
#include <stdio.h>

int __isoc23_sscanf(const char *restrict s, const char *restrict format, ...);
```

#### **DESCRIPTION**

`__isoc23_sscanf()` reads from the null-terminated byte string `s` according to `format`. It stores converted values in the objects addressed by the subsequent pointer arguments. It does not modify the input string.

| Parameter | Description |
| --- | --- |
| `s` | Readable, null-terminated input byte string. |
| `format` | Readable, null-terminated format byte string. |
| `...` | Pointers to destination objects, in the order required by the format. |

Input ends at the terminating null byte. `%n` reports the number of bytes consumed, including skipped whitespace; it does not count a terminating null byte.

The following integer formats are supported:

| Format | Meaning | Destination without another length modifier |
| --- | --- | --- |
| `%i` | Detect decimal, leading-zero octal, `0x`/`0X` hexadecimal, or `0b`/`0B` binary input; an optional sign is accepted. | `int *` |
| `%b` | Read binary digits, optionally preceded by a sign and `0b` or `0B`. | `unsigned int *` |
| `%wNd`, `%wNi` | Store a signed integer of the specified width. | Pointer to the corresponding `intN_t` type |
| `%wNb`, `%wNo`, `%wNu`, `%wNx`, `%wNX` | Store an unsigned integer of the specified width. | Pointer to the corresponding `uintN_t` type |
| `%wfNd`, `%wfNi` and unsigned variants | Use the corresponding fast integer type. | Pointer to `int_fastN_t` or `uint_fastN_t`, as appropriate |

Here `N` is one of `8`, `16`, `32`, or `64`; for example, `%w32d` expects `int32_t *`. The `wN` and `wfN` modifiers also apply to `%n`, with a pointer to the corresponding signed integer type. They do not apply to floating-point, pointer, character, or string conversions. Fast integer types can be wider than `N` bits. Use the matching `int_fastN_t` or `uint_fastN_t` declared by the target `<stdint.h>`; do not infer the destination type from `N` alone.

A decimal field width must fit in `int` (`INT_MAX` from `<limits.h>`); a larger value is rejected with `EINVAL`. A field width precedes the length modifier: in `%3w16d`, `3` limits the input field and `16` selects the integer type.

The `SCNb*` macros from `<inttypes.h>`, such as `SCNb32`, provide binary conversion formats for the corresponding unsigned types and are available without `_OHOS_ISOC23_CONVERSION_SOURCE`. Use them with `__isoc23_*` scanning functions, either directly or through a function pointer obtained with `dlsym()`, or define `_OHOS_ISOC23_CONVERSION_SOURCE` to redirect the corresponding ordinary calls. These macros only provide format strings; their availability does not enable `%b` support in legacy scanning functions.

For `%lc`, `%ls`, and `%l[`, field widths count wide characters produced, while `%n` still counts input bytes. Multibyte decoding uses `LC_CTYPE`. Numeric parsing uses ASCII digits and a period as the floating-point radix character; localized grouping and alternative digits are not supported.

Assignment suppression with `*` consumes input without storing a result. `%n` stores the consumed length without consuming input. For `%s` and `%[`, successful output is null-terminated; `%c` does not append a terminator. Provide correctly typed destination objects and enough storage, including any required terminator. The POSIX `m` modifier allocates output for `c`, `s`, or `[`; pass the appropriate pointer-to-pointer and release successfully assigned buffers with `free()`.

#### **RETURN VALUE**

The return value is the number of input items successfully assigned and counted. Assignment-suppressed conversions (`%*`), literal matches, and `%n` do not increase this count.

A matching failure before any counted assignment returns `0`. Input exhaustion or an input-processing failure before any counted assignment returns `EOF`; after counted assignments, the count obtained so far is returned. An unsupported `wN`/`wfN` modifier, an invalid conversion used with it, or a field width larger than `INT_MAX` returns the count obtained so far, possibly `0`, and sets `errno` to `EINVAL`.

The result is neither the converted numeric value nor the number of input characters consumed. Use `%n` when the consumed length is needed.

#### **ERRORS**

| Error | Condition |
| --- | --- |
| `EINVAL` | A malformed or unsupported `wN`/`wfN` modifier, a conversion incompatible with that modifier, or a field width larger than `INT_MAX`. |
| `EILSEQ` | A required multibyte/wide-character conversion encounters an invalid or incomplete sequence, or an unrepresentable wide character. |
| `ENOMEM` | Memory allocation fails, or the requested `%m` buffer size or growth cannot be represented in `size_t`. |
| `ERANGE` | The internal numeric conversion detects overflow or underflow. |

An ordinary numeric matching failure is not itself reported as `EINVAL` by these C23 entry points. A positive assignment count does not guarantee that `errno` is unchanged or zero; it is not a complete error indication.

`ERANGE` is not a complete check of the destination type's range: integer parsing uses an intermediate type before storing into the requested object. Supply representable values; do not rely on scanf-style conversion to validate arbitrary out-of-range input.

Null or invalid input/format pointers, missing arguments, and incorrectly typed or undersized destination objects are not supported error-reporting cases.

#### ATTRIBUTES

| Interface | Attribute | Value |
| --- | --- | --- |
| `__isoc23_sscanf()` | Thread safety | MT-Safe locale |
| | Signal safety | Not Safe |

The caller must avoid concurrent modification of input strings, format strings, or destination objects. Locale changes must be coordinated with other threads.

#### HISTORY

2026.

#### NOTES

This scanf extension covers `scanf`, `fscanf`, `sscanf`, `vfscanf`, `vsscanf`, and `swscanf` only. Other members, including `vscanf`, `fwscanf`, `wscanf`, `vfwscanf`, `vwscanf`, and `vswscanf`, continue to use the legacy entry points even when C23 redirection is enabled.

With `_OHOS_ISOC23_CONVERSION_SOURCE` defined, ordinary `sscanf()` calls use `__isoc23_sscanf()`. You can also call `__isoc23_sscanf()` directly without defining this macro.

Define `_OHOS_ISOC23_CONVERSION_SOURCE` before the first header include, or pass `-D_OHOS_ISOC23_CONVERSION_SOURCE=1` when compiling the application. This selects the new behavior for the ordinary function names delivered in this batch. The macro is tested for presence: defining it as `0` also enables redirection. Leave it undefined to retain ordinary calls to the legacy entry points.

Neither `_GNU_SOURCE`, `_ALL_SOURCE`, `_ISOC23_SOURCE`, `_ISOC2X_SOURCE` nor a C2x/C23 language mode enables this batch automatically. The macro selects only this batch of conversion interfaces.

After all C23 standard library interfaces have been implemented, OpenHarmony musl plans to enable `_ISOC23_SOURCE` as the common C23 feature-selection entry point. This is a future plan; use `_OHOS_ISOC23_CONVERSION_SOURCE` for the current release.

#### INPUT DETAILS

For integer conversions that recognize the corresponding prefix, an incomplete `0b` or `0x` prefix causes a matching failure. An incomplete floating-point exponent such as `1e+` or `0x1p+` also fails. The numeric destination is unchanged and subsequent conversions, including `%n`, are not executed. For stream input, the prefix or exponent already read remains consumed.

Valid `nan`, `nan()`, and `nan(payload)` input produces a NaN with the specified sign. A parenthesized payload is consumed through its closing `)`, and its characters must be ASCII letters, digits, or underscores. An invalid payload character is consumed and causes a matching failure. An unclosed payload, including one truncated by a field width, also fails. A width of three can match only `nan` in `nan(...)`. The NaN payload bit representation is implementation-specific.

| Input case | Behavior |
| --- | --- |
| `%p` with `(nil)` | Accepts the text without regard to letter case and stores a null pointer. |
| `%c` reaches end-of-input before its field width | Can succeed if at least one character was read. Use `%n` to determine the amount consumed. |
| `%f` with `"0x."` | Matching failure; leaves the destination unchanged. |
| `%f` with `"nX!"` | Matching failure. For stream input, `n` is consumed and `X` remains unread. |
| End-of-input before a numeric or string conversion | Preserves the incoming `errno`; inspect the return value to detect failure. |

For `%m`, the caller owns each successfully assigned buffer and must release it with `free()`. A character conversion can reserve space from its field width before reading the complete item, so a very large width can fail with `ENOMEM` even for short input. Allocation sizes and timing are implementation details.

For `%lc`, `%ls`, or `%l[`, an invalid or incomplete multibyte sequence causes an encoding failure and sets `EILSEQ`. Before any counted assignment, the return value is `EOF`; after counted assignments, it is the count obtained so far. An encoding error can leave partial output and consumed input, so a failed conversion must not be treated as a complete string. In the built-in `C` locale, bytes above `0x7f` use musl's byte-preserving mapping; select a UTF-8 locale when UTF-8 decoding is required.

The rules above describe the new entry points. Ordinary calls use them only when redirected as described under NOTES. Legacy symbols retain their existing parsing, input consumption, error reporting, and allocation behavior.

#### CONFORMING TO

These musl entry points provide the documented C23 scanning features and POSIX-style `%m` allocation. The `__isoc23_*` names are musl entry-point names; ISO C specifies the corresponding ordinary function names. Use the behavior and error rules documented on this page when developing for OpenHarmony musl.

#### EXAMPLES

The following program checks the conversion results before printing them.

```c
#include <stdarg.h>
#include <inttypes.h>
#include <stdio.h>

int main(void)
{
    /*
     * Expected output:
     * value=5 exact=42 binary=6 count=12
     */
    int value = 0;
    int count = -1;
    int32_t exact = 0;
    uint32_t binary = 0;
    int ret = __isoc23_sscanf("0b101 42 110", "%i %w32d %" SCNb32 "%n",
        &value, &exact, &binary, &count);
    if (ret != 3 || value != 5 || exact != 42 || binary != 6 || count != 12) {
        return 1;
    }
    printf("value=%d exact=%" PRId32 " binary=%" PRIu32 " count=%d\n",
        value, exact, binary, count);
    return 0;
}
```

#### SEE ALSO

[__isoc23_scanf()](./__ISOC23_SCANF.md), [__isoc23_fscanf()](./__ISOC23_FSCANF.md), [__isoc23_vfscanf()](./__ISOC23_VFSCANF.md), [__isoc23_vsscanf()](./__ISOC23_VSSCANF.md), [__isoc23_swscanf()](./__ISOC23_SWSCANF.md), `scanf(3)`, `wscanf(3)`.

#### COLOPHON

This page is part of the C library user-space interface documentation.
Information about the project can be found at <https://gitcode.com/openharmony/third_party_musl/blob/master/docs/>.
