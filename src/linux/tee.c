#define _GNU_SOURCE
#include <fcntl.h>
#include "syscall.h"
#ifdef IO_GUARD_ENABLE
#include "io_guard_impl.h"
#endif

ssize_t tee(int src, int dest, size_t len, unsigned flags)
{
#ifdef IO_GUARD_ENABLE
	if (__io_guard_check(dest, 0) < 0) {
		return -1;
	}
#endif
	return syscall(SYS_tee, src, dest, len, flags);
}
