#define _GNU_SOURCE
#include <fcntl.h>
#include "syscall.h"
#ifdef IO_GUARD_ENABLE
#include "io_guard_impl.h"
#endif

int fallocate(int fd, int mode, off_t base, off_t len)
{
#ifdef IO_GUARD_ENABLE
	if (__io_guard_check(fd, 0) < 0) {
		return -1;
	}
#endif
	return syscall(SYS_fallocate, fd, mode, __SYSCALL_LL_E(base),
		__SYSCALL_LL_E(len));
}

#undef fallocate64
weak_alias(fallocate, fallocate64);
