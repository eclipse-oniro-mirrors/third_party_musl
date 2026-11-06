## GWP_ASAN_SET_UNWIND_CALLBACKS &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; OHOS Programmer's Manual


#### **NAME**

​       libc_gwp_asan_set_unwind_callbacks - register or clear the GWP-ASan custom unwind backend.

#### **SYNOPSIS**

​       #include <gwp_asan.h>

       int libc_gwp_asan_set_unwind_callbacks(const gwp_asan_unwind_callbacks *callbacks);

#### **DESCRIPTION**

Registers a process-global unwind callback used by GWP-ASan when recording allocation, deallocation, and crash stacks. A later successful registration replaces the previous one. Passing NULL clears the registration and restores the built-in frame-pointer unwinder.

The callback may run on any thread, including inside the SIGSEGV reporting path, and therefore must be async-signal-safe. Same-thread reentry does not call the callback again; that nested unwind uses the frame-pointer unwinder.

**gwp_asan_unwind_callbacks**

| Field | Description |
| -------- | -------- |
| struct_size | Size of this structure. Must be at least sizeof(gwp_asan_unwind_callbacks). |
| abi_version | Must be GWP_ASAN_UNWIND_ABI_VERSION. |
| unwind | Callback. Must be non-NULL when registering. |

**Callback contract**

The callback receives the frame buffer, the number of frames already stored, the maximum frame count, the current frame address, and the stack bounds. It appends frames at frame_buf[num_frames .. max_record_stack-1] and returns the total frame count. The count must not exceed max_record_stack, and frames already stored in frame_buf[0 .. num_frames-1] must be kept. libc clamps a return value outside [num_frames, max_record_stack].

**Parameters**

| Name | Description |
| -------- | -------- |
| callbacks | Registration description. NULL unregisters the current callback. |

​       Note: Registration itself is MT-Safe. The registered callback is not signal-safe unless the provider makes it so. It is invoked from a signal handler on the crash path.

#### **RETURN VALUE**

| Value | Meaning |
| -------- | -------- |
| GWP_ASAN_UNWIND_REGISTER_OK | Registered or unregistered. |
| GWP_ASAN_UNWIND_REGISTER_INVALID_ARGUMENT | struct_size is too small, or unwind is NULL. The previous callback is kept. |
| GWP_ASAN_UNWIND_REGISTER_UNSUPPORTED_ABI | abi_version is not GWP_ASAN_UNWIND_ABI_VERSION. The previous callback is kept. |

#### ATTRIBUTES

| Attribute     | Value    |
| ------------- | -------- |
| Thread safety | MT-safe  |
| Signal safety | Registration is not called from a signal handler. The callback itself must be async-signal-safe. |

#### HISTORY

​       -- 2026

#### EXAMPLES

```c
static size_t my_unwind(size_t *frame_buf, size_t num_frames, size_t max_record_stack,
                        size_t current_frame_addr, size_t stack_start, size_t stack_end)
{
    (void)current_frame_addr;
    (void)stack_start;
    (void)stack_end;
    if (num_frames >= max_record_stack) {
        return num_frames;
    }
    frame_buf[num_frames] = 0;
    return num_frames + 1;
}

int register_backend(void)
{
    gwp_asan_unwind_callbacks callbacks = {
        .struct_size = sizeof(callbacks),
        .abi_version = GWP_ASAN_UNWIND_ABI_VERSION,
        .unwind = my_unwind,
    };
    return libc_gwp_asan_set_unwind_callbacks(&callbacks);
}
```

#### CONFORMING TO

​      This is a platform-specific extension and is not part of any POSIX standard.

#### COLOPHTON

​      this page is part of the C library user-space interface documentation.
​      Information about the project can be found at (https://gitcode.com/openharmony/third_party_musl/blob/master/docs/)
