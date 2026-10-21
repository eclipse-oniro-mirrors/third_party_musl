## __LIBC_STACK_END &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; OHOS Programmer's Manual



#### **NAME**

​       __libc_stack_end - pointer to the top (highest address) of the process stack.

#### **SYNOPSIS**

```c
extern void *__libc_stack_end;
```

#### **DESCRIPTION**

​The __libc_stack_end variable is a global pointer that holds the highest address of the process stack. It is set once during process startup and remains constant for the entire lifetime of the process.

The variable is assigned in one of two ways depending on how the process is loaded:

- **Statically linked or direct-exec processes**: In the C runtime startup code (_start → _start_c), __libc_stack_end is assigned from the initial stack pointer value passed by the kernel.

- **Dynamically linked processes**: During dynamic linker initialization (__dls2), __libc_stack_end is assigned from the stack pointer (sp).

The value is used internally by the C library for stack-related operations, such as stack unwinding and backtrace support. Applications and libraries may also read this variable to determine the extent of the main thread's stack.

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

The variable is written once during startup before any user code runs and is never modified afterward. All subsequent reads are safe from any thread or signal handler.

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
