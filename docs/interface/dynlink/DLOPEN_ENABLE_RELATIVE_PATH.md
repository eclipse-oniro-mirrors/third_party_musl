## DLOPEN_ENABLE_RELATIVE_PATH &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; OHOS Programmer's Manual



#### **NAME**

​       dlopen_enable_relative_path - Enable loading shared libraries by a relative pathname

#### **SYNOPSIS**

       void dlopen_enable_relative_path(void);

#### **DESCRIPTION**

By default the dynamic loader rejects a `dlopen` pathname that contains `/` but is not absolute,
such as `"./libfoo.so"`. Calling `dlopen_enable_relative_path()` enables this form of loading
for the entire process.

After the switch is enabled, a relative pathname is processed as follows:

1. the pathname is opened relative to the current working directory;
2. the opened descriptor is resolved to its real absolute path through `/proc/self/fd/<fd>`;
3. the resolved path must pass the namespace accessibility check (`is_accessible`) of the
   caller's namespace;
4. the validated descriptor is used directly; the file is not reopened.

Absolute pathnames, bare library names (resolved through the namespace search path) and
ZIP-internal paths are not affected. The DSO keeps the pathname exactly as passed by the
caller; the resolved absolute path is used only for the accessibility check.

The switch is process-wide and one-way: once enabled it cannot be disabled, and it applies
to every subsequent `dlopen` in the process.

The function is compiled only when `musl_extended_function` is enabled and
`ENABLE_DLOPEN_RELATIVE_PATH` is defined (L2 standard system on Linux). In other builds the
function and its symbol are not compiled.

Note: This function is MT-Safe(multi-thread safe) and not signal-safe.

#### ATTRIBUTES

| Interface               | Attribute     | Value    |
| ----------------------- | ------------- | -------- |
| void dlopen_enable_relative_path(void) | Thread safety | MT-safe |
|                         | Signal safety | Not Safe |

#### HISTORY

​       -- 2026

#### NOTES

- The call is idempotent; repeated calls have no additional effect.
- The switch is process-wide and cannot be turned off.
- `/proc` must be mounted because the resolved path is read from `/proc/self/fd/<fd>`.
- Only pathnames that contain `/` and do not start with `/` are affected.
- Namespace access control is still enforced on the resolved path. For a namespace that is not
  separated, the accessibility check is a no-op, the same as for absolute pathnames.
- No public header declaration is provided; use `dlsym` to obtain the symbol.

#### CONFORMING TO

​      This is a platform-specific extension and is not part of any POSIX standard.

#### EXAMPLES

```c
#include <dlfcn.h>
#include <stdio.h>

typedef void(Enable)(void);

int main(void)
{
    Enable *enable = dlsym(RTLD_DEFAULT, "dlopen_enable_relative_path");
    if (!enable) {
        perror("can not find this symbol");
        return -1;
    }
    enable();

    void *handle = dlopen("./plugins/libdemo.so", RTLD_NOW);
    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        return -1;
    }
    // You can execute other operation here

    return 0;
}
```


#### COLOPHON

​      this page is part of the C library user-space interface documentation.
​      Information about the project can be found at (https://gitcode.com/openharmony/third_party_musl/blob/master/docs/).
