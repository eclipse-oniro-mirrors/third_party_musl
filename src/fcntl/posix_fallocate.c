#include <fcntl.h>
#include <errno.h>
#include "syscall.h"
#ifdef IO_GUARD_ENABLE
#include "io_guard_impl.h"
#endif

int posix_fallocate(int fd, off_t base, off_t len)
{
#ifdef IO_GUARD_ENABLE
	if (__io_guard_check(fd, 0) < 0) {
		return EPERM;
	}
#endif
	return -__syscall(SYS_fallocate, fd, 0, __SYSCALL_LL_E(base),
		__SYSCALL_LL_E(len));
}

weak_alias(posix_fallocate, posix_fallocate64);
