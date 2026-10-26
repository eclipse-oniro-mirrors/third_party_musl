## __LIBC_STACK_END &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; OHOS Programmer's Manual



#### **NAME**

​       __libc_stack_end - approximate pointer to the top (highest address) of the process stack.

#### **SYNOPSIS**

```c
extern void *__libc_stack_end;
```

#### **DESCRIPTION**

​The __libc_stack_end variable is a global pointer that approximates the highest address of the process stack. The value is not the exact stack top: __builtin_frame_address(0) reflects the stack frame of __libc_start_main, which is below the actual stack top. It is set during process startup and remains constant for the entire lifetime of the process.

The variable is assigned in __libc_start_main from __builtin_frame_address(0), which reflects the runtime stack frame address.

- **Dynamically linked processes**: __libc_stack_end is first assigned in __dls2 from the stack pointer (sp), then overwritten in __libc_start_main with __builtin_frame_address(0).

- **Statically linked or direct-exec processes**: __libc_stack_end is assigned only in __libc_start_main from __builtin_frame_address(0).

After startup completes, the variable is never modified again.

The value is provided for compatibility with glibc ABI. Third-party libraries and tools (such as performance profilers and backtrace utilities) may reference this variable to approximate the extent of the main thread's stack. The C library itself does not read this variable internally.

The variable is defined in the C library internal source and is not declared in any public header; users who wish to access it must declare it as extern themselves.

#### **RETURN VALUE**

​Not applicable; __libc_stack_end is a global variable of type void *.

#### **ERRORS**

​Not applicable.

#### ATTRIBUTES

| Interface | Attribute     | Value          |
| --------- | ------------- | -------------- |
| __libc_stack_end | Thread safety | MT-Safe (read-only after initialization) |
|             | Async-signal-safety | AS-Safe (read) |

The variable is written during startup (once for statically linked, twice for dynamically linked) before any user code runs, and is never modified afterward. All subsequent reads are safe from any thread or signal handler.

#### EXAMPLES

```c
#include <stdio.h>

extern void *__libc_stack_end;

int main(void)
{
    int local;
    printf("Stack end: %p\n", __libc_stack_end);
    printf("Stack frame: %p\n", (void *)&local);
    return 0;
}
```

#### COLOPHON

​This page is part of the C library user-space interface documentation.
Information about the project can be found at (https://gitcode.com/openharmony/third_party_musl/blob/master/docs/).
