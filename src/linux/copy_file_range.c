#define _GNU_SOURCE
#include <unistd.h>
#include "syscall.h"
#ifdef IO_GUARD_ENABLE
#include "io_guard_impl.h"
#endif

ssize_t copy_file_range(int fd_in, off_t *off_in, int fd_out, off_t *off_out, size_t len, unsigned flags)
{
#ifdef IO_GUARD_ENABLE
	if (__io_guard_check(fd_out, 0) < 0) {
		return -1;
	}
#endif
	return syscall(SYS_copy_file_range, fd_in, off_in, fd_out, off_out, len, flags);
}
